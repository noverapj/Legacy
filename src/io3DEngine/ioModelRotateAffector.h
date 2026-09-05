

#ifndef _ioModelRotateAffector_h_
#define _ioModelRotateAffector_h_

class ioModelParticleAffector;
class ioModelParticle;
class ioParamCommand;

class __EX ioModelRotateAffector : public ioModelParticleAffector
{
public:
	class CmdRotAxis : public ioParamCommand
	{
		virtual std::string Get( const void *pTarget ) const;
		virtual void Set( void *pTarget, const std::string &kVal );
	};
	class CmdRotSpeed : public ioParamCommand
	{
		virtual std::string Get( const void *pTarget ) const;
		virtual void Set( void *pTarget, const std::string &kVal );
	};

	static CmdRotAxis		m_RotAxisCmd;
	static CmdRotSpeed		m_RotSpeedCmd;

protected:
	D3DXVECTOR3	m_vAxis;
	float	m_fMinRotSpeed;
	float	m_fMaxRotSpeed;

public:
	virtual void InitModel( ioModelParticle *pMdl );
	virtual void AffectModel( ioModelParticleSystem *pSys, float fTimePerSec );
	virtual void ParentSystemDirectionChanged( const D3DXQUATERNION &qtRotate );

	virtual bool IsSameAffector( const ioModelParticleAffector *pRhs ) const;
	virtual void ParametersCopy( const ioModelParticleAffector *pSrc );

	virtual const ioHashString& GetType() const;

public:
	void SetRotAxis( const D3DXVECTOR3 &vAxis ) { m_vAxis = vAxis; }
	void SetRotSpeed( float fMinSpeed, float fMaxSpeed );

public:
	inline const D3DXVECTOR3& GetRotAxis() const { return m_vAxis; }
	inline float GetMinRotSpeed() const { return m_fMinRotSpeed; }
	inline float GetMaxRotSpeed() const { return m_fMaxRotSpeed; }

public:
	ioModelRotateAffector();
	virtual ~ioModelRotateAffector();
};

#endif