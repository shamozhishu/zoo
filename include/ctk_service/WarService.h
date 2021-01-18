#ifndef __WAR_SERVICE_H__
#define __WAR_SERVICE_H__

#include <QtPlugin>

enum EWindowID
{
	MAIN_VIEW_WGT = 0,
	MATERIAL_DISPLAY_WGT
};

class WarService
{
public:
	virtual ~WarService() {}
	virtual bool openScene(int id) = 0;
	virtual void closeScene() = 0;
	virtual void saveScene() = 0;
	virtual bool startSimulation() = 0;
	virtual bool pauseSimulation() = 0;
	virtual bool stopSimulation() = 0;
};

#define WarService_iid "ckt.war.service"
Q_DECLARE_INTERFACE(WarService, WarService_iid)

#endif // __WAR_SERVICE_H__
