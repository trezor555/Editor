#include "draglistwidget.h"

draglistwidget::draglistwidget(QWidget *parent)
: QListWidget(parent)
{
	setDragEnabled(true);
	setDragDropMode(QAbstractItemView::DragOnly);

}

draglistwidget::~draglistwidget()
{

}

void draglistwidget::mousePressEvent(QMouseEvent *ev)
{
	QListWidget::mousePressEvent(ev);
	QListWidgetItem *item = currentItem();
	if (ev->button() == Qt::LeftButton)
	{
		if (item)
		{
			QDrag *drag = new QDrag(this);
			QMimeData *mimeData = new QMimeData;

			mimeData->setText(item->data(Qt::UserRole).toString());
			drag->setMimeData(mimeData);
			//drag->setPixmap(currentItem()->icon().pixmap(QSize(50, 50)));
			//drag->setHotSpot(QPoint(25, 25));
			drag->exec();

		}
	}
}