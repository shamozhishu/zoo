#ifndef __WAR_COMPONENTS_H__
#define __WAR_COMPONENTS_H__

#include <zoo/Component.h>

using namespace zoo;

struct Behavior : public Component {
	string _scriptFile;
	bool _scriptInited;
private:
	bool _scriptValid;
	class LuaScript* _script;
	ZOO_REFLEX_DECLARE(Behavior)

public:
	Behavior();
	~Behavior();
	void exec();
	void reset();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);
};

struct DoF : public Component { // tolua_export
	enum
	{
		Dof_ = ESTATE_01,
		Parent_ = ESTATE_02
	};

	double _x, _y, _z;
	float _sx, _sy, _sz;
	float _heading, _pitch, _roll;
	DoF* _parent;
	int _mountEntID;
	int _mountEntBreed;
	bool _lonLatHeight;
	vector<DoF*> _children;
	ZOO_REFLEX_DECLARE(DoF)

public:
	DoF();
	~DoF();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

public:
	// tolua_begin
	bool isLLH() const;
	void setPos(double x, double y, double z, bool lon_lat_height = false);
	double getPosX() const;
	double getPosY() const;
	double getPosZ() const;
	void setRot(float h, float p, float r);
	float getHeading() const;
	float getPitch() const;
	float getRoll() const;
	void setScale(float sx, float sy, float sz);
	float getScaleX() const;
	float getScaleY() const;
	float getScaleZ() const;
	void setParent(DoF* parent);
	DoF* getParent() const;
};
// tolua_end

struct Model : public Component { // tolua_export
	enum
	{
		Visible_ = ESTATE_01,
		ModelFile_ = ESTATE_02
	};

	bool _visible;
	string _modelFile;
	ZOO_REFLEX_DECLARE(Model)

public:
	Model();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

public:
	// tolua_begin
	bool isVisible() const;
	void setVisible(bool visible);
	void setModelFile(const string& modelFile);
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
	enum
	{
		Bgcolour_ = ESTATE_01,
		Viewport_ = ESTATE_02,
		TrackEnt_ = ESTATE_03,
		Manipulator_ = ESTATE_04
	};

	enum
	{
		Earth_ = 0,
		NodeTracker_,
		Trackball_,
		Flight_,
		Drive_,
		Terrain_,
		Orbit_,
		FirstPerson_,
		Spherical_,
		Custom_
	};

	int _manipulatorKey;
	int _trackEntID, _trackEntBreed;
	int _red, _green, _blue, _alpha;
	float _lRatio, _rRatio, _bRatio, _tRatio;
	ZOO_REFLEX_DECLARE(Camera)

public:
	Camera();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

public:
	// tolua_begin
	void setManipulator(int key);
	void setTrackEnt(int id, int breed);
	void setBgColor(int r, int g, int b, int a = 255);
	void setViewport(float leftRatio, float rightRatio, float bottomRatio, float topRatio);
};
// tolua_end

struct Environment : public Component { // tolua_export
	enum
	{
		Weather_ = ESTATE_01
	};
	// tolua_begin
	enum EWeather { Sunny_, Rain_, Snow_ };
	// tolua_end
	EWeather _type;
	float _intensity;
	ZOO_REFLEX_DECLARE(Environment)

public:
	Environment();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

public:
	// tolua_begin
	void setWeather(EWeather type, float intensity);
};
// tolua_end

struct Earth : public Component { // tolua_export
	enum
	{
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
