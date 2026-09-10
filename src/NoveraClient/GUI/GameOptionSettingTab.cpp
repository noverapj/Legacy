#include "StdAfx.h"

#include "../ioPlayStage.h"
#include "../ioMyRoomMode.h"
#include "../ioBaseChar.h"
#include "../DirectInput.h"
#include "../ioKeyManager.h"
#include "../ioApplication.h"
#include "../ioCustomSoundMgr.h"
#include "../WndID.h"
#include "../EtcHelpFunc.h"
#include "../ioMovieMaker.h"
#include "../TextColorTable.h"

#include "../io3DEngine/InputBox.h"
#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/ioButton.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioUITitle.h"
#include "../io3Dengine/ioXMLDocument.h"
#include "../io3Dengine/ioXMLElement.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/HelpFunc.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3dengine/safesprintf.h"
#include "../io3DEngine/ioMovingWnd.h"

#include "ioModeHelpManager.h"
#include "ioSP2GUIManager.h"

#include "GameOptionWnd.h"
#include "FileListWnd.h"
#include "ResolutionList.h"
#include "GameOptionSettingJoyKeyTab.h"
#include "NewShopWnd.h"

#include <shlobj.h>
#include <strsafe.h>
#include ".\gameoptionsettingtab.h"
#include "gameoptionsettingkeyinput.h"

SettingCustomSoundItemWnd::SettingCustomSoundItemWnd()
{

}

SettingCustomSoundItemWnd::~SettingCustomSoundItemWnd()
{

}

void SettingCustomSoundItemWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_KEYWORD_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if(GetParent())
				GetParent()->iwm_command( this, cmd, ID_PARAM_KEYWORD_BTNUP );
		}
		break;
	case ID_KEYWORD_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1) // enter , tab
			{
				if(GetParent())
					GetParent()->iwm_command( this, cmd, param );
			}
		}
		break;
	case ID_FILENAME_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->iwm_command( this, cmd, ID_PARAM_FILENAME_BTNUP );
		}
		break;
	case ID_PLAY_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->iwm_command( this, cmd, ID_PARAM_PLAY_BTNUP );
		}
		break;
	}
}

void SettingCustomSoundItemWnd::iwm_show()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_KEYWORD_EDIT ) );
	if(pEdit)
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();
	}
}

void SettingCustomSoundItemWnd::iwm_hide()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_KEYWORD_EDIT ) );
	if(pEdit)
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();
	}
}

void SettingCustomSoundItemWnd::SetInfo( const ioHashString &rszKeyword, const char *szFileName )
{
	ioButton *pFirstBtn = dynamic_cast<ioButton*> ( FindChildWnd( ID_KEYWORD_BTN) );
	if( pFirstBtn )
	{
		pFirstBtn->SetTitleText( rszKeyword.c_str() );
	}

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_KEYWORD_EDIT ) );
	if(pEdit)
	{
		pEdit->SetText( rszKeyword.c_str() );
		pEdit->KillKeyFocus();
	}

	ioButton *pSecondBtn = dynamic_cast<ioButton*> ( FindChildWnd( ID_FILENAME_BTN ) );
	if( pSecondBtn )
	{
		pSecondBtn->SetTitleText( szFileName );
	}

	ioWnd *pPlayBtn = FindChildWnd( ID_PLAY_BTN );
	if( !pPlayBtn )
		return;

	if( strcmp( szFileName, "") == 0 )
		pPlayBtn->SetInActive();
	else
		pPlayBtn->SetActive();
}

void SettingCustomSoundItemWnd::GetInfo( OUT ioHashString &rszKeyword, OUT ioHashString &rszFileName )
{
	ioButton *pFirstBtn = dynamic_cast<ioButton*> ( FindChildWnd( ID_KEYWORD_BTN) );
	if( pFirstBtn )
	{
		ioUITitle *pTitle = pFirstBtn->GetTitle();
		if( pTitle )
			pTitle->GetFirstText( rszKeyword );
	}

	ioButton *pSecondBtn = dynamic_cast<ioButton*> ( FindChildWnd( ID_FILENAME_BTN ) );
	if( pSecondBtn )
	{
		ioUITitle *pTitle = pSecondBtn->GetTitle();
		if( pTitle )
			pTitle->GetFirstText( rszFileName );
	}
}

void SettingCustomSoundItemWnd::ClearInfo()
{
	ioButton *pFirstBtn = dynamic_cast<ioButton*> ( FindChildWnd( ID_KEYWORD_BTN) );
	if( pFirstBtn )
	{
		pFirstBtn->SetTitleText( "" );
	}

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_KEYWORD_EDIT ) );
	if(pEdit)
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();
	}

	ioButton *pSecondBtn = dynamic_cast<ioButton*> ( FindChildWnd( ID_FILENAME_BTN ) );
	if( pSecondBtn )
	{
		pSecondBtn->SetTitleText( "" );
	}

	ioWnd *pPlayBtn = FindChildWnd( ID_PLAY_BTN );
	if( pPlayBtn )
		pPlayBtn->SetInActive();
}

void SettingCustomSoundItemWnd::SetKeywordBtnToEdit()
{
	HideChildWnd( ID_KEYWORD_BTN );
	ShowChildWnd( ID_KEYWORD_EDIT );

	ioHashString szKeyword;
	ioButton *pFirstBtn = dynamic_cast<ioButton*> ( FindChildWnd( ID_KEYWORD_BTN) );
	if( pFirstBtn )
	{
		ioUITitle *pTitle = pFirstBtn->GetTitle();
		if( pTitle )
			pTitle->GetFirstText( szKeyword );
	}

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd(ID_KEYWORD_EDIT) );
	if( pEdit )
	{
		pEdit->SetText( szKeyword.c_str() );
		pEdit->SetKeyFocus();
	}
}


void SettingCustomSoundItemWnd::SetKeywordEditToBtn()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_KEYWORD_EDIT ) );
	if( !pEdit )
		return;

	ioButton *pFirstBtn = dynamic_cast<ioButton*> ( FindChildWnd( ID_KEYWORD_BTN) );
	if( !pFirstBtn )
		return; 
	
	pFirstBtn->SetTitleText( pEdit->GetText() );
}


void SettingCustomSoundItemWnd::ShowKeywordBtn()
{
	HideChildWnd( ID_KEYWORD_EDIT);
	ShowChildWnd( ID_KEYWORD_BTN );
}

void SettingCustomSoundItemWnd::SetFileNameInfo( const char *szFileName )
{
	ioButton *pSecondBtn = dynamic_cast<ioButton*> ( FindChildWnd( ID_FILENAME_BTN ) );
	if( pSecondBtn )
		pSecondBtn->SetTitleText( szFileName );

	ioWnd *pPlayBtn = FindChildWnd( ID_PLAY_BTN );
	if( !pPlayBtn )
		return;

	if( strcmp( szFileName, "") == 0 )
		pPlayBtn->SetInActive();
	else
		pPlayBtn->SetActive();
}

void SettingCustomSoundItemWnd::GetFileName( OUT ioHashString &rszFileName )
{
	ioWnd *pBtn = FindChildWnd( ID_FILENAME_BTN );
	if( pBtn )
	{
		ioUITitle *pTitle = pBtn->GetTitle();
		if( pTitle )
			pTitle->GetFirstText( rszFileName );
	}
}

void SettingCustomSoundItemWnd::iwm_create()
{
	ioButton *pFirstBtn = dynamic_cast<ioButton*> ( FindChildWnd( ID_KEYWORD_BTN) );
	if( pFirstBtn )
	{
		ioUITitle *pTitle = pFirstBtn->GetTitle();
		if( pTitle )
		{
			pTitle->SetWidthCutSize(71.0f);
			pTitle->SetPrintTextStyle( ioUITitle::PTS_WIDTHCUT );
		}
	}

	ioButton *pSecondBtn = dynamic_cast<ioButton*> ( FindChildWnd( ID_FILENAME_BTN ) );
	if( pSecondBtn )
	{
		ioUITitle *pTitle = pSecondBtn->GetTitle();
		if( pTitle )
		{
			pTitle->SetWidthCutSize(114.0f);
			pTitle->SetPrintTextStyle( ioUITitle::PTS_WIDTHCUT );
		}
	}
}

const char * SettingCustomSoundItemWnd::GetKeywordByEdit()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_KEYWORD_EDIT ) );
	if( !pEdit )
		return NULL;

	return pEdit->GetText();
}

void SettingCustomSoundItemWnd::SetKeywordEdit( const char *szKeyword )
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_KEYWORD_EDIT ) );
	if( !pEdit )
		return ;

	pEdit->SetText( szKeyword );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GameOptionSettingCustomSoundTab::GameOptionSettingCustomSoundTab()
{
	m_pPreEdit           = NULL;
	m_dwSelectEditItemID = ID_NONE_SELECT_ITEM_WND;
	m_pDescIcon          = NULL;
}

GameOptionSettingCustomSoundTab::~GameOptionSettingCustomSoundTab()
{
	SAFEDELETE( m_pDescIcon );
}

void GameOptionSettingCustomSoundTab::iwm_show()
{
	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	for (int i = 0; i < MAX_SHOW_ITEM_WND ; i++)
	{
		SettingCustomSoundItemWnd *pItem = dynamic_cast<SettingCustomSoundItemWnd*> ( FindChildWnd( ID_SETTING_CUSTOMSOUND_ITEM_WND1 + i));
		if( !pItem )
			continue;
		pItem->ShowWnd();
		pItem->ShowKeywordBtn();
	}
}

void GameOptionSettingCustomSoundTab::iwm_hide()
{
	if( m_pPreEdit )
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
			{
				m_pPreEdit->SetKeyFocus();
			}
		}
	}
	m_pPreEdit = NULL;

	for (int i = 0; i < MAX_SHOW_ITEM_WND ; i++)
	{
		HideChildWnd( ID_SETTING_CUSTOMSOUND_ITEM_WND1 + i );
	}
}

void GameOptionSettingCustomSoundTab::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_SETTING_CUSTOMSOUND_ITEM_WND1:
	case ID_SETTING_CUSTOMSOUND_ITEM_WND2:
	case ID_SETTING_CUSTOMSOUND_ITEM_WND3:
	case ID_SETTING_CUSTOMSOUND_ITEM_WND4:
	case ID_SETTING_CUSTOMSOUND_ITEM_WND5:
	case ID_SETTING_CUSTOMSOUND_ITEM_WND6:
	case ID_SETTING_CUSTOMSOUND_ITEM_WND7:
	case ID_SETTING_CUSTOMSOUND_ITEM_WND8:
	case ID_SETTING_CUSTOMSOUND_ITEM_WND9:
	case ID_SETTING_CUSTOMSOUND_ITEM_WND10:
		if( cmd == IOBN_BTNUP )
		{
			SetKeywordEditToBtnByID();
			if( param == SettingCustomSoundItemWnd::ID_PARAM_KEYWORD_BTNUP )
			{
				SettingCustomSoundItemWnd *pItem = dynamic_cast<SettingCustomSoundItemWnd*> (pWnd);
				if( pItem )
				{
					pItem->SetKeywordBtnToEdit();
					m_dwSelectEditItemID = dwID;
				}
			}
			else if( param == SettingCustomSoundItemWnd::ID_PARAM_FILENAME_BTNUP )
			{	
				FileListWnd *pFileListWnd = dynamic_cast<FileListWnd*> (g_GUIMgr.FindWnd( FILE_LIST_WND ) );
				if( pFileListWnd )
				{
					char szCurPath[MAX_PATH*2]="";
					GetCurrentDirectory( sizeof( szCurPath ), szCurPath );
					char szFullPath[MAX_PATH*2]="";
					StringCbPrintf( szFullPath, sizeof(szFullPath), "%s\\Usersound", szCurPath);
					CreateDirectory( szFullPath, NULL );
					pFileListWnd->SetFileList( szFullPath, "wav" );
					if( !pFileListWnd->IsFileListEmpty() )
					{	
						pFileListWnd->SetReturnInfo( FindChildWnd( dwID ) , FileListWnd::RWT_COUSTOMSOUND_SETTING_ITEM_WND );
						pFileListWnd->ShowWnd();
					}
					else
					{
						g_GUIMgr.SetMsgBox(MB_OK, NULL, STR(1), szFullPath );
					}
				}
			}
			else if( param == SettingCustomSoundItemWnd::ID_PARAM_PLAY_BTNUP )
			{
				SettingCustomSoundItemWnd *pItem = dynamic_cast<SettingCustomSoundItemWnd*> ( pWnd );
				if( pItem )
				{
					ioHashString szFileName;
					pItem->GetFileName( szFileName );
					char szCurPath[MAX_PATH*2]="";
					GetCurrentDirectory( sizeof( szCurPath ), szCurPath );
					char szFullPath[MAX_PATH*2]="";
					StringCbPrintf( szFullPath, sizeof(szFullPath), "%s\\Usersound\\%s", szCurPath, szFileName.c_str() );

					if( g_CustomSoundMgr.CheckWaveFile( szFullPath ) )
						g_CustomSoundMgr.PlayByPath( szFullPath );
				}
			}
		}
		else if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1)
			{
				SetKeywordEditToBtnByID();
			}
		}
		break;
	case ID_SAVE:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->iwm_command( this , cmd, GameOptionSettingTab::ID_PARAM_CHILD_SAVE );
		}
		break;
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			if(GetParent())
				GetParent()->iwm_command( this, cmd, GameOptionSettingTab::ID_PARAM_CHILD_CANCEL );
		}
		break;
	case ID_SHOP:
		if( cmd == IOBN_BTNUP )
		{
			NewShopWnd *pNewShop = dynamic_cast< NewShopWnd* > ( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShop)
			{
				pNewShop->ShowWnd();
				pNewShop->ShowEtcItemTab();
			}
		}
		break;
	}
}


void GameOptionSettingCustomSoundTab::Init()
{
	for (int i = 0; i < MAX_SHOW_ITEM_WND ; i++)
	{
		SettingCustomSoundItemWnd *pItem = dynamic_cast<SettingCustomSoundItemWnd*> ( FindChildWnd( ID_SETTING_CUSTOMSOUND_ITEM_WND1 + i));
		if( !pItem )
			continue;
		
		ioHashString szKeyword;
		ioHashString szPath;
		g_CustomSoundMgr.GetMyKeywordAndPath( i+1, szKeyword, szPath );
		
		char szName[MAX_PATH]="";
		char szExt[MAX_PATH]="";
		_splitpath( szPath.c_str(), NULL, NULL, szName, szExt );
		char szNameExt[MAX_PATH]="";
		if( strcmp( szName , "") != 0)
			StringCbPrintf( szNameExt, sizeof(szNameExt), "%s%s", szName, szExt );

		pItem->SetInfo( szKeyword, szNameExt );
	}
}

void GameOptionSettingCustomSoundTab::Save()
{
	SetKeywordEditToBtnByID();
	SaveKeywordAndFileName();
}

void GameOptionSettingCustomSoundTab::SetKeywordEditToBtnByID()
{
	if( m_dwSelectEditItemID == ID_NONE_SELECT_ITEM_WND ) return;

	SettingCustomSoundItemWnd *pItem = dynamic_cast<SettingCustomSoundItemWnd*> ( FindChildWnd( m_dwSelectEditItemID ) );
	if( !pItem ) return;
	if( !pItem->GetKeywordByEdit() ) return;

	ioHashString szFiltered;
	g_CustomSoundMgr.GetFilteredKeyword( pItem->GetKeywordByEdit(), szFiltered );
	if( szFiltered != pItem->GetKeywordByEdit() )
	{
		g_InputBox.SetString( szFiltered.c_str() );
		pItem->SetKeywordEdit( szFiltered.c_str() );
	}
	
	if( IsExistKeyword( pItem->GetKeywordByEdit(), m_dwSelectEditItemID ) &&
		strcmp( pItem->GetKeywordByEdit(), "") != 0 ) 
	{
		pItem->ClearInfo();
		pItem->ShowKeywordBtn();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
	else
	{
		pItem->ShowKeywordBtn();
		pItem->SetKeywordEditToBtn();
	}
	m_dwSelectEditItemID = ID_NONE_SELECT_ITEM_WND;
}

void GameOptionSettingCustomSoundTab::SaveKeywordAndFileName()
{
	for (int i = 0; i < MAX_SHOW_ITEM_WND ; i++)
	{
		SettingCustomSoundItemWnd *pItem = dynamic_cast<SettingCustomSoundItemWnd*> ( FindChildWnd( ID_SETTING_CUSTOMSOUND_ITEM_WND1 + i));
		if( !pItem )
			continue;

		ioHashString szKeyword;
		ioHashString szPath;
		g_CustomSoundMgr.GetMyKeywordAndPath( i+1, szKeyword, szPath );

		char szName[MAX_PATH]="";
		char szExt[MAX_PATH]="";
		_splitpath( szPath.c_str(), NULL, NULL, szName, szExt );
		char szNameExt[MAX_PATH]="";
		StringCbPrintf( szNameExt, sizeof(szNameExt), "%s%s", szName, szExt );

		ioHashString szCurKeyword;
		ioHashString szCurNameExt;
		pItem->GetInfo( szCurKeyword, szCurNameExt );

		if( szCurKeyword.IsEmpty() )
			szCurNameExt.Clear();
		else
		{
			if( szCurNameExt.IsEmpty() )
				szCurKeyword.Clear();
		}

		if( stricmp( szKeyword.c_str(), szCurKeyword.c_str()) == 0 &&
			stricmp( szNameExt, szCurNameExt.c_str()) == 0 )
			continue;

		if( stricmp( szNameExt, szCurNameExt.c_str()) == 0 )
		{
			g_CustomSoundMgr.UpdateMyKeyword( i+1, szCurKeyword.c_str() );
			g_CustomSoundMgr.SaveINI( i+1 );
		}
		else
		{
			g_CustomSoundMgr.DeleteMyCustomSoundData( i+1 );
			g_CustomSoundMgr.AddMyCustomSoundData( i+1, szCurKeyword.c_str(), szCurNameExt.c_str() );
			g_CustomSoundMgr.SaveINI( i+1 );
		}
	}
}


bool GameOptionSettingCustomSoundTab::CheckWaveFile( const char *szFileName )
{
	char szCurPath[MAX_PATH*2]="";
	GetCurrentDirectory( sizeof( szCurPath ), szCurPath );
	char szFullPath[MAX_PATH*2]="";
	StringCbPrintf( szFullPath, sizeof(szFullPath), "%s\\Usersound\\%s", szCurPath, szFileName );
	if( !g_CustomSoundMgr.CheckWaveFile( szFullPath ) )
		return false;

	return true;
}

void GameOptionSettingCustomSoundTab::PlayWaveFile( const char *szFileName )
{
	char szCurPath[MAX_PATH*2]="";
	GetCurrentDirectory( sizeof( szCurPath ), szCurPath );
	char szFullPath[MAX_PATH*2]="";
	StringCbPrintf( szFullPath, sizeof(szFullPath), "%s\\Usersound\\%s", szCurPath, szFileName );
	g_CustomSoundMgr.PlayByPath( szFullPath );
}

bool GameOptionSettingCustomSoundTab::IsExistKeyword( const char *szKeyword , DWORD dwExceptID )
{
	for (int i = 0; i < MAX_SHOW_ITEM_WND ; i++)
	{
		if( dwExceptID == ID_SETTING_CUSTOMSOUND_ITEM_WND1 + i )
			continue;

		SettingCustomSoundItemWnd *pItem = dynamic_cast<SettingCustomSoundItemWnd*> ( FindChildWnd( ID_SETTING_CUSTOMSOUND_ITEM_WND1 + i));
		if( !pItem )
			continue;

		ioHashString szCurKeyword;
		ioHashString szCurFileName;
		pItem->GetInfo( szCurKeyword, szCurFileName );

		if( szCurKeyword == szKeyword )
			return true;
	}

	return false;
}

void GameOptionSettingCustomSoundTab::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetAlignType( TAT_LEFT );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 58, iYPos + 233, FONT_SIZE_13, STR(1));
	g_FontMgr.PrintText( iXPos + 58, iYPos + 253, FONT_SIZE_13, STR(2));

	if( m_pDescIcon )
	{
		m_pDescIcon->SetScale( FLOAT05 );
		m_pDescIcon->Render( iXPos + 37, iYPos + 255, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
}

void GameOptionSettingCustomSoundTab::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DescIcon" )
	{
		SAFEDELETE( m_pDescIcon );
		m_pDescIcon = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SettingBtn::SettingBtn()
{
	m_eKeyType = CKeyInfo::KTA_NONE;
	m_wKey     = KEY_NULL;
}

SettingBtn::~SettingBtn()
{

}

void SettingBtn::SetKeyType( CKeyInfo::KeyTypeArray eKeyType )
{
	m_eKeyType = eKeyType;
}

CKeyInfo::KeyTypeArray SettingBtn::GetKeyType()
{
	return m_eKeyType;
}


void SettingBtn::SetKeyValue( WORD wKey )
{
	m_wKey = wKey;
}

void SettingBtn::SetTitle( float fWidthCutSize )
{
	if(m_pUITitle)
	{
		if( IsDisabled() )
			m_pUITitle->SetAllColor( 0xFFA7A7A7, 0xFF000000);
		else
			m_pUITitle->SetAllColor( 0xFF3A3A3A, 0xFF000000 );
		m_pUITitle->SetText( g_KeyManager.GetKeyText(m_wKey) , "");
		m_pUITitle->SetWidthCutSize( fWidthCutSize );
		m_pUITitle->SetPrintTextStyle( ioUITitle::PTS_WIDTHCUT );
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SettingKeyItemWnd::SettingKeyItemWnd()
{
	m_eType = IT_NONE;
	
	m_pIconBack = NULL;	
	m_pIconKey  = NULL;
}

SettingKeyItemWnd::~SettingKeyItemWnd()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIconKey );
}

void SettingKeyItemWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "IconKey" )
	{
		SAFEDELETE( m_pIconKey );
		m_pIconKey = pImage;

	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void SettingKeyItemWnd::SetItem( ItemType eItemType,
								 CKeyInfo::KeyTypeArray eFirstKeyType,
								 CKeyInfo::KeyTypeArray eSecondKeyType,
								 WORD wFirstKey,
								 WORD wSecondKey )
{
	m_eType = eItemType;

	if( m_eType == IT_TITLE_BASIC   ||
		m_eType == IT_TITLE_ADDTION ||
		m_eType == IT_TITLE_SKILL   ||
		m_eType == IT_TITLE_FUNCTION||
		m_eType == IT_TITLE_SOLDIER ||
		m_eType == IT_TITLE_BATTLE_MODE ||
		m_eType == IT_TITLE_MACRO )
	{
		HideChildWnd( ID_BIG_BTN );
		HideChildWnd( ID_SMALL_FIRST_BTN );
		HideChildWnd( ID_SMALL_SECOND_BTN );
	}
    // 방향 Second key 사용하지 않음 ( IT_UP ~ IT_RIGHT )
	else if( COMPARE(m_eType, IT_ATTACK, IT_JUMP+1) )
	{
		HideChildWnd( ID_BIG_BTN );
		ShowChildWnd( ID_SMALL_FIRST_BTN );
		ShowChildWnd( ID_SMALL_SECOND_BTN );

		SettingBtn *pBtn = dynamic_cast<SettingBtn*> ( FindChildWnd(ID_SMALL_FIRST_BTN) );
		if(pBtn)
		{
			pBtn->SetKeyType(eFirstKeyType);
			pBtn->SetKeyValue(wFirstKey);
			pBtn->SetTitle( 150.0f );
		}

		pBtn = dynamic_cast<SettingBtn*> ( FindChildWnd(ID_SMALL_SECOND_BTN) );
		if(pBtn)
		{
			pBtn->SetKeyType(eSecondKeyType);
			pBtn->SetKeyValue(wSecondKey);
			pBtn->SetTitle( 150.0f );
		}
	}
	else
	{
		ShowChildWnd( ID_BIG_BTN );
		HideChildWnd( ID_SMALL_FIRST_BTN );
		HideChildWnd( ID_SMALL_SECOND_BTN );

		SettingBtn *pBtn = dynamic_cast<SettingBtn*> ( FindChildWnd(ID_BIG_BTN) );
		if(pBtn)
		{
			pBtn->SetKeyType(eFirstKeyType);
			pBtn->SetKeyValue(wFirstKey);
			pBtn->SetTitle( 320.0f );
		}
	}	

	ioHashString sText;
	SetHelpText( m_eType, sText );
	SetTitle( sText );
}

void SettingKeyItemWnd::OnRender()
{
	ioWnd::OnRender();

	if( m_eType == IT_TITLE_BASIC   ||
		m_eType == IT_TITLE_ADDTION ||
		m_eType == IT_TITLE_SKILL ||
		m_eType == IT_TITLE_FUNCTION||
		m_eType == IT_TITLE_SOLDIER ||
		m_eType == IT_TITLE_BATTLE_MODE ||
		m_eType == IT_TITLE_MACRO )
	{
		// 타이틀 아이콘
		int iXPos = GetDerivedPosX();
		int iYPos = GetDerivedPosY();
		if( m_pIconBack )
		{
			m_pIconBack->SetScale( 0.30f );
			m_pIconBack->Render( iXPos + 13, iYPos + 12, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		}
		if( m_pIconKey )
		{
			m_pIconKey->SetScale( 0.30f );
			m_pIconKey->Render( iXPos + 13, iYPos + 12, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
}

void SettingKeyItemWnd::SetHelpText( IN ItemType eItemType, OUT ioHashString &rsText )
{
	if(eItemType == IT_TITLE_BASIC)
		rsText = STR(1);
	else if(eItemType == IT_UP)
		rsText = STR(2);
	else if(eItemType == IT_DOWN)
		rsText = STR(3);
	else if(eItemType == IT_LEFT)
		rsText = STR(4);
	else if(eItemType == IT_RIGHT)
		rsText = STR(5);
	else if(eItemType == IT_ATTACK)
		rsText = STR(6);
	else if(eItemType == IT_DEFENSE)
		rsText = STR(7);
	else if(eItemType == IT_JUMP)
		rsText = STR(8);
	else if(eItemType == IT_TITLE_ADDTION)
		rsText = STR(9);
	else if(eItemType == IT_LEFT_UP)
		rsText = STR(10);
	else if(eItemType == IT_RIGHT_UP)
		rsText = STR(11);
	else if(eItemType == IT_LEFT_DOWN)
		rsText = STR(12);
	else if(eItemType == IT_RIGHT_DOWN)
		rsText = STR(13);
	else if(eItemType == IT_TITLE_SKILL)
		rsText = STR(14);
	else if(eItemType == IT_CLOAK_SKILL)
		rsText = "AS";
	else if(eItemType == IT_HELM_SKILL)
		rsText = "AD";
	else if(eItemType == IT_ARMOR_SKILL)
		rsText = "ASD";
	else if(eItemType == IT_WEAPON_SKILL)
		rsText = "SD";
	else if(eItemType == IT_TITLE_FUNCTION)
		rsText = STR(15);
	else if(eItemType == IT_CONSUMPTION1)
		rsText = "소모품1";
	else if(eItemType == IT_CONSUMPTION2)
		rsText = "소모품2";
	else if(eItemType == IT_CONSUMPTION3)
		rsText = "소모품3";
	else if(eItemType == IT_CONSUMPTION4)
		rsText = "소모품4";
	else if(eItemType == IT_HELP)
		rsText = STR(16);
	else if(eItemType == IT_SHOP)
		rsText = STR(21);
	else if(eItemType == IT_GROWTH)
		rsText = STR(17);
	else if(eItemType == IT_PRESENT)
		rsText = STR(18);
	else if(eItemType == IT_QUEST)
		rsText = STR(19);
	else if(eItemType == IT_MYHOMEPAGE)
		rsText = STR(20);
	else if(eItemType == IT_OPTION)
		rsText = STR(22);
	else if(eItemType == IT_MESSENGER)
		rsText = STR(23);
	else if(eItemType == IT_VOICE)
		rsText = STR(24);
	else if(eItemType == IT_SCREENSHOT)
		rsText = STR(25);
	else if(eItemType == IT_MOVIECAPTURE )
		rsText = STR(26);
	else if(eItemType == IT_SPECIAL_MOTION )
		rsText = STR(27);
	else if(eItemType == IT_ZOOMIN )
		rsText = STR(28);
	else if(eItemType == IT_ZOOMOUT )
		rsText = STR(29);
	else if(eItemType == IT_TITLE_SOLDIER)
		rsText = STR(30);
	else if(eItemType == IT_SOLDIER1)
		rsText = STR(31);
	else if(eItemType == IT_SOLDIER2)
		rsText = STR(32);
	else if(eItemType == IT_SOLDIER3)
		rsText = STR(33);
	else if(eItemType == IT_SOLDIER4)
		rsText = STR(34);
	else if(eItemType == IT_SOLDIER5)
		rsText = STR(35);
	else if(eItemType == IT_SOLDIER6)
		rsText = STR(36);
	else if(eItemType == IT_SOLDIER7)
		rsText = STR(37);
	else if(eItemType == IT_SOLDIER8)
		rsText = STR(38);
	else if(eItemType == IT_SOLDIER9)
		rsText = STR(39);
	else if(eItemType == IT_SOLDIER10)
		rsText = STR(40);
	else if(eItemType == IT_SOLDIER11)
		rsText = STR(41);
	else if(eItemType == IT_SOLDIER12)
		rsText = STR(42);
	else if(eItemType == IT_SOLDIER13)
		rsText = STR(56);
	else if(eItemType == IT_SOLDIER14)
		rsText = STR(57);
	else if(eItemType == IT_SOLDIER15)
		rsText = STR(58);
	else if(eItemType == IT_SOLDIER16)
		rsText = STR(59);
	else if(eItemType == IT_SOLDIER17)
		rsText = STR(60);
	else if(eItemType == IT_SOLDIER18)
		rsText = STR(61);
	else if(eItemType == IT_SOLDIER19)
		rsText = STR(62);
	else if(eItemType == IT_SOLDIER20)
		rsText = STR(63);
	else if(eItemType == IT_SOLDIER21)
		rsText = STR(64);
	else if(eItemType == IT_SOLDIER22)
		rsText = STR(65);
	else if(eItemType == IT_SOLDIER23)
		rsText = STR(66);
	else if(eItemType == IT_SOLDIER24)
		rsText = STR(67);
	else if(eItemType == IT_SOLDIER25)
		rsText = STR(68);
	else if(eItemType == IT_SOLDIER26)
		rsText = STR(69);
	else if(eItemType == IT_SOLDIER27)
		rsText = STR(70);
	else if(eItemType == IT_SOLDIER28)
		rsText = STR(71);
	else if(eItemType == IT_SOLDIER29)
		rsText = STR(72);
	else if(eItemType == IT_SOLDIER30)
		rsText = STR(73);
	else if(eItemType == IT_SOLDIER31)
		rsText = STR(74);
	else if(eItemType == IT_SOLDIER32)
		rsText = STR(75);
	else if(eItemType == IT_SOLDIER33)
		rsText = STR(76);
	else if(eItemType == IT_SOLDIER34)
		rsText = STR(77);
	else if(eItemType == IT_SOLDIER35)
		rsText = STR(78);
	else if(eItemType == IT_SOLDIER36)
		rsText = STR(79);
	else if(eItemType == IT_SOLDIER37)
		rsText = STR(81);
	else if(eItemType == IT_SOLDIER38)
		rsText = STR(82);
	else if(eItemType == IT_SOLDIER39)
		rsText = STR(83);
	else if(eItemType == IT_SOLDIER40)
		rsText = STR(84);
	else if(eItemType == IT_SOLDIER41)
		rsText = STR(85);
	else if(eItemType == IT_SOLDIER42)
		rsText = STR(86);
	else if(eItemType == IT_SOLDIER43)
		rsText = STR(87);
	else if(eItemType == IT_SOLDIER44)
		rsText = STR(88);
	else if(eItemType == IT_SOLDIER45)
		rsText = STR(89);
	else if(eItemType == IT_SOLDIER46)
		rsText = STR(90);
	else if(eItemType == IT_SOLDIER47)
		rsText = STR(91);
	else if(eItemType == IT_SOLDIER48)
		rsText = STR(92);
	else if(eItemType == IT_SOLDIER49)
		rsText = STR(93);
	else if(eItemType == IT_SOLDIER50)
		rsText = STR(94);
	else if(eItemType == IT_TITLE_BATTLE_MODE)
		rsText = STR(95);
	else if(eItemType == IT_ENTRY)
		rsText = STR(96);
	else if(eItemType == IT_SOLDIER_SELECT)
		rsText = STR(97);
	else if(eItemType == IT_TAG_SWITCH)
		rsText = STR(98);
	else if(eItemType == IT_TITLE_MACRO)
		rsText = STR(43);
	else if(eItemType == IT_MACRO1)
		rsText = STR(44);
	else if(eItemType == IT_MACRO2)
		rsText = STR(45);
	else if(eItemType == IT_MACRO3)
		rsText = STR(46);
	else if(eItemType == IT_MACRO4)
		rsText = STR(47);
	else if(eItemType == IT_MACRO5)
		rsText = STR(48);
	else if(eItemType == IT_MACRO6)
		rsText = STR(49);
	else if(eItemType == IT_MACRO7)
		rsText = STR(50);
	else if(eItemType == IT_MACRO8)
		rsText = STR(51);
	else if(eItemType == IT_MACRO9)
		rsText = STR(52);
	else if(eItemType == IT_MACRO10)
		rsText = STR(53);
	else if(eItemType == IT_MACRO11)
		rsText = STR(54);
	else if(eItemType == IT_MACRO12)
		rsText = STR(55);
	else if(eItemType == IT_PET)
		rsText = "펫";
	else if(eItemType == IT_COOLTIME_RESET)
		rsText = STR(80);
}


void SettingKeyItemWnd::SetTitle( const ioHashString &rsText )
{
	if( !m_pUITitle ) return;

	if( m_eType == IT_TITLE_BASIC    ||
	    m_eType == IT_TITLE_ADDTION  ||
		m_eType == IT_TITLE_SKILL    ||
	    m_eType == IT_TITLE_FUNCTION ||
	    m_eType == IT_TITLE_SOLDIER  ||
		m_eType == IT_TITLE_BATTLE_MODE ||
	    m_eType == IT_TITLE_MACRO )
	{
		m_pUITitle->SetOffset( 27, 4 );
		m_pUITitle->SetSize( 15 );
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( rsText.c_str(), "");
		m_pUITitle->SetAllColor( 0xFF3A3A3A, 0xFF000000 );
	}
	else
	{
		m_pUITitle->SetOffset( 4, 4 );
		m_pUITitle->SetSize( 14 );
		m_pUITitle->SetStyle( TS_NORMAL );
		m_pUITitle->SetText( rsText.c_str(), "");
		m_pUITitle->SetAllColor( 0xFF6E6E6E, 0xFF000000 );
	}
}


void SettingKeyItemWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_BIG_BTN:
	case ID_SMALL_FIRST_BTN:
	case ID_SMALL_SECOND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
			{
				SettingBtn *pBtn = dynamic_cast<SettingBtn*> (pWnd);
				if(pBtn)
				{
					GetParent()->iwm_command( this , cmd, pBtn->GetKeyType());
				}
			}
		}
		break;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioPlayStage *GameOptionSettingKeyTab::m_pPlayStage = NULL;
GameOptionSettingKeyTab::GameOptionSettingKeyTab()
{
	m_eSelectKeyType = CKeyInfo::KTA_NONE;
	m_iCurPage = 0;
	m_vWholeItemType.reserve(SettingKeyItemWnd::IT_MAX);
	m_KeyInfo.Load();

	for (int i = 0; i < SettingKeyItemWnd::IT_MAX ; i++)
	{
		m_vWholeItemType.push_back((SettingKeyItemWnd::ItemType)i);
	}
}

GameOptionSettingKeyTab::~GameOptionSettingKeyTab()
{
	m_vWholeItemType.clear();
}

void GameOptionSettingKeyTab::iwm_show()
{
	UpdateSetPage( m_iCurPage );
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, SettingKeyItemWnd::IT_MAX );
		pScroll->SetScrollPos( m_iCurPage );
	}
	SetChildInActive( ID_APPLY );
}

void GameOptionSettingKeyTab::iwm_hide()
{
	m_eSelectKeyType = CKeyInfo::KTA_NONE;
}

void GameOptionSettingKeyTab::iwm_vscroll( DWORD dwID, int curPos )
{
	UpdateSetPage( curPos );
}

void GameOptionSettingKeyTab::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{ 
		// 휠 이벤트 처리를 했다면 부모로 이벤트를 날리지 않는다.
		if( zDelta == WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage - 1 );
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage + 1 );
		}
	}
}

void GameOptionSettingKeyTab::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_SETTING_KEY_ITEM_WND1:
	case ID_SETTING_KEY_ITEM_WND2:
	case ID_SETTING_KEY_ITEM_WND3:
	case ID_SETTING_KEY_ITEM_WND4:
	case ID_SETTING_KEY_ITEM_WND5:
	case ID_SETTING_KEY_ITEM_WND6:
	case ID_SETTING_KEY_ITEM_WND7:
	case ID_SETTING_KEY_ITEM_WND8:
	case ID_SETTING_KEY_ITEM_WND9:
	case ID_SETTING_KEY_ITEM_WND10:
	case ID_SETTING_KEY_ITEM_WND11:
	case ID_SETTING_KEY_ITEM_WND12:
	case ID_SETTING_KEY_ITEM_WND13:
	case ID_SETTING_KEY_ITEM_WND14:
		if( cmd == IOBN_BTNUP )
		{
			m_eSelectKeyType = (CKeyInfo::KeyTypeArray) param;
			ShowKeyInputControlWnd(pWnd);
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_SAVE:
		if( cmd == IOBN_BTNUP )
		{
			if( Save() )
			{
				if( GetParent() )
					GetParent()->HideWnd();
			}
		}
		break;
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->HideWnd();
		}
		break;
	case ID_APPLY:
		if( cmd == IOBN_BTNUP )
		{
			if( Save() )
			{
				UpdateSetPage( m_iCurPage );
				SetChildInActive( ID_APPLY );
			}
		}
		break;
	case ID_DEFAULT:
		if( cmd == IOBN_BTNUP )
		{
			g_KeyManager.GetDefault( m_KeyInfo );
			UpdateSetPage( m_iCurPage );
			SetChildActive( ID_APPLY );
		}
		break;
	case GAME_OPTION_SETTING_KEYINPUT_WND:
		if( cmd == IOBN_BTNUP )
		{
			if( param == GameOptionSettingKeyInput::ID_BTNUP_DONT_SETTING )
				ChangeKeyValue( KEY_NULL );
			else if( !ShowExistKeyControlWnd(param) )
					ChangeKeyValue( param );
		}
		break;
	}
}


void GameOptionSettingKeyTab::UpdateSetPage( int iNewPage )
{
	m_iCurPage = iNewPage;
	m_eSelectKeyType = CKeyInfo::KTA_NONE;

	for (int i = 0; i < MAX_SHOW_ITEM_WND ; i++)
	{
		SettingKeyItemWnd *pChild = dynamic_cast<SettingKeyItemWnd*> ( FindChildWnd( ID_SETTING_KEY_ITEM_WND1 + i ) );
		if(pChild)
		{
			SettingKeyItemWnd::ItemType eItemType = GetItemType( m_iCurPage + i);
			CKeyInfo::KeyTypeArray eFirsKeyType = CKeyInfo::KTA_NONE;
			CKeyInfo::KeyTypeArray eSecondKeyType = CKeyInfo::KTA_NONE;
			GetKeyType( eItemType, eFirsKeyType, eSecondKeyType );
			pChild->SetItem(eItemType,
				            eFirsKeyType,
							eSecondKeyType,
							m_KeyInfo.GetKey(eFirsKeyType),
							m_KeyInfo.GetKey(eSecondKeyType) );
		}
	}
}

SettingKeyItemWnd::ItemType GameOptionSettingKeyTab::GetItemType( int iPage )
{
	if( !COMPARE(iPage, 0, (int) m_vWholeItemType.size()) )
		return SettingKeyItemWnd::IT_NONE;

	return m_vWholeItemType[iPage];
}

void GameOptionSettingKeyTab::GetKeyType( IN SettingKeyItemWnd::ItemType eItemType, 
										  OUT CKeyInfo::KeyTypeArray &reFirstKeyType, 
										  OUT CKeyInfo::KeyTypeArray &reSecondKeyType)
{

	reFirstKeyType = CKeyInfo::KTA_NONE;
	reSecondKeyType = CKeyInfo::KTA_NONE;
	
	if( eItemType == SettingKeyItemWnd::IT_UP )
	{	
		reFirstKeyType  = CKeyInfo::KTA_UP;
		reSecondKeyType = CKeyInfo::KTA_SECOND_UP;
	}
	else if( eItemType == SettingKeyItemWnd::IT_DOWN )
	{
		reFirstKeyType  = CKeyInfo::KTA_DOWN;
		reSecondKeyType = CKeyInfo::KTA_SECOND_DOWN;
	}
	else if( eItemType == SettingKeyItemWnd::IT_LEFT )
	{
		reFirstKeyType  = CKeyInfo::KTA_LEFT;
		reSecondKeyType = CKeyInfo::KTA_SECOND_LEFT;
	}
	else if( eItemType == SettingKeyItemWnd::IT_RIGHT )
	{
		reFirstKeyType  = CKeyInfo::KTA_RIGHT;
		reSecondKeyType = CKeyInfo::KTA_SECOND_RIGHT;
	}
	else if( eItemType ==  SettingKeyItemWnd::IT_ATTACK )
	{
		reFirstKeyType  = CKeyInfo::KTA_ATTACK;
		reSecondKeyType = CKeyInfo::KTA_SECOND_ATTACK;
	}
	else if( eItemType ==  SettingKeyItemWnd::IT_DEFENSE )
	{
		reFirstKeyType  = CKeyInfo::KTA_DEFENSE;
		reSecondKeyType = CKeyInfo::KTA_SECOND_DEFENSE;
	}
	else if( eItemType ==  SettingKeyItemWnd::IT_JUMP )
	{
		reFirstKeyType  = CKeyInfo::KTA_JUMP;
		reSecondKeyType = CKeyInfo::KTA_SECOND_JUMP;
	}
	else if( eItemType ==  SettingKeyItemWnd::IT_LEFT_UP )
		reFirstKeyType  = CKeyInfo::KTA_LEFTUP;
	else if( eItemType ==  SettingKeyItemWnd::IT_RIGHT_UP )
		reFirstKeyType  = CKeyInfo::KTA_RIGHTUP;
	else if( eItemType ==  SettingKeyItemWnd::IT_LEFT_DOWN )
		reFirstKeyType  = CKeyInfo::KTA_LEFTDOWN;
	else if( eItemType ==  SettingKeyItemWnd::IT_RIGHT_DOWN )
		reFirstKeyType  = CKeyInfo::KTA_RIGHTDOWN;
	else if( eItemType ==  SettingKeyItemWnd::IT_CLOAK_SKILL )
		reFirstKeyType = CKeyInfo::KTA_CLOAK_SKILL;
	else if( eItemType ==  SettingKeyItemWnd::IT_HELM_SKILL )
		reFirstKeyType = CKeyInfo::KTA_HELM_SKILL;
	else if( eItemType ==  SettingKeyItemWnd::IT_ARMOR_SKILL )
		reFirstKeyType = CKeyInfo::KTA_ARMOR_SKILL;
	else if( eItemType ==  SettingKeyItemWnd::IT_WEAPON_SKILL )
		reFirstKeyType = CKeyInfo::KTA_WEAPON_SKILL;
	else if( eItemType ==  SettingKeyItemWnd::IT_CONSUMPTION1 )
		reFirstKeyType = CKeyInfo::KTA_CONSUMPTION1;
	else if( eItemType ==  SettingKeyItemWnd::IT_CONSUMPTION2 )
		reFirstKeyType = CKeyInfo::KTA_CONSUMPTION2;
	else if( eItemType ==  SettingKeyItemWnd::IT_CONSUMPTION3 )
		reFirstKeyType = CKeyInfo::KTA_CONSUMPTION3;
	else if( eItemType ==  SettingKeyItemWnd::IT_CONSUMPTION4 )
		reFirstKeyType = CKeyInfo::KTA_CONSUMPTION4;
	else if( eItemType ==  SettingKeyItemWnd::IT_HELP )
		reFirstKeyType = CKeyInfo::KTA_HELP;
	else if( eItemType ==  SettingKeyItemWnd::IT_SHOP )
		reFirstKeyType  = CKeyInfo::KTA_SHOP;
	else if( eItemType ==  SettingKeyItemWnd::IT_GROWTH )
		reFirstKeyType = CKeyInfo::KTA_GROWTH;
	else if( eItemType ==  SettingKeyItemWnd::IT_QUEST )
		reFirstKeyType = CKeyInfo::KTA_QUEST;
	else if( eItemType ==  SettingKeyItemWnd::IT_PRESENT )
		reFirstKeyType = CKeyInfo::KTA_PRESENT;
	else if( eItemType ==  SettingKeyItemWnd::IT_MYHOMEPAGE )
		reFirstKeyType = CKeyInfo::KTA_MYHOMEPAGE;
	else if( eItemType ==  SettingKeyItemWnd::IT_OPTION )
		reFirstKeyType  = CKeyInfo::KTA_OPTION;
	else if( eItemType ==  SettingKeyItemWnd::IT_MESSENGER )
		reFirstKeyType  = CKeyInfo::KTA_MESSENGER;
	else if( eItemType ==  SettingKeyItemWnd::IT_VOICE )
		reFirstKeyType  = CKeyInfo::KTA_VOICE;
	else if( eItemType ==  SettingKeyItemWnd::IT_SCREENSHOT )
		reFirstKeyType  = CKeyInfo::KTA_SCREENSHOT;
	else if( eItemType ==  SettingKeyItemWnd::IT_MOVIECAPTURE )
		reFirstKeyType  = CKeyInfo::KTA_MOVIECAPTURE;
	else if( eItemType ==  SettingKeyItemWnd::IT_SPECIAL_MOTION )
		reFirstKeyType  = CKeyInfo::KTA_SPECIAL_MOTION;
	else if( eItemType ==  SettingKeyItemWnd::IT_ZOOMIN )
		reFirstKeyType  = CKeyInfo::KTA_ZOOMIN;
	else if( eItemType ==  SettingKeyItemWnd::IT_ZOOMOUT )
		reFirstKeyType  = CKeyInfo::KTA_ZOOMOUT;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER1 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER1;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER2 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER2;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER3 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER3;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER4 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER4;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER5 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER5;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER6 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER6;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER7 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER7;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER8 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER8;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER9 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER9;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER10 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER10;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER11 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER11;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER12 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER12;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER13 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER13;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER14 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER14;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER15 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER15;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER16 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER16;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER17 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER17;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER18 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER18;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER19 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER19;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER20 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER20;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER21 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER21;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER22 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER22;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER23 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER23;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER24 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER24;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER25 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER25;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER26 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER26;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER27 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER27;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER28 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER28;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER29 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER29;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER30 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER30;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER31 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER31;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER32 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER32;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER33 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER33;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER34 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER34;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER35 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER35;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER36 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER36;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER37 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER37;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER38 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER38;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER39 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER39;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER40 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER40;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER41 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER41;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER42 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER42;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER43 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER43;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER44 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER44;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER45 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER45;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER46 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER46;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER47 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER47;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER48 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER48;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER49 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER49;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER50 )
		reFirstKeyType  = CKeyInfo::KTA_SOLDIER50;
	else if( eItemType ==  SettingKeyItemWnd::IT_ENTRY )
		reFirstKeyType  = CKeyInfo::KTA_BATTLE_MODE_ENTRY;
	else if( eItemType ==  SettingKeyItemWnd::IT_SOLDIER_SELECT )
		reFirstKeyType  = CKeyInfo::KTA_BATTLE_MODE_SOLDIER_SELECT;
	else if( eItemType ==  SettingKeyItemWnd::IT_TAG_SWITCH )
		reFirstKeyType  = CKeyInfo::KTA_BATTLE_MODE_TAG_SWITCH;
	else if( eItemType ==  SettingKeyItemWnd::IT_MACRO1 )
		reFirstKeyType  = CKeyInfo::KTA_MACRO1;
	else if( eItemType ==  SettingKeyItemWnd::IT_MACRO2 )
		reFirstKeyType  = CKeyInfo::KTA_MACRO2;
	else if( eItemType ==  SettingKeyItemWnd::IT_MACRO3 )
		reFirstKeyType  = CKeyInfo::KTA_MACRO3;
	else if( eItemType ==  SettingKeyItemWnd::IT_MACRO4 )
		reFirstKeyType  = CKeyInfo::KTA_MACRO4;
	else if( eItemType ==  SettingKeyItemWnd::IT_MACRO5 )
		reFirstKeyType  = CKeyInfo::KTA_MACRO5;
	else if( eItemType ==  SettingKeyItemWnd::IT_MACRO6 )
		reFirstKeyType  = CKeyInfo::KTA_MACRO6;
	else if( eItemType ==  SettingKeyItemWnd::IT_MACRO7 )
		reFirstKeyType  = CKeyInfo::KTA_MACRO7;
	else if( eItemType ==  SettingKeyItemWnd::IT_MACRO8 )
		reFirstKeyType  = CKeyInfo::KTA_MACRO8;
	else if( eItemType ==  SettingKeyItemWnd::IT_MACRO9 )
		reFirstKeyType  = CKeyInfo::KTA_MACRO9;
	else if( eItemType ==  SettingKeyItemWnd::IT_MACRO10 )
		reFirstKeyType  = CKeyInfo::KTA_MACRO10;
	else if( eItemType ==  SettingKeyItemWnd::IT_MACRO11 )
		reFirstKeyType  = CKeyInfo::KTA_MACRO11;
	else if( eItemType ==  SettingKeyItemWnd::IT_MACRO12 )
		reFirstKeyType  = CKeyInfo::KTA_MACRO12;
	else if( eItemType ==  SettingKeyItemWnd::IT_PET )
		reFirstKeyType = CKeyInfo::KTA_PET;
	else if( eItemType ==  SettingKeyItemWnd::IT_COOLTIME_RESET )
		reFirstKeyType = CKeyInfo::KTA_COOLTIME_RESET;
}

SettingKeyItemWnd::ItemType GameOptionSettingKeyTab::GetItemTypeByKeyType( CKeyInfo::KeyTypeArray eKeyType )
{

	SettingKeyItemWnd::ItemType eReturnItemType = SettingKeyItemWnd::IT_NONE;

	   	 if( eKeyType == CKeyInfo::KTA_UP  ||
		 	 eKeyType == CKeyInfo::KTA_SECOND_UP )
			 eReturnItemType = SettingKeyItemWnd::IT_UP;
	else if( eKeyType == CKeyInfo::KTA_DOWN || 
			 eKeyType == CKeyInfo::KTA_SECOND_DOWN )
			 eReturnItemType = SettingKeyItemWnd::IT_DOWN;
	else if( eKeyType == CKeyInfo::KTA_LEFT ||
			 eKeyType == CKeyInfo::KTA_SECOND_LEFT )
			 eReturnItemType = SettingKeyItemWnd::IT_LEFT;
	else if( eKeyType == CKeyInfo::KTA_RIGHT ||
			 eKeyType == CKeyInfo::KTA_SECOND_RIGHT )
			 eReturnItemType = SettingKeyItemWnd::IT_RIGHT;
	else if( eKeyType == CKeyInfo::KTA_ATTACK ||
		     eKeyType == CKeyInfo::KTA_SECOND_ATTACK )
			 eReturnItemType = SettingKeyItemWnd::IT_ATTACK;
	else if( eKeyType == CKeyInfo::KTA_DEFENSE ||
		     eKeyType == CKeyInfo::KTA_SECOND_DEFENSE )
			 eReturnItemType = SettingKeyItemWnd::IT_DEFENSE;
	else if( eKeyType == CKeyInfo::KTA_JUMP  ||
	         eKeyType == CKeyInfo::KTA_SECOND_JUMP )
			 eReturnItemType = SettingKeyItemWnd::IT_JUMP;
	else if( eKeyType == CKeyInfo::KTA_LEFTUP )
			 eReturnItemType = SettingKeyItemWnd::IT_LEFT_UP;
	else if( eKeyType == CKeyInfo::KTA_RIGHTUP )
			 eReturnItemType = SettingKeyItemWnd::IT_RIGHT_UP;
	else if( eKeyType == CKeyInfo::KTA_LEFTDOWN )
			 eReturnItemType = SettingKeyItemWnd::IT_LEFT_DOWN;
	else if( eKeyType == CKeyInfo::KTA_RIGHTDOWN )
			 eReturnItemType = SettingKeyItemWnd::IT_RIGHT_DOWN;
	else if( eKeyType == CKeyInfo::KTA_CLOAK_SKILL )
			 eReturnItemType = SettingKeyItemWnd::IT_CLOAK_SKILL;
	else if( eKeyType == CKeyInfo::KTA_HELM_SKILL )
			 eReturnItemType = SettingKeyItemWnd::IT_HELM_SKILL;
	else if( eKeyType == CKeyInfo::KTA_ARMOR_SKILL )
			 eReturnItemType = SettingKeyItemWnd::IT_ARMOR_SKILL;
	else if( eKeyType == CKeyInfo::KTA_WEAPON_SKILL )
			 eReturnItemType = SettingKeyItemWnd::IT_WEAPON_SKILL;
	else if( eKeyType == CKeyInfo::KTA_CONSUMPTION1 )
			 eReturnItemType = SettingKeyItemWnd::IT_CONSUMPTION1;
	else if( eKeyType == CKeyInfo::KTA_CONSUMPTION2 )
			 eReturnItemType = SettingKeyItemWnd::IT_CONSUMPTION2;
	else if( eKeyType == CKeyInfo::KTA_CONSUMPTION3 )
			 eReturnItemType = SettingKeyItemWnd::IT_CONSUMPTION3;
	else if( eKeyType == CKeyInfo::KTA_CONSUMPTION4 )
			 eReturnItemType = SettingKeyItemWnd::IT_CONSUMPTION4;
	else if( eKeyType == CKeyInfo::KTA_HELP )
			 eReturnItemType = SettingKeyItemWnd::IT_HELP;
	else if( eKeyType == CKeyInfo::KTA_SHOP )
			 eReturnItemType = SettingKeyItemWnd::IT_SHOP;
	else if( eKeyType == CKeyInfo::KTA_GROWTH )
	         eReturnItemType = SettingKeyItemWnd::IT_GROWTH;
	else if( eKeyType == CKeyInfo::KTA_QUEST )
			 eReturnItemType = SettingKeyItemWnd::IT_QUEST;
	else if( eKeyType == CKeyInfo::KTA_PRESENT )
	         eReturnItemType = SettingKeyItemWnd::IT_PRESENT;
	else if( eKeyType == CKeyInfo::KTA_MYHOMEPAGE )
	         eReturnItemType = SettingKeyItemWnd::IT_MYHOMEPAGE;
	else if( eKeyType == CKeyInfo::KTA_OPTION )
			 eReturnItemType = SettingKeyItemWnd::IT_OPTION;
	else if( eKeyType == CKeyInfo::KTA_MESSENGER )
			 eReturnItemType = SettingKeyItemWnd::IT_MESSENGER;
	else if( eKeyType == CKeyInfo::KTA_VOICE )
			 eReturnItemType = SettingKeyItemWnd::IT_VOICE;
	else if( eKeyType == CKeyInfo::KTA_SCREENSHOT )
		     eReturnItemType = SettingKeyItemWnd::IT_SCREENSHOT;
	else if( eKeyType == CKeyInfo::KTA_MOVIECAPTURE )
		     eReturnItemType = SettingKeyItemWnd::IT_MOVIECAPTURE;
	else if( eKeyType == CKeyInfo::KTA_SPECIAL_MOTION )
	         eReturnItemType = SettingKeyItemWnd::IT_SPECIAL_MOTION;
	else if( eKeyType == CKeyInfo::KTA_ZOOMIN )
	         eReturnItemType = SettingKeyItemWnd::IT_ZOOMIN;
	else if( eKeyType == CKeyInfo::KTA_ZOOMOUT )
			 eReturnItemType = SettingKeyItemWnd::IT_ZOOMOUT;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER1 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER1;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER2 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER2;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER3 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER3;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER4 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER4;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER5 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER5;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER6 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER6;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER7 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER7;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER8 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER8;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER9 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER9;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER10 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER10;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER11 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER11;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER12 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER12;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER13 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER13;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER14 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER14;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER15 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER15;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER16 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER16;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER17 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER17;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER18 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER18;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER19 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER19;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER20 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER20;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER21 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER21;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER22 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER22;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER23 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER23;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER24 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER24;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER25 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER25;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER26 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER26;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER27 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER27;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER28 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER28;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER29 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER29;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER30 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER30;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER31 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER31;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER32 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER32;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER33 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER33;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER34 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER34;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER35 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER35;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER36 )
		     eReturnItemType = SettingKeyItemWnd::IT_SOLDIER36;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER37 )
			 eReturnItemType = SettingKeyItemWnd::IT_SOLDIER37;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER38 )
			 eReturnItemType = SettingKeyItemWnd::IT_SOLDIER38;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER39 )
			 eReturnItemType = SettingKeyItemWnd::IT_SOLDIER39;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER40 )
			 eReturnItemType = SettingKeyItemWnd::IT_SOLDIER40;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER41 )
			 eReturnItemType = SettingKeyItemWnd::IT_SOLDIER41;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER42 )
			 eReturnItemType = SettingKeyItemWnd::IT_SOLDIER42;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER43 )
			 eReturnItemType = SettingKeyItemWnd::IT_SOLDIER43;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER44 )
			 eReturnItemType = SettingKeyItemWnd::IT_SOLDIER44;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER45 )
			 eReturnItemType = SettingKeyItemWnd::IT_SOLDIER45;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER46 )
			 eReturnItemType = SettingKeyItemWnd::IT_SOLDIER46;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER47 )
			 eReturnItemType = SettingKeyItemWnd::IT_SOLDIER47;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER48 )
			 eReturnItemType = SettingKeyItemWnd::IT_SOLDIER48;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER49 )
			 eReturnItemType = SettingKeyItemWnd::IT_SOLDIER49;
	else if( eKeyType == CKeyInfo::KTA_SOLDIER50 )
			 eReturnItemType = SettingKeyItemWnd::IT_SOLDIER50;
	else if( eKeyType == CKeyInfo::KTA_BATTLE_MODE_ENTRY )
			 eReturnItemType = SettingKeyItemWnd::IT_ENTRY;
	else if( eKeyType == CKeyInfo::KTA_BATTLE_MODE_SOLDIER_SELECT )
			 eReturnItemType = SettingKeyItemWnd::IT_SOLDIER_SELECT;
	else if( eKeyType == CKeyInfo::KTA_BATTLE_MODE_TAG_SWITCH )
			 eReturnItemType = SettingKeyItemWnd::IT_TAG_SWITCH;
	else if( eKeyType == CKeyInfo::KTA_MACRO1 )
		     eReturnItemType = SettingKeyItemWnd::IT_MACRO1;
	else if( eKeyType == CKeyInfo::KTA_MACRO2 )
		     eReturnItemType = SettingKeyItemWnd::IT_MACRO2;
	else if( eKeyType == CKeyInfo::KTA_MACRO3 )
		     eReturnItemType = SettingKeyItemWnd::IT_MACRO3;
	else if( eKeyType == CKeyInfo::KTA_MACRO4 )
		     eReturnItemType = SettingKeyItemWnd::IT_MACRO4;
	else if( eKeyType == CKeyInfo::KTA_MACRO5 )
		     eReturnItemType = SettingKeyItemWnd::IT_MACRO5;
	else if( eKeyType == CKeyInfo::KTA_MACRO6 )
		     eReturnItemType = SettingKeyItemWnd::IT_MACRO6;
	else if( eKeyType == CKeyInfo::KTA_MACRO7 )
		     eReturnItemType = SettingKeyItemWnd::IT_MACRO7;
	else if( eKeyType == CKeyInfo::KTA_MACRO8 )
		     eReturnItemType = SettingKeyItemWnd::IT_MACRO8;
	else if( eKeyType == CKeyInfo::KTA_MACRO9 )
		     eReturnItemType = SettingKeyItemWnd::IT_MACRO9;
	else if( eKeyType == CKeyInfo::KTA_MACRO10 )
		     eReturnItemType = SettingKeyItemWnd::IT_MACRO10;
	else if( eKeyType == CKeyInfo::KTA_MACRO11 )
		     eReturnItemType = SettingKeyItemWnd::IT_MACRO11;
	else if( eKeyType == CKeyInfo::KTA_MACRO12 )
		     eReturnItemType = SettingKeyItemWnd::IT_MACRO12;
	else if( eKeyType == CKeyInfo::KTA_PET )
	         eReturnItemType = SettingKeyItemWnd::IT_PET;
	else if( eKeyType == CKeyInfo::KTA_COOLTIME_RESET )
		eReturnItemType = SettingKeyItemWnd::IT_COOLTIME_RESET;

	return eReturnItemType;
}

void GameOptionSettingKeyTab::ChangeKeyValue( WORD wKey )
{
	if( !m_KeyInfo.IsRightKey(wKey) )
		return;
	
	CKeyInfo::KeyTypeArray eEqualKeyType = m_KeyInfo.GetEqual( wKey );
	if( eEqualKeyType == CKeyInfo::KTA_NONE )
	{
		m_KeyInfo.SetKey( m_eSelectKeyType, wKey );
		UpdateSetPage(m_iCurPage);
	}
	else if( m_eSelectKeyType == eEqualKeyType )
	{
		UpdateSetPage(m_iCurPage);
	}
	else if( m_eSelectKeyType != eEqualKeyType )
	{
		m_KeyInfo.SetKey( eEqualKeyType, KEY_NULL);
		m_KeyInfo.SetKey( m_eSelectKeyType, wKey );
		UpdateSetPage(m_iCurPage);
	}
}


void GameOptionSettingKeyTab::Init()
{
	g_KeyManager.GetCurrent(m_KeyInfo);
	m_iCurPage = 0;
}

bool GameOptionSettingKeyTab::Save()
{
	enum { MAX_CHECK_KEY = 10, };
	CKeyInfo::KeyTypeArray eKeyTypeArray[MAX_CHECK_KEY]={ CKeyInfo::KTA_UP, CKeyInfo::KTA_DOWN, CKeyInfo::KTA_LEFT, CKeyInfo::KTA_RIGHT, 
		                                                  CKeyInfo::KTA_ATTACK, CKeyInfo::KTA_DEFENSE, CKeyInfo::KTA_JUMP, CKeyInfo::KTA_SECOND_ATTACK,
														  CKeyInfo::KTA_SECOND_DEFENSE, CKeyInfo::KTA_SECOND_JUMP };

	for (int i = 0; i < MAX_CHECK_KEY ; i++)
	{
		if( m_KeyInfo.GetKey( eKeyTypeArray[i]) == KEY_NULL )
		{
			UpdateSetPage( 0 );
			ioScroll *pScroll = dynamic_cast<ioScroll*> ( FindChildWnd( ID_VERT_SCROLL ) );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPage );

			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			return false;
		}
	}

	if( g_KeyManager.SetCustomKey( m_KeyInfo ) )
	{
		g_KeyManager.SaveKey();

		if(m_pPlayStage)
		{
			ioBaseChar *pOnwer = m_pPlayStage->GetOwnerChar();
			if(pOnwer)
			{
				ioUserKeyInput::UseKeyValue kKeys;
				kKeys.SetDefaultFirstKey();
				ioUserKeyInput::UseKeyValue kSecondKeys;
				kSecondKeys.SetDefaultSecondKey();

				g_KeyManager.GetGameKey(kKeys);
				g_KeyManager.GetSecondGameKey(kSecondKeys);

				pOnwer->SetUpKeyValue( kKeys );
				pOnwer->SetUpSecondKeyValue( kSecondKeys );
			}
		}
	}

	return true;
}

bool GameOptionSettingKeyTab::ShowExistKeyControlWnd( WORD wKey )
{
	if( !m_KeyInfo.IsRightKey( wKey ) )
		return false;
	
	CKeyInfo::KeyTypeArray eEqualKeyType = m_KeyInfo.GetEqual( wKey );
	if( eEqualKeyType == CKeyInfo::KTA_NONE )
		return false;

	if( m_eSelectKeyType == eEqualKeyType )
		return false;
	
	GameOptionSettingKeyInput *pKeyInput = dynamic_cast<GameOptionSettingKeyInput*> ( g_GUIMgr.FindWnd( GAME_OPTION_SETTING_KEYINPUT_WND ) );
	if( !pKeyInput )
		return false;
	ioWnd *pUse = pKeyInput->FindChildWnd( GameOptionSettingKeyInput::ID_USE );
	if( pUse && pUse->IsShow() )
		return false;
	
	pKeyInput->ShowWnd();
	pKeyInput->ShowChildWnd( GameOptionSettingKeyInput::ID_USE );
	pKeyInput->ShowChildWnd( GameOptionSettingKeyInput::ID_CANCEL );

	SettingKeyItemWnd *pItem = dynamic_cast<SettingKeyItemWnd*> ( FindChildWnd( ID_SETTING_KEY_ITEM_WND1 ) );
	if( !pItem )
		return false;

	SettingKeyItemWnd::ItemType eItemType = GetItemTypeByKeyType( eEqualKeyType );
	if( eItemType == SettingKeyItemWnd::IT_NONE )
		return false;

	ioHashString sText;
	pItem->SetHelpText( eItemType, sText);
	pKeyInput->SetExistKeyTitle( sText );
	return true;	
}

void GameOptionSettingKeyTab::ShowKeyInputControlWnd( ioWnd *pWnd )
{
	if( !pWnd )
		return;
	GameOptionSettingKeyInput *pKeyInput = dynamic_cast<GameOptionSettingKeyInput*> ( g_GUIMgr.FindWnd( GAME_OPTION_SETTING_KEYINPUT_WND ) );
	if( !pKeyInput )
		return;
	
	pKeyInput->SetReturnWnd( this );
	pKeyInput->ShowWnd();
	pKeyInput->HideChildWnd( GameOptionSettingKeyInput::ID_USE );
	pKeyInput->HideChildWnd( GameOptionSettingKeyInput::ID_CANCEL );

	ioWnd *pChild = pKeyInput->FindChildWnd( GameOptionSettingKeyInput::ID_DONT_SETTING );
	if( !pChild )
		return;

	if( m_eSelectKeyType == CKeyInfo::KTA_UP      || 
		m_eSelectKeyType == CKeyInfo::KTA_DOWN    || 
		m_eSelectKeyType == CKeyInfo::KTA_LEFT    || 
		m_eSelectKeyType == CKeyInfo::KTA_RIGHT   || 
		m_eSelectKeyType == CKeyInfo::KTA_ATTACK  || 
		m_eSelectKeyType == CKeyInfo::KTA_DEFENSE || 
		m_eSelectKeyType == CKeyInfo::KTA_JUMP    || 
		m_eSelectKeyType == CKeyInfo::KTA_SECOND_ATTACK  || 
		m_eSelectKeyType == CKeyInfo::KTA_SECOND_DEFENSE || 
		m_eSelectKeyType == CKeyInfo::KTA_SECOND_JUMP )
		pChild->AddWndStyle( IWS_EXACTIVE );
	else
		pChild->RemoveWndStyle( IWS_EXACTIVE );
	

	ioUITitle *pTitle = pWnd->GetTitle();
	if( !pTitle )
		return;
	
	ioHashString sTitle;
	pTitle->GetFirstText( sTitle );
	pKeyInput->SetCurKeyTitle( sTitle );
}
//////////////////////////////////////////////////////////////////////////
MacroEdit::MacroEdit()
{

}

MacroEdit::~MacroEdit()
{

}

void MacroEdit::OnRender()
{
	ioWnd::OnRender();
	ioEdit::OnRender();
}
//////////////////////////////////////////////////////////////////////////
SettingMacroItemBtn::SettingMacroItemBtn()
{

}

SettingMacroItemBtn::~SettingMacroItemBtn()
{

}

void SettingMacroItemBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 20, iYPos + 4, FONT_SIZE_12, m_szKeyText.c_str() );
	g_FontMgr.SetTextColor( TCT_DEFAULT_LIGHTGRAY );
	g_FontMgr.PrintText( iXPos + 38, iYPos + 4, FONT_SIZE_12, "|" );
}
//////////////////////////////////////////////////////////////////////////
SettingMacroItemWnd::SettingMacroItemWnd()
{
	m_szDefaultTypeAll  = STR(1);
	m_szDefaultTypeTeam = STR(2);
}

SettingMacroItemWnd::~SettingMacroItemWnd()
{

}

void SettingMacroItemWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_MACRO_TYPE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			ChangeTypeText( pWnd );
			if(GetParent())
				GetParent()->iwm_command( this, cmd, ID_PARAM_MACRO_TYP_BTNUP );
		}
		break;
	case ID_MACRO_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1) // enter , tab
			{
				if(GetParent())
					GetParent()->iwm_command( this, cmd, param );
			}
		}
		else if ( cmd == IOBN_BTNDOWN )
		{
			if(GetParent())
				GetParent()->iwm_command( this, IOBN_BTNUP, ID_PARAM_MACRO_BTNUP );
		}
		break;
	}
}

void SettingMacroItemWnd::SetItem( WORD wKey , const CKeyInfo::MecroTextInfo &rMacroTextInfo )
{
	SettingMacroItemBtn *pMacroItemBtn = dynamic_cast<SettingMacroItemBtn *>( FindChildWnd( ID_MACRO_TYPE_BTN ) );
	if( pMacroItemBtn )
	{
		if( rMacroTextInfo.m_bAll )
			pMacroItemBtn->SetTitleText( m_szDefaultTypeAll.c_str() );
		else
			pMacroItemBtn->SetTitleText( m_szDefaultTypeTeam.c_str() );

		char szKeyText[MAX_PATH] = "";
		StringCbCopy( szKeyText, sizeof( szKeyText ), g_KeyManager.GetKeyText( wKey ) );
		if( strcmp( szKeyText, STR(1) ) == 0 )		
			StringCbPrintf( szKeyText, sizeof( szKeyText ), "-" );				
		pMacroItemBtn->SetKeyText( szKeyText );
	}

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_MACRO_EDIT ) );
	if(pEdit)
	{
		pEdit->SetText( rMacroTextInfo.m_szText.c_str() );
		pEdit->KillKeyFocus();
	}
}


void SettingMacroItemWnd::iwm_show()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_MACRO_EDIT ) );
	if(pEdit)
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();
	}
}

void SettingMacroItemWnd::iwm_hide()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_MACRO_EDIT ) );
	if(pEdit)
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();
	}
}

void SettingMacroItemWnd::GetMecroTextInfo( OUT CKeyInfo::MecroTextInfo &rkInfo )
{
	ioWnd *pWnd = FindChildWnd( ID_MACRO_TYPE_BTN );
	if( pWnd )
	{
		ioUITitle *pTitle = pWnd->GetTitle();
		if( pTitle )
		{
			ioHashString szTitle;
			pTitle->GetFirstText( szTitle );
			if( szTitle == m_szDefaultTypeAll )
				rkInfo.m_bAll = true;
			else
				rkInfo.m_bAll = false;
		}
	}

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_MACRO_EDIT ) );
	if(pEdit)
	{
		rkInfo.m_szText =  pEdit->GetText();
		pEdit->KillKeyFocus();
	}
}


void SettingMacroItemWnd::SetMacroEdit()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd(ID_MACRO_EDIT) );
	if( pEdit )
		pEdit->SetKeyFocus();
}

void SettingMacroItemWnd::ChangeTypeText( ioWnd *pWnd )
{
	if( !pWnd )
		return;

	ioUITitle *pTitle = pWnd->GetTitle();
	if( !pTitle )
		return;

	ioHashString szText;
	pTitle->GetFirstText( szText );

	if( szText == m_szDefaultTypeAll )
		pTitle->SetText( m_szDefaultTypeTeam.c_str(), "" );
	else
		pTitle->SetText( m_szDefaultTypeAll.c_str(), "" );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GameOptionSettingGameTab::GameOptionSettingGameTab()
{	
	m_pIconBack = NULL;
	m_pIconGame = NULL;
	m_pIconMacro = NULL;

	m_KeyInfo.Load();
	m_dwSelectItemIDForMacroEdit = ID_NONE_SELECT_ITEM_WND;
	m_pPreEdit = NULL;
}

GameOptionSettingGameTab::~GameOptionSettingGameTab()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIconGame );
	SAFEDELETE( m_pIconMacro );
}

void GameOptionSettingGameTab::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "IconGame" )
	{
		SAFEDELETE( m_pIconGame );
		m_pIconGame = pImage;

	}
	else if( szType == "IconMacro" )
	{
		SAFEDELETE( m_pIconMacro );
		m_pIconMacro = pImage;
	}
	else
	{
		GameOptionBasicWnd::AddRenderImage( szType, pImage );
	}
}

void GameOptionSettingGameTab::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_AUTO_REJECT_PARTY_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bAutoRejctParty = !m_CurrentOption.m_bAutoRejctParty;
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_AUTO_REJECT_CAMP_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bAutoRejctCamp = !m_CurrentOption.m_bAutoRejctCamp;
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_CHAT_BUBBLE_ALARM:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bChatBubbleAlarm = !m_CurrentOption.m_bChatBubbleAlarm;
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_GUILD_CHAT_ALARM:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bGuildChatAlarm = !m_CurrentOption.m_bGuildChatAlarm;
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_BLOCK_USERS_CHAT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bBlockUsersChat = !m_CurrentOption.m_bBlockUsersChat;
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_SHOW_SKILL_BUFF_EMOTICON:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bHideSkillBuffEmoticon = !m_CurrentOption.m_bHideSkillBuffEmoticon;
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_HIDE_MINI_SOLDIER_SELECT_SLOT:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bHideMiniSoldierSelectSlot = !m_CurrentOption.m_bHideMiniSoldierSelectSlot;
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_HIDE_CHAR_CHANGE_GAUGE:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bHideCharChangeGauge = !m_CurrentOption.m_bHideCharChangeGauge;
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_COSTUME_SHOW_FUNC:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bCostumeShowChange = !m_CurrentOption.m_bCostumeShowChange;
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_SAVE:
		if ( cmd == IOBN_BTNUP )
		{
			Save();
			if( GetParent() )
				GetParent()->HideWnd();
		}
		break;
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->HideWnd();
		}
		break;
	case ID_APPLY:
		if ( cmd == IOBN_BTNUP )
		{
			Save();
			SetChildInActive( ID_APPLY );
		}
		break;
	case ID_DEFAULT:
		if ( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bAutoRejctParty = false;
			m_CurrentOption.m_bAutoRejctCamp  = false;
			m_CurrentOption.m_bBlockUsersChat = false;
			m_CurrentOption.m_bChatBubbleAlarm = true;
			m_CurrentOption.m_bGuildChatAlarm = true;

			m_CurrentOption.m_bSlangFilterChange = false;
			m_CurrentOption.m_bSlangFilterOther = false;

			m_CurrentOption.m_bCostumeShowChange = true;

			m_CurrentOption.m_bPlayKillSound = true;
			m_CurrentOption.m_bPlayComboSound = true;
			m_CurrentOption.m_bPlayExtendSound = false;

			m_CurrentOption.m_bHideSkillBuffEmoticon = true;
			SetChildActive( ID_APPLY );
			g_KeyManager.GetDefaultTextInfo( m_KeyInfo );
			UpdateByCurrentOption();
			UpdateSetPage();
		}
		break;
	case ID_BLOCK_USERS_CUSTOM_SOUND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bBlockUsersCustomSound = !m_CurrentOption.m_bBlockUsersCustomSound;
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_SLANG_FILTER_CHANGE:
	case ID_SLANG_FILTER_STAR:
		if( cmd == IOBN_BTNDOWN )
		{
			SetSlangFilterFunc( dwID );
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_SLANG_FILTER_ALL:
	case ID_SLANG_FILTER_OTHER:
		if( cmd == IOBN_BTNDOWN )
		{
			SetSlangFilterTarget( dwID );
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_SETTING_MACRO_ITEM_WND1:
	case ID_SETTING_MACRO_ITEM_WND2:
	case ID_SETTING_MACRO_ITEM_WND3:
	case ID_SETTING_MACRO_ITEM_WND4:
	case ID_SETTING_MACRO_ITEM_WND5:
	case ID_SETTING_MACRO_ITEM_WND6:
	case ID_SETTING_MACRO_ITEM_WND7:
	case ID_SETTING_MACRO_ITEM_WND8:
	case ID_SETTING_MACRO_ITEM_WND9:
	case ID_SETTING_MACRO_ITEM_WND10:
	case ID_SETTING_MACRO_ITEM_WND11:
	case ID_SETTING_MACRO_ITEM_WND12:
		if( cmd == IOBN_BTNUP )
		{
			SetMacroTextInfo( m_dwSelectItemIDForMacroEdit );
			if( param == SettingMacroItemWnd::ID_PARAM_MACRO_BTNUP )
			{
				SettingMacroItemWnd *pItem = dynamic_cast<SettingMacroItemWnd*> (pWnd);
				if( pItem )
				{
					UpdateSetPage();
					pItem->SetMacroEdit();
					m_dwSelectItemIDForMacroEdit = dwID;
					SetChildActive( ID_APPLY );
				}
			}
			else if( param == SettingMacroItemWnd::ID_PARAM_MACRO_TYP_BTNUP )
			{
				SetMacroTextInfo( dwID );
				UpdateSetPage();
				SetChildActive( ID_APPLY );
			}
		}
		else if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1)
			{
				SetMacroTextInfo( dwID );
				UpdateSetPage();
				SetChildActive( ID_APPLY );
			}
		}
		break;
	}
}

void GameOptionSettingGameTab::iwm_show()
{
	GameOptionBasicWnd::iwm_show();

	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	for (int i = 0; i < MAX_SHOW_ITEM_WND ; i++)
	{
		ShowChildWnd( ID_SETTING_MACRO_ITEM_WND1 + i );
	}

	SetChildInActive( ID_APPLY );
	UpdateByCurrentOption();
	UpdateSetPage();
}

void GameOptionSettingGameTab::Init()
{
	m_DefaultSpec   = Setting::GetSpec();
	m_CurrentOption = Setting::GetSettingOption();
	g_KeyManager.GetCurrent( m_KeyInfo );
}

void GameOptionSettingGameTab::Save()
{
	ApplyChangedOption();

	SetMacroTextInfo( m_dwSelectItemIDForMacroEdit );
	if( g_KeyManager.SetCustomMacroTextInfo( m_KeyInfo ) )
	{
		g_KeyManager.SaveMacroTextInfo();
	}
}

void GameOptionSettingGameTab::SetSlangFilterFunc( DWORD dwID )
{
	CheckRadioButton( ID_SLANG_FILTER_CHANGE, ID_SLANG_FILTER_STAR, dwID );

	switch( dwID )
	{
	case ID_SLANG_FILTER_CHANGE:
		m_CurrentOption.m_bSlangFilterChange = true;
		break;
	case ID_SLANG_FILTER_STAR:
		m_CurrentOption.m_bSlangFilterChange = false;
		break;
	}
}

void GameOptionSettingGameTab::SetSlangFilterTarget( DWORD dwID )
{
	CheckRadioButton( ID_SLANG_FILTER_OTHER, ID_SLANG_FILTER_ALL, dwID );

	switch( dwID )
	{
	case ID_SLANG_FILTER_OTHER:
		m_CurrentOption.m_bSlangFilterOther = true;
		break;
	case ID_SLANG_FILTER_ALL:
		m_CurrentOption.m_bSlangFilterOther = false;
		break;
	}
}

void GameOptionSettingGameTab::UpdateByCurrentOption()
{
	SetButtonCheck( ID_AUTO_REJECT_PARTY_BTN, m_CurrentOption.m_bAutoRejctParty );
	SetButtonCheck( ID_AUTO_REJECT_CAMP_BTN, m_CurrentOption.m_bAutoRejctCamp );
	SetButtonCheck( ID_BLOCK_USERS_CHAT_BTN, m_CurrentOption.m_bBlockUsersChat );
	SetButtonCheck( ID_CHAT_BUBBLE_ALARM, !m_CurrentOption.m_bChatBubbleAlarm );
	SetButtonCheck( ID_GUILD_CHAT_ALARM, !m_CurrentOption.m_bGuildChatAlarm );
	SetButtonCheck( ID_SHOW_SKILL_BUFF_EMOTICON, !m_CurrentOption.m_bHideSkillBuffEmoticon );
	SetButtonCheck( ID_HIDE_MINI_SOLDIER_SELECT_SLOT, m_CurrentOption.m_bHideMiniSoldierSelectSlot );
	SetButtonCheck( ID_HIDE_CHAR_CHANGE_GAUGE, m_CurrentOption.m_bHideCharChangeGauge );
	SetButtonCheck( ID_BLOCK_USERS_CUSTOM_SOUND_BTN, m_CurrentOption.m_bBlockUsersCustomSound );
	SetButtonCheck( ID_COSTUME_SHOW_FUNC, m_CurrentOption.m_bCostumeShowChange );

	if( m_CurrentOption.m_bSlangFilterChange )
		CheckRadioButton( ID_SLANG_FILTER_CHANGE, ID_SLANG_FILTER_STAR, ID_SLANG_FILTER_CHANGE );
	else
		CheckRadioButton( ID_SLANG_FILTER_CHANGE, ID_SLANG_FILTER_STAR, ID_SLANG_FILTER_STAR );

	if( m_CurrentOption.m_bSlangFilterOther )
		CheckRadioButton( ID_SLANG_FILTER_OTHER, ID_SLANG_FILTER_ALL, ID_SLANG_FILTER_OTHER );
	else
		CheckRadioButton( ID_SLANG_FILTER_OTHER, ID_SLANG_FILTER_ALL, ID_SLANG_FILTER_ALL );
}

void GameOptionSettingGameTab::UpdateSetPage()
{
	m_dwSelectItemIDForMacroEdit = ID_NONE_SELECT_ITEM_WND;

	for (int i = 0; i < MAX_SHOW_ITEM_WND ; i++)
	{
		SettingMacroItemWnd *pItem = dynamic_cast<SettingMacroItemWnd*> ( FindChildWnd( ID_SETTING_MACRO_ITEM_WND1 + i));
		if(pItem)
		{
			CKeyInfo::KeyTypeArray eKeyType = (CKeyInfo::KeyTypeArray) (CKeyInfo::KTA_MACRO1 + i );
			CKeyInfo::MecroTextInfo *pInfo = m_KeyInfo.GetMacroTextInfo( eKeyType );
			if( pInfo )
			{
				pItem->SetItem( m_KeyInfo.GetKey( eKeyType ), *pInfo );
			}
		}
	}
}

void GameOptionSettingGameTab::SetMacroTextInfo( DWORD dwID )
{
	SettingMacroItemWnd *pItem = dynamic_cast<SettingMacroItemWnd*> ( FindChildWnd( dwID ) );
	if( pItem )
	{
		CKeyInfo::MecroTextInfo kInfo;
		pItem->GetMecroTextInfo( kInfo );
		if( !kInfo.m_szText.IsEmpty() )
		{
			CKeyInfo::KeyTypeArray eKeyType = (CKeyInfo::KeyTypeArray)( ( dwID - ID_SETTING_MACRO_ITEM_WND1 ) + CKeyInfo::KTA_MACRO1 );
			m_KeyInfo.SetMacroTextInfo( eKeyType, kInfo );
		}
	}
}

void GameOptionSettingGameTab::OnProcessText()
{
	if( m_dwSelectItemIDForMacroEdit == ID_NONE_SELECT_ITEM_WND ) return;

	if( g_Input.IsKeyPress( KEY_ESCAPE )  )
	{
		UpdateSetPage();
	}
}

void GameOptionSettingGameTab::iwm_hide()
{
	if( m_pPreEdit )
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent )
		{
			if( pParent->IsShow() )
			{
				m_pPreEdit->SetKeyFocus();
			}
		}
	}
	m_pPreEdit = NULL;

	for (int i = 0; i < MAX_SHOW_ITEM_WND ; i++)
	{
		HideChildWnd( ID_SETTING_MACRO_ITEM_WND1 + i );
	}
}

void GameOptionSettingGameTab::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	OnProcessText();
}

void GameOptionSettingGameTab::OnRender()
{
	GameOptionBasicWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( m_pIconBack )
	{
		m_pIconBack->SetScale( 0.30f );
		m_pIconBack->Render( iXPos + 30, iYPos + 27, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		m_pIconBack->Render( iXPos + 30, iYPos + 195, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}
	if( m_pIconGame )
	{
		m_pIconGame->SetScale( 0.30f );
		m_pIconGame->Render( iXPos + 30, iYPos + 27, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	if( m_pIconMacro )
	{
		m_pIconMacro->SetScale( 0.30f );
		m_pIconMacro->Render( iXPos + 30, iYPos + 195, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 44, iYPos + 19, FONT_SIZE_14, STR(1) );
	g_FontMgr.PrintText( iXPos + 44, iYPos + 187, FONT_SIZE_14, STR(2) );
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ioPlayStage *GameOptionMovieScreenShotTab::m_pPlayStage = NULL;

GameOptionMovieScreenShotTab::GameOptionMovieScreenShotTab()
{	
	m_pPreEdit = NULL;

	m_pIconBack	     = NULL;
	m_pIconMovie	 = NULL;
	m_pIconScreenShot= NULL;
}

GameOptionMovieScreenShotTab::~GameOptionMovieScreenShotTab()
{
	SAFEDELETE( m_pIconBack );
	SAFEDELETE( m_pIconMovie );
	SAFEDELETE( m_pIconScreenShot );
}

void GameOptionMovieScreenShotTab::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "IconBack" )
	{
		SAFEDELETE( m_pIconBack );
		m_pIconBack = pImage;
	}
	else if( szType == "IconMovie" )
	{
		SAFEDELETE( m_pIconMovie );
		m_pIconMovie = pImage;

	}
	else if( szType == "IconScreenShot" )
	{
		SAFEDELETE( m_pIconScreenShot );
		m_pIconScreenShot = pImage;
	}
	else
	{
		GameOptionBasicWnd::AddRenderImage( szType, pImage );
	}
}

void GameOptionMovieScreenShotTab::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{	
	case ID_MOVIE_SIZE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			OpenResolutionList( pWnd, ioMovieMaker::MO_SIZE, m_CurrentOption.m_iMovieSizeType );
		}
		else if( cmd == IO_RESOLUTION_CHANGE )
		{
			if( m_CurrentOption.m_iMovieSizeType != param )
				SetChildActive( ID_APPLY );

			m_CurrentOption.m_iMovieSizeType   = param;
			UpdateEditBtn(true);
			UpdateByCurrentOption();
		}
		break;
	case ID_MOVIE_FRAME_BTN:
		if( cmd == IOBN_BTNUP )
		{
			OpenResolutionList( pWnd, ioMovieMaker::MO_FRAME, m_CurrentOption.m_iMovieFrameType );
		}
		else if( cmd == IO_RESOLUTION_CHANGE )
		{
			if( m_CurrentOption.m_iMovieFrameType != param )
				SetChildActive( ID_APPLY );

			m_CurrentOption.m_iMovieFrameType  = param;
			UpdateEditBtn(true);
			UpdateByCurrentOption();
		}
		break;
	case ID_MOVIE_QUALITY_BTN:
		if( cmd == IOBN_BTNUP )
		{
			OpenResolutionList( pWnd, ioMovieMaker::MO_QUALITY, m_CurrentOption.m_iMovieQualityType );
		}
		else if( cmd == IO_RESOLUTION_CHANGE )
		{
			if( m_CurrentOption.m_iMovieQualityType != param )
				SetChildActive( ID_APPLY );

			m_CurrentOption.m_iMovieQualityType= param;
			UpdateEditBtn(true);
			UpdateByCurrentOption();
		}
		break;
	case ID_MOVIE_SOUND_BTN:
		if( cmd == IOBN_BTNUP )
		{
			OpenResolutionList( pWnd, ioMovieMaker::MO_SOUND, m_CurrentOption.m_iMovieSoundType );
		}
		else if( cmd == IO_RESOLUTION_CHANGE )
		{
			if( m_CurrentOption.m_iMovieSoundType != param )
				SetChildActive( ID_APPLY );

			m_CurrentOption.m_iMovieSoundType  = param;
			UpdateEditBtn(true);
			UpdateByCurrentOption();
		}
		break;
	case ID_MOVIE_CODEC_BTN:
		if( cmd == IOBN_BTNUP )
		{
			OpenResolutionList( pWnd, ioMovieMaker::MO_CODEC, m_CurrentOption.m_iMovieCodec );
		}
		else if( cmd == IO_RESOLUTION_CHANGE )
		{
			if( m_CurrentOption.m_iMovieCodec != param )
				SetChildActive( ID_APPLY );

			m_CurrentOption.m_iMovieCodec  = param;
			UpdateEditBtn(true);
			UpdateByCurrentOption();
		}
		break;
	case ID_SCREENSHOT_BTN:
		if( cmd == IOBN_BTNUP )
		{
			OpenScreenShotList( pWnd );
		}
		break;
	case ID_SCREEN_SHOT_RESOLUTION_LIST:
		if( cmd == IO_RESOLUTION_CHANGE )
		{
			pWnd->HideWnd();

			bool bScreenShotFormatJPG = ( (param == 0) ? true : false );
			if( m_CurrentOption.m_bScreenShotFormatJPG == bScreenShotFormatJPG )
				return;

			m_CurrentOption.m_bScreenShotFormatJPG = bScreenShotFormatJPG;
			UpdateEditBtn(true);
			UpdateByCurrentOption();
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_TEAMCIRCLE_SHOW_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bNoUIModeTeamCircleShow = !m_CurrentOption.m_bNoUIModeTeamCircleShow;
			UpdateEditBtn(true);
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_NAME_SHOW_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bNoUIModeIDShow = !m_CurrentOption.m_bNoUIModeIDShow;
			UpdateEditBtn(true);
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_HP_SHOW_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bNoUIModeHPShow = !m_CurrentOption.m_bNoUIModeHPShow;
			UpdateEditBtn(true);
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_CHAT_BUBBLE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bNoUIModeChatBubbleShow = !m_CurrentOption.m_bNoUIModeChatBubbleShow;
			UpdateEditBtn(true);
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_MOVIE_MODE_UI_SHOW_BTN:
		if( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bMovieModeUIHide = !m_CurrentOption.m_bMovieModeUIHide;
			UpdateEditBtn(true);
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_SCREENSHOT_PATH_BTN:
		if( cmd == IOBN_BTNUP )
		{
			UpdateEditBtn(false);
			SetChildActive( ID_APPLY );
		}
		break;	
	case ID_SCREENSHOT_PATH_EDIT:
		if( cmd == IOED_INPUT )
		{
			if( param == 0 || param == 1)
			{
				UpdateEditBtn(true);
				SetChildActive( ID_APPLY );
			}
		}
		break;
	case ID_SAVE:
		if ( cmd == IOBN_BTNUP )
		{
			UpdateEditBtn(true);
			SetOptionSavePath();
			Save();

			if( GetParent() )
				GetParent()->HideWnd();
		}
		break;
	case ID_APPLY:
		if ( cmd == IOBN_BTNUP )
		{
			UpdateEditBtn(true);
			SetOptionSavePath();
			Save();

			SetChildInActive( ID_APPLY );
		}
		break;
	case ID_CANCEL:
		if( cmd == IOBN_BTNUP )
		{
			if( GetParent() )
				GetParent()->HideWnd();
		}
		break;
	case ID_DEFAULT:
		if ( cmd == IOBN_BTNUP )
		{
			m_CurrentOption.m_bScreenShotFormatJPG = true;	
			m_CurrentOption.m_iMovieSizeType   = ioMovieMaker::SIZE_DEF;
			m_CurrentOption.m_iMovieFrameType  = ioMovieMaker::FRAME_DEF;
			m_CurrentOption.m_iMovieQualityType= ioMovieMaker::QUALITY_DEF;
			m_CurrentOption.m_iMovieSoundType  = ioMovieMaker::SOUND_DEF;
			m_CurrentOption.m_iMovieCodec      = ioMovieMaker::CODEC_DEF;
			m_CurrentOption.m_bNoUIModeTeamCircleShow = false;
			m_CurrentOption.m_bNoUIModeIDShow = false;
			m_CurrentOption.m_bNoUIModeHPShow = false;
			m_CurrentOption.m_bNoUIModeChatBubbleShow = false;
			m_CurrentOption.m_bMovieModeUIHide= true;

			ioHashString szText;
			GetDefaultScreenShotFolderName( szText );
			SetTextEditBtn( szText.c_str() );
			UpdateEditBtn( true );

			UpdateByCurrentOption();
			SetChildActive( ID_APPLY );
		}
		break;
	case ID_SCREENSHOT_MODE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( m_pPlayStage )
			{
				if( m_pPlayStage->GetModeType() == MT_MYROOM )
				{
					if( ioMyRoomMode::GetMyRoomType() != MRT_PRACTICE )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
						return;
					}
				}
				else if( m_pPlayStage->GetModeType() == MT_PRACTICE )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "수련장에서는 스크린샷 모드를 사용할 수 없습니다." );
					return;
				}

				iwm_command( FindChildWnd( ID_SAVE ), cmd, 0 );
				g_GUIMgr.SetNoneUIMode( true );
			}
		}
		break;
	case ID_MOVIE_MODE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( m_pPlayStage )
			{
				if( m_pPlayStage->GetModeType() == MT_MYROOM )
				{
					if( ioMyRoomMode::GetMyRoomType() != MRT_PRACTICE )
					{
						g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
						return;
					}
				}

				iwm_command( FindChildWnd( ID_SAVE ), cmd, 0 );
				g_App.CheckMouseControlMode( true );
			}
		}
		break;
	}
}

void GameOptionMovieScreenShotTab::iwm_show()
{
	GameOptionBasicWnd::iwm_show();

	m_pPreEdit = ioEdit::m_pKeyFocusEdit;

	SetTextEditBtn( m_CurrentOption.m_szScreenShotSavePath.c_str() );
	UpdateEditBtn( true );
	UpdateByCurrentOption();

	SetChildInActive( ID_APPLY );
}

void GameOptionMovieScreenShotTab::Init()
{
	m_DefaultSpec   = Setting::GetSpec();
	m_CurrentOption = Setting::GetSettingOption();

	if( !CreateScreenShotFolder( m_CurrentOption.m_szScreenShotSavePath.c_str() ) ||
		strcmp( m_CurrentOption.m_szScreenShotSavePath.c_str(), "") == 0          || 
      ( strstr( m_CurrentOption.m_szScreenShotSavePath.c_str(), ":\\") == NULL && strstr( m_CurrentOption.m_szScreenShotSavePath.c_str(), ":/") == NULL ) )
	{	
		bool bMsgBox = true;
		if( strcmp( m_CurrentOption.m_szScreenShotSavePath.c_str(), "") == 0 )
			bMsgBox = false;

		GetDefaultScreenShotFolderName( m_CurrentOption.m_szScreenShotSavePath );
		Save();
		if( bMsgBox)
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
}

void GameOptionMovieScreenShotTab::Save()
{
	ApplyChangedOption();
}

void GameOptionMovieScreenShotTab::UpdateByCurrentOption()
{
	SetButtonCheck( ID_TEAMCIRCLE_SHOW_BTN, m_CurrentOption.m_bNoUIModeTeamCircleShow );
	SetButtonCheck( ID_NAME_SHOW_BTN, m_CurrentOption.m_bNoUIModeIDShow );
	SetButtonCheck( ID_HP_SHOW_BTN, m_CurrentOption.m_bNoUIModeHPShow );
	SetButtonCheck( ID_CHAT_BUBBLE_BTN, m_CurrentOption.m_bNoUIModeChatBubbleShow );
	SetButtonCheck( ID_MOVIE_MODE_UI_SHOW_BTN, !m_CurrentOption.m_bMovieModeUIHide );

	ioButton *pMovieSizeBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_MOVIE_SIZE_BTN ) );
	if( pMovieSizeBtn )
		pMovieSizeBtn->SetTitleText( g_MovieMaker.GetSizeTypeText( m_CurrentOption.m_iMovieSizeType ).c_str() );

	ioButton *pMovieFrameBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_MOVIE_FRAME_BTN ) );
	if( pMovieFrameBtn )
		pMovieFrameBtn->SetTitleText( g_MovieMaker.GetFrameTypeText( m_CurrentOption.m_iMovieFrameType ).c_str() );

	ioButton *pMovieQualityBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_MOVIE_QUALITY_BTN ) );
	if( pMovieQualityBtn )
		pMovieQualityBtn->SetTitleText( g_MovieMaker.GetQualityTypeText( m_CurrentOption.m_iMovieQualityType ).c_str() );

	ioButton *pMovieSoundBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_MOVIE_SOUND_BTN ) );
	if( pMovieSoundBtn )
		pMovieSoundBtn->SetTitleText( g_MovieMaker.GetSoundTypeText( m_CurrentOption.m_iMovieSoundType ).c_str() );

	ioButton *pMovieCodecBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_MOVIE_CODEC_BTN ) );
	if( pMovieCodecBtn )
		pMovieCodecBtn->SetTitleText( g_MovieMaker.GetCodecTypeText( m_CurrentOption.m_iMovieCodec ).c_str() );

	ioButton *pScreenShotBtn = dynamic_cast<ioButton*>( FindChildWnd( ID_SCREENSHOT_BTN ) );
	if( pScreenShotBtn )
	{
		if( m_CurrentOption.m_bScreenShotFormatJPG )
			pScreenShotBtn->SetTitleText( "JPG" );
		else
			pScreenShotBtn->SetTitleText( "BMP" );
	}
}

void GameOptionMovieScreenShotTab::OpenResolutionList( ioWnd *pOpenWnd, int iOptionType, int iValue )
{
	MovieResolutionList *pList = (MovieResolutionList*)FindChildWnd( ID_MOVIE_RESOLUTION_LIST );
	if( pList )
	{
		pList->OpenList( pOpenWnd, iOptionType, iValue );
	}
	UpdateEditBtn(true);
}

void GameOptionMovieScreenShotTab::OpenScreenShotList( ioWnd *pOpenWnd )
{
	StringSelectList *pList = (StringSelectList*)FindChildWnd( ID_SCREEN_SHOT_RESOLUTION_LIST );
	if( pList )
	{
		StringVector vNameList;
		vNameList.reserve( 2 );
		vNameList.push_back( std::string( "JPG" ) );
		vNameList.push_back( std::string( "BMP" ) );
		pList->OpenList( pOpenWnd, vNameList, (int)(!m_CurrentOption.m_bScreenShotFormatJPG), IO_RESOLUTION_CHANGE );
	}
}

bool GameOptionMovieScreenShotTab::CreateScreenShotFolder( const char *szPath )
{
	bool isCreated = true;
	Help::SetCreateDirectory( szPath );
	
	DWORD dwAtt = GetFileAttributes(szPath);
	if( dwAtt == INVALID_FILE_ATTRIBUTES )
	{
		isCreated = false;
	}
	else
	{
		if( dwAtt & FILE_ATTRIBUTE_DIRECTORY)
			isCreated = true;
		else
			isCreated = false;
	}

	return isCreated;
}

void GameOptionMovieScreenShotTab::GetDefaultScreenShotFolderName( OUT ioHashString &rFolderName )
{
	char szDesktopPath[MAX_PATH];
	SHGetSpecialFolderPath( g_App.GetHWnd(), szDesktopPath, CSIDL_DESKTOP, FALSE );

	char szDirectory[MAX_PATH];
	SafeSprintf( szDirectory, sizeof( szDirectory ), STR(1), szDesktopPath );
	rFolderName = szDirectory;
}

void GameOptionMovieScreenShotTab::UpdateEditBtn( bool bShowBtn )
{
	ioButton *pBtn = dynamic_cast<ioButton*> ( FindChildWnd( ID_SCREENSHOT_PATH_BTN ) );
	if( !pBtn )
		return;

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_SCREENSHOT_PATH_EDIT ) );
	if( !pEdit )
		return;

	if( bShowBtn )
	{	
		pEdit->KillKeyFocus();
		ioUITitle *pTitle = pBtn->GetTitle();

		ioHashString szText;
		if(  strcmp( pEdit->GetText(), "") == 0 || 
			( strstr( pEdit->GetText(), ":\\") == NULL && strstr( pEdit->GetText(), ":/") == NULL ) )
		{
			GetDefaultScreenShotFolderName( szText );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		}
		else
			szText = pEdit->GetText();


		if( pTitle )
			pTitle->SetText( szText.c_str(), "");

		ShowChildWnd( ID_SCREENSHOT_PATH_BTN );
		HideChildWnd( ID_SCREENSHOT_PATH_EDIT );
	}	
	else
	{
		ioHashString szText;
		ioUITitle *pTitle = pBtn->GetTitle();
		if( pTitle )
			pTitle->GetFirstText( szText );
	
		pEdit->SetKeyFocus();
		pEdit->SetText( szText.c_str() );
		g_InputBox.SetString( szText.c_str() );

		ShowChildWnd( ID_SCREENSHOT_PATH_EDIT );
		HideChildWnd( ID_SCREENSHOT_PATH_BTN );
	}
}

void GameOptionMovieScreenShotTab::iwm_hide()
{
	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_SCREENSHOT_PATH_EDIT ) );
	if( pEdit )
	{
		pEdit->ClearString();
		pEdit->KillKeyFocus();
	}

	if( m_pPreEdit )	
	{
		ioWnd *pParent = m_pPreEdit->GetParent();
		if( pParent && pParent->IsShow() )
		{
			m_pPreEdit->SetKeyFocus();
		}

		m_pPreEdit = NULL;
	}
}

void GameOptionMovieScreenShotTab::ParseExtraInfo( ioXMLElement &xElement )
{
}

void GameOptionMovieScreenShotTab::iwm_create()
{
	ioButton *pBtn = dynamic_cast<ioButton*> ( FindChildWnd( ID_SCREENSHOT_PATH_BTN ) );
	if( !pBtn )
		return;

	ioUITitle *pTitle = pBtn->GetTitle();
	if( !pTitle )
		return;
	
	pTitle->SetPrintTextStyle( ioUITitle::PTS_WIDTHCUT );
	pTitle->SetWidthCutSize( 300.0f );
}

void GameOptionMovieScreenShotTab::SetTextEditBtn( const char *szSavePath )
{
	ioButton *pBtn = dynamic_cast<ioButton*> ( FindChildWnd( ID_SCREENSHOT_PATH_BTN ) );
	if( !pBtn )
		return;

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_SCREENSHOT_PATH_EDIT ) );
	if( !pEdit )
		return;

	ioUITitle *pTitle = pBtn->GetTitle();
	if( pTitle )
		pTitle->SetText( szSavePath, "");

	pEdit->SetKeyFocus();
	g_InputBox.SetString( szSavePath );
	pEdit->SetText( szSavePath );
}

void GameOptionMovieScreenShotTab::GetTextEditBtn( OUT ioHashString &rszText  )
{
	ioButton *pBtn = dynamic_cast<ioButton*> ( FindChildWnd( ID_SCREENSHOT_PATH_BTN ) );
	if( !pBtn )
		return;

	ioUITitle *pTitle = pBtn->GetTitle();
	if( pTitle )
		pTitle->GetFirstText( rszText );	
}

void GameOptionMovieScreenShotTab::SetOptionSavePath()
{
	ioHashString szText;
	GetTextEditBtn( szText );

	if( CreateScreenShotFolder( szText.c_str() ) )
	{
		m_CurrentOption.m_szScreenShotSavePath = szText;
	}
	else
	{
		ioHashString szDefault;
		GetDefaultScreenShotFolderName( szDefault );
		m_CurrentOption.m_szScreenShotSavePath = szDefault;

		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
	
}

void GameOptionMovieScreenShotTab::OnRender()
{
	GameOptionBasicWnd::OnRender();

	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();
	if( m_pIconBack )
	{
		m_pIconBack->SetScale( 0.30f );
		m_pIconBack->Render( iXPos + 30, iYPos + 27, UI_RENDER_MULTIPLY, TFO_BILINEAR );
		m_pIconBack->Render( iXPos + 30, iYPos + 243, UI_RENDER_MULTIPLY, TFO_BILINEAR );
	}
	if( m_pIconMovie )
	{
		m_pIconMovie->SetScale( 0.30f );
		m_pIconMovie->Render( iXPos + 30, iYPos + 27, UI_RENDER_NORMAL, TFO_BILINEAR );
	}
	if( m_pIconScreenShot )
	{
		m_pIconScreenShot->SetScale( 0.30f );
		m_pIconScreenShot->Render( iXPos + 30, iYPos + 243, UI_RENDER_NORMAL, TFO_BILINEAR );
	}

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );
	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 44, iYPos + 19, FONT_SIZE_14, STR(1) );
	g_FontMgr.PrintText( iXPos + 44, iYPos + 235, FONT_SIZE_14, STR(2) );
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
GameOptionSettingTab::GameOptionSettingTab(void)
{
	m_dwCheckBtn = ID_MOVIE_AND_SCREENSHOT_TAB_BTN;
}

GameOptionSettingTab::~GameOptionSettingTab(void)
{

}

void GameOptionSettingTab::iwm_create()
{
	//g_GUIMgr.AddWnd( "XML/GameOptionSettingCustomSoundTab.xml", this);

	ioWnd *pBtn = FindChildWnd( ID_CUSTOMSOUND_SETTING_TAB_BTN );
	if( pBtn )
	{
		ioHashString szTitle;
		ioUITitle *pTitle = pBtn->GetTitle();
		if( pTitle )
			pTitle->GetFirstText( szTitle );
		pBtn->SetTitleText( szTitle.c_str(), "", -1 );
	}
}

void GameOptionSettingTab::iwm_show()
{
	GameOptionSettingCustomSoundTab *pCustomChild =
	dynamic_cast<GameOptionSettingCustomSoundTab*> ( FindChildWnd( ID_CUSTOMSOUND_SETTING_TAB_WND ) );
	if( pCustomChild )
		pCustomChild->Init();

	//
	ChangeWindow( ID_MOVIE_AND_SCREENSHOT_TAB_BTN );
}

void GameOptionSettingTab::iwm_hide()
{
	HideChildWnd( ID_GAME_SETTING_TAB_WND );
	HideChildWnd( ID_KEY_SETTING_TAB_WND );
	HideChildWnd( ID_MACRO_SETTING_TAB_WND );
	HideChildWnd( ID_CUSTOMSOUND_SETTING_TAB_WND );
	HideChildWnd( ID_MOVIE_AND_SCREENSHOT_TAB_WND );
	HideChildWnd( ID_JOY_KEY_SETTING_TAB_WND );
}

void GameOptionSettingTab::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_GAME_SETTING_TAB_BTN:
	case ID_KEY_SETTING_TAB_BTN:
	case ID_MACRO_SETTING_TAB_BTN:
	case ID_CUSTOMSOUND_SETTING_TAB_BTN:
	case ID_MOVIE_AND_SCREENSHOT_TAB_BTN:
	case ID_JOY_KEY_SETTING_TAB_BTN:
		if( cmd == IOBN_BTNDOWN )
		{
			ChangeWindow( dwID );
		}
		break;
	case ID_GAME_SETTING_TAB_WND:
	case ID_KEY_SETTING_TAB_WND:
	case ID_MACRO_SETTING_TAB_WND:
	case ID_CUSTOMSOUND_SETTING_TAB_WND:
	case ID_MOVIE_AND_SCREENSHOT_TAB_WND:
	case ID_JOY_KEY_SETTING_TAB_WND:
		if( cmd == IOBN_BTNUP )
		{
			if( param == ID_PARAM_CHILD_SAVE )
			{
				GameOptionSettingCustomSoundTab *pCustomChild =
				dynamic_cast<GameOptionSettingCustomSoundTab*> ( FindChildWnd( ID_CUSTOMSOUND_SETTING_TAB_WND ) );
				if( pCustomChild )
					pCustomChild->Save();

				if( GetParent() )
					GetParent()->HideWnd();
			}
			else if( param == ID_PARAM_CHILD_CANCEL )
			{
				if( GetParent() )
					GetParent()->HideWnd();
			}
		}
		break;
	}
}

void GameOptionSettingTab::ChangeWindow( DWORD dwBtn )
{
	m_dwCheckBtn = dwBtn;

	CheckRadioButton( ID_GAME_SETTING_TAB_BTN, ID_JOY_KEY_SETTING_TAB_BTN, m_dwCheckBtn );

	switch( m_dwCheckBtn )
	{
	case ID_GAME_SETTING_TAB_BTN:
		ShowChildWnd( ID_GAME_SETTING_TAB_WND );
		HideChildWnd( ID_KEY_SETTING_TAB_WND );
		HideChildWnd( ID_MACRO_SETTING_TAB_WND );
		HideChildWnd( ID_CUSTOMSOUND_SETTING_TAB_WND );
		HideChildWnd( ID_MOVIE_AND_SCREENSHOT_TAB_WND );
		HideChildWnd( ID_JOY_KEY_SETTING_TAB_WND );
		break;
	case ID_KEY_SETTING_TAB_BTN:
		ShowChildWnd( ID_KEY_SETTING_TAB_WND );
		HideChildWnd( ID_GAME_SETTING_TAB_WND );
		HideChildWnd( ID_MACRO_SETTING_TAB_WND );
		HideChildWnd( ID_CUSTOMSOUND_SETTING_TAB_WND );
		HideChildWnd( ID_MOVIE_AND_SCREENSHOT_TAB_WND );
		HideChildWnd( ID_JOY_KEY_SETTING_TAB_WND );
		break;
	case ID_MACRO_SETTING_TAB_BTN:
		ShowChildWnd( ID_MACRO_SETTING_TAB_WND );
		HideChildWnd( ID_GAME_SETTING_TAB_WND );
		HideChildWnd( ID_KEY_SETTING_TAB_WND );
		HideChildWnd( ID_CUSTOMSOUND_SETTING_TAB_WND );
		HideChildWnd( ID_MOVIE_AND_SCREENSHOT_TAB_WND );
		HideChildWnd( ID_JOY_KEY_SETTING_TAB_WND );
		break;
	case ID_CUSTOMSOUND_SETTING_TAB_BTN:
		ShowChildWnd( ID_CUSTOMSOUND_SETTING_TAB_WND );
		HideChildWnd( ID_MACRO_SETTING_TAB_WND );
		HideChildWnd( ID_GAME_SETTING_TAB_WND );
		HideChildWnd( ID_KEY_SETTING_TAB_WND );
		HideChildWnd( ID_MOVIE_AND_SCREENSHOT_TAB_WND );
		HideChildWnd( ID_JOY_KEY_SETTING_TAB_WND );
		break;
	case ID_MOVIE_AND_SCREENSHOT_TAB_BTN:
		ShowChildWnd( ID_MOVIE_AND_SCREENSHOT_TAB_WND );
		HideChildWnd( ID_GAME_SETTING_TAB_WND );
		HideChildWnd( ID_KEY_SETTING_TAB_WND );
		HideChildWnd( ID_MACRO_SETTING_TAB_WND );
		HideChildWnd( ID_CUSTOMSOUND_SETTING_TAB_WND );
		HideChildWnd( ID_JOY_KEY_SETTING_TAB_WND );
		break;
	case ID_JOY_KEY_SETTING_TAB_BTN:
		ShowChildWnd( ID_JOY_KEY_SETTING_TAB_WND );
		HideChildWnd( ID_MOVIE_AND_SCREENSHOT_TAB_WND );
		HideChildWnd( ID_GAME_SETTING_TAB_WND );
		HideChildWnd( ID_KEY_SETTING_TAB_WND );
		HideChildWnd( ID_MACRO_SETTING_TAB_WND );
		HideChildWnd( ID_CUSTOMSOUND_SETTING_TAB_WND );
		break;
	}
}
