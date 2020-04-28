#pragma once

#include <QObject>
#include <ctk_service/war/WarService.h>

class WarServiceImpl : public QObject, public WarService
{
	Q_OBJECT
	Q_INTERFACES(WarService)
public:
	WarServiceImpl();
	~WarServiceImpl();
	bool openScene(int id) Q_DECL_OVERRIDE;
	void closeScene() Q_DECL_OVERRIDE;
	void saveScene() Q_DECL_OVERRIDE;
};
