#include "StdAfx.h"

#include "MonsterCoinWnd.h"

MonsterCoinUseWnd::MonsterCoinUseWnd()
{
	m_dwCoinUseType = USETYPE_START_COIN_LACK;
	m_bStartMonsterCoin = false;
	m_dwRevivalCoinUseTime = 0;
	m_iStartUseCoinCnt = 0;
	m_pOwnerChar = NULL;

	m_pIcon            = NULL;
	m_pExclamationMark = NULL;

	m_iExitModeSec     = 0;
}

MonsterCoinUseWnd::~MonsterCoinUseWnd()
{
	SAFEDELETE( m_pIcon );
	SAFEDELETE( m_pExclamationMark );
}

void MonsterCoinUseWnd::iwm_show()
{
	HideChildWnd( ID_EXIT_MODE );
	HideChildWnd( ID_SHOP_LINK );
	HideChildWnd( ID_REVIVAL );
	HideChildWnd( ID_CALCEL );
	HideChildWnd( ID_CONFIRM );

	SetChildActive( ID_EXIT );

	// 윈도우 스탈 정리
//	DWORD dwStyle = GetWndStyle();
//	dwStyle ^= IWS_MODAL;
//	ioWnd::SetWndStyle( dwStyle );

	ioWnd *pChild = FindChildWnd( ID_SHOP_LINK );
	if( pChild )
	{
		WORD wCustomKey = g_KeyManager.GetCurrentShopKey();
		if( wCustomKey != 0 )
		{
			char szTitle[MAX_PATH]="";
			SafeSprintf( szTitle, sizeof( szTitle ), STR(1), g_KeyManager.GetKeyText( wCustomKey ) );
			pChild->SetTitleText( szTitle );
		}

	}
	
	SAFEDELETE( m_pIcon );

	switch( m_dwCoinUseType )
	{
	case USETYPE_START_COIN_LACK:
		{
			SetTitleText( STR(2) );
			ShowChildWnd( ID_EXIT_MODE );
			ShowChildWnd( ID_SHOP_LINK );

			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( "UIIconPack10#goods_pvecoin_2" );
		}
		break;
	case USETYPE_REVIVAL_COIN:
		{
			SetTitleText( STR(3) );
			ShowChildWnd( ID_REVIVAL );
			ShowChildWnd( ID_CALCEL );

			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( "UIIconPack10#goods_pvecoin_1" );
		}
		break;
	case USETYPE_REVIVAL_COIN_LACK:
		{
			SetTitleText( STR(4) );
			ShowChildWnd( ID_SHOP_LINK );
			ShowChildWnd( ID_CALCEL );

			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( "UIIconPack10#goods_pvecoin_1" );
		}
		break;
	case USETYPE_JOIN_COIN_LACK:
	case USETYPE_PVE_JOIN_COIN_LACK:
	case USETYPE_PRESTART_COIN_LACK:
		{
			SetTitleText( STR(5) );
			ShowChildWnd( ID_SHOP_LINK );
			ShowChildWnd( ID_CALCEL );
			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( "UIIconPack10#goods_pvecoin_2" );
		}
		break;
	case USETYPE_PRESTART_RAID_COIN_LACK:
	case USETYPE_RAID_START_COIN_LACK:
		{
			SetTitleText( STR(5) );
			ShowChildWnd( ID_CONFIRM );

			m_pIcon = g_UIImageSetMgr.CreateImageByFullName( "UIIconPack110#Raid_Ticket" );
		}
		break;
	}
}

void MonsterCoinUseWnd::iwm_hide()
{

}

bool MonsterCoinUseWnd::iwm_esc()
{
	if( m_dwCoinUseType == USETYPE_START_COIN_LACK )
		return false;
	return ioWnd::iwm_esc();
}

bool MonsterCoinUseWnd::iwm_spacebar()
{
	if( !IsShow() )
		return false;

	switch( m_dwCoinUseType )
	{
	case USETYPE_START_COIN_LACK:
	case USETYPE_REVIVAL_COIN_LACK:
	case USETYPE_JOIN_COIN_LACK:
	case USETYPE_PVE_JOIN_COIN_LACK:
	case USETYPE_PRESTART_COIN_LACK:
		{
			//ioWnd *pShopLink = FindChildWnd( ID_SHOP_LINK );
			//if( pShopLink )
			//{
			//	iwm_command( pShopLink, IOBN_BTNUP, 0 );
			//	return true;
			//}
		}
		break;
	case USETYPE_REVIVAL_COIN:
		{
			ioWnd *pRevival = FindChildWnd( ID_REVIVAL );
			if( pRevival )
			{
				iwm_command( pRevival, IOBN_BTNUP, 0 );
				return true;
			}
		}
		break;
	}
	return false;
}

void MonsterCoinUseWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();	
	switch( dwID )
	{
	case ID_EXIT:
	case ID_CALCEL:
	case ID_CONFIRM:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_EXIT_MODE:
		if( cmd == IOBN_BTNUP )
		{
			if( !m_bStartMonsterCoin )
			{
				SP2Packet kPacket( CTPK_EXIT_ROOM );
				kPacket << EXIT_ROOM_MONSTER_COIN_LACK << 0 << true;
				TCPNetwork::SendToServer( kPacket );
				g_BattleRoomMgr.LeaveRoom();
			}
			m_bStartMonsterCoin = true;
			HideWnd();
		}
		break;
	case ID_SHOP_LINK:
		if( cmd == IOBN_BTNUP )
		{
			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShopWnd )
			{
				pNewShopWnd->ShowWnd();

				if( ioPlayMode::GetModeType() == MT_MONSTER_SURVIVAL )
					pNewShopWnd->SetDirectTab( NewShopWnd::ID_ETC_TAB_BTN, ioEtcItem::EIT_ETC_GOLDMONSTER_COIN, 0 );
				else
					pNewShopWnd->SetDirectTab( NewShopWnd::ID_ETC_TAB_BTN, ioEtcItem::EIT_ETC_MONSTER_COIN, 0 );
				
			}
		}
		break;
	case ID_REVIVAL:
		if( cmd == IOBN_BTNUP )
		{
			// 포로 상태에서 드랍존에 있으면 코인 사용 불가
			if( m_pOwnerChar && m_pOwnerChar->IsPrisonerMode() && m_pOwnerChar->IsCharDropZonePos() )
			{
				g_ChatMgr.SetSystemMsg( STR(1) );
			}
			else
			{
				m_dwRevivalCoinUseTime = FRAMEGETTIME();
				SP2Packet kPacket( CTPK_USE_MONSTER_COIN );
				kPacket << USE_MONSTER_COIN_REVIVAL;
				TCPNetwork::SendToServer( kPacket );
			}
			HideWnd();
		}
		break;
	}
}

bool MonsterCoinUseWnd::UpdateUseStartCoinLack( const ioHashString &rkMent, int iUseCoinCnt, DWORD dwCurStartCoinSec, DWORD dwMaxStartCoinSec )
{
	m_szMent = rkMent;
	m_iStartUseCoinCnt = iUseCoinCnt;
	if( !m_bStartMonsterCoin )
	{
		m_dwCoinUseType = USETYPE_START_COIN_LACK;

		char szTitle[MAX_PATH] = "";
		//SafeSprintf( szTitle, sizeof( szTitle), STR(1), dwMaxStartCoinSec - dwCurStartCoinSec );
		//SetTitleText( szTitle );

		m_iExitModeSec = dwMaxStartCoinSec - dwCurStartCoinSec;
		if( m_iExitModeSec < 0)
			m_iExitModeSec = 0;

		if( !IsShow() )
			ShowWnd();

		if( dwCurStartCoinSec > dwMaxStartCoinSec )
		{
			// 시간 종료
			ioWnd *pExitMode = FindChildWnd( ID_EXIT_MODE );
			if( pExitMode )
				iwm_command( pExitMode, IOBN_BTNUP, 0 );
		}
	}
	return m_bStartMonsterCoin;
}

void MonsterCoinUseWnd::UpdateUseRevivalCoin( ioBaseChar *pOwnerChar )
{	
	m_pOwnerChar = pOwnerChar;
	
	if( IsShow() ) return;

	int iMonsterCoin = g_MyInfo.GetEtcGoldMonsterCoin();
	if( iMonsterCoin == 0 )
		m_dwCoinUseType = USETYPE_REVIVAL_COIN_LACK;
	else
	{
		// 포로가 드랍존에서 누르면 패스
		if( m_pOwnerChar && m_pOwnerChar->IsPrisonerMode() && m_pOwnerChar->IsCharDropZonePos() )
			return;

		// 연속으로 누르는 것 방지
		if( FRAMEGETTIME() - m_dwRevivalCoinUseTime < 5000 ) 
			return; 

		m_dwCoinUseType = USETYPE_REVIVAL_COIN;
	}
	ShowWnd();
}

void MonsterCoinUseWnd::ShowJoinCoinLack( const ioHashString &rkMent, int iUseCoinCnt, ModeType eMode )
{
	HideWnd();

	if (eMode == MT_RAID)
		m_dwCoinUseType = USETYPE_RAID_START_COIN_LACK;
	else if(Help::IsMonsterDungeonMode(eMode))
		m_dwCoinUseType = USETYPE_PVE_JOIN_COIN_LACK;
	else
		m_dwCoinUseType = USETYPE_JOIN_COIN_LACK;

	m_szMent = rkMent;
	m_iStartUseCoinCnt = iUseCoinCnt;
	ShowWnd();
}

void MonsterCoinUseWnd::ShowPreStartCoinLack( const ioHashString &rkMent, int iUseCoinCnt, ModeType eMode )
{
	HideWnd();

	if (eMode == MT_RAID)
		m_dwCoinUseType = USETYPE_PRESTART_RAID_COIN_LACK;
	else
		m_dwCoinUseType = USETYPE_PRESTART_COIN_LACK;

	m_szMent = rkMent;
	m_iStartUseCoinCnt = iUseCoinCnt;
	ShowWnd();
}

void MonsterCoinUseWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	switch( m_dwCoinUseType )
	{
	case USETYPE_REVIVAL_COIN:
		{
			// 포로 상태에서 드랍존에 있으면 코인 사용 불가
			if( m_pOwnerChar )
			{
				if( !m_pOwnerChar->IsPrisonerMode() && m_pOwnerChar->GetState() != CS_DIE )
					HideWnd();
				else if( m_pOwnerChar->IsPrisonerMode() && m_pOwnerChar->IsCharDropZonePos() )
					HideWnd();
			}
		}
		break;
	case USETYPE_START_COIN_LACK:
		{
			int iMonsterCoin = g_MyInfo.GetEtcMonsterCoin() + g_MyInfo.GetEtcGoldMonsterCoin();

			if(Help::IsMonsterDungeonMode(ioPlayMode::GetModeType()))
				iMonsterCoin = g_MyInfo.GetEtcMonsterCoin();

			if( iMonsterCoin >= m_iStartUseCoinCnt )
				HideWnd();
		}
		break;
	case USETYPE_REVIVAL_COIN_LACK:
		{
			int iMonsterCoin = g_MyInfo.GetEtcGoldMonsterCoin();
			if( iMonsterCoin > 0 )
			{
				HideWnd();
				if( m_pOwnerChar )
				{
					if( m_pOwnerChar->IsPrisonerMode() || m_pOwnerChar->GetState() == CS_DIE )
						UpdateUseRevivalCoin( m_pOwnerChar );
				}				
				g_GUIMgr.HideWnd( NEW_SHOP_WND );
			}
		}
		break;
	case USETYPE_JOIN_COIN_LACK:
	case USETYPE_PRESTART_COIN_LACK:
		{
			int iMonsterCoin = g_MyInfo.GetEtcGoldMonsterCoin() + g_MyInfo.GetEtcMonsterCoin();

			if( iMonsterCoin >= m_iStartUseCoinCnt )
			{
				HideWnd();
				g_GUIMgr.HideWnd( NEW_SHOP_WND );
			}
		}
		break;
	case USETYPE_PVE_JOIN_COIN_LACK:
		{
			int iMonsterCoin = g_MyInfo.GetEtcMonsterCoin();

			if( iMonsterCoin >= m_iStartUseCoinCnt )
			{
				HideWnd();
				g_GUIMgr.HideWnd( NEW_SHOP_WND );
			}
		}
		break;
	case USETYPE_PRESTART_RAID_COIN_LACK:
	case USETYPE_RAID_START_COIN_LACK:
		{

		}
		break;
	}
}

void MonsterCoinUseWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	enum 
	{
		ICON_X_OFFSET = 125,
		ICON_Y_OFFSET = 98,

		MARK_X_OFFSET = 125,
		MARK_Y_OFFSET = 98,

		BIG_DESC_X_OFFSET = 125,
		BIG_DESC_Y_OFFSET = 146,
		BIG_DESC_Y_GAP    = 20,
		MAX_BIG_DESC      = 2,

		DESC_X_OFFSET     = 125,
		DESC_Y_OFFSET     = 197,
		DESC_Y_GAP        = 18,
		MAX_DESC          = 3,

		EMPHASIS_X_OFFSET = 125,
		EMPHASIS_Y_OFFSET = 260,
	};

	if( m_pIcon )
		m_pIcon->Render( iXPos + ICON_X_OFFSET, iYPos + ICON_Y_OFFSET );

	if( m_dwCoinUseType != USETYPE_REVIVAL_COIN )
	{
		if( m_pExclamationMark )
			m_pExclamationMark->Render( iXPos + MARK_X_OFFSET, iYPos + MARK_Y_OFFSET );
	}

	//////////////////////////////////////////////////////////////////////////	
	ioHashStringVec vBigDescVec;
	ioHashStringVec vDescVec;
	ioHashString    sEmphasis;
	DWORD           dwEmphasisColor = 0;

	switch( m_dwCoinUseType )
	{
	case USETYPE_START_COIN_LACK:
	case USETYPE_JOIN_COIN_LACK:
	case USETYPE_PRESTART_COIN_LACK:
		{
			if(ioPlayMode::GetModeType() == MT_MONSTER_SURVIVAL)
				vBigDescVec.push_back( ioHashString(STR(1)) );
			else
				vBigDescVec.push_back( ioHashString("몬스터던전을 플레이하려면") );

			char szTemp[MAX_PATH]="";
			SafeSprintf( szTemp, sizeof( szTemp ), STR(2), m_iStartUseCoinCnt );
			vBigDescVec.push_back( ioHashString(szTemp) );
			
			if(ioPlayMode::GetModeType() == MT_MONSTER_SURVIVAL)
				vDescVec.push_back( ioHashString(STR(3)) );
			else
				vDescVec.push_back("몬스터 코인이 없으면 몬스터던전을");

			vDescVec.push_back( ioHashString(STR(4)) );
			vDescVec.push_back( ioHashString(STR(5)) );
			
			if( m_dwCoinUseType == USETYPE_START_COIN_LACK )
			{ 
				dwEmphasisColor = TCT_DEFAULT_RED;
				ZeroMemory( szTemp, sizeof( szTemp ) );
				SafeSprintf( szTemp, sizeof( szTemp ), STR(6), m_iExitModeSec );
				sEmphasis = szTemp;
			}
			else
			{
				dwEmphasisColor = D3DCOLOR_XRGB( 141,87,4 );
				sEmphasis = STR(7);
			}
		}
		break;
	case USETYPE_PVE_JOIN_COIN_LACK:
		{
			vBigDescVec.push_back( ioHashString("몬스터던전을 플레이하려면") );

			char szTemp[MAX_PATH]="";
			SafeSprintf( szTemp, sizeof( szTemp ), STR(2), m_iStartUseCoinCnt );
			vBigDescVec.push_back( ioHashString(szTemp) );

			vDescVec.push_back("몬스터 코인이 없으면 몬스터던전을");

			vDescVec.push_back( ioHashString(STR(4)) );
			vDescVec.push_back( ioHashString(STR(5)) );

			if( m_dwCoinUseType == USETYPE_START_COIN_LACK )
			{ 
				dwEmphasisColor = TCT_DEFAULT_RED;
				ZeroMemory( szTemp, sizeof( szTemp ) );
				SafeSprintf( szTemp, sizeof( szTemp ), STR(6), m_iExitModeSec );
				sEmphasis = szTemp;
			}
			else
			{
				dwEmphasisColor = D3DCOLOR_XRGB( 141,87,4 );
				sEmphasis = STR(7);
			}
		}
		break;
	case USETYPE_PRESTART_RAID_COIN_LACK:
	case USETYPE_RAID_START_COIN_LACK:
		{
			vBigDescVec.push_back( ioHashString("레이드를 플레이하려면") );

			char szTemp[MAX_PATH]="";
			SafeSprintf( szTemp, sizeof( szTemp ), "레이드 티켓이 %d개 필요합니다.", m_iStartUseCoinCnt );
			vBigDescVec.push_back( ioHashString(szTemp) );

			vDescVec.push_back("레이드 티켓이 없으면 레이드를");
			vDescVec.push_back( "플레이할 수 없습니다. 레이드 티켓은" );
			vDescVec.push_back( "매일 5시에 충전됩니다." );
		}
		break;
	case USETYPE_REVIVAL_COIN:
		{
			vBigDescVec.push_back( ioHashString(STR(8)) );
			vBigDescVec.push_back( ioHashString(STR(9)) );

			vDescVec.push_back( ioHashString(STR(10)) );
			vDescVec.push_back( STR(20) );
			vDescVec.push_back( ioHashString(STR(12)) );

			dwEmphasisColor = D3DCOLOR_XRGB( 141,87,4 );
			int iMonsterCoin = g_MyInfo.GetEtcGoldMonsterCoin();
			char szTemp[MAX_PATH]="";
			SafeSprintf( szTemp, sizeof( szTemp ), STR(13), iMonsterCoin );
			sEmphasis = szTemp;
		}
		break;
	case USETYPE_REVIVAL_COIN_LACK:
		{
			vBigDescVec.push_back( ioHashString(STR(14)) );
			vBigDescVec.push_back( ioHashString(STR(15)) );

			vDescVec.push_back( ioHashString(STR(16)) );
			vDescVec.push_back( ioHashString(STR(17)) );
			vDescVec.push_back( ioHashString(STR(18)) );

			dwEmphasisColor = D3DCOLOR_XRGB( 141,87,4 );
			sEmphasis = STR(19);
		}
		break;
	}

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iBigSize = vBigDescVec.size();
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	for (int i = 0; i < MAX_BIG_DESC ; i++)
	{
		if( COMPARE( i , 0, iBigSize ) )
			g_FontMgr.PrintText( iXPos + BIG_DESC_X_OFFSET, iYPos + BIG_DESC_Y_OFFSET + (BIG_DESC_Y_GAP*i), FONT_SIZE_17, vBigDescVec[i].c_str() );
	}

	int iSize = vDescVec.size();
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	for (int i = 0; i < MAX_DESC ; i++)
	{
		if( COMPARE( i , 0, iSize ) )
			g_FontMgr.PrintText( iXPos + DESC_X_OFFSET, iYPos + DESC_Y_OFFSET + (DESC_Y_GAP*i), FONT_SIZE_13, vDescVec[i].c_str() );
	}

	if (!sEmphasis.IsEmpty())
	{
		g_FontMgr.SetTextColor( dwEmphasisColor );
		g_FontMgr.PrintText( iXPos + EMPHASIS_X_OFFSET, iYPos +EMPHASIS_Y_OFFSET, FONT_SIZE_13, sEmphasis.c_str() );
	}
}

void MonsterCoinUseWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ExclamationMark" )
	{
		SAFEDELETE( m_pExclamationMark );
		m_pExclamationMark = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}