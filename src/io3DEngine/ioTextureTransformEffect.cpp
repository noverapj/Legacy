

#include "stdafx.h"

#include "ioTextureEffect.h"
#include "ioTextureUnitState.h"
#include "ioTextureTransformEffect.h"

ioTextureTransformEffect::ioTextureTransformEffect( ioTextureUnitState *pState )
 : ioTextureEffect( pState )
{
	m_TrasformType = TTT_TRANSLATE_U;
	m_WaveType = WFT_SINE;

	m_fDeltaCount = 0.0f;

	m_fBase		 = 0.0f;
	m_fFrequency = 1.0f;
	m_fPhase	 = 0.0f;
	m_fAmplitude = 1.0f;
	
	m_bOnceLoop = false;
	m_bDone = false;
}

ioTextureTransformEffect::~ioTextureTransformEffect()
{
}

void ioTextureTransformEffect::SetAttribute( TextureTransformType xFormType,
										     WaveformType waveType,
											 float fBase,
											 float fFrequency,
											 float fPhase,
											 float fAmplitude,
											 bool bOnce )
{
	m_TrasformType = xFormType;
	m_WaveType	   = waveType;

	m_fBase		 = fBase;
	m_fFrequency = fFrequency;
	m_fPhase	 = fPhase;
	m_fAmplitude = fAmplitude;

	m_bOnceLoop = bOnce;
	m_bDone = false;
}

float ioTextureTransformEffect::GetAdjustedInput( float fInput )
{
	if( !m_bOnceLoop || !m_bDone )
	{
		m_fDeltaCount += fInput;
		while( m_fDeltaCount >= 1.0f )
		{
			m_fDeltaCount -= 1.0f;
			m_bDone = true;
		}

		if( m_bOnceLoop && m_bDone )
			m_fDeltaCount = 1.0f;
	}

	return m_fDeltaCount;
}

void ioTextureTransformEffect::Update( float fTimePerSec )
{
	float fInput = GetAdjustedInput( fTimePerSec * m_fFrequency );

	float fOutput = 0.0f;
	switch( m_WaveType )
	{
	case WFT_SINE:
		fOutput = sinf( fInput * D3DX_PI * 2.0f );
		break;
	case WFT_TRIANGLE:
		if( fInput < 0.25f )
		{
			fOutput = fInput * 4.0f;
		}
		else if( COMPARE( fInput, 0.25f, 0.75f ) )
		{
			fOutput = 1.0f - ( ( fInput - 0.25f ) * 4.0f );
		}
		else
		{
			fOutput = ( (fInput - 0.75f) * 4.0f ) - 1.0f;
		}
		break;
	case WFT_SQUARE:
		if( fInput <= FLOAT05 )
		{
			fOutput = 1.0f;
		}
		else
		{
			fOutput = -1.0f;
		}
		break;
	case WFT_SAWTOOTH:
		fOutput = ( fInput*2.0f ) - 1.0f;
		break;
	case WFT_INVERSE_SAWTOOTH:
		fOutput = 1.0f - ( fInput * 2.0f );
		break;
	}

	float fResult = m_fBase + (( fOutput + 1.0f) * FLOAT05 * m_fAmplitude );

	switch( m_TrasformType )
	{
	case TTT_TRANSLATE_U:
		m_pUnitState->SetTextureUScroll( fResult );
		break;
	case TTT_TRANSLATE_V:
		m_pUnitState->SetTextureVScroll( fResult );
		break;
	case TTT_SCALE_U:
		if( fResult >= 0.0f )
		{
			m_pUnitState->SetTextureUScale( 1.0f + fResult );
		}
		else
		{
			m_pUnitState->SetTextureUScale( 1.0f / -fResult );
		}
		break;
	case TTT_SCALE_V:
		if( fResult >= 0.0f )
		{
			m_pUnitState->SetTextureVScale( 1.0f + fResult );
		}
		else
		{
			m_pUnitState->SetTextureVScale( 1.0f / -fResult );
		}
		break;
	case TTT_ROTATE:
		m_pUnitState->SetTextureRotate( fResult * 360.0f );
		break;
	}
}
