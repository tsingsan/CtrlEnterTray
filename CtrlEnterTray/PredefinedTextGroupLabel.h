#pragma once
#include <QLabel>

#define PTLABEL_HEIGHT 30

class PredefinedTextGroupLabel :
	public QLabel
{
	Q_OBJECT
public:
	PredefinedTextGroupLabel(const QString& str);
	~PredefinedTextGroupLabel(void);
};
