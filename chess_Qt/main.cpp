#include <QApplication>
#include "mainwindow.h"

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	CMainWindow mw;
	mw.show();

	return a.exec();
}
