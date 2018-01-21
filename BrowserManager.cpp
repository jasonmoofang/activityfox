#include "BrowserManager.h"

BrowserManager::BrowserManager(const QString &startUpActivityId, const QString &binPath,
                               const QString &profileDir, const QString &templateProfileName,
                               const QString &profileName) {
    this->startUpActivityId = startUpActivityId;
    this->binPath = binPath;
    this->profileDir = profileDir;
    this->templateProfileName = templateProfileName;
    this->profileName = profileName;
}

void BrowserManager::init() {
    if (binPath == nullptr || binPath == "") {
        binPath = getDefaultBinPath();
    }

    if (profileDir == nullptr || profileDir == "") {
        profileDir = getDefaultProfileDir();
    }

    // if profile name was not specified, use current activity id
    if (profileName == nullptr || profileName == "") {
        profileName = startUpActivityId;
    }

    if (!checkIfProfileExists(profileDir, profileName)) {
        qDebug() << "Profile doesn't exist, creating it, name is: " << profileName;
        createProfile(binPath, profileDir, profileName, templateProfileName);
    }

    qDebug() << "Starting the browser";
    startBrowser(process, binPath, profileDir, profileName);
}

// expects a non-zero pid, so the process must be running
void BrowserManager::closeBrowser(KProcess &process) {
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


void BrowserManager::onSaveProperties() {
    qDebug() << "onSaveProperties() was called";

    if (process.pid() > 0) {
        closeBrowser(process);
    } else {
        // the browser was closed by user or unexpectedly, so we don't want to restore it automatically
        exit(0);
    }
}
