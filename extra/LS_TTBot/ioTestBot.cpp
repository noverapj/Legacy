#include "StdAfx.h"
#include "Protocol.h"
#include "ioTestBot.h"
#include "common.h"
#include "LS_TTBotDlg.h"

static int INDEX = 0;
ioTestBot::ioTestBot(void) : m_index(++INDEX)
{
	Init();
}

ioTestBot::~ioTestBot(void)
{
	Destroy();
}

void ioTestBot::Init()
{
}

void ioTestBot::Destroy()
{
	int loopCount = 0;
	 
	while(m_socketTCP.GetReceiveThreadID() )
	{
		Sleep(1);
		 
		++loopCount;
		if(loopCount > 1000)
			break;
	}
}

void ioTestBot::PacketProcess(SP2Packet& packet)
{
	switch( packet.GetPacketID() )
	{
	case STPK_ABSTRACT:
		{
			int i = 0;
		}
		break;

	case DTPK_QUERY :
		{
			CQueryResultData query_data;
			packet >> query_data;
			break;

			switch( query_data.GetMsgType() )
			{
			case DBAGENT_TOTAL_REG_USER_SET:
				{
					bool bServerDown;
					query_data.GetValue( bServerDown, sizeof(bool) );

					int iTotalRegUser;
					query_data.GetValue( iTotalRegUser, sizeof(int) );

					if( bServerDown == true || iTotalRegUser < 0 || iTotalRegUser > 10000 )
					{
						MessageBox( NULL, "query(simple) packet data break", "packet data break", MB_OK );
					}
				}
				break;

			case DBAGENT_GUILD_INFO_GET:
				{
					while( query_data.IsExist() )
					{
						DWORD dwGuildIndex = 0;
						query_data.GetValue( dwGuildIndex, sizeof(DWORD) );	 //길드 인덱스

						if( dwGuildIndex > 50000 )
						{
							MessageBox( NULL, "query(guild) packet data break", "packet data break", MB_OK );
						}

						ioHashString m_szGuildName;      //길드 이름.
						ioHashString m_szGuildTitle;     //길드 소개.

						char szGuildName[GUILD_NAME_NUM_PLUS_ONE] = "";
						query_data.GetValue( szGuildName, GUILD_NAME_NUM_PLUS_ONE );		 //길드 이름
						m_szGuildName = szGuildName;

						char szGuildTitle[GUILD_TITLE_NUMBER_PLUS_ONE] = "";
						query_data.GetValue( szGuildTitle, GUILD_TITLE_NUMBER_PLUS_ONE );	 //길드 소개
						m_szGuildTitle = szGuildTitle;

						DWORD m_dwGuildMark;
						DWORD m_dwGuildPoint;
						DWORD m_dwGuildMaxEntry;

						query_data.GetValue( m_dwGuildMark, sizeof(DWORD) );    //길드 마크
						query_data.GetValue( m_dwGuildPoint, sizeof(DWORD) );   //길드 포인트
						query_data.GetValue( m_dwGuildMaxEntry, sizeof(DWORD) );//길드 인원 제한

						DWORD m_dwGuildRegDate;

						DBTIMESTAMP dts;
						query_data.GetValue( (char*)&dts, sizeof(DBTIMESTAMP) );			 //길드 생성일
						m_dwGuildRegDate = (dts.year * 10000) + (dts.month * 100) + dts.day;

						DWORD m_dwGuildJoinUser;
						DWORD m_dwCurGuildPoint;
						DWORD m_dwGuildLevel;
						DWORD m_dwGuildCampPos;

						query_data.GetValue( m_dwGuildJoinUser, sizeof(DWORD) );//길드 인원
						query_data.GetValue( m_dwCurGuildPoint, sizeof(DWORD) );   //금일 획득 포인트
						query_data.GetValue( m_dwGuildLevel, sizeof(DWORD) );   //길드 레벨
						query_data.GetValue( m_dwGuildCampPos, sizeof(DWORD) );   //길드 진영

						// 전적 정보
						int m_iWin;
						int m_iLose;
						int m_iKill;
						int m_iDeath;
						query_data.GetValue( m_iWin, sizeof( int ) );
						query_data.GetValue( m_iLose, sizeof( int ) );
						query_data.GetValue( m_iKill, sizeof( int ) );
						query_data.GetValue( m_iDeath, sizeof( int ) );
					}
				}
			}
		}
		break;
	case EPROTOCOL::LSPTK_TIMEOUT_CLOSE_REQUEST:
		{

		}
		break;
	case EPROTOCOL::LSTPK_SVR_STATUS_RESPONSE:
		{
// 			TCHAR* ipaddr = new TCHAR[64];
// 			int port;
// 			packet >> ipaddr;
// 			packet >> port;
// 			g_Log->PostMessageA(WM_LOG,(WPARAM)ipaddr,0);
		 
// 			ioTestCase* child = (ioTestCase*)this;
// 			child->Current(TESTSTATE_DISCONNECT);
			

		}
		break;
	case STPK_CONNECT:
		{
			int result;
			packet >> result;

			switch(result)
			{
			case CONNECT_OK :
				{
					DWORD userIndex;
					ioHashString publicId;
					packet >> userIndex >> publicId;

					//UDP HOLE PUNCHING.
					SP2Packet output( CUPK_CONNECT );
					output << publicId;

					SendUDP( output );
				}
				break;
			case CONNECT_ID_NOT :
				break;
			case CONNECT_PW_NOT :
				TRACE("CONNECT_PW_NOT");
				break;
			case CONNECT_ID_ALREADY :
				break;
			case CONNECT_EXCEPT :
				break;
			case CONNECT_EXPIRE_KEY :
				break;
			case CONNECT_EXITING_SERVER :
				break;
			case CONNECT_WRONG_CLIENT_VER :
				break;
			case CONNECT_WRONG_LOCAL_VER :
				break;
			case CONNECT_GAMESERVER_FULL :
				break;
			default :
				TRACE( "STPK_CONNECT : %lu", packet.GetPacketID() );
				break;
			}
		}
		break;

	default:
		TRACE( "unknown packet : %lu", packet.GetPacketID() );
		return;
	}
}


void PacketProcess(ioTestBot* unit, SP2Packet& packet)
{
	unit->PacketProcess(packet);
}
