#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QCryptographicHash>
#include <QStringList>
#include <QSettings>
#include <QCloseEvent>
#include <QTime>
#include <QClipboard>
#include <QKeyEvent>

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
    void setCurrentPath(QString sPath);
    virtual void keyPressEvent(QKeyEvent *event);
    QString getHashAlgString(int hAlg, bool bFilter=0);

signals:
    void setThreadStop(bool bStop);
    void KeyPressed();

public slots:
    void slot_doHash();
    void slot_setStatus(QTreeWidgetItem *itm, QString sMsg);
    void slot_setError(QTreeWidgetItem *itm, QString sMsg);
    void slot_setProgress(QTreeWidgetItem *itm, qint64 iPos);
    void slot_setChecksum(QTreeWidgetItem*itm, QString chksum);
    void slot_save();
    void slot_load();
    void slot_add();
    void slot_copyChecksum();

private slots:
    void p_slot_myDebug(QString msg);
    void p_slot_prepareRightClickMenu( const QPoint & pos );
    void slot_openOptions();
    void slot_saveOptions();
    void slot_loadOptions();
    void slot_pressed();

private:
    Ui::MainWindow *ui;
    Ui::OptionsWindow *ui_option;
    QClipboard* clipboard;
    QByteArray fileChecksum(const QString &fileName,
                            QCryptographicHash::Algorithm hashAlgorithm);
    QString configFile;
    QDir currentDIR; //store current path
    //QString currentPath; //store hashsum file path; or current running launch path
    int  hashAlg;
    int isCheckMode; //0: calculate mode, 1: check mode
    QTreeWidgetItem * addTopLevelItem(QString sName );
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
