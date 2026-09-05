

#ifndef _ioParticleAffector_h_
#define _ioParticleAffector_h_

#include "io3DCommon.h"

class ioParticle;
class ioParticleSystem;
class ioStringInterface;
class ioParamCommand;

class __EX ioParticleAffector : public ioStringInterface
{
public:
	class CmdWaitTime : public ioParamCommand
	{
	public:
		virtual std::string Get( const void *pTarget ) const;
		virtual void Set( void *pTarget, const std::string &kVal );
	};

	static CmdWaitTime m_WaitTimeCmd;

protected:
	float	m_fWaitTime;
	float	m_fCurTime;
	bool	m_bEnable;

protected:
	void AddBaseParameters();
	bool CheckWaitTime( float fTimePerSec );

public:
	virtual void InitParticle( ioParticleSystem *pSys, ioParticle *pParticle );
	virtual void AffectParticle( ioParticleSystem *pSys, float fTimePerSec ) = 0;
	virtual void ParentSystemScaled( float fScaleRate );
	virtual void ParentSystemDirectionChanged( const D3DXQUATERNION &qtRotate );

	virtual void Restart();
	virtual bool IsSameAffector( const ioParticleAffector *pRhs ) const;
	virtual void ParametersCopy( const ioParticleAffector *pSrc );

	virtual const ioHashString& GetType() const = 0;

public:
	void SetWaitTime( float fTime ) { m_fWaitTime = fTime; }
	void SetEnable( bool bEnable ) { m_bEnable = bEnable; }

public:
	inline float GetWaitTime() const { return m_fWaitTime; }
	inline bool IsEnable() const { return m_bEnable; }

public:
	ioParticleAffector();
	virtual ~ioParticleAffector();
};

#endif
