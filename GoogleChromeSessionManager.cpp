#include "GoogleChromeSessionManager.h"

QString GoogleChromeSessionManager::getDefaultProfileDir() {
    static const char *defaultProfileDirTail = ".config/google-chrome";

    std::string defaultProfileDir = getenv("HOME");
    return (defaultProfileDir + '/' + defaultProfileDirTail).c_str();
}

QString GoogleChromeSessionManager::getDefaultBinPath() {
    static const char *defaultBinPath = "/opt/google/chrome/google-chrome";

    return defaultBinPath;
}

GoogleChromeSessionManager::GoogleChromeSessionManager(QStringList allArguments, const QString &binPath,
                                                       const QString &profileDir,
                                                       const QString &templateProfileName, const QString &profileName)
        : ChromiumSessionManager(allArguments,
                                 binPath, profileDir, templateProfileName, profileName) {}