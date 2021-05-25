#include "progressbardelegate.h"

//#include <QStyledItemDelegate>

#include <QDebug>

ProgressBarDelegate::ProgressBarDelegate(QTreeWidget *parent) :
    QItemDelegate(parent)
{
}

void ProgressBarDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QItemDelegate::paint(painter, option, index);

    int progress = index.data(Qt::DisplayRole).toInt();
    painter->save();
    if (progress >0) {
        QStyleOptionProgressBar progressBarStyleOption;
        //TODO: look not good on selected!!
        /*
        QRect rect(option.rect.x()+5, option.rect.y()+5, option.rect.width()-10,
                       option.rect.height()-10);
        progressBarStyleOption.rect = rect;
        */
        progressBarStyleOption.rect = option.rect;
        progressBarStyleOption.palette = option.palette;

        progressBarStyleOption.state = option.state;
        progressBarStyleOption.fontMetrics = option.fontMetrics;

        // QColor brown(212, 140, 95);
        //QPalette pal(brown);

        progressBarStyleOption.minimum =  index.data(MyMinimumRole).toInt();;
        progressBarStyleOption.maximum =  index.data(MyMaximumRole).toInt();
        progressBarStyleOption.textAlignment = Qt::AlignCenter;
        progressBarStyleOption.progress = progress ;
        //show in %
        progressBarStyleOption.text = QString( "%1%" ).arg( progress);
        progressBarStyleOption.textVisible = true;

        QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarStyleOption, painter );

    } else {
        qDebug() << "paint column:" << QString::number( index.column());
        //TODO this will cause the rect not show as selected?
        //TODO: how to just paint text as original do?
        //QApplication::style()->drawControl(QStyle::CE_ItemViewItem, &option, painter );
    }
    painter->restore();
}
/*
void ProgressBarDelegate::paintEvent(QPaintEvent *)
{
 QStyleOption opt;
 opt.init(this);
 QPainter p(this);
 style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}
*/
