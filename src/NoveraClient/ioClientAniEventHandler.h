#pragma once


#include "../io3DEngine/ioAniEventHandler.h"
#include "AniEventHelpStruct.h"

class ioBaseChar;
class ioEntity;

/*
 이벤트를 추가할 일이 생기면 엔진을 수정하기 보다는 

  virtual bool EventHandling( const AniEventTime *pEvent,
								const ioAnimation *pNotifier );

  이 함수를 재정의 할것...

ex : )
bool ioClientAniEventHandler::EventHandling( const AniEventTime *pEvent,
											 const ioAnimation *pNotifier )
{
    // 엔진 내부에 정의된 이벤트 처리
    if( ioAniEventHandler::EventHandling( pEvent, pNotifier ) )
	    return true;

    if( pEvent->szEventType == "new_event" )
	{
	   OnNewEvent( pEvent, pNotifier );
	   return true;
	}

    ......

    return false;
}
*/

class ioClientAniEventHandler : public ioAniEventHandler
{
protected:
	ioBaseChar *m_pChar;
	
	AniEventInfoList m_ShopEffectList;
	AniSoundEventInfoList m_ShopSoundList;
	AniEventInfoList m_HideMeshLinkEffectList;

	bool m_bItemMeshHide;

public:
	virtual bool EventHandling( const AniEventTime *pEvent,
								const ioAnimation *pNotifier );

protected:
	virtual void OnTrailStart( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	virtual void OnAnimationClear( const AniEventTime *pEvent, const ioAnimation *pNotifier );

	void OnAutoShakeCamera( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnFaceChange( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnMotionSound( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnVoiceSound( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnAutoEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnShopEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnShopSound( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnDummyCharAttack( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnLobeliaAction( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnCmdAction( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnCmdEnd( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnSkillAction( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnUseGPAction( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnShowTrinityMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnHideTrinityMesh( const AniEventTime *pEvent, const ioAnimation *pNotifier );

	bool IsCanShopSound();

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

	void ClearAniEvent();

	virtual void ClearAniEffect();

protected:
	ioEntity* GetWeaponEntity();
	ioEntity* GetHandEntity();
	ioEntity* GetLegEntity();

protected:
	void CreateEntityTrails( ioEntity *pEntity,
							 const ioHashString &szTexture,
							 FXRenderType eType,
							 int iSegment,
							 float fTime,
							 float fSampleTick );

	void CreateMeshTrails( const ConstCtrlPointList &rkStart,
						   const ConstCtrlPointList &rkEnd,
						   const ioHashString &szTexture,
						   FXRenderType eType,
						   int iSegment,
						   float fTime,
						   float fSampleTick );

public:
	void SetBaseChar( ioBaseChar *pChar );

public:
	ioClientAniEventHandler();
	virtual ~ioClientAniEventHandler();
};

