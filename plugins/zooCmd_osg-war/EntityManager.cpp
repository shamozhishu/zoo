#include "EntityManager.h"
#include "WarCmd.h"
#include <zoo/Log.h>
#include <zoo/DoF.h>
#include <zoo/UserData.h>

EntityManager::EntityManager()
{
}

EntityManager::~EntityManager()
{
}

void EntityManager::onCreateEffect(const UserData& userdata)
{
	ENTITY ent = addEntity(userdata, ENTITY_EFFECT);
	if (ent._dof)
	{
		ent._switchNode = new osg::Switch;
		ent._switchNode->addChild(ent._entNode);
		ent._switchNode->setAllChildrenOff();
		ent._entGroup->addChild(ent._switchNode);
		ent._entTransform->addChild(ent._entGroup);
		WarCmd::getSingleton().getMainNode()->addChild(ent._entTransform);
	}
}

void EntityManager::onCreateWeapon(const UserData& userdata)
{
	ENTITY ent = addEntity(userdata, ENTITY_WEAPON);
	if (ent._dof)
	{
		ent._entGroup->addChild(ent._entNode);
		ent._entTransform->addChild(ent._entGroup);
		WarCmd::getSingleton().getMainNode()->addChild(ent._entTransform);
		ent._entGroup->getOrCreateStateSet()->setRenderBinDetails(-1, "RenderBin");
		ent.m_lineSeg = new osgUtil::LineSegmentIntersector(osg::Vec3(0.0, 0.0, 0.0), osg::Vec3(0.0, 0.0, 0.0));
	}
}

void EntityManager::onCreateRedArmy(const UserData& userdata)
{
	ENTITY ent = addEntity(userdata, ENTITY_REDARMY);
	if (ent._dof)
	{
		ent._entGroup->addChild(ent._entNode);
		ent._entTransform->addChild(ent._entGroup);
		WarCmd::getSingleton().getMainNode()->addChild(ent._entTransform);
		ent._entGroup->getOrCreateStateSet()->setRenderBinDetails(-1, "RenderBin");
	}
}

void EntityManager::onCreateBlueArmy(const UserData& userdata)
{
	ENTITY ent = addEntity(userdata, ENTITY_BLUEARMY);
	if (ent._dof)
	{
		ent._entGroup->addChild(ent._entNode);
		ent._entTransform->addChild(ent._entGroup);
		WarCmd::getSingleton().getMainNode()->addChild(ent._entTransform);
		ent._entGroup->getOrCreateStateSet()->setRenderBinDetails(-1, "RenderBin");
	}
}

void EntityManager::onCreateAllyArmy(const UserData& userdata)
{
	ENTITY ent = addEntity(userdata, ENTITY_ALLYARMY);
	if (ent._dof)
	{
		ent._entGroup->addChild(ent._entNode);
		ent._entTransform->addChild(ent._entGroup);
		WarCmd::getSingleton().getMainNode()->addChild(ent._entTransform);
		ent._entGroup->getOrCreateStateSet()->setRenderBinDetails(-1, "RenderBin");
	}
}

void EntityManager::onCreateStaticObj(const UserData& userdata)
{
	ENTITY ent = addEntity(userdata, ENTITY_STATICOBJ);
	if (ent._dof)
	{
		const char* vertexShader =
		{
			"void main(void) {\n"
			"   gl_TexCoord[0] = gl_MultiTexCoord0;\n"
			"   gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;\n"
			"}\n"
		};

		const char* fragShader =
		{
			"uniform sampler2D baseTexture;\n"
			"void main(void) {\n"
			"   vec2 coord = gl_TexCoord[0].xy;\n"
			"   vec4 C = texture2D(baseTexture, coord)\n;"
			"	gl_FragColor = C;\n"
			"}\n"
		};

		osg::ref_ptr<osg::Program> program = new  osg::Program();
		program->addShader(new osg::Shader(osg::Shader::VERTEX, vertexShader));
		program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragShader));
		ent._entNode->getOrCreateStateSet()->setAttributeAndModes(program, osg::StateAttribute::ON);

		ent._entGroup->addChild(ent._entNode);
		ent._entTransform->addChild(ent._entGroup);
		WarCmd::getSingleton().getMainNode()->addChild(ent._entTransform);
		ent._entGroup->getOrCreateStateSet()->setRenderBinDetails(-1, "RenderBin");
	}
}

ENTITY EntityManager::addEntity(const UserData& userdata, ENTITY_TYPE entType)
{
	ENTITY ent;
	int id = any_cast<int>(userdata.getData("id"));
	ent._dof = DoF::findDoF(id, entType);
	if (!ent._dof)
	{
		Log::print(ELL_ERROR, "Entity[id:%d][type:%d]: DoF does not exist.", id, entType);
		return ent;
	}

	string filename = any_cast<string>(userdata.getData("model_file"));
	filename = ZOOCMD_DATA_DIR + filename;
	ent._entNode = osgDB::readNodeFile(filename);
	if (!ent._entNode)
	{
		Log::print(ELL_ERROR, "Model file [%s] load failed", filename.c_str());
		ent._dof = nullptr;
		return ent;
	}

	ent._entGroup = new osg::Group;
	ent._entTransform = new osg::MatrixTransform;

	long long i64ID = entType;
	i64ID = i64ID << 32 | id;
	_entities[i64ID] = ent;
	return ent;
}
