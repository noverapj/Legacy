#include "stdafx.h"
#include "UserTitleInfo.h"
#include "../QueryData/QueryResultData.h"
#include "../DataBase/DBClient.h"
#include "../DataBase/LogDBClient.h"
#include "TitleManager.h"
#include "TitleData.h"
#include "Room.h"
#include "UserTitleInven.h"

UserTitleInven::UserTitleInven()
{
	Init();
}

UserTitleInven::~UserTitleInven()
{
	Destroy();
}

void UserTitleInven::Init()
{
	Initialize(NULL);
}

void UserTitleInven::Destroy()
{
	int iSize	= m_vTitleInven.size();

	for( int i = 0; i < iSize; i++ )
	{
		UserTitleInfo* pInfo	= m_vTitleInven[i];
		if( pInfo )
			delete pInfo;
	}

	m_vTitleInven.clear();
}

void UserTitleInven::Initialize(User* pUser)
{
	m_pUser			= pUser;
	m_bActive		= FALSE;
	m_bEarlyGetCash	= FALSE;

	Destroy();
}

void UserTitleInven::DBtoData(CQueryResultData *query_data)
{
	if( !m_pUser )
		return;

	if( !query_data )
		return;

	Destroy();

	DWORD dwCode		= 0;
	__int64 iValue		= 0;
	int iLevel			= 0;
	BYTE byPremium		= 0;
	BYTE byEquip		= 0;
	BYTE byStatus		= 0;
	
	while( query_data->IsExist() )
	{
		PACKET_GUARD_VOID( query_data->GetValue( dwCode, sizeof(dwCode) ) );
		PACKET_GUARD_VOID( query_data->GetValue( iValue, sizeof(iValue) ) );
		PACKET_GUARD_VOID( query_data->GetValue( iLevel, sizeof(iLevel) ) );
		PACKET_GUARD_VOID( query_data->GetValue( byPremium, sizeof(byPremium) ) );
		PACKET_GUARD_VOID( query_data->GetValue( byEquip, sizeof(byEquip) ) );
		PACKET_GUARD_VOID( query_data->GetValue( byStatus, sizeof(byStatus) ) );

		UserTitleInfo* pInfo	= new UserTitleInfo;
		if( !pInfo )
			return;

		pInfo->CreateTitle(dwCode, iValue, iLevel, byPremium, byEquip, static_cast<TitleStatus>(byStatus));
		m_vTitleInven.push_back(pInfo);
	}
	
	g_TitleManager.FillAccumulateTitleInfo(this);

	SetActive(TRUE);
	if( m_bEarlyGetCash )
	{
		CheckTitleValue(TITLE_HAVE_GOLD, m_pUser->GetCash());
	}

	//Send All InvenInfo
	int iSize = m_vTitleInven.size();

	SP2Packet kPacket(STPK_TITLE_INVEN_INFO);
	PACKET_GUARD_VOID( kPacket.Write(iSize) );

	for( int i = 0; i < iSize; i++ )
	{
		UserTitleInfo* pInfo = m_vTitleInven[i];
		if( !pInfo )
			continue;

		PACKET_GUARD_VOID( kPacket.Write(pInfo->GetCode()) );
		PACKET_GUARD_VOID( kPacket.Write(pInfo->GetCurValue()) );
		PACKET_GUARD_VOID( kPacket.Write(pInfo->GetTitleLevel()) );
		PACKET_GUARD_VOID( kPacket.Write((BYTE)pInfo->IsPremium()) );
		PACKET_GUARD_VOID( kPacket.Write((BYTE)pInfo->IsEquip()) );
		PACKET_GUARD_VOID( kPacket.Write((BYTE)pInfo->GetTitleStatus()) );
	}

	m_pUser->SendMessage(kPacket);
}

void UserTitleInven::FillMoveData(SP2Packet &rkPacket)
{
	int iSize	= m_vTitleInven.size();

	PACKET_GUARD_VOID( rkPacket.Write(iSize) );

	for( int i = 0; i < iSize; i++ )
	{
		UserTitleInfo* pInfo	= m_vTitleInven[i];
		if( !pInfo )
			continue;

		pInfo->FillData(rkPacket);
	}
}

void UserTitleInven::ApplyMoveData(SP2Packet &rkPacket, bool bDummyNode)
{
	int iSize	= m_vTitleInven.size();

	PACKET_GUARD_VOID( rkPacket.Read(iSize) );

	for( int i = 0; i < iSize; i++ )
	{
		UserTitleInfo* pInfo	= new UserTitleInfo;
		if( !pInfo )
			continue;

		pInfo->ApplyData(rkPacket);

		m_vTitleInven.push_back(pInfo);
	}
}

void UserTitleInven::SQLUpdateTitle(UserTitleInfo *pData, TitleUpdateType eType)
{
	if( !pData || !m_pUser )
		return;

	//SQL Update문 호출.
	g_DBClient.OnInsertOrUpdateTitleInfo(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), pData->GetCode(), pData->GetCurValue(), pData->GetTitleLevel(), pData->IsPremium(), 
									pData->IsEquip(), pData->GetTitleStatus(), eType);
}

UserTitleInfo* UserTitleInven::GetEquipTitle()
{
	int iSize	= m_vTitleInven.size();

	for( int i = 0; i < iSize; i++ )
	{
		UserTitleInfo* pInfo	= m_vTitleInven[i];
		if( !pInfo )
			continue;

		if( pInfo->IsEquip() )
			return pInfo;
	}

	return NULL;
}

int UserTitleInven::GetAllTitleCount()
{
	return m_vTitleInven.size();
}

int	UserTitleInven::GetActiveTitleCount()
{
	int iSize	= m_vTitleInven.size();
	int iCount	= 0;

	for( int i = 0; i < iSize; i++ )
	{
		UserTitleInfo* pInfo	= m_vTitleInven[i];
		if( !pInfo )
			continue;

		if( pInfo->IsActiveTitle() )
			iCount++;
	}

	return iCount;
}

BOOL UserTitleInven::CheckHavingPrecedeTitle(const DWORD dwCode)
{
	UserTitleInfo* pInfo = GetTitle(dwCode);
	if( !pInfo )
		return FALSE;

	if( !pInfo->IsActiveTitle() )
		return FALSE;

	return TRUE;
}

void UserTitleInven::CheckTitleValue(TitleTriggerClasses eClass, const __int64 iValue)
{
	//Trigger 요청
	if( !m_pUser )
		return;

	if( !IsActive() )
	{
		SetGetCashFlag(TRUE);
		return;
	}

	if( g_TitleManager.IsAccumulateData(eClass) )
	{
		int iSize	= m_vTitleInven.size();

		for( int i = 0; i < iSize; i++ )
		{
			UserTitleInfo* pInfo	= m_vTitleInven[i];
			if( !pInfo )
				continue;

			if( pInfo->GetTitleStatus() != TITLE_DISABLE )
				continue;

			TitleData* pData = g_TitleManager.GetTitleData(pInfo->GetCode());
			if( !pData )
				continue;

			if( pData->GetClass() == eClass )
			{
				//선행 칭호 획득 여부 확인
				DWORD dwPrecedeCode = pData->GetPrecedeCode();
				if( dwPrecedeCode != 0 )
				{
					if( !CheckHavingPrecedeTitle(dwPrecedeCode) )
						continue;
				}

				g_TitleManager.CheckAchevement(pInfo, m_pUser, iValue);
				if( g_TitleManager.IsComplete(pInfo) )
				{
					g_DBClient.OnInsertOrUpdateTitleInfo(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), pInfo->GetCode(), 0,
													0, 0, 0, TITLE_NEW, TUT_INSERT);
				}
				else
				{
					g_DBClient.OnInsertOrUpdateTitleInfo(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), pInfo->GetCode(), pInfo->GetCurValue(),
													pInfo->GetTitleLevel(), pInfo->IsPremium(), pInfo->IsEquip(), pInfo->GetTitleStatus(), TUT_UPDATE);
				}
			}
		}
	}
	else
	{
		static IntVec vCodeVec;
		vCodeVec.clear();

		g_TitleManager.GetTargetClassTitleInfo(eClass, vCodeVec);

		for( int i = 0; i < (int)vCodeVec.size(); i++ )
		{
			 int iCode = vCodeVec[i];

			UserTitleInfo* pInfo = GetTitle(iCode);
			if( pInfo )
				continue;

			//TitleData* pData = g_TitleManager.GetTitleData(pInfo->GetCode());
			//if( !pData )
			//	continue;

			////선행 칭호 획득 여부 확인
			//DWORD dwPrecedeCode = pData->GetPrecedeCode();
			//if( dwPrecedeCode != 0 )
			//{
			//	if( !CheckHavingPrecedeTitle(dwPrecedeCode) )
			//		continue;
			//}

			if( g_TitleManager.IsComplete(iCode, iValue) )
			{
				//Insert
				g_DBClient.OnInsertOrUpdateTitleInfo(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), iCode, 0,
													0, 0, 0, TITLE_NEW, TUT_INSERT);
			}
		}
	}
	
}

void UserTitleInven::AddTitle(const DWORD dwCode, const __int64 iValue, const int iStatus)
{
	UserTitleInfo* pInfo	= new UserTitleInfo;
	if( !pInfo )
		return;

	pInfo->CreateTitle(dwCode, iValue, 0, FALSE, FALSE, static_cast<TitleStatus>(iStatus));
	m_vTitleInven.push_back(pInfo);
}

void UserTitleInven::EquipTitle(const DWORD dwCode)
{
	if( !m_pUser )
		return;

	int iSize	= m_vTitleInven.size();
	
	//장착 중인건 해제.
	for( int i = 0; i < iSize; i++ )
	{
		UserTitleInfo* pInfo	= m_vTitleInven[i];
		if( !pInfo )
			continue;

		if( pInfo->GetCode() == dwCode )
		{

			pInfo->SetEquipInfo(TRUE);
			SQLUpdateTitle(pInfo, TUT_EQUIP);
			////응답.
			//SP2Packet kPacket(STPK_TITLE_CHANGE);
			//PACKET_GUARD_VOID( kPacket.Write(m_pUser->GetPublicID()) );
			//PACKET_GUARD_VOID( kPacket.Write(pInfo->GetCode()) );
			//PACKET_GUARD_VOID( kPacket.Write(pInfo->GetCurValue()) );
			//PACKET_GUARD_VOID( kPacket.Write(pInfo->GetTitleLevel()) );
			//PACKET_GUARD_VOID( kPacket.Write((BYTE)pInfo->IsPremium()) );
			//PACKET_GUARD_VOID( kPacket.Write((BYTE)pInfo->IsEquip()) );
			//PACKET_GUARD_VOID( kPacket.Write((BYTE)pInfo->GetTitleStatus()) );

			//Room* pRoom = m_pUser->GetMyRoom();
			//if( pRoom )
			//	pRoom->RoomSendPacketTcp(kPacket);
			//else
			//	m_pUser->SendMessage(kPacket);

			continue;
		}

		if( pInfo->IsEquip() )
		{
			pInfo->SetEquipInfo(FALSE);
			SQLUpdateTitle(pInfo, TUT_RELEASE);
		}
	}
}

void UserTitleInven::ConfirmNewData()
{
	if( !m_pUser )
		return;

	int iSize	= m_vTitleInven.size();

	for( int i = 0; i < iSize; i++ )
	{
		UserTitleInfo* pInfo	= m_vTitleInven[i];
		if( pInfo )
		{
			if( pInfo->IsNewData() )
			{
				//SQL 호출.
				g_DBClient.OnUpdateTitleStatus(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex());
				return;
			}
		}
	}
}

UserTitleInfo* UserTitleInven::GetTitle(const DWORD dwCode)
{
	int iSize	= m_vTitleInven.size();

	for( int i = 0; i < iSize; i++ )
	{
		UserTitleInfo* pInfo	= m_vTitleInven[i];
		if( pInfo )
		{
			if( pInfo->GetCode() == dwCode )
				return pInfo;
		}
	}

	return NULL;
}

BOOL UserTitleInven::IsExist(const DWORD dwCode)
{
	UserTitleInfo* pInfo	= GetTitle(dwCode);

	if( pInfo )
		return TRUE;

	return FALSE;
}

BOOL UserTitleInven::IsBoradCastingInfo(const int iState)
{
	switch( iState )
	{
	case TUT_EQUIP:
	case TUT_ALL_RELEASE:
	case TUT_PREMIUM:
	case TUT_LEVELUP:
		return TRUE;
	}

	return FALSE;
}

void UserTitleInven::UpdateTitle(const DWORD dwCode, const __int64 iValue, const int iLevel, const BYTE byPremium, const BYTE byEquip, const BYTE byStatus, const BYTE byActionType)
{
	UserTitleInfo* pInfo	= GetTitle(dwCode);
	if( !pInfo )
	{
		//Insert
		AddTitle(dwCode, iValue, byStatus);
	}
	else
	{
		//Update
		pInfo->SetValue(iValue);
		pInfo->SetLevel(iLevel);
		pInfo->SetPremium(byPremium);
		pInfo->SetEquipInfo(byEquip);
		pInfo->SetTitleStatus(static_cast<TitleStatus>(byStatus));
	}

	if( m_pUser )
	{
		if( byStatus != TITLE_DISABLE )
		{
			BYTE byType = byActionType;
			if( TUT_INSERT_ETC == byType )
				byType	= TUT_INSERT;

			SP2Packet kPacket(STPK_TITLE_UPDATE);
			PACKET_GUARD_VOID( kPacket.Write(byType) );
			
			switch( byType )
			{
			case TUT_EQUIP:
				{
					PACKET_GUARD_VOID( kPacket.Write(m_pUser->GetPublicID()) );
					PACKET_GUARD_VOID( kPacket.Write(pInfo->GetCode()) );
					PACKET_GUARD_VOID( kPacket.Write(pInfo->GetCurValue()) );
					PACKET_GUARD_VOID( kPacket.Write(pInfo->GetTitleLevel()) );
					PACKET_GUARD_VOID( kPacket.Write((BYTE)pInfo->IsPremium()) );
					PACKET_GUARD_VOID( kPacket.Write((BYTE)pInfo->IsEquip()) );
					PACKET_GUARD_VOID( kPacket.Write((BYTE)pInfo->GetTitleStatus()) );
				}
				break;

			case TUT_ALL_RELEASE:
				{
					PACKET_GUARD_VOID( kPacket.Write(m_pUser->GetPublicID()) );
					PACKET_GUARD_VOID( kPacket.Write(0) );
					PACKET_GUARD_VOID( kPacket.Write((__int64)0) );
					PACKET_GUARD_VOID( kPacket.Write(0) );
					PACKET_GUARD_VOID( kPacket.Write((BYTE)0) );
					PACKET_GUARD_VOID( kPacket.Write((BYTE)0) );
					PACKET_GUARD_VOID( kPacket.Write((BYTE)0) )
				}
				break;

			case TUT_RELEASE:
				{}
				break;

			default:
				{
					PACKET_GUARD_VOID( kPacket.Write(m_pUser->GetPublicID()) );
					PACKET_GUARD_VOID( kPacket.Write(dwCode) );
					PACKET_GUARD_VOID( kPacket.Write(iValue) );
					PACKET_GUARD_VOID( kPacket.Write(iLevel) );
					PACKET_GUARD_VOID( kPacket.Write(byPremium) );
					PACKET_GUARD_VOID( kPacket.Write(byEquip) );
					PACKET_GUARD_VOID( kPacket.Write(byStatus) );
				}
				break;
			}
			
			if( byType != TUT_RELEASE )
			{
				if( m_pUser->GetMyRoom() && IsBoradCastingInfo(byActionType) )
				{
					Room* pRoom = m_pUser->GetMyRoom();
					pRoom->RoomSendPacketTcp(kPacket);
				}
				else
				{
					m_pUser->SendMessage(kPacket);
				}
			}
		}
	}
}

void UserTitleInven::ConvertNewToActiveStatus()
{
	int iSize	= m_vTitleInven.size();

	for( int i = 0; i < iSize; i++ )
	{
		UserTitleInfo* pInfo	= m_vTitleInven[i];
		if( pInfo )
		{
			if( pInfo->IsNewData() )
			{
				pInfo->ActiveTitle();
			}
		}
	}
}

BOOL UserTitleInven::PremiumLevelUpCheck(const DWORD dwCode)
{
	UserTitleInfo* pInfo	= GetTitle(dwCode);
	if( !pInfo )
		return FALSE;

	if( !pInfo->IsPremium() )
		return FALSE;

	__int64 iTemp	= pInfo->GetCurValue();
	
	//if( !IsRightAccumulateSeconds(iTemp, iValue) )
	//	return FALSE;

	pInfo->SetValue(iTemp+g_TitleManager.GetAddPremiumTime());

	BOOL bUP = g_TitleManager.IsLevelUp(dwCode, pInfo->GetTitleLevel(), pInfo->GetCurValue());

	if( bUP )
	{
		//Max Level 여부 확인.
		int iMaxLevel	= g_TitleManager.GetMaxPremiumLevel(dwCode);
		if( iMaxLevel <= 0 )
		{
			//LOG
			LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL,"[warning][title]Invalid title max level : [%d]", dwCode );
			return FALSE;
		}
		
		if( iMaxLevel == pInfo->GetTitleLevel() + 1 )
		{
			g_DBClient.OnInsertOrUpdateTitleInfo(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), pInfo->GetCode(), 0,
												pInfo->GetTitleLevel() + 1, pInfo->IsPremium(), pInfo->IsEquip(), pInfo->GetTitleStatus(), TUT_LEVELUP);
		}
		else
		{
			g_DBClient.OnInsertOrUpdateTitleInfo(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), pInfo->GetCode(), pInfo->GetCurValue(),
												pInfo->GetTitleLevel() + 1, pInfo->IsPremium(), pInfo->IsEquip(), pInfo->GetTitleStatus(), TUT_LEVELUP);
		}
	}
	else
	{
		g_DBClient.OnInsertOrUpdateTitleInfo(m_pUser->GetUserDBAgentID(), m_pUser->GetAgentThreadID(), m_pUser->GetUserIndex(), pInfo->GetCode(), pInfo->GetCurValue(),
											pInfo->GetTitleLevel(), pInfo->IsPremium(), pInfo->IsEquip(), pInfo->GetTitleStatus(), TUT_UPDATE);
	}

	return TRUE;
}

BOOL UserTitleInven::IsRightAccumulateSeconds(const __int64 iCurData, const __int64 iNewData)
{
	if( iCurData > iNewData )
		return FALSE;

	__int64 iGap = iNewData - iCurData;
	if( iGap > 3 * 60 * 1000 )
		return FALSE;

	return TRUE;
}

void UserTitleInven::ReleaseTitle(const DWORD dwCode)
{
	if( !m_pUser )
		return;

	int iSize	= m_vTitleInven.size();
	
	//장착 중인건 해제.
	for( int i = 0; i < iSize; i++ )
	{
		UserTitleInfo* pInfo	= m_vTitleInven[i];
		if( !pInfo )
			continue;

		if( pInfo->IsEquip() )
		{
			pInfo->SetEquipInfo(FALSE);
			SQLUpdateTitle(pInfo, TUT_ALL_RELEASE);
		}
	}

	//응답.
	/*SP2Packet kPacket(STPK_TITLE_CHANGE);
	PACKET_GUARD_VOID( kPacket.Write(m_pUser->GetPublicID()) );
	PACKET_GUARD_VOID( kPacket.Write(0) );
	PACKET_GUARD_VOID( kPacket.Write((__int64)0) );
	PACKET_GUARD_VOID( kPacket.Write(0) );
	PACKET_GUARD_VOID( kPacket.Write((BYTE)0) );
	PACKET_GUARD_VOID( kPacket.Write((BYTE)0) );
	PACKET_GUARD_VOID( kPacket.Write((BYTE)0) );

	Room* pRoom = m_pUser->GetMyRoom();
	if( pRoom )
		pRoom->RoomSendPacketTcp(kPacket);
	else
		m_pUser->SendMessage(kPacket);*/
}