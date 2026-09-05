#pragma once


#include "../io3DEngine/ioMaterialModifier.h"

class ioMaterial;
class ioTechnique;

class LoadingScriptMod : public ioMaterialScriptModifier
{
protected:
	LightQuality m_LightQuality;
	bool m_bUseFixedMode;

public:
	virtual void ModifyMaterial( ioMaterial *pMaterial );

public:
	void SetLightQuality( LightQuality eQuality ) { m_LightQuality = eQuality; }
	void SetUseFixedMode( bool bUse ) { m_bUseFixedMode = bUse; }

protected:
	void LightMapShaderDisable( ioTechnique *pTech );
	void StaticShaderDisable( ioTechnique *pTech );

	void PixelShaderEnable( ioTechnique *pTech, const char *szGroup );
	void PixelShaderDisable( ioTechnique *pTech, const char *szGroup );

protected:
	void AddTwoPassShade( ioTechnique *pTech );

	void StaticToonShade( ioTechnique *pTech );

	void AnimateToonShade( ioTechnique *pTech );
	void AnimateToonShadeDefault( ioTechnique *pTech );
	void AnimateToonShadeSkin( ioTechnique *pTech );	// 피부색이 들어있는 메터리얼

public:
	LoadingScriptMod();
	virtual ~LoadingScriptMod();
};

class CloneScriptMod : public ioMaterialScriptModifier
{
protected:
	bool m_bEnableOutLine;

public:
	virtual void ModifyMaterial( ioMaterial *pMaterial );

public:
	void EnableOutLine( bool bEnable );

protected:
	void EnableOutLinePass( ioTechnique *pTech );

public:
	CloneScriptMod();
	virtual ~CloneScriptMod();
};

//----------------------------------------------------------------------------

class LightQualityMod : public ioMaterialScriptModifier
{
protected:
	LightQuality m_LightQuality;
	bool m_bEntityModify;

public:
	void SetLightQuality( LightQuality eQuality ) { m_LightQuality = eQuality; }
	void SetEntityModify( bool bEntity ) { m_bEntityModify = bEntity; }

public:
	virtual void ModifyMaterial( ioMaterial *pMaterial );

protected:
	void AddToonShadePass( ioTechnique *pTech, int iToonTextureStage );
	void RemoveTwoPassShade( ioTechnique *pTech );

	enum
	{
		CHANGE_NONE,
		CHANGE_FAIL,
		CHANGE_SKIN,
		CHANGE_NO_SKIN,
	};

	int  ChangeDefaultToToonShade( ioTechnique *pTech, const char *szGroupName );
	void ChangeToonShadeToDefault( ioTechnique *pTech, const char *szGroupName );
    
public:
	LightQualityMod();
	LightQualityMod( LightQuality eQuality );
	virtual ~LightQualityMod();
};

//----------------------------------------------------------------------------

class EnableOutLineMod : public ioMaterialScriptModifier
{
protected:
	bool m_bEnableOutLine;

public:
	void SetOutLineEnable( bool bEnable ) { m_bEnableOutLine = bEnable; }
	virtual void ModifyMaterial( ioMaterial *pMaterial );

public:
	EnableOutLineMod();
	EnableOutLineMod( bool bEnable );
	virtual ~EnableOutLineMod();
};

//-------------------------------------------------------------

class ApplyAnimateGroupOnly : public ioEntityGrpMaterialModifier
{
protected:
	ioMaterialScriptModifier *m_pModifier;

public:
	virtual void ModifyMaterial( ioEntityGroup *pGrp );

public:
	ApplyAnimateGroupOnly();
	ApplyAnimateGroupOnly( ioMaterialScriptModifier *pModifer );
	virtual ~ApplyAnimateGroupOnly();
};

class ApplyAllEntityGroup : public ioEntityGrpMaterialModifier
{
protected:
	ioMaterialScriptModifier *m_pModifier;

public:
	virtual void ModifyMaterial( ioEntityGroup *pGrp );

public:
	ApplyAllEntityGroup();
	ApplyAllEntityGroup( ioMaterialScriptModifier *pModifer );
	virtual ~ApplyAllEntityGroup();
};

