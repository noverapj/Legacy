#include "stdafx.h"

#include "ioUserPcRoom.h"

#include "User.h"
#include "ioExerciseCharIndexManager.h"

#include "../MainProcess.h"

ioUserPcRoom::ioUserPcRoom()
{
}

ioUserPcRoom::~ioUserPcRoom()
{
	SlotClear();
}

void ioUserPcRoom::Initialize()
{
	m_PCRoomCharVec.clear();

	ioINILoader kLoader( "ls_config_game.ini" );
	kLoader.SetTitle( "AddExerciseCharIndex" );
		
	m_iExcercisePCRoomCharMax = kLoader.LoadInt( "pc_room_char_max", 2 );
	if( 0 < m_iExcercisePCRoomCharMax )
		m_PCRoomCharVec.reserve( m_iExcercisePCRoomCharMax );
}

void ioUserPcRoom::SlotClear()
{
	m_PCRoomCharVec.clear();
	m_NewPCRoomCharVec.clear();
}

bool ioUserPcRoom::IsNewPcRoomChar( int iCharIndex )
{
	if( m_NewPCRoomCharVec.empty() )
		return false;

	LOOP_GUARD();
	IntVec::iterator iter = m_NewPCRoomCharVec.begin();
	for( ; iter != m_NewPCRoomCharVec.end(); ++iter )
	{
		if( iCharIndex == *iter )
			return true;
	}
	LOOP_GUARD_CLEAR();

	return false;
}

bool ioUserPcRoom::HasCalss( int iClassType )
{
	if( m_PCRoomCharVec.empty() )
		return false;

	LOOP_GUARD();
	PCRoomCharVec::const_iterator iter = m_PCRoomCharVec.begin();
	for( ; iter != m_PCRoomCharVec.end(); ++iter )
	{
		ioCharacter* pChar = *iter;
		if( !pChar )
			continue;

		if( pChar->GetCharInfo().m_class_type == iClassType )
			return true;
	}
	LOOP_GUARD_CLEAR();

	return false;
}

void ioUserPcRoom::InsertPcRoomChar( ioCharacter* pChar )
{
	m_PCRoomCharVec.push_back( pChar );
}

int ioUserPcRoom::GetUseSlotCount()
{
	return (int)m_PCRoomCharVec.size();
}

int ioUserPcRoom::GetPcRoomCharMax()
{
	return m_iExcercisePCRoomCharMax;
}

int ioUserPcRoom::GetDeleteCount( const PcRoomCharInfoVec& InfoVec )
{
	int iDelCount = m_PCRoomCharVec.size();
	LOOP_GUARD();
	for( PCRoomCharVec::const_iterator iter = m_PCRoomCharVec.begin(); iter != m_PCRoomCharVec.end(); ++iter )
	{
		if( *iter == NULL )
		{
			iDelCount--;
		}
	}
	LOOP_GUARD_CLEAR();
	
	LOOP_GUARD();
	for( PcRoomCharInfoVec::const_iterator iter = InfoVec.begin(); iter != InfoVec.end(); ++iter )
	{
		const PcRoomCharInfo& rkInfo = *iter;
		if( HasCalss(rkInfo.m_iClassType ) )
			iDelCount--;
	}
	LOOP_GUARD_CLEAR();

	return max( 0, iDelCount );
}

int ioUserPcRoom::GetHasCount( const PcRoomCharInfoVec& InfoVec )
{
	int iHasCount = 0;

	LOOP_GUARD();
	for( PcRoomCharInfoVec::const_iterator iter = InfoVec.begin(); iter != InfoVec.end(); ++iter )
	{
		const PcRoomCharInfo& rkInfo = *iter;
		if( HasCalss (rkInfo.m_iClassType ) )
			iHasCount++;
	}
	LOOP_GUARD_CLEAR();

	return iHasCount;
}

const ioCharacter* ioUserPcRoom::GetPcRoomChar( UINT iSlotIndex )
{	
	if( iSlotIndex < m_PCRoomCharVec.size() )
	{
		return m_PCRoomCharVec[iSlotIndex];
	}

	return NULL;
}

const ioUserPcRoom::PCRoomCharVec& ioUserPcRoom::GetRoomCharVec()
{
	return m_PCRoomCharVec;
}

void ioUserPcRoom::AllocPCRoomChar( User* pUser, const PcRoomCharInfoVec& InfoVec )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog(0, "%s Failed - pUser == NULL ", __FUNCTION__ );
		return;
	}

	//PC방캐릭터 슬롯을 비운다
	SlotClear();

	LOOP_GUARD();
	for( PcRoomCharInfoVec::const_iterator iter = InfoVec.begin(); iter != InfoVec.end(); ++iter )
	{
		const PcRoomCharInfo& rkInfo = *iter;
		ioCharacter* pPcRoomChar = NULL;

		//0일때는 PC방 용병을 생성하지 않는것으로 간주하고 비워둠
		if( rkInfo.m_iClassType != 0 )
		{
			//기존에 만들어진 PC방 캐릭터라면 PC방 슬롯에 재할당
			pPcRoomChar = pUser->GetPCRoomChar( rkInfo.m_iClassType );

			//없는 캐릭터는 새로 생성
			if( pPcRoomChar == NULL )
				pPcRoomChar = pUser->CreatePCRoomChar( rkInfo.m_iClassType );
		}

		InsertPcRoomChar( pPcRoomChar );
	}
	LOOP_GUARD_CLEAR();
}

void ioUserPcRoom::DeletePCRoomCharBySlot( User* pUser )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog(0, "%s Failed - pUser == NULL ", __FUNCTION__ );
		return;
	}

	LOOP_GUARD();
	DWORDVec ClassVec;
	for( int i = 0; i < pUser->GetCharCount(); ++i )
	{
		ioCharacter* pChar = pUser->GetCharacter( i );
		if( pChar && pChar->HasExerciseStyle( EXERCISE_PCROOM ) )
		{
			if( HasCalss( pChar->GetCharInfo().m_class_type ) )
			{				
				continue;
			}			
			ClassVec.push_back( pChar->GetCharIndex() );
		}
	}
	LOOP_GUARD_CLEAR();

	LOOP_GUARD();
	for( DWORDVec::iterator iter = ClassVec.begin(); iter != ClassVec.end(); ++iter )
	{
		DWORD dwCharIdx = *iter;
		pUser->DeleteExercisePCRoomChar( dwCharIdx );
	}
	LOOP_GUARD_CLEAR();
}

void ioUserPcRoom::CheckNewPcRoomCharSlot( User* pUser )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog(0, "%s Failed - pUser == NULL ", __FUNCTION__ );
		return;
	}

	LOOP_GUARD();
	for( PCRoomCharVec::iterator iter = m_PCRoomCharVec.begin(); iter != m_PCRoomCharVec.end(); ++iter )
	{
		ioCharacter* pCharacter = *iter;
		if( pCharacter && pCharacter->GetCharSlotIndex() == -1 )
		{
			pUser->CheckCharSlot( pCharacter );
			m_NewPCRoomCharVec.push_back( pCharacter->GetCharIndex() );
		}
	}
	LOOP_GUARD_CLEAR();
}

void ioUserPcRoom::FillMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID( rkPacket.Write( (int)m_PCRoomCharVec.size() ) );

	LOOP_GUARD();
	for( PCRoomCharVec::iterator iter = m_PCRoomCharVec.begin(); iter != m_PCRoomCharVec.end(); ++iter )
	{
		ioCharacter* pCharacter = *iter;
		if( pCharacter )
		{
			PACKET_GUARD_VOID( rkPacket.Write( pCharacter->GetCharInfo().m_class_type ) );
		}
		else
		{
			PACKET_GUARD_VOID( rkPacket.Write( 0 ) );
		}
	}
	LOOP_GUARD_CLEAR();
}

void ioUserPcRoom::ApplyMoveData( SP2Packet &rkPacket, User* pUser, bool bDummyNode )
{
	if( !pUser )
	{
		LOG.PrintTimeAndLog(0, "%s Failed - pUser == NULL ", __FUNCTION__ );
		return;
	}

	m_PCRoomCharVec.clear();

	int iCount = 0;
	PACKET_GUARD_VOID( rkPacket.Read( iCount ) );

	LOOP_GUARD();
	for( int i = 0; i < iCount; ++i )
	{
		int iCharClass = 0;
		PACKET_GUARD_VOID( rkPacket.Read( iCharClass ) );
		if( iCharClass == 0 )
		{
			InsertPcRoomChar( NULL );
		}
		else
		{
			int iArray = pUser->GetCharArrayByClass( iCharClass );
			if( COMPARE( iArray, 0, pUser->GetCharCount() ) )
			{
				ioCharacter* pChar = pUser->GetCharacter( iArray );
				if( pChar )
				{
					InsertPcRoomChar( pChar );
				}
				else
				{
					LOG.PrintTimeAndLog(0, "%s - %s worng pChar( iArray : %d )", __FUNCTION__, pUser->GetPublicID().c_str(), iArray );
				}
			}
			else
			{
				LOG.PrintTimeAndLog(0, "%s - %s worng iCharClass value( iArray : %d )", __FUNCTION__, pUser->GetPublicID().c_str(), iCharClass );
			}
		}
	}
	LOOP_GUARD_CLEAR();
}