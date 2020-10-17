#include "MaterialDisplayWgt.h"
#include <zooCmdLoader/ZooCmdLoader.h>
#include <vector>
#include <string>
#include <QThread>
#include <QScreen>
#include <QWindow>
#include <QLayout>
#include <QSettings>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QMainWindow>
#include <QApplication>
#include <QInputDialog>
#include <zoo/Component.h>
#include <zoo/ServiceLocator.h>
#include <UniversalGlobalServices.h>
#include <component/war/WarComponents.h>
#include <ctk_service/zoocmd_ui/Win32Service.h>

using namespace zoo;

#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
#pragma execution_character_set("utf-8")
#endif

MaterialDisplayWgt::MaterialDisplayWgt(QWidget* parent /*= Q_NULLPTR*/)
	: QWidget(parent)
{
	setMouseTracking(true);
	setFocusPolicy(Qt::ClickFocus);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	zooCmdL_Load();

	QScreen* screen = windowHandle() && windowHandle()->screen() ? windowHandle()->screen() : qApp->screens().front();
	QWidget* pGLWgt = (QWidget*)zooCmd_Setup(width(), height(), screen->devicePixelRatio(), MATERIALDISPLAYWGT);
	if (pGLWgt)
	{
		setLayout(new QHBoxLayout);
		layout()->setMargin(0);
		layout()->addWidget(pGLWgt);
		Spawner* pSpawner = Spawner::create(-99, "OsgEarthContextImpl", "material sphere");
		Entity* pMaterialSphere = pSpawner->born(1, 1);
		Model* model = pMaterialSphere->addComponent<Model>("ModelImpl");
		model->_mesh._currentUseMeshName = "Sphere";
		ServiceLocator<MeshList>().getService()->getMeshConfigInfo("Sphere", &model->_mesh);
		Camera* cam = pMaterialSphere->addComponent<Camera, int>("CameraImpl", MATERIALDISPLAYWGT);
		cam->_manipulatorKey = Camera::NodeTracker_;
		cam->_trackEntID = cam->_trackEntBreed = 1;
		pSpawner->awakeAll();
		pSpawner->updateAll();
	}
}

MaterialDisplayWgt::~MaterialDisplayWgt()
{
	Spawner::destroy(-99);
}
