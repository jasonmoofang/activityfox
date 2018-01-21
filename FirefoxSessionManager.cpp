#include "FirefoxSessionManager.h"

bool FirefoxSessionManager::checkIfProfileExists(const QString &profileDir, const QString &profileName) {
    KProcess checkIfProfileExists;

    QString command = "cat " + profileDir + '/' + "profiles.ini" + " | grep " + profileName;
    qDebug() << command;

    checkIfProfileExists.setShellCommand(command);
    return (checkIfProfileExists.execute() == 0);
}

void FirefoxSessionManager::createProfile(const QString &binPath, const QString &profileDir, const QString &profileName,
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

void FirefoxSessionManager::startBrowser(KProcess &browserProcess, const QString &binPath, const QString &,
                                         const QString &profileName) {
    qDebug() << "FirefoxSessionManager::startBrowser was called";
    browserProcess << binPath << "-p" << profileName << "-no-remote";
    qDebug() << browserProcess.program();
    browserProcess.start();
}

void FirefoxSessionManager::copyProfile(const QString &profileDir, const QString &fromName, const QString &toName) {
    QStringList command;

    // make copy of a profile
    command << "cp" << "-r" << profileDir + '/' + fromName << profileDir + '/' + toName;
    qDebug() << command;

    KProcess::execute(command);
}

QString FirefoxSessionManager::getDefaultProfileDir() {
    static const char *defaultProfileDirTail = ".mozilla/firefox";

    std::string defaultProfileDir = getenv("HOME");
    return (defaultProfileDir + '/' + defaultProfileDirTail).c_str();
}

QString FirefoxSessionManager::getDefaultBinPath() {
    static const char *defaultBinPath = "/usr/lib/firefox/firefox";

    return defaultBinPath;
}

FirefoxSessionManager::FirefoxSessionManager(QStringList allArguments, const QString &binPath,
                                             const QString &profileDir, const QString &templateProfileName,
                                             const QString &profileName) : BrowserSessionManager(allArguments, binPath,
                                                                                                 profileDir,
                                                                                                 templateProfileName,
                                                                                                 profileName) {}