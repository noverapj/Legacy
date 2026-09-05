

#ifndef _ioTextureColorTransformEffect_h_
#define _ioTextureColorTransformEffect_h_

#include "io3DCommon.h"

class ioTextureUnitState;

class __EX ioTextureColorTransformEffect : public ioTextureEffect
{
protected:	
	ColorTransformType m_TrasformType;
	WaveformType	m_WaveType;

	float	m_fDeltaCount;

	float	m_fBase;
	float	m_fFrequency;
	float	m_fPhase;
	float	m_fAmplitude;
	
	bool	m_bOnceLoop;
	bool	m_bDone;

public:
	void SetAttribute( ColorTransformType xFormType,
					   WaveformType waveType,
					   float fBase = 0.0f,
					   float fFrequency = 1.0f,
					   float fPhase = 0.0f,
					   float fAmplitude = 1.0f,
					   bool bOnce = false );

public:
	virtual void Update( float fTimePerSec );
	virtual TextureEffectType GetType() const { return TET_COLOR_TRANSFORM; }

protected:
	float GetAdjustedInput( float fInput );

public:
	inline ColorTransformType GetColorTransformType() const { return m_TrasformType; }
	inline WaveformType GetWaveformType() const { return m_WaveType; }
	inline float GetBase() const { return m_fBase; }
	inline float GetFrequency() const { return m_fFrequency; }
	inline float GetPhase() const { return m_fPhase; }
	inline float GetAmplitude() const { return m_fAmplitude; }

public:
	ioTextureColorTransformEffect( ioTextureUnitState *pState );
	virtual ~ioTextureColorTransformEffect();
};

inline ioTextureColorTransformEffect* ToColorTransformEffect( ioTextureEffect *pEffect )
{
	if( !pEffect || pEffect->GetType() != TET_COLOR_TRANSFORM )
		return NULL;

	return dynamic_cast<ioTextureColorTransformEffect*>(pEffect);
}

#endif
