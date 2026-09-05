

#include "stdafx.h"

#include "ioClientMaterialScriptModifier.h"

void LoggingMaterial( ioMaterial *pMaterial )
{
	std::vector< ioMaterial* > vList;
	vList.push_back( pMaterial );

	char szMaterialName[MAX_PATH];
	strcpy( szMaterialName, pMaterial->GetName().c_str() );

	int iLen = strlen(szMaterialName);
	for( int i=0 ;i<iLen ; i++ )
	{
		if( szMaterialName[i] == '/' )
			szMaterialName[i] = '_';
	}

	char szPath[MAX_PATH];
	wsprintf( szPath, "Test/%s.txt", szMaterialName );

	g_MaterialMgr.SaveScript( vList, szPath );
}

LoadingScriptMod::LoadingScriptMod()
{
	m_LightQuality = LQ_MIDDLE;
	m_bUseFixedMode = false;
}

LoadingScriptMod::~LoadingScriptMod()
{
}

void LoadingScriptMod::ModifyMaterial( ioMaterial *pMaterial )
{
	int iTechCnt = pMaterial->GetNumTechniques();
	for( int i=0 ; i<iTechCnt ; i++ )
	{
		ioTechnique *pTech = pMaterial->GetTechnique( i );
		if( !pTech ) continue;

		ioPass *pPass = pTech->GetPass( 0 );
		if( !pPass ) return;

		const ioHashString *pStr = pPass->GetShaderGroupName();
		if( !pStr )	return;

		char szLowName[MAX_PATH];
		strcpy( szLowName, pStr->c_str() );
		strlwr( szLowName );

		if( m_bUseFixedMode )
		{
		/*
			라이트맵은 부하가 커서 제외했음
			if( strstr( szLowName, "lightmap" ) )
			{
				LightMapShaderDisable( pTech );
			}
			else 
		*/
			if( !strcmp( szLowName, "static" ) )
			{
				StaticShaderDisable( pTech );
			}
		}

		if( m_LightQuality != LQ_LOW )
		{
			PixelShaderEnable( pTech, szLowName );
		}
		else
		{
			PixelShaderDisable( pTech, szLowName );
		}
	}

//	LoggingMaterial( pMaterial );
}

void LoadingScriptMod::LightMapShaderDisable( ioTechnique *pTech )
{
	ioPass *pPass = pTech->GetPass( 0 );
	if( pPass )
	{
		pPass->SetDisableShader( true );

		ioTextureUnitState *pState = pPass->GetTextureUnitState( 0 );
		if( pState )
		{
			pState->SetColorStageBlendOp( D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
		}
	}
}

void LoadingScriptMod::StaticShaderDisable( ioTechnique *pTech )
{
	ioPass *pPass = pTech->GetPass( 0 );
	if( pPass )
	{
		pPass->SetDisableShader( true );
	}
}

void LoadingScriptMod::PixelShaderDisable( ioTechnique *pTech, const char *szGroup )
{
	if( !strstr( szGroup, "static" ) && !strstr( szGroup, "animate" ) )
		return;

	if( pTech->GetNumPasses() < 3 )
		return;

	pTech->RemovePass( 0 );		// delete ToonShade Pass
	pTech->ReArrangePassIndex();

	ioPass *pPass = pTech->GetPass( 0 );
	if( !pPass )	return;

	if( pPass->GetSourceBlendFactor() != D3DBLEND_ZERO || pPass->GetDestBlendFactor() != D3DBLEND_SRCCOLOR )
		return;

	pPass->SetSceneBlending( D3DBLENDOP_ADD, D3DBLEND_ONE, D3DBLEND_ZERO );
	pPass->SetDepthWriteEnable( true );
	pPass->SetLightingEnable( true );
	pPass->AddPassProperty( ioPass::PP_SHADOW_CAST_PASS );	// 최저사양에서 그림자 켜는 상황 대비

	ioTextureUnitState *pStage = pPass->GetTextureUnitState(0);
	if( pStage )
	{
		if( pStage->GetColorStageBlendOp().m_StageOp == D3DTOP_MODULATE )
		{
			pStage->SetColorStageBlendOp( D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
		}
	}

	int iTexUnitCount = pPass->GetNumTextureUnitStates();
	while( iTexUnitCount > 1 )
	{
		--iTexUnitCount;
		pPass->RemoveTextureUnitState( iTexUnitCount );
	}
}

void LoadingScriptMod::PixelShaderEnable( ioTechnique *pTech, const char *szGroup )
{
	if( !strcmp( szGroup, "statictoonshade" ) )
	{
		StaticToonShade( pTech );
	}
	else if( !strcmp( szGroup, "animatetoonshade" ) )
	{
		AnimateToonShade( pTech );
	}
}

void LoadingScriptMod::StaticToonShade( ioTechnique *pTech )
{
	if( pTech->GetNumPasses() < 2 )
		return;

	ioPass *pPass = pTech->GetPass( 0 );
	if( pPass->GetNumTextureUnitStates() != 2 )
		return;

	ioPass *pSecondPass = pTech->GetPass( 1 );
	if( pSecondPass->GetNumTextureUnitStates() < 1 )
		return;

	pPass->RemovePassProperty( ioPass::PP_LIGHT_ITERATE_PASS );
	
	pPass->CreateTextureUnitState();	// 3번째 textureUnit 생성
	pPass->SwapTextureUnitStateOrder( 1, 2 );	// 두번째와 세번째의 순서 변경

	ioTextureUnitState *pSecond = pPass->GetTextureUnitState( 1 );
	if( pSecond )
	{
		pSecond->SetColorStageBlendOp( D3DTOP_ADD, D3DTA_CURRENT, D3DTA_TFACTOR );
		pSecond->SetAlphaStageBlendOp( D3DTOP_SELECTARG1, D3DTA_CURRENT, D3DTA_TFACTOR );
		pSecond->SetTextureCoordSet( 1 );
	}

	ioTextureUnitState *pThird  = pPass->GetTextureUnitState( 2 );
	if( pThird )
	{
		pThird->SetColorStageBlendOp( D3DTOP_MODULATE, D3DTA_CURRENT, D3DTA_TEXTURE );
		pThird->SetAlphaStageBlendOp( D3DTOP_MODULATE, D3DTA_CURRENT, D3DTA_TEXTURE );
		pThird->SetTextureFiltering( pSecondPass->GetTextureUnitState(0)->GetTextureFiltering() );
		pThird->SetTextureCoordSet( 2 );
	}

	pTech->RemovePass( 1 );	// 중간의 텍스쳐 패스 삭제
	pTech->ReArrangePassIndex();	// 뒤쪽 인덱스 땡김
}

void LoadingScriptMod::AnimateToonShade( ioTechnique *pTech )
{
	if( pTech->GetNumPasses() < 2 )
		return;

	ioPass *pSecond = pTech->GetPass( 1 );
	if( pSecond->GetNumTextureUnitStates() == 1 )
	{
		AnimateToonShadeDefault( pTech );
	}
	else
	{
		AnimateToonShadeSkin( pTech );
	}

	if( m_LightQuality == LQ_HIGH )
	{
		AddTwoPassShade( pTech );
	}
}

void LoadingScriptMod::AnimateToonShadeDefault( ioTechnique *pTech )
{
	ioPass *pPass = pTech->GetPass( 0 );
	if( pPass->GetNumTextureUnitStates() != 2 )
		return;

	pPass->RemovePassProperty( ioPass::PP_LIGHT_ITERATE_PASS );

	pPass->CreateTextureUnitState();	// 3번째 textureUnit 생성
	pPass->SwapTextureUnitStateOrder( 1, 2 );	// 두번째와 세번째의 순서 변경

	ioTextureUnitState *pSecond = pPass->GetTextureUnitState( 1 );
	if( pSecond )
	{
		pSecond->SetColorStageBlendOp( D3DTOP_ADD, D3DTA_CURRENT, D3DTA_TFACTOR );
		pSecond->SetAlphaStageBlendOp( D3DTOP_SELECTARG1, D3DTA_CURRENT, D3DTA_TFACTOR );
		pSecond->SetTextureCoordSet( 1 );
	}

	ioTextureUnitState *pThird  = pPass->GetTextureUnitState( 2 );
	if( pThird )
	{
		pThird->SetColorStageBlendOp( D3DTOP_MODULATE, D3DTA_CURRENT, D3DTA_TEXTURE );
		pThird->SetAlphaStageBlendOp( D3DTOP_MODULATE, D3DTA_CURRENT, D3DTA_TEXTURE );
		pThird->SetTextureCoordSet( 2 );
	}

	ioPass *pSecondPass = pTech->GetPass( 1 );
	if( pSecondPass->GetNumTextureUnitStates() > 1 )
	{
		pThird->SetTextureFiltering( pSecondPass->GetTextureUnitState(0)->GetTextureFiltering() );
	}

	pTech->RemovePass( 1 );	// 중간의 텍스쳐 패스 삭제
	pTech->ReArrangePassIndex();	// 뒤쪽 인덱스 땡김
}

void LoadingScriptMod::AnimateToonShadeSkin( ioTechnique *pTech )
{
	ioPass *pPass = pTech->GetPass( 0 );
	if( pPass->GetNumTextureUnitStates() != 2 )
		return;

	pPass->SetShaderGroup( "AnimateToonShadeSkin" );
	pPass->RemovePassProperty( ioPass::PP_LIGHT_ITERATE_PASS );

	ioTextureUnitState *pSecond = pPass->GetTextureUnitState( 1 );
	if( pSecond )
	{
		pSecond->SetColorStageBlendOp( D3DTOP_MODULATE, D3DTA_CURRENT, D3DTA_TEXTURE );
		pSecond->SetAlphaStageBlendOp( D3DTOP_MODULATE, D3DTA_CURRENT, D3DTA_TEXTURE );
	}

	ioPass *pSecondPass = pTech->GetPass( 1 );
	if( pSecondPass->GetNumTextureUnitStates() > 1 )
	{
		pSecond->SetTextureFiltering( pSecondPass->GetTextureUnitState(0)->GetTextureFiltering() );
	}

	pTech->RemovePass( 1 );	// 중간의 텍스쳐 패스 삭제
	pTech->ReArrangePassIndex();	// 뒤쪽 인덱스 땡김
}

void LoadingScriptMod::AddTwoPassShade( ioTechnique *pTech )
{
	if( pTech->IsTransparent() )	return;

	ioPass *pToon = pTech->GetPassByShaderGrp( SGT_TOON_SHADE );
	if( !pToon )	return;

	ioPass *p2Pass = pTech->CreatePass();
	*p2Pass = *pToon;

	p2Pass->SetDepthWriteEnable( false );
	p2Pass->SetSceneBlending( D3DBLENDOP_ADD, D3DBLEND_INVDESTCOLOR, D3DBLEND_ONE );

	p2Pass->AddPassProperty( ioPass::PP_LIGHT_ITERATE_PASS );
	p2Pass->RemovePassProperty( ioPass::PP_SHADOW_CAST_PASS );
	p2Pass->RemovePassLightType( ioPass::PLT_DIRECTIONAL );

	if( p2Pass->GetNumTextureUnitStates() == 3 )
	{
		p2Pass->RemoveTextureUnitState( 1 );
	}

	ioTextureUnitState *pSecond = p2Pass->GetTextureUnitState( 1 );
	if( pSecond )
	{
		pSecond->SetColorStageBlendOp( D3DTOP_SELECTARG1, D3DTA_CURRENT, D3DTA_TEXTURE );
		pSecond->SetAlphaStageBlendOp( D3DTOP_MODULATE,   D3DTA_CURRENT, D3DTA_TEXTURE );
		pSecond->SetTextureCoordSet( 1 );
	}
}

//-----------------------------------------------------------------------------------------

CloneScriptMod::CloneScriptMod()
{
	m_bEnableOutLine = false;
}

CloneScriptMod::~CloneScriptMod()
{
}

void CloneScriptMod::EnableOutLine( bool bEnable )
{
	m_bEnableOutLine = bEnable;
}

void CloneScriptMod::ModifyMaterial( ioMaterial *pMaterial )
{
	int iTechCnt = pMaterial->GetNumTechniques();
	for( int i=0 ; i<iTechCnt ; i++ )
	{
		ioTechnique *pTech = pMaterial->GetTechnique( i );
		if( pTech )
		{
			EnableOutLinePass( pTech );
		}
	}
}

void CloneScriptMod::EnableOutLinePass( ioTechnique *pTech )
{
	ioPass *pOutLine = pTech->GetPassByShaderGrp( SGT_OUTLINE );
	if( pOutLine )
	{
		pOutLine->SetPassEnable( m_bEnableOutLine );
	}
}

//----------------------------------------------------------------------------------------------

LightQualityMod::LightQualityMod()
{
	m_LightQuality = LQ_MIDDLE;
	m_bEntityModify = false;
}

LightQualityMod::LightQualityMod( LightQuality eQuality )
{
	m_LightQuality = eQuality;
	m_bEntityModify = false;
}

LightQualityMod::~LightQualityMod()
{
}

void LightQualityMod::ModifyMaterial( ioMaterial *pMaterial )
{
	char szLowName[MAX_PATH];

	int iTechCnt = pMaterial->GetNumTechniques();
	for( int i=0 ; i<iTechCnt ; i++ )
	{
		ioTechnique *pTech = pMaterial->GetTechnique( i );
		if( !pTech ) continue;

		ioPass *pPass = pTech->GetPass( 0 );
		if( !pPass ) continue;

		const ioHashString *pStr = pPass->GetShaderGroupName();
		if( !pStr ) continue;

		strcpy( szLowName, pStr->c_str() );
		strlwr( szLowName );

/*
	m_bEntityModify : true  => 현재 오브젝트의 매터리얼로써 사용중인것들
	                  false => 매니저의 메모리에 로딩만 되있는 스크립트

	if( !m_bEntityModify && !strstr( szLowName, "animate" ) )
	의 의미는 매니저의 것들중에 캐릭터에 쓰이는 것들만 이라는 뜻임

	m_bEntityModify가 true인 경우에는 캐릭터에 쓰이는 것들중에 "static"도 있다. ( CPU애니메이션때문에 )
*/

		if( !m_bEntityModify && !strstr( szLowName, "animate" ) )
			continue;

		if( m_LightQuality == LQ_HIGH )
		{
			int iChange = ChangeDefaultToToonShade( pTech, szLowName );

			if( iChange == CHANGE_NONE )
			{
				if( strstr( szLowName, "toonshadeskin" ) )
					iChange = CHANGE_SKIN;
				else if( strstr(szLowName, "toonshade" ) )
					iChange = CHANGE_NO_SKIN;
			}

			if( iChange == CHANGE_SKIN )
				AddToonShadePass( pTech, 1 );
			else if( iChange == CHANGE_NO_SKIN )
				AddToonShadePass( pTech, 2 );
		}
		else
		{
			RemoveTwoPassShade( pTech );

			if( m_LightQuality == LQ_MIDDLE )
				ChangeDefaultToToonShade( pTech, szLowName );
			else	// LOW
				ChangeToonShadeToDefault( pTech, szLowName );
		}
	}

//	LoggingMaterial( pMaterial );
}

int LightQualityMod::ChangeDefaultToToonShade( ioTechnique *pTech, const char *szGroupName )
{
	if( pTech->GetNumPasses() < 2 )
		return CHANGE_FAIL;

	ioPass *pPass = pTech->GetPass( 0 );
	if( pPass->GetShaderGroupType() != SGT_DEFAULT )
		return CHANGE_NONE;

	ioTextureUnitState *pTexUnit = pPass->GetTextureUnitState( 0 );
	if( !pTexUnit )	return CHANGE_FAIL;

	bool bSkin = false;
	if( pTexUnit->GetColorStageBlendOp().m_StageOp == D3DTOP_BLENDTEXTUREALPHA )
		bSkin = true;

	ioHashString szTexture = pTexUnit->GetTextureName();
	TextureFilterOption eFilter = pTexUnit->GetTextureFiltering();
	pTexUnit = NULL;

	pPass->RemoveAllTextureUnitStates();

	bool bAnimate = true;
	if( strstr( szGroupName, "static" ) )
		bAnimate = false;

	if( bSkin )
	{
		if( bAnimate )
			pPass->SetShaderGroup( "AnimateToonShadeSkin" );
		else
			pPass->SetShaderGroup( "StaticToonShadeSkin" );
	}
	else
	{
		if( bAnimate )
			pPass->SetShaderGroup( "AnimateToonShade" );
		else
			pPass->SetShaderGroup( "StaticToonShade" );
	}

	pPass->AddPassProperty( ioPass::PP_SHADOW_CAST_PASS );

	ioTextureUnitState *pFirst = pPass->CreateTextureUnitState();
	if( pFirst )
	{
		pFirst->SetUseLightTexture();
		pFirst->SetTextureAddressMode( TAM_CLAMP );
		pFirst->SetAlphaStageBlendOp( D3DTOP_SELECTARG2, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	}

	if( bSkin )
	{
		ioTextureUnitState *pSecond = pPass->CreateTextureUnitState( szTexture );
		if( pSecond )
		{
			pSecond->SetColorStageBlendOp( D3DTOP_MODULATE, D3DTA_CURRENT, D3DTA_TEXTURE );
			pSecond->SetAlphaStageBlendOp( D3DTOP_MODULATE, D3DTA_CURRENT, D3DTA_TEXTURE );
			pSecond->SetTextureFiltering( eFilter );
		}
	}
	else
	{
		ioTextureUnitState *pSecond = pPass->CreateTextureUnitState();
		if( pSecond )
		{
			pSecond->SetColorStageBlendOp( D3DTOP_ADD, D3DTA_CURRENT, D3DTA_TFACTOR );
			pSecond->SetAlphaStageBlendOp( D3DTOP_SELECTARG1, D3DTA_CURRENT, D3DTA_TFACTOR );
		}

		ioTextureUnitState *pThird = pPass->CreateTextureUnitState( szTexture );
		if( pThird )
		{
			pThird->SetColorStageBlendOp( D3DTOP_MODULATE, D3DTA_CURRENT, D3DTA_TEXTURE );
			pThird->SetAlphaStageBlendOp( D3DTOP_MODULATE, D3DTA_CURRENT, D3DTA_TEXTURE );
			pThird->SetTextureFiltering( eFilter );
		}
	}

	pPass->Load();

	if( bSkin )
		return CHANGE_SKIN;

	return CHANGE_NO_SKIN;
}

void LightQualityMod::ChangeToonShadeToDefault( ioTechnique *pTech, const char *szGroupName )
{
	ioPass *pPass = pTech->GetPass( 0 );
	if( pPass->GetShaderGroupType() != SGT_TOON_SHADE )
		return;

	bool bSkin;
	ioTextureUnitState *pTexUnit = NULL;
	if( strstr( szGroupName, "skin" ) )
	{
		pTexUnit = pPass->GetTextureUnitState( 1 );
		bSkin = true;
	}
	else
	{
		pTexUnit = pPass->GetTextureUnitState( 2 );
		bSkin = false;
	}

	if( !pTexUnit )	return;

	// BackUp..
	ioHashString szTexName = pTexUnit->GetTextureName();
	TextureFilterOption eFilter = pTexUnit->GetTextureFiltering();
	pTexUnit = NULL;

	pPass->RemoveAllTextureUnitStates();

	if( strstr( szGroupName, "animate" ) )
		pPass->SetShaderGroup( "Animate" );
	else
		pPass->SetShaderGroup( "Static" );

	ioTextureUnitState *pFirst = pPass->CreateTextureUnitState( szTexName );
	if( pFirst )
	{
		if( bSkin )
		{
			pFirst->SetColorStageBlendOp( D3DTOP_BLENDTEXTUREALPHA, D3DTA_TEXTURE, D3DTA_TFACTOR );
			pFirst->SetAlphaStageBlendOp( D3DTOP_SELECTARG1, D3DTA_DIFFUSE, D3DTA_DIFFUSE );
		}
		else
		{
			pFirst->SetColorStageBlendOp( D3DTOP_SELECTARG1, D3DTA_TEXTURE, D3DTA_DIFFUSE );
		}

		pFirst->SetTextureFiltering( eFilter );
	}

	pPass->Load();
}

void LightQualityMod::AddToonShadePass( ioTechnique *pTech, int iToonTextureStage )
{
	if( pTech->GetNumPasses() >= 3 || pTech->IsTransparent() )
		return;

	ioPass *pToon = pTech->GetPassByShaderGrp( SGT_TOON_SHADE );
	if( !pToon )	return;

	ioPass *p2Pass = pTech->CreatePass();
	p2Pass->CopyExceptTextureUnitStates( *pToon );

	p2Pass->SetSceneBlending( D3DBLENDOP_ADD, D3DBLEND_INVDESTCOLOR, D3DBLEND_ONE );
	p2Pass->SetDepthWriteEnable( false );

	p2Pass->AddPassProperty( ioPass::PP_LIGHT_ITERATE_PASS );
	p2Pass->RemovePassProperty( ioPass::PP_SHADOW_CAST_PASS );
	p2Pass->RemovePassLightType( ioPass::PLT_DIRECTIONAL );

	ioTextureUnitState *pFirst = p2Pass->CreateTextureUnitState();
	if( pFirst )
	{
		pFirst->SetUseLightTexture();
		pFirst->SetTextureAddressMode( TAM_CLAMP );
		pFirst->SetAlphaStageBlendOp( D3DTOP_SELECTARG2, D3DTA_TEXTURE, D3DTA_DIFFUSE );
	}

	ioTextureUnitState *pToonSec = pToon->GetTextureUnitState( iToonTextureStage );
	if( pToonSec )
	{
		ioTextureUnitState *pSecond = p2Pass->CreateTextureUnitState();
		if( pSecond )
		{
			*pSecond = *pToonSec;
			pSecond->SetColorStageBlendOp( D3DTOP_SELECTARG1, D3DTA_CURRENT, D3DTA_TEXTURE );
			pSecond->SetAlphaStageBlendOp( D3DTOP_MODULATE, D3DTA_CURRENT, D3DTA_TEXTURE );
			pSecond->SetTextureCoordSet( 1 );
		}
	}
}

void LightQualityMod::RemoveTwoPassShade( ioTechnique *pTech )
{
	if( pTech->GetNumPasses() < 3 || pTech->IsTransparent() )
		return;

	int iRemovePass = pTech->GetNumPasses() - 1;
	ioPass *pRemovePass = pTech->GetPass( iRemovePass );
	if( pRemovePass && pRemovePass->GetShaderGroupType() == SGT_TOON_SHADE )
	{
		if( pRemovePass->HasProperty( ioPass::PP_LIGHT_ITERATE_PASS ) &&
			!pRemovePass->IsLightAvailableType( ioPass::PLT_DIRECTIONAL ) )
		{
			pTech->RemovePass( iRemovePass );
			pTech->ReArrangePassIndex();
			pRemovePass = NULL;
		}
	}
}

//-----------------------------------------------------------------------------------------------------------------

EnableOutLineMod::EnableOutLineMod()
{
	m_bEnableOutLine = false;
}

EnableOutLineMod::EnableOutLineMod( bool bEnable )
{
	m_bEnableOutLine = bEnable;
}

EnableOutLineMod::~EnableOutLineMod()
{
}

void EnableOutLineMod::ModifyMaterial( ioMaterial *pMaterial )
{
	int iTechCnt = pMaterial->GetNumTechniques();
	for( int i=0 ; i<iTechCnt ; i++ )
	{
		ioTechnique *pTech = pMaterial->GetTechnique( i );
		if( pTech )
		{
			ioPass *pOutLine = pTech->GetPassByShaderGrp( SGT_OUTLINE );
			if( pOutLine )
			{
				pOutLine->SetPassEnable( m_bEnableOutLine );
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------

ApplyAnimateGroupOnly::ApplyAnimateGroupOnly()
{
	m_pModifier = NULL;
}

ApplyAnimateGroupOnly::ApplyAnimateGroupOnly( ioMaterialScriptModifier *pModifer )
{
	m_pModifier = pModifer;
}

ApplyAnimateGroupOnly::~ApplyAnimateGroupOnly()
{
	SAFEDELETE( m_pModifier );
}

void ApplyAnimateGroupOnly::ModifyMaterial( ioEntityGroup *pGrp )
{
	if( !pGrp->HasSkeleton() ) 
		return;

	int iNumEntity = pGrp->GetNumEntities();
	for( int i=0 ; i<iNumEntity ; i++ )
	{
		ioEntity *pEntity = pGrp->GetEntity( i );

		int iNumSubEntity = pEntity->GetNumSubEntity();
		for( int j=0 ; j<iNumSubEntity ; j++ )
		{
			ioSubEntity *pSubEntity = pEntity->GetSubEntity( j );
			m_pModifier->ModifyMaterial( pSubEntity->GetMaterial() );
		}
	}
}

//--------------------------------------------------------------------------------------------------------------------

ApplyAllEntityGroup::ApplyAllEntityGroup()
{
	m_pModifier = NULL;
}

ApplyAllEntityGroup::ApplyAllEntityGroup( ioMaterialScriptModifier *pModifer )
{
	m_pModifier = pModifer;
}

ApplyAllEntityGroup::~ApplyAllEntityGroup()
{
	SAFEDELETE( m_pModifier );
}

void ApplyAllEntityGroup::ModifyMaterial( ioEntityGroup *pGrp )
{
	int iNumEntity = pGrp->GetNumEntities();
	for( int i=0 ; i<iNumEntity ; i++ )
	{
		ioEntity *pEntity = pGrp->GetEntity( i );

		int iNumSubEntity = pEntity->GetNumSubEntity();
		for( int j=0 ; j<iNumSubEntity ; j++ )
		{
			ioSubEntity *pSubEntity = pEntity->GetSubEntity( j );
			m_pModifier->ModifyMaterial( pSubEntity->GetMaterial() );
		}
	}
}
