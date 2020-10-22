%{
#include <stdio.h>
#include "MaterialScript.h"
#define YYDEBUG 1

typedef enum
{
	UNKNOWN_ = 0,
	MATERIAL_,
	TECHNIQUE_,
	PASS_,
	TEXTURE_UNIT_,
	BLOCK_COUNT_
} EBlockType;

static int s_curBlockIdx = UNKNOWN_;
static int s_curTextureUnit = -1;

static EBlockType s_curBlockStack[BLOCK_COUNT_];

static MaterialScript* s_curMaterialScript = NULL;
static osg::ref_ptr<osg::StateSet> s_curPass;
static osg::ref_ptr<MaterialEffect> s_curEffect;
static osg::ref_ptr<MaterialTechnique> s_curTechnique;

static void resetCompileContext()
{
	s_curBlockIdx = UNKNOWN_;
	s_curTextureUnit = -1;
	s_curPass = NULL;
	s_curEffect = NULL;
	s_curTechnique = NULL;
	s_curMaterialScript = NULL;
	extern void reset_string_literal_buffer(void);
	reset_string_literal_buffer();
	extern FILE *yyin;
	if (yyin)
	{
		fseek(yyin, 0u, SEEK_SET);
		fclose(yyin);
		yyin = NULL;
	}
}

string compileMaterialScript(const string& materialFile)
{
	extern FILE *yyin;
	extern int yyparse(void);
	yyin = fopen(materialFile.c_str(), "r");;
	if (!yyin || yyparse())
	{
		ServiceLocator<OsgMaterialManager>().getService()->removeMaterial(s_curMaterialScript);
		zoo_error("Failed to compile material script file \"%s\"!", materialFile.c_str());
		resetCompileContext();
		return "Default";
	}

	string material_name = s_curMaterialScript->getMaterialName();
	resetCompileContext();
	return material_name;
}

static MaterialEffect* getEffect()
{
	if (!s_curEffect.valid())
	{
		s_curEffect = new MaterialEffect;
		s_curMaterialScript->setMaterialEffect(s_curEffect);
	}
	return s_curEffect;
}

static osg::Material* getMaterial()
{
	osg::Material* material = dynamic_cast<osg::Material*>(s_curPass->getAttribute(osg::StateAttribute::MATERIAL));
	if (!material)
	{
		material = new osg::Material;
		s_curPass->setAttributeAndModes(material, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	}
	return material;
}

int yyerror(char const *str)
{
	extern char *yytext;
	fprintf(stderr, "parser error near %s\n", yytext);
	return 0;
}

extern int yylex(void);
%}
%union {
	char        *identifier;
	double       double_value;
};
%token <identifier> IDENTIFIER
%token <identifier> STRING_LITERAL
%token <double_value> DOUBLE_LITERAL
%token LC RC
%token MATERIAL TECHNIQUE PASS TEXTURE_UNIT
%token AMBIENT DIFFUSE SPECULAR EMISSIVE
%token TEXTURE
%%
translation_unit
	: statement
	| translation_unit statement
	;
statement
	: RC
	{
		switch (s_curBlockStack[s_curBlockIdx--])
		{
		case MATERIAL_: s_curEffect = nullptr; break;
		case TECHNIQUE_: s_curTechnique = nullptr; break;
		case PASS_: s_curPass = nullptr; s_curTextureUnit = -1; break;
		}
	}
	| MATERIAL STRING_LITERAL LC
	{
		s_curBlockStack[++s_curBlockIdx] = MATERIAL_;
		s_curMaterialScript = new MaterialScript($2);
	}
	| TECHNIQUE STRING_LITERAL LC
	{
		s_curBlockStack[++s_curBlockIdx] = TECHNIQUE_;
		s_curTechnique = new MaterialTechnique($2);
		getEffect()->pushTechnique(s_curTechnique);
	}
	| TECHNIQUE LC
	{
		s_curBlockStack[++s_curBlockIdx] = TECHNIQUE_;
		s_curTechnique = new MaterialTechnique;
		getEffect()->pushTechnique(s_curTechnique);
	}
	| PASS LC
	{
		s_curBlockStack[++s_curBlockIdx] = PASS_;
		s_curPass = new osg::StateSet;
		s_curTechnique->pushPass(s_curPass);
	}
	| TEXTURE_UNIT LC
	{
		s_curBlockStack[++s_curBlockIdx] = TEXTURE_UNIT_;
		s_curTextureUnit++;
	}
	| AMBIENT DOUBLE_LITERAL DOUBLE_LITERAL DOUBLE_LITERAL DOUBLE_LITERAL
	{
		getMaterial()->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4($2, $3, $4, $5));
	}
	| DIFFUSE DOUBLE_LITERAL DOUBLE_LITERAL DOUBLE_LITERAL DOUBLE_LITERAL
	{
		getMaterial()->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4($2, $3, $4, $5));
	}
	| SPECULAR DOUBLE_LITERAL DOUBLE_LITERAL DOUBLE_LITERAL DOUBLE_LITERAL
	{
		getMaterial()->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4($2, $3, $4, $5));
	}
	| EMISSIVE DOUBLE_LITERAL DOUBLE_LITERAL DOUBLE_LITERAL DOUBLE_LITERAL
	{
		getMaterial()->setEmission(osg::Material::FRONT_AND_BACK, osg::Vec4($2, $3, $4, $5));
	}
	| TEXTURE STRING_LITERAL
	{
		osg::Image* img = osgDB::readImageFile(ZOO_DATA_ROOT_DIR + $2);
		osg::StateAttribute::GLModeValue value = osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE;
		s_curPass->setTextureAttributeAndModes(s_curTextureUnit, new osg::Texture2D(img), value);
	}
	;
%%
