

#ifndef _ioEmitAffector_h_
#define _ioEmitAffector_h_

#include "ioTimeRateFactor.h"

class ioParamCommand;
class ioParticleAffector;
class ioParticleSystem;

class __EX ioEmitAffector : public ioParticleAffector
{
public:
	class CmdEmitPower : public ioParamCommand
	{
	public:
		virtual std::string Get( const void *pTarget ) const;
		virtual void Set( void *pTarget, const std::string &kVal );
	};
	class CmdForceRate : public ioParamCommand
	{
	public:
		virtual std::string Get( const void *pTarget ) const;
		virtual void Set( void *pTarget, const std::string &kVal );
	};
	class CmdDirectionChange : public ioParamCommand
	{
	public:
		virtual std::string Get( const void *pTarget ) const;
		virtual void Set( void *pTarget, const std::string &kVal );
	};

	static CmdEmitPower		m_EmitPowerCmd;
	static CmdForceRate		m_ForceRateCmd;
	static CmdDirectionChange m_DirectionChangeCmd;

protected:
	ioTimeRateFactor m_ForceRateMap;
	float	m_fPower;
	bool	m_bDirectionChange;

public:
	virtual void InitParticle( ioParticleSystem *pSys, ioParticle *pParticle );
	virtual void AffectParticle( ioParticleSystem *pSys, float fTimePerSec );

	virtual void ParentSystemScaled( float fScaleRate );
	virtual bool IsSameAffector( const ioParticleAffector *pRhs ) const;
	virtual void ParametersCopy( const ioParticleAffector *pSrc );

	virtual const ioHashString& GetType() const;

public:
	void SetPower( float fPower ) { m_fPower = fPower; }
	void SetDirectionChange( bool bChange ) { m_bDirectionChange = bChange; }

public:
	void AddForceRate( float fTime, float fRate );
	void SetForceRateList( const Vector2Vec &rkForceList );

	void RemoveForceRate( int iIdx );
	void RemoveAllForceRate();

public:
	int GetForceRateCnt() const;
	float GetForceRateTime( int iIdx ) const;
	float GetForceRate( int iIdx ) const;
	float GetCurForceRate( float fTimeRate );

	inline float GetPower() const { return m_fPower; }
	inline bool IsDirectionChange() const { return m_bDirectionChange; }

public:
	ioEmitAffector();
	virtual ~ioEmitAffector();
};

#endif