#include "PredefinedTextWin.h"
#include "qtwin.h"

PredefinedTextWin::PredefinedTextWin(const QString& str)
:QLabel(str, NULL, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::CustomizeWindowHint)
{
	resize(PTWIN_WIDTH, PTWIN_HEIGHT);

	QFont font;
	font.setPixelSize(18);
	setFont(font);
	setIndent(15);

	setFocusPolicy(Qt::NoFocus);
	if (QtWin::isCompositionEnabled()) {
		QtWin::extendFrameIntoClientArea(this);
		setContentsMargins(0, 0, 0, 0);
	}
}

PredefinedTextWin::~PredefinedTextWin(void)
{
}
