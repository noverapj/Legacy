#include "stdafx.h"
#include "ioUserAccessory.h"
#include "User.h"
#include "AccessoryManager.h"
#include "../ioCriticalError.h"
#include "../MainProcess.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "ioCharacter.h"
#include "../EtcHelpFunc.h"

ioUserAccessory::ioUserAccessory()
{
}

ioUserAccessory::~ioUserAccessory()
{
	Destroy();
}

void ioUserAccessory::Init(User* pUser)
{
	m_mUserAccessoryMap.clear();
	m_pUser = pUser;
}

void ioUserAccessory::Destroy()
{
	m_mUserAccessoryMap.clear();
	m_pUser = NULL;
}

void ioUserAccessory::DBtoData( CQueryResultData *query_data, int& iLastIndex )
{
	if( !m_pUser )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioUserAccessory::DBtoData() User NULL!!"); 
		return;
	}

	LOOP_GUARD();
	while( query_data->IsExist() )
	{	
		Accessory cAccessory;

		DBTIMESTAMP dts;
		int iIndex			= 0;
		DWORD dwAccessoryCode = 0;
		BYTE byPeriodType	= 0;
		int iClassType		= 0;	
		int iAccessoryValue = 0;
		bool bChange		= false;
		int iComposeCode = 0;
		int iComposeValue = 0;

		PACKET_GUARD_BREAK( query_data->GetValue( iIndex, sizeof(int) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( dwAccessoryCode, sizeof(int) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( byPeriodType, sizeof(BYTE) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( (char*)&dts, sizeof(DBTIMESTAMP) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( iAccessoryValue, sizeof(int) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( iClassType, sizeof(int) ) );				//장착중인 클래스.
		PACKET_GUARD_BREAK( query_data->GetValue( iComposeCode, sizeof(int) ) );
		PACKET_GUARD_BREAK( query_data->GetValue( iComposeValue, sizeof(int) ) );
		
		if( 0 != iClassType && !m_pUser->IsCharClassType( iClassType ) )
		{
			iClassType	= 0;
			bChange = true;
		}
		else
		{
			//해당 클래스에 장착
			SetEquipInfo(iClassType, dwAccessoryCode, iIndex, iAccessoryValue, iComposeCode, iComposeValue);
		}

		CTime kLimitTime(Help::GetSafeValueForCTimeConstructor(dts.year,dts.month,dts.day, dts.hour,dts.minute,dts.second));
		
		cAccessory.SetAccessoryCode(dwAccessoryCode);
		cAccessory.SetAccessoryIndex(iIndex);
		cAccessory.SetPeriodType(byPeriodType);
		cAccessory.SetWearingClass(iClassType);
		cAccessory.SetDate(kLimitTime.GetYear(), kLimitTime.GetMonth(), kLimitTime.GetDay(), kLimitTime.GetHour(), kLimitTime.GetMinute());
		cAccessory.SetAccessoryValue(iAccessoryValue);
		cAccessory.SetComposeCode(iComposeCode);
		cAccessory.SetComposeValue(iComposeValue);

		m_mUserAccessoryMap.insert(make_pair(iIndex, cAccessory));
		//LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] accessory dbtodata : [user:%d index:%d code:%d value:%d]", m_pUser->GetUserIndex(), iIndex, dwAccessoryCode, iAccessoryValue);

		//들어온 장비에대한 마지막 index를 가지고 있어야되.
		iLastIndex = iIndex;

		if( bChange )
		{
			SYSTEMTIME sys = {0,0,0,0,0,0,0,0};
			g_AccessoryMgr.ConvertCTimeToSystemTime(sys, kLimitTime);
			g_DBClient.OnUpdateAccessoryData(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), iIndex, dwAccessoryCode, byPeriodType, sys, iClassType, iAccessoryValue, iComposeCode, iComposeValue);
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] equip accessory update : [user:%d index:%d code:%d value:%d]", m_pUser->GetUserIndex(), iIndex, dwAccessoryCode, iAccessoryValue);
		}
	}
	LOOP_GUARD_CLEAR();
}

void ioUserAccessory::FillMoveData( SP2Packet &rkPacket )
{
	PACKET_GUARD_VOID(rkPacket.Write((int)m_mUserAccessoryMap.size()));
	UserAccessoryItem::iterator it = m_mUserAccessoryMap.begin();

	for( ; it != m_mUserAccessoryMap.end() ; it++ )
	{
		Accessory cAccessory = it->second;
		cAccessory.FillMoveData(rkPacket);
	}
}

void ioUserAccessory::ApplyMoveData( SP2Packet &rkPacket, bool bDummyNode )
{
	int iSize = 0;
	PACKET_GUARD_VOID( rkPacket.Read(iSize) );

	for( int i = 0; i < iSize; i++ )
	{
		Accessory cAccessory;
		cAccessory.ApplyMoveData(rkPacket);

		m_mUserAccessoryMap.insert(make_pair(cAccessory.GetAccessoryIndex(), cAccessory));
	}
}

bool ioUserAccessory::IsEnableAdd()
{
	int iMaxCount = g_CriticalError.GetMaxAccessoryCount();

	if( iMaxCount <= (int)m_mUserAccessoryMap.size() )
		return false;

	return true;
}

void ioUserAccessory::AddAccessoryItem(Accessory& AccessoryInfo)
{
	m_mUserAccessoryMap.insert( make_pair(AccessoryInfo.GetAccessoryIndex(), AccessoryInfo) );
}

bool ioUserAccessory::DeleteAccessoryItem(DWORD& dwIndex)
{
	if( !m_pUser )
		return false;

	UserAccessoryItem::iterator it = m_mUserAccessoryMap.find(dwIndex);

	if( it == m_mUserAccessoryMap.end() )
		return false;	//없는 아이템

	m_mUserAccessoryMap.erase(it);
	
	//DB삭제
	g_DBClient.OnDeleteAccessoryData(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), dwIndex);
	return true;	//성공
}

bool ioUserAccessory::IsEmpty()
{
	if( m_mUserAccessoryMap.size() != 0 )
		return false;

	return true;
}

Accessory* ioUserAccessory::GetAccessory(DWORD dwIndex)
{
	UserAccessoryItem::iterator it = m_mUserAccessoryMap.find(dwIndex);
	if( it == m_mUserAccessoryMap.end() )
		return NULL;

	return &(it->second);
}

//bool ioUserAccessory::ChangeAccessory(ioCharacter* pCharacter, DWORD dwClassIndex, int iEquipPos, DWORD dwTargetIndex, DWORD dwAccessoryCode, bool bEquip)
//{
//	if( bEquip )
//	{
//		//캐릭터에 장착
//		pCharacter->ChangeAccessoryItem(iEquipPos, dwTargetIndex, dwAccessoryCode);
//	}
//	else
//	{
//		//캐릭터에 장착 해제
//		pCharacter->ChangeAccessoryItem(iEquipPos, 0, 0);
//	}
//
//	return true;
//}

int ioUserAccessory::ChangeEquipInfo(int iClassArray, DWORD& dwTargetIndex, int iEquipPos, bool bEquip, Accessory* pAccessory)
{
	if( !m_pUser )
		return ACCESSORY_CHANGE_EXECPTION;

	if( iEquipPos < 0 || iEquipPos >= MAX_CHAR_ACCESSORY_SLOT )
		return ACCESSORY_CHANGE_DISABLE_POS;

	if( !pAccessory )
		return ACCESSORY_CHANGE_NO_TARGET;

	if( (pAccessory->GetAccessoryCode()-ACCESSORY_SUBTYPE_DELIMITER)/ACCESSORY_SUBTYPE_DELIMITER != iEquipPos )
		return ACCESSORY_CHANGE_DISABLE_POS;

	if( bEquip && g_AccessoryMgr.IsEquipHero( pAccessory->GetAccessoryCode(), iClassArray ) )
		return ACCESSORY_CHANGE_EXECPTION;

	if( pAccessory->GetPeriodType() == PCPT_TIME )
	{
		CTime kCurTime = CTime::GetCurrentTime();
		CTime kLimitTime( Help::GetSafeValueForCTimeConstructor( pAccessory->GetYear(),
																	 pAccessory->GetMonth(),
																	 pAccessory->GetDay(),
																	 pAccessory->GetHour(),
																	 pAccessory->GetMinute(),
																	 0) );
		CTimeSpan kRemainTime = kLimitTime - kCurTime;
		if( kRemainTime.GetTotalMinutes() <= 0 && bEquip )
			return ACCESSORY_CHANGE_PERIOD;
	}

	ioCharacter* pCharacter = m_pUser->GetCharacter(iClassArray);
	if( !pCharacter )
		return ACCESSORY_CHANGE_NO_CHARACTER;

	if( pAccessory->GetWearingClass() != 0 )
	{
		int iArray = m_pUser->GetCharArray(pAccessory->GetWearingClass());
		ioCharacter* pChar =  m_pUser->GetCharacter(iArray);
		if( pChar )
		{
			int iArray = pChar->GetAccessorySlot(dwTargetIndex);
			if( iArray >= 0 )
				return ACCESSORY_CHANGE_EQUIP_ITEM;
			pAccessory->SetWearingClass(0);
		}
		else
			pAccessory->SetWearingClass(0);
	}

	int iPrevAccessoryIndex = pCharacter->GetAccessoryIndex(iEquipPos);

	if( bEquip )
	{
		Accessory* pPrevAccessory = GetAccessory(iPrevAccessoryIndex);

		if( pPrevAccessory )
		{
			ReleaseAccessory(pPrevAccessory);
		}

		pCharacter->ChangeAccessoryItem(iEquipPos, dwTargetIndex, pAccessory->GetAccessoryCode(), pAccessory->GetAccessoryValue(), pAccessory->GetComposeCode(), pAccessory->GetComposeValue() );
		EquipAccessory(pCharacter->GetClassType(), pAccessory);
	}
	else
	{
		//캐릭터에 장착 해제
		pCharacter->ChangeAccessoryItem(iEquipPos, 0, 0, 0, 0, 0);
		ReleaseAccessory(pAccessory);
	}

	return ACCESSORY_CHANGE_SUCCESS;
}

void ioUserAccessory::ReleaseAccessoryWithAccessoryIndex(DWORD dwIndex)
{
	UserAccessoryItem::iterator it = m_mUserAccessoryMap.find(dwIndex);
	if( it == m_mUserAccessoryMap.end() )
		return;

	Accessory* pAccessory = &(it->second);
	if( !pAccessory )
		return;

	ReleaseAccessory(pAccessory);
}

void ioUserAccessory::ReleaseAccessoryWithClassType(int iClassType)
{
	UserAccessoryItem::iterator it = m_mUserAccessoryMap.begin();
	if( it == m_mUserAccessoryMap.end() )
		return;

	for( ; it != m_mUserAccessoryMap.end(); it++ )
	{
		Accessory* pAccessory = &(it->second);
		if( !pAccessory )
			continue;

		if( pAccessory->GetWearingClass() == iClassType )
		{
			ReleaseAccessory(pAccessory);
		}
	}
}
void ioUserAccessory::EquipAccessory(const int iClassType, Accessory* pAccessory)
{
	if( !m_pUser )
		return;

	if( pAccessory )
	{
		pAccessory->SetWearingClass(iClassType);
		//장착 정보 DBupdate
		SYSTEMTIME sysTime = {0,0,0,0,0,0,0,0};
		pAccessory->GetAccessoryLimitDate(sysTime);
		g_DBClient.OnUpdateAccessoryData(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), pAccessory->GetAccessoryIndex(), pAccessory->GetAccessoryCode(), pAccessory->GetPeriodType(),
			sysTime, pAccessory->GetWearingClass(), pAccessory->GetAccessoryValue(), pAccessory->GetComposeCode(), pAccessory->GetComposeValue() );

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] equip accessory update : [user:%d index:%d code:%d value:%d]", m_pUser->GetUserIndex(), pAccessory->GetAccessoryIndex(), pAccessory->GetAccessoryCode(), pAccessory->GetAccessoryValue());
	}
}

void ioUserAccessory::ReleaseAccessory(Accessory* pAccessory)
{
	if( !m_pUser )
		return;

	if( pAccessory )
	{
		pAccessory->SetWearingClass(0);
		SYSTEMTIME sysTime = {0,0,0,0,0,0,0,0};
		pAccessory->GetAccessoryLimitDate(sysTime);
		g_DBClient.OnUpdateAccessoryData(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(),  pAccessory->GetAccessoryIndex(), pAccessory->GetAccessoryCode(), pAccessory->GetPeriodType(),
			sysTime, 0, pAccessory->GetAccessoryValue(), pAccessory->GetComposeCode(), pAccessory->GetComposeValue());

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "[test][accessory] release accessory update : [user:%d index:%d code:%d value:%d]", m_pUser->GetUserIndex(), pAccessory->GetAccessoryIndex(), pAccessory->GetAccessoryCode(), pAccessory->GetAccessoryValue());
	}
}

void ioUserAccessory::GetAccessoryPassedDate(IntVec &vDeleteIndex, IntVec &vDeleteCode)
{
	CTime kCurTime = CTime::GetCurrentTime();

	UserAccessoryItem::iterator it = m_mUserAccessoryMap.begin();
	if( it == m_mUserAccessoryMap.end() )
		return;

	while( it != m_mUserAccessoryMap.end() )
	{
		Accessory *pAccessory = &(it->second);
		if( pAccessory )
		{
			if( pAccessory->GetPeriodType() == PCPT_MORTMAIN )
			{
				++it;
				continue;
			}

			CTime kLimitTime( Help::GetSafeValueForCTimeConstructor( pAccessory->GetYear(),
																	 pAccessory->GetMonth(),
																	 pAccessory->GetDay(),
																	 pAccessory->GetHour(),
																	 pAccessory->GetMinute(),
																	 0) );

			CTimeSpan kRemainTime = kLimitTime - kCurTime;
			if( kRemainTime.GetTotalMinutes() > 0 )
			{
				++it;
				continue;
			}

			vDeleteIndex.push_back(pAccessory->GetAccessoryIndex());
			vDeleteCode.push_back(pAccessory->GetAccessoryCode());
		}
		++it;
	}
}

int ioUserAccessory::GetAccessoryEquipPos(int iCode)
{
	if( iCode <= 0 )
		return -1;

	return (iCode-ACCESSORY_SUBTYPE_DELIMITER) / ACCESSORY_SUBTYPE_DELIMITER;
}

void ioUserAccessory::SetEquipInfo(int& iClassType, int iAccessoryCode, int iAccessoryIndex, int iAccessoryValue, int iComposeCode, int iComposeValue)
{
	if( !m_pUser )
		return;

	int iArray = m_pUser->GetCharArrayByClass(iClassType);
	ioCharacter* pChar = m_pUser->GetCharacter(iArray);

	if( !pChar )
	{
		iClassType = 0;
		return;
	}

	int iSlot = GetAccessoryEquipPos(iAccessoryCode);
	if( iSlot < 0 )
	{
		iClassType	= 0;
		return;
	}

	int iCurEquipInfo = pChar->GetAccessoryIndex(iSlot);
	if( iCurEquipInfo != 0 )
	{
		iClassType	= 0;
		return;
	}

	pChar->ChangeAccessoryItem(iSlot, iAccessoryIndex, iAccessoryCode, iAccessoryValue, iComposeCode, iComposeValue);
}

void ioUserAccessory::SendAllAccessoryInfo()
{
	if( !m_pUser )
		return;
		
	int iSize = m_mUserAccessoryMap.size();

	SP2Packet kPacket( STPK_USER_ACCESSORY_DATA );
	PACKET_GUARD_VOID( kPacket.Write(iSize) );

	UserAccessoryItem::iterator it = m_mUserAccessoryMap.begin();

	for( ; it != m_mUserAccessoryMap.end(); it++ )
	{
		Accessory cAccessory = it->second;

		if( cAccessory.GetAccessoryCode() <= 0 )
			continue;

		PACKET_GUARD_VOID( kPacket.Write(cAccessory.GetAccessoryIndex()) );
		PACKET_GUARD_VOID( kPacket.Write(cAccessory.GetAccessoryCode()) );
		PACKET_GUARD_VOID( kPacket.Write((BYTE)cAccessory.GetPeriodType()) );
		PACKET_GUARD_VOID( kPacket.Write(cAccessory.GetYearMonthDayValue()) );
		PACKET_GUARD_VOID( kPacket.Write(cAccessory.GetHourMinute()) );
		PACKET_GUARD_VOID( kPacket.Write(cAccessory.GetWearingClass()) );
		PACKET_GUARD_VOID( kPacket.Write(cAccessory.GetAccessoryValue()) );
		PACKET_GUARD_VOID( kPacket.Write(cAccessory.GetComposeCode()) );
		PACKET_GUARD_VOID( kPacket.Write(cAccessory.GetComposeValue()) );
	}

	m_pUser->SendMessage(kPacket);
}

void ioUserAccessory::OnCompose( int iAccessoryIndex, int iMaterialIndex1, int iMaterialIndex2, int iMaterialIndex3 )
{
	int iComposeCode = g_AccessoryMgr.GetComposeCode();
	int iComposeValue = g_AccessoryMgr.GetComposeValue( iComposeCode );

	DeleteAccessoryItem((DWORD&)iMaterialIndex1);
	DeleteAccessoryItem((DWORD&)iMaterialIndex2);
	DeleteAccessoryItem((DWORD&)iMaterialIndex3);

	SP2Packet kPacket( STPK_DELETE_ACCESSORY_DATE );
	PACKET_GUARD_VOID(kPacket.Write((int)3));
	PACKET_GUARD_VOID(kPacket.Write(iMaterialIndex1));
	PACKET_GUARD_VOID(kPacket.Write(iMaterialIndex2));
	PACKET_GUARD_VOID(kPacket.Write(iMaterialIndex3));
	m_pUser->SendMessage(kPacket);

	UserAccessoryItem::iterator it = m_mUserAccessoryMap.find(iAccessoryIndex);
	if( it == m_mUserAccessoryMap.end() )
		return;

	Accessory* pAccessory = &(it->second);
	if( !pAccessory )
		return;

	pAccessory->SetComposeCode(iComposeCode);
	pAccessory->SetComposeValue(iComposeValue);

	SYSTEMTIME sysTime = {0,0,0,0,0,0,0,0};
	pAccessory->GetAccessoryLimitDate(sysTime);
	g_DBClient.OnUpdateAccessoryData(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), pAccessory->GetAccessoryIndex(), pAccessory->GetAccessoryCode(), pAccessory->GetPeriodType(),
		sysTime, pAccessory->GetWearingClass(), pAccessory->GetAccessoryValue(), iComposeCode, iComposeValue);

	SP2Packet kPacket1( STPK_ACCESSORY_COMPOSE );
	PACKET_GUARD_VOID( kPacket1.Write( ACCESSORY_COMPOSE_SUCCESS ) );
	PACKET_GUARD_VOID( kPacket1.Write( iAccessoryIndex ) );
	PACKET_GUARD_VOID( kPacket1.Write( iComposeCode ) );
	PACKET_GUARD_VOID( kPacket1.Write( iComposeValue ) );
	m_pUser->SendMessage( kPacket1 );
}

int ioUserAccessory::OnReinforce( int iAccessoryIndex, int iMaterialIndex )
{
	UserAccessoryItem::iterator it = m_mUserAccessoryMap.find(iAccessoryIndex);
	if( it == m_mUserAccessoryMap.end() )
		return ACCESSORY_REINFORCE_NOT_ACCESSORY;

	Accessory* pAccessory = &(it->second);
	if( !pAccessory )
		return ACCESSORY_REINFORCE_NOT_ACCESSORY;

	it = m_mUserAccessoryMap.find(iMaterialIndex);
	if( it == m_mUserAccessoryMap.end() )
		return ACCESSORY_REINFORCE_NOT_MATERIAL;
	Accessory* pMaterial = &(it->second);
	if( !pMaterial )
		return ACCESSORY_REINFORCE_NOT_MATERIAL;

	SYSTEMTIME sysTime = {0,0,0,0,0,0,0,0};
	pAccessory->GetAccessoryLimitDate(sysTime);

	SYSTEMTIME sysMaterialTime = {0,0,0,0,0,0,0,0};
	pMaterial->GetAccessoryLimitDate(sysMaterialTime);

	CTime cCurrentTime = CTime::GetCurrentTime();
	CTime cMatrialTime( sysMaterialTime );
	CTime cAccessoryTime( sysTime );
	CTimeSpan cGapTime = cMatrialTime - cCurrentTime;
	CTimeSpan cRemainTime = cAccessoryTime - cCurrentTime;
	int iAddTime = 0;
	int iRemainTime = cRemainTime.GetTotalMinutes();
	if( iRemainTime < 0 || cGapTime.GetTotalMinutes() < 0 )
		return ACCESSORY_REINFORCE_NOT_ENOUGHTIME;

	if( cGapTime.GetDays() >= 1 )
	{
		CTimeSpan cAddTime( cGapTime.GetTotalSeconds() * g_AccessoryMgr.GetReinforceTimeRate() );
		cAccessoryTime += cAddTime;
		iAddTime = cAddTime.GetTotalMinutes();
	}	
	SYSTEMTIME sysFinalTime = {0,0,0,0,0,0,0,0};	
	cAccessoryTime.GetAsSystemTime( sysFinalTime );

	int iGapValue = g_AccessoryMgr.GetAccessoryGapValue( pAccessory->GetAccessoryCode() );
	int iPrevValue = pAccessory->GetAccessoryValue();

	pAccessory->SetDate(cAccessoryTime.GetYear(), cAccessoryTime.GetMonth(), cAccessoryTime.GetDay(), cAccessoryTime.GetHour(), cAccessoryTime.GetMinute());
	pAccessory->SetAccessoryValue( pAccessory->GetAccessoryValue() + iGapValue );
	g_DBClient.OnUpdateAccessoryData(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), pAccessory->GetAccessoryIndex(), pAccessory->GetAccessoryCode(), pAccessory->GetPeriodType(),
		sysFinalTime, pAccessory->GetWearingClass(), pAccessory->GetAccessoryValue(), pAccessory->GetComposeCode(), pAccessory->GetComposeValue());

	// 악세서리 강화 성공 로그
	g_LogDBClient.OnInsertAccessoryCompound( m_pUser, pAccessory->GetAccessoryCode(), iPrevValue, pAccessory->GetAccessoryValue(), pMaterial->GetAccessoryCode(), LogDBClient::MCR_COMPOUND_SUCCESS );
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "DB AccessoryReinforce :%d - (Index %d)(Code %d)(RemainTime %d)(AddTime %d)", m_pUser->GetUserIndex(), iAccessoryIndex, pAccessory->GetAccessoryCode(), iRemainTime, iAddTime );

	DeleteAccessoryItem((DWORD&)iMaterialIndex);
	SP2Packet kPacket( STPK_DELETE_ACCESSORY_DATE );
	PACKET_GUARD_INT(kPacket.Write((int)1));
	PACKET_GUARD_INT(kPacket.Write(iMaterialIndex));
	m_pUser->SendMessage(kPacket);

	int iYMD	= 0;
	int iHM		= 0;
	g_AccessoryMgr.CalcLimitDateValue(sysFinalTime, iYMD, iHM);

	SP2Packet kPacket1( STPK_ACCESSORY_REINFORCE );
	PACKET_GUARD_INT( kPacket1.Write( ACCESSORY_REINFORCE_SUCCESS ) );
	PACKET_GUARD_INT( kPacket1.Write( iAccessoryIndex ) );
	PACKET_GUARD_INT( kPacket1.Write( pAccessory->GetAccessoryValue() ) );
	PACKET_GUARD_INT( kPacket1.Write( iYMD ) );
	PACKET_GUARD_INT( kPacket1.Write( iHM ) );
	m_pUser->SendMessage( kPacket1 );

	return ACCESSORY_REINFORCE_SUCCESS;
}

bool ioUserAccessory::IsCompose( int iAccessoryIndex )
{
	UserAccessoryItem::iterator it = m_mUserAccessoryMap.find(iAccessoryIndex);
	if( it == m_mUserAccessoryMap.end() )
		return false;

	Accessory* pAccessory = &(it->second);
	if( !pAccessory )
		return false;

	if( pAccessory->GetComposeCode() == 0 )
		return false;

	return true;
}