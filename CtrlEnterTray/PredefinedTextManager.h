#pragma once

#include <map>
#include <deque>
#include <QString>

class PredefinedTextWin;
class PredefinedTextGroupLabel;
class PredefinedTextManager
{
protected:
	PredefinedTextManager(void);
	~PredefinedTextManager(void);

public:
	void initTextWin();
	
	QString text() const;
	bool isshow() const{return bShow;}
	void hide();
	void show();
	void slideUp();
	void slideDown();
	void slideLeft();
	void slideRight();

	static PredefinedTextManager& Instance()
	{
		static PredefinedTextManager instance_;
		return instance_;
	}

private:
	bool bShow;

	typedef std::deque<PredefinedTextWin*> WinList;
	typedef std::map<PredefinedTextGroupLabel*, WinList> WinMap;

	WinMap winlistmap;
	WinMap::iterator winlistmapit;
};
