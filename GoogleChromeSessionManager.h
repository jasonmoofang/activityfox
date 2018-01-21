#ifndef ACTIVITYFOX_GOOGLECHROMESESSIONMANAGER_H
#define ACTIVITYFOX_GOOGLECHROMESESSIONMANAGER_H

#include "ChromiumSessionManager.h"

class GoogleChromeSessionManager : public ChromiumSessionManager {
public:
    GoogleChromeSessionManager(QStringList allArguments, const QString &binPath, const QString &profileDir,
                               const QString &templateProfileName,
                               const QString &profileName);

private:

    QString getDefaultBinPath() override;

    QString getDefaultProfileDir() override;
};


#endif //ACTIVITYFOX_GOOGLECHROMESESSIONMANAGER_H
