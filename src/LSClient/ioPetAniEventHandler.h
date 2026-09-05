#pragma once


#include "../io3DEngine/ioAniEventHandler.h"
#include "AniEventHelpStruct.h"

class ioPet;

class ioPetAniEventHandler : public ioAniEventHandler
{
protected:
	ioPet *m_pChar;

	AniEventInfoList m_ShopEffectList;
	AniEventInfoList m_HideMeshLinkEffectList;

public:
	virtual bool EventHandling( const AniEventTime *pEvent, const ioAnimation *pNotifier );

public:
	void SetCreatePet( ioPet *m_pChar );

protected:
	virtual void OnAnimationClear( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	
	void OnAutoEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnShopEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier );

public:
	ioPetAniEventHandler();
	virtual ~ioPetAniEventHandler();
};

