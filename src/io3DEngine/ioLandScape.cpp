

#include "stdafx.h"

#include "ioRay.h"
#include "ioLight.h"

#include "ioAxisAlignBox.h"
#include "ioMovableObject.h"

#include "ioCamera.h"

#include "ioTexture.h"
#include "ioShader.h"

#include "ioRenderSystem.h"
#include "ioAutoShaderParamSource.h"

#include "ioPatch.h"
#include "ioPatchDefault.h"
#include "ioPatchMorphSW.h"
#include "ioLandScape.h"

#include "ioShaderManager.h"
#include "ioTextureManager.h"

#include "ioFrameTimer.h"

#include "ioVertexBufferInstance.h"
#include "ioVertexBufferManager.h"
#include "ioIndexBufferInstance.h"
#include "ioIndexBufferManager.h"

#include "ioMath.h"

#include "ioStream.h"
#include "ioResourceLoader.h"
#include "ioCPU.h"
#include "ErrorReport.h"

ioLandScape::ioLandScape( IDirect3DDevice9 *pDevice )
{
	m_pD3DDevice = pDevice;
	m_pD3DDevice->AddRef();

	m_fSizeX = m_fSizeY = m_fSizeZ = 0.0f;
	m_fPatchDX = m_fPatchDY = m_fPatchDZ = 0.0f;
	m_iMinX = m_iMinY = m_iMaxX = m_iMaxY = 0;

	m_iNumPatchX = m_iNumPatchY = 0;

	m_fMaxError		= 0.008f;
	m_iDesiredFPS	= 60;

	m_pPatch	= NULL;
	m_pHeight	= NULL;
	m_pNormal	= NULL;

	m_iHMapWidth = m_iHMapHeight = 0;

	m_pVtxDecl   = NULL;
	m_pVtxInstance = NULL;
	m_pNrmInstance = NULL;
	m_iNumBufVertex = m_iNewNumVertex = m_iCurStartVtx = 0;

	m_pIdxInstance = NULL;
	m_iNumBufIndex = m_iNewNumIndex = m_iCurStartIdx = 0;

	m_pActivePatch = NULL;

	m_LandAmbient.r = m_LandAmbient.g = m_LandAmbient.b = m_LandAmbient.a = 1.0f;
	m_LandSpecular.r = m_LandSpecular.g = m_LandSpecular.b = m_LandSpecular.a = 1.0f;
	m_fSpecularLv = 1.0f;
	m_fSpecularPower = 32.0f;

	m_RenderMode = RM_MORPH_SW;

	LoadShader();

	m_ShadowMapType = SMT_NOT_ENABLE;

	m_fAlphaMinDist = 12000.0f;
	m_fAlphaMaxDist = 15000.0f;
	
	m_pLightMapColor  = NULL;
	m_iDiffuseRateMapWidth  = 0;
	m_iDiffuseRateMapHeight = 0;

	m_bUseSpecular = false;
}

ioLandScape::~ioLandScape()
{
	for( int y=0 ; y<m_iNumPatchY ; y++ )
	{
		for( int x=0 ; x<m_iNumPatchX ; x++ )
		{
			m_pPatch[y][x]->DeInit();
			SAFEDELETE( m_pPatch[y][x] );
		}

		SAFEDELETEARRAY( m_pPatch[y] );
	}

	SAFEDELETEARRAY( m_pPatch );
	SAFEDELETEARRAY( m_pHeight );
	SAFEDELETEARRAY( m_pNormal );

	SAFEDELETE( m_pVtxDecl );

	if( m_pVtxInstance )
	{
		g_VtxBufMgr.DestroyInstance( m_pVtxInstance );
		m_pVtxInstance = NULL;
	}

	if( m_pNrmInstance )
	{
		g_VtxBufMgr.DestroyInstance( m_pNrmInstance );
		m_pNrmInstance = NULL;
	}

	if( m_pIdxInstance )
	{
		g_IdxBufMgr.DestroyInstance( m_pIdxInstance );
		m_pIdxInstance = NULL;
	}

	vMultiPass::iterator iter;
	for( iter = m_vMultiPass.begin() ; iter!=m_vMultiPass.end() ; ++iter )
	{
		DESTROY_TEXTURE( iter->pTexColor );
		DESTROY_TEXTURE( iter->pTexMod );
	}
	m_vMultiPass.clear();

	SAFEDELETEARRAY( m_pLightMapColor );

	UnLoadShader();

	SAFERELEASE( m_pD3DDevice );
}

void ioLandScape::LoadShader()
{
	m_pSWShader[0]		 = g_ShaderMgr.CreateShader( "PatchSW.fxo" );
	m_pSWShadowMapD16[0] = g_ShaderMgr.CreateShader( "RenderPatchShadowMapD16.fxo" );

	char szShaderName[MAX_PATH];
	for( int i=1 ; i<MAX_PATCH_SHADER ; i++ )
	{
		wsprintf( szShaderName, "PatchSWPoint%d.fxo", i );
		m_pSWShader[i] = g_ShaderMgr.CreateShader( szShaderName );

		wsprintf( szShaderName, "RenderPatchPoint%dShadowMapD16.fxo", i );
		m_pSWShadowMapD16[i] = g_ShaderMgr.CreateShader( szShaderName );
	}
}

void ioLandScape::UnLoadShader()
{
	for( int i=0 ; i<MAX_PATCH_SHADER ; i++ )
	{
		if( m_pSWShader[i] )
		{
			g_ShaderMgr.Destroy( m_pSWShader[i] );
			m_pSWShader[i] = NULL;
		}
		if( m_pSWShadowMapD16[i] )
		{
			g_ShaderMgr.Destroy( m_pSWShadowMapD16[i] );
			m_pSWShadowMapD16[i] = NULL;
		}
	}
}

void ioLandScape::SetHeightMap( BYTE *pHeight, int iWidth, int iHeight )
{
	SAFEDELETEARRAY( m_pHeight );

	int iTotalVtx = iWidth * iHeight;
	m_pHeight = new float[ iTotalVtx ];

	for( int i=0 ; i<iTotalVtx ; i++ )
	{
		m_pHeight[i] = (float)pHeight[i]/255.0f * m_fSizeY;
	}

	SetHMapSize( iWidth, iHeight );
}

void ioLandScape::SetHeightMap( float *pHeight, int iWidth, int iHeight )
{
	SAFEDELETEARRAY( m_pHeight );

	m_pHeight = new float[ iWidth * iHeight ];

	ioCPU::FastMemcpy( m_pHeight, pHeight, sizeof(float)* iWidth * iHeight );

	SetHMapSize( iWidth, iHeight );
}

void ioLandScape::AddMultiPassTex( const ioMultiPassInfo &info )
{
	m_vMultiPass.push_back( info );
}

void ioLandScape::SetMapSize( float fSizeX, float fSizeY, float fSizeZ )
{
	m_fSizeX = fSizeX;
	m_fSizeY = fSizeY;
	m_fSizeZ = fSizeZ;
}

void ioLandScape::SetHMapSize( int iHMapX, int iHMapY )
{
	m_iHMapWidth = iHMapX;
	m_iHMapHeight = iHMapY;
}

void ioLandScape::SetNumPatch( int iPatchX, int iPatchY )
{
	m_iNumPatchX = iPatchX;
	m_iNumPatchY = iPatchY;
}

void ioLandScape::SetPatchSize( float fPatchDX, float fPatchDY, float fPatchDZ )
{
	m_fPatchDX = fPatchDX;
	m_fPatchDY = fPatchDY;
	m_fPatchDZ = fPatchDZ;

	m_fCellDX = fPatchDX / (ioPatch::SOURCE_WIDTH - 1);
	m_fCellDZ = fPatchDZ / (ioPatch::SOURCE_HEIGHT - 1);
}

bool ioLandScape::MoveAboveSurface( D3DXVECTOR3 &vPos, float fHeight ) const
{
	float fLandHeight = GetHeight( vPos.x, vPos.z );

	if( vPos.y < fLandHeight + fHeight )
	{
		vPos.y = fLandHeight + fHeight;
		return true;
	}

	return false;
}

float ioLandScape::GetLineMaxHeight( const D3DXVECTOR3 &vStart,
									 const D3DXVECTOR3 &vEnd ) const
{
	D3DXVECTOR3 vDir = vEnd - vStart;
	vDir.y = 0;

	float fLength = D3DXVec3Length( &vDir );
	D3DXVec3Normalize( &vDir, &vDir );

	float fMaxHeight = GetHeight( vStart.x, vStart.z );

	float fAddDist = m_fCellDX;
	float fCurHeight = 0.0f;
	D3DXVECTOR3 vCurCheckPos;
	while( true )
	{
		if( fAddDist > fLength )
			break;

		vCurCheckPos = vStart + vDir * fAddDist;
		fCurHeight = GetHeight( vCurCheckPos.x, vCurCheckPos.z );

		if( fMaxHeight < fCurHeight )
			fMaxHeight = fCurHeight;

		fAddDist += fLength;
	}

	return max( fMaxHeight, GetHeight( vEnd.x, vEnd.z ) );
}

ioPatch* ioLandScape::GetPickPos( const ioRay &rkRay, D3DXVECTOR3 &vPickPos )
{
	float fPickPatchT = ioMath::FLOAT_INFINITY;

	ioPatch *pPickPatch = NULL;
	ioPatch *pCurPatch  = GetActiveListHead();
	for( ; pCurPatch != NULL ; pCurPatch = pCurPatch->GetNextActive() )
	{
		if( !ioMath::TestIntersection( rkRay, pCurPatch->GetWorldSphere() ) )
			continue;
		if( !ioMath::TestIntersection( rkRay, pCurPatch->GetWorldAABB() ) )
			continue;

		const D3DXVECTOR3 *pVtx = pCurPatch->GetVertexP();
		const WORD *pIdx = pCurPatch->GetIndexP();

		int iNumFace = pCurPatch->GetNumIndex() - 2;

		WORD idx0, idx1, idx2;
		for( int i=0 ; i<iNumFace ; i++ )
		{
			idx0 = pIdx[ i+0 ];
			idx1 = pIdx[ i+1 ];
			idx2 = pIdx[ i+2 ];

			if( idx0 == idx1 || idx1==idx2 || idx2 == idx0 )
				continue;	// degenerate triangle

			float t=0.0f;
			D3DXVECTOR3 vPos;
			if( ioMath::IntersectionTri( rkRay,
									 	 pVtx[idx0], pVtx[idx1], pVtx[idx2],
										 &t, &vPos ) )
			{
				if( fPickPatchT > t )
				{
					fPickPatchT = t;
					pPickPatch = pCurPatch;
					vPickPos = vPos;
				}
			}
		}	// iNumFace loop
	}	// pCurPatch loop

	return pPickPatch;
}

ioPatch* ioLandScape::CreatePatch( int nX, int nY )
{
	ioPatch *pPatch = NULL;

	switch( m_RenderMode )
	{
	case RM_DEFAULT:
		pPatch = new ioPatchDefault( this, nX, nY );
		break;
	case RM_MORPH_SW:
		pPatch = new ioPatchMorphSW( this, nX, nY );
		break;
	}	

	return pPatch;
}

bool ioLandScape::Build()
{
	int x, y;

	m_pPatch = new ioPatch**[ m_iNumPatchY ];
	for( y=0; y< m_iNumPatchY ; y++ )
	{
		m_pPatch[ y ] = new ioPatch*[ m_iNumPatchX ];
	}

	ioPatch *pPatch = NULL;
	int iXHeightPos, iYHeightPos;
	float fXPos, fZPos;

	fZPos = 0.0f;
	iYHeightPos = 0;
	for( y=0 ; y<m_iNumPatchY ; y++ )
	{
		fXPos = 0.0f;
		iXHeightPos = 0;
		for( x=0 ; x<m_iNumPatchX ; x++ )
		{
			pPatch = CreatePatch( x, y );
			m_pPatch[ y ][ x ] = pPatch;

			pPatch->SetScale( D3DXVECTOR3( m_fPatchDX, m_fPatchDY, m_fPatchDZ ) );
			pPatch->SetPosition( D3DXVECTOR3( fXPos, 0.0f, fZPos ) );
			pPatch->SetHeightMapPos( iXHeightPos, iYHeightPos );
			pPatch->CalculateCollisionObject();
			pPatch->CalErrors();

			fXPos += m_fPatchDX;
			iXHeightPos += ioPatch::SOURCE_WIDTH - 1;
		}

		fZPos += m_fPatchDZ;
		iYHeightPos += ioPatch::SOURCE_HEIGHT - 1;
	}

	for( y=0 ; y<m_iNumPatchY ; y++ )
	{
		for( x=0 ; x<m_iNumPatchX ; x++ )
		{
			m_pPatch[y][x]->SetNeighbor( (x>0 ? m_pPatch[y][x-1] : NULL),
										 (x<m_iNumPatchX-1 ? m_pPatch[y][x+1] : NULL),
										 (y>0 ? m_pPatch[y-1][x] : NULL),
										 (y<m_iNumPatchY-1 ? m_pPatch[y+1][x] : NULL) );
		}
	}

	CreateNormals( true );

	return true;
}

void ioLandScape::SetVisLayers( DWORD *pVisLayer )
{
	int iYHelper = 0;
	for( int y=0 ; y<m_iNumPatchY ; y++ )
	{
		for( int x=0 ; x<m_iNumPatchX ; x++ )
		{
			m_pPatch[ y ][ x ]->SetVisibleLayersFlag( pVisLayer[ x + iYHelper ] );
		}

		iYHelper += m_iNumPatchX;
	}
}

void ioLandScape::SetNotLODList( bool *pNotLOD )
{
	int iYHelper = 0;
	for( int iY=0 ; iY<m_iNumPatchY ; iY++ )
	{
		for( int iX=0 ; iX<m_iNumPatchX ; iX++ )
		{
			m_pPatch[ iY ][ iX ]->SetNotLOD( pNotLOD[ iX + iYHelper ] );
		}

		iYHelper += m_iNumPatchX;
	}
}

void ioLandScape::CheckBuffer()
{
	if( m_iNewNumVertex > m_iNumBufVertex || !m_pVtxInstance )
	{
		int iNumNewVtx = max( m_iNewNumVertex, 87380 );

		if( m_pVtxInstance )
		{
			g_VtxBufMgr.DestroyInstance( m_pVtxInstance );
			m_pVtxInstance = NULL;
		}

		m_pVtxInstance = g_VtxBufMgr.CreateExclusiveInstance( BT_DYNAMIC, IOFVF_POSITION, iNumNewVtx );

		if( m_pNrmInstance )
		{
			g_VtxBufMgr.DestroyInstance( m_pNrmInstance );
			m_pNrmInstance = NULL;
		}

		m_pNrmInstance = g_VtxBufMgr.CreateExclusiveInstance( BT_DYNAMIC, IOFVF_NORMAL, iNumNewVtx );

		if( !m_pVtxDecl )
		{
			m_pVtxDecl = new ioVertexDeclaration;
			m_pVtxDecl->InsertStreamSource( 0, IOFVF_POSITION );
			m_pVtxDecl->InsertStreamSource( 1, IOFVF_NORMAL );
		}

		m_iNumBufVertex = iNumNewVtx;
	}

	if( m_iNewNumIndex > m_iNumBufIndex || !m_pIdxInstance )
	{
		int iNumNewIdx = max( m_iNewNumIndex, 524288 );	// 524288 = 0.5M

		if( m_pIdxInstance )
		{
			g_IdxBufMgr.DestroyInstance( m_pIdxInstance );
			m_pIdxInstance = NULL;
		}

		m_pIdxInstance = g_IdxBufMgr.CreateExclusiveInstance( BT_DYNAMIC, iNumNewIdx );
		m_iNumBufIndex = iNumNewIdx;
	}

	if( m_iCurStartVtx + m_iNewNumVertex > m_iNumBufVertex )
		m_iCurStartVtx = 0;

	if( m_iCurStartIdx + m_iNewNumIndex > m_iNumBufIndex )
		m_iCurStartIdx = 0;
}

ioShader* ioLandScape::GetBestShaderAndSetLight( const ioAutoShaderParamSource &rkSource,
												 const ioLight *pMainLight,
												 const ioLightList &kPointList )
{
	ioRenderSystem &kRenderSystem = RenderSystem();

	int iPointCnt = min( MAX_POINT_LIGHT, kPointList.size() );

	ioShader *pShader = NULL;
	if( m_ShadowMapType == SMT_NOT_ENABLE )
	{
		pShader = m_pSWShader[iPointCnt];
	}
	else
	{
		pShader = m_pSWShadowMapD16[iPointCnt];
		pShader->SetMatrix( ioHashString("matShadowMap"), *rkSource.GetShadowMapMatrix() );

//		HARDCODE: 셰도우맵 텍스쳐 설정이 필요하다.
//		kRenderSystem.SetTextureShadowMap( 2 );
	}

	if( iPointCnt > 0 )
	{
		D3DXVECTOR4 vLightPos[MAX_POINT_LIGHT];
		D3DXVECTOR4 vAttenuation[MAX_POINT_LIGHT];
		D3DCOLORVALUE vDiffuseColor[MAX_POINT_LIGHT];

		ioLight *pLight = NULL;
		for( int i=0 ; i<iPointCnt ; i++ )
		{
			pLight = kPointList[i];

			vLightPos[i].x = pLight->GetDerivedPosition().x;
			vLightPos[i].y = pLight->GetDerivedPosition().y;
			vLightPos[i].z = pLight->GetDerivedPosition().z;
			vLightPos[i].w = 1.0f;

			vAttenuation[i]  = pLight->GetAttenuationFactor();
			vDiffuseColor[i] = pLight->GetDiffuseColor();
		}

		pShader->SetVectorArray( ioHashString("PointLightPosArray"), vLightPos, iPointCnt );
		pShader->SetVectorArray( ioHashString("PointAttenuationArray"), vAttenuation, iPointCnt );
		pShader->SetVectorArray( ioHashString("PointDiffuseColorArray"),
								 (const D3DXVECTOR4*)vDiffuseColor, iPointCnt );
	}

	D3DCOLORVALUE cAmbientColor = kRenderSystem.GetAmbientColor();
	cAmbientColor.r *= m_LandAmbient.r;
	cAmbientColor.g *= m_LandAmbient.g;
	cAmbientColor.b *= m_LandAmbient.b;
	cAmbientColor.a *= m_LandAmbient.a;
	pShader->SetColorValue( ioHashString("AmbientEmmColor"), cAmbientColor );

	if( pMainLight )
	{
		D3DXVECTOR3 vLightDir = pMainLight->GetDirection();
		pShader->SetVector( ioHashString("DirLightDir"), -vLightDir );

		D3DCOLORVALUE cSpecularColor = pMainLight->GetRatedSpecularColor();
		cSpecularColor.r *= m_LandSpecular.r;
		cSpecularColor.g *= m_LandSpecular.g;
		cSpecularColor.b *= m_LandSpecular.b;
		cSpecularColor.a *= m_LandSpecular.a;
		pShader->SetColorValue( ioHashString("DirSpecularColor"), cSpecularColor );

		D3DXVECTOR4 vSpecularParam( m_fSpecularPower, m_fSpecularLv, 0.0f, 0.0f );
		pShader->SetVector( ioHashString("SpecularParam"), vSpecularParam );
	}

	return pShader;
}

void ioLandScape::Render( const ioAutoShaderParamSource &rkSource,
						  const ioLight *pMainLight,
						  const ioLightList &kPointList )
{
	m_iNumRenderedTris = 0;
	m_iNumRenderedVtx  = 0;

	ioRenderSystem &kRenderSystem = RenderSystem();

	kRenderSystem.SetTextureAddressMode( 0, TAM_WRAP );
	kRenderSystem.SetTextureAddressMode( 1, TAM_WRAP );
	kRenderSystem.SetVertexDeclaration( m_pVtxDecl );

	int iVtxOffsetInBytes = m_iCurStartVtx * sizeof(D3DXVECTOR3);

	m_pD3DDevice->SetStreamSource( 0,
								   m_pVtxInstance->GetD3DBuffer(),
								   iVtxOffsetInBytes,
								   m_pVtxInstance->GetVertexStride() );

	m_pD3DDevice->SetStreamSource( 1,
								   m_pNrmInstance->GetD3DBuffer(),
								   iVtxOffsetInBytes,
								   m_pNrmInstance->GetVertexStride() );

	m_pD3DDevice->SetIndices( m_pIdxInstance->GetD3DBuffer() );

	ioShader *pShader = GetBestShaderAndSetLight( rkSource, pMainLight, kPointList );

	D3DXVECTOR4 vAlphaFactor;
	vAlphaFactor.x = m_fAlphaMaxDist;
	vAlphaFactor.y = 1.0f/( m_fAlphaMaxDist - m_fAlphaMinDist );
	vAlphaFactor.z = 1.0f;
	vAlphaFactor.w = 0.0f;

	pShader->SetMatrix( ioHashString( "matModelViewProj" ), *rkSource.GetViewProjectionMatrix() );
	pShader->SetVector( ioHashString( "AlphaFactor" ), vAlphaFactor );
	pShader->SetVector( ioHashString( "CameraPos" ), rkSource.GetCameraPos() );
	pShader->SetVector( ioHashString( "FogFactor" ), rkSource.GetFogFactor() );

	D3DXVECTOR4 vTexParam;
	vTexParam.x = 1.0f / m_fSizeX;
	vTexParam.y = -1.0f / m_fSizeZ;

	int iNumCurPassIdx = 0;
	int iCurPassIdxOffset = 0;
	int iPassCnt = m_vMultiPass.size();
	for( int iPass=0 ; iPass<iPassCnt ; iPass++ )
	{
		iNumCurPassIdx = m_vLayerNumIndexList[iPass];

		if( iNumCurPassIdx == 0 )	continue;

		if( !m_bUseSpecular || !m_vMultiPass[iPass].bSpecular )
		{
			pShader->ValidateTechnique(0);
		}
		else
		{
			pShader->ValidateTechnique(1);
		}

		vTexParam.z = m_vMultiPass[iPass].fRepeatX / m_fSizeX;
		vTexParam.w = -m_vMultiPass[iPass].fRepeatY / m_fSizeZ;
		pShader->SetVector( ioHashString("TexMatrixParam"), vTexParam );

		SetupTextureStage( iPass );

		int iShaderPass = pShader->Begin();
		for( int i=0 ; i<iShaderPass ; i++ )
		{
			pShader->BeginPass( i );

			m_pD3DDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP,
												0,
												0,
												m_iNewNumVertex,
												m_iCurStartIdx + iCurPassIdxOffset,
												iNumCurPassIdx-2 );

			pShader->EndPass();
		}
		pShader->End();

		iCurPassIdxOffset += iNumCurPassIdx;
		m_iNumRenderedTris += iNumCurPassIdx-2;
	}

	kRenderSystem.SetTexture( 2, NULL );

	m_iCurStartVtx += m_iNewNumVertex;
	m_iCurStartIdx += m_iNewNumIndex;
	m_iNumRenderedVtx  += m_iNewNumVertex;
}

void ioLandScape::SetupTextureStage( int iPass )
{
	bool bBasePass = false;
	if( iPass == 0 )
		bBasePass = true;

	ioRenderSystem &rkSystem = RenderSystem();
	rkSystem.SetDepthFunction( D3DCMP_LESSEQUAL, bBasePass, true );
	rkSystem.SetSceneBlend( D3DBLENDOP_ADD, D3DBLEND_SRCALPHA, D3DBLEND_INVSRCALPHA );
	rkSystem.SetAlphaTest( D3DCMP_GREATER, 0 );

	//-- Stage 0 -------------------------------------
	// LightMap + DiffuseColor + Ambient
	rkSystem.SetTextureStageColorBlend( 0, D3DTOP_ADD, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	rkSystem.SetTextureStageAlphaBlend( 0, D3DTOP_MODULATE, D3DTA_TEXTURE, D3DTA_DIFFUSE );

	//-- Stage 1 ----------------------------------------
	// Color Map
	if( m_vMultiPass[iPass].pTexMod )
	{
		rkSystem.SetTextureStageColorBlend( 1, D3DTOP_MODULATE, D3DTA_CURRENT, D3DTA_TEXTURE );
		rkSystem.SetTextureStageAlphaBlend( 1, D3DTOP_SELECTARG1, D3DTA_CURRENT, D3DTA_TEXTURE );
		rkSystem.DisableTextureUnitsFrom( 2 );
	}
	else
	{
		rkSystem.DisableTextureUnitsFrom( 1 );
	}

	rkSystem.SetioTexture( 0, m_vMultiPass[iPass].pTexMod );
	rkSystem.SetioTexture( 1, m_vMultiPass[iPass].pTexColor );
}

void ioLandScape::UpdateTerrain( ioCamera *pCamera )
{
	if( !m_pPatch )		return;

	if( m_iDesiredFPS > 0 )
	{
		/// 이전 프레임의 FPS를 가지고 현재 MaxError를 수정한다.
	}

	m_iNumProcessPatch = 0;
	m_iNumVisiblePatch = 0;

	UpdateTextureMatrix( pCamera );

	// For View Culling
	UpdateMinMaxArea( pCamera );
	UpdateActivePatchList( pCamera );

	UpdatePatchLevels( pCamera );
	ReUpdatePatchLevelByCheckAround();
	TesselationPatches();

	GatherBuffer();
}

void ioLandScape::UpdateMinMaxArea( const ioCamera *pCamera )
{
	float fFarP = pCamera->GetFarP();
	float fact = (float)tan( DEGtoRAD( pCamera->GetFovX() / 2.0f ) );

	D3DXVECTOR3	vCPos, vLPos, vRPos;
	vCPos = ioMath::VEC3_ZERO;
	vLPos = D3DXVECTOR3( -fFarP*fact, 0.0f, fFarP );
	vRPos = D3DXVECTOR3( fFarP*fact, 0.0f, fFarP );

	const D3DXMATRIX *pViewInv = pCamera->GetViewInverseMatrix();

	D3DXVECTOR3	vWCPos, vWLPos, vWRPos;
	D3DXVec3TransformCoord( &vWCPos, &vCPos, pViewInv );
	D3DXVec3TransformCoord( &vWLPos, &vLPos, pViewInv );
	D3DXVec3TransformCoord( &vWRPos, &vRPos, pViewInv );

	int x0 = (int)( vWCPos.x / m_fPatchDX),		y0 = (int)( vWCPos.z / m_fPatchDZ );
	int x1 = (int)( vWLPos.x / m_fPatchDX),		y1 = (int)( vWLPos.z / m_fPatchDZ );
	int x2 = (int)( vWRPos.x / m_fPatchDX),		y2 = (int)( vWRPos.z / m_fPatchDZ );

	m_iMaxX = m_iMaxY = -1;
	m_iMinX = m_iNumPatchX + 1;
	m_iMinY = m_iNumPatchY + 1;

	if( x0<m_iMinX )	m_iMinX = x0;
	if( y0<m_iMinY )	m_iMinY = y0;
	if( x0>m_iMaxX )	m_iMaxX = x0;
	if( y0>m_iMaxY )	m_iMaxY = y0;
	if( x1<m_iMinX )	m_iMinX = x1;
	if( y1<m_iMinY )	m_iMinY = y1;
	if( x1>m_iMaxX )	m_iMaxX = x1;
	if( y1>m_iMaxY )	m_iMaxY = y1;
	if( x2<m_iMinX )	m_iMinX = x2;
	if( y2<m_iMinY )	m_iMinY = y2;
	if( x2>m_iMaxX )	m_iMaxX = x2;
	if( y2>m_iMaxY )	m_iMaxY = y2;

	m_iMinX -= 2;
	m_iMinY -= 2;
	m_iMaxX += 2;
	m_iMaxY += 2;

	if( m_iMinX<0 )			m_iMinX = 0;
	if( m_iMinY<0 )			m_iMinY = 0;
	if( m_iMaxX>=m_iNumPatchX )	m_iMaxX = m_iNumPatchX - 1;
	if( m_iMaxY>=m_iNumPatchY ) m_iMaxY = m_iNumPatchY - 1;
}

void ioLandScape::UpdateActivePatchList( const ioCamera *pCamera )
{
	ioPatch *pCurPatch = NULL;

	pCurPatch = m_pActivePatch;
	while( pCurPatch )
	{
		pCurPatch->SetActiveFlag( false );
		pCurPatch = pCurPatch->GetNextActive();
	}

	D3DXVECTOR3 vCameraPos = pCamera->GetPosition();

	for( int y=m_iMinY ; y<=m_iMaxY ; y++ )
	{
		for( int x=m_iMinX ; x<=m_iMaxX ; x++ )
		{
			pCurPatch = m_pPatch[y][x];
			if( !pCurPatch )	continue;

			m_iNumProcessPatch++;

			if( pCamera->IsVisible( pCurPatch->GetWorldSphere() ) )
			{
				pCurPatch->SetVisible( true );
				pCurPatch->UpdateCameraDistanceSq( vCameraPos );

				m_iNumVisiblePatch++;
			}
			else
			{
				pCurPatch->SetVisible( false );
			}
		}
	}

	pCurPatch = m_pActivePatch;
	while( pCurPatch )
	{
		if( !pCurPatch->GetActiveFlag() )
		{
			pCurPatch->DeInit();
		}

		pCurPatch = pCurPatch->GetNextActive();
	}
}

void ioLandScape::UpdatePatchLevels( const ioCamera *pCamera )
{
/*
	D3DXMATRIX	matTemp, matWVP;
	D3DXMatrixTranslation( &matTemp, 0.0f, 0.0f, 1.2f*max( m_fPatchDY, max(m_fPatchDX,m_fPatchDZ) ) );
	D3DXMatrixMultiply(&matTemp, &matTemp, RenderSystem().GetProjectMatrix() );
	D3DXMatrixMultiply(&matWVP, RenderSystem().GetViewMatrix(), &matTemp );
*/
	D3DXMATRIX matWVP;
	D3DXMatrixMultiply( &matWVP, pCamera->GetViewMatrix(), pCamera->GetProjMatrix() );

	ioPatch *pCurPatch = m_pActivePatch;
	while( pCurPatch )
	{
		pCurPatch->UpdateProjectionError( &matWVP );

		if( !pCurPatch->IsNotLOD() )
		{
			for( int i=ioPatch::MAX_SUBDIV ; i>=0 ; i-- )
			{
				if( pCurPatch->GetProjectedError( i ) < m_fMaxError )
				{
					pCurPatch->SetRealLevel( i );
					pCurPatch->SetLevel( i );
					break;
				}
			}
		}
		else
		{
			pCurPatch->SetRealLevel( 0 );
			pCurPatch->SetLevel( 0 );
		}

		pCurPatch = pCurPatch->GetNextActive();
	}
}

void ioLandScape::ReUpdatePatchLevelByCheckAround()
{	
	ioPatch *pCurPatch = NULL;
	bool bChanged = false;

	int iLLv, iRLv, iBLv, iTLv, iSLv;

	do
	{
		bChanged = false;
		
		pCurPatch = m_pActivePatch;
		while( pCurPatch )
		{
			iLLv = pCurPatch->GetLeft()->GetNewLevelSafe();
			iRLv = pCurPatch->GetRight()->GetNewLevelSafe();
			iBLv = pCurPatch->GetBottom()->GetNewLevelSafe();
			iTLv = pCurPatch->GetTop()->GetNewLevelSafe();

			iSLv = min( min(iLLv+1, iRLv+1), min(iBLv+1, iTLv+1) );

			if( pCurPatch->GetNewLevel() > iSLv )
			{
				pCurPatch->SetLevel( iSLv );
				bChanged = true;
			}

			pCurPatch = pCurPatch->GetNextActive();
		}
	}while( bChanged );
}

class ioPatchSort : public std::binary_function< const ioPatch*, const ioPatch*, bool >
{
public:
	bool operator()( const ioPatch *lhs , const ioPatch *rhs ) const
	{
		if( lhs->GetCameraDistanceSq() < rhs->GetCameraDistanceSq() )
			return true;

		return false;
	}
};

void ioLandScape::TesselationPatches()
{
	ioPatch *pCurPatch = m_pActivePatch;
	while( pCurPatch )
	{
		pCurPatch->UpdateLevel();

		pCurPatch = pCurPatch->GetNextActive();
	}

	pCurPatch = m_pActivePatch;
	while( pCurPatch )
	{
		pCurPatch->UpdateLevelSecond();
		pCurPatch = pCurPatch->GetNextActive();
	}

	pCurPatch = m_pActivePatch;
	while( pCurPatch )
	{
		pCurPatch->UpdateLevelThird();
		pCurPatch = pCurPatch->GetNextActive();
	}

	m_vPatchCameraSortList.clear();

	pCurPatch = m_pActivePatch;
	while( pCurPatch )
	{
		m_vPatchCameraSortList.push_back( pCurPatch );
		pCurPatch = pCurPatch->GetNextActive();
	}

	std::sort( m_vPatchCameraSortList.begin(),
			   m_vPatchCameraSortList.end(),
			   ioPatchSort() );

	int i, j;
	int iCurPatchCnt = m_vPatchCameraSortList.size();

	m_iNewNumVertex = 0;
	m_vPatchVtxStartIndexList.clear();
	for( i=0 ; i<iCurPatchCnt ; i++ )
	{
		pCurPatch = m_vPatchCameraSortList[i];

		m_vPatchVtxStartIndexList.push_back( m_iNewNumVertex );
		m_iNewNumVertex += pCurPatch->GetNumVertex();
	}

	m_iNewNumIndex = 0;
	m_vLayerNumIndexList.clear();

	int iMultiPassCnt = m_vMultiPass.size();
	for( i=0 ; i<iMultiPassCnt ; i++ )
	{
		DWORD dwLayer = 1<<i;

		int iLayerIdxCnt = 0;
		for( j=0 ; j<iCurPatchCnt ; j++ )
		{
			pCurPatch = m_vPatchCameraSortList[j];

			if( pCurPatch->GetVisibleLayersFlag() &dwLayer )
			{
				iLayerIdxCnt += pCurPatch->GetNumIndex() + 2;
			}
		}

		if( iLayerIdxCnt > 0 )
		{
			iLayerIdxCnt -= 2;
		}

		m_vLayerNumIndexList.push_back( iLayerIdxCnt );
		m_iNewNumIndex += iLayerIdxCnt;
	}
}

void ioLandScape::GatherBuffer()
{
	CheckBuffer();

	int i, j;
	ioPatch *pCurPatch = NULL;
	int iCurPatchCnt = m_vPatchCameraSortList.size();

	int iLockStartBytes, iLockSizeBytes;

	// Gather Vertex Buffer

	DWORD dwLockFlags = D3DLOCK_NOOVERWRITE;
	if( m_iCurStartVtx == 0 )
		dwLockFlags = D3DLOCK_DISCARD;

	iLockStartBytes = m_iCurStartVtx * sizeof( D3DXVECTOR3 );
	iLockSizeBytes  = m_iNewNumVertex * sizeof( D3DXVECTOR3 );

	if( m_pVtxInstance->Lock( dwLockFlags, iLockStartBytes, iLockSizeBytes ) )
	{
		D3DXVECTOR3 *pVertices = (D3DXVECTOR3*)m_pVtxInstance->GetBuffer();
		for( i=0 ; i<iCurPatchCnt ; i++ )
		{
			pCurPatch = m_vPatchCameraSortList[i];
			pVertices += pCurPatch->FillVertices( pVertices );
		}
		m_pVtxInstance->UnLock();
	}

	if( m_pNrmInstance->Lock( dwLockFlags, iLockStartBytes, iLockSizeBytes ) )
	{
		D3DXVECTOR3 *pNrm = (D3DXVECTOR3*)m_pNrmInstance->GetBuffer();
		for( i=0 ; i<iCurPatchCnt ; i++ )
		{
			pCurPatch = m_vPatchCameraSortList[i];
			pNrm += pCurPatch->FillNormalVertices( pNrm );
		}
		m_pNrmInstance->UnLock();
	}

	m_iNumRenderPatch = 0;

	// Gather Index Buffers

	dwLockFlags = D3DLOCK_NOOVERWRITE;
	if( m_iCurStartIdx == 0 )
		dwLockFlags = D3DLOCK_DISCARD;

	iLockStartBytes = m_iCurStartIdx * sizeof(WORD);
	iLockSizeBytes  = m_iNewNumIndex * sizeof(WORD);

	if( !m_pIdxInstance->Lock( dwLockFlags, iLockStartBytes, iLockSizeBytes ) )
		return;

	WORD *pIndices = (WORD*)m_pIdxInstance->GetBuffer();

	DWORD dwLayer = 0;
	int iNumCurIdx, iNumCurPassIdx;
	int iPatchIdx;
	WORD wNumStartIdx;
	WORD wPrePatchLastIdx;

	int iPassCnt = m_vMultiPass.size();
	for( i=0 ; i<iPassCnt ; i++ )
	{
		if( m_vLayerNumIndexList[i] == 0 )	continue;

		dwLayer = 1<<i;
		iNumCurIdx = iNumCurPassIdx = iPatchIdx = 0;

		for( j=0 ; j<iCurPatchCnt ; j++ )
		{
			pCurPatch = m_vPatchCameraSortList[j];

			if( pCurPatch->GetVisibleLayersFlag() & dwLayer )
			{
				wNumStartIdx = (WORD)m_vPatchVtxStartIndexList[iPatchIdx];
				iNumCurIdx   = pCurPatch->GetNumIndex();

				const WORD *pCurIdx = pCurPatch->GetIndexP();

				if( iNumCurPassIdx > 0 )
				{
					pIndices[ iNumCurPassIdx++ ] = wPrePatchLastIdx;
					pIndices[ iNumCurPassIdx++ ] = pCurIdx[0] + wNumStartIdx;
				}

				for( int j=0 ; j<iNumCurIdx ; j++ )
				{
					pIndices[ iNumCurPassIdx++ ] = pCurIdx[j] + wNumStartIdx;
				}

				wPrePatchLastIdx = pCurIdx[iNumCurIdx-1] + wNumStartIdx;

				m_iNumRenderPatch++;
			}

			iPatchIdx++;
		}

		pIndices += iNumCurPassIdx;
	}

	m_pIdxInstance->UnLock();
}

char* ioLandScape::GetRenderModeByText()
{
	switch( m_RenderMode )
	{
	case RM_DEFAULT:
		return "Default";
	case RM_MORPH_SW:
		return "Software Morphing";
	}

	return "Unknown";
}

void ioLandScape::ChangeMode( RenderMode mode )
{
	for( int y=0 ; y<m_iNumPatchY ; y++ )
	{
		for( int x=0 ; x<m_iNumPatchX ; x++ )
		{
			m_pPatch[y][x]->DeInit();

			delete m_pPatch[y][x];
		}

		SAFEDELETEARRAY( m_pPatch[y] );		
	}

	SAFEDELETEARRAY( m_pPatch );

	m_RenderMode = mode;

	Build();
}

float ioLandScape::GetHeight( float fXPos, float fZPos ) const
{
	float fx, fy;
	fx = (float)(m_iHMapWidth-1) * fXPos / m_fSizeX;
	fy = (float)(m_iHMapHeight-1) * fZPos / m_fSizeZ;

	int ix, iy;
	ix = (int)fx;
	iy = (int)fy;
	
	ix = max( 0, min( ix, m_iHMapWidth-2) );
	iy = max( 0, min( iy, m_iHMapHeight-2) );

	float h00, h01, h10, h11;
	h00 = GetHeight( ix+0, iy+0 );
	h10 = GetHeight( ix+1, iy+0 );
	h01 = GetHeight( ix+0, iy+1 );
	h11 = GetHeight( ix+1, iy+1 );

	float dx, dy;
	dx = fx - (float)ix;
	dy = fy - (float)iy;

	float fHeight;
	fHeight = (1.0f-dy) * ( (1.0f-dx)*h00 + dx*h10 ) +
				    dy  * ( (1.0f-dx)*h01 + dx*h11 );

	return fHeight;	
}

// in GPG3 4.2 빠른 높이필드 법선 계산
void ioLandScape::CreateNormals( bool bMustReAlloc )
{
	if( !m_pHeight )	return;
	if( m_iHMapWidth <= 0 || m_iHMapHeight <= 0 )	return;

	if( !m_pNormal || bMustReAlloc )
	{
		SAFEDELETEARRAY( m_pNormal );
		m_pNormal = new D3DXVECTOR3[ m_iHMapWidth * m_iHMapHeight ];
	}

	int x, y;
	float fNormalY = 2.0f * m_fPatchDY;
	float h1, h2, h3, h4;
	D3DXVECTOR3 vNormal;

	for( y=1 ; y<m_iHMapHeight-1 ; y++ )
	{
		for( x=1 ; x<m_iHMapWidth-1 ; x++ )
		{
			h1 = GetHeight( x+1, y );
			h2 = GetHeight( x, y-1 );
			h3 = GetHeight( x-1, y );
			h4 = GetHeight( x, y+1 );

			vNormal.x = (h3 - h1) * ioPatch::SOURCE_WIDTH;
			vNormal.y = fNormalY;
			vNormal.z = (h2 - h4) * ioPatch::SOURCE_HEIGHT;
			D3DXVec3Normalize( &m_pNormal[ y*m_iHMapWidth + x ], &vNormal );
		}
	}

	m_pNormal[0] = m_pNormal[ 1+m_iHMapWidth ];
	m_pNormal[ m_iHMapWidth-1 ] = m_pNormal[ m_iHMapWidth + m_iHMapWidth-2 ];

	m_pNormal[ (m_iHMapHeight-1)*m_iHMapWidth ] = m_pNormal[ (m_iHMapHeight-2)*m_iHMapWidth + 1 ];
	m_pNormal[ (m_iHMapHeight-1)*m_iHMapWidth + m_iHMapWidth-1 ] = m_pNormal[ (m_iHMapHeight-2)*m_iHMapWidth + m_iHMapWidth-2];

	for( x=1 ; x<m_iHMapWidth-1 ; x++ )
	{
		m_pNormal[x] = m_pNormal[ x + m_iHMapWidth ];
		m_pNormal[x+(m_iHMapHeight-1)*m_iHMapWidth] = m_pNormal[x+(m_iHMapHeight-2)*m_iHMapWidth];
	}

	for( y=1 ; y<m_iHMapHeight-1 ; y++ )
	{
		m_pNormal[y*m_iHMapWidth] = m_pNormal[ y*m_iHMapWidth + 1];
		m_pNormal[y*m_iHMapWidth + m_iHMapWidth -1] = m_pNormal[y*m_iHMapWidth + m_iHMapWidth-2];
	}
}

void ioLandScape::UpdateTextureMatrix( const ioCamera *pCamera )
{
	const D3DXMATRIX *pViewInv = pCamera->GetViewInverseMatrix();

	D3DXMATRIX matScale;
	ZeroMemory( &matScale, sizeof(matScale) );
	matScale._11 = (1.0f / m_fSizeX );

#ifdef FLIP_Y
	matScale._32 = -(1.0f / m_fSizeZ );
#else
	matScale._32 = (1.0f / m_fSizeZ );
#endif

	D3DXMATRIX matPassScale;

	vMultiPass::iterator iter;
	for( iter = m_vMultiPass.begin(); iter!=m_vMultiPass.end() ; ++iter )
	{
		matPassScale = matScale;
		matPassScale._11 *= iter->fRepeatX;
		matPassScale._32 *= iter->fRepeatY;

		D3DXMatrixMultiply( &(iter->matTex), pViewInv, &matPassScale );
	}	
}

void ioLandScape::SetUseSpecular( bool bUse )
{
	if( bUse && RenderSystem().GetPixelShaderVersion() > 10 )
	{
		m_bUseSpecular = true;
	}
	else
	{
		m_bUseSpecular = false;
	}
}

void ioLandScape::SetLandAmbient( float fRed, float fGreen, float fBlue )
{
	m_LandAmbient.r = fRed;
	m_LandAmbient.g = fGreen;
	m_LandAmbient.b = fBlue;
	m_LandAmbient.a = 1.0f;
}

void ioLandScape::SetLandSpecular( float fRed, float fGreen, float fBlue )
{
	m_LandSpecular.r = fRed;
	m_LandSpecular.g = fGreen;
	m_LandSpecular.b = fBlue;
	m_LandSpecular.a = 1.0f;
}

void ioLandScape::SetSpecularLv( float fLv )
{
	m_fSpecularLv = fLv;
}

float ioLandScape::GetSpecularLv() const
{
	return m_fSpecularLv;
}

void ioLandScape::SetSpecularPower( float fPower )
{
	m_fSpecularPower = fPower;
}

float ioLandScape::GetSpecularPower() const
{
	return m_fSpecularPower;
}

bool ioLandScape::InitDiffuseRateByBaseLightMap()
{
	if( m_vMultiPass.empty() || !m_vMultiPass[0].pTexMod )
	{
		LOG.PrintTimeAndLog( 0, "ioLandScape::InitDiffuseRateByBaseLightMap - BaseLightMap Not Exist" );
		return false;
	}

	ioTexture *pLightMap = m_vMultiPass[0].pTexMod;

	char szFileName[MAX_PATH];
	
	if( pLightMap->IsDefaultPath() )
	{
		wsprintf( szFileName, "%s/%s", g_TextureMgr.GetStartDir(),
									   m_vMultiPass[0].pTexMod->GetName().c_str() );
	}
	else
	{
		strcpy( szFileName, m_vMultiPass[0].pTexMod->GetName().c_str() );
	}

	ioBinaryStream kStream;
	if( !g_ResourceLoader.LoadStream( szFileName, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioLandScape::InitDiffuseRateByBaseLightMap - %s Load Failed",szFileName);
		return false;
	}

	IDirect3DTexture9 *pSrcTex = NULL;
	HRESULT hr = D3DXCreateTextureFromFileInMemoryEx( RenderSystem().GetDevice(),
													  kStream.GetPtr(),
													  kStream.GetSize(),
													  pLightMap->GetWidth(),
													  pLightMap->GetHeight(),
													  1,
													  D3DPOOL_DEFAULT,
													  D3DFMT_X8R8G8B8,
													  D3DPOOL_SYSTEMMEM,
													  D3DX_FILTER_POINT,
													  D3DX_DEFAULT,
													  0,
													  NULL,
													  NULL,
													  &pSrcTex );

	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "InitDiffuseRateByBaseLightMap1: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioLandScape::InitDiffuseRateByBaseLightMap - %s Load Failed(%x)",
								szFileName, hr );
		return false;
	}

	D3DLOCKED_RECT rcLock;
	hr = pSrcTex->LockRect( 0, &rcLock, NULL, D3DLOCK_READONLY );
	if( FAILED(hr) )
	{
		ErrorReport::SetFormat( "InitDiffuseRateByBaseLightMap2: %x", hr );
		LOG.PrintTimeAndLog( 0, "ioLandScape::InitDiffuseRateByBaseLightMap - %s Lock Failed(%x)",
								szFileName, hr );
		return false;
	}

	SAFEDELETEARRAY( m_pLightMapColor );

	BYTE *pBits = (BYTE*)rcLock.pBits;
	m_iDiffuseRateMapWidth  = pLightMap->GetWidth();
	m_iDiffuseRateMapHeight = pLightMap->GetHeight();

	m_pLightMapColor = new D3DCOLORVALUE[ m_iDiffuseRateMapWidth * m_iDiffuseRateMapHeight ];

	int iYHelper = m_iDiffuseRateMapWidth*m_iDiffuseRateMapHeight;
	for( int iY=0 ; iY<m_iDiffuseRateMapHeight ; iY++ )
	{
		iYHelper -= m_iDiffuseRateMapWidth;
		for( int iX=0 ; iX<m_iDiffuseRateMapWidth ; iX++ )
		{
			m_pLightMapColor[ iYHelper + iX ].r = ( *pBits++ ) / 255.0f;
			m_pLightMapColor[ iYHelper + iX ].g = ( *pBits++ ) / 255.0f;
			m_pLightMapColor[ iYHelper + iX ].b = ( *pBits++ ) / 255.0f;
			m_pLightMapColor[ iYHelper + iX ].a = 1.0f;
			pBits++;
		}
	}

	pSrcTex->UnlockRect(0);

	SAFERELEASE( pSrcTex );

	return true;
}

const D3DCOLORVALUE& ioLandScape::GetLightMapColor( float fXPos, float fZPos )
{
	if( !m_pLightMapColor )
	{
		LOG.PrintTimeAndLog( 0, "ioLandScape::GetLightMapColor - Not Initialized" );

		static D3DCOLORVALUE sLightMapColor;
		return sLightMapColor;
	}

	int iXPos, iZPos;
	iXPos = m_iDiffuseRateMapWidth * ( fXPos / m_fSizeX );
	iXPos = max( 0, min( iXPos, m_iDiffuseRateMapWidth-1 ) );

	iZPos = m_iDiffuseRateMapHeight * ( fZPos / m_fSizeZ );
	iZPos = max( 0, min( iZPos, m_iDiffuseRateMapHeight-1 ) );

	return m_pLightMapColor[ iZPos * m_iDiffuseRateMapWidth + iXPos ];
}

