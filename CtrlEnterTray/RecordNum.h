#pragma once
#include <QObject>

class RecordNum :
	public QObject
{
	Q_OBJECT
public:
	RecordNum(void);
	~RecordNum(void);

	int value(){return _num;}
	bool status(){return _status;}

public slots:
	void setValue(int);
	void increment();
	void decrement();
	void clear();

	void pause();
	void start();
	void toggle();

signals:
	void valueChanged(int);
	void statusChanged(bool);

private:
	int _num;
	bool _status;
};
