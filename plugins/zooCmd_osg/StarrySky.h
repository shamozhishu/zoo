#ifndef __STARRY_SKY_H__
#define __STARRY_SKY_H__

#include <OpenThreads/Mutex>
#include <osg/NodeCallback>
#include <osgViewer/View>
#include <osgEarthUtil/Sky>
#include "NebulaNode.h"

class AnimateSkyCallback;
class SychLocalTimeCallback;

/**
* @class CFreeSky
* @brief 星空类，封装了系统提供的星空背景
* @note 提供统一的星空背景来显示远距离恒星、太阳、月亮等
* @author l00008
*/
class  StarrySky : public osg::Group
{
public:
	StarrySky(osgEarth::MapNode* pMapNode, osgViewer::View* view);

public:
	/**
	* 使用MapNode创建一个星空节点
	*/
	bool initialize();

	/**
	* 使用MapNode和选项创建一个星空节点
	*/
	bool initialize(const osgEarth::Util::SkyOptions&  options);

	/**
	*@note: 关联View
	*/
	void attach(osgViewer::View* pView, int nLightNum = 0);

	/**
	*@note: 显隐控制,是对总体天体系统的控制，只有当其显示时其他的比如太阳、月亮、
	*		星系、星云才会显示，否则只是作为可显示状态，但未必显示。同理当其隐藏时
	*		其他的无论出于显示或者隐藏状态都无法显示。
	*/
	virtual void show();

	/**
	*@note: 显隐控制,是对总体天体系统的控制，只有当其显示时其他的比如太阳、月亮、
	*		星系、星云才会显示，否则只是作为可显示状态，但未必显示。同理当其隐藏时
	*		其他的无论出于显示或者隐藏状态都无法显示。
	*/
	virtual void hide();

	virtual bool isHide();

	/**
	* @note 是否使用标准环境
	* @param  bNormal [in] true 标准环境（太阳、月亮、星空、星云） false 非标准（强制隐藏太阳、月亮、星空、星云）
	*/
	virtual void setNormalEnvironmentState(bool bNormal);
	virtual bool isNormalEnvironment();

	///控制类型的接口，主要控制状态的显隐
public:
	/**
	*@note: 设置太阳的显隐
	*/
	void setSunVisible(bool bValue);

	/**
	*@note: 获取太阳的显隐
	*/
	bool getSunVisible();

	/**
	*@note: 设置月球的显隐
	*/
	void setMoonVisible(bool bValue);

	/**
	*@note: 获取月球的显隐
	*/
	bool getMoonVisible();

	/**
	*@note: 设置星系的显隐
	*/
	void setStarsVisible(bool bValue);

	/**
	*@note: 获取星系的显隐
	*/
	bool getStarsVisible();

	/**
	*@note: 设置星云的显隐
	*/
	void setNebulaVisible(bool bValue);

	/**
	*@note: 获取星云的显隐
	*/
	bool getNebulaVisible();

	/**
	*@note: 开启或者关闭大气的显隐
	*/
	void setAtmosphereVisible(bool bState);

	/**
	*@note: 获取大气的显隐
	*/
	bool getAtmosphereVisible() const;

	/**
	*@note: 开启或者关闭时间的流逝状态
	*/
	void setTimeFlyVisible(bool bState);

	/**
	*@note: 获取是否开启或者关闭时间的流逝
	*/
	bool getTimeFlyVisible() const;

	/**
	*@note: 开启或者关闭同步本地时间
	*/
	void setSyncLocalTime(bool bState);

	/**
	*@note: 获取是否开启或者关闭同步本地时间
	*/
	bool getSyncLocalTime() const;

public:
	/**
	*@note: 设置当前时间流逝速度
	*/
	void setAnimationRate(double dRate);

	/**
	*@note: 获取当前时间流逝速度
	*/
	double getAnimationRate() const;

	/**
	*@note: 设置当前的UTC时间
	*/
	void setDateTime(const osgEarth::DateTime& time);

	/**
	*@note: 获得当前的UTC时间
	*/
	osgEarth::DateTime getDateTime() const;

	/**
	*@note: 设置环境光照，这个设置只有在光照开启状态下有效
	*/
	void setAmbient(const osg::Vec4& vecAmbient);

	/**
	*@note: 获得环境光照
	*/
	osg::Vec4 getAmbient() const;

	///获得一些特殊的节点，这些一般用户不会使用
public:
	///获得地球大气层节点
	osg::Node* getAtmosphereNode();

	///获得太阳实体节点
	osg::Node* getSunNode();

	///获得月亮实体节点
	osg::Node* getMoonNode();

	///获得远距离恒星实体节点
	osg::Node* getStarsNode();

	///获得星云节点
	osg::Node* getNebulaNode();

	///获得星SkyNode节点
	osg::Light* getLight();

	/**
		* @note 大气节点雾化参数
	*/
	osg::Uniform* getUniformFogEnabled();

	osg::Uniform* getUniformFogColor();

protected:
	virtual ~StarrySky(void);
	virtual void traverse(osg::NodeVisitor& nv);

protected:
	///太阳天空节点
	osg::observer_ptr<osgEarth::Util::SkyNode> m_opSkyNode;

	///星云背景节点
	osg::observer_ptr<NebulaNode>              m_opNebulaNode;

	///时间流逝是否开启
	bool                                       m_bAnimationSwitch;

	///时间流逝的回调
	osg::ref_ptr<AnimateSkyCallback>           m_rpAnimation;

	///同步本地时间是否开启
	bool                                       m_bSychLocalTime;

	///同步本地时间回调
	osg::ref_ptr<SychLocalTimeCallback>        m_rpSychCallback;

	///是否显示
	bool                                       m_bShow;

	///地图节点
	osg::observer_ptr<osgEarth::MapNode>       m_opMapNode;

	///视图
	osg::observer_ptr<osgViewer::View>         m_opView;

	///是否是标准环境
	bool                                       m_bNormal;

	///自转矩阵
	osg::ref_ptr<osg::MatrixTransform>         m_rpRotateMT;
};

/**
* @class AnimateSkyCallback
* @brief 更新时间的动画回调
* @note 通过此动画回调，可以更新时间来显示一天中的不同时刻地球状态
* @author l00008
*/
class AnimateSkyCallback : public osg::NodeCallback
{
public:
	AnimateSkyCallback(double rate = 1440);

public:
	/**
	*@note: 设置更新的速率
	*/
	void setUpdateRate(double dRate);

	/**
	*@note: 获取更新的速率
	*/
	double getUpdateRate();

	/**
	*@note: 用于重置参数
	*/
	void reset();

	/**
	*@note: 每一帧更新的操作，逐步的增加时间
	*/
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

protected:
	virtual ~AnimateSkyCallback();

private:
	///累计时间
	double m_dAccumTime;

	///上一次时间
	double m_dPrevTime;

	///速率
	double m_dRate;

	///互斥体
	OpenThreads::Mutex m_rateMutex;
};

/**
* @class AnimateSkyCallback
* @brief 更新时间的动画回调
* @note 通过此动画回调，可以更新时间来显示一天中的不同时刻地球状态
* @author l00008
*/
class SychLocalTimeCallback : public osg::NodeCallback
{
public:
	SychLocalTimeCallback();

public:
	/**
	*@note: 每一帧更新的操作，逐步的增加时间
	*/
	virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

protected:
	virtual ~SychLocalTimeCallback();
};

#endif // __STARRY_SKY_H__
