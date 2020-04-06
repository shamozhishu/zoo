#pragma once

#include <QtPlugin>

class WarService
{
public:
	virtual ~WarService() {}
	virtual bool enterScene(int sceneId) = 0;
	virtual void exitScene(int sceneId) = 0;
};

#define WarService_iid "ckt.service.WarService"
Q_DECLARE_INTERFACE(WarService, WarService_iid)
