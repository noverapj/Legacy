#include "StdAfx.h"

#include "../ioDateHelp.h"
#include "EventWnd.h"

//----------------------------------------------------------------------------------------------------------------------------------
// EVT_EXERCISESOLDIER
ExerciseSoldierEventWnd::ExerciseSoldierEventWnd()
{
	m_iCurPos			 = 0;
	m_iMaxPage			 = 0;
	m_iCurBtnSize        = 0;
	m_pPlayStage         = NULL;
	m_vItemInfoList.reserve(10);
	m_vExtraClassVec.clear();
	m_pLeftFrm           = NULL;
}

ExerciseSoldierEventWnd::~ExerciseSoldierEventWnd()
{
	m_vItemInfoList.clear();
	m_vExtraClassVec.clear();
	SAFEDELETE( m_pLeftFrm );
}

void ExerciseSoldierEventWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXERCISE_SOLDIER_1_BTN:
	case ID_EXERCISE_SOLDIER_2_BTN:
	case ID_EXERCISE_SOLDIER_3_BTN:
	case ID_EXERCISE_SOLDIER_4_BTN:
	case ID_EXERCISE_SOLDIER_5_BTN:
	case ID_EXERCISE_SOLDIER_6_BTN:
	case ID_EXERCISE_SOLDIER_7_BTN:
	case ID_EXERCISE_SOLDIER_8_BTN:
	case ID_EXERCISE_SOLDIER_9_BTN:
	case ID_EXERCISE_SOLDIER_10_BTN:
	case ID_EXERCISE_SOLDIER_11_BTN:
	case ID_EXERCISE_SOLDIER_12_BTN:
	case ID_EXERCISE_SOLDIER_13_BTN:
	case ID_EXERCISE_SOLDIER_14_BTN:
	case ID_EXERCISE_SOLDIER_15_BTN:
	case ID_EXERCISE_SOLDIER_16_BTN:
	case ID_EXERCISE_SOLDIER_17_BTN:
	case ID_EXERCISE_SOLDIER_18_BTN:
	case ID_EXERCISE_SOLDIER_19_BTN:
	case ID_EXERCISE_SOLDIER_20_BTN:
	case ID_EXERCISE_SOLDIER_21_BTN:
	case ID_EXERCISE_SOLDIER_22_BTN:
	case ID_EXERCISE_SOLDIER_23_BTN:
	case ID_EXERCISE_SOLDIER_24_BTN:
	case ID_EXERCISE_SOLDIER_25_BTN:
		if( cmd == IOBN_BTNUP )
		{
			bool bWait = true;
			if( !IsCanExerciseChar( bWait ) )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				if( bWait )
					g_GUIMgr.HideWnd( SOLDIER_SELECT_WND );
				return;
			}

			SoldierExerciseBtn *pItemBtn = dynamic_cast<SoldierExerciseBtn *>( FindChildWnd( dwID ) );
			if( !pItemBtn ) 
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				return;
			}

			// 용병 교체했으므로 리턴
			if( ChangeBoughtChar( g_MyInfo.GetClassArray( pItemBtn->GetClassType() ), bWait ) )
			{
				return;
			}

			int iCurMaxSlot = g_MyInfo.GetCurMaxCharSlot();
			bool bCharDelete = g_MyInfo.IsCharInActive( pItemBtn->GetClassType() );
			if( g_MyInfo.GetCharCount() >= iCurMaxSlot && g_MyInfo.GetExerciseCharCount() == 0 )
			{
				if( !bCharDelete )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
					return;
				}
			}

			bool bOneChar = true;
			if( m_pPlayStage && !m_pPlayStage->IsOneMyChar() )
				bOneChar = false;

			SP2Packet kPacket( CTPK_EXERCISE_EVENT_CHAR_CREATE );
			kPacket << EXERCISE_EVENT;
			kPacket << pItemBtn->GetClassType();
			kPacket << bWait;
			kPacket << bOneChar;
			kPacket << bCharDelete;
			TCPNetwork::SendToServer( kPacket );
			g_App.SetMouseBusy( true );

			if( bWait )
			{
				SetOwnerChangeWaitState();
				g_GUIMgr.HideWnd( SOLDIER_SELECT_WND );
			}
			else
			{   
				// 모드 시작시 선택
				if( m_pPlayStage)
				{
					ioPlayMode *pPlayMode = m_pPlayStage->GetPlayMode();
					if( pPlayMode )
						pPlayMode->EndChangeCharacter();
				}
			}
		}
		break;
	}
}

void ExerciseSoldierEventWnd::iwm_show()
{
	if( !g_EventMgr.IsAlive( EVT_EXERCISESOLDIER, g_MyInfo.GetChannelingType() ) )
	{
		HideWnd();
		return;
	}


	if( g_MyInfo.IsTutorialUser() )
	{
		HideWnd();
		return;
	}

	if( g_GUIMgr.IsShow( PRACTICE_HELP_WND ) )
	{
		HideWnd();
		return;
	}

	SetWndBtnSize();
}

void ExerciseSoldierEventWnd::UpdateChildPos()
{
	if( !IsShow() )
		return;

	for (int i = ID_EXERCISE_SOLDIER_1_BTN; i < ID_EXERCISE_SOLDIER_25_BTN + 1; i++)
	{
		ioWnd *pBtn = FindChildWnd( i );
		if( !pBtn ) continue;
		pBtn->HideWnd();
	}

	int iStart = m_iCurPos * m_iCurBtnSize;
	int iEnd = iStart + m_iCurBtnSize;
	int iWndID = ID_EXERCISE_SOLDIER_1_BTN;
	SortItemInfo();
	for (int i = iStart; i < iEnd ; i++)
	{
		_UpdateChildPos( i , iWndID);
		iWndID++;
	}
}

void ExerciseSoldierEventWnd::_UpdateChildPos( int a_iArray ,int iWndID )
{
	if( !COMPARE( a_iArray, 0, (int) m_vItemInfoList.size()) )
	{
		SetBlankIcon( iWndID );
		return;
	}
	int iSetIdx = GetSetIdx( a_iArray );
	const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
	if( !pInfo ) return;

	SoldierExerciseBtn *pItemBtn = dynamic_cast<SoldierExerciseBtn *>( FindChildWnd( iWndID ) );
	if( !pItemBtn ) return;

	const ioItem *pItem = g_ItemMaker.GetItemConst( pInfo->GetSetItemCode(0), __FUNCTION__ );
	if( !pItem ) return;

	char szBuf[MAX_PATH] = "";
	DWORD dwSoldierType = pItem->GetItemCode() % DEFAULT_BASIC_ITEM_CODE;
	ioUIRenderImage *pImg = g_MyInfo.GetMySoldierIcon( dwSoldierType );
	ioUIRenderImage *pSubImg = g_MyInfo.GetSoldierSubIcon( dwSoldierType );
	if( !pImg )
		return;

	pItemBtn->SetMarkType( pInfo->GetShopMarkType() );
	pItemBtn->SetSoldierIcon( pImg, EVENT_ICON_SCALE, false );
	pItemBtn->SetSoldierSubIcon( pSubImg );

	int iClassType = pInfo->GetSetCode() - SET_ITEM_CODE;
	SoldierExerciseBtn::SoldierBoughtType eBoughtType = SoldierExerciseBtn::SBT_NONE;
	int iArray = g_MyInfo.GetClassArray( iClassType );
	if( iArray != -1 )
	{
		if( g_MyInfo.IsCharExerciseStyle( iArray, EXERCISE_GENERAL ) )
			eBoughtType = SoldierExerciseBtn::SBT_EXERCISE_GENERAL;
		else if( g_MyInfo.IsCharExerciseStyle( iArray, EXERCISE_PCROOM ) )
			eBoughtType = SoldierExerciseBtn::SBT_EXERCISE_PCROOM;
		else if( g_MyInfo.IsCharExerciseStyle( iArray, EXERCISE_EVENT ) )
			eBoughtType = SoldierExerciseBtn::SBT_EXERCISE_EVENT;
		else if( g_MyInfo.IsCharActive( iArray ) )
			eBoughtType = SoldierExerciseBtn::SBT_BOUGHT;
	}
	pItemBtn->SetBoughtType( eBoughtType );
	pItemBtn->SetClassType( iClassType );
	pItemBtn->SetActive();
	pItemBtn->ShowWnd();	

	int iType  = ioSetItemInfo::NLT_NONE;
	int iLevel = -1;

	if( iType == ioSetItemInfo::NLT_NONE )
		pItemBtn->SetNeedTypeLv( (ioSetItemInfo::NeedLevelType)iType, -1 ); 
	else
		pItemBtn->SetNeedTypeLv( (ioSetItemInfo::NeedLevelType)iType, iLevel );
}

void ExerciseSoldierEventWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

bool ExerciseSoldierEventWnd::IsCanExerciseChar( bool bSelectWait )
{
	if( !m_pPlayStage ) return true;
	ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
	if( !pOwner ) return true;
	if( g_MyInfo.GetCharCount() == 0 ) return true;
	if( bSelectWait && !pOwner->IsCanChangeCharState( -1, true, false, false ) ) return false;

	if( pOwner->GetExperienceMode() == EMS_CHANGE_EXPERIENCE )
		return false;

	return true;
}

void ExerciseSoldierEventWnd::SetBoughtType( SoldierExerciseBtn::SoldierBoughtType eBoughtType, int iCharArray )
{
	int iClassType = g_MyInfo.GetClassType( iCharArray );

	for (int i = ID_EXERCISE_SOLDIER_1_BTN; i <  ID_EXERCISE_SOLDIER_25_BTN +1; i++)
	{
		SoldierExerciseBtn *pItemBtn = dynamic_cast<SoldierExerciseBtn *>( FindChildWnd( i ) );
		if( !pItemBtn )	 continue;

		if( iClassType == pItemBtn->GetClassType() )
		{
			pItemBtn->SetBoughtType( eBoughtType );
			break;
		}		
	}
}

void ExerciseSoldierEventWnd::SetOwnerChangeWaitState()
{
	if( !m_pPlayStage ) return;

	if( m_pPlayStage->GetModeType() == MT_MYROOM )
		return;

	ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
	if( !pOwner ) return;

	if( m_pPlayStage->IsOneMyChar() )
		pOwner->SetChangeWaitState( pOwner->GetSelectCharArray(), false );
}

void ExerciseSoldierEventWnd::SendChangeWait( int iArray )
{
	if( !m_pPlayStage ) return;

	ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
	if( !pOwner ) return;

	if( m_pPlayStage->GetModeType() != MT_MYROOM )
	{
		pOwner->SetChangeWaitState( iArray );
	}
	else
	{
		SP2Packet kPacket( CTPK_CHANGE_SINGLE_CHAR );
		kPacket << pOwner->GetIndex();
		kPacket << g_MyInfo.GetCharIndex( iArray );
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );

		pOwner->ClearBullet();
	}
}

void ExerciseSoldierEventWnd::SetBlankIcon( int a_iWndID )
{
	SoldierExerciseBtn *pItemBtn = dynamic_cast<SoldierExerciseBtn *>( FindChildWnd( a_iWndID ) );
	if( !pItemBtn ) return;

	ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( "UIIconPack3#empty_large" );
	if( !pImg )	return;

	pItemBtn->SetSoldierIcon( pImg, EVENT_ICON_SCALE, false );
	pItemBtn->SetBoughtType( SoldierExerciseBtn::SBT_NONE );
	pItemBtn->ShowWnd();		
	pItemBtn->SetInActive();
	pItemBtn->SetNeedTypeLv( ioSetItemInfo::NLT_NONE, -1 );
}

void ExerciseSoldierEventWnd::SetMaxPage()
{
	enum { MINUS_VALUE = 138, DIVISION_VALUE = 60 };
	m_iCurBtnSize = ( Setting::Width() - MINUS_VALUE ) / DIVISION_VALUE;
	if( m_iCurBtnSize > MAX_EXERCISE_SOLDIER_BTN )
		m_iCurBtnSize = MAX_EXERCISE_SOLDIER_BTN;

	int iMaxPage = 0;
	int iUseCharCnt = m_vItemInfoList.size();
	if( iUseCharCnt <= 0 )
	{
		return;
	}
	iMaxPage = (  iUseCharCnt / m_iCurBtnSize );
	if( ( iUseCharCnt % m_iCurBtnSize ) != 0 )
		iMaxPage++;

	m_iCurPos = 0;
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, iMaxPage );
		pScroll->SetScrollPos( m_iCurPos );
		pScroll->ShowWnd();
	}
}

bool ExerciseSoldierEventWnd::ChangeBoughtChar( int iArray, bool bWait )
{
	if( iArray == -1 ) return false;

	if( g_MyInfo.IsCharActive( iArray ) )
	{	
		if( bWait )
		{
			SendChangeWait( iArray );
			g_GUIMgr.HideWnd( SOLDIER_SELECT_WND );
		}
		else
		{
			// 모드 시작시 선택
			SP2Packet kPacket( CTPK_CHANGE_CHAR );
			kPacket << iArray;
			kPacket << false;
			kPacket << MAX_INT_VALUE;
			TCPNetwork::SendToServer( kPacket );

			if( m_pPlayStage)
			{
				ioPlayMode *pPlayMode = m_pPlayStage->GetPlayMode();
				if( pPlayMode )
					pPlayMode->EndChangeCharacter();
			}
		}
		return true;
	}
	return false;
}

void ExerciseSoldierEventWnd::SortItemInfo()
{
	m_vItemInfoList.clear();
	int iMax = g_ClassPrice.MaxClassPrice();
	for (int i = 0; i <  iMax; i++)
	{
		if( !g_ClassPrice.IsActiveClass( i ) )
		{
			// inactive 용병이지만 표시해야 하는 경우가 있다 ( 해외 가챠 캡슐용병등 )
			bool bContinue = true;
			int iClassType = g_ClassPrice.GetClassType( i );
			int iSize = m_vExtraClassVec.size();
			for (int j = 0; j < iSize ; j++)
			{
				int &rClassType = m_vExtraClassVec[j];
				if( iClassType == rClassType )
				{
					bContinue = false;
					break;
				}
			}

			if( bContinue )
				continue;
		}

		if( g_ClassPrice.IsCashOnlyByArray( i ) )
			continue;

		int iSetIdx = g_ClassPrice.ArrayToSetItemIdx( i );
		const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
		if( !pInfo ) continue;

		ShopItemInfo kInfo;
		kInfo.m_iSetItemIdx = iSetIdx;
		kInfo.m_bCanBuy     = g_MyInfo.IsCanBuyItem( pInfo );
		kInfo.m_iPrice      = g_ClassPrice.GetClassBuyPeso( pInfo->GetSetCode() - SET_ITEM_CODE, g_ClassPrice.GetDefaultLimit() );
		kInfo.m_iPriorityOrder = pInfo->GetShopOrder();
		kInfo.m_iShopMarkType  = pInfo->GetShopMarkType();

		int iClassType = pInfo->GetSetCode() - SET_ITEM_CODE;
		int iArray = g_MyInfo.GetClassArray( iClassType );

		if( g_MyInfo.IsCharExerciseStyle( iArray, EXERCISE_NONE ) )
			continue;

		if( g_MyInfo.IsCharExerciseStyle( iArray, EXERCISE_PCROOM ) )
			continue;

		m_vItemInfoList.push_back( kInfo );
	}

	if( m_vItemInfoList.empty() )
		return;
	std::sort( m_vItemInfoList.begin() , m_vItemInfoList.end(), ShopItemInfoSort() );
}

int ExerciseSoldierEventWnd::GetSetIdx( int iArray )
{
	if( COMPARE( iArray, 0, (int) m_vItemInfoList.size() ) )
		return m_vItemInfoList[iArray].m_iSetItemIdx;

	return 0;
}

void ExerciseSoldierEventWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /*= false */ )
{
	ioWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	if( !bCreate )
	{
		SetWndBtnSize();
	}
}

void ExerciseSoldierEventWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;
	
	if( !g_EventMgr.IsAlive( EVT_EXERCISESOLDIER, g_MyInfo.GetChannelingType() ) )
		HideWnd();
}

void ExerciseSoldierEventWnd::SetWndBtnSize()
{
	ioWnd::SetSize( Setting::Width(), GetHeight() );

	enum { X_OFFSET = 32, };
	ioWnd *pChild = FindChildWnd( ID_VERT_SCROLL );
	if( pChild )
		pChild->SetWndPos( Setting::Width() - X_OFFSET, pChild->GetYPos() );
	
	SortItemInfo();
	SetMaxPage();
}

void ExerciseSoldierEventWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_vExtraClassVec.clear();
	m_vExtraClassVec.reserve(10);
	int iMax = xElement.GetIntAttribute_e( "MaxClass" );
	for (int i = 0; i < iMax ; i++)
	{
		char szName[MAX_PATH]="";
		StringCbPrintf_e( szName, sizeof( szName ), "ClassType%d", i+1 );
		int iClassType = xElement.GetIntAttribute( szName );
		if( iClassType  <= 0 )
			continue;
		m_vExtraClassVec.push_back( iClassType );
	}
}

void ExerciseSoldierEventWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_pLeftFrm )
		m_pLeftFrm->Render( iXPos, iYPos, UI_RENDER_MULTIPLY );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );	

	enum 
	{
		X_OFFSET = 12,
		Y_OFFSET = 18,
		Y_LINFGAP= 18,
	};
	g_FontMgr.PrintText( iXPos + X_OFFSET, iYPos + Y_OFFSET , FONT_SIZE_13, STR(1) );
	g_FontMgr.PrintText( iXPos + X_OFFSET, iYPos + Y_OFFSET + Y_LINFGAP, FONT_SIZE_13, STR(2) );
}

void ExerciseSoldierEventWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{
		if( g_App.IsMouseBusy() ) return;

		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( pScroll == NULL ) return;
		if( !pScroll->IsShow() ) return;

		if( zDelta == WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos - 1 );
		else if( zDelta == -WHEEL_DELTA )
			pScroll->SetScrollPos( m_iCurPos + 1 );
	}
}

void ExerciseSoldierEventWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll == NULL ) return;
	if( !pScroll->IsShow() ) return;

	m_iCurPos = curPos;

	UpdateChildPos();
}

void ExerciseSoldierEventWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "LeftFrm" )
	{
		SAFEDELETE( m_pLeftFrm );
		m_pLeftFrm = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}