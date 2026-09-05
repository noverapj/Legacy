

#ifndef _ioModelEmitter_h_
#define _ioModelEmitter_h_

#include "io3DCommon.h"
#include "ioStringInterface.h"
#include "ioModelEmitCommand.h"
#include "ioTimeRateFactor.h"

class ioModelParticle;
class ioModelParticleSystem;
class ioEmitPointGenerator;

class __EX ioModelEmitter : public ioStringInterface
{
public:
	static ioModelEmitCommand::CmdEmitterPos			m_EmitterPosCmd;
	static ioModelEmitCommand::CmdEmitterDir			m_EmitterDirCmd;
	static ioModelEmitCommand::CmdEmitterAngle			m_EmitterAngleCmd;
	static ioModelEmitCommand::CmdEmitterEmitRate		m_EmitterEmitRateCmd;
	static ioModelEmitCommand::CmdEmitterStartRotAxis	m_EmitterStartRotAxisCmd;
	static ioModelEmitCommand::CmdEmiiterStartRotAngle	m_EmitterStartRotAngleCmd;
	static ioModelEmitCommand::CmdEmitterSpeed			m_EmitterSpeedCmd;
	static ioModelEmitCommand::CmdEmitterLiveTime		m_EmitterLiveTimeCmd;
	static ioModelEmitCommand::CmdEmitterWaitTime		m_EmitterWaitTimeCmd;
	static ioModelEmitCommand::CmdEmitterAlphaList		m_EmitterAlphaListCmd;

protected:
	ioEmitPointGenerator *m_pGenerator;

	float	m_fEmitAngle;

	float	m_fEmissionRate;
	float	m_fRemainder;

	D3DXVECTOR3 m_vStartRotAxis;
	float	m_fMinStartRotation;
	float	m_fMaxStartRotation;

	float	m_fMinSpeed;
	float	m_fMaxSpeed;

	float	m_fMinLiveTime;
	float	m_fMaxLiveTime;

	float	m_fWaitTime;
	float   m_fCurWaitTime;

	ioTimeRateFactor m_AlphaRateMap;

protected:
	void AddBaseParameters();

public:
	virtual bool SetParameter( const ioHashString &kName, const std::string &kValue );

public:
	void Restart();
	void InitParticle( ioModelParticle *pMdl );

	void ScaleActualSize( float fScaleRate );
	void RotateActualDirection( const D3DXQUATERNION &qtRotate );

protected:
	D3DXVECTOR3 GenEmissionDir() const;
	float GenEmissionVelocity() const;
	float GenEmissionLiveTime() const;

public:
	void SetPosition( const D3DXVECTOR3 &vPos );
	void SetDirection( const D3DXVECTOR3 &vDir );

	void SetEmitAngle( float fAngle );
	void SetEmissionRate( float fRate ) { m_fEmissionRate = fRate; }

	void SetStartRotAxis( const D3DXVECTOR3 &vAxis ) { m_vStartRotAxis = vAxis; }
	void SetStartRotAngle( float fMin, float fMax );

	void SetModelSpeed( float fSpeed );
	void SetModelSpeed( float fMin, float fMax );

	void SetModelLiveTime( float fTime );
	void SetModelLiveTime( float fMinTime, float fMaxTime );

	void SetWaitTime( float fTime );

public:
	void AddAlphaRate( float fTimeRate, float fAlphaRate );
	void SetAlphaRateList( const Vector2Vec &rkAlphaList );
	void SetParticleAlphaRate( int iIdx, float fAlphaRate );

	void RemoveParticleAlphaRate( int iIdx );
	void RemoveAllAlphaRate();

	int GetAlphaRateCnt() const;
	float GetAlphaTimeRate( int iIdx ) const;
	float GetParticleAlphaRate( int iIdx ) const;

	int GetCurAlphaRate( float fTimeRate );

public:
	int GetCurEmitModelCnt( float fTime );

	const D3DXVECTOR3& GetPosition() const;
	const D3DXVECTOR3& GetDirection() const;

	float GetEmitAngle() const;
	inline float GetEmissionRate() const { return m_fEmissionRate; }

	inline const D3DXVECTOR3& GetStartRotAxis() const { return m_vStartRotAxis; }
	float GetMinStartRotAngle() const;
	float GetMaxStartRotAngle() const;

	inline float GetMinSpeed() const { return m_fMinSpeed; }
	inline float GetMaxSpeed() const { return m_fMaxSpeed; }

	inline float GetMinLiveTime() const { return m_fMinLiveTime; }
	inline float GetMaxLiveTime() const { return m_fMaxLiveTime; }

	inline float GetWaitTime() const { return m_fWaitTime; }

public:
	void SetEmitPosGenSeed( unsigned int uSeed );
	unsigned int GetEmitPosGenSeed() const;

public:
	void ChangeGenerator( ioEmitPointGenerator *pNewGenerator );
	ioEmitPointGenerator* GetGenerator() const { return m_pGenerator; }
	const ioHashString& GetGeneratorType() const;

public:
	virtual void ParametersCopy( const ioModelEmitter *pSrc );

public:
	bool operator==( const ioModelEmitter &rhs ) const;
	bool operator!=( const ioModelEmitter &rhs ) const;

public:
	ioModelEmitter( ioEmitPointGenerator *pGenerator );
	virtual ~ioModelEmitter();
};

#endif