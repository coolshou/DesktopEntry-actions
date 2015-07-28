#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QDebug>

MainWindow::MainWindow(int &argc, char **argv, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    int i;
    int iParser=0;
    ui->setupUi(this);
    //read config file
    QSettings *GlobalSettings = new QSettings(configFile,QSettings::NativeFormat);
    //load files to treeview
    QStringList labels;
    labels << "Name" << "Size" << "Status" << "Checksum";
    ui->treeWidget_files->setHeaderLabels(labels);

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

                QTreeWidgetItem * topLevel = new QTreeWidgetItem();
                topLevel->setText(0,  argv[i]); //name
                int size = 0;
                QFile myFile(argv[i]);
                if (myFile.open(QIODevice::ReadOnly)){
                    size = myFile.size();  //when file does open.
                    myFile.close();
                }
                topLevel->setText(1,  QString::number(size)); //size

                ui->treeWidget_files->addTopLevelItem(topLevel);
                //TODO: parser many file will need times => move to thread?
                QApplication::processEvents();
            }
        }
    } else {
    }

    //
    delete GlobalSettings;
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::startHash()
{
    //treefile list
    //ui->treeView_files->children().count();
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
