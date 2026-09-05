
#include "stdafx.h"

#include "ioAwardManager.h"

#include "ioPlayMode.h"

template<> ioAwardManager* Singleton< ioAwardManager >::ms_Singleton = 0;

ioAwardManager::ioAwardManager()
{
}

ioAwardManager::~ioAwardManager()
{
	ClearAwardInfoList();
}

ioAwardManager& ioAwardManager::GetSingleton()
{
	return Singleton< ioAwardManager >::GetSingleton();
}

ioAwardManager::AwardData* ioAwardManager::GetAwardData( int iCategory )
{
	vAwardData::iterator iter;
	for( iter=m_vAwardList.begin() ; iter!=m_vAwardList.end() ; ++iter )
	{
		AwardData *pItem = *iter;
		if( pItem->m_iType == iCategory )
			return pItem;
	}

	return NULL;
}

void ioAwardManager::ClearAwardInfoList()
{
	vAwardData::iterator iter;
	for( iter=m_vAwardList.begin() ; iter!=m_vAwardList.end() ; ++iter )
	{
		AwardData *pItem = *iter;
		
		pItem->m_ModeTypeList.clear();
		SAFEDELETE( pItem->m_pTitleRes );
		SAFEDELETE( pItem->m_pIcon );
		SAFEDELETE( pItem );
	}
	m_vAwardList.clear();
	m_AwardSelectMap.clear();
}

void ioAwardManager::LoadAwardInfo()
{
	ClearAwardInfoList();

	char szBuf[MAX_PATH];
	ioINILoader_e kLoader( "config/sp2_awarding.ini" );
	kLoader.SetTitle_e( "Info" );

	int i = 0;
	for(i = 0;i < MAX_MODE_TYPE;i++)
	{
		sprintf_e( szBuf, "ModeSelectAward_%d", i );
		m_AwardSelectMap.insert( AwardSelectMap::value_type( (ModeType)i, kLoader.LoadInt( szBuf, AWARD_SELECT_ALL ) ) );
	}
	
	int iMaxAwardCount = kLoader.LoadInt_e( "MaxAward", 0 );	
	for( i=0; i<iMaxAwardCount; i++ )
	{
		sprintf_e( szBuf, "Award%d", i + 1 );
		kLoader.SetTitle( szBuf );
		
		AwardData *pAward = new AwardData;
		pAward->m_iType		      = kLoader.LoadInt_e( "Type", 0 );
		pAward->m_iLimitValue     = kLoader.LoadInt_e( "LimitValue", 0 );
		pAward->m_iSortPoint      = kLoader.LoadInt_e( "SortPoint", 0 );
		pAward->m_iGoodBadType    = kLoader.LoadInt_e( "GoodBad_type", 0 );
		pAward->m_iMultiplySortPoint = kLoader.LoadInt_e( "MultiplySortPoint", 1 );
		pAward->m_bDescVariable   = kLoader.LoadBool_e( "DescVariable", false );

		// 적용 모드 리스트
		kLoader.LoadString_e( "ModeType", "", szBuf, MAX_PATH );
		if( strlen( szBuf ) > 0 )
		{
			StringVector vStringVector = ioStringConverter::Split( szBuf, "." );
			StringVector::iterator iter;
			for(iter = vStringVector.begin();iter != vStringVector.end();++iter)
			{
				int iValue = atoi( iter->c_str() );
				if( iValue == 0 )      // 0이면 모두 포함한다.
					break;
				pAward->m_ModeTypeList.push_back( iValue );
			}
		}
		
		kLoader.LoadString_e( "DescColor", "", szBuf, MAX_PATH );
		char *pTemp = NULL;
		pAward->m_dwDescColor = strtoul( szBuf, &pTemp, 16 );

		//String
		kLoader.LoadString_e( "Title", "", szBuf, MAX_PATH );
		pAward->m_szTitle = szBuf;

		kLoader.LoadString_e( "Desc", "", szBuf, MAX_PATH );
		pAward->m_szDesc = szBuf;
		
		kLoader.LoadString_e( "TitleRes", "", szBuf, MAX_PATH );
		pAward->m_pTitleRes = g_UIImageSetMgr.CreateImageByFullName( szBuf );

		kLoader.LoadString_e( "Icon", "", szBuf, MAX_PATH );
		pAward->m_pIcon = g_UIImageSetMgr.CreateImageByFullName( szBuf );

		kLoader.LoadString( "Ani", "", szBuf, MAX_PATH );
		pAward->m_szCharAni = szBuf;

		kLoader.LoadString_e( "Sound", "", szBuf, MAX_PATH );
		pAward->m_szSound = szBuf;

		m_vAwardList.push_back( pAward );
	}
}

void ioAwardManager::GetDebugContent( int iCategory, int iUserArray, char *szContent )
{
	AwardData *pAward = GetAwardData( iCategory );
	if( !pAward )	return;

	if( COMPARE( iUserArray, 0, (int)pAward->m_vUserList.size() ) )
	{
		sprintf( szContent, "%s : %d(%d)", pAward->m_vUserList[iUserArray].m_szName.c_str(),
										   pAward->m_vUserList[iUserArray].m_iOriginalValue,
										   pAward->m_vUserList[iUserArray].m_iValue );
	}
}

int ioAwardManager::GetRandAwardType()
{
	IntVec vAwardTypeList;
	vAwardData::iterator iter;
	for( iter=m_vAwardList.begin() ; iter!=m_vAwardList.end() ; ++iter )
	{
		AwardData *pItem = *iter;
		if( !IsAwardSameModeType( pItem ) ) continue;

		vAwardTypeList.push_back( pItem->m_iType );
	}

	if( vAwardTypeList.empty() )
		return 1;

	int iMaxRand = vAwardTypeList.size();
	int iRandValue = rand()%iMaxRand;
	int iReturnValue = vAwardTypeList[iRandValue];
	vAwardTypeList.clear();
	
	return iReturnValue;
}

int ioAwardManager::GetGoodBadType( int iCategory )
{
	AwardData *pAward = GetAwardData( iCategory );
	if( pAward )
		return pAward->m_iGoodBadType;

	return 0;
}

const ioHashString &ioAwardManager::GetAwardTitle( int iCategory )
{
	static ioHashString stReturn = "NoneTitle";
	AwardData *pAward = GetAwardData( iCategory );
	if( !pAward )
		return stReturn;
	return pAward->m_szTitle;
}

const ioHashString &ioAwardManager::GetAwardDesc( int iCategory )
{
	static ioHashString stReturn = "NoneAwardDesc";
	AwardData *pAward = GetAwardData( iCategory );
	if( !pAward )
		return stReturn;
	return pAward->m_szDesc;
}

const DWORD ioAwardManager::GetAwardDescColor( int iCategory )
{
	AwardData *pAward = GetAwardData( iCategory );
	if( !pAward )
		return 0xFFFFB400;
	return pAward->m_dwDescColor;
}

bool ioAwardManager::IsAwardDescVariable( int iCategory )
{
	AwardData *pAward = GetAwardData( iCategory );
	if( !pAward )
		return false;
	return pAward->m_bDescVariable;
}

ioUIRenderImage *ioAwardManager::GetAwardTitleRes( int iCategory )
{
	AwardData *pAward = GetAwardData( iCategory );
	if( !pAward || !pAward->m_pTitleRes )
		return NULL;

	pAward->m_pTitleRes->SetScale( FLOAT1 );
	return pAward->m_pTitleRes;
}

ioUIRenderImage *ioAwardManager::GetAwardIcon( int iCategory )
{
	AwardData *pAward = GetAwardData( iCategory );
	if( !pAward )
		return NULL;

	return pAward->m_pIcon;
}

const ioHashString &ioAwardManager::GetAwardAniName( int iCategory )
{
	static ioHashString stReturn = "NoneAniName";
	AwardData *pAward = GetAwardData( iCategory );
	if( !pAward )
		return stReturn;
	return pAward->m_szCharAni;
}

const ioHashString &ioAwardManager::GetAwardSound( int iCategory )
{
	static ioHashString stReturn = "NoneSound";
	AwardData *pAward = GetAwardData( iCategory );
	if( !pAward )
		return stReturn;
	return pAward->m_szSound;
}

bool ioAwardManager::IsAwardSameModeType( AwardData *pAward )
{
	if( !pAward ) return false;
	if( pAward->m_ModeTypeList.empty() ) return true;

	for(int i = 0;i < (int)pAward->m_ModeTypeList.size();i++)
	{
		if( (int)ioPlayMode::GetModeType() == pAward->m_ModeTypeList[i] )
			return true;
	}
	return false;
}

int ioAwardManager::GetContribute( const ioHashString &rkName )
{
	AwardData *pAward = GetAwardData( AC_CONTRIBUTE );
	if( !pAward ) return 0;

	vAwardUser_iter iter;
	for( iter=pAward->m_vUserList.begin() ; iter!=pAward->m_vUserList.end() ; ++iter )
	{
		AwardUser &kUser = *iter;
		if( kUser.m_szName == rkName )
		{
			return kUser.m_iValue;
		}
	}

	return 0;
}

const AwardUser &ioAwardManager::GetTopAwardUser( int iCategory )
{
	static AwardUser kReturnAward;
	AwardData *pAward = GetAwardData( iCategory );
	if( !pAward )
		return kReturnAward;
	// 1위를 뽑는다. 조건 : 동일 점수일 때 기여도 우선 순위.		
	vAwardUser_iter iter;
	for( iter=pAward->m_vUserList.begin() ; iter!=pAward->m_vUserList.end() ; ++iter )
	{
		AwardUser &kUser = *iter;
		kUser.m_iContribute = GetContribute( kUser.m_szName );
	}
	if( pAward->m_vUserList.empty() )
		return kReturnAward;
	std::sort( pAward->m_vUserList.begin(), pAward->m_vUserList.end(), AwardInfoSort() );

	return pAward->m_vUserList[0];
}

const AwardUser &ioAwardManager::GetTopAwardUser( AwardData *pAward )
{
	static AwardUser kReturnAward;
	if( !pAward )
		return kReturnAward;
	// 1위를 뽑는다. 조건 : 동일 점수일 때 기여도 우선 순위.		
	vAwardUser_iter iter;
	for( iter=pAward->m_vUserList.begin() ; iter!=pAward->m_vUserList.end() ; ++iter )
	{
		AwardUser &kUser = *iter;
		kUser.m_iContribute = GetContribute( kUser.m_szName );
	}
	if( pAward->m_vUserList.empty() )
		return kReturnAward;
	std::sort( pAward->m_vUserList.begin(), pAward->m_vUserList.end(), AwardInfoSort() );

	return pAward->m_vUserList[0];
}

void ioAwardManager::AddValue( AwardData *pAward, const ioHashString &rkName, int iValue, int iOriginalValue  )
{
	vAwardUser_iter iter;
	for( iter=pAward->m_vUserList.begin() ; iter!=pAward->m_vUserList.end() ; ++iter )
	{
		AwardUser &kUser = *iter;
		if( kUser.m_szName == rkName )
		{
			kUser.m_iValue += iValue;
			kUser.m_iOriginalValue += iOriginalValue;
			break;
		}
	}
}

void ioAwardManager::AddUser( AwardData *pAward, const ioHashString &rkName )
{
	vAwardUser_iter iter;
	for( iter=pAward->m_vUserList.begin() ; iter!=pAward->m_vUserList.end() ; ++iter )
	{
		AwardUser &kUser = *iter;
		if( kUser.m_szName == rkName )
		{
			kUser.m_iValue = 0;
			kUser.m_iOriginalValue = 0;
			return;
		}
	}

	AwardUser kUser;
	kUser.m_szName = rkName;
	pAward->m_vUserList.push_back( kUser );
}

void ioAwardManager::RemoveUser( AwardData *pAward, const ioHashString &rkName )
{
	vAwardUser_iter iter;
	for( iter=pAward->m_vUserList.begin() ; iter!=pAward->m_vUserList.end() ; ++iter )
	{
		AwardUser &kUser = *iter;
		if( kUser.m_szName == rkName )
		{
			pAward->m_vUserList.erase( iter );
			return;
		}
	}
}

void ioAwardManager::AddNewAwardUser( const ioHashString &rkName )
{
	vAwardData::iterator iter;
	for( iter=m_vAwardList.begin() ; iter!=m_vAwardList.end() ; ++iter )
	{
		AwardData *pItem = *iter;
		if( pItem )
		{
			AddUser( pItem, rkName );
		}
	}
}

void ioAwardManager::RemoveAwardUser( const ioHashString &rkName )
{
	vAwardData::iterator iter;
	for( iter=m_vAwardList.begin() ; iter!=m_vAwardList.end() ; ++iter )
	{
		AwardData *pItem = *iter;
		if( pItem )
		{
			RemoveUser( pItem, rkName );
		}
	}
}

void ioAwardManager::RemoveAllUser()
{
	vAwardData::iterator iter;
	for( iter=m_vAwardList.begin() ; iter!=m_vAwardList.end() ; ++iter )
	{
		AwardData *pItem = *iter;
		if( pItem )
			pItem->m_vUserList.clear();
	}
}

//////////////////////////////////////////////////////////////////////////
void ioAwardManager::AddContribute( const ioHashString &rkName, int iContribute )
{
	AwardData *pAward = GetAwardData( AC_CONTRIBUTE );
	if( !pAward ) return;

	vAwardUser_iter iter;
	for( iter=pAward->m_vUserList.begin() ; iter!=pAward->m_vUserList.end() ; ++iter )
	{
		AwardUser &kUser = *iter;
		if( kUser.m_szName == rkName )
		{
			kUser.m_iValue = iContribute;
			kUser.m_iOriginalValue = iContribute;
			break;
		}
	}
}

void ioAwardManager::AddDamage( const ioHashString &rkName, int iDamage, int iOriginalValue )
{
	AwardData *pAward = GetAwardData( AC_DAMAGE );
	if( !pAward ) return;

	AddValue( pAward, rkName, iDamage, iOriginalValue );
}

void ioAwardManager::AddHit( const ioHashString &rkName, int iHit )
{
	AwardData *pAward = GetAwardData( AC_HIT );
	if( !pAward ) return;

	AddValue( pAward, rkName, iHit, iHit );
}

void ioAwardManager::AddShopping( const ioHashString &rkName, int iBuyCount )
{
	AwardData *pAward = GetAwardData( AC_SHOPPING );
	if( !pAward ) return;

	AddValue( pAward, rkName, iBuyCount, iBuyCount );
}

void ioAwardManager::AddDrop( const ioHashString &rkName, int iDropCount, int iOriginalValue )
{
	AwardData *pAward = GetAwardData( AC_DROP );
	if( !pAward ) return;

	AddValue( pAward, rkName, iDropCount, iOriginalValue );
}

void ioAwardManager::AddWounded( const ioHashString &rkName, int iWoundedCount, int iOriginalValue )
{
	AwardData *pAward = GetAwardData( AC_WOUNDED );
	if( !pAward ) return;

	AddValue( pAward, rkName, iWoundedCount, iOriginalValue );
}

void ioAwardManager::AddDefense( const ioHashString &rkName, int iDefenseCount, int iOriginalValue )
{
	AwardData *pAward = GetAwardData( AC_DEFENSE );
	if( !pAward ) return;

	AddValue( pAward, rkName, iDefenseCount, iOriginalValue );
}

void ioAwardManager::AddTeamKill( const ioHashString &rkName, int iTeamKill )
{
	AwardData *pAward = GetAwardData( AC_TEAMKILL );
	if( !pAward ) return;

	AddValue( pAward, rkName, iTeamKill, iTeamKill );
}

void ioAwardManager::AddCombo( const ioHashString &rkName, int iComboCount )
{
	AwardData *pAward = GetAwardData( AC_COMBO );
	if( !pAward ) return;

	AddValue( pAward, rkName, iComboCount, iComboCount );
}

void ioAwardManager::AddTeamStoneDamage( const ioHashString &rkName, int iDamage, int iOriginalValue )
{
	AwardData *pAward = GetAwardData( AC_TEAM_STONE_DAMAGE );
	if( !pAward ) return;

	AddValue( pAward, rkName, iDamage, iOriginalValue );
}

void ioAwardManager::AddOtherTeamStoneDamage( const ioHashString &rkName, int iDamage, int iOriginalValue )
{
	AwardData *pAward = GetAwardData( AC_OTHER_TEAM_STONE_DAMAGE );
	if( !pAward ) return;

	AddValue( pAward, rkName, iDamage, iOriginalValue );
}

void ioAwardManager::AddBlockStoneAttack( const ioHashString &rkName, int iCount )
{
	AwardData *pAward = GetAwardData( AC_BLOCK_STONE_ATTACK );
	if( !pAward ) return;

	AddValue( pAward, rkName, iCount, iCount );
}

void ioAwardManager::AddPrisonerEscape( const ioHashString &rkName, int iEscapeCount, int iOriginalValue )
{
	AwardData *pAward = GetAwardData( AC_PRISONER_ESCAPE );
	if( !pAward ) return;

	AddValue( pAward, rkName, iEscapeCount, iOriginalValue );
}

void ioAwardManager::AddPrisonerDrop( const ioHashString &rkName, int iDropCount, int iOriginalValue )
{
	AwardData *pAward = GetAwardData( AC_PRISONER_DROP );
	if( !pAward ) return;

	AddValue( pAward, rkName, iDropCount, iOriginalValue );
}

void ioAwardManager::AddPrisonerLastKill( const ioHashString &rkName, int iLastKill, int iOriginalValue )
{
	AwardData *pAward = GetAwardData( AC_PRISONER_LAST_KILL );
	if( !pAward ) return;

	AddValue( pAward, rkName, iLastKill, iOriginalValue );
}

void ioAwardManager::AddPrisonerDie( const ioHashString &rkName, int iDie )
{
	AwardData *pAward = GetAwardData( AC_PRISONER_DIE );
	if( !pAward ) return;

	AddValue( pAward, rkName, iDie, iDie );
}

void ioAwardManager::AddKingGuard( const ioHashString &rkName, int iTime )
{
	AwardData *pAward = GetAwardData( AC_KING_GUARD );
	if( !pAward ) return;

	AddValue( pAward, rkName, iTime, iTime );
}

void ioAwardManager::AddKing( const ioHashString &rkName, int iTime )
{
	AwardData *pAward = GetAwardData( AC_THE_KING );
	if( !pAward ) return;

	AddValue( pAward, rkName, iTime, iTime );
}

void ioAwardManager::AddOtherKingAttack( const ioHashString &rkName, int iAttackCount )
{
	AwardData *pAward = GetAwardData( AC_OTHER_KING_ATTACK );
	if( !pAward ) return;

	AddValue( pAward, rkName, iAttackCount, iAttackCount );
}

void ioAwardManager::AddTeamKingAttack( const ioHashString &rkName, int iAttackCount )
{
	AwardData *pAward = GetAwardData( AC_TEAM_KING_ATTACK );
	if( !pAward ) return;

	AddValue( pAward, rkName, iAttackCount, iAttackCount );
}

void ioAwardManager::AddGoal( const ioHashString &rkName, int iValue, int iOriginalValue )
{
	AwardData *pAward = GetAwardData( AC_GOAL );
	if( !pAward ) return;

	AddValue( pAward, rkName, iValue, iOriginalValue );
}

void ioAwardManager::AddAssist( const ioHashString &rkName, int iValue, int iOriginalValue )
{
	AwardData *pAward = GetAwardData( AC_ASSIST );
	if( !pAward ) return;

	AddValue( pAward, rkName, iValue, iOriginalValue );
}

void ioAwardManager::AddOwnerGoal( const ioHashString &rkName, int iValue, int iOriginalValue )
{
	AwardData *pAward = GetAwardData( AC_OWNER_GOAL );
	if( !pAward ) return;

	AddValue( pAward, rkName, iValue, iOriginalValue );
}

void ioAwardManager::AddGoalSave( const ioHashString &rkName, int iValue, int iOriginalValue )
{
	AwardData *pAward = GetAwardData( AC_GOAL_SAVE );
	if( !pAward ) return;

	AddValue( pAward, rkName, iValue, iOriginalValue );
}

void ioAwardManager::AddGangsiKing( const ioHashString &rkName, int iValue )
{
	AwardData *pAward = GetAwardData( AC_GANGSI_KING );
	if( !pAward ) return;

	AddValue( pAward, rkName, iValue, iValue );
}

void ioAwardManager::AddGangsiSurvivor( const ioHashString &rkName, int iTime )
{
	AwardData *pAward = GetAwardData( AC_GANGSI_SURVIVOR );
	if( !pAward ) return;

	AddValue( pAward, rkName, iTime, iTime );
}

void ioAwardManager::AddShuffleFulStar( const ioHashString &rkName, int iStar )
{
	AwardData *pAward = GetAwardData( AC_SHUFFLE_FUL_STAR );
	if( !pAward ) return;

	AddValue( pAward, rkName, iStar, iStar );
}

void ioAwardManager::AddShuffleLessStar( const ioHashString &rkName, int iStar )
{
	AwardData *pAward = GetAwardData( AC_SHUFFLE_LESS_STAR );
	if( !pAward ) return;
	AddValue( pAward, rkName, iStar, iStar );
}

void ioAwardManager::AddFlagCapture( const ioHashString &rkName, int iValue )
{
	AwardData *pAward = GetAwardData( AC_FLAG_CAPTURE );
	if( !pAward ) return;

	AddValue( pAward, rkName, iValue, iValue );
}

void ioAwardManager::SendAwardInfoSync( const ioHashString &rkJoiner )
{
	if( rkJoiner.IsEmpty() ) return;

	int iMaxUserCount = 0;
	vAwardData vSyncAward;
	vAwardData::iterator iter;
	for( iter=m_vAwardList.begin() ; iter!=m_vAwardList.end() ; ++iter )
	{
		AwardData *pItem = *iter;
		if( !IsAwardSameModeType( pItem ) ) continue;

		if( pItem )
		{
			if( pItem->m_iType == AC_CONTRIBUTE ) continue;        //기여도는 기여도 항목에서 해주므로 필요 없음.

			const AwardUser &kUser = GetTopAwardUser( pItem );
			if( kUser.m_szName.IsEmpty() ) continue;
			if( kUser.m_iValue == 0 ) continue;

			vSyncAward.push_back( pItem );
			if( iMaxUserCount < (int)pItem->m_vUserList.size() )
				iMaxUserCount = (int)pItem->m_vUserList.size();
		}
	}

	if( vSyncAward.empty() )
		return;

	int iSendListSize = 1;         // 12인방 초과는 1개씩 동기화
	if( iMaxUserCount <= 4 )       // 4인방
		iSendListSize = 8;         // 8개씩 동기화
	else if( iMaxUserCount <= 8 )  // 8인방
		iSendListSize = 4;         // 4개씩 동기화
	else if( iMaxUserCount <= 12 ) // 12인방
		iSendListSize = 3;         // 3개씩 동기화

	int iMaxAwardCount = (int)vSyncAward.size();
	for(int iStartArray = 0;iStartArray < iMaxAwardCount;)
	{
		int iLoop = iStartArray;
		int iSendSize = min( iMaxAwardCount - iStartArray, iSendListSize );		

		SP2Packet kPacket( CTPK_AWARDING_INFO );
		kPacket << rkJoiner << iSendSize;	
		for(;iLoop < iStartArray + iSendSize;iLoop++)
		{
			AwardData *pAward = vSyncAward[iLoop];
			if( pAward )
			{
				kPacket << pAward->m_iType;      //동기화 시켜야하는 수상 항목

				int i;
				vAwardUser vUserList;
				for(i = 0;i < (int)pAward->m_vUserList.size();i++)
				{
					AwardUser &kUser = pAward->m_vUserList[i];
					if( kUser.m_iValue != 0 )
						vUserList.push_back( kUser );
				}
				kPacket << (int)vUserList.size(); //동기화 시켜야하는 수상 항목 유저 갯수
				for(i = 0;i < (int)vUserList.size();i++)
					kPacket << vUserList[i].m_szName << vUserList[i].m_iValue << vUserList[i].m_iOriginalValue; //동기화 시켜야하는 수상 항목 유저
				vUserList.clear();
			}
			else
			{
				kPacket << -1 << 0;
			}
		}
		TCPNetwork::SendToServer( kPacket );
		iStartArray = iLoop;
	}
	vSyncAward.clear();
}

void ioAwardManager::FillAwardInfo( SP2Packet &rkPacket )
{
	vAwardData vSyncAward;

	vAwardData::iterator iter;
	for( iter=m_vAwardList.begin() ; iter!=m_vAwardList.end() ; ++iter )
	{
		AwardData *pItem = *iter;
		if( !IsAwardSameModeType( pItem ) ) continue;

		if( pItem )
		{
			if( pItem->m_iType == AC_CONTRIBUTE ) continue;        //기여도는 기여도 항목에서 해주므로 필요 없음.

			const AwardUser &kUser = GetTopAwardUser( pItem );
			if( kUser.m_szName.IsEmpty() ) continue;
			if( kUser.m_iValue == 0 ) continue;

			vSyncAward.push_back( pItem );
		}
	}

	if( vSyncAward.empty() )
		return;

	rkPacket << (int)vSyncAward.size();       //동기화 시켜야하는 수상 항목 갯수

	for( iter=vSyncAward.begin() ; iter!=vSyncAward.end() ; ++iter )
	{
		AwardData *pAward = *iter;
		if( pAward )
		{
			rkPacket << pAward->m_iType;      //동기화 시켜야하는 수상 항목

			int i;
			vAwardUser vUserList;
			for(i = 0;i < (int)pAward->m_vUserList.size();i++)
			{
				AwardUser &kUser = pAward->m_vUserList[i];
				if( kUser.m_iValue != 0 )
					vUserList.push_back( kUser );
			}
            rkPacket << (int)vUserList.size(); //동기화 시켜야하는 수상 항목 유저 갯수
			for(i = 0;i < (int)vUserList.size();i++)
				rkPacket << vUserList[i].m_szName << vUserList[i].m_iValue << vUserList[i].m_iOriginalValue; //동기화 시켜야하는 수상 항목 유저
			vUserList.clear();
		}
	}

	vSyncAward.clear();
}

void ioAwardManager::ApplyAwardInfo( SP2Packet &rkPacket )
{
	int iMaxAward;
	rkPacket >> iMaxAward;

	int i, j;
	for(i = 0;i < iMaxAward;i++)
	{
		int iCategory, iMaxUser;
		rkPacket >> iCategory >> iMaxUser;
		AwardData *pAward = GetAwardData( iCategory );
		
		for(j = 0;j < iMaxUser;j++)
		{
			ioHashString szName;
			int iValue, iOriginalValue;
			rkPacket >> szName >> iValue >> iOriginalValue;
			
			if( pAward )			
				AddValue( pAward, szName, iValue, iOriginalValue );
		}
	}
}

void ioAwardManager::FillAwardResult( SP2Packet &rkPacket )
{	
	AwardSelectUserSplitCheck();

	vAwardRandSortData vResultAward;
	vAwardData::iterator iter;
	for( iter=m_vAwardList.begin() ; iter!=m_vAwardList.end() ; ++iter )
	{
		AwardData *pAward = *iter;
		if( !IsAwardSameModeType( pAward ) ) continue;

		if( pAward )
		{
			const AwardUser &kUser = GetTopAwardUser( pAward );
			if( kUser.m_szName.IsEmpty() ) continue;
			if( kUser.m_iValue < pAward->m_iLimitValue ) continue;

			AwardRandSortData kSortAward;
			kSortAward.m_pAwardData = pAward;
			kSortAward.m_szTopName  = kUser.m_szName;
			vResultAward.push_back( kSortAward );
		}
	}

	if( vResultAward.empty() )
		return;

	int i = 0;	
	int iMaxSize = vResultAward.size();	
	// 최대 수상 개수를 넘으면 랜덤을 적용
	if( iMaxSize > MAX_AWARDING )
	{
		// 기본값 세팅
		int iDefaultSortPoint = ( vResultAward.size() * 10 );
		for(i = 0;i < iMaxSize;i++)
		{
			AwardRandSortData &rkSrc = vResultAward[i];
			rkSrc.m_iSortPoint = ( iDefaultSortPoint * rkSrc.m_pAwardData->m_iMultiplySortPoint );
		}

		// 수상 개수에 따라 랜덤값은 줄인다.
		for(i = 0;i < iMaxSize;i++)
		{
			AwardRandSortData &rkSrc = vResultAward[i];
			for(int j = 0;j < iMaxSize;j++)
			{
				AwardRandSortData &rkDst = vResultAward[j];
				if( rkSrc.m_szTopName == rkDst.m_szTopName )
				{
					rkDst.m_iSortPoint = max( 10, rkDst.m_iSortPoint - 10 );
				}
			}
		}

		// 수상 항목들의 랜덤값이 큰값으로 정렬한다.
		for(i = 0;i < iMaxSize;i++)
		{
			AwardRandSortData &rkSortData = vResultAward[i];
			rkSortData.m_iSortPoint = rand()%rkSortData.m_iSortPoint + 1;
		}
		std::sort( vResultAward.begin(), vResultAward.end(), AwardRandSort() );	
	}

	// 상위 4개를 정렬한다.
	int iCurSize = min( (int)vResultAward.size(), MAX_AWARDING );
	vAwardData vRandAward;
	for(i = 0;i < iCurSize;i++)
	{
		AwardRandSortData &rkSortData = vResultAward[i];
		vRandAward.push_back( rkSortData.m_pAwardData );
	}
	std::sort( vRandAward.begin(), vRandAward.end(), AwardDataSort() );

	rkPacket << iCurSize;
	for(i = 0;i  < iCurSize;i++)
	{
		AwardData *pAward = vRandAward[i];
		rkPacket << pAward->m_iType;
		const AwardUser &kUser = GetTopAwardUser( pAward );
		switch( pAward->m_iType )
		{		
		case AC_KING_GUARD:               // 초 단위로 변경 : 무조건 올림이다.
		case AC_THE_KING:
		case AC_GANGSI_SURVIVOR:
			{
				rkPacket << kUser.m_szName << ( ( kUser.m_iOriginalValue + 999 ) / 1000 );
			}
			break;
		default:
			{
				rkPacket << kUser.m_szName << kUser.m_iOriginalValue;
			}
			break;
		}
	}
	vRandAward.clear();
	vResultAward.clear();
}

void ioAwardManager::AwardSelectUserSplitCheck()
{
	AwardSelectMap::iterator iter = m_AwardSelectMap.find( ioPlayMode::GetModeType() );
	if( iter == m_AwardSelectMap.end() )
		return;

	if( iter->second == AWARD_SELECT_WIN )
		AwardSelectWinUserSplit();
	else if( iter->second == AWARD_SELECT_LOSE )
		AwardSelectLoseUserSplit();
}

void ioAwardManager::AwardSelectWinUserSplit()
{
	vAwardData::iterator iter = m_vAwardList.begin();
	for(;iter != m_vAwardList.end();++iter)
	{
		AwardData *pItem = *iter;

		for(int i = 0;i < (int)pItem->m_vUserList.size();)
		{
			AwardUser &rkUser = pItem->m_vUserList[i];			
			const FinalInfo &rkFinal = GetFindUserInfo( rkUser.m_szName );
			if( rkFinal.m_szName.IsEmpty() )
			{
				i++;
			}
			else
			{
				switch( ioPlayMode::GetModeType() )
				{
				case MT_SYMBOL:
				case MT_TOWER_DEFENSE:
				case MT_DARK_XMAS:
				case MT_FIRE_TEMPLE:
				case MT_CATCH:
				case MT_KING:
				case MT_TEAM_SURVIVAL:
				case MT_TEAM_SURVIVAL_AI:
				case MT_DOUBLE_CROWN:
				case MT_FOOTBALL:
				case MT_HEROMATCH:
				case MT_GANGSI:
				case MT_CATCH_RUNNINGMAN:
				case MT_FACTORY:
				case MT_RAID:
				case MT_UNDERWEAR:
				case MT_CBT:
				case MT_FLAG_CAPTURE:
				case MT_ARENA:
				case MT_FARMING:
					{
						// 패배 팀은 삭제
						if( !IsWinTeam( m_eWinTeam, rkFinal.m_Team ) )  
							pItem->m_vUserList.erase( pItem->m_vUserList.begin() + i );
						else
							i++;
					}
					break;
				case MT_SURVIVAL:				
				case MT_FIGHT_CLUB:
				case MT_BOSS:
					{
						// 기여도 100미만 삭제
						if( rkFinal.m_iContributePer < 100 )   
							pItem->m_vUserList.erase( pItem->m_vUserList.begin() + i );
						else
							i++;
					}
					break;
				case MT_SHUFFLE_BONUS:
					{
						//셔플은 모두유저 대상
						i++;
					}
					break;
				default:
					{
						i++;
					}
					break;
				}
			}
		}
	}
}

void ioAwardManager::AwardSelectLoseUserSplit()
{
	vAwardData::iterator iter = m_vAwardList.begin();
	for(;iter != m_vAwardList.end();++iter)
	{
		AwardData *pItem = *iter;

		for(int i = 0;i < (int)pItem->m_vUserList.size();)
		{
			AwardUser &rkUser = pItem->m_vUserList[i];			
			const FinalInfo &rkFinal = GetFindUserInfo( rkUser.m_szName );
			if( rkFinal.m_szName.IsEmpty() )
			{
				i++;
			}
			else
			{
				switch( ioPlayMode::GetModeType() )
				{
				case MT_SYMBOL:
				case MT_TOWER_DEFENSE:
				case MT_DARK_XMAS:
				case MT_FIRE_TEMPLE:
				case MT_CATCH:
				case MT_KING:
				case MT_TEAM_SURVIVAL:
				case MT_TEAM_SURVIVAL_AI:
				case MT_DOUBLE_CROWN:
				case MT_FOOTBALL:
				case MT_HEROMATCH:
				case MT_GANGSI:
				case MT_CATCH_RUNNINGMAN:
				case MT_FACTORY:
				case MT_RAID:
				case MT_UNDERWEAR:
				case MT_CBT:
				case MT_FLAG_CAPTURE:
				case MT_ARENA:
				case MT_FARMING:
					{
						// 승리 팀은 삭제
						if( IsWinTeam( m_eWinTeam, rkFinal.m_Team ) )  
							pItem->m_vUserList.erase( pItem->m_vUserList.begin() + i );
						else
							i++;
					}
					break;
				case MT_SURVIVAL:				
				case MT_FIGHT_CLUB:
				case MT_BOSS:				
					{
						// 기여도 100이상 삭제
						if( rkFinal.m_iContributePer >= 100 )   
							pItem->m_vUserList.erase( pItem->m_vUserList.begin() + i );
						else
							i++;
					}
					break;
				case MT_SHUFFLE_BONUS:
					{
						//셔플은 모두유저 대상
						i++;
					}
					break;
				default:
					{
						i++;
					}
					break;
				}
			}
		}
	}
}

const FinalInfo &ioAwardManager::GetFindUserInfo( const ioHashString &rkName )
{
	for(int i = 0;i < (int)m_FinalInfoList.size();i++)
	{
		if( m_FinalInfoList[i].m_szName == rkName )
			return m_FinalInfoList[i];
	}

	static FinalInfo kReturn;
	return kReturn;
}

void ioAwardManager::SetFinalResult( WinTeamType eWinTeam )
{
	m_eWinTeam = eWinTeam;
	m_FinalInfoList.clear();
}

void ioAwardManager::AddFinalInfo( const FinalInfo &rkInfo )
{
	m_FinalInfoList.push_back( rkInfo );
}
