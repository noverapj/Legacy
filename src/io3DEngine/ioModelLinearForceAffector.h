

#ifndef _ioModelLinearForceAffector_h_
#define _ioModelLinearForceAffector_h_

#include "ioTimeRateFactor.h"

class ioModelParticleAffector;
class ioParamCommand;
class ioModelParticleSystem;

class __EX ioModelLinearForceAffector : public ioModelParticleAffector
{
public:
	class CmdForceVector : public ioParamCommand
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

	static CmdForceVector m_ForceVectorCmd;
	static CmdForceRate   m_ForceRateCmd;

protected:
	ioTimeRateFactor m_ForceRateMap;
	D3DXVECTOR3 m_vForceDir;

public:
	virtual void AffectModel( ioModelParticleSystem *pSys, float fTimePerSec );
	virtual void ParentSystemScaled( float fScaleRate );
	virtual void ParentSystemDirectionChanged( const D3DXQUATERNION &qtRotate );

	virtual bool IsSameAffector( const ioModelParticleAffector *pRhs ) const;
	virtual void ParametersCopy( const ioModelParticleAffector *pSrc );

	virtual const ioHashString& GetType() const;

public:
	void SetForceDir( const D3DXVECTOR3 &vForceDir ) { m_vForceDir = vForceDir; }
	inline const D3DXVECTOR3& GetForceDir() const { return m_vForceDir; }

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

public:
	ioModelLinearForceAffector();
	virtual ~ioModelLinearForceAffector();
};

#endif
