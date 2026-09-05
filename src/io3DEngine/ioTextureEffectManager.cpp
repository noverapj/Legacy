

#include "stdafx.h"

#include "ioTextureEffect.h"
#include "ioTextureAnimationEffect.h"
#include "ioTextureRotateEffect.h"
#include "ioTextureScrollEffect.h"
#include "ioTextureTransformEffect.h"
#include "ioTextureColorTransformEffect.h"
#include "ioTextureUnitState.h"

#include "ioTextureEffectManager.h"

#include "ioFrameTimer.h"

template<> ioTextureEffectManager* Singleton< ioTextureEffectManager >::ms_Singleton = 0;

ioTextureEffectManager::ioTextureEffectManager()
{
	m_dwLastUpdateFrame = 0;
}

ioTextureEffectManager::~ioTextureEffectManager()
{
	UnLoadAllEffect();
}

void ioTextureEffectManager::UpdateAllEffect()
{
	if( m_dwLastUpdateFrame >= g_FrameTimer.GetFrameCounter() )
		return;

	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	TextureEffectSet::iterator iter;
	for( iter=m_EffectSet.begin() ; iter!=m_EffectSet.end() ; ++iter )
	{
		ioTextureEffect *pTexEff = *iter;
		if( !pTexEff->IsStopped() )
		{
			pTexEff->Update( fTimePerSec );
		}
	}

	m_dwLastUpdateFrame = g_FrameTimer.GetFrameCounter();
}

void ioTextureEffectManager::CreateEffect( ioTextureUnitState *pState, TextureEffect &rkEffect )
{
	if( rkEffect.m_pController )
	{
		UnLoadEffect( rkEffect.m_pController );
		rkEffect.m_pController = NULL;
	}

	ioTextureEffect *pEffect = NULL;

	ioTextureScrollEffect *pScroll;
	ioTextureRotateEffect *pRotate;
	ioTextureAnimationEffect *pAnimate;
	ioTextureTransformEffect *pXform;
	ioTextureColorTransformEffect *pColorXform;

	bool bOnce = false;
	if( rkEffect.m_dwOnce )
		bOnce = true;
	else
		bOnce = false;

	switch( rkEffect.m_Type )
	{
	case TET_SCROLL:
		pScroll = new ioTextureScrollEffect( pState );
		pScroll->SetScrollSpeed( rkEffect.m_fArg1, rkEffect.m_fArg2, bOnce );
		pEffect = pScroll;
		break;
	case TET_ROTATE:
		pRotate = new ioTextureRotateEffect( pState );
		pRotate->SetRotateSpeed( rkEffect.m_fArg1, bOnce );
		pEffect = pRotate;
		break;
	case TET_ANIMATE:
		pAnimate = new ioTextureAnimationEffect( pState );
		pAnimate->SetTimeAttribute( rkEffect.m_fArg2, bOnce );
		break;
	case TET_TRANSFORM:
		pXform = new ioTextureTransformEffect( pState );
		pXform->SetAttribute( rkEffect.m_XformType,
							  rkEffect.m_WaveType,
							  rkEffect.m_fBase,
							  rkEffect.m_fFrequency,
							  rkEffect.m_fPhase,
							  rkEffect.m_fAmplitude,
							  bOnce );
		pEffect = pXform;
		break;
	case TET_COLOR_TRANSFORM:
		pColorXform = new ioTextureColorTransformEffect( pState );
		pColorXform->SetAttribute( rkEffect.m_ColorXformType,
								   rkEffect.m_WaveType,
								   rkEffect.m_fBase,
								   rkEffect.m_fFrequency,
								   rkEffect.m_fPhase,
								   rkEffect.m_fAmplitude,
								   bOnce );
		pEffect = pColorXform;
		break;
	}

	if( pEffect )
	{
		m_EffectSet.insert( pEffect );
		rkEffect.m_pController = pEffect;
	}
}

void ioTextureEffectManager::UnLoadEffect( ioTextureEffect *pEffect )
{
	if( !pEffect )	return;

	TextureEffectSet::iterator iter = m_EffectSet.find( pEffect );
	if( iter != m_EffectSet.end() )
	{
		delete *iter;
		m_EffectSet.erase( iter );
	}
}

void ioTextureEffectManager::UnLoadAllEffect()
{
	TextureEffectSet::iterator iter;
	for( iter=m_EffectSet.begin() ; iter!=m_EffectSet.end() ; ++iter )
	{
		delete *iter;
	}

	m_EffectSet.clear();
}

ioTextureEffectManager& ioTextureEffectManager::GetSingleton()
{
	return Singleton<ioTextureEffectManager>::GetSingleton();
}