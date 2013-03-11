#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KLocale>
#include <KMainWindow>
#include <KConfigGroup>
#include <KProcess>
//#include <KMessageBox>
#include <iostream>

const char* firefoxbin_path = "/usr/lib64/firefox/firefox-bin";

class MainWindow : public KMainWindow
{
  public:
    MainWindow(QString profile = "", QWidget *parent=0);
  protected:
    void saveProperties(KConfigGroup& );
    void readProperties(const KConfigGroup& );
  private:
    QString firefoxprofile;
    KProcess p;
};

MainWindow::MainWindow(QString profile, QWidget *parent) : KMainWindow(parent)
{
    setGeometry(100,100,200,100);
    if (profile != "") {
        firefoxprofile = profile;
        QStringList createProfileCommand;
        createProfileCommand << firefoxbin_path << "-CreateProfile" << firefoxprofile;
        KProcess::execute(createProfileCommand);
        p << firefoxbin_path << "-P" << firefoxprofile << "-no-remote";
        p.start();
    } else {
        firefoxprofile = "";
    }
}

void MainWindow::saveProperties(KConfigGroup& conf) {
    conf.writeEntry("ffProfile", firefoxprofile);
    p.kill();
}

void MainWindow::readProperties(const KConfigGroup& conf) {
    firefoxprofile = conf.readEntry("ffProfile", QString());
    p << firefoxbin_path << "-P" << firefoxprofile << "-no-remote";
    p.start();
}

int main (int argc, char *argv[])
{
  KAboutData aboutData( "activityfox", 0,
      ki18n("ActivityFox"), "0.1",
      ki18n("Help Firefox work with KDE activities."),
      KAboutData::License_GPL,
      ki18n("Copyright (c) 2012 Yuen Hoe (Jason moofang)") );
  KCmdLineArgs::init( argc, argv, &aboutData );
  KCmdLineOptions options;
  options.add("+profilename", ki18n("Firefox profile name"));
  KCmdLineArgs::addCmdLineOptions(options);
  if (argc <= 1) {
      std::cout << "Please specify firefox profile name" << std::endl;
      return 0;
  }
 
  KApplication app;
  if ( app.isSessionRestored() ) {
    kRestoreMainWindows< MainWindow >();
  } else {
    // create default application as usual
    // example:
    MainWindow * window = new MainWindow(argv[1]);
    window->setObjectName("MyWindow#");
    window->show();
  }
 
  return app.exec();
}