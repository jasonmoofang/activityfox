#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KMainWindow>
#include <KConfigGroup>
#include <KProcess>
#include <QtDBus>

const char *defaultProfileDirTail = ".mozilla/firefox";
const char *defaultBinPath = "/usr/lib/firefox/firefox";

class MainWindow : public KMainWindow {
public:
    MainWindow(const QString &binPath, const QString &profileDir,
               const QString &templateProfileName, const QString &profileName);

    MainWindow();

protected:
    void saveProperties(KConfigGroup &) override;

    void readProperties(const KConfigGroup &) override;

private:
    QString firefoxProfileName;
    QString firefoxBinPath;
    KProcess firefoxProcess;
    bool firefoxWasClosedByThisApp = false;

    bool checkIfProfileExists(const QString &profileDir, const QString &profileName);

    QString getCurrentActivityId();

    void closeFirefox(KProcess &process);

    void startFirefox(KProcess &firefoxProcess, const QString &binPath, const QString &profileName);

    void createProfile(const QString &binPath, const QString &profileDir, const QString &profileName,
                       const QString &templateProfileName);

    void copyProfile(const QString &profileDir, const QString &fromName, const QString &toName);

};

// empty constructor is used when session is restored
MainWindow::MainWindow() : KMainWindow() {
    qDebug() << "MainWindow: Session was restored";
}

MainWindow::MainWindow(const QString &binPath, const QString &profileDir, const QString &templateProfileName,
                       const QString &profileName) : KMainWindow() {
    // if profile name was not specified, use current activity id
    if (profileName == nullptr || profileName == "") {
        firefoxProfileName = getCurrentActivityId();
    } else {
        firefoxProfileName = profileName;
    }

    firefoxBinPath = binPath;

    if (!checkIfProfileExists(profileDir, firefoxProfileName)) {
        qDebug() << "Profile doesn't exist, creating it, name is: " << firefoxProfileName;
        createProfile(firefoxBinPath, profileDir, firefoxProfileName, templateProfileName);
    }

    qDebug() << "Starting firefox";
    startFirefox(firefoxProcess, firefoxBinPath, firefoxProfileName);
}

void MainWindow::saveProperties(KConfigGroup &conf) {
    qDebug() << "ActivityFox is closing, saving the session";

    // check if Firefox is running
    if (firefoxProcess.pid() > 0) {
        conf.writeEntry("ffProfileName", firefoxProfileName);
        conf.writeEntry("ffBinPath", firefoxBinPath);

        closeFirefox(firefoxProcess);
        firefoxWasClosedByThisApp = true;
    } else if (!firefoxWasClosedByThisApp) {
        // don't need to restore this app and Firefox if Firefox was closed by user or unexpectedly
        qDebug("Firefox was closed, exiting from this app");
        exit(0);
    }
}

void MainWindow::readProperties(const KConfigGroup &conf) {
    qDebug() << "ActivityFox is restored, starting Firefox";
    // restart Firefox
    firefoxProfileName = conf.readEntry("ffProfileName", QString());
    firefoxBinPath = conf.readEntry("ffBinPath", QString());
    startFirefox(firefoxProcess, firefoxBinPath, firefoxProfileName);
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

bool MainWindow::checkIfProfileExists(const QString &profileDir, const QString &profileName) {
    KProcess checkIfProfileExists;

    QString command = "cat " + profileDir + '/' + "profiles.ini" + " | grep " + profileName;
    qDebug() << command;

    checkIfProfileExists.setShellCommand(command);
    return (checkIfProfileExists.execute() == 0);
}

void MainWindow::createProfile(const QString &binPath, const QString &profileDir, const QString &profileName,
                               const QString &templateProfileName) {
    // if a template profile name is specified, copy this profile into the new profile directory
    if (templateProfileName != nullptr && templateProfileName != "") {
        copyProfile(profileDir, templateProfileName, profileName);
    }

    QStringList command;

    // create a profile
    command << binPath << "-CreateProfile" << profileName + ' ' + profileDir + '/' + profileName
            << "-no-remote";
    qDebug() << command;
    KProcess::execute(command);
}

void MainWindow::startFirefox(KProcess &firefoxProcess, const QString &binPath, const QString &profileName) {
    firefoxProcess << binPath << "-p" << profileName << "-no-remote";
    firefoxProcess.start();
}

// expects a non-zero pid, so the process must be running
void MainWindow::closeFirefox(KProcess &process) {
    char pid[21];
    KProcess closeFirefox;

    sprintf(pid, "%d", process.pid());
    QString command = "wmctrl -i -c `wmctrl -lp | grep " + QString(pid) + " | cut -c -10`";
    qDebug() << command;

    // trying to gracefully terminate Firefox by closing the window with wmctrl
    closeFirefox.setShellCommand(command);
    int exitcode = closeFirefox.execute();

    // if wmctrl failed, terminate Firefox normally
    if (exitcode != 0) {
        qDebug() << "wmctrl failed";
        process.terminate();
    }

    // wait until Firefox is closed for at most 30 seconds, terminate if it wasn't closed after that
    if (!process.waitForFinished()) {
        qDebug() << "Firefox hasn't finished in 30 seconds";
        process.terminate();
    }
}

void MainWindow::copyProfile(const QString &profileDir, const QString &fromName, const QString &toName) {
    QStringList command;

    // make copy of a profile
    command << "cp" << "-r" << profileDir + '/' + fromName << profileDir + '/' + toName;
    qDebug() << command;

    KProcess::execute(command);
}

#ifndef NDEBUG
FILE *fp;

void myMessageOutput(QtMsgType type, const char *msg) {
    switch (type) {
        case QtDebugMsg:
            fprintf(fp, "Debug: %s\n", msg);
            fflush(fp);
            break;
        case QtWarningMsg:
            fprintf(fp, "Warning: %s\n", msg);
            fflush(fp);
            break;
        case QtCriticalMsg:
            fprintf(fp, "Critical: %s\n", msg);
            fflush(fp);
            break;
        case QtFatalMsg:
            fprintf(fp, "Fatal: %s\n", msg);
            fflush(fp);
            abort();
    }
}
#endif

int main(int argc, char *argv[]) {
#ifndef NDEBUG
    fp = fopen("activityfox.log", "a");
    qInstallMsgHandler(myMessageOutput);
#endif

    std::string defaultProfileDir = getenv("HOME");
    defaultProfileDir = defaultProfileDir + '/' + defaultProfileDirTail;

    KAboutData aboutData("activityfox", nullptr,
                         ki18n("ActivityFox"), "0.1",
                         ki18n("Help Firefox work with KDE activities."),
                         KAboutData::License_GPL,
                         ki18n("Copyright (c) 2012 Yuen Hoe (Jason moofang), 2018 Leonid Kalichkin (hellishnoob)"));
    KCmdLineArgs::init(argc, argv, &aboutData);
    KCmdLineOptions options;
    options.add("b");
    options.add("bin-path <path>", ki18n("Path to Firefox executable"), defaultBinPath);
    options.add("p");
    options.add("profile-dir <path>", ki18n("Path to Firefox profile directory"), defaultProfileDir.c_str());
    options.add("t");
    options.add("template-profile-name <name>", ki18n("Template profile name"));
    options.add("+[profile-name]", ki18n("Firefox profile name"));
    KCmdLineArgs::addCmdLineOptions(options);

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    QString binPath = args->getOption("bin-path");
    QString profileDir = args->getOption("profile-dir");
    QString templateProfileName = args->getOption("template-profile-name");
    QString profileName = nullptr;

    if (args->count() > 0) {
        profileName = args->arg(0);
    }

    KApplication app;
    if (app.isSessionRestored()) {
        kRestoreMainWindows<MainWindow>();
    } else {
        MainWindow *window = new MainWindow(binPath, profileDir, templateProfileName, profileName);
        window->setObjectName("ActivityFoxWindow");
        window->show();
    }

    int result = app.exec();

#ifndef NDEBUG
    fclose(fp);
#endif

    return result;
}
