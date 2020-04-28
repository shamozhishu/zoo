#pragma once

#include <QtPlugin>

class WarService
{
public:
	virtual ~WarService() {}
	virtual bool openScene(int id) = 0;
	virtual void closeScene() = 0;
	virtual void saveScene() = 0;
};

#define WarService_iid "ckt.war.service"
Q_DECLARE_INTERFACE(WarService, WarService_iid)
