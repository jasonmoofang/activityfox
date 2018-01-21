#ifdef NDEBUG
#define QT_NO_DEBUG_OUTPUT
#else

#include <unistd.h>
#endif

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KMainWindow>
#include <KConfigGroup>

#include "BrowserManager.h"
#include "FirefoxManager.h"
#include "ChromiumManager.h"
#include "GoogleChromeManager.h"

#ifndef NDEBUG

FILE *fp;

void myMessageOutput(QtMsgType type, const char *msg) {
    switch (type) {
        case QtDebugMsg:
            fprintf(fp, "Debug (%d): %s\n", getpid(), msg);
            fflush(fp);
            break;
        case QtWarningMsg:
            fprintf(fp, "Warning (%d): %s\n", getpid(), msg);
            fflush(fp);
            break;
        case QtCriticalMsg:
            fprintf(fp, "Critical (%d): %s\n", getpid(), msg);
            fflush(fp);
            break;
        case QtFatalMsg:
            fprintf(fp, "Fatal (%d): %s\n", getpid(), msg);
            fflush(fp);
            abort();
    }
}

#endif

QString getCurrentActivityId() {
    // get current activity id with QDBus, also possible to get by executing:
    // qdbus org.kde.ActivityManager /ActivityManager/Activities org.kde.ActivityManager.Activities.CurrentActivity
    QDBusInterface interface("org.kde.ActivityManager",
                             "/ActivityManager/Activities",
                             "org.kde.ActivityManager.Activities",
                             QDBusConnection::sessionBus());

    QDBusMessage result = interface.call("CurrentActivity");
    return result.arguments().at(0).value<QString>();
}

class MainWindow : public KMainWindow {
public:
    MainWindow(QString &startUpActivityId, unsigned browser, const QString &binPath,
               const QString &profileDir, const QString &templateProfileName,
               const QString &profileName) : KMainWindow() {
        this->startUpActivityId = startUpActivityId;
        this->browser = browser;
        this->binPath = binPath;
        this->profileDir = profileDir;
        this->templateProfileName = templateProfileName;
        this->profileName = profileName;

        if (browser == BrowserManager::CHROMIUM) {
            bm = new ChromiumManager(startUpActivityId, binPath, profileDir, templateProfileName, profileName);
        } else if (browser == BrowserManager::GOOGLE_CHROME) {
            bm = new GoogleChromeManager(startUpActivityId, binPath, profileDir, templateProfileName, profileName);
        } else {
            bm = new FirefoxManager(startUpActivityId, binPath, profileDir, templateProfileName, profileName);
        }

        bm->init();
    }

    MainWindow() {
        bm = nullptr;
        browser = BrowserManager::FIREFOX;
    }

protected:

    // that's a bit of a hack: listen to X11 PropertyNotify (28) event to see if activity was changed
    // when it's changed to our start up activity, we start the browser, so it is shown in the right activity
    bool x11Event(XEvent *event) override {
        if (initWhenVisible && *(int *) event == 28 && startUpActivityId == getCurrentActivityId()) {
            initWhenVisible = false;
            bm->init();
        }

        return QWidget::x11Event(event);
    }

    void readProperties(const KConfigGroup &conf) override {
        startUpActivityId = conf.readEntry("startUpActivityId");
        browser = conf.readEntry("browser", unsigned());
        binPath = conf.readEntry("binPath");
        profileDir = conf.readEntry("profileDir");
        profileName = conf.readEntry("profileName");
        templateProfileName = conf.readEntry("templateProfileName");

        if (browser == BrowserManager::CHROMIUM) {
            bm = new ChromiumManager(startUpActivityId, binPath, profileDir, templateProfileName, profileName);
        } else if (browser == BrowserManager::GOOGLE_CHROME) {
            bm = new GoogleChromeManager(startUpActivityId, binPath, profileDir, templateProfileName, profileName);
        } else {
            bm = new FirefoxManager(startUpActivityId, binPath, profileDir, templateProfileName, profileName);
        }

        // if current activity is not the activity that the browser was started in, we will wait until it is
        if (startUpActivityId == getCurrentActivityId()) {
            bm->init();
        } else {
            initWhenVisible = true;
        }
    }

    void saveProperties(KConfigGroup &conf) override {
        conf.writeEntry("startUpActivityId", startUpActivityId);
        conf.writeEntry("browser", browser);
        conf.writeEntry("binPath", binPath);
        conf.writeEntry("profileDir", profileDir);
        conf.writeEntry("templateProfileName", templateProfileName);
        conf.writeEntry("profileName", profileName);

        static bool firstCall = true;
        if (firstCall) {
            firstCall = false;

            // we don't need to close the browser if we haven't yet opened it
            if (!initWhenVisible) {
                bm->onSaveProperties();
            }
        }
    }

private:
    BrowserManager *bm;
    unsigned browser;
    QString binPath;
    QString profileDir;
    QString templateProfileName;
    QString profileName;
    QString startUpActivityId;
    bool initWhenVisible = false;
};

KCmdLineArgs *parseCommandLineArguments(int argc, char **argv) {
    static KAboutData aboutData("activityfox", nullptr,
                                ki18n("ActivityFox"), "0.2",
                                ki18n("Help Firefox and Chromium-based browsers work with KDE activities."),
                                KAboutData::License_GPL,
                                ki18n("Copyright (c) 2012 Yuen Hoe (Jason moofang), 2018 Leonid Kalichkin (hellishnoob)"));

    KCmdLineArgs::init(argc, argv, &aboutData);

    static KCmdLineOptions options;
    options.add("c");
    options.add("chromium", ki18n("Use proxy for Chromium-based browsers"));
    options.add("g");
    options.add("google-chrome", ki18n("Use proxy for Google Chrome"));
    options.add("b");
    options.add("bin-path <path>", ki18n("Path to the browser executable"), nullptr);
    options.add("p");
    options.add("profile-dir <path>", ki18n("Path to the browser profile directory"), nullptr);
    options.add("t");
    options.add("template-profile-name <name>", ki18n("Profile name of a template to be used for profile creation"));
    options.add("+[profile-name]", ki18n("Profile name"));
    KCmdLineArgs::addCmdLineOptions(options);

    return KCmdLineArgs::parsedArgs();
}

int main(int argc, char *argv[]) {
#ifndef NDEBUG
    fp = fopen("activityfox.log", "a");
    qInstallMsgHandler(myMessageOutput);
#endif

    KCmdLineArgs *args = parseCommandLineArguments(argc, argv);

    QString binPath = args->getOption("bin-path");
    QString profileDir = args->getOption("profile-dir");
    QString templateProfileName = args->getOption("template-profile-name");
    QString profileName = nullptr;

    bool useChromium = args->isSet("chromium");
    bool useGoogleChrome = args->isSet("google-chrome");

    if (args->count() > 0) {
        profileName = args->arg(0);
    }

    BrowserManager::Browser browser;

    if (useChromium) {
        browser = BrowserManager::CHROMIUM;
    } else if (useGoogleChrome) {
        browser = BrowserManager::GOOGLE_CHROME;
    } else {
        browser = BrowserManager::FIREFOX;
    }

    QString startUpActivityId = getCurrentActivityId();

    KApplication app;

    if (app.isSessionRestored()) {
        kRestoreMainWindows<MainWindow>();
    } else {
        // having a visible window is necessary for session management
        MainWindow *window = new MainWindow(startUpActivityId, browser, binPath, profileDir, templateProfileName,
                                            profileName);
        window->show();
    }

    int result = app.exec();

#ifndef NDEBUG
    fclose(fp);
#endif

    return result;
}