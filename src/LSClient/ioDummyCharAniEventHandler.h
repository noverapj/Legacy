#pragma once


#include "../io3DEngine/ioAniEventHandler.h"
#include "AniEventHelpStruct.h"

class ioDummyChar;

class ioDummyCharAniEventHandler : public ioAniEventHandler
{
protected:
	ioDummyChar *m_pChar;

	AniEventInfoList m_ShopEffectList;
	AniEventInfoList m_HideMeshLinkEffectList;

public:
	virtual bool EventHandling( const AniEventTime *pEvent, const ioAnimation *pNotifier );

public:
	void SetCreateChar( ioDummyChar *m_pChar );

protected:
	virtual void OnAnimationClear( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	
	void OnAutoEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier );
	void OnShopEffect( const AniEventTime *pEvent, const ioAnimation *pNotifier );

public:
	ioDummyCharAniEventHandler();
	virtual ~ioDummyCharAniEventHandler();
};

