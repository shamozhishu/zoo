#ifndef __WIN32_SERVICE_H__
#define __WIN32_SERVICE_H__

#include <QObject>

enum Win32WindowID
{
	ZOOCMDWGT = 0,
	MATERIALDISPLAYWGT
};

class Win32Service
{
public:
	virtual HWND getWnd() = 0;
	virtual bool isFocus() = 0;
};

#define Win32Service_iid "com.Interface.Win32Service"
Q_DECLARE_INTERFACE(Win32Service, Win32Service_iid)

#endif // __WIN32_SERVICE_H__
