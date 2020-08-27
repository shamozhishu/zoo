#ifndef __FOG_EFFECT_H__
#define __FOG_EFFECT_H__

#include <osg/Fog>
#include <osgEarthUtil/Fog>

class FogUpdateCallBack;
class FogEffect : public osg::Referenced
{
public:
	FogEffect();
	~FogEffect();
	void show();
	void hide();
	bool isShow();
	void setIntensity(float intensity);

private:
	bool _isShow;
	osg::ref_ptr<osg::Fog> _fog;
	osg::ref_ptr<osg::StateSet> _stateSet;
	osg::ref_ptr<FogUpdateCallBack> _fogUpdateCallback;
	osg::ref_ptr<osgEarth::Util::FogEffect> _fogEffect;
};

#endif // __FOG_EFFECT_H__
