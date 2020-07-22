#include "ComPropertyBoard.h"
#include "UIActivator.h"
#include <ctk_service/zoocmd_ui/UIManagerService.h>
#include <QMessageBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QLabel>
#include "PropertyWgts.h"
#include "ArmyListWgt.h"

// Qt5中文乱码
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

enum ComType
{
	dof_,
	model_,
	sound_,
	animator_,
	collider_,
	camera_,
	environment_
};

const char* g_comTypeName[] = { "DoF", "Model", "Sound", "Animator", "Collider", "Camera", "Environment", nullptr };

static QHash<QString, ComType> s_comsTypeMap;
ComPropertyBoard::ComPropertyBoard()
	: _curSelEnt(nullptr)
	, _rootLayout(new QVBoxLayout(this))
	, _addComponentBtn(new QPushButton("添加组件", this))
	, _removeComponentBtn(new QPushButton("移除组件", this))
{
	s_comsTypeMap[g_comTypeName[dof_]] = dof_;
	s_comsTypeMap[g_comTypeName[model_]] = model_;
	s_comsTypeMap[g_comTypeName[sound_]] = sound_;
	s_comsTypeMap[g_comTypeName[animator_]] = animator_;
	s_comsTypeMap[g_comTypeName[collider_]] = collider_;
	s_comsTypeMap[g_comTypeName[camera_]] = camera_;
	s_comsTypeMap[g_comTypeName[environment_]] = environment_;

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
	connect(_addComponentBtn, &QPushButton::clicked, [this, pComListWgt]
	{
		if (_curSelEnt)
		{
			pComListWgt->refreshComList(_curSelEnt, true);
			pComListWgt->setVisible(true);
		}
	});

	connect(_removeComponentBtn, &QPushButton::clicked, [this, pComListWgt]
	{
		if (_curSelEnt)
		{
			pComListWgt->refreshComList(_curSelEnt, false);
			pComListWgt->setVisible(true);
		}
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
		default:
			return;
		}

		_comPropertyWgts.insert(comTypeName, pWgt);
		int cnt = _rootLayout->count();
		_rootLayout->insertWidget(cnt - 2, pWgt);
	}

	pWgt->setCom(pCom);
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
