#include "ctrlentertray.h"
#include <QtGui>

#include "RecordNum.h"
#include "PredefinedTextManager.h"

RecordNum* gNum = NULL;
RecordNum* gTotalNum = NULL;
QString gWinFilterName;

void SendAscii(ushort str)
{
	KEYBDINPUT kb={0};
	INPUT Input={0};

	// down
	kb.wScan = str/*enter unicode here*/;
	kb.dwFlags = KEYEVENTF_UNICODE; // KEYEVENTF_UNICODE=4
	Input.type = INPUT_KEYBOARD;
	Input.ki = kb;

	::SendInput(1,&Input,sizeof(Input));

	// up
	kb.wScan = str/*enter unicode here*/;
	kb.dwFlags = KEYEVENTF_UNICODE|KEYEVENTF_KEYUP; //KEYEVENTF_UNICODE=4
	Input.type = INPUT_KEYBOARD;
	Input.ki = kb;

	::SendInput(1,&Input,sizeof(Input));

}

LRESULT CALLBACK keyProc(int nCode,WPARAM wParam,LPARAM lParam )  
{  
	KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *) lParam; 
	
	switch(nCode)
	{
	case HC_ACTION:
		{
			bool bControlKeyDown = GetAsyncKeyState (VK_CONTROL) >> ((sizeof(SHORT) * 8) - 1);
			bool bEnterPressed = GetAsyncKeyState(VK_RETURN) & 1;
			if (wParam == WM_KEYDOWN && bControlKeyDown)
			{
				if(pkbhs->vkCode == VK_DOWN)
				{
					if (!PredefinedTextManager::Instance().isshow())
					{
						PredefinedTextManager::Instance().show();
					}
					else
					{
						PredefinedTextManager::Instance().slideDown();
					}					
				}
				else if(pkbhs->vkCode == VK_UP)
				{
					if (!PredefinedTextManager::Instance().isshow())
					{
						PredefinedTextManager::Instance().show();
					}
					else
					{
						PredefinedTextManager::Instance().slideUp();
					}					
				}
				else if(pkbhs->vkCode == VK_LEFT)
				{
					if (!PredefinedTextManager::Instance().isshow())
					{
						PredefinedTextManager::Instance().show();
					}
					else
					{
						PredefinedTextManager::Instance().slideLeft();
					}
				}
				else if(pkbhs->vkCode == VK_RIGHT)
				{
					if (!PredefinedTextManager::Instance().isshow())
					{
						PredefinedTextManager::Instance().show();
					}
					else
					{
						PredefinedTextManager::Instance().slideRight();
					}
				}
				else if (pkbhs->vkCode == VK_RETURN)
				{
					if(gNum && !bEnterPressed)
					{
						TCHAR buf[255];
						GetWindowText(GetForegroundWindow(), buf, 255);
						QString wintitle = QString::fromUtf16(buf);
						if (gWinFilterName.isEmpty() || wintitle.contains(gWinFilterName))
						{
							gNum->increment();
						}
					}
				}
			}
			else if (wParam == WM_KEYUP)
			{
				if(pkbhs->vkCode == VK_RCONTROL || pkbhs->vkCode == VK_LCONTROL)
				{
					if (PredefinedTextManager::Instance().isshow())
					{
						QString str = PredefinedTextManager::Instance().text();
						foreach(QChar c, str)
						{
							SendAscii(c.unicode());
						}

						PredefinedTextManager::Instance().hide();
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
	ui.decbtn->setFixedWidth(30);
	ui.incbtn->setFixedWidth(30);

	gNum = new RecordNum;
	gTotalNum = new RecordNum;

	icons["on"] = QIcon(":/CtrlEnterTray/Resources/on.png");
	icons["off"] = QIcon(":/CtrlEnterTray/Resources/off.png");

	createActions();
	createTrayIcon();

	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
		this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
	connect(ui.clrBtn, SIGNAL(clicked()), this, SLOT(onClearCount()));
	connect(ui.decbtn, SIGNAL(clicked()), gNum, SLOT(decrement()));
	connect(ui.incbtn, SIGNAL(clicked()), gNum, SLOT(increment()));
	connect(ui.processbtn, SIGNAL(clicked()), gNum, SLOT(toggle()));

	connect(gNum, SIGNAL(valueChanged(int)), ui.labelNum, SLOT(setNum(int)));
	connect(gNum, SIGNAL(valueChanged(int)), this,  SLOT(onValueChanged(int)));
	connect(gNum, SIGNAL(statusChanged(bool)), this, SLOT(onStatusChanged(bool)));

	connect(gTotalNum, SIGNAL(valueChanged(int)), this, SLOT(onTotalValueChanged(int)));

	QSettings settings("Tsingsan-Studio", "CtrlEnterTray");
	gTotalNum->setValue(settings.value("TotalCount", 0).toInt());

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

void CtrlEnterTray::onClearCount()
{
	int ret = QMessageBox::warning(this, tr("注意"),
		tr("将清除所有的回复记录！"),
		QMessageBox::Yes | QMessageBox::No,
		QMessageBox::No);

	if (ret == QMessageBox::Yes)
	{
		gNum->clear();
		gTotalNum->clear();
	}	
}

void CtrlEnterTray::onValueChanged(int num)
{
	static int oldnum = 0;

	int targetnum =  ui.spinBox->value();
	trayIcon->setToolTip(tr("回复数:%1").arg(num) + (targetnum ? tr("/%1").arg(targetnum) : ""));
	if (targetnum && num >= targetnum)
	{
		onShowMessage(tr("任务完成!!!"), tr("已经回复%1帖子").arg(num));
	}
	else if(num > oldnum)
	{
		onShowMessage(tr("回复+1"), tr("已经回复%1帖子").arg(num));
	}

	gTotalNum->setValue(gTotalNum->value() + num - oldnum);
	oldnum = num;
}

void CtrlEnterTray::onTotalValueChanged(int num)
{
	QSettings settings("Tsingsan-Studio", "CtrlEnterTray");
	settings.setValue("TotalCount", num);

	ui.labelTotalNum->setNum(num);
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