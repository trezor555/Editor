#ifndef CREATENEWLEVEL_H
#define CREATENEWLEVEL_H

#include <QDialog>
#include "ui_createnewlevel.h"

class createnewlevel : public QDialog
{
	Q_OBJECT

public:
	virtual void accept();
	createnewlevel(QWidget *parent = 0);
	~createnewlevel();

	Ui::createnewlevel ui;
};

#endif // CREATENEWLEVEL_H
