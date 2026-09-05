

#include "StdAfx.h"

#include "SoldierSelectWnd.h"
#include "ExPCRoomWnd.h"

ExPCRoomAuthorityWnd::ExPCRoomAuthorityWnd()
{
}

ExPCRoomAuthorityWnd::~ExPCRoomAuthorityWnd()
{
}

bool ExPCRoomAuthorityWnd::iwm_spacebar()
{
	iwm_command( FindChildWnd( ID_SOLDIER_BTN ), IOBN_BTNUP, 0 );
	return true;
}

void ExPCRoomAuthorityWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_SOLDIER_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal && pLocal->IsPCRoomBonusSoldier() )
			{
				ExPCRoomSoldierBonusWnd* pBonusWnd; 
				if( g_MyInfo.GetPCRoomAuthority() ==  FREEDAY_EVENT_CODE )
					pBonusWnd = dynamic_cast<ExPCRoomSoldierBonusWnd*>( g_GUIMgr.FindWnd(USER_SELECT_HERO_BONUS_WND  ) );
				else if( g_MyInfo.GetPCRoomAuthority() != 0 )
					pBonusWnd = dynamic_cast<ExPCRoomSoldierBonusWnd*>( g_GUIMgr.FindWnd( EX_PCROOM_SOLDIER_BONUS_WND ) );
				if( pBonusWnd )
				{
					pBonusWnd->SetFirstAutoSelect();
					pBonusWnd->ShowWnd();
				}
			}
			HideWnd();
		}
		break;
	}
}
//////////////////////////////////////////////////////////////////////////
ExPCRoomSoldierBonusBtn::ExPCRoomSoldierBonusBtn()
{
	m_pIconBack = NULL;
	m_pCharIcon = NULL;
	m_pCharSubIcon = NULL;
	m_pNumBack  = NULL;
		
	m_pCheckedFrm = NULL;
	
	m_iSelectNum = 0;
	m_iClassType = -1;	

	m_bChecked   = false;

	m_nGradeType = 0;
}

ExPCRoomSoldierBonusBtn::~ExPCRoomSoldierBonusBtn()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pCharIcon );
	SAFEDELETE( m_pCharSubIcon );
	SAFEDELETE( m_pNumBack );

	SAFEDELETE( m_pCheckedFrm );
}

void ExPCRoomSoldierBonusBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "NumBack" )
	{
		SAFEDELETE( m_pNumBack );
		m_pNumBack = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void ExPCRoomSoldierBonusBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "Checked" )
	{		
		SAFEDELETE( m_pCheckedFrm );
		m_pCheckedFrm = pFrame;
	}
	else
	{
		ioButton::AddRenderFrame( szType, pFrame );
	}
}

void ExPCRoomSoldierBonusBtn::SetClassType( int iClassType, bool bCheck, int iSelectNum )
{
	m_iClassType = iClassType;
	m_bChecked   = bCheck;
	m_iSelectNum = iSelectNum;
	m_nGradeType = 0;

	if( m_iClassType != -1 )
	{
		SAFEDELETE( m_pCharIcon );
		SAFEDELETE( m_pCharSubIcon );
		m_pCharIcon = g_MyInfo.GetMySoldierIcon( iClassType );
		m_pCharSubIcon = g_MyInfo.GetSoldierSubIcon( iClassType );
		m_nGradeType = g_UIImageRenderMgr.GetPowerUpGradeType( iClassType );
		SetActive();
	}
	else
	{
		SetInActive();
	}
}

void ExPCRoomSoldierBonusBtn::OnRender()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	ioButton::OnDrawNormal( iXPos, iYPos );

	if( IsDisabled() ) 
		return;

	if( m_pIconBack )
	{
		m_pIconBack->SetScale( 0.60f );
		m_pIconBack->Render( iXPos + 29, iYPos + 29, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}

	g_UIImageRenderMgr.RenderGradeIconBack( m_nGradeType, iXPos + 29, iYPos + 29, 0.60f );

	if( m_pCharIcon )
	{
		m_pCharIcon->SetScale( 0.60f );
		m_pCharIcon->Render( iXPos + 29, iYPos + 29, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	if( m_pCharSubIcon )
	{
		m_pCharSubIcon->SetScale( 0.60f );
		m_pCharSubIcon->Render( iXPos + 29, iYPos + 29, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	if( m_bChecked && m_pCheckedFrm )
	{
		m_pCheckedFrm->Render( iXPos, iYPos );

		if( m_pNumBack )
		{
			m_pNumBack->Render( iXPos + 4, iYPos + 4 );

			g_FontMgr.SetBkColor( 170, 55, 77 );
			g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
			g_FontMgr.SetAlignType( TAT_CENTER );
			g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
			g_FontMgr.PrintText( iXPos + 12, iYPos + 5, FONT_SIZE_9, "%d", m_iSelectNum+1 );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
ExPCRoomSoldierBonusWnd::ExPCRoomSoldierBonusWnd()
{
	m_iCurPage = m_iMaxPage = 0;

	m_pPlayStage  = NULL;

	m_pCharFrameOne	   = NULL;
	m_pCharFrameTwo    = NULL;

	m_pCharBottomFrame = NULL;
	m_pCharFrameNum    = NULL;

	m_bFirstAutoSelect = false;
	m_bEraseBegin      = false;
}

ExPCRoomSoldierBonusWnd::~ExPCRoomSoldierBonusWnd()
{
	SAFEDELETE( m_pCharFrameOne );	
	SAFEDELETE( m_pCharFrameTwo );

	SAFEDELETE( m_pCharBottomFrame );
	SAFEDELETE( m_pCharFrameNum );
}

void ExPCRoomSoldierBonusWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "CharFrameOne" )
	{
		SAFEDELETE( m_pCharFrameOne );
		m_pCharFrameOne = pFrame;
	}
	else if( szType == "CharFrameTwo" )
	{
		SAFEDELETE( m_pCharFrameTwo );
		m_pCharFrameTwo = pFrame;
	}
	else if( szType == "CharFrameNum" )
	{
		SAFEDELETE( m_pCharFrameNum );
		m_pCharFrameNum = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void ExPCRoomSoldierBonusWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "CharBottomFrame" )
	{
		SAFEDELETE( m_pCharBottomFrame );
		m_pCharBottomFrame = pImage;

		if( m_pCharBottomFrame )
		{
			m_pCharBottomFrame->SetSize( 141, 46 );
			m_pCharBottomFrame->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pCharBottomFrame->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
		}
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ExPCRoomSoldierBonusWnd::iwm_show()
{
	m_PCRoomSoldierSelectVec.clear();
	m_ClassTypeList.clear();
			
	AddPCRoomSoldierList( ioSetItemInfo::PT_NORMAL );
	AddPCRoomSoldierList( ioSetItemInfo::PT_PREMIUM );
	AddPCRoomSoldierList( ioSetItemInfo::PT_RARE );

	//선택된 PC방 용병이 없고 자동선택 기능이 활성화되있을때 1번째, 2번째 캐릭터를 선택 용병으로 지정
	if( g_MyInfo.GetExerciseCharCount( EXERCISE_PCROOM ) == 0 )
	{
		for( int i = 0; i < g_MyInfo.GetPcRoomCharMax(); ++i )
		{
			if( i < (int)m_ClassTypeList.size() )
			{
				int iClassType = m_ClassTypeList[i];
				InsertSelectInfo( iClassType, IsTimeExpireChar( iClassType ) );
			}
		}
	}
	
	m_iMaxPage = max( 0, (int)m_ClassTypeList.size() - 1 ) / MAX_PAGE_LIST;
	UpdateSetPage( 0 );

	RefreshCharWnd();
	m_bFirstAutoSelect = false;
	m_bEraseBegin      = false;

	if( m_ClassTypeList.empty() )
	{
		HideWnd();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
}

void ExPCRoomSoldierBonusWnd::AddPCRoomSoldierList( ioSetItemInfo::PackageType eType )
{
	int iMax = g_ClassPrice.MaxClassPrice();
	for ( int i = 0; i <  iMax; i++ )
	{
		int iSetIdx = g_ClassPrice.ArrayToSetItemIdx( i );
		const ioSetItemInfo *pInfo = g_SetItemInfoMgr.GetSetInfoByIdx( iSetIdx );
		if( !pInfo )
			continue;

		if( pInfo->GetPackageType() != eType )
			continue;

		bool bChecked = false;
		bool bDelete = false;
		int iClassType = pInfo->GetSetCode() - SET_ITEM_CODE;
		DWORD dwCharIndex = g_MyInfo.GetCharIndexByClassType( iClassType );
		if( !g_ClassPrice.IsPcRoomActive( iClassType ) )
		{
			continue;
		}

		if( dwCharIndex != 0 )
		{
			if( g_MyInfo.IsCharExerciseStyle( g_MyInfo.GetCharArray( dwCharIndex ), EXERCISE_PCROOM ) )
			{				
				bChecked = true;
			}
			else if( IsTimeExpireChar( iClassType ) )
			{
				bDelete = true;
			}
			else
			{
				continue;
			}
		}

		m_ClassTypeList.insert( m_ClassTypeList.begin(), iClassType );

		if( bChecked )
		{
			PCRoomSoldierSelect PcRoomSelect;
			PcRoomSelect.m_iClassType  = iClassType;
			PcRoomSelect.m_bCharDelete = bDelete;
			m_PCRoomSoldierSelectVec.insert( m_PCRoomSoldierSelectVec.begin(), PcRoomSelect );
		}
	}
}

bool ExPCRoomSoldierBonusWnd::IsTimeExpireChar( int iClassType )
{
	const CHARACTER& rkChar = g_MyInfo.GetCharacter( g_MyInfo.GetCharArray( g_MyInfo.GetCharIndexByClassType( iClassType ) ) );
	if( rkChar.m_bActive )
		return false;

	if( rkChar.m_ePeriodType == CPT_MORTMAIN )
		return false;

	if( 0 < rkChar.m_dwRentalMinute )
		return false;

	return true;
}

void ExPCRoomSoldierBonusWnd::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void ExPCRoomSoldierBonusWnd::SetFirstAutoSelect()
{
	m_bFirstAutoSelect = true;
}

void ExPCRoomSoldierBonusWnd::InsertSelectInfo( int iClassType, bool bCharDelete )
{
	PCRoomSoldierSelect PcRoomSelect;
	PcRoomSelect.m_iClassType  = iClassType;
	PcRoomSelect.m_bCharDelete = bCharDelete;
		
	if( m_PCRoomSoldierSelectVec.size() == g_MyInfo.GetPcRoomCharMax() )
	{	
		for( int i = 0; i < (int)m_PCRoomSoldierSelectVec.size(); ++i )
		{
			//플레이 중인 용병이 나와있다면 교체 못하도록 함
			if( g_MyInfo.GetClassType() == m_PCRoomSoldierSelectVec[i].m_iClassType )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "플레이 중인 용병을#다른 용병으로 교체 해주세요" );
				return;
			}
		}

		if( m_bEraseBegin )
		{
			m_PCRoomSoldierSelectVec.erase( m_PCRoomSoldierSelectVec.begin() );
			m_bEraseBegin = false;
			m_PCRoomSoldierSelectVec.insert( m_PCRoomSoldierSelectVec.begin(), PcRoomSelect );
		}
		else
		{	
			m_PCRoomSoldierSelectVec.pop_back();
			m_bEraseBegin = true;
			m_PCRoomSoldierSelectVec.push_back( PcRoomSelect );
		}
	}
	else
	{
		m_bEraseBegin = false;
		m_PCRoomSoldierSelectVec.push_back( PcRoomSelect );
	}
	RefreshCharWnd();
}

void ExPCRoomSoldierBonusWnd::RemoveSelectInfo( int iClassType )
{	
	PCRoomSoldierSelectVec::iterator iter = m_PCRoomSoldierSelectVec.begin();
	for( ; iter != m_PCRoomSoldierSelectVec.end(); ++iter )
	{
		PCRoomSoldierSelect& rkInfo = *iter;
		if( rkInfo.m_iClassType == iClassType )
		{
			m_PCRoomSoldierSelectVec.erase( iter );
			break;
		}
	}

	RefreshCharWnd();
}

void ExPCRoomSoldierBonusWnd::GetSelectInfo( int iClassType, OUT bool& bHas, OUT int& iSelectNum )
{
	PCRoomSoldierSelectVec::iterator iter = m_PCRoomSoldierSelectVec.begin();
	for( int i = 0; iter != m_PCRoomSoldierSelectVec.end(); ++iter, ++i )
	{
		PCRoomSoldierSelect& rkInfo = *iter;
		if( rkInfo.m_iClassType == iClassType )
		{
			bHas = true;
			iSelectNum = i;
			return;
		}
	}
}

void ExPCRoomSoldierBonusWnd::RefreshCharWnd()
{	
	HideChildWnd( ID_SELECT_CHAR_VIEW1 );
	HideChildWnd( ID_SELECT_CHAR_VIEW2 );

	m_szOneCharName.Clear();	
	m_szTwoCharName.Clear();
	m_iOneCharType = -1;
	m_iTwoCharType = -1;

	for( int i = 0; i < g_MyInfo.GetPcRoomCharMax(); ++i )
	{
		if( i < (int)m_PCRoomSoldierSelectVec.size() )
		{
			const CHARACTER& rkCharInfo = g_MyInfo.GetCharacter( g_MyInfo.GetClassArray( m_PCRoomSoldierSelectVec[i].m_iClassType ) );
			if( m_PCRoomSoldierSelectVec[i].m_iClassType != rkCharInfo.m_class_type )
			{
				CHARACTER NewCharInfo;
				NewCharInfo.Init();
				Help::GetDefaultCharInfo( NewCharInfo, m_PCRoomSoldierSelectVec[i].m_iClassType );

				if( i == 0 )
				{
					SelectNewChar( NewCharInfo, ID_SELECT_CHAR_VIEW1 );
					const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( NewCharInfo.m_class_type - 1 );
					if( pSetInfo )
					{
						m_szOneCharName = pSetInfo->GetName();

						const ioItem *pItem = g_ItemMaker.GetItemConst( NewCharInfo.m_class_type, __FUNCTION__ );
						if( pItem )
							m_iOneCharType = pItem->GetGradeType(); //pSetInfo->GetPackageType();			
					}
				}
				else
				{
					SelectNewChar( NewCharInfo, ID_SELECT_CHAR_VIEW2 );
					const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( NewCharInfo.m_class_type - 1 );					
					if( pSetInfo )
					{
						m_szTwoCharName = pSetInfo->GetName();

						const ioItem *pItem = g_ItemMaker.GetItemConst( NewCharInfo.m_class_type, __FUNCTION__ );
						if( pItem )
							m_iTwoCharType = pItem->GetGradeType(); //pSetInfo->GetPackageType();				
					}
				}
			}
			else
			{
				if( i == 0 )
				{
					SelectNewChar( rkCharInfo, ID_SELECT_CHAR_VIEW1 );
					const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( rkCharInfo.m_class_type - 1 );
					if( pSetInfo )
					{
						m_szOneCharName = pSetInfo->GetName();

						const ioItem *pItem = g_ItemMaker.GetItemConst( rkCharInfo.m_class_type, __FUNCTION__ );
						if( pItem )
							m_iOneCharType = pItem->GetGradeType(); //pSetInfo->GetPackageType();
					}
				}
				else
				{
					SelectNewChar( rkCharInfo, ID_SELECT_CHAR_VIEW2 );
					const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( rkCharInfo.m_class_type - 1 );
					if( pSetInfo )
					{
						m_szTwoCharName = pSetInfo->GetName();

						const ioItem *pItem = g_ItemMaker.GetItemConst( rkCharInfo.m_class_type, __FUNCTION__ );
						if( pItem )
							m_iTwoCharType = pItem->GetGradeType(); //pSetInfo->GetPackageType();
					}
				}
			}
		}
	}
}

void ExPCRoomSoldierBonusWnd::UpdateSetPage( int iCurPage )
{
	int iPrevPage = m_iCurPage;
	m_iCurPage = iCurPage;

	int iListSIze = m_ClassTypeList.size();
	int iStartPos = iCurPage * MAX_PAGE_LIST;
	for(int i = ID_BONUS_CHAR_BTN01; i <= ID_BONUS_CHAR_BTN15; ++i, ++iStartPos )
	{
		ExPCRoomSoldierBonusBtn *pBtn = dynamic_cast<ExPCRoomSoldierBonusBtn*>( FindChildWnd( i ) );
		if( pBtn )
		{
			if( iStartPos < iListSIze )
			{
				bool bHas = false;
				int iSelectNum = 0;
				GetSelectInfo( m_ClassTypeList[iStartPos], bHas, iSelectNum );
				pBtn->SetClassType( m_ClassTypeList[iStartPos], bHas, iSelectNum );
			}
			else
			{
				pBtn->SetClassType( -1, false, 0 );
			}
		}
	}	
}

bool ExPCRoomSoldierBonusWnd::IsCanExerciseChar( bool bSelectWait )
{
	if( !m_pPlayStage )
		return true;

	if( g_MyInfo.GetCharCount() == 0 )
		return true;

	ioBaseChar *pOwner = m_pPlayStage->GetOwnerChar();
	if( !pOwner )
		return true;

	if( bSelectWait && !pOwner->IsCanChangeCharState( -1, true, false, true ) )
		return false;

	if( pOwner->GetExperienceMode() == EMS_CHANGE_EXPERIENCE )
		return false;

	return true;
}

void ExPCRoomSoldierBonusWnd::SelectNewChar( const CHARACTER& CharInfo, int iWND )
{
	SelectCharViewWnd *pCharWnd = dynamic_cast<SelectCharViewWnd *>( FindChildWnd( iWND ));
	if( pCharWnd )
	{
		pCharWnd->SetChar( CharInfo );
		pCharWnd->SetCharScale( FLOAT1 );
		pCharWnd->SetEquip( false, false, false );
		pCharWnd->ShowWnd();
	}
}

bool ExPCRoomSoldierBonusWnd::iwm_spacebar()
{
	iwm_command( FindChildWnd( ID_OK ), IOBN_BTNUP, 0 );
	return true;
}

void ExPCRoomSoldierBonusWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
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
	case ID_LEFT_PAGE:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( max( 0, m_iCurPage - 1 ) );
		}
		break;
	case ID_RIGHT_PAGE:
		if( cmd == IOBN_BTNUP )
		{
			UpdateSetPage( min( m_iMaxPage, m_iCurPage + 1 ) );
		}
		break;
	case ID_BONUS_CHAR_BTN01:
	case ID_BONUS_CHAR_BTN02:
	case ID_BONUS_CHAR_BTN03:
	case ID_BONUS_CHAR_BTN04:
	case ID_BONUS_CHAR_BTN05:
	case ID_BONUS_CHAR_BTN06:
	case ID_BONUS_CHAR_BTN07:
	case ID_BONUS_CHAR_BTN08:
	case ID_BONUS_CHAR_BTN09:
	case ID_BONUS_CHAR_BTN10:
	case ID_BONUS_CHAR_BTN11:
	case ID_BONUS_CHAR_BTN12:
	case ID_BONUS_CHAR_BTN13:
	case ID_BONUS_CHAR_BTN14:
	case ID_BONUS_CHAR_BTN15:
		if( cmd == IOBN_BTNUP )
		{
			ExPCRoomSoldierBonusBtn *pBtn = dynamic_cast<ExPCRoomSoldierBonusBtn*>( pWnd );
			if( pBtn->GetClassType() == -1 )
			{
				HideWnd();
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			}
			else
			{
				int iSelectClass = pBtn->GetClassType();
				if( pBtn->IsChecked() )
				{
					if( iSelectClass == g_MyInfo.GetClassType() )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, "플레이 중인 용병은#해제 할 수 없습니다." );
						return;
					}

					RemoveSelectInfo( iSelectClass );
					UpdateSetPage( m_iCurPage );
				}
				else
				{
					InsertSelectInfo( iSelectClass, IsTimeExpireChar( pBtn->GetClassType() ) );
					UpdateSetPage( m_iCurPage );
				}
			}
		}
		break;
	case ID_OK:
		if( cmd == IOBN_BTNUP )
		{
			bool bOneChar = true;
			if( m_pPlayStage && !m_pPlayStage->IsOneMyChar() )
				bOneChar = false;

			bool bWait = true;
			SoldierSelectWnd *pSelectWnd = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
			if( pSelectWnd && pSelectWnd->IsShow() && pSelectWnd->IsForcedWait() ) // hide 초기값이 forcewait이 되므로 IsShow 확인
				bWait = false;

			if( !IsCanExerciseChar( bWait ) )
			{
				HideWnd();
				return;
			}

			SP2Packet kPacket( CTPK_EXERCISE_PCROOM_CHAR_CREATE );
			kPacket << bWait;
			kPacket << bOneChar;
			kPacket << g_MyInfo.GetPcRoomCharMax();
			for( int i = 0; i < g_MyInfo.GetPcRoomCharMax(); ++i )
			{
				if( i < (int)m_PCRoomSoldierSelectVec.size() )
				{
					PCRoomSoldierSelect PcRoomSelect = m_PCRoomSoldierSelectVec[i];
					kPacket << PcRoomSelect.m_iClassType;
					kPacket << PcRoomSelect.m_bCharDelete;

					if( g_MyInfo.GetCharCount() >= g_MyInfo.GetCurMaxCharSlot() && g_MyInfo.GetExerciseCharCount() == 0 )
					{
						if( !PcRoomSelect.m_bCharDelete )
						{
							HideWnd();
							g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
							return;
						}
					}
				}
				else
				{
					kPacket << 0;
					kPacket << false;
				}
			}

			TCPNetwork::SendToServer( kPacket );
			g_App.SetMouseBusy( true );
			HideWnd();
		}
		break;
	}
}

ioHashString ExPCRoomSoldierBonusWnd::GetPackageType( int iType )
{
	static ioHashString szNone;
	if( iType == -1 )
		return szNone;

	switch( iType )
	{
	case ioSetItemInfo::PT_PREMIUM:
		return STR(1);
	case ioSetItemInfo::PT_RARE:
		return STR(2);
	}

	return STR(3);
}

ioHashString ExPCRoomSoldierBonusWnd::GetGradeType( int iType )
{
	static ioHashString szNone;
	if( iType == -1 )
		return szNone;

	switch( iType )
	{
	case 2:
		return "프리미엄";
	case 3:
		return "레어";
	case 4:
		return "유니크";
	}

	return "일반";
}

void ExPCRoomSoldierBonusWnd::OnRenderAfterChild()
{
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	On3DCharInfoRender( iXPos, iYPos );
	OnMentRender( iXPos, iYPos );
}

void ExPCRoomSoldierBonusWnd::On3DCharInfoRender( int iXPos, int iYPos )
{
	if( m_pCharFrameOne )
		m_pCharFrameOne->Render( iXPos, iYPos );

	if( m_pCharFrameTwo )
		m_pCharFrameTwo->Render( iXPos, iYPos );

	if( m_pCharFrameNum )
	{
		m_pCharFrameNum->SetSize( 25, 25 );
		m_pCharFrameNum->Render( iXPos + 25,  iYPos + 62 );
		m_pCharFrameNum->Render( iXPos + 179, iYPos + 62 );

		g_FontMgr.SetBkColor( 170, 55, 77 );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_WHITE );
		g_FontMgr.PrintText( iXPos + 37,  iYPos + 63, FONT_SIZE_17, STR(1), 1 );
		g_FontMgr.PrintText( iXPos + 191, iYPos + 63, FONT_SIZE_17, STR(1), 2 );
	}

	if( m_pCharBottomFrame )
	{		
		m_pCharBottomFrame->Render( iXPos + 21,  iYPos + 324, UI_RENDER_MULTIPLY );		
		m_pCharBottomFrame->Render( iXPos + 175, iYPos + 324, UI_RENDER_MULTIPLY );
	}

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 91,  iYPos + 332, FONT_SIZE_14, STR(2), m_szOneCharName.c_str() );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 91,  iYPos + 351, FONT_SIZE_12, STR(2), GetGradeType( m_iOneCharType ) );

	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 245,  iYPos + 332, FONT_SIZE_14, STR(2), m_szTwoCharName.c_str() );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 245,  iYPos + 351, FONT_SIZE_12, STR(2), GetGradeType( m_iTwoCharType ) );
}

void ExPCRoomSoldierBonusWnd::OnMentRender( int iXPos, int iYPos )
{
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(1) );
	kPrinter.PrintFullText( iXPos + 502, iYPos + 53, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );

	if( g_MyInfo.IsUserEvent() )
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(8) );
	else if( g_MyInfo.GetPCRoomAuthority() > 0 && !g_MyInfo.IsUserEvent() )
		kPrinter.AddTextPiece( FONT_SIZE_12, STR(2) );

	kPrinter.SetTextColor( TCT_DEFAULT_RED );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(3), 2 );

	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(4) );
	kPrinter.PrintFullText( iXPos + 502, iYPos + 71, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(5) );
	kPrinter.PrintFullText( iXPos + 502, iYPos + 89, TAT_CENTER );
	kPrinter.ClearList();

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_DARKGRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(6), m_iCurPage + 1 );

	kPrinter.SetTextStyle( TS_NORMAL );
	kPrinter.SetBkColor( 0, 0, 0 );
	kPrinter.SetTextColor( TCT_DEFAULT_GRAY );
	kPrinter.AddTextPiece( FONT_SIZE_12, STR(7), m_iMaxPage + 1 );
	kPrinter.PrintFullText( iXPos + 502, iYPos + 314, TAT_CENTER );
}
//////////////////////////////////////////////////////////////////////////
SelectCharViewWnd::SelectCharViewWnd()
{
	m_pUI3DRender = NULL;
	m_aMyChar	  = NULL;
	m_p3DBack	  = NULL;

	m_iClassType  = 0;
	m_fZPosition  = FLOAT500;
	m_fRotateYaw  = 0.0f;

	m_bReleaseHelm	 = false;
	m_bReleaseAllItem= false;
	m_bShopMotion    = false;
	m_bUserInfoToolTip = false;
}

SelectCharViewWnd::~SelectCharViewWnd()
{
	SAFEDELETE( m_aMyChar );
	SAFEDELETE( m_pUI3DRender );
	SAFEDELETE( m_p3DBack );
}

void SelectCharViewWnd::iwm_create()
{
	SAFEDELETE( m_pUI3DRender );

	m_pUI3DRender = new ioUI3DRender;
	m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "battle_cam" );

	m_fZPosition = FLOAT500;
	m_fRotateYaw = 0.0f;
}

void SelectCharViewWnd::iwm_hide()
{
	SAFEDELETE( m_aMyChar );
}

void SelectCharViewWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "3D_Back" )
	{
		SAFEDELETE(m_p3DBack);
		m_p3DBack = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void SelectCharViewWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_fXMLRotateYaw = xElement.GetFloatAttribute_e( "RotateYaw" );
}

void SelectCharViewWnd::CreateCharacter( const CHARACTER &rkInfo )
{
	ioCreateChar *pChar = m_pUI3DRender->CreateChar( rkInfo, "ResultChar" );
	if( !pChar )
		return;
	
	pChar->SetPosition( 0.0f, -FLOAT90, 1050.0f );	
	pChar->SetYaw( 30.0f );
	m_pUI3DRender->SetLookAt( 0.0f, 0.0f, 900.0f );

	float fAspect = (float)VIEW_WIDTH/(float)VIEW_HEIGHT;
	m_pUI3DRender->SetProjection( FLOAT1, 600.0f, 45.0f, fAspect );

	SAFEDELETE( m_aMyChar );
	m_aMyChar = pChar;

	if( m_iClassType != rkInfo.m_class_type )
	{
		m_iClassType = rkInfo.m_class_type;
	}
}

void SelectCharViewWnd::SetCharAnimation( bool bShopMotion )
{
	if( !m_aMyChar )
		return;

	ioHashString szUIAnimation = m_pUI3DRender->GetModelAnimation( 0, m_aMyChar->IsMale() );
	if( !szUIAnimation.IsEmpty() )
	{
		m_aMyChar->SetLoopAni( szUIAnimation );
	}
}

void SelectCharViewWnd::SetCharAnimation( DWORD dwEtcItem, bool bMale )
{
	if( !m_aMyChar ) 
		return;

	if( !COMPARE( dwEtcItem, ioEtcItem::EIT_ETC_MOTION1, ioEtcItem::EIT_ETC_MOTION100 + 1 ) && !COMPARE( dwEtcItem, ioEtcItem::EIT_ETC_MOTION101, ioEtcItem::EIT_ETC_MOTION400 + 1 ) ) 
		return;

	ioEtcItemMotion *pEtcItemMotion = dynamic_cast< ioEtcItemMotion * >( g_EtcItemMgr.FindEtcItem( dwEtcItem ) );
	if( pEtcItemMotion )
	{
		if( bMale )
		{
			m_aMyChar->SetMotionItemAni( pEtcItemMotion->GetHumanManMotion() );
		}
		else
		{
			m_aMyChar->SetMotionItemAni( pEtcItemMotion->GetHumanWomanMotion() );
		}
	}
}

void SelectCharViewWnd::SetCharLoopAnimation( const ioHashString &szAniName, float fFadeIn /*= 0.0f*/  )
{
	if( !m_aMyChar )
		return;

	if( szAniName.IsEmpty() )
		return;

	m_aMyChar->SetLoopAni( szAniName, fFadeIn );
}

void SelectCharViewWnd::SetCharActionAnimation( const ioHashString &szAniName, float fFadeIn /*= FLOAT100*/, float fFadeOut /*= FLOAT100 */ )
{
	if( !m_aMyChar )
		return;

	if( szAniName.IsEmpty() )
		return;

	m_aMyChar->SetActionAni( szAniName, fFadeIn, fFadeOut );
}

void SelectCharViewWnd::SetCharMotionItemAnimation( const ioHashString &szAniName )
{
	if( !m_aMyChar )
		return;

	if( szAniName.IsEmpty() )
		return;

	m_aMyChar->SetMotionItemAni( szAniName );
}

void SelectCharViewWnd::SetChangeSkeleton( DWORD dwEtcItem, bool bMale )
{
	if( !m_aMyChar ) return;
	if( !COMPARE( dwEtcItem, ioEtcItem::EIT_ETC_SKELETON_BIG, ioEtcItem::EIT_ETC_SKELETON_SMALL + 1 ) )
		return;

	ioHashString kEtcItemSkeleton;
	if( dwEtcItem > 0 )
	{
		ioEtcItemSkeleton *pEtcItemSkeleton = dynamic_cast< ioEtcItemSkeleton * >( g_EtcItemMgr.FindEtcItem( dwEtcItem ) );
		if( pEtcItemSkeleton )
		{
			if( bMale )
				kEtcItemSkeleton = pEtcItemSkeleton->GetHumanManSkl();
			else
				kEtcItemSkeleton = pEtcItemSkeleton->GetHumanWomanSkl();
		}
	}

	if( !kEtcItemSkeleton.IsEmpty() )
		m_aMyChar->ChangeSkeleton( kEtcItemSkeleton );
}

void SelectCharViewWnd::EquipItem( DWORD dwItemCode, DWORD dwItemMaleCustom, DWORD dwItemFemaleCustom )
{
	if( m_aMyChar )
	{
		m_aMyChar->EquipItem( dwItemCode, dwItemMaleCustom, dwItemFemaleCustom );
	}
}

void SelectCharViewWnd::ReleaseItem( int iSlot )
{
	if( m_aMyChar )
	{
		m_aMyChar->ReleaseItem( iSlot );
	}
}

void SelectCharViewWnd::InitRotate()
{
	RotateY( 0.0f );
	if( m_aMyChar )
		m_aMyChar->SetYaw( FLOAT10 );
}

void SelectCharViewWnd::RotateY( float fYaw )
{
	m_fRotateYaw = fYaw;
}

void SelectCharViewWnd::SetZPosition( float ZPos )
{
	m_fZPosition = ZPos;
}

void SelectCharViewWnd::SetChar( const CHARACTER &rkInfo )
{
	if( rkInfo.m_class_type <= 0 )
		return;

	int iPreClassType = m_CharInfo.m_class_type;
	int iPreSexType   = m_CharInfo.m_sex;

	m_CharInfo = rkInfo;
	CreateCharacter( m_CharInfo );

	if( iPreClassType != m_CharInfo.m_class_type || iPreSexType != m_CharInfo.m_sex )
		InitRotate();
}

void SelectCharViewWnd::SetEquip( bool bReleaseHelm, bool bReleaseAll, bool bShopMotion )
{	
	for( int i = 0; i < MAX_INVENTORY; ++i )
	{
		int iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + m_CharInfo.m_class_type;
		EquipItem( iItemCode, 0, 0 );
	}

	if( g_GUIMgr.IsShow( MY_INVENTORY_WND ) || g_GUIMgr.IsShow( NEW_SHOP_WND ) )
	{
		int iArray = g_MyInfo.GetClassArray( m_CharInfo.m_class_type );
		for( int i=0; i < MAX_INVENTORY; ++i )
		{
			if( g_GUIMgr.IsShow( NEW_SHOP_WND ) && g_MyInfo.IsCharExerciseStyle( iArray, EXERCISE_RENTAL ) )
			{				
				for(int i = 0;i < MAX_INVENTORY;i++)
				{
					int iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + m_CharInfo.m_class_type;
					EquipItem( iItemCode, 0, 0 );
				}
			}
			else if( COMPARE( iArray, 0, g_MyInfo.GetCharCount() ) )
			{
				const ITEM_DATA &rkItemData = g_MyInfo.GetCharItem( iArray, i );
				const ioItem *pItem = g_ItemMaker.GetItemConst( rkItemData.m_item_code, __FUNCTION__ );
				if( pItem )
				{
					EquipItem( pItem->GetItemCode(), rkItemData.m_item_male_custom, rkItemData.m_item_female_custom );
				}
			}
			else
			{				
				for(int i = 0;i < MAX_INVENTORY;i++)
				{
					int iItemCode = (DEFAULT_BASIC_ITEM_CODE * i) + m_CharInfo.m_class_type;
					EquipItem( iItemCode, 0, 0 );
				}
			}
		}
	}

	SetCharAnimation( bShopMotion );

	if( bReleaseAll )
	{
		ReleaseItem( ioCreateChar::CCS_HELMET );
		ReleaseItem( ioCreateChar::CCS_ARMOR );
		ReleaseItem( ioCreateChar::CCS_CLOAK );
		ReleaseItem( ioCreateChar::CCS_WEAPON );
	}
	else if( bReleaseHelm )
	{
		ReleaseItem( ioCreateChar::CCS_HELMET );
	}

	m_bReleaseHelm    = bReleaseHelm;
	m_bReleaseAllItem = bReleaseAll;
	m_bShopMotion     = bShopMotion;
}

void SelectCharViewWnd::SetCharScale( float fScale )
{
	if( m_aMyChar )
		m_aMyChar->SetScale( fScale );
}

void SelectCharViewWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_CHAR_CONTROL_WND:
		if( cmd == IOBN_BTNDOWN)
		{
			if( param == SelectCharControlWnd::ID_LEFT_ROTATE_BTN )
			{
				RotateY( m_fXMLRotateYaw );				
			}
			else if( param == SelectCharControlWnd::ID_RIGHT_ROTATE_BTN )
			{
				RotateY( -m_fXMLRotateYaw );			
			}
		}
		else if( cmd == IOBN_BTNUP )
		{
			if( param == SelectCharControlWnd::ID_DEFAULT_POS_BTN )
			{
				if( m_aMyChar )
					m_aMyChar->SetYaw( FLOAT10 );				
				InitRotate();
			}
			else if( param == SelectCharControlWnd::ID_LEFT_ROTATE_BTN )
			{
				RotateY( 0.0f );
			}
			else if( param == SelectCharControlWnd::ID_RIGHT_ROTATE_BTN )
			{
				RotateY( 0.0f );
			}
		}
		break;
	}
}

void SelectCharViewWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	if( m_aMyChar )
	{
		m_aMyChar->RotateY( m_fRotateYaw );
		m_aMyChar->Update( fTimePerSec );
	}

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )
		return;

	RECT rcNameRect;
	rcNameRect.left = GetDerivedPosX();
	rcNameRect.top  = GetDerivedPosY();
	rcNameRect.right  = rcNameRect.left + GetWidth();
	rcNameRect.bottom = rcNameRect.top + GetHeight();

	if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
	{
		ShowChildWnd( ID_CHAR_CONTROL_WND );

		if( m_bUserInfoToolTip )
		{
			if( g_GUIMgr.GetPreOverWnd() == this || g_GUIMgr.GetPreOverWnd() == FindChildWnd( ID_CHAR_CONTROL_WND ) )
				pMouse->SetToolTipID( g_MyInfo.GetPublicID(), m_CharInfo.m_class_type );
		}
	}
	else
	{
		SelectCharControlWnd *pControlWnd = dynamic_cast<SelectCharControlWnd*> (FindChildWnd( ID_CHAR_CONTROL_WND ) );
		if( pControlWnd )
			pControlWnd->HideWndAnimation();
	}
}

void SelectCharViewWnd::OnRender()
{
	ioWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	
	D3DRECT rcD3D;
	rcD3D.x1 = iXPos;
	rcD3D.y1 = iYPos;
	rcD3D.x2 = rcD3D.x1 + VIEW_WIDTH;
	rcD3D.y2 = rcD3D.y1 + VIEW_HEIGHT;
	m_pUI3DRender->RenderViewPort( &rcD3D, true );
}
//////////////////////////////////////////////////////////////////////////
SelectCharControlBtn::SelectCharControlBtn()
{
	m_pBtn             = NULL;
}

SelectCharControlBtn::~SelectCharControlBtn()
{
	SAFEDELETE( m_pBtn );
}

void SelectCharControlBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( strcmp(szType.c_str(), "Btn") == 0 )
	{
		SAFEDELETE( m_pBtn );
		m_pBtn = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );	
	}	
}

void SelectCharControlBtn::OnDrawNormal( int iXPos, int iYPos )
{
	ioButton::OnDrawNormal( iXPos, iYPos );
	RenderInfo( iXPos, iYPos , UI_RENDER_NORMAL );
}

void SelectCharControlBtn::OnDrawOvered( int iXPos, int iYPos )
{
	ioButton::OnDrawOvered( iXPos, iYPos );
	RenderInfo( iXPos, iYPos , UI_RENDER_NORMAL );
	RenderInfo( iXPos, iYPos , UI_RENDER_SCREEN );
}

void SelectCharControlBtn::OnDrawPushed( int iXPos, int iYPos )
{
	ioButton::OnDrawPushed( iXPos, iYPos );
	RenderInfo( iXPos, iYPos+2 , UI_RENDER_NORMAL );
	RenderInfo( iXPos, iYPos+2 , UI_RENDER_SCREEN );
}

void SelectCharControlBtn::OnDrawDisabled( int iXPos, int iYPos )
{
	ioButton::OnDrawDisabled( iXPos, iYPos );
	RenderInfo( iXPos, iYPos , UI_RENDER_GRAY );
}

void SelectCharControlBtn::RenderInfo( int iXPos, int iYPos, UIRenderType eType )
{
	if( m_pBtn )
		m_pBtn->Render( iXPos, iYPos, eType );
}

void SelectCharControlBtn::SetBtnAlphaRate( int iAlphaRate )
{
	if( m_pBtn )
		m_pBtn->SetAlpha( (BYTE) iAlphaRate );
}
//////////////////////////////////////////////////////////////////////////

SelectCharControlWnd::SelectCharControlWnd()
{
	InitAniState();
}

SelectCharControlWnd::~SelectCharControlWnd()
{
}

void SelectCharControlWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_LEFT_ROTATE_BTN:
	case ID_RIGHT_ROTATE_BTN:
	case ID_DEFAULT_POS_BTN:
		if( cmd == IOBN_BTNUP || cmd == IOBN_BTNDOWN )
		{
			if( ioWnd::GetParent() )
				ioWnd::GetParent()->iwm_command( this, cmd, dwID );
		}
		break;
	}
}

void SelectCharControlWnd::SetDefaultPosTitle( const char* szTitle )
{
	ioWnd *pWnd = FindChildWnd( ID_DEFAULT_POS_BTN );
	if( pWnd )
		pWnd->SetTitleText( szTitle );
}

void SelectCharControlWnd::InitAniState()
{
	enum { DELAY_ALPHA = 500, };

	FSMState *pState = new FSMState(AT_NONE);
	if( pState )
		pState->Add( AT_ALPHA_IN,  AT_ALPHA_IN );
	m_AniState.Add( pState );

	pState = new FSMState(AT_ALPHA_IN);
	if( pState )
	{
		pState->Add( AT_ALPHA_IN, AT_NORMAL, DELAY_ALPHA );
		pState->Add( AT_ALPHA_OUT, AT_ALPHA_OUT );
	}
	m_AniState.Add( pState );	

	pState = new FSMState(AT_NORMAL);
	if( pState )
	{
		pState->Add( AT_ALPHA_OUT, AT_ALPHA_OUT );
	}
	m_AniState.Add( pState );	

	pState = new FSMState(AT_ALPHA_OUT);
	if( pState )
	{
		pState->Add( AT_ALPHA_OUT, AT_HIDE, DELAY_ALPHA );
	}
	m_AniState.Add( pState );	

	m_AniState.SetCurState( AT_NONE );
}

void SelectCharControlWnd::iwm_show()
{
	m_AniState.SetCurState( AT_ALPHA_IN );
	HideChildWnd( ID_DEFAULT_POS_BTN );
}

void SelectCharControlWnd::iwm_hide()
{
	m_AniState.SetCurState( AT_NONE );
}

void SelectCharControlWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return ;

	m_AniState.UpdateTime();

	if( m_AniState.GetCurState() == AT_ALPHA_IN )
	{
		float fRate = ( (float) m_AniState.GetCurTime() / (float) m_AniState.GetDelay() );
		int iAlpha = 255.0f * fRate;

		SetChildAlpha( iAlpha );
	}
	else if( m_AniState.GetCurState() == AT_ALPHA_OUT )
	{
		float fRate = ( (float) m_AniState.GetCurTime() / (float) m_AniState.GetDelay() );
		int iAlpha = 255.0f * ( FLOAT1 - fRate );

		SetChildAlpha( iAlpha );
	}
	else if( m_AniState.GetCurState() == AT_HIDE )
	{
		HideWnd();
	}
	else
	{
		SetChildAlpha( 255 );
	}
}

void SelectCharControlWnd::SetChildAlpha( int iAlphaRate )
{
	for (int i = 0; i < MAX_BTN ; i++)
	{
		NewShopHelpControlBtn *pBtn = dynamic_cast<NewShopHelpControlBtn*> ( FindChildWnd( ID_LEFT_ROTATE_BTN + i ) );
		if( !pBtn )
			continue;
		pBtn->SetBtnAlphaRate( iAlphaRate );
	}
}

void SelectCharControlWnd::HideWndAnimation()
{
	if( !IsShow() )
		return;
	m_AniState.ChangeState( AT_ALPHA_OUT );
}
