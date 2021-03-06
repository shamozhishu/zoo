/************************************************************************\
* 概述: 组件接口,其中DoF是实体必须拥有的组件,实体的其他类型的组件可有可无
* https://github.com/shamozhishu
* Copyright (c) 2020-2020 authored by shamozhishu
\************************************************************************/
#ifndef __WAR_COMPONENTS_H__
#define __WAR_COMPONENTS_H__

#include <zoo/Component.h>

using namespace zoo;

struct Behavior : public Component {
	string _scriptFile;
	bool _scriptInited;
private:
	bool _scriptValid;
	class Battlefield* _btfield;
	class LuaScriptStack* _script;
	ZOO_REFLEX_DECLARE(Behavior)

public:
	Behavior();
	~Behavior();
	void exec(const char* func);
	void reset();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);
};

struct DoF : public Component { // tolua_export
	enum {
		Dof_ = ESTATE_01,
		Parent_ = ESTATE_02
	};

	union
	{
		struct
		{
			double x, y, z;
		} _pos;
		struct
		{
			double lon, lat, alt;
		} _lla;
	};
	
	float _sx, _sy, _sz;
	float _heading, _pitch, _roll;

	bool _isLLA;
	DoF* _parent;
	int _mountEntID;
	int _mountEntBreed;
	const int _sceneType; // [0:地形场景;1:地球场景;2:地图场景]
	vector<DoF*> _children;
	ZOO_REFLEX_DECLARE(DoF)

public:
	DoF();
	~DoF();
	void init();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

public:
	// tolua_begin
	bool isLLA() const;
	void setLLA(bool lla);
	void llaTo(double lon, double lat, double alt);
	void setLon(double lon);
	void setLat(double lat);
	void setAlt(double alt);
	double getLon() const;
	double getLat() const;
	double getAlt() const;
	void moveTo(double x, double y, double z);
	void moveBy(double x, double y, double z);
	void setPosX(double x);
	void setPosY(double y);
	void setPosZ(double z);
	double getPosX() const;
	double getPosY() const;
	double getPosZ() const;
	void rotateTo(float h, float p, float r);
	void rotateBy(float h, float p, float r);
	void setHeading(float h);
	void setPitch(float p);
	void setRoll(float r);
	float getHeading() const;
	float getPitch() const;
	float getRoll() const;
	void scaleTo(float sx, float sy, float sz);
	void scaleBy(float sx, float sy, float sz);
	void setScaleX(float sx);
	void setScaleY(float sy);
	void setScaleZ(float sz);
	float getScaleX() const;
	float getScaleY() const;
	float getScaleZ() const;
	void setParent(DoF* parent);
	DoF* getParent() const;
};
// tolua_end

struct Mesh : public Serializer { // tolua_export
	enum {
		Changed_ = ESTATE_30
	};

	bool _enableResource;
	string _resourceFile;
	string _currentUseMeshName;
	map<string, vector<double>> _params;
	PROPERTY_RW(Component*, _parent, Parent)

public:
	Mesh();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

public:
	// tolua_begin
	void changeMesh(const string& name);
	void changeParam(const string& var, const vector<double>& val);
};
// tolua_end

struct Material : public Serializer { // tolua_export
	enum {
		Changed_ = ESTATE_31,
		Uniform_ = ESTATE_32
	};

	string _currentUseMatName;
	map<string, vector<double>> _uniforms;
	static const int TexUnitNum = 8;
	std::pair<string, string> _textureFiles[TexUnitNum];
	PROPERTY_RW(Component*, _parent, Parent)

public:
	Material();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

public:
	// tolua_begin
	void changeMat(const string& name);
	void changeUniform(const string& uniform, const vector<double>& val);
};
// tolua_end

struct Model : public Component { // tolua_export
	enum {
		Visible_ = ESTATE_01
	};

	bool _visible;
	Mesh _mesh;
	Material _material;
	ZOO_REFLEX_DECLARE(Model)

public:
	Model();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

public:
	// tolua_begin
	bool isVisible() const;
	void setVisible(bool visible);
	Mesh* getMesh();
	Material* getMaterial();
};
// tolua_end

struct Sound : public Component { // tolua_export
	bool _isPlay;
	bool _isLoop;
	string _soundFile;
	ZOO_REFLEX_DECLARE(Sound)

public:
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

public:
	// tolua_begin
	void play();
	void stop();
	void pause();
	bool isLoop() const;
	void setLoop(bool loop);
};
// tolua_end

struct Animator : public Component { // tolua_export
	string _trajFile;
	ZOO_REFLEX_DECLARE(Animator)

public:
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

public:
	// tolua_begin
};
// tolua_end

struct Collider : public Component { // tolua_export

public:
	// tolua_begin
};
// tolua_end

struct Camera : public Component { // tolua_export
	enum {
		Bgcolour_ = ESTATE_01,
		Viewport_ = ESTATE_02,
		TrackEnt_ = ESTATE_03,
		Manipulator_ = ESTATE_04,
		Pass1_ = ESTATE_05,
		Pass2_ = ESTATE_06,
		Pass3_ = ESTATE_07,
		Pass4_ = ESTATE_08,
		Pass5_ = ESTATE_09,
		Pass6_ = ESTATE_10,
		Pass7_ = ESTATE_11,
		Pass8_ = ESTATE_12
	};
	enum {
		Earth_ = 0,
		Terrain_ = 0,
		NodeTracker_,
		Trackball_,
		Flight_,
		Drive_,
		Orbit_,
		FirstPerson_,
		Spherical_,
		Custom_ // >=8表示指定了自定义的摄像机操纵器
	};
	// tolua_begin
	enum RenderTarget {
		Nothing_ = 0,
		Window_,
		TextureColor_,
		TextureDepth_,
		HeadUpDisplay_
	};
	typedef unsigned int PassIndex;
	// tolua_end
	struct Pass
	{
		RenderTarget _rt;
		Material _material;
		Pass() : _rt(Nothing_) {}
	};
	const int _viewID;
	const int _windowID;
	int _manipulatorKey;
	int _trackEntID, _trackEntBreed;
	int _red, _green, _blue, _alpha;
	float _lRatio, _rRatio, _bRatio, _tRatio;
	static const PassIndex MaxPassCount = 8;
	Pass _passes[MaxPassCount]; // 最多支持八通道

private:
	ZOO_REFLEX_DECLARE(Camera)
	static Reflex<Camera, int> _dynReflex_INT;

public:
	Camera();
	Camera(int windowID);
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

public:
	// tolua_begin
	void setManipulator(int key);
	Material* getMaterial(PassIndex pass);
	void setTrackEntity(int id, int breed);
	void setBgColor(int r, int g, int b, int a = 255);
	void setRenderTarget(PassIndex pass, RenderTarget rt);
	void setViewport(float leftRatio, float rightRatio, float bottomRatio, float topRatio);
};
// tolua_end

struct Environment : public Component { // tolua_export
	enum {
		SkyBox_ = ESTATE_01,
		Weather_ = ESTATE_02
	};
	// tolua_begin
	enum Weather { Sunny_, Rain_, Snow_ };
	// tolua_end
	Weather _type;
	float _intensity;
	Material _skyBox;
	ZOO_REFLEX_DECLARE(Environment)

public:
	Environment();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

public:
	// tolua_begin
	void showSkyBox();
	void hideSkyBox();
	void setWeather(Weather type, float intensity);
};
// tolua_end

struct Earth : public Component { // tolua_export
	enum {
		SunVisible_ = ESTATE_01,
		MoonVisible_ = ESTATE_02,
		StarVisible_ = ESTATE_03,
		NebulaVisible_ = ESTATE_04,
		AtmosphereVisible_ = ESTATE_05,
		SunlightIntensity_ = ESTATE_06
	};
	enum Sky { sun_, moon_, star_, nebula_, atmosphere_, count_ };
	string _earthFile;
	float _sunlightIntensity;
	bool _skyVisibility[count_];
	ZOO_REFLEX_DECLARE(Earth)

public:
	Earth();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

public:
	// tolua_begin
	void setSunVisible(bool visible);
	void setMoonVisible(bool visible);
	void setStarVisible(bool visible);
	void setNebulaVisible(bool visible);
	void setAtmosphereVisible(bool visible);
	void setSunlightIntensity(float intensity);
};
// tolua_end

struct AI : public Component {
	AI();
	~AI();
};

#endif // __WAR_COMPONENTS_H__
