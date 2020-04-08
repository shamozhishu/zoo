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
	bool openScene(int sceneId) Q_DECL_OVERRIDE;
	void closeScene(int sceneId) Q_DECL_OVERRIDE;
};
