#pragma once

#define SPIRIT_RANDOM_MAX 100
class ioSpiritManager
{
public:
	enum SpecialSpiritType
	{
		SST_SPECIAL_SPIRIT = 1001,
	};

	enum DecomposeType
	{
		DECOMPOSE_SPIRIT = 1,
		DECOMPOSE_SPECIAL_SPIRIT = 2,
	};
//OTG#ifdef SRC_ID
	enum DecomposeMercenaryGrade
	{
		DECOMPOSE_NORMAL_MERCENARY = 0,
		DECOMPOSE_RARE_MERCENARY,
		DECOMPOSE_UNIQUE_MERCENARY,
		DECOMPOSE_PREMIUM_MERCENARY,
		DECOMPOSE_GRADE_MAX
	};
//OTG#endif
private:
	static ioSpiritManager *sg_Instance;

	struct SpiritInfo
	{
		int sell_peso;
		int compose_need_min;
		int compose_need_max;
		int decompose_quantity;
		int decompose_critical_value;
		int decompose_critical_quantity;
		int decompose_type;
		IntVec item_code_list;

		SpiritInfo()
		{
			Init();
		}
		void Init()
		{
			sell_peso = 0;;
			compose_need_min = 0;
			compose_need_max = 0;
			item_code_list.clear();
			decompose_type = DECOMPOSE_SPIRIT;
		}
	};
	typedef std::vector<SpiritInfo> vSpiritInfo;

	struct RewardInfo
	{
		int table_index;
		int random_value;
	};
	typedef std::vector<RewardInfo> vRewardInfo;

	struct ModeSpiritReward
	{
		ModeType mode_type;
		vRewardInfo reward;
	};
	typedef std::vector<ModeSpiritReward> vModeSpiritReward;

	vSpiritInfo m_vSpirit;
	vSpiritInfo m_vSpecialSpirit;
	vModeSpiritReward m_vReward;

	int m_iSpiritConversionInput;
	int m_iSpiritConversionOutput;
	int m_iSpiritConversionCriticialValue;
	int m_iSpiritConversionCriticalOutput;

	IORandom m_RewardRandom;
	int m_iModeRewardQuantity;

	int m_iExtraItemDecomposeQuantity;

	struct PresentInfo
	{
		int present_ment;
		int present_period;
	};

	PresentInfo m_ComposePrsent;
	PresentInfo m_SoulStonePrsent;
	PresentInfo m_PesoPrsent;
	PresentInfo m_PiecePrsent;

//OTG#ifdef SRC_ID		// 2018-05-08 용병 분해 소울스톤만 지급되도록 변경
	typedef std::map< int, int > MercenaryRankMap;
	MercenaryRankMap m_MercenaryRankMap;

	struct DismantlingItemInfo
	{
		int decompose_item_type[DECOMPOSE_GRADE_MAX];
		int decompose_item_group[DECOMPOSE_GRADE_MAX];
		int decompose_item_count[DECOMPOSE_GRADE_MAX];
	};

	DismantlingItemInfo m_DecomposeItemPresent;
//#endif

public:
	static ioSpiritManager &GetInstance();
	static void ReleaseInstance();

public:
	void LoadINI();

public:

	int GetComposePresentMent()		{ return m_ComposePrsent.present_ment; }
	int GetComposePresentPeriod()	{ return m_ComposePrsent.present_period; }
	int GetSoulStonePresentMent()	{ return m_SoulStonePrsent.present_ment; }
	int GetSoulStonePresentPeriod()	{ return m_SoulStonePrsent.present_period; }
	int GetPesoPresentMent()		{ return m_PesoPrsent.present_ment; }
	int GetPesoPresentPeriod()		{ return m_PesoPrsent.present_period; }
	int GetPiecePresentMent()		{ return m_PiecePrsent.present_ment; }
	int GetPiecePresentPeriod()		{ return m_PiecePrsent.present_period; }
#ifdef SRC_ID
	int GetDecomposeItemType(int iGrade)		{ return m_DecomposeItemPresent.decompose_item_type[iGrade]; }
	int GetDecomposeItemGroup(int iGrade)		{ return m_DecomposeItemPresent.decompose_item_group[iGrade]; }
	int GetDecomposeItemCount(int iGrade)		{ return m_DecomposeItemPresent.decompose_item_count[iGrade]; }
#endif

public:
	// 정기 전환
	int GetSpiritConversionInput()	{ return m_iSpiritConversionInput; }
	int GetSpiritConversionOutput()	{ return m_iSpiritConversionOutput; }
	int GetSpiritConversionCriticalValue()	{ return m_iSpiritConversionCriticialValue; }
	int GetSpiritConversionCriticalOutput()	{ return m_iSpiritConversionCriticalOutput; }

public:
	int GetSellPesoByGroup( int iGroup );
	int GetSellPesoByItemCode( int iItemCode );
	int GetComposeNeedMinSpirit( int iItemCode );
	int GetComposeNeedMaxSpirit( int iItemCode );
	int GetDecomposeType( int iItemCode );
	int GetDecomposeQuantity( int iItemCode );
	int GetDecomposeCriticalValue( int iItemCode );
	int GetDecomposeCriticalQuantity( int iItemCode );

public:
	int GetExtraItemDecomposeQuantity() { return m_iExtraItemDecomposeQuantity; }

public:
	bool EnableSpirit( int iItemCode, int iTargetCode );

public:
	// 모드보상
	int GetModeReward( ModeType eType );
	int GetModeRewardQuantity() { return m_iModeRewardQuantity; }

//OTG#ifdef SRC_ID
	int CheckMercenaryRank(int iClassType);
	int GetItemGainCntByMercenaryRank(int iClassType);
	int GetItemTypeByMercenaryRank(int iClassType);
//#endif

private:
	bool GetSpiritTable( IN int iTableIndex, IntVec &SpiritTable );

private:
	ioSpiritManager();
	virtual ~ioSpiritManager();
};

#define g_SpiritManager ioSpiritManager::GetInstance()