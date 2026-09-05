

#ifndef _ioParticleEmitter_h_
#define _ioParticleEmitter_h_

#include "io3DCommon.h"
#include "ioStringInterface.h"
#include "ioEmitterCommands.h"
#include "ioTimeRateFactor.h"
#include "ioParticleColorTable.h"

class ioParticle;
class ioEmitPointGenerator;

struct ParticleRGB
{
	BYTE	bRed;
	BYTE	bGreen;
	BYTE	bBlue;
	BYTE	bReserved;	// for 4byte align
};

class __EX ioParticleEmitter : public ioStringInterface
{
public:
	static ioEmitterCommands::CmdEmitterPos				m_EmitterPosCmd;
	static ioEmitterCommands::CmdEmitterDir				m_EmitterDirCmd;
	static ioEmitterCommands::CmdEmitterAngle			m_EmitterAngleCmd;
	static ioEmitterCommands::CmdEmitterEmitRate		m_EmitterEmitRateCmd;
	static ioEmitterCommands::CmdEmitterStartTexRotate	m_EmitterStartTexRotateCmd;
	static ioEmitterCommands::CmdEmitterSpeed			m_EmitterSpeedCmd;
	static ioEmitterCommands::CmdEmitterLiveTime		m_EmitterLiveTimeCmd;
	static ioEmitterCommands::CmdEmitterWaitTime		m_EmitterWaitTimeCmd;
	static ioEmitterCommands::CmdEmitterColorList		m_EmitterColorListCmd;
	static ioEmitterCommands::CmdEmitterAlphaList		m_EmitterAlphaListCmd;
	
protected:
	struct RGBFactor
	{
		float fTimeRate;
		ParticleRGB kRGB;

		bool operator==( const RGBFactor &rhs ) const
		{
			if( fTimeRate != rhs.fTimeRate )
				return false;

			if( kRGB.bRed != rhs.kRGB.bRed ||
				kRGB.bGreen != rhs.kRGB.bGreen ||
				kRGB.bBlue != rhs.kRGB.bBlue )
				return false;

			return true;
		}
		
		bool operator!=( const RGBFactor &rhs ) const
		{
			if( *this == rhs )
				return false;

			return true;
		}
	};

	class FactorTimeSort : public std::binary_function< const RGBFactor&, const RGBFactor&, bool >
	{
	public:
		bool operator()( const RGBFactor &lhs, const RGBFactor &rhs ) const
		{
			return ( lhs.fTimeRate < rhs.fTimeRate );
		}
	};

protected:
	ioEmitPointGenerator *m_pGenerator;

	float	m_fEmitAngle;

	float	m_fEmissionRate;
	float	m_fRemainder;

	float	m_fMinStartTexRotation;
	float	m_fMaxStartTexRotation;

	float	m_fMinSpeed;
	float	m_fMaxSpeed;

	float	m_fMinLiveTime;
	float	m_fMaxLiveTime;

	float	m_fWaitTime;
	float   m_fCurWaitTime;

	typedef std::vector< RGBFactor > RGBFactorList;
	RGBFactorList m_RGBFactorList;
	ioTimeRateFactor m_AlphaFactor;

	ioParticleColorTablePtr m_pColorTable;

protected:
	void AddBaseParameters();

public:
	virtual void ParametersCopy( const ioParticleEmitter *pSrc );

public:
	void Restart();

	void InitParticle( ioParticle *pParticle );
	
	void ScaleActualSize( float fScaleRate );
	void RotateActualDirection( const D3DXQUATERNION &qtRotate );

	int  GetAboutMaxQuota() const;

public:
	void SetPosition( const D3DXVECTOR3 &vPos );
	void SetDirection( const D3DXVECTOR3 &vDir );

	void SetEmitAngle( float fAngle );
	void SetEmissionRate( float fRate );

	void SetStartTexRotation( float fMin, float fMax );

	void SetParticleSpeed( float fSpeed );
	void SetParticleSpeed( float fMin, float fMax );

	void SetParticleLiveTime( float fTime );
	void SetParticleLiveTime( float fMinTime, float fMaxTime );

	void SetWaitTime( float fTime );

public:
	const D3DXVECTOR3& GetPosition() const;
	const D3DXVECTOR3& GetDirection() const;

	inline float GetEmitAngle() const { return RADtoDEG( m_fEmitAngle ); }
	inline float GetEmissionRate() const { return m_fEmissionRate; }

	inline float GetMinStartTexRotation() const { return m_fMinStartTexRotation; }
	inline float GetMaxStartTexRotation() const { return m_fMaxStartTexRotation; }

	inline float GetMinSpeed() const { return m_fMinSpeed; }
	inline float GetMaxSpeed() const { return m_fMaxSpeed; }

	inline float GetMinLiveTime() const { return m_fMinLiveTime; }
	inline float GetMaxLiveTime() const { return m_fMaxLiveTime; }
	
	inline float GetWaitTime() const { return m_fWaitTime; }

public:
	void BuildColorTable();
	void UpdateColorTable();

	void SetColorTable( ioParticleColorTablePtr &pTable );
	ioParticleColorTablePtr& GetColorTable();

protected:
	DWORD BuildColor( float fTimeRate ) const;

public:
	void AddColor( float fTimeRate, const ParticleRGB &kColor );
	void AddColor( float fTimeRate, BYTE bRed, BYTE bGreen, BYTE bBlue );
	void AddAlphaRate( float fTimeRate, float fAlphaRate );

	void SetColorList( const Vector4Vec &rkColorList );
	void SetAlphaList( const Vector2Vec &rkAlphaList );

	void SetParticleColor( int iIdx, const ParticleRGB &kRGB );
	void SetParticleAlphaRate( int iIdx, float fAlphaRate );

	int GetColorRateCnt() const;
	float GetColorTimeRate( int iIdx ) const;
	ParticleRGB GetParticleColor( int iIdx ) const;

	int GetAlphaRateCnt() const;
	float GetAlphaTimeRate( int iIdx ) const;
	float GetParticleAlphaRate( int iIdx ) const;

	void RemoveParticleColor( int iIdx );
	void RemoveAllColorRate();

	void RemoveParticleAlphaRate( int iIdx );
	void RemoveAllAlphaRate();

public:
	D3DXVECTOR3 GenEmissionDirection() const;
	DWORD GenEmissionColor( float fTimeRate );
	int GetCurEmitParticleCount( float fTime );

	inline float GenEmissionVelocity() const
	{
		return ioMath::RangeRandom( m_fMinSpeed, m_fMaxSpeed );
	}

	inline float GenEmissionLiveTime() const
	{
		return ioMath::RangeRandom( m_fMinLiveTime, m_fMaxLiveTime );
	}

public:
	void SetEmitPosGenSeed( unsigned int uSeed );
	unsigned int GetEmitPosGenSeed() const;

public:
	virtual bool SetParameter( const ioHashString &kName, const std::string &kValue );

	void ChangeGenerator( ioEmitPointGenerator *pNewGenerator );
	ioEmitPointGenerator* GetGenerator() const { return m_pGenerator; }
	const ioHashString& GetGeneratorType() const;

public:
	bool operator==( const ioParticleEmitter &rhs ) const;
	bool operator!=( const ioParticleEmitter &rhs ) const;

public:
	ioParticleEmitter( ioEmitPointGenerator *pGenerator );
	virtual ~ioParticleEmitter();
};

#endif

