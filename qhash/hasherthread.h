#ifndef HASHERTHREAD_H
#define HASHERTHREAD_H

#include <QThread>
#include <QCryptographicHash>


class HasherThread : public QThread
{
    Q_OBJECT
public:
    explicit HasherThread(QObject *parent = 0, const QString &filename, QCryptographicHash::Algorithm hMode);

signals:
    error(QString);
    completed(QString);
    fileSize(qint64);
    fileReadPos(qint64);

public slots:

private:
   QCryptographicHash hasher;
   QString fullFileName;
   QCryptographicHash::Algorithm hashMode;
};

#endif // HASHERTHREAD_H
