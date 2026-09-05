
#pragma once

#define COMPARE(x,min,max) (((x)>=(min))&&((x)<(max)))
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
///                        기타관련                      ///
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

class CEtcValue
{
public:
	int iValue;		//  갯수
	int iPeso;		// 돈
	int iCash;		// 캐쉬
	int iActive;		// 실행여부
};


class CItemEtc
{
public:
	CString strName;	// 이름
	int		iType;		// type

	std::vector<CEtcValue*> EtcValueList;
};

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
///                        치장관련                      ///
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


typedef std::vector<DWORD> DWORDVec;
	typedef struct tagSoldierInfo
	{
		int m_iLimitSeconds;
		int m_iGoodsNoValue;

		tagSoldierInfo()
		{
			m_iLimitSeconds = 0;
			m_iGoodsNoValue = 0;
		}
	}SoldierInfo;
	//////////////////////////////////////////////////////////////////////////////

typedef struct tagDecoData
{
	CString m_szName;
	int          m_iDecoCode;
	int          m_iPeso;
	int          m_iCash;
	int          m_iBonusPeso;
	int          m_iSellPeso;
	bool         m_bActive;
	int          m_iLimitLevel;
	
	int m_iSubscriptionType;

	tagDecoData()
	{
		m_iDecoCode   = 999999999;
		m_iPeso		  = 0;
		m_iCash		  = 0;
		m_iBonusPeso  = 0;
		m_iSellPeso   = 0;
		m_bActive	  = false;
		m_iLimitLevel = 0;
	}
}DecoData;
typedef std::vector< DecoData > vDecoData; // vector을 다른것으로 대체할때 INI Reload 개선 필요
typedef struct tagDecoList
{
	CString		 m_strTitle;
	int          m_iDecoType;
	int			 m_iPackageKeepPeso;
	vDecoData    m_vList;
	tagDecoList()
	{
		m_strTitle = "";
		m_iDecoType = m_iPackageKeepPeso = 0;
	}

}DecoList;
typedef std::vector< DecoList > vDecoList;	 // vector을 다른것으로 대체할때 INI Reload 개선 필요

typedef std::vector<int> IntVec;
typedef struct tagDefaultList
{
	int    m_iDecoType;	
	IntVec m_vDecoCodeList;
	tagDefaultList()
	{
		m_iDecoType = 0;
	}
}DefaultList;

typedef std::vector< DefaultList > vDefaultList; // vector을 다른것으로 대체할때 INI Reload 개선 필요


		struct SexDecoList
	{		
		CString m_szName;
		CString m_szINI;
		int          m_iSex;

		vDecoList    m_vList;
		vDefaultList m_vDefaultList;
		SexDecoList()
		{
			m_iSex			= 0;
		}
	};


		/////////////////////////////////////////////////////////////////////////
	enum ItemType 
	{
		IT_NONE  = -1,
		IT_CLASS = 0,
		IT_DECO  = 1,
		IT_ETC   = 2,
		IT_EXTRA_BOX = 3,
	};

	struct BackUpInfo // 향후 항목 추가를 대비해서 struct로
	{
		int  m_iCash;
		int  m_iPeso;
		bool m_bItemActive;

		BackUpInfo()
		{
			Clear();
		}

		void Clear()
		{
			m_iCash   = 0;
			m_iPeso   = 0;
			m_bItemActive = false;
		}
	};


////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
///                        용병관련                      ///
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

struct HeroClassRate
{
	int m_iStart;
	int m_iEnd;
	int m_iRate;

	HeroClassRate()
	{
		m_iStart = 0;
		m_iEnd = 0;
		m_iRate = 0;
	}
};

typedef std::vector< HeroClassRate* > vHeroClassRate;

enum PriceType
{
	PT_NORMAL  = 0,
	PT_PREMIUM = 1,
	PT_RARE    = 2,
};
struct PriceData
{
	int   m_iSetCode;              
	int   m_bActive;
	int   m_iBuyCash;
	PriceType  m_eType;

	PriceData()
	{
		m_iSetCode			= 0;
		m_bActive			= 0;
		m_iBuyCash			= 0;
		m_eType				= PT_NORMAL;
	}
};

// 기간제 & 영구
enum  CharPeriodType
{
	CPT_TIME     = 0,
	CPT_MORTMAIN = 1,		// 영구
	CPT_MAX		 = 2,
};

struct LimitData
{
	int   m_iLimitDate;
	float m_fLimitPricePer;

	LimitData()
	{
		m_iLimitDate	 = 0;			
		m_fLimitPricePer = 1.0f;
	}
};

////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
///                        추가관련                      ///
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

struct RandomItem
{
	int m_iItemCode;
	int m_iRandomRate;

	RandomItem()
	{
		m_iItemCode = 0;
		m_iRandomRate = 0;
	}
};
typedef std::vector< RandomItem > RandomItemList;


struct ExtraItem
{
	int m_iMachine_code;
	int m_iType;
	int m_iActive;
	int m_iBuyCash;
	int m_iPackage;
	CString m_strMachineName;
	RandomItemList m_vRandomItemList;

	ExtraItem()
	{
		m_iMachine_code = 0;
		m_iType = 0;
		m_iActive = 0;
		m_iBuyCash = 0;
		m_strMachineName.Empty();
		m_vRandomItemList.clear();
	}
};

typedef std::map<int, ExtraItem*> mExtraItem;



////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
///                      특별상점관련                    ///
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////


struct sPopupItemInfo
{
	int m_iActive;

	int m_iIndex;
	int m_iPresentType;
	int m_iPresentValue1;
	int m_iPresentValue2;
	int m_iCash;
	int m_iPrevCash;
	int m_iDisCount;
};

typedef std::vector< sPopupItemInfo* > vPopupItemInfo;






////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////
///                      특별상점관련                    ///
////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////

struct SaleInfo
{
	ItemType m_eItemType;
	int    m_iType1;
	int    m_iType2;
	int    m_iStartDate;
	int    m_iEndDate;
	int      m_iCash;
	bool     m_bActive;

	SaleInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_eItemType   = IT_NONE;
		m_iType1     = 0;
		m_iType2     = 0;
		m_iStartDate = 0;
		m_iEndDate   = 0;
		m_iCash       = 0;
		m_bActive     = false;
	}
};

typedef std::vector< SaleInfo* > vSaleInfoVector;