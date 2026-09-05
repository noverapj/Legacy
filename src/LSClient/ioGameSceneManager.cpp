

#include "stdafx.h"

#include "../io3DEngine/ioRenderTexture.h"

#include "../io3DEngine/ioPatch.h"
#include "../io3DEngine/ioLandScape.h"

#include "../io3DEngine/ioVertexDeclaration.h"
#include "../io3DEngine/ioVertexBufferInstance.h"
#include "../io3DEngine/ioVertexBufferBinder.h"
#include "../io3DEngine/ioSceneShadowBox.h"

#include "ioGameSceneManager.h"



#include "ioPlayStage.h"
#include "ioWorldMesh.h"
#include "ioScreenBlind.h"

#include "DirectInput.h"

ioGameSceneManager::ioGameSceneManager( ioRenderSystem *pRenderer ) : ioSceneManager( pRenderer )
{
	m_pLandScape = NULL;
	m_pWorldMesh = NULL;
	m_pSkyDome   = NULL;

	m_pScreenBlinder = NULL;
	m_bUseScreenBlinder = false;

	m_DefaultDiffuseRate.r = FLOAT1;
	m_DefaultDiffuseRate.g = FLOAT1;
	m_DefaultDiffuseRate.b = FLOAT1;
	m_DefaultDiffuseRate.a = FLOAT1;

	m_bShowSky   = true;

	m_bMultiMap  = false;
	m_iMultiMapCurrentIndex = 0;
	m_iMaxMultiMapCount = 1;
}

ioGameSceneManager::~ioGameSceneManager()
{
	ClearScene();
	ClearPostProcess();
	ClearShadowMapResource();
}

void ioGameSceneManager::UpdateSceneGraph( ioCamera *pCamera, const D3DXMATRIX *pExtraProj, bool bResetVolume )
{
	Setting::SetDimensionConvertInfo( m_pRenderSystem->GetViewPort(),
									  *pCamera->GetViewMatrix(),
									  *pCamera->GetProjMatrix() );

	if( m_pLandScape )
	{
		m_pLandScape->UpdateTerrain( pCamera );
	}

	if( m_pWorldMesh )
	{
		m_pWorldMesh->Update();
	}

	if( m_pSkyDome )
	{
		m_pSkyDome->NotifyCurrentCamera( pCamera );
	}

	CheckDecalList();

	ioSceneManager::UpdateSceneGraph( pCamera, pExtraProj, bResetVolume );

	if( m_bUseScreenBlinder && m_pScreenBlinder )
	{
		m_RenderQueue.AddRenderable( m_pScreenBlinder, RENDER_GROUP_4 );
	}
}

void ioGameSceneManager::RenderLandscape()
{
	if( m_pSkyDome && m_bShowSky )
	{
		m_pRenderSystem->SetCullingMode( CM_NO );
		m_pSkyDome->Render( IsNowFog() );
	}

	if( m_pLandScape )
	{
		for( int i=0 ; i<2 ; i++ )
		{
			m_pRenderSystem->SetTextureAddressMode( i, TAM_WRAP );
			m_pRenderSystem->SetTextureFiltering( i, m_TextureFilterOpt[i] );
			m_pRenderSystem->SetTextureMipMapBias( i, m_TextureMipMapBias[i] );
		}
		m_pRenderSystem->SetCullingMode( CM_CCW );

		ioLightList kPointList;
		PopulateLandScapePointLightList( kPointList );
		m_pLandScape->Render( m_AutoShaderParamSource,
							  GetLight( "MainLight" ),
							  kPointList );
	}
}

void ioGameSceneManager::PopulateLandScapePointLightList( ioLightList &kPointList )
{
	D3DXVECTOR3 vCameraPos, vLength;
	vCameraPos = m_AutoShaderParamSource.GetCameraPos();

	SceneLightList::iterator iter;
	for( iter=m_SceneLightList.begin() ; iter!=m_SceneLightList.end() ; ++iter )
	{
		ioLight *pLight = iter->second;

		if( !pLight->IsVisible() )	continue;
		if( pLight->GetType() != LT_POINT )	continue;
		if( !pLight->IsValidLightByReceiveType( LRT_JUST_PURE_REAL_TIME ) )
			continue;

		vLength = pLight->GetDerivedPosition() - vCameraPos;
		pLight->m_fTempSquareDist = D3DXVec3LengthSq( &vLength );
		kPointList.push_back( pLight );
	}

	if( kPointList.size() > 1 )
	{
		std::sort( kPointList.begin(), kPointList.end(), LightLess() );
	}

	if( kPointList.size() > MAX_POINT_LIGHT )
	{
		kPointList.erase( kPointList.begin() + MAX_POINT_LIGHT, kPointList.end() );
	}
}

void ioGameSceneManager::CheckDecalList()
{
	if( !m_pWorldMesh )	return;
	if( Setting::GetLightQuality() != LQ_HIGH )
		return;

	SceneLightList::iterator iter;
	for( iter=m_SceneLightList.begin() ; iter!=m_SceneLightList.end() ; ++iter )
	{
		ioLight *pLight = iter->second;

		if( pLight->GetType() != LT_POINT )
			continue;

		if( !m_pWorldMesh->HasLightDecal( GetMultiMapCurrentIndex(), pLight->GetName() ) )
		{
			m_pWorldMesh->AddOmniLightDecal( GetMultiMapCurrentIndex(), pLight );
		}
		else
		{
			if( pLight->HasAttribute( ioLight::LAM_EFFECT_LIGHT ) )
			{
				m_pWorldMesh->UpdateEffectLightDecal( GetMultiMapCurrentIndex(), pLight );
			}
		}
	}
}

void ioGameSceneManager::SetMainLight( const D3DXVECTOR3 &vDir,
									   const D3DCOLORVALUE &rkDiffuse,
									   const D3DCOLORVALUE &rkSpecular,
									   const ioHashString &rkLightTexture )
{
	ioLight *pMainLight = GetLight( "MainLight" );
	if( !pMainLight )
	{
		pMainLight = CreateLight( "MainLight" );
		pMainLight->SetAttribute( ioLight::LAM_REAL_TIME );
		pMainLight->SetType( LT_DIRECTIONAL );
	}

	pMainLight->SetDiffuseColor( rkDiffuse.r, rkDiffuse.g, rkDiffuse.b );
	pMainLight->SetSpecularColor( rkSpecular.r, rkSpecular.g, rkSpecular.b );
	pMainLight->SetLightTexture( rkLightTexture );

	D3DXVECTOR3 vNormalizeDir;
	D3DXVec3Normalize( &vNormalizeDir, &vDir );
	pMainLight->SetDirection( vNormalizeDir );
	pMainLight->SetLightApplyMask( APPLY_FIRST_MAINLIGHT );
}

void ioGameSceneManager::LoadMeshMap( ioINILoader &rkLoader, ioPlayStage *pStage )
{
	rkLoader.SetTitle( "map" );
	m_bMultiMap = rkLoader.LoadBool_e( "multi_map", false );
	m_iMaxMultiMapCount = rkLoader.LoadInt_e( "max_multi_map", 1 );
	m_iMultiMapCurrentIndex = 0;          

	SAFEDELETE( m_pWorldMesh );

	m_pWorldMesh = new ioWorldMesh( this );
	if( !m_pWorldMesh->LoadWorldMesh( rkLoader ) )
		return;

	m_ShadowMapDataList.clear();
	m_CameraMapLimitList.clear();
	m_BoostLimitList.clear();
	m_AxisLimitList.clear();
	m_AmbientDataList.clear();

	if( IsMultiMap() )
	{
		int i = 0;
		char szKey[MAX_PATH];
		// Ambient
		rkLoader.SetTitle_e( "ambient" );
		for(i = 0;i < GetMaxMultiMapCount();i++)
		{
			AmbientData kAmbient;
			sprintf_e( szKey, "red%d", i + 1 );
			kAmbient.m_fRed   = rkLoader.LoadFloat( szKey, 127.0f ) / 255.0f;
			sprintf_e( szKey, "green%d", i + 1 );
			kAmbient.m_fGreen = rkLoader.LoadFloat( szKey, 127.0f ) / 255.0f;
			sprintf_e( szKey, "blue%d", i + 1 );
			kAmbient.m_fBlue  = rkLoader.LoadFloat( szKey, 127.0f ) / 255.0f;
			m_AmbientDataList.push_back( kAmbient );
		}

		// Shadow 
		rkLoader.SetTitle_e( "shadow" );
		for(i = 0;i < GetMaxMultiMapCount();i++)
		{
			ShadowMapData kShadow;

			sprintf_e( szKey, "texture_size%d", i + 1 );
			kShadow.m_iTextureSize = rkLoader.LoadInt( szKey, 1024 );

			sprintf_e( szKey, "shadow_r%d", i + 1 );
			kShadow.m_fRed   = (float)rkLoader.LoadInt( szKey, 127 ) / 255.0f;
			sprintf_e( szKey, "shadow_g%d", i + 1 );
			kShadow.m_fGreen = (float)rkLoader.LoadInt( szKey, 127 ) / 255.0f;
			sprintf_e( szKey, "shadow_b%d", i + 1 );
			kShadow.m_fBlue  = (float)rkLoader.LoadInt( szKey, 127 ) / 255.0f;

			D3DXVECTOR3 vPos, vTarget;
			sprintf_e( szKey, "pos_x%d", i + 1 );
			vPos.x = rkLoader.LoadFloat( szKey, 0.0f );
			sprintf_e( szKey, "pos_y%d", i + 1 );
			vPos.y = rkLoader.LoadFloat( szKey, FLOAT100 );
			sprintf_e( szKey, "pos_z%d", i + 1 );
			vPos.z = rkLoader.LoadFloat( szKey, FLOAT100 );

			sprintf_e( szKey, "target_x%d", i + 1 );
			vTarget.x = rkLoader.LoadFloat( szKey, 0.0f );
			sprintf_e( szKey, "target_y%d", i + 1 );
			vTarget.y = rkLoader.LoadFloat( szKey, 0.0f );
			sprintf_e( szKey, "target_z%d", i + 1 );
			vTarget.z = rkLoader.LoadFloat( szKey, 0.0f );
	
			kShadow.m_vShadowDir = vTarget - vPos;
			m_ShadowMapDataList.push_back( kShadow );
		}

		// Map Camera
		rkLoader.SetTitle_e( "map_camera" );
		for(i = 0;i < GetMaxMultiMapCount();i++)
		{
			CameraMapLimit kCameraLimit;
			sprintf_e( szKey, "min_x%d", i + 1 );
			kCameraLimit.m_vMinLookAt.x = rkLoader.LoadFloat( szKey, -100000.0f );
			sprintf_e( szKey, "min_y%d", i + 1 );
			kCameraLimit.m_vMinLookAt.y = rkLoader.LoadFloat( szKey, 0.0f );
			sprintf_e( szKey, "min_z%d", i + 1 );
			kCameraLimit.m_vMinLookAt.z = rkLoader.LoadFloat( szKey, -100000.0f );

			sprintf_e( szKey, "max_x%d", i + 1 );
			kCameraLimit.m_vMaxLookAt.x = rkLoader.LoadFloat( szKey, 100000.0f );
			sprintf_e( szKey, "max_y%d", i + 1 );
			kCameraLimit.m_vMaxLookAt.y = rkLoader.LoadFloat( szKey, 100000.0f );
			sprintf_e( szKey, "max_z%d", i + 1 );
			kCameraLimit.m_vMaxLookAt.z = rkLoader.LoadFloat( szKey, 100000.0f );
			
			m_CameraMapLimitList.push_back( kCameraLimit );
		}

		// Boost Limit
		rkLoader.SetTitle_e( "boost_limit" );
		for(i = 0;i < GetMaxMultiMapCount();i++)
		{
			BoostLimit kBoostLimit;
			sprintf_e( szKey, "min_x%d", i + 1 );
			kBoostLimit.m_vMin.x = rkLoader.LoadFloat( szKey, -6500.0f );
			sprintf_e( szKey, "min_y%d", i + 1 );
			kBoostLimit.m_vMin.y = rkLoader.LoadFloat( szKey, -FLOAT1000 );
			sprintf_e( szKey, "min_z%d", i + 1 );
			kBoostLimit.m_vMin.z = rkLoader.LoadFloat( szKey, -4500.0f );

			sprintf_e( szKey, "max_x%d", i + 1 );
			kBoostLimit.m_vMax.x = rkLoader.LoadFloat( szKey, 7000.0f );
			sprintf_e( szKey, "max_y%d", i + 1 );
			kBoostLimit.m_vMax.y = rkLoader.LoadFloat( szKey, 5000.0f );
			sprintf_e( szKey, "max_z%d", i + 1 );
			kBoostLimit.m_vMax.z = rkLoader.LoadFloat( szKey, 4500.0f );
			m_BoostLimitList.push_back( kBoostLimit );
		}

		// Axis Limit
		rkLoader.SetTitle_e( "axis_limit" );
		for(i = 0;i < GetMaxMultiMapCount();i++)
		{
			AxisLimit kAxisLimit;

			sprintf_e( szKey, "min_x%d", i + 1 );
			kAxisLimit.m_vMin.x = rkLoader.LoadFloat( szKey, 0.0f );
			sprintf_e( szKey, "min_y%d", i + 1 );
			kAxisLimit.m_vMin.y = rkLoader.LoadFloat( szKey, 0.0f );
			sprintf_e( szKey, "min_z%d", i + 1 );
			kAxisLimit.m_vMin.z = rkLoader.LoadFloat( szKey, 0.0f );

			sprintf_e( szKey, "max_x%d", i + 1 );
			kAxisLimit.m_vMax.x = rkLoader.LoadFloat( szKey, 0.0f );
			sprintf_e( szKey, "max_y%d", i + 1 );
			kAxisLimit.m_vMax.y = rkLoader.LoadFloat( szKey, 0.0f );
			sprintf_e( szKey, "max_z%d", i + 1 );
			kAxisLimit.m_vMax.z = rkLoader.LoadFloat( szKey, 0.0f );

			m_AxisLimitList.push_back( kAxisLimit );
		}
	}
	else
	{   // ±âÁ¸ ¸Ê INI Á¤º¸ - 
		// Ambient
		rkLoader.SetTitle_e( "ambient" );
		AmbientData kAmbient;
		kAmbient.m_fRed   = rkLoader.LoadFloat( "red", 127.0f ) / 255.0f;
		kAmbient.m_fGreen = rkLoader.LoadFloat_e( "green", 127.0f ) / 255.0f;
		kAmbient.m_fBlue  = rkLoader.LoadFloat_e( "blue", 127.0f ) / 255.0f;
		m_AmbientDataList.push_back( kAmbient );

		// Shadow
		rkLoader.SetTitle_e( "shadow" );
		ShadowMapData kShadow;
		kShadow.m_iTextureSize = rkLoader.LoadInt_e( "texture_size", 512 );
		kShadow.m_fRed   = (float)rkLoader.LoadInt_e( "shadow_r", 127 ) / 255.0f;
		kShadow.m_fGreen = (float)rkLoader.LoadInt_e( "shadow_g", 127 ) / 255.0f;
		kShadow.m_fBlue  = (float)rkLoader.LoadInt_e( "shadow_b", 127 ) / 255.0f;

		D3DXVECTOR3 vPos, vTarget;
		vPos.x = rkLoader.LoadFloat_e( "pos_x", 0.0f );
		vPos.y = rkLoader.LoadFloat_e( "pos_y", FLOAT100 );
		vPos.z = rkLoader.LoadFloat_e( "pos_z", FLOAT100 );
		vTarget.x = rkLoader.LoadFloat_e( "target_x", 0.0f );
		vTarget.y = rkLoader.LoadFloat_e( "target_y", 0.0f );
		vTarget.z = rkLoader.LoadFloat_e( "target_z", 0.0f );
		kShadow.m_vShadowDir = vTarget - vPos;
		m_ShadowMapDataList.push_back( kShadow );

		// Map Camera
		CameraMapLimit kCameraLimit;
		rkLoader.SetTitle_e( "map_camera" );
		kCameraLimit.m_vMinLookAt.x = rkLoader.LoadFloat_e( "min_x", -100000.0f );
		kCameraLimit.m_vMinLookAt.y = rkLoader.LoadFloat_e( "min_y", 0.0f );
		kCameraLimit.m_vMinLookAt.z = rkLoader.LoadFloat_e( "min_z", -100000.0f );

		kCameraLimit.m_vMaxLookAt.x = rkLoader.LoadFloat_e( "max_x", 100000.0f );
		kCameraLimit.m_vMaxLookAt.y = rkLoader.LoadFloat_e( "max_y", 100000.0f );
		kCameraLimit.m_vMaxLookAt.z = rkLoader.LoadFloat_e( "max_z", 100000.0f );
		m_CameraMapLimitList.push_back( kCameraLimit );

		// Boost Limit
		BoostLimit kBoostLimit;
		rkLoader.SetTitle_e( "boost_limit" );
		kBoostLimit.m_vMin.x = rkLoader.LoadFloat_e( "min_x", -6500.0f );
		kBoostLimit.m_vMin.y = rkLoader.LoadFloat_e( "min_y", -FLOAT1000 );
		kBoostLimit.m_vMin.z = rkLoader.LoadFloat_e( "min_z", -4500.0f );

		kBoostLimit.m_vMax.x = rkLoader.LoadFloat_e( "max_x", 7000.0f );
		kBoostLimit.m_vMax.y = rkLoader.LoadFloat_e( "max_y", 5000.0f );
		kBoostLimit.m_vMax.z = rkLoader.LoadFloat_e( "max_z", 4500.0f );
		m_BoostLimitList.push_back( kBoostLimit );

		// Axis Limit
		rkLoader.SetTitle_e( "axis_limit" );

		AxisLimit kAxisLimit;

		kAxisLimit.m_vMin.x = rkLoader.LoadFloat_e( "min_x", 0.0f );
		kAxisLimit.m_vMin.y = rkLoader.LoadFloat_e( "min_y", 0.0f );
		kAxisLimit.m_vMin.z = rkLoader.LoadFloat_e( "min_z", 0.0f );

		kAxisLimit.m_vMax.x = rkLoader.LoadFloat_e( "max_x", 0.0f );
		kAxisLimit.m_vMax.y = rkLoader.LoadFloat_e( "max_y", 0.0f );
		kAxisLimit.m_vMax.z = rkLoader.LoadFloat_e( "max_z", 0.0f );

		m_AxisLimitList.push_back( kAxisLimit );
	}	
    
	PreLoadAmbientDataSet();
	PreLoadShadowMapDataSet();

	LoadMeshMapMainLights( rkLoader );
	
	rkLoader.SetTitle( "fog" );
	if( rkLoader.LoadBool_e( "enable", false ) )
	{
		int iRed, iGreen, iBlue;
		iRed   = rkLoader.LoadInt( "red", 255 );
		iGreen = rkLoader.LoadInt_e( "green", 255 );
		iBlue  = rkLoader.LoadInt_e( "blue", 255 );

		DWORD dwFogColor = D3DCOLOR_ARGB( 255, iRed, iGreen, iBlue );

		float fStart, fEnd;
		fStart = rkLoader.LoadFloat_e( "start", FLOAT1000 );
		fEnd   = rkLoader.LoadFloat( "end", 20000.0f );

		m_pRenderSystem->SetFogFactor( dwFogColor, fStart, fEnd );
		SetFogEnable( true );
	}
	else
	{
		SetFogEnable( false );
	}

	float fGlowAlpha = rkLoader.LoadFloat_e( "glow", "glow_alpha", 0.2f );
	fGlowAlpha = max( 0.0f, min( fGlowAlpha, FLOAT1 ) );
	SetGlowAlphaValue( (int)(255.0f * fGlowAlpha) );
	SetGlowSecondAlphaValue( 0 );

	EnableMapEdge( Setting::UseMapEdge() );
}

void ioGameSceneManager::EnableGlow( bool bEnable )
{
	EnableGlowProcess( bEnable, "blurfilter.dds", "channel.dds" );
}


void ioGameSceneManager::EnableLocalGlow( bool bEnable )
{
	//EnableGlowProcessSecond( bEnable, "blurfilter.dds", "alpha_90.dds" );
	//EnableGlowProcessSecond( bEnable, "channel.dds", "channel.dds" );
	//EnableGlowProcessSecond( bEnable, "alpha_90.dds", "alpha_90.dds" );
	EnableGlowProcessSecond( bEnable, "alpha_10.dds", "alpha_10.dds" );
}

void ioGameSceneManager::EnableFilter( FilterType eType )
{
	switch( eType )
	{
	case FILTER_NONE:
		EnableFilterProcess( false, "" );
		break;
	case FILTER_GRAY40:
		SetFilterAlphaValue( 102 );
		EnableFilterProcess( true, "filter_gray.fxo" );
		break;
	case FILTER_GRAY80:
		SetFilterAlphaValue( 204 );
		EnableFilterProcess( true, "filter_gray.fxo" );
		break;
	case FILTER_GRAY100:
		SetFilterAlphaValue( 255 );
		EnableFilterProcess( true, "filter_gray.fxo" );
		break;
	case FILTER_SEPIA40:
		SetFilterAlphaValue( 102 );
		EnableFilterProcess( true, "filter_sepia.fxo" );
		break;
	case FILTER_SEPIA80:
		SetFilterAlphaValue( 204 );
		EnableFilterProcess( true, "filter_sepia.fxo" );
		break;
	case FILTER_SEPIA100:
		SetFilterAlphaValue( 255 );
		EnableFilterProcess( true, "filter_sepia.fxo" );
		break;
	}
}

void ioGameSceneManager::EnableOveray( OverayType eType )
{
	switch( eType )
	{
	case OVERAY_NONE:
		EnableOverayProcess( false, "", 0, 0, 0 );
		break;
	case OVERAY_BLACK:
		EnableOverayProcess( true, "channel.dds", 0, 0, 0 );
		break;
	case OVERAY_WHITE:
		EnableOverayProcess( true, "channel.dds", 255, 255, 255 );
		break;
	}
}

void ioGameSceneManager::EnableMapEdge( bool bEnable )
{
	if( m_pWorldMesh )
	{
		m_pWorldMesh->MapEdgeEnable( bEnable );
	}
}

void ioGameSceneManager::StopWorldMeshTexEffect( bool bStop )
{
	if( m_pWorldMesh )
	{
		m_pWorldMesh->StopAllTextureEffect( bStop );
	}
}

void ioGameSceneManager::SetScreenBlind( DWORD dwColor )
{
	if( ( dwColor & 0xff000000 ) <= 0 )
	{
		m_bUseScreenBlinder = false;
		return;
	}

	if( !m_pScreenBlinder )
	{
		m_pScreenBlinder = new ioScreenBlind;
		m_pScreenBlinder->InitResource();
	}

	m_pScreenBlinder->SetTextureFactor( dwColor );
	m_bUseScreenBlinder = true;
}

void ioGameSceneManager::LoadMeshMapMainLights( ioINILoader &rkLoader )
{
	m_MainLIghtDataList.clear();

	if( IsMultiMap() )
	{
		rkLoader.SetTitle_e( "main_light" );
		char szKey[MAX_PATH], szBuf[MAX_PATH];
		for(int i = 0;i < GetMaxMultiMapCount();i++)
		{
			MainLIghtData kMainLightData;
						
			sprintf_e( szKey, "toon_texture%d", i + 1 );
			rkLoader.LoadString( szKey, "toongray.dds", szBuf, MAX_PATH );
			kMainLightData.m_szToonTexture = szBuf;

			sprintf_e( szKey, "red%d", i + 1 );
			kMainLightData.m_Diffuse.r = rkLoader.LoadFloat( szKey, 127.0f ) / 255.0f;
			sprintf_e( szKey, "green%d", i + 1 );
			kMainLightData.m_Diffuse.g = rkLoader.LoadFloat( szKey, 127.0f ) / 255.0f;
			sprintf_e( szKey, "blue%d", i + 1 );
			kMainLightData.m_Diffuse.b = rkLoader.LoadFloat( szKey, 127.0f ) / 255.0f;

			D3DXVECTOR3 vPos, vTarget;
			sprintf_e( szKey, "pos_x%d", i + 1 );
			vPos.x = rkLoader.LoadFloat( szKey, 0.0f );
			sprintf_e( szKey, "pos_y%d", i + 1 );
			vPos.y = rkLoader.LoadFloat( szKey, FLOAT100 );
			sprintf_e( szKey, "pos_z%d", i + 1 );
			vPos.z = rkLoader.LoadFloat( szKey, FLOAT100 );

			sprintf_e( szKey, "target_x%d", i + 1 );
			vTarget.x = rkLoader.LoadFloat( szKey, 0.0f );
			sprintf_e( szKey, "target_y%d", i + 1 );
			vTarget.y = rkLoader.LoadFloat( szKey, 0.0f );
			sprintf_e( szKey, "target_z%d", i + 1 );
			vTarget.z = rkLoader.LoadFloat( szKey, 0.0f );

			kMainLightData.m_vLightDir = vTarget - vPos;
			D3DXVec3Normalize( &kMainLightData.m_vLightDir, &kMainLightData.m_vLightDir );

			sprintf_e( szKey, "second_toon_texture%d", i + 1 );
			rkLoader.LoadString( szKey, "toongray.dds", szBuf, MAX_PATH );
			kMainLightData.m_szSecondToonTexture = szBuf;

			m_MainLIghtDataList.push_back( kMainLightData );
		}
	}
	else
	{
		char szBuf[MAX_PATH];
		rkLoader.SetTitle_e( "main_light" );

		MainLIghtData kMainLightData;
		rkLoader.LoadString_e( "toon_texture", "toongray.dds", szBuf, MAX_PATH );
		kMainLightData.m_szToonTexture = szBuf;

		kMainLightData.m_Diffuse.r = rkLoader.LoadFloat( "red", 127.0f ) / 255.0f;
		kMainLightData.m_Diffuse.g = rkLoader.LoadFloat_e( "green", 127.0f ) / 255.0f;
		kMainLightData.m_Diffuse.b = rkLoader.LoadFloat_e( "blue", 127.0f ) / 255.0f;

		D3DXVECTOR3 vPos, vTarget;
		vPos.x = rkLoader.LoadFloat_e( "pos_x", 0.0f );
		vPos.y = rkLoader.LoadFloat_e( "pos_y", FLOAT100 );
		vPos.z = rkLoader.LoadFloat_e( "pos_z", FLOAT100 );

		vTarget.x = rkLoader.LoadFloat_e( "target_x", 0.0f );
		vTarget.y = rkLoader.LoadFloat_e( "target_y", 0.0f );
		vTarget.z = rkLoader.LoadFloat_e( "target_z", 0.0f );

		kMainLightData.m_vLightDir = vTarget - vPos;
		D3DXVec3Normalize( &kMainLightData.m_vLightDir, &kMainLightData.m_vLightDir );

		rkLoader.LoadString_e( "second_toon_texture", "toongray.dds", szBuf, MAX_PATH );
		kMainLightData.m_szSecondToonTexture = szBuf;

		m_MainLIghtDataList.push_back( kMainLightData );
	}
	
	PreLoadMainLightDataSet();
}

void ioGameSceneManager::LoadMeshMapLight( ioINILoader &rkLoader )
{
	char szLightFile[MAX_PATH];
	rkLoader.LoadString( "map", "light_file", "", szLightFile, MAX_PATH );
	if( !strcmp( szLightFile, "" ) )
		return;

	ioBinaryStream kStream;
	if( !g_ResourceLoader.LoadStream( szLightFile, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioGameSceneManager::LoadMeshMapLight - %s Load Failed", szLightFile );
		return;
	}

	int iLightCnt;
	kStream.Read(&iLightCnt, sizeof(int));
	for( int i=0 ; i<iLightCnt ; i++ )
	{
		int iNameLen;
		kStream.Read(&iNameLen, sizeof(int));

		char szBuf[MAX_PATH];
		memset( szBuf, 0, MAX_PATH );
		kStream.Read(szBuf, iNameLen);

		D3DLIGHT9 kD3DLight;
		kStream.Read(&kD3DLight, sizeof(D3DLIGHT9));
		
		ioLight *pLight = CreateLight( szBuf );
		pLight->SetD3DLight( kD3DLight );
		pLight->SetAttribute( ioLight::LAM_REAL_TIME );

		kStream.Read(&iNameLen, sizeof(int));
		if( iNameLen > 0 )
		{
			memset( szBuf, 0, MAX_PATH );
			kStream.Read(szBuf, iNameLen);
			pLight->SetLightTexture( szBuf );
		}
	}
	
	m_pWorldMesh->LoadLightDecalTextureList( rkLoader );

	char szKey[MAX_PATH], szBuf[MAX_PATH];
	rkLoader.SetTitle_e( "omni_light" );
	if( rkLoader.LoadBool_e( "ini_light_enable", false ) )
	{
		int iLight = 0;

		SceneLightList::iterator iter;
		for( iter=m_SceneLightList.begin() ; iter!=m_SceneLightList.end() ; ++iter )
		{
			ioLight *pLight = iter->second;
			if( pLight->GetType() != LT_POINT )
				continue;

			wsprintf_e( szKey, "range%d", iLight + 1 );
			float fLightRange = rkLoader.LoadFloat( szKey, FLOAT1000 );
			pLight->SetAttenuationRange( fLightRange );

			D3DCOLORVALUE kLightColor;
			wsprintf_e( szKey, "red%d", iLight + 1 );
			kLightColor.r = rkLoader.LoadFloat( szKey, 0.0f ) / 255.0f;

			wsprintf_e( szKey, "green%d", iLight + 1 );
			kLightColor.g = rkLoader.LoadFloat( szKey, 0.0f ) / 255.0f;

			wsprintf_e( szKey, "blue%d", iLight + 1 );
			kLightColor.b = rkLoader.LoadFloat( szKey, 0.0f ) / 255.0f;
			kLightColor.a = FLOAT1;

			pLight->SetDiffuseColor( kLightColor );

			wsprintf_e( szKey, "decal_tex%d", iLight + 1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			pLight->SetDecalTexture( szBuf );

			if( Setting::GetLightQuality() == LQ_HIGH )
			{
				m_pWorldMesh->AddOmniLightDecal( GetMultiMapCurrentIndex(), pLight );
			}

			iLight++;
		}
	}
	else
	{
		if( Setting::GetLightQuality() == LQ_HIGH )
		{
			SceneLightList::iterator iter;
			for( iter=m_SceneLightList.begin() ; iter!=m_SceneLightList.end() ; ++iter )
			{
				ioLight *pLight = iter->second;
				if( pLight->GetType() == LT_POINT )
				{
					m_pWorldMesh->AddOmniLightDecal( GetMultiMapCurrentIndex(), pLight );
				}
			}
		}
	}
}

void ioGameSceneManager::LoadHeightMap( const char *szMapName, ioPlayStage *pStage )
{
	LoadLandScape( szMapName, pStage );
	LoadVertexColor( szMapName );

	CreateSkyDome( szMapName );
	SetMapFog( szMapName );

	LoadHeightMapMainLight( szMapName );
}

TextureFilterOption ConvertStringToFilter( const char *szFilter )
{
	if( !strcmp( szFilter, "ANISOTROPIC" ) )
		return TFO_ANISOTROPIC;
	else if( !strcmp( szFilter, "TRILINEAR" ) )
		return TFO_TRILINEAR;
	else if( !strcmp( szFilter, "BILINEAR" ) )
		return TFO_BILINEAR;

	return TFO_NEAREST;
}

void ioGameSceneManager::LoadLandScape( const char *szMapName, ioPlayStage *pPlayStage )
{
	SAFEDELETE( m_pLandScape );

	m_pLandScape = new ioLandScape( m_pRenderSystem->GetDevice() );	
	m_pLandScape->SetMapName( szMapName );
	m_pLandScape->SetDesireFPS( 60 );
	m_pLandScape->SetMaxError( 0.006f );

	char szMapPath[MAX_PATH];
	wsprintf( szMapPath, "Map/%s/%s.map", szMapName, szMapName );

	ioBinaryStream kStream;
	if( !g_ResourceLoader.LoadStream( szMapPath, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "LoadLandScape - %s Load Failed", szMapPath );
		return;
	}

	float fSizeX, fSizeY, fSizeZ;
	kStream.Read(&fSizeX, sizeof(float));
	kStream.Read(&fSizeY, sizeof(float));
	kStream.Read(&fSizeZ, sizeof(float));

	m_pLandScape->SetMapSize( fSizeX, fSizeY, fSizeZ );

	int iHMapWidth, iHMapHeight;
	kStream.Read(&iHMapWidth, sizeof(int));
	kStream.Read(&iHMapHeight, sizeof(int));

	float *pHeight = new float[ iHMapWidth*iHMapHeight ];
	kStream.Read(pHeight, sizeof(float)*iHMapWidth*iHMapHeight);

	m_pLandScape->SetHeightMap( pHeight, iHMapWidth, iHMapHeight );
	SAFEDELETEARRAY( pHeight );

	int iNumPatchW, iNumPatchH;
	iNumPatchW = (iHMapWidth-1) / (ioPatch::SOURCE_WIDTH-1);
	iNumPatchH = (iHMapHeight-1) / (ioPatch::SOURCE_HEIGHT-1);

	m_pLandScape->SetNumPatch( iNumPatchW, iNumPatchH );
	m_pLandScape->SetPatchSize( fSizeX / iNumPatchW, fSizeY, fSizeZ / iNumPatchH );

	m_pLandScape->Build();

	//------------ Vis Layer ------------------

	DWORD *pdwLayers = new DWORD[iNumPatchW*iNumPatchH];

	char szVisFileName[MAX_PATH];
	wsprintf( szVisFileName, "Map/%s/%s.vis", szMapName, szMapName );

	if( !g_ResourceLoader.LoadStream( szVisFileName, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "LoadLandScape 2- %s Load Failed", szVisFileName );
		return;
	}

	kStream.Read(pdwLayers, sizeof(DWORD)*iNumPatchW*iNumPatchH);

	m_pLandScape->SetVisLayers( pdwLayers );
	SAFEDELETEARRAY(pdwLayers);

	//------------ Not Lod ------------------

	char szNotLODFileName[MAX_PATH];
	wsprintf( szNotLODFileName, "Map/%s/%s.lod", szMapName, szMapName );
	if( g_ResourceLoader.LoadStream( szNotLODFileName, &kStream ) )
	{
		bool *pNotLOD = new bool[iNumPatchW*iNumPatchH];
		kStream.Read(pNotLOD, iNumPatchW*iNumPatchH);
		
		m_pLandScape->SetNotLODList( pNotLOD );
		SAFEDELETEARRAY(pNotLOD);
	}

	LoadModelList( szMapName, pPlayStage );
	LoadEffectList( szMapName, pPlayStage );
	LoadLightList( szMapName );
	LoadMapAmbient( szMapName );

	ioINILoader_e kConLoader( "config/sp2.ini" );

	char szResourcePath[MAX_PATH];
	kConLoader.LoadString_e( "resource", "path", "", szResourcePath, MAX_PATH );

	char szMapINI[MAX_PATH];
	wsprintf_e( szMapINI, "Map/%s/%s.ini", szMapName, szMapName );
	ioINILoader kLoader( szMapINI );

	int iTexLayerCnt = kLoader.LoadInt_e( "texture_info", "count", 0 );
	for( int i=0 ; i<iTexLayerCnt ; i++ )
	{
		char szTitle[MAX_PATH];
		wsprintf_e( szTitle, "texture%d", i+1 );
		kLoader.SetTitle( szTitle );

		char szTexName[MAX_PATH];
		kLoader.LoadString_e( "name", "Error", szTexName, MAX_PATH );

		ioMultiPassInfo info;
		info.pTexColor = g_TextureMgr.CreateTexture( szTexName );
		info.fRepeatX  = (float)kLoader.LoadInt_e( "repeat_x", 32 );
		info.fRepeatY  = (float)kLoader.LoadInt_e( "repeat_y", 32 );
		info.bSpecular = kLoader.LoadBool_e( "specular", false );

		if( i==0 )
			wsprintf( szTexName, "%s\\BaseLight.dds", szMapName );
		else
			wsprintf( szTexName, "%s\\AlphaMap0%d.dds", szMapName, i );

		char szLightMapPath[MAX_PATH];
		wsprintf( szLightMapPath, "%s/Texture/%s", szResourcePath, szTexName );
		if( g_ResourceLoader.IsFileExist( szLightMapPath ) )
		{
			info.pTexMod = g_TextureMgr.CreateTexture( szTexName );
		}
		else
		{
			if( i==0 )
				wsprintf( szLightMapPath, "%s\\BaseLight.tga", szMapName );
			else
				wsprintf( szLightMapPath, "%s\\AlphaMap0%d.tga", szMapName, i );

			if( g_ResourceLoader.IsFileExist( szLightMapPath ) )
			{
				info.pTexMod = g_TextureMgr.CreateTexture( szLightMapPath );
			}
		}

		m_pLandScape->AddMultiPassTex( info );
	}
	
	m_pLandScape->InitDiffuseRateByBaseLightMap();

	kLoader.SetTitle_e( "MapAlpha" );
	float fAlphaMin = kLoader.LoadFloat_e( "Start", 15000.0f );
	float fAlphaMax = kLoader.LoadFloat( "End", 25000.0f );

	m_pLandScape->SetAlphaMaxDist( fAlphaMax );
	m_pLandScape->SetAlphaMinDist( fAlphaMin );	

	ioCamera *pCamera = g_App.GetCamera();

	kLoader.SetTitle_e( "ObjAlpha" );
	fAlphaMin = kLoader.LoadFloat_e( "Start", 10000.0f );
	fAlphaMax = kLoader.LoadFloat( "End", 20000.0f );
	pCamera->SetDistAlphaFactor( fAlphaMin, fAlphaMax );
	
 	m_pLandScape->SetUseSpecular( true );

	char szFilter[MAX_PATH];
	float fMipMapBias = 0.0f;
	kConLoader.SetTitle_e( "map_filter" );

	kConLoader.LoadString_e( "alpha_tex", "", szFilter, MAX_PATH );
	SetTextureFilterOpt( 0, ConvertStringToFilter( szFilter ) );
	fMipMapBias = kConLoader.LoadFloat_e( "alpha_mipmap_bias", 0.0f );
	SetTextureMipMapBias( 0, fMipMapBias );

	kConLoader.LoadString_e( "base_tex", "", szFilter, MAX_PATH );
	SetTextureFilterOpt( 1, ConvertStringToFilter( szFilter ) );
	fMipMapBias = kConLoader.LoadFloat_e( "base_mipmap_bias", 0.0f );
	SetTextureMipMapBias( 1, fMipMapBias );

	kLoader.SetTitle_e( "LandAmbientMaterial" );
	float fRed, fGreen, fBlue;
	fRed   = kLoader.LoadFloat( "Red", 255.0f ) / 255.0f;
	fGreen = kLoader.LoadFloat_e( "Green", 255.0f ) / 255.0f;
	fBlue  = kLoader.LoadFloat_e( "Blue", 255.0f ) / 255.0f;
	m_pLandScape->SetLandAmbient( fRed, fGreen, fBlue );
	
	kLoader.SetTitle_e( "LandSpecularMaterial" );
	fRed   = kLoader.LoadFloat( "Red", 255.0f ) / 255.0f;
	fGreen = kLoader.LoadFloat_e( "Green", 255.0f ) / 255.0f;
	fBlue  = kLoader.LoadFloat_e( "Blue", 255.0f ) / 255.0f;
	m_pLandScape->SetLandSpecular( fRed, fGreen, fBlue );

	float fSpecularLv, fSpecularPower;
	fSpecularLv = kLoader.LoadFloat_e( "SpecularLv", FLOAT1 );
	fSpecularPower = kLoader.LoadFloat_e( "SpecularPower", 32.0f );

	m_pLandScape->SetSpecularLv( fSpecularLv );
	m_pLandScape->SetSpecularPower( fSpecularPower );
}

void ioGameSceneManager::LoadModelList( const char *szMapName, ioPlayStage *pPlayStage )
{
	char szFileName[MAX_PATH];
	wsprintf( szFileName, "Map/%s/%s.ent", szMapName, szMapName );
	
	ioBinaryStream kStream;
	if( !g_ResourceLoader.LoadStream( szFileName, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "LoadModelList - %s Load Failed", szFileName );
		return;
	}

	m_LoadListMap.clear();		// For LoadVertexColor...

	int iModelTypeCnt;
	kStream.Read(&iModelTypeCnt, sizeof(int));

	int iNameLen;
	char szModelFile[MAX_PATH];
	for( int i=0 ; i<iModelTypeCnt ; i++ )
	{
		kStream.Read(&iNameLen, sizeof(int));
		kStream.Read(szModelFile, iNameLen);
		szModelFile[iNameLen] = '\0';

		ioHashString kFileName( szModelFile );
		EntityGrpList *pList = new EntityGrpList;
		m_LoadListMap.insert( EntityGrpListMap::value_type( kFileName, pList ) );

		int iModelCnt;
		kStream.Read(&iModelCnt, sizeof(int));
		for( int j=0 ; j<iModelCnt ; j++ )
		{
			D3DXVECTOR3 vPos;
			kStream.Read(&vPos, sizeof(D3DXVECTOR3));

			D3DXQUATERNION qtRot;
			kStream.Read(&qtRot, sizeof(D3DXQUATERNION));

			D3DXVECTOR3 vScale;
			kStream.Read(&vScale, sizeof(D3DXVECTOR3));

			ioWorldDecoration *pDeco = pPlayStage->CreateWorldDecoration( kFileName );
			if( pDeco )
			{
				pDeco->SetWorldPosition( vPos );
				pDeco->SetWorldOrientation( qtRot );
				pDeco->SetWorldScale( vScale );

				pList->push_back( pDeco->GetGroup() );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "LoadModelList - %s Not Exist", kFileName.c_str() );
			}
		}
	}
}

void ioGameSceneManager::LoadEffectList( const char *szMapName, ioPlayStage *pPlayStage )
{
	char szFileName[MAX_PATH];
	wsprintf( szFileName, "Map/%s/%s.eff", szMapName, szMapName );

	ioBinaryStream kStream;
	if( !g_ResourceLoader.LoadStream( szFileName, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "LoadEffectList - %s Load Failed", szFileName );
		return;
	}

	int iEffectTypeCnt;
	kStream.Read(&iEffectTypeCnt, sizeof(int));

	int iNameLen, iEffectCnt;
	char szEffectFileName[MAX_PATH];
	D3DXVECTOR3 vPos;
	D3DXQUATERNION qtRot;

	for( int i=0 ; i<iEffectTypeCnt ; i++ )
	{
		kStream.Read(&iNameLen, sizeof(int));
		kStream.Read(szEffectFileName, iNameLen);
		szEffectFileName[ iNameLen ] = '\0';

		ioHashString kFileName( szEffectFileName );
		kStream.Read(&iEffectCnt, sizeof(int));
		for( int j=0 ; j<iEffectCnt ; j++ )
		{
			kStream.Read(&vPos, sizeof(D3DXVECTOR3));
			kStream.Read(&qtRot, sizeof(D3DXQUATERNION));

			ioMapEffect *pEffect = pPlayStage->CreateMapEffect( kFileName, vPos, ioMath::UNIT_ALL );
			if( pEffect )
			{
				pEffect->SetWorldOrientation( qtRot );
			}
		}
	}
}

void ioGameSceneManager::LoadLightList( const char *szMapName )
{
	char szFileName[MAX_PATH];
	wsprintf( szFileName, "Map/%s/%s.rlt", szMapName, szMapName );

	ioBinaryStream kStream;
	if( !g_ResourceLoader.LoadStream( szFileName, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "LoadLightList - %s Load Failed", szFileName );
		return;
	}

	int iLightCnt = 0;
	kStream.Read(&iLightCnt, sizeof(int));

	int  iNameLen;
	char szLightName[MAX_PATH];
	D3DLIGHT9 kLight;
	D3DXVECTOR3 vTargetPos;
	DWORD dwLightMask;
	ioLight *pLight = NULL;

	for( int i=0 ; i<iLightCnt ; i++ )
	{
		kStream.Read(&iNameLen, sizeof(int));

		memset( szLightName, 0, MAX_PATH );
		kStream.Read(szLightName, iNameLen);

		kStream.Read(&kLight, sizeof(D3DLIGHT9));

		pLight = CreateLight( szLightName );
		pLight->SetD3DLight( kLight );

		kStream.Read(&vTargetPos, sizeof(D3DXVECTOR3));
		pLight->SetTargetPos( vTargetPos );

		kStream.Read(&dwLightMask, sizeof(DWORD));
		pLight->SetAttribute( dwLightMask );
	}
}

void ioGameSceneManager::LoadMapAmbient( const char *szMapName )
{
	char szMapINI[MAX_PATH];
	wsprintf_e( szMapINI, "Map/%s/%s.ini", szMapName, szMapName );
	ioINILoader kLoader( szMapINI );

	D3DCOLORVALUE kAmbient;

	kLoader.SetTitle_e( "Ambient" );
	int iRed	= kLoader.LoadInt( "Red", 127 );
	int iGreen	= kLoader.LoadInt_e( "Green", 127 );
	int iBlue   = kLoader.LoadInt_e( "Blue", 127 );

	kAmbient.r = (float)iRed/255.0f;
	kAmbient.g = (float)iGreen/255.0f;
	kAmbient.b = (float)iBlue/255.0f;

	m_pRenderSystem->SetAmbientLight( kAmbient.r, kAmbient.g, kAmbient.b );
}

void ioGameSceneManager::LoadVertexColor( const char *szMapName )
{
	char szFileName[MAX_PATH];
	wsprintf( szFileName, "Map/%s/%s.vxg", szMapName, szMapName );

	ioBinaryStream kStream;
	if( !g_ResourceLoader.LoadStream( szFileName, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "LoadVertexColor - %s Load Failed", szFileName );
		return;
	}

	int iModelTypeCnt;
	kStream.Read(&iModelTypeCnt, sizeof(int));

	int iNameLen;
	char szModelFileName[MAX_PATH];
	for( int i=0 ; i<iModelTypeCnt ; i++ )
	{
		memset( szModelFileName, 0, MAX_PATH );

		kStream.Read(&iNameLen, sizeof(int));
		kStream.Read(szModelFileName, iNameLen);

		EntityGrpListMap::iterator iterList;
		iterList = m_LoadListMap.find( ioHashString( szModelFileName ) );
		if( iterList == m_LoadListMap.end() )
		{
			LOG.PrintTimeAndLog( 0, "LoadVertexColor - %s Not Exist", szModelFileName );
			return;
		}

		EntityGrpList *pList = iterList->second;

		EntityGrpList::iterator iterModel;
		for( iterModel = pList->begin() ; iterModel!=pList->end() ; iterModel++ )
		{
			LoadEntityVertexColor( kStream, *iterModel );
		}

		SAFEDELETE( pList );
	}

	m_LoadListMap.clear();
}

void ioGameSceneManager::LoadEntityVertexColor( ioBinaryStream &rkStream, ioEntityGroup *pGrp  )
{
	int iSaveMeshCnt;
	rkStream.Read(&iSaveMeshCnt, sizeof(int));
	if( iSaveMeshCnt == 0 )	return;

	int iVtxCnt = 0;
	for( int i=0 ; i<iSaveMeshCnt ; i++ )
	{
		rkStream.Read(&iVtxCnt, sizeof(int));
		if( iVtxCnt == 0 )	continue;

		ioEntity *pEntity = pGrp->GetEntity( i );
		pEntity->ConvertToVertexColor();

		ioVertexBufferInstance *pColorBuf = pEntity->GetExtraBuffer();
		if( pColorBuf->Lock() )
		{
			rkStream.Read(pColorBuf->GetBuffer(), sizeof(DWORD)*iVtxCnt);
			pColorBuf->UnLock();
		}
		else
		{
			rkStream.SetCurPos( rkStream.GetCurPos() + sizeof(DWORD)*iVtxCnt );
		}
	}
}

void ioGameSceneManager::PreLoadAmbientDataSet()
{
	if( m_AmbientDataList.empty() ) return;

	int iIndex = min( GetMultiMapCurrentIndex(), max( 0, (int)m_AmbientDataList.size() - 1 ) );

	AmbientData &rkAmbient = m_AmbientDataList[iIndex];

	SetSceneAmbient( rkAmbient.m_fRed, rkAmbient.m_fGreen, rkAmbient.m_fBlue );
}

void ioGameSceneManager::PreLoadMainLightDataSet()
{
	if( m_MainLIghtDataList.empty() ) return;

	int iIndex = min( GetMultiMapCurrentIndex(), max( 0, (int)m_MainLIghtDataList.size() - 1 ) );

	MainLIghtData &rkMainLight = m_MainLIghtDataList[iIndex];

	SetMainLight( rkMainLight.m_vLightDir, rkMainLight.m_Diffuse, rkMainLight.m_Diffuse, rkMainLight.m_szToonTexture );
	
	ioLight *pMainLight = GetLight( "SecondMainLight" );
	if( !pMainLight )
	{
		pMainLight = CreateLight( "SecondMainLight" );
		pMainLight->SetAttribute( ioLight::LAM_REAL_TIME );
		pMainLight->SetType( LT_DIRECTIONAL );
	}

	pMainLight->SetDiffuseColor( rkMainLight.m_Diffuse.r, rkMainLight.m_Diffuse.g, rkMainLight.m_Diffuse.b );
	pMainLight->SetSpecularColor( rkMainLight.m_Diffuse.r, rkMainLight.m_Diffuse.g, rkMainLight.m_Diffuse.b );
	pMainLight->SetLightTexture( rkMainLight.m_szSecondToonTexture );
	pMainLight->SetDirection( rkMainLight.m_vLightDir );
	pMainLight->SetLightApplyMask( APPLY_SECOND_MAIN_LIGHT );
}

void ioGameSceneManager::PreLoadShadowMapDataSet()
{
	if( m_ShadowMapDataList.empty() ) return;

	int iIndex = min( GetMultiMapCurrentIndex(), max( 0, (int)m_ShadowMapDataList.size() - 1 ) );
    
	ShadowMapData &rkShadow = m_ShadowMapDataList[iIndex];

	SetShadowMapSize( rkShadow.m_iTextureSize );
	SetShadowColor( FLOAT1, rkShadow.m_fRed, rkShadow.m_fGreen,rkShadow.m_fBlue );
	SetShadowLightDir( rkShadow.m_vShadowDir );
	SetShadowMapEnable( Setting::UseShadow() );
}

D3DXVECTOR3 ioGameSceneManager::CheckMapCameraLimit( D3DXVECTOR3 vNewLookAt ) const
{
	if( m_CameraMapLimitList.empty() ) return vNewLookAt;

	int iIndex = min( GetMultiMapCurrentIndex(), max( 0, (int)m_CameraMapLimitList.size() - 1 ) );

	const CameraMapLimit &rkCamera = m_CameraMapLimitList[iIndex];
	vNewLookAt.x = max( rkCamera.m_vMinLookAt.x, min( vNewLookAt.x, rkCamera.m_vMaxLookAt.x ) );
	vNewLookAt.y = max( rkCamera.m_vMinLookAt.y, min( vNewLookAt.y, rkCamera.m_vMaxLookAt.y ) );
	vNewLookAt.z = max( rkCamera.m_vMinLookAt.z, min( vNewLookAt.z, rkCamera.m_vMaxLookAt.z ) );
	return vNewLookAt;
}

void ioGameSceneManager::CheckFlyMapLimit( D3DXVECTOR3 *pFlyPos )
{
	if( pFlyPos == NULL ) return;
	if( m_BoostLimitList.empty() ) return;

	int iIndex = min( GetMultiMapCurrentIndex(), max( 0, (int)m_BoostLimitList.size() - 1 ) );
	
	BoostLimit &rkBoost = m_BoostLimitList[iIndex];
	pFlyPos->x = max( rkBoost.m_vMin.x, min( pFlyPos->x, rkBoost.m_vMax.x ) );
	pFlyPos->y = max( rkBoost.m_vMin.y, min( pFlyPos->y, rkBoost.m_vMax.y ) );
	pFlyPos->z = max( rkBoost.m_vMin.z, min( pFlyPos->z, rkBoost.m_vMax.z ) );
}

bool ioGameSceneManager::CheckAxisLimit( D3DXVECTOR3 *pNewPos )
{
	if( pNewPos == NULL ) return false;
	if( m_AxisLimitList.empty() ) return false;

	int iIndex = min( GetMultiMapCurrentIndex(), max( 0, (int)m_AxisLimitList.size() - 1 ) );
	
	bool bCheck = false;
	AxisLimit &rkAxis = m_AxisLimitList[iIndex];

	if( rkAxis.m_vMin.x != rkAxis.m_vMax.x )
	{
		if( pNewPos->x <= rkAxis.m_vMin.x || pNewPos->x >= rkAxis.m_vMax.x )
			bCheck = true;
		pNewPos->x = max( rkAxis.m_vMin.x, min( pNewPos->x, rkAxis.m_vMax.x ) );
	}

	if( rkAxis.m_vMin.y != rkAxis.m_vMax.y )
	{
		if( pNewPos->y <= rkAxis.m_vMin.y || pNewPos->y >= rkAxis.m_vMax.y )
			bCheck = true;
		pNewPos->y = max( rkAxis.m_vMin.y, min( pNewPos->y, rkAxis.m_vMax.y ) );
	}

	if( rkAxis.m_vMin.z != rkAxis.m_vMax.z )
	{
		if( pNewPos->z <= rkAxis.m_vMin.z || pNewPos->z >= rkAxis.m_vMax.z )
			bCheck = true;
		pNewPos->z = max( rkAxis.m_vMin.z, min( pNewPos->z, rkAxis.m_vMax.z ) );
	}

	return bCheck;
}

bool ioGameSceneManager::CheckAxisLimit( const D3DXVECTOR3& vNewPos )
{
	if( m_AxisLimitList.empty() )
		return false;

	int iIndex = min( GetMultiMapCurrentIndex(), max( 0, (int)m_AxisLimitList.size() - 1 ) );
	
	AxisLimit &rkAxis = m_AxisLimitList[iIndex];

	bool bCheck = false;
	if( rkAxis.m_vMin.x != rkAxis.m_vMax.x )
	{
		if( vNewPos.x <= rkAxis.m_vMin.x || vNewPos.x >= rkAxis.m_vMax.x )
			bCheck = true;
	}

	if( !bCheck && rkAxis.m_vMin.y != rkAxis.m_vMax.y )
	{
		if( vNewPos.y <= rkAxis.m_vMin.y || vNewPos.y >= rkAxis.m_vMax.y )
			bCheck = true;
	}

	if( !bCheck && rkAxis.m_vMin.z != rkAxis.m_vMax.z )
	{
		if( vNewPos.z <= rkAxis.m_vMin.z || vNewPos.z >= rkAxis.m_vMax.z )
			bCheck = true;
	}
	
	return bCheck;
}

void ioGameSceneManager::CreateSkyDome( const char *szMapName )
{
	SAFEDELETE( m_pSkyDome );

	m_pSkyDome = new ioSkyDome( "SkyDome" );

	char szTexName[MAX_PATH];
	wsprintf( szTexName, "%s\\sky.dds", szMapName );

	if( !g_ResourceLoader.IsFileExist( szTexName ) )
	{
		wsprintf( szTexName, "%s\\sky.jpg", szMapName );
	}

	m_pSkyDome->SetSkyTexture( szTexName );

	char szMapINI[MAX_PATH];
	wsprintf_e( szMapINI, "Map/%s/%s.ini", szMapName, szMapName );
	ioINILoader kLoader( szMapINI );
	kLoader.SetTitle( "Sky" );

	float fRadius, fScale;
	int iSegment;

	fRadius  = kLoader.LoadFloat_e( "Radius", 10000.0f );
	fScale   = kLoader.LoadFloat_e( "HeightScale", FLOAT1 );
	iSegment = kLoader.LoadInt_e( "Segment", 16 );

	bool bFogEnable;
	bFogEnable = kLoader.LoadBool_e( "FogEnable", false );
	m_bShowSky = kLoader.LoadBool_e( "ShowSky", true );

	m_pSkyDome->Create( fRadius, iSegment, fScale );
	m_pSkyDome->SetFogEnable( bFogEnable );
}

void ioGameSceneManager::SetMapFog( const char *szMapName )
{
	char szMapINI[MAX_PATH];
	wsprintf_e( szMapINI, "Map/%s/%s.ini", szMapName, szMapName );

	ioINILoader kLoader( szMapINI );
	kLoader.SetTitle( "Fog" );

	bool bFogEnable = true;

	char szFogType[MAX_PATH];
	kLoader.LoadString_e( "Type", "LINEAR", szFogType, MAX_PATH );
	if( !strcmp(szFogType, "NONE" ) )
	{
		bFogEnable = false;
	}

	int iRed, iGreen, iBlue;
	iRed   = kLoader.LoadInt_e( "FogRed", 255 );
	iGreen = kLoader.LoadInt_e( "FogGreen", 255 );
	iBlue  = kLoader.LoadInt_e( "FogBlue", 255 );

	DWORD dwColor = D3DCOLOR_ARGB( 255, iRed, iGreen, iBlue );

	float fStart, fEnd;
	fStart = kLoader.LoadFloat_e( "Start", FLOAT500 );
	fEnd   = kLoader.LoadFloat( "End", 10000.0f );

	m_pRenderSystem->SetFogFactor( dwColor, fStart, fEnd );
	SetFogEnable( bFogEnable );
}

void ioGameSceneManager::LoadHeightMapMainLight( const char *szMapName )
{
	char szMapINI[MAX_PATH];
	wsprintf_e( szMapINI, "Map/%s/%s.ini", szMapName, szMapName );

	ioINILoader kLoader( szMapINI );

	kLoader.SetTitle_e( "Light" );
	int iRed	= kLoader.LoadInt( "Red", 127 );
	int iGreen  = kLoader.LoadInt_e( "Green", 127 );
	int iBlue   = kLoader.LoadInt_e( "Blue", 127 );

	D3DCOLORVALUE kDiffuse;
	kDiffuse.r = (float)iRed / 255.0f;
	kDiffuse.g = (float)iGreen / 255.0f;
	kDiffuse.b = (float)iBlue / 255.0f;

	D3DXVECTOR3 vDir;
	vDir.x = kLoader.LoadFloat_e( "X", 0.0f );
	vDir.y = kLoader.LoadFloat_e( "Y", -FLOAT1 );
	vDir.z = kLoader.LoadFloat_e( "Z", -FLOAT1 );
	D3DXVec3Normalize( &vDir, &vDir );

	SetMainLight( vDir, kDiffuse, kDiffuse, "" );
}

void ioGameSceneManager::SetShadowMapEnable( bool bEnable )
{
	ioSceneManager::SetShadowMapEnable( bEnable );

	if( m_pLandScape )
	{
		switch( m_ShadowMapType )
		{
		case SMT_NOT_ENABLE:
			m_pLandScape->SetShadowMapType( SMT_NOT_ENABLE );
			break;
		case SMT_DEPTH_BUF:
			m_pLandScape->SetShadowMapType( SMT_DEPTH_BUF );
			break;
		case SMT_R32F_TEX:
			m_pLandScape->SetShadowMapType( SMT_R32F_TEX );
			break;
		}
	}
	
	if( m_pWorldMesh )
	{
		if( m_ShadowMapType == SMT_NOT_ENABLE )
			m_pWorldMesh->ShadowMapEnable( false );
		else
			m_pWorldMesh->ShadowMapEnable( true );
	}
}

void ioGameSceneManager::ClearScene()
{
	SAFEDELETE( m_pSkyDome );
	SAFEDELETE( m_pLandScape );
	SAFEDELETE( m_pWorldMesh );

	SAFEDELETE( m_pScreenBlinder );
	m_bUseScreenBlinder = false;

	ioSceneManager::ClearScene();
}

void ioGameSceneManager::FindShadowCasterObjects( ioCamera *pCamera )
{
	ioSceneShadowBox kSceneBox( pCamera, m_vShadowLightDir );

	kSceneBox.SetExtendDiagonalRate( 1.5f );

	EntityGrpIterator iter = GetEntityGrpIterator();
	while( iter.HasMoreElements() )
	{
		ioEntityGroup *pGrp = iter.Next();
		if( pGrp->IsVisible() && pGrp->IsShadowCastEnable() )
		{
			if( kSceneBox.AddShadowCaster( pGrp, 1.5f ) )
			{
				pGrp->UpdateGenShadowMapToQueue( m_RenderQueue );
			}
		}
	}

	if( kSceneBox.IsShadowBoxEmpty() )
	{
		D3DXVECTOR3 vCenter = pCamera->GetPosition() + pCamera->GetDirection() * FLOAT1000;

		ioAxisAlignBox kDummyBox;
		kDummyBox.SetMinMaxPos( vCenter - D3DXVECTOR3( FLOAT100, FLOAT100, FLOAT100 ),
								vCenter + D3DXVECTOR3( FLOAT100, FLOAT100, FLOAT100 ) );

		kSceneBox.AddShadowReceiveBox( kDummyBox, 1.5f );
	}

	kSceneBox.CreateShadowFrustum( m_vShadowAreaList );
}

void ioGameSceneManager::DestroyLight( const ioHashString &kName )
{
	if( m_pWorldMesh )
	{
		m_pWorldMesh->DestroyLightDecal( GetMultiMapCurrentIndex(), kName );
	}

	ioSceneManager::DestroyLight( kName );
}

void ioGameSceneManager::DestroyAllLights()
{
	if( m_pWorldMesh )
	{
		for(int i = 0;i < GetMaxMultiMapCount();i++)
			m_pWorldMesh->DestroyAllLightDecal( i );
	}

	ioSceneManager::DestroyAllLights();
}

void ioGameSceneManager::SetSoftwareVertexBlending( bool bSoftware )
{
	ioSceneManager::SetSoftwareVertexBlending( bSoftware );
	ioUI3DRender::SetSoftSkinning( bSoftware );
}

void ioGameSceneManager::ModifyEntityGroupMaterial( ioEntityGrpMaterialModifier *pModifier )
{
	ioSceneManager::ModifyEntityGroupMaterial( pModifier );
	ioUI3DRender::ModifyEntityGroupMaterial( pModifier );
}

ioOpcodeShape* ioGameSceneManager::GetCollisionShape() const
{
	if( m_pWorldMesh )
		return m_pWorldMesh->GetCollisionShape();

	return NULL;
}

D3DCOLORVALUE ioGameSceneManager::GetDiffuseColorRate( const D3DXVECTOR3 &vPos )
{
	if( m_pLandScape )
	{
		//GRAY=(R*30 + G*59 + B*11)/100 
		D3DCOLORVALUE kDiffuseColor = m_pLandScape->GetLightMapColor( vPos.x, vPos.z );
		float fBrightNess = kDiffuseColor.r * 0.3f + kDiffuseColor.g * 0.59f + kDiffuseColor.b * 0.11f;
		if( fBrightNess > 0.2f )
		{
			kDiffuseColor.r = kDiffuseColor.g = kDiffuseColor.b = FLOAT1;
		}

		return kDiffuseColor;
	}

	return m_DefaultDiffuseRate;
}

ioAxisAlignBox ioGameSceneManager::GetWorldMapBox() const
{
	if( m_pWorldMesh )
		return m_pWorldMesh->GetWorldAABB();

	if( m_pLandScape )
	{
		D3DXVECTOR3 vMin( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 vMax;
		vMax.x = m_pLandScape->GetMapSizeX();
		vMax.y = m_pLandScape->GetMapSizeY();
		vMax.z = m_pLandScape->GetMapSizeZ();

		return ioAxisAlignBox( vMin, vMax );
	}

	return ioAxisAlignBox();
}

float ioGameSceneManager::GetHeight( float fXPos, float fZPos ) const
{
	if( m_pWorldMesh )
		return m_pWorldMesh->GetHeight( fXPos, fZPos );

	if( m_pLandScape )
		return m_pLandScape->GetHeight( fXPos, fZPos );

	return 0.0f;
}

float ioGameSceneManager::GetHeight( float fXPos, float fYPos, float fZPos ) const
{
	if( m_pWorldMesh )
		return m_pWorldMesh->GetHeight( D3DXVECTOR3( fXPos, fYPos, fZPos ) );

	if( m_pLandScape )
		return m_pLandScape->GetHeight( fXPos, fZPos );

	return 0.0f;
}

float ioGameSceneManager::GetBoundHeight()
{
	if( m_pWorldMesh )
		return m_pWorldMesh->GetWorldAABB().GetMaxPos().y;
	
	if( m_pLandScape )
		return m_pLandScape->GetMapSizeY();

	return 0.0f;
}

void ioGameSceneManager::RenderShadowMap( int iX, int iY, int iWidth, int iHeight )
{
	if( !m_pShadowMapRT || m_ShadowMapType == SMT_NOT_ENABLE )
		return;

	ioVtxRHWTex kVtx[4];
	FillVtxRHWTexRectStrip( kVtx, iX, iY, iWidth, iHeight );

	m_pRenderSystem->SetFixedFVF( D3DFVF_XYZRHW | D3DFVF_TEX1 );
	m_pRenderSystem->DisableShader();
	m_pRenderSystem->SetLightEnable( false );
	m_pRenderSystem->SetDepthFunction( D3DCMP_LESSEQUAL, false, false );
	m_pRenderSystem->SetCullingMode( CM_CCW );
	m_pRenderSystem->SetAlphaTest( D3DCMP_GREATER, 0 );
	m_pRenderSystem->SetTextureFiltering( 0, TFO_BILINEAR );
	m_pRenderSystem->SetTextureAddressMode( 0, TAM_CLAMP );
	m_pRenderSystem->SetTextureTransformFlag( 0, D3DTTFF_DISABLE );

	m_pRenderSystem->SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_ONE, D3DBLEND_ZERO );
	m_pRenderSystem->SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	m_pRenderSystem->SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	m_pRenderSystem->SetTexture( 0, m_pShadowMapRT->GetTexture() );

	m_pRenderSystem->DisableTextureUnitsFrom( 1 );
	m_pRenderSystem->ClearStreamDesc();

	m_pRenderSystem->GetDevice()->DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, kVtx, sizeof(ioVtxRHWTex) );

	m_pRenderSystem->DisableTextureUnitsFrom( 0 );
}

void ioGameSceneManager::SetMultiMapCurrentIndex( int iMapIndex )
{
	if( !m_bMultiMap ) return;
	if( !m_pWorldMesh ) return;
	if( !COMPARE( iMapIndex, 0, GetMaxMultiMapCount() ) )
	{
		LOG.PrintTimeAndLog( 0, "MultiMap Count Error : %d - %d", iMapIndex, GetMaxMultiMapCount() );
		return;
	}

	m_iMultiMapCurrentIndex = iMapIndex;
	m_pWorldMesh->SetMultiMapCurrentIndex( m_iMultiMapCurrentIndex );

	PreLoadAmbientDataSet();
	PreLoadShadowMapDataSet();
}