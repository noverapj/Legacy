

#ifndef _ioTextureTransformEffect_h_
#define _ioTextureTransformEffect_h_

class __EX ioTextureTransformEffect : public ioTextureEffect
{
protected:
	TextureTransformType m_TrasformType;
	WaveformType	m_WaveType;

	float	m_fDeltaCount;

	float	m_fBase;
	float	m_fFrequency;
	float	m_fPhase;
	float	m_fAmplitude;
	
	bool	m_bOnceLoop;
	bool	m_bDone;

protected:
	float GetAdjustedInput( float fInput );

public:
	virtual void Update( float fTimePerSec );
	virtual TextureEffectType GetType() const { return TET_TRANSFORM; }

public:
	void SetAttribute( TextureTransformType xFormType,
					   WaveformType waveType,
					   float fBase = 0.0f,
					   float fFrequency = 1.0f,
					   float fPhase = 0.0f,
					   float fAmplitude = 1.0f,
					   bool bOnce = false );

public:
	inline TextureTransformType GetTransformType() const { return m_TrasformType; }
	inline WaveformType GetWaveformType() const { return m_WaveType; }
	inline float GetBase() const { return m_fBase; }
	inline float GetFrequency() const { return m_fFrequency; }
	inline float GetPhase() const { return m_fPhase; }
	inline float GetAmplitude() const { return m_fAmplitude; }

public:
	ioTextureTransformEffect( ioTextureUnitState *pState );
	virtual ~ioTextureTransformEffect();
};

inline ioTextureTransformEffect* ToTransformEffect( ioTextureEffect *pEffect )
{
	if( pEffect == NULL || pEffect->GetType() != TET_TRANSFORM )
		return NULL;

	return dynamic_cast<ioTextureTransformEffect*>(pEffect);
}

#endif