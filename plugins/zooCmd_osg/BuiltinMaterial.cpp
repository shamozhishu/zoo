#include "BuiltinMaterial.h"
#include <zoo/Utils.h>
#include <osgUtil/TangentSpaceGenerator>

using namespace zooCmd;

//////////////////////////////////////////////////////////////////////////
// shaders

static const char* vertSource = {
	"attribute vec3 tangent;\n"
	"attribute vec3 binormal;\n"
	"varying vec3 lightDir;\n"
	"void main()\n"
	"{\n"
	"    vec3 normal = normalize(gl_NormalMatrix * gl_Normal);\n"
	"    mat3 rotation = mat3(tangent, binormal, normal);\n"
	"    vec4 vertexInEye = gl_ModelViewMatrix * gl_Vertex;\n"
	"    lightDir = vec3(gl_LightSource[0].position.xyz - vertexInEye.xyz);\n"
	"    lightDir = normalize(rotation * normalize(lightDir));\n"
	"    gl_Position = ftransform();\n"
	"    gl_TexCoord[0] = gl_MultiTexCoord0;\n"
	"}\n"
};

static const char* fragSource = {
	"uniform sampler2D colorTex;\n"
	"uniform sampler2D normalTex;\n"
	"varying vec3 lightDir;\n"
	"void main (void)\n"
	"{\n"
	"    vec4 base = texture2D(colorTex, gl_TexCoord[0].xy);\n"
	"    vec3 bump = texture2D(normalTex, gl_TexCoord[0].xy).xyz;\n"
	"    bump = normalize(bump * 2.0 - 1.0);\n"

	"    float lambert = max(dot(bump, lightDir), 0.0);\n"
	"    if (lambert > 0.0)\n"
	"    {\n"
	"        gl_FragColor = base * gl_LightSource[0].diffuse * lambert;\n"
	"        gl_FragColor += gl_LightSource[0].specular * pow(lambert, 2.0);\n"
	"    }\n"
	"    gl_FragColor += gl_LightSource[0].ambient;\n"
	"}\n"
};

//////////////////////////////////////////////////////////////////////////

class ComputeTangentVisitor : public osg::NodeVisitor
{
	bool _generate;
public:
	ComputeTangentVisitor(bool isGenerate) : _generate(isGenerate) {}
	void apply(osg::Node& node) { traverse(node); }

	void apply(osg::Geode& node)
	{
		for (unsigned int i = 0; i < node.getNumDrawables(); ++i)
		{
			osg::Geometry* geom = dynamic_cast<osg::Geometry*>(node.getDrawable(i));
			if (geom)
			{
				if (_generate)
					generateTangentArray(geom);
				else
					dissolveTangentArray(geom);
			}
		}
		traverse(node);
	}

	void generateTangentArray(osg::Geometry* geom)
	{
		osg::ref_ptr<osgUtil::TangentSpaceGenerator> tsg = new osgUtil::TangentSpaceGenerator;
		tsg->generate(geom);
		geom->setVertexAttribArray(6, tsg->getTangentArray());
		geom->setVertexAttribBinding(6, osg::Geometry::BIND_PER_VERTEX);
		geom->setVertexAttribArray(7, tsg->getBinormalArray());
		geom->setVertexAttribBinding(7, osg::Geometry::BIND_PER_VERTEX);
	}

	void dissolveTangentArray(osg::Geometry* geom)
	{
		geom->setVertexAttribArray(6, NULL);
		geom->setVertexAttribArray(7, NULL);
	}
};

BumpMapping::BumpMapping()
{
	ServiceLocator<OsgMaterialManager>().getService()->addMaterial(this);
}

string BumpMapping::getMaterialName() const
{
	return "Bump mapping";
}

bool BumpMapping::reform(osg::Node* node)
{
	ComputeTangentVisitor ctv(true);
	ctv.setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
	node->accept(ctv);
	return true;
}

void BumpMapping::restore(osg::Node* node)
{
	ComputeTangentVisitor ctv(false);
	ctv.setTraversalMode(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN);
	node->accept(ctv);
}

void BumpMapping::getMaterialConfigInfo(Material* material)
{
	material->_textureFiles[0] = make_pair("颜色纹理：", _colorImgPath);
	material->_textureFiles[1] = make_pair("法线纹理：", _normalImgPath);
}

void BumpMapping::setupStateSet(Material* material, osg::StateSet* stateset)
{
	_colorImgPath = material->_textureFiles[0].second;
	_normalImgPath = material->_textureFiles[1].second;

	osg::Image* colorImg = osgDB::readImageFile(ZOO_DATA_ROOT_DIR + _colorImgPath);
	if (!colorImg)
	{
		zoo_warning("材质[Bump mapping]：加载颜色纹理%s失败！", _colorImgPath.c_str());
		return;
	}

	osg::Image* normalImg = osgDB::readImageFile(ZOO_DATA_ROOT_DIR + _normalImgPath);
	if (!normalImg)
	{
		zoo_warning("材质[Bump mapping]：加载法线纹理%s失败！", _normalImgPath.c_str());
		return;
	}

	osg::ref_ptr<osg::Program> program = new osg::Program;
	program->addShader(new osg::Shader(osg::Shader::VERTEX, vertSource));
	program->addShader(new osg::Shader(osg::Shader::FRAGMENT, fragSource));
	program->addBindAttribLocation("tangent", 6);
	program->addBindAttribLocation("binormal", 7);

	osg::ref_ptr<osg::Texture2D> colorTex = new osg::Texture2D;
	colorTex->setImage(colorImg);

	osg::ref_ptr<osg::Texture2D> normalTex = new osg::Texture2D;
	normalTex->setImage(normalImg);

	stateset->addUniform(new osg::Uniform("colorTex", 0));
	stateset->addUniform(new osg::Uniform("normalTex", 1));
	stateset->setAttributeAndModes(program.get());

	osg::StateAttribute::GLModeValue value = osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE;
	stateset->setTextureAttributeAndModes(0, colorTex.get(), value);
	stateset->setTextureAttributeAndModes(1, normalTex.get(), value);
}
