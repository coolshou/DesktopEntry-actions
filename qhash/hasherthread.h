#ifndef HASHERTHREAD_H
#define HASHERTHREAD_H

#include <QThread>
#include <QCryptographicHash>
#include <QString>
#include <QDateTime>
#include <QTreeWidgetItem>

class HasherThread : public QThread
{
    Q_OBJECT
public:
    explicit HasherThread(QObject *parent = 0, const QString &filename="", QCryptographicHash::Algorithm hMode = QCryptographicHash::Md5);
    void run();
    void setItem(QTreeWidgetItem *itm);

signals:
    void error(QTreeWidgetItem *itm, QString msg);
    void completed(QTreeWidgetItem *itm, QString msg);
    void fileSize(QTreeWidgetItem *itm, qint64 iSize);
    void fileReadPos(QTreeWidgetItem *itm, qint64 iPos);

public slots:
    void slot_setStop(bool bStop);

//private slots:
//    void filehashPos(qint64 iPos);

private:
   QCryptographicHash *hasher;
   QString fullFileName;
   QCryptographicHash::Algorithm hashMode;
//   int treeitemIdx;
  QTreeWidgetItem *treeitem;
   bool m_stop ; //flag to tell thread stop
   int pagesize;
};

#endif // HASHERTHREAD_H
