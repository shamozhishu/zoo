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
		scene_,
		simulation_,
		help_,
	};

public:
	static void create(UIManagerService* service, EMenu menutype, QObject* receiver, bool hasToolButton);
	static void destroy(UIManagerService* service, EMenu menutype);
};

#endif // __MENU_TOOL_BUTTONS_H__
