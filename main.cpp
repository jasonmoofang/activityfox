#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KMainWindow>
#include <KConfigGroup>
#include <KProcess>
#include <iostream>
#include <QtDBus>

const char *defaultProfileDirTail = ".mozilla/firefox";
const char *defaultBinPath = "/usr/lib/firefox/firefox";

class MainWindow : public KMainWindow {
public:
    explicit MainWindow(const QString &binPath = nullptr, const QString &profileDir = nullptr,
                        const QString &templateProfile = nullptr, QString profile = nullptr, QWidget *parent = nullptr);

protected:
    void saveProperties(KConfigGroup &) override;

    void readProperties(const KConfigGroup &) override;

    QString getCurrentActivityId();

private:
    QString firefoxProfile;
    QString binPath;
    KProcess p;

};

MainWindow::MainWindow(const QString &binPath, const QString &profileDir, const QString &templateProfile,
                       QString profile, QWidget *parent) : KMainWindow(parent) {
    // args such as binPath are null when session is restored, not null on the first run
    if (binPath != nullptr) {
        if (profile == "") {
            profile = getCurrentActivityId();
        }

        this->binPath = binPath;

        setGeometry(100, 100, 200, 100);
        firefoxProfile = profile;
        QStringList createProfileCommand;

        // create profile with a specified profile name (or activity id) in a specified profile directory
        createProfileCommand << binPath << "-CreateProfile" << firefoxProfile + ' ' + profileDir + '/' + firefoxProfile
                             << "-no-remote";
        KProcess::execute(createProfileCommand);

        // run Firefox
        p << binPath << "-p" << firefoxProfile << "-no-remote";
        p.start();
    }
}

void MainWindow::saveProperties(KConfigGroup &conf) {
    // check if Firefox is running
    if (p.pid() > 0) {
        conf.writeEntry("ffProfile", firefoxProfile);
        conf.writeEntry("binPath", binPath);

        // trying to gracefully terminate Firefox by closing the window with wmctrl
        QStringList closeFirefoxCommand;
        char pid[21];

        sprintf(pid, "%d", p.pid());
        std::string arguments = "wmctrl -i -c `wmctrl -lp | grep ";
        arguments = arguments + pid + " | cut -c -10`";
        closeFirefoxCommand << arguments.c_str();

        int exitcode = KProcess::execute(closeFirefoxCommand);

        // if we can't close the window, terminate the process anyway
        if (exitcode != 0) {
            p.terminate();
        }
    } else {
        // do not need to restore this app if Firefox was terminated
        exit(0);
    }
}

void MainWindow::readProperties(const KConfigGroup &conf) {
    // restart Firefox
    firefoxProfile = conf.readEntry("ffProfile", QString());
    binPath = conf.readEntry("binPath", QString());
    p << binPath << "-p" << firefoxProfile << "-no-remote";
    p.start();
}

QString MainWindow::getCurrentActivityId() {
    // get current activity id with QDBus, also possible to get by executing:
    // qdbus org.kde.ActivityManager /ActivityManager/Activities org.kde.ActivityManager.Activities.CurrentActivity
    QDBusInterface interface("org.kde.ActivityManager",
                             "/ActivityManager/Activities",
                             "org.kde.ActivityManager.Activities",
                             QDBusConnection::sessionBus());

    QDBusMessage result = interface.call("CurrentActivity");
    return result.arguments().at(0).value<QString>();
}

int main(int argc, char *argv[]) {
    std::string defaultProfileDir = getenv("HOME");
    defaultProfileDir = defaultProfileDir + '/' + defaultProfileDirTail;

    KAboutData aboutData("activityfox", nullptr,
                         ki18n("ActivityFox"), "0.1",
                         ki18n("Help Firefox work with KDE activities."),
                         KAboutData::License_GPL,
                         ki18n("Copyright (c) 2012 Yuen Hoe (Jason moofang), 2018 Leonid Kalichkin (hellishnoob)"));
    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineOptions options;
    options.add("bin-path <path>", ki18n("Path to Firefox executable"), defaultBinPath);
    options.add("profile-dir <path>", ki18n("Path to Firefox profile directory"), defaultProfileDir.c_str());
    options.add("template-profile <name>", ki18n("Template profile name"));
    options.add("+[profile-name]", ki18n("Firefox profile name"));
    KCmdLineArgs::addCmdLineOptions(options);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    QString binPath = args->getOption("bin-path");
    QString profileDir = args->getOption("profile-dir");
    QString templateProfile = args->getOption("template-profile");
    QString profileName = nullptr;

    if (args->count() > 0) {
        profileName = args->arg(0);
    }

    KApplication app;
    if (app.isSessionRestored()) {
        kRestoreMainWindows<MainWindow>();
    } else {
        // create default application as usual
        // example:
        MainWindow *window = new MainWindow(binPath, profileDir, templateProfile, profileName);
        window->setObjectName("ActivityFoxWindow");
        window->show();
    }

    return app.exec();
}
