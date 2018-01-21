#ifndef ACTIVITYFOX_GOOGLECHROMEMANAGER_H
#define ACTIVITYFOX_GOOGLECHROMEMANAGER_H

#include "ChromiumManager.h"

class GoogleChromeManager : public ChromiumManager {
public:
    GoogleChromeManager(const QString &startUpActivityId, const QString &binPath, const QString &profileDir,
                        const QString &templateProfileName,
                        const QString &profileName);

private:

    QString getDefaultBinPath() override;

    QString getDefaultProfileDir() override;
};


#endif //ACTIVITYFOX_GOOGLECHROMEMANAGER_H
