

#ifndef _ioModelBipedTrailAffector_h_
#define _ioModelBipedTrailAffector_h_

#include "ioTimeRateFactor.h"
#include "ioModelParticleAffector.h"

class ioModelParticleSystem;

class __EX ioModelBipedTrailAffector : public ioModelParticleAffector
{
public:
	class CmdTrailSpeed : public ioParamCommand
	{
	public:
		virtual std::string Get( const void *pTarget ) const;
		virtual void Set( void *pTarget, const std::string &kVal );
	};
	class CmdTrailBiped : public ioParamCommand
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

	static CmdTrailSpeed m_TrailSpeedCmd;
	static CmdTrailBiped m_TrailBipedCmd;
	static CmdForceRate  m_TrailForceCmd;

protected:
	ioTimeRateFactor m_ForceRateMap;
	ioHashString m_TrailBiped;
	float m_fTrailSpeed;

public:
	virtual void AffectModel( ioModelParticleSystem *pSys, float fTimePerSec );
	virtual void ParentSystemScaled( float fScaleRate );

	virtual bool IsSameAffector( const ioModelParticleAffector *pRhs ) const;
	virtual void ParametersCopy( const ioModelParticleAffector *pSrc );

	virtual const ioHashString& GetType() const;

public:
	void SetTrailSpeed( float fSpeed );
	void SetTrailBiped( const ioHashString &rkName );

public:
	inline float GetTrailSpeed() const { return m_fTrailSpeed; }
	inline const ioHashString& GetTrailBiped() const { return m_TrailBiped; }

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
	ioModelBipedTrailAffector();
	virtual ~ioModelBipedTrailAffector();
};

#endif
