#include "StdAfx.h"

#include "../ioVoiceChat/Fourier.h"
#include "../ioBaseChar.h"
#include "../ioNpcChar.h"
#include "../ioHelmetItem.h"
#include "../ioSlotMgr.h"

#include "EtcItemSlotWnd.h"
#include "TowerDefWnd.h"

bool TowerDefUserGaugeWnd::m_bPosToDefault = false;
bool ConsumptionWnd::m_bPosToDefault = false;

//////////////////////////////////////////////////////////////////////////
TowerDef3DChar::TowerDef3DChar()
{
	m_pUI3DRender = NULL;
	m_pAvata      = NULL;
	
	m_iOffsetXPos = 0;
	m_iOffsetYPos = 0;
}

TowerDef3DChar::~TowerDef3DChar()
{
	SAFEDELETE( m_pAvata );
	SAFEDELETE( m_pUI3DRender );
}

void TowerDef3DChar::Initialize()
{
	SAFEDELETE( m_pUI3DRender );
	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "ladder_cam" );
}

void TowerDef3DChar::SetChar( ioBaseChar *pChar )
{
	if( !pChar ) return;
	if( !m_pUI3DRender )	return;

	const CHARACTER &rkInfo = pChar->GetCharacterInfo();

	SAFEDELETE( m_pAvata );
	m_pAvata = m_pUI3DRender->CreateChar( rkInfo, "ResultChar" );
	if( !m_pAvata )
	{
		LOG.PrintTimeAndLog( 0, "TowerDef3DChar::SetAvata - CreateAvata Fail" );
		return;
	}

	m_pAvata->DestroyHandAndBody();

	m_pAvata->SetPosition( -8.f, -122.f, 714.0f );
	m_pAvata->SetPitch(15.f);
	m_pUI3DRender->SetLookAt( 0.0f, 0.0f, 714.0f );

	m_pUI3DRender->SetProjection( FLOAT1, 600.0f, 45.0f, (float)VIEW_WIDTH/(float)VIEW_HEIGHT );

	// 코스튬 관련 (타워디펜스모드 헬멧)
	int TempCostume = 200001;

	ioHelmetItem *pItem = pChar->GetHelmet();
	if( pItem )
	{
		m_pAvata->EquipItem( pItem->GetItemCode(), pItem->GetItemMaleCustom(), pItem->GetItemFemaleCustom(), pItem->GetCostumeCode() );
	}
	else
	{
		m_pAvata->ReleaseItem( ES_HELMET );
	}

	m_pAvata->SetLoopAni( "public_shop_delay.ani" );
}

void TowerDef3DChar::Render( int iXPos, int iYPos )
{
	if( m_pAvata && m_pUI3DRender )
	{
		D3DRECT rcD3D;
		rcD3D.x1 = iXPos + 2;//max( 0, iXPos );
		rcD3D.y1 = iYPos + 2;//max( 0, iYPos );
		rcD3D.x2 = rcD3D.x1 + VIEW_WIDTH;
		rcD3D.y2 = rcD3D.y1 + VIEW_HEIGHT;
		m_pUI3DRender->RenderViewPort( &rcD3D, true );
	}
}

void TowerDef3DChar::FaceChange( const char *szTexPostfix, int iDuration )
{
	if( m_pAvata )
	{
		m_pAvata->DirectFaceChange( szTexPostfix, iDuration );
	}
}

void TowerDef3DChar::SetHelmChange( ioBaseChar *pChar )
{
	if( pChar == NULL ) 
		return;

	if( m_pAvata )
	{
		ioHelmetItem *pHelmet = pChar->GetHelmet();
		if( pHelmet )
			m_pAvata->EquipItem( pHelmet->GetItemCode(), pHelmet->GetItemMaleCustom(), pHelmet->GetItemFemaleCustom() );
		else
			m_pAvata->ReleaseItem( ES_HELMET );
	}
}

void TowerDef3DChar::Update( float fTimePerSec )
{
	if( m_pAvata )
	{
		m_pAvata->Update( fTimePerSec );

		// TEST
/*		ioINILoader_e kLoader( "test.ini" );
		kLoader.SetTitle_e( "common2" );
		D3DXVECTOR3 vCharPos( kLoader.LoadFloat_e( "px", 0.0f ), kLoader.LoadFloat_e( "py", 0.0f ), kLoader.LoadFloat_e( "pz", FLOAT500 ) );
		m_pAvata->SetPosition( vCharPos );
		m_pAvata->SetScale( kLoader.LoadFloat_e( "ps", FLOAT1 ) );
		m_pAvata->SetYaw( kLoader.LoadFloat_e( "yaw", FLOAT10 ) );
		m_pUI3DRender->SetLookAt( vCharPos );

		m_iOffsetXPos = kLoader.LoadInt_e( "offX", 0 );
		m_iOffsetYPos = kLoader.LoadInt_e( "offY", 0 );
*/
//		m_pAvata->SetPosition( kLoader.LoadFloat_e( "px", 0.0f ), kLoader.LoadFloat_e( "py", -90.0f ), kLoader.LoadFloat_e( "pz", 900.0f ) );
//		m_pAvata->SetYaw( kLoader.LoadFloat_e( "yaw", FLOAT10 ) );
//		m_pUI3DRender->SetLookAt( kLoader.LoadFloat_e( "lax", 0.0f ), kLoader.LoadFloat_e( "lay", 0.0f ), kLoader.LoadFloat_e( "laz", 900.0f ) );
	}
}


//////////////////////////////////////////////////////////////////////////
// TowerDefWnd
//////////////////////////////////////////////////////////////////////////
TowerDefWnd::TowerDefWnd()
{
	ZeroMemory(m_stUserGauge, sizeof(stUserGauge) * MAX_USER);
	m_szNpcName = "jalnan";
}

TowerDefWnd::~TowerDefWnd()
{

}

void TowerDefWnd::GetUserNameList(ioHashStringVec &vecName)
{
	for(int i = 0; i < MAX_USER; i++)
	{
		if(m_stUserGauge[i].eID != GAUGE_NONE)
		{
			vecName.push_back(m_stUserGauge[i].szUserName);
		}
	}
}

void TowerDefWnd::SetUser(ioBaseChar *pChar, int nInstantLv)
{
	if(!pChar)
		return;

	for(int i = 0; i < MAX_USER; i++)
	{
		if(m_stUserGauge[i].eID == GAUGE_NONE)
		{
			m_stUserGauge[i].eID = (eGaugeWndID)(GAUGE_WND1+i);
			m_stUserGauge[i].szUserName = pChar->GetCharName();

			TowerDefUserGaugeWnd* pWnd = dynamic_cast<TowerDefUserGaugeWnd*>( FindChildWnd(m_stUserGauge[i].eID) );
			if(!pWnd)
				break;

			pWnd->SetUser(pChar, nInstantLv);
			StatusValue &stHp = pChar->GetHP();
			pWnd->UpdateGauge(stHp.m_fCurValue, stHp.m_fMaxValue, nInstantLv);
			pWnd->ShowWnd();
			m_stUserGauge[i].pGaugeWnd = pWnd;

			if( pChar->IsOwnerChar())
			{
				ConsumptionWnd* pWnd = dynamic_cast<ConsumptionWnd*>( FindChildWnd(SLOT_WND) );
				if(pWnd)
				{
					pWnd->ShowWnd();
					pWnd->SetSlotWnd();
				}
			}
			break;
		}
	}
}

void TowerDefWnd::UpdateSlotItem(const int nSlot)
{
	ConsumptionWnd* pWnd = dynamic_cast<ConsumptionWnd*>( FindChildWnd(SLOT_WND) );
	if(pWnd)
	{
		ConsumptionSlotBtn* pBtn = dynamic_cast<ConsumptionSlotBtn*>( pWnd->FindChildWnd(nSlot) );
		if(pBtn)
			pBtn->UpdateItem(g_SlotMgr.GetSlot(nSlot));
	}
}

void TowerDefWnd::InitSlotItem()
{
	ConsumptionWnd* pWnd = dynamic_cast<ConsumptionWnd*>( FindChildWnd(SLOT_WND) );
	if(pWnd)
		pWnd->SetSlotWnd();
}

void TowerDefWnd::SetSurrender(const ioHashString &szName, bool bSurrender)
{
	for(int i = 0; i < MAX_USER; i++)
	{
		if(m_stUserGauge[i].szUserName == szName)
		{
			if(m_stUserGauge[i].pGaugeWnd)
				m_stUserGauge[i].pGaugeWnd->SetSurrender(bSurrender);

			break;
		}
	}
}

void TowerDefWnd::SetNPC(ioNpcChar* pNpc, int nGradeLv, bool bBoss)
{
	if( bBoss)
	{
		TowerDefUserGaugeWnd* pWnd = dynamic_cast<TowerDefUserGaugeWnd*>( FindChildWnd(NPC_GAUGE_WND) );
		if(pWnd)
			pWnd->HideWnd();

		pWnd = dynamic_cast<TowerDefUserGaugeWnd*>( FindChildWnd(NPC_BOSS_WND) );
		if(pWnd)
		{
			pWnd->ShowWnd();
			pWnd->SetUser((ioBaseChar*)pNpc, nGradeLv, GAUGE_NPC_BOSS);

			StatusValue &stHp = ((ioBaseChar*)pNpc)->GetHP();
			pWnd->UpdateGauge(stHp.m_fCurValue, stHp.m_fMaxValue, nGradeLv);
		}
	}
	else
	{
		TowerDefUserGaugeWnd* pWnd =  dynamic_cast<TowerDefUserGaugeWnd*>( FindChildWnd(NPC_BOSS_WND) );
		if(pWnd)
			pWnd->HideWnd();

		pWnd = dynamic_cast<TowerDefUserGaugeWnd*>( FindChildWnd(NPC_GAUGE_WND) );
		if(pWnd)
		{
			pWnd->ShowWnd();
			pWnd->SetUser((ioBaseChar*)pNpc, nGradeLv, GAUGE_NPC);

			StatusValue &stHp = ((ioBaseChar*)pNpc)->GetHP();
			pWnd->UpdateGauge(stHp.m_fCurValue, stHp.m_fMaxValue, nGradeLv);
		}
	}
}

void TowerDefWnd::UpdateNpcGauge(const ioHashString &rkName, float fCurHP, float fMaxHP, int nInstantLv)
{
	TowerDefUserGaugeWnd* pWnd = dynamic_cast<TowerDefUserGaugeWnd*>( FindChildWnd(NPC_GAUGE_WND) );

	if(pWnd)
	{
		if( pWnd->GetName() == rkName )
		{
			pWnd->UpdateGauge(fCurHP, fMaxHP, nInstantLv);
		}
		else
		{
			pWnd = dynamic_cast<TowerDefUserGaugeWnd*>( FindChildWnd(NPC_BOSS_WND) );

			if(pWnd)
			{
				if( pWnd->GetName() == rkName )
				{
					pWnd->UpdateGauge(fCurHP, fMaxHP, nInstantLv);
				}
			}
		}
	}
}

void TowerDefWnd::ResetNpcGauge()
{
	TowerDefUserGaugeWnd* pWnd = dynamic_cast<TowerDefUserGaugeWnd*>( FindChildWnd(NPC_GAUGE_WND) );
	if(pWnd)
	{
		pWnd->InitAniState();
		pWnd->HideWnd();
	}

	pWnd = dynamic_cast<TowerDefUserGaugeWnd*>( FindChildWnd(NPC_BOSS_WND) );
	if(pWnd)
	{
		pWnd->InitAniState();
		pWnd->HideWnd();
	}
}


const ioHashString &TowerDefWnd::GetNpcName()
{
	TowerDefUserGaugeWnd* pWnd = dynamic_cast<TowerDefUserGaugeWnd*>( FindChildWnd(NPC_GAUGE_WND) );

	if( pWnd && pWnd->IsShow())
		return pWnd->GetName();

	pWnd = dynamic_cast<TowerDefUserGaugeWnd*>( FindChildWnd(NPC_BOSS_WND) );

	if( pWnd && pWnd->IsShow())
		return pWnd->GetName();

	return m_szNpcName;
}

void TowerDefWnd::ResetAllGauge()
{
	for(int i = 0; i < MAX_USER; i++)
	{
		if(m_stUserGauge[i].eID != GAUGE_NONE)
		{
			TowerDefUserGaugeWnd* pWnd = dynamic_cast<TowerDefUserGaugeWnd*>( FindChildWnd(m_stUserGauge[i].eID) );
			if(pWnd)
			{
				pWnd->HideWnd();
				pWnd->InitAniState();
			}
		}
	}

	ZeroMemory(m_stUserGauge, sizeof(stUserGauge) * MAX_USER);
}

void TowerDefWnd::HideWnd()
{
	ioWnd::HideWnd();
	ResetAllGauge();
	ResetNpcGauge();
}

void TowerDefWnd::SetCharChange( ioBaseChar *pChar )
{
	for(int i = 0; i < MAX_USER; i++)
	{
		if( m_stUserGauge[i].szUserName == pChar->GetCharName())
		{
			if(m_stUserGauge[i].pGaugeWnd)
			{
				m_stUserGauge[i].pGaugeWnd->SetCharChange(pChar);
				break;
			}
		}
	}
}

void TowerDefWnd::SetFaceChange( const ioHashString &rkName, const char *szTexPostfix, int iDuration )
{
	for(int i = 0; i < MAX_USER; i++)
	{
		if( m_stUserGauge[i].szUserName == rkName)
		{
			if(m_stUserGauge[i].pGaugeWnd)
			{
				m_stUserGauge[i].pGaugeWnd->SetFaceChange(rkName, szTexPostfix, iDuration );
				break;
			}
		}
	}
}



void TowerDefWnd::SetHelmChange( ioBaseChar *pChar )
{
	for(int i = 0; i < MAX_USER; i++)
	{
		if( m_stUserGauge[i].szUserName == pChar->GetCharName())
		{
			if(m_stUserGauge[i].pGaugeWnd)
			{
				m_stUserGauge[i].pGaugeWnd->SetHelmChange(pChar);
				break;
			}
		}
	}
}

void TowerDefWnd::ChangeShowAniState( ShowAniType eShowAniType )
{
	for(int i = 0; i < END_GAUGE; i++)
	{
		TowerDefUserGaugeWnd *pWnd = dynamic_cast<TowerDefUserGaugeWnd*>( FindChildWnd(GAUGE_WND1 + i) );
		if( pWnd)
			pWnd->ChangeShowAniState((TowerDefUserGaugeWnd::ShowAniType)eShowAniType);
	}

	ConsumptionWnd *pSlotWnd = dynamic_cast<ConsumptionWnd*>( FindChildWnd(SLOT_WND) );
	if(pSlotWnd)
		pSlotWnd->ChangeShowAniState((ConsumptionWnd::ShowAniType)eShowAniType);
}

void TowerDefWnd::UpdateGauge(const ioHashString &rkName, float fCurHP, float fMaxHP, int nInstantLv )
{
	for(int i = 0; i < MAX_USER; i++)
	{
		if( m_stUserGauge[i].szUserName == rkName)
		{
			if( m_stUserGauge[i].pGaugeWnd)
			{
				if(m_stUserGauge[i].pGaugeWnd->IsShow())
				{
					m_stUserGauge[i].pGaugeWnd->UpdateGauge(fCurHP, fMaxHP, nInstantLv);
					break;
				}
			}
		}
	}
}



//////////////////////////////////////////////////////////////////////////
//  TowerDefUserGaugeWnd  real gauge
//////////////////////////////////////////////////////////////////////////

TowerDefUserGaugeWnd::TowerDefUserGaugeWnd()
{
	m_pPotrait[IMG_POTRAIT_BACK] = NULL;
	m_pPotrait[IMG_POTRAIT_OVERBACK] = NULL;

	m_pCharPotrait = NULL;
	m_pNpcFace = NULL;

	m_pGauge = NULL;
	m_pSurrender = NULL;

	m_pGaugeFrame = NULL;
	m_pGaugeFrameOver = NULL;

	m_iBlindOffSetY   = 0;
	m_iPreBlindOffSetY= 0;
	m_iDefaultX       = 0;
	m_iDefaultY       = 0;

	m_fMaxHP = m_fCurHP = m_fAniHP = 0.f;

	m_fCurAniTime = 0.0f;

	m_bScreen = false;
	m_fScreenTime = 0.0f;

	m_bSurrender = false;
	m_fSurrenderTime = 0.f;

	m_eGaugeType = TowerDefWnd::GAUGE_USER;

	InitShowAniState();
}

TowerDefUserGaugeWnd::~TowerDefUserGaugeWnd()
{
	SAFEDELETE(m_pPotrait[IMG_POTRAIT_BACK]);
	SAFEDELETE(m_pPotrait[IMG_POTRAIT_OVERBACK]);
	SAFEDELETE(m_pCharPotrait);
	SAFEDELETE(m_pNpcFace);

	SAFEDELETE(m_pGauge);
	SAFEDELETE(m_pSurrender);

	SAFEDELETE(m_pGaugeFrame);
	SAFEDELETE(m_pGaugeFrameOver);


	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	char szTitle[MAX_PATH] = "";
	sprintf_e( szTitle, "TowerDefUserGaugeWnd%d", ioWnd::GetID() );

 	g_GUIMgr.GUIPosSaveInt( szTitle, "XPos", m_iDefaultX ,POS_SAVE_VERSION );
 	g_GUIMgr.GUIPosSaveInt( szTitle, "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void TowerDefUserGaugeWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "UserGaugeFrame" )
	{
		SAFEDELETE( m_pGaugeFrame );
		m_pGaugeFrame = pFrame;
	}
	else if( szType == "UserGaugeFrameOver" )
	{
		SAFEDELETE( m_pGaugeFrameOver );
		m_pGaugeFrameOver = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void TowerDefUserGaugeWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{	
	if( szType == "UserFaceBG" )
	{
		SAFEDELETE( m_pPotrait[IMG_POTRAIT_BACK] );
		m_pPotrait[IMG_POTRAIT_BACK] = pImage;
	}
	else if( szType == "UserFaceOverBG" )
	{
		SAFEDELETE( m_pPotrait[IMG_POTRAIT_OVERBACK] );
		m_pPotrait[IMG_POTRAIT_OVERBACK] = pImage;
	}
	else if( szType == "GaugeBlue" )
	{
		SAFEDELETE( m_pGauge );
		m_pGauge = pImage;
	}
	else if( szType == "Surrender" )
	{
		SAFEDELETE( m_pSurrender );
		m_pSurrender = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}


void TowerDefUserGaugeWnd::SetUser( ioBaseChar *pChar, int nInstantLv, TowerDefWnd::GaugeType eGaugeType  )
{
	m_eGaugeType = eGaugeType;

	if( m_eGaugeType == TowerDefWnd::GAUGE_USER)
	{
		m_szUserName = m_szShowName = pChar->GetCharName();
		m_nLevel = nInstantLv;
		SetSurrender(false);

		SAFEDELETE(m_pCharPotrait);

		m_pCharPotrait = new TowerDef3DChar;
		m_pCharPotrait->Initialize();

		if( m_pCharPotrait )
			m_pCharPotrait->SetChar( pChar );

		m_fAniHP = 0.f;
	}
	else
	{
		m_szUserName = m_szShowName = pChar->GetCharName();
		if(ToNpcChar(pChar))
		{
			m_szShowName = ToNpcChar(pChar)->GetViewName();
			SAFEDELETE( m_pNpcFace );
			m_pNpcFace = g_UIImageSetMgr.CreateImageByFullName(ToNpcChar(pChar)->GetMonsterIcon());
		}
		m_nLevel = nInstantLv;
		StatusValue &pHp = pChar->GetHP();
		
		m_fAniHP = pHp.m_fCurValue;
	}
}

void TowerDefUserGaugeWnd::SetFaceChange( const ioHashString &rkName, const char *szTexPostfix, int iDuration )
{
	if( m_szUserName == rkName)
	{
		if( m_pCharPotrait )
			m_pCharPotrait->FaceChange( szTexPostfix, iDuration );
	}
}

void TowerDefUserGaugeWnd::SetCharChange( ioBaseChar *pChar )
{
	if( pChar->GetCharName() == m_szUserName )
	{
		if( m_pCharPotrait )
			m_pCharPotrait->SetChar( pChar );
	}
}

void TowerDefUserGaugeWnd::SetHelmChange( ioBaseChar *pChar )
{
	if( pChar->GetCharName() == m_szUserName )
	{
		if( m_pCharPotrait )
			m_pCharPotrait->SetHelmChange( pChar );
	}
}


void TowerDefUserGaugeWnd::UpdateGaugeAni()
{	
	float fGetTime = (float)FRAMEGETTIME();
	if( fGetTime - m_fScreenTime > FLOAT100 )
	{
		m_fScreenTime = fGetTime;
		m_bScreen = !m_bScreen;
	}

	if( m_fAniHP > m_fCurHP )
	{
		float fTick = FLOAT1000;
		float fGapTime = fGetTime - m_fCurAniTime;
		m_fAniHP = max( m_fCurHP, m_fAniHP - ( fGapTime / fTick ) );
		if( fGapTime >= fTick )
			m_fCurAniTime = fGetTime;
	}

	if( m_bSurrender && m_pSurrender)
	{
		if(fGetTime < (FLOAT500 + m_fSurrenderTime))
		{
			float fSurrenderAlpha = FLOAT1 - ((fGetTime - m_fSurrenderTime)/FLOAT500);
			m_pSurrender->SetAlpha( (BYTE)(fSurrenderAlpha * 0xff) );
		}
		else if(fGetTime < (FLOAT1000 + m_fSurrenderTime))
		{
			float fSurrenderAlpha = ( fGetTime - m_fSurrenderTime - FLOAT500 ) / FLOAT500;
			m_pSurrender->SetAlpha( (BYTE)(fSurrenderAlpha * 0xff) );
		}
		else
		{
			m_fSurrenderTime = fGetTime;
		}
	}
}

void TowerDefUserGaugeWnd::OnRenderUser(int iXPos, int iYPos)
{
	if( m_pGaugeFrame )
		m_pGaugeFrame->Render( iXPos + 37, iYPos + 19 );

	float fNameWidth = 200.0f;
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetTextColor( 255, 255, 255);
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.PrintTextWidthCut( iXPos + 40, iYPos + 3, FONT_SIZE_12, fNameWidth, "%s", m_szShowName.c_str() );

	float fHPRate = m_fCurHP / m_fMaxHP;

	if( m_pGauge )
	{
		float fWidth = m_pGauge->GetWidth() * fHPRate;
		m_pGauge->RenderWidthCut( iXPos + 39, iYPos + 21, 0, static_cast<int>(fWidth) );
		if( fHPRate <= 0.30f && m_bScreen )
		{
			// Screen x 1
			m_pGauge->RenderWidthCut( iXPos + 39, iYPos + 21, 0, static_cast<int>(fWidth), UI_RENDER_SCREEN );

			if( m_fCurHP <= 0.f && m_pGaugeFrame )
				m_pGaugeFrame->Render( iXPos + 37, iYPos + 19, UI_RENDER_SCREEN );

		}

		if( m_fAniHP > m_fCurHP )
		{
			float fAniHPRate = m_fAniHP / m_fMaxHP;
			float fAniWidth = m_pGauge->GetWidth() * fAniHPRate;
			// Screen x 2
			m_pGauge->RenderWidthCut( iXPos + 39 + fWidth, iYPos + 21, 0, static_cast<int>(fAniWidth - fWidth), UI_RENDER_SCREEN );
			m_pGauge->RenderWidthCut( iXPos + 39 + fWidth, iYPos + 21, 0, static_cast<int>(fAniWidth - fWidth), UI_RENDER_SCREEN );
		}
	}

	if( m_pGaugeFrameOver )
		m_pGaugeFrameOver->Render( iXPos + 37, iYPos + 19, UI_RENDER_ADD );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( 255, 255, 255);
	g_FontMgr.PrintTextAlpha( iXPos + 42, iYPos + 22, FONT_SIZE_12, 176, "Lv%d", m_nLevel+1);

	if( m_pPotrait[IMG_POTRAIT_BACK] )
		m_pPotrait[IMG_POTRAIT_BACK]->Render(iXPos, iYPos );

	if( m_pCharPotrait )
		m_pCharPotrait->Render(iXPos, iYPos);

	if( m_pPotrait[IMG_POTRAIT_OVERBACK] )
		m_pPotrait[IMG_POTRAIT_OVERBACK]->Render(iXPos, iYPos, UI_RENDER_ADD );

	if(m_bSurrender && m_pSurrender)
		m_pSurrender->Render(iXPos, iYPos);

}

void TowerDefUserGaugeWnd::OnRenderNPC(int iXPos, int iYPos)
{
	if( m_pGaugeFrame )
		m_pGaugeFrame->Render( iXPos + 37, iYPos + 19 );

	float fNameWidth = 200.0f;
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetTextColor( 255, 255, 255);
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.PrintTextWidthCut( iXPos + 40, iYPos + 3, FONT_SIZE_12, fNameWidth, "%s", m_szShowName.c_str() );

	if( m_pGauge )
	{
		float fHPRate = m_fCurHP / m_fMaxHP;
		float fWidth = m_pGauge->GetWidth() * fHPRate;
		m_pGauge->RenderWidthCut( iXPos + 39, iYPos + 21, 0, static_cast<int>(fWidth) );

		if( m_fAniHP > m_fCurHP )
		{
			float fAniHPRate = m_fAniHP / m_fMaxHP;
			float fAniWidth = m_pGauge->GetWidth() * fAniHPRate;
			// Screen x 2
			m_pGauge->RenderWidthCut( iXPos + 39 + fWidth, iYPos + 21, 0, static_cast<int>(fAniWidth - fWidth), UI_RENDER_SCREEN );
			m_pGauge->RenderWidthCut( iXPos + 39 + fWidth, iYPos + 21, 0, static_cast<int>(fAniWidth - fWidth), UI_RENDER_SCREEN );
		}
	}

	if( m_pGaugeFrameOver )
		m_pGaugeFrameOver->Render( iXPos + 37, iYPos + 19, UI_RENDER_ADD );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( 255, 255, 255);
	g_FontMgr.PrintTextAlpha( iXPos + 42, iYPos + 22, FONT_SIZE_12, 176, "Lv%d", m_nLevel+1);

	if( m_pPotrait[IMG_POTRAIT_BACK] )
		m_pPotrait[IMG_POTRAIT_BACK]->Render(iXPos, iYPos );

	if( m_pNpcFace )
		m_pNpcFace->Render(iXPos+2, iYPos+2);

	if( m_pPotrait[IMG_POTRAIT_OVERBACK] )
		m_pPotrait[IMG_POTRAIT_OVERBACK]->Render(iXPos, iYPos, UI_RENDER_ADD );
}

void TowerDefUserGaugeWnd::OnRenderBoss(int iXPos, int iYPos)
{
	if( m_pGaugeFrame )
		m_pGaugeFrame->Render( iXPos + 37, iYPos + 0 );

	if( m_pGauge )
	{
		float fHPRate = m_fCurHP / m_fMaxHP;
		float fWidth = m_pGauge->GetWidth() * fHPRate;
		m_pGauge->RenderWidthCut( iXPos + 39, iYPos + 2, 0, static_cast<int>(fWidth) );

		if( m_fAniHP > m_fCurHP )
		{
			float fAniHPRate = m_fAniHP / m_fMaxHP;
			float fAniWidth = m_pGauge->GetWidth() * fAniHPRate;
			// Screen x 2
			m_pGauge->RenderWidthCut( iXPos + 39 + fWidth, iYPos + 2, 0, static_cast<int>(fAniWidth - fWidth), UI_RENDER_SCREEN );
			m_pGauge->RenderWidthCut( iXPos + 39 + fWidth, iYPos + 2, 0, static_cast<int>(fAniWidth - fWidth), UI_RENDER_SCREEN );
		}
	}

	if( m_pGaugeFrameOver )
		m_pGaugeFrameOver->Render( iXPos + 37, iYPos + 0, UI_RENDER_ADD );

	float fNameWidth = 200.0f;
	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetTextColor( 255, 255, 255);
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.PrintTextWidthCut( iXPos + 40, iYPos + 3, FONT_SIZE_12, fNameWidth, "%s", m_szShowName.c_str() );

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( 255, 255, 255);
	g_FontMgr.PrintTextAlpha( iXPos + 42, iYPos + 22, FONT_SIZE_12, 176, "Lv%d", m_nLevel+1);

	if( m_pPotrait[IMG_POTRAIT_BACK] )
		m_pPotrait[IMG_POTRAIT_BACK]->Render(iXPos, iYPos );

	if( m_pNpcFace )
		m_pNpcFace->Render(iXPos+2, iYPos+2);

	if( m_pPotrait[IMG_POTRAIT_OVERBACK] )
		m_pPotrait[IMG_POTRAIT_OVERBACK]->Render(iXPos, iYPos, UI_RENDER_ADD );
}



void TowerDefUserGaugeWnd::OnRender()
{
	if( m_ShowAniState.GetCurState() == SAT_DELAY || 
		m_ShowAniState.GetCurState() == SAT_START_ROUND ) 
		return;

	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();	

	m_fMaxHP = max( FLOAT1, m_fMaxHP );      //

	switch(m_eGaugeType)
	{
	case TowerDefWnd::GAUGE_USER:
		OnRenderUser(iXPos, iYPos);
		break;

	case TowerDefWnd::GAUGE_NPC:
		OnRenderNPC(iXPos, iYPos);
		break;

	case TowerDefWnd::GAUGE_NPC_BOSS:
		OnRenderBoss(iXPos, iYPos);
		break;
	}
}

void TowerDefUserGaugeWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	UpdateAniState();
	UpdateGaugeAni();
	ioMovingWnd::OnProcess( fTimePerSec );

	if( m_pCharPotrait )
		m_pCharPotrait->Update( fTimePerSec );
}

void TowerDefUserGaugeWnd::iwm_create()
{
	char szTitle[MAX_PATH] = "";
	sprintf_e( szTitle, "TowerDefUserGaugeWnd%d", ioWnd::GetID() );

 	int iXPos = g_GUIMgr.GUIPosLoadInt( szTitle, "XPos", GetXPos() ,POS_SAVE_VERSION );
 	int iYPos = g_GUIMgr.GUIPosLoadInt( szTitle, "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && 
		COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}

	m_iDefaultX = GetDerivedPosX();
	m_iDefaultY = GetDerivedPosY();

	if( m_bPosToDefault )
	{
		m_bPosToDefault = false;
		SetPositionToDefault( Setting::Width(), Setting::Height(), true );
	}
}


void TowerDefUserGaugeWnd::UpdateGauge( float fCurHP, float fMaxHP, int nInstantLv )
{
	float fPrevHP = m_fCurHP;
	m_fCurHP = fCurHP;
	m_fMaxHP = fMaxHP;

	if( m_fAniHP == 0.0f && m_fCurHP > 0.0f )
	{	
		m_fAniHP = fMaxHP;
	}
	else if( fPrevHP != m_fCurHP )
	{
		if( m_fCurHP > fPrevHP)
			m_fAniHP = m_fCurHP;

		m_fCurAniTime = FRAMEGETTIME();
	}

	m_nLevel = nInstantLv;
	if(m_eGaugeType != TowerDefWnd::GAUGE_USER)
	{
		if(IsShow())
		{
			if( m_fCurHP == 0 && m_fAniHP == 0 )
				HideWnd();
		}
		
	}
}

void TowerDefUserGaugeWnd::SetBlindOffSet( int iYOffSet )
{
	m_iBlindOffSetY = iYOffSet;
}

void TowerDefUserGaugeWnd::InitAniState()
{
	m_ShowAniState.SetCurState( SAT_NONE );
}

void TowerDefUserGaugeWnd::InitShowAniState()
{
	enum { DELAY_DELAY = 165, DELAY_UP = 198, DELAY_DOWN = 66, DELAY_UP_STOP = 66, };

	m_ShowAniState.SetCurState( SAT_NONE );

	FSMState *pState = new FSMState(SAT_NONE);
	if( pState )
	{
		pState->Add( SAT_DELAY,  SAT_DELAY );
		pState->Add( SAT_START_ROUND, SAT_START_ROUND );
		pState->Add( SAT_END,  SAT_END );
	}
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_START_ROUND);
	if( pState )
	{
		pState->Add( SAT_DELAY,  SAT_DELAY);
		pState->Add( SAT_NONE,  SAT_NONE );
	}
	m_ShowAniState.Add( pState );	

	pState = new FSMState(SAT_DELAY);
	if( pState )
		pState->Add( SAT_DELAY,  SAT_UP , DELAY_DELAY);
	m_ShowAniState.Add( pState );	

	pState = new FSMState(SAT_UP);
	if( pState )
		pState->Add( SAT_UP,  SAT_DOWN , DELAY_UP);
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_DOWN);
	if( pState )
		pState->Add( SAT_DOWN,  SAT_UP_STOP , DELAY_DOWN );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_UP_STOP);
	if( pState )
		pState->Add( SAT_UP_STOP,  SAT_STOP , DELAY_UP_STOP );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_STOP);
	if( pState )
		pState->Add( SAT_END,  SAT_END );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_END);
	if( pState )
	{
		pState->Add( SAT_NONE,  SAT_NONE );
		pState->Add( SAT_START_ROUND, SAT_START_ROUND );
	}
	m_ShowAniState.Add( pState );
}

void TowerDefUserGaugeWnd::UpdateAniState()
{
	if( UpdateShowAni() )
		return;

	UpdateDefaultPos();

	// blind ani
	if( m_iBlindOffSetY != m_iPreBlindOffSetY )
	{
		int iCurYPos = m_iDefaultY + m_iBlindOffSetY;
		if( !COMPARE( iCurYPos, 0, ( Setting::Height() - GetHeight() ) + 1) )
			return;
		SetWndPos( m_iDefaultX, iCurYPos );
		m_iPreBlindOffSetY = m_iBlindOffSetY;	
	}
}

void TowerDefUserGaugeWnd::ChangeShowAniState( ShowAniType eShowAniType )
{
	m_ShowAniState.ChangeState( (int) eShowAniType );
}

bool TowerDefUserGaugeWnd::UpdateShowAni()
{
	if( !g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_NONE )
		m_ShowAniState.ChangeState( SAT_END );

	if( !g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_START_ROUND )
		m_ShowAniState.ChangeState( SAT_DELAY );

	if( g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_END )
		m_ShowAniState.ChangeState( SAT_NONE );

	m_ShowAniState.UpdateTime();

	if( m_ShowAniState.GetCurState() == SAT_UP )
	{
		enum { MINUS_Y_POS = 140 };
		int iStartYPos = MINUS_Y_POS + m_iDefaultY;
		int iCurYpos = m_iDefaultY - ( cos( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * iStartYPos );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_DOWN )
	{
		enum { MAX_OFFSET_YPOS = 11 };
		int iCurYpos = m_iDefaultY - ( sin( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_UP_STOP )
	{
		enum { MAX_OFFSET_YPOS = 11 };
		int iCurYpos = m_iDefaultY - ( cos( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_STOP )
	{
		SetWndPos( m_iDefaultX, m_iDefaultY );
		m_ShowAniState.ChangeState( SAT_END );
		return true;
	}

	return false;
}

int TowerDefUserGaugeWnd::GetCurShowAniState()
{
	return m_ShowAniState.GetCurState();
}

void TowerDefUserGaugeWnd::UpdateDefaultPos()
{
	if( g_GUIMgr.IsDragMove( m_dwID ) || IsMoving() )
	{
		if( m_iDefaultX != GetXPos() )
		{
			if( COMPARE( GetXPos(), 0, (Setting::Width() - GetWidth() ) + 1 ))
				m_iDefaultX = GetXPos();
		}

		if( m_iDefaultY != GetYPos() )
		{
			int iYPos = GetYPos() - m_iBlindOffSetY;
			if( COMPARE( iYPos, 0, (Setting::Height() - GetHeight() ) + 1 ))
				m_iDefaultY = iYPos;
		}
	}
}

void TowerDefUserGaugeWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();
		SetWndPos( m_iDefaultX, m_iDefaultY );
	}
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

ConsumptionWnd::ConsumptionWnd()
{
	m_iBlindOffSetY   = 0;
	m_iPreBlindOffSetY= 0;
	m_iDefaultX       = 0;
	m_iDefaultY       = 0;

	InitShowAniState();
}

ConsumptionWnd::~ConsumptionWnd()
{
	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "ConsumptionWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "ConsumptionWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}



void ConsumptionWnd::OnRender()
{
	if( m_ShowAniState.GetCurState() == SAT_DELAY || 
		m_ShowAniState.GetCurState() == SAT_START_ROUND ) 
		return;

	ioWnd::OnRender();
}

void ConsumptionWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	UpdateAniState();
	ioMovingWnd::OnProcess( fTimePerSec );
}

void ConsumptionWnd::iwm_create()
{
	int iXPos = g_GUIMgr.GUIPosLoadInt( "ConsumptionWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "ConsumptionWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && 
		COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}

	m_iDefaultX = GetDerivedPosX();
	m_iDefaultY = GetDerivedPosY();

	if( m_bPosToDefault )
	{
		m_bPosToDefault = false;
		SetPositionToDefault( Setting::Width(), Setting::Height(), true );
	}
}

void ConsumptionWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case SLOT_1:
	case SLOT_2:
	case SLOT_3:
	case SLOT_4:
		if( cmd == IOBN_BTNUP )
		{
			EtcItemSlotWnd *pSlotWnd = dynamic_cast< EtcItemSlotWnd * >( g_GUIMgr.FindWnd( ETC_ITEM_SLOT_WND ) );
			if(pSlotWnd->IsShow())
				pSlotWnd->GoToTop();
			else
				pSlotWnd->ShowSlotWnd();
		}
		break;
	}
}


void ConsumptionWnd::SetBlindOffSet( int iYOffSet )
{
	m_iBlindOffSetY = iYOffSet;
}

void ConsumptionWnd::InitAniState()
{
	m_ShowAniState.SetCurState( SAT_NONE );
}

void ConsumptionWnd::InitShowAniState()
{
	enum { DELAY_DELAY = 165, DELAY_UP = 198, DELAY_DOWN = 66, DELAY_UP_STOP = 66, };

	m_ShowAniState.SetCurState( SAT_NONE );

	FSMState *pState = new FSMState(SAT_NONE);
	if( pState )
	{
		pState->Add( SAT_DELAY,  SAT_DELAY );
		pState->Add( SAT_START_ROUND, SAT_START_ROUND );
		pState->Add( SAT_END,  SAT_END );
	}
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_START_ROUND);
	if( pState )
	{
		pState->Add( SAT_DELAY,  SAT_DELAY);
		pState->Add( SAT_NONE,  SAT_NONE );
	}
	m_ShowAniState.Add( pState );	

	pState = new FSMState(SAT_DELAY);
	if( pState )
		pState->Add( SAT_DELAY,  SAT_UP , DELAY_DELAY);
	m_ShowAniState.Add( pState );	

	pState = new FSMState(SAT_UP);
	if( pState )
		pState->Add( SAT_UP,  SAT_DOWN , DELAY_UP);
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_DOWN);
	if( pState )
		pState->Add( SAT_DOWN,  SAT_UP_STOP , DELAY_DOWN );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_UP_STOP);
	if( pState )
		pState->Add( SAT_UP_STOP,  SAT_STOP , DELAY_UP_STOP );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_STOP);
	if( pState )
		pState->Add( SAT_END,  SAT_END );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_END);
	if( pState )
	{
		pState->Add( SAT_NONE,  SAT_NONE );
		pState->Add( SAT_START_ROUND, SAT_START_ROUND );
	}
	m_ShowAniState.Add( pState );
}

void ConsumptionWnd::UpdateAniState()
{
	if( UpdateShowAni() )
		return;

	UpdateDefaultPos();

	// blind ani
	if( m_iBlindOffSetY != m_iPreBlindOffSetY )
	{
		int iCurYPos = m_iDefaultY + m_iBlindOffSetY;
		if( !COMPARE( iCurYPos, 0, ( Setting::Height() - GetHeight() ) + 1) )
			return;
		SetWndPos( m_iDefaultX, iCurYPos );
		m_iPreBlindOffSetY = m_iBlindOffSetY;	
	}
}

void ConsumptionWnd::ChangeShowAniState( ShowAniType eShowAniType )
{
	m_ShowAniState.ChangeState( (int) eShowAniType );
}

bool ConsumptionWnd::UpdateShowAni()
{
	if( !g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_NONE )
		m_ShowAniState.ChangeState( SAT_END );

	if( !g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_START_ROUND )
		m_ShowAniState.ChangeState( SAT_DELAY );

	if( g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_END )
		m_ShowAniState.ChangeState( SAT_NONE );

	m_ShowAniState.UpdateTime();

	if( m_ShowAniState.GetCurState() == SAT_UP )
	{
		enum { MINUS_Y_POS = 140 };
		int iStartYPos = MINUS_Y_POS + m_iDefaultY;
		int iCurYpos = m_iDefaultY - ( cos( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * iStartYPos );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_DOWN )
	{
		enum { MAX_OFFSET_YPOS = 11 };
		int iCurYpos = m_iDefaultY - ( sin( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_UP_STOP )
	{
		enum { MAX_OFFSET_YPOS = 11 };
		int iCurYpos = m_iDefaultY - ( cos( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
		SetWndPos( m_iDefaultX, iCurYpos );
		return true;
	}
	else if( m_ShowAniState.GetCurState() == SAT_STOP )
	{
		SetWndPos( m_iDefaultX, m_iDefaultY );
		m_ShowAniState.ChangeState( SAT_END );
		return true;
	}

	return false;
}

int ConsumptionWnd::GetCurShowAniState()
{
	return m_ShowAniState.GetCurState();
}

void ConsumptionWnd::UpdateDefaultPos()
{
	if( g_GUIMgr.IsDragMove( m_dwID ) || IsMoving() )
	{
		if( m_iDefaultX != GetXPos() )
		{
			if( COMPARE( GetXPos(), 0, (Setting::Width() - GetWidth() ) + 1 ))
				m_iDefaultX = GetXPos();
		}

		if( m_iDefaultY != GetYPos() )
		{
			int iYPos = GetYPos() - m_iBlindOffSetY;
			if( COMPARE( iYPos, 0, (Setting::Height() - GetHeight() ) + 1 ))
				m_iDefaultY = iYPos;
		}
	}
}

void ConsumptionWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate )
{
	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();
		SetWndPos( m_iDefaultX, m_iDefaultY + m_iBlindOffSetY );
	}
}

void ConsumptionWnd::SetSlotWnd()
{
	for(int i = 0; i < MAX_SLOT; i++)
	{
		ConsumptionSlotBtn* pBtn = (ConsumptionSlotBtn*)FindChildWnd(i);
		if(pBtn)
			pBtn->InitItem(g_SlotMgr.GetSlot(i));
	}
}


//////////////////////////////////////////////////////////////////////////
ConsumptionSlotBtn::ConsumptionSlotBtn()
{
	m_pEmptySlotImg = NULL;
	m_pItemIcon = NULL;
	m_pItemBackImg = NULL;

	m_dwCoolTime = 0;
	m_nItem = 0;
}

ConsumptionSlotBtn::~ConsumptionSlotBtn()
{
	SAFEDELETE(m_pEmptySlotImg);
	SAFEDELETE(m_pItemIcon);
	SAFEDELETE(m_pItemBackImg);
}

void ConsumptionSlotBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "EmptySlot" )
	{
		SAFEDELETE(m_pEmptySlotImg);
		m_pEmptySlotImg = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void ConsumptionSlotBtn::iwm_create()
{
}


void ConsumptionSlotBtn::OnRender()
{
	ConsumptionWnd* pConsumptionWnd = dynamic_cast<ConsumptionWnd*>( GetParent() );
	if ( pConsumptionWnd )
	{
		if( pConsumptionWnd->GetCurShowAniState() == ConsumptionWnd::SAT_DELAY || 
			pConsumptionWnd->GetCurShowAniState() == ConsumptionWnd::SAT_START_ROUND ) 
			return;
	}

	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( m_nItem == 0)
	{
		if( m_pEmptySlotImg)
		{
			m_pEmptySlotImg->Render(iXPos, iYPos, UI_RENDER_MULTIPLY);
		}
	}
	else
	{
		if(m_dwCoolTime > FRAMEGETTIME())
		{
			if(m_pItemBackImg)
			{
				m_pItemBackImg->SetScale(0.42f);
				m_pItemBackImg->Render(iXPos, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR);
			}
			
			if(m_pItemIcon)
			{
				m_pItemIcon->SetScale(0.40f);
				m_pItemIcon->Render(iXPos, iYPos, UI_RENDER_GRAY, TFO_BILINEAR);

				g_FontMgr.SetAlignType(TAT_CENTER);
				g_FontMgr.SetTextStyle(TS_OUTLINE_FULL_2X );
				g_FontMgr.SetBkColor(0, 0, 0);
				g_FontMgr.SetTextColor(255, 60, 0);
				g_FontMgr.PrintText( iXPos + 2, iYPos - 8, FONT_SIZE_13, "%d", (m_dwCoolTime - FRAMEGETTIME())/1000 );
			}
		}
		else
		{
			if(m_pItemBackImg)
			{
				m_pItemBackImg->SetScale(0.42f);
				m_pItemBackImg->Render(iXPos, iYPos, UI_RENDER_MULTIPLY, TFO_BILINEAR);
			}

			if(m_pItemIcon)
			{
				m_pItemIcon->SetScale(0.40f);
				m_pItemIcon->Render(iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR);

				if(!m_strKey.IsEmpty())
				{
					g_FontMgr.SetAlignType(TAT_CENTER);
					g_FontMgr.SetTextStyle(TS_OUTLINE_FULL_2X );
					g_FontMgr.SetBkColor(18, 78, 148);
					g_FontMgr.SetTextColor(TCT_DEFAULT_WHITE);
					g_FontMgr.PrintText( iXPos - 11, iYPos - 17, FONT_SIZE_11, m_strKey.c_str() );

					g_FontMgr.SetAlignType(TAT_CENTER);
					g_FontMgr.SetTextStyle(TS_OUTLINE_FULL_3X );
					g_FontMgr.SetBkColor(0, 0, 0);
					g_FontMgr.SetTextColor(TCT_DEFAULT_WHITE);
					g_FontMgr.PrintText( iXPos + 11, iYPos, FONT_SIZE_11, "%d", m_nItem );
				}
			}
			else
			{
				if( m_pEmptySlotImg)
				{
					m_pEmptySlotImg->Render(iXPos, iYPos, UI_RENDER_MULTIPLY);
				}
			}
		}
		
	}
}

void ConsumptionSlotBtn::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

}

void ConsumptionSlotBtn::InitItem(ioSlotItem* pItem)
{
	if(pItem == NULL)
		return;

	SAFEDELETE(m_pItemIcon);
	m_pItemIcon = g_UIImageSetMgr.CreateImageByFullName(pItem->GetImg());

	SAFEDELETE(m_pItemBackImg);
	m_pItemBackImg = g_UIImageSetMgr.CreateImageByFullName(pItem->GetBackImg());

	char szKeyText[MAX_PATH] = "";
	StringCbCopy( szKeyText, sizeof( szKeyText ), g_KeyManager.GetKeyText(pItem->GetKeyName()));

	if( strcmp( szKeyText, "지정되지 않음" ) == 0 )
		strcpy(szKeyText, "");

	szKeyText[2] = NULL;
	m_strKey = szKeyText;

	m_nItem = pItem->GetCountItem();
	m_dwCoolTime = pItem->GetCTFrame();
}

void ConsumptionSlotBtn::UpdateItem(ioSlotItem* pItem)
{
	if(pItem == NULL)
		return;

	char szKeyText[MAX_PATH] = "";
	StringCbCopy( szKeyText, sizeof( szKeyText ), g_KeyManager.GetKeyText(pItem->GetKeyName()));

	if( strcmp( szKeyText, "지정되지 않음" ) == 0 )
		strcpy(szKeyText, "");

	szKeyText[2] = NULL;
	m_strKey = szKeyText;

	m_nItem = pItem->GetCountItem();
	m_dwCoolTime = pItem->GetCTFrame();
}