#include "StarrySky.h"
#include <zoo/Utils.h>
#include <OpenThreads/ScopedLock>
#include <osgEarth/NodeUtils>
#include <osgEarth/Version>

static const double s_dTimeFlyRate = 1440.0; // 默认的流逝速率

StarrySky::StarrySky(osgEarth::MapNode* pMapNode, osgViewer::View* view)
	: m_rpAnimation(NULL)
	, m_rpSychCallback(NULL)
	, m_opSkyNode(NULL)
	, m_opNebulaNode(NULL)
	, m_bAnimationSwitch(false)
	, m_bSychLocalTime(false)
	, m_bShow(false)
	, m_opMapNode(pMapNode)
	, m_opView(view)
	, m_bNormal(true)
	, m_rpRotateMT(new osg::MatrixTransform)
{
}

StarrySky::~StarrySky()
{
}

void StarrySky::show()
{
	if (!m_bShow)
	{
		setSunVisible(true);
		m_bShow = true;
	}
}

void StarrySky::hide()
{
	if (m_bShow)
	{
		m_bShow = false;
		setSunVisible(false);
	}
}

bool StarrySky::initialize()
{
	return initialize(osgEarth::Util::SkyOptions());
}

bool StarrySky::initialize(const osgEarth::Util::SkyOptions& options)
{
	// 创建默认的太阳系光照系统以及星空系统
	m_opSkyNode = osgEarth::Util::SkyNode::create(options, m_opMapNode.get());
	if (m_opSkyNode.valid())
	{
		// 默认打开环境时 开启大气、星辰和光照
		setAtmosphereVisible(true);
		setStarsVisible(true);
		setSunVisible(true);

		if (!m_opMapNode->containsNode(m_opSkyNode.get()))
		{
			osgEarth::insertGroup(m_opSkyNode.get(), m_opMapNode.get());
		}

		//// z00013 添加地球自转MT
		//if(!m_rpRotateMT->containsNode(m_opSkyNode.get()))
		//{
		//	osg::Group *pGroup = m_opRenderContext->GetRootNode();
		//	m_rpRotateMT->addChild(m_opSkyNode.get());
		//	pGroup->addChild(m_rpRotateMT);
		//}
	}

	//添加Nebula（星空背景），自定义附加的节点
	std::string strNubela = ZOO_DATA_ROOT_DIR + "texture/sky/nebula.jpg";
	m_opNebulaNode = new NebulaNode(m_opMapNode.get(), strNubela);
	addChild(m_opNebulaNode.get());
	//创建时光流逝的callback
	m_rpAnimation = new AnimateSkyCallback();
	m_rpSychCallback = new SychLocalTimeCallback();

	m_bShow ? show() : hide();

	setTimeFlyVisible(true);

	return true;
}

void StarrySky::setTimeFlyVisible(bool bState)
{
	if (bState && m_bSychLocalTime)
	{
		setSyncLocalTime(false);
	}

	if (m_opSkyNode.valid() && m_rpAnimation.valid())
	{
		if (bState)
		{
			addUpdateCallback(m_rpAnimation.get());
		}
		else
		{
			removeUpdateCallback(m_rpAnimation.get());
			m_rpAnimation->reset();
		}
		m_bAnimationSwitch = bState;
	}
}

bool StarrySky::getTimeFlyVisible() const
{
	return m_bAnimationSwitch;
}

void StarrySky::setAnimationRate(double dRate)
{
	if (m_rpAnimation.valid())
	{
		m_rpAnimation->setUpdateRate(dRate);
	}
}

double StarrySky::getAnimationRate() const
{
	if (m_rpAnimation.valid())
	{
		return m_rpAnimation->getUpdateRate();
	}

	return s_dTimeFlyRate;
}

void StarrySky::setDateTime(const osgEarth::DateTime& time)
{
	if (m_opSkyNode.valid())
	{
		m_opSkyNode->setDateTime(time);
	}

	if (m_opNebulaNode.valid())
	{
		m_opNebulaNode->SetDateTime(time);
	}
}

osgEarth::DateTime StarrySky::getDateTime() const
{
	if (m_opSkyNode.valid())
	{
		return m_opSkyNode->getDateTime();
	}

	return osgEarth::DateTime();
}

void StarrySky::attach(osgViewer::View* pView, int nLightNum)
{
	if (m_opSkyNode.valid())
	{
		return m_opSkyNode->attach(pView, nLightNum);
	}
}

void StarrySky::setAmbient(const osg::Vec4& vecAmbient)
{
	if (m_opSkyNode.valid())
	{
		osg::Light* pLight = m_opSkyNode->getSunLight();
		if (pLight)
		{
			pLight->setAmbient(vecAmbient);
		}
	}
}

osg::Vec4 StarrySky::getAmbient() const
{
	if (m_opSkyNode.valid())
	{
		osg::Light* pLight = m_opSkyNode->getSunLight();
		if (pLight)
		{
			return pLight->getAmbient();
		}
	}

	return osg::Vec4();
}

void StarrySky::setSunVisible(bool bValue)
{
	if (m_opSkyNode.valid())
	{
		m_opSkyNode->setLighting(bValue);
		if (m_bNormal)
		{
			m_opSkyNode->setSunVisible(bValue);
		}
		else
		{
			m_opSkyNode->setSunVisible(m_bNormal);
		}
	}
}

bool StarrySky::getSunVisible()
{
	if (m_opSkyNode.valid())
	{
		return m_opSkyNode->getSunVisible();
	}

	return false;
}

void StarrySky::setMoonVisible(bool bValue)
{
	if (m_opSkyNode.valid())
	{
		if (m_bNormal)
		{
			m_opSkyNode->setMoonVisible(bValue);
		}
		else
		{
			m_opSkyNode->setMoonVisible(m_bNormal);
		}
	}
}

bool StarrySky::getMoonVisible()
{
	if (m_opSkyNode.valid())
	{
		return m_opSkyNode->getMoonVisible();
	}

	return false;
}

void StarrySky::setStarsVisible(bool bValue)
{
	if (m_opSkyNode.valid())
	{
		if (m_bNormal)
		{
			m_opSkyNode->setStarsVisible(bValue);
		}
		else
		{
			m_opSkyNode->setStarsVisible(m_bNormal);
		}
	}
}

bool StarrySky::getStarsVisible()
{
	if (m_opSkyNode.valid())
	{
		return m_opSkyNode->getStarsVisible();
	}

	return false;
}

void StarrySky::setNebulaVisible(bool bValue)
{
	if (m_opNebulaNode.valid())
	{
		if (m_bNormal)
		{
			m_opNebulaNode->SetNebulaVisible(bValue);
		}
		else
		{
			m_opNebulaNode->SetNebulaVisible(m_bNormal);
		}
	}
}

bool StarrySky::getNebulaVisible()
{
	if (m_opNebulaNode.valid())
	{
		return m_opNebulaNode->GetNebulaVisible();
	}

	return false;
}

void StarrySky::setAtmosphereVisible(bool bState)
{
	if (m_opSkyNode.valid())
	{
#if OSGEARTH_MIN_VERSION_REQUIRED(2, 8, 0)
		m_opSkyNode->setAtmosphereVisible(bState);
#endif
	}
}

bool StarrySky::getAtmosphereVisible() const
{
	if (m_opSkyNode.valid())
	{
#if OSGEARTH_MIN_VERSION_REQUIRED(2, 8, 0)
		return m_opSkyNode->getAtmosphereVisible();
#endif
	}

	return false;
}

osg::Node* StarrySky::getNebulaNode()
{
	return m_opNebulaNode.get();
}

osg::Light* StarrySky::getLight()
{
	return m_opSkyNode->getSunLight();
}

void StarrySky::setSyncLocalTime(bool bState)
{
	if (bState && m_bAnimationSwitch)
	{
		setTimeFlyVisible(false);
	}

	if (m_opSkyNode.valid() && m_rpSychCallback.valid())
	{
		if (bState)
		{
			addUpdateCallback(m_rpSychCallback.get());
		}
		else
		{
			removeUpdateCallback(m_rpSychCallback.get());
		}

		m_bSychLocalTime = bState;
	}
}

bool StarrySky::getSyncLocalTime() const
{
	return m_bSychLocalTime;
}

bool StarrySky::isHide()
{
	return !m_bShow;
}

void StarrySky::setNormalEnvironmentState(bool bNormal)
{
	if (m_opSkyNode.valid())
	{
		m_bNormal = bNormal;
		if (bNormal)
		{
			m_opSkyNode->setSunVisible(m_bShow);
			m_opSkyNode->setMoonVisible(m_bShow);
			m_opSkyNode->setStarsVisible(bNormal);
			setNebulaVisible(bNormal);

		}
		else
		{
			m_opSkyNode->setSunVisible(bNormal);
			m_opSkyNode->setSunVisible(bNormal);
			m_opSkyNode->setMoonVisible(bNormal);
			m_opSkyNode->setStarsVisible(bNormal);
			setNebulaVisible(bNormal);
		}
	}
}

bool StarrySky::isNormalEnvironment()
{
	return m_bNormal;
}

void StarrySky::traverse(osg::NodeVisitor& nv)
{
	// zs  处理大气临时方案
	if (nv.getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
	{
		/*if(m_opSkyNode.valid() && m_opRenderContext.valid())
		{
			FxGeoUtil::IFxStarBody* pStarBody = m_opRenderContext->getStarBody(FxGeoUtil::EARTH_STAR);
			if(pStarBody)
			{
				osg::Matrix viewMatrixInv = m_opRenderContext->getView()->getCamera()->getInverseViewMatrix();
				m_opSkyNode->setRelativeViewMatrixInv(viewMatrixInv *osg::Matrix::inverse(pStarBody->getStarMatrix()));
				m_opSkyNode->setSunMatrix(pStarBody->getStarMatrix());
			}
		}*/
	}

	osg::Group::traverse(nv);
}

typedef ::time_t TimeStamp;

AnimateSkyCallback::AnimateSkyCallback(double rate /*= 1440 */)
	:m_dRate(rate)
	, m_dPrevTime(-1)
	, m_dAccumTime(0.0)
{
}

AnimateSkyCallback::~AnimateSkyCallback()
{
}

void AnimateSkyCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
	{
		StarrySky* pSky = dynamic_cast<StarrySky*>(node);
		if (pSky)
		{
			double time = nv->getFrameStamp()->getSimulationTime();
			double dJD = time;
			int nYear;
			int nMonth;
			int nDay;
			int nHour;
			int nMin;
			double dSec;
			tm UTCTM;
			UTCTM.tm_year = nYear - 1900;
			UTCTM.tm_mon = nMonth - 1;
			UTCTM.tm_mday = nDay;
			UTCTM.tm_hour = nHour + 8;
			UTCTM.tm_min = nMin;
			UTCTM.tm_sec = dSec;

			//if (m_dPrevTime > 0)
			{
				//TimeStamp t = pSky->GetDateTime().asTimeStamp();                  
				//double delta = ceil((time - m_dPrevTime) * m_dRate);
				//m_dAccumTime += delta;

				//if (m_dAccumTime > 1.0)
				{
					//double deltaS = floor(m_dAccumTime );                    
					//m_dAccumTime -= deltaS;
					//t += deltaS;
					pSky->setDateTime(UTCTM);
				}
			}
			m_dPrevTime = time;
		}
	}

	traverse(node, nv);
}

void AnimateSkyCallback::setUpdateRate(double dRate)
{
	OpenThreads::ScopedLock<OpenThreads::Mutex> lock(m_rateMutex);
	m_dRate = dRate;
}

double AnimateSkyCallback::getUpdateRate()
{
	return m_dRate;
}

void AnimateSkyCallback::reset()
{
	m_dPrevTime = -1;
	m_dAccumTime = 0.0;
}

SychLocalTimeCallback::SychLocalTimeCallback()
{
}

SychLocalTimeCallback::~SychLocalTimeCallback()
{
}

void SychLocalTimeCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
	if (nv->getVisitorType() == osg::NodeVisitor::UPDATE_VISITOR)
	{
		StarrySky* pSky = dynamic_cast<StarrySky*>(node);
		if (pSky)
		{
			osgEarth::DateTime dateTime = 0;//FreeXUtil::GetLocalTime();
			pSky->setDateTime(dateTime);
		}
	}

	traverse(node, nv);
}
