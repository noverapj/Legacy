#pragma once


class SP2Packet;
class ioINILoader;

class ioClassPrice : public Singleton< ioClassPrice >
{
private:
	enum 
	{
		DEFAULT_RESELL_PESO = 3750,
	};

	enum PriceType
	{
		PT_NORMAL  = 0,
		PT_PREMIUM = 1,
		PT_RARE    = 2,
	};

private:
	struct PriceData
	{
		int   m_iSetCode;   
		bool  m_bActive;
		bool  m_bPcRoomActive;
		bool  m_bFreeDayHero;
		int   m_iBuyPeso;
		int   m_iBuyCash;
		int   m_iBonusPeso;
		PriceType  m_eType;
		CTime	m_StartDay;
		CTime	m_EndDay;

		int m_iSubscriptionType;

		PriceData()
		{
			m_iSetCode	    = 0;	
			m_bActive       = false;
			m_bPcRoomActive = false;
			m_iBuyPeso      = 999999999;
			m_iBuyCash      = 999999999;
			m_iBonusPeso    = 0;
			m_eType         = PT_NORMAL;

			m_iSubscriptionType = SUBSCRIPTION_NONE;

			m_StartDay = 0;
			m_EndDay = 0;
		}
	};
	typedef std::vector< PriceData* > vPriceData;
	vPriceData m_vPriceData;

	struct LimitData
	{
		int   m_iLimitDate;
		float m_fLimitPricePer;

		LimitData()
		{
			m_iLimitDate	 = 0;			
			m_fLimitPricePer = FLOAT1;
		}
	};

	typedef std::vector< LimitData* > vLimitData;
	vLimitData m_vPesoLimitDataList;
	vLimitData m_vCashLimitDataList;
	vLimitData m_vPremiumCashLimitDataList;
	vLimitData m_vRareCashLimitDataList;

	DWORD m_dwFirstHireLimit;                //최초 용병 고용 기간
	DWORD m_dwDefaultLimit;                  //기본 용병 고용 기간
	DWORD m_dwBankruptcyLimit;               //파산 용병 고용 기간

	int   m_iBankruptcyCount;                //파산시 지급 용병 범위 정렬된 리스트의 0 ~
	float m_fLimitExtendDiscount;            //재고용 할인가격
	int   m_iBankruptcyPeso;                 //파산시 지급하는 페소

	int   m_iPeriodCharPseoChargeLimit;      //기간 충전 제한(페소)
	int	  m_iPeriodCharGoldChargeLimit;		 //기간 충전 제한(골드)
	bool  m_bSoilderTrainingActive;

	float m_fMortmainCharMultiply;           //무제한 용병 판매가격 곱하기 >> (2시간 고용가격) x m_fMortmainCharMultiply
	float m_fTimeCharDivision;               //기간제 용병 되팔기 보상 >> { (해당용병의 현재 2시간 고용가격 / 7200) x 해당 용병의 남은 보유시간 } / m_fTimeCharDivision
	float m_fMortmainCharDivision;           //무제한 용병 되팔기 보상 >> { (2시간 고용가격) x m_fMortmainCharMultiply } / m_fMortmainCharDivision
	float m_fMortmainCharMultiplyCash;       //무제한 용병의 캐쉬 판매가격 곱하기 >> cash x m_fMortmainCharMultiplyCash
	float m_fMortmainPremiumCharMultiplyCash;//무제한 프리미엄 용병의 캐쉬 판매가격 곱하기 >> cash x m_fMortmainPremiumCharMultiplyCash
	float m_fMortmainRareCharMultiplyCash;   //무제한 레어 용병의 캐쉬 판매가격 곱하기 >> cash x m_fMortmainRareCharMultiplyCash
	
	void ClearCurInfo();

public:
	void ApplyPrice( SP2Packet &rkPacket );
	
public:
	void LoadClassCommon();

	int GetLimitDatePeso( int iClassType, int iArray );
	int GetLimitDateCash( int iClassType, int iArray );
	int GetClassBuyPeso( int iClassType, int iLimitDate, bool bResell = false );
	int GetClassBuyCash( int iClassType, int iLimitDate  );
	int GetMortmainCharPeso( int iClassType, bool bResell = false );
	int GetMortmainCharCash( int iClassType );
	int GetBonusPeso( int iClassType );
	int GetLowPesoClass( int iLimitDate );
	int GetHighPesoClass();
	int GetLowPesoClassType();
	int GetSubscriptionType( int iClassType );

	int MaxClassPrice(){ return m_vPriceData.size(); }
	int MaxActiveClassPrice();
	int ArrayToSetItemIdx( int iArray );

	DWORD GetFirstHireLimit(){ return m_dwFirstHireLimit; }
	DWORD GetDefaultLimit(){ return m_dwDefaultLimit; }
	DWORD GetBankruptcyLimit(){ return m_dwBankruptcyLimit; }	
	int   GetBankruptcyCount(){ return m_iBankruptcyCount; }
	float GetLimitExtendDiscount(){ return m_fLimitExtendDiscount; }

	int GetBankruptcyPeso() const { return m_iBankruptcyPeso; }
	int GetPeriodCharPesoChargeLimit() const { return m_iPeriodCharPseoChargeLimit; }
	int GetPeriodCharGoldChargeLimit() const { return m_iPeriodCharGoldChargeLimit; }
	bool IsSoldierTraingActive() const { return m_bSoilderTrainingActive; }

	int GetClassType( int iArray );
	
	int GetTimeCharResellPeso( int iClassType, int iRemainTime );
	int GetMortmainCharResllPeso( int iClassType );

	bool IsCashOnly( int iClassType );
	bool IsPesoOnly( int iClassType );
	bool IsCashPeso( int iClassType );
	bool IsActive( int iClassType );
	bool IsPcRoomActive( int iClassType );

	bool IsActiveClass( int iArray );
	bool IsCashOnlyByArray( int iArray ); 
	bool IsRecommendedClass( int iClassType );

public:
	void SetActive( int iClassType, bool bActive );

public:
	static ioClassPrice& GetSingleton();

public:
	ioClassPrice();
	virtual ~ioClassPrice();
};

#define g_ClassPrice ioClassPrice::GetSingleton()