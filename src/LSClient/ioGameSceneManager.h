#pragma once


class ioLandScape;
class ioSkyDome;
class ioPlayStage;
class ioEntityGroup;
class ioOpcodeShape;
class ioWorldMesh;
class ioBinaryStream;
class ioEntityGrpMaterialModifier;
class ioScreenBlind;

#define APPLY_FIRST_MAINLIGHT	1
#define APPLY_SECOND_MAIN_LIGHT	2

class ioGameSceneManager : public ioSceneManager
{
protected:
	ioLandScape	*m_pLandScape;
	ioWorldMesh *m_pWorldMesh;
	ioSkyDome	*m_pSkyDome;

	ioScreenBlind *m_pScreenBlinder;
	bool m_bUseScreenBlinder;

	D3DCOLORVALUE m_DefaultDiffuseRate;
	bool m_bShowSky;

protected: // 멀티 맵
	bool m_bMultiMap;
	int  m_iMultiMapCurrentIndex;
	int  m_iMaxMultiMapCount;

protected: // Ambient INI.
	struct AmbientData
	{
		float m_fRed;
		float m_fGreen;
		float m_fBlue;
	};
	typedef std::vector< AmbientData > AmbientDataList;
	AmbientDataList m_AmbientDataList;

protected: // Main Light INI.
	struct MainLIghtData
	{
		D3DCOLORVALUE m_Diffuse;
		D3DXVECTOR3   m_vLightDir;
		ioHashString  m_szToonTexture;
		ioHashString  m_szSecondToonTexture;
	};
	typedef std::vector< MainLIghtData > MainLIghtDataList;
	MainLIghtDataList m_MainLIghtDataList;

protected: // Shadow Map INI.
	struct ShadowMapData
	{
		int m_iTextureSize;

		float m_fRed;
		float m_fGreen;
		float m_fBlue;

		D3DXVECTOR3 m_vShadowDir;
	};
	typedef std::vector< ShadowMapData > ShadowMapDataList;
	ShadowMapDataList m_ShadowMapDataList;

protected: // Map Camera Limit INI.
	struct CameraMapLimit
	{
		D3DXVECTOR3 m_vMinLookAt;
		D3DXVECTOR3 m_vMaxLookAt;
	};
	typedef std::vector< CameraMapLimit > CameraMapLimitList;
	CameraMapLimitList m_CameraMapLimitList;

protected: // Boost Limit INI.
	struct BoostLimit
	{
		D3DXVECTOR3 m_vMin;
		D3DXVECTOR3 m_vMax;
	};
	typedef std::vector< BoostLimit > BoostLimitList;
	BoostLimitList m_BoostLimitList;

protected: // Axis Limit INI.
	struct AxisLimit
	{
		D3DXVECTOR3 m_vMin;
		D3DXVECTOR3 m_vMax;
	};
	typedef std::vector< AxisLimit > AxisLimitList;
	AxisLimitList m_AxisLimitList;

protected:
	//LoadModelList 와 LoadVertexColor간의 로딩순서를 맞추기 위함
	typedef std::list< ioEntityGroup* > EntityGrpList;
	typedef std::map< ioHashString, EntityGrpList* > EntityGrpListMap;
	EntityGrpListMap m_LoadListMap;

public:
	virtual void UpdateSceneGraph( ioCamera *pCamera, const D3DXMATRIX *pExtraProj = NULL, bool bResetVolume=false );
	virtual void SetShadowMapEnable( bool bEnable );
	virtual void ClearScene();

protected:
	virtual void FindShadowCasterObjects( ioCamera *pCamera );

public:
	virtual void DestroyLight( const ioHashString &kName );
	virtual void DestroyAllLights();

	virtual void SetSoftwareVertexBlending( bool bSoftware );
	virtual void ModifyEntityGroupMaterial( ioEntityGrpMaterialModifier *pModifier );

public:
	void SetMainLight( const D3DXVECTOR3 &vDir,
					   const D3DCOLORVALUE &rkDiffuse,
					   const D3DCOLORVALUE &rkSpecular,
					   const ioHashString &rkLightTexture );

	void LoadHeightMap( const char *szMapName, ioPlayStage *pStage );
	void LoadMeshMap( ioINILoader &rkLoader, ioPlayStage *pStage );

	void RenderLandscape();

public:
	void EnableGlow( bool bEnable );
	void EnableLocalGlow( bool bEnable );
	void EnableFilter( FilterType eType );
	void EnableOveray( OverayType eType );

public:
	void EnableMapEdge( bool bEnable );
	void StopWorldMeshTexEffect( bool bStop );
	void SetScreenBlind( DWORD dwColor );

protected:
	void LoadMeshMapMainLights( ioINILoader &rkLoader );
	void LoadMeshMapLight( ioINILoader &rkLoader );

	void LoadHeightMapMainLight( const char *szMapName );
	void LoadLandScape( const char *szMapName, ioPlayStage *pStage );
	void LoadModelList( const char *szMapName, ioPlayStage *pStage );

	void LoadEffectList( const char *szMapName, ioPlayStage *pStage );
	void LoadLightList( const char *szMapName );
	void LoadMapAmbient( const char *szMapName );

	void LoadVertexColor( const char *szMapName );
	void LoadEntityVertexColor( ioBinaryStream &rkStream, ioEntityGroup *pGrp );

protected:
	void CreateSkyDome( const char *szMapName );
	void SetMapFog( const char *szMapName );

	void CheckDecalList();
	void PopulateLandScapePointLightList( ioLightList &kPointList );

public:
	D3DCOLORVALUE GetDiffuseColorRate( const D3DXVECTOR3 &vPos );
	ioOpcodeShape* GetCollisionShape() const;

	ioAxisAlignBox GetWorldMapBox() const;

public:
	void RenderShadowMap( int iX, int iY, int iWidth, int iHeight );

public:
	float GetHeight( float fXPos, float fZPos ) const;	
	float GetHeight( float fXPos, float fYPos, float fZPos ) const;
	float GetBoundHeight();

public:
	inline const bool IsMultiMap() const { return m_bMultiMap; }
	inline const int GetMaxMultiMapCount() const { return m_iMaxMultiMapCount; }
	inline const int GetMultiMapCurrentIndex() const { return m_iMultiMapCurrentIndex; }

public:
	void SetMultiMapCurrentIndex( int iMapIndex );

public:
	void PreLoadAmbientDataSet();
	void PreLoadMainLightDataSet();
	void PreLoadShadowMapDataSet();
	D3DXVECTOR3 CheckMapCameraLimit( D3DXVECTOR3 vNewLookAt ) const;
	void CheckFlyMapLimit( D3DXVECTOR3 *pFlyPos );
	bool CheckAxisLimit( D3DXVECTOR3 *pNewPos );
	bool CheckAxisLimit( const D3DXVECTOR3& vNewPos );

public:
	ioGameSceneManager( ioRenderSystem *pRenderer );
	virtual ~ioGameSceneManager();
};

