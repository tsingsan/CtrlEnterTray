#include "RecordNum.h"

RecordNum::RecordNum(void)
:_num(-1),_status(false)
{
}

RecordNum::~RecordNum(void)
{
}

void RecordNum::increment()
{	
	if (!_status)
	{
		return;
	}

	_num ++;
	emit valueChanged(_num);
}

void RecordNum::clear()
{
	if (_num)
	{
		_num = 0;
		emit valueChanged(_num);
	}
}

void RecordNum::start()
{
	if (!_status)
	{
		_status = true;
		emit statusChanged(_status);
	}
}

void RecordNum::pause()
{
	if (_status)
	{
		_status = false;
		emit statusChanged(_status);
	}
}

void RecordNum::toggle()
{
	_status = !_status;
	emit statusChanged(_status);
}