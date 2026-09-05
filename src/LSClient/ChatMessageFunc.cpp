
#include "stdafx.h"

#include "ChatMessageFunc.h"

#include "ioLadderTeamMgr.h"
#include "ioPlayStage.h"
#include "ioEtcItem.h"
#include "ioDecorationPrice.h"
#include "ioNoviceGradeBenefitManager.h"

#include "GUI/ioFriendManager.h"
#include "GUI/ChatMsgWnd.h"
#include "GUI/GlobalWnd.h"

void SetChatMsg( TeamType eTeam, const char *szID, const char *szChat, bool bNoChatSound, bool bCustomSound /*= false*/ )
{
	if( g_BlackListManager.IsBlackList( ioHashString( szID ) ) ) return;
	if( Setting::UseBlockUsersChat() && !g_FriendManager.IsFriend( szID ) && !g_GuildInfo.IsMyGuildUser( szID ) && g_MyInfo.GetPublicID() != szID ) return;
	if( g_LadderTeamMgr.IsLadderTeam() && g_LadderTeamMgr.IsOtherChatHide() && g_MyInfo.GetPublicID() != szID ) return;

	g_ChatMgr.SetChatMsg( eTeam, szID, szChat , bNoChatSound, bCustomSound );

	g_App.StartChatBubble( szID, szChat, false );
	g_BattleRoomMgr.StartChatBubble( szID, szChat, false );
	g_LadderTeamMgr.StartChatBubble( szID, szChat, false );
}

void SetServerLobbyChatMsg( TeamType eTeam, const char *szID, const char *szChat, bool bNoChatSound )
{
	if(g_BlackListManager.IsBlackList( ioHashString(szID))) return;
	if( Setting::UseBlockUsersChat() && !g_FriendManager.IsFriend( szID ) && !g_GuildInfo.IsMyGuildUser( szID ) && g_MyInfo.GetPublicID() != szID ) return;

	g_ChatMgr.SetServerLobbyChatMsg( eTeam, szID, szChat, bNoChatSound );
}

void SetTeamChatMsg( TeamType eTeam, const char *szID, const char *szChat, bool bNoChatSound, bool bCustomSound /*= false*/ )
{
	if(g_BlackListManager.IsBlackList( ioHashString(szID))) return;
	if( Setting::UseBlockUsersChat() && !g_FriendManager.IsFriend( szID ) && !g_GuildInfo.IsMyGuildUser( szID ) && g_MyInfo.GetPublicID() != szID ) return;

	g_ChatMgr.SetTeamChatMsg( eTeam, szID, szChat , bNoChatSound, bCustomSound );

	g_App.StartChatBubble( szID, szChat, false );
	g_BattleRoomMgr.StartChatBubble( szID, szChat, false );
	g_LadderTeamMgr.StartChatBubble( szID, szChat, false );
}

void SetPartyChatMsg( TeamType eTeam, const char *szID, const char *szChat, bool bNoChatSound )
{
	if(g_BlackListManager.IsBlackList( ioHashString(szID))) return;
	if( Setting::UseBlockUsersChat() && !g_FriendManager.IsFriend( szID ) && !g_GuildInfo.IsMyGuildUser( szID ) && g_MyInfo.GetPublicID() != szID ) return;

	g_ChatMgr.SetPartyChatMsg( eTeam, szID, szChat , bNoChatSound );

	g_App.StartChatBubble( szID, szChat, false );
	g_BattleRoomMgr.StartChatBubble( szID, szChat, false );
}

void SetLadderChatMsg( const char *szID, const char *szChat, bool bNoChatSound )
{
	if(g_BlackListManager.IsBlackList( ioHashString(szID))) return;
	if( Setting::UseBlockUsersChat() && !g_FriendManager.IsFriend( szID ) && !g_GuildInfo.IsMyGuildUser( szID ) && g_MyInfo.GetPublicID() != szID ) return;
	
	g_ChatMgr.SetLadderChatMsg( szID, szChat, bNoChatSound );

	g_App.StartChatBubble( szID, szChat, false );
	g_LadderTeamMgr.StartChatBubble( szID, szChat, false );
}

void SetMemberBalanceMsg( TeamType eTeam )
{
	g_ChatMgr.SetMemberBalanceMsg( eTeam );
}

void SetWholeServerChatMsg( const char *szID, const char *szChat, bool bNoChatSound )
{
	if(g_BlackListManager.IsBlackList( ioHashString(szID))) return;
	if( Setting::UseBlockUsersChat() && !g_FriendManager.IsFriend( szID ) && !g_GuildInfo.IsMyGuildUser( szID ) && g_MyInfo.GetPublicID() != szID ) return;

	g_ChatMgr.SetWholeServerChatMsg( szID, szChat, bNoChatSound );

//	g_App.StartChatBubble( szID, szChat, true );
//	g_BattleRoomMgr.StartChatBubble( szID, szChat, true );
//	g_LadderTeamMgr.StartChatBubble( szID, szChat, true );
}

//확성기2
void SetWholeServerRainbowChatMsg( const char *szID, const char *szChat, bool bNoChatSound )
{
	if(g_BlackListManager.IsBlackList( ioHashString(szID))) return;
	if( Setting::UseBlockUsersChat() && !g_FriendManager.IsFriend( szID ) && !g_GuildInfo.IsMyGuildUser( szID ) && g_MyInfo.GetPublicID() != szID ) return;

	g_ChatMgr.SetWholeServerRainbowChatMsg( szID, szChat, bNoChatSound );
}

void SendChatMsg( TeamType eTeam, const char *szID, const char *szChat, ModeType eModeType /*= MT_NONE */, const int iRoomIndex /*= -1*/,
				  bool bCustomSound /*= false */ )
{
	if( !P2PNetwork::IsNetworkPlaying() )
		return;

	SP2Packet kPacket( CUPK_CHAT );
	kPacket << CT_ALL;
	kPacket << (int)eTeam;
	kPacket << szID;
	kPacket << szChat;
	kPacket << g_ChatMgr.IsNoChatSound();
	kPacket << bCustomSound;

	P2PNetwork::SendToAllPlayingUser( kPacket );

	g_ChatMgr.SendChatLog( ioMannerTrialChatManager::TT_NORMAL_CHAT, szID, szChat, eModeType,iRoomIndex );
}

void SendServerLobbyChatMsg( const char *szID, const char *szChat )
{
	if( !P2PNetwork::IsNetworkPlaying() )
		return;

	// 패킷 전송 TCP
	SP2Packet kPacket( CTPK_SERVER_LOBBY_CHAT );
	kPacket << szID;
	kPacket << szChat;
	TCPNetwork::SendToServer( kPacket );

	g_ChatMgr.SendChatLog( ioMannerTrialChatManager::TT_SERVER_LOBBY_CHAT, szID, szChat, MT_MYROOM );
}

void SendTeamChatMsg( TeamType eTeam, const char *szID, const char *szChat, ModeType eModeType /*= MT_NONE */, const int iRoomIndex /*= -1*/,
					  bool bCustomSound /*= false */ )
{
	if( !P2PNetwork::IsNetworkPlaying() )
		return;

	SP2Packet kPacket( CUPK_CHAT );
	kPacket << CT_TEAM;
	kPacket << (int)eTeam;
	kPacket << szID;
	kPacket << szChat;
	kPacket << g_ChatMgr.IsNoChatSound();
	kPacket << bCustomSound;
	
	P2PNetwork::SendToAllPlayingUser( kPacket );

	g_ChatMgr.SendChatLog( ioMannerTrialChatManager::TT_NORMAL_CHAT, szID, szChat, eModeType ,iRoomIndex);
}

void SendPartyChatMsg( TeamType eTeam, const char *szID, const char *szChat )
{
	if( !g_BattleRoomMgr.IsBattleRoom() )
		return;

	// TCP
	SP2Packet kPacket( CTPK_RELAY_CHAT );
	kPacket << CT_PARTY;
	kPacket << (int)eTeam;
	kPacket << szID;
	kPacket << szChat;
	kPacket << g_ChatMgr.IsNoChatSound();
	TCPNetwork::SendToServer( kPacket );

	g_ChatMgr.SendChatLog( ioMannerTrialChatManager::TT_BATTLE_CHAT, szID, szChat, MT_NONE, g_BattleRoomMgr.GetIndex() );
}

void SendLadderChatMsg( const char *szID, const char *szChat )
{
	if( !g_LadderTeamMgr.IsLadderTeam() )
		return;

	// TCP
	SP2Packet kPacket( CTPK_RELAY_CHAT );
	kPacket << CT_LADDER;;
	kPacket << szID;
	kPacket << szChat;
	kPacket << g_ChatMgr.IsNoChatSound();
	TCPNetwork::SendToServer( kPacket );

	g_ChatMgr.SendChatLog( ioMannerTrialChatManager::TT_LADDER_CHAT, szID, szChat, MT_NONE, g_LadderTeamMgr.GetLadderTeamIndex() );
}

void SendAnnChatMsg( const char *szID, const char *szChat )
{
	if( !P2PNetwork::IsNetworkPlaying() )
		return;

	SP2Packet kPacket( CUPK_CHAT );
	kPacket << CT_ANNOUNCE;
	kPacket << szID;
	kPacket << szChat;

	P2PNetwork::SendToAllPlayingUser( kPacket );
}

void SendWholeServerChatMsg( const char *szID, const char *szChat )
{
	if( !P2PNetwork::IsNetworkPlaying() )
		return;

	// 패킷 전송 TCP
	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)ioEtcItem::EIT_ETC_WHOLE_CHAT;
	kPacket << szChat;
	TCPNetwork::SendToServer( kPacket );

	g_ChatMgr.SendChatLog( ioMannerTrialChatManager::TT_WHOLE_SERVER_CHAT, szID, szChat, MT_NONE );
}

void SendWholeServerRainbowChatMsg( const char *szID, const char *szChat )
{
	if( !P2PNetwork::IsNetworkPlaying() )
		return;

	// 패킷 전송 TCP
	SP2Packet kPacket( CTPK_ETCITEM_USE );
	kPacket << (int)ioEtcItem::EIT_ETC_RAINBOW_WHOLE_CHAT;
	kPacket << szChat;
	TCPNetwork::SendToServer( kPacket );

	g_ChatMgr.SendChatLog( ioMannerTrialChatManager::TT_WHOLE_SERVER_RAINBOW_CHAT, szID, szChat, MT_NONE );
}

//---------------------------------------------------------------------------------------
TextStyle MsgStyle::GetHeadStyle()
{
	if( g_ChatMgr.GetColorTableType() == ioSP2ChatManager::NORMAL_COLOR_TYPE )
		return m_eNormalHeadStyle;
	return m_eCustomHeadStyle;
}

TextStyle MsgStyle::GetChatStyle()
{
	if( g_ChatMgr.GetColorTableType() == ioSP2ChatManager::NORMAL_COLOR_TYPE )
		return m_eNormalChatStyle;
	return m_eCustomChatStyle;
}

DWORD MsgStyle::GetChatOverColor()
{
	if( g_ChatMgr.GetColorTableType() == ioSP2ChatManager::NORMAL_COLOR_TYPE )
		return m_dwNormalChatOverColor;
	return m_dwCustomChatOverColor;
}

ChatMsgItem::ChatMsgItem( MsgStyle *pMsgStyle, const ioHashString &szHead, ioComplexStringPrinter &ChatPrinter, const char *szID, DWORD dwSentenceIndex, bool bFirstLine, bool bMultiLine, ChatType eChatType, bool bCustomSound )
{
	m_Alpha	          = 255;
	m_dwCurTime       = FRAMEGETTIME();
	m_dwStartTime     = 0;
	m_dwSentenceIndex = dwSentenceIndex;
	m_bFirstLine      = bFirstLine;
	m_bMultiLine      = bMultiLine;
	m_eChatType       = eChatType;
	m_bCustomSound    = bCustomSound;
	m_pMsgStyle       = pMsgStyle;
	m_szHead          = szHead;
	
	m_iCurChatColorRed = 0;
	m_iCurChatColorGreen = 0;
	m_iCurChatColorBlue = 0;

	m_dwRainbowRedStartTime = 0;
	m_dwRainbowBlueStartTime = 0;
	m_dwRainbowGreenStartTime = 0;
	m_dwRainbowStartTime = 0;
	m_iCurStyleNum	  = 0;

	if( m_pMsgStyle )
	{
		m_dwHeadColor = m_pMsgStyle->m_dwHeadColor;
		m_HeadPrinter.SetTextStyle( m_pMsgStyle->m_eNormalHeadStyle );
		m_HeadPrinter.SetBkColor( RGB( 0, 0, 0 ) );
		m_HeadPrinter.SetTextColor( m_dwHeadColor );
		m_HeadPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), m_szHead.c_str() );
	}
	ChatPrinter.CopyFullText( m_ChatPrinter );
	
	if( szID )
	{
		m_szChatID = szID;
	}

	if( m_pMsgStyle && !m_szChatID.IsEmpty() )
	{
		m_HeadPrinter.CopyFullText( m_HeadPrinterOver );
		m_HeadPrinterOver.SetFullTextColor( m_pMsgStyle->GetChatOverColor() );

		m_ChatPrinter.CopyFullText( m_ChatPrinterOver );
		m_ChatPrinterOver.SetFullTextColor( m_pMsgStyle->GetChatOverColor() );
	}

	if( g_ChatMgr.GetColorTableType() == ioSP2ChatManager::CUSTOM_COLOR_TYPE )
	{
		ChangeProcess();
	}

	if( m_pMsgStyle && !m_pMsgStyle->m_RainbowTypeList.empty() )
	{
		if ( !m_pMsgStyle->m_RainbowTypeList[g_ChatMgr.GetColorTableType()].empty() )
		{
			DWORD dwCurTime = FRAMEGETTIME();
			m_iCurChatColorRed = m_pMsgStyle->m_RainbowTypeList[g_ChatMgr.GetColorTableType()][0].m_iRedColorMin;
			m_iCurChatColorGreen = m_pMsgStyle->m_RainbowTypeList[g_ChatMgr.GetColorTableType()][0].m_iGreenColorMin;
			m_iCurChatColorBlue = m_pMsgStyle->m_RainbowTypeList[g_ChatMgr.GetColorTableType()][0].m_iBlueColorMin;
			m_dwRainbowRedStartTime = dwCurTime;
			m_dwRainbowBlueStartTime = dwCurTime;
			m_dwRainbowGreenStartTime = dwCurTime;
			m_dwRainbowStartTime = dwCurTime;
			m_iCurStyleNum = 0;
		}
	}
	
}

ChatMsgItem::~ChatMsgItem()
{
}

void ChatMsgItem::AlphaTimeInit()
{
	m_dwCurTime		= FRAMEGETTIME();
	m_Alpha			= MAX_ALPHA_RATE;
	m_dwStartTime	= 0;
}

void ChatMsgItem::ChangeProcess()
{
	if( !m_pMsgStyle ) return;
	// 
	m_HeadPrinter.SetFullTextStyle( m_pMsgStyle->GetHeadStyle() );
	m_dwHeadColor = g_ChatMgr.ChangeColorTableHead( m_dwHeadColor );
	m_HeadPrinter.SetFullTextColor( m_dwHeadColor );

	//
	m_ChatPrinter.SetFullTextStyle( m_pMsgStyle->GetChatStyle() );
	m_ChatPrinter.SetFullTextTableColor();

	//
	m_ChatPrinterOver.SetFullTextStyle( m_pMsgStyle->GetChatStyle() );
	m_ChatPrinterOver.SetFullTextColor( m_pMsgStyle->GetChatOverColor() );
}

bool ChatMsgItem::DrawItem( int iXPos, int iYPos, bool bOver )
{
	if( m_Alpha > 0 )
	{
		if( m_pMsgStyle )
		{
			if( !m_szHead.IsEmpty() )
			{
				if( g_ChatMgr.GetColorTableType() == ioSP2ChatManager::CUSTOM_COLOR_TYPE )
				{
					if( m_pMsgStyle->m_pOutlineHeadImage )
					{
						m_pMsgStyle->m_pOutlineHeadImage->SetAlpha( m_Alpha );
						m_pMsgStyle->m_pOutlineHeadImage->Render( iXPos, iYPos );
					}
				}

				if( m_pMsgStyle->m_pNormalHeadImage )
				{
					if( bOver )
					{
						m_pMsgStyle->m_pNormalHeadImage->SetColor( m_pMsgStyle->GetChatOverColor() );
					}
					else
					{
						m_pMsgStyle->m_pNormalHeadImage->SetColor( m_dwHeadColor );
					}

					m_pMsgStyle->m_pNormalHeadImage->SetAlpha( m_Alpha );
					m_pMsgStyle->m_pNormalHeadImage->Render( iXPos, iYPos, UI_RENDER_NORMAL_ADD_COLOR );
				}

				if( g_ChatMgr.GetColorTableType() == ioSP2ChatManager::NORMAL_COLOR_TYPE )
				{
					if( bOver )
					{
						m_HeadPrinterOver.PrintFullText( iXPos + 18, iYPos + 1, TAT_CENTER, m_Alpha );
					}
					else
					{
						m_HeadPrinter.PrintFullText( iXPos + 18, iYPos + 1, TAT_CENTER, m_Alpha );
					}
				}
				else
				{
					if( bOver )
					{
						m_HeadPrinterOver.PrintFullText( iXPos + 18, iYPos, TAT_CENTER, m_Alpha );
					}
					else
					{
						m_HeadPrinter.PrintFullText( iXPos + 18, iYPos, TAT_CENTER, m_Alpha );
					}
				}
			}

			if( bOver )
			{
				if( g_ChatMgr.GetColorTableType() == ioSP2ChatManager::NORMAL_COLOR_TYPE )      
					m_ChatPrinterOver.PrintFullText( iXPos + 38, iYPos + 1, TAT_LEFT, m_Alpha );
				else
					m_ChatPrinterOver.PrintFullText( iXPos + 37, iYPos, TAT_LEFT, m_Alpha );
			}
			else
			{
				if( g_ChatMgr.GetColorTableType() == ioSP2ChatManager::NORMAL_COLOR_TYPE )
					m_ChatPrinter.PrintFullText( iXPos + 38, iYPos + 1, TAT_LEFT, m_Alpha );
				else
					m_ChatPrinter.PrintFullText( iXPos + 37, iYPos, TAT_LEFT, m_Alpha );
			}
		}		
		return true;
	}
	return false;
}

void ChatMsgItem::SetStartTime( DWORD dwStartTime )
{
	DWORD dwItemLivedTime = FRAMEGETTIME() - m_dwCurTime;
	
	if( dwItemLivedTime <= dwStartTime )
		m_dwStartTime = FRAMEGETTIME();
}

void ChatMsgItem::ProcessItem( DWORD dwStartTime, DWORD dwAlphaTime )
{
	DWORD dwItemLivedTime = FRAMEGETTIME() - m_dwCurTime;
	DWORD dwAlphaGap = 0;

	if( m_dwStartTime > 0 )
	{
		DWORD dwDuration = 0;
		if( m_dwCurTime > m_dwStartTime )
			return;

		dwDuration = m_dwStartTime - m_dwCurTime;
		dwAlphaGap = dwItemLivedTime - dwDuration;
	}
	else
	{
		if( dwItemLivedTime <= dwStartTime )	
		{
			ChangeTextColor( g_ChatMgr.GetColorTableType() );
			return;
		}

		dwAlphaGap = dwItemLivedTime - dwStartTime;
	}

	if(dwAlphaGap < dwAlphaTime )
	{
		float fAlphaRate = (float)dwAlphaGap / (float)dwAlphaTime;
		fAlphaRate = max( 0.0f, min( fAlphaRate, FLOAT1 ) );
		m_Alpha = MAX_ALPHA_RATE - (int)( MAX_ALPHA_RATE * fAlphaRate );
	}
	else
	{
		m_Alpha = 0;
	}
}

bool ChatMsgItem::CheckPtInItemRect( const POINT &ptPos, int iHeight, ioHashString &szReturn, bool bAlphaNoCheck )
{
	if( (m_szChatID.IsEmpty() || m_Alpha == 0) && bAlphaNoCheck == false)
		return false;

	RECT rcItemRect;
	rcItemRect.left = 0;
	rcItemRect.top  = 0;
	rcItemRect.right  = max(0, (int)GetChatWidth() + 38 );
	rcItemRect.bottom = max(0, iHeight );

	if( !PtInRect( &rcItemRect, ptPos ) )
		return false;

	szReturn = m_szChatID;
	return true;
}

float ChatMsgItem::GetChatWidth()
{
	return m_ChatPrinter.GetFullWidth();
}

void ChatMsgItem::ChangeTextColor( int iColorType )
{
	//외침2라면 색을 바꾸는 작업을 시작하자!
	if ( m_eChatType != CT_WHOLE_SERVER_RAINBOW )
		return;
	if ( !COMPARE( iColorType, 0, ioSP2ChatManager::COLOR_TYPE_MAX ) )
		return;
	if ( m_pMsgStyle->m_RainbowTypeList.empty() || m_pMsgStyle->m_RainbowTypeList[iColorType].empty() )
		return;

	int iSize = m_pMsgStyle->m_RainbowTypeList[iColorType].size();

	DWORD dwCurTime = FRAMEGETTIME();
	int iPreRedColor,iPreGreenColor,iPreBlueColor;
	iPreRedColor = m_iCurChatColorRed;
	iPreGreenColor = m_iCurChatColorGreen;
	iPreBlueColor = m_iCurChatColorBlue;
	if ( m_dwRainbowStartTime + m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_dwRainbowDuration < dwCurTime )
	{
		m_dwRainbowStartTime = dwCurTime;
		m_iCurStyleNum++;
		if ( m_iCurStyleNum >= iSize)
			m_iCurStyleNum = 0;

		if ( m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_bRedReverse )
			m_iCurChatColorRed = m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iRedColorMax;
		else
			m_iCurChatColorRed = m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iRedColorMin;

		if ( m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_bBlueReverse )
			m_iCurChatColorBlue = m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iBlueColorMax;
		else
			m_iCurChatColorBlue = m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iBlueColorMin;

		if ( m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_bGreenReverse )
			m_iCurChatColorGreen = m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iGreenColorMax;
		else
			m_iCurChatColorGreen = m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iGreenColorMin;
	}
	else
	{
		//RedColor
		if ( m_dwRainbowRedStartTime + m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_dwRedColorTicTime < dwCurTime )
		{
			m_dwRainbowRedStartTime = dwCurTime;
			if ( m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_bRedReverse )
			{
				m_iCurChatColorRed -= m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_dwRedColorSpeed;
				if ( m_iCurChatColorRed < m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iRedColorMin )
					m_iCurChatColorRed = m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iRedColorMin;
			}
			else
			{
				m_iCurChatColorRed += m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_dwRedColorSpeed;
				if ( m_iCurChatColorRed > m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iRedColorMax )
					m_iCurChatColorRed = m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iRedColorMax;
			}
		}

		//BlueColor
		if ( m_dwRainbowBlueStartTime + m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_dwBlueColorTicTime < dwCurTime )
		{
			m_dwRainbowBlueStartTime = dwCurTime;
			if ( m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_bBlueReverse )
			{
				m_iCurChatColorBlue -= m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_dwBlueColorSpeed;
				if ( m_iCurChatColorBlue < m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iBlueColorMin )
					m_iCurChatColorBlue = m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iBlueColorMin;
			}
			else
			{
				m_iCurChatColorBlue += m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_dwBlueColorSpeed;
				if ( m_iCurChatColorBlue > m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iBlueColorMax )
					m_iCurChatColorBlue = m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iBlueColorMax;
			}
				
		}

		//GreenColor
		if ( m_dwRainbowGreenStartTime + m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_dwGreenColorTicTime < dwCurTime )
		{
			m_dwRainbowGreenStartTime = dwCurTime;
			if ( m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_bGreenReverse )
			{
				m_iCurChatColorGreen -= m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_dwGreenColorSpeed;
				if ( m_iCurChatColorGreen < m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iGreenColorMin )
					m_iCurChatColorGreen = m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iGreenColorMin;
			}
			else
			{
				m_iCurChatColorGreen += m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_dwGreenColorSpeed;
				if ( m_iCurChatColorGreen > m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iGreenColorMax )
					m_iCurChatColorGreen = m_pMsgStyle->m_RainbowTypeList[iColorType][m_iCurStyleNum].m_iGreenColorMax;
			}
				
		}
	}
		
	if ((iPreRedColor != m_iCurChatColorRed) ||	( iPreGreenColor != m_iCurChatColorGreen ) ||
		( iPreBlueColor != m_iCurChatColorBlue ))
	{
		DWORD dwCurTextColor = D3DCOLOR_XRGB( m_iCurChatColorRed, m_iCurChatColorGreen, m_iCurChatColorBlue );
		m_dwHeadColor = dwCurTextColor;
		m_HeadPrinter.SetFullTextColor( m_dwHeadColor );
		m_ChatPrinter.SetFullTextColor( m_dwHeadColor );
	}
}

//-----------------------------------------------------------------------------------
template<> ioSP2ChatManager* Singleton< ioSP2ChatManager >::ms_Singleton = 0;
ioSP2ChatManager::ioSP2ChatManager()
{
	memset( m_szMsgBuf, 0, MSG_BUFF_SIZE );
	m_fChatFontScale      = 0.65f;
	m_iMaxViewMsgCnt      = 0;
	m_iMaxHelpMsgCnt      = 0;
	m_iMaxLimitMsgCnt     = 1000;//MAX_INT_VALUE;
	m_iMaxLimitAlphaMsg   = 10;
	m_dwLastMsgAddTime    = 0;
	m_dwDeleteMsgDuration = 5000;
	m_fSplitWidth         = 0.0f;
	m_dwStartAlphaTime    = 0;
	m_dwCurAlphaTime      = 0;

	m_bNoChatSound        = false;
	m_dwSentenceIndexer   = 0;
	m_pPlayStage		  = NULL;
	m_pCurMsgStyle		  = NULL;

	m_dwColorTableType = NORMAL_COLOR_TYPE;

	m_dwMsgSortFeature = 0;
}

ioSP2ChatManager::~ioSP2ChatManager()
{
	DeleteAllMsg();
	ClearChatStyleMap();
}

void ioSP2ChatManager::ClearChatStyleMap()
{
	MsgStyleMap::iterator iCreate;
	for(iCreate = m_MsgStyleMap.begin();iCreate != m_MsgStyleMap.end();++iCreate)
	{
		MsgStyle *pStyle = iCreate->second;
		if( pStyle )
		{
			SAFEDELETE( pStyle->m_pNormalHeadImage );		
			SAFEDELETE( pStyle->m_pOutlineHeadImage );		
			SAFEDELETE( pStyle );
		}
	}
	m_MsgStyleMap.clear();
}

void ioSP2ChatManager::InitChatManager()
{
	{
		char szKey[MAX_PATH], szBuf[MAX_PATH];
		ioINILoader kLoader( "config/sp2_sound.ini" );
		kLoader.SetTitle_e( "chat_sound" );

		m_DefaultChatSoundList.clear();

		int iSoundCnt = kLoader.LoadInt_e( "chat_sound_count", 0 );
		for( int i=0; i < iSoundCnt; ++i )
		{
			wsprintf_e( szKey, "chat_sound%d", i+1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			m_DefaultChatSoundList.push_back( ioHashString(szBuf) );
		}

		kLoader.LoadString_e( "system_chat_sound", "", szBuf, MAX_PATH );
		m_SystemMsgSound = szBuf;

		kLoader.LoadString_e( "event_chat_sound", "", szBuf, MAX_PATH );
		m_EventMsgSound = szBuf;
	}
	//////////////////////////////////////////////////////////////////////////
	{
		char *pTempColor = NULL;
		ioINILoader_e kLoader( "config/sp2_chat_info.ini" );
		
		kLoader.SetTitle_e( "HeadColorTable" );		
		int i;
		int iMaxTable = kLoader.LoadInt_e( "MaxHeadTable", 0 );
		char szKey[MAX_PATH], szBuf[MAX_PATH];
		for(i = 0;i < iMaxTable;i++)
		{
			ColorFlipTable kTable;
			sprintf_e( szKey, "HeadColorTable%d", i + 1 );
			kLoader.SetTitle( szKey );            
			kLoader.LoadString_e( "NormalType", "", szBuf, MAX_PATH );
			kTable.m_dwNolmalColor = strtoul( szBuf, &pTempColor, 16 );
			kLoader.LoadString_e( "CustomType", "", szBuf, MAX_PATH );
			kTable.m_dwCustomColor = strtoul( szBuf, &pTempColor, 16 );
            m_HeadColorTable.push_back( kTable );
		}

		kLoader.SetTitle_e( "ChatColorTable" );		
		iMaxTable = kLoader.LoadInt_e( "MaxChatTable", 0 );
		for(i = 0;i < iMaxTable;i++)
		{
			ColorFlipTable kTable;
			sprintf_e( szKey, "ChatColorTable%d", i + 1 );
			kLoader.SetTitle( szKey );            
			kLoader.LoadString_e( "NormalType", "", szBuf, MAX_PATH );
			kTable.m_dwNolmalColor = strtoul( szBuf, &pTempColor, 16 );
			kLoader.LoadString_e( "CustomType", "", szBuf, MAX_PATH );
			kTable.m_dwCustomColor = strtoul( szBuf, &pTempColor, 16 );
			m_ChatColorTable.push_back( kTable );
		}

		kLoader.SetTitle_e( "ChatStyle" );		
		int iMaxChatStyle = kLoader.LoadInt_e( "MaxChatStyle", 0 );
		for(i = 0;i < iMaxChatStyle;i++)
		{
			sprintf_e( szKey, "ChatStyle%d", i + 1 );
			kLoader.SetTitle( szKey );

			int iStyleIndex = kLoader.LoadInt_e( "StyleIndex", 0 );
			if( iStyleIndex == 0 ) continue;

			char *pTempColor = NULL;
			MsgStyle *pMsgStyle = new MsgStyle;        
			kLoader.LoadString_e( "NormalHeadImage", "", szBuf, MAX_PATH );
			if( strlen( szBuf ) > 0 )
			{
				pMsgStyle->m_pNormalHeadImage = g_UIImageSetMgr.CreateImageByFullName( szBuf );
			}

			kLoader.LoadString_e( "OutlineHeadImage", "", szBuf, MAX_PATH );
			if( strlen( szBuf ) > 0 )
			{
				pMsgStyle->m_pOutlineHeadImage = g_UIImageSetMgr.CreateImageByFullName( szBuf );
			}

			pMsgStyle->m_eNormalHeadStyle = (TextStyle)kLoader.LoadInt_e( "NormalHeadStyle", 0 );
			pMsgStyle->m_eCustomHeadStyle = (TextStyle)kLoader.LoadInt_e( "CustomHeadStyle", 0 );
			pMsgStyle->m_eNormalChatStyle = (TextStyle)kLoader.LoadInt_e( "NormalTextStyle", 0 );
			pMsgStyle->m_eCustomChatStyle = (TextStyle)kLoader.LoadInt_e( "CustomTextStyle", 0 );

			kLoader.LoadString_e( "NormalChatOverColor", "", szBuf, MAX_PATH );
			pMsgStyle->m_dwNormalChatOverColor = strtoul( szBuf, &pTempColor, 16 );		

			kLoader.LoadString_e( "CustomChatOverColor", "", szBuf, MAX_PATH );
			pMsgStyle->m_dwCustomChatOverColor = strtoul( szBuf, &pTempColor, 16 );

			kLoader.LoadString_e( "HeadColor", "", szBuf, MAX_PATH );
			pMsgStyle->m_dwHeadColor = strtoul( szBuf, &pTempColor, 16 );

			kLoader.LoadString_e( "ChatColor", "", szBuf, MAX_PATH );
			pMsgStyle->m_dwChatColor = strtoul( szBuf, &pTempColor, 16 );

			kLoader.LoadString_e( "BlueIDColor", "", szBuf, MAX_PATH );
			pMsgStyle->m_dwBlueIDColor = strtoul( szBuf, &pTempColor, 16 );

			kLoader.LoadString_e( "RedIDColor", "", szBuf, MAX_PATH );
			pMsgStyle->m_dwRedIDColor = strtoul( szBuf, &pTempColor, 16 );

			InitRainbowChat( kLoader, pMsgStyle );

			m_MsgStyleMap.insert( MsgStyleMap::value_type( iStyleIndex, pMsgStyle ) );
		}
	}
}

void ioSP2ChatManager::InitRainbowChat( ioINILoader& kLoader, MsgStyle* pMsgStyle )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];

	pMsgStyle->m_RainbowTypeList.clear();
	pMsgStyle->m_RainbowTypeList.reserve( COLOR_TYPE_MAX );

	RainbowStyleVec kInfoVec;
	for ( int itype=0 ; itype < COLOR_TYPE_MAX ; ++itype )
	{
		kInfoVec.clear();
		if ( itype == 0 )
			SafeSprintf( szKey, sizeof( szKey ), "Normal" );
		else if ( itype == 1 )
			SafeSprintf( szKey, sizeof( szKey ), "Custom" );

		StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_Rainbow_Style_Count", szKey );
		int iMaxNum =  kLoader.LoadInt( szBuf, 0 );
		if( iMaxNum > 0 )
			kInfoVec.reserve( iMaxNum );

		for( int i=0; i < iMaxNum; ++i )
		{
			RainbowStyle kInfo;

			StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_RedColor%dMin", szKey, i+1 );
			kInfo.m_iRedColorMin = kLoader.LoadInt( szBuf, 0 );
			StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_RedColor%dMax", szKey, i+1 );
			kInfo.m_iRedColorMax = kLoader.LoadInt( szBuf, 0 );
			StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_RedColor%dTicTime", szKey, i+1 );
			kInfo.m_dwRedColorTicTime = (DWORD)kLoader.LoadInt( szBuf, 0 );
			StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_RedColor%dSpeed", szKey, i+1 );
			kInfo.m_dwRedColorSpeed = (DWORD)kLoader.LoadInt( szBuf, 0 );

			StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_BlueColor%dMin", szKey, i+1 );
			kInfo.m_iBlueColorMin = kLoader.LoadInt( szBuf, 0 );
			StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_BlueColor%dMax", szKey, i+1 );
			kInfo.m_iBlueColorMax = kLoader.LoadInt( szBuf, 0 );
			StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_BlueColor%dTicTime", szKey, i+1 );
			kInfo.m_dwBlueColorTicTime = (DWORD)kLoader.LoadInt( szBuf, 0 );
			StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_BlueColor%dSpeed", szKey, i+1 );
			kInfo.m_dwBlueColorSpeed = (DWORD)kLoader.LoadInt( szBuf, 0 );

			StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_GreenColor%dMin", szKey, i+1 );
			kInfo.m_iGreenColorMin = kLoader.LoadInt( szBuf, 0 );
			StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_GreenColor%dMax", szKey, i+1 );
			kInfo.m_iGreenColorMax = kLoader.LoadInt( szBuf, 0 );
			StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_GreenColor%dTicTime", szKey, i+1 );
			kInfo.m_dwGreenColorTicTime = (DWORD)kLoader.LoadInt( szBuf, 0 );
			StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_GreenColor%dSpeed", szKey, i+1 );
			kInfo.m_dwGreenColorSpeed = (DWORD)kLoader.LoadInt( szBuf, 0 );

			StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_RainbowDuration%d", szKey, i+1 );
			kInfo.m_dwRainbowDuration = (DWORD)kLoader.LoadInt( szBuf, 0 );

			StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_RedColor%dReverse", szKey, i+1 );
			kInfo.m_bRedReverse = kLoader.LoadBool( szBuf, false );
			StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_GreenColor%dReverse", szKey, i+1 );
			kInfo.m_bGreenReverse = kLoader.LoadBool( szBuf, false );
			StringCbPrintf_e( szBuf, sizeof(szBuf), "%s_BlueColor%dReverse", szKey, i+1 );
			kInfo.m_bBlueReverse = kLoader.LoadBool( szBuf, false );

			kInfoVec.push_back( kInfo );
		}
		if ( !kInfoVec.empty() )
			pMsgStyle->m_RainbowTypeList.push_back( kInfoVec );
	}
}

void ioSP2ChatManager::SetMaxLimitMsg( int iLimit )
{
	m_iMaxLimitMsgCnt = iLimit;
	ReArrangeByMaxLimit();
}

void ioSP2ChatManager::SetLimitAlphaMsg( int iLimit )
{
	m_iMaxLimitAlphaMsg = iLimit;
}

void ioSP2ChatManager::SetDelMsgDuration( DWORD dwDelDuration )
{
	m_dwDeleteMsgDuration = dwDelDuration;
}

ioHashString ioSP2ChatManager::GetDefaultChatSound()
{
	int iSoundCnt = m_DefaultChatSoundList.size();
	if( iSoundCnt > 0 )
	{
		int iRand = rand() % iSoundCnt;
		return m_DefaultChatSoundList[iRand];
	}
	
	return ioHashString();
}

void ioSP2ChatManager::SetCurChatSound( ioHashString szSound )
{
	m_CurChatSound = szSound;
}

void ioSP2ChatManager::SetSplitWidth( float fWidth )
{
	m_fSplitWidth = fWidth;
}

void ioSP2ChatManager::SetAlphaFade( DWORD dwStartTime, DWORD dwCurTime )
{
	m_dwStartAlphaTime= dwStartTime;
	m_dwCurAlphaTime  = dwCurTime;
}

void ioSP2ChatManager::SetMsgStyle( MsgStyleType eStyle )
{
	MsgStyleMap::iterator iter = m_MsgStyleMap.find( (DWORD)eStyle );
	if( iter != m_MsgStyleMap.end() )
		m_pCurMsgStyle = iter->second;
}

void ioSP2ChatManager::DeleteAllMsg()
{
	ChatMsgList::iterator iter;
	for( iter=m_ChatList.begin() ; iter!=m_ChatList.end() ; ++iter )
	{
		delete *iter;
	}

	m_ChatList.clear();
	m_ChatListByFeature.clear();
}

bool ioSP2ChatManager::CheckHanByte( char *szText )
{
	int len = strlen(szText);
	int hbyte_count = 0;
	for(int i = 0; i < len; i++ )
	{

#if defined( SRC_OVERSEAS )

#if defined( MULTI_BYTE_CHECK )
		if( IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szText[i] ) )
#else
		if( false && IsDBCSLeadByteEx( COUNTRY_CODE_PAGE, (BYTE)szText[i] ) )
#endif

#else
		if( IsDBCSLeadByte( (BYTE)szText[i] ) )
#endif

		{
			i++;		
			if(len <= i) // 마지막 한글이 깨진 글자다.
				return true;
		}	
	}
	
	return false;
}

void ioSP2ChatManager::SplitText( const char *szText, float fFirstSplitWidth  , OUT char szSplitText[MAX_SPLIT_LINE][MAX_PATH])
{
	int  TextLen = strlen(szText);
	const char *pSrc = &szText[0];
	int  iCurLen = 0;
	for(int i = 0;i < MAX_SPLIT_LINE;i++)
	{
		if(iCurLen >= TextLen)
			break;    

		int iLineLen = 0;
		
		int fCurWidth = 0.0f;
		if( i == 0)
			fCurWidth = fFirstSplitWidth;
		else
			fCurWidth = m_fSplitWidth;

		while(true)
		{
			if( iLineLen >= MAX_PATH )
				break;
			szSplitText[i][iLineLen] = *pSrc;
			
			float fTextW = g_FontMgr.GetTextWidth( szSplitText[i], TS_OUTLINE_FULL_2X, GetChatFontScale() );
			if(fTextW < fCurWidth )
			{
				pSrc++;
				iLineLen++;
				iCurLen++;
			}
			else
			{
				szSplitText[i][iLineLen] = NULL;
				if(CheckHanByte(szSplitText[i]))          //한글을 자르면 글자가 깨지므로...
				{
					szSplitText[i][iLineLen - 1] = NULL;
					pSrc--;
					iCurLen--;
				}
				break;    //다음 라인으로 넘김.
			}
			if(iCurLen >= TextLen)
				break;                     //입력된 텍스트를 모두 검사했다.
		}
	}
}

void ioSP2ChatManager::AddChatItem( const char *a_head, ioComplexStringPrinter &rkChatPrinter, const char *szID, bool bFirstLine, bool bMultiLIne, ChatType eChatType /*= CT_NONE*/, bool bCustmSound /*= false */ )
{
	AddSentenceIndex( bFirstLine );
	ChatMsgItem *pItem = new ChatMsgItem( m_pCurMsgStyle, a_head, rkChatPrinter, szID , m_dwSentenceIndexer, bFirstLine , bMultiLIne, eChatType, bCustmSound );
	if( !pItem )	return;

	ChatMsgWnd::ChatRefresh();
	m_ChatList.push_back( pItem );

	bool bInitCheck = false;
	if( szID && pItem->GetChatID() )
	{
		ioHashString szIsID = szID;
		if( g_MyInfo.GetPublicID() == szIsID )
		{
			bool bAdd = CheckEnableAddByFeature( pItem );
			if( !bAdd )
			{
				bInitCheck = true;
			}
		}
	}

	if( bInitCheck )
	{
		GlobalChatWnd *pChatWnd = dynamic_cast<GlobalChatWnd*>( g_GUIMgr.FindWnd( GLOBAL_CHAT_WND ) );
		if( pChatWnd )
		{
			pChatWnd->SetInitState();
		}
	}
	else
	{
		AddChatItemByFeature( pItem );
	}

	ReArrangeByMaxLimit();
	m_dwLastMsgAddTime = FRAMEGETTIME();

	if( m_ChatList.size() > 0 )
	{
		int iGap = m_ChatList.size() - m_iMaxLimitAlphaMsg;
		if( iGap > 0 && m_dwStartAlphaTime > 0 )
		{
			int iIndex = 0;
			ChatMsgList::iterator iter;
			for( iter=m_ChatList.begin() ; iter!=m_ChatList.end() ; ++iter )
			{
				++iIndex;
				if( iIndex == iGap )
				{
					(*iter)->SetStartTime( m_dwStartAlphaTime );
					break;
				}
			}
		}
	}
}

void ioSP2ChatManager::AddHelpItem( const char *a_head, ioComplexStringPrinter &rkChatPrinter, const char *szID, bool bFirstLine, bool bMultiLIne, ChatType eChatType /*= CT_NONE*/, bool bCustmSound /*= false */ )
{
	AddSentenceIndex( bFirstLine );

	ChatMsgItem *pItem = new ChatMsgItem( m_pCurMsgStyle, a_head, rkChatPrinter, szID , m_dwSentenceIndexer, bFirstLine , bMultiLIne, eChatType, bCustmSound );
	if( !pItem )	return;

	ChatMsgWnd::ChatRefresh();
	m_TextHelp.push_back( pItem );
}

void ioSP2ChatManager::ReArrangeByMaxLimit()
{
	ChatMsgList::size_type tMaxCount = (ChatMsgList::size_type)m_iMaxLimitMsgCnt;
	if( m_ChatList.size() <= tMaxCount )
		return;

	while( m_ChatList.size() > tMaxCount )
	{
		DestroyHeadMessage();
	}

	SetMsgWndFeature( m_dwMsgSortFeature );
}

void ioSP2ChatManager::DestroyHeadMessage()
{
	if( !m_ChatList.empty() )
	{
		IMsgItem *pItem = m_ChatList.front();
		delete pItem;

		m_ChatList.pop_front();
	}
}

void ioSP2ChatManager::AllAlphaTimeInit()
{
	if( m_dwStartAlphaTime > 0 && m_dwCurAlphaTime > 0 )
	{
		ChatMsgList::iterator iter;
		for( iter=m_ChatList.begin() ; iter!=m_ChatList.end() ; ++iter )
		{
			IMsgItem *pItem = (*iter);
			if( !pItem ) continue;
			
			pItem->AlphaTimeInit();
		}
	}
}

void ioSP2ChatManager::LoadChatInfo( const ioHashString &rkLoadTitle )
{
	ioINILoader_e kLoader( "config/sp2_chat_info.ini" );
	kLoader.SetTitle( rkLoadTitle.c_str() );

	m_fChatFontScale = kLoader.LoadFloat_e( "ChatFontScale", 0.65f );
	SetAlphaFade( kLoader.LoadInt_e( "StartFadeTime", 0 ), kLoader.LoadInt_e( "FadeOutTime", 0 ) );
	SetSplitWidth( kLoader.LoadInt_e( "SplitWidth", 0 ) );
	SetDelMsgDuration( kLoader.LoadInt_e( "DeleteTime", 0 ) );

	DWORD dwPrevTable = m_dwColorTableType;
	m_dwColorTableType = kLoader.LoadInt_e( "ColorTableType", 0 );
	ChatMsgList::iterator iter;
	for( iter=m_ChatList.begin() ; iter!=m_ChatList.end() ; ++iter )
	{
		IMsgItem *pItem = (*iter);
		if( !pItem ) continue;

		if( dwPrevTable != m_dwColorTableType )
			pItem->ChangeProcess();
		if( m_dwColorTableType == NORMAL_COLOR_TYPE )
			pItem->AlphaTimeInit();
	}
}

void ioSP2ChatManager::ProcessChat()
{
	if( m_ChatList.empty() )	return;
	
	if( m_dwDeleteMsgDuration != 0 )
	{
		DWORD dwCurTime = FRAMEGETTIME();
		if( m_dwLastMsgAddTime + m_dwDeleteMsgDuration < dwCurTime )
		{
			m_dwLastMsgAddTime = dwCurTime;
			DestroyHeadMessage();
		}
	}

	if( m_dwStartAlphaTime > 0 && m_dwCurAlphaTime > 0 )
	{
		ChatMsgList::iterator iter;
		for( iter=m_ChatList.begin() ; iter!=m_ChatList.end() ; ++iter )
		{
			IMsgItem *pItem = (*iter);
			if( !pItem ) continue;
			pItem->ProcessItem( m_dwStartAlphaTime, m_dwCurAlphaTime );
		}
	}
	else
	{
		ChatMsgList::iterator iter;
		for( iter=m_ChatList.begin() ; iter!=m_ChatList.end() ; ++iter )
		{
			IMsgItem *pItem = (*iter);
			if( !pItem ) continue;
			pItem->ChangeTextColor( m_dwColorTableType );
		}
	}
}

IMsgItem* ioSP2ChatManager::GetReverseChatMsg( int list_count )
{
	int iReverse = ( GetMaxChatMsg() - list_count - 1 );
	if( COMPARE( iReverse, 0, GetMaxChatMsg() ) )
		return m_ChatList[iReverse];

	return NULL;
}

IMsgItem* ioSP2ChatManager::GetChatMsg( int list_count )
{
	if( COMPARE( list_count, 0, GetMaxChatMsg() ) )
		return m_ChatList[list_count];

	return NULL;
}

DWORD ioSP2ChatManager::GetColorTableType()
{
	return m_dwColorTableType;
}

DWORD ioSP2ChatManager::ChangeColorTableHead( DWORD dwColor )
{
	int iMaxHead = m_HeadColorTable.size();
	for(int i = 0;i < iMaxHead;i++)
	{
		ColorFlipTable kColorTable = m_HeadColorTable[i]; 
		if( m_dwColorTableType == NORMAL_COLOR_TYPE )
		{
			if( dwColor == kColorTable.m_dwCustomColor )
				return kColorTable.m_dwNolmalColor;
		}
		else
		{
			if( dwColor == kColorTable.m_dwNolmalColor )
				return kColorTable.m_dwCustomColor;
		}
	}
	return dwColor;
}

DWORD ioSP2ChatManager::ChangeColorTableChat( DWORD dwColor )
{
	int iMaxHead = m_ChatColorTable.size();
	for(int i = 0;i < iMaxHead;i++)
	{
		ColorFlipTable kColorTable = m_ChatColorTable[i]; 
		if( m_dwColorTableType == NORMAL_COLOR_TYPE )
		{
			if( dwColor == kColorTable.m_dwCustomColor )
				return kColorTable.m_dwNolmalColor;
		}
		else
		{
			if( dwColor == kColorTable.m_dwNolmalColor )
				return kColorTable.m_dwCustomColor;
		}
	}
	return dwColor;
}

TextStyle ioSP2ChatManager::GetTextStyle()
{
	if( !m_pCurMsgStyle ) return TS_NORMAL;
	
	return m_pCurMsgStyle->m_eNormalChatStyle;
}

DWORD ioSP2ChatManager::GetChatColor()
{
	if( !m_pCurMsgStyle ) return 0;

	return m_pCurMsgStyle->m_dwChatColor;
}

DWORD ioSP2ChatManager::GetChatBlueIDColor()
{
	if( !m_pCurMsgStyle ) return 0;

	return m_pCurMsgStyle->m_dwBlueIDColor;
}

DWORD ioSP2ChatManager::GetChatRedIDColor()
{
	if( !m_pCurMsgStyle ) return 0;

	return m_pCurMsgStyle->m_dwRedIDColor;
}

DWORD ioSP2ChatManager::GetChatBkColor()
{
	if( !m_pCurMsgStyle ) return 0;

	return 0xFF000000;
}

void ioSP2ChatManager::SetMsg( const char *szSource )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_NONEHEAD_CHAT );
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szSource );
	
	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetTextColor( GetChatColor() );	
	kPrinter.SetBkColor( GetChatBkColor() );
	AddChatSplitItem( "", kPrinter, NULL, m_szMsgBuf );
}

void ioSP2ChatManager::SetMsg( const char *szSource, ioComplexStringPrinter& kPrinter )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_NONEHEAD_CHAT );
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szSource );
	
	//ioComplexStringPrinter kPrinter;
	if(kPrinter.IsEmpty())
	{
		kPrinter.SetTextStyle( GetTextStyle() );
		kPrinter.SetTextColor( GetChatColor() );	
		kPrinter.SetBkColor( GetChatBkColor() );
	}
	AddHelpSplitItem( "", kPrinter, NULL, m_szMsgBuf );
}

void ioSP2ChatManager::SetRecvChatMsg( DWORD dwColor, const char *szID, const char *szChat )
{	
	SetMsgStyle( MSG_NONEHEAD_CHAT );
		
	char szSplitText[MAX_SPLIT_LINE][MAX_PATH];
	ZeroMemory( szSplitText, sizeof( szSplitText ) );
	SplitText( szChat, m_fSplitWidth, szSplitText );

	for (int i = 0; i < MAX_SPLIT_LINE ; i++)
	{
		ioComplexStringPrinter kPrinter;
		kPrinter.SetTextStyle( TS_BOLD );
		kPrinter.SetTextColor( dwColor );
		kPrinter.SetBkColor( GetChatBkColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), szSplitText[i] );

		if( i == 0)
		{
			AddChatItem( "", kPrinter, szID , true, (strcmp( szSplitText[1], "") != 0), CT_ALL, false );
		}
		else 
		{
			if( strcmp( szSplitText[i], "") == 0 )
				break;
			
			AddChatItem( "", kPrinter, szID , false, true, CT_ALL, false );
		}
	}
}

void ioSP2ChatManager::SetChatMsg( TeamType eTeam, const char *szID, const char *szChat, bool bNoChatSound , bool bCustomSound /*= false*/ )
{
	ioComplexStringPrinter kPrinter;

	if( eTeam == TEAM_NONE )
	{
		SetMsgStyle( MSG_OBSERVER_CHAT );
		kPrinter.SetTextStyle( GetTextStyle() );
		kPrinter.SetTextColor( GetChatBlueIDColor() );
	}
	else if( eTeam == TEAM_BLUE )
	{
		SetMsgStyle( MSG_BLUE_ALL_CHAT );
		kPrinter.SetTextStyle( GetTextStyle() );
		kPrinter.SetTextColor( GetChatBlueIDColor() );
	}
	else if( eTeam == TEAM_RED )
	{
		SetMsgStyle( MSG_RED_ALL_CHAT );
		kPrinter.SetTextStyle( GetTextStyle() );
		kPrinter.SetTextColor( GetChatRedIDColor() );
	}
	else   //개인전
	{
		if( m_pPlayStage && m_pPlayStage->GetOwnerTeam() == eTeam )
		{
			SetMsgStyle( MSG_BLUE_ALL_CHAT );
			kPrinter.SetTextStyle( GetTextStyle() );
			kPrinter.SetTextColor( GetChatBlueIDColor() );
		}
		else
		{
			SetMsgStyle( MSG_RED_ALL_CHAT );
			kPrinter.SetTextStyle( GetTextStyle() );
			kPrinter.SetTextColor( GetChatRedIDColor() );
		}
	}	
	kPrinter.AddTextPiece( GetChatFontScale(), STR(2), szID );

	AddChatSplitItem( STR(1), kPrinter, szID, szChat, CT_ALL, bCustomSound );

	if( !bNoChatSound )
	{
		if( m_CurChatSound.IsEmpty() )
		{
			g_SoundMgr.PlaySound( GetDefaultChatSound() );
		}
		else
		{
			g_SoundMgr.PlaySound( m_CurChatSound );
		}
	}

	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetServerLobbyChatMsg( TeamType eTeam, const char *szID, const char *szChat, bool bNoChatSound )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_USER_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );
	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(1), szID );
	AddChatSplitItem( STR(2), kPrinter, szID, szChat, CT_SERVER_LOBBY, false );

	if( !bNoChatSound )
	{
		if( m_CurChatSound.IsEmpty() )
		{
			g_SoundMgr.PlaySound( GetDefaultChatSound() );
		}
		else
		{
			g_SoundMgr.PlaySound( m_CurChatSound );
		}
	}

	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetTeamChatMsg( TeamType eTeam, const char *szID, const char *szChat, bool bNoChatSound, bool bCustomSound /*= false*/ )
{
	ioComplexStringPrinter kPrinter;

	if( eTeam == TEAM_BLUE )
	{	
		SetMsgStyle( MSG_BLUE_TEAM_CHAT );
		kPrinter.SetTextStyle( GetTextStyle() );
		kPrinter.SetBkColor( GetChatBkColor() );
		kPrinter.SetTextColor( GetChatBlueIDColor() );
	}
	else if( eTeam == TEAM_RED )
	{	
		SetMsgStyle( MSG_RED_TEAM_CHAT );
		kPrinter.SetTextStyle( GetTextStyle() );
		kPrinter.SetBkColor( GetChatBkColor() );
		kPrinter.SetTextColor( GetChatRedIDColor() );
	}
	else   //개인전
	{
		if( m_pPlayStage && m_pPlayStage->GetOwnerTeam() == eTeam )
		{	
			SetMsgStyle( MSG_BLUE_TEAM_CHAT );
			kPrinter.SetTextStyle( GetTextStyle() );
			kPrinter.SetBkColor( GetChatBkColor() );
			kPrinter.SetTextColor( GetChatBlueIDColor() );
		}
		else
		{	
			SetMsgStyle( MSG_RED_TEAM_CHAT );
			kPrinter.SetTextStyle( GetTextStyle() );
			kPrinter.SetBkColor( GetChatBkColor() );
			kPrinter.SetTextColor( GetChatRedIDColor() );
		}
	}
	kPrinter.AddTextPiece( GetChatFontScale(), STR(2), szID );

	AddChatSplitItem( STR(1), kPrinter, szID, szChat, CT_TEAM, bCustomSound );

	if( !bNoChatSound )
	{
		if( m_CurChatSound.IsEmpty() )
		{
			g_SoundMgr.PlaySound( GetDefaultChatSound() );
		}
		else
		{
			g_SoundMgr.PlaySound( m_CurChatSound );
		}
	}

	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetPartyChatMsg( TeamType eTeam, const char *szID, const char *szChat, bool bNoChatSound )
{
	SetMsgStyle( MSG_USER_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );
	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(2), szID );

	AddChatSplitItem( STR(1), kPrinter, szID, szChat, CT_PARTY );

	if( !bNoChatSound )
	{
		if( m_CurChatSound.IsEmpty() )
		{
			g_SoundMgr.PlaySound( GetDefaultChatSound() );
		}
		else
		{
			g_SoundMgr.PlaySound( m_CurChatSound );
		}
	}

	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetLadderChatMsg( const char *szID, const char *szChat, bool bNoChatSound )
{
	SetMsgStyle( MSG_USER_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );
	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(2), szID );

	AddChatSplitItem( STR(1), kPrinter, szID, szChat, CT_LADDER );

	if( !bNoChatSound )
	{
		if( m_CurChatSound.IsEmpty() )
		{
			g_SoundMgr.PlaySound( GetDefaultChatSound() );
		}
		else
		{
			g_SoundMgr.PlaySound( m_CurChatSound );
		}
	}

	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetSoldierObtainMsg( int iClassType, int iLimitDate, int iPeriodType, int iExerciseStyle )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	char szBuf[MAX_PATH] = "";
	switch( iExerciseStyle )
	{
	case EXERCISE_NONE:                            // 구매 용병
		{
			if( iPeriodType == CPT_TIME )
			{
				SafeSprintf( szBuf, sizeof( szBuf ), STR(1), g_MyInfo.GetClassName( iClassType ), ( ( iLimitDate / 60 ) / 60 ) );
			}
			else
			{
				SafeSprintf( szBuf, sizeof( szBuf ), STR(2), g_MyInfo.GetClassName( iClassType ) );
			}
		}
		break;
	case EXERCISE_GENERAL:                         // 일반 체험 용병
		{
			SafeSprintf( szBuf, sizeof( szBuf ), STR(3), g_MyInfo.GetClassName( iClassType ) );
		}
		break;
	case EXERCISE_PCROOM:                          // PC방 체험 용병
		{
			SafeSprintf( szBuf, sizeof( szBuf ), STR(4), g_MyInfo.GetClassName( iClassType ) );
		}
		break;
	case EXERCISE_EVENT:                           // 레벨 제한 없이 본부에서만 사용 가능한 체험 용병( ExerciseSoldierEvent ) 
		{
			SafeSprintf( szBuf, sizeof( szBuf ), STR(5), g_MyInfo.GetClassName( iClassType ) );
		}
		break;
	}	
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szBuf );
	AddSystemMsg();
}

void ioSP2ChatManager::SetGradeExpObtainMsg( int iGradeExp )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	char szBuf[MAX_PATH] = "";
	SafeSprintf( szBuf, sizeof( szBuf ), STR(1), iGradeExp );
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szBuf );
	AddSystemMsg();
}

void ioSP2ChatManager::SetPesoObtainMsg( int iPeso )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	char szBuf[MAX_PATH] = "";
	SafeSprintf( szBuf, sizeof( szBuf ), STR(1), iPeso );
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szBuf );
	AddSystemMsg();
}

void ioSP2ChatManager::SetDecoObtainMsg( int iClassType, int iSexType, int iDecoType, int iDecoCode )
{
	if( Setting::ChatInfoOff() )
		return;

	const DecoData *pDecoData =  g_DecorationPrice.GetDecoDataPtr( iSexType, iDecoType, iDecoCode );
	if( !pDecoData ) return;

	SetMsgStyle( MSG_INFO_CHAT );

	char szBuf[MAX_PATH] = "";
	if( iSexType == 0 ) 
		SafeSprintf( szBuf, sizeof( szBuf ), STR(1), g_MyInfo.GetClassName( iClassType ), pDecoData->m_szName.c_str() );
	else
		SafeSprintf( szBuf, sizeof( szBuf ), STR(2), g_MyInfo.GetClassName( iClassType ), pDecoData->m_szName.c_str() );
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szBuf );
	AddSystemMsg();
}

void ioSP2ChatManager::SetEtcItemObtainMsg( int iItemType, int iValue )
{
	if( Setting::ChatInfoOff() )
		return;

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iItemType );
	if( !pEtcItem ) return;

	SetMsgStyle( MSG_INFO_CHAT );

	char szBuf[MAX_PATH] = "";
	if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_FRIEND_SLOT_EXTEND )
	{
		SafeSprintf( szBuf, sizeof( szBuf ), STR(1), pEtcItem->GetName().c_str(), iValue );
	}
	else if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_CHAR_SLOT_EXTEND ||
			 pEtcItem->GetType() == ioEtcItem::EIT_ETC_EXTRAITEM_SLOT_EXTEND )
	{
		SafeSprintf( szBuf, sizeof( szBuf ), STR(2), pEtcItem->GetName().c_str(), iValue );
	}
	else if( pEtcItem->GetUseType() == ioEtcItem::UT_ETERNITY )
	{
		SafeSprintf( szBuf, sizeof( szBuf ), STR(3), pEtcItem->GetName().c_str() );
	}
	else if( pEtcItem->GetUseType() == ioEtcItem::UT_TIME )
	{
		if( iValue > 3600 )
		{
			SafeSprintf( szBuf, sizeof( szBuf ), STR(4), pEtcItem->GetName().c_str(), iValue / 3600 );
		}
		else
		{
			SafeSprintf( szBuf, sizeof( szBuf ), STR(5), pEtcItem->GetName().c_str(), iValue / 60 );
		}
	}
	else if( pEtcItem->GetUseType() == ioEtcItem::UT_COUNT || 
			 pEtcItem->GetUseType() == ioEtcItem::UT_ONCE )
	{
		char szConvertNum[MAX_PATH] = "";
		Help::ConvertNumToStrComma( iValue, szConvertNum, sizeof( szConvertNum ) );
		SafeSprintf( szBuf, sizeof( szBuf ), STR(6), pEtcItem->GetName().c_str(), szConvertNum );
	}
	else if( pEtcItem->GetUseType() == ioEtcItem::UT_DATE )
	{
		if( g_EtcItemMgr.IsBlockEtcItem( iItemType ) && iValue == 0 )
			SafeSprintf( szBuf, sizeof( szBuf ), STR(10), pEtcItem->GetName().c_str(), iValue );
		else
			SafeSprintf( szBuf, sizeof( szBuf ), STR(7), pEtcItem->GetName().c_str(), iValue );
	}

	char szBuffer[MAX_PATH] = "";
	if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_SILVER_COIN )
	{		
		SafeSprintf( szBuffer, sizeof( szBuffer ), STR(8), szBuf, g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_SILVER_COIN ) );
	}
	else if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_CLOVER )
	{		
		SafeSprintf( szBuffer, sizeof( szBuffer ), "%s (%d개 보유)", szBuf, g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_CLOVER ) );
	}
	else if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_MILEAGE_COIN )
	{		
		SafeSprintf( szBuffer, sizeof( szBuffer ), "%s: %d", szBuf, g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_MILEAGE_COIN ) );
	}
	else if( pEtcItem->GetType() == ioEtcItem::EIT_ETC_SOUL_STONE )
	{		
		SafeSprintf( szBuffer, sizeof( szBuffer ), "%s (%d개 보유)", szBuf, g_MyInfo.GetEtcCoinMoney( ioEtcItem::EIT_ETC_SOUL_STONE ) );
	}
	else
	{
		SafeSprintf( szBuffer, sizeof( szBuffer ), STR(9), szBuf );
	}
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szBuffer );
	AddSystemMsg();
}

void ioSP2ChatManager::SetExtraItemObtainMsg( int iItemType, int iReinforce, int iPeriodType, int iDate )
{
	if( Setting::ChatInfoOff() )
		return;

	const ioItem *pItem = g_ItemMaker.GetItemConst( iItemType, __FUNCTION__ );
	if( !pItem ) return;

	SetMsgStyle( MSG_INFO_CHAT );

	char szBuf[MAX_PATH] = "";
	if( iPeriodType != EPT_MORTMAIN )
	{
		if( (iDate/24) > 0 )
			SafeSprintf( szBuf, sizeof( szBuf ), STR(1), pItem->GetName().c_str(), iReinforce, (iDate/24) );
		else
			SafeSprintf( szBuf, sizeof( szBuf ), STR(2), pItem->GetName().c_str(), iReinforce, iDate );
	}
	else
	{
		SafeSprintf( szBuf, sizeof( szBuf ), STR(3), pItem->GetName().c_str(), iReinforce );
	}
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szBuf );
	AddSystemMsg();
}

void ioSP2ChatManager::SetMedalItemObtainMsg( int iItemType, int iLimitTime )
{
	if( Setting::ChatInfoOff() )
		return;

	const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( iItemType );
	if( !pItemInfo )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	char szBuf[MAX_PATH]="";
	if( iLimitTime != 0 )
	{
		if( (iLimitTime/24) > 0 )
			SafeSprintf( szBuf, sizeof( szBuf ), STR(1), pItemInfo->m_sName.c_str(), (iLimitTime/24) );
		else
			SafeSprintf( szBuf, sizeof( szBuf ), STR(2), pItemInfo->m_sName.c_str(), iLimitTime );
	}
	else
	{
		SafeSprintf( szBuf, sizeof( szBuf ), STR(3), pItemInfo->m_sName.c_str() );
	}
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szBuf );
	AddSystemMsg();
}

void ioSP2ChatManager::SetCostumeObtainMsg( int nCostumeCode, int nPeriodType, int nDate )
{
	if( Setting::ChatInfoOff() )
		return;

	ioHashString szName = g_CostumeInfoMgr.GetCostumeName( nCostumeCode );

	SetMsgStyle( MSG_INFO_CHAT );

	char szBuf[MAX_PATH] = "";
	if( nPeriodType != EPT_MORTMAIN )
	{
		if( (nDate/24) > 0 )
			SafeSprintf( szBuf, sizeof( szBuf ), "[%s +%d일] 장비 획득", szName.c_str(), (nDate/24) );
		else
			SafeSprintf( szBuf, sizeof( szBuf ), "[%s +%d시간] 장비 획득", szName.c_str(), nDate );
	}
	else
	{
		SafeSprintf( szBuf, sizeof( szBuf ), "[%s] 영구 장비 획득", szName.c_str() );
	}
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szBuf );
	AddSystemMsg();
}

void ioSP2ChatManager::SetWholeServerChatMsg( const char *szID, const char *szChat, bool bNoChatSound )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_WHOLE_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );
	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(2), szID );
	AddChatSplitItem( STR(1), kPrinter, szID, szChat, CT_WHOLE_SERVER, false );

	if( !bNoChatSound )
	{
		if( m_CurChatSound.IsEmpty() )
		{
			g_SoundMgr.PlaySound( GetDefaultChatSound() );
		}
		else
		{
			g_SoundMgr.PlaySound( m_CurChatSound );
		}
	}

	m_CurChatSound.Clear();
}

//확성기2
void ioSP2ChatManager::SetWholeServerRainbowChatMsg( const char *szID, const char *szChat, bool bNoChatSound )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_WHOLE_CHANGE_COLOR_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );
	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(2), szID );
	AddChatSplitItem( STR(1), kPrinter, szID, szChat, CT_WHOLE_SERVER_RAINBOW, false );

	if( !bNoChatSound )
	{
		if( m_CurChatSound.IsEmpty() )
		{
			g_SoundMgr.PlaySound( GetDefaultChatSound() );
		}
		else
		{
			g_SoundMgr.PlaySound( m_CurChatSound );
		}
	}

	m_CurChatSound.Clear();
}


void ioSP2ChatManager::AddSystemMsg()
{
	if( Setting::ChatInfoOff() )
		return;

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );
	kPrinter.SetTextColor( GetChatColor() );
	AddChatSplitItem( STR(1), kPrinter, NULL, m_szMsgBuf );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
void ioSP2ChatManager::SetSystemMsg( const char *szSource, ... )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	if( ioLocalManagerParent::IsVPrintf() )
	{
		va_list args;
		va_start( args, szSource );
		StringCbVPrintf( m_szMsgBuf, MSG_BUFF_SIZE, szSource, args );
		va_end( args );
		AddSystemMsg();
	}
	else
	{
		SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szSource );
		AddSystemMsg();
	}
}
#else
void ioSP2ChatManager::SetSystemMsg( const char *szSource )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szSource );
	AddSystemMsg();
}
#endif

void ioSP2ChatManager::SetSystemMsg( const char *szSource , int iArg1 )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szSource, iArg1 );
	AddSystemMsg();
}

void ioSP2ChatManager::SetSystemMsg( const char *szSource, const char *szArg1 )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szSource, szArg1 );
	AddSystemMsg();
}

void ioSP2ChatManager::SetSystemMsg( const char *szSource, int iArg1, int iArg2 )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szSource, iArg1, iArg2 );
	AddSystemMsg();
}

void ioSP2ChatManager::SetSystemMsg( const char *szSource, const char *szArg1, int iArg2 )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szSource, szArg1, iArg2 );
	AddSystemMsg();
}

void ioSP2ChatManager::SetSystemMsg( const char *szSource, const char *szArg1, int iArg2, int iArg3 )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szSource, szArg1, iArg2, iArg3 );
	AddSystemMsg();
}

void ioSP2ChatManager::SetSystemMsg( const char *szSource, int iArg1, const char *szArg2, int iArg3, int iArg4 )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szSource, iArg1, szArg2, iArg3, iArg4 );
	AddSystemMsg();
}

void ioSP2ChatManager::SetSystemMsg( const char *szSource, const char *szArg1, int iArg2, int iArg3, int iArg4 )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szSource, szArg1, iArg2, iArg3, iArg4 );
	AddSystemMsg();
}

void ioSP2ChatManager::SetSystemMsg( const char *szSource, int iArg1, int iArg2, int iArg3, int iArg4, int iArg5, int iArg6 )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );
	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szSource, iArg1, iArg2, iArg3, iArg4, iArg5, iArg6 );
	AddSystemMsg();
}

void ioSP2ChatManager::SetInfomationMsg( const char *szMsg )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );
	kPrinter.SetTextColor( GetChatColor() );
	AddChatSplitItem( STR(1), kPrinter, NULL, szMsg );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetKOMsgTeamColor( TeamType eTeam, const ioHashString &szID, ioComplexStringPrinter &rkPrinter )
{
	if( Setting::ChatInfoOff() )
		return;

	if( eTeam == TEAM_BLUE )
		rkPrinter.SetTextColor( GetChatBlueIDColor() );
	else if( eTeam == TEAM_RED )
		rkPrinter.SetTextColor( GetChatRedIDColor() );
	else
	{
		if( szID == g_MyInfo.GetPublicID() )
			rkPrinter.SetTextColor( GetChatBlueIDColor() );
		else
			rkPrinter.SetTextColor( GetChatRedIDColor() );
	}
}

void ioSP2ChatManager::SetLastScoreMsg( TeamType eTeam, ioHashString szName )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );

	if( !szName.IsEmpty() )
	{
		if( eTeam == TEAM_BLUE )
			kPrinter.SetTextColor( GetChatBlueIDColor() );
		else
			kPrinter.SetTextColor( GetChatRedIDColor() );

		kPrinter.AddTextPiece( GetChatFontScale(), szName.c_str() );
		
		kPrinter.SetTextColor( GetChatColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(2) );
	}
	else
	{
		if( eTeam == TEAM_BLUE )
		{
			kPrinter.SetTextColor( GetChatBlueIDColor() );
			kPrinter.AddTextPiece( GetChatFontScale(), STR(3) );
		}
		else
		{
			kPrinter.SetTextColor( GetChatRedIDColor() );
			kPrinter.AddTextPiece( GetChatFontScale(), STR(4) );
		}

		kPrinter.SetTextColor( GetChatColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(5) );
	}

	AddChatItem( STR(1), kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetScoreMsg( TeamType eTeam, ioHashString szName, bool bParty )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );

	if( bParty && !szName.IsEmpty() )
	{
		if( eTeam == TEAM_BLUE )
			kPrinter.SetTextColor( GetChatBlueIDColor() );
		else
			kPrinter.SetTextColor( GetChatRedIDColor() );

		kPrinter.AddTextPiece( GetChatFontScale(), szName.c_str() );
		
		kPrinter.SetTextColor( GetChatColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(2) );
	}
	else
	{
		if( eTeam == TEAM_BLUE )
		{
			kPrinter.SetTextColor( GetChatBlueIDColor() );
			kPrinter.AddTextPiece( GetChatFontScale(), STR(3) );
		}
		else
		{
			kPrinter.SetTextColor( GetChatRedIDColor() );
			kPrinter.AddTextPiece( GetChatFontScale(), STR(4) );
		}

		kPrinter.SetTextColor( GetChatColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(5) );
	}

	AddChatItem( STR(1), kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}


void ioSP2ChatManager::SetSymbolDieMsg( TeamType eTeam, const ioHashString &szName )
{
	if( Setting::ChatInfoOff() )
		return;

	if( szName.IsEmpty() ) return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );

	if( eTeam == TEAM_BLUE )
		kPrinter.SetTextColor( GetChatBlueIDColor() );
	else if( eTeam == TEAM_RED )
		kPrinter.SetTextColor( GetChatRedIDColor() );
	else
	{
		if( m_pPlayStage && m_pPlayStage->GetOwnerTeam() == eTeam )
			kPrinter.SetTextColor( GetChatBlueIDColor() );
		else
			kPrinter.SetTextColor( GetChatRedIDColor() );
	}

	kPrinter.AddTextPiece( GetChatFontScale(), szName.c_str() );

	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(2) );

	AddChatItem( STR(1), kPrinter, NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetGetCrownMsg( TeamType eTeam, const ioHashString &szName )
{
	if( Setting::ChatInfoOff() )
		return;

	if( szName.IsEmpty() ) return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );

	if( eTeam == TEAM_BLUE )
		kPrinter.SetTextColor( GetChatBlueIDColor() );
	else
		kPrinter.SetTextColor( GetChatRedIDColor() );

	kPrinter.AddTextPiece( GetChatFontScale(), szName.c_str() );

	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(2) );

	AddChatItem( STR(1), kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetDropCrownMsg( TeamType eAttacker, const ioHashString &szAttacker,
										TeamType eKing, const ioHashString &szKing )
{
	if( Setting::ChatInfoOff() )
		return;

	if( szKing.IsEmpty() ) return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );

	if( szAttacker.IsEmpty() )
	{
		if( eKing == TEAM_BLUE )
			kPrinter.SetTextColor( GetChatBlueIDColor() );
		else
			kPrinter.SetTextColor( GetChatRedIDColor() );

		kPrinter.AddTextPiece( GetChatFontScale(), szKing.c_str() );

		kPrinter.SetTextColor( GetChatColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(2) );
	}
	else
	{
		if( eAttacker == TEAM_BLUE )
			kPrinter.SetTextColor( GetChatBlueIDColor() );
		else
			kPrinter.SetTextColor( GetChatRedIDColor() );

		kPrinter.AddTextPiece( GetChatFontScale(), szAttacker.c_str() );

		kPrinter.SetTextColor( GetChatColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(3) );

		if( eKing == TEAM_BLUE )
			kPrinter.SetTextColor( GetChatBlueIDColor() );
		else
			kPrinter.SetTextColor( GetChatRedIDColor() );

		kPrinter.AddTextPiece( GetChatFontScale(), szKing.c_str() );

		kPrinter.SetTextColor( GetChatColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(4) );
	}

	AddChatItem( STR(1), kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetBadPingCrownDropMsg( TeamType eKing, const ioHashString &szKing )
{
	if( Setting::ChatInfoOff() )
		return;

	if( szKing.IsEmpty() ) return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );

	if( eKing == TEAM_BLUE )
		kPrinter.SetTextColor( GetChatBlueIDColor() );
	else
		kPrinter.SetTextColor( GetChatRedIDColor() );

	kPrinter.AddTextPiece( GetChatFontScale(), szKing.c_str() );

	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(2) );

	AddChatItem( STR(1), kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetDropFlagMsg( TeamType eTeam, const ioHashString &szName )
{
	if( Setting::ChatInfoOff() )
		return;

	if( szName.IsEmpty() ) return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );

	if( eTeam == TEAM_BLUE )
		kPrinter.SetTextColor( GetChatBlueIDColor() );
	else
		kPrinter.SetTextColor( GetChatRedIDColor() );

	kPrinter.AddTextPiece( GetChatFontScale(), szName.c_str() );

	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(2) );

	AddChatItem( STR(1), kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetPickupFlagMsg( TeamType eTeam, const ioHashString &szName )
{
	if( Setting::ChatInfoOff() )
		return;

	if( szName.IsEmpty() ) return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );

	if( eTeam == TEAM_BLUE )
		kPrinter.SetTextColor( GetChatBlueIDColor() );
	else
		kPrinter.SetTextColor( GetChatRedIDColor() );

	kPrinter.AddTextPiece( GetChatFontScale(), szName.c_str() );

	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(2) );

	AddChatItem( STR(1), kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetBadPingFlagDropMsg( TeamType eTeam, const ioHashString &szName )
{
	if( Setting::ChatInfoOff() )
		return;

	if( szName.IsEmpty() ) return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );

	if( eTeam == TEAM_BLUE )
		kPrinter.SetTextColor( GetChatBlueIDColor() );
	else
		kPrinter.SetTextColor( GetChatRedIDColor() );

	kPrinter.AddTextPiece( GetChatFontScale(), szName.c_str() );

	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(2) );

	AddChatItem( STR(1), kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetGoalMsg( TeamType eGoalTeam, TeamType eAttackerTeam, const ioHashString &szName,
								   TeamType eAssistTeam, const ioHashString &szAssist )
{
	if( Setting::ChatInfoOff() )
		return;

	if( szName.IsEmpty() ) return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );

	if( eGoalTeam == eAttackerTeam )
	{
		if( eGoalTeam == TEAM_BLUE )
			kPrinter.SetTextColor( GetChatBlueIDColor() );
		else if( eGoalTeam == TEAM_RED )
			kPrinter.SetTextColor( GetChatRedIDColor() );

		kPrinter.AddTextPiece( GetChatFontScale(), STR(1), szName.c_str() );

		kPrinter.SetTextColor( GetChatColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(2) );

		if( eGoalTeam == eAssistTeam )
		{
			if( eGoalTeam == TEAM_BLUE )
				kPrinter.SetTextColor( GetChatBlueIDColor() );
			else if( eGoalTeam == TEAM_RED )
				kPrinter.SetTextColor( GetChatRedIDColor() );

			kPrinter.AddTextPiece( GetChatFontScale(), STR(3), szAssist.c_str() );

			kPrinter.SetTextColor( GetChatColor() );
			kPrinter.AddTextPiece( GetChatFontScale(), STR(4) );
		}
	}
	else
	{
		if( eGoalTeam == TEAM_BLUE )
		{
			kPrinter.SetTextColor( GetChatBlueIDColor() );
			kPrinter.AddTextPiece( GetChatFontScale(), STR(5) );
		}
		else if( eGoalTeam == TEAM_RED )
		{
			kPrinter.SetTextColor( GetChatRedIDColor() );
			kPrinter.AddTextPiece( GetChatFontScale(), STR(6) );
		}

		kPrinter.SetTextColor( GetChatColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(7) );


		if( eAttackerTeam == TEAM_BLUE )
			kPrinter.SetTextColor( GetChatBlueIDColor() );
		else if( eAttackerTeam == TEAM_RED )
			kPrinter.SetTextColor( GetChatRedIDColor() );

		kPrinter.AddTextPiece( GetChatFontScale(), STR(8), szName.c_str() );

		kPrinter.SetTextColor( GetChatColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(9) );
	}

	AddChatItem( STR(10), kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetJoinUserChatMsg( const char *szID, TeamType eTeam, int nGrade )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );

	if( m_pPlayStage )
	{
		ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayStage->GetPlayMode() );
		if( pHeadquartersMode )
		{
			if( pHeadquartersMode->GetMasterName() == szID )
			{
				kPrinter.SetTextColor( GetChatColor() );
				kPrinter.AddTextPiece( GetChatFontScale(), STR(8) );
			}
		}
	}

	if( eTeam == TEAM_BLUE )
		kPrinter.SetTextColor( GetChatBlueIDColor() );
	else if( eTeam == TEAM_RED )
		kPrinter.SetTextColor( GetChatRedIDColor() );
	else
	{
		if( m_pPlayStage && m_pPlayStage->GetOwnerTeam() == eTeam )
			kPrinter.SetTextColor( GetChatBlueIDColor() );
		else
			kPrinter.SetTextColor( GetChatRedIDColor() );
	}
	
	kPrinter.AddTextPiece( GetChatFontScale(), szID );
	
	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(2) );
	
	if( eTeam == TEAM_BLUE )
	{
		kPrinter.SetTextColor( GetChatBlueIDColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(3));
		kPrinter.SetTextColor( GetChatColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(4) );
	}
	else if( eTeam == TEAM_RED )
	{
		kPrinter.SetTextColor( GetChatRedIDColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(5));
		kPrinter.SetTextColor( GetChatColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(6) );
	}

	kPrinter.AddTextPiece( GetChatFontScale(), STR(7) );
	
	if ( g_NoviceGradeBenefitMgr.IsBenefitGrade( nGrade ) )
	{
		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( GetChatFontScale(), " 초보자 버프" );
		kPrinter.SetTextColor( GetChatColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), " 상태." );
	}

	AddChatItem( STR(1), kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetMemberBalanceMsg( TeamType eTeam )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );

	if( eTeam == TEAM_BLUE )
	{
		kPrinter.SetTextColor( GetChatBlueIDColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(2));
	}
	else if( eTeam == TEAM_RED )
	{
		kPrinter.SetTextColor( GetChatRedIDColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(3));
	}

	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(4) );

	AddChatItem( STR(1), kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetOutUserChatMsg( const char *szID, TeamType eTeam )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );

	if( eTeam == TEAM_BLUE )
		kPrinter.SetTextColor( GetChatBlueIDColor() );
	else if( eTeam == TEAM_RED )
		kPrinter.SetTextColor( GetChatRedIDColor() );
	else
	{
		if( m_pPlayStage && m_pPlayStage->GetOwnerTeam() == eTeam )
			kPrinter.SetTextColor( GetChatBlueIDColor() );
		else
			kPrinter.SetTextColor( GetChatRedIDColor() );
	}
	kPrinter.AddTextPiece( GetChatFontScale(), szID );

	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(2) );

	if( eTeam == TEAM_BLUE )
	{
		kPrinter.SetTextColor( GetChatBlueIDColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(3));
		kPrinter.SetTextColor( GetChatColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(4) );
	}
	else if( eTeam == TEAM_RED )
	{
		kPrinter.SetTextColor( GetChatRedIDColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(5));
		kPrinter.SetTextColor( GetChatColor() );
		kPrinter.AddTextPiece( GetChatFontScale(), STR(6) );
	}

	kPrinter.AddTextPiece( GetChatFontScale(), STR(7) );
	AddChatItem( STR(1), kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetKickUserChatMsg( const char *szID, TeamType eTeam )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );

	if( eTeam == TEAM_BLUE )
		kPrinter.SetTextColor( GetChatBlueIDColor() );
	else
		kPrinter.SetTextColor( GetChatRedIDColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), szID );

	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(2) );

	AddChatItem( STR(1), kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetBadNetworkKickUserChatMsg( const char *szID, TeamType eTeam )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );

	if( eTeam == TEAM_BLUE )
		kPrinter.SetTextColor( GetChatBlueIDColor() );
	else
		kPrinter.SetTextColor( GetChatRedIDColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), szID );

	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(1) );

	AddChatItem( STR(2), kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetKickHighLevelUserChatMsg( const char *szID, TeamType eTeam )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinterOne;
	kPrinterOne.SetTextStyle( GetTextStyle() );
	kPrinterOne.SetBkColor( GetChatBkColor() );		
	kPrinterOne.SetTextColor( eTeam == TEAM_BLUE ? GetChatBlueIDColor() : GetChatRedIDColor() );	
	kPrinterOne.AddTextPiece( GetChatFontScale(), szID );

	kPrinterOne.SetTextColor( GetChatColor() );
	kPrinterOne.AddTextPiece( GetChatFontScale(), "님, 실력이 너무 뛰어나 유저보호를 위해 전투방에서" );

	ioComplexStringPrinter kPrinterTwo;
	kPrinterTwo.SetTextStyle( GetTextStyle() );
	kPrinterTwo.SetBkColor( GetChatBkColor() );
	kPrinterTwo.SetTextColor( GetChatColor() );
	kPrinterTwo.AddTextPiece( GetChatFontScale(), "이탈되셨습니다." );

	AddChatItem( "정보", kPrinterOne , NULL, true, false );
	AddChatItem( "", kPrinterTwo , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetKickLowLevelUserChatMsg( const char *szID, TeamType eTeam )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );	
	kPrinter.SetTextColor( eTeam == TEAM_BLUE ? GetChatBlueIDColor() : GetChatRedIDColor() );	
	kPrinter.SetTextColor( GetChatRedIDColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), szID );
	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), "님을 보호하기 위해 해당 전투방에서 이탈되셨습니다." );

	AddChatItem( "정보", kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetKickSysError( const char *szID, TeamType eTeam )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );	
	kPrinter.SetTextColor( eTeam == TEAM_BLUE ? GetChatBlueIDColor() : GetChatRedIDColor() );	
	kPrinter.SetTextColor( GetChatRedIDColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), szID );
	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), "내부 시스템에러로 모드가 종료 처리 되었습니다." );

	AddChatItem( "정보", kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetHostChange( const char *szTitle, const char *szHostID, TeamType eTeam )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_INFO_CHAT );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetBkColor( GetChatBkColor() );

	if( eTeam == TEAM_BLUE )
		kPrinter.SetTextColor( GetChatBlueIDColor() );
	else if( eTeam == TEAM_RED )
		kPrinter.SetTextColor( GetChatRedIDColor() );
	else
		kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), szHostID );

	kPrinter.SetTextColor( GetChatColor() );
	kPrinter.AddTextPiece( GetChatFontScale(), STR(1) );

	AddChatItem( szTitle, kPrinter , NULL, true, false );

	g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::PresetChatComplexString( MsgStyleType eStyle, ioComplexStringPrinter &rkChatPrinter )
{
	SetMsgStyle( eStyle );
	rkChatPrinter.SetTextStyle( GetTextStyle() );
	rkChatPrinter.SetTextColor( GetChatColor() );	
	rkChatPrinter.SetBkColor( GetChatBkColor() );
}

void ioSP2ChatManager::SetChatComplexString( const ioHashString &szHead, ioComplexStringPrinter &rkChatPrinter, bool bPlaySound /* = true  */ )
{
	if( Setting::ChatInfoOff() )
		return;

	ioComplexStringPrinter kPrinterA, kPrinterB;
	rkChatPrinter.SplitFullText( kPrinterA, kPrinterB, m_fSplitWidth );

	if( !kPrinterA.IsEmpty() )
		AddChatItem( szHead.c_str(), kPrinterA , NULL, true, !kPrinterB.IsEmpty() );
	if( !kPrinterB.IsEmpty() )
		AddChatItem( "", kPrinterB , NULL, false, true );

	if( bPlaySound )
		g_SoundMgr.PlaySound( m_SystemMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SetEventMsg( const char *szSource, DWORD dwColor /*= 0xFFE5E5E5 */ )
{
	if( Setting::ChatInfoOff() )
		return;

	SetMsgStyle( MSG_ANNOUNCE_CHAT );

	SafeSprintf( m_szMsgBuf, MSG_BUFF_SIZE, szSource );

	ioComplexStringPrinter kPrinter;
	kPrinter.SetTextStyle( GetTextStyle() );
	kPrinter.SetTextColor( GetChatColor() );	
	kPrinter.SetBkColor( GetChatBkColor() );
	AddChatSplitItem( STR(1), kPrinter, NULL, m_szMsgBuf );

	g_SoundMgr.PlaySound( m_EventMsgSound );
	m_CurChatSound.Clear();
}

void ioSP2ChatManager::SendChatLog( ioMannerTrialChatManager::TrialType eTrialType, const char *szID, const char *szChat, ModeType eModeType /*= MT_NONE */, const int iRoomIndex /*= -1*/ )
{
	//외치기 를 제외한 채팅 로그는 남기지 않도록 수정
	if( eTrialType != ioMannerTrialChatManager::TT_WHOLE_SERVER_RAINBOW_CHAT && eTrialType != ioMannerTrialChatManager::TT_WHOLE_SERVER_CHAT )
		return;

	char szModeTypeName[MAX_PATH]="";
	switch( eModeType )
	{
	case MT_TOWER_DEFENSE:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), "깊은 숲" ); //UNDONE : don't know
		break;
	case MT_DARK_XMAS:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), "스노우타운" ); //UNDONE : don't know
		break;
	case MT_FIRE_TEMPLE:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), "불의 신전" ); //UNDONE : don't know
		break;
	case MT_SYMBOL:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(1) );
		break;
	case MT_CATCH:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(2) );
		break;
	case MT_CATCH_RUNNINGMAN:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(16) );
		break;
	case MT_KING:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(3) );
		break;
	case MT_TRAINING:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(4) );
		break;
	case MT_MYROOM:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(5) );
		break;
	case MT_SURVIVAL:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(6) );
		break;
	case MT_FIGHT_CLUB:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(17) );
		break;
	case MT_TEAM_SURVIVAL:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(7) );
		break;
	case MT_BOSS:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(8) );
		break;
	case MT_MONSTER_SURVIVAL:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(9) );
		break;
	case MT_FOOTBALL:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(10) );
		break;
	case MT_HEROMATCH:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(11) );
		break;
	case MT_GANGSI:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(12) );
		break;
	case MT_DUNGEON_A:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(14) );
		break;
	case MT_HEADQUARTERS:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), "훈련소" );
		break;
	case MT_DOUBLE_CROWN:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), "더블 크라운" );
		break;
	case MT_SHUFFLE_BONUS:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), "오늘의모드" );
		break;
	case MT_FACTORY:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), "비밀공장" );
		break;
	case MT_TEAM_SURVIVAL_AI:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), "팀서바이벌AI" );
		break;
	case MT_HOUSE:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), "개인본부" );
		break;
	case MT_RAID:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), "레이드" );
		break;
	case MT_UNDERWEAR:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), "팬티몬" );
		break;
	case MT_CBT:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), "클베모드" );
		break;
	case MT_FLAG_CAPTURE:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(18) );
		break;
	case MT_ARENA:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(19) );
		break;
	case MT_BATTLE:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(20) );
		break;
	case MT_FARMING:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), STR(21) );
		break;
	default:
		SafeSprintf( szModeTypeName, sizeof( szModeTypeName ), "NONE" );
		break;
	}		

	char szLog[MAX_PATH]="";
	StringCbPrintf( szLog, sizeof( szLog ), "[%s]<%d>(%s)[%s] %s", g_MannerTrialChatMgr.GetTrialTypeName( eTrialType ), iRoomIndex+1, szModeTypeName, szID, szChat );

	SP2Packet kPacket( LUPK_LOG );
	kPacket << "ChatLog";  // 로그 파일 타입
	kPacket << szLog;
	kPacket << 0;     // 오류번호 없음
	kPacket << false; // write db
	P2PNetwork::SendLog(kPacket);
}

ioSP2ChatManager& ioSP2ChatManager::GetSingleton()
{
	return Singleton< ioSP2ChatManager >::GetSingleton();
}

void ioSP2ChatManager::SetPlayStage( ioPlayStage *pPlayStage )
{
	m_pPlayStage = pPlayStage;
}

void ioSP2ChatManager::AddSentenceIndex( bool bFirsLine )
{
	if( bFirsLine )
		m_dwSentenceIndexer++;
}

void ioSP2ChatManager::AddChatSplitItem( const char *a_head, ioComplexStringPrinter &a_rkPrinter, const char *a_szID ,const char *a_szChat , ChatType a_eChatType /*= CT_NONE */, bool a_bCustomSound /*= false */ )
{
	char szSplitText[MAX_SPLIT_LINE][MAX_PATH];
	ZeroMemory( szSplitText, sizeof( szSplitText ) );
	SplitText( a_szChat, m_fSplitWidth - a_rkPrinter.GetFullWidth(), szSplitText );
	
	for (int i = 0; i < MAX_SPLIT_LINE ; i++)
	{
		if( i == 0)
		{
			a_rkPrinter.SetTextStyle( GetTextStyle() );
			a_rkPrinter.SetBkColor( GetChatBkColor() );
			a_rkPrinter.SetTextColor( GetChatColor() );
			a_rkPrinter.AddTextPiece( GetChatFontScale(), szSplitText[i] );
			AddChatItem( a_head, a_rkPrinter, a_szID , true, (strcmp( szSplitText[1], "") != 0), a_eChatType, a_bCustomSound );
		}
		else 
		{
			if( strcmp( szSplitText[i], "") == 0 )
				break;

			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( GetTextStyle() );
			kPrinter.SetBkColor( GetChatBkColor() );
			kPrinter.SetTextColor( GetChatColor() );
			kPrinter.AddTextPiece( GetChatFontScale(), szSplitText[i]);
			AddChatItem( "", kPrinter, a_szID , false, true, a_eChatType, a_bCustomSound );
		}
	}
}

void ioSP2ChatManager::SetMsgWndFeature( DWORD dwFeature )
{
	m_dwMsgSortFeature = dwFeature;

	m_ChatListByFeature.clear();

	if( m_dwMsgSortFeature == 0 )
	{
		if( !m_ChatList.empty() )
			AllAlphaTimeInit();

		return;
	}

	ChatMsgList::iterator iter;
	for( iter=m_ChatList.begin(); iter!=m_ChatList.end(); ++iter )
	{
		IMsgItem *pItem = (*iter);
		if( !pItem ) continue;

		AddChatItemByFeature( pItem );
	}

	if( !m_ChatListByFeature.empty() )
		AllAlphaTimeInit();
}

bool ioSP2ChatManager::CheckEnableAddByFeature( IMsgItem *pItem )
{
	if( !pItem ) return false;

	if( m_dwMsgSortFeature == 0 )
	{
		return true;
	}
	else
	{
		ChatType eType = pItem->GetChatType();
		bool bAdd = false;

		switch( eType )
		{
		case CT_NONE:
		case CT_ANNOUNCE:
			if( m_dwMsgSortFeature & MSF_SYSTEM_CHAT )
				bAdd = true;
			break;
		case CT_ALL:
		case CT_TEAM:
		case CT_PRIVATE:
		case CT_PARTY:
		case CT_LADDER:
		case CT_SERVER_LOBBY:
			if( m_dwMsgSortFeature & MSF_NORMAL_CHAT )
				bAdd = true;
			break;
		case CT_WHOLE_SERVER:
		case CT_WHOLE_SERVER_RAINBOW:
			if( m_dwMsgSortFeature & MSF_WHOLE_CHAT )
				bAdd = true;
			break;
		}

		return bAdd;
	}

	return false;
}

void ioSP2ChatManager::AddChatItemByFeature( IMsgItem *pItem )
{
	if( !pItem ) return;

	if( m_dwMsgSortFeature == 0 )
	{
		return;
	}
	else
	{
		bool bAdd = CheckEnableAddByFeature( pItem );

		if( bAdd )
		{
			m_ChatListByFeature.push_back( pItem );
		}
	}
}

int ioSP2ChatManager::GetMaxChatMsgByFeature()
{
	if( m_dwMsgSortFeature == 0 )
	{
		return GetMaxChatMsg();
	}

	return m_ChatListByFeature.size();
}

IMsgItem* ioSP2ChatManager::GetReverseChatMsgByFeature( int list_count )
{
	if( m_dwMsgSortFeature == 0 )
	{
		return GetReverseChatMsg( list_count );
	}

	int iReverse = ( GetMaxChatMsgByFeature() - list_count - 1 );
	if( COMPARE( iReverse, 0, GetMaxChatMsgByFeature() ) )
		return m_ChatListByFeature[iReverse];

	return NULL;
}

IMsgItem* ioSP2ChatManager::GetChatMsgByFeature( int list_count )
{
	if( m_dwMsgSortFeature == 0 )
	{
		return GetChatMsg( list_count );
	}

	if( COMPARE( list_count, 0, GetMaxChatMsgByFeature() ) )
		return m_ChatListByFeature[list_count];

	return NULL;
}

void ioSP2ChatManager::AddHelpSplitItem( const char *a_head, ioComplexStringPrinter &a_rkPrinter, const char *a_szID ,const char *a_szChat , ChatType a_eChatType /*= CT_NONE */, bool a_bCustomSound /*= false */ )
{
	char szSplitText[MAX_SPLIT_LINE][MAX_PATH];
	ZeroMemory( szSplitText, sizeof( szSplitText ) );
	SplitText( a_szChat, 155.0f - a_rkPrinter.GetFullWidth(), szSplitText );

	for (int i = 0; i < MAX_SPLIT_LINE ; i++)
	{
		if( i == 0)
		{
			a_rkPrinter.SetTextStyle( GetTextStyle() );
			a_rkPrinter.SetBkColor( GetChatBkColor() );
			a_rkPrinter.SetTextColor( GetChatColor() );
			a_rkPrinter.AddTextPiece( GetChatFontScale(), szSplitText[i] );
			AddHelpItem( a_head, a_rkPrinter, a_szID , true, (strcmp( szSplitText[1], "") != 0), a_eChatType, a_bCustomSound );
		}
		else 
		{
			if( strcmp( szSplitText[i], "") == 0 )
				break;

			ioComplexStringPrinter kPrinter;
			kPrinter.SetTextStyle( GetTextStyle() );
			kPrinter.SetBkColor( GetChatBkColor() );
			kPrinter.SetTextColor( GetChatColor() );
			kPrinter.AddTextPiece( GetChatFontScale(), szSplitText[i]);
			AddHelpItem( "", kPrinter, a_szID , false, true, a_eChatType, a_bCustomSound );
		}
	}
}




