#ifndef __SHARED_COMPONENTS_H__
#define __SHARED_COMPONENTS_H__

#include <zoo/Component.h>

using namespace zoo;

class DoF : public Component { // tolua_export
public:
	enum
	{
		dof_ = ESTATE_01,
		parent_ = ESTATE_02
	};

	double _x;
	double _y;
	double _z;
	float _heading;
	float _pitch;
	float _roll;
	float _sx;
	float _sy;
	float _sz;
	DoF* _parent;
	int _mountEntID;
	int _mountEntBreed;
	vector<DoF*> _children;
	ZOO_REFLEX_DECLARE(DoF)

public:
	DoF();
	~DoF();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

private:
	void onSetParent(const UserData& userdata);

public:
	// tolua_begin
	void setPosX(double x);
	double getPosX() const;
	void setPosY(double y);
	double getPosY() const;
	void setPosZ(double z);
	double getPosZ() const;
	void setHeading(double h);
	float getHeading() const;
	void setPitch(double p);
	float getPitch() const;
	void setRoll(double r);
	float getRoll() const;
	void setScaleX(double x);
	float getScaleX() const;
	void setScaleY(double y);
	float getScaleY() const;
	void setScaleZ(double z);
	float getScaleZ() const;
	void setParent(DoF* parent);
	DoF* getParent() const;
};
// tolua_end

class Model : public Component { // tolua_export
public:
	enum
	{
		visible_ = ESTATE_01,
		modelFile_ = ESTATE_02
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

class Sound : public Component { // tolua_export
public:
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

class Animator : public Component { // tolua_export
public:
	string _trajFile;
	ZOO_REFLEX_DECLARE(Animator)

public:
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

public:
	// tolua_begin
};
// tolua_end

class Collider : public Component { // tolua_export


public:
	// tolua_begin
};
// tolua_end

class Camera : public Component { // tolua_export
public:
	enum
	{
		trackEnt_ = ESTATE_01,
		bgcolour_ = ESTATE_02,
		viewport_ = ESTATE_03
	};

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
	void setTrackEnt(int id, int breed);
	void setBgColor(int r, int g, int b, int a = 255);
	void setViewport(float leftRatio, float rightRatio, float bottomRatio, float topRatio);
};
// tolua_end

class Environment : public Component { // tolua_export
	ZOO_REFLEX_DECLARE(Environment)

public:
	Environment();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

public:
	// tolua_begin
};
// tolua_end

class Earth : public Component { // tolua_export
public:
	string _earthFile;
	ZOO_REFLEX_DECLARE(Earth)

public:
	Earth();
	void serialize(Spawner* spawner);
	void deserialize(Spawner* spawner);

public:
	// tolua_begin
	void setEarthFile(const string& earthFile);
};
// tolua_end

#endif // __SHARED_COMPONENTS_H__
