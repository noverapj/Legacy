#include "StdAfx.h"

#include "../ioKeyManager.h"
#include "../WndID.h"
#include "../TextColorTable.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/ioButton.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioUITitle.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3DEngine/ioXMLDocument.h"
#include "../io3DEngine/ioXMLElement.h"
#include "../io3DEngine/ioMovingWnd.h"

#include "../ioApplication.h"
#include "../ioMyInfo.h"
#include "../ioMyRoomMode.h"

#include "ioSP2GUIManager.h"
#include "ioInterfaceBackground.h"
#include "GameOptionWnd.h"
#include "GameOptionSettingTab.h"
#include "ItemCustomWnd.h"
#include ".\filelistwnd.h"

#include <strsafe.h>


FileBtn::FileBtn()
{
}

FileBtn::~FileBtn()
{
}

void FileBtn::iwm_create()
{
	if( m_pUITitle )
	{
		m_pUITitle->SetWidthCutSize( 288.0f );
		m_pUITitle->SetPrintTextStyle( ioUITitle::PTS_WIDTHCUT );
	}
}
//////////////////////////////////////////////////////////////////////////////////
FileListItem::FileListItem()
{
	m_bFolder = false;
}

FileListItem::~FileListItem()
{

}

void FileListItem::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_FILENAME_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->iwm_command( this, cmd, ID_PARAM_FILENAME_BTNUP );
		}
		break;
	}
}

void FileListItem::SetInfo( const char *szFileName, bool bFolder, bool bShowPlayBtn )
{
	m_bFolder = bFolder;

	ioWnd *pWnd = FindChildWnd( ID_FILENAME_BTN );
	if( !pWnd )
		return;
	
	if( m_bFolder )
	{
		char szFolderName[MAX_PATH]="";
		StringCbPrintf( szFolderName, sizeof( szFolderName ), "[%s]", szFileName );

		ioUITitle *pTitle = pWnd->GetTitle();
		if( pTitle )
		{
			pTitle->SetColor( "Normal", 0xFF3A3A3A, 0xFF000000 );
			pTitle->SetColor( "Over", 0xFFFFFFFF, 0xFF000000 );
			pTitle->SetColor( "Push", 0xFFFFFFFF, 0xFF000000 );
			pTitle->SetText( szFolderName, "" );
		}
		
	}
	else
	{
		ioUITitle *pTitle = pWnd->GetTitle();
		if( pTitle )
		{
			pTitle->SetColor( "Normal", 0xFFCE4E00, 0xFF000000 );
			pTitle->SetColor( "Over", 0xFFFFFFFF, 0xFF000000 );
			pTitle->SetColor( "Push", 0xFFFFFFFF, 0xFF000000 );
			pTitle->SetText( szFileName, "" );
		}
	}

	if( strcmp( szFileName, "") == 0 )
		pWnd->SetInActive();
	else
		pWnd->SetActive();
}

void FileListItem::GetFileName( OUT ioHashString &rszFileName )
{
	ioWnd *pWnd = FindChildWnd( ID_FILENAME_BTN );
	if( !pWnd )
		return;
	ioUITitle *pTitle = pWnd->GetTitle();
	if( !pTitle )
		return;

	pTitle->GetFirstText( rszFileName );
}
//////////////////////////////////////////////////////////////////////////
FileDriveListWnd::FileDriveListWnd()
{
	m_pDarkLine	= NULL;
	m_pLightLine= NULL;
	m_pOverLine	= NULL;

	m_iAddWidth	= 0;
	m_iAddHeight= 0;
	m_iListXPos = 0;
	m_iListYPos = 0;
	m_iStartXPos= 0;
	m_iStartYPos= 0;
	m_iHeightGap= 0;
	m_iCutWidth = 0;
	m_iCurPos   = 0;
}

FileDriveListWnd::~FileDriveListWnd()
{
	SAFEDELETE( m_pDarkLine	);
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine	);
}

void FileDriveListWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_iAddWidth	= xElement.GetIntAttribute_e( "AddWidth" );
	m_iAddHeight= xElement.GetIntAttribute_e( "AddHeight" );
	m_iListXPos = xElement.GetIntAttribute_e( "ListX" );
	m_iListYPos = xElement.GetIntAttribute_e( "ListY" );
	m_iHeightGap= xElement.GetIntAttribute_e( "HeightGap" );
	m_iCutWidth = xElement.GetIntAttribute_e( "CutWidth" );
}

void FileDriveListWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DarkLine" )
	{
		SAFEDELETE( m_pDarkLine );
		m_pDarkLine = pImage;
	}
	else if( szType == "LightLine" )
	{
		SAFEDELETE( m_pLightLine );
		m_pLightLine = pImage;
	}
	else if( szType == "OverLine" )
	{
		SAFEDELETE( m_pOverLine );
		m_pOverLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void FileDriveListWnd::ShowList( vFileDriveItem &rList )
{
	if( rList.empty() )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	m_FileDriveItemList.clear();
	m_FileDriveItemList.resize( rList.size() );
	std::copy( rList.begin(), rList.end(), m_FileDriveItemList.begin() );
	ShowWnd();
}

void FileDriveListWnd::iwm_show()
{	
	if( m_FileDriveItemList.empty() )
		HideWnd();
	else
	{
		m_OverDriveItem.m_szName.Clear();

		m_iCurPos = 0;
		if( (int)m_FileDriveItemList.size() <= MAX_PAGE_LIST )
		{
			HideChildWnd( ID_VERT_SCROLL );
		}
		else
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
			{
				pScroll->SetScrollRange( 0, m_FileDriveItemList.size() );
				pScroll->SetScrollPos( m_iCurPos );
				pScroll->ShowWnd();
			}
		}

		// 윈도우 사이즈 결정	
		SetSize( GetWidth(), ( min( MAX_PAGE_LIST, m_FileDriveItemList.size() ) * m_iHeightGap ) + m_iAddHeight );
		SetCheckParentOutSide( true );
	}
}

void FileDriveListWnd::iwm_hide()
{
	SetCheckParentOutSide( false );
}

void FileDriveListWnd::iwm_wheel( int zDelta )
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

void FileDriveListWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll == NULL ) return;
	if( !pScroll->IsShow() ) return;

	m_iCurPos = curPos;
}

void FileDriveListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )	return;

	m_OverDriveItem.m_szName.Clear();

	// 다른 곳에서 클릭이 일어났는지 확인
	ioWnd *pPreClick = g_GUIMgr.GetPreClickWnd();
	if( pPreClick && GetParent() )
	{
		if( pPreClick != this && 
			pPreClick != GetParent()->FindChildWnd( FileListWnd::ID_SELECT_DISK ) &&
			pPreClick != FindChildWnd( ID_VERT_SCROLL ) &&
			pPreClick->GetParent() != FindChildWnd( ID_VERT_SCROLL ) )
		{
			HideWnd();
			return;
		}
	}

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	if( (int)m_FileDriveItemList.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	ioLSMouse *pMouse = g_App.GetMouse();
	int iListSize = m_FileDriveItemList.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= MAX_PAGE_LIST )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		RECT rcNameRect;
		rcNameRect.left = iXPos + m_iListXPos;
		rcNameRect.top  = iYPos + m_iListYPos + iIndex * m_iHeightGap;
		rcNameRect.right  = rcNameRect.left + ( ( GetWidth() - m_iAddWidth ) - iCutWidth );
		rcNameRect.bottom = rcNameRect.top + m_iHeightGap;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			m_OverDriveItem  = m_FileDriveItemList[iListPos];
			if( pMouse->IsLBtnUp() )
			{
				if( GetParent() )
					GetParent()->iwm_command( this, IOBN_BTNUP, 0 );
				HideWnd();
			}
			break;
		}
	}
}

void FileDriveListWnd::OnRender()
{
	ioWnd::OnRender();

	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	int iCutWidth = 0;
	if( (int)m_FileDriveItemList.size() > MAX_PAGE_LIST )
		iCutWidth = m_iCutWidth;

	bool bDarkRender = false;
	int iListSize = m_FileDriveItemList.size();
	for(int iIndex = 0 ; iIndex < iListSize ; ++iIndex )
	{
		if( iIndex >= MAX_PAGE_LIST )
			break;

		int iListPos = m_iCurPos + iIndex;
		if( iListPos >= iListSize )
			break;

		bDarkRender = !bDarkRender;
		if( m_OverDriveItem.m_iMagicCode == m_FileDriveItemList[iListPos].m_iMagicCode )
		{
			m_pOverLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pOverLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pOverLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pOverLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pDarkLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pDarkLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pDarkLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->SetSize( ( GetWidth() - m_iAddWidth ) - iCutWidth, 21 );
			m_pLightLine->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pLightLine->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			m_pLightLine->Render( iXPos + m_iListXPos, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		g_FontMgr.PrintTextWidthCut( iXPos + m_iListXPos + 5, iYPos + m_iListYPos + ( iIndex * m_iHeightGap ) + 3, FONT_SIZE_12, 181.0f - iCutWidth, 
									 m_FileDriveItemList[iListPos].m_szName.c_str() );
	}
}
//////////////////////////////////////////////////////////////////////////
FileListWnd::FileListWnd(void)
{
	m_iCurPage = 0;
	m_iMaxPage = 0;
	m_pReturnWnd  = NULL;
	m_eReturnType = RWT_NONE;
	m_pDescIcon   = NULL;
}

FileListWnd::~FileListWnd(void)
{
	ClearFileList();
	m_pReturnWnd = NULL;
	SAFEDELETE( m_pDescIcon );
}

void FileListWnd::iwm_create()
{
}

bool FileListWnd::iwm_esc()
{
	ioWnd *pWnd = FindChildWnd( ID_PULLDOWN_WND );
	if( pWnd && pWnd->IsShow() )
	{
		pWnd->HideWnd();
		return true;
	}

	return ioWnd::iwm_esc();
}

void FileListWnd::SetFileList( const char *szRootFolder, const char *szFileExt )
{
	ClearFileList();
	m_szRootFolder = szRootFolder;
	m_szFileExt    = szFileExt;

	if( strlen( szRootFolder ) == 2 ) // c:
		SetChildInActive( ID_PARENT_FOLDER );
	else
		SetChildActive( ID_PARENT_FOLDER );

	HANDLE			hSrch;
	WIN32_FIND_DATA wfd;
	BOOL			bResult = TRUE;

	char szFindPath[MAX_PATH*2]="";
	StringCbPrintf( szFindPath, sizeof(szFindPath), "%s\\*.*", szRootFolder );
	hSrch = FindFirstFile(szFindPath, &wfd);

	if(hSrch == INVALID_HANDLE_VALUE)
		return;

	while(bResult)
	{
		FileInfo kInfo;
		if( (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) )
			kInfo.m_bFolder = true;
		else
			kInfo.m_bFolder = false;

		char szCurExt[MAX_PATH]="";
		_splitpath( wfd.cFileName, NULL, NULL, NULL, szCurExt );

		bool bRight = true;
		if( strcmp( wfd.cFileName, "") == 0 )
			bRight = false;
		else if( strcmp( wfd.cFileName, ".") == 0 )
			bRight = false;
		else if( strcmp( wfd.cFileName, "..") == 0 )
			bRight = false;
		else if( kInfo.m_bFolder == false && stricmp( m_szFileExt.c_str(), &szCurExt[1] ) != 0 ) // szCurExt에는 .Wav로 되어 있어 .을 생략
			bRight = false;

		if( bRight )
		{
			kInfo.m_szFileName = wfd.cFileName;
			m_vFileList.push_back( kInfo );	
		}

		bResult = FindNextFile(hSrch, &wfd);
	}
	FindClose(hSrch);

	std::sort( m_vFileList.begin(), m_vFileList.end(), FileListInfoSort() );
}

void FileListWnd::ClearFileList()
{
	for(FileInfoVector::iterator iter = m_vFileList.begin(); iter != m_vFileList.end(); ++iter)
	{
	    FileInfo &rFileInfo = *iter;
		rFileInfo.m_szFileName.Clear();
	}
	m_vFileList.clear();
}

void FileListWnd::UpdateShowFileList()
{
	int iCurStartArray = m_iCurPage * MAX_LIST;
	int iMaxArray      = m_vFileList.size();

	for (int i = 0; i < MAX_LIST ; i++)
	{
		int iCurArray = iCurStartArray + i;
		FileListItem *pItem = dynamic_cast<FileListItem*> ( FindChildWnd( ID_LIST_1 + i ) );
		if( !pItem )
			continue;
		if( iCurArray >= iMaxArray )
		{  
			pItem->SetInfo( "", false, false );
			pItem->SetInActive();
			continue;
		}

		if( m_vFileList[iCurArray].m_szFileName.IsEmpty() )
		{
			pItem->SetInfo( "", false, false );
			pItem->SetInActive();
			continue;
		}

		bool bShowPlayBtn = false;
		if( m_eReturnType == RWT_COUSTOMSOUND_SETTING_ITEM_WND )
			bShowPlayBtn  = true;
		pItem->SetInfo( m_vFileList[iCurArray].m_szFileName.c_str(), m_vFileList[iCurArray].m_bFolder, bShowPlayBtn );
		pItem->SetActive();
	}
}

void FileListWnd::iwm_show()
{
	SetScrollRange();
	UpdateShowFileList();
	SetShowWndPos();
	SetDefaultDriveName();

	switch( m_eReturnType )
	{
	case RWT_LOBBY_BACK_CHANGE_WND:
		{
			ShowChildWnd( ID_BACKGROUND_CLEAR );
			if( g_BackgroundMgr.IsCustomLobbyBack() )
				SetChildWndStyleRemove( ID_BACKGROUND_CLEAR, IWS_EXACTIVE );
			else
				SetChildWndStyleAdd( ID_BACKGROUND_CLEAR, IWS_EXACTIVE );
		}
		break;
	case RWT_LOADING_BACK_CHANGE_WND:
		{
			ShowChildWnd( ID_BACKGROUND_CLEAR );
			if( g_BackgroundMgr.IsCustomLoadingBack() )
				SetChildWndStyleRemove( ID_BACKGROUND_CLEAR, IWS_EXACTIVE );
			else
				SetChildWndStyleAdd( ID_BACKGROUND_CLEAR, IWS_EXACTIVE );
		}
		break;
	case RWT_BATTLE_BACK_CHANGE_WND:
		{
			ShowChildWnd( ID_BACKGROUND_CLEAR );
			if( g_BackgroundMgr.IsCustomBattleBack() )
				SetChildWndStyleRemove( ID_BACKGROUND_CLEAR, IWS_EXACTIVE );
			else
				SetChildWndStyleAdd( ID_BACKGROUND_CLEAR, IWS_EXACTIVE );
		}
		break;
	case RWT_LADDER_BACK_CHANGE_WND:
		{
			ShowChildWnd( ID_BACKGROUND_CLEAR );
			if( g_BackgroundMgr.IsCustomLadderBack() )
				SetChildWndStyleRemove( ID_BACKGROUND_CLEAR, IWS_EXACTIVE );
			else
				SetChildWndStyleAdd( ID_BACKGROUND_CLEAR, IWS_EXACTIVE );
		}
		break;
	case RWT_MODE_FIND_BACK_CHANGE_WND:
		{
			ShowChildWnd( ID_BACKGROUND_CLEAR );
			if( g_BackgroundMgr.IsCustomLobbyBack() )
				SetChildWndStyleRemove( ID_BACKGROUND_CLEAR, IWS_EXACTIVE );
			else
				SetChildWndStyleAdd( ID_BACKGROUND_CLEAR, IWS_EXACTIVE );
		}
	default:
		HideChildWnd( ID_BACKGROUND_CLEAR );
		break;
	}
}

void FileListWnd::iwm_hide()
{
	ClearFileList();
	m_pReturnWnd = NULL;
	m_szRootFolder.Clear();
	m_szFileExt.Clear();
}

void FileListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_LIST_1:
	case ID_LIST_2:
	case ID_LIST_3:
	case ID_LIST_4:
	case ID_LIST_5:
	case ID_LIST_6:
	case ID_LIST_7:
	case ID_LIST_8:
	case ID_LIST_9:
	case ID_LIST_10:
	case ID_LIST_11:
	case ID_LIST_12:
	case ID_LIST_13:
		if( cmd == IOBN_BTNUP )
		{
			if( param == FileListItem::ID_PARAM_FILENAME_BTNUP )
			{
				SetFileNameBtnUP( dwID );
			}
		}
		break;
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	case ID_PARENT_FOLDER:
		if( cmd == IOBN_BTNUP )
		{
			SetParentFolderFileList();
		}
		break;
	case ID_SELECT_DISK:
		if( cmd == IOBN_BTNUP )
		{
			SetPullDownMenu( pWnd );
		}
		break;
	case ID_PULLDOWN_WND:
		if( cmd == IOBN_BTNUP )
		{
			SetPullDownBtnUP( pWnd );
		}
		break;
	case ID_BACKGROUND_CLEAR:
		if( cmd == IOBN_BTNUP )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );			
			switch( m_eReturnType )
			{
			case RWT_LOBBY_BACK_CHANGE_WND:
				g_BackgroundMgr.ClearLobbyBackground();
				break;
			case RWT_LOADING_BACK_CHANGE_WND:
				g_BackgroundMgr.ClearLoadingBackground();
				break;
			case RWT_BATTLE_BACK_CHANGE_WND:
				g_BackgroundMgr.ClearBattleBackground();
				break;
			case RWT_LADDER_BACK_CHANGE_WND:
				g_BackgroundMgr.ClearLadderBackground();
				break;
			case RWT_MODE_FIND_BACK_CHANGE_WND:
				g_BackgroundMgr.ClearFindModeBackground();				
				break;
			}
			HideWnd();
		}
		else if( cmd == IOEX_BTNUP )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		}
		break;
	}
}

void FileListWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{ 
		ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
		if( !pScroll )
			return;

		if( zDelta == WHEEL_DELTA )
		{
			pScroll->SetScrollPos( m_iCurPage - 1 );
			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			pScroll->SetScrollPos( m_iCurPage + 1 );
			if( g_GUIMgr.GetPreOverWnd() )
			{
				ioMouse NullMouse;
				g_GUIMgr.GetPreOverWnd()->iwm_mouseover( NullMouse );
			}
		}
	}
}

void FileListWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	m_iCurPage = curPos;
	UpdateShowFileList();
}

void FileListWnd::SetFileNameBtnUP( DWORD dwID )
{
	FileListItem *pItem = dynamic_cast<FileListItem*> (FindChildWnd( dwID ) );
	if( !pItem )
		return;

	ioHashString szFileName;
	pItem->GetFileName( szFileName );

	if( szFileName.IsEmpty() )
		return;

	if( pItem->IsFolder() )
	{
		char szFolderName[MAX_PATH]="";
		// 폴더이름의 []제거
		int iLength = szFileName.Length();
		if( iLength >= MAX_PATH )
		{
			LOG.PrintTimeAndLog( 0, "%s Error Long Folder Name.", __FUNCTION__ );
			return;
		}
		for (int i = 1; i < iLength-1 ; i++)
			szFolderName[i-1] = szFileName.At(i);
	
		char szNewRootPath[MAX_PATH*2]="";
		StringCbPrintf( szNewRootPath, sizeof( szNewRootPath ), "%s\\%s", m_szRootFolder.c_str(), szFolderName );
		UpdateByNewPath( szNewRootPath );
		SetChildActive( ID_PARENT_FOLDER );
		return;
	}

	if( !m_pReturnWnd )
		return;

	if( m_eReturnType == RWT_NONE )
	{
		if( !m_pReturnWnd->IsShow() )
			return;
		m_pReturnWnd->SetTitleText( szFileName.c_str() );
	}
	else if( m_eReturnType == RWT_COUSTOMSOUND_SETTING_ITEM_WND )
	{
		SettingCustomSoundItemWnd *pItem = dynamic_cast<SettingCustomSoundItemWnd*> (m_pReturnWnd);
		if( !pItem )
			return;
		if( !pItem->IsShow() )
			return;
		GameOptionSettingCustomSoundTab *pParent = dynamic_cast<GameOptionSettingCustomSoundTab*> ( pItem->GetParent() );
		if( !pParent )
			return;
		if( pParent->CheckWaveFile( szFileName.c_str() ) )
			pItem->SetFileNameInfo( szFileName.c_str() );
		else
			pItem->SetFileNameInfo( "" );
	}	
	else if( m_eReturnType == RWT_GUILD_MARK_CHANGE_WND )
	{
		if( m_pReturnWnd )
		{
			char szFullPath[MAX_PATH*2]="";
			StringCbPrintf( szFullPath, sizeof( szFullPath ), "%s\\%s", m_szRootFolder.c_str(), szFileName.c_str() );
			ioEdit *pEdit = dynamic_cast<ioEdit*> (m_pReturnWnd);
			if( pEdit )
			{
				pEdit->SetText( szFullPath );
				if( pEdit->GetParent() )
					pEdit->GetParent()->iwm_command( pEdit, IOBN_BTNUP, COMPLETE_FILE_NAME_PARAM );
			}
		}
	}
	else if( m_eReturnType == RWT_LOBBY_BACK_CHANGE_WND )
	{
		if( m_pReturnWnd )
		{
			char szFullPath[MAX_PATH*2]="";
			StringCbPrintf( szFullPath, sizeof( szFullPath ), "%s\\%s", m_szRootFolder.c_str(), szFileName.c_str() );
			if( m_pReturnWnd )
			{
				if( g_BackgroundMgr.CopyCustomLobbyBack( szFullPath ) )
				{
					g_BackgroundMgr.LoadCustomLobbyBack( ( ioMyRoomMode::GetMyRoomType() == MRT_LOBBY ) );
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				}
			}
		}
	}
	else if( m_eReturnType == RWT_LOADING_BACK_CHANGE_WND )
	{
		if( m_pReturnWnd )
		{
			char szFullPath[MAX_PATH*2]="";
			StringCbPrintf( szFullPath, sizeof( szFullPath ), "%s\\%s", m_szRootFolder.c_str(), szFileName.c_str() );
			if( m_pReturnWnd )
			{
				if( g_BackgroundMgr.CopyCustomLoadingBack( szFullPath ) )
				{
					g_BackgroundMgr.LoadCustomLoadingBack();
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
				}
			}
		}
	}
	else if( m_eReturnType == RWT_BATTLE_BACK_CHANGE_WND )
	{
		if( m_pReturnWnd )
		{
			char szFullPath[MAX_PATH*2]="";
			StringCbPrintf( szFullPath, sizeof( szFullPath ), "%s\\%s", m_szRootFolder.c_str(), szFileName.c_str() );
			if( m_pReturnWnd )
			{
				if( g_BackgroundMgr.CopyCustomBattleBack( szFullPath ) )
				{
					g_BackgroundMgr.LoadCustomBattleBack();
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
				}
			}
		}
	}
	else if( m_eReturnType == RWT_LADDER_BACK_CHANGE_WND )
	{
		if( m_pReturnWnd )
		{
			char szFullPath[MAX_PATH*2]="";
			StringCbPrintf( szFullPath, sizeof( szFullPath ), "%s\\%s", m_szRootFolder.c_str(), szFileName.c_str() );
			if( m_pReturnWnd )
			{
				if( g_BackgroundMgr.CopyCustomLadderBack( szFullPath ) )
				{
					g_BackgroundMgr.LoadCustomLadderBack();
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
				}
			}
		}
	}
	else if( m_eReturnType == RWT_CUSTOM_TEXTURE_FIND_WND )
	{
		if( m_pReturnWnd )
		{
			char szFullPath[MAX_PATH*2]="";
			StringCbPrintf( szFullPath, sizeof( szFullPath ), "%s\\%s", m_szRootFolder.c_str(), szFileName.c_str() );
						
			if( m_pReturnWnd->GetParent() )
			{
				ItemCustomMakeUploadWnd *pCustomUploadWnd = dynamic_cast< ItemCustomMakeUploadWnd * >( m_pReturnWnd->GetParent() );
				pCustomUploadWnd->SetFilePath( m_pReturnWnd, szFullPath );
			}
		}
	}
	else if( m_eReturnType == RWT_MODE_FIND_BACK_CHANGE_WND )
	{
		if( m_pReturnWnd )
		{
			char szFullPath[MAX_PATH*2]="";
			StringCbPrintf( szFullPath, sizeof( szFullPath ), "%s\\%s", m_szRootFolder.c_str(), szFileName.c_str() );
			if( m_pReturnWnd )
			{
				if( g_BackgroundMgr.CopyCustomFindModeBack( szFullPath ) )
				{
					g_BackgroundMgr.LoadCustomFindModeBack();
				}
				else
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
				}
			}
		}
	}

	HideWnd();
}

bool FileListWnd::IsFileListEmpty()
{
	return m_vFileList.empty();
}

void FileListWnd::SetReturnInfo( ioWnd *pReturnWnd, ReturnWndType eType /*= RWT_NONE */ )
{
	m_pReturnWnd = pReturnWnd;
	m_eReturnType= eType;
}

void FileListWnd::PlayByCaller( DWORD dwID )
{
	if( !m_pReturnWnd )
		return;

	FileListItem *pItem = dynamic_cast<FileListItem*> (FindChildWnd( dwID ) );
	if( !pItem )
		return;

	ioHashString szFileName;
	pItem->GetFileName( szFileName );

	if( szFileName.IsEmpty() )
		return;

	if( m_eReturnType == RWT_COUSTOMSOUND_SETTING_ITEM_WND )
	{
		SettingCustomSoundItemWnd *pItem = dynamic_cast<SettingCustomSoundItemWnd*> (m_pReturnWnd);
		if( !pItem )
			return;
		GameOptionSettingCustomSoundTab *pParent = dynamic_cast<GameOptionSettingCustomSoundTab*> ( pItem->GetParent() );
		if( !pParent )
			return;
		if( pParent->CheckWaveFile( szFileName.c_str() ) )
			pParent->PlayWaveFile( szFileName.c_str() );
	}	
}

void FileListWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	if( m_eReturnType == RWT_COUSTOMSOUND_SETTING_ITEM_WND )
	{
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_LEFT );

		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		g_FontMgr.PrintText( iXPos + 55, iYPos + 42, FONT_SIZE_13, "C:/Program Files/");
		g_FontMgr.PrintText( iXPos + 55, iYPos + 62, FONT_SIZE_13, "LostSaga/Usersou");
		g_FontMgr.PrintText( iXPos + 17, iYPos + 82, FONT_SIZE_13, STR(1));
		g_FontMgr.PrintText( iXPos + 17, iYPos + 102, FONT_SIZE_13,STR(2));
		g_FontMgr.PrintText( iXPos + 17, iYPos + 122, FONT_SIZE_13,STR(3));

		if( m_pDescIcon )
		{
			m_pDescIcon->SetScale( FLOAT05 );
			m_pDescIcon->Render( iXPos + 33, iYPos + 64, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
	else if( m_eReturnType == RWT_LOBBY_BACK_CHANGE_WND   ||
		     m_eReturnType == RWT_LOADING_BACK_CHANGE_WND ||
			 m_eReturnType == RWT_BATTLE_BACK_CHANGE_WND  ||
			 m_eReturnType == RWT_LADDER_BACK_CHANGE_WND  ||
			 m_eReturnType == RWT_MODE_FIND_BACK_CHANGE_WND )
	{
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
		g_FontMgr.PrintText( iXPos + 177, iYPos + 402, FONT_SIZE_13, STR(4));
	}
}

void FileListWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DescIcon" )
	{
		SAFEDELETE( m_pDescIcon );
		m_pDescIcon = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void FileListWnd::SetShowWndPos()
{
	if( !m_pReturnWnd )
		return;
	if( m_eReturnType != RWT_COUSTOMSOUND_SETTING_ITEM_WND )
		return;
	
	ioWnd *pOptionWnd = g_GUIMgr.FindWnd( GAME_OPTION_WND );
	if( !pOptionWnd )
		return;

	int iWndWidthCenter = pOptionWnd->GetWidth()/2;
	int iCenterXPos = pOptionWnd->GetXPos() + iWndWidthCenter;

	int iScreenWidthCenter = Setting::Width()/2;

	if( iScreenWidthCenter >= iCenterXPos )
		SetWndPos( pOptionWnd->GetXPos() + 272, pOptionWnd->GetYPos() );
	else
		SetWndPos( pOptionWnd->GetXPos() - 202, pOptionWnd->GetYPos() );
}

void FileListWnd::SetParentFolderFileList()
{
	if( m_szRootFolder.IsEmpty() ) return;
	if( m_szFileExt.IsEmpty() ) return;

	int iNewLength = 0;
	int iLength = m_szRootFolder.Length();
	for (int i = iLength - 1; i >= 0 ; i--)
	{
		if( m_szRootFolder.At( i ) == '\\' || m_szRootFolder.At( i ) == '/' )
		{
			iNewLength = i;
			break;
		}
	}

	// 드라이브 경우 \\, / 없을 수 있다.
	if( iNewLength == 0 )
		return;

	char szNewRootFolder[MAX_PATH*2]="";
	StringCbCopyN( szNewRootFolder, sizeof( szNewRootFolder ), m_szRootFolder.c_str(), iNewLength );
	UpdateByNewPath( szNewRootFolder );
	if( strlen( szNewRootFolder ) == 2 ) // c:
		SetChildInActive( ID_PARENT_FOLDER );
}

void FileListWnd::SetPullDownMenu( ioWnd *pOpenBtn )
{
	if( !pOpenBtn ) return;

	FileDriveListWnd *pPullDownWnd = dynamic_cast<FileDriveListWnd *>(FindChildWnd( ID_PULLDOWN_WND ));
	if( pPullDownWnd )
	{
		if( pPullDownWnd->IsShow() )
			pPullDownWnd->HideWnd();
		else
		{			
			vFileDriveItem vList;
			SetDriveList( vList );
			pPullDownWnd->ShowList( vList );
			vList.clear();
		}
	}
}

void FileListWnd::SetDriveList( vFileDriveItem &rList )
{
	rList.clear();

	enum { MAX_DRIVE = 26, };
	ioHashStringVec szDirveTextVec;
	// 스트링 추출 문제로 loop 돌지 않는다.
	szDirveTextVec.push_back( ioHashString( STR(1) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(2) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(3) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(4) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(5) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(6) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(7) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(8) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(9) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(10) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(11) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(12) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(13) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(14) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(15) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(16) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(17) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(18) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(19) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(20) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(21) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(22) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(23) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(24) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(25) ) ); 
	szDirveTextVec.push_back( ioHashString( STR(26) ) ); 

	DWORD dwDrives = GetLogicalDrives();
	for (int i = 0; i < MAX_DRIVE ; i++)
	{
		// dwDrives : 1100 : DCBA : A,B 드라이브는 없고 C, D 드라이브는 있다 ( 비트연산 )
		if( dwDrives & 0x1 )
		{
			FileDriveItem kItem;
			kItem.m_iMagicCode = i;
			if( COMPARE( i, 0, (int)szDirveTextVec.size() ) )
				kItem.m_szName = szDirveTextVec[i];

			if( !kItem.m_szName.IsEmpty() )
				rList.push_back( kItem );
		}
		dwDrives >>= 0x1;
	}
}

void FileListWnd::SetPullDownBtnUP( ioWnd *pBtnUpWnd )
{
	FileDriveListWnd *pPullDownWnd = dynamic_cast<FileDriveListWnd *>(pBtnUpWnd);
	if( pPullDownWnd )
	{
		FileDriveItem kItem = pPullDownWnd->GetOverDriveItem();
		if( kItem.m_szName.IsEmpty() )
		{
			LOG.PrintTimeAndLog( 0, "%s Fail", __FUNCTION__ );
			return;
		}
	
		ioWnd *pDiskBtn = FindChildWnd( ID_SELECT_DISK );
		if( pDiskBtn )
			pDiskBtn->SetTitleText( kItem.m_szName.c_str() );

		char szDriveName[MAX_PATH]="";
		StringCbPrintf( szDriveName, sizeof( szDriveName ), "%c:", kItem.m_iMagicCode+65 ); // 65는 A
		UpdateByNewPath( szDriveName );
		SetChildInActive( ID_PARENT_FOLDER );

		pPullDownWnd->HideWnd();
	}
}

void FileListWnd::SetScrollRange()
{
	m_iCurPage = 0;
	int iSize = m_vFileList.size();
	m_iMaxPage = iSize/MAX_LIST;
	if( iSize%MAX_LIST )
		m_iMaxPage++;

	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, m_iMaxPage );
		pScroll->SetScrollPos( m_iCurPage );
	}
}

void FileListWnd::SetDefaultDriveName()
{
	// drive 버튼 이름 셋팅
	int iDriveMagicCode = 0;
	char szUpperDriveAlphabat[MAX_PATH]="";
	if( !m_szRootFolder.IsEmpty() )
	{
		szUpperDriveAlphabat[0] = m_szRootFolder.At(0);
		strupr( szUpperDriveAlphabat );
		iDriveMagicCode = (int)szUpperDriveAlphabat[0];
		iDriveMagicCode = iDriveMagicCode - 65; // 65는 A
	}

	vFileDriveItem vList;
	vList.reserve( 10 );
	SetDriveList( vList );
	int iMax = vList.size();
	for (int i = 0; i < iMax ; i++)
	{
		if( iDriveMagicCode == vList[i].m_iMagicCode )
		{
			ioWnd *pWnd = FindChildWnd( ID_SELECT_DISK );
			if( pWnd )
				pWnd->SetTitleText( vList[i].m_szName.c_str() );
			break;
		}
	}
	vList.clear();
	if( strlen( m_szRootFolder.c_str() ) == 2 ) // c:
		SetChildInActive( ID_PARENT_FOLDER );
}

void FileListWnd::UpdateByNewPath( const char *szNewPath )
{
	SetFileList( szNewPath, m_szFileExt.c_str() );
	UpdateShowFileList();
	SetScrollRange();
}