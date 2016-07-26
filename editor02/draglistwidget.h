#ifndef DRAGLISTWIDGET_H
#define DRAGLISTWIDGET_H

#include <QListWidget>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

class draglistwidget : public QListWidget
{
	Q_OBJECT

public:
	draglistwidget(QWidget *parent);
	~draglistwidget();

protected:
	void mousePressEvent(QMouseEvent *ev);
};

#endif // DRAGLISTWIDGET_H
