#include "BuiltinMesh.h"
#include <zoo/Utils.h>
#include <osg/ShapeDrawable>

using namespace osg;

SphereMesh::SphereMesh()
	: _radius(1)
	, _center(0, 0, 0)
{
	ServiceLocator<OsgMeshManager>().getService()->addMesh(this);
}

std::string SphereMesh::getMeshName() const
{
	return "Sphere";
}

void SphereMesh::getMeshConfigInfo(Mesh* mesh)
{
	vector<double> val;
	val.push_back(_center.x());
	val.push_back(_center.y());
	val.push_back(_center.z());
	mesh->_params["center"] = val;
	val.clear();
	val.push_back(_radius);
	mesh->_params["radius"] = val;
}

Geode* SphereMesh::setupMeshModel(Mesh* mesh)
{
	vector<double> val = mesh->_params["center"];
	_center.set(val[0], val[1], val[2]);
	val = mesh->_params["radius"];
	_radius = val[0];

	ref_ptr<Geode> geode = new Geode;
	ref_ptr<Sphere> sphere = new osg::Sphere(_center, _radius);
	ref_ptr<ShapeDrawable> shape = new ShapeDrawable(sphere.get());
	geode->addDrawable(shape);
	return geode.release();
}
//////////////////////////////////////////////////////////////////////////
BoxMesh::BoxMesh()
	: _center(0, 0, 0)
	, _lengthX(1)
	, _lengthY(1)
	, _lengthZ(1)
{
	ServiceLocator<OsgMeshManager>().getService()->addMesh(this);
}

std::string BoxMesh::getMeshName() const
{
	return "Box";
}

void BoxMesh::getMeshConfigInfo(Mesh* mesh)
{
	vector<double> val;
	val.push_back(_center.x());
	val.push_back(_center.y());
	val.push_back(_center.z());
	mesh->_params["center"] = val;
	val.clear();
	val.push_back(_lengthX);
	mesh->_params["lengthX"] = val;
	val.clear();
	val.push_back(_lengthY);
	mesh->_params["lengthY"] = val;
	val.clear();
	val.push_back(_lengthZ);
	mesh->_params["lengthZ"] = val;
}

osg::Geode* BoxMesh::setupMeshModel(Mesh* mesh)
{
	vector<double> val = mesh->_params["center"];
	_center.set(val[0], val[1], val[2]);
	val = mesh->_params["lengthX"];
	_lengthX = val[0];
	val = mesh->_params["lengthY"];
	_lengthY = val[0];
	val = mesh->_params["lengthZ"];
	_lengthZ = val[0];

	ref_ptr<Geode> geode = new Geode;
	ref_ptr<Box> box = new osg::Box(_center, _lengthX, _lengthY, _lengthZ);
	ref_ptr<ShapeDrawable> shape = new ShapeDrawable(box.get());
	geode->addDrawable(shape);
	return geode.release();
}
