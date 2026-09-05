#include "stdafx.h"
#include "ioSpiritManager.h"

ioSpiritManager *ioSpiritManager::sg_Instance = NULL;
ioSpiritManager::ioSpiritManager()
{
	m_RewardRandom.SetRandomSeed( timeGetTime() );
}

ioSpiritManager::~ioSpiritManager()
{

}

ioSpiritManager &ioSpiritManager::GetInstance()
{
	if( !sg_Instance )
		sg_Instance = new ioSpiritManager;

	return *sg_Instance;
}

void ioSpiritManager::ReleaseInstance()
{
	SAFEDELETE( sg_Instance );
}

void ioSpiritManager::LoadINI()
{
	m_vSpirit.clear();

	ioINILoader kLoader;
	kLoader.ReloadFile( "config/sp2_spirit_info.ini" );

	kLoader.SetTitle( "extra_item" );
	m_iExtraItemDecomposeQuantity = kLoader.LoadInt( "extraitem_decompose", 0 );

	kLoader.SetTitle( "special_spirit" );
	int iTableCnt = kLoader.LoadInt( "spirit_count", 0 );
	SpiritInfo cSpecialinfo;
	cSpecialinfo.Init();
	for( int i=0; i<iTableCnt; ++i )
	{
		cSpecialinfo.sell_peso = kLoader.LoadInt( "spirit_sell_peso", 0 );

		char szKey[MAX_PATH] = "";
		wsprintf( szKey, "spirit_code%d", i+1 );
		int iCode = kLoader.LoadInt( szKey, 0 );
		if( iCode == 0 )
			continue;
		cSpecialinfo.item_code_list.push_back( iCode );
	}
	m_vSpecialSpirit.push_back( cSpecialinfo );

	kLoader.SetTitle( "common" );
	int iCnt = kLoader.LoadInt( "spirit_table_count", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		char szTitle[MAX_PATH] = "";
		wsprintf( szTitle, "spirit_table%d", i+1 );
		kLoader.SetTitle( szTitle );
		SpiritInfo info;
		info.Init();

		info.sell_peso = kLoader.LoadInt( "spirit_sell_peso", 0 );
		info.compose_need_min = kLoader.LoadInt( "spirit_compose_need_min", 0 );
		info.compose_need_max = kLoader.LoadInt( "spirit_compose_need_max", 0 );
		info.decompose_quantity = kLoader.LoadInt( "spirit_decompose_quantity", 0 );
		float fRate = kLoader.LoadFloat( "spirit_decompose_critical_rate", 0 );
		fRate *= 0.01;
		info.decompose_critical_value = SPIRIT_RANDOM_MAX * fRate;
		info.decompose_critical_quantity = kLoader.LoadInt( "spirit_decompose_critical_quantity", 0 );
		info.decompose_type = kLoader.LoadInt( "spirit_decompose_type", DECOMPOSE_SPIRIT );
		

		int iTableCnt = kLoader.LoadInt( "spirit_count", 0 );
		for( int j=0; j<iTableCnt; ++j )
		{
			char szKey[MAX_PATH] = "";
			wsprintf( szKey, "spirit_code%d", j+1 );
			int iCode = kLoader.LoadInt( szKey, 0 );
			if( iCode == 0 )
				continue;
			info.item_code_list.push_back( iCode );
		}

		m_vSpirit.push_back( info );
	}

	kLoader.SetTitle( "mode_reward" );
	m_iModeRewardQuantity = kLoader.LoadInt( "mode_reward_quantity", 1 );
	iCnt = kLoader.LoadInt( "mode_count", 0 );
	for( int i=0; i<iCnt; ++i )
	{
		char szKey[MAX_PATH] = "";
		ModeSpiritReward reward;

		wsprintf( szKey, "mode%d_type", i+1 );
		reward.mode_type = (ModeType)kLoader.LoadInt( szKey, 0 );
		if( reward.mode_type == MT_NONE )
			continue;

		wsprintf( szKey, "mode%d_reward_count", i+1 );
		int iRewardCnt = kLoader.LoadInt( szKey, 0 );
		int iTemp = 0;
		for( int j=0; j<iRewardCnt; ++j )
		{
			RewardInfo info;
			wsprintf( szKey, "mode%d_reward_list%d", i+1, j+1 );
			info.table_index = kLoader.LoadInt( szKey, 0 );

			wsprintf( szKey, "mode%d_reward_rand%d", i+1, j+1 );
			float fRate = kLoader.LoadFloat( szKey, 0 );
			fRate *= 0.01;
			info.random_value = iTemp + ( SPIRIT_RANDOM_MAX * fRate );
			iTemp = info.random_value;

			reward.reward.push_back( info );
		}

		m_vReward.push_back( reward );
	}

	kLoader.SetTitle( "conversion_info" );
	m_iSpiritConversionInput = kLoader.LoadInt( "spirit_conversion_input", 0 );
	m_iSpiritConversionOutput = kLoader.LoadInt( "spirit_conversion_output", 0 );
	float fRate = kLoader.LoadFloat( "spirit_conversion_critical_rate", 0 );
	fRate *= 0.01;
	m_iSpiritConversionCriticialValue = SPIRIT_RANDOM_MAX * fRate;
	m_iSpiritConversionCriticalOutput = kLoader.LoadInt( "spirit_conversion_critical_output", 0 );

	kLoader.SetTitle( "present_info" );
#ifdef SRC_OVERSEAS
	m_szSendID = DEV_K_NAME;
#else
#endif


	m_ComposePrsent.present_ment = kLoader.LoadInt( "compose_present_ment", 0 );
	m_ComposePrsent.present_period = kLoader.LoadInt( "compose_present_period", 0 );

	m_SoulStonePrsent.present_ment = kLoader.LoadInt( "soulstone_present_ment", 0 );
	m_SoulStonePrsent.present_period = kLoader.LoadInt( "soulstone_present_period", 0 );

	m_PesoPrsent.present_ment = kLoader.LoadInt( "peso_present_ment", 0 );
	m_PesoPrsent.present_period = kLoader.LoadInt( "peso_present_period", 0 );

	m_PiecePrsent.present_ment = kLoader.LoadInt( "piece_present_ment", 0 );
	m_PiecePrsent.present_period = kLoader.LoadInt( "piece_present_period", 0 );

#ifdef SRC_ID
	kLoader.SetTitle( "after_dismantling_item" );

	m_DecomposeItemPresent.decompose_item_type[DECOMPOSE_PREMIUM_MERCENARY] = kLoader.LoadInt( "premium_disassembly_item_type", 0 );
	m_DecomposeItemPresent.decompose_item_group[DECOMPOSE_PREMIUM_MERCENARY] = kLoader.LoadInt( "premium_disassembly_item_group", 0 );
	m_DecomposeItemPresent.decompose_item_count[DECOMPOSE_PREMIUM_MERCENARY] = kLoader.LoadInt( "premium_disassembly_item_count", 0 );

	m_DecomposeItemPresent.decompose_item_type[DECOMPOSE_UNIQUE_MERCENARY] = kLoader.LoadInt( "unique_disassembly_item_type", 0 );
	m_DecomposeItemPresent.decompose_item_group[DECOMPOSE_UNIQUE_MERCENARY] = kLoader.LoadInt( "unique_disassembly_item_group", 0 );
	m_DecomposeItemPresent.decompose_item_count[DECOMPOSE_UNIQUE_MERCENARY] = kLoader.LoadInt( "unique_disassembly_item_count", 0 );

	m_DecomposeItemPresent.decompose_item_type[DECOMPOSE_RARE_MERCENARY] = kLoader.LoadInt( "rare_disassembly_item_type", 0 );
	m_DecomposeItemPresent.decompose_item_group[DECOMPOSE_RARE_MERCENARY] = kLoader.LoadInt( "rare_disassembly_item_group", 0 );
	m_DecomposeItemPresent.decompose_item_count[DECOMPOSE_RARE_MERCENARY] = kLoader.LoadInt( "rare_disassembly_item_count", 0 );

	m_DecomposeItemPresent.decompose_item_type[DECOMPOSE_NORMAL_MERCENARY] = kLoader.LoadInt( "normal_disassembly_item_type", 0 );
	m_DecomposeItemPresent.decompose_item_group[DECOMPOSE_NORMAL_MERCENARY] = kLoader.LoadInt( "normal_disassembly_item_group", 0 );
	m_DecomposeItemPresent.decompose_item_count[DECOMPOSE_NORMAL_MERCENARY] = kLoader.LoadInt( "normal_disassembly_item_count", 0 );

	kLoader.SetTitle( "mercenary_rank" );

	iTableCnt = kLoader.LoadInt( "table_cnt", 0 ); // 랭크 관련 테이블은 3개(일반, 레어, 프리미엄)
	int iMercenaryCnt = 0;  // 랭크 별 용병 갯수

	for (int i = 0; i < iTableCnt; i++)
	{
		char szKey[MAX_PATH] = "";
		wsprintf( szKey, "disassembly_table%d_list_cnt", i + 1 );
		iMercenaryCnt = kLoader.LoadInt( szKey, 0 );
		for (int j = 0; j < iMercenaryCnt; j++)
		{
			int iMercenaryidx = 0;
			wsprintf( szKey, "disassembly_table%d_value%d", i + 1, j + 1 );
			iMercenaryidx = kLoader.LoadInt( szKey, 0 );
			m_MercenaryRankMap.insert( MercenaryRankMap::value_type(iMercenaryidx, i));
		}
	}
#endif
}

int ioSpiritManager::GetSellPesoByGroup( int iGroup )
{
	int iGroupArray = m_vSpirit.size();
	if( !COMPARE( iGroup, 0, iGroupArray ) )
		return -1;

	return m_vSpirit[iGroup].sell_peso;
}

int ioSpiritManager::GetSellPesoByItemCode( int iItemCode )
{
	for each( SpiritInfo info in m_vSpirit )
	{
		IntVec::iterator iter = std::find( info.item_code_list.begin(), info.item_code_list.end(), iItemCode );
		if( iter != info.item_code_list.end() )
		{
			return info.sell_peso;
		}
	}

	for each( SpiritInfo info in m_vSpecialSpirit )
	{
		IntVec::iterator iter = std::find( info.item_code_list.begin(), info.item_code_list.end(), iItemCode );
		if( iter != info.item_code_list.end() )
		{
			return info.sell_peso;
		}
	}

	return -1;
}

int ioSpiritManager::GetComposeNeedMinSpirit( int iItemCode )
{
	for each( SpiritInfo info in m_vSpirit )
	{
		IntVec::iterator iter = std::find( info.item_code_list.begin(), info.item_code_list.end(), iItemCode );
		if( iter != info.item_code_list.end() )
		{
			return info.compose_need_min;
		}
	}

	return -1;
}

int ioSpiritManager::GetComposeNeedMaxSpirit( int iItemCode )
{
	for each( SpiritInfo info in m_vSpirit )
	{
		IntVec::iterator iter = std::find( info.item_code_list.begin(), info.item_code_list.end(), iItemCode );
		if( iter != info.item_code_list.end() )
		{
			return info.compose_need_max;
		}
	}

	return -1;
}

int ioSpiritManager::GetDecomposeType( int iItemCode )
{
	for each( SpiritInfo info in m_vSpirit )
	{
		IntVec::iterator iter = std::find( info.item_code_list.begin(), info.item_code_list.end(), iItemCode );
		if( iter != info.item_code_list.end() )
		{
			return info.decompose_type;
		}
	}

	return -1;
}

int ioSpiritManager::GetDecomposeQuantity( int iItemCode )
{
	for each( SpiritInfo info in m_vSpirit )
	{
		IntVec::iterator iter = std::find( info.item_code_list.begin(), info.item_code_list.end(), iItemCode );
		if( iter != info.item_code_list.end() )
		{
			return info.decompose_quantity;
		}
	}

	return -1;
}

int ioSpiritManager::GetDecomposeCriticalValue( int iItemCode )
{
	for each( SpiritInfo info in m_vSpirit )
	{
		IntVec::iterator iter = std::find( info.item_code_list.begin(), info.item_code_list.end(), iItemCode );
		if( iter != info.item_code_list.end() )
		{
			return info.decompose_critical_value;
		}
	}

	return -1;
}

int ioSpiritManager::GetDecomposeCriticalQuantity( int iItemCode )
{
	for each( SpiritInfo info in m_vSpirit )
	{
		IntVec::iterator iter = std::find( info.item_code_list.begin(), info.item_code_list.end(), iItemCode );
		if( iter != info.item_code_list.end() )
		{
			return info.decompose_critical_quantity;
		}
	}

	return -1;
}

int ioSpiritManager::GetModeReward( ModeType eType )
{
	auto find_reward = [eType]( ModeSpiritReward info )->bool { return info.mode_type == eType; };
	vModeSpiritReward::iterator iter =  std::find_if( m_vReward.begin(), m_vReward.end(), find_reward );
	if( iter != m_vReward.end() )
	{
		int iCheck = 0;
		int iValue = m_RewardRandom.Random( SPIRIT_RANDOM_MAX );

		vRewardInfo kRewardList = (*iter).reward;
		for each( RewardInfo info in kRewardList )
		{
			if( COMPARE( iValue, iCheck, info.random_value )  )
			{
				IntVec kSpiritList;
				if( GetSpiritTable( info.table_index, kSpiritList ) )
				{
					io_random_shuffle( kSpiritList.begin(), kSpiritList.end() );
					if( kSpiritList.empty() )
					{
						return -1;
					}
					else
					{
						return kSpiritList[0];
					}
				}
				else
				{
					return -1;
				}
			}
			else
			{
				iCheck = info.random_value;
			}
		}
	}

	return -1;
}
//OTG#ifdef SRC_ID
int ioSpiritManager::CheckMercenaryRank(int iClassType)
{
	MercenaryRankMap::iterator iter = m_MercenaryRankMap.find(iClassType);

	if( iter == m_MercenaryRankMap.end() )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "%s [ %d Cannot Find MercenaryRank ].",  __FUNCTION__, iClassType );
		return -1;
	}

	return iter->second;
}

int ioSpiritManager::GetItemGainCntByMercenaryRank(int iClassType)
{
	int iMercenaryRank = CheckMercenaryRank(iClassType);

	if(iMercenaryRank < 0)
		return 0;

	return m_DecomposeItemPresent.decompose_item_count[iMercenaryRank];
}

int ioSpiritManager::GetItemTypeByMercenaryRank(int iClassType)
{
	int iMercenaryRank = CheckMercenaryRank(iClassType);

	if(iMercenaryRank < 0)
		return 0;

	return m_DecomposeItemPresent.decompose_item_type[iMercenaryRank];
}
//#endif
bool ioSpiritManager::GetSpiritTable( IN int iTableIndex, IntVec &SpiritTable )
{
	int iArray = iTableIndex - 1;
	if( COMPARE( iArray, 0, (int)m_vSpirit.size() ) )
	{
		if( m_vSpirit[iArray].item_code_list.empty() )
			return false;

		SpiritTable.clear();
		SpiritTable = m_vSpirit[iArray].item_code_list;
		return true;
	}

	return false;
}

bool ioSpiritManager::EnableSpirit( int iItemCode, int iTargetCode )
{
	for each( SpiritInfo info in m_vSpirit )
	{
		IntVec::iterator iter = std::find( info.item_code_list.begin(), info.item_code_list.end(), iItemCode );
		IntVec::iterator target_iter = std::find( info.item_code_list.begin(), info.item_code_list.end(), iTargetCode );
		if( iter != info.item_code_list.end() && target_iter != info.item_code_list.end() )
		{
			return true;
		}
	}
	return false;
}