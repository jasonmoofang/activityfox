#include "BrowserSessionManager.h"

#include <QtDBus>

BrowserSessionManager::BrowserSessionManager(const QStringList allArguments, const QString &binPath, const QString &profileDir,const QString &templateProfileName,
                       const QString &profileName) : KApplication() {
    restartArguments = allArguments;
    browserBinPath = binPath;
    browserProfileDir = profileDir;
    browserTemplateProfileName = templateProfileName;
    browserProfileName = profileName;
}

void BrowserSessionManager::init() {
    if (browserBinPath == nullptr || browserBinPath == "") {
        browserBinPath = getDefaultBinPath();
    }

    if (browserProfileDir == nullptr || browserProfileDir == "") {
        browserProfileDir = getDefaultProfileDir();
    }

    // if profile name was not specified, use current activity id
    if (browserProfileName == nullptr || browserProfileName == "") {
        browserProfileName = getCurrentActivityId();
    }

    if (!checkIfProfileExists(browserProfileDir, browserProfileName)) {
        qDebug() << "Profile doesn't exist, creating it, name is: " << browserProfileName;
        createProfile(browserBinPath, browserProfileDir, browserProfileName, browserTemplateProfileName);
    }

    qDebug() << "Starting the browser";
    startBrowser(browserProcess, browserBinPath, browserProfileDir, browserProfileName);
}

QString BrowserSessionManager::getCurrentActivityId() {
    // get current activity id with QDBus, also possible to get by executing:
    // qdbus org.kde.ActivityManager /ActivityManager/Activities org.kde.ActivityManager.Activities.CurrentActivity
    QDBusInterface interface("org.kde.ActivityManager",
                             "/ActivityManager/Activities",
                             "org.kde.ActivityManager.Activities",
                             QDBusConnection::sessionBus());

    QDBusMessage result = interface.call("CurrentActivity");
    return result.arguments().at(0).value<QString>();
}

// expects a non-zero pid, so the process must be running
void BrowserSessionManager::closeBrowser(KProcess &process) {
    char pid[21];
    KProcess closeBrowser;

    sprintf(pid, "%d", process.pid());
    QString command = "wmctrl -i -c `wmctrl -lp | grep " + QString(pid) + " | cut -c -10`";
    qDebug() << command;

    // trying to gracefully terminate the browser by closing the window with wmctrl
    closeBrowser.setShellCommand(command);
    int exitcode = closeBrowser.execute();

    // if wmctrl failed, terminate the browser normally
    if (exitcode != 0) {
        qDebug() << "wmctrl failed";
        process.terminate();
    }

    // wait until the browser is closed for at most 30 seconds, terminate if it wasn't closed after that
    if (!process.waitForFinished()) {
        qDebug() << "The browser hasn't finished in 30 seconds";
        process.terminate();
    }
}


void BrowserSessionManager::commitData(QSessionManager &sm) {
    qDebug() << "Application::commitData was called";

    // we will just tell the session manager to restart this application with the same arguments on session start
    QStringList restartCommand = sm.restartCommand();
    restartCommand.append(restartArguments);
    sm.setRestartCommand(restartCommand);

    if (browserProcess.pid() > 0) {
        closeBrowser(browserProcess);
    } else {
        // the browser was closed by user or unexpectedly, so we don't want to restore it automatically
        exit(0);
    }

    KApplication::commitData(sm);
}
