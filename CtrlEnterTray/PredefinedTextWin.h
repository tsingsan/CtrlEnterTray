#pragma once
#include <QLabel>
#include <QString>

#define PTWIN_WIDTH 240
#define PTWIN_HEIGHT 60

class PredefinedTextWin :
	public QLabel
{
	Q_OBJECT
public:
	PredefinedTextWin(const QString& str);
	~PredefinedTextWin(void);
};
