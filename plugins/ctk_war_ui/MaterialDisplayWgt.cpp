#include "MaterialDisplayWgt.h"
#include <QScreen>
#include <QWindow>
#include <QLayout>
#include <QApplication>
#include <zoo/Component.h>
#include <ctk_service/WarService.h>
#include <UniversalGlobalServices.h>
#include <zooCmdLoader/ZooCmdLoader.h>
#include <component/WarComponents.h>

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

#define MAT_DISPLAY_SCENEID (-99)

MaterialDisplayWgt::MaterialDisplayWgt(QWidget* parent /*= Q_NULLPTR*/)
	: QWidget(parent)
	, _turnOnLight(false)
	, _displayedMat(nullptr)
	, _materialSphere(nullptr)
{
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	zooCmdL_Load();

	QScreen* screen = windowHandle() && windowHandle()->screen() ? windowHandle()->screen() : qApp->screens().front();
	QWidget* pGLWgt = (QWidget*)zooCmd_Setup(width(), height(), screen->devicePixelRatio(), MATERIAL_DISPLAY_WGT);
	if (pGLWgt)
	{
		setLayout(new QHBoxLayout);
		layout()->setMargin(0);
		layout()->addWidget(pGLWgt);
		Spawner* pSpawner = Spawner::create(MAT_DISPLAY_SCENEID, 0, "DoF", "OsgContextImpl", "material sphere");
		_materialSphere = pSpawner->born(1, 1);
		Model* model = _materialSphere->addComponent<Model>("ModelImpl");
		model->_mesh._currentUseMeshName = "Sphere";
		ServiceLocator<MeshList>().getService()->switchMesh("Sphere", &model->_mesh);
		Camera* cam = _materialSphere->addComponent<Camera, int>("CameraImpl", MATERIAL_DISPLAY_WGT);
		cam->_manipulatorKey = Camera::NodeTracker_;
		cam->_trackEntID = cam->_trackEntBreed = 1;
		pSpawner->awakeAll();
		pSpawner->updateAll();
		_displayedMat = &model->_material;
	}
}

MaterialDisplayWgt::~MaterialDisplayWgt()
{
	Spawner::destroy(MAT_DISPLAY_SCENEID);
}

Material* MaterialDisplayWgt::getDisplayedMat() const
{
	return _displayedMat;
}

void MaterialDisplayWgt::refreshMaterialSphere(bool turnOnLight)
{
	if (_materialSphere)
	{
		if (_turnOnLight != turnOnLight)
		{
			_turnOnLight = turnOnLight;
			Camera* pCam = _materialSphere->getComponent<Camera>();
			if (pCam)
			{
				int clr = turnOnLight ? 255 : 0;
				pCam->_red = clr;
				pCam->_green = clr;
				pCam->_blue = clr;
				pCam->_alpha = 255;
				pCam->dirtyBit().addState(Camera::Bgcolour_);
			}
		}

		_materialSphere->updateAll();
	}
}

void MaterialDisplayWgt::generateMaterialSphere(std::string materialFile)
{
	ServiceLocator<MaterialList>::getService()->compileMaterial(materialFile, _displayedMat);
}
