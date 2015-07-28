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

signals:
    void error(QString msg);
    void completed(QString msg);
    void fileSize(qint64);
    void fileReadPos(qint64);

public slots:

private:
   QCryptographicHash *hasher;
   QString fullFileName;
   QCryptographicHash::Algorithm hashMode;
};

#endif // HASHERTHREAD_H
