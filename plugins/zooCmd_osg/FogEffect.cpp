#include "FogEffect.h"
#include <osgEarth/GeoData>
#include <zooCmd_osg/OsgEarthContext.h>

//class FogUpdateCallBack : public osg::NodeCallback
//{
//public:
//	FogUpdateCallBack(osg::Fog * pfog)
//		:m_ipUniformSkyFogColor(nullptr)
//		, m_ipUniformSkyFogEnabled(nullptr)
//		, m_fIntensity(0.1)
//		, m_rpFog(pfog)
//	{
//		if (pSkyNode != nullptr)
//		{
//			CFreeSky * pFreeSky = dynamic_cast<CFreeSky*>(pSkyNode);
//			if (pFreeSky != nullptr)
//			{
//				m_ipUniformSkyFogColor = pFreeSky->getUniformFogColor();
//				m_ipUniformSkyFogEnabled = pFreeSky->getUniformFogEnabled();
//				m_pFreeSky = pFreeSky;
//			}
//		}
//	}
//
//	void operator()(osg::Node * node, osg::NodeVisitor * nv)
//	{
//		traverse(node, nv);
//
//		if (m_pFreeSky == nullptr || !m_rpFog.valid())
//		{
//			return;
//		}
//		osg::Vec3d eye, center, up;
//		// 计算浓度的参数,如果不用radio,拉远之后,整个地球呈现白色
//		m_pContext->getView()->getCamera()->getViewMatrixAsLookAt(eye, center, up);
//
//		osgEarth::GeoPoint map;
//		map.fromWorld(m_pContext->getMapSRS(), eye);
//
//		// 计算雾浓度
//		float fMaxDensity = 0.000125;
//		float fFogStartHeight = 1e4;
//		float fRatio = (fFogStartHeight - map.z()) / fFogStartHeight;
//		fRatio = osg::clampBetween(fRatio, 0.0f, 1.0f);
//		float fDensity = fRatio * fMaxDensity * m_fIntensity; // 浓度*3倍显示效果好
//		m_rpFog->setMode(osg::Fog::EXP2);
//		m_rpFog->setDensity(fDensity);
//		if (m_fIntensity == 0.0f || fDensity == 0.0f) // 添加计算雾的密度为0时不显示雾效果
//			m_ipUniformSkyFogEnabled->set(false);
//		else
//			m_ipUniformSkyFogEnabled->set(true);
//	}
//public:
//	void setIntensity(float intensity)
//	{
//		m_fIntensity = intensity;
//	}
//
//	void setFogEnable(bool flag)
//	{
//		if (m_ipUniformSkyFogEnabled.valid())
//			m_ipUniformSkyFogEnabled->set(flag);
//	}
//
//private:
//	float m_fIntensity;
//	osg::observer_ptr<osg::Fog> m_rpFog;
//	osg::observer_ptr<osg::Uniform>	m_ipUniformSkyFogColor;
//	osg::observer_ptr<osg::Uniform>	m_ipUniformSkyFogEnabled;
//};
//
//FogEffect::FogEffect()
//	: _isShow(false)
//{
//	_stateSet = pContext->getMapNode()->getOrCreateStateSet();
//	osg::Vec4 fogColor(0.66f, 0.7f, 0.81f, 1.0f);
//	_fog = new osg::Fog();
//	_fog->setColor(fogColor);
//	_fogEffect = new osgEarth::Util::FogEffect();
//	_fogUpdateCallback = new FogUpdateCallBack(_fog.get());
//}
//
//FogEffect::~FogEffect()
//{
//}
//
//void FogEffect::show()
//{
//	if (m_pContext == nullptr || !_stateSet.valid())
//		return;
//	_fogEffect->attach(_stateSet);
//	m_pContext->getMapNode()->addUpdateCallback(_fogUpdateCallback);
//	m_pContext->getMapNode()->getOrCreateStateSet()->setAttributeAndModes(_fog, osg::StateAttribute::ON);
//	_fogUpdateCallback->setFogEnable(true);
//	_isShow = true;
//}
//
//
//void FogEffect::hide()
//{
//	if (m_pContext == nullptr || !_stateSet.valid())
//		return;
//	_fogEffect->detach(_stateSet);
//	m_pContext->getMapNode()->removeUpdateCallback(_fogUpdateCallback);
//	m_pContext->getMapNode()->getOrCreateStateSet()->setAttributeAndModes(_fog, osg::StateAttribute::OFF);
//	_fogUpdateCallback->setFogEnable(false);
//	_isShow = false;
//}
//
//
//bool FogEffect::isShow()
//{
//	return _isShow;
//}
//
//void FogEffect::setIntensity(float intensity)
//{
//	if (_fogUpdateCallback.valid())
//		_fogUpdateCallback->setIntensity(intensity);
//}
