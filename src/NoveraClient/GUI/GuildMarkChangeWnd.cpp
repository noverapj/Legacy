#include "StdAfx.h"

#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioEdit.h"
#include "../io3DEngine/ioButton.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioUIRenderFrame.h"
#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/iostringmanager.h"
#include "../io3DEngine/ioUIImageSetManager.h"

#include "../WndID.h"
#include "../TextColorTable.h"
#include "../ioGuildMarkMgr.h"
#include "../ioApplication.h"
#include "../EtcHelpFunc.h"
#include "../ioGuildInfo.h"
#include "../TCPProtocol.h"
#include "../NetworkWrappingFunc.h"

#include "ioSP2GUIManager.h"
#include "FileListWnd.h"
#include ".\guildmarkchangewnd.h"
#include <strsafe.h>

ioHashString GuildMarkChangeWnd::m_szFileListRootFoler;

GuildMarkChangeWnd::GuildMarkChangeWnd(void)
{
	m_eFileUplaodState = FUS_NONE;

	m_szDefaultBigEdit   = STR(1);	
	m_szDefaultSmallEdit = STR(2);

	m_pEmptyBigMark = NULL;
	m_pEmptySmallMark = NULL;
	m_pBigMark   = NULL;
	m_pSmallMark = NULL;
	m_pMarkFrame = NULL;

	m_dwUploadDelayTime = 0;
}

GuildMarkChangeWnd::~GuildMarkChangeWnd(void)
{
	SAFEDELETE( m_pEmptyBigMark );
	SAFEDELETE( m_pEmptySmallMark );
	SAFEDELETE( m_pBigMark );
	SAFEDELETE( m_pSmallMark );
	SAFEDELETE( m_pMarkFrame );
}

void GuildMarkChangeWnd::iwm_show()
{
	Init();
}

void GuildMarkChangeWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();

	switch( dwID )
	{
	case ID_EXIT:
	case ID_CANCEL_BTN:
		if( cmd == IOBN_BTNUP )
		{
			// 업로드 중이면 블럭 조치
			if( m_eFileUplaodState == FUS_NONE )
				HideWnd();
		}
		break;
	case ID_FIND_BIG_FILE_BTN:
	case ID_FIND_SMALL_FILE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			OpenFileListWnd(dwID);
		}
		break;
	case ID_GUILD_MARK_CHANGE_BTN:
		if( cmd == IOBN_BTNUP )
		{
			if( param == 0 ) // user click
				SetChangeBtnUp();
			else  // g_FileWriteClient 응답
				EndFileUpload( param );
		}
		break;
	case ID_BIG_PATH_EDIT:
	case ID_SMALL_PATH_EDIT:
		if( cmd == IOBN_BTNUP )
		{
			if( param == FileListWnd::COMPLETE_FILE_NAME_PARAM )
			{
				SetMarkImg( dwID );
			}
		}
		break;
	}
}

bool GuildMarkChangeWnd::iwm_esc()
{
	// 업로드 중이면 블럭 조치
	 if( m_eFileUplaodState == FUS_NONE )
		return ioWnd::iwm_esc();

	 return false;
}


void GuildMarkChangeWnd::OnRender()
{
	ioWnd::OnRender();
	int iXPos = GetDerivedPosX();
	int iYPos = GetDerivedPosY();

	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetBkColor( 0, 0, 0 );	

	g_FontMgr.SetTextColor( 255, 255, 255 );
	g_FontMgr.PrintText( iXPos + 44, iYPos + 63, FONT_SIZE_11, STR(1) );
	g_FontMgr.PrintText( iXPos + 283, iYPos + 63, FONT_SIZE_11, STR(2) );

	g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	g_FontMgr.PrintText( iXPos + 74, iYPos + 185, FONT_SIZE_12, STR(3) );
	g_FontMgr.PrintText( iXPos + 74, iYPos + 201, FONT_SIZE_12, STR(4) );

	g_FontMgr.PrintText( iXPos + 178, iYPos + 185, FONT_SIZE_12, STR(5) );
	g_FontMgr.PrintText( iXPos + 178, iYPos + 201, FONT_SIZE_12, STR(6) );

	g_FontMgr.PrintText( iXPos + 313, iYPos + 185, FONT_SIZE_12, STR(7) );
	g_FontMgr.PrintText( iXPos + 313, iYPos + 201, FONT_SIZE_12, STR(8) );

	g_FontMgr.PrintText( iXPos + 417, iYPos + 185, FONT_SIZE_12, STR(9) );
	g_FontMgr.PrintText( iXPos + 417, iYPos + 201, FONT_SIZE_12, STR(10) );

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextColor( TCT_DEFAULT_GRAY );
	g_FontMgr.PrintText( iXPos + 23, iYPos + 296, FONT_SIZE_12, STR(11) );
	g_FontMgr.PrintText( iXPos + 23, iYPos + 314, FONT_SIZE_12, STR(12) );
	g_FontMgr.PrintText( iXPos + 39, iYPos + 332, FONT_SIZE_12, STR(13) );
	
	// current
	ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
	if( pGuildData )
	{
		g_GuildMarkMgr.RenderMarkSize( 100, 100, pGuildData->GetGuildIndex(), pGuildData->GetGuildMark(), iXPos + 24, iYPos + 81 );
		g_GuildMarkMgr.RenderSmallMark( pGuildData->GetGuildIndex(), pGuildData->GetGuildMark(), iXPos + 171, iYPos + 124 );
	}
	else
	{
		if( m_pEmptyBigMark )
			m_pEmptyBigMark->Render( iXPos + 24, iYPos + 81 );
		if( m_pEmptySmallMark )
			m_pEmptySmallMark->Render( iXPos + 171, iYPos + 124 );
	}
	if( m_pMarkFrame )
		m_pMarkFrame->Render( iXPos + 22, iYPos + 79 );

	// new
	if( m_pBigMark )
		m_pBigMark->Render( iXPos + 263, iYPos + 81 );
	else if( m_pEmptyBigMark )
		m_pEmptyBigMark->Render( iXPos + 263, iYPos + 81 );

	if( m_pSmallMark )
		m_pSmallMark->Render( iXPos + 410, iYPos + 124 );
	else if( m_pEmptySmallMark )
		m_pEmptySmallMark->Render( iXPos + 410, iYPos + 124 );

	if( m_pMarkFrame )
		m_pMarkFrame->Render( iXPos + 261, iYPos + 79 );
}

void GuildMarkChangeWnd::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "MarkFrame" )
	{
		SAFEDELETE( m_pMarkFrame );
		m_pMarkFrame = pFrame;
	}
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void GuildMarkChangeWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "EmptyBigMark" )
	{
		SAFEDELETE( m_pEmptyBigMark );
		m_pEmptyBigMark = pImage;
	}
	else if( szType == "EmptySmallMark" )
	{
		SAFEDELETE( m_pEmptySmallMark );
		m_pEmptySmallMark = pImage;
	}
	else
		ioWnd::AddRenderImage( szType, pImage );
}

void GuildMarkChangeWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return ;

	ProcessUploadDelay();
}

void GuildMarkChangeWnd::OpenFileListWnd( DWORD dwID )
{
	FileListWnd *pFileListWnd = dynamic_cast<FileListWnd*> (g_GUIMgr.FindWnd( FILE_LIST_WND ) );
	if( !pFileListWnd )
	{
		LOG.PrintTimeAndLog( 0, "%s Error FileListWnd", __FUNCTION__ );
		return;
	}
	
	if( m_szFileListRootFoler.IsEmpty() )
	{
		char szCurPath[MAX_PATH*2]="";
		GetCurrentDirectory( sizeof( szCurPath ), szCurPath );
		char szDrive[MAX_PATH]="";
		_splitpath( szCurPath, szDrive, NULL, NULL, NULL );
		m_szFileListRootFoler = szDrive;
	}

	pFileListWnd->SetFileList( m_szFileListRootFoler.c_str(), "jpg" );

	DWORD dwEditID = ID_BIG_PATH_EDIT;
	if( dwID == ID_FIND_SMALL_FILE_BTN )
		dwEditID = ID_SMALL_PATH_EDIT;

	pFileListWnd->SetReturnInfo( FindChildWnd( dwEditID ) , FileListWnd::RWT_GUILD_MARK_CHANGE_WND );
	pFileListWnd->ShowWnd();
}

void GuildMarkChangeWnd::SetChangeBtnUp()
{
	if( m_eFileUplaodState != FUS_NONE )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	if( !m_pBigMark || !m_pSmallMark )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}

	ioEdit *pBigEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_BIG_PATH_EDIT ) );
	if( !pBigEdit )
	{
		LOG.PrintTimeAndLog( 0, "%s Edit == NULL", __FUNCTION__ );
		return;
	}

	if( strcmp( pBigEdit->GetText(), "" ) == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}

	ioEdit *pSmallEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_SMALL_PATH_EDIT ) );
	if( !pSmallEdit )
	{
		LOG.PrintTimeAndLog( 0, "%s Edit == NULL (2)", __FUNCTION__ );
		return;
	}

	if( strcmp( pSmallEdit->GetText(), "" ) == 0 )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}

	int iFileSize = Help::GetFileSize( pBigEdit->GetText() );
	if(  iFileSize <= 0 ||  iFileSize > MAX_FILE_SIZE )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5), MAX_FILE_SIZE );
		Init();
		return;
	}

	iFileSize = Help::GetFileSize( pSmallEdit->GetText() );
	if(  iFileSize <= 0 ||  iFileSize > MAX_FILE_SIZE )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6), MAX_FILE_SIZE );
		Init();
		return;
	}

    m_eFileUplaodState = FUS_BIG_FILE_UPLOAD_DALAY;
	m_dwUploadDelayTime= REALGETTIME();
}

void GuildMarkChangeWnd::EndFileUpload( DWORD param )
{
	if( param == ioFileWriteClient::RETURN_WND_PARAM_FAIL )
	{
		Init();
		HideWnd();
	}
	else if( param == ioFileWriteClient::RETURN_WND_PARAM_COMPLETE )
	{
		// big 업로드후 small 업로드 한다.
		if( m_eFileUplaodState == FUS_SMALL_FILE_UPLOAD )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			g_App.NotifyGuildMarkChangeComplete();
			Init();
			HideWnd();
		}
		else if( m_eFileUplaodState == FUS_BIG_FILE_UPLOAD )
		{
			ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_SMALL_PATH_EDIT ) );
			if( !pEdit )
			{
				LOG.PrintTimeAndLog( 0, "%s Edit == NULL", __FUNCTION__ );
				Init();
				HideWnd();
				return;
			}

			int iFileSize = Help::GetFileSize( pEdit->GetText() );
			if(  iFileSize <= 0 ||  iFileSize > MAX_FILE_SIZE )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), MAX_FILE_SIZE );
				Init();
				HideWnd();
				return;
			}

			m_eFileUplaodState = FUS_SMALL_FILE_UPLOAD_DALAY;
			m_dwUploadDelayTime= REALGETTIME();
		}
	}
}

void GuildMarkChangeWnd::Init()
{
	m_eFileUplaodState    = FUS_NONE;

	SAFEDELETE( m_pBigMark );
	SAFEDELETE( m_pSmallMark );

	ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_BIG_PATH_EDIT ) );
	if( pEdit )
		pEdit->SetText( m_szDefaultBigEdit.c_str() );
	pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_SMALL_PATH_EDIT ) );
	if( pEdit )
		pEdit->SetText( m_szDefaultSmallEdit.c_str() );
}

void GuildMarkChangeWnd::SetMarkImg( DWORD dwID )
{
	if( dwID == ID_BIG_PATH_EDIT )
	{
		bool bRight = false;
		ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_BIG_PATH_EDIT ) );
		if( !pEdit )
		{
			LOG.PrintTimeAndLog( 0, "%s Edit == NULL", __FUNCTION__ );
			return;
		}

		D3DXIMAGE_INFO kSrcInfo;

		SAFEDELETE( m_pBigMark );
		m_pBigMark = g_UIImageSetMgr.CreateImageByFullName( "Guildmark_Small#PL001" );

		if( m_pBigMark )
		{
			enum { WIDTH = 128, HIGHT = 128, };
			m_pBigMark->SetHorzFormatting( ioUIRenderImage::HF_STRETCH );
			m_pBigMark->SetVertFormatting( ioUIRenderImage::VF_STRETCH );
			if( m_pBigMark->FillFileImage( pEdit->GetText(), WIDTH, HIGHT, &kSrcInfo ) )
				bRight = true;
		}

		if( !bRight )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			SAFEDELETE( m_pBigMark );
			pEdit->SetText( m_szDefaultBigEdit.c_str() );
		}
		else if( kSrcInfo.Width != BIG_MARK_WIDTH || kSrcInfo.Height != BIG_MARK_HEIGHT )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			SAFEDELETE( m_pBigMark );
			pEdit->SetText( m_szDefaultBigEdit.c_str() );
		}
		else if( kSrcInfo.ImageFileFormat != D3DXIFF_JPG )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			SAFEDELETE( m_pBigMark );
			pEdit->SetText( m_szDefaultBigEdit.c_str() );
		}
		else
		{
			SetFileListRootFolder( pEdit->GetText() );
		}
	}
	else if( dwID == ID_SMALL_PATH_EDIT )
	{
		bool bRight = false;
		ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_SMALL_PATH_EDIT ) );
		if( !pEdit )
		{
			LOG.PrintTimeAndLog( 0, "%s Edit == NULL (2)", __FUNCTION__ );
			return;
		}
		
		D3DXIMAGE_INFO kSrcInfo;

		SAFEDELETE( m_pSmallMark );
		m_pSmallMark = g_UIImageSetMgr.CreateImageByFullName( "Guildmark_Small#PS001" );
		if( m_pSmallMark )
		{
			enum { WIDTH = 16, HIGHT = 16, };
			if( m_pSmallMark->FillFileImage( pEdit->GetText(), WIDTH, HIGHT, &kSrcInfo ) )
				bRight = true;
		}

		if( !bRight )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
			SAFEDELETE( m_pSmallMark );
			pEdit->SetText( m_szDefaultSmallEdit.c_str() );
		}
		else if( kSrcInfo.Width != SMALL_MARK_WIDTH || kSrcInfo.Height != SMALL_MARK_HEIGHT )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
			SAFEDELETE( m_pSmallMark );
			pEdit->SetText( m_szDefaultSmallEdit.c_str() );
		}
		else if( kSrcInfo.ImageFileFormat != D3DXIFF_JPG )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
			SAFEDELETE( m_pSmallMark );
			pEdit->SetText( m_szDefaultSmallEdit.c_str() );
		}
		else
		{
			SetFileListRootFolder( pEdit->GetText() );
		}
	}
}

void GuildMarkChangeWnd::iwm_hide()
{
	SP2Packet kPacket( CTPK_GUILD_MARK_KEY_VALUE_DELETE );
	TCPNetwork::SendToServer( kPacket );
}

void GuildMarkChangeWnd::ProcessUploadDelay()
{
	// g_FileWriteClient.Start()에서 소켓을 연결하게 되는데 패킷을 받을때 호출하면 크래쉬가 발생함. 그래서 딜레이를 주어 처리
	if( m_eFileUplaodState != FUS_BIG_FILE_UPLOAD_DALAY &&
		m_eFileUplaodState != FUS_SMALL_FILE_UPLOAD_DALAY )
		return;

	enum { DELAY_TIME = 100, };
	if( REALGETTIME() - m_dwUploadDelayTime < DELAY_TIME )
		return;

	m_dwUploadDelayTime = 0;
	if( m_eFileUplaodState == FUS_BIG_FILE_UPLOAD_DALAY )
	{
		ioEdit *pBigEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_BIG_PATH_EDIT ) );
		if( !pBigEdit )
		{
			LOG.PrintTimeAndLog( 0, "%s Edit == NULL", __FUNCTION__ );
			return;
		}

		ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( !pGuildData ) 
		{
			LOG.PrintTimeAndLog( 0, "%s GuildData == NULL", __FUNCTION__ );
			return;
		}
		DWORD dwNewGuildMark = max( (DWORD)g_GuildMarkMgr.GetMaxReserveMarkIndex(), pGuildData->GetGuildMark() ) + 1;

		char szWriteFileName[MAX_PATH]="";
		StringCbPrintf( szWriteFileName, sizeof( szWriteFileName ), "%d_%d_l.jpg" , g_GuildInfo.GetGuildIndex(), dwNewGuildMark );

		if( g_FileWriteClient.Start( pBigEdit->GetText(), szWriteFileName, MAX_FILE_SIZE, BIG_MARK_WIDTH, BIG_MARK_HEIGHT, GetID(), ID_GUILD_MARK_CHANGE_BTN ) )
		{
			m_eFileUplaodState = FUS_BIG_FILE_UPLOAD;		
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		}
	}
	else
	{
		ioEdit *pEdit = dynamic_cast<ioEdit*> ( FindChildWnd( ID_SMALL_PATH_EDIT ) );
		if( !pEdit )
		{
			LOG.PrintTimeAndLog( 0, "%s Edit == NULL", __FUNCTION__ );
			Init();
			HideWnd();
			return;
		}

		ioGuildData *pGuildData = g_GuildInfo.GetGuildData( g_GuildInfo.GetGuildIndex() );
		if( !pGuildData ) 
		{
			LOG.PrintTimeAndLog( 0, "%s GuildData == NULL", __FUNCTION__ );
			Init();
			HideWnd();
			return;
		}

		DWORD dwNewGuildMark = max( (DWORD)g_GuildMarkMgr.GetMaxReserveMarkIndex(), pGuildData->GetGuildMark() ) + 1;

		char szWriteFileName[MAX_PATH]="";
		StringCbPrintf( szWriteFileName, sizeof( szWriteFileName ), "%d_%d_s.jpg" , g_GuildInfo.GetGuildIndex(), dwNewGuildMark );

		if( g_FileWriteClient.Start( pEdit->GetText(), szWriteFileName, MAX_FILE_SIZE, SMALL_MARK_WIDTH, SMALL_MARK_HEIGHT, GetID(), ID_GUILD_MARK_CHANGE_BTN ) )
			m_eFileUplaodState = FUS_SMALL_FILE_UPLOAD;
		else
		{
			Init();
			HideWnd();
		}
	}
}

void GuildMarkChangeWnd::SetFileListRootFolder( const char *szFullPath )
{
	char szDrive[MAX_PATH*2]="";
	char szPath[MAX_PATH*2]="";
	_splitpath( szFullPath, szDrive, szPath, NULL, NULL );
	StringCbCat( szDrive, sizeof( szDrive ), szPath );
	int iSize = strlen( szDrive );
	if( COMPARE( iSize-1, 0, MAX_PATH*2) )
		szDrive[iSize-1] = NULL; // end \\ delete
	m_szFileListRootFoler = szDrive;
}