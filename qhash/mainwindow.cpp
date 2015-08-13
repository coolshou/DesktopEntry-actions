#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QFile>
#include <QDebug>
#include <QVariant>
#include <QMenu>
#include <QFileDialog>


QString SUPPORT_Filter("md4 files (*.md4);;md5 files (*.md5);;sha1 files (*.sha1);;sha256 files (*.sha256);;sha512 files (*.sha512)");
QString DEFAULT_Filter("md5 files (*.md5)");

int MainWindow::clearTopLevelItem()
{
    ui->treeWidget_files->clear();
    return 0;
}

//return toplevelItem index number
int MainWindow::addTopLevelItem(QString sName )
{
    //const int pageSize = getpagesize();
    //qDebug() << "pageSize:" << QString::number(pageSize);
    int idx;
    QTreeWidgetItem * topLevelitem = new QTreeWidgetItem();
    topLevelitem->setText(COL_NAME, sName); //name
    qint64 size = 0;
    QFile myFile(sName);
    if (myFile.open(QIODevice::ReadOnly)){
        size = myFile.size();  //when file does open.

        topLevelitem->setText(COL_SIZE,  QString::number(size)); //size
        topLevelitem->setTextAlignment(COL_SIZE, Qt::AlignRight);
        //add hasherThread,
        //TODO: QCryptographicHash type
       HasherThread * hasherThread = new HasherThread( this ,  myFile.fileName(),QCryptographicHash::Algorithm(hashAlg));
       QVariant v;
       v.setValue(hasherThread);
        topLevelitem->setData(COL_STATUS, MyHashThreadRole, v);
        topLevelitem->setData(COL_STATUS, MyMinimumRole, 0);
        topLevelitem->setData(COL_STATUS, MyMaximumRole, 100);
        //topLevelitem->setText(COL_STATUS,  "TEST"); //test
        topLevelitem->setData(COL_CHECKSUM, MyCheckModeRole, isCheckMode); //default 0: calc mode, 1: check mode

        connect( hasherThread, SIGNAL(error(int, const QString &)), this, SLOT(slot_setError(int,QString)) );
        connect( hasherThread, SIGNAL(fileReadPos(int,qint64)), this, SLOT(slot_setProgress(int,qint64)) );
        connect( this, SIGNAL(setThreadStop(bool)), hasherThread, SLOT(slot_setStop(bool)) );
        connect( hasherThread, SIGNAL(completed(int, const QString &)), this, SLOT(slot_setChecksum(int,QString)) );

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
    //TODO: show UI position relative to mouse position or at center of screen?
    isCheckMode=0;
    int i;
    int iParser=0;
    ui->setupUi(this);
    //load config from config file
    slot_loadOptions();
    clipboard =  QApplication::clipboard();
    /*
    QString originalText = clipboard->text();
    clipboard->setText( newText );
    */
    //QSettings *GlobalSettings = new QSettings(configFile,QSettings::NativeFormat);
    //TODO: read config setting
    //Delegate ProgressBar
    ui->treeWidget_files->setItemDelegateForColumn(COL_STATUS, new ProgressBarDelegate(ui->treeWidget_files));

    if (argc > 1) {
        //qDebug() << "argv[1]:" <<  argv[1];
        if (argc==2) {
            //TODO: only 1 file :expect md5 file contain filelist for check
           //qDebug() <<"parserChechsumFile:" << argv[1];
           iParser = parserChechsumFile(argv[1]);
        }
        if ( iParser <= 0 ) {
            //many file : add file to treeWidget_files,
            for(i =1; i<argc; i++) {
                //load files to treeview
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
            this,SLOT(p_slot_prepareRightClickMenu(const QPoint&)));

    //signal/solt of button push
    QObject::connect(ui->pushButton_hash,SIGNAL(clicked()),this,SLOT(slot_doHash()));
    connect(ui->pushButton_option,SIGNAL(clicked()),this,SLOT(slot_openOptions()));

    //delete GlobalSettings;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slot_openOptions()
{
    ui_option = new Ui::OptionsWindow;
     QDialog *dialog = new QDialog;
     ui_option->setupUi(dialog);
     //load config to UI
     switch (hashAlg) {
         case QCryptographicHash::Md4:
             ui_option->radioButton_md4->setChecked(true);
             break;
         case QCryptographicHash::Md5:
             ui_option->radioButton_md5->setChecked(true);
             break;
         case QCryptographicHash::Sha1:
             ui_option->radioButton_sha1->setChecked(true);
             break;
         case QCryptographicHash::Sha256:
             ui_option->radioButton_sha256->setChecked(true);
             break;
         case QCryptographicHash::Sha512:
             ui_option->radioButton_sha512->setChecked(true);
             break;
     }
     ui_option->buttonBox->button(ui_option->buttonBox->Cancel)->setFocus();
     //signal/slot
     connect(ui_option->buttonBox->button((QDialogButtonBox::Ok)), SIGNAL(clicked()), this, SLOT(slot_saveOptions()));
    //show options dialog
    dialog->show();
}

//save options settings
void MainWindow::slot_saveOptions()
{
    QString hashmode;
//    qDebug() << "saveOptions";
//    qDebug() << "checkedId" << ui_option->buttonGroup_hash->checkedId();
//    qDebug() << "checkedButton" << ui_option->buttonGroup_hash->checkedButton()->text();
    hashmode = ui_option->buttonGroup_hash->checkedButton()->text();
    if  ( hashmode == "md4") {
            hashAlg = QCryptographicHash::Md4;
    } else if  ( hashmode ==  "md5") {
            hashAlg = QCryptographicHash::Md5;
    } else if  ( hashmode ==  "sha1") {
            hashAlg = QCryptographicHash::Sha1;
    } else if  ( hashmode ==  "sha256") {
            hashAlg = QCryptographicHash::Sha256;
    } else if  ( hashmode ==  "sha512") {
            hashAlg = QCryptographicHash::Sha512;
    } else {
            hashAlg = QCryptographicHash::Md5;
    }

    QSettings *GlobalSettings = new QSettings(configFile,QSettings::NativeFormat);
    GlobalSettings->beginGroup("hash");
    GlobalSettings->setValue(CFG_KEY_HashAlg, hashAlg);
    GlobalSettings->endGroup();
    delete GlobalSettings;
}
//load options settings from cconfig file
void MainWindow::slot_loadOptions()
{
    QSettings *GlobalSettings = new QSettings(configFile,QSettings::NativeFormat);
    GlobalSettings->beginGroup("hash");
    hashAlg =  GlobalSettings->value(CFG_KEY_HashAlg,QCryptographicHash::Md5).toInt();
    GlobalSettings->endGroup();
    delete GlobalSettings;
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

    //check file writeable
    if (f.open( QIODevice::WriteOnly )) {
        //store data in f
        QTextStream out(&f);
        QTreeWidget *tree = ui->treeWidget_files;
        for (int i=0;i<tree->topLevelItemCount();i++){
            QTreeWidgetItem * topLevelitem = tree->topLevelItem(i);
            out << topLevelitem->text(COL_CHECKSUM) << "  " << topLevelitem->text(COL_NAME) << endl;
            QApplication::processEvents();
        }
        f.close();
    } else {
        //TODO: error info for file not write able
    }
}

//save shecksum to file
void MainWindow::slot_save()
{
    // "Save to file";
    qDebug() << "TODO: check we do have checksum to save and all hasherthread had done it's job";

    QFileDialog *fileDialog = new QFileDialog;
//    fileDialog->setDefaultSuffix("md5");

    QString filename = fileDialog->getSaveFileName( this, "Save file", QDir::currentPath(), SUPPORT_Filter , &DEFAULT_Filter);
    if (! (filename.endsWith(".md5", Qt::CaseInsensitive) ||
            filename.endsWith(".sha1", Qt::CaseInsensitive) )) {
        //TODO: md4, sha256, sha512
        //default ext from setting option
        filename += ".md5"; //default
    }
    if (filename != "") {
        if (filename.endsWith(".md5",Qt::CaseInsensitive)) {
            //TODO: md4, sha256, sha512
            saveMD5file(filename);
        } else {
            qDebug() << "TODO: save other format of shecksum file: " << filename;
        }
    }
}
// copy checksum value
void MainWindow::slot_copyChecksum()
{
    QTreeWidget *tree = ui->treeWidget_files;
    clipboard->setText(tree->currentItem()->text(tree->currentColumn()));
}
//load checksum file
void MainWindow::slot_load()
{
    //Load shecksum from file
    QFileDialog *fileDialog = new QFileDialog(this, "Select file", QDir::currentPath(), SUPPORT_Filter );
    fileDialog->setFileMode(QFileDialog::ExistingFile);
    QStringList fileNames;
    if (fileDialog->exec()) {
       fileNames = fileDialog->selectedFiles();
        if (fileNames.count() >0 ) {
            //do we need multi file select?
           QString filename = fileNames.first();
            clearTopLevelItem();
            parserChechsumFile(filename);
        }
    }
}

void MainWindow::p_slot_prepareRightClickMenu( const QPoint & pos )
{
    int curCol;
    QTreeWidget *tree = ui->treeWidget_files;
    //load
    QAction *loadAct = new QAction(QIcon(":/pixmaps/load.png"), tr("&Load"), this);
    connect(loadAct, SIGNAL(triggered()), this, SLOT(slot_load()));
    //save
    QAction *saveAct = new QAction(QIcon(":/pixmaps/save.png"), tr("&Save"), this);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(slot_save()));
    //add menu for copy checksum to clipboard
    curCol = tree->currentColumn();
    //if (curCol == COL_CHECKSUM) {
    //copy
        QAction *copyAct = new QAction(QIcon(":/pixmaps/copy.png"), tr("&Copy"), this);
        connect(copyAct, SIGNAL(triggered()), this, SLOT(slot_copyChecksum()));
    //}
    //QTreeWidgetItem *itm =   tree->itemAt(pos.x(),pos.y);
    //qDebug() << "QTreeWidgetItem" <<itm->text();
    //TODO: add files menu item & action

    //menu, first time right click will not show menu
    /*
    tree->setContextMenuPolicy(Qt::ActionsContextMenu);
    tree->addAction(loadAct);
    tree->addAction(saveAct);
    */

    //menu
    QMenu menu(this);
    if (curCol == COL_CHECKSUM) {
        menu.addAction(copyAct);
        //TODO: add sepelate bar
    }

    menu.addAction(loadAct);
    menu.addAction(saveAct);
    //QPoint pt(pos);
    menu.exec( tree->mapToGlobal(pos) );

}
void MainWindow::slot_doHash()
{
    startHash();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    //tell all thread stop runnng
   emit setThreadStop(true);
    //wait
    delay(500);
    //check if threadis running
    if (checkThreadStoped()) {
        event->accept();
    } else {
        qDebug() << "some thread not finish running!! can not close application now";
        event->ignore();
    }
}

void MainWindow::delay( int millisecondsToWait )
{
    QTime dieTime = QTime::currentTime().addMSecs( millisecondsToWait );
    while( QTime::currentTime() < dieTime )
    {
        QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
}

/* return:
// true : all thread stop
// false: one of the thread not stop
*/
bool MainWindow::checkThreadStoped()
{
    int i;
    //treefile toplevel item
    for (i=0;i<ui->treeWidget_files->topLevelItemCount();i++){
       QTreeWidgetItem * topLevelitem = ui->treeWidget_files->topLevelItem(i);
        //qDebug() << topLevelitem->text(COL_NAME);
        //get hasherthread
        QVariant v = topLevelitem->data(COL_STATUS, MyHashThreadRole);
        HasherThread * hasherT = v.value<HasherThread*>();
         //qDebug() << "isRunning: " << hasherT->isRunning();
         if (hasherT->isRunning()) {
            return false;
         }
        QApplication::processEvents();
    }
    return true;
}

bool MainWindow::startHash()
{
    int i;
    int cMode;
    //treefile toplevel item
    if (checkThreadStoped()) {
        for (i=0;i<ui->treeWidget_files->topLevelItemCount();i++){
           QTreeWidgetItem * topLevelitem = ui->treeWidget_files->topLevelItem(i);
           cMode = topLevelitem->data(COL_CHECKSUM,MyCheckModeRole).toInt();
            //qDebug() << topLevelitem->text(COL_NAME);
            if (cMode!=1) {
                //not checkMode, clear checksum
                topLevelitem->setText(COL_CHECKSUM,"");
            }
            //get hasherthread
            QVariant v = topLevelitem->data(COL_STATUS, MyHashThreadRole);
            HasherThread * hasherT = v.value<HasherThread*>();
            hasherT->start();
            QApplication::processEvents();
        }
    }
    return true;
}

void  MainWindow::setConfigFile(QString sFileName)
{
    configFile=sFileName;
}

//TODO: md4, sha1, sha256, sha512 checksum file format?
int MainWindow::parserMD5File(QString sFileName)
{
      int idx ;
    //read file line by line
    QFile f(sFileName);
    //check file can be read
    if (f.open( QIODevice::ReadOnly )) {
        QTextStream in(&f);
        while (!in.atEnd()) {
            QString line = in.readLine();
            //TODO: binary mode
            QStringList fields = line.split(' ');
            //qDebug() << "fields" << fields;
            if (fields.size() == 3) {
               // qDebug() << "0:" << fields.at(0); //checksum
               // qDebug() << "1:" << fields.at(1);
               // qDebug() << "2:" << fields.at(2);//filename

                idx =  addTopLevelItem( fields.at(2));
                slot_setChecksum(idx, fields.at(0));
                /*
                int row = fields.takeFirst().toInt();
                int column = fields.takeFirst().toInt();
                setFormula(row, column, fields.join(' '));
                */
            }
            QApplication::processEvents();
        }
        return idx;
    } else {
        //TODO: error info for file can not be read
        return -1;
    }
    return 0;
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
    } else if (sFileName.endsWith(".sha", Qt::CaseInsensitive)) {
        qDebug() << "TODO: parser sha file: " << sFileName;
    } else {
        qDebug() << "TODO: parser file: " << sFileName;
    }
    return rs;
}

void MainWindow::p_slot_myDebug(QString msg)
{
    qDebug() << msg;
}

void MainWindow::slot_setError(int idx, QString sMsg)
{
    //QTreeWidgetItem * topLevelitem = ui->treeWidget_files->topLevelItem(idx);
    //topLevelitem->setText(COL_STATUS,sMsg);
    qDebug() << QString::number(idx) << ": " <<sMsg;
}


void MainWindow::slot_setStatus(int idx, QString sMsg)
{
    QTreeWidgetItem * topLevelitem = ui->treeWidget_files->topLevelItem(idx);
    topLevelitem->setText(COL_STATUS,sMsg);
}

void MainWindow::slot_setProgress(int idx, qint64 iPos)
{
    slot_setStatus(idx, QString::number(iPos));
}

void MainWindow::slot_setChecksum(int idx, QString chksum)
{
    int cMode;
   QTreeWidgetItem * topLevelitem = ui->treeWidget_files->topLevelItem(idx);

   cMode = topLevelitem->data(COL_CHECKSUM,MyCheckModeRole).toInt();
   if (cMode==1) {
       //TODO: checkmode?
    topLevelitem->setTextColor(COL_CHECKSUM,Qt::gray);
    qDebug() << "TODO: compare hash";
   } else {
     //calc mode?
   }
       //TODO: update status
   topLevelitem->setText(COL_CHECKSUM,chksum);
}
