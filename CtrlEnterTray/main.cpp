#include "ctrlentertray.h"
#include <QtGui>

int main(int argc, char *argv[])
{
	QTextCodec::setCodecForTr(QTextCodec::codecForName("System"));

	QApplication a(argc, argv);

	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(0, QObject::tr("Systray"),
			QObject::tr("I couldn't detect any system tray "
			"on this system."));
		return 1;
	}

	CtrlEnterTray w;
	return a.exec();
}
