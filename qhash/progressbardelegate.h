#ifndef PROGRESSBARDELEGATE_H
#define PROGRESSBARDELEGATE_H

#include <QItemDelegate>
#include <QTreeWidget>
#include <QProgressBar>
#include <QApplication>

#include "customRoles.h"

class ProgressBarDelegate : public QItemDelegate
{
    Q_OBJECT
public:
    explicit ProgressBarDelegate(QTreeWidget *parent = 0);
    void paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const;

signals:

public slots:

};

#endif // PROGRESSBARDELEGATE_H
