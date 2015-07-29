#ifndef HASHERTHREAD_H
#define HASHERTHREAD_H

#include <QThread>
#include <QCryptographicHash>
#include <QString>

class HasherThread : public QThread
{
    Q_OBJECT
public:
    explicit HasherThread(QObject *parent = 0, const QString &filename="", QCryptographicHash::Algorithm hMode = QCryptographicHash::Md5);
    void run();
    void setIdx(int idx); //treeitem index

signals:
    void error(int idx, QString msg);
    void completed(int idx, QString msg);
    void fileSize(int idx, qint64 iSize);
    void fileReadPos(int idx, qint64 iPos);

public slots:

private:
   QCryptographicHash *hasher;
   QString fullFileName;
   QCryptographicHash::Algorithm hashMode;
   int treeitemIdx;

};

#endif // HASHERTHREAD_H
