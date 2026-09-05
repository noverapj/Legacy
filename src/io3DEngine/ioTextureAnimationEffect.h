

#ifndef _ioTextureAnimationEffect_h_
#define _ioTextureAnimationEffect_h_

#include "io3DCommon.h"

class ioTextureEffect;
class ioTextureUnitState;

class __EX ioTextureAnimationEffect : public ioTextureEffect
{
protected:
	float	m_fFullTime;
	float	m_fCurTime;
	
	bool	m_bOnceLoop;
	bool	m_bDone;

public:
	void SetTimeAttribute( float fFullTime, bool bOnce = false );

public:
	virtual void Update( float fTimePerSec );
	virtual TextureEffectType GetType() const { return TET_ANIMATE; }

public:
	ioTextureAnimationEffect( ioTextureUnitState *pState );
	virtual ~ioTextureAnimationEffect();
};

inline ioTextureAnimationEffect* ToAnimateEffect( ioTextureEffect *pEffect )
{
	if( pEffect == NULL || pEffect->GetType() != TET_ANIMATE )
		return NULL;

	return dynamic_cast<ioTextureAnimationEffect*>(pEffect);
}

#endif
