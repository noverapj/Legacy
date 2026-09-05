

#ifndef _ioRotationAffector_h_
#define _ioRotationAffector_h_

class ioParamCommand;
class ioParticleSystem;
class ioParticleAffector;

class __EX ioRotationAffector : public ioParticleAffector
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
	class CmdStartRotAxis : public ioParamCommand
	{
		virtual std::string Get( const void *pTarget ) const;
		virtual void Set( void *pTarget, const std::string &kVal );
	};
	class CmdStartRotRange : public ioParamCommand
	{
	public:
		virtual std::string Get( const void *pTarget ) const;
		virtual void Set( void *pTarget, const std::string &kVal );
	};
	class CmdSetStartAngle : public ioParamCommand
	{
	public:
		virtual std::string Get( const void *pTarget ) const;
		virtual void Set( void *pTarget, const std::string &kVal );
	};

	static CmdRotAxis	m_RotAxisCmd;
	static CmdRotSpeed	m_RotSpeedCmd;

	static CmdStartRotAxis	m_StartRotAxisCmd;
	static CmdStartRotRange	m_StartRotRangeCmd;
	static CmdSetStartAngle m_SetStartAngleCmd;

protected:
	D3DXVECTOR3	m_vRotAxis;
	float	m_fMinRotSpeed;
	float	m_fMaxRotSpeed;

	D3DXVECTOR3 m_vStartRotAxis;
	float	m_fRotRangeStart;
	float	m_fRotRangeEnd;
	bool	m_bSetStartAngle;

public:
	virtual void InitParticle( ioParticleSystem *pSys, ioParticle *pParticle );
	virtual void AffectParticle( ioParticleSystem *pSys, float fTimePerSec );
	virtual void ParentSystemDirectionChanged( const D3DXQUATERNION &qtRotate );

	virtual bool IsSameAffector( const ioParticleAffector *pRhs ) const;
	virtual void ParametersCopy( const ioParticleAffector *pSrc );

	virtual const ioHashString& GetType() const;

public:
	public:
	void SetRotAxis( const D3DXVECTOR3 &vAxis );
	void SetRotSpeed( float fMinSpeed, float fMaxSpeed );

	void SetStartRotAxis( const D3DXVECTOR3 &vAxis );
	void SetStartRotRange( float fStart, float fEnd );
	void SetStartAngle( bool bSet );

public:
	inline const D3DXVECTOR3& GetRotAxis() const { return m_vRotAxis; }
	inline float GetMinRotSpeed() const { return m_fMinRotSpeed; }
	inline float GetMaxRotSpeed() const { return m_fMaxRotSpeed; }

	inline const D3DXVECTOR3& GetStartRotAxis() const { return m_vStartRotAxis; }
	inline float GetRotRangeStart() const { return m_fRotRangeStart; }
	inline float GetRotRangeEnd() const { return m_fRotRangeEnd; }
	inline bool  IsSetStartAngle() const { return m_bSetStartAngle; }

public:
	ioRotationAffector();
	virtual ~ioRotationAffector();
};

#endif
