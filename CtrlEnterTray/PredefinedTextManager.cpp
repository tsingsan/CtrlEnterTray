#include "PredefinedTextManager.h"
#include "PredefinedTextWin.h"
#include "PredefinedTextGroupLabel.h"

#include <QPoint>
#include <QFile>
#include <QTextStream>
#include <QDesktopWidget>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QApplication>

const int shownum = 4;

const int duration = 200;
const int margin = 20;

#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MAX(a,b) ((a) > (b) ? (a) : (b))

PredefinedTextManager::PredefinedTextManager(void)
:bShow(false)
{
}

PredefinedTextManager::~PredefinedTextManager(void)
{
}

static QPoint posHint(int index)
{
	static int currentScreenWidth = QApplication::desktop()->width();
	static int currentScreenHeight = QApplication::desktop()->height();

	static QPoint initpos(currentScreenWidth - PTWIN_WIDTH - margin, currentScreenHeight - PTWIN_HEIGHT - margin * 3 - PTLABEL_HEIGHT);
	return QPoint(initpos.x(), initpos.y() - index * (PTWIN_HEIGHT + margin));
}

static double alphaHint(int index)
{
	if (index < 0)
	{
		return 0.0;
	}

	return MAX(1.0 - index*1.0f/shownum, 0.0);
}

QString PredefinedTextManager::text() const
{
	WinList& winlist = winlistmapit->second;
	if (winlist.size())
	{
		return winlist.front()->text();
	}
	return "";
}

void PredefinedTextManager::initTextWin()
{
	winlistmap.clear();

	QFile file("phrases.ini");
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		return;

	//QString groupname = "";
	PredefinedTextGroupLabel* grouplabel = new PredefinedTextGroupLabel("");
	QTextStream in(&file);
	while (!in.atEnd()) {
		QString line = in.readLine().trimmed();
		if (line.isEmpty())
		{
			continue;
		}
		if (line.at(0) == '#')
		{
			grouplabel = new PredefinedTextGroupLabel(line.remove(0, 1));			
		}
		else
		{
			winlistmap[grouplabel].push_back(new PredefinedTextWin(line));
		}
	}

	winlistmapit = winlistmap.begin();
}

void PredefinedTextManager::hide()
{
	if (!bShow)
	{
		return;
	}
	bShow = false;

	for (WinMap::iterator it = winlistmap.begin(); it != winlistmap.end(); it++)
	{
		WinList& winlist = it->second;
		for (int i = 0; i < winlist.size(); i++)
		{
			winlist[i]->hide();
		}

		PredefinedTextGroupLabel* winlabel = it->first;
		winlabel->hide();
	}
}

void PredefinedTextManager::show()
{
	if (bShow)
	{
		return;
	}
	bShow = !winlistmap.empty();

	for (WinMap::iterator it = winlistmap.begin(); it != winlistmap.end(); it++)
	{
		WinList& winlist = it->second;
		for (int i = 0; i < winlist.size(); i++)
		{
			winlist[i]->setWindowOpacity(it == winlistmapit ? alphaHint(i) : 0);
			winlist[i]->move(posHint(i));
			winlist[i]->show();
		}

		PredefinedTextGroupLabel* winlabel = it->first;
		winlabel->setWindowOpacity(it == winlistmapit ? 1.0 : 0);
		winlabel->move(posHint(-1));
		winlabel->show();
	}
}

void PredefinedTextManager::slideUp()
{
	if (winlistmapit == winlistmap.end())
	{
		return;
	}

	WinList& winlist = winlistmapit->second;
	if (winlist.size() <= shownum)
	{
		for(int i = 0; i < winlist.size() - 1; i++)
		{
			winlist[i]->move(posHint(i + 1));
			winlist[i]->setWindowOpacity(alphaHint(i+1));
		}
		{
			winlist.back()->setWindowOpacity(alphaHint(0));
			winlist.back()->move(posHint(0));
		}
	}
	else
	{
		winlist[shownum]->setWindowOpacity(0);

		QParallelAnimationGroup* animgroup = new QParallelAnimationGroup;
		{
			QPropertyAnimation* posanim = new QPropertyAnimation(winlist.back(), "pos");
			posanim->setDuration(duration);
			posanim->setStartValue(posHint(-1));
			posanim->setEndValue(posHint(0));
			animgroup->addAnimation(posanim);

			QPropertyAnimation* alphaanim = new QPropertyAnimation(winlist.back(), "windowOpacity");
			alphaanim->setDuration(duration);
			alphaanim->setStartValue(0);
			alphaanim->setEndValue(1.0);
			animgroup->addAnimation(alphaanim);
		}

		for (int i = 0; i < shownum; i++)
		{
			QPropertyAnimation* posanim = new QPropertyAnimation(winlist[i], "pos");
			posanim->setDuration(duration);
			posanim->setStartValue(posHint(i));
			posanim->setEndValue(posHint(i + 1));
			animgroup->addAnimation(posanim);

			QPropertyAnimation* alphaanim = new QPropertyAnimation(winlist[i], "windowOpacity");
			alphaanim->setDuration(duration);
			alphaanim->setStartValue(alphaHint(i));
			alphaanim->setEndValue(alphaHint(i + 1));
			animgroup->addAnimation(alphaanim);
		}

		animgroup->start(QAbstractAnimation::DeleteWhenStopped);
	}	

	winlist.push_front(winlist.back());
	winlist.pop_back();
}

void PredefinedTextManager::slideDown()
{
	if (winlistmapit == winlistmap.end())
	{
		return;
	}

	WinList& winlist = winlistmapit->second;
	if (winlist.size() <= shownum)
	{
		for(int i = 1; i < winlist.size(); i++)
		{
			winlist[i]->move(posHint(i - 1));
			winlist[i]->setWindowOpacity(alphaHint(i - 1));
		}
		{
			winlist.front()->setWindowOpacity(alphaHint(winlist.size() - 1));
			winlist.front()->move(posHint(winlist.size() - 1));
		}
	}
	else
	{
		winlist.back()->setWindowOpacity(0);

		QParallelAnimationGroup* animgroup = new QParallelAnimationGroup;

		for (int i = 0; i < shownum + 1; i++)
		{
			QPropertyAnimation* posanim = new QPropertyAnimation(winlist[i], "pos");
			posanim->setDuration(duration);
			posanim->setStartValue(posHint(i));
			posanim->setEndValue(posHint(i - 1));
			animgroup->addAnimation(posanim);

			QPropertyAnimation* alphaanim = new QPropertyAnimation(winlist[i], "windowOpacity");
			alphaanim->setDuration(duration);
			alphaanim->setStartValue(alphaHint(i));
			alphaanim->setEndValue(alphaHint(i - 1));
			animgroup->addAnimation(alphaanim);
		}

		animgroup->start(QAbstractAnimation::DeleteWhenStopped);
	}	

	winlist.push_back(winlist.front());
	winlist.pop_front();
}

void PredefinedTextManager::slideLeft()
{
	if (winlistmap.size() <= 1)
	{
		return;
	}
	QParallelAnimationGroup* animgroup = new QParallelAnimationGroup;

	WinList& winlist = winlistmapit->second;
	for(int i = 0; i < MIN(shownum, winlist.size()) ; i++)
	{
		QPropertyAnimation* posanim = new QPropertyAnimation(winlist[i], "pos");
		posanim->setDuration(duration);
		posanim->setStartValue(posHint(i));
		posanim->setEndValue(posHint(i) - QPoint(PTWIN_WIDTH + margin, 0));
		animgroup->addAnimation(posanim);

		QPropertyAnimation* alphaanim = new QPropertyAnimation(winlist[i], "windowOpacity");
		alphaanim->setDuration(duration);
		alphaanim->setStartValue(alphaHint(i));
		alphaanim->setEndValue(0);
		animgroup->addAnimation(alphaanim);
	}
	{
		PredefinedTextGroupLabel* winlabel = winlistmapit->first;
		QPropertyAnimation* posanim = new QPropertyAnimation(winlabel, "pos");
		posanim->setDuration(duration);
		posanim->setStartValue(posHint(-1));
		posanim->setEndValue(posHint(-1) - QPoint(PTWIN_WIDTH + margin, 0));
		animgroup->addAnimation(posanim);

		QPropertyAnimation* alphaanim = new QPropertyAnimation(winlabel, "windowOpacity");
		alphaanim->setDuration(duration);
		alphaanim->setStartValue(1.0);
		alphaanim->setEndValue(0);
		animgroup->addAnimation(alphaanim);
	}
	
	if (++winlistmapit == winlistmap.end())
	{
		winlistmapit = winlistmap.begin();
	}
	WinList& winlist2 = winlistmapit->second;
	for(int i = 0; i < MIN(shownum, winlist2.size()) ; i++)
	{
		QPropertyAnimation* posanim = new QPropertyAnimation(winlist2[i], "pos");
		posanim->setDuration(duration);
		posanim->setStartValue(posHint(i) + QPoint(PTWIN_WIDTH + margin, 0));
		posanim->setEndValue(posHint(i));
		animgroup->addAnimation(posanim);

		QPropertyAnimation* alphaanim = new QPropertyAnimation(winlist2[i], "windowOpacity");
		alphaanim->setDuration(duration);
		alphaanim->setStartValue(0.0);
		alphaanim->setEndValue(alphaHint(i));
		animgroup->addAnimation(alphaanim);
	}
	{
		PredefinedTextGroupLabel* winlabel = winlistmapit->first;
		QPropertyAnimation* posanim = new QPropertyAnimation(winlabel, "pos");
		posanim->setDuration(duration);
		posanim->setStartValue(posHint(-1) + QPoint(PTWIN_WIDTH + margin, 0));
		posanim->setEndValue(posHint(-1));
		animgroup->addAnimation(posanim);

		QPropertyAnimation* alphaanim = new QPropertyAnimation(winlabel, "windowOpacity");
		alphaanim->setDuration(duration);
		alphaanim->setStartValue(0.0);
		alphaanim->setEndValue(1.0);
		animgroup->addAnimation(alphaanim);
	}

	animgroup->start(QAbstractAnimation::DeleteWhenStopped);
}

void PredefinedTextManager::slideRight()
{
	if (winlistmap.size() <= 1)
	{
		return;
	}
	QParallelAnimationGroup* animgroup = new QParallelAnimationGroup;

	WinList& winlist = winlistmapit->second;
	for(int i = 0; i < MIN(shownum, winlist.size()) ; i++)
	{
		QPropertyAnimation* posanim = new QPropertyAnimation(winlist[i], "pos");
		posanim->setDuration(duration);
		posanim->setStartValue(posHint(i));
		posanim->setEndValue(posHint(i) + QPoint(PTWIN_WIDTH + margin, 0));
		animgroup->addAnimation(posanim);

		QPropertyAnimation* alphaanim = new QPropertyAnimation(winlist[i], "windowOpacity");
		alphaanim->setDuration(duration);
		alphaanim->setStartValue(alphaHint(i));
		alphaanim->setEndValue(0);
		animgroup->addAnimation(alphaanim);
	}
	{
		PredefinedTextGroupLabel* winlabel = winlistmapit->first;
		QPropertyAnimation* posanim = new QPropertyAnimation(winlabel, "pos");
		posanim->setDuration(duration);
		posanim->setStartValue(posHint(-1));
		posanim->setEndValue(posHint(-1) + QPoint(PTWIN_WIDTH + margin, 0));
		animgroup->addAnimation(posanim);

		QPropertyAnimation* alphaanim = new QPropertyAnimation(winlabel, "windowOpacity");
		alphaanim->setDuration(duration);
		alphaanim->setStartValue(1.0);
		alphaanim->setEndValue(0.0);
		animgroup->addAnimation(alphaanim);
	}

	if (winlistmapit-- == winlistmap.begin())
	{
		winlistmapit = --winlistmap.end();
	}
	WinList& winlist2 = winlistmapit->second;
	for(int i = 0; i < MIN(shownum, winlist2.size()) ; i++)
	{
		QPropertyAnimation* posanim = new QPropertyAnimation(winlist2[i], "pos");
		posanim->setDuration(duration);
		posanim->setStartValue(posHint(i) - QPoint(PTWIN_WIDTH + margin, 0));
		posanim->setEndValue(posHint(i));
		animgroup->addAnimation(posanim);

		QPropertyAnimation* alphaanim = new QPropertyAnimation(winlist2[i], "windowOpacity");
		alphaanim->setDuration(duration);
		alphaanim->setStartValue(0.0);
		alphaanim->setEndValue(alphaHint(i));
		animgroup->addAnimation(alphaanim);
	}
	{
		PredefinedTextGroupLabel* winlabel = winlistmapit->first;
		QPropertyAnimation* posanim = new QPropertyAnimation(winlabel, "pos");
		posanim->setDuration(duration);
		posanim->setStartValue(posHint(-1) - QPoint(PTWIN_WIDTH + margin, 0));
		posanim->setEndValue(posHint(-1));
		animgroup->addAnimation(posanim);

		QPropertyAnimation* alphaanim = new QPropertyAnimation(winlabel, "windowOpacity");
		alphaanim->setDuration(duration);
		alphaanim->setStartValue(0.0);
		alphaanim->setEndValue(1.0);
		animgroup->addAnimation(alphaanim);
	}

	animgroup->start(QAbstractAnimation::DeleteWhenStopped);
}