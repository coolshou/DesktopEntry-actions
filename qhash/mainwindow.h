#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QCryptographicHash>
#include <QStringList>
#include <QSettings>

#include "hasherthread.h"
#include "progressbardelegate.h"
#include "customRoles.h"

//define
enum {
    COL_NAME=0,
    COL_SIZE,
    COL_STATUS,
    COL_CHECKSUM
};


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int &argc, char **argv, QWidget *parent = 0);
    ~MainWindow();
    void setConfigFile(QString sFileName);

public slots:
    void doHash();
    void setStatus(int idx, QString sMsg);
    void setProgress(int idx, qint64 iPos);
    void setChecksum(int idx, QString chksum);
    void save();
    void load();

private slots:
    void myDebug(QString msg);
    void prepareRightClickMenu( const QPoint & pos );

private:
    Ui::MainWindow *ui;
    QByteArray fileChecksum(const QString &fileName,
                            QCryptographicHash::Algorithm hashAlgorithm);
    QString configFile; //TODO
    int isCheckMode; //0: calculate mode, 1: check mode
    int  parserChechsumFile(QString sFileName);

    bool startHash();

};

#endif // MAINWINDOW_H
