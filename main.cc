#include <QApplication>

#include "window.hh"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	QCoreApplication::setOrganizationName("Qmacs");
	QCoreApplication::setApplicationName("Qmacs");

	Window window;
	window.show();

	return app.exec();
}
