

#include "stdafx.h"

#include "ioStringInterface.h"
#include "ioStringConverter.h"

#include "ioModelParticleAffector.h"

ioModelParticleAffector::CmdWaitTime ioModelParticleAffector::m_WaitTimeCmd;

ioModelParticleAffector::ioModelParticleAffector()
{
	m_fWaitTime = 0.0f;
	m_fCurTime  = 0.0f;
	m_bEnable = true;
}

ioModelParticleAffector::~ioModelParticleAffector()
{
}

void ioModelParticleAffector::AddBaseParameters()
{
	ioParamDictionary *pDict = GetParamDictionary();

	pDict->AddParameter( ioParameterDef("wait_time", PT_FLOAT),
						 &m_WaitTimeCmd );
}

void ioModelParticleAffector::ParametersCopy( const ioModelParticleAffector *pSrc )
{
	SetWaitTime( pSrc->GetWaitTime() );
}

bool ioModelParticleAffector::CheckWaitTime( float fTimePerSec )
{
	m_fCurTime += fTimePerSec *FLOAT1000;

	if( m_fCurTime > m_fWaitTime )
		return true;

	return false;
}

void ioModelParticleAffector::InitModel( ioModelParticle *pMdl )
{
}

void ioModelParticleAffector::ParentSystemScaled( float fScaleRate )
{
}

void ioModelParticleAffector::ParentSystemDirectionChanged( const D3DXQUATERNION &qtRotate )
{
}

void ioModelParticleAffector::Restart()
{
	m_fCurTime = 0.0f;
}

bool ioModelParticleAffector::IsSameAffector( const ioModelParticleAffector *pRhs ) const
{
	if( GetType() != pRhs->GetType() )
		return false;

	if( m_fWaitTime != pRhs->m_fWaitTime )
		return false;

	return true;
}

//-----------------------------------------------------------------------
// Command objects
//-----------------------------------------------------------------------

std::string ioModelParticleAffector::CmdWaitTime::Get( const void *pTarget ) const
{
	const ioModelParticleAffector *pAff = static_cast< const ioModelParticleAffector* >(pTarget);

	return ioStringConverter::toString( pAff->GetWaitTime() );
}

void ioModelParticleAffector::CmdWaitTime::Set( void *pTarget, const std::string &kVal )
{
	ioModelParticleAffector *pAff = static_cast< ioModelParticleAffector* >(pTarget);

	pAff->SetWaitTime( ioStringConverter::ParseFloat( kVal.c_str() ) );
}

