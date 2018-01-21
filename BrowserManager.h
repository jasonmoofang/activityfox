#ifndef ACTIVITYFOX_BROWSERMANAGER_H
#define ACTIVITYFOX_BROWSERMANAGER_H

#ifdef NDEBUG
#define QT_NO_DEBUG_OUTPUT
#else

#include <QDebug>

#endif

#include <KApplication>
#include <KProcess>
#include <QSessionManager>
#include <QtDBus>

class BrowserManager {
public:
    BrowserManager(const QString &startUpActivityId, const QString &binPath, const QString &profileDir,
                   const QString &templateProfileName, const QString &profileName);

    virtual ~BrowserManager() = default;

    void init();

    void onSaveProperties();

    enum Browser {
        CHROMIUM, GOOGLE_CHROME, FIREFOX
    };

protected:


private:
    QString startUpActivityId;
    QString profileName;
    QString binPath;
    QString profileDir;
    QString templateProfileName;
    KProcess process;

    virtual bool checkIfProfileExists(const QString &profileDir, const QString &profileName) = 0;

    virtual void closeBrowser(KProcess &process);

    virtual void startBrowser(KProcess &process, const QString &binPath, const QString &profileDir,
                              const QString &profileName) = 0;

    virtual void createProfile(const QString &binPath, const QString &profileDir, const QString &profileName,
                               const QString &templateProfileName) = 0;

    virtual void copyProfile(const QString &profileDir, const QString &fromName, const QString &toName) = 0;

    virtual QString getDefaultBinPath() = 0;

    virtual QString getDefaultProfileDir() = 0;

};


#endif //ACTIVITYFOX_BROWSERMANAGER_H
