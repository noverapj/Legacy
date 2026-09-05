#include "stdafx.h"

#include "../MainProcess.h"
#include "..\DataBase\LogDBClient.h"

#include "User.h"
#include "ioEtcItem.h"
#include "ioRelativeGradeManager.h"
#include <strsafe.h>
#include "../MainServerNode/MainServerNode.h"

template<> ioRelativeGradeManager* Singleton< ioRelativeGradeManager >::ms_Singleton = 0;

ioRelativeGradeManager::ioRelativeGradeManager(void)
{
}

ioRelativeGradeManager::~ioRelativeGradeManager(void)
{
}

ioRelativeGradeManager& ioRelativeGradeManager::GetSingleton()
{
	return Singleton< ioRelativeGradeManager >::GetSingleton();
}

void ioRelativeGradeManager::LoadINI()
{
	char szBuf[MAX_PATH] = "", szKey[MAX_PATH] = "";

	ioINILoader kLoader( "config/sp2_relative_grade.ini" );

	kLoader.SetTitle( "common" );
	m_iReduceRate = kLoader.LoadInt( "reduce_rate", 0 );

	for( int i=0; i<FIVE_STAR; ++i )
	{
		wsprintf( szBuf, "grade%d", i+1 );
		kLoader.SetTitle( szBuf );

		m_RelativeGradePresent[i].clear();

		int iSize = kLoader.LoadInt( "max_present", 0 );
		for( int j=0; j<iSize; ++j )
		{
			RelativeGradePresent kPresent;
			wsprintf( szKey, "present%d_name", j+1 );
			kLoader.LoadString( szKey, "°³¹ßÀÚK", szBuf, MAX_PATH );
			kPresent.m_szSendID = szBuf;

			wsprintf( szKey, "present%d_type", j+1 );
			kPresent.m_iPresentType = kLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "present%d_state", j+1 );
			kPresent.m_iPresentState = kLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "present%d_ment", j+1 );
			kPresent.m_iPresentMent = kLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "present%d_period", j+1 );
			kPresent.m_iPresentPeriod = kLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "present%d_value1", j+1 );
			kPresent.m_iPresentValue1 = kLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "present%d_value2", j+1 );
			kPresent.m_iPresentValue2 = kLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "present%d_value3", j+1 );
			kPresent.m_iPresentValue3 = kLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "present%d_value4", j+1 );
			kPresent.m_iPresentValue4 = kLoader.LoadInt( szKey, 0 );

			m_RelativeGradePresent[i].push_back(kPresent);
		}
	}
}

void ioRelativeGradeManager::SendGeneralRewardPresent( User *pUser, int iRelativeGrade )
{
	if( !pUser )
		return;

	int iGrade = iRelativeGrade - 50;
	if( !COMPARE( iGrade, 0, FIVE_STAR ) )
		return;

	vRelativeGradePresent kPresentList = m_RelativeGradePresent[iGrade];
	int iSize = kPresentList.size();
	for( int i=0; i<iSize; ++i )
	{
		RelativeGradePresent kPresent = kPresentList[i];

		CTimeSpan cPresentGapTime( kPresent.m_iPresentPeriod, 0, 0, 0 );
		CTime kPresentTime = CTime::GetCurrentTime() + cPresentGapTime;

		pUser->AddPresentMemory( g_MainServer.GetSendID(), kPresent.m_iPresentType,
			                     kPresent.m_iPresentValue1, kPresent.m_iPresentValue2, kPresent.m_iPresentValue3, kPresent.m_iPresentValue4,
								 kPresent.m_iPresentMent, kPresentTime, kPresent.m_iPresentState );

		g_LogDBClient.OnInsertPresent( 0, g_MainServer.GetSendID(), g_App.GetPublicIP().c_str(), pUser->GetUserIndex(), kPresent.m_iPresentType,
									   kPresent.m_iPresentValue1, kPresent.m_iPresentValue2, kPresent.m_iPresentValue3, kPresent.m_iPresentValue4,
									   LogDBClient::PST_RECIEVE, "GeneralUserReward" );
	}
	pUser->SendPresentMemory();

	SP2Packet kPacket( STPK_RELATIVE_GRADE_REWARD );
	PACKET_GUARD_VOID( kPacket.Write(pUser->GetBackupGradeExp()) );
	for( int i=0; i<2; ++i )
	{
		RelativeGradePresent kPresent = kPresentList[i];
		PACKET_GUARD_VOID( kPacket.Write(kPresent.m_iPresentType) );
		PACKET_GUARD_VOID( kPacket.Write(kPresent.m_iPresentValue1) );
		PACKET_GUARD_VOID( kPacket.Write(kPresent.m_iPresentValue2) );
	}
	pUser->SendMessage( kPacket );

	return;
}