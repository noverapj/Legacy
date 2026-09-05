

#include "stdafx.h"

#include "ioTextureEffect.h"
#include "ioTextureUnitState.h"
#include "ioTextureColorTransformEffect.h"

ioTextureColorTransformEffect::ioTextureColorTransformEffect( ioTextureUnitState *pState )
: ioTextureEffect( pState )
{
	m_TrasformType = CTT_EMISSIVE;
	m_WaveType = WFT_SINE;

	m_fDeltaCount = 0.0f;

	m_fBase		 = 0.0f;
	m_fFrequency = 1.0f;
	m_fPhase	 = 0.0f;
	m_fAmplitude = 1.0f;
	
	m_bOnceLoop = false;
	m_bDone = false;
}

ioTextureColorTransformEffect::~ioTextureColorTransformEffect()
{
}

void ioTextureColorTransformEffect::SetAttribute( ColorTransformType xFormType,
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

float ioTextureColorTransformEffect::GetAdjustedInput( float fInput )
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

void ioTextureColorTransformEffect::Update( float fTimePerSec )
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
		fOutput = (fInput*2.0f) - 1.0f;
		break;
	case WFT_INVERSE_SAWTOOTH:
		fOutput = 1.0f - (fInput * 2.0f );
		break;
	}

	float fResult = m_fBase + (( fOutput + 1.0f) * FLOAT05 * m_fAmplitude );

	m_pUnitState->SetColorTransformRate( m_TrasformType, fResult );
}

