#ifndef __ioSaleManager_h__
#define __ioSaleManager_h__

#include "../Util/Singleton.h"

class User;

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
	};
protected:
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
		BackUpInfo m_kBackUp;

		SaleInfo()
		{
			Clear();
		}

		void Clear()
		{
			m_eItemType   = IT_NONE;
			m_dwType1     = 0;
			m_dwType2     = 0;
			m_dwStartDate = 0;
			m_dwEndDate   = 0;
			m_iCash       = 0;
			m_iPeso       = 0;
			m_bActive     = false;
			m_kBackUp.Clear();
		}
	};
protected:
	typedef std::vector< SaleInfo* > vSaleInfoVector;

protected:
	vSaleInfoVector m_vSaleInfoVector;
	DWORD           m_dwProcessTime;
	DWORD           m_dwLastChangedServerDate;

public:
	void LoadINI( bool bCreateLoad, ioINILoader &rkLoader, ItemType eItemType, DWORD dwType1, DWORD dwType2 = 0, int iINIArray = 0);
	void ProcessTime();
	void SendLastActiveDate( User *pUser );

	int  GetCash( ItemType eItemType, DWORD dwType1, DWORD dwType2 );
	int  GetPeso( ItemType eItemType, DWORD dwType1, DWORD dwType2 );
	bool IsSelling( ItemType eItemType, DWORD dwType, DWORD dwSubType = 0, bool bActive = true);

protected:
	void Clear();
	SaleInfo *GetInfo( ItemType eType, DWORD dwType1, DWORD dwType2 );
	bool IsCheckAlive( SYSTEMTIME st , DWORD dwStartDate, DWORD dwEndDate );

public:
	static ioSaleManager &GetSingleton();

public:
	ioSaleManager(void);
	virtual ~ioSaleManager(void);
};

#define g_SaleMgr ioSaleManager::GetSingleton()

#endif // __ioSaleManager_h__