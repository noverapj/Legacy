

#include "stdafx.h"

#include "ioShader.h"
#include "ioShaderParameter.h"

static ioHashString AutoParamTable[ ACT_MAX_CONSTANT_TYPE ] =
{
	"matWorld",
	"matView",
	"matProj",
	"BipedList",
	"WeightedBipedCnt",
	"matViewProj",
	"matWorldView",
	"matWorldViewProj",
	"matInvWorld",
	"matInvView",
	"matInvWorldView",
	"matInvTransposeWorld",
	"matInvTransposeWorldView",
	"matTex",
	"matLightWorldViewProj",
	"matShadowMap",

	"ShadowColor",
	"MaterialDiffuse",
	
	"DirLightDir",
	"DirDiffuseColor",
	"DirSpecularColor",
	"SpecularParam",
	
	"PointLightPos",
	"PointDiffuseColor",
	"PointAttenuation",

	"SpotDiffuseColor",
	"SpotSpecularColor",
	"SpotAttenuation",
	"SpotLightDir",
	"SpotLightPos",
	"SpotFactor",

	"AmbientEmmColor",
	"CameraObjectPos",
	"CameraObjectDir",
	"ViewPort",
	"CameraFovNearFar",
	"FogFactor",
	"AlphaFactor",
	"OutLineConstant",
};

AutoConstantEntry ParsingConstantEntry( const D3DXPARAMETER_DESC &rkDesc )
{
	ioHashString kConstantName;
	AutoConstantEntry kAutoEntry;
	
	if( rkDesc.Name[0] != '_' )
	{
		kConstantName = rkDesc.Name;
	}
	else
	{
		std::string szParsingName = rkDesc.Name;
		size_t nEndPos = szParsingName.find( '_', 1 );
		kConstantName = szParsingName.substr( 1, nEndPos-1 ).c_str();

		szParsingName.erase( 0, nEndPos+1 );
		kAutoEntry.m_iUnitIndex = atoi( szParsingName.c_str() );
	}

	for( int i=0 ; i<ACT_MAX_CONSTANT_TYPE ; i++ )
	{
		if( AutoParamTable[i] == kConstantName )
		{
			kAutoEntry.m_Type = (AutoConstantType)i;
			return kAutoEntry;
		}
	}

	return kAutoEntry;
}

__EX bool IsAutoConstant( const ioHashString &szName )
{
	if( szName.IsEmpty() )
		return false;

	ioHashString szConstant;
	if( szName.At(0) != '_' )
	{
		szConstant = szName;
	}
	else
	{
		std::string szParsingName = szName.c_str();
		size_t nEndPos = szParsingName.find( '_', 1 );
		szConstant = szParsingName.substr( 1, nEndPos-1 ).c_str();
	}

	for( int i=0 ; i<ACT_MAX_CONSTANT_TYPE ; i++ )
	{
		if( AutoParamTable[i] == szConstant )
			return true;
	}

	return false;
}

ioShaderCustomArg::ioShaderCustomArg( const char *szName )
: m_Name( szName ), m_vData( 0.0f, 0.0f, 0.0f, 0.0f )
{
}

ioShaderCustomArg::ioShaderCustomArg( const ioShaderCustomArg &rhs )
{
	*this = rhs;
}

ioShaderCustomArg::~ioShaderCustomArg()
{
}

void ioShaderCustomArg::SetName( const char *szName )
{
	m_Name = szName;
}

void ioShaderCustomArg::SetValue( float fVal )
{
	m_ValueType = CPT_FLOAT;
	m_vData.x = fVal;
}

void ioShaderCustomArg::SetValue( const D3DXVECTOR3 &kVal )
{
	m_ValueType = CPT_VECTOR3;
	m_vData.x = kVal.x;
	m_vData.y = kVal.y;
	m_vData.z = kVal.z;
}

void ioShaderCustomArg::SetValue( const D3DXVECTOR4 &kVal )
{
	m_ValueType = CPT_VECTOR4;
	m_vData = kVal;
}

void ioShaderCustomArg::SetValue( const D3DCOLORVALUE &kVal )
{
	m_ValueType = CPT_COLORVALUE;
	m_vData = (const D3DXVECTOR4&)kVal;
}

void ioShaderCustomArg::SetValue( const ioShaderCustomArg &kVal )
{
	m_ValueType = kVal.m_ValueType;
	m_vData = kVal.m_vData;
}

bool ioShaderCustomArg::operator==( const ioShaderCustomArg &rhs ) const
{
	if( m_Name != rhs.m_Name )
		return false;

	if( m_ValueType != rhs.m_ValueType )
		return false;

	if( m_vData != rhs.m_vData )
		return false;

	return true;
}

bool ioShaderCustomArg::operator!=( const ioShaderCustomArg &rhs ) const
{
	if( *this == rhs )
		return false;

	return true;
}

//-------------------------------------------------------------------------

ioShaderCustomParameters::ioShaderCustomParameters()
{
}

ioShaderCustomParameters::~ioShaderCustomParameters()
{
	m_CustomArgList.clear();
}

void ioShaderCustomParameters::SetCustomArg( const ioShaderCustomArg &kVal )
{
	CustomArgList::iterator iter;
	for( iter=m_CustomArgList.begin() ; iter!=m_CustomArgList.end() ; ++iter )
	{
		if( iter->GetName() == kVal.GetName() )
		{
			iter->SetValue( kVal );
			return;
		}
	}

	m_CustomArgList.push_back( kVal );
}

void ioShaderCustomParameters::SetCustomArg( const char *szName, float fVal )
{
	ioShaderCustomArg kArg( szName );
	kArg.SetValue( fVal );

	SetCustomArg( kArg );
}

void ioShaderCustomParameters::SetCustomArg( const char *szName, const D3DXVECTOR3 &kVal )
{
	ioShaderCustomArg kArg( szName );
	kArg.SetValue( kVal );

	SetCustomArg( kArg );
}

void ioShaderCustomParameters::SetCustomArg( const char *szName, const D3DXVECTOR4 &kVal )
{
	ioShaderCustomArg kArg( szName );
	kArg.SetValue( kVal );

	SetCustomArg( kArg );
}

void ioShaderCustomParameters::SetCustomArg( const char *szName, const D3DCOLORVALUE &kVal )
{
	ioShaderCustomArg kArg( szName );
	kArg.SetValue( kVal );

	SetCustomArg( kArg );
}

const ioShaderCustomArg* ioShaderCustomParameters::GetValue( const ioHashString &name ) const
{
	CustomArgList::const_iterator iter;
	for( iter=m_CustomArgList.begin() ; iter!=m_CustomArgList.end() ; ++iter )
	{
		if( iter->GetName() == name )
			return &(*iter);
	}

	return NULL;
}

const ioShaderCustomArg* ioShaderCustomParameters::GetValue( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetCustomArgCount() ) )
		return &m_CustomArgList[iIndex];

	return NULL;
}

ioShaderCustomArg* ioShaderCustomParameters::GetArg( const ioHashString &name )
{
	CustomArgList::iterator iter;
	for( iter=m_CustomArgList.begin() ; iter!=m_CustomArgList.end() ; ++iter )
	{
		if( iter->GetName() == name )
			return &(*iter);
	}

	return NULL;
}

ioShaderCustomArg* ioShaderCustomParameters::GetArg( int iIndex )
{
	if( COMPARE( iIndex, 0, GetCustomArgCount() ) )
		return &m_CustomArgList[iIndex];

	return NULL;
}

const ioShaderCustomArg* ioShaderCustomParameters::GetArg( const ioHashString &name ) const
{
	CustomArgList::const_iterator iter;
	for( iter=m_CustomArgList.begin() ; iter!=m_CustomArgList.end() ; ++iter )
	{
		if( iter->GetName() == name )
			return &(*iter);
	}

	return NULL;
}

const ioShaderCustomArg* ioShaderCustomParameters::GetArg( int iIndex ) const
{
	if( COMPARE( iIndex, 0, GetCustomArgCount() ) )
		return &m_CustomArgList[iIndex];

	return NULL;
}

ioShaderCustomParameters& ioShaderCustomParameters::operator=( const ioShaderCustomParameters &rhs )
{
	m_CustomArgList.erase( m_CustomArgList.begin(), m_CustomArgList.end() );

	int iCustomParams = rhs.GetCustomArgCount();
	for( int i=0 ; i<iCustomParams ; i++ )
	{
		m_CustomArgList.push_back( *rhs.GetValue(i) );
	}

	return *this;
}

bool ioShaderCustomParameters::operator==( const ioShaderCustomParameters &rhs ) const
{
	if( m_CustomArgList.size() != rhs.m_CustomArgList.size() )
		return false;

	int iArgCnt = m_CustomArgList.size();
	for( int i=0 ; i<iArgCnt ; i++ )
	{
		if( m_CustomArgList[i] != rhs.m_CustomArgList[i] )
			return false;
	}

	return true;
}

bool ioShaderCustomParameters::operator!=( const ioShaderCustomParameters &rhs ) const
{
	if( *this == rhs )
		return false;

	return true;
}