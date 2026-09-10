#pragma once

class ioRay;
class ioSceneManager;
class ioCamera;
class ioCreateChar;
class ioEntityGrpMaterialModifier;
class ioEffect;
struct CHARACTER;
class ioPetView;

#include "../io3DEngine/ioMath.h"

class ioUI3DRender
{
protected:
	ioSceneManager *m_pSceneMgr;
	ioCamera *m_pCamera;

	D3DCOLORVALUE m_UISceneAmbient;
	D3DCOLORVALUE m_BackUpAmbient;

	ioHashStringVec m_ModelAnimation;

	typedef std::list< ioEffect* > EffectList;
	EffectList m_EffectList;

protected:
	typedef std::set< ioSceneManager* > SceneMgrList;
	static SceneMgrList m_CreatedSceneMgrList;

public:
	static void SetSoftSkinning( bool bSoft );
	static void ModifyEntityGroupMaterial( ioEntityGrpMaterialModifier *pMod );

protected:
	static void AddSceneMgr( ioSceneManager *pSceneMgr );
	static void RemoveSceneMgr( ioSceneManager *pSceneMgr );

public:
	void Initialize( const char *szININame, const char *szCamInfo );
	void Destroy();

public:
	void Render( const D3DXMATRIX *pExtraProj, bool bZClear );
	void RenderNow( const D3DXMATRIX *pExtraProj, bool bZClear );
	void RenderViewPort( const D3DRECT *pRcView, bool bZClear );
	void RenderViewPort( const D3DRECT *pRcView, const D3DXMATRIX *pProj, bool bZClear );

public:
	void ApplyUIAmbient();
	void RestoreAmbient();

	void SetCameraPos( const D3DXVECTOR3 &vPos );
	void SetCameraPos( float fXPos, float fYPos, float fZPos );

	void SetLookAt( const D3DXVECTOR3 &vLookAt );
	void SetLookAt( float fXPos, float fYPos, float fZPos );

	void SetProjection( float fNear, float fFar, float fFov, float fAspect );

public:
	D3DXVECTOR3 GetCameraPosition() const;

public:
	D3DXMATRIX MakeUIProjection( int iXPos, int iYPos );
	D3DXMATRIX MakeUIProjection( float vwidth, float vheight, float near, float far );

protected:
	void SetMainLight( const D3DXVECTOR3 &vDir,
					   const D3DCOLORVALUE &rkDiffuse,
					   const D3DCOLORVALUE &rkSpecular,
					   const ioHashString &rkLightTexture );

public:
	ioCreateChar* CreateChar( const CHARACTER &rkInfo,
							  const ioHashString &rkName,
							  bool bThread = true );

	void ChangeModel( ioCreateChar* &pOldChar, const CHARACTER &rkInfo );
	void ChangeDeco( ioCreateChar* pChar, const CHARACTER &rkInfo );
	void ChangeBeard( ioCreateChar *pChar, const CHARACTER &rkInfo );
	void ChangeFace( ioCreateChar *pChar, const CHARACTER &rkInfo );
	void ChangeHair( ioCreateChar *pChar, const CHARACTER &rkInfo );
	void ChangeAccessory( ioCreateChar *pChar, const CHARACTER &rkInfo );
	void ChangeUnderwear( ioCreateChar *pChar, const CHARACTER &rkInfo );

	void ChangeHairColor( ioCreateChar *pChar, const CHARACTER &rkInfo );
	void ChangeSkinColor( ioCreateChar *pChar, const CHARACTER &rkInfo );

protected:
	ioCreateChar* LoadChar( const CHARACTER &rkInfo,
							const ioHashString &rkName,
							bool bThread );

public:
	bool AddEffect( const ioHashString &rkName,
					const D3DXVECTOR3 &vPos,
					const D3DXVECTOR3 &vScale = ioMath::UNIT_ALL,
					const D3DXVECTOR3 &vDir = ioMath::UNIT_Y );

	void UpdateEffectList();

protected:
	void DestroyAllEffects();
	void DestroyEffect( ioEffect *pEffect );

public:	
	ioHashString GetModelAnimation( int iIndex, bool bMale ) const;
	const ioRay& GetCurPickRay() const;

	ioCamera* GetCamera() const { return m_pCamera; }
	
public:
	void UpdateCameraByMouse( float fForward, float fStrafe, float fRot );

public:
	ioPetView* CreatePet( const ioHashString &rkName, bool bThread = true );

public:
	ioUI3DRender();
	virtual ~ioUI3DRender();
};

