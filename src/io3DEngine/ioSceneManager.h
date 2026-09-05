

#ifndef _ioSceneManager_h_
#define _ioSceneManager_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioVectorIterator.h"
#include "ioMapIterator.h"

#include "ioRay.h"
#include "ioLight.h"
#include "ioRenderQueue.h"
#include "ioRenderQueueGroup.h"
#include "ioAutoShaderParamSource.h"

class ioSceneNode;
class ioEntityGroup;
class ioCamera;
class ioRenderSystem;
class ioRenderTexture;
class ioEntityGrpMaterialModifier;

class ioGlowPostProcess;
class ioPostFilter;
class ioPostOveray;

class __EX ioSceneManager
{
public:
	typedef std::map< UniqueNodeID, ioSceneNode* > SceneNodeMap;
	typedef ioMapIterator< SceneNodeMap > SceneNodeIter;

	typedef std::map< UniqueObjID, ioEntityGroup* > EntityGroupMap;
	typedef ioMapIterator< EntityGroupMap > EntityGrpIterator;

	typedef std::map< ioHashString, ioLight* > SceneLightList;

protected:
	SceneNodeMap m_SceneNodeMap;
	ioSceneNode	 *m_pSceneRoot;

	EntityGroupMap m_EntityGroupMap;
	SceneLightList m_SceneLightList;

	ioRenderQueue	m_RenderQueue;
	ioRenderSystem *m_pRenderSystem;
	ioAutoShaderParamSource m_AutoShaderParamSource;

	// in WorldSpace, MUST transform to model space, if object picking
	ioRay	m_PickRay;

	ShadowMapType   m_ShadowMapType;
	ioRenderTexture *m_pShadowMapRT;

	Vector3Vec m_vShadowAreaList;
	D3DXVECTOR3 m_vShadowLightDir;
	int		m_iShadowMapSize;

	ioRenderTexture *m_pPostTarget1;
	ioRenderTexture *m_pPostTarget2;

	ioRenderTexture *m_pPostTarget3;
	ioRenderTexture *m_pPostTarget4;

	ioGlowPostProcess *m_pPostGlow;
	ioGlowPostProcess *m_pPostGlowSecond;
	ioPostFilter *m_pPostFilter;
	ioPostOveray *m_pPostOveray;

	int m_iGlowAlphaValue;
	int m_iGlowSecondAlphaValue;
	int m_iFilerAlphaValue;

	TextureFilterOption m_TextureFilterOpt[2];
	float	m_TextureMipMapBias[2];
	bool	m_bNowFog;

	bool	m_bDrawAllTransObjForGlow;

protected:
	static bool	m_bSoftwareVertexBlending;

public:
	virtual void UpdateSceneGraph( ioCamera *pCamera, const D3DXMATRIX *pExtraProj = NULL, bool bResetVolume=false );
	virtual void RenderVisibleObject();
	virtual void RenderVisibleGlowObject();
	virtual void ClearScene();
	virtual void ClearBack( DWORD dwFlags );

protected:
	virtual void FindVisibleObjects( ioCamera *pCamera );
	virtual void FindShadowCasterObjects( ioCamera *pCamera );

public:
	ioRay Calc2DTo3DRay( ioCamera *pCamera, const POINT &ptCursor );
	inline const ioRay& GetCurCameraPickRay() const { return m_PickRay; }

	ioEntityGroup* FindPickEntityGroup();

protected:
	void UpdatePickRay( ioCamera *pCamera );
	void SetViewPortAutoAndSystem( const D3DVIEWPORT9 &rkViewPort );

	void RenderQueueFlush( ioRenderQueue &rkQueue );
	void RenderQueueGroupObjects( ioRenderQueueGroup *pGrp );
	void RenderQueueGroupObjectsGlow( ioRenderQueueGroup *pGrp );
	void RenderOpaqueObjects( const ioRenderableList &rkPassList );
	void RenderTransparentObjects( const ioRenderableList &rkPassList );

	void RenderOpaqueObjectsLocalGlow( const ioRenderableList &rkPassList );
	void RenderTransparentObjectsLocalGlow( const ioRenderableList &rkPassList );

	//yunga test
	void RenderQueueFlushGlow( ioRenderQueue &rkQueue );

	void RenderToShadowMap();
	void RenderToShadowMapQueueGroup( ioRenderQueueGroup *pGrp );

	void SetRenderPass( const RenderablePass &rkPass, bool bGenShadow = false );
	void SetRenderPassLocalGlow( const RenderablePass &rkPass, bool bGenShadow = false );
	void SetRenderPassAlpha( const RenderablePass &rkPass );
	void SetPassLightTexture( ioPass *pPass, const ioLight *pLight );

	void RenderSingleObject( const RenderablePass &rkPass );
	void RenderSingleObjectLocalGlow( const RenderablePass &rkPass );
	void FinalFlushSingleObject( const RenderablePass &rkPass, ioRenderOperation &rkRo );

	void RenderToShadowMapSingleObject( const RenderablePass &rkPass );

public:
	virtual void SetShadowMapEnable( bool bEnable );

	void SetShadowMapSize( int iMapSize ) { m_iShadowMapSize = iMapSize; }
	void SetShadowColor( float fAlpha, float fRed, float fGreen, float fBlue );
	void SetShadowLightDir( const D3DXVECTOR3 &vDir );

	void GenerateShadowMap();
	void ClearShadowMapResource();
	ShadowMapType GetShadowMapType() const;

protected:
	void UpdateShadowMapMatrix( ioCamera *pCamera );
	void GenerateShadowMapR32F();
	void GenerateShadowMapD16();

public:
	void EnableGlowProcess( bool bEnable, const ioHashString &szBlur, const ioHashString &szFilter );
	void EnableGlowProcessSecond( bool bEnable, const ioHashString &szBlur, const ioHashString &szFilter );
	void EnableFilterProcess( bool bEnable, const ioHashString &szShader );
	void EnableOverayProcess( bool bEnable, const ioHashString &szTex, int iRed, int iGreen, int iBlue );

	void EnableDrawAllTransObjForGlow( bool b);

	void RenderPostProcess();
	void ClearPostProcess();

	void CalcLocalGlowProcess();
	void RenderLocalGlowProcess();
	//void ClearSecondProcess();

	void SetGlowAlphaValue( int iAlpha );
	void SetGlowSecondAlphaValue( int iAlpha );
	void SetFilterAlphaValue( int iAlpha );

	void SetGlownessAndTexcelInc( float fGlowness, float fTexcel);

protected:
	bool CheckPostRenderTexture();
	bool CheckEnoughPostRenderTexture( ioRenderTexture* &pTarget,
		int iIndex,
		int iNeedWidth,
		int iNeedHeight );

	bool CheckSecondRenderTexture();
	bool CheckEnoughSecondRenderTexture( ioRenderTexture* &pTarget,
		int iIndex,
		int iNeedWidth,
		int iNeedHeight );

public:
	virtual ioSceneNode* CreateSceneNode();
	virtual void DestroySceneNode( UniqueNodeID eNodeID );

	virtual ioSceneNode* GetRootSceneNode() const;
	virtual ioSceneNode* GetSceneNode( UniqueNodeID eNodeID ) const;

public:
	virtual ioEntityGroup* CreateEntityGrp();

	virtual void DestroyEntityGrp( ioEntityGroup *pGroup );
	virtual void DestroyEntityGrp( UniqueObjID eObjID );
	virtual void DestroyAllEntityGrp();

	virtual ioEntityGroup* GetEntityGrp( UniqueObjID eObjID );
	EntityGrpIterator GetEntityGrpIterator();
	int GetEntityGrpCnt() const;

public:
	virtual ioLight* CreateLight( const ioHashString &kName );
	virtual void DestroyLight( const ioHashString &kName );
	virtual void DestroyLight( ioLight *pLight );
	virtual void DestroyAllLights();

	ioLight* GetLight( const ioHashString &kName );

	void PopulateLightList( const D3DXVECTOR3 &vPos,
		float fRadius,
		ioLightList &rkDestList );

	void PopulateLightList( const D3DXVECTOR3 vPos,
		float fRadius,
		LightReceiveType eType,
		ioLightList &rkDestList );

public:
	virtual void SetSoftwareVertexBlending( bool bSoftware );
	virtual void ModifyEntityGroupMaterial( ioEntityGrpMaterialModifier *pModifier );

	void SetFogEnable( bool bEnable );
	void SetSceneAmbient( float fRed, float fGreen, float fBlue );

public:
	inline bool IsSoftwareVertexBlending() const { return m_bSoftwareVertexBlending; }
	inline bool IsNowFog() const { return m_bNowFog; }

	D3DCOLORVALUE GetSceneAmbient() const;

public:
	void SetTextureFilterOpt( int iStage, TextureFilterOption filter );
	void SetTextureMipMapBias( int iStage, float fBias );

	TextureFilterOption GetTextureFilterOpt( int iStage ) const;
	float GetTextureMipMapBias( int iStage ) const;

public:
	ioSceneManager( ioRenderSystem *pRenderSystem );
	virtual ~ioSceneManager();
};

#endif
