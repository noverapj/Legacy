

#ifndef _ioLandScape_h_
#define _ioLandScape_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioPatch;
class ioTexture;
class ioRay;
class ioCamera;
class ioShader;
class ioAutoShaderParamSource;

class ioVertexDeclaration;
class ioVertexBufferInstance;
class ioIndexBufferInstance;

typedef struct tagioMultiPassInfo
{
	ioTexture *pTexColor;
	ioTexture *pTexMod;
	bool	bSpecular;

	float	fRepeatX;
	float	fRepeatY;
	D3DXMATRIX	matTex;

	tagioMultiPassInfo()
	{
		pTexColor = pTexMod = NULL;
		bSpecular = false;

		fRepeatX = fRepeatY = 32.0f;
	}
}ioMultiPassInfo;

#define MAX_POINT_LIGHT  4
#define MAX_PATCH_SHADER 5	// MAX_POINT_LIGHT + 1( No Light )

typedef std::vector< ioMultiPassInfo > vMultiPass;
typedef std::vector< ioPatch* > vPatchList;

class __EX ioLandScape
{
public:
	enum RenderMode
	{
		RM_EDIT,	// for edit
		RM_DEFAULT,
		RM_MORPH_SW
	};

protected:
	IDirect3DDevice9	*m_pD3DDevice;
	ioHashString		m_szMapName;

	float	m_fSizeX, m_fSizeY, m_fSizeZ;
	float	m_fPatchDX, m_fPatchDY, m_fPatchDZ;		// scaled patch size ( m_fSizeX : m_iHMapWidth )
	float	m_fCellDX, m_fCellDZ;
	int		m_iMinX, m_iMinY, m_iMaxX, m_iMaxY;		// patch's index range in view frustum
	int		m_iNumPatchX, m_iNumPatchY;
	int		m_iHMapWidth, m_iHMapHeight;

	RenderMode	m_RenderMode;
	float	m_fMaxError;
	int		m_iDesiredFPS;

	ioPatch	***m_pPatch;
	float	*m_pHeight;
	D3DXVECTOR3	*m_pNormal;

	D3DCOLORVALUE	m_LandAmbient;
	D3DCOLORVALUE	m_LandSpecular;
	float	m_fSpecularLv;
	float	m_fSpecularPower;

	vPatchList m_vPatchCameraSortList;

	ioVertexDeclaration *m_pVtxDecl;
	ioVertexBufferInstance *m_pVtxInstance;
	ioVertexBufferInstance *m_pNrmInstance;

	IntVec m_vPatchVtxStartIndexList;
	int m_iNumBufVertex, m_iNewNumVertex;
	int m_iCurStartVtx;

	ioIndexBufferInstance *m_pIdxInstance;
	IntVec m_vLayerNumIndexList;
	int m_iNumBufIndex, m_iNewNumIndex;
	int m_iCurStartIdx;

	ioPatch	*m_pActivePatch;
	vMultiPass	m_vMultiPass;

	ioShader *m_pSWShader[MAX_PATCH_SHADER];
	ioShader *m_pSWShadowMapD16[MAX_PATCH_SHADER];

	ShadowMapType m_ShadowMapType;

	float	m_fAlphaMinDist;
	float	m_fAlphaMaxDist;

	D3DCOLORVALUE *m_pLightMapColor;
	int		m_iDiffuseRateMapWidth;
	int		m_iDiffuseRateMapHeight;

	bool	m_bUseSpecular;

public:	// for debugging
	int		m_iNumProcessPatch;
	int		m_iNumVisiblePatch;
	int		m_iNumRenderPatch;
	int		m_iNumRenderedTris;
	int		m_iNumRenderedVtx;

public:
	void SetRenderMode( RenderMode mode ) { m_RenderMode = mode; }
	void SetMapName( const ioHashString &name ) { m_szMapName = name; }
	void SetMapSize( float fSizeX, float fSizeY, float fSizeZ );
	void SetPatchSize( float fPatchDX, float fPatchDY, float fPatchDZ );
	void SetHMapSize( int iHMapX, int iHMapY );
	void SetNumPatch( int iPatchX, int iPatchY );
	void SetMaxError( float fMaxError ) { m_fMaxError = fMaxError; }
	void SetDesireFPS( int iFPS ) { m_iDesiredFPS = iFPS; }
	void SetUseSpecular( bool bUse );
	void SetShadowMapType( ShadowMapType eType ) { m_ShadowMapType = eType; }

	float GetMaxError() const { return m_fMaxError; }
	int GetDesireFPS() const { return m_iDesiredFPS; }

	int GetHMapWidth() const { return m_iHMapWidth; }
	int GetHMapHeight() const { return m_iHMapHeight; }
	int GetNumPatchX() const { return m_iNumPatchX; }
	int GetNumPatchY() const { return m_iNumPatchY; }

	float GetMapSizeX() const { return m_fSizeX; }
	float GetMapSizeY() const { return m_fSizeY; }
	float GetMapSizeZ() const { return m_fSizeZ; }

	float GetCellDX() const { return m_fCellDX; }
	float GetCellDZ() const { return m_fCellDZ; }

	const ioHashString& GetMapName() const { return m_szMapName; }
	bool IsSpecularUsed() const { return m_bUseSpecular; }

	ioPatch*& GetActiveListHead() { return m_pActivePatch; }
	virtual char* GetRenderModeByText();

public:
	void SetHeightMap( BYTE *pHeight, int iWidth, int iHeight );
	void SetHeightMap( float *pHeight, int iWidth, int iHeight );
	void SetNotLODList( bool *pNotLOD );

	void AddMultiPassTex( const ioMultiPassInfo &info );
	void CreateNormals( bool bMustReAlloc = false );

	bool Build();
	virtual void UpdateTerrain( ioCamera *pCamera );
	void ChangeMode( RenderMode mode );	

	void SetVisLayers( DWORD *pVisLayer );

	void SetAlphaMinDist( float fMin ) { m_fAlphaMinDist = fMin; }
	void SetAlphaMaxDist( float fMax ) { m_fAlphaMaxDist = fMax; }
	float GetAlphaMinDist() const { return m_fAlphaMinDist; }
	float GetAlphaMaxDist() const { return m_fAlphaMaxDist; }

public:
	bool MoveAboveSurface( D3DXVECTOR3 &vPos, float fHeight ) const;
	float GetLineMaxHeight( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd ) const;
	
	ioPatch* GetPickPos( const ioRay &rkRay, D3DXVECTOR3 &vPickPos );

public:
	virtual void Render( const ioAutoShaderParamSource &rkSource,
						 const ioLight *pMainLight,
						 const ioLightList &kPointList );

protected:
	virtual ioPatch* CreatePatch( int nX, int nY );

	void UpdateTextureMatrix( const ioCamera *pCamera );
	void UpdateMinMaxArea( const ioCamera *pCamera );
	void UpdateActivePatchList( const ioCamera *pCamera );
	virtual void UpdatePatchLevels( const ioCamera *pCamera );
	void ReUpdatePatchLevelByCheckAround();
	void TesselationPatches();

	void CheckBuffer();
	void GatherBuffer();

	void SetupTextureStage( int iPass );

	ioShader* GetBestShaderAndSetLight( const ioAutoShaderParamSource &rkSource,
										const ioLight *pMainLight,
										const ioLightList &kPointList );

	void LoadShader();
	void UnLoadShader();

public:
	void SetLandAmbient( float fRed, float fGreen, float fBlue );
	const D3DCOLORVALUE& GetLandAmbient() const { return m_LandAmbient; }

	void SetLandSpecular( float fRed, float fGreen, float fBlue );
	const D3DCOLORVALUE& GetLandSpecular() const { return m_LandSpecular; }

	void SetSpecularLv( float fLv );
	float GetSpecularLv() const;

	void SetSpecularPower( float fPower );
	float GetSpecularPower() const;

	bool InitDiffuseRateByBaseLightMap();
	const D3DCOLORVALUE& GetLightMapColor( float fXPos, float fZPos );

public:
	float GetHeight( float fXPos, float fZPos ) const;
	inline float GetHeight( int nX, int nY ) const;
	inline const D3DXVECTOR3& GetNormal( int nX, int nY ) const;

public:
	ioLandScape( IDirect3DDevice9 *pDevice );
	virtual ~ioLandScape();
};

#define FLIP_Y

inline float ioLandScape::GetHeight(int nX, int nY) const
{
	assert( COMPARE( nX, 0, m_iHMapWidth ) );
	assert( COMPARE( nY, 0, m_iHMapHeight ) );

#ifdef FLIP_Y
	return m_pHeight[nX + (m_iHMapHeight-1-nY)*m_iHMapWidth];
#else
	return m_pHeight[nX + nY*m_iHMapWidth];
#endif // FLIP_Y
}

//HARDCODE: Normal은 GetHeight를 기반으로 만들어지기 때문에 기본적으로 뒤집어져 있다
inline const D3DXVECTOR3& ioLandScape::GetNormal( int nX, int nY ) const
{
	assert( COMPARE( nX, 0, m_iHMapWidth ) );
	assert( COMPARE( nY, 0, m_iHMapHeight ) );
	assert( m_pNormal );

#ifdef FLIP_Y
	return m_pNormal[nX + nY * m_iHMapWidth ];
#else
	return m_pNormal[nX + (m_iHMapHeight-1-nY)*m_iHMapWidth];
#endif
}

#endif