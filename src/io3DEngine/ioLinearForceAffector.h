

#ifndef _ioLinearForceAffector_h_
#define _ioLinearForceAffector_h_

#include "ioTimeRateFactor.h"

class ioParamCommand;
class ioParticleSystem;
class ioParticleAffector;

class __EX ioLinearForceAffector : public ioParticleAffector
{
public:
	class CmdForceVector : public ioParamCommand
	{
	public:
		virtual std::string Get( const void *pTarget ) const;
		virtual void Set( void *pTarget, const std::string &kVal );
	};

	class CmdForceMethod : public ioParamCommand
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

	enum ForceMethod
	{
		FORCE_ADD,
		FORCE_AVERAGE
	};

	static CmdForceVector m_ForceVectorCmd;
	static CmdForceMethod m_ForceMethodCmd;
	static CmdForceRate   m_ForceRateCmd;

protected:
	D3DXVECTOR3 m_vForceDir;
	ForceMethod m_eForceMethod;

	ioTimeRateFactor m_ForceRateMap;

public:
	virtual void AffectParticle( ioParticleSystem *pSys, float fTimePerSec );
	virtual void ParentSystemScaled( float fScaleRate );
	virtual void ParentSystemDirectionChanged( const D3DXQUATERNION &qtRotate );

	virtual bool IsSameAffector( const ioParticleAffector *pRhs ) const;
	virtual void ParametersCopy( const ioParticleAffector *pSrc );

	virtual const ioHashString& GetType() const;

public:
	void SetForceDir( const D3DXVECTOR3 &vForceDir );
	void SetForceDir( float fForceX, float fForceY, float fForceZ );

	void SetForceMethod( ForceMethod eMethod );

	void AddForceRate( float fTime, float fRate );
	void SetForceRateList( const Vector2Vec &rkForceList );
	void RemoveForceRate( int iIdx );
	void RemoveAllForceRate();
	
public:
	const D3DXVECTOR3& GetForceDir() const { return m_vForceDir; }
	ForceMethod GetForceMethod() const { return m_eForceMethod; }

	int GetForceRateCnt() const;
	float GetForceRateTime( int iIdx ) const;
	float GetForceRate( int iIdx ) const;
	float GetCurForceRate( float fTimeRate );

public:
	ioLinearForceAffector();
	virtual ~ioLinearForceAffector();
};

#endif
