

#include "stdafx.h"

#include "ioMath.h"

#include "ioParticle.h"
#include "ioParticleEmitter.h"
#include "ioParticleColorTable.h"
#include "ioEffectFactory.h"

#include "ioEmitPointGenerator.h"
#include "HelpFunc.h"

ioEmitterCommands::CmdEmitterPos			ioParticleEmitter::m_EmitterPosCmd;
ioEmitterCommands::CmdEmitterDir			ioParticleEmitter::m_EmitterDirCmd;
ioEmitterCommands::CmdEmitterAngle			ioParticleEmitter::m_EmitterAngleCmd;
ioEmitterCommands::CmdEmitterEmitRate		ioParticleEmitter::m_EmitterEmitRateCmd;
ioEmitterCommands::CmdEmitterStartTexRotate	ioParticleEmitter::m_EmitterStartTexRotateCmd;
ioEmitterCommands::CmdEmitterSpeed			ioParticleEmitter::m_EmitterSpeedCmd;
ioEmitterCommands::CmdEmitterLiveTime		ioParticleEmitter::m_EmitterLiveTimeCmd;
ioEmitterCommands::CmdEmitterWaitTime		ioParticleEmitter::m_EmitterWaitTimeCmd;
ioEmitterCommands::CmdEmitterColorList		ioParticleEmitter::m_EmitterColorListCmd;
ioEmitterCommands::CmdEmitterAlphaList		ioParticleEmitter::m_EmitterAlphaListCmd;

ioParticleEmitter::ioParticleEmitter( ioEmitPointGenerator *pGenerator )
{
	m_pGenerator = pGenerator;

	m_fEmitAngle	= 0.0f;
	m_fEmissionRate = 10.0f;
	m_fRemainder	= 0.0f;

	m_fMinStartTexRotation = 0.0f;
	m_fMaxStartTexRotation = 0.0f;

	m_fMinSpeed = 1.0f;
	m_fMaxSpeed = 1.0f;

	m_fMinLiveTime = 5.0f;
	m_fMaxLiveTime = 5.0f;

	m_fWaitTime = 0.0f;
	m_fCurWaitTime = 0.0f;

	RGBFactor kFactor;
	kFactor.fTimeRate = 0.0f;
	kFactor.kRGB.bRed   = 255;
	kFactor.kRGB.bGreen = 255;
	kFactor.kRGB.bBlue  = 255;
	m_RGBFactorList.push_back( kFactor );

	if( CreateParamDictionary("ParticleEmitter") )
    {
		AddBaseParameters();
	}
}

ioParticleEmitter::~ioParticleEmitter()
{
	SAFEDELETE( m_pGenerator );
}

void ioParticleEmitter::AddBaseParameters()
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

	pDict->AddParameter( ioParameterDef( "emit_start_tex_rotation", PT_VECTOR2 ),
						 &m_EmitterStartTexRotateCmd );

	pDict->AddParameter( ioParameterDef( "emit_speed", PT_VECTOR2 ),
					     &m_EmitterSpeedCmd );

	pDict->AddParameter( ioParameterDef( "emit_live_time", PT_VECTOR2 ),
						 &m_EmitterLiveTimeCmd );

	pDict->AddParameter( ioParameterDef( "emit_wait_time", PT_FLOAT ),
						 &m_EmitterWaitTimeCmd );

	pDict->AddParameter( ioParameterDef( "color_rate", PT_VECTOR_LIST ),
						 &m_EmitterColorListCmd );

	pDict->AddParameter( ioParameterDef( "alpha_rate", PT_VECTOR_LIST ),
						 &m_EmitterAlphaListCmd );
}

void ioParticleEmitter::ParametersCopy( const ioParticleEmitter *pSrc )
{
	m_fEmitAngle = pSrc->m_fEmitAngle;
	m_fEmissionRate = pSrc->m_fEmissionRate;

	m_fMinStartTexRotation = pSrc->m_fMinStartTexRotation;
	m_fMaxStartTexRotation = pSrc->m_fMaxStartTexRotation;

	m_fMinSpeed = pSrc->m_fMinSpeed;
	m_fMaxSpeed = pSrc->m_fMaxSpeed;

	m_fMinLiveTime = pSrc->m_fMinLiveTime;
	m_fMaxLiveTime = pSrc->m_fMaxLiveTime;

	m_fWaitTime = pSrc->m_fWaitTime;

	m_RGBFactorList = pSrc->m_RGBFactorList;
	m_AlphaFactor   = pSrc->m_AlphaFactor;

	if( m_pGenerator )
	{
		m_pGenerator->ParametersCopy( pSrc->GetGenerator() );
	}
}

void ioParticleEmitter::SetPosition( const D3DXVECTOR3 &vPos )
{
	m_pGenerator->SetDefaultPosition( vPos );
}

void ioParticleEmitter::SetDirection( const D3DXVECTOR3 &vDir )
{
	m_pGenerator->SetDefaultDirection( vDir );
}

void ioParticleEmitter::SetEmissionRate( float fRate )
{
	m_fEmissionRate = fRate;
}

void ioParticleEmitter::SetEmitAngle( float fAngle )
{
	m_fEmitAngle = DEGtoRAD( fAngle );
}

void ioParticleEmitter::SetStartTexRotation( float fMin, float fMax )
{
	m_fMinStartTexRotation = fMin;
	m_fMaxStartTexRotation = fMax;
}

void ioParticleEmitter::SetParticleSpeed( float fSpeed )
{
	m_fMinSpeed = m_fMaxSpeed = fSpeed;
}

void ioParticleEmitter::SetParticleSpeed( float fMin, float fMax )
{
	m_fMinSpeed = min( fMin, fMax );
	m_fMaxSpeed = max( fMin, fMax );
}

void ioParticleEmitter::SetParticleLiveTime( float fTime )
{
	m_fMinLiveTime = m_fMaxLiveTime = fTime;
}

void ioParticleEmitter::SetParticleLiveTime( float fMinTime, float fMaxTime )
{
	m_fMinLiveTime = min( fMinTime, fMaxTime );
	m_fMaxLiveTime = max( fMinTime, fMaxTime );
}

void ioParticleEmitter::SetWaitTime( float fTime )
{
	m_fWaitTime = fTime;
	m_fCurWaitTime = m_fWaitTime;
}

void ioParticleEmitter::BuildColorTable()
{
	m_pColorTable = ioParticleColorTablePtr( new ioParticleColorTable );

	UpdateColorTable();
}

void ioParticleEmitter::UpdateColorTable()
{
	if( m_pColorTable.IsNull() )	return;

	float fRate = 0.0f;
	float fAddAmt = 1.0f / FLOAT_COLOR_TABLE_RATE;
	for( int i=0 ; i<MAX_COLOR_TABLE ; i++ )
	{
		m_pColorTable->SetColor( i, BuildColor( fRate) );
		fRate += fAddAmt;
	}
}

void ioParticleEmitter::SetColorTable( ioParticleColorTablePtr &pTable )
{
	m_pColorTable = pTable;
}

ioParticleColorTablePtr& ioParticleEmitter::GetColorTable()
{
	return m_pColorTable;
}

void ioParticleEmitter::AddColor( float fTimeRate, const ParticleRGB &kColor )
{
	RGBFactorList::iterator iter;
	for( iter=m_RGBFactorList.begin() ; iter!=m_RGBFactorList.end() ; ++iter )
	{
		if( iter->fTimeRate == fTimeRate )
		{
			iter->kRGB = kColor;
			return;
		}
	}

	RGBFactor kFactor;
	kFactor.fTimeRate = fTimeRate;
	kFactor.kRGB = kColor;
	m_RGBFactorList.push_back( kFactor );
	
	if( m_RGBFactorList.size() > 1 )
	{
		std::sort( m_RGBFactorList.begin(), m_RGBFactorList.end(), FactorTimeSort() );
	}
}

void ioParticleEmitter::AddColor( float fTimeRate, BYTE bRed, BYTE bGreen, BYTE bBlue )
{
	ParticleRGB kColor;
	kColor.bRed   = bRed;
	kColor.bGreen = bGreen;
	kColor.bBlue  = bBlue;

	AddColor( fTimeRate, kColor );
}

void ioParticleEmitter::SetColorList( const Vector4Vec &rkColorList )
{
	m_RGBFactorList.clear();
	m_RGBFactorList.reserve( rkColorList.size() );

	Vector4Vec::const_iterator iter;
	for( iter=rkColorList.begin() ; iter!=rkColorList.end() ; ++iter )
	{
		RGBFactor kFactor;
		kFactor.fTimeRate = iter->x;
		kFactor.kRGB.bRed   = (BYTE)iter->y;
		kFactor.kRGB.bGreen = (BYTE)iter->z;
		kFactor.kRGB.bBlue  = (BYTE)iter->w;
		m_RGBFactorList.push_back( kFactor );
	}
}

void ioParticleEmitter::AddAlphaRate( float fTimeRate, float fAlphaRate )
{
	m_AlphaFactor.AddTimeFactor( fTimeRate, fAlphaRate );
}

void ioParticleEmitter::SetAlphaList( const Vector2Vec &rkAlphaList )
{
	m_AlphaFactor.SetTimeFactorList( rkAlphaList );
}

const D3DXVECTOR3& ioParticleEmitter::GetPosition() const
{
	return m_pGenerator->GetDefaultPosition();
}

const D3DXVECTOR3& ioParticleEmitter::GetDirection() const
{
	return m_pGenerator->GetDefaultDirection();
}

int ioParticleEmitter::GetColorRateCnt() const
{
	return m_RGBFactorList.size();
}

ParticleRGB ioParticleEmitter::GetParticleColor( int iIdx ) const
{
	if( COMPARE( iIdx, 0, GetColorRateCnt() ) )
		return m_RGBFactorList[iIdx].kRGB;

	return ParticleRGB();
}

float ioParticleEmitter::GetColorTimeRate( int iIdx ) const
{
	if( COMPARE( iIdx, 0, GetColorRateCnt() ) )
		return m_RGBFactorList[iIdx].fTimeRate;

	return 0.0f;
}

void ioParticleEmitter::SetParticleColor( int iIdx, const ParticleRGB &kRGB )
{
	if( COMPARE( iIdx, 0, GetColorRateCnt() ) )
	{
		m_RGBFactorList[iIdx].kRGB = kRGB;
	}
}

void ioParticleEmitter::RemoveParticleColor( int iIdx )
{
	if( COMPARE( iIdx, 0, GetColorRateCnt() ) )
	{
		m_RGBFactorList.erase( m_RGBFactorList.begin() + iIdx );
	}
}

void ioParticleEmitter::RemoveAllColorRate()
{
	m_RGBFactorList.clear();
}

DWORD ioParticleEmitter::BuildColor( float fTimeRate ) const
{
	if( m_RGBFactorList.empty() )
		return 0xffffffff;

	RGBFactorList::const_iterator iterBefore, iterAfter;
	for( iterAfter=m_RGBFactorList.begin() ; iterAfter!=m_RGBFactorList.end() ; ++iterAfter )
	{
		if( iterAfter->fTimeRate > fTimeRate )
			break;
	}

	ParticleRGB kRGB;
	if( iterAfter == m_RGBFactorList.end() )
	{
		--iterAfter;
		kRGB = iterAfter->kRGB;
	}
	else if( iterAfter == m_RGBFactorList.begin() )
	{
		kRGB = iterAfter->kRGB;
	}
	else
	{
		iterBefore = iterAfter;
		--iterBefore;

		ParticleRGB kRGBBefore, kRGBAfter;
		kRGBBefore = iterBefore->kRGB;
		kRGBAfter  = iterAfter->kRGB;

		float fBlend = (fTimeRate - iterBefore->fTimeRate) /
					   (iterAfter->fTimeRate - iterBefore->fTimeRate);

		kRGB.bRed   = kRGBBefore.bRed * ( 1.0f - fBlend ) + kRGBAfter.bRed * fBlend ;
		kRGB.bGreen = kRGBBefore.bGreen * ( 1.0f - fBlend ) + kRGBAfter.bGreen * fBlend;
		kRGB.bBlue  = kRGBBefore.bBlue * ( 1.0f - fBlend ) + kRGBAfter.bBlue * fBlend;
	}

	BYTE bAlpha = (BYTE)( m_AlphaFactor.GetCurFactor( fTimeRate ) * 255.0f );

	return D3DCOLOR_ARGB( bAlpha, kRGB.bRed, kRGB.bGreen, kRGB.bBlue );
}

int ioParticleEmitter::GetAlphaRateCnt() const
{
	return m_AlphaFactor.GetFactorCnt();
}

float ioParticleEmitter::GetParticleAlphaRate( int iIdx ) const
{
	return m_AlphaFactor.GetFactor( iIdx );
}

float ioParticleEmitter::GetAlphaTimeRate( int iIdx ) const
{
	return m_AlphaFactor.GetTimeRate( iIdx );
}

void ioParticleEmitter::SetParticleAlphaRate( int iIdx, float fAlphaRate )
{
	m_AlphaFactor.SetFactor( iIdx, fAlphaRate );
}

void ioParticleEmitter::RemoveParticleAlphaRate( int iIdx )
{
	m_AlphaFactor.RemoveFactor( iIdx );
}

void ioParticleEmitter::RemoveAllAlphaRate()
{
	m_AlphaFactor.RemoveAllFactor();
}

void ioParticleEmitter::Restart()
{
	m_fRemainder = 0.0f;
	m_fCurWaitTime = m_fWaitTime;
}

void ioParticleEmitter::InitParticle( ioParticle *pParticle )
{
	pParticle->m_vPos       = m_pGenerator->GetStartPosition();
	pParticle->m_vDir       = GenEmissionDirection();
	pParticle->m_fMoveSpeed = GenEmissionVelocity();

	if( !m_pColorTable.IsNull() )
		pParticle->m_dwColor = m_pColorTable->GetStartColor();
	else
		pParticle->m_dwColor = 0xffffffff;

	pParticle->m_fCurTime   = 0.0f;
	pParticle->m_fLiveTime  = GenEmissionLiveTime();

	float fTexRotation = ioMath::RangeRandom( m_fMinStartTexRotation, m_fMaxStartTexRotation );
	pParticle->SetTexRotation( fTexRotation );
}

void ioParticleEmitter::ScaleActualSize( float fScaleRate )
{
	m_fMinSpeed *= fScaleRate;
	m_fMaxSpeed *= fScaleRate;

	if( m_pGenerator )
	{
		m_pGenerator->ScaleGenerateArea( fScaleRate );
	}
}

void ioParticleEmitter::RotateActualDirection( const D3DXQUATERNION &qtRotate )
{
	D3DXVECTOR3 vChangeDir = qtRotate * GetDirection();
	D3DXVec3Normalize( &vChangeDir, &vChangeDir );
	SetDirection( vChangeDir );

	D3DXVECTOR3 vRotatePos = qtRotate * GetPosition();
	SetPosition( vRotatePos );
}

int ioParticleEmitter::GetAboutMaxQuota() const
{
	float fLiveSec = m_fMaxLiveTime * 0.001f;
	fLiveSec = max( m_fEmissionRate * fLiveSec, 1.0f );

	return (int)fLiveSec;
}

int ioParticleEmitter::GetCurEmitParticleCount( float fTime )
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

D3DXVECTOR3 ioParticleEmitter::GenEmissionDirection() const
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

DWORD ioParticleEmitter::GenEmissionColor( float fTimeRate )
{
	if( !m_pColorTable.IsNull() )
		return m_pColorTable->GetColor( fTimeRate );

	return 0xffffffff;
}

void ioParticleEmitter::SetEmitPosGenSeed( unsigned int uSeed )
{
	if( m_pGenerator )
	{
		m_pGenerator->SetRandomSeed( uSeed );
	}
}

unsigned int ioParticleEmitter::GetEmitPosGenSeed() const
{
	if( m_pGenerator )
		return m_pGenerator->GetRandomSeed();

	return 0xffffffff;
}

bool ioParticleEmitter::SetParameter( const ioHashString &kName, const std::string &kValue )
{
	if( ioStringInterface::SetParameter( kName, kValue ) )
		return true;

	return m_pGenerator->SetParameter( kName, kValue );
}

void ioParticleEmitter::ChangeGenerator( ioEmitPointGenerator *pNewGenerator )
{
	SAFEDELETE( m_pGenerator );
	m_pGenerator = pNewGenerator;
}

const ioHashString& ioParticleEmitter::GetGeneratorType() const
{
	return m_pGenerator->GetType();
}

bool ioParticleEmitter::operator==( const ioParticleEmitter &rhs ) const
{
	if( !m_pGenerator->IsSameGenerator( rhs.m_pGenerator ) )
		return false;

	if( m_fEmitAngle != rhs.m_fEmitAngle )
		return false;

	if( m_fEmissionRate != rhs.m_fEmissionRate )
		return false;

	if( m_fMinStartTexRotation != rhs.m_fMinStartTexRotation )
		return false;

	if( m_fMaxStartTexRotation != rhs.m_fMaxStartTexRotation )
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

	if( m_RGBFactorList.size() != rhs.m_RGBFactorList.size() )
		return false;

	int iFactorCnt = m_RGBFactorList.size();
	for( int i=0 ; i<iFactorCnt ; i++ )
	{
		if( m_RGBFactorList[i] != rhs.m_RGBFactorList[i] )
			return false;
	}

	if( m_AlphaFactor != rhs.m_AlphaFactor )
		return false;

	return true;
}

bool ioParticleEmitter::operator!=( const ioParticleEmitter &rhs ) const
{
	if( *this == rhs )
		return false;

	return true;
}