

#ifndef _ioTextureEffect_h_
#define _ioTextureEffect_h_

#include "io3DCommon.h"

class ioTextureUnitState;

class __EX ioTextureEffect
{
protected:	
	ioTextureUnitState *m_pUnitState;
	bool m_bStoppedUpdate;

public:
	virtual void Update( float fTimePerSec ) = 0;
	virtual TextureEffectType GetType() const = 0;

public:
	inline void StopUpdate( bool bStop ) { m_bStoppedUpdate = bStop; }
	inline bool IsStopped() const { return m_bStoppedUpdate; }

public:
	ioTextureEffect( ioTextureUnitState *pState );
	virtual ~ioTextureEffect();
};

#endif