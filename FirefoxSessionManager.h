#ifndef ACTIVITYFOX_FIREFOXSESSIONMANAGER_H
#define ACTIVITYFOX_FIREFOXSESSIONMANAGER_H

#include "BrowserSessionManager.h"

class FirefoxSessionManager : public BrowserSessionManager {
public:
    FirefoxSessionManager(QStringList allArguments, const QString &binPath, const QString &profileDir, const QString &templateProfileName,
                          const QString &profileName);

private:

    bool checkIfProfileExists(const QString &profileDir, const QString &profileName) override;

    void startBrowser(KProcess &browserProcess, const QString &binPath, const QString &profileDir, const QString &profileName) override;

    void createProfile(const QString &binPath, const QString &profileDir, const QString &profileName,
                       const QString &templateProfileName) override;

    void copyProfile(const QString &profileDir, const QString &fromName, const QString &toName) override;

    QString getDefaultBinPath() override;

    QString getDefaultProfileDir() override;
};


#endif //ACTIVITYFOX_FIREFOXSESSIONMANAGER_H
