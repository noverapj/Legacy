
#include "StdAfx.h"

#include "BingoWnd.h"
#include "BingoRightWnd.h"
#include "BingoToolTip.h"
#include "../ioBingo.h"

//////////////////////////////////////////////////////////////////////////
BingoWnd::BingoWnd()
{
	for( int i=0; i<10; ++i )
	{
		m_pStopEffect[i] = NULL;
	}
	m_State = US_NONE;
	m_dwRollingStartTime = 0;
	m_dwNumberRollingEndTime = 0;
	m_dwRewardRollingEndTime = 0;
	m_dwRollingCheckTime = 0;

	m_iSelectDummyCode = 0;
	m_iSelectNumber    = 0;
}

BingoWnd::~BingoWnd()
{
	for( int i=0; i<10; ++i )
	{
		SAFEDELETE( m_pStopEffect[i] );
	}
}

void BingoWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "StopEffect1" )
	{
		SAFEDELETE( m_pStopEffect[0] );
		m_pStopEffect[0] = pImage;
	}
	else if( szType == "StopEffect2" )
	{
		SAFEDELETE( m_pStopEffect[1] );
		m_pStopEffect[1] = pImage;
	}
	else if( szType == "StopEffect3" )
	{
		SAFEDELETE( m_pStopEffect[2] );
		m_pStopEffect[2] = pImage;
	}
	else if( szType == "StopEffect4" )
	{
		SAFEDELETE( m_pStopEffect[3] );
		m_pStopEffect[3] = pImage;
	}
	else if( szType == "StopEffect5" )
	{
		SAFEDELETE( m_pStopEffect[4] );
		m_pStopEffect[4] = pImage;
	}
	else if( szType == "StopEffect6" )
	{
		SAFEDELETE( m_pStopEffect[5] );
		m_pStopEffect[5] = pImage;
	}
	else if( szType == "StopEffect7" )
	{
		SAFEDELETE( m_pStopEffect[6] );
		m_pStopEffect[6] = pImage;
	}
	else if( szType == "StopEffect8" )
	{
		SAFEDELETE( m_pStopEffect[7] );
		m_pStopEffect[7] = pImage;
	}
	else if( szType == "StopEffect9" )
	{
		SAFEDELETE( m_pStopEffect[8] );
		m_pStopEffect[8] = pImage;
	}
	else if( szType == "StopEffect10" )
	{
		SAFEDELETE( m_pStopEffect[9] );
		m_pStopEffect[9] = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void BingoWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwRollingGapTime = xElement.GetIntAttribute_e( "RollingGapTime" );
	m_szRollingSound   = xElement.GetStringAttribute_e( "RollingSound" );
	m_szStopSound      = xElement.GetStringAttribute_e( "StopSound" );
}

void BingoWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/bingo/bingorightwnd.xml", this );
	g_GUIMgr.AddWnd( "XML/bingo/bingotooltip.xml", this );
}

void BingoWnd::ShowBingoWnd( const ioHashString& szTitle )
{
	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( !pBingo )
		return;

	if( !szTitle.IsEmpty() )
	{
		SetTitleText( szTitle.c_str() );
	}

	m_iBingoType = pBingo->GetBingoType();

	ioBingo::RewardInfoList kRewardInfo;
	pBingo->GetRewardList( kRewardInfo );

	for( int i=0; i<ioBingo::REWARD_MAX_COUNT; ++i )
	{
		int iIndex = ID_REWARD_BTN_1 - ID_REWARD_BTN_1 + i;

		RewardButton *pBtn = dynamic_cast<RewardButton*>( FindChildWnd( ID_REWARD_BTN_1 + i ) );
		if( pBtn )
		{
			pBtn->SetInfo( kRewardInfo[iIndex].m_iIndex, true );
		}
	}

	ioBingo::RewardInfo kAllBingoRewardInfo = pBingo->GetAllBingoRewardInfo();
	RewardButton *pBtn = dynamic_cast<RewardButton*>( FindChildWnd( ID_ALL_BINGO_REWARD_BTN ) );
	if( pBtn )
	{
		pBtn->SetInfo( kAllBingoRewardInfo.m_iIndex, true );
	}

	ioBingo::BingoInfoList kNumberInfo;
	pBingo->GetBingoInfo( kNumberInfo );
	for( int i = 0; i < ioBingo::BINGO_NUMBER_MAX_COUNT; ++i )
	{
		NumberButton *pBtn = dynamic_cast<NumberButton*>( FindChildWnd(ID_NUMBER_BTN_1 + i ) );
		if( pBtn )
		{
			pBtn->SetInfo( kNumberInfo[i].m_iIndex, true );
		}
	}

	BingoRightWnd *pWnd = dynamic_cast<BingoRightWnd*>( FindChildWnd(ID_RIGHT_WND) );
	if( pWnd )
	{
		if( m_iBingoType == ioBingo::BT_FIXED )
		{
			g_GUIMgr.ChangeWndElementValue( "XML/bingo/bingorightwnd.xml", "XML/bingo/changebingorightwnd.xml", "Fixed", this );
		}

		if( !pBingo->IsSettedData() )
		{
			pWnd->SetWindowType( BingoRightWnd::RWT_START );
			pWnd->ShowWnd();
		}
		else if( pBingo->CheckAllBingo() )
		{
			pWnd->SetWindowType( BingoRightWnd::RWT_ALL_BINGO );
			pWnd->ShowWnd();
		}
		else
		{
			if( pBingo->GetBingoType() == ioBingo::BT_FIXED )
			{
				pWnd->SetWindowType( BingoRightWnd::RWT_FIXED_PLAY );
			}
			else
			{
				pWnd->SetWindowType( BingoRightWnd::RWT_RAND_PLAY );
			}
			pWnd->ShowWnd();
		}

		pWnd->SetActvieChildBtn();
	}

	ShowWnd();
}

void BingoWnd::iwm_show()
{
	m_dwRollingCheckTime = 0;
	m_iSelectDummyCode   = 0;
	m_iSelectNumber	     = 0;	
}

void BingoWnd::iwm_hide()
{
	HideChildWnd( ID_RIGHT_WND );

	for( int i = ID_REWARD_BTN_1; i< ID_ALL_BINGO_REWARD_BTN + 1; ++i )
		HideChildWnd( i );

	for( int i = ID_NUMBER_BTN_1; i < ID_NUMBER_BTN_25 + 1; ++i )
		HideChildWnd( i );

	m_State = US_NONE;
	m_dwRollingStartTime = 0;
	m_dwNumberRollingEndTime = 0;
	m_dwRewardRollingEndTime = 0;
}

void BingoWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case MESSAGE_BOX:
		if( param == IDYES )
		{
			SP2Packet kPacket( CTPK_ETCITEM_USE );
			kPacket << m_iSelectDummyCode;
			kPacket << m_iSelectNumber;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );
		}
		break;
	}
	
	if( COMPARE( dwID, ID_NUMBER_BTN_1, ID_NUMBER_BTN_25+1 ) )
	{
		if( cmd == IOBN_BTNUP )
		{
			iwm_bingo_btn( dwID );
		}
	}
}

void BingoWnd::iwm_bingo_btn( DWORD dwID )
{
	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( !pBingo )
		return;

	if( pBingo->GetBingoType() != ioBingo::BT_FIXED )
		return;
	
	if( !pBingo->IsSettedData() )
		return;

	NumberButton* pBtn = dynamic_cast<NumberButton*>( FindChildWnd( dwID ) );
	if( !pBtn )
		return;

	int iNumber = pBtn->GetNumber();
	if( pBingo->IsNumberClear( iNumber ) )
		return;

	DWORD dwEtcCode = pBingo->GetBingoDummyCode( iNumber );
	ioEtcItem* pItem = g_EtcItemMgr.FindEtcItem( dwEtcCode );
	if( !pItem )
		return;

	if( g_MyInfo.GetEtcCoinMoney( dwEtcCode ) > 0 )
	{
		m_iSelectDummyCode = dwEtcCode;
		m_iSelectNumber	   = iNumber;
		g_GUIMgr.SetMsgBox( MB_YESNO, this, "[%s] 아이템을 사용하시겠습니까?", pItem->GetName() );
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, this, "[%s] 아이템을 소지 하지 않았습니다.", pItem->GetName() );
	}
}

void BingoWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	switch( m_State )
	{
	case US_ROLLING:
		{
			if( m_dwRollingCheckTime>0 && m_dwRollingCheckTime <FRAMEGETTIME() )
			{
				m_dwRollingCheckTime = FRAMEGETTIME() + 250;
				g_SoundMgr.PlaySound( m_szRollingSound );
			}

			if( m_dwRollingStartTime>0 && m_dwRollingStartTime < FRAMEGETTIME() )
			{
				m_dwRollingStartTime = 0;
				m_State = US_STOP;

				SetStopNumber();
				SetStopReward();
			}
		}
		break;
	case US_STOP:
		if( m_dwRollingCheckTime>0 && m_dwRollingCheckTime <FRAMEGETTIME() )
		{
			if( IsAllStop() )
			{
				m_dwRollingCheckTime = 0;
			}
			else
			{
				m_dwRollingCheckTime = FRAMEGETTIME() + 250;
				g_SoundMgr.PlaySound( m_szRollingSound );
			}
		}

		DWORD dwEndTime = max( m_dwNumberRollingEndTime, m_dwRewardRollingEndTime );
		if( dwEndTime > 0 && dwEndTime < FRAMEGETTIME() )
		{
			m_dwNumberRollingEndTime = 0;
			m_dwRewardRollingEndTime = 0;
			m_dwRollingCheckTime = 0;

			BingoRightWnd *pWnd = dynamic_cast<BingoRightWnd*>( FindChildWnd(ID_RIGHT_WND) );
			if( pWnd )
			{
				pWnd->SetActvieChildBtn();
			}
		}
		break;
	}
}

void BingoWnd::OnRender()
{
	ioWnd::OnRender();
}

void BingoWnd::SetRollingNumber()
{
	if( m_State != US_ROLLING )
		m_State = US_ROLLING;

	if( m_dwRollingStartTime == 0 )
	{
		m_dwRollingStartTime = FRAMEGETTIME() + 250;
		m_dwRollingCheckTime = FRAMEGETTIME() + 250;
		m_dwNumberRollingEndTime = 0;
	}

	for( int i=0; i<ioBingo::BINGO_NUMBER_MAX_COUNT; ++i )
	{
		NumberButton *pBtn = dynamic_cast<NumberButton*>( FindChildWnd(ID_NUMBER_BTN_1+i) );
		if( pBtn )
		{
			pBtn->SetRolling();
		}
	}
}

void BingoWnd::SetRollingReward()
{
	if( m_State != US_ROLLING )
		m_State = US_ROLLING;

	if( m_dwRollingStartTime == 0 )
	{
		m_dwRollingStartTime = FRAMEGETTIME() + 250;
		m_dwRollingCheckTime = FRAMEGETTIME() + 250;
		m_dwRewardRollingEndTime = 0;
	}

	for( int i = 0; i < ioBingo::REWARD_MAX_COUNT + 1; ++i )
	{
		RewardButton *pBtn = dynamic_cast<RewardButton*>( FindChildWnd( ID_REWARD_BTN_1 + i ) );
		if( pBtn )
		{
			pBtn->SetRolling();
		}
	}
}

void BingoWnd::SetStopNumber()
{
	int iStartIndex = 0;
	int iCnt = 1;
	bool bReverse = false;
	int iTime = 2;
	while( iCnt > 0 )
	{
		for( int j=0; j<iCnt; ++j )
		{
			int iIndex = iStartIndex + j*4;
			NumberButton *pBtn = dynamic_cast<NumberButton*>( FindChildWnd(ID_NUMBER_BTN_1+iIndex) );
			if( pBtn )
			{
				pBtn->SetStop( ( m_dwRollingGapTime * 2 ) * iTime );

				if( pBtn->GetID() == ID_NUMBER_BTN_25 )
				{
					m_dwNumberRollingEndTime += ( m_dwRollingGapTime * 2 ) * iTime + 500;
				}
			}
			else
			{
				m_dwNumberRollingEndTime += 1500;
			}
		}

		if( iCnt >= 5 )
			bReverse = true;

		if( !bReverse )
		{
			iStartIndex++;
			iCnt++;
		}
		else
		{
			iStartIndex += 5;
			iCnt--;
		}

		iTime++;
	}

	m_dwNumberRollingEndTime += FRAMEGETTIME();
}

void BingoWnd::SetStopReward()
{
	for( int i=0; i<ioBingo::REWARD_MAX_COUNT+1; ++i )
	{
		RewardButton *pBtn = dynamic_cast<RewardButton*>( FindChildWnd(ID_REWARD_BTN_1+i) );
		if( pBtn )
		{
			pBtn->SetStop( ( m_dwRollingGapTime * 2 ) * i );

			if( pBtn->GetID() == ID_REWARD_BTN_12 )
			{
				m_dwRewardRollingEndTime += ( m_dwRollingGapTime * 2 ) * i + 500;
			}
		}
		else
		{
			m_dwRewardRollingEndTime += 1500;
		}
	}

	m_dwRewardRollingEndTime += FRAMEGETTIME();
}

void BingoWnd::SetNumber( bool bCheckClear )
{
	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		ioBingo::BingoInfoList kNumberInfo;
		pBingo->GetBingoInfo( kNumberInfo );
		for( int i=0; i<ioBingo::BINGO_NUMBER_MAX_COUNT; ++i )
		{
			int iIndex = ID_NUMBER_BTN_1 - ID_NUMBER_BTN_1 + i;

			NumberButton *pBtn = dynamic_cast<NumberButton*>( FindChildWnd(ID_NUMBER_BTN_1+i) );
			if( pBtn )
			{
				pBtn->SetInfo( kNumberInfo[iIndex].m_iIndex, bCheckClear );
			}
		}
	}
}

void BingoWnd::SetReward(bool bCheckClear)
{
	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		ioBingo::RewardInfoList kRewardInfo;
		pBingo->GetRewardList( kRewardInfo );
		for( int i=0; i<ioBingo::REWARD_MAX_COUNT; ++i )
		{
			int iIndex = ID_REWARD_BTN_1 - ID_REWARD_BTN_1 + i;

			RewardButton *pBtn = dynamic_cast<RewardButton*>( FindChildWnd( ID_REWARD_BTN_1 + i ) );
			if( pBtn )
			{
				pBtn->SetInfo( kRewardInfo[iIndex].m_iIndex, bCheckClear );
			}
		}

		ioBingo::RewardInfo kAllBingoRewardInfo = pBingo->GetAllBingoRewardInfo();
		RewardButton *pBtn = dynamic_cast<RewardButton*>( FindChildWnd( ID_ALL_BINGO_REWARD_BTN ) );
		if( pBtn )
		{
			pBtn->SetInfo( kAllBingoRewardInfo.m_iIndex, bCheckClear );
		}
	}
}

void BingoWnd::SetBingo( int iLineIndex )
{
	switch( iLineIndex )
	{
	case ioBingo::BINGO_LINE_LEFT_TOP:
		{
			for( int i=0; i<5; ++i )
			{
				int iArray = i * 6;

				NumberButton *pBtn = dynamic_cast<NumberButton*>( FindChildWnd(ID_NUMBER_BTN_1+iArray) );
				if( pBtn )
					pBtn->SetClear();
			}
		}
		break;
	case ioBingo::BINGO_LINE_LEFT_BOTTOM:
		{
			for( int i=0; i<5; ++i )
			{
				int iArray = 4 + i * 4;

				NumberButton *pBtn = dynamic_cast<NumberButton*>( FindChildWnd(ID_NUMBER_BTN_1+iArray) );
				if( pBtn )
					pBtn->SetClear();
			}
		}
		break;
	case ioBingo::BINGO_LINE_HORZ1:
	case ioBingo::BINGO_LINE_HORZ2:
	case ioBingo::BINGO_LINE_HORZ3:
	case ioBingo::BINGO_LINE_HORZ4:
	case ioBingo::BINGO_LINE_HORZ5:
		{
			for( int i=0; i<5; ++i )
			{
				int iIndex = iLineIndex - ioBingo::BINGO_LINE_HORZ1;
				int iArray = iIndex * 5 + i;

				NumberButton *pBtn = dynamic_cast<NumberButton*>( FindChildWnd(ID_NUMBER_BTN_1+iArray) );
				if( pBtn )
					pBtn->SetClear();
			}
		}
		break;
	case ioBingo::BINGO_LINE_VERT1:
	case ioBingo::BINGO_LINE_VERT2:
	case ioBingo::BINGO_LINE_VERT3:
	case ioBingo::BINGO_LINE_VERT4:
	case ioBingo::BINGO_LINE_VERT5:
		{
			for( int i=0; i<5; ++i )
			{
				int iIndex = iLineIndex - ioBingo::BINGO_LINE_VERT1;
				int iArray = iIndex + i * 5;

				NumberButton *pBtn = dynamic_cast<NumberButton*>( FindChildWnd(ID_NUMBER_BTN_1+iArray) );
				if( pBtn )
					pBtn->SetClear();
			}
		}
		break;
	}
}

void BingoWnd::GetEffectList( OUT ioUIRenderImageList &rkImageList )
{
	rkImageList.clear();
	rkImageList.reserve( 10 );

	for( int i=0; i<10; ++i )
	{
		if( m_pStopEffect[i] )
			rkImageList.push_back( m_pStopEffect[i] );
	}
}

bool BingoWnd::IsAllStop()
{
	for( int i=ID_NUMBER_BTN_1; i<ID_NUMBER_BTN_25+1; ++i )
	{
		NumberButton *pBtn = dynamic_cast<NumberButton*>( FindChildWnd(i) );
		if( pBtn && pBtn->IsRolling() )
			return false;
	}

	for( int i=ID_REWARD_BTN_1; i<ID_REWARD_BTN_12+2; ++i )
	{
		RewardButton *pBtn = dynamic_cast<RewardButton*>( FindChildWnd(i) );
		if( pBtn && pBtn->IsRolling() )
			return false;
	}

	return true;
}

bool BingoWnd::IsNumberBingoHorz( int iIndex )
{
	// 가로 다섯줄 체크
	for( int i=0; i<5; ++i )
	{
		NumberButton *pBtn1 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + i*5 ) );
		NumberButton *pBtn2 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + i*5+1 ) );
		NumberButton *pBtn3 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + i*5+2 ) );
		NumberButton *pBtn4 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + i*5+3 ) );
		NumberButton *pBtn5 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + i*5+4 ) );

		if( pBtn1 && pBtn2 && pBtn3 && pBtn4 && pBtn5 &&
			pBtn1->IsClear() &&
			pBtn2->IsClear() &&
			pBtn3->IsClear() &&
			pBtn4->IsClear() &&
			pBtn5->IsClear() )
		{
			if( pBtn1->GetNumber() == iIndex )	return true;
			if( pBtn2->GetNumber() == iIndex )	return true;
			if( pBtn3->GetNumber() == iIndex )	return true;
			if( pBtn4->GetNumber() == iIndex )	return true;
			if( pBtn5->GetNumber() == iIndex )	return true;
		}
	}

	return false;
}

bool BingoWnd::IsNumberBingoVert( int iIndex )
{
	// 세로 다섯줄 체크
	for( int i=0; i<5; ++i )
	{
		NumberButton *pBtn1 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + i ) );
		NumberButton *pBtn2 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + i + 5) );
		NumberButton *pBtn3 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + i + 10 ) );
		NumberButton *pBtn4 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + i + 15 ) );
		NumberButton *pBtn5 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + i + 20 ) );

		if( pBtn1 && pBtn2 && pBtn3 && pBtn4 && pBtn5 &&
			pBtn1->IsClear() &&
			pBtn2->IsClear() &&
			pBtn3->IsClear() &&
			pBtn4->IsClear() &&
			pBtn5->IsClear() )
		{
			if( pBtn1->GetNumber() == iIndex )	return true;
			if( pBtn2->GetNumber() == iIndex )	return true;
			if( pBtn3->GetNumber() == iIndex )	return true;
			if( pBtn4->GetNumber() == iIndex )	return true;
			if( pBtn5->GetNumber() == iIndex )	return true;
		}
	}

	return false;
}

bool BingoWnd::IsNumberBingoLT( int iIndex )
{
	NumberButton *pBtn1 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 ) );
	NumberButton *pBtn2 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + 6 ) );
	NumberButton *pBtn3 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + 12 ) );
	NumberButton *pBtn4 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + 18 ) );
	NumberButton *pBtn5 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + 24 ) );

	if( pBtn1 && pBtn2 && pBtn3 && pBtn4 && pBtn5 &&
		pBtn1->IsClear() &&
		pBtn2->IsClear() &&
		pBtn3->IsClear() &&
		pBtn4->IsClear() &&
		pBtn5->IsClear() )
	{
		if( pBtn1->GetNumber() == iIndex )	return true;
		if( pBtn2->GetNumber() == iIndex )	return true;
		if( pBtn3->GetNumber() == iIndex )	return true;
		if( pBtn4->GetNumber() == iIndex )	return true;
		if( pBtn5->GetNumber() == iIndex )	return true;
	}

	return false;
}

bool BingoWnd::IsNumberBingoRB( int iIndex )
{
	NumberButton *pBtn1 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + 4) );
	NumberButton *pBtn2 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + 8 ) );
	NumberButton *pBtn3 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + 12 ) );
	NumberButton *pBtn4 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + 16 ) );
	NumberButton *pBtn5 = dynamic_cast<NumberButton*>( FindChildWnd( ID_NUMBER_BTN_1 + 20 ) );

	if( pBtn1 && pBtn2 && pBtn3 && pBtn4 && pBtn5 &&
		pBtn1->IsClear() &&
		pBtn2->IsClear() &&
		pBtn3->IsClear() &&
		pBtn4->IsClear() &&
		pBtn5->IsClear() )
	{
		if( pBtn1->GetNumber() == iIndex )	return true;
		if( pBtn2->GetNumber() == iIndex )	return true;
		if( pBtn3->GetNumber() == iIndex )	return true;
		if( pBtn4->GetNumber() == iIndex )	return true;
		if( pBtn5->GetNumber() == iIndex )	return true;
	}

	return false;
}

void BingoWnd::ReloadBtnEffect()
{
	//용준
	for( int i = 0; i < ioBingo::BINGO_NUMBER_MAX_COUNT; ++i )
	{
		NumberButton *pBtn = dynamic_cast<NumberButton*>( FindChildWnd(ID_NUMBER_BTN_1 + i ) );
		if( pBtn )
		{
			pBtn->ReLoadEffect();
		}
	}
	for( int i=0; i<ioBingo::REWARD_MAX_COUNT; ++i )
	{
		RewardButton *pBtn = dynamic_cast<RewardButton*>( FindChildWnd( ID_REWARD_BTN_1 + i ) );
		if( pBtn )
		{
			pBtn->ReLoadEffect();
		}
	}

	RewardButton *pBtn = dynamic_cast<RewardButton*>( FindChildWnd( ID_ALL_BINGO_REWARD_BTN ) );
	if( pBtn )
		pBtn->ReLoadEffect();
}

//////////////////////////////////////////////////////////////////////////
RewardButton::RewardButton()
{
	m_pIcon = NULL;
	m_pHiddenIcon = NULL;
	m_pIconBg = NULL;
	m_pButtonBg = NULL;
	m_pGettedItem = NULL;

	m_State = US_NONE;
	m_dwRollingCheckTime = 0;
	m_dwRollingGapTime = 0;
	m_dwRollingStopTime = 0;
	m_OpenEffect.Init();
	m_GetEffect.Init();

	m_bClear = false;

	m_dwCheckTime = 0;
	m_bLightIcon = false;
}

RewardButton::~RewardButton()
{
	SAFEDELETE( m_pIconBg );
	SAFEDELETE( m_pHiddenIcon );
	SAFEDELETE( m_pButtonBg );
	SAFEDELETE( m_pGettedItem );
	
	m_OpenEffect.Init();
	m_GetEffect.Init();
}

void RewardButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ButtonBg" )
	{
		SAFEDELETE( m_pButtonBg );
		m_pButtonBg = pImage;
	}
	else if( szType == "IconBg" )
	{
		SAFEDELETE( m_pIconBg );
		m_pIconBg = pImage;
	}
	else if( szType == "GetItem" )
	{
		SAFEDELETE( m_pGettedItem );
		m_pGettedItem = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void RewardButton::iwm_create()
{
}


void RewardButton::ReLoadEffect()
{
	BingoWnd *pWnd = dynamic_cast<BingoWnd*>( GetParent() );
	if( pWnd )
	{
		m_dwRollingGapTime = pWnd->GetRollingGapTime();

		ioUIRenderImageList vImageList;
		pWnd->GetEffectList( vImageList );
		m_OpenEffect.Init();
		m_OpenEffect.DestroySpriteList();
		m_OpenEffect.AddSpriteList( vImageList );
		m_OpenEffect.SetType( ioSprite::AT_PLAY_ONCE );
		m_OpenEffect.SetAnimationGapTime( m_dwRollingGapTime );

		vImageList.clear();
		vImageList.push_back( m_pGettedItem );
		m_GetEffect.Init();
		m_GetEffect.DestroySpriteList();
		m_GetEffect.AddSpriteList( vImageList );
		m_GetEffect.SetType( ioSprite::AT_LOOP );
		m_GetEffect.SetAnimationDuration( 100 );
		m_GetEffect.SetAlphaAnimation( MAX_ALPHA_RATE, 0 );
		m_GetEffect.SetScaleAnimation( 3.0f, FLOAT1 );
	}
}


void RewardButton::iwm_show()
{
	m_dwCheckTime = 0;
	m_bLightIcon = false;

	ReLoadEffect();

	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		m_pHiddenIcon = g_UIImageSetMgr.CreateImageByFullName( pBingo->GetHiddenIconName() );
	}
}

void RewardButton::iwm_hide()
{
	m_State = US_NONE;
	m_dwRollingCheckTime = 0;
	m_dwRollingStopTime = 0;

	m_OpenEffect.Init();
	m_GetEffect.Init();
}

void RewardButton::iwm_mouseover( const ioMouse& mouse )
{
	if( m_State == US_ROLLING )
		return;

	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo && !pBingo->IsSettedData() )
		return;

	ioWnd *pWnd = GetParent();
	if( pWnd )
	{
		BingoToolTip *pChild = dynamic_cast<BingoToolTip*>( pWnd->FindChildWnd(BingoWnd::ID_TOOLTIP) );
		if( pChild )
		{
			if( m_dwID == BingoWnd::ID_ALL_BINGO_REWARD_BTN )
			{
				ioBingo::RewardInfo kInfo = pBingo->GetAllBingoRewardInfo( m_iRollingIndex );
				pChild->SetInfo( kInfo.m_iIndex, kInfo.m_iManualIndex, BingoToolTip::TTT_ALL_BINGO_REWARD );
			}
			else
			{
				ioBingo::RewardInfo kInfo = pBingo->GetRewardInfo( m_iRollingIndex );
				pChild->SetInfo( kInfo.m_iIndex, kInfo.m_iManualIndex, BingoToolTip::TTT_REWARD );
			}
		}
	}
}

void RewardButton::iwm_mouseleave( const ioMouse& mouse )
{
	ioWnd *pWnd = GetParent();
	if( pWnd )
	{
		pWnd->HideChildWnd( BingoWnd::ID_TOOLTIP );
	}
}

void RewardButton::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( !pBingo )
		return;

	switch( m_State )
	{
	case US_ROLLING:
		{
			if( m_dwRollingCheckTime < FRAMEGETTIME() )
			{
				IORandom random;
				random.SetRandomSeed( REALGETTIME() + m_dwID );
				m_iRollingIndex = random.Random( 12 ) + 1;

				if( m_dwID == BingoWnd::ID_ALL_BINGO_REWARD_BTN )
				{
					int iSize = pBingo->GetAllBingoRewardListCount();
					m_iRollingIndex = random.Random( iSize ) + 1;
					m_pIcon = pBingo->GetAllBingoRewardIcon( m_iRollingIndex );
				}
				else
				{
					int iSize = pBingo->GetRewardCount();
					m_iRollingIndex = random.Random( iSize ) + 1;
					m_pIcon = pBingo->GetRewardIcon( m_iRollingIndex );
				}

				m_dwRollingCheckTime = FRAMEGETTIME() + m_dwRollingGapTime;
			}

			if( m_dwRollingStopTime > 0 && m_dwRollingStopTime < FRAMEGETTIME() )
			{
				BingoWnd *pWnd = dynamic_cast<BingoWnd*>( GetParent() );
				if( pWnd )
					g_SoundMgr.PlaySound( pWnd->GetStopSound() );

				m_dwRollingStopTime = 0;
				m_State = US_STOP;

				m_OpenEffect.SetPlay();

				if( m_dwID == BingoWnd::ID_ALL_BINGO_REWARD_BTN )
				{
					m_iRollingIndex = m_iRewardIndex;
					m_pIcon = pBingo->GetAllBingoRewardIcon( m_iRollingIndex );
				}
				else
				{
					m_iRollingIndex = m_iRewardIndex;
					m_pIcon = pBingo->GetRewardIcon( m_iRollingIndex );
				}
			}
		}
		break;
	case US_STOP:
		break;
	}

	m_OpenEffect.Process();
	m_GetEffect.Process();
}

void RewardButton::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum { GAP_X = 26, GAP_Y = 26 };

	if( m_pButtonBg )
		m_pButtonBg->Render( iXPos, iYPos );

	if( m_pIconBg )
	{
		m_pIconBg->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
		m_pIconBg->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		m_pIconBg->SetScale( 0.53f );
		m_pIconBg->Render( iXPos + 27, iYPos + 27, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}

	bool bHidden = false;
	bool bClear = false;
	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( !pBingo )
		return;

	if( !m_pHiddenIcon || !m_pIcon )
		return;

	if( pBingo->IsRewardHidden(m_iRollingIndex) )
		bHidden = true;

	if( m_State == US_ROLLING )
	{
		if( m_dwID == BingoWnd::ID_ALL_BINGO_REWARD_BTN )
		{
			ioBingo::RewardInfo kInfo = pBingo->GetAllBingoRewardInfo();
			bClear = kInfo.m_bClear;
		}
		else
		{
			if( pBingo->IsRewardClear(m_iRollingIndex) )
				bClear = true;
		}
	}
	else
		bClear = m_bClear;

	if( m_dwID == BingoWnd::ID_ALL_BINGO_REWARD_BTN )
	{
		if( !pBingo->IsSettedData() )
		{
			m_pHiddenIcon->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pHiddenIcon->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pHiddenIcon->SetScale( 0.53f );
			m_pHiddenIcon->Render( iXPos + GAP_X, iYPos + GAP_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else
		{
			m_pIcon->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pIcon->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pIcon->SetScale( 0.53f );
			m_pIcon->Render( iXPos + 27, iYPos + 25, UI_RENDER_NORMAL, TFO_BILINEAR );
			
			if( pBingo->CheckAllBingo() )
			{
				if( m_pGettedItem )
					m_pGettedItem->Render( iXPos + GAP_X, iYPos + GAP_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
		}
	}
	else
	{
		if( bHidden )
		{
			m_pHiddenIcon->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pHiddenIcon->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pHiddenIcon->SetScale( 0.53f );
			m_pHiddenIcon->Render( iXPos + GAP_X, iYPos + GAP_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else
		{
			m_pIcon->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pIcon->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pIcon->SetScale( 0.53f );
			m_pIcon->Render( iXPos + GAP_X, iYPos + GAP_Y, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
	

	if( bClear && m_pGettedItem )
	{
		m_pGettedItem->SetAlpha( MAX_ALPHA_RATE );
		m_pGettedItem->SetScale( FLOAT1 );
		m_pGettedItem->Render( iXPos + GAP_X, iYPos + GAP_Y, UI_RENDER_NORMAL, TFO_BILINEAR );

		if( m_dwCheckTime < FRAMEGETTIME() )
		{
			m_bLightIcon ^= true;
			m_dwCheckTime = FRAMEGETTIME() + 500;
		}

		if( m_bLightIcon )
			m_pGettedItem->Render( iXPos + GAP_X, iYPos + GAP_Y, UI_RENDER_SCREEN, TFO_BILINEAR );
	}

	m_OpenEffect.Render( iXPos + GAP_X, iYPos + GAP_Y, 1.06f, UI_RENDER_ADD );
	m_GetEffect.Render( iXPos + GAP_X, iYPos + GAP_Y, FLOAT1, UI_RENDER_NORMAL, TFO_BILINEAR );
}

void RewardButton::SetInfo( int iIndex, bool bCheckClear )
{
	if( !IsShow() )
		ShowWnd();

	m_iRewardIndex = m_iRollingIndex = iIndex;

	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( !pBingo )
		return;

	if( m_dwID == BingoWnd::ID_ALL_BINGO_REWARD_BTN )
	{
		m_pIcon = pBingo->GetAllBingoRewardIcon();

		if( bCheckClear )
		{
			if( pBingo->CheckAllBingo() )
				m_bClear = true;
			else
				m_bClear = false;
		}
	}
	else
	{
		m_pIcon = pBingo->GetRewardIcon( iIndex );

		if( bCheckClear )
			m_bClear = pBingo->IsRewardClear( iIndex );
	}
}

void RewardButton::SetClear()
{
	if( m_bClear )
		return;

	m_bClear = true;

	if( m_GetEffect.GetState() == ioSprite::SS_PLAY )
		return;

	m_GetEffect.SetPlay();

	m_dwCheckTime = FRAMEGETTIME() + 500;
}

void RewardButton::SetRolling()
{
	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo && pBingo->IsRewardClear(m_iRewardIndex) )
		return;

	m_State = US_ROLLING;
	m_dwRollingCheckTime = 0;
	m_dwRollingStopTime  = 0;
	m_OpenEffect.Init();
}

void RewardButton::SetStop( DWORD dwStopTime )
{
	m_dwRollingStopTime = FRAMEGETTIME() + dwStopTime;
}

bool RewardButton::IsStop()
{
	if( m_State == US_STOP )
		return true;

	return false;
}

bool RewardButton::IsRolling()
{
	if( m_State == US_ROLLING )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////
NumberButton::NumberButton()
{
	m_pButtonBg = NULL;
	m_pButtonBgLight = NULL;
	m_pButtonBgClear = NULL;
	m_pButtonBgClearTop = NULL;
	m_pButtonBgClearLeft = NULL;
	m_pButtonBgClearLeftTop = NULL;
	m_pButtonBingoEffect = NULL;
	m_pHasDummy	= NULL;

	m_State = US_NONE;
	m_dwRollingCheckTime = 0;
	m_dwRollingGapTime = 0;
	m_dwRollingStopTime = 0;

	m_OpenEffect.Init();
	m_BingoEffect.Init();

	m_bClear = false;

	m_dwCheckTime = 0;
	m_bLightIcon  = false;
	m_bHasDummy   = false;

	m_pCheckIconImg   = NULL;
	m_pUnCheckIconImg = NULL;
}

NumberButton::~NumberButton()
{
	SAFEDELETE( m_pButtonBg );
	SAFEDELETE( m_pButtonBgLight );
	SAFEDELETE( m_pButtonBgClear );
	SAFEDELETE( m_pButtonBgClearTop );
	SAFEDELETE( m_pButtonBgClearLeft );
	SAFEDELETE( m_pButtonBgClearLeftTop );
	SAFEDELETE( m_pButtonBingoEffect );
	SAFEDELETE( m_pHasDummy );

	m_OpenEffect.Init();
	m_BingoEffect.Init();
}

void NumberButton::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ButtonBg" )
	{
		SAFEDELETE( m_pButtonBg );
		m_pButtonBg = pImage;
	}
	else if( szType == "ButtonBgLight" )
	{
		SAFEDELETE( m_pButtonBgLight );
		m_pButtonBgLight = pImage;
	}
	else if( szType == "ButtonBgRed" )
	{
		SAFEDELETE( m_pButtonBgClear );
		m_pButtonBgClear = pImage;
	}
	else if( szType == "ButtonBgRedLeft" )
	{
		SAFEDELETE( m_pButtonBgClearLeft );
		m_pButtonBgClearLeft = pImage;
	}
	else if( szType == "ButtonBgRedTop" )
	{
		SAFEDELETE( m_pButtonBgClearTop );
		m_pButtonBgClearTop = pImage;
	}
	else if( szType == "ButtonBgRedLeftTop" )
	{
		SAFEDELETE( m_pButtonBgClearLeftTop );
		m_pButtonBgClearLeftTop = pImage;
	}
	else if( szType == "ButtonBingoEffect" )
	{
		SAFEDELETE( m_pButtonBingoEffect );
		m_pButtonBingoEffect = pImage;
	}
	else if( szType == "HasDummy" )
	{
		SAFEDELETE( m_pHasDummy );
		m_pHasDummy = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void NumberButton::iwm_create()
{
}


void NumberButton::ReLoadEffect()
{
	BingoWnd *pWnd = dynamic_cast<BingoWnd*>( GetParent() );
	if( pWnd )
	{
		m_dwRollingGapTime = pWnd->GetRollingGapTime();

		ioUIRenderImageList vImageList;
		pWnd->GetEffectList( vImageList );
		m_OpenEffect.Init();
		m_OpenEffect.DestroySpriteList();
		m_OpenEffect.AddSpriteList( vImageList );
		m_OpenEffect.SetType( ioSprite::AT_PLAY_ONCE );
		m_OpenEffect.SetAnimationGapTime( m_dwRollingGapTime );

		vImageList.clear();
		vImageList.push_back( m_pButtonBingoEffect );
		m_BingoEffect.Init();
		m_BingoEffect.DestroySpriteList();
		m_BingoEffect.AddSpriteList( vImageList );
		m_BingoEffect.SetType( ioSprite::AT_LOOP );
		m_BingoEffect.SetAnimationDuration( 500 );
	}
}

void NumberButton::iwm_show()
{
	m_dwCheckTime = 0;
	m_bLightIcon  = false;
	m_bHasDummy   = false;

	ReLoadEffect();
}

void NumberButton::iwm_hide()
{
	m_State              = US_NONE;
	m_dwRollingCheckTime = 0;
	m_dwRollingStopTime  = 0;

	m_OpenEffect.Init();
	m_BingoEffect.Init();
}

void NumberButton::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( !pBingo )
		return;

	switch( m_State )
	{
	case US_ROLLING:
		{
			if( m_dwRollingCheckTime < FRAMEGETTIME() )
			{
				IORandom random;
				random.SetRandomSeed( REALGETTIME() + m_dwID );

				int iSize = pBingo->GetMaxNumber();
				int iRand = random.Random( iSize );
				m_iRollingNumber = iRand + 1;
				m_dwRollingCheckTime = FRAMEGETTIME() + m_dwRollingGapTime;
			}

			if( m_dwRollingStopTime > 0 && m_dwRollingStopTime < FRAMEGETTIME() )
			{
				BingoWnd *pWnd = dynamic_cast<BingoWnd*>( GetParent() );
				if( pWnd )
					g_SoundMgr.PlaySound( pWnd->GetStopSound() );

				m_dwRollingStopTime = 0;
				m_State = US_STOP;

				m_OpenEffect.SetPlay();
				m_iRollingNumber = m_iNumber;

				m_bClear = pBingo->IsNumberClear( m_iRollingNumber );
				if( m_bClear )
					pBingo->CheckBingoLine( m_iRollingNumber );				
			}
		}
		break;
	case US_NONE:
	case US_STOP:
		{
			if( pBingo->HasBingoDummy( m_iNumber ) && pBingo->IsSettedData() )
			{
				m_bHasDummy = true;

				if( m_dwCheckTime < FRAMEGETTIME() )
				{
					m_bLightIcon ^= true;
					m_dwCheckTime = FRAMEGETTIME() + 500;
				}
			}
			else
			{
				m_bHasDummy  = false;
				m_bLightIcon = false;
			}
		}
		break;
	}

	m_OpenEffect.Process();
	m_BingoEffect.Process();
}

void NumberButton::OnRender()
{
	ioButton::OnRender();

	switch( m_iBingoType )
	{
	case ioBingo::BT_RAND:		
		OnRenderByRandType();
		break;
	case ioBingo::BT_FIXED:
		OnRenderByFixedType();
		break;
	}
}

void NumberButton::OnRenderByRandType()
{
	if( !m_pButtonBg || !m_pButtonBgLight || !m_pButtonBgClear || !m_pButtonBgClearTop || !m_pButtonBgClearLeft || !m_pButtonBgClearLeftTop )
		return;

	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( !pBingo )
		return;

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum { GAP_X = 26, GAP_Y = 26, TEXT_GAP_X = 26, TEXT_GAP_Y = 15 };

	bool bClear = false;

	if( m_State == US_ROLLING )
	{
		if( pBingo->IsNumberClear( m_iRollingNumber ) )
			bClear = true;
	}
	else
		bClear = m_bClear;

	if( bClear )
	{
		m_pButtonBgClear->Render( iXPos, iYPos );

		BingoWnd *pParent = dynamic_cast<BingoWnd*>( GetParent() );
		if( pParent && m_State != US_ROLLING )
		{
			if( pParent->IsNumberBingoHorz( m_iNumber ) )
			{
				m_pButtonBgClearLeft->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
				m_pButtonBgClearLeft->Render( GetDerivedPosX() + 1, GetDerivedPosY() + 18 );
				m_pButtonBgClearLeft->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
				m_pButtonBgClearLeft->Render( GetDerivedPosX() + 35, GetDerivedPosY() + 18 );
			}

			if( pParent->IsNumberBingoVert( m_iNumber ) )
			{
				m_pButtonBgClearTop->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
				m_pButtonBgClearTop->Render( GetDerivedPosX() + 18, GetDerivedPosY() + 1 );
				m_pButtonBgClearTop->SetReverseFormatting( ioUIRenderImage::RF_VERT_REVERSE );
				m_pButtonBgClearTop->Render( GetDerivedPosX() + 18, GetDerivedPosY() + 35 );
			}

			if( pParent->IsNumberBingoLT( m_iNumber ) )
			{
				m_pButtonBgClearLeftTop->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
				m_pButtonBgClearLeftTop->Render( GetDerivedPosX() + 1, GetDerivedPosY() + 1 );
				m_pButtonBgClearLeftTop->SetReverseFormatting( ioUIRenderImage::RF_HORZ_VERT_REVERSE );
				m_pButtonBgClearLeftTop->Render( GetDerivedPosX() + 35, GetDerivedPosY() + 35 );
			}

			if( pParent->IsNumberBingoRB( m_iNumber ) )
			{
				m_pButtonBgClearLeftTop->SetReverseFormatting( ioUIRenderImage::RF_VERT_REVERSE );
				m_pButtonBgClearLeftTop->Render( GetDerivedPosX() + 1, GetDerivedPosY() + 35 );
				m_pButtonBgClearLeftTop->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
				m_pButtonBgClearLeftTop->Render( GetDerivedPosX() + 35, GetDerivedPosY() + 1 );
			}
		}
		
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( 255, 150, 128 );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.PrintText( iXPos + TEXT_GAP_X, iYPos + TEXT_GAP_Y, FONT_SIZE_20, "%d", m_iRollingNumber );		
	}
	else
	{
		m_pButtonBg->Render( iXPos, iYPos );
		
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetTextColor( 200, 213, 242 );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.PrintText( iXPos + TEXT_GAP_X, iYPos + TEXT_GAP_Y, FONT_SIZE_20, "%d", m_iRollingNumber );
	}

	m_OpenEffect.Render( iXPos + GAP_X, iYPos + GAP_Y, 1.06f, UI_RENDER_ADD );
	m_BingoEffect.Render( iXPos + GAP_X, iYPos + GAP_Y );

}

void NumberButton::OnRenderByFixedType()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	enum { GAP_X = 26, GAP_Y = 26, TEXT_GAP_X = 26, TEXT_GAP_Y = 15 };

	bool bClear = false;

	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( !pBingo )
		return;

	if( m_State == US_ROLLING && pBingo->IsNumberClear( m_iRollingNumber ) )
	{	
		bClear = true;
	}
	else
		bClear = m_bClear;
	
	if( bClear )
	{
		if( m_pCheckIconImg )
			m_pCheckIconImg->Render( iXPos, iYPos );
	}
	else
	{
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_OUTLINE );

		if( m_pHasDummy && m_bHasDummy )
		{			
			m_pHasDummy->Render( iXPos, iYPos );

			if( m_bLightIcon )						
			{
				m_pHasDummy->Render( iXPos, iYPos, UI_RENDER_SCREEN );
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
				g_FontMgr.SetBkColor( 180, 91, 208 );
			}
			else
			{
				g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
				g_FontMgr.SetBkColor( 152, 53, 190 );
			}

			g_FontMgr.PrintText( iXPos + TEXT_GAP_X, iYPos + TEXT_GAP_Y, FONT_SIZE_20, "%d", m_iRollingNumber );
		}
		else
		{
			if( m_pUnCheckIconImg )
				m_pUnCheckIconImg->Render( iXPos, iYPos );
						
			g_FontMgr.SetTextColor( 194, 194, 194 );
			g_FontMgr.SetBkColor( 0, 0, 0 );
			g_FontMgr.PrintText( iXPos + TEXT_GAP_X, iYPos + TEXT_GAP_Y, FONT_SIZE_20, "%d", m_iRollingNumber );
		}
	}

	ioButton::OnRender();

	m_OpenEffect.Render( iXPos + GAP_X, iYPos + GAP_Y, 1.06f, UI_RENDER_ADD );
	m_BingoEffect.Render( iXPos + GAP_X, iYPos + GAP_Y );
}

void NumberButton::SetInfo( int iNumber, bool bCheckClear )
{
	if( !IsShow() )
		ShowWnd();

	m_iBingoType = 0;
	m_iNumber = m_iRollingNumber = iNumber;
	
	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( !pBingo )
		return;

	m_iBingoType = pBingo->GetBingoType();

	if( bCheckClear )
		m_bClear = pBingo->IsNumberClear( iNumber );
	
	if( m_iBingoType == ioBingo::BT_FIXED )
	{		
		m_pUnCheckIconImg = pBingo->GetBingoDummyUnCheckIcon( iNumber );
		m_pCheckIconImg   = pBingo->GetBingoDummyCheckIcon( iNumber );
	}
}

void NumberButton::SetClear()
{
	m_bClear = true;

	if( m_BingoEffect.GetState() == ioSprite::SS_PLAY )
		return;

	m_BingoEffect.SetPlay();
}

void NumberButton::SetRolling()
{
	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo && pBingo->IsNumberBingo( m_iRollingNumber ) )
	{
		m_bClear = pBingo->IsNumberClear( m_iRollingNumber );
		return;
	}

	m_State = US_ROLLING;
	m_dwRollingCheckTime = 0;
	m_dwRollingStopTime  = 0;
	m_OpenEffect.Init();
}

void NumberButton::SetStop( DWORD dwStopTime )
{
	m_dwRollingStopTime = FRAMEGETTIME() + dwStopTime;
}

bool NumberButton::IsRolling()
{
	if( m_State == US_ROLLING )
		return true;

	return false;
}

void NumberButton::iwm_mouseover( const ioMouse& mouse )
{
	if( m_iBingoType != ioBingo::BT_FIXED )
		return;

	ioButton::iwm_mouseover( mouse );

	if( m_State == US_ROLLING )
		return;

	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( !pBingo )
		return;

	if( !pBingo->IsSettedData() )
		return;

	if( pBingo->IsNumberClear( m_iNumber ) )
		return;

	ioWnd *pWnd = GetParent();
	if( pWnd )
	{
		BingoToolTip *pToolTip = dynamic_cast<BingoToolTip*>( pWnd->FindChildWnd( BingoWnd::ID_TOOLTIP ) );
		if( pToolTip )
		{
			pToolTip->SetInfo( GetNumber(), 0, BingoToolTip::TTT_DUMMY );
		}
	}
}

void NumberButton::iwm_mouseleave( const ioMouse& mouse )
{
	if( m_iBingoType != ioBingo::BT_FIXED )
		return;

	ioButton::iwm_mouseleave( mouse );

	ioWnd *pWnd = GetParent();
	if( pWnd )
	{
		pWnd->HideChildWnd( BingoWnd::ID_TOOLTIP );
	}
}