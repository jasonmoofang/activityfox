#include "ChromiumSessionManager.h"

bool ChromiumSessionManager::checkIfProfileExists(const QString &profileDir, const QString &profileName) {
    DIR *dir = opendir((profileDir + '/' + profileName).toAscii());
    return dir != nullptr;
}

void ChromiumSessionManager::createProfile(const QString &, const QString &profileDir, const QString &profileName,
                                           const QString &templateProfileName) {
    // if a template profile name is specified, copy this profile into the new profile directory
    if (templateProfileName != nullptr && templateProfileName != "") {
        copyProfile(profileDir, templateProfileName, profileName);
    }

    // no need to call Chromium to create the profile, Chromium creates it automatically on the first run
}

void ChromiumSessionManager::startBrowser(KProcess &browserProcess, const QString &binPath, const QString &profileDir,
                                          const QString &profileName) {
    browserProcess << binPath << "--user-data-dir=" + profileDir + '/' + profileName;
    qDebug() << browserProcess.program();
    browserProcess.start();
}

void ChromiumSessionManager::copyProfile(const QString &profileDir, const QString &fromName, const QString &toName) {
    QStringList command;

    // make copy of a profile
    command << "cp" << "-r" << profileDir + '/' + fromName << profileDir + '/' + toName;
    qDebug() << command;

    KProcess::execute(command);
}

QString ChromiumSessionManager::getDefaultProfileDir() {
    static const char *defaultProfileDirTail = ".config/chromium";

    std::string defaultProfileDir = getenv("HOME");
    return (defaultProfileDir + '/' + defaultProfileDirTail).c_str();
}

QString ChromiumSessionManager::getDefaultBinPath() {
    static const char *defaultBinPath = "/usr/lib/chromium-browser/chromium-browser";

    return defaultBinPath;
}

ChromiumSessionManager::ChromiumSessionManager(QStringList allArguments, const QString &binPath,
                                               const QString &profileDir, const QString &templateProfileName,
                                               const QString &profileName) : BrowserSessionManager(allArguments,
                                                                                                   binPath, profileDir,
                                                                                                   templateProfileName,
                                                                                                   profileName) {}