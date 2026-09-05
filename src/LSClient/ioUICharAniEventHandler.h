#pragma once


#include "../io3DEngine/ioAniEventHandler.h"
#include "AniEventHelpStruct.h"

class ioCreateChar;

class ioUICharAniEventHandler : public ioAniEventHandler
{
protected:
	ioCreateChar *m_pChar;

	AniEventInfoList m_ShopEffectList;
	AniSoundEventInfoList m_ShopSoundList;
	AniEventInfoList m_HideMeshLinkEffectList;

public:
	virtual bool EventHandling( const AniEventTime *pEvent, const ioAnimation *pNotifier );

public:
	void SetCreateChar( ioCreateChar *m_pChar );

protected:
	virtual void OnAnimationClear( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	
	void OnAutoEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnShopEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnShopSound( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnFaceChange( const AniEventTime *pEvent, const ioAnimation *pNotifier );

	void OnShowWeaponMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnShowArmorMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnShowHelmMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnShowCloakMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnShowHairMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void ShowMesh( const ioHashString &rkMeshName, const ioHashString &rkEffectName, const ioAnimation *pNotifier );
	void ShowMeshLinkEffect( const ioHashString &rkEffectName, const ioAnimation *pNotifier );

	void OnHideWeaponMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnHideArmorMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnHideHelmMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnHideCloakMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnHideHairMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void HideMesh( const ioHashString &rkMeshName, const ioHashString &rkEffectName, const ioAnimation *pNotifier );
	void HideMeshLinkEffect( const ioHashString &rkEffectName, const ioAnimation *pNotifier );

	bool IsCanShopSound();

protected:
	ioEntity* GetWeaponEntity();
	ioEntity* GetHandEntity();
	ioEntity* GetLegEntity();

public:
	ioUICharAniEventHandler();
	virtual ~ioUICharAniEventHandler();
};

