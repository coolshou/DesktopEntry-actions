#include "mainwindow.h"
#include "ui_mainwindow.h"


#include <QFile>
#include <QDebug>
#include <QVariant>
#include <QMenu>
#include <QFileDialog>


QString SUPPORT_Filter("md4 files (*.md4);;md5 files (*.md5);;sha1 files (*.sha1);;sha256 files (*.sha256);;sha512 files (*.sha512)");
QString DEFAULT_Filter("md5 files (*.md5)");
QString ALL_FILE_Filter("All files (*.*)");

int MainWindow::clearTopLevelItem()
{
    ui->treeWidget_files->clear();
    return 0;
}

//return toplevelItem index number
//int MainWindow::addTopLevelItem(QString sName )
QTreeWidgetItem * MainWindow::addTopLevelItem(QString sName )
{
    //const int pageSize = getpagesize();
    //qDebug() << "pageSize:" << QString::number(pageSize);
    //int idx;
    QTreeWidgetItem * topLevelitem = new QTreeWidgetItem();
    topLevelitem->setText(COL_NAME, sName); //name
    qint64 size = 0;
    QFile myFile(sName);
    if (myFile.open(QIODevice::ReadOnly)){
        size = myFile.size();  //when file does open.

        topLevelitem->setText(COL_SIZE,  QString::number(size)); //size
        topLevelitem->setTextAlignment(COL_SIZE, Qt::AlignRight);
        //add hasherThread,
       HasherThread * hasherThread = new HasherThread( this ,  myFile.fileName(),QCryptographicHash::Algorithm(hashAlg));
       QVariant v;
       v.setValue(hasherThread);
        topLevelitem->setData(COL_STATUS, MyHashThreadRole, v);
        topLevelitem->setData(COL_STATUS, MyMinimumRole, 0);
        topLevelitem->setData(COL_STATUS, MyMaximumRole, 100);
        //topLevelitem->setText(COL_STATUS,  "TEST"); //test
        topLevelitem->setData(COL_CHECKSUM, MyCheckModeRole, isCheckMode); //default 0: calc mode, 1: check mode

        connect( hasherThread, SIGNAL(error(QTreeWidgetItem*, const QString &)), this, SLOT(slot_setError(QTreeWidgetItem*,QString)) );
        connect( hasherThread, SIGNAL(msg(QTreeWidgetItem*, const QString &)), this, SLOT(slot_setMsg(QTreeWidgetItem*,QString)) );
        connect( hasherThread, SIGNAL(fileReadPos(QTreeWidgetItem*,qint64)), this, SLOT(slot_setProgress(QTreeWidgetItem*,qint64)) );
        connect( this, SIGNAL(setThreadStop(bool)), hasherThread, SLOT(slot_setStop(bool)) );
        connect( hasherThread, SIGNAL(completed(QTreeWidgetItem*, const QString &)), this, SLOT(slot_setChecksum(QTreeWidgetItem*,QString)) );

        ui->treeWidget_files->addTopLevelItem(topLevelitem);
        hasherThread->setItem(topLevelitem);
//toplevelitem index may change? when item remove?!
/*
        connect( hasherThread, SIGNAL(error(int, const QString &)), this, SLOT(slot_setError(int,QString)) );
        connect( hasherThread, SIGNAL(fileReadPos(int,qint64)), this, SLOT(slot_setProgress(int,qint64)) );
        connect( this, SIGNAL(setThreadStop(bool)), hasherThread, SLOT(slot_setStop(bool)) );
        connect( hasherThread, SIGNAL(completed(int, const QString &)), this, SLOT(slot_setChecksum(int,QString)) );

        ui->treeWidget_files->addTopLevelItem(topLevelitem);
        idx = ui->treeWidget_files->indexOfTopLevelItem(topLevelitem);
        hasherThread->setIdx(idx);
*/
        myFile.close();
    }
    //return idx;
    return topLevelitem;
}

MainWindow::MainWindow(int &argc, char **argv, QString cfgFile, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    //TODO: show UI position relative to mouse position or at center of screen? or load from setting
    isCheckMode=0;
    int i;
    int iParser=0;
    ui->setupUi(this);
    //load config from config file
    configFile = cfgFile;
    slot_loadOptions();
    clipboard =  QApplication::clipboard();
    /*    QString originalText = clipboard->text();
            clipboard->setText( newText );     */

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
            // TODO: get app lunch path
            QFileInfo fi(argv[1]);
            setCurrentPath(fi.path());
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
    connect(ui->pushButton_tool,SIGNAL(clicked()),this,SLOT(slot_openTools()));

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
          default:
             ui_option->radioButton_md5->setChecked(true);
             break;
     }
     ui_option->checkBox_hashUppercase->setChecked(hashUppercase);

     ui_option->buttonBox->button(ui_option->buttonBox->Cancel)->setFocus();
     //signal/slot
     connect(ui_option->buttonBox->button((QDialogButtonBox::Ok)), SIGNAL(clicked()), this, SLOT(slot_saveOptions()));
    //show options dialog
    dialog->show();
}

void MainWindow::slot_openTools()
{
    ui_tools = new Ui::ToolsWindow;
    QDialog *dialog = new QDialog;
    ui_tools->setupUi(dialog);
    //signal/slot
    connect(ui_tools->pushButton_hash,SIGNAL(clicked()),this,SLOT(slot_hashString()));
    dialog->show();
}
void MainWindow::slot_hashString()
{
    if (ui_tools->lineEdit_src->text().size() >0) {
        if (ui_tools->checkBox_md4->isChecked()) {
            QByteArray result = hash->hash(ui_tools->lineEdit_src->text().toUtf8() ,QCryptographicHash::Md4);
            QString hashResult(result.toHex());
            ui_tools->lineEdit_md4->setText(hashResult);
        }
        if (ui_tools->checkBox_md5->isChecked()) {
            QByteArray result = hash->hash(ui_tools->lineEdit_src->text().toUtf8() ,QCryptographicHash::Md5);
            QString hashResult(result.toHex());
            ui_tools->lineEdit_md5->setText(hashResult);
        }
        if (ui_tools->checkBox_sha1->isChecked()) {
            QByteArray result = hash->hash(ui_tools->lineEdit_src->text().toUtf8() ,QCryptographicHash::Sha1);
            QString hashResult(result.toHex());
            ui_tools->lineEdit_sha1->setText(hashResult);
        }
        if (ui_tools->checkBox_sha256->isChecked()) {
            QByteArray result = hash->hash(ui_tools->lineEdit_src->text().toUtf8() ,QCryptographicHash::Sha256);
            QString hashResult(result.toHex());
            ui_tools->lineEdit_sha256->setText(hashResult);
        }
        if (ui_tools->checkBox_sha512->isChecked()) {
            QByteArray result = hash->hash(ui_tools->lineEdit_src->text().toUtf8() ,QCryptographicHash::Sha512);
            QString hashResult(result.toHex());
            ui_tools->lineEdit_sha512->setText(hashResult);
        }
    }
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
    hashUppercase = ui_option->checkBox_hashUppercase->isChecked();

    QSettings *GlobalSettings = new QSettings(configFile,QSettings::NativeFormat);
    GlobalSettings->beginGroup("hash");
    GlobalSettings->setValue(CFG_KEY_HashAlg, hashAlg);
    GlobalSettings->setValue(CFG_KEY_HashUppercase, hashUppercase?1:0);
    GlobalSettings->endGroup();
    delete GlobalSettings;
}
//load options settings from cconfig file
void MainWindow::slot_loadOptions()
{
    QSettings *GlobalSettings = new QSettings(configFile,QSettings::NativeFormat);
    GlobalSettings->beginGroup("hash");
    hashAlg =  GlobalSettings->value(CFG_KEY_HashAlg,QCryptographicHash::Md5).toInt();
    hashUppercase =  GlobalSettings->value(CFG_KEY_HashUppercase,false).toBool();
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
            out << topLevelitem->text(COL_CHECKSUM) << "  " << topLevelitem->text(COL_NAME) << Qt::endl;
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
    //DefaultSuffix
    fileDialog->setDefaultSuffix(getHashAlgString(hashAlg,true));

    QString filename = fileDialog->getSaveFileName( this, "Save file",currentDIR.path(), SUPPORT_Filter , &DEFAULT_Filter);
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
// copy checksum value in lower case
void MainWindow::slot_copyLowChecksum()
{
    QTreeWidget *tree = ui->treeWidget_files;
    clipboard->setText(tree->currentItem()->text(tree->currentColumn()).toLower());
}

//add file
void MainWindow::slot_add()
{
//    QString selfilter =getHashAlgString(hashAlg,true);
    //QStringList fileNames= QFileDialog::getOpenFileNames( this, "Add file", currentDIR.path(), ALL_FILE_Filter, &selfilter);
    QStringList fileNames= QFileDialog::getOpenFileNames( this, "Add file", currentDIR.path(), ALL_FILE_Filter);
    if (fileNames.count() >0 ) {
        for (int i = 0; i < fileNames.size(); ++i) {
             QString filename= fileNames.at(i).toUtf8().constData() ;
             //change file path to relative path
             filename = currentDIR.relativeFilePath(filename);
             addTopLevelItem(filename);
        }
    }
}
//load checksum file
void MainWindow::slot_load()
{
    QString selfilter =getHashAlgString(hashAlg,true);
    QString filename= QFileDialog::getOpenFileName(this, "Select file", currentDIR.path(), SUPPORT_Filter, &selfilter);
    if (filename != "") {
        setCurrentPath(currentDIR.filePath(filename));
        clearTopLevelItem();
        parserChechsumFile(filename);
    }
}

void MainWindow::p_slot_prepareRightClickMenu( const QPoint & pos )
{
    QTreeWidget *tree = ui->treeWidget_files;
    //load
    QAction *loadAct = new QAction(QIcon(":/pixmaps/load.png"), tr("&Load"), this);
    connect(loadAct, SIGNAL(triggered()), this, SLOT(slot_load()));
    //save
    QAction *saveAct = new QAction(QIcon(":/pixmaps/save.png"), tr("&Save"), this);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(slot_save()));
    //add file
    QAction *addAct = new QAction(QIcon(":/pixmaps/add.png"), tr("&Add"), this);
    connect(addAct, SIGNAL(triggered()), this, SLOT(slot_add()));

    //menu
    QMenu menu(this);
    //add menu for copy checksum to clipboard
    int curCol = tree->currentColumn();
    if (curCol == COL_CHECKSUM) {
        QAction *copyAct = new QAction(QIcon(":/pixmaps/copy.png"), tr("&Copy"), this);
        connect(copyAct, SIGNAL(triggered()), this, SLOT(slot_copyChecksum()));
        QAction *copylAct = new QAction(QIcon(":/pixmaps/copy.png"), tr("&Copy Lower Case"), this);
        connect(copylAct, SIGNAL(triggered()), this, SLOT(slot_copyLowChecksum()));

        menu.addAction(copyAct);
        menu.addAction(copylAct);
        // add sepelate bar
        menu.addSeparator();
    }
    menu.addAction(addAct);
    menu.addSeparator();
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
            hasherT->setMode(hashAlg);
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
void MainWindow::setCurrentPath(QString sPath)
{
    //currentPath = sPath;
    currentDIR.setPath(sPath);
}

//return : hash mode string : md5 ...
QString MainWindow::getHashAlgString(int hAlg, bool bFilter)
{
    QString hStr;
    switch (hAlg) {
        case QCryptographicHash::Md4:
           hStr="md4";
            break;
        case QCryptographicHash::Md5:
           hStr="md5";
            break;
        case QCryptographicHash::Sha1:
            hStr="sha1";
            break;
        case QCryptographicHash::Sha256:
            hStr="sha256";
            break;
        case QCryptographicHash::Sha512:
            hStr="sha512";
            break;
        default:
            hStr="md5";
            break;
    }
    if (bFilter) {
        hStr=  hStr + " files (*." + hStr+")";
    }
    return hStr;
}

//TODO: md4, sha1, sha256, sha512 checksum file format?
int MainWindow::parserMD5File(QString sFileName)
{
      int idx =0;
    //read file line by line
    QFile f(sFileName);
    //check file can be read
    if (f.open( QIODevice::ReadOnly )) {
        QTextStream in(&f);
        while (!in.atEnd()) {
            QString line = in.readLine();
            bool binMode = line.contains("*");
            if (binMode) {
              //TODO: binary mode
                QStringList fields = line.split('*');
                if (fields.size() == 2) {
                    //qDebug() << "0:" << fields.at(0); //checksum
                    //qDebug() << "1:" << fields.at(1);//filename
                    slot_setChecksum(addTopLevelItem( fields.at(1).trimmed()), fields.at(0).trimmed());
                } else {
                    qDebug() << "unknown md5 file format (binary mode)";
                }
            } else {
                QStringList fields = line.split(' ');
                //qDebug() << "fields" << fields;
                if (fields.size() == 3) {
                   // qDebug() << "0:" << fields.at(0); //checksum
                   // qDebug() << "1:" << fields.at(1);
                   // qDebug() << "2:" << fields.at(2);//filename

                    //idx =  addTopLevelItem( fields.at(2));
                    //slot_setChecksum(idx, fields.at(0));
                    slot_setChecksum(addTopLevelItem( fields.at(2).trimmed()), fields.at(0).trimmed());
                    /*
                    int row = fields.takeFirst().toInt();
                    int column = fields.takeFirst().toInt();
                    setFormula(row, column, fields.join(' '));
                    */
                } else {
                    qDebug() << "unknown md5 file format";
                }
            }
            QApplication::processEvents();
            idx++;
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

void MainWindow::slot_setError(QTreeWidgetItem *itm, QString sMsg)
{
    int idx = ui->treeWidget_files->indexOfTopLevelItem(itm);
    qDebug() << "TODO: error message: "<<QString::number(idx) << ": " <<sMsg;
    //topLevelitem->setText(COL_STATUS,sMsg);

}

void MainWindow::slot_setMsg(QTreeWidgetItem *itm, QString sMsg)
{
    int idx = ui->treeWidget_files->indexOfTopLevelItem(itm);
    qDebug() << "message: "<<QString::number(idx) << ": " <<sMsg;
    //topLevelitem->setText(COL_STATUS,sMsg);

}

void MainWindow::slot_setStatus(QTreeWidgetItem *itm, QString sMsg)
{
    QTreeWidgetItem * topLevelitem = NULL;
   topLevelitem = itm;
    if (topLevelitem !=NULL) {
        topLevelitem->setText(COL_STATUS,sMsg);
    }
}

void MainWindow::slot_setProgress(QTreeWidgetItem *itm, qint64 iPos)
{
    slot_setStatus(itm, QString::number(iPos));
}

void MainWindow::slot_setChecksum(QTreeWidgetItem *itm, QString chksum)
{
    int cMode;
   QTreeWidgetItem * topLevelitem = NULL;
   topLevelitem = itm;
    if (topLevelitem != NULL) {
       cMode = topLevelitem->data(COL_CHECKSUM,MyCheckModeRole).toInt();
       if (cMode==1) {
           //TODO: checkmode?
        //topLevelitem->setTextColor(COL_CHECKSUM,Qt::gray);
	topLevelitem->setForeground(COL_CHECKSUM,Qt::gray);
        qDebug() << "TODO: compare hash";
       } else {
         //calc mode?
        topLevelitem->setForeground(COL_CHECKSUM,Qt::black);
       }
           //TODO: update status
       if (hashUppercase) {
        topLevelitem->setText(COL_CHECKSUM,chksum.toUpper());
       } else {
        topLevelitem->setText(COL_CHECKSUM,chksum.toLower());
       }
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    //QTreeWidget::keyPressEvent(event);
    /*
    if (event->key() == Qt::Key_Escape)
    {
        event->accept();
        emit KeyPressed(); //send signal to QTreeWidgetItem
    }
    */

    if ( event->key() == Qt::Key_Delete )
     {
      QList<QTreeWidgetItem *> selectedItems =  ui->treeWidget_files->selectedItems();

      for (int i = 0; i < selectedItems.size(); ++i)
      {
       QTreeWidgetItem  *item = selectedItems.at(i);
       int nDelIndex = ui->treeWidget_files->indexOfTopLevelItem (item);
       QTreeWidgetItem *itemTop = ui->treeWidget_files->takeTopLevelItem(nDelIndex);
       QVariant v = itemTop->data(COL_STATUS, MyHashThreadRole);
       HasherThread * hasherT = v.value<HasherThread*>();
       if (hasherT->isRunning()) {
          hasherT->slot_setStop(true);
       }
       disconnect( hasherT, 0,0,0);
       disconnect( this, SIGNAL(setThreadStop(bool)), hasherT, SLOT(slot_setStop(bool)) );
        hasherT->deleteLater();
       delete itemTop;
      }
     }

}
//QTreeWidgetItem slot
void MainWindow::slot_pressed()
{
    qDebug() <<"//TODO:process your treewidgetitem when key press";
}

