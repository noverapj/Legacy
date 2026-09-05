

#ifndef _ioModelBuilder_h_
#define _ioModelBuilder_h_

//----- Config File Identity -----------
#define	MODEL_TOKEN				"#MODEL_CFG"
#define MESH_LIST				"#MESH_LIST"
#define MESH_LIST_END			"#MESH_LIST_END"
#define SKELETON				"#SKELETON"			//Honey-pot: 미끼, 실제로는 사용안하고 LineParsing() 코드내에서 초기화
#define COLLISION_MESH			"#COLLISION_MESH"	//Honey-pot: 미끼, 실제로는 사용안하고 LineParsing() 코드내에서 초기화
#define AABOX					"#AABOX"			//Honey-pot: 미끼, 실제로는 사용안하고 LineParsing() 코드내에서 초기화
#define SPHERE					"#SPHERE"
#define OUTLINE_EDGE			"#OUTLINE"
#define VIEW_CHECK_ONLY			"#VIEW_CHECK_ONLY"
#define MODEL_COLOR				"#MODEL_COLOR"
#define OWN_ALPHA_DIST			"#OWN_ALPHA_DIST"
#define EFFECT_LIST				"#EFFECT_LIST"
#define EFFECT_LIST_END			"#EFFECT_LIST_END"
#define ANIMATION_LIST			"#ANIMATION_LIST"
#define ANIMATION_LIST_END		"#ANIMATION_LIST_END"
#define MODEL_END				"#MODEL_END"

class ioEntityGroup;
class ioTextStream;

class __EX ioEntityGroupBuilder
{
public:
	enum TOKEN_TYPE
	{
		TT_UNKNOWN,
		TT_TITLE,
		TT_DATA,
		TT_COMMENT,

		TT_MODEL_TOKEN,

		TT_MESH_LIST,
		TT_MESH_LIST_END,

		TT_AABOX,
		TT_SPHERE,
		TT_OUTLINE,
		TT_VIEW_CHECK_ONLY,
		TT_MODEL_COLOR,
		TT_OWN_ALPHA_DIST,

		TT_SKELETON,
		TT_COLLISION_MESH,

		TT_EFFECT_LIST,
		TT_EFFECT_LIST_END,

		TT_ANIMATION_LIST,
		TT_ANIMATION_LIST_END,

		TT_MODEL_END,
		TT_ERROR,
	};

private:
	int	m_iRed, m_iGreen, m_iBlue;

private:
	static char m_szStartPath[MAX_PATH];

public:
	static void SetStartDir( const char *szPath );
	static const char* GetStartDir() { return m_szStartPath; }

public:
	bool BuildGroup( const char *szFileName, ioEntityGroup *pGrp, bool bThread = false );
	bool SaveGroup( const char *szFileName, ioEntityGroup *pGrp );

	bool LoadMeshNameList( const char *szFileName, ioHashStringVec &rkNameList );

private:
	void InitDefaultMeshColor();

	bool IsModel( ioTextStream &rkStream );
	void Parsing( ioTextStream &rkStream, ioEntityGroup *pGrp, bool bThread );

	void ParsingMeshNameList( ioTextStream &rkStream, ioHashStringVec &rkNameList );

	TOKEN_TYPE LineParsing( ioTextStream &rkStream, char *szBuf, int iBufLen );
	TOKEN_TYPE ParsingMesh( ioTextStream &rkStream, ioEntityGroup *pGrp, bool bThread );
	TOKEN_TYPE ParsingAABox( ioTextStream &rkStream, ioEntityGroup *pGrp );
	TOKEN_TYPE ParsingSphere( ioTextStream &rkStream, ioEntityGroup *pGrp );
	TOKEN_TYPE ParsingOutLine( ioTextStream &rkStream, ioEntityGroup *pGrp );
	TOKEN_TYPE ParsingOwnDistAlpha( ioTextStream &rkStream, ioEntityGroup *pGrp );
	TOKEN_TYPE ParsingViewCheckOnly( ioTextStream &rkStream, ioEntityGroup *pGrp );
	TOKEN_TYPE ParsingModelColor( ioTextStream &rkStream );
	TOKEN_TYPE ParsingEffect( ioTextStream &rkStream, ioEntityGroup *pGrp );
	TOKEN_TYPE ParsingSkeleton( ioTextStream &rkStream, ioEntityGroup *pGrp );
	TOKEN_TYPE ParsingCollisionMesh( ioTextStream &rkStream, ioEntityGroup *pGrp );
	TOKEN_TYPE ParsingAnimation( ioTextStream &rkStream, ioEntityGroup *pGrp, bool bThread );
public:
	ioEntityGroupBuilder();
};

#endif