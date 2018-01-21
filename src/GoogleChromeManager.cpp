#include "GoogleChromeManager.h"

QString GoogleChromeManager::getDefaultProfileDir() {
    static const char *defaultProfileDirTail = ".config/google-chrome";

    std::string defaultProfileDir = getenv("HOME");
    return (defaultProfileDir + '/' + defaultProfileDirTail).c_str();
}

QString GoogleChromeManager::getDefaultBinPath() {
    static const char *defaultBinPath = "/opt/google/chrome/google-chrome";

    return defaultBinPath;
}

GoogleChromeManager::GoogleChromeManager(const QString &startUpActivityId, const QString &binPath,
                                         const QString &profileDir, const QString &templateProfileName,
                                         const QString &profileName)
        : ChromiumManager(startUpActivityId, binPath, profileDir, templateProfileName, profileName) {}