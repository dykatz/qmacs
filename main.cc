#include <QApplication>

#include "window.hh"

int main(int argc, char** argv)
{
	QApplication app(argc, argv);

	QCoreApplication::setOrganizationName("Qmacs");
	QCoreApplication::setApplicationName("Qmacs");

	QFont font;
	font.setFamily("courier");
	font.setStyleHint(QFont::Monospace);
	font.setFixedPitch(true);
	QGuiApplication::setFont(font);

	Window window;
	window.show();

	return app.exec();
}
