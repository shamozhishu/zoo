#pragma once

#include "WarService.h"
#include <QObject>

class WarServiceImpl : public QObject, public WarService
{
	Q_OBJECT
	Q_INTERFACES(WarService)
public:
	WarServiceImpl();
	~WarServiceImpl();
	bool enterScene(int sceneId) Q_DECL_OVERRIDE;
	void exitScene(int sceneId) Q_DECL_OVERRIDE;
};
