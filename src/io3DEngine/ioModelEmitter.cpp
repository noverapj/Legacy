
#include "stdafx.h"

#include "ioMath.h"

#include "ioMovableObject.h"
#include "ioModelParticle.h"
#include "ioModelEmitter.h"
#include "ioEmitPointGenerator.h"

#include "HelpFunc.h"

ioModelEmitCommand::CmdEmitterPos		ioModelEmitter::m_EmitterPosCmd;
ioModelEmitCommand::CmdEmitterDir		ioModelEmitter::m_EmitterDirCmd;
ioModelEmitCommand::CmdEmitterAngle		ioModelEmitter::m_EmitterAngleCmd;
ioModelEmitCommand::CmdEmitterEmitRate	ioModelEmitter::m_EmitterEmitRateCmd;
ioModelEmitCommand::CmdEmitterStartRotAxis	ioModelEmitter::m_EmitterStartRotAxisCmd;
ioModelEmitCommand::CmdEmiiterStartRotAngle	ioModelEmitter::m_EmitterStartRotAngleCmd;
ioModelEmitCommand::CmdEmitterSpeed		ioModelEmitter::m_EmitterSpeedCmd;
ioModelEmitCommand::CmdEmitterLiveTime	ioModelEmitter::m_EmitterLiveTimeCmd;
ioModelEmitCommand::CmdEmitterWaitTime	ioModelEmitter::m_EmitterWaitTimeCmd;
ioModelEmitCommand::CmdEmitterAlphaList ioModelEmitter::m_EmitterAlphaListCmd;

ioModelEmitter::ioModelEmitter( ioEmitPointGenerator *pGenerator ) 
: m_vStartRotAxis( 0.0f, 1.0f, 0.0f )
{
	m_pGenerator = pGenerator;

	m_fEmitAngle	= 0.0f;
	m_fEmissionRate = 10.0f;
	m_fRemainder	= 0.0f;

	m_fMinStartRotation = 0.0f;
	m_fMaxStartRotation = 0.0f;

	m_fMinSpeed = 1.0f;
	m_fMaxSpeed = 1.0f;

	m_fMinLiveTime = 5000.0f;
	m_fMaxLiveTime = 5000.0f;

	m_fWaitTime = 0.0f;
	m_fCurWaitTime = 0.0f;
	
	if ( CreateParamDictionary("ModelEmitter") )
    {
		AddBaseParameters();
	}
}

ioModelEmitter::~ioModelEmitter()
{
	SAFEDELETE( m_pGenerator );
}

void ioModelEmitter::AddBaseParameters()
{
	ioParamDictionary *pDict = GetParamDictionary();

	pDict->AddParameter( ioParameterDef( "emit_pos", PT_VECTOR3 ),
						 &m_EmitterPosCmd );

	pDict->AddParameter( ioParameterDef( "emit_dir", PT_VECTOR3 ),
						 &m_EmitterDirCmd );

	pDict->AddParameter( ioParameterDef( "emit_angle", PT_FLOAT ),
						 &m_EmitterAngleCmd );

	pDict->AddParameter( ioParameterDef( "emit_rate", PT_FLOAT ),
					     &m_EmitterEmitRateCmd );

	pDict->AddParameter( ioParameterDef( "start_rot_axis", PT_VECTOR3 ),
						 &m_EmitterStartRotAxisCmd );

	pDict->AddParameter( ioParameterDef( "start_rot_angle", PT_VECTOR2 ),
						 &m_EmitterStartRotAngleCmd );

	pDict->AddParameter( ioParameterDef( "emit_speed", PT_VECTOR2 ),
					     &m_EmitterSpeedCmd );

	pDict->AddParameter( ioParameterDef( "emit_live_time", PT_VECTOR2 ),
						 &m_EmitterLiveTimeCmd );

	pDict->AddParameter( ioParameterDef( "emit_wait_time", PT_FLOAT ),
						 &m_EmitterWaitTimeCmd );
	
	pDict->AddParameter( ioParameterDef( "alpha_rate", PT_VECTOR_LIST ),
						 &m_EmitterAlphaListCmd );
}

void ioModelEmitter::Restart()
{
	m_fRemainder = 0.0f;
	m_fCurWaitTime = m_fWaitTime;
}

void ioModelEmitter::InitParticle( ioModelParticle *pMdl )
{
	float fRotate = ioMath::RangeRandom( m_fMinStartRotation, m_fMaxStartRotation );
	D3DXQuaternionRotationAxis( &pMdl->m_qtOrientation, &m_vStartRotAxis, fRotate );

	pMdl->m_vPosition = m_pGenerator->GetStartPosition();
	pMdl->m_vDir = GenEmissionDir();
	pMdl->m_fMoveSpeed = GenEmissionVelocity();
	pMdl->m_fCurTime = 0.0f;
	pMdl->m_fLiveTime = GenEmissionLiveTime();
}

void ioModelEmitter::ScaleActualSize( float fScaleRate )
{
	m_fMinSpeed *= fScaleRate;
	m_fMaxSpeed *= fScaleRate;

	if( m_pGenerator )
	{
		m_pGenerator->ScaleGenerateArea( fScaleRate );
	}
}

void ioModelEmitter::RotateActualDirection( const D3DXQUATERNION &qtRotate )
{
	D3DXVECTOR3 vChangeDir;

	vChangeDir = qtRotate * GetDirection();
	D3DXVec3Normalize( &vChangeDir, &vChangeDir );
	SetDirection( vChangeDir );

	vChangeDir = qtRotate * GetStartRotAxis();
	D3DXVec3Normalize( &vChangeDir, &vChangeDir );
	SetStartRotAxis( vChangeDir );

	D3DXVECTOR3 vRotatePos = qtRotate * GetPosition();
	SetPosition( vRotatePos );
}

int ioModelEmitter::GetCurAlphaRate( float fTimeRate )
{
	return (int)( m_AlphaRateMap.GetCurFactor( fTimeRate ) * 255.0f );
}

int ioModelEmitter::GetCurEmitModelCnt( float fTime )
{
	if( m_fCurWaitTime == 0.0f )
	{
		m_fRemainder += ( m_fEmissionRate * fTime ) /FLOAT1000;

		int iRequest = (int)m_fRemainder;
		m_fRemainder -= iRequest;

		return iRequest;
	}

	m_fCurWaitTime = max( m_fCurWaitTime - fTime, 0.0f );

	return 0;
}

D3DXVECTOR3 ioModelEmitter::GenEmissionDir() const
{
	if( m_fEmitAngle != 0.0f )
	{
		float fAngle = m_pGenerator->UnitRandom() * m_fEmitAngle;
		float fRandAngle = m_pGenerator->UnitRandom() * D3DX_PI * 2.0f;

		return ioMath::Deviant( fAngle,
								fRandAngle,
								m_pGenerator->GetDefaultDirection() );
	}

	return m_pGenerator->GetDefaultDirection();
}

float ioModelEmitter::GenEmissionVelocity() const
{
	return ioMath::RangeRandom( m_fMinSpeed, m_fMaxSpeed );
}

float ioModelEmitter::GenEmissionLiveTime() const
{
	return ioMath::RangeRandom( m_fMinLiveTime, m_fMaxLiveTime );
}

void ioModelEmitter::SetPosition( const D3DXVECTOR3 &vPos )
{
	m_pGenerator->SetDefaultPosition( vPos );
}

void ioModelEmitter::SetDirection( const D3DXVECTOR3 &vDir )
{
	m_pGenerator->SetDefaultDirection( vDir );
}

void ioModelEmitter::SetEmitAngle( float fAngle )
{
	m_fEmitAngle = DEGtoRAD(fAngle);
}

void ioModelEmitter::SetStartRotAngle( float fMin, float fMax )
{
	m_fMinStartRotation = DEGtoRAD(fMin);
	m_fMaxStartRotation = DEGtoRAD(fMax);
}

void ioModelEmitter::SetModelSpeed( float fSpeed )
{
	m_fMinSpeed = fSpeed;
	m_fMaxSpeed = fSpeed;
}

void ioModelEmitter::SetModelSpeed( float fMin, float fMax )
{
	m_fMinSpeed = fMin;
	m_fMaxSpeed = fMax;
}

void ioModelEmitter::SetModelLiveTime( float fTime )
{
	m_fMinLiveTime = fTime;
	m_fMaxLiveTime = fTime;
}

void ioModelEmitter::SetModelLiveTime( float fMinTime, float fMaxTime )
{
	m_fMinLiveTime = fMinTime;
	m_fMaxLiveTime = fMaxTime;
}

void ioModelEmitter::SetWaitTime( float fTime )
{
	m_fWaitTime = fTime;
	m_fCurWaitTime = fTime;
}

const D3DXVECTOR3& ioModelEmitter::GetPosition() const
{
	return m_pGenerator->GetDefaultPosition();
}

const D3DXVECTOR3& ioModelEmitter::GetDirection() const
{
	return m_pGenerator->GetDefaultDirection();
}

float ioModelEmitter::GetEmitAngle() const
{
	return RADtoDEG( m_fEmitAngle );
}

float ioModelEmitter::GetMinStartRotAngle() const
{
	return RADtoDEG( m_fMinStartRotation );
}

float ioModelEmitter::GetMaxStartRotAngle() const
{
	return RADtoDEG( m_fMaxStartRotation );
}

void ioModelEmitter::AddAlphaRate( float fTimeRate, float fAlphaRate )
{
	m_AlphaRateMap.AddTimeFactor( fTimeRate, fAlphaRate );
}

void ioModelEmitter::SetAlphaRateList( const Vector2Vec &rkAlphaList )
{
	m_AlphaRateMap.SetTimeFactorList( rkAlphaList );
}

int ioModelEmitter::GetAlphaRateCnt() const
{
	return m_AlphaRateMap.GetFactorCnt();
}

float ioModelEmitter::GetParticleAlphaRate( int iIdx ) const
{
	return m_AlphaRateMap.GetFactor( iIdx );
}

float ioModelEmitter::GetAlphaTimeRate( int iIdx ) const
{
	return m_AlphaRateMap.GetTimeRate( iIdx );
}

void ioModelEmitter::SetParticleAlphaRate( int iIdx, float fAlphaRate )
{
	m_AlphaRateMap.SetFactor( iIdx, fAlphaRate );
}

void ioModelEmitter::RemoveParticleAlphaRate( int iIdx )
{
	m_AlphaRateMap.RemoveFactor( iIdx );
}

void ioModelEmitter::RemoveAllAlphaRate()
{
	m_AlphaRateMap.RemoveAllFactor();
}

void ioModelEmitter::SetEmitPosGenSeed( unsigned int uSeed )
{
	if( m_pGenerator )
	{
		m_pGenerator->SetRandomSeed( uSeed );
	}
}

unsigned int ioModelEmitter::GetEmitPosGenSeed() const
{
	if( m_pGenerator )
		return m_pGenerator->GetRandomSeed();

	return 0xffffffff;
}

bool ioModelEmitter::SetParameter( const ioHashString &kName, const std::string &kValue )
{
	if( ioStringInterface::SetParameter( kName, kValue ) )
		return true;

	return m_pGenerator->SetParameter( kName, kValue );
}

void ioModelEmitter::ChangeGenerator( ioEmitPointGenerator *pNewGenerator )
{
	SAFEDELETE( m_pGenerator );
	m_pGenerator = pNewGenerator;
}

const ioHashString& ioModelEmitter::GetGeneratorType() const
{
	return m_pGenerator->GetType();
}

void ioModelEmitter::ParametersCopy( const ioModelEmitter *pSrc )
{
	m_fEmitAngle = pSrc->m_fEmitAngle;
	m_fEmissionRate = pSrc->m_fEmissionRate;

	m_vStartRotAxis = pSrc->m_vStartRotAxis;
	m_fMinStartRotation = pSrc->m_fMinStartRotation;
	m_fMaxStartRotation = pSrc->m_fMaxStartRotation;

	m_fMinSpeed = pSrc->m_fMinSpeed;
	m_fMaxSpeed = pSrc->m_fMaxSpeed;

	m_fMinLiveTime = pSrc->m_fMinLiveTime;
	m_fMaxLiveTime = pSrc->m_fMaxLiveTime;

	m_fWaitTime = pSrc->m_fWaitTime;
	m_AlphaRateMap = pSrc->m_AlphaRateMap;

	if( m_pGenerator )
	{
		m_pGenerator->ParametersCopy( pSrc->GetGenerator() );
	}
}

bool ioModelEmitter::operator==( const ioModelEmitter &rhs ) const
{
	if( !m_pGenerator->IsSameGenerator( rhs.m_pGenerator ) )
		return false;

	if( m_fEmitAngle != rhs.m_fEmitAngle )
		return false;

	if( m_fEmissionRate != rhs.m_fEmissionRate )
		return false;

	if( !ioMath::IsEqual( m_vStartRotAxis, rhs.m_vStartRotAxis ) )
		return false;

	if( m_fMinStartRotation != rhs.m_fMinStartRotation )
		return false;

	if( m_fMaxStartRotation != rhs.m_fMaxStartRotation )
		return false;

	if( m_fMinSpeed != rhs.m_fMinSpeed )
		return false;

	if( m_fMaxSpeed != rhs.m_fMaxSpeed )
		return false;

	if( m_fMinLiveTime != rhs.m_fMinLiveTime )
		return false;

	if( m_fMaxLiveTime != rhs.m_fMaxLiveTime )
		return false;

	if( m_fWaitTime != rhs.m_fWaitTime )
		return false;

	if( m_AlphaRateMap != rhs.m_AlphaRateMap )
		return false;

	return true;
}

bool ioModelEmitter::operator!=( const ioModelEmitter &rhs ) const
{
	if( *this == rhs )
		return false;

	return true;
}