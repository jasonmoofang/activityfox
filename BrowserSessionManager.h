#ifndef ACTIVITYFOX_BROWSERSESSIONMANAGER_H
#define ACTIVITYFOX_BROWSERSESSIONMANAGER_H

#include <KApplication>
#include <KSessionManager>
#include <KProcess>
#include <QDebug>

class BrowserSessionManager : public KApplication {
public:
    BrowserSessionManager(QStringList allArguments, const QString &binPath, const QString &profileDir,
               const QString &templateProfileName, const QString &profileName);

    void commitData(QSessionManager &sm) override;

    virtual void init();

protected:


private:
    QStringList restartArguments;
    QString browserProfileName;
    QString browserBinPath;
    QString browserProfileDir;
    QString browserTemplateProfileName;
    KProcess browserProcess;

    QString getCurrentActivityId();

    virtual bool checkIfProfileExists(const QString &profileDir, const QString &profileName) = 0;

    virtual void closeBrowser(KProcess &process);

    virtual void startBrowser(KProcess &browserProcess, const QString &binPath, const QString &profileDir, const QString &profileName) = 0;

    virtual void createProfile(const QString &binPath, const QString &profileDir, const QString &profileName,
                       const QString &templateProfileName) = 0;

    virtual void copyProfile(const QString &profileDir, const QString &fromName, const QString &toName) = 0;

    virtual QString getDefaultBinPath() = 0;

    virtual QString getDefaultProfileDir() = 0;

};


#endif //ACTIVITYFOX_BROWSERSESSIONMANAGER_H
