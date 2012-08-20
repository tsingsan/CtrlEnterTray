#include "ctrlentertray.h"
#include <QtGui>

#include "RecordNum.h"

RecordNum* gNum = NULL;

LRESULT CALLBACK keyProc(int nCode,WPARAM wParam,LPARAM lParam )  
{  
	KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *) lParam; 
	
	switch(nCode)
	{
	case HC_ACTION:
		{
			//static bool bEnterPressed = false;
			bool bControlKeyDown = GetAsyncKeyState (VK_CONTROL) >> ((sizeof(SHORT) * 8) - 1);
			bool bEnterPressed = GetAsyncKeyState(VK_RETURN) & 1;
			if(wParam == WM_KEYDOWN && pkbhs->vkCode == VK_RETURN && bControlKeyDown)  
			{
				if(gNum && !bEnterPressed)
				{
					TCHAR buf[255];
					GetWindowText(GetForegroundWindow(), buf, 255);
					QString wintitle = QString::fromUtf16(buf);
					if (wintitle.contains(QObject::tr("沪江")))
					{
						gNum->increment();
					}
				}
			}
		}
		break;
	default:
		break;
	}	
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}  

CtrlEnterTray::CtrlEnterTray(QWidget *parent, Qt::WFlags flags)
	: QDialog(parent, flags)
{
	ui.setupUi(this);
	gNum = new RecordNum;

	icons["on"] = QIcon(":/CtrlEnterTray/Resources/on.png");
	icons["off"] = QIcon(":/CtrlEnterTray/Resources/off.png");

	createActions();
	createTrayIcon();

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
	connect(ui.clrBtn, SIGNAL(clicked()), gNum, SLOT(clear()));
	connect(ui.processbtn, SIGNAL(clicked()), gNum, SLOT(toggle()));

	connect(gNum, SIGNAL(valueChanged(int)), ui.labelNum,  SLOT(setNum(int)));
	connect(gNum, SIGNAL(valueChanged(int)), this,  SLOT(onValueChanged(int)));
	connect(gNum, SIGNAL(statusChanged(bool)), this, SLOT(onStatusChanged(bool)));

	trayIcon->show();

	keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyProc, GetModuleHandle(NULL), 0);

	gNum->clear();
	gNum->start();
}

CtrlEnterTray::~CtrlEnterTray()
{
	UnhookWindowsHookEx(keyHook); 
	delete gNum;
	gNum = NULL;
}

void CtrlEnterTray::closeEvent(QCloseEvent *event)
{
	if (trayIcon->isVisible()) {
		hide();
		event->ignore();
	}
}

void CtrlEnterTray::createActions()
{
	quitAction = new QAction(tr("退出"), this);
	connect(quitAction, SIGNAL(triggered()), qApp, SLOT(quit()));

	processAction = new QAction(this);
	connect(processAction, SIGNAL(triggered()), gNum, SLOT(toggle()));
}

void CtrlEnterTray::createTrayIcon()
{
	trayIconMenu = new QMenu(this);
	
	trayIconMenu->addAction(processAction);
	trayIconMenu->addSeparator();
	trayIconMenu->addAction(quitAction);

	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setContextMenu(trayIconMenu);
}

void CtrlEnterTray::onSetIcon(const char* name)
{
	QIcon icon = icons[name];
	trayIcon->setIcon(icon);
	setWindowIcon(icon);
}

void CtrlEnterTray::onShowMessage(QString title, QString message)
{
	trayIcon->showMessage(title, message);
}

void CtrlEnterTray::iconActivated(QSystemTrayIcon::ActivationReason reason)
{
	if (reason == QSystemTrayIcon::DoubleClick)
	{
		show();
	}
}

void CtrlEnterTray::onValueChanged(int num)
{
	int targetnum =  ui.spinBox->value();
	trayIcon->setToolTip(tr("回复数:%1").arg(num) + (targetnum ? tr("/%1").arg(targetnum) : ""));
	if (targetnum && num >= targetnum)
	{
		onShowMessage(tr("任务完成!!!"), tr("已经回复%1帖子").arg(num));
	}
	else if(num)
	{
		onShowMessage(tr("回复+1"), tr("已经回复%1帖子").arg(num));
	}
}

void CtrlEnterTray::onStatusChanged(bool status)
{
	if (status)
	{
		processAction->setText(tr("暂停计数"));
		ui.processbtn->setText(tr("暂停计数"));
		onSetIcon("on");
	}
	else
	{
		processAction->setText(tr("继续计数"));
		ui.processbtn->setText(tr("继续计数"));
		onSetIcon("off");
	}
}