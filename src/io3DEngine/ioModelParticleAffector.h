

#ifndef _ioModelParticleAffector_h_
#define _ioModelParticleAffector_h_

#include "io3DCommon.h"

class ioStringInterface;
class ioParamCommand;
class ioModelParticle;
class ioModelParticleSystem;

class __EX ioModelParticleAffector : public ioStringInterface
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
	float m_fWaitTime;
	float m_fCurTime;
	bool m_bEnable;

protected:
	void AddBaseParameters();
	bool CheckWaitTime( float fTimePerSec );

public:
	virtual void InitModel( ioModelParticle *pMdl );
	virtual void AffectModel( ioModelParticleSystem *pSys, float fTimePerSec ) = 0;
	virtual void ParentSystemScaled( float fScaleRate );
	virtual void ParentSystemDirectionChanged( const D3DXQUATERNION &qtRotate );

public:
	virtual void Restart();
	virtual bool IsSameAffector( const ioModelParticleAffector *pRhs ) const;
	virtual void ParametersCopy( const ioModelParticleAffector *pSrc );

	virtual const ioHashString& GetType() const = 0;

public:
	void SetWaitTime( float fTime ) { m_fWaitTime = fTime; }
	void SetEnable( bool bEnable ) { m_bEnable = bEnable; }

public:
	inline float GetWaitTime() const { return m_fWaitTime; }
	inline bool IsEnable() const { return m_bEnable; }

public:
	ioModelParticleAffector();
	virtual ~ioModelParticleAffector();
};

#endif
