#ifndef CTRLENTERTRAY_H
#define CTRLENTERTRAY_H

#include <QMap>
#include <QtGui/QSystemTrayIcon>
#include <QtGui/QDialog>
#include <Windows.h>
#include "ui_ctrlentertray.h"

class CtrlEnterTray : public QDialog
{
	Q_OBJECT

public:
	CtrlEnterTray(QWidget *parent = 0, Qt::WFlags flags = 0);
	~CtrlEnterTray();

protected:
	void closeEvent(QCloseEvent *event);

private slots:
	void iconActivated(QSystemTrayIcon::ActivationReason reason);
	void onShowMessage(QString title, QString message);
	void onSetIcon(const char* name);

	void onClearCount();

	void onStatusChanged(bool);
	void onValueChanged(int);
	void onTotalValueChanged(int);

private:
	void createActions();
	void createTrayIcon();

private:
	QAction *quitAction;
	QAction *processAction;

	QSystemTrayIcon *trayIcon;
	QMenu *trayIconMenu;
	QMap<QString, QIcon> icons;

	HHOOK keyHook;

	Ui::CtrlEnterTrayClass ui;
};

extern QString gWinFilterName;

#endif // CTRLENTERTRAY_H
