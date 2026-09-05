

#include "stdafx.h"

#include "ioTextureEffect.h"
#include "ioTextureUnitState.h"
#include "ioTextureAnimationEffect.h"

ioTextureAnimationEffect::ioTextureAnimationEffect( ioTextureUnitState *pState )
: ioTextureEffect(pState)
{
	m_fFullTime = 0.0f;
	m_fCurTime  = 0.0f;

	m_bOnceLoop = false;
	m_bDone = false;
}

ioTextureAnimationEffect::~ioTextureAnimationEffect()
{
}

void ioTextureAnimationEffect::SetTimeAttribute( float fFullTime, bool bOnce )
{
	m_fFullTime = fFullTime /FLOAT1000;
	m_fCurTime  = 0.0f;

	m_bOnceLoop = false;
	m_bDone = false;
}

void ioTextureAnimationEffect::Update( float fTimePerSec )
{
	if( !m_bOnceLoop || !m_bDone )
	{
		m_fCurTime += fTimePerSec;
		while( m_fCurTime >= m_fFullTime )
		{
			m_fCurTime -= m_fFullTime;
			m_bDone = true;
		}
		while( m_fCurTime < 0.0f )
		{
			m_fCurTime += m_fFullTime;
			m_bDone = true;
		}

		if( m_bOnceLoop && m_bDone )
			m_fCurTime = m_fFullTime;
	}

	m_pUnitState->SetCurFrameByRate( m_fCurTime/m_fFullTime );
}
