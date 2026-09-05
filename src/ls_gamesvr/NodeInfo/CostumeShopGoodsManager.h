#pragma once

class CostumeGoodsInfo
{
public:
	struct PesoSellInfo
	{
		int iPeriod;
		int iNeedPeso;

		PesoSellInfo()
		{
			iPeriod = 0;
			iNeedPeso = 0;
		}
	};
public:
	CostumeGoodsInfo();
	~CostumeGoodsInfo();

	void Init();
	void Destroy();
	
public:
	inline void SetGoodsCode(int iGoodsCode) { m_iGoodsCode = iGoodsCode; }
	void SetPesoGoodsInfo(int iPeriod, int iPeso);
	inline void SetItemCode(int iCode) { m_iItemCode = iCode; }
	inline void SetActive( bool bActive )	{ m_bActive = bActive; }

	inline int GetGoodsCode() { return m_iGoodsCode; }
	inline int GetItemCode() { return m_iItemCode; }
	inline bool GetActive() { return m_bActive; }
	inline int GetGoodsCount() { return m_vPesoGoodsInfo.size(); }

	int GetPesoGoodsPriceInfo(int iArray);
	int GetPesoGoodsPeriodInfo(int iArray);

	PesoSellInfo* GetPesoGoodsInfo(int iArray);

	bool IsRightPeriod(int iPeriod);

protected:
	typedef std::vector<PesoSellInfo> PesoGoodsInfo;

protected:
	int m_iGoodsCode;		// 상점에서 분류하는 코드
	int m_iItemCode;		// 아이템 고유 코드
	bool m_bActive;
	PesoGoodsInfo m_vPesoGoodsInfo;	//해당상품의 기간에따른 패소값 저장.
};

class CostumeShopGoodsManager : public Singleton< CostumeShopGoodsManager >
{
public:
	CostumeShopGoodsManager();
	virtual ~CostumeShopGoodsManager();

	void Init();
	void Destroy();

	void LoadINI();
public:
	CostumeGoodsInfo* GetCostumeGoodsInfo(int iGoodsCode);

	bool IsRightGoods(int iGoodsCode);
	bool IsRightPeriod(int iGoodsCode, int iPeriod);
	bool IsRightArray(int iGoodsCode, int iArray);

	int GetNeedPeso(int iGoodsCode, int iArray);

	int GetPeriod(int iGoodsCode, int iArray);
	int GetItemCode(int iGoodsCode);

protected:
	void LoadCustumeShopGoods( ioINILoader &rkLoader );
public:
	static CostumeShopGoodsManager& GetSingleton();

protected:
	typedef boost::unordered_map<int, CostumeGoodsInfo> GoodsList;		//<상품코드, class>

protected:
	GoodsList m_mCostumeGoodsMap;
	bool m_bINILoading;
};

#define g_CostumeShopGoodsMgr CostumeShopGoodsManager::GetSingleton()