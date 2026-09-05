#pragma once


#include "../io3DEngine/ioAniEventHandler.h"
#include "AniEventHelpStruct.h"

class ioPetView;

class ioUIPetAniEventHandler : public ioAniEventHandler
{
protected:
	ioPetView *m_pChar;

	AniEventInfoList m_ShopEffectList;
	AniSoundEventInfoList m_ShopSoundList;
	AniEventInfoList m_HideMeshLinkEffectList;

public:
	virtual bool EventHandling( const AniEventTime *pEvent, const ioAnimation *pNotifier );

public:
	void SetCreatePet( ioPetView *m_pChar );

protected:
	virtual void OnAnimationClear( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	
	void OnAutoEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnShopEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnShopSound( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	
public:
	ioUIPetAniEventHandler();
	virtual ~ioUIPetAniEventHandler();
};

