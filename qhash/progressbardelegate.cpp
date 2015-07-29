#include "progressbardelegate.h"

#include <QDebug>

ProgressBarDelegate::ProgressBarDelegate(QTreeWidget *parent) :
    QItemDelegate(parent)
{
}

void ProgressBarDelegate::paint( QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index ) const
{
    QStyleOptionProgressBar progressBarStyleOption;
    progressBarStyleOption.rect = option.rect;

    int progress = index.data(Qt::DisplayRole).toInt();

    progressBarStyleOption.minimum =  index.data(MyMinimumRole).toInt();;
    progressBarStyleOption.maximum =  index.data(MyMaximumRole).toInt();;
    progressBarStyleOption.textAlignment = Qt::AlignCenter;
    progressBarStyleOption.progress = progress ;
    //TODO: change to %
    progressBarStyleOption.text = QString( "%1" ).arg( progress);
    progressBarStyleOption.textVisible = true;

    //qApp->style()->drawControl( QStyle::CE_ProgressBar, &progressBarStyleOption, painter );
    QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarStyleOption, painter );

}
