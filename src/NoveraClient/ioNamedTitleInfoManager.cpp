
#include "stdafx.h"

#include "ioNamedTitleInfoManager.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "GUI/MannerValuationWnd.h"
#include "GUI/NamedTitleSelectWnd.h"

template<> ioNamedTitleInfoManager* Singleton< ioNamedTitleInfoManager >::ms_Singleton = 0;

ioNamedTitleInfoManager::ioNamedTitleInfoManager()
{
	m_vInfoList.clear();
	m_pPremiumBaseMark = NULL;
	m_dwCurEquipCode = 0;
	m_dwEquipTime = 0;
}

ioNamedTitleInfoManager::~ioNamedTitleInfoManager()
{
	ClearData();
}

ioNamedTitleInfoManager& ioNamedTitleInfoManager::GetSingleton()
{
	return Singleton< ioNamedTitleInfoManager >::GetSingleton();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNamedTitleInfoManager::ClearData()
{
	m_vInfoList.clear();
	SAFEDELETE( m_pPremiumBaseMark );
}

void ioNamedTitleInfoManager::LoadProperty()
{	
	ioINILoader_e kLoader( "config/sp2_named_title.ini" );

	char szBuf[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";
	char szGroupName[MAX_PATH] = "";

	kLoader.SetTitle_e( "common_info" );

	char *pTemp = NULL;
	ioHashString szColorTemp;
	kLoader.LoadString( "title_text_base_color", "", szBuf, MAX_PATH );
	szColorTemp = szBuf;
	m_dwTextBaseColor = strtoul(szBuf, &pTemp, 16);

	kLoader.LoadString( "title_text_premium_color", "", szBuf, MAX_PATH );
	szColorTemp = szBuf;
	m_dwTextPremiumColor = strtoul(szBuf, &pTemp, 16);
		
	kLoader.LoadString( "title_text_equip_color", "", szBuf, MAX_PATH );
	szColorTemp = szBuf;
	m_dwTextEquipColor = strtoul(szBuf, &pTemp, 16);
	
	kLoader.LoadString( "title_red_base_color", "", szBuf, MAX_PATH );
	szColorTemp = szBuf;
	m_dwRedBaseEquipColor = strtoul(szBuf, &pTemp, 16);

	kLoader.LoadString( "title_blue_base_color", "", szBuf, MAX_PATH );
	szColorTemp = szBuf;
	m_dwBlueBaseEquipColor = strtoul(szBuf, &pTemp, 16);
	
	kLoader.LoadString( "named_title_icon", "", szBuf, MAX_PATH );
	m_szNamedTitleIcon = szBuf;

	kLoader.LoadString( "named_title_premium_icon", "", szBuf, MAX_PATH );
	m_szNamedTitlePremiumIcon = szBuf;

	kLoader.LoadString( "named_title_ui_mark", "", szBuf, MAX_PATH );
	m_szNamedTitleUIMark = szBuf;

	kLoader.LoadString( "named_title_premium_base_mark", "", szBuf, MAX_PATH );
	m_pPremiumBaseMark = g_UIImageSetMgr.CreateImageByFullName( szBuf );

	m_dwEquipRefreshTime = kLoader.LoadInt_e( "named_title_refresh_time", 0 );

	int nGroupCnt = kLoader.LoadInt_e( "named_title_group_cnt", 0 );
	for( int i=0; i<nGroupCnt; ++i )
	{
		wsprintf_e( szTitle, "named_title_group%d", i+1 );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		wsprintf_e( szGroupName, "config/%s", szBuf );
		
		ioINILoader kSetNamedTitleLoader( szGroupName );
		kSetNamedTitleLoader.SetTitle_e( "common_info" );

		int iCnt = kSetNamedTitleLoader.LoadInt_e( "named_title_cnt", 0 );
		for( int j=0; j < iCnt; ++j )
		{
			wsprintf_e( szTitle, "named_title%d", j+1 );
			kSetNamedTitleLoader.SetTitle( szTitle );

			LoadBase( kSetNamedTitleLoader );
		}
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNamedTitleInfoManager::LoadBase( ioINILoader &rkLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	DWORD dwCode = rkLoader.LoadInt_e( "title_code", 0 );
	if ( dwCode <= 0 )
		return;

	NamedTitleInfo sNamedTitleInfo;
	sNamedTitleInfo.m_dwCode = dwCode;

	rkLoader.LoadString_e( "title_name", "", szBuf, MAX_PATH );
	sNamedTitleInfo.m_szName = szBuf;
	
	int nLevelCnt = rkLoader.LoadInt_e( "title_grade_cnt", 0 );
	char *pTemp = NULL;
	ioHashString szColorTemp;
	for ( int i=0; i<nLevelCnt; i++ )
	{
		NamedTitleGrade sNamedTitleGrade;

		wsprintf_e( szKey, "title_grade%d_color", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		szColorTemp = szBuf;
		pTemp = NULL;
		sNamedTitleGrade.m_dwColor = strtoul(szBuf, &pTemp, 16);
		
		wsprintf_e( szKey, "title_grade%d_mark", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sNamedTitleGrade.m_szMark = szBuf;

		sNamedTitleInfo.m_GradeList.push_back( sNamedTitleGrade );
	}

	m_vInfoList.insert( std::make_pair( dwCode, sNamedTitleInfo ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool ioNamedTitleInfoManager::GetNamedTitleInfo( DWORD dwCode, NamedTitleInfo &sNamedTitleInfo )
{
	sNamedTitleInfo.Init();

	if ( dwCode <= 0 || m_vInfoList.empty() )
		return false;
	
	NamedTitleInfoList::iterator FindIter = m_vInfoList.find( dwCode );
	if ( FindIter != m_vInfoList.end() )
	{
		const NamedTitleInfo &rNamedTitleInfo = FindIter->second;
		sNamedTitleInfo = rNamedTitleInfo;
		return true;
	}

	return false;
}

bool ioNamedTitleInfoManager::GetNamedTitleGrade( DWORD dwCode, int nLevel, NamedTitleGrade &sNamedTitleGrade )
{
	sNamedTitleGrade.Init();

	if ( dwCode <= 0 || m_vInfoList.empty() )
		return false;

	NamedTitleInfoList::iterator FindIter = m_vInfoList.find( dwCode );
	if ( FindIter != m_vInfoList.end() )
	{
		const NamedTitleInfo &rNamedTitleInfo = FindIter->second;
		if ( !COMPARE( nLevel, 0, (int)rNamedTitleInfo.m_GradeList.size() ) )
			return false;

		sNamedTitleGrade = rNamedTitleInfo.m_GradeList[nLevel];
		return true;
	}

	return false;
}

bool ioNamedTitleInfoManager::IsEnableUpgrade( DWORD dwCode, int nLevel )
{
	if ( dwCode <= 0 || m_vInfoList.empty() )
		return false;

	NamedTitleInfoList::iterator FindIter = m_vInfoList.find( dwCode );
	if ( FindIter != m_vInfoList.end() )
	{
		const NamedTitleInfo &rNamedTitleInfo = FindIter->second;
		int nGradeCnt = (int)rNamedTitleInfo.m_GradeList.size();
		if ( nGradeCnt > 1 && nLevel + 1 < nGradeCnt )
			return true;
	}

	return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ioHashString ioNamedTitleInfoManager::GetNamedTitleName( DWORD dwCode )
{
	static ioHashString szNull;

	if ( dwCode <= 0 || m_vInfoList.empty() )
		return szNull;

	NamedTitleInfoList::iterator FindIter = m_vInfoList.find( dwCode );
	if ( FindIter != m_vInfoList.end() )
	{
		const NamedTitleInfo &rNamedTitleInfo = FindIter->second;
		return rNamedTitleInfo.m_szName;
	}

	return szNull;
}

ioHashString ioNamedTitleInfoManager::GetNamedTitleIcon()
{
	return m_szNamedTitleIcon;
}

ioHashString ioNamedTitleInfoManager::GetNamedTitlePremiumIcon()
{
	return m_szNamedTitlePremiumIcon;
}

ioHashString ioNamedTitleInfoManager::GetNamedTitleUiMark()
{
	return m_szNamedTitleUIMark;
}

DWORD ioNamedTitleInfoManager::GetNamedTitleTextColor( DWORD dwCode, int nLevel, BYTE byPremium, BYTE byEquip )
{
	if ( byEquip )
		return m_dwTextEquipColor;
	else if ( byPremium )
		return m_dwTextPremiumColor;

	return m_dwTextBaseColor;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNamedTitleInfoManager::RenderNamedTitle( int nXPos, int nYPos, float fFontSize, DWORD dwCode, int nLevel, BYTE byPremium, TeamType eTeam, ioUIRenderImage* pCurMark, UIRenderType eRenderType )
{
	if ( dwCode <= 0 )
		return;

	NamedTitleInfo sNamedTitleInfo;
	if ( !GetNamedTitleInfo( dwCode, sNamedTitleInfo ) )
		return;
	
	DWORD dwColor = 0;
	ioUIRenderImage* pPremiumMark = m_pPremiumBaseMark;
	if ( byPremium )
	{
		if ( COMPARE( nLevel, 0, (int)sNamedTitleInfo.m_GradeList.size() ) )
			dwColor = sNamedTitleInfo.m_GradeList[nLevel].m_dwColor;
		
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( dwColor );
		g_FontMgr.PrintText( nXPos, nYPos, fFontSize, "%s", sNamedTitleInfo.m_szName.c_str() );

		if ( pCurMark )
			pPremiumMark = pCurMark;
	}
	else
	{
		switch( eTeam )
		{
		case TEAM_BLUE:
			dwColor = m_dwBlueBaseEquipColor;
			break;
		case TEAM_RED:
			dwColor = m_dwRedBaseEquipColor;
			break;
		}

		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( dwColor );
		g_FontMgr.PrintText( nXPos, nYPos, fFontSize, "[%s]", sNamedTitleInfo.m_szName.c_str() );
	}
	
	if( byPremium && pPremiumMark )
	{
		int nNamedTitleHalfWidth = ( g_FontMgr.GetTextWidth( sNamedTitleInfo.m_szName.c_str(), TS_BOLD_OUTLINE_FULL, fFontSize ) + pPremiumMark->GetWidth() ) / 2 + 1;
		pPremiumMark->SetColor( dwColor );
		pPremiumMark->Render( nXPos - nNamedTitleHalfWidth, nYPos, eRenderType, TFO_BILINEAR );
		pPremiumMark->SetReverseFormatting( ioUIRenderImage::RF_HORZ_REVERSE );
		pPremiumMark->Render( nXPos + nNamedTitleHalfWidth, nYPos, eRenderType, TFO_BILINEAR );
		pPremiumMark->SetReverseFormatting( ioUIRenderImage::RF_NONE_REVERSE );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 칭호 관련 (칭호 데이터 교체)
void ioNamedTitleInfoManager::OnNamedTitleUpdate( SP2Packet &rkPacket, ioPlayStage* pPlayStage )
{	
	TCPNetwork::MouseBusy( false );

	byte eUpdateType;
	rkPacket >> eUpdateType;

	if ( eUpdateType == TUT_INSERT )
		OnNamedTitleInsertSuccess( rkPacket );
	else if ( eUpdateType == TUT_UPDATE )
		OnNamedTitleUpdateSuccess( rkPacket );
	else if ( eUpdateType == TUT_LEVELUP )
		OnNamedTitleLevelUpSuccess( rkPacket, pPlayStage );
	else if ( eUpdateType == TUT_PREMIUM )
		OnNamedTitlePremiumSuccess( rkPacket, pPlayStage );
	else if ( eUpdateType == TUT_EQUIP || eUpdateType == TUT_ALL_RELEASE )
		OnNamedTitleChange( rkPacket, pPlayStage );
}

void ioNamedTitleInfoManager::OnNamedTitleChange( SP2Packet &rkPacket, ioPlayStage* pPlayStage )
{
	TCPNetwork::MouseBusy( false );

	if ( !pPlayStage )
		return;

	ioHashString szOwner;
	rkPacket >> szOwner;

	DWORD dwCode;
	__int64 nValue;
	int nLevel;
	BYTE byPremium, byEquip, byStatus;

	rkPacket >> dwCode >> nValue >> nLevel;
	rkPacket >> byPremium >> byEquip >> byStatus;

	// 교체 대상이 자신일때
	if( szOwner == g_MyInfo.GetPublicID() )
	{
		g_MyInfo.ApplyEquipChangeNamedTitle( dwCode, byEquip );

		NamedTitleSelectWnd *pSelectWnd = dynamic_cast<NamedTitleSelectWnd*>( g_GUIMgr.FindWnd( NAMED_TITLE_SELECT_WND ) );
		if( pSelectWnd && pSelectWnd->IsShow() )
			pSelectWnd->Update();
	}

	// 유저 노드 적용
	UserInfoNode *pInfo = g_UserInfoMgr.GetUserInfoNode( szOwner, false );
	if ( pInfo )
	{
		if ( byEquip )
			pInfo->SetNamedTitle( dwCode, nLevel, byPremium );
		else
			pInfo->SetNamedTitle( 0, 0, false );
	}

	// 캐릭터 적용
	ioBaseChar *pOwner = pPlayStage->GetBaseChar( szOwner );
	if ( pOwner )
	{
		if ( byEquip )
			pOwner->SetNamedTitle( dwCode, nLevel, byPremium );
		else
			pOwner->SetNamedTitle( 0, 0, false );
	}

	MannerValuationExtensionWnd *pManner = dynamic_cast<MannerValuationExtensionWnd *>( g_GUIMgr.FindWnd( MANNER_VALUATION_WND ) );
	if( pManner && pManner->IsShow() )
		pManner->ApplyTitle();
}

void ioNamedTitleInfoManager::OnNamedTitlePremium( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int nResult;
	rkPacket >> nResult;

	if ( nResult == SET_PREMIUM_TITLE_SUCCESS )
	{
		DWORD dwCode;
		rkPacket >> dwCode;
		
		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
		if( pInvenWnd && pInvenWnd->IsShow() )
			pInvenWnd->ShowNamedTitlePremiumResultWnd( dwCode, 0, 1 );
	}
	else
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "칭호 프리미엄 실패: %d", nResult );
}

void ioNamedTitleInfoManager::OnNamedTitleEtcUse( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	
	int nResult;
	rkPacket >> nResult;

	if ( nResult == TITLE_ETC_USE_SUCCESS )
	{
		DWORD dwCode;
		rkPacket >> dwCode;

		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
		if( pInvenWnd && pInvenWnd->IsShow() )
			pInvenWnd->ShowNamedTitleResultWnd( dwCode, 0, 0 );
	}
	else
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "칭호 특별아이템 사용 실패: %d", nResult );
}

void ioNamedTitleInfoManager::OnNamedTitleEquip( SP2Packet &rkPacket, ioPlayStage* pPlayStage )
{
	TCPNetwork::MouseBusy( false );

	if ( !pPlayStage )
		return;

	ioHashString szOwner;
	rkPacket >> szOwner;

	DWORD dwCode;
	BYTE byPremium;
	int nLevel;

	rkPacket >> dwCode >> byPremium >> nLevel;
	
	// 유저 노드 적용
	UserInfoNode *pInfo = g_UserInfoMgr.GetUserInfoNode( szOwner, false );
	if ( pInfo )
		pInfo->SetNamedTitle( dwCode, nLevel, byPremium );

	// 캐릭터 적용
	ioBaseChar *pOwner = pPlayStage->GetBaseChar( szOwner );
	if ( pOwner )
		pOwner->SetNamedTitle( dwCode, nLevel, byPremium );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// 칭호 관련 (칭호 데이터 추가)
void ioNamedTitleInfoManager::OnNamedTitleInsertSuccess( SP2Packet &rkPacket )
{
	ioHashString szOwner;
	rkPacket >> szOwner;

	DWORD dwCode;
	__int64 nValue;
	int nLevel;
	BYTE byPremium, byEquip, byStatus;
	
	rkPacket >> dwCode >> nValue >> nLevel;
	rkPacket >> byPremium >> byEquip >> byStatus;

	ioUserNamedTitle *pUserNamedTitle = g_MyInfo.GetUserNamedTitle();
	if( !pUserNamedTitle )
		return;

	NamedTitleSlot kSlot;
	kSlot.m_dwCode = dwCode;
	kSlot.m_nValue = nValue;
	kSlot.m_nLevel = nLevel;
	kSlot.m_byPremium = byPremium;
	kSlot.m_byEquip = byEquip;
	kSlot.m_byStatus = byStatus;

	if ( !pUserNamedTitle->AddNamedTitleSlotData( kSlot ) )
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "칭호 추가 실패" );
}

// 칭호 관련 (칭호 데이터 변경)
void ioNamedTitleInfoManager::OnNamedTitleUpdateSuccess( SP2Packet &rkPacket )
{
	ioHashString szOwner;
	rkPacket >> szOwner;

	DWORD dwCode;
	__int64 nValue;
	int nLevel;
	BYTE byPremium, byEquip, byStatus;

	rkPacket >> dwCode >> nValue >> nLevel;
	rkPacket >> byPremium >> byEquip >> byStatus;
	
	ioUserNamedTitle *pUserNamedTitle = g_MyInfo.GetUserNamedTitle();
	if( !pUserNamedTitle )
		return;

	NamedTitleSlot kSlot;
	kSlot.m_dwCode = dwCode;
	kSlot.m_nValue = nValue;
	kSlot.m_nLevel = nLevel;
	kSlot.m_byPremium = byPremium;
	kSlot.m_byEquip = byEquip;
	kSlot.m_byStatus = byStatus;

	if ( !pUserNamedTitle->SetNamedTitleSlot( kSlot ) )
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "칭호 정보 변경 실패" );

	NamedTitleSelectWnd *pSelectWnd = dynamic_cast<NamedTitleSelectWnd*>( g_GUIMgr.FindWnd( NAMED_TITLE_SELECT_WND ) );
	if( pSelectWnd && pSelectWnd->IsShow() )
		pSelectWnd->Update();
}

// 칭호 관련 (칭호 등급 변경 완료)
void ioNamedTitleInfoManager::OnNamedTitleLevelUpSuccess( SP2Packet &rkPacket, ioPlayStage* pPlayStage )
{
	if ( !pPlayStage )
		return;

	ioHashString szOwner;
	rkPacket >> szOwner;

	DWORD dwCode;
	__int64 nValue;
	int nLevel;
	BYTE byPremium, byEquip, byStatus;

	rkPacket >> dwCode >> nValue >> nLevel;
	rkPacket >> byPremium >> byEquip >> byStatus;
	
	// 교체 대상이 자신일때
	if( szOwner == g_MyInfo.GetPublicID() )
	{
		g_MyInfo.ApplyGradeChangeNamedTitle( dwCode, nLevel );

		NamedTitleSelectWnd *pSelectWnd = dynamic_cast<NamedTitleSelectWnd*>( g_GUIMgr.FindWnd( NAMED_TITLE_SELECT_WND ) );
		if( pSelectWnd && pSelectWnd->IsShow() )
			pSelectWnd->Update();
	}

	if ( byEquip )
	{
		// 유저 노드 적용
		UserInfoNode *pInfo = g_UserInfoMgr.GetUserInfoNode( szOwner, false );
		if ( pInfo )
			pInfo->SetNamedTitle( dwCode, nLevel, byPremium );

		// 캐릭터 적용
		ioBaseChar *pOwner = pPlayStage->GetBaseChar( szOwner );
		if ( pOwner )
			pOwner->SetNamedTitle( dwCode, nLevel, byPremium );
	}

	MannerValuationExtensionWnd *pManner = dynamic_cast<MannerValuationExtensionWnd *>( g_GUIMgr.FindWnd( MANNER_VALUATION_WND ) );
	if( pManner && pManner->IsShow() )
		pManner->ApplyTitle();

}

// 칭호 관련 (칭호 프리미엄 변경 완료)
void ioNamedTitleInfoManager::OnNamedTitlePremiumSuccess( SP2Packet &rkPacket, ioPlayStage* pPlayStage )
{
	if ( !pPlayStage )
		return;

	ioHashString szOwner;
	rkPacket >> szOwner;

	DWORD dwCode;
	__int64 nValue;
	int nLevel;
	BYTE byPremium, byEquip, byStatus;

	rkPacket >> dwCode >> nValue >> nLevel;
	rkPacket >> byPremium >> byEquip >> byStatus;
	
	// 교체 대상이 자신일때
	if( szOwner == g_MyInfo.GetPublicID() )
		g_MyInfo.ApplyPremiumChangeNamedTitle( dwCode );

	if ( byEquip )
	{
		// 유저 노드 적용
		UserInfoNode *pInfo = g_UserInfoMgr.GetUserInfoNode( szOwner, false );
		if ( pInfo )
			pInfo->SetNamedTitle( dwCode, nLevel, byPremium );

		// 캐릭터 적용
		ioBaseChar *pOwner = pPlayStage->GetBaseChar( szOwner );
		if ( pOwner )
			pOwner->SetNamedTitle( dwCode, nLevel, byPremium );
	}

	MannerValuationExtensionWnd *pManner = dynamic_cast<MannerValuationExtensionWnd *>( g_GUIMgr.FindWnd( MANNER_VALUATION_WND ) );
	if( pManner && pManner->IsShow() )
		pManner->ApplyTitle();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioNamedTitleInfoManager::Process()
{
	ioUserNamedTitle *pUserNamedTitle = g_MyInfo.GetUserNamedTitle();
	if ( !pUserNamedTitle )
		return;

	NamedTitleSlot kEquipSlot;
	if ( !pUserNamedTitle->GetNamedTitleSlotEquip( kEquipSlot) )
		return;

	if ( !kEquipSlot.m_byPremium || !kEquipSlot.m_byEquip || kEquipSlot.m_dwCode <= 0 || !IsEnableUpgrade( kEquipSlot.m_dwCode, kEquipSlot.m_nLevel ) )
		return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCurEquipCode != kEquipSlot.m_dwCode )
	{
		m_dwCurEquipCode = kEquipSlot.m_dwCode;
		m_dwEquipTime = dwCurTime;
	}

	if ( m_dwCurEquipCode > 0 && m_dwEquipTime > 0 && m_dwEquipTime + m_dwEquipRefreshTime < dwCurTime )
	{
		m_dwEquipTime = dwCurTime;

		SP2Packet kPacket( CTPK_TITLE_SYNC_EQUIP_TIME );
		kPacket << m_dwCurEquipCode;
		TCPNetwork::SendToServer( kPacket );
	}	
}