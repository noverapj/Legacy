

#include "stdafx.h"

#include "ioEmitPointGenerator.h"
#include "ioStringConverter.h"

#include "ioMath.h"

ioEmitPointGenerator::ioEmitPointGenerator() 
: m_vDir( 0.0f, 1.0f, 0.0f )
{
}

ioEmitPointGenerator::~ioEmitPointGenerator()
{
}

void ioEmitPointGenerator::ParametersCopy( const ioEmitPointGenerator *pSrc )
{
	SetDefaultPosition( pSrc->GetDefaultPosition() );
	SetDefaultDirection( pSrc->GetDefaultDirection() );
}

D3DXVECTOR3 ioEmitPointGenerator::GetStartPosition() const
{
	return m_vPos;
}

void ioEmitPointGenerator::SetDefaultPosition( const D3DXVECTOR3 &vPos )
{
	m_vPos = vPos;
}

void ioEmitPointGenerator::SetDefaultDirection( const D3DXVECTOR3 &vDir )
{
	m_vDir = vDir;
}

void ioEmitPointGenerator::ScaleGenerateArea( float fScale )
{
}

void ioEmitPointGenerator::SetRandomSeed( unsigned int uSeed )
{
	m_RandomCreator.SetRandomSeed( uSeed );
}

unsigned int ioEmitPointGenerator::GetRandomSeed() const
{
	return m_RandomCreator.GetRandomSeed();
}

float ioEmitPointGenerator::UnitRandom()
{
	return m_RandomCreator.UnitRandom();
}

float ioEmitPointGenerator::RangeRandom( float fMin, float fMax )
{
	return m_RandomCreator.RangeRandom( fMin, fMin );
}

float ioEmitPointGenerator::SymmetricRandom()
{
	return m_RandomCreator.SymmetricRandom();
}

bool ioEmitPointGenerator::IsSameGenerator( const ioEmitPointGenerator *pRhs ) const
{
	if( GetType() != pRhs->GetType() )
		return false;

	if( !ioMath::IsEqual( m_vPos, pRhs->m_vPos ) )
		return false;

	if( !ioMath::IsEqual( m_vDir, pRhs->m_vDir ) )
		return false;

	return true;
}

//---------------------------------------------------------------------------------

ioPointEmitGenerator::ioPointEmitGenerator()
{
	CreateParamDictionary( "PointGenerator" );
}

ioPointEmitGenerator::~ioPointEmitGenerator()
{
}

const ioHashString& ioPointEmitGenerator::GetType() const
{
	static ioHashString szType( "Point" );
	return szType;
}

//----------------------------------------------------------------------------------

ioAreaEmitGenerator::CmdAreaSize ioAreaEmitGenerator::m_AreaSizeCmd;

ioAreaEmitGenerator::ioAreaEmitGenerator()
{
	SetSize( 100.0f, 100.0f, 100.0f );
}

ioAreaEmitGenerator::~ioAreaEmitGenerator()
{
}

bool ioAreaEmitGenerator::InitDefaultParameters( const char *szType )
{
	char szTemp[MAX_PATH];
	wsprintf( szTemp, "%sGenerator", szType );

	if( CreateParamDictionary( szTemp ) )
	{
		ioParamDictionary *pDict = GetParamDictionary();

		pDict->AddParameter( ioParameterDef( "area_size", PT_VECTOR3 ),
							 &m_AreaSizeCmd );

		return true;
	}

	return false;
}

void ioAreaEmitGenerator::ParametersCopy( const ioEmitPointGenerator *pSrc )
{
	ioEmitPointGenerator::ParametersCopy( pSrc );

	const ioAreaEmitGenerator *pArea = dynamic_cast< const ioAreaEmitGenerator* >( pSrc );
	SetSize( pArea->GetSize() );
}

void ioAreaEmitGenerator::SetDefaultDirection( const D3DXVECTOR3 &vDir )
{
	ioEmitPointGenerator::SetDefaultDirection( vDir );

	GenAreaAxis();
}

void ioAreaEmitGenerator::ScaleGenerateArea( float fScale )
{
	SetSize( m_vSize * fScale );
}

bool ioAreaEmitGenerator::IsSameGenerator( const ioEmitPointGenerator *pRhs ) const
{
	if( !ioEmitPointGenerator::IsSameGenerator( pRhs ) )
		return false;

	const ioAreaEmitGenerator *pArea = dynamic_cast< const ioAreaEmitGenerator* >( pRhs );

	return ioMath::IsEqual( m_vSize, pArea->m_vSize );
}

void ioAreaEmitGenerator::SetSize( const D3DXVECTOR3 &vSize )
{
	m_vSize = vSize;

	GenAreaAxis();
}

void ioAreaEmitGenerator::SetSize( float fSizeX, float fSizeY, float fSizeZ )
{
	m_vSize.x = fSizeX;
	m_vSize.y = fSizeY;
	m_vSize.z = fSizeZ;

	GenAreaAxis();
}

void ioAreaEmitGenerator::GenAreaAxis()
{
	D3DXVECTOR3 vDir;
	D3DXVec3Normalize( &vDir, &m_vDir );

	D3DXVECTOR3 vUp = ioMath::UNIT_X;
	if( fabsf( D3DXVec3Dot( &vDir, &vUp ) ) > 0.99f )
		vUp = -ioMath::UNIT_Y;

	D3DXVECTOR3 vSide;
	D3DXVec3Cross( &vSide, &vUp, &vDir );
	D3DXVec3Cross( &vUp, &vDir, &vSide );

	m_vRangeX = vUp * ( m_vSize.x * FLOAT05 );
	m_vRangeY = vDir * ( m_vSize.y * FLOAT05 );
	m_vRangeZ = vSide  * ( m_vSize.z * FLOAT05 );
}

std::string ioAreaEmitGenerator::CmdAreaSize::Get( const void *pTarget ) const
{
	const ioAreaEmitGenerator *pArea = static_cast< const ioAreaEmitGenerator* >(pTarget);

	return ioStringConverter::toString( pArea->GetSize() );	
}

void ioAreaEmitGenerator::CmdAreaSize::Set( void *pTarget, const std::string &kVal )
{
	ioAreaEmitGenerator *pArea = static_cast< ioAreaEmitGenerator* >(pTarget);

	pArea->SetSize( ioStringConverter::ParseVector3( kVal.c_str() ) );
}

//--------------------------------------------------------------------------------------

ioBoxEmitGenerator::ioBoxEmitGenerator()
{
	InitDefaultParameters( "Box" );
}

ioBoxEmitGenerator::~ioBoxEmitGenerator()
{
}

D3DXVECTOR3 ioBoxEmitGenerator::GetStartPosition() const
{
	D3DXVECTOR3 vXOff, vYOff, vZOff;
	vXOff = m_RandomCreator.SymmetricRandom() * m_vRangeX;
	vYOff = m_RandomCreator.SymmetricRandom() * m_vRangeY;
	vZOff = m_RandomCreator.SymmetricRandom() * m_vRangeZ;

	return m_vPos + vXOff + vYOff + vZOff;
}

const ioHashString& ioBoxEmitGenerator::GetType() const
{
	static ioHashString szType( "Box" );
	return szType;
}

//---------------------------------------------------------------------------------------

ioCylinderEmitGenerator::ioCylinderEmitGenerator()
{
	InitDefaultParameters( "Cylinder" );
}

ioCylinderEmitGenerator::~ioCylinderEmitGenerator()
{
}

D3DXVECTOR3 ioCylinderEmitGenerator::GetStartPosition() const
{
	float fX, fY, fZ;
	fX = m_RandomCreator.SymmetricRandom();
	fY = m_RandomCreator.SymmetricRandom();
	fZ = m_RandomCreator.SymmetricRandom();

	float fXSq = fX * fX;
	float fZSq = fZ * fZ;
	if( fXSq + fZSq > 1.0f )
	{
		float fDet = sqrt( fXSq + fZSq );
		fX /= fDet;
		fZ /= fDet;
	}

	return m_vPos +  m_vRangeX * fX + m_vRangeY * fY + m_vRangeZ * fZ;
}

const ioHashString& ioCylinderEmitGenerator::GetType() const
{
	static ioHashString szType( "Cylinder" );
	return szType;
}

//--------------------------------------------------------------------------

ioEllipseEmitGenerator::CmdInnerSize ioEllipseEmitGenerator::m_InnerSizeCmd;

ioEllipseEmitGenerator::ioEllipseEmitGenerator()
{
	if( InitDefaultParameters( "Ellipse" ) )
	{
		ioParamDictionary *pDict = GetParamDictionary();

		pDict->AddParameter( ioParameterDef( "inner_size", PT_VECTOR2 ),
							 &m_InnerSizeCmd );
	}

	SetInnerSize( FLOAT05 );		
}

ioEllipseEmitGenerator::~ioEllipseEmitGenerator()
{
}

void ioEllipseEmitGenerator::ParametersCopy( const ioEmitPointGenerator *pSrc )
{
	ioAreaEmitGenerator::ParametersCopy( pSrc );

	const ioEllipseEmitGenerator *pEllipse = dynamic_cast< const ioEllipseEmitGenerator* >( pSrc );
	SetInnerSize( pEllipse->GetInnerSizeX(), pEllipse->GetInnerSizeZ() );
}

const ioHashString& ioEllipseEmitGenerator::GetType() const
{
	static ioHashString szType( "Ellipse" );
	return szType;
}

void ioEllipseEmitGenerator::SetInnerSize( float fSize )
{
	m_fInnerSizeX = m_fInnerSizeZ = fSize;
}

void ioEllipseEmitGenerator::SetInnerSize( float fSizeX, float fSizeZ )
{
	m_fInnerSizeX = fSizeX;
	m_fInnerSizeZ = fSizeZ;
}

D3DXVECTOR3 ioEllipseEmitGenerator::GetStartPosition() const
{
	float fAngle = m_RandomCreator.RangeRandom( 0.0f, D3DX_PI * 2.0f );

	float fXOff, fZOff;
	fXOff = m_RandomCreator.RangeRandom( m_fInnerSizeX, 1.0f );
	fZOff = m_RandomCreator.RangeRandom( m_fInnerSizeZ, 1.0f );

	D3DXVECTOR3 vOffset;
	vOffset.x = m_fInnerSizeX * sinf( fAngle );
	vOffset.y = m_RandomCreator.UnitRandom();
	vOffset.z = m_fInnerSizeZ * cosf( fAngle );

	return m_vPos +  m_vRangeX * vOffset.x + m_vRangeY * vOffset.y + m_vRangeZ * vOffset.z;
}

bool ioEllipseEmitGenerator::IsSameGenerator( const ioEmitPointGenerator *pRhs ) const
{
	if( !ioAreaEmitGenerator::IsSameGenerator( pRhs ) )
		return false;

	const ioEllipseEmitGenerator *pEllipse = dynamic_cast< const ioEllipseEmitGenerator* >( pRhs );

	if( m_fInnerSizeX != pEllipse->m_fInnerSizeX )
		return false;

	if( m_fInnerSizeZ != pEllipse->m_fInnerSizeZ )
		return false;

	return true;
}

std::string ioEllipseEmitGenerator::CmdInnerSize::Get( const void *pTarget ) const
{
	const ioEllipseEmitGenerator *pEllipse = static_cast< const ioEllipseEmitGenerator* >(pTarget);

	D3DXVECTOR2 vInnerSize;
	vInnerSize.x = pEllipse->GetInnerSizeX();
	vInnerSize.y = pEllipse->GetInnerSizeZ();

	return ioStringConverter::toString( vInnerSize );
}

void ioEllipseEmitGenerator::CmdInnerSize::Set( void *pTarget, const std::string &kVal )
{
	ioEllipseEmitGenerator *pEllipse = static_cast< ioEllipseEmitGenerator* >(pTarget);

	D3DXVECTOR2 vInnerSize;
	vInnerSize = ioStringConverter::ParseVector2( kVal.c_str() );

	pEllipse->SetInnerSize( vInnerSize.x, vInnerSize.y );
}

//-------------------------------------------------------------------------------

ioEllipsoidEmitGenerator::ioEllipsoidEmitGenerator()
{
	InitDefaultParameters( "Ellipsoid" );
}

ioEllipsoidEmitGenerator::~ioEllipsoidEmitGenerator()
{
}

D3DXVECTOR3 ioEllipsoidEmitGenerator::GetStartPosition() const
{
	D3DXVECTOR3 vOffset;
	vOffset.x = m_RandomCreator.SymmetricRandom();
	vOffset.y = m_RandomCreator.SymmetricRandom();
	vOffset.z = m_RandomCreator.SymmetricRandom();
	
	if( D3DXVec3LengthSq( &vOffset ) > 1.0f )
	{
		D3DXVec3Normalize( &vOffset, &vOffset );
	}

	return m_vPos + m_vRangeX * vOffset.x + m_vRangeY * vOffset.y + m_vRangeZ * vOffset.z;
}

const ioHashString& ioEllipsoidEmitGenerator::GetType() const
{
	static ioHashString szType( "Ellipsoid" );
	return szType;
}

//-------------------------------------------------------------------------------------

ioHollowEllipsoidEmitGenerator::CmdInnerSizeXYZ ioHollowEllipsoidEmitGenerator::m_InnerSizeXYZCmd;

ioHollowEllipsoidEmitGenerator::ioHollowEllipsoidEmitGenerator()
{
	if( InitDefaultParameters( "HollowEllipsoid" ) )
	{
		ioParamDictionary *pDict = GetParamDictionary();

		pDict->AddParameter( ioParameterDef("inner_size", PT_VECTOR3),
							 &m_InnerSizeXYZCmd );
	}

	SetInnerSize( FLOAT05, FLOAT05, FLOAT05 );
}

ioHollowEllipsoidEmitGenerator::~ioHollowEllipsoidEmitGenerator()
{
}

void ioHollowEllipsoidEmitGenerator::ParametersCopy( const ioEmitPointGenerator *pSrc )
{
	ioAreaEmitGenerator::ParametersCopy( pSrc );

	const ioHollowEllipsoidEmitGenerator *pHollow = dynamic_cast< const ioHollowEllipsoidEmitGenerator* >( pSrc );
	SetInnerSize( pHollow->GetInnerSize() );
}

D3DXVECTOR3 ioHollowEllipsoidEmitGenerator::GetStartPosition() const
{
	float fAlpha, fBeta;
	fAlpha = m_RandomCreator.RangeRandom( 0.0f, D3DX_PI * 2.0f );
	fBeta  = m_RandomCreator.RangeRandom( 0.0f, D3DX_PI );

	float fXRadius, fYRadius, fZRadius;
	fXRadius = m_RandomCreator.RangeRandom( m_vInnerSize.x, 1.0f );
	fYRadius = m_RandomCreator.RangeRandom( m_vInnerSize.y, 1.0f );
	fZRadius = m_RandomCreator.RangeRandom( m_vInnerSize.z, 1.0f );

	D3DXVECTOR3 vOffset;
	vOffset.x = fXRadius * cosf( fAlpha ) * sinf( fBeta );
	vOffset.y = fYRadius * cosf( fBeta );
	vOffset.z = fZRadius * sinf( fAlpha ) * sinf( fBeta );

	return m_vPos + m_vRangeX * vOffset.x + m_vRangeY * vOffset.y + m_vRangeZ * vOffset.z;
}

const ioHashString& ioHollowEllipsoidEmitGenerator::GetType() const
{
	static ioHashString szType( "HollowEllipsoid" );
	return szType;
}

bool ioHollowEllipsoidEmitGenerator::IsSameGenerator( const ioEmitPointGenerator *pRhs ) const
{
	if( !ioAreaEmitGenerator::IsSameGenerator( pRhs ) )
		return false;

	const ioHollowEllipsoidEmitGenerator *pHollow = dynamic_cast< const ioHollowEllipsoidEmitGenerator* >( pRhs );

	return ioMath::IsEqual( m_vInnerSize, pHollow->m_vInnerSize );
}

void ioHollowEllipsoidEmitGenerator::SetInnerSize( const D3DXVECTOR3 &vInnerSize )
{
	m_vInnerSize.x = max( 0.0f, min( vInnerSize.x, 1.0f ) );
	m_vInnerSize.y = max( 0.0f, min( vInnerSize.y, 1.0f ) );
	m_vInnerSize.z = max( 0.0f, min( vInnerSize.z, 1.0f ) );
}

void ioHollowEllipsoidEmitGenerator::SetInnerSize( float fInnerX, float fInnerY, float fInnerZ )
{
	m_vInnerSize.x = max( 0.0f, min( fInnerX, 1.0f ) );
	m_vInnerSize.y = max( 0.0f, min( fInnerY, 1.0f ) );
	m_vInnerSize.z = max( 0.0f, min( fInnerZ, 1.0f ) );
}

std::string ioHollowEllipsoidEmitGenerator::CmdInnerSizeXYZ::Get( const void *pTarget ) const
{
	const ioHollowEllipsoidEmitGenerator *pHollow = static_cast<const ioHollowEllipsoidEmitGenerator*>(pTarget);

	return ioStringConverter::toString( pHollow->GetInnerSize() );
}

void ioHollowEllipsoidEmitGenerator::CmdInnerSizeXYZ::Set( void *pTarget, const std::string &kVal )
{
	ioHollowEllipsoidEmitGenerator *pHollow = static_cast<ioHollowEllipsoidEmitGenerator*>(pTarget);

	pHollow->SetInnerSize( ioStringConverter::ParseVector3( kVal.c_str() ) );

}
