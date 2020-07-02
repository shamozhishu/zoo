#ifndef __WAR_SERVICE_H__
#define __WAR_SERVICE_H__

#include <QtPlugin>

class WarService
{
public:
	virtual ~WarService() {}
	virtual bool openScene(int id) = 0;
	virtual void closeScene() = 0;
	virtual void saveScene() = 0;
	virtual void startSimulation() = 0;
	virtual void stopSimulation() = 0;
};

#define WarService_iid "ckt.war.service"
Q_DECLARE_INTERFACE(WarService, WarService_iid)

#endif // __WAR_SERVICE_H__
