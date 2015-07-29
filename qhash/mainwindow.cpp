#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QDebug>
#include <QVariant>

MainWindow::MainWindow(int &argc, char **argv, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int i, idx;
    int iParser=0;
    ui->setupUi(this);
    //read config file
    QSettings *GlobalSettings = new QSettings(configFile,QSettings::NativeFormat);
    //load files to treeview
    //move to .UI
    //QStringList labels;
    //labels << "Name" << "Size" << "Status" << "Checksum";
    //ui->treeWidget_files->setHeaderLabels(labels);
    ui->treeWidget_files->setItemDelegateForColumn(COL_STATUS, new ProgressBarDelegate(ui->treeWidget_files));

    qDebug() << "argc:" <<  argc;
    if (argc > 1) {
        //qDebug() << "argv[1]:" <<  argv[1];
        if (argc==2) {
            //TODO: only 1 file :expect md5 file contain filelist for check
           iParser = parserChechsumFile(argv[1]);
        }
        if ( iParser <= 0 ) {
            //many file : add file to treeWidget_files,
            for(i =1; i<argc; i++) {
                qDebug() << "argv[" <<  i << "]:" << argv[i];

                QTreeWidgetItem * topLevelitem = new QTreeWidgetItem();
                topLevelitem->setText(COL_NAME,  argv[i]); //name
                int size = 0;
                QFile myFile(argv[i]);
                if (myFile.open(QIODevice::ReadOnly)){
                    size = myFile.size();  //when file does open.

                    topLevelitem->setText(COL_SIZE,  QString::number(size)); //size
                    topLevelitem->setTextAlignment(COL_SIZE, Qt::AlignRight);
                    //TODO: progressbar
                    //TODO: add hashthread?
                   HasherThread * hasherThread = new HasherThread( this ,  myFile.fileName(), QCryptographicHash::Md5);
                   QVariant v;
                   v.setValue(hasherThread);
                    topLevelitem->setData(COL_STATUS, MyHashThreadRole, v);
                    //topLevelitem->setTextAlignment(COL_STATUS, Qt::AlignCenter);
                    topLevelitem->setData(COL_STATUS, MyMinimumRole, 0);
                    topLevelitem->setData(COL_STATUS, MyMaximumRole, size);


                    //connect( hasherThread, SIGNAL(error(const QString &)), ui->treeWidget_files, SLOT() );
                    //connect( hasherThread, SIGNAL(completed(int, const QString &)), this, SLOT(myDebug(const QString &)) );
                    connect( hasherThread, SIGNAL(error(int, const QString &)), this, SLOT(setStatus(int,QString)) );
                    connect( hasherThread, SIGNAL(fileReadPos(int,qint64)), this, SLOT(setProgress(int,qint64)) );
                    connect( hasherThread, SIGNAL(completed(int, const QString &)), this, SLOT(setChecksum(int,QString)) );

                    ui->treeWidget_files->addTopLevelItem(topLevelitem);
                    idx = ui->treeWidget_files->indexOfTopLevelItem(topLevelitem);
                    hasherThread->setIdx(idx);

                    myFile.close();
                }
                //TODO: parser many file will need times => move to thread?
                QApplication::processEvents();
            }
        }
    } else {
        //
    }


    //signal/solt of button push
    QObject::connect(ui->pushButton_hash,SIGNAL(clicked()),this,SLOT(doHash()));

    delete GlobalSettings;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::doHash()
{
    //qDebug() << "doHash";
    startHash();
}

bool MainWindow::startHash()
{
    int i;
    //treefile toplevel item
    for (i=0;i<ui->treeWidget_files->topLevelItemCount();i++){
       QTreeWidgetItem * topLevelitem = ui->treeWidget_files->topLevelItem(i);
        qDebug() << topLevelitem->text(COL_NAME);
        //get hasherthread
        QVariant v = topLevelitem->data(COL_STATUS, MyHashThreadRole);
        HasherThread * hasherT = v.value<HasherThread*>();
        hasherT->start();
        QApplication::processEvents();
    }
    return true;
}

/*
// Returns empty QByteArray() on failure. TODO: run in thread?
QByteArray MainWindow::fileChecksum(const QString &fileName,
                        QCryptographicHash::Algorithm hashAlgorithm)
{
    QFile f(fileName);
    if (f.open(QFile::ReadOnly)) {
        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&f)) {
            return hash.result();
        }
    }
    return QByteArray();
}
*/

void  MainWindow::setConfigFile(QString sFileName)
{
    configFile=sFileName;
}

/*
 * -1 : error happen
 * 0:
 * >0: files be process
*/
int  MainWindow::parserChechsumFile(QString sFileName)
{
    //configFile=sFileName;
    return 0;
}

void MainWindow::myDebug(QString msg)
{
    qDebug() << msg;
}

void MainWindow::setStatus(int idx, QString sMsg)
{
    QTreeWidgetItem * topLevelitem = ui->treeWidget_files->topLevelItem(idx);
    topLevelitem->setText(COL_STATUS,sMsg);
}

void MainWindow::setProgress(int idx, qint64 iPos)
{
    //TODO: this will cause high CPU usage!!
    setStatus(idx, QString::number(iPos));
    //TODO: update progressbar
}

void MainWindow::setChecksum(int idx, QString chksum)
{
       QTreeWidgetItem * topLevelitem = ui->treeWidget_files->topLevelItem(idx);
       //TODO: update status
       topLevelitem->setText(COL_CHECKSUM,chksum);
}
