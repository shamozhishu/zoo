#include "WarCmd.h"
#include "EntityManager.h"
#include <zooCmd_osg/InputDevice.h>

using namespace zooCmd_osg;

ZOO_REGISTER(WarCmd)
REFLEX_IMPLEMENT(WarCmd);

WarCmd::WarCmd()
{
	new EntityManager;
}

WarCmd::~WarCmd()
{
	delete EntityManager::getSingletonPtr();
}

bool WarCmd::init()
{
	m_root = InputDevice::getIns()->getRootNode();
	m_main = InputDevice::getIns()->getRootNode();
	m_root->addChild(m_main);

	m_mainView = InputDevice::getIns()->createView(0, 1, 0.5f, 1);
	m_bottom_leftView = InputDevice::getIns()->createView(0, 0.5f, 0, 0.5f);
	m_bottom_rightView = InputDevice::getIns()->createView(0.5f, 1, 0, 0.5f);

	m_tankTrack = new osgGA::NodeTrackerManipulator;
	m_seekerTrack = new osgGA::NodeTrackerManipulator;
	m_missileTrack = new osgGA::NodeTrackerManipulator;
	m_helicopterTrack = new osgGA::NodeTrackerManipulator;
	m_top_view_default_track = new osgGA::TrackballManipulator;
	m_bottom_left_view_default_track = new osgGA::TrackballManipulator;
	m_bottom_right_view_default_track = new osgGA::TrackballManipulator;

	m_mainView->setSceneData(m_root);
	m_bottom_leftView->setSceneData(m_main);
	m_bottom_rightView->setSceneData(m_main);

	m_mainView->setCameraManipulator(m_missileTrack);
	m_bottom_leftView->setCameraManipulator(m_tankTrack);
	m_bottom_rightView->setCameraManipulator(m_seekerTrack);

	m_root->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	m_main->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

	osg::ref_ptr<osg::Geode> panelGeode = createPanel();
	m_HUDProjection = new osg::Projection;
	m_HUDProjection->setMatrix(osg::Matrix::ortho2D(0, 1366, 0, 550));
	osg::ref_ptr<osg::MatrixTransform> HUDViewMatrix = new osg::MatrixTransform;
	HUDViewMatrix->setMatrix(osg::Matrix::identity());
	HUDViewMatrix->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	m_HUDProjection->addChild(HUDViewMatrix.get());
	HUDViewMatrix->addChild(panelGeode.get());

	m_text = new osgText::Text;
	m_text->setCharacterSize(37.5f);
	m_text->setFont("fonts/simhei.ttf");
	m_text->setText("");
	m_text->setAlignment(osgText::Text::LEFT_CENTER);
	m_text->setAxisAlignment(osgText::Text::SCREEN);
	m_text->setPosition(osg::Vec3(500, 50, -1.5));
	m_text->setColor(osg::Vec4(199, 77, 15, 1));
	panelGeode->addDrawable(m_text.get());
	m_root->addChild(m_HUDProjection.get());

	return true;
}

void WarCmd::parseCmdArg(CmdParser& cmdarg, UserData& retValue)
{
	std::function<bool(string, SlotMethod<EntityManager>::SLOT_METHOD)> func = [&, this](string cmd, SlotMethod<EntityManager>::SLOT_METHOD para)
	{
		int id;
		string model_file;

		if (cmdarg.read(cmd, id, model_file))
		{
			_subCommands.userData().setData("id", id);
			_subCommands.userData().setData("model_file", model_file);
			SignalTrigger::connect(_subCommands, EntityManager::getSingletonPtr(), para);
			return true;
		}

		return false;
	};

	do
	{
		if (func("--effect", &EntityManager::onCreateEffect))
			break;

		if (func("--weapon", &EntityManager::onCreateWeapon))
			break;

		if (func("--redarmy", &EntityManager::onCreateRedArmy))
			break;

		if (func("--bluearmy", &EntityManager::onCreateBlueArmy))
			break;

		if (func("--allyarmy", &EntityManager::onCreateAllyArmy))
			break;

		if (func("--staticobj", &EntityManager::onCreateStaticObj))
			break;
	} while (0);
}

void WarCmd::helpInformation(CmdUsage* usage)
{
	usage->setDescription("war command: war scene visual simulation.");
	usage->addCommandLineOption("--weapon <id:int> <model_file:string>", "Create weapon model.");
}

osg::Group* WarCmd::getRootNode() const
{
	return m_root.get();
}

osg::Group* WarCmd::getMainNode() const
{
	return m_main.get();
}

osg::Geode* WarCmd::createPanel()
{
	osg::ref_ptr<osg::Geode> panelGeode = new osg::Geode;
	osg::ref_ptr<osg::Geometry> panelGeom = new osg::Geometry;

	osg::ref_ptr<osg::Vec3Array> vertexArray = new osg::Vec3Array;
	vertexArray->push_back(osg::Vec3(0, 0, -1));
	vertexArray->push_back(osg::Vec3(1366, 0, -1));
	vertexArray->push_back(osg::Vec3(1366, 100, -1));
	vertexArray->push_back(osg::Vec3(0, 100, -1));

	osg::ref_ptr<osg::Vec4Array> colorArray = new osg::Vec4Array;
	colorArray->push_back(osg::Vec4(0.8f, 0.8f, 0.8f, 0.3f));

	osg::ref_ptr<osg::Vec3Array> normalArray = new osg::Vec3Array;
	normalArray->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));

	osg::ref_ptr<osg::Vec2Array> textrueIndex = new osg::Vec2Array;
	textrueIndex->push_back(osg::Vec2(0.0f, 0.0f));
	textrueIndex->push_back(osg::Vec2(1.0f, 0.0f));
	textrueIndex->push_back(osg::Vec2(1.0f, 1.0f));
	textrueIndex->push_back(osg::Vec2(0.0f, 1.0f));

	osg::ref_ptr<osg::DrawElementsUInt> panelUnit = new osg::DrawElementsUInt(osg::PrimitiveSet::POLYGON, 0);
	panelUnit->push_back(0);
	panelUnit->push_back(1);
	panelUnit->push_back(2);
	panelUnit->push_back(3);

	panelGeom->setVertexArray(vertexArray.get());
	panelGeom->setNormalArray(normalArray.get());
	panelGeom->setNormalBinding(osg::Geometry::BIND_OVERALL);
	panelGeom->setColorArray(colorArray.get());
	panelGeom->setColorBinding(osg::Geometry::BIND_OVERALL);
	panelGeom->setTexCoordArray(0, textrueIndex.get());
	panelGeom->addPrimitiveSet(panelUnit.get());
	panelGeode->addDrawable(panelGeom.get());

	osg::ref_ptr<osg::StateSet> HUDStateSet = panelGeode->getOrCreateStateSet();
	HUDStateSet->setMode(GL_BLEND, osg::StateAttribute::ON);
	HUDStateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	HUDStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	HUDStateSet->setRenderBinDetails(11, "RenderBin");

	return panelGeode.release();
}
