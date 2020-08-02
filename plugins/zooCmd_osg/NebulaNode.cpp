#include "NebulaNode.h"
#include <osg/Geode>
#include <osg/Camera>
#include <osg/Geometry>
#include <osg/Depth>
#include <osg/CullFace>
#include <osg/FrontFace>
#include <osg/Texture2D>
#include <osgEarth/Utils>
#include <osgEarth/StringUtils>
#include <osgEarth/CullingUtils>

class Nebula : public osg::MatrixTransform
{
public:
	Nebula(float fNebulaRadius);
	bool SetInterfaceImage(unsigned int unit, const std::string& strImage);

protected:
	virtual ~Nebula() {}
	virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const;
	virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const;

protected:
	osg::observer_ptr<osg::Geode> m_pGeodeBox;
};

// 下面是对星空背景的具体实现
#define BIN_NEBULA (-100004)
static const double dDegreePI = 180.0;
static const double dDegreePI_2 = 90.0;
static const double dDegree2_PI = 360.0;

static osg::Geometry* makeSphereGeometry(double dRadius, osg::Vec4 vecDefaultColor)
{
	int nLatSegments = 20;
	int nLonSegments = 2 * nLatSegments;
	double dSegmentSize = dDegreePI / (double)nLatSegments; // degrees

	osg::Geometry* pGeome = new osg::Geometry();
	pGeome->setUseVertexBufferObjects(true);

	osg::Vec2Array* pTexCoords = new osg::Vec2Array();
	pGeome->setTexCoordArray(0, pTexCoords);

	osg::Vec3Array* pNormals = new osg::Vec3Array();
	pGeome->setNormalArray(pNormals);
	pGeome->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	osg::DrawElementsUShort* pEL = new osg::DrawElementsUShort(GL_TRIANGLES);
	osg::Vec3Array* pVertex = new osg::Vec3Array();
	pGeome->setVertexArray(pVertex);
	pGeome->addPrimitiveSet(pEL);

	for (int nLat = 0; nLat <= nLatSegments; ++nLat)
	{
		double dLat = -dDegreePI_2 + dSegmentSize * (double)nLat;
		for (int nLon = 0; nLon <= nLonSegments; ++nLon)
		{
			double dLon = -dDegreePI + dSegmentSize * (double)nLon;
			double dSinLatitude = sin(osg::DegreesToRadians(dLat));
			double dCosLatitude = cos(osg::DegreesToRadians(dLat));

			double dGx = dRadius * dCosLatitude * cos(osg::DegreesToRadians(dLon));
			double dGy = dRadius * dCosLatitude * sin(osg::DegreesToRadians(dLon));
			double dGz = dRadius * dSinLatitude;
			pVertex->push_back(osg::Vec3(dGx, dGy, dGz));

			double dS = (dLon + dDegreePI) / dDegree2_PI;
			double dT = (dLat + dDegreePI_2) / dDegreePI;
			pTexCoords->push_back(osg::Vec2(dS, dT));


			osg::Vec3 vecNormal(dGx, dGy, dGz);
			vecNormal.normalize();
			pNormals->push_back(vecNormal);

			if ((nLat < nLatSegments) && (nLon < nLonSegments))
			{
				int nXPlus = nLon + 1;
				int nYPlus = nLat + 1;
				pEL->push_back(nLat * (nLonSegments + 1) + nLon);
				pEL->push_back(nYPlus * (nLonSegments + 1) + nLon);
				pEL->push_back(nLat * (nLonSegments + 1) + nXPlus);
				pEL->push_back(nLat * (nLonSegments + 1) + nXPlus);
				pEL->push_back(nYPlus * (nLonSegments + 1) + nLon);
				pEL->push_back(nYPlus * (nLonSegments + 1) + nXPlus);
			}
		}
	}

	return pGeome;
}

Nebula::Nebula(float fNebulaRadius)
{
	osg::Vec3 vec3Pos(0.0f, 0.0f, 0.0f);
	osg::Vec4 vec4Black(1.0, 1.0, 1.0, 1.0);

	osg::Geometry* pGeomBox = makeSphereGeometry(fNebulaRadius, vec4Black);

	osg::StateSet* pNebulaStateSet = pGeomBox->getOrCreateStateSet();
	pNebulaStateSet->setRenderBinDetails(BIN_NEBULA, "RenderBin");
	pNebulaStateSet->setAttributeAndModes(
		new osg::Depth(osg::Depth::ALWAYS, 0, 1, false),
		osg::StateAttribute::ON);
	pNebulaStateSet->setMode(GL_BLEND, 1);

	osg::Geode* pGeodeBox = new osg::Geode;
	pGeodeBox->addDrawable(pGeomBox);
	addChild(pGeodeBox);
	m_pGeodeBox = pGeodeBox;

	setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	setCullingActive(false);

	osg::StateSet* pStateSet = getOrCreateStateSet();
	pStateSet->setAttributeAndModes(new osg::Depth(
		osg::Depth::LEQUAL, 1.0f, 1.0f));
	pStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	pStateSet->setMode(GL_CULL_FACE, osg::StateAttribute::OFF);
	pStateSet->setRenderBinDetails(BIN_NEBULA, "RenderBin");
}

bool Nebula::SetInterfaceImage(unsigned int unit, const std::string& strImage)
{
	osg::Image* pImage = osgDB::readImageFile(strImage);
	if (pImage)
	{
		osg::Texture2D* pTexture = new osg::Texture2D(pImage);

		pTexture->setFilter(osg::Texture::MIN_FILTER, osg::Texture::NEAREST);
		pTexture->setFilter(osg::Texture::MAG_FILTER, osg::Texture::NEAREST);
		pTexture->setWrap(osg::Texture::WRAP_R, osg::Texture::REPEAT);
		pTexture->setResizeNonPowerOfTwoHint(false);

		m_pGeodeBox->getOrCreateStateSet()->setTextureAttributeAndModes(
			0, pTexture,
			osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);

		return true;
	}

	return false;
}

bool Nebula::computeLocalToWorldMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const
{
	if (nv && nv->getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
	{
		osgUtil::CullVisitor* pCV = static_cast<osgUtil::CullVisitor*>(nv);
		matrix.preMult(osg::Matrix::translate(pCV->getEyeLocal()));
		return true;
	}

	return osg::Transform::computeLocalToWorldMatrix(matrix, nv);
}

bool Nebula::computeWorldToLocalMatrix(osg::Matrix& matrix, osg::NodeVisitor* nv) const
{
	if (nv && nv->getVisitorType() == osg::NodeVisitor::CULL_VISITOR)
	{
		osgUtil::CullVisitor* pCV = static_cast<osgUtil::CullVisitor*>(nv);
		matrix.postMult(osg::Matrix::translate(-pCV->getEyeLocal()));
		return true;
	}

	return osg::Transform::computeWorldToLocalMatrix(matrix, nv);
}

NebulaNode::NebulaNode(osgEarth::MapNode* pMapNode, const std::string& strImageFile)
	: m_rpNebulaXform(NULL)
	, m_strNebulaImage(strImageFile)
	, m_bNebulaVisible(true)
{
	Initialize(pMapNode);
}

NebulaNode::~NebulaNode()
{
}

void NebulaNode::Initialize(osgEarth::MapNode* pMapNode)
{
	if (pMapNode)
	{
		double dInnerRadius = pMapNode->getMapSRS()->getGeodeticSRS()->getEllipsoid()->getRadiusPolar();
		m_fNebulaRadius = 12000 * dInnerRadius;

		Nebula* pNebulaNode = new Nebula(m_fNebulaRadius);

		if (pNebulaNode->SetInterfaceImage(0, m_strNebulaImage))
		{
			m_rpNebulaXform = new osg::MatrixTransform;

			osg::Camera* pCameraNebula = new osg::Camera();
			pCameraNebula->getOrCreateStateSet()->setRenderBinDetails(BIN_NEBULA, "RenderBin");
			pCameraNebula->setRenderOrder(osg::Camera::NESTED_RENDER);
			pCameraNebula->setComputeNearFarMode(osg::CullSettings::COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
			pCameraNebula->addChild(pNebulaNode);
			m_rpNebulaXform->addChild(pCameraNebula);
		}
	}
}

void NebulaNode::traverse(osg::NodeVisitor& nv)
{
	osgUtil::CullVisitor* pCullVisitor = osgEarth::Culling::asCullVisitor(nv);
	if (pCullVisitor)
	{
		osg::ref_ptr<osg::CullSettings::ClampProjectionMatrixCallback> rpCMC
			= pCullVisitor->getClampProjectionMatrixCallback();

		// lh 2019-4-28改，修改
		// 原 pCullVisitor->setClampProjectionMatrixCallback( 0L );
		osg::CullSettings::InheritanceMaskActionOnAttributeSetting  inheritanceMask;
		if (rpCMC.valid())
		{
			inheritanceMask = pCullVisitor->getInheritanceMaskActionOnAttributeSetting();
			pCullVisitor->setInheritanceMaskActionOnAttributeSetting(osg::CullSettings::DO_NOT_MODIFY_INHERITANCE_MASK);
			pCullVisitor->setClampProjectionMatrixCallback(0L);
		}

		if (m_bNebulaVisible && m_rpNebulaXform.valid())
		{
			m_rpNebulaXform->accept(nv);
		}

		// lh 2019-4-28改，修改
		// 原 pCullVisitor->setClampProjectionMatrixCallback( rpCMC.get() );
		if (rpCMC.valid())
		{
			pCullVisitor->setClampProjectionMatrixCallback(rpCMC.get());
			pCullVisitor->setInheritanceMaskActionOnAttributeSetting(inheritanceMask);
		}
	}
	else
	{
		osg::Group::traverse(nv);
	}
}

void NebulaNode::SetNebulaVisible(bool bShow)
{
	m_bNebulaVisible = bShow;
}

bool NebulaNode::GetNebulaVisible() const
{
	return m_bNebulaVisible;
}

void NebulaNode::SetDateTime(const osgEarth::DateTime& dateTime)
{
	m_dateTime = dateTime;
	OnSetDateTime();
}
const osgEarth::DateTime& NebulaNode::GetDateTime() const
{
	return m_dateTime;
}

void NebulaNode::OnSetDateTime()
{
	double time_r = m_dateTime.hours() / 24.0; // 0..1
	double rot_z = -osg::PI + osg::PI * 2.0 * time_r;

	if (m_rpNebulaXform.valid())
	{
		m_rpNebulaXform->setMatrix(osg::Matrixd::rotate(-rot_z, 0, 0, 1));
	}
}
