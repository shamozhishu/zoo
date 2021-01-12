#include "BuiltinMesh.h"
#include <zoo/Utils.h>
#include <osg/Geometry>
#include <osg/ShapeDrawable>
#include <osg/Billboard>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

using namespace osg;

DefaultMesh::DefaultMesh()
{
	ServiceLocator<OsgMeshManager>().getService()->addMesh(this);
}

std::string DefaultMesh::getMeshName() const
{
	return "Default";
}

void DefaultMesh::getMeshConfigInfo(Mesh* mesh)
{
	mesh->_enableResource = true;
	mesh->_resourceFile = _resourceFile;
}

osg::Node* DefaultMesh::setupMeshModel(Mesh* mesh)
{
	osg::ref_ptr<osg::Node> node = osgDB::readNodeFile(ZOO_DATA_ROOT_DIR + mesh->_resourceFile);
	if (node.valid())
		return node.release();
	zoo_warning("Read resource file [%s] failed!", mesh->_resourceFile.c_str());
	return nullptr;
}
//////////////////////////////////////////////////////////////////////////
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
	mesh->_enableResource = false;
	vector<double> val;
	val.push_back(_center.x());
	val.push_back(_center.y());
	val.push_back(_center.z());
	mesh->_params["center"] = val;
	val.clear();
	val.push_back(_radius);
	mesh->_params["radius"] = val;
}

Node* SphereMesh::setupMeshModel(Mesh* mesh)
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
	mesh->_enableResource = false;
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

osg::Node* BoxMesh::setupMeshModel(Mesh* mesh)
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
//////////////////////////////////////////////////////////////////////////
BillboardMesh::BillboardMesh()
	: _resourceFile("material/texture/tree.rgba")
{
	ServiceLocator<OsgMeshManager>().getService()->addMesh(this);
}

std::string BillboardMesh::getMeshName() const
{
	return "Billboard";
}

void BillboardMesh::getMeshConfigInfo(Mesh* mesh)
{
	mesh->_enableResource = true;
	mesh->_resourceFile = _resourceFile;
}

osg::Node* BillboardMesh::setupMeshModel(Mesh* mesh)
{
	ref_ptr<Billboard> treeGeode = new osg::Billboard;
	treeGeode->setMode(osg::Billboard::POINT_ROT_EYE);
	treeGeode->getOrCreateStateSet()->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	treeGeode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ref_ptr<osg::Texture2D> texture = new osg::Texture2D(osgDB::readImageFile(ZOO_DATA_ROOT_DIR + _resourceFile));
	if (texture.valid())
	{
		ref_ptr<osg::Drawable> tree = osg::createTexturedQuadGeometry(osg::Vec3(0, 0, 0), osg::Vec3(1, 0, 0), osg::Vec3(0, 0, 1));
		tree->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture);
		treeGeode->addDrawable(tree);
		return treeGeode.release();
	}

	return nullptr;
}
