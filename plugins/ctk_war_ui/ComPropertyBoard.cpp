#include "ComPropertyBoard.h"
#include "UIActivator.h"
#include <ctk_service/zoocmd_ui/UIManagerService.h>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include "PropertyWgts.h"
#include "ArmyListWgt.h"

// Qt5��������
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

enum ComType
{
	dof_,
	behavior_,
	camera_,
	earth_,
	model_,
	sound_,
	animator_,
	collider_,
	environment_
};

static const char* s_comTypeName[] = { "DoF", "Behavior", "Camera", "Earth", "Model", "Sound", "Animator", "Collider", "Environment", nullptr };

static QHash<QString, ComType> s_comsTypeMap;
ComPropertyBoard::ComPropertyBoard()
	: _curSelEnt(nullptr)
	, _rootLayout(new QVBoxLayout(this))
	, _addComponentBtn(new QPushButton("�������", this))
	, _removeComponentBtn(new QPushButton("�Ƴ����", this))
{
	int i = dof_;
	while (s_comTypeName[i])
	{
		s_comsTypeMap[s_comTypeName[i]] = (ComType)i;
		++i;
	}

	hideAll();
	setLayout(_rootLayout);
	setMinimumWidth(300);
	setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

	QHBoxLayout* pLayoutTailer = new QHBoxLayout(this);
	pLayoutTailer->addWidget(_addComponentBtn);
	pLayoutTailer->addWidget(_removeComponentBtn);

	_rootLayout->addLayout(pLayoutTailer);
	_rootLayout->addStretch();

	ComListWgt* pComListWgt = new ComListWgt(this);
	std::function<void(bool)> addDelComsFunc = [this, pComListWgt](bool bAddComponentBtn)
	{
		if (_curSelEnt)
		{
			QStringList comlist;
			if (_curSelEnt->isSpawner())
			{
				comlist << s_comTypeName[behavior_] << s_comTypeName[earth_] << s_comTypeName[model_] << s_comTypeName[sound_]
					<< s_comTypeName[animator_] << s_comTypeName[collider_] << s_comTypeName[environment_];
			}
			else
			{
				comlist << s_comTypeName[behavior_] << s_comTypeName[camera_] << s_comTypeName[model_] << s_comTypeName[sound_]
					<< s_comTypeName[animator_] << s_comTypeName[collider_];
			}

			pComListWgt->refreshComList(_curSelEnt, bAddComponentBtn, comlist);
			pComListWgt->setVisible(true);
		}
	};

	connect(_addComponentBtn, &QPushButton::clicked, [addDelComsFunc]
	{
		addDelComsFunc(true);
	});
	connect(_removeComponentBtn, &QPushButton::clicked, [addDelComsFunc]
	{
		addDelComsFunc(false);
	});
}

ComPropertyBoard::~ComPropertyBoard()
{
	s_comsTypeMap.clear();
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

		_addComponentBtn->setVisible(true);
		_removeComponentBtn->setVisible(true);
	}
}

void ComPropertyBoard::showCom(QString comTypeName, zoo::Component* pCom)
{
	PropertyWgt* pWgt = _comPropertyWgts.value(comTypeName);
	if (!pWgt)
	{
		switch (s_comsTypeMap[comTypeName])
		{
		case dof_:
			pWgt = new DoFPropertyWgt(this);
			break;
		case model_:
			pWgt = new ModelPropertyWgt(this);
			break;
		case camera_:
			pWgt = new CameraPropertyWgt(this);
			break;
		case earth_:
			pWgt = new EarthPropertyWgt(this);
			break;
		case behavior_:
			pWgt = new BehaviorPropertyWgt(this);
			break;
		case environment_:
			pWgt = new EnvirPropertyWgt(this);
			break;
		default:
			return;
		}

		_comPropertyWgts.insert(comTypeName, pWgt);
		int cnt = _rootLayout->count();
		_rootLayout->insertWidget(cnt - 2, pWgt);
	}

	pWgt->resetCom(pCom);
	pWgt->setVisible(true);
}

void ComPropertyBoard::hideCom(QString comTypeName)
{
	PropertyWgt* pWgt = _comPropertyWgts.value(comTypeName);
	if (pWgt)
		pWgt->setVisible(false);
}

void ComPropertyBoard::hideAll()
{
	_addComponentBtn->setVisible(false);
	_removeComponentBtn->setVisible(false);
	auto it = _comPropertyWgts.begin();
	auto itEnd = _comPropertyWgts.end();
	for (; it != itEnd; ++it)
	{
		it.value()->setVisible(false);
	}
}