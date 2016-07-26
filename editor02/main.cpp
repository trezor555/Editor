#include "editor02.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);


	Engine* engine = new Engine();
	editor02 w(engine);
	w.showMaximized();
	a.exec();
	delete engine;
	return 0;
}
