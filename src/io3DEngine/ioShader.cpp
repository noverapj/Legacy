
#include "stdafx.h"

#include "ioShader.h"
#include "ioShaderManager.h"
#include "ioPass.h"
#include "HelpFunc.h"

#include "ioRenderable.h"
#include "ioAutoShaderParamSource.h"
#include "ioShaderParameter.h"
#include "ioMath.h"
#include "ErrorReport.h"


ioShader::ShaderConstant::ShaderConstant()
{
	m_Handle = NULL;

	memset( &m_iValue, 0xf, sizeof(int) );
	memset( &m_vValue, 0xf, sizeof(D3DXVECTOR4) );
	memset( &m_matValue, 0xf, sizeof(D3DXMATRIX) );
}

ioShader::ioShader( const ioHashString &name ) : ioResource( name )
{
	m_pEffect = NULL;
	m_iCurTechIdx = -1;

	m_LightCombination = LC_DIR;
	m_VertexProfile = VS_1_1;
	m_PixelProfile = PS_FIXED;
	m_ShaderType = ST_NORMAL_SHADER;
	
	m_bValidate = false;
}

ioShader::~ioShader()
{
	ClearParameterHandle();
	SAFERELEASE( m_pEffect );

	g_ShaderMgr.RemoveMem( GetName() );
}

void ioShader::InitShader( ID3DXEffect *pEffect )
{
	SAFERELEASE( m_pEffect );

	m_pEffect = pEffect;
	m_iCurTechIdx = -1;

	ParseParameterHandle();
	ValidateTechnique( 0 );
}

void ioShader::ValidateTechnique( int iIndex )
{
	if( m_iCurTechIdx == iIndex )	return;

	D3DXHANDLE hTechnique = m_pEffect->GetTechnique( iIndex );
	if( hTechnique )
	{
		m_pEffect->SetTechnique( hTechnique );
		m_iCurTechIdx = iIndex;
		m_bValidate = true;
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioShader::ValidateTechnique - %s(%d) Failed", m_Name.c_str(), iIndex );
	}
}

void ioShader::SetLightCombination( LightCombination eCombo )
{
	m_LightCombination = eCombo;
}

void ioShader::SetVertexShaderProfile( VertexShaderProfile eProfile )
{
	m_VertexProfile = eProfile;
}

void ioShader::SetPixelShaderProfile( PixelShaderProfile eProfile )
{
	m_PixelProfile = eProfile;
}

void ioShader::SetShaderType( ShaderType eType )
{
	m_ShaderType = eType;
}

void ioShader::OnLostDevice()
{
	if( m_pEffect )
		m_pEffect->OnLostDevice();
}

void ioShader::OnResetDevice()
{
	if( !m_pEffect )	return;

	m_pEffect->OnResetDevice();

	ParseParameterHandle();

	int iPreTechIdx = m_iCurTechIdx;

	m_iCurTechIdx = -1;
	ValidateTechnique( iPreTechIdx );
}

int ioShader::Begin()
{
	if( !m_bValidate )	return 0;

	UINT numPasses;

	HRESULT hr;
	hr = m_pEffect->Begin( &numPasses, D3DXFX_DONOTSAVESTATE );
	if( FAILED( hr ) )
	{
		LOG.PrintTimeAndLog( 0, "ioShader::Begin - %s Begin Failed(%X)", GetName().c_str(), hr );
		return 0;
	}

	return numPasses;
}

void ioShader::End()
{
	if( !m_bValidate )	return;

	HRESULT hr = m_pEffect->End();
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ioShader::End: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioShader::End - %s End Failed(%X)", GetName().c_str(), hr );
	}
}

void ioShader::BeginPass( int iPass )
{
	if( !m_bValidate )	return;

	HRESULT hr = m_pEffect->BeginPass( iPass );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "BeginPass: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioShader::BeginPass - %s BeginPass Failed(%X)", GetName().c_str(), hr );
	}
}

void ioShader::CommitChanges()
{
	if( !m_bValidate )	return;

	HRESULT hr = m_pEffect->CommitChanges();
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "CommitChanges: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioShader::CommitChanges - %s CommitChanges Failed(%X)", GetName().c_str(), hr );
	}
}

void ioShader::EndPass()
{
	if( !m_bValidate )	return;

	HRESULT hr = m_pEffect->EndPass();
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "EndPass: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioShader::EndPass - %s EndPass Failed(%X)", GetName().c_str(), hr );
	}
}

void ioShader::BindAutoShaderParamNoLights( const ioAutoShaderParamSource &rkSource )
{
	const D3DXVECTOR4 *pBipedList;
	int iVectorCnt = 0;

	AutoParameterList::iterator iter;
	for( iter=m_AutoParameters.begin() ; iter!=m_AutoParameters.end() ; ++iter )
	{
		switch( iter->m_Entry.m_Type )
		{
		case ACT_WORLD_MATRIX:
			SetMatrix( iter->m_pConstant, rkSource.GetWorldMatrix() );
			break;
		case ACT_VIEW_MATRIX:
			SetMatrix( iter->m_pConstant, rkSource.GetViewMatrix() );
			break;
		case ACT_PROJECTION_MATRIX:
			SetMatrix( iter->m_pConstant, rkSource.GetProjectionMatrix() );
			break;
		case ACT_BIPED_LIST:
			pBipedList = rkSource.GetBipedList( iVectorCnt );
			if( iVectorCnt > 0 )
			{
				SetBipedList( iter->m_pConstant, pBipedList, iVectorCnt );
			}
			break;
		case ACT_WEIGHTED_BIPED_COUNT:
			SetFloat( iter->m_pConstant, rkSource.GetWeightedBipedCnt() );
			break;
		case ACT_VIEWPROJ_MATRIX:
			SetMatrix( iter->m_pConstant, rkSource.GetViewProjectionMatrix() );
			break;
		case ACT_WORLDVIEW_MATRIX:
			SetMatrix( iter->m_pConstant, rkSource.GetWorldViewMatrix() );
			break;
		case ACT_WORLDVIEWPROJ_MATRIX:
			SetMatrix( iter->m_pConstant, rkSource.GetWorldViewProjMatrix() );
			break;
		case ACT_INVERSE_WORLD_MATRIX:
			SetMatrix( iter->m_pConstant, rkSource.GetInvWorldMatrix() );
			break;
		case ACT_INVERSE_VIEW_MATRIX:
			SetMatrix( iter->m_pConstant, rkSource.GetInvViewMatrix() );
			break;
		case ACT_INVERSE_WORLDVIEW_MATRIX:
			SetMatrix( iter->m_pConstant, rkSource.GetInvWorldViewMatrix() );
			break;
		case ACT_INVERSE_TRANSPOSE_WORLD_MATRIX:
			SetMatrix( iter->m_pConstant, rkSource.GetInvTransposeWorldMatrix() );
			break;
		case ACT_INVERSE_TRANSPOSE_WORLDVIEW_MATRIX:
			SetMatrix( iter->m_pConstant, rkSource.GetInvTransposeWorldViewMatrix() );
			break;
		case ACT_TEXTURE_MATRIX:
			SetMatrix( iter->m_pConstant, rkSource.GetTextureMatrix( iter->m_Entry.m_iUnitIndex ) );
			break;
		case ACT_LIGHT_WORLDVIEWPROJ_MATRIX:
			SetMatrix( iter->m_pConstant, rkSource.GetLightWorldViewProjMatrix() );
			break;
		case ACT_SHADOWMAP_MATRIX:
			SetMatrix( iter->m_pConstant, rkSource.GetShadowMapMatrix() );
			break;
		case ACT_SHADOW_COLOR:
			SetColorValue( iter->m_pConstant, rkSource.GetShadowColor() );
			break;
		case ACT_MATERIAL_DIFFUSE_COLOR:
			SetColorValue( iter->m_pConstant, rkSource.GetMaterialDiffuseColor() );
			break;
		case ACT_AMBIENT_EMISSIVE_COLOR:
			SetColorValue( iter->m_pConstant, rkSource.GetAmbientEmissiveColor() );
			break;
		case ACT_CAMERA_POSITION_OBJECT_SPACE:
			SetVector( iter->m_pConstant, rkSource.GetCameraPosObjectSpace() );
			break;
		case ACT_CAMERA_DIRECTION_OBJECT_SPACE:
			SetVector( iter->m_pConstant, rkSource.GetCameraDirObjectSpace() );
			break;
		case ACT_VIEWPORT:
			SetVector( iter->m_pConstant, rkSource.GetViewPort() );
			break;
		case ACT_CAMERA_FOV_NEAR_FAR:
			SetVector( iter->m_pConstant, rkSource.GetCameraFovNearFar() );
			break;
		case ACT_FOG_FACTOR:
			SetVector( iter->m_pConstant, rkSource.GetFogFactor() );
			break;
		case ACT_ALPHA_FACTOR:
			SetVector( iter->m_pConstant, rkSource.GetAlphaFactor() );
			break;
		case ACT_OUTLINE_CONSTANT:
			SetVector( iter->m_pConstant, rkSource.GetOutLineConstant() );
			break;
		}
	}

	BindCustomShaderParam( rkSource.GetCurRenderable()->GetCustomParameters() );
}

void ioShader::BindCustomShaderParam( const ioShaderCustomParameters &rkParamList )
{
	int iNumParams = rkParamList.GetCustomArgCount();
	for( int i=0 ; i<iNumParams ; i++ )
	{
		const ioShaderCustomArg *pArg = rkParamList.GetArg( i );

		ShaderConstant *pConstant = GetCustomConstant( pArg->GetName() );
		if( pConstant )
		{
			switch( pArg->GetType() )
			{
			case CPT_FLOAT:
				SetFloat( pConstant, pArg->GetFloat() );
				break;
			case CPT_VECTOR3:
				SetVector( pConstant, pArg->GetVector3() );
				break;
			case CPT_VECTOR4:
			case CPT_COLORVALUE:
				SetVector( pConstant, pArg->GetVector4() );
				break;
			}
		}
	}
}

void ioShader::BindAutoShaderParamOnlyLights( const ioAutoShaderParamSource &rkSource )
{
	int iApplyLightCnt = rkSource.GetApplyLightCnt();
	if( iApplyLightCnt == 0 || m_LightCombination == LC_NOLIGHT )
		return;

	int iMaxLightCnt = 0;
	switch( m_LightCombination )
	{
	case LC_DIR:
	case LC_POINT:
	case LC_SPOT:
		iMaxLightCnt = 1;
		break;
	case LC_DIR_POINT:
	case LC_DIR_SPOT:
		iMaxLightCnt = 2;
		break;
	}

	iApplyLightCnt = min( iMaxLightCnt, iApplyLightCnt );
	for( int i=0 ; i<iApplyLightCnt ;  i++ )
	{
		const ioLight *pLight = rkSource.GetApplyLight( i );
		if( !pLight )	continue;

		switch( pLight->GetType() )
		{
		case LT_DIRECTIONAL:
			BindDirectionalLight( pLight, rkSource );
			break;
		case LT_POINT:
			BindPointLight( pLight, rkSource );
			break;
		case LT_SPOT:
			BindSpotLight( pLight, rkSource );
			break;
		}
	}
}

void ioShader::BindDirectionalLight( const ioLight *pLight, const ioAutoShaderParamSource &rkSource )
{
	D3DXVECTOR3 vLightDir;
	D3DCOLORVALUE kColor;

	AutoParameterList::iterator iter;
	for( iter=m_AutoParameters.begin() ; iter!=m_AutoParameters.end() ; ++iter )
	{
		switch( iter->m_Entry.m_Type )
		{
		case ACT_DIRECTIONAL_LIGHT_DIRECTION_OBJECT_SPACE:
			vLightDir = pLight->GetDerivedDirection();
			D3DXVec3TransformNormal( &vLightDir, &vLightDir, rkSource.GetInvWorldMatrix() );
			D3DXVec3Normalize( &vLightDir, &vLightDir );
			SetVector( iter->m_pConstant, -vLightDir );
			break;
		case ACT_DIRECTIONAL_LIGHT_DIFFUSE_COLOR:
			kColor = pLight->GetRatedDiffuseColor() * rkSource.GetMaterialDiffuseColor();
			SetColorValue( iter->m_pConstant, kColor );
			break;
		case ACT_DIRECTIONAL_LIGHT_SPECULAR_COLOR:
			kColor = pLight->GetRatedSpecularColor() * rkSource.GetCurPass()->GetSpecular();
			SetColorValue( iter->m_pConstant, kColor );
			break;
		case ACT_SPECULAR_PARAM:
			SetVector( iter->m_pConstant, rkSource.GetCurPass()->GetSpecularParam() );
			break;
		}
	}
}

void ioShader::BindPointLight( const ioLight *pLight, const ioAutoShaderParamSource &rkSource )
{
	const ioPass *pPass = rkSource.GetCurPass();

	D3DXVECTOR3 vLightPos;
	D3DCOLORVALUE kColor;

	AutoParameterList::iterator iter;
	for( iter=m_AutoParameters.begin() ; iter!=m_AutoParameters.end() ; ++iter )
	{
		switch( iter->m_Entry.m_Type )
		{
		case ACT_POINT_LIGHT_POSITION_OBJECT_SPACE:
			vLightPos = pLight->GetDerivedPosition();
			D3DXVec3TransformCoord( &vLightPos, &vLightPos, rkSource.GetInvWorldMatrix() );
			SetVector( iter->m_pConstant, vLightPos );
			break;
		case ACT_POINT_LIGHT_DIFFUSE_COLOR:
			kColor = pLight->GetRatedDiffuseColor() * rkSource.GetMaterialDiffuseColor();
			SetColorValue( iter->m_pConstant, kColor );
			break;
		case ACT_POINT_LIGHT_ATTENUATION:
			SetVector( iter->m_pConstant, pLight->GetAttenuationFactor() );
			break;
		}
	}
}

void ioShader::BindSpotLight( const ioLight *pLight, const ioAutoShaderParamSource &rkSource )
{
	const ioPass *pPass = rkSource.GetCurPass();

	D3DCOLORVALUE kColor;
	D3DXVECTOR3 vTemp;

	AutoParameterList::iterator iter;
	for( iter=m_AutoParameters.begin() ; iter!=m_AutoParameters.end() ; ++iter )
	{
		switch( iter->m_Entry.m_Type )
		{
		case ACT_SPOT_LIGHT_DIFFUSE_COLOR:
			kColor = pLight->GetRatedDiffuseColor() * rkSource.GetMaterialDiffuseColor();
			SetColorValue( iter->m_pConstant, kColor );
			break;
		case ACT_SPOT_LIGHT_SPECULAR_COLOR:
			kColor = pLight->GetRatedSpecularColor() * pPass->GetSpecular();
			SetColorValue( iter->m_pConstant, kColor );
			break;
		case ACT_SPOT_LIGHT_ATTENUATION:
			SetVector( iter->m_pConstant, pLight->GetAttenuationFactor() );
			break;
		case ACT_SPOT_LIGHT_DIRECTION_OBJECT_SPACE:
			vTemp = pLight->GetDerivedDirection();
			D3DXVec3TransformNormal( &vTemp, &vTemp, rkSource.GetInvWorldMatrix() );
			D3DXVec3Normalize( &vTemp, &vTemp );
			SetVector( iter->m_pConstant, vTemp );
			break;
		case ACT_SPOT_LIGHT_POSITION_OBJECT_SPACE:
			vTemp = pLight->GetDerivedPosition();
			D3DXVec3TransformCoord( &vTemp, &vTemp, rkSource.GetInvWorldMatrix() );
			SetVector( iter->m_pConstant, vTemp );
			break;
		case ACT_SPOT_LIGHT_FACTOR:
			SetVector( iter->m_pConstant, pLight->GetSpotFactor() );
			break;
		}
	}
}

void ioShader::ClearParameterHandle()
{
	NameParameterList::iterator iter;
	for( iter=m_NameParameters.begin() ; iter!=m_NameParameters.end() ; ++iter )
	{
		delete iter->second;
	}

	m_AutoParameters.clear();
	m_CustomParameters.clear();
	m_NameParameters.clear();
}

void ioShader::ParseParameterHandle()
{
	ClearParameterHandle();

	D3DXEFFECT_DESC kFxDesc;

	HRESULT hr = m_pEffect->GetDesc( &kFxDesc );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "ParseParameterHandle: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioShader::ParseParameterHandle - %s Failed(%X)", m_Name.c_str(), hr );
		return;
	}

	ShaderConstant *pConstant;
	D3DXPARAMETER_DESC kParamDesc;
	for( UINT i=0 ; i<kFxDesc.Parameters ; i++ )
	{
		pConstant = new ShaderConstant;
		pConstant->m_Handle = m_pEffect->GetParameter( NULL, i );
		if( pConstant->m_Handle == NULL )
		{
			LOG.PrintTimeAndLog( 0, "ioShader::ParseParameterHandle - %s:%d GetParameter Failed(%x)",
									GetName().c_str(), i, hr );
			delete pConstant;
			continue;
		}

		hr = m_pEffect->GetParameterDesc( pConstant->m_Handle, &kParamDesc );
		if( FAILED(hr) )
		{
			ErrorReport::SetFormat( "ParseParameterHandle: %d, %x", i, hr );
			LOG.PrintTimeAndLog( 0, "ioShader::ParseParameterHandle - %s:%d GetParameterDesc Failed(%x)",
									GetName().c_str(), i, hr );
			continue;
		}

		AutoConstantEntry kConstantEntry = ParsingConstantEntry( kParamDesc );
		if( kConstantEntry.m_Type != ACT_CUSTOM )
		{
			AutoParameter kParameter;
			kParameter.m_Entry = kConstantEntry;
			kParameter.m_pConstant = pConstant;
			m_AutoParameters.push_back( kParameter );
		}
		else
		{
			CustomParameter kParameter;
			kParameter.m_ParamName = kParamDesc.Name;
			kParameter.m_Entry = kConstantEntry;
			kParameter.m_pConstant = pConstant;
			m_CustomParameters.push_back( kParameter );
		}

		m_NameParameters.insert( NameParameterList::value_type( ioHashString( kParamDesc.Name), pConstant ) );
	}
}

ioShader::ShaderConstant* ioShader::GetNameConstant( const ioHashString &szName )
{
	NameParameterList::iterator iter = m_NameParameters.find( szName );
	if( iter != m_NameParameters.end() )
		return iter->second;

	return NULL;
}

ioShader::ShaderConstant* ioShader::GetCustomConstant( const ioHashString &szName )
{
	CustomParameterList::iterator iter;
	for( iter= m_CustomParameters.begin() ; iter!=m_CustomParameters.end() ; ++iter )
	{
		if( iter->m_ParamName == szName )
			return iter->m_pConstant;
	}

	return NULL;
}

ioShader::ShaderConstant* ioShader::GetEntryConstant( const AutoConstantEntry &rkEntry )
{
	AutoParameterList::iterator iter;
	for( iter=m_AutoParameters.begin() ; iter!=m_AutoParameters.end() ; ++iter )
	{
		if( iter->m_Entry.m_Type == rkEntry.m_Type &&
			iter->m_Entry.m_iUnitIndex == rkEntry.m_iUnitIndex )
			return iter->m_pConstant;
	}

	return NULL;
}

void ioShader::SetInt( const ioHashString &szName, int iVal )
{
	ShaderConstant *pConstant = GetNameConstant( szName );
	if( pConstant )
	{
		SetInt( pConstant, iVal );
	}
}

void ioShader::SetFloat( const ioHashString &szName, float fVal )
{
	ShaderConstant *pConstant = GetNameConstant( szName );
	if( pConstant )
	{
		SetFloat( pConstant, fVal );
	}
}

void ioShader::SetFloatArray( const ioHashString &szName, const float *pArray, int iCount )
{
	ShaderConstant *pConstant = GetNameConstant( szName );
	if( pConstant )
	{
		SetFloatArray( pConstant, pArray, iCount );
	}
}

void ioShader::SetVector( const ioHashString &szName, const D3DXVECTOR3 &vVec3 )
{
	D3DXVECTOR4	vVec4( vVec3.x, vVec3.y, vVec3.z, 1.0f );
	SetVector( szName, vVec4 );
}

void ioShader::SetVector( const ioHashString &szName, const D3DXVECTOR4 &vVec4 )
{
	ShaderConstant *pConstant = GetNameConstant( szName );
	if( pConstant )
	{
		SetVector( pConstant, vVec4 );
	}
}

void ioShader::SetVectorArray( const ioHashString &szName, const D3DXVECTOR4 *pVec4, int iCount )
{
	ShaderConstant *pConstant = GetNameConstant( szName );
	if( pConstant )
	{
		SetVectorArray( pConstant, pVec4, iCount );
	}
}

void ioShader::SetColorValue( const ioHashString &szName, const D3DCOLORVALUE &rColor )
{
	ShaderConstant *pConstant = GetNameConstant( szName );
	if( pConstant )
	{
		SetVector( pConstant, (const D3DXVECTOR4&)rColor );
	}
}

void ioShader::SetMatrix( const ioHashString &szName, const D3DXMATRIX &mat )
{
	ShaderConstant *pConstant = GetNameConstant( szName );
	if( pConstant )
	{
		SetMatrix( pConstant, &mat );
	}
}

void ioShader::SetTexture( const ioHashString &szName, IDirect3DTexture9 *pTex )
{
	ShaderConstant *pConstant = GetNameConstant( szName );
	if( pConstant )
	{
		m_pEffect->SetTexture( pConstant->m_Handle, pTex );
	}
}

void ioShader::SetInt( ShaderConstant *pConstant, int iVal )
{
	if( pConstant->m_iValue != iVal )
	{
		HRESULT hr = m_pEffect->SetInt( pConstant->m_Handle, iVal );
		if( SUCCEEDED(hr) )
		{
			pConstant->m_iValue = iVal;
		}
	}
}

void ioShader::SetFloat( ShaderConstant *pConstant, float fVal )
{
	if( pConstant->m_vValue.x != fVal )
	{
		HRESULT hr = m_pEffect->SetFloat( pConstant->m_Handle, fVal );
		if( SUCCEEDED(hr) )
		{
			pConstant->m_vValue.x = fVal;
		}
	}
}

void ioShader::SetFloatArray( ShaderConstant *pConstant, const float *pArray, int iCount )
{
	m_pEffect->SetFloatArray( pConstant->m_Handle, pArray, iCount );
}

void ioShader::SetVector( ShaderConstant *pConstant, const D3DXVECTOR3 &vVec3 )
{
	SetVector( pConstant, D3DXVECTOR4( vVec3.x, vVec3.y, vVec3.z, 1.0f ) );
}

void ioShader::SetVector( ShaderConstant *pConstant, const D3DXVECTOR4 &vVec4 )
{
	if( !ioMath::IsEqual( pConstant->m_vValue, vVec4 ) )
	{
		HRESULT hr = m_pEffect->SetVector( pConstant->m_Handle, &vVec4 );
		if( SUCCEEDED(hr) )
		{
			pConstant->m_vValue = vVec4;
		}
	}
}

void ioShader::SetColorValue( ShaderConstant *pConstant, const D3DCOLORVALUE &rColor )
{
	if( !ioMath::IsEqual( pConstant->m_vValue, (const D3DXVECTOR4&)rColor ) )
	{
		HRESULT hr = m_pEffect->SetVector( pConstant->m_Handle, &(const D3DXVECTOR4&)rColor );
		if( SUCCEEDED(hr) )
		{
			pConstant->m_vValue = (const D3DXVECTOR4&)rColor;
		}
	}
}

void ioShader::SetVectorArray( ShaderConstant *pConstant, const D3DXVECTOR4 *pVec4, int iCount )
{
	m_pEffect->SetVectorArray( pConstant->m_Handle, pVec4, iCount );
}

void ioShader::SetBipedList( ShaderConstant *pConstant, const D3DXVECTOR4 *pVec4, int iCount )
{
	SetVectorArray( pConstant, pVec4, iCount );
}

void ioShader::SetMatrix( ShaderConstant *pConstant, const D3DXMATRIX *pMat )
{
	if( pConstant->m_matValue != *pMat )
	{
		HRESULT hr = m_pEffect->SetMatrix( pConstant->m_Handle, pMat );
		if( SUCCEEDED(hr) )
		{
			pConstant->m_matValue = *pMat;
		}
	}
}
