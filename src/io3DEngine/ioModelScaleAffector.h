

#ifndef _ioModelScaleAffector_h_
#define _ioModelScaleAffector_h_

class ioParamCommand;
class ioModelParticleAffector;
class ioModelParticle;
class ioModelParticleSystem;

class __EX ioModelScaleAffector : public ioModelParticleAffector
{
public:
	class CmdScaleRate : public ioParamCommand
	{
		virtual std::string Get( const void *pTarget ) const;
		virtual void Set( void *pTarget, const std::string &kVal );
	};

	static CmdScaleRate		m_ScaleRateCmd;

protected:
	typedef std::map< float, D3DXVECTOR3 > ModelScaleRateMap;
	ModelScaleRateMap m_ScaleMap;

public:
	virtual void InitModel( ioModelParticle *pMdl );
	virtual void AffectModel( ioModelParticleSystem *pSys, float fTimePerSec );
	virtual void ParentSystemScaled( float fScaleRate );

	virtual bool IsSameAffector( const ioModelParticleAffector *pRhs ) const;
	virtual void ParametersCopy( const ioModelParticleAffector *pSrc );

	virtual const ioHashString& GetType() const;

public:
	void AddScaleRate( float fTime, const D3DXVECTOR3 &vScale );
	void AddScaleRate( float fTime, float xScale, float yScale, float zScale );
	void RemoveScaleRate( int iIdx );
	void RemoveAllScaleRate();

	D3DXVECTOR3 GetScaleRate( float fTime ) const;
	D3DXVECTOR3 GetScaleRateByIdx( int iIdx ) const;
	float GetScaleRateTime( int iIdx ) const;
	int GetScaleRateCnt() const;

public:
	ioModelScaleAffector();
	virtual ~ioModelScaleAffector();
};

#endif