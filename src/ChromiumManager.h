#ifndef ACTIVITYFOX_CHROMIUMMANAGER_H
#define ACTIVITYFOX_CHROMIUMMANAGER_H

#include <dirent.h>
#include "BrowserManager.h"

class ChromiumManager : public BrowserManager {
public:
    ChromiumManager(const QString &startUpActivityId, const QString &binPath, const QString &profileDir,
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


#endif //ACTIVITYFOX_CHROMIUMMANAGER_H
