#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QDebug>
#include <QVariant>
#include <QMenu>
#include <QFileDialog>

QString SUPPORT_Filter("md5 files (*.md5);;sha files (*.sha)");
QString DEFAULT_Filter("md5 files (*.md5)");

//return toplevelItem index number
int MainWindow::addTopLevelItem(QString sName )
{
    int idx;
    QTreeWidgetItem * topLevelitem = new QTreeWidgetItem();
    topLevelitem->setText(COL_NAME, sName); //name
    int size = 0;
    QFile myFile(sName);
    if (myFile.open(QIODevice::ReadOnly)){
        size = myFile.size();  //when file does open.

        topLevelitem->setText(COL_SIZE,  QString::number(size)); //size
        topLevelitem->setTextAlignment(COL_SIZE, Qt::AlignRight);
        //TODO: progressbar
        //add hasherThread, TODO: QCryptographicHash type
       HasherThread * hasherThread = new HasherThread( this ,  myFile.fileName(), QCryptographicHash::Md5);
       QVariant v;
       v.setValue(hasherThread);
        topLevelitem->setData(COL_STATUS, MyHashThreadRole, v);
        topLevelitem->setData(COL_STATUS, MyMinimumRole, 0);
        topLevelitem->setData(COL_STATUS, MyMaximumRole, size);
        topLevelitem->setText(COL_STATUS,  "TEST"); //terst

        //connect( hasherThread, SIGNAL(error(const QString &)), ui->treeWidget_files, SLOT() );
        connect( hasherThread, SIGNAL(error(int, const QString &)), this, SLOT(setStatus(int,QString)) );
        connect( hasherThread, SIGNAL(fileReadPos(int,qint64)), this, SLOT(setProgress(int,qint64)) );
        //TODO: calc mode or checkmode
        connect( hasherThread, SIGNAL(completed(int, const QString &)), this, SLOT(setChecksum(int,QString)) );

        ui->treeWidget_files->addTopLevelItem(topLevelitem);
        idx = ui->treeWidget_files->indexOfTopLevelItem(topLevelitem);
        hasherThread->setIdx(idx);

        myFile.close();
    }
    return idx;
}

MainWindow::MainWindow(int &argc, char **argv, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    isCheckMode=0;
    int i;
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
            qDebug() <<"parserChechsumFile:" << argv[1];

           iParser = parserChechsumFile(argv[1]);
        }
        if ( iParser <= 0 ) {
            //many file : add file to treeWidget_files,
            for(i =1; i<argc; i++) {
                qDebug() << "argv[" <<  i << "]:" << argv[i];

                addTopLevelItem( argv[i]);
                //TODO: parser many file will need times => move to thread?
                QApplication::processEvents();
            }
        }
    } else {
        //
    }

    //add right click menu to ui->treeWidget_files
    ui->treeWidget_files->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->treeWidget_files,SIGNAL(customContextMenuRequested(const QPoint&)),
            this,SLOT(prepareRightClickMenu(const QPoint&)));

    //signal/solt of button push
    QObject::connect(ui->pushButton_hash,SIGNAL(clicked()),this,SLOT(doHash()));

    delete GlobalSettings;
}

MainWindow::~MainWindow()
{
    delete ui;
}

/* md5sum file content format
 * md5sum<space><space>filename
 * second <space> indicate text mode
 * use <asterisk> for binary mode
 * line must end with UNIX line ending
*/
void MainWindow::saveMD5file(QString filename)
{
    QFile f( filename);

    f.open( QIODevice::WriteOnly );//TODO: check file writeable
    //TODO store data in f
    QTextStream out(&f);
    QTreeWidget *tree = ui->treeWidget_files;
    for (int i=0;i<tree->topLevelItemCount();i++){
        QTreeWidgetItem * topLevelitem = tree->topLevelItem(i);
//            qDebug() << "name:" <<topLevelitem->text(COL_NAME);
//            qDebug() << "checksum:" << topLevelitem->text(COL_CHECKSUM);
        out << topLevelitem->text(COL_CHECKSUM) << "  " << topLevelitem->text(COL_NAME) << endl;
    }
    f.close();
}

void MainWindow::save()
{
    //save shecksum to file
    qDebug() << "Save to file";
    //TODO: check we do have checksum to save and all hasherthread had done it's job

    QFileDialog *fileDialog = new QFileDialog;
//    fileDialog->setDefaultSuffix("md5");

    QString filename = fileDialog->getSaveFileName( this, "Save file", QDir::currentPath(), SUPPORT_Filter , &DEFAULT_Filter);
    if (! (filename.endsWith(".md5", Qt::CaseInsensitive) ||
            filename.endsWith(".sha", Qt::CaseInsensitive) )) {
        //TODO: default ext from setting option
        filename += ".md5"; //default
    }
    if (filename != "") {
        if (filename.endsWith(".md5",Qt::CaseInsensitive)) {
            saveMD5file(filename);
        } else {
            qDebug() << "TODO: save other format of shecksum file: " << filename;
        }
    }
}

void MainWindow::load()
{
    //Load shecksum from file
    qDebug() << "TODO: Load from file";
    //TODO: file select
    // parserChechsumFile(QString sFileName);
}

void MainWindow::prepareRightClickMenu( const QPoint & pos )
{
    QTreeWidget *tree = ui->treeWidget_files;

    //QTreeWidgetItem *nd = tree->itemAt( pos );
    //qDebug()<<pos<<nd->text(0);
    //TODO: load checksum file
    QAction *loadAct = new QAction(QIcon(":/pixmaps/load.png"), tr("&Load"), this);
    connect(loadAct, SIGNAL(triggered()), this, SLOT(load()));

    QAction *saveAct = new QAction(QIcon(":/pixmaps/save.png"), tr("&Save"), this);
    //newAct->setStatusTip(tr("Save"));
    //TODO: only finish checksum then can be save
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));
    //menu
    QMenu menu(this);
    menu.addAction(loadAct);
    menu.addAction(saveAct);

    //QPoint pt(pos);
    menu.exec( tree->mapToGlobal(pos) );
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

void  MainWindow::setConfigFile(QString sFileName)
{
    configFile=sFileName;
}


int MainWindow::parserMD5File(QString sFileName)
{
    //read file line by line
    QFile f(sFileName);
    //TODO: check file can be read?
    f.open( QIODevice::ReadOnly );
    QTextStream in(&f);
    while (!in.atEnd()) {
        QString line = in.readLine();
        //TODO: binary mode
        QStringList fields = line.split(' ');
        qDebug() << "fields" << fields;
        if (fields.size() == 3) {
            qDebug() << "0:" << fields.at(0); //checksum
            qDebug() << "1:" << fields.at(1);
            qDebug() << "2:" << fields.at(2);//filename

            int idx =  addTopLevelItem( fields.at(2));
            setChecksum(idx, fields.at(0));
            /*
            int row = fields.takeFirst().toInt();
            int column = fields.takeFirst().toInt();
            setFormula(row, column, fields.join(' '));
            */
        }

    }
    return 1;
}

/*
 * -1 : error happen
 * 0:
 * >0: files be process
*/
int  MainWindow::parserChechsumFile(QString sFileName)
{
    int rs=0;
    //configFile=sFileName;
    isCheckMode =1;

    if (sFileName.endsWith(".md5", Qt::CaseInsensitive)) {
        rs = parserMD5File(sFileName);
    } else {
        qDebug() << "TODO: parser file: " << sFileName;
        rs = 0;
    }

    return rs;
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
}

void MainWindow::setChecksum(int idx, QString chksum)
{
       QTreeWidgetItem * topLevelitem = ui->treeWidget_files->topLevelItem(idx);
       topLevelitem->setText(COL_CHECKSUM,chksum);
       if (isCheckMode) {
           //TODO: checkmode?
        topLevelitem->setTextColor(COL_CHECKSUM,Qt::gray);
       } else {
         //calc mode?

       }
       //TODO: update status

}
