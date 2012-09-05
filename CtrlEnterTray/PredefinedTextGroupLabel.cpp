#include "PredefinedTextGroupLabel.h"
#include "PredefinedTextWin.h"

PredefinedTextGroupLabel::PredefinedTextGroupLabel(const QString& str)
:QLabel(str, NULL, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint)
{
	resize(PTWIN_WIDTH, PTLABEL_HEIGHT);

	QFont font;
	font.setPixelSize(15);
	setFont(font);
	
	setAlignment(Qt::AlignRight);

	setFocusPolicy(Qt::NoFocus);

	setAttribute(Qt::WA_TranslucentBackground, true);
}

PredefinedTextGroupLabel::~PredefinedTextGroupLabel(void)
{
}
