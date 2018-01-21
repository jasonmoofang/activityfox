#ifndef ACTIVITYFOX_CHROMIUMSESSIONMANAGER_H
#define ACTIVITYFOX_CHROMIUMSESSIONMANAGER_H

#include <dirent.h>
#include "BrowserSessionManager.h"

class ChromiumSessionManager : public BrowserSessionManager {
public:
    ChromiumSessionManager(QStringList allArguments, const QString &binPath, const QString &profileDir,
                           const QString &templateProfileName,
                           const QString &profileName);

private:

    bool checkIfProfileExists(const QString &profileDir, const QString &profileName) override;

    void startBrowser(KProcess &browserProcess, const QString &binPath, const QString &profileDir,
                      const QString &profileName) override;

    void createProfile(const QString &binPath, const QString &profileDir, const QString &profileName,
                       const QString &templateProfileName) override;

    void copyProfile(const QString &profileDir, const QString &fromName, const QString &toName) override;

    QString getDefaultBinPath() override;

    QString getDefaultProfileDir() override;
};


#endif //ACTIVITYFOX_CHROMIUMSESSIONMANAGER_H
