#include "ctrlentertray.h"
#include "PredefinedTextManager.h"
#include <QtGui>

const int shownum = 3;

int main(int argc, char *argv[])
{
	QTextCodec::setCodecForTr(QTextCodec::codecForName("System"));
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("System"));

	gWinFilterName = argc > 1 ? argv[1] : "";

	QApplication a(argc, argv);

	if (!QSystemTrayIcon::isSystemTrayAvailable()) {
		QMessageBox::critical(0, QObject::tr("Systray"),
			QObject::tr("I couldn't detect any system tray "
			"on this system."));
		return 1;
	}

	PredefinedTextManager::Instance().initTextWin();

	CtrlEnterTray w(NULL, Qt::WindowStaysOnTopHint);
	return a.exec();
}
