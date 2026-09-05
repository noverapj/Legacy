

#ifndef _ioTextureScrollEffect_h_
#define _ioTextureScrollEffect_h_

class ioTextureEffect;

class __EX ioTextureScrollEffect : public ioTextureEffect
{
protected:
	float	m_fCurUCoord;
	float	m_fCurVCoord;

	float	m_fUSpeed;
	float	m_fVSpeed;

	bool	m_bOnceLoop;
	bool	m_bUEnded;
	bool	m_bVEnded;

public:
	void SetScrollSpeed( float fUSpeed, float fVSpeed, bool bOnce );

	inline float GetUSpeed() const { return m_fUSpeed; }
	inline float GetVSpeed() const { return m_fVSpeed; }

public:
	virtual void Update( float fTimePerSec );
	virtual TextureEffectType GetType() const { return TET_SCROLL; }

public:
	ioTextureScrollEffect( ioTextureUnitState *pState );
	virtual ~ioTextureScrollEffect();
};

inline ioTextureScrollEffect* ToScrollEffect( ioTextureEffect *pEffect )
{
	if( pEffect == NULL || pEffect->GetType() != TET_SCROLL )
		return NULL;

	return dynamic_cast<ioTextureScrollEffect*>(pEffect);
}

#endif