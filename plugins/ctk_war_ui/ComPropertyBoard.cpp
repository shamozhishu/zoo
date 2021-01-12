#include "ComPropertyBoard.h"
#include "UIActivator.h"
#include "ui_ComPropertyBoard.h"
#include <ctk_service/UIManagerService.h>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include <QScrollArea>
#include "PropertyWgts.h"
#include "ArmyListWgt.h"

// Qt5ÖÐÎÄÂÒÂë
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

static const char** s_curSelectComTypeName = nullptr;

enum { dof_ts = 0, behavior_ts, camera_ts, model_ts, sound_ts, animator_ts, collider_ts, environment_ts };
static const char* s_comTypeName_terrain_scene[] = { "DoF", "Behavior", "Camera", "Model", "Sound", "Animator", "Collider", "Environment", nullptr };
enum { dof_te = 0, behavior_te, camera_te, model_te, sound_te, animator_te, collider_te };
static const char* s_comTypeName_terrain_entity[] = { "DoF", "Behavior", "Camera", "Model", "Sound", "Animator", "Collider", nullptr };

enum { dof_es = 0, behavior_es, camera_es, model_es, sound_es, animator_es, collider_es, earth_es };
static const char* s_comTypeName_earth_scene[] = { "DoF", "Behavior", "Camera", "Model", "Sound", "Animator", "Collider", "Earth", nullptr };
enum { dof_ee = 0, behavior_ee, camera_ee, model_ee, sound_ee, animator_ee, collider_ee };
static const char* s_comTypeName_earth_entity[] = { "DoF", "Behavior", "Camera", "Model", "Sound", "Animator", "Collider", nullptr };

enum { };
static const char* s_comTypeName_map_scene[] = { nullptr };
enum { };
static const char* s_comTypeName_map_entity[] = { nullptr };

static QHash<QString, int> s_comsTypeMap_terrain_scene;
static QHash<QString, int> s_comsTypeMap_terrain_entity;

static QHash<QString, int> s_comsTypeMap_earth_scene;
static QHash<QString, int> s_comsTypeMap_earth_entity;

static QHash<QString, int> s_comsTypeMap_map_scene;
static QHash<QString, int> s_comsTypeMap_map_entity;

ComPropertyBoard::ComPropertyBoard()
	: _curSelEnt(nullptr)
	, _ui(new Ui::ComPropertyBoard)
{
	int i = 0;
	while (s_comTypeName_terrain_scene[i])
	{
		s_comsTypeMap_terrain_scene[s_comTypeName_terrain_scene[i]] = i;
		++i;
	}

	i = 0;
	while (s_comTypeName_terrain_entity[i])
	{
		s_comsTypeMap_terrain_entity[s_comTypeName_terrain_entity[i]] = i;
		++i;
	}

	i = 0;
	while (s_comTypeName_earth_scene[i])
	{
		s_comsTypeMap_earth_scene[s_comTypeName_earth_scene[i]] = i;
		++i;
	}

	i = 0;
	while (s_comTypeName_earth_entity[i])
	{
		s_comsTypeMap_earth_entity[s_comTypeName_earth_entity[i]] = i;
		++i;
	}

	_ui->setupUi(this);
	hideAll();

	ComListWgt* pComListWgt = new ComListWgt(this);
	std::function<void(bool)> addDelComsFunc = [this, pComListWgt](bool bAddComponentBtn)
	{
		if (_curSelEnt)
		{
			QStringList comlist;
			QStringList fixedComs;
			if (_curSelEnt->isSpawner())
			{
				switch (_curSelEnt->breed())
				{
				case 0:
					s_curSelectComTypeName = s_comTypeName_terrain_scene;
					fixedComs << s_comTypeName_terrain_scene[dof_ts] << s_comTypeName_terrain_scene[camera_ts];
					break;
				case 1:
					s_curSelectComTypeName = s_comTypeName_earth_scene;
					fixedComs << s_comTypeName_earth_scene[dof_es] << s_comTypeName_earth_scene[camera_es] << s_comTypeName_earth_scene[earth_es];
					break;
				case 2:
					s_curSelectComTypeName = s_comTypeName_map_scene;
					break;
				default:
					s_curSelectComTypeName = { nullptr };
					break;
				}
			}
			else
			{
				switch (_curSelEnt->getSpawner()->breed())
				{
				case 0:
					s_curSelectComTypeName = s_comTypeName_terrain_entity;
					fixedComs << s_comTypeName_terrain_entity[dof_te];
					break;
				case 1:
					s_curSelectComTypeName = s_comTypeName_earth_entity;
					fixedComs << s_comTypeName_earth_entity[dof_ee];
					break;
				case 2:
					s_curSelectComTypeName = s_comTypeName_map_entity;
					break;
				default:
					s_curSelectComTypeName = { nullptr };
					break;
				}
			}

			int i = 0;
			while (s_curSelectComTypeName[i])
			{
				if (!fixedComs.contains(s_curSelectComTypeName[i]))
					comlist << s_curSelectComTypeName[i];
				i++;
			}
			
			pComListWgt->refreshComList(_curSelEnt, bAddComponentBtn, comlist);
			pComListWgt->setVisible(true);
		}
	};

	connect(_ui->pushButton_add, &QPushButton::clicked, [addDelComsFunc]
	{
		addDelComsFunc(true);
	});
	connect(_ui->pushButton_remove, &QPushButton::clicked, [addDelComsFunc]
	{
		addDelComsFunc(false);
	});
}

ComPropertyBoard::~ComPropertyBoard()
{
	s_comsTypeMap_terrain_scene.clear();
	s_comsTypeMap_terrain_entity.clear();
	s_comsTypeMap_earth_scene.clear();
	s_comsTypeMap_earth_entity.clear();
	s_comsTypeMap_map_scene.clear();
	s_comsTypeMap_map_entity.clear();
}

void ComPropertyBoard::showCurEntComs(zoo::Entity* ent)
{
	_curSelEnt = ent;
	hideAll();
	if (ent)
	{
		unordered_map<string, Component*> coms = ent->getComponents();
		auto it = coms.begin();
		auto itEnd = coms.end();
		for (; it != itEnd; ++it)
		{
			showCom(it->first.c_str(), it->second);
		}

		_ui->pushButton_add->setVisible(true);
		_ui->pushButton_remove->setVisible(true);
	}
}

void ComPropertyBoard::showCom(QString comTypeName, zoo::Component* pCom)
{
	PropertyWgt* pWgt = _comPropertyWgts.value(comTypeName);
	if (!pWgt)
	{
		if (pCom->getEntity()->isSpawner())
		{
			switch (pCom->getEntity()->breed())
			{
			case 0:
				pWgt = createPropertyWgtForTerrainScene(comTypeName);
				break;
			case 1:
				pWgt = createPropertyWgtForEarthScene(comTypeName);
				break;
			case 2:
				pWgt = createPropertyWgtForMapScene(comTypeName);
				break;
			}
		}
		else
		{
			switch (pCom->getEntity()->getSpawner()->breed())
			{
			case 0:
				pWgt = createPropertyWgtForTerrainEntity(comTypeName);
				break;
			case 1:
				pWgt = createPropertyWgtForEarthEntity(comTypeName);
				break;
			case 2:
				pWgt = createPropertyWgtForMapEntity(comTypeName);
				break;
			}
		}

		if (pWgt)
		{
			_comPropertyWgts.insert(comTypeName, pWgt);
			int cnt = _ui->verticalLayout->count();
			_ui->verticalLayout->insertWidget(cnt - 2, pWgt);
		}
	}

	if (pWgt)
	{
		pWgt->resetCom(pCom);
		pWgt->setVisible(true);
	}
}

void ComPropertyBoard::hideCom(QString comTypeName)
{
	PropertyWgt* pWgt = _comPropertyWgts.value(comTypeName);
	if (pWgt)
		pWgt->setVisible(false);
}

void ComPropertyBoard::hideAll()
{
	_ui->pushButton_add->setVisible(false);
	_ui->pushButton_remove->setVisible(false);
	auto it = _comPropertyWgts.begin();
	auto itEnd = _comPropertyWgts.end();
	for (; it != itEnd; ++it)
	{
		it.value()->setVisible(false);
	}
}

PropertyWgt* ComPropertyBoard::createPropertyWgtForTerrainScene(QString comTypeName)
{
	PropertyWgt* pWgt = nullptr;
	if (!s_comsTypeMap_terrain_scene.contains(comTypeName))
		return nullptr;
	switch (s_comsTypeMap_terrain_scene[comTypeName])
	{
	case dof_ts: pWgt = new DoFPropertyWgt(this); break;
	case behavior_ts: pWgt = new BehaviorPropertyWgt(this); break;
	case camera_ts: pWgt = new CameraPropertyWgt(this); break;
	case model_ts: pWgt = new ModelPropertyWgt(this); break;
	case sound_ts: break;
	case animator_ts: break;
	case collider_ts: break;
	case environment_ts: pWgt = new EnvirPropertyWgt(this); break;
	default: break;
	}
	return pWgt;
}

PropertyWgt* ComPropertyBoard::createPropertyWgtForTerrainEntity(QString comTypeName)
{
	PropertyWgt* pWgt = nullptr;
	if (!s_comsTypeMap_terrain_entity.contains(comTypeName))
		return nullptr;
	switch (s_comsTypeMap_terrain_entity[comTypeName])
	{
	case dof_te: pWgt = new DoFPropertyWgt(this); break;
	case behavior_te: pWgt = new BehaviorPropertyWgt(this); break;
	case camera_te: pWgt = new CameraPropertyWgt(this); break;
	case model_te: pWgt = new ModelPropertyWgt(this); break;
	case sound_te: break;
	case animator_te: break;
	case collider_te: break;
	default: break;
	}
	return pWgt;
}

PropertyWgt* ComPropertyBoard::createPropertyWgtForEarthScene(QString comTypeName)
{
	PropertyWgt* pWgt = nullptr;
	if (!s_comsTypeMap_earth_scene.contains(comTypeName))
		return nullptr;
	switch (s_comsTypeMap_earth_scene[comTypeName])
	{
	case dof_es: pWgt = new DoFPropertyWgt(this); break;
	case behavior_es: pWgt = new BehaviorPropertyWgt(this); break;
	case camera_es: pWgt = new CameraPropertyWgt(this); break;
	case model_es: pWgt = new ModelPropertyWgt(this); break;
	case sound_es: break;
	case animator_es: break;
	case collider_es: break;
	case earth_es: pWgt = new EarthPropertyWgt(this); break;
	default: break;
	}
	return pWgt;
}

PropertyWgt* ComPropertyBoard::createPropertyWgtForEarthEntity(QString comTypeName)
{
	PropertyWgt* pWgt = nullptr;
	if (!s_comsTypeMap_earth_entity.contains(comTypeName))
		return nullptr;
	switch (s_comsTypeMap_earth_entity[comTypeName])
	{
	case dof_ee: pWgt = new DoFPropertyWgt(this); break;
	case behavior_ee: pWgt = new BehaviorPropertyWgt(this); break;
	case camera_ee: pWgt = new CameraPropertyWgt(this); break;
	case model_ee: pWgt = new ModelPropertyWgt(this); break;
	case sound_ee: break;
	case animator_ee: break;
	case collider_ee: break;
	default: break;
	}
	return pWgt;
}

PropertyWgt* ComPropertyBoard::createPropertyWgtForMapScene(QString comTypeName)
{
	PropertyWgt* pWgt = nullptr;
	if (!s_comsTypeMap_map_scene.contains(comTypeName))
		return nullptr;
	switch (s_comsTypeMap_map_scene[comTypeName])
	{
	default: break;
	}
	return pWgt;
}

PropertyWgt* ComPropertyBoard::createPropertyWgtForMapEntity(QString comTypeName)
{
	PropertyWgt* pWgt = nullptr;
	if (!s_comsTypeMap_map_entity.contains(comTypeName))
		return nullptr;
	switch (s_comsTypeMap_map_entity[comTypeName])
	{
	default: break;
	}
	return pWgt;
}
