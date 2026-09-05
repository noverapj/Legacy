

#ifndef _ioTextureRotateEffect_h_
#define _ioTextureRotateEffect_h_

class ioTextureEffect;
class ioTextureUnitState;

class __EX ioTextureRotateEffect : public ioTextureEffect
{
protected:
	float	m_fCurDegree;
	float	m_fRotSpeed;

	bool	m_bOnceLoop;
	bool	m_bDone;

public:
	virtual void Update( float fTimePerSec );
	virtual TextureEffectType GetType() const { return TET_ROTATE; }

public:
	void SetRotateSpeed( float fSpeed, bool bOnce );

	inline float GetRotateSpeed() const { return m_fRotSpeed; }

public:
	ioTextureRotateEffect( ioTextureUnitState *pState );
	virtual ~ioTextureRotateEffect();
};

inline ioTextureRotateEffect* ToRotateEffect( ioTextureEffect *pEffect )
{
	if( pEffect == NULL || pEffect->GetType() != TET_ROTATE )
		return NULL;

	return dynamic_cast<ioTextureRotateEffect*>(pEffect);
}

#endif