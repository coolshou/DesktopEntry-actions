#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include <QDir>

#include <QDebug>

const QString AppName = "qhash";
const QString AppMajorVersion = "0";
const QString AppMinorVersion = "2";
const QString AppVersion = AppMajorVersion + "." + AppMinorVersion;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(AppName);
    app.setApplicationVersion(AppVersion);

    MainWindow w(argc, argv);
    w.setConfigFile(QString("%1%2%3%4%5").arg(QDir::homePath(), "/.local/",AppName, "/", "qhash.conf"));
//qDebug() << "applicationDirPath" <<app.applicationDirPath();
    w.setCurrentPath(app.applicationDirPath());
    w.show();
    int exitCode;
    {
        exitCode =app.exec();
    }
    return exitCode;
}
