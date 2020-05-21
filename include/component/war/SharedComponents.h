#pragma once

#include <zoo/Component.h>

class Serializer
{
public:
	virtual void serialize(stringstream& ss) = 0;
	virtual void deserialize(zoo::TableCSV* pTable) = 0;
};

class DoF : public zoo::Component, public Serializer { // tolua_export
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
	vector<DoF*> _children;
	ZOO_REFLEX_DECLARE(DoF)
	ZOO_COMPONENT_IMPL(DoF)

public:
	DoF();
	~DoF();
	void serialize(stringstream& ss);
	void deserialize(zoo::TableCSV* pTable);

	enum
	{
		dof_ = zoo::ESTATE_01,
		parent_ = zoo::ESTATE_02
	};

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

class Model : public zoo::Component, public Serializer { // tolua_export
	bool _visible;
	string _modelFile;
	ZOO_REFLEX_DECLARE(Model)
	ZOO_COMPONENT_IMPL(Model)

public:
	Model();
	void serialize(stringstream& ss);
	void deserialize(zoo::TableCSV* pTable);

	enum
	{
		visible_ = zoo::ESTATE_01,
		modelFile_ = zoo::ESTATE_02
	};

public:
	// tolua_begin
	bool isVisible() const;
	void setVisible(bool visible);
	void setModelFile(const string& modelFile);
};
// tolua_end

class Sound : public zoo::Component, public Serializer { // tolua_export
	bool _isPlay;
	bool _isLoop;
	string _soundFile;
	ZOO_REFLEX_DECLARE(Sound)
	ZOO_COMPONENT_IMPL(Sound)

public:
	void serialize(stringstream& ss);
	void deserialize(zoo::TableCSV* pTable);

public:
	// tolua_begin
	void play();
	void stop();
	void pause();
	bool isLoop() const;
	void setLoop(bool loop);
};
// tolua_end

class Animator : public zoo::Component, public Serializer { // tolua_export
	string _trajFile;
	ZOO_REFLEX_DECLARE(Animator)
	ZOO_COMPONENT_IMPL(Animator)

public:
	void serialize(stringstream& ss);
	void deserialize(zoo::TableCSV* pTable);

public:
	// tolua_begin
};
// tolua_end
