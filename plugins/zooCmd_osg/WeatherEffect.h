#ifndef __WEATHER_EFFECT_H__
#define __WEATHER_EFFECT_H__

#include <osg/CoordinateSystemNode>
#include <osgParticle/PrecipitationEffect>

class OsgEarthContext;
class WeatherEffect : public osgParticle::PrecipitationEffect
{
public:
	WeatherEffect(OsgEarthContext* context);
	WeatherEffect(const WeatherEffect& copy, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY);
	~WeatherEffect();

	// 设置天气位置
	void setWeatherPosition(osg::Vec3 pos, bool lonlat = false, bool local = false)
	{
		_weatherPos = pos;
		_islonlat = lonlat;
		_isLocal = local;
		_dirty = true;
	}

	// 获取天气位置
	osg::Vec3 getWeatherPosition() { return _weatherPos; }

	// 是否为经纬高
	bool isLonLat() { return _islonlat; }

	// 是否为局部雨雪
	bool isLocal() { return _isLocal; }

	// 设置天气产生的高度
	void setWeatherHeight(float height) { _weatherHeight = height; _dirty = true; }

	// 获取天气产生的高度
	float getWeatherHeight() { return _weatherHeight; }

protected:
	void update();
	bool build(const osg::Vec3 eyeLocal, int i, int j, int k, float startTime,
		PrecipitationDrawableSet& pds, osg::Polytope& frustum, osgUtil::CullVisitor* cv) const;

protected:
	bool _islonlat; // 是否使用经纬高
	bool _isLocal; // 全局或者局部雨雪
	osg::Vec3 _weatherPos;
	float _weatherHeight; // 高度
	osg::Vec4 _weatherRange; // 范围，如果是经纬高则等价于最小经度、最大经度、最小纬度、最大纬度
	OsgEarthContext* _earthContext;
};

#endif // __WEATHER_EFFECT_H__
