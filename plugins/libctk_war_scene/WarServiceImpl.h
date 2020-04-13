#pragma once

#include <QObject>
#include <ctk_service/war_scene/WarService.h>

class WarServiceImpl : public QObject, public WarService
{
	Q_OBJECT
	Q_INTERFACES(WarService)
public:
	WarServiceImpl();
	~WarServiceImpl();
	bool openScene(int sceneId) Q_DECL_OVERRIDE;
	void saveScene() Q_DECL_OVERRIDE;
};
