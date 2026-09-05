
#include "stdafx.h"

#include "ioGrowthInfoManager.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"
#include "ioGrowthUpInfo.h"

template<> ioGrowthInfoManager* Singleton< ioGrowthInfoManager >::ms_Singleton = 0;

ioGrowthInfoManager::ioGrowthInfoManager()
{
	m_iGrowthInitPeso = 0;

	m_fDiscountRate = 0.0f;
	m_fReturnRate    = 0.0f;
	m_fReturnAllRate = 0.0f;

	m_iGetGrowthPoint = 0;
	m_iCharGrowthUpPoint = 0;
	m_iItemGrowthUpPoint = 0;
	m_iWomanTotalGrowthPoint = 0;

	//m_nRebalanceNormalMaxLv = 3000;

	ClearGrowthInfoList();
}

ioGrowthInfoManager::~ioGrowthInfoManager()
{
	ClearGrowthInfoList();

	m_vGrowthTypeInfoList.clear();
	m_vReinforceAddStatList.clear();
}

ioGrowthInfoManager& ioGrowthInfoManager::GetSingleton()
{
	return Singleton< ioGrowthInfoManager >::GetSingleton();
}

void ioGrowthInfoManager::ClearGrowthInfoList()
{
	GrowthInfoList::iterator iter;
	for( iter=m_vGrowthInfoList.begin() ; iter!=m_vGrowthInfoList.end() ; ++iter )
	{
		delete *iter;
	}
	m_vGrowthInfoList.clear();
	m_vReinforceAddStatList.clear();
}

void ioGrowthInfoManager::LoadGrowthInfoList()
{
	ClearGrowthInfoList();

	ioINILoader_e kLoader( "config/sp2_growth_info.ini" );

	char szGroupName[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";
	char szBuf[MAX_PATH] = "";

	int i = 0;

	kLoader.SetTitle_e( "common_info" );

	m_iGrowthInitPeso = (__int64)kLoader.LoadInt_e( "growth_init_peso", 0 );

	m_fDiscountRate = kLoader.LoadFloat_e( "discount_rate", 0.0f );
	m_fDiscountRate = min( m_fDiscountRate, FLOAT1 );

	m_fReturnRate = kLoader.LoadFloat_e( "return_rate", 0.0f );
	m_fReturnRate = min( m_fReturnRate, FLOAT1 );

	m_fReturnAllRate = kLoader.LoadFloat_e( "return_all_rate", 0.0f );
	m_fReturnAllRate = min( m_fReturnAllRate, FLOAT1 );

	m_iMaxLevel = kLoader.LoadInt_e( "max_level", 100 );

	m_iCharNeedPesoA = kLoader.LoadInt_e( "char_const_a", 0 );
	m_iCharNeedPesoB = kLoader.LoadInt_e( "char_const_b", 0 );
	m_iItemNeedPesoA = kLoader.LoadInt_e( "item_const_a", 0 );
	m_iItemNeedPesoB = kLoader.LoadInt_e( "item_const_b", 0 );

	m_fTimeGrowthConstA = kLoader.LoadFloat_e( "time_growth_const_a", 0.0f );
	m_fTimeGrowthConstB = kLoader.LoadFloat_e( "time_growth_const_b", 0.0f );
	m_fTimeGrowthConstC = kLoader.LoadFloat_e( "time_growth_const_c", 0.0f );

	m_fConfirmConst = kLoader.LoadFloat_e( "confirm_const", FLOAT1 );

	m_iTimeGrowthLimitLevel = kLoader.LoadInt_e( "time_growth_limit_level", 0 );
	m_iTimeGrowthEnableCharCnt = kLoader.LoadInt_e( "time_growth_enable_cnt", 0 );
	m_iTimeGrowthEnableTotalCnt = kLoader.LoadInt_e( "time_growth_enable_total_cnt", 0 );

	m_iGetGrowthPoint = kLoader.LoadInt_e( "get_growth_point", 0 );
	m_iCharGrowthUpPoint = kLoader.LoadInt_e( "char_growth_up_point", 0 );
	m_iItemGrowthUpPoint = kLoader.LoadInt_e( "item_growth_up_point", 0 );
	m_iWomanTotalGrowthPoint = kLoader.LoadInt_e( "woman_total_growth_point", 0 );

	kLoader.LoadString_e( "zero_level_desc", "", szBuf, MAX_PATH );
	m_ZeroLevelDesc = szBuf;

	//////////////////////////////////////////////////////////////////////////
	m_bUseNextLevelUpCheck = kLoader.LoadBool_e( "use_levelup_next_level", true );
	//////////////////////////////////////////////////////////////////////////

	int iGroupCnt = kLoader.LoadInt_e( "growth_group_cnt", 0 );
	for( i=0 ; i<iGroupCnt ; i++ )
	{
		// 그룹 호출
		wsprintf_e( szTitle, "growth_group%d", i );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		wsprintf_e( szGroupName, "config/%s", szBuf );

		ioINILoader kGrowthLoader( szGroupName );

		kGrowthLoader.SetTitle_e( "common" );
		int iGrowthInfoCnt = kGrowthLoader.LoadInt_e( "growth_info_cnt", 0 );

		for( int j=0; j < iGrowthInfoCnt; ++j )
		{
			// 그룹에 속한 성장정보 추가
			wsprintf_e( szTitle, "growth_info_%d", j+1 );
			kGrowthLoader.SetTitle( szTitle );

			ioGrowthInfo *pGrowthInfo = new ioGrowthInfo;
			if( pGrowthInfo )
			{
				pGrowthInfo->LoadProperty( kGrowthLoader );

				m_vGrowthInfoList.push_back( pGrowthInfo );
			}
		}
	}

	// GrowthType Desc
	kLoader.SetTitle_e( "growth_type_desc" );

	m_vGrowthTypeInfoList.clear();

	int iTypeSize = kLoader.LoadInt_e( "max_cnt", 0 );
	for( i=0; i < iTypeSize; ++i )
	{
		GrowthTypeInfo kTypeInfo;

		wsprintf_e( szTitle, "growth_type%d", i+1 );
		kTypeInfo.m_GrowthType = (GrowthType)kLoader.LoadInt( szTitle, 0 );

		wsprintf_e( szTitle, "growth_desc%d", i+1 );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		kTypeInfo.m_TypeDesc = szBuf;

		m_vGrowthTypeInfoList.push_back( kTypeInfo );
	}

	//강화도에 따른 추가 스탯
	kLoader.SetTitle_e( "reinforce_add_stat" );
	m_vReinforceAddStatList.clear();

	int nAddStatSize = kLoader.LoadInt_e( "max_add_stat_cnt", 0 );
	for( i=0; i < nAddStatSize; ++i )
	{
		ReinforceAddStat sAddStatInfo;

		wsprintf_e( szBuf, "reinforce_level%d", i+1 );
		sAddStatInfo.m_nReinforceLv = (int)kLoader.LoadInt( szBuf, 0 );

		wsprintf_e( szBuf, "add_stat%d", i+1 );
		sAddStatInfo.m_nAddStat = (int)kLoader.LoadInt( szBuf, 0 );

		m_vReinforceAddStatList.push_back( sAddStatInfo );
	}
}

/*int ioGrowthInfoManager::GetRebalanceHightLevelValue( int nLevel )
{
	if( nLevel <= m_nRebalanceNormalMaxLv )
		return 0;

	if( (int)m_vRebalanceHightLevelValue.size() < (nLevel-m_nRebalanceNormalMaxLv) )
		return 0;

	return m_vRebalanceHightLevelValue[ nLevel-m_nRebalanceNormalMaxLv-1 ];
}*/

ioHashString ioGrowthInfoManager::GetGrowthTypeDesc( int iGrowthType )
{
	for( int i=0; i < (int)m_vGrowthTypeInfoList.size(); ++i )
	{
		if( m_vGrowthTypeInfoList[i].m_GrowthType == iGrowthType )
		{
			return m_vGrowthTypeInfoList[i].m_TypeDesc;
		}
	}

	return ioHashString();
}

ioGrowthInfo* ioGrowthInfoManager::GetGrowthInfo( int iInfoNum )
{
	if( iInfoNum == 0 )
		return NULL;

	int iMaxCnt = m_vGrowthInfoList.size();
	for( int i=0; i < iMaxCnt; ++i )
	{
		if( m_vGrowthInfoList[i]->GetGrowthInfoNum() == iInfoNum )
			return m_vGrowthInfoList[i];
	}

	return NULL;
}

ioGrowthUpInfo* ioGrowthInfoManager::GetGrowthUpInfo( int iInfoNum, int iLevel )
{
	ioGrowthInfo *pInfo = GetGrowthInfo( iInfoNum );
	if( pInfo )
	{
		return pInfo->GetGrowthUpInfo( iLevel );
	}

	return NULL;
}

bool ioGrowthInfoManager::CheckGrowthInfoType( int iInfoNum, int iType )
{
	ioGrowthInfo *pInfo = GetGrowthInfo( iInfoNum );
	if( pInfo )
	{
		return pInfo->CheckGrowthInfoType(iType);
	}

	return false;
}

int ioGrowthInfoManager::GetGrowthUpNeedPoint( bool bChar )
{
	if( bChar )
		return m_iCharGrowthUpPoint;

	return m_iItemGrowthUpPoint;
}

__int64 ioGrowthInfoManager::GetGrowthUpNeedPeso( bool bChar, int iLevel )
{
	__int64 iNeedPeso = 0;
	if( bChar )
		iNeedPeso = ( m_iCharNeedPesoA * iLevel * iLevel ) + ( m_iCharNeedPesoB * iLevel );
	else
		iNeedPeso = ( m_iItemNeedPesoA * iLevel * iLevel ) + ( m_iItemNeedPesoB * iLevel );

	float fRate = FLOAT1 - m_fDiscountRate;
	iNeedPeso = (__int64)(iNeedPeso * fRate);

	return iNeedPeso;
}

__int64 ioGrowthInfoManager::GetGrowthReturnPeso( bool bChar, int iLevel )
{
	__int64 iNeedPeso = GetGrowthUpNeedPeso( bChar, iLevel );
	__int64 iReturnPeso = (__int64)(iNeedPeso * m_fReturnRate);

	return iReturnPeso;
}

__int64 ioGrowthInfoManager::GetGrowthReturnAllPeso( bool bChar, int iLevel )
{
	__int64 iNeedPeso = GetGrowthUpNeedPeso( bChar, iLevel );
	__int64 iReturnPeso = (__int64)(iNeedPeso * m_fReturnAllRate);

	return iReturnPeso;
}

DWORD ioGrowthInfoManager::GetTimeGrowthNeedTime( bool bChar, int iLevel )
{
	DWORD dwTotalTime = 0;
	dwTotalTime = (DWORD)((m_fTimeGrowthConstA * pow((long double)iLevel, 2)) + (m_fTimeGrowthConstB * iLevel) + (m_fTimeGrowthConstC * pow((long double)iLevel, 3)));

	if( bChar )
		dwTotalTime *= 2;

	return dwTotalTime;
}

DWORD ioGrowthInfoManager::GetTimeGrowthConfirmTime( bool bChar, int iLevel )
{
	DWORD dwTotalTime = 0;
	dwTotalTime = (DWORD)((m_fTimeGrowthConstA * pow((long double)iLevel, 2)) + (m_fTimeGrowthConstB * iLevel) + (m_fTimeGrowthConstC * pow((long double)iLevel, 3)));

	if( bChar )
		dwTotalTime *= 2;

	DWORD dwConfirmTime = (DWORD)(dwTotalTime * m_fConfirmConst);

	return dwConfirmTime;
}

int ioGrowthInfoManager::GetMaxLevel()
{
	return m_iMaxLevel;
}

int ioGrowthInfoManager::GetTotalGrowthPoint( int iLevel )
{
	int iTotal = iLevel * m_iGetGrowthPoint;

	return iTotal;
}

//강화레벨에 따른 추가 스탯 얻기
int ioGrowthInfoManager::GetReinforceAddStat( int nReinforce )
{
	if ( nReinforce <= 0 || m_vReinforceAddStatList.empty() )
		return 0;

	int nStatInfoSize = (int)m_vReinforceAddStatList.size();
	for ( int i=0; i<nStatInfoSize; ++i )
	{
		if ( m_vReinforceAddStatList[i].m_nReinforceLv == nReinforce )
			return m_vReinforceAddStatList[i].m_nAddStat;
	}
	return 0;
}

//------------------------------------------------------------------------------------------
ioGrowthInfo::ioGrowthInfo()
{
	m_pGrowthUpInfo = NULL;
}

ioGrowthInfo::~ioGrowthInfo()
{
	SAFEDELETE(m_pGrowthUpInfo);

	ClearGrowthUpInfoList();
}

void ioGrowthInfo::ClearGrowthUpInfoList()
{
	for( int i=0; i < MAX_DESC_LINE; ++i )
	{
		m_MainDesc[i].ClearList();
	}
}

void ioGrowthInfo::LoadProperty( ioINILoader &rkLoader )
{
	ClearGrowthUpInfoList();

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_iGrowthInfoNum = rkLoader.LoadInt_e( "growth_info_num", 0 );
	m_GrowthType = (GrowthType)rkLoader.LoadInt_e( "type", 0 );

	rkLoader.LoadString_e( "name", "", szBuf, MAX_PATH );
	m_GrowthName = szBuf;

	// MainDesc
	int i, j;
	for( i=0; i < MAX_DESC_LINE; ++i )
	{
		m_MainDesc[i].SetTextStyle( TS_OUTLINE_FULL_2X );
		m_MainDesc[i].SetBkColor( 0, 0, 0 );

		for( j=0; j < MAX_DESC_COLOR; ++j )
		{
			wsprintf_e( szKey, "desc%d_color%d", i+1, j+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			char *pTemp = NULL;
			DWORD dwColor = strtoul( szBuf, &pTemp, 16 );
			m_MainDesc[i].SetTextColor( dwColor );

			wsprintf_e( szKey, "desc%d_text%d", i+1, j+1 );
			rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );

			if( strcmp( szBuf, "" ) == 0 )
				continue;

			Help::ChangeCharSet( szBuf, '#', ' ' );
			m_MainDesc[i].AddTextPiece( FONT_SIZE_13, szBuf );
		}
	}

	ioGrowthUpInfo* pInfo = CreateGrowthUpInfo();
	if( pInfo )
	{
		pInfo->LoadProperty( rkLoader );
		pInfo->SetType( m_GrowthType );

		m_pGrowthUpInfo = pInfo;
	}
}

ioGrowthUpInfo* ioGrowthInfo::CreateGrowthUpInfo()
{
	switch( m_GrowthType )
	{
	case GT_CHAR_ATTACK:
	case GT_SKILL_ATTACK:
		return new ioGrowthAttackUpInfo();
	case GT_BLOW_PUSH:
		return new ioGrowthBlowPushUpInfo();
	case GT_TELEPORT_ANI_RATE:
		return new ioGrowthTeleportUpInfo();
	case GT_SKILL_ATTACK_FORCE:
		return new ioGrowthAttackForceUpInfo();
	default:
		return new ioGrowthNormalUpInfo();
	}

	return NULL;
}

ioGrowthUpInfo* ioGrowthInfo::GetGrowthUpInfo( int iLevel )
{
	if( m_pGrowthUpInfo )
		m_pGrowthUpInfo->SetCurLevel( iLevel );
	
	return m_pGrowthUpInfo;
}

bool ioGrowthInfo::CheckGrowthInfoType( int iType ) const
{
	if( m_GrowthType == iType )
		return true;

	return false;
}

/*void ioGrowthInfo::SetItemReinForceLv( int nLv )
{
	if( m_pGrowthUpInfo )
		m_pGrowthUpInfo->SetItemReinForceLv( nLv );
}*/
