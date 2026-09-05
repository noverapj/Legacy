

#ifndef _ioTexRotationAffector_h_
#define _ioTexRotationAffector_h_

class ioParamCommand;
class ioParticleSystem;
class ioParticleAffector;

class __EX ioTexRotationAffector : public ioParticleAffector
{
public:
	class CmdRotSpeed : public ioParamCommand
	{
	public:
		virtual std::string Get( const void *pTarget ) const;
		virtual void Set( void *pTarget, const std::string &kVal );
	};
	class CmdRotRange : public ioParamCommand
	{
	public:
		virtual std::string Get( const void *pTarget ) const;
		virtual void Set( void *pTarget, const std::string &kVal );
	};

	static CmdRotSpeed	m_RotSpeedCmd;
	static CmdRotRange	m_RotRangeCmd;

protected:
	float	m_fMinRotSpeed;
	float	m_fMaxRotSpeed;

	float	m_fRotRangeStart;
	float	m_fRotRangeEnd;

public:
	virtual void InitParticle( ioParticleSystem *pSys, ioParticle *pParticle );
	virtual void AffectParticle( ioParticleSystem *pSys, float fTimePerSec );

	virtual bool IsSameAffector( const ioParticleAffector *pRhs ) const;
	virtual void ParametersCopy( const ioParticleAffector *pSrc );
	virtual const ioHashString& GetType() const;

public:
	void SetRotateSpeed( float fMin, float fMax );
	void SetRotateRange( float fStart, float fEnd );
	
public:
	inline float GetRotateMinSpeed() const { return m_fMinRotSpeed; }
	inline float GetRotateMaxSpeed() const { return m_fMaxRotSpeed; }

	inline float GetRotateRangeStart() const { return m_fRotRangeStart; }
	inline float GetRotateRangeEnd() const { return m_fRotRangeEnd; }

public:
	ioTexRotationAffector();
	virtual ~ioTexRotationAffector();
};

#endif
