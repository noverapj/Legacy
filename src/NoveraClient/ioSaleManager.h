#pragma once


class SP2Packet;

class ioSaleManager : public Singleton< ioSaleManager >
{
public:
	enum ItemType 
	{
		IT_NONE  = -1,
		IT_CLASS = 0,
		IT_DECO  = 1,
		IT_ETC   = 2,
		IT_EXTRA_BOX = 3,
		IT_COSTUME = 4,
	};

	enum 
	{
		SALE_SHOP_ORDER = 1,
	};
protected:
	struct BackUpInfo // 향후 항목 추가를 대비해서 struct로
	{
		int  m_iCash;
		int  m_iPeso;
		int  m_iShopOrder;
		bool m_bItemActive;
		int  m_iShopMarkType; // SHOP_MARK_TYPE_NONE = 0, SHOP_MARK_TYPE_SALE = 1, SHOP_MARK_TYPE_NEW = 2, SHOP_MARK_TYPE_HIT = 3, SHOP_MARK_TYPE_EVENT = 4,

		BackUpInfo()
		{
			Clear();
		}

		void Clear()
		{
			m_iCash      = 0;
			m_iPeso      = 0;
			m_iShopOrder = 0;
			m_bItemActive= false;
			m_iShopMarkType = 0; // SHOP_MARK_TYPE_NONE
		}
	};

	struct SaleInfo
	{
		ItemType m_eItemType;
		DWORD    m_dwType1;
		DWORD    m_dwType2;
		DWORD    m_dwStartDate;
		DWORD    m_dwEndDate;
		int      m_iCash;
		int      m_iPeso;
		bool     m_bActive;
		int      m_iShopOrder;
		int      m_iShopMarkType; 
		int      m_iINIArray;
		BackUpInfo m_kBackUp;

		

		SaleInfo()
		{
			Clear();
		}

		void Clear()
		{
			m_eItemType     = IT_NONE;
			m_dwType1       = 0;
			m_dwType2       = 0;
			m_dwStartDate   = 0;
			m_dwEndDate     = 0;
			m_iCash         = 0;
			m_iPeso         = 0;
			m_bActive       = false;
			m_iShopOrder    = 0;
			m_iShopMarkType = 0; // SHOP_MARK_TYPE_NONE;
			m_iINIArray     = 0;

			m_kBackUp.Clear();
		}
	};
protected:
	typedef std::vector< SaleInfo* > vSaleInfoVector;

protected:
	vSaleInfoVector m_vSaleInfoVector;
	DWORD           m_dwProcessTime;

public:
	void LoadINI( bool bCreateLoad, ioINILoader &rkLoader, ItemType eItemType, DWORD dwType1, DWORD dwType2 = 0, int iINIArray = 0);
	void CheckAlive( SP2Packet &rkPacket );
	
	int  GetCash( ItemType eItemType, DWORD dwType1, DWORD dwType2 );
	int  GetPeso( ItemType eItemType, DWORD dwType1, DWORD dwType2 );

protected:
	void Clear();
	SaleInfo *GetInfo( ItemType eType, DWORD dwType1, DWORD dwType2 );
	bool IsCheckAlive( DWORD dwCurServerDate, DWORD dwStartDate, DWORD dwEndDate );
	DWORD ConvertYYMMDDHHMMToDate( WORD wYear, WORD wMonth, WORD wDay, WORD wHour, WORD wMinute );

public:
	bool IsSaleByRecommended( ItemType eItemType, DWORD dwType1, int iIniArray = 0 );
public:
	static ioSaleManager &GetSingleton();

public:
	ioSaleManager(void);
	virtual ~ioSaleManager(void);
};

#define g_SaleMgr ioSaleManager::GetSingleton()

