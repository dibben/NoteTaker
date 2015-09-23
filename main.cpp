

#include "MainWindow.h"

#include <QApplication>


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setOrganizationName("NoteTaker");
	QCoreApplication::setApplicationName("Note Taker");

	MainWindow w;
	w.show();

	return a.exec();
}
