

#ifndef _ioAlchemicMgr_h_
#define _ioAlchemicMgr_h_

#include "../Util/Singleton.h"

#include "Item.h"
#include "ioExtraItemInfoManager.h"

////////////////////////////////////////////////////////////////////////////////////////////////
/// ioAlchemicInventory에 있는 물품들을 실제로 사용할수 있게하는 장치들
////////////////////////////////////////////////////////////////////////////////////////////////

#define ALCHEMIC_ADDITIVE_DIV	100000

enum AlchemicType
{
	ALT_NONE,
	ALT_SOLDIER			= 1,	// 용병
	ALT_ITEM			= 2,	// 장비
	ALT_CHANGE			= 3,	// 다른걸로 변경
	ALT_EXCHANGE		= 4,	// 특정한 것으로 변경
	ALT_SELL			= 5,	// peso로 전환
	ALT_NEW_SOLDIER		= 6,	// 새로운 용병 조합
	ALT_NEW_ITEM		= 7,	// 새로운 장비 조합
};

enum AlchemicResultType
{
	ART_SUCCESS,		// 성공
	ART_FAIL,			// 실패
	ART_NOT_FIND_FUNC,	// 기능 code 없음
	ART_NOT_MACH_FUNC,	// code는 있지만 AlchemicType이 다름
	ART_NOT_MACH_VALUE,	// 갯수가 맞지 않음
	ART_NOT_EMPTY_SLOT,	// 빈칸없음
	ART_OVER_MAX_CNT,	// 최대갯수초과
	ART_NOT_ENOUGH_CNT,	// 갯수부족
	ART_EXCEPTION,		// 예외오류
	ART_NOT_MACH_CODE,	// 필요한 조각, 첨가물 정보가 다름
	ART_PERIOD_ERROR,	// 기간값 오류
	ART_TABLE_ERROR,	// alchemic table error
};

enum DisassembleType
{
	ADT_NONE		= 0,
	ADT_SOLDIER		= 1,	// 용병분해
	ADT_EXTRAITEM	= 2,	// 장비분해
};

enum 
{
	SOLDIER_PICE	= 1,
	ADDITION_AGENT	= 2,
	PESO			= 3
};

struct RecipeResultInfo
{
	int m_iPieceCode1;
	int m_iPieceCode2;
	int m_iPieceCode3;
	int m_iPieceCode4;
	int m_iAdditiveCode;

	float m_fSuccessRate;

	int m_iResultCode;
	
	RecipeResultInfo()
	{
		m_iPieceCode1 = 0;
		m_iPieceCode2 = 0;
		m_iPieceCode3 = 0;
		m_iPieceCode4 = 0;
		m_iAdditiveCode = 0;

		m_fSuccessRate = 0.0f;

		m_iResultCode = 0;
	}
};
typedef std::vector< RecipeResultInfo > vRecipeResultInfoList;

struct RandomResult
{
	int m_iResultCode;
	int m_iRandomRate;

	RandomResult()
	{
		m_iResultCode = 0;
		m_iRandomRate = 0;
	}
};
typedef std::vector< RandomResult > vRandomResultList;

struct RandomResultInfo
{
	vRandomResultList m_vList;
	DWORD m_dwTotalRate;

	RandomResultInfo()
	{
		m_vList.clear();
		m_dwTotalRate = 0;
	}
};

struct RandomReinforceInfo
{
	RandomReinforceList m_vList;
	DWORD m_dwTotalRate;

	RandomReinforceInfo()
	{
		m_vList.clear();
		m_dwTotalRate = 0;
	}
};

// new alchemic
struct NewAlchemicInfo
{
	int m_iPieceNum;
	int m_iAdditiveNum;
};

struct NewAlchemicPeriodInfo
{
	DWORD m_dwLimitCnt;
	DWORD m_dwTableNum;

	NewAlchemicPeriodInfo()
	{
		m_dwLimitCnt = 0;
		m_dwTableNum = 0;
	}
};
typedef std::vector< NewAlchemicPeriodInfo > vNewAlchemicPeriodInfoList;

struct RandomValue
{
	int m_iRandomRate;
	int m_iValue;

	RandomValue()
	{
		m_iRandomRate = 0;
		m_iValue = 0;
	}
};
typedef std::vector< RandomValue > vRandomValueList;

struct NewAlchemicPeriodTable
{
	vRandomValueList m_vList;
	DWORD m_dwTotalRate;

	NewAlchemicPeriodTable()
	{
		m_vList.clear();
		m_dwTotalRate = 0;
	}
};

struct AlchemicFuncInfo
{
	int m_iCode;
	bool m_bActive;
	AlchemicType m_AlchemicType;

	int m_iMaxCnt1;
	int m_iMaxCnt2;
	int m_iMaxCnt3;
	int m_iMaxCnt4;
	int m_iMaxAdditive;

	float m_fSuccessRate;
	int m_iPeriodValue;

	int m_iRecipeListNum;
	int m_iRandomListNum;
	int m_iReinforceNum;
	int m_iTradeTypeNum;

	int m_iNewAlchemicListTable;

	AlchemicFuncInfo()
	{
		Init();
	}

	void Init()
	{
		m_iCode = 0;
		m_bActive = false;
		m_AlchemicType = ALT_NONE;

		m_iMaxCnt1 = 0;
		m_iMaxCnt2 = 0;
		m_iMaxCnt3 = 0;
		m_iMaxCnt4 = 0;
		m_iMaxAdditive = 0;

		m_fSuccessRate = 0.0f;
		m_iPeriodValue = 0;

		m_iRecipeListNum = 0;
		m_iRandomListNum = 0;
		m_iReinforceNum = 0;
		m_iTradeTypeNum = 0;

		m_iNewAlchemicListTable = 0;
	}
};
typedef std::vector< AlchemicFuncInfo > vAlchemicFuncInfoList;

class PeriodInfoSort
{
public:
	bool operator()( const NewAlchemicPeriodInfo &lhs , const NewAlchemicPeriodInfo &rhs ) const
	{
		if( lhs.m_dwLimitCnt <= rhs.m_dwLimitCnt )
		{
			return true;
		}

		return false;
	}
};

class ioAlchemicMgr : public Singleton< ioAlchemicMgr >
{
protected:
	typedef std::map< int, vRecipeResultInfoList > RecipeResultInfoMap;
	typedef std::map< int, RandomResultInfo > RandomResultInfoMap;
	typedef std::map< int, RandomReinforceInfo > RandomReinforceInfoMap;
	typedef std::map< int, RandomTradeType > RandomTradeTypeInfoMap;
	typedef std::map< int, int > DisassembleInfoMap;
	typedef std::vector<int> SoulStoneGainRandom;

	RecipeResultInfoMap m_RecipeMap;
	RandomResultInfoMap m_RandomMap;
	RandomReinforceInfoMap m_ReinforceMap;
	RandomTradeTypeInfoMap m_TradeTypeMap;

	DisassembleInfoMap m_SoldierDisassembleMap;
	DisassembleInfoMap m_ExtraItemDisassembleMap;

	vAlchemicFuncInfoList m_AlchemicFuncInfoList;

	SoulStoneGainRandom m_vSoultoneRandomInfo;

	IORandom m_MainFailRandom;
	IORandom m_CountRandom;
	IORandom m_RecipeRandom;
	IORandom m_RandomRandom;
	IORandom m_ReinforceRandom;
	IORandom m_TradeTypeRandom;

	IORandom m_DisassembleRandom;

	IORandom m_NormalRandom;

// new alchemic
protected:
	typedef std::map< int, IntVec > AlcemicListMap;
	typedef std::map< int, NewAlchemicInfo > NewAlchemicInfoMap;
	typedef std::map< int, NewAlchemicPeriodTable > NewAlchemicPeriodTableMap;

	AlcemicListMap m_NewAlchemicListMap;

	NewAlchemicInfoMap m_NewAlchemicSoldierInfoMap;
	NewAlchemicInfoMap m_NewAlchemicItemInfoMap;

	NewAlchemicPeriodTableMap m_SoldierPeriodTabelMap;
	NewAlchemicPeriodTableMap m_ItemPeriodTabelMap;

	vNewAlchemicPeriodInfoList m_SoldierPeriodInfoList;
	vNewAlchemicPeriodInfoList m_ItemPeriodInfoList;

	IORandom m_PeriodRandom;

	int m_iSoldierAdditive;
	int m_iWeaponAdditive;
	int m_iArmorAdditive;
	int m_iHelmetAdditive;
	int m_iCloakAdditive;

	int m_iNewAlchemicMinTotalCnt;

protected:
	float m_fMinFailRate;
	float m_fMaxFailRate;

	float m_fAdditiveConstSoldier;
	float m_fAdditiveConstItem;
	float m_fAdditiveConstExchange;

	float m_fMinExchangeRate;
	float m_fMaxExchangeRate;

	int m_iMinTotalCnt;
	float m_fPieceChangeConstMin;
	float m_fPieceChangeConstMax;
	float m_fItemSellConst;

	int m_iTotalSoulStoneRandom;
protected:
	DWORD m_dwSoldierDisassembleConst;
	DWORD m_dwExtraItemDisassembleConst;

public:
	void CheckNeedReload();

	void LoadMgrInfo();

	void LoadRecipeInfo( ioINILoader &rkLoader );
	void LoadRandomInfo( ioINILoader &rkLoader );
	void LoadReinforceInfo( ioINILoader &rkLoader );
	void LoadTradeTypeInfo( ioINILoader &rkLoader );

	void LoadFuncInfo( ioINILoader &rkLoader );
	
	void LoadSoldierDisassembleInfo( ioINILoader &rkLoader );
	void LoadExtraItemDisassembleInfo( ioINILoader &rkLoader );

	void LoadSoulStoneInfo( ioINILoader &rkLoader );
	//
	void LoadNewAlchemicListTable( ioINILoader &rkLoader );
	void LoadNewAlchemicInfo( ioINILoader &rkLoader );
	void LoadNewAlchemicPeriodInfo( ioINILoader &rkLoader );

protected:
	void ClearAllInfo();

	bool FindAlchemicFunc( int iCode, AlchemicFuncInfo &rkFuncInfo );
	int CheckRecipeFunc( int iRecipeNum,
						 int iPiece1,
						 int iPiece2,
						 int iPiece3,
						 int iPiece4,
						 int iAdditive );

	int CheckRandomFunc( int iRandomNum );
	int CheckReinforceFunc( int iReinforceNum );
	int CheckTradeTypeFunc( int iTradeNum );

	float GetCurFailRandomRate();

public:
	// 용병 : return 값은 실패타입
	int AlchemicSoldierFunc( User *pUser,
							 int iCode,
							 int iPiece1, int iValue1,
							 int iPiece2, int iValue2,
							 int iPiece3, int iValue3,
							 int iPiece4, int iValue4,
							 int iAdditive, int iAddValue );

	// 장비 : return 값은 실패타입
	int AlchemicItemFunc( User *pUser,
						  int iCode,
						  int iPiece1, int iValue1,
						  int iPiece2, int iValue2,
						  int iPiece3, int iValue3,
						  int iPiece4, int iValue4,
						  int iAdditive, int iAddValue );

	// Change : 여러개를 다른걸로 : return 값은 실패타입
	int AlchemicChangeFunc( User *pUser,
							int iCode,
							int iPiece1, int iValue1,
							int iPiece2, int iValue2,
							int iPiece3, int iValue3,
							int iPiece4, int iValue4 );

	// Exchange : 하나를 지정된 다른걸로 : return 값은 실패타입
	// piece1은 재료, piece2는 목표. 단, piece2가 단 하나라도 소유하고 있어야한다
	int AlchemicExchangeFunc( User *pUser,
							  int iCode,
							  int iPiece1, int iValue1,
							  int iPiece2, int iValue2,
							  int iAdditive, int iAddValue );

	// Sell : item을 peso로 전환 : return 값은 실패타입
	int AlchemicSellFunc( User *pUser,
						  int iCode,
						  int iPiece1, int iValue1,
						  int iPiece2, int iValue2,
						  int iPiece3, int iValue3,
						  int iPiece4, int iValue4 );

	// new 용병 : return 값은 실패타입
	int NewAlchemicSoldierFunc( User *pUser,
								int iCode,
								int iClassType,
								int iPiece1, int iValue1,
								int iAdditive, int iAddValue );

	// new 장비 : return 값은 실패타입
	int NewAlchemicItemFunc( User *pUser,
							 int iCode,
							 int iItemCode,
							 int iPiece1, int iValue1,
							 int iAdditive, int iAddValue );

	int GetSoldierPeriodTime( int iPieceCnt );
	int GetItemPeriodTime( int iPieceCnt );

	bool CheckAlchemicTable( int iTableNum, int iValue );

public:
	int GetDisassembleCode( int iType, DWORD dwMagicCode );
	int GetDisassembleCnt( int iType, bool bMortmain, DWORD dwTime, DWORD dwMagicValue );

	int GetSoldierNeedPiece( int iClassType );

	int GetSouleStoneGainCnt();

public:
	inline float GetSellConst() const { return m_fItemSellConst; }

public:
	static ioAlchemicMgr& GetSingleton();

public:
	ioAlchemicMgr();
	virtual ~ioAlchemicMgr();
};

#define g_AlchemicMgr ioAlchemicMgr::GetSingleton()

#endif
