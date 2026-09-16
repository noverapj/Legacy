#include "StdAfx.h"
#include "GuildHomeAdminInfoListWnd.h"

GuildHomeAdminInfoListWnd::GuildHomeAdminInfoListWnd() : m_pGuildData( NULL )
{
}

GuildHomeAdminInfoListWnd::~GuildHomeAdminInfoListWnd()
{

}

void GuildHomeAdminInfoListWnd::iwm_show()
{
}

void GuildHomeAdminInfoListWnd::Update()
{	
	if( m_pGuildData )
	{
		for(int i = ID_USER_LIST_1; i <= ID_USER_LIST_32; ++i )
		{
			GuildHomeAdminListBtn *pUserBtn = dynamic_cast<GuildHomeAdminListBtn*>( FindChildWnd( i ) );
			if( pUserBtn )
			{
				DWORD dwTableIdx, dwUserIdx;
				int nUserLevel;
				ioHashString szUserID, szUserPos;
				m_pGuildData->GetUserInfo( i, dwTableIdx, dwUserIdx, nUserLevel, szUserID, szUserPos );
				if ( szUserID.IsEmpty() )
				{
					pUserBtn->SetInActive();
				}
				else
				{
					pUserBtn->SetUserInfo( m_pGuildData, szUserID, dwUserIdx, nUserLevel, szUserPos );
				}
			}
		}
	}
}

void GuildHomeAdminInfoListWnd::OnProcess( float fTimePerSec )
{
	Update();
}

void GuildHomeAdminInfoListWnd::UpdateUserList( ioGuildData* pGuildData )
{
	m_pGuildData = pGuildData;
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------

GuildHomeAdminListBtn::GuildHomeAdminListBtn()
{
	m_pMasterMark		= NULL;
	m_pSecondMasterMark = NULL;	
}

GuildHomeAdminListBtn::~GuildHomeAdminListBtn()
{
	SAFEDELETE( m_pMasterMark );
	SAFEDELETE( m_pSecondMasterMark );
}

void GuildHomeAdminListBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "MasterMark" )
	{
		SAFEDELETE( m_pMasterMark );
		m_pMasterMark = pImage;
	}
	else if( szType == "SecondMasterMark" )
	{
		SAFEDELETE( m_pSecondMasterMark );
		m_pSecondMasterMark = pImage;
	}
	else
	{
		ioButton::AddRenderImage( szType, pImage );
	}
}

void GuildHomeAdminListBtn::SetUserInfo( ioGuildData *pGuildData, const ioHashString& UserID, DWORD dwUserIdx, int nUserLevel, const ioHashString& szUserPos )
{
	m_UserID		= UserID;
	m_UserPos		= szUserPos; 
	m_nUserLevel	= nUserLevel;
	m_dwUserIdx		= dwUserIdx;
		
	if( HasWndStyle( IWS_INACTIVE ) )
		HideChildWnd( ID_CHECK_BOX );
	else
		ShowChildWnd( ID_CHECK_BOX );

	ioCheckButton* pButton = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_CHECK_BOX ) );
	if( pButton )
	{
		if( g_GuildInfo.IsMyGuild( pGuildData->GetGuildIndex() ) )
		{
			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal )
			{
				if( m_UserPos == pLocal->GetGuildAdminPosition() || m_UserPos == pLocal->GetGuildMasterPostion() || m_UserPos == pLocal->GetGuildSecondMasterPosition() )
				{
					pButton->SetCheck( true );
				}
				else
				{
					pButton->SetCheck( false );
				}
			}

			if( g_GuildInfo.IsGuildMaster() )
			{
				pButton->SetActive();
			}
			else
			{
				pButton->SetInActive();
			}
		}
		else
		{
			pButton->SetInActive();
		}
	}
}

void GuildHomeAdminListBtn::SetInActive()
{
	HideChildWnd( ID_CHECK_BOX );
	ioWnd::SetInActive();
}

void GuildHomeAdminListBtn::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_CHECK_BOX:
		if( cmd == IOBN_BTNUP )
		{
			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal )
			{
				if( m_UserPos == pLocal->GetGuildMasterPostion() || m_UserPos == pLocal->GetGuildSecondMasterPosition() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
					ioCheckButton* pButton = dynamic_cast<ioCheckButton*>( FindChildWnd( ID_CHECK_BOX ) );
					if( pButton )
						pButton->SetCheck( true );
				}
				else if( m_UserPos == pLocal->GetGuildAdminPosition() )
				{
					SP2Packet kPacket( CTPK_GUILD_POSITION_CHANGE );
					kPacket << m_dwUserIdx << m_UserID << pLocal->GetGuildGeneralPosition();
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
				else
				{
					SP2Packet kPacket( CTPK_GUILD_POSITION_CHANGE );
					kPacket << m_dwUserIdx << m_UserID << pLocal->GetGuildAdminPosition();
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
				}
			}
		}
		break;
	}
}

void GuildHomeAdminListBtn::OnRender()
{
	ioButton::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	if( !HasWndStyle( IWS_INACTIVE ) )
	{
		// 계급
		g_LevelMgr.RenderGrade( m_UserID, iXPos + 4, iYPos + 2, m_nUserLevel, TEAM_PRIVATE_1 );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.SetTextStyle( TS_NORMAL );
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
	

		// 아이디 / 직책
		ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
		if( pLocal )
		{
			if( m_UserPos == pLocal->GetGuildMasterPostion() )
			{
				g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 80.0f, m_UserID.c_str() );
				int iRealNameSize = min( 80.0f, g_FontMgr.GetTextWidthCutSize( m_UserID.c_str(), TS_NORMAL, FONT_SIZE_12, 80.0f ) );
				if( m_pMasterMark )
					m_pMasterMark->Render( iXPos + 23 + iRealNameSize + 3, iYPos + 3 );
			}
			else if( m_UserPos == pLocal->GetGuildSecondMasterPosition() )
			{
				g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 71.0f, m_UserID.c_str() );
				int iRealNameSize = min( 71.0f, g_FontMgr.GetTextWidthCutSize( m_UserID.c_str(), TS_NORMAL, FONT_SIZE_12, 71.0f ) );
				if( m_pSecondMasterMark )
					m_pSecondMasterMark->Render( iXPos + 23 + iRealNameSize + 3, iYPos + 3 );
			}
			else
			{
				g_FontMgr.PrintTextWidthCut( iXPos + 23, iYPos + 3, FONT_SIZE_12, 118.0f, m_UserID.c_str() );
			}
		}
	}
}