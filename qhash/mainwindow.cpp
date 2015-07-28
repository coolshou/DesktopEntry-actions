#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //read config file
    //load files to treeview
    //
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::startHash()
{
    //treefile list
    //ui->treeView_files->children().count();

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
