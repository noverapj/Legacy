

#include "stdafx.h"

#include "ioStringInterface.h"
#include "ioStringConverter.h"

#include "ioModelEmitter.h"
#include "ioModelEmitCommand.h"

namespace ioModelEmitCommand
{
	std::string CmdEmitterPos::Get( const void *pTarget ) const
	{
		const ioModelEmitter *pEmit = static_cast< const ioModelEmitter* >(pTarget);

		return ioStringConverter::toString( pEmit->GetPosition() );
	}
	void CmdEmitterPos::Set( void *pTarget, const std::string &kVal )
	{
		ioModelEmitter *pEmit = static_cast< ioModelEmitter* >(pTarget);

		pEmit->SetPosition( ioStringConverter::ParseVector3( kVal.c_str() ) );
	}

	std::string CmdEmitterDir::Get( const void *pTarget ) const
	{
		const ioModelEmitter *pEmit = static_cast< const ioModelEmitter* >(pTarget);

		return ioStringConverter::toString( pEmit->GetDirection() );
	}
	void CmdEmitterDir::Set( void *pTarget, const std::string &kVal )
	{
		ioModelEmitter *pEmit = static_cast< ioModelEmitter* >(pTarget);

		pEmit->SetDirection( ioStringConverter::ParseVector3( kVal.c_str() ) );
	}

	std::string CmdEmitterAngle::Get( const void *pTarget ) const
	{
		const ioModelEmitter *pEmit = static_cast< const ioModelEmitter* >(pTarget);

		return ioStringConverter::toString( pEmit->GetEmitAngle() );
	}
	void CmdEmitterAngle::Set( void *pTarget, const std::string &kVal )
	{
		ioModelEmitter *pEmit = static_cast< ioModelEmitter* >(pTarget);

		pEmit->SetEmitAngle( ioStringConverter::ParseFloat( kVal.c_str() ) );
	}

	std::string CmdEmitterEmitRate::Get( const void *pTarget ) const
	{
		const ioModelEmitter *pEmit = static_cast< const ioModelEmitter* >(pTarget);

		return ioStringConverter::toString( pEmit->GetEmissionRate() );
	}
	void CmdEmitterEmitRate::Set( void *pTarget, const std::string &kVal )
	{
		ioModelEmitter *pEmit = static_cast< ioModelEmitter* >(pTarget);

		pEmit->SetEmissionRate( ioStringConverter::ParseFloat( kVal.c_str() ) );
	}

	std::string CmdEmitterStartRotAxis::Get( const void *pTarget ) const
	{
		const ioModelEmitter *pEmit = static_cast< const ioModelEmitter* >(pTarget);

		return ioStringConverter::toString( pEmit->GetStartRotAxis() );
	}
	void CmdEmitterStartRotAxis::Set( void *pTarget, const std::string &kVal )
	{
		ioModelEmitter *pEmit = static_cast< ioModelEmitter* >(pTarget);
		pEmit->SetStartRotAxis( ioStringConverter::ParseVector3( kVal.c_str() ) );
	}

	std::string CmdEmiiterStartRotAngle::Get( const void *pTarget ) const
	{
		const ioModelEmitter *pEmit = static_cast< const ioModelEmitter* >(pTarget);

		D3DXVECTOR2 vRotAngle;
		vRotAngle.x = pEmit->GetMinStartRotAngle();
		vRotAngle.y = pEmit->GetMaxStartRotAngle();

		return ioStringConverter::toString( vRotAngle );
	}
	void CmdEmiiterStartRotAngle::Set( void *pTarget, const std::string &kVal )
	{
		ioModelEmitter *pEmit = static_cast< ioModelEmitter* >(pTarget);

		D3DXVECTOR2 vRotAngle = ioStringConverter::ParseVector2( kVal.c_str() );
		pEmit->SetStartRotAngle( vRotAngle.x, vRotAngle.y );
	}

	std::string CmdEmitterSpeed::Get( const void *pTarget ) const
	{
		const ioModelEmitter *pEmit = static_cast< const ioModelEmitter* >(pTarget);

		D3DXVECTOR2 vSpeed;
		vSpeed.x = pEmit->GetMinSpeed();
		vSpeed.y = pEmit->GetMaxSpeed();

		return ioStringConverter::toString( vSpeed );
	}
	void CmdEmitterSpeed::Set( void *pTarget, const std::string &kVal )
	{
		ioModelEmitter *pEmit = static_cast< ioModelEmitter* >(pTarget);

		D3DXVECTOR2 vSpeed;
		vSpeed = ioStringConverter::ParseVector2( kVal.c_str() );

		pEmit->SetModelSpeed( vSpeed.x, vSpeed.y );
	}

	std::string CmdEmitterLiveTime::Get( const void *pTarget ) const
	{
		const ioModelEmitter *pEmit = static_cast< const ioModelEmitter* >(pTarget);
		
		D3DXVECTOR2 vLiveTime;
		vLiveTime.x = pEmit->GetMinLiveTime();
		vLiveTime.y = pEmit->GetMaxLiveTime();

		return ioStringConverter::toString( vLiveTime );
	}
	void CmdEmitterLiveTime::Set( void *pTarget, const std::string &kVal )
	{
		ioModelEmitter *pEmit = static_cast< ioModelEmitter* >(pTarget);

		D3DXVECTOR2 vLiveTime;
		vLiveTime = ioStringConverter::ParseVector2( kVal.c_str() );

		pEmit->SetModelLiveTime( vLiveTime.x, vLiveTime.y );
	}

	std::string CmdEmitterWaitTime::Get( const void *pTarget ) const
	{
		const ioModelEmitter *pEmit = static_cast< const ioModelEmitter* >(pTarget);

		return ioStringConverter::toString( pEmit->GetWaitTime() );
	}
	void CmdEmitterWaitTime::Set( void *pTarget, const std::string &kVal )
	{
		ioModelEmitter *pEmit = static_cast< ioModelEmitter* >(pTarget);

		pEmit->SetWaitTime( ioStringConverter::ParseFloat( kVal.c_str() ) );
	}
	
	std::string CmdEmitterAlphaList::Get( const void *pTarget ) const
	{
		const ioModelEmitter *pEmit = static_cast< const ioModelEmitter* >(pTarget);

		int iAlphaCnt = pEmit->GetAlphaRateCnt();

		D3DXVECTOR2 vAlpha;
		Vector2Vec vAlphaList;
		vAlphaList.reserve( iAlphaCnt );

		for( int i=0 ; i<iAlphaCnt ; i++ )
		{
			vAlpha.x = pEmit->GetAlphaTimeRate( i );
			vAlpha.y = pEmit->GetParticleAlphaRate( i );

			vAlphaList.push_back( vAlpha );
		}

		return ioStringConverter::toString( vAlphaList );
	}
	void CmdEmitterAlphaList::Set( void *pTarget, const std::string &kVal )
	{
		ioModelEmitter *pEmit = static_cast< ioModelEmitter* >(pTarget);
		
		Vector2Vec vAlphaList;
		ioStringConverter::ParseVector2Vec( kVal.c_str(), vAlphaList );
		pEmit->SetAlphaRateList( vAlphaList );
	}
}
