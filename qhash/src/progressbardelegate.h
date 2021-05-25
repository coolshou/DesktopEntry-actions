#ifndef PROGRESSBARDELEGATE_H
#define PROGRESSBARDELEGATE_H

#include <QItemDelegate>
#include <QTreeWidget>
#include <QProgressBar>
#include <QApplication>
#include <QPainter>
#include <QPalette>
#include <QtGui>

#include "customRoles.h"

class ProgressBarDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ProgressBarDelegate(QTreeWidget *parent = 0);
    void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;
    //void paintEvent(QPaintEvent *);

signals:

public slots:

};

#endif // PROGRESSBARDELEGATE_H
