

#ifndef _ioMaterialSerializer_h_
#define _ioMaterialSerializer_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioStringConverter.h"

class ioMaterial;
class ioTechnique;
class ioPass;
class ioTextureUnitState;
class ioShaderGroup;
class ioTextStream;

enum MaterialScriptSection
{
    MSS_NONE,
    MSS_MATERIAL,
    MSS_TECHNIQUE,
    MSS_PASS,
    MSS_TEXTUREUNIT,
	MSS_PROGRAM_REF,
	MSS_PROGRAM,
    MSS_DEFAULT_PARAMETERS,
};

struct MaterialScriptContext 
{
    MaterialScriptSection section;
	ioMaterial* material;
    ioTechnique* technique;
    ioPass* pass;
    ioTextureUnitState* textureUnit;

	char szScriptName[MAX_PATH];
	int iMaterialCnt;

    size_t lineNo;	// Error reporting state
};

typedef bool (*ATTRIBUTE_PARSER)( std::string& params, MaterialScriptContext& context);

class __EX ioMaterialSerializer
{
protected:
	typedef std::map< std::string, ATTRIBUTE_PARSER > AttribParserList;

	MaterialScriptContext m_ScriptContext;

    AttribParserList m_RootAttribParsers;
    AttribParserList m_MaterialAttribParsers;
    AttribParserList m_TechniqueAttribParsers;
    AttribParserList m_PassAttribParsers;
    AttribParserList m_TextureUnitAttribParsers;

	FILE *m_pExportfp;
	char m_szTab[MAX_PATH];

public:
	int  ParseScript( ioTextStream &rkStream, const char *szScriptName );
	void SaveScript( FILE *fp, ioMaterial *pMtl );

protected:
	void InitMaterialScriptContext( const char *szScriptName );
	bool ParseScriptLine( std::string &rkLine );
	bool InvokeParser( std::string &rkLine, AttribParserList &rkParsers );

protected:
	void BeginExport( FILE *fp );
	void SetExportTabSize( int iSize );
	void ExportScript( LPSTR fmt,... );
	void EndExport();

	void SaveTechnique( ioTechnique *pTech );
	void SavePass( ioPass *pPass );
	void SaveTextureUnitState( ioTextureUnitState *pUnit );

public:
	ioMaterialSerializer();
	virtual ~ioMaterialSerializer();
};

D3DBLENDOP ConvertBlendOp( const std::string &token );
D3DBLEND ConvertBlendFactor( const std::string &token );

#endif
