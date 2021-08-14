#include <QApplication>

#include <iostream>

#include "window.hh"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	Window window;
	window.show();

	return app.exec();
}
