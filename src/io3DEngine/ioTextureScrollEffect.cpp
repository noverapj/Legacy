

#include "stdafx.h"

#include "ioTextureEffect.h"
#include "ioTextureUnitState.h"
#include "ioTextureScrollEffect.h"

ioTextureScrollEffect::ioTextureScrollEffect( ioTextureUnitState *pState ) : ioTextureEffect(pState)
{
	m_fCurUCoord = 0.0f;
	m_fCurVCoord = 0.0f;

	m_fUSpeed = 0.0f;
	m_fVSpeed = 0.0f;

	m_bOnceLoop = false;
	m_bUEnded = false;
	m_bVEnded = false;
}

ioTextureScrollEffect::~ioTextureScrollEffect()
{
}

void ioTextureScrollEffect::SetScrollSpeed( float fUSpeed, float fVSpeed, bool bOnce )
{
	m_fUSpeed = -fUSpeed;
	m_fVSpeed = -fVSpeed;

	m_bOnceLoop = bOnce;
	m_bUEnded = false;
	m_bVEnded = false;
}

void ioTextureScrollEffect::Update( float fTimePerSec )
{
	if( !m_bOnceLoop || !m_bUEnded )
	{
		m_fCurUCoord += m_fUSpeed * fTimePerSec;
		if( m_fUSpeed > 0.0f )
		{
			while( m_fCurUCoord >= 1.0f )
			{
				m_fCurUCoord -= 1.0f;
				m_bUEnded = true;
			}
		}
		else
		{
			while( m_fCurUCoord <= -1.0f )
			{
				m_fCurUCoord += 1.0f;
				m_bUEnded = true;
			}
		}

		if( m_bOnceLoop && m_bUEnded )
			m_fCurUCoord = 1.0f;
	}
	
	if( !m_bOnceLoop || !m_bVEnded )
	{
		m_fCurVCoord += m_fVSpeed * fTimePerSec;
		if( m_fVSpeed > 0.0f )
		{
			while( m_fCurVCoord >= 1.0f )
			{
				m_fCurVCoord -= 1.0f;
				m_bVEnded = true;
			}
		}
		else
		{
			while( m_fCurVCoord <= -1.0f )
			{
				m_fCurVCoord += 1.0f;
				m_bVEnded = true;
			}
		}

		if( m_bOnceLoop && m_bVEnded )
			m_fCurUCoord = 1.0f;
	}
	
	m_pUnitState->SetTextureScroll( m_fCurUCoord, m_fCurVCoord );
}

