#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QCryptographicHash>
#include <QStringList>
#include <QSettings>
#include <QCloseEvent>
#include <QTime>

#include "hasherthread.h"
#include "progressbardelegate.h"
#include "customRoles.h"
#include "ui_options.h"

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

signals:
    void setThreadStop(bool bStop);

public slots:
    void slot_doHash();
    void slot_setStatus(int idx, QString sMsg);
    void slot_setError(int idx, QString sMsg);
    void slot_setProgress(int idx, qint64 iPos);
    void slot_setChecksum(int idx, QString chksum);
    void slot_save();
    void slot_load();

private slots:
    void p_slot_myDebug(QString msg);
    void p_slot_prepareRightClickMenu( const QPoint & pos );
    void slot_openOptions();
    void slot_saveOptions();
    void slot_loadOptions();

private:
    Ui::MainWindow *ui;
    Ui::OptionsWindow *ui_option;
    QByteArray fileChecksum(const QString &fileName,
                            QCryptographicHash::Algorithm hashAlgorithm);
    QString configFile;
    int  hashAlg;
    int isCheckMode; //0: calculate mode, 1: check mode
    int addTopLevelItem(QString sName );
    int clearTopLevelItem();
    int  parserChechsumFile(QString sFileName);
    int parserMD5File(QString sFileName);
    void saveMD5file(QString filename);
    bool startHash();
    void closeEvent(QCloseEvent *event);
    bool checkThreadStoped();
    void delay( int millisecondsToWait );

};

#endif // MAINWINDOW_H
