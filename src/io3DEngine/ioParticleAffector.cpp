

#include "stdafx.h"
#include "ioStringInterface.h"
#include "ioStringConverter.h"

#include "ioParticleAffector.h"

ioParticleAffector::CmdWaitTime ioParticleAffector::m_WaitTimeCmd;

ioParticleAffector::ioParticleAffector()
{
	m_fWaitTime = 0.0f;
	m_fCurTime  = 0.0f;

	m_bEnable = true;
}

ioParticleAffector::~ioParticleAffector()
{
}

void ioParticleAffector::AddBaseParameters()
{
	ioParamDictionary *pDict = GetParamDictionary();
	if( pDict )
	{
		pDict->AddParameter( ioParameterDef("wait_time", PT_FLOAT),
							 &m_WaitTimeCmd );
	}	
}

bool ioParticleAffector::CheckWaitTime( float fTimePerSec )
{
	m_fCurTime += fTimePerSec *FLOAT1000;

	if( m_fCurTime > m_fWaitTime )
		return true;

	return false;
}

void ioParticleAffector::InitParticle( ioParticleSystem *pSys, ioParticle *pParticle )
{
}

void ioParticleAffector::ParentSystemScaled( float fScaleRate )
{
}

void ioParticleAffector::ParentSystemDirectionChanged( const D3DXQUATERNION &qtRotate )
{
}

void ioParticleAffector::Restart()
{
	m_fCurTime = 0.0f;
}

bool ioParticleAffector::IsSameAffector( const ioParticleAffector *pRhs ) const
{
	if( GetType() != pRhs->GetType() )
		return false;

	if( m_fWaitTime != pRhs->m_fWaitTime )
		return false;

	return true;
}

void ioParticleAffector::ParametersCopy( const ioParticleAffector *pSrc )
{
	m_fWaitTime = pSrc->m_fWaitTime;
}

//-----------------------------------------------------------------------
// Command objects
//-----------------------------------------------------------------------

std::string ioParticleAffector::CmdWaitTime::Get( const void *pTarget ) const
{
	const ioParticleAffector *pAff = static_cast< const ioParticleAffector* >(pTarget);

	return ioStringConverter::toString( pAff->GetWaitTime() );
}

void ioParticleAffector::CmdWaitTime::Set( void *pTarget, const std::string &kVal )
{
	ioParticleAffector *pAff = static_cast< ioParticleAffector* >(pTarget);

	pAff->SetWaitTime( ioStringConverter::ParseFloat( kVal.c_str() ) );
}

