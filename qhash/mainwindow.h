#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QByteArray>
#include <QCryptographicHash>
#include <QStringList>
#include <QSettings>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(int &argc, char **argv, QWidget *parent = 0);
    ~MainWindow();
    bool startHash();
    void setConfigFile(QString sFileName);

private:
    Ui::MainWindow *ui;
    QByteArray fileChecksum(const QString &fileName,
                            QCryptographicHash::Algorithm hashAlgorithm);
    QString configFile; //TODO
    int  parserChechsumFile(QString sFileName);

};

#endif // MAINWINDOW_H
