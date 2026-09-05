

#ifndef _ioAutoShaderParam_h_
#define _ioAutoShaderParam_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"
#include "ioLight.h"

class ioPass;
class ioRenderable;
class ioCamera;

class __EX ioAutoShaderParamSource
{
private:
	mutable D3DXMATRIX m_WorldMatrix;
	mutable D3DXMATRIX m_WorldViewMatrix;
	mutable D3DXMATRIX m_ViewProjMatrix;
	mutable D3DXMATRIX m_WorldViewProjMatrix;
	mutable D3DXMATRIX m_InvWorldMatrix;
	mutable D3DXMATRIX m_InvWorldViewMatrix;
	mutable D3DXMATRIX m_InvTransposeWorldMatrix;
	mutable D3DXMATRIX m_InvTransposeWorldViewMatrix;

	mutable D3DXMATRIX m_ProjMatrix;
	mutable D3DXMATRIX m_ExtraProjMatrix;

	D3DXMATRIX	m_LightViewProjMatrix;
	D3DXMATRIX	m_ShadowMapTextureMatrix;

	mutable D3DXMATRIX m_LightWorldViewProjMatrix;
	mutable D3DXMATRIX m_ShadowMapMatrix;

	mutable D3DXVECTOR4 m_CameraPosObjectSpace;
	mutable D3DXVECTOR4 m_CameraDirObjectSpace;
	mutable D3DXVECTOR3 m_CameraPos;
	mutable D3DXVECTOR3 m_CameraDir;
	mutable D3DXVECTOR4 m_CameraFovNearFar;

	mutable D3DXMATRIX m_matIdentity;
	mutable D3DXVECTOR4 m_ViewPort;

	D3DCOLORVALUE m_ShadowColor;

	mutable D3DCOLORVALUE m_AmbientLight;
	mutable D3DCOLORVALUE m_AmbientEmissive;
	mutable D3DXVECTOR4 m_FogFactor;
	mutable D3DXVECTOR4 m_AlphaFactor;
	mutable D3DXVECTOR4 m_OutLineConstant;

private:
	mutable bool m_bWorldMatrixDirty;
	mutable bool m_bWorldViewMatrixDirty;
	mutable bool m_bViewProjMatrixDirty;
	mutable bool m_bWorldViewProjMatrixDirty;
	mutable bool m_bInvWorldMatrixDirty;
	mutable bool m_bInvWorldViewMatrixDirty;
	mutable bool m_bInvTransposeWorldMatrixDirty;
	mutable bool m_bInvTransposeWorldViewMatrixDirty;

	mutable bool m_bProjMatrixDirty;

	mutable bool m_bLightWorldViewProjMatrixDirty;
	mutable bool m_bShadowMapMatrixDirty;

	mutable bool m_bCameraPosObjectSpaceDirty;
	mutable bool m_bCameraDirObjectSpaceDirty;
	mutable bool m_bAlphaFactorDirty;
	mutable bool m_bOutLineConstantDirty;

private:
	const ioPass *m_pCurPass;
	const ioRenderable *m_pCurRenderable;

	const ioCamera	  *m_pCurCamera;
	const ioLightList *m_pApplyLightList;

	bool m_bIgnoreAmbient;
	bool m_bAmbientLightChanged;

public:
	void SetCurrentPass( const ioPass *pPass );
	void SetCurrentRenderable( const ioRenderable *pRend );
	void SetCurrentCamera( const ioCamera *pCam );
	void SetApplyLightList( const ioLightList *pList );

	void SetLightViewProjMatrix( const D3DXMATRIX &matLightViewProj );
	void SetShadowMapTexMatrix( const D3DXMATRIX &matShadowTex );

	void SetShadowColor( const D3DCOLORVALUE &cShadow );
	void SetAmbientLightColor( const D3DCOLORVALUE &cAmbient );
	void SetFogFactor( const D3DXVECTOR4 &vFogFactor );
	void SetViewPort( const D3DVIEWPORT9 &kViewPort );	// 차후 RenderWidnow시스템으로 교체
	void SetExtraProjectionMatrix( const D3DXMATRIX *pExtraProj );

	void SetIgnoreAmbient( bool bIgnore );

public:
	const D3DXMATRIX* GetWorldMatrix() const;
	const D3DXVECTOR4* GetBipedList( int &iVectorCnt ) const;
	const D3DXMATRIX* GetViewMatrix() const;
	const D3DXMATRIX* GetViewProjectionMatrix() const;
	const D3DXMATRIX* GetProjectionMatrix() const;
	const D3DXMATRIX* GetWorldViewProjMatrix() const;
	const D3DXMATRIX* GetWorldViewMatrix() const;
	const D3DXMATRIX* GetInvWorldMatrix() const;
	const D3DXMATRIX* GetInvWorldViewMatrix() const;
	const D3DXMATRIX* GetInvViewMatrix() const;
	const D3DXMATRIX* GetInvTransposeWorldMatrix() const;
	const D3DXMATRIX* GetInvTransposeWorldViewMatrix() const;

	const D3DXMATRIX* GetTextureMatrix( int iUnitIndex ) const;

	const D3DXMATRIX* GetLightWorldViewProjMatrix() const;
	const D3DXMATRIX* GetShadowMapMatrix() const;

	inline const D3DXMATRIX* GetLightViewProjMatrix() const { return &m_LightViewProjMatrix; }
	inline const D3DXMATRIX* GetShadowTextureMatrix() const { return &m_ShadowMapTextureMatrix; }

	const D3DXVECTOR4& GetCameraPosObjectSpace() const;
	const D3DXVECTOR4& GetCameraDirObjectSpace() const;
	inline const D3DXVECTOR4& GetCameraFovNearFar() const { return m_CameraFovNearFar; }
	inline const D3DXVECTOR3& GetCameraPos() const { return m_CameraPos; }
	inline const D3DXVECTOR3& GetCameraDir() const { return m_CameraDir; }

	inline const ioPass* GetCurPass() const { return m_pCurPass; }
	inline const ioRenderable* GetCurRenderable() const { return m_pCurRenderable; }

	int GetApplyLightCnt() const;
	const ioLight* GetApplyLight( int iLight ) const;
	const ioLightList* GetApplyLightList() const { return m_pApplyLightList; }

	inline const D3DCOLORVALUE& GetShadowColor() const { return m_ShadowColor; }
	inline const D3DCOLORVALUE& GetAmbientLightColor() const { return m_AmbientLight; }
	D3DCOLORVALUE GetAmbientEmissiveColor() const;
	D3DCOLORVALUE GetMaterialDiffuseColor() const;

	inline const D3DXVECTOR4& GetFogFactor() const { return m_FogFactor; }
	const D3DXVECTOR4& GetAlphaFactor() const;
	inline const D3DXVECTOR4& GetViewPort() const { return m_ViewPort; }
	D3DXVECTOR4 GetOutLineConstant() const;

	int GetWeightedBipedCnt() const;

public:
	ioAutoShaderParamSource();
	~ioAutoShaderParamSource();
};

#endif
