

#include "stdafx.h"

#include "ioStringInterface.h"
#include "ioStringConverter.h"

#include "ioMovableObject.h"

#include "ioModelParticle.h"

#include "ioModelEmitter.h"
#include "ioModelParticleAffector.h"

#include "ioModelEmitAffector.h"

#include "ioModelParticleIterator.h"
#include "ioModelParticleSystem.h"

ioModelEmitAffector::CmdEmitPower ioModelEmitAffector::m_EmitPowerCmd;

ioModelEmitAffector::ioModelEmitAffector()
{
	m_fPower = 0.0f;

	if( CreateParamDictionary("ModelEmitAffector") )
	{
		ioParamDictionary *pDict = GetParamDictionary();

		if( pDict )
		{
			pDict->AddParameter( ioParameterDef( "power", PT_FLOAT),
								 &m_EmitPowerCmd );
		}
	}
}

ioModelEmitAffector::~ioModelEmitAffector()
{
}

void ioModelEmitAffector::AffectModel( ioModelParticleSystem *pSys,
									   float fTimePerSec )
{
	if( !CheckWaitTime( fTimePerSec ) )
		return;

	D3DXVECTOR3 vDir;
	D3DXVECTOR3 vPos = pSys->GetEmitPosition();
	float fPower = m_fPower * fTimePerSec;

	ioModelParticle *pMdl = NULL;
	ioModelParticleIterator iter = pSys->GetIterator();
	while( !iter.end() )
	{
		pMdl = iter.GetNext();

		vDir = pMdl->m_vPosition - vPos;
		D3DXVec3Normalize( &vDir, &vDir );

		pMdl->m_vPosition += vDir * fPower;
	}
}

void ioModelEmitAffector::ParentSystemScaled( float fScaleRate )
{
	m_fPower *= fScaleRate;
}

bool ioModelEmitAffector::IsSameAffector( const ioModelParticleAffector *pRhs ) const
{
	if( !ioModelParticleAffector::IsSameAffector( pRhs ) )
		return false;

	const ioModelEmitAffector *pEmit = dynamic_cast< const ioModelEmitAffector* >( pRhs );
	if( m_fPower != pEmit->m_fPower )
		return false;

	return true;
}

void ioModelEmitAffector::ParametersCopy( const ioModelParticleAffector *pSrc )
{
	ioModelParticleAffector::ParametersCopy( pSrc );

	const ioModelEmitAffector *pEmit = dynamic_cast< const ioModelEmitAffector* >( pSrc );
	m_fPower = pEmit->m_fPower;
}

const ioHashString& ioModelEmitAffector::GetType() const
{
	static ioHashString szType( "Emit" );
	return szType;
}

//-----------------------------------------------------------------------
// Command objects
//-----------------------------------------------------------------------

std::string ioModelEmitAffector::CmdEmitPower::Get( const void *pTarget ) const
{
	const ioModelEmitAffector* pEmit = static_cast<const ioModelEmitAffector*>(pTarget);

	return ioStringConverter::toString( pEmit->GetPower() );
}

void ioModelEmitAffector::CmdEmitPower::Set( void *pTarget, const std::string &kVal )
{
	ioModelEmitAffector* pEmit = static_cast<ioModelEmitAffector*>(pTarget);

	pEmit->SetPower( ioStringConverter::ParseFloat( kVal.c_str() ) );
}

