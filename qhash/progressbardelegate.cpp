#include "progressbardelegate.h"

#include <QDebug>

ProgressBarDelegate::ProgressBarDelegate(QTreeWidget *parent) :
    QItemDelegate(parent)
{
}

void ProgressBarDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    int progress = index.data(Qt::DisplayRole).toInt();
    if (progress >0) {
        QStyleOptionProgressBar progressBarStyleOption;
        progressBarStyleOption.rect = option.rect;
        progressBarStyleOption.minimum =  index.data(MyMinimumRole).toInt();;
        progressBarStyleOption.maximum =  index.data(MyMaximumRole).toInt();;
        progressBarStyleOption.textAlignment = Qt::AlignCenter;
        progressBarStyleOption.progress = progress ;
        //TODO: change to %
        progressBarStyleOption.text = QString( "%1" ).arg( progress);
        progressBarStyleOption.textVisible = true;

        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarStyleOption, painter );
    } else {
        qDebug() << "paint column:" << QString::number( index.column());
        //TODO this will cause the rect not show as selected

        //TODO: how to just paint text as original do?
        /*
        QStyleOptionViewItem viOpt;
        viOpt.rect =option.rect;
        viOpt.text =  index.data(Qt::DisplayRole).toString();
        QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &viOpt, painter );
        */
        //QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &option, painter );
    }
}
