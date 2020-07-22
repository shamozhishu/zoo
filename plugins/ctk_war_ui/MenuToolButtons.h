#ifndef __MENU_TOOL_BUTTONS_H__
#define __MENU_TOOL_BUTTONS_H__

#include <QString>
#include <QObject>

class UIManagerService;
class MenuToolButtons
{
public:
	enum EMenu
	{
		file_,
		edit_
	};

public:
	static void create(UIManagerService* service, EMenu menutype, const QObject* receiver);
	static void destroy(UIManagerService* service, EMenu menutype);
};

#endif // __MENU_TOOL_BUTTONS_H__
