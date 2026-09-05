

#ifndef _ioModelEmitAffector_h_
#define _ioModelEmitAffector_h_

class ioParamCommand;
class ioModelParticleAffector;
class ioModelParticleSystem;

class __EX ioModelEmitAffector : public ioModelParticleAffector
{
public:
	class CmdEmitPower : public ioParamCommand
	{
	public:
		virtual std::string Get( const void *pTarget ) const;
		virtual void Set( void *pTarget, const std::string &kVal );
	};

	static CmdEmitPower	m_EmitPowerCmd;

protected:
	float m_fPower;

public:
	virtual void AffectModel( ioModelParticleSystem *pSys, float fTimePerSec );
	virtual void ParentSystemScaled( float fScaleRate );

	virtual bool IsSameAffector( const ioModelParticleAffector *pRhs ) const;
	virtual void ParametersCopy( const ioModelParticleAffector *pSrc );

	virtual const ioHashString& GetType() const;

public:
	void SetPower( float fPower ) { m_fPower = fPower; }
	inline float GetPower() const { return m_fPower; }

public:
	ioModelEmitAffector();
	virtual ~ioModelEmitAffector();
};

#endif