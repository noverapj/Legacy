

#include "stdafx.h"

#include "ioMath.h"
#include "ioStringConverter.h"
#include "ioStringInterface.h"

#include "ioParticleEmitter.h"
#include "ioEmitterCommands.h"

namespace ioEmitterCommands
{
	std::string CmdEmitterPos::Get( const void *pTarget ) const
	{
		const ioParticleEmitter *pEmit = static_cast< const ioParticleEmitter* >(pTarget);

		return ioStringConverter::toString( pEmit->GetPosition() );
	}
	void CmdEmitterPos::Set( void *pTarget, const std::string &kVal )
	{
		ioParticleEmitter *pEmit = static_cast< ioParticleEmitter* >(pTarget);

		pEmit->SetPosition( ioStringConverter::ParseVector3( kVal.c_str() ) );
	}

	std::string CmdEmitterDir::Get( const void *pTarget ) const
	{
		const ioParticleEmitter *pEmit = static_cast< const ioParticleEmitter* >(pTarget);

		return ioStringConverter::toString( pEmit->GetDirection() );
	}
	void CmdEmitterDir::Set( void *pTarget, const std::string &kVal )
	{
		ioParticleEmitter *pEmit = static_cast< ioParticleEmitter* >(pTarget);

		pEmit->SetDirection( ioStringConverter::ParseVector3( kVal.c_str() ) );
	}

	std::string CmdEmitterAngle::Get( const void *pTarget ) const
	{
		const ioParticleEmitter *pEmit = static_cast< const ioParticleEmitter* >(pTarget);

		return ioStringConverter::toString( pEmit->GetEmitAngle() );
	}
	void CmdEmitterAngle::Set( void *pTarget, const std::string &kVal )
	{
		ioParticleEmitter *pEmit = static_cast< ioParticleEmitter* >(pTarget);

		pEmit->SetEmitAngle( ioStringConverter::ParseFloat( kVal.c_str() ) );
	}

	std::string CmdEmitterEmitRate::Get( const void *pTarget ) const
	{
		const ioParticleEmitter *pEmit = static_cast< const ioParticleEmitter* >(pTarget);

		return ioStringConverter::toString( pEmit->GetEmissionRate() );
	}
	void CmdEmitterEmitRate::Set( void *pTarget, const std::string &kVal )
	{
		ioParticleEmitter *pEmit = static_cast< ioParticleEmitter* >(pTarget);

		pEmit->SetEmissionRate( ioStringConverter::ParseFloat( kVal.c_str() ) );
	}

	std::string CmdEmitterStartTexRotate::Get( const void *pTarget ) const
	{
		const ioParticleEmitter *pEmit = static_cast< const ioParticleEmitter* >(pTarget);

		D3DXVECTOR2 vTexRotate;
		vTexRotate.x = pEmit->GetMinStartTexRotation();
		vTexRotate.y = pEmit->GetMaxStartTexRotation();

		return ioStringConverter::toString( vTexRotate );
	}
	void CmdEmitterStartTexRotate::Set( void *pTarget, const std::string &kVal )
	{
		ioParticleEmitter *pEmit = static_cast< ioParticleEmitter* >(pTarget);

		D3DXVECTOR2 vTexRotate = ioStringConverter::ParseVector2( kVal.c_str() );
		pEmit->SetStartTexRotation( vTexRotate.x, vTexRotate.y );
	}
	
	std::string CmdEmitterSpeed::Get( const void *pTarget ) const
	{
		const ioParticleEmitter *pEmit = static_cast< const ioParticleEmitter* >(pTarget);

		D3DXVECTOR2 vSpeed;
		vSpeed.x = pEmit->GetMinSpeed();
		vSpeed.y = pEmit->GetMaxSpeed();

		return ioStringConverter::toString( vSpeed );
	}
	void CmdEmitterSpeed::Set( void *pTarget, const std::string &kVal )
	{
		ioParticleEmitter *pEmit = static_cast< ioParticleEmitter* >(pTarget);

		D3DXVECTOR2 vSpeed = ioStringConverter::ParseVector2( kVal.c_str() );
		pEmit->SetParticleSpeed( vSpeed.x, vSpeed.y );
	}

	std::string CmdEmitterLiveTime::Get( const void *pTarget ) const
	{
		const ioParticleEmitter *pEmit = static_cast< const ioParticleEmitter* >(pTarget);
		
		D3DXVECTOR2 vLiveTime;
		vLiveTime.x = pEmit->GetMinLiveTime();
		vLiveTime.y = pEmit->GetMaxLiveTime();

		return ioStringConverter::toString( vLiveTime );
	}
	void CmdEmitterLiveTime::Set( void *pTarget, const std::string &kVal )
	{
		ioParticleEmitter *pEmit = static_cast< ioParticleEmitter* >(pTarget);

		D3DXVECTOR2 vLiveTime = ioStringConverter::ParseVector2( kVal.c_str() );
		pEmit->SetParticleLiveTime( vLiveTime.x, vLiveTime.y );
	}

	std::string CmdEmitterWaitTime::Get( const void *pTarget ) const
	{
		const ioParticleEmitter *pEmit = static_cast< const ioParticleEmitter* >(pTarget);

		return ioStringConverter::toString( pEmit->GetWaitTime() );
	}
	void CmdEmitterWaitTime::Set( void *pTarget, const std::string &kVal )
	{
		ioParticleEmitter *pEmit = static_cast< ioParticleEmitter* >(pTarget);

		pEmit->SetWaitTime( ioStringConverter::ParseFloat( kVal.c_str() ) );
	}

	std::string CmdEmitterColorList::Get( const void *pTarget ) const
	{
		const ioParticleEmitter *pEmit = static_cast< const ioParticleEmitter* >(pTarget);

		int iColorCnt = pEmit->GetColorRateCnt();

		D3DXVECTOR4 vColor;
		std::vector< D3DXVECTOR4 > vColorList;
		vColorList.reserve( iColorCnt );
		ParticleRGB kRGB;

		for( int i=0 ; i<iColorCnt ; i++ )
		{
			kRGB = pEmit->GetParticleColor( i );

			vColor.x = pEmit->GetColorTimeRate( i );
			vColor.y = (float)kRGB.bRed;
			vColor.z = (float)kRGB.bGreen;
			vColor.w = (float)kRGB.bBlue;

			vColorList.push_back( vColor );
		}

		return ioStringConverter::toString( vColorList );
	}
	void CmdEmitterColorList::Set( void *pTarget, const std::string &kVal )
	{
		ioParticleEmitter *pEmit = static_cast< ioParticleEmitter* >(pTarget);

		std::vector< D3DXVECTOR4 > vColorList;
		ioStringConverter::ParseVector4Vec( kVal.c_str(), vColorList );
		pEmit->SetColorList( vColorList );
	}

	std::string CmdEmitterAlphaList::Get( const void *pTarget ) const
	{
		const ioParticleEmitter *pEmit = static_cast< const ioParticleEmitter* >(pTarget);

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
		ioParticleEmitter *pEmit = static_cast< ioParticleEmitter* >(pTarget);
		
		Vector2Vec vAlphaList;
		ioStringConverter::ParseVector2Vec( kVal.c_str(), vAlphaList );
		pEmit->SetAlphaList( vAlphaList );
	}
}
