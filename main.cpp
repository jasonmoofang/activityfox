#ifdef NDEBUG
#define QT_NO_DEBUG_OUTPUT
#endif

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <QWidget>

#include "BrowserSessionManager.h"
#include "FirefoxSessionManager.h"
#include "ChromiumSessionManager.h"
#include "GoogleChromeSessionManager.h"

#ifndef NDEBUG

#include <unistd.h>

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

KCmdLineArgs *parseCommandLineArguments(int argc, char **argv);

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

    BrowserSessionManager *sm;
    QStringList allArguments = args->allArguments();
    allArguments.removeFirst();

    if (useChromium) {
        sm = new ChromiumSessionManager(allArguments, binPath, profileDir, templateProfileName, profileName);
    } else if (useGoogleChrome) {
        sm = new GoogleChromeSessionManager(allArguments, binPath, profileDir, templateProfileName, profileName);
    } else {
        sm = new FirefoxSessionManager(allArguments, binPath, profileDir, templateProfileName, profileName);
    }

    // having a visible window is necessary for session management
    QWidget *window = new QWidget();
    window->show();

    sm->init();

    int result = sm->exec();

#ifndef NDEBUG
    fclose(fp);
#endif

    return result;
}

KCmdLineArgs *parseCommandLineArguments(int argc, char **argv) {
    static KAboutData aboutData("activityfox", nullptr,
                                ki18n("ActivityFox"), "0.1",
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
