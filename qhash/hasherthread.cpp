#include <QFile>

#include "hasherthread.h"
/*
 * Usage
hasherThread = new HasherThread( this ,  filename, hMode);
    connect( hasherThread, SIGNAL(error(const QString &)),
             ui->resultEdit, SLOT(setText(const QString &)) );
    connect( hasherThread, SIGNAL(completed(const QString &)),
             ui->resultEdit, SLOT(setText(const QString &)) );

hasherThread->start()


QCryptographicHash::Md4	0	Generate an MD4 hash sum
QCryptographicHash::Md5	1	Generate an MD5 hash sum
QCryptographicHash::Sha1	2	Generate an SHA-1 hash sum
QCryptographicHash::Sha224	3	Generate an SHA-224 hash sum (SHA-2). Introduced in Qt 5.0
QCryptographicHash::Sha256	4	Generate an SHA-256 hash sum (SHA-2). Introduced in Qt 5.0
QCryptographicHash::Sha384	5	Generate an SHA-384 hash sum (SHA-2). Introduced in Qt 5.0
QCryptographicHash::Sha512	6	Generate an SHA-512 hash sum (SHA-2). Introduced in Qt 5.0
QCryptographicHash::Sha3_224	7	Generate an SHA3-224 hash sum. Introduced in Qt 5.1
QCryptographicHash::Sha3_256	8	Generate an SHA3-256 hash sum. Introduced in Qt 5.1
QCryptographicHash::Sha3_384	9	Generate an SHA3-384 hash sum. Introduced in Qt 5.1
QCryptographicHash::Sha3_512	10	Generate an SHA3-512 hash sum. Introduced in Qt 5.1

 */
//TODO: buffer size? best value?
#define BEST_BUFFER_SIZE 1024*1024

HasherThread::HasherThread(QObject *parent , const QString &filename, QCryptographicHash::Algorithm hMode) :
    QThread(parent)
{
    hashMode = hMode;
    hasher = new QCryptographicHash( hashMode );
    fullFileName = filename;
}

void HasherThread::setIdx(int idx)
{
    treeitemIdx = idx;
}

void HasherThread::run()
{
    qint64 size;
    qint64 iReadCount=0;
    int iProgress=0;
    m_stop = false;
    QFile f( fullFileName );
    if ( !f.open(QIODevice::ReadOnly) ) {
        emit error(treeitemIdx,  QString("Unable to open file %1").arg(fullFileName) );
        return;
    }
    size = f.size();
    hasher->reset();

    char buffer[BEST_BUFFER_SIZE];
    int count;
    do {
        count = f.read( buffer, sizeof(buffer) );
        if ( count == -1 ) {
            emit error(treeitemIdx,  QString("Read error") );
            break;
        }
        //TODO: read position
        hasher->addData( buffer, count );
        iReadCount = iReadCount + count;
        //signal how many byte read
        iProgress = ((iReadCount *100) / size);
        emit fileReadPos(treeitemIdx, iProgress);
    } while( !f.atEnd()  &&  (!m_stop));
    if (!m_stop) {
        emit completed(treeitemIdx, hasher->result().toHex().toUpper() );
    }
//*/
    //add by filename
    /*
        //connect(&f,SIGNAL(f.),this,filehashPos() );

        QCryptographicHash hash(hashAlgorithm);
        if (hash.addData(&f)) {
            emit completed(treeitemIdx, hasher->result().toHex().toUpper() );
        }
    //*/
    f.close();
}

void HasherThread::setStop(bool bStop)
{
    m_stop = bStop;
}
