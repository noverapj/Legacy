#ifndef _ExtraItemGrowthCatalystMgr_h_
#define _ExtraItemGrowthCatalystMgr_h_

class ServerNode;
class ExtraItemGrowthCatalystMgr : public Singleton< ExtraItemGrowthCatalystMgr >
{
protected:
	DWORDVec m_dwLevelUPRandList;           // 0lv ~ Nlv
	DWORDVec m_dwReinforceRandList;         // +0 ~ +N

protected:
	struct ExtraItemMortmainLimit
	{
		DWORD m_dwExtraItemCode;
		DWORD m_dwMortmainCount;
		DWORD m_dwMortmainMinute;             // Original : Minute 
		DWORD m_dwCurrentMortmainMinue;       // Current  : Month:Day:Hour:Minute

		ExtraItemMortmainLimit()
		{
			m_dwExtraItemCode = m_dwMortmainCount = m_dwMortmainMinute = m_dwCurrentMortmainMinue = 0;
		}
	};
	typedef std::vector< ExtraItemMortmainLimit > vExtraItemMortmainLimit;
	vExtraItemMortmainLimit m_vExtraItemMortmainLimit;
	vExtraItemMortmainLimit m_vCurrentExtraItemMortmain;

protected:
	void ClearData();

	bool IsExtraItemMortmainTimeCheck( DWORD dwItemCode );
	int GetExtraItemMortmainLimit( DWORD dwItemCode );
	int GetCurrentExtraItemMortmain( DWORD dwItemCode );
	DWORD GetExtraItemMortmainLimitMinute( DWORD dwItemCode );
	void SaveCurrentExtraItemMortmain( DWORD dwItemCode );

public:
	void LoadINIData( bool bCreate );

public:
	bool IsExtraItemMortmainCheck( DWORD dwItemCode );
	void MinusExtraItemMortmainCount( DWORD dwItemCode );
	void GetExtraItemMortmainInfo( DWORD dwItemCode, DWORD &rCount, DWORD &rDate );

public:
	void SendLoadData( ServerNode *pServerNode );

public:
	static ExtraItemGrowthCatalystMgr& GetSingleton();

public:
	ExtraItemGrowthCatalystMgr();
	virtual ~ExtraItemGrowthCatalystMgr();
};
#define g_ExtraItemGrowthCatalystMgr ExtraItemGrowthCatalystMgr::GetSingleton()
#endif