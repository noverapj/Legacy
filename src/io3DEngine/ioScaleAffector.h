

#ifndef _ioScaleAffector_h_
#define _ioScaleAffector_h_

class ioParamCommand;
class ioParticleSystem;
class ioParticleAffector;

class __EX ioScaleAffector : public ioParticleAffector
{
public:
	class CmdScaleRate : public ioParamCommand
	{
	public:
		std::string Get( const void *pTarget ) const;
		void Set( void *pTarget, const std::string &kVal );
	};

	static CmdScaleRate	m_ScaleRateCmd;

protected:
	struct ScaleFactor
	{
		float fTimeRate;
		float fScaleX;
		float fScaleY;

		ScaleFactor( float fTime, float fX, float fY )
		{
			fTimeRate = fTime;
			fScaleX = fX;
			fScaleY = fY;
		}
	};

	class FactorSort : public std::binary_function< const ScaleFactor&, const ScaleFactor&, bool >
	{
	public:
		bool operator()( const ScaleFactor &lhs, const ScaleFactor &rhs ) const
		{
			return ( lhs.fTimeRate < rhs.fTimeRate );
		}
	};

protected:
	typedef std::vector< ScaleFactor > ScaleFactorList;
	ScaleFactorList m_ScaleFactorList;

public:
	virtual void AffectParticle( ioParticleSystem *pSys,
								 float fTimePerSec );

public:
	virtual bool IsSameAffector( const ioParticleAffector *pRhs ) const;
	virtual void ParametersCopy( const ioParticleAffector *pSrc );
	virtual const ioHashString& GetType() const;

public:
	void AddScaleRate( float fTimeRate, float fScaleX, float fScaleY );
	void AddScaleRate( float fTimeRate, const D3DXVECTOR2 &vScale );
	void RemoveScaleRate( int iIdx );
	void RemoveAllScaleRate();

	void SetScaleRateList( const Vector3Vec &rkList );
	D3DXVECTOR2 GenScaleRate( float fTimeRate );

public:
	float GetScaleTimeRate( int iIdx ) const;
	D3DXVECTOR2 GetScaleRate( int iIdx ) const;

	inline int GetScaleRateCnt() const { return m_ScaleFactorList.size(); }

public:
	ioScaleAffector();
	virtual ~ioScaleAffector();
};

#endif
