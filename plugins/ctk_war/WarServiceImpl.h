#ifndef __WAR_SERVICE_IMPL_H__
#define __WAR_SERVICE_IMPL_H__

#include <QObject>
#include "Battlefield.h"
#include <ctk_service/war/WarService.h>

class WarServiceImpl : public QObject, public WarService
{
	Q_OBJECT
	Q_INTERFACES(WarService)
public:
	WarServiceImpl();
	bool openScene(int id);
	void closeScene();
	void saveScene();
	bool startSimulation();
	bool pauseSimulation();
	bool stopSimulation();

private:
	ESimState _simuState;
};

#endif // __WAR_SERVICE_IMPL_H__
