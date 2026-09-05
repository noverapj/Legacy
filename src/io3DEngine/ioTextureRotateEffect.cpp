

#include "stdafx.h"

#include "ioTextureEffect.h"
#include "ioTextureUnitState.h"
#include "ioTextureRotateEffect.h"

ioTextureRotateEffect::ioTextureRotateEffect( ioTextureUnitState *pState ) 
: ioTextureEffect( pState )
{
	m_fCurDegree = 0.0f;
	m_fRotSpeed  = 0.0f;

	m_bOnceLoop = false;
	m_bDone = false;
}

ioTextureRotateEffect::~ioTextureRotateEffect()
{
}

void ioTextureRotateEffect::SetRotateSpeed( float fSpeed, bool bOnce )
{
	m_fRotSpeed = -fSpeed;

	m_bOnceLoop = bOnce;
	m_bDone = false;
}

void ioTextureRotateEffect::Update( float fTimePerSec )
{
	if( !m_bOnceLoop || !m_bDone )
	{
		m_fCurDegree += m_fRotSpeed * fTimePerSec;
		if( m_fRotSpeed > 0.0f )
		{
			while( m_fCurDegree >= 360.0f )
			{
				m_fCurDegree -= 360.0f;
				m_bDone = true;
			}
		}
		else if( m_fRotSpeed < 0.0f )
		{
			while( m_fCurDegree <= -360.0f )
			{
				m_fCurDegree += 360.0f;
				m_bDone = true;
			}
		}
		
		if( m_bOnceLoop && m_bDone )
			m_fCurDegree = 0.0f;
	}

	m_pUnitState->SetTextureRotate( m_fCurDegree );
}


