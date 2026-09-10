#pragma once


enum CompoundType
{
	CMT_SMALL,
	CMT_BIG,
	CMT_DOWN,
};

typedef struct tagCompoundRateInfo
{
	int m_iLevel;
	int m_iSameItemRate;
	int m_iSameItemRateS;
	int m_iOtherItemRate;
	int m_iOtherItemRateS;

	tagCompoundRateInfo()
	{
		m_iLevel = 0;
		m_iSameItemRate = 0;
		m_iSameItemRateS = 0;
		m_iOtherItemRate = 0;
		m_iOtherItemRateS = 0;
	}
} CompoundRateInfo;
typedef std::vector<CompoundRateInfo> CompoundRateInfoList;

typedef struct tagCompoundInfo
{
	int m_iSmallUpReinforce;
	int m_iBigUpReinforce;
	int m_iDownReinforce;

	CompoundRateInfoList m_vRateInfoList;

	tagCompoundInfo()
	{
		m_iSmallUpReinforce = 1;
		m_iBigUpReinforce = 2;
		m_iDownReinforce = 1;

		m_vRateInfoList.clear();
	}
} CompoundInfo;

class ioItemCompoundManager : public Singleton< ioItemCompoundManager >
{
protected:
	ioHashStringVec m_vCompoundItemIconList;

	typedef std::map< DWORD, CompoundInfo > CompoundInfoMap;
	CompoundInfoMap m_CompoundInfoMap;

	CEncrypt<int> m_iMaxReinforce;

	CEncrypt<int> m_iNeedLevel;
	CEncrypt<bool> m_bEnableSamePart;
	CEncrypt<bool> m_bEnableSameCode;

	CEncrypt<int> m_iRoomAlarmLevel;

protected:
	void ClearAllInfo();
	void ClearCompoundInfoMap();

public:
	void LoadCompoundInfo();
	
	int GetCompoundSuccessRate( int iTargetSlot, int iVictimSlot, CompoundType eType, DWORD dwCode );
	int GetCompoundReinforce( CompoundType eType, DWORD dwCode );

	inline int GetMaxReinforce() const { return m_iMaxReinforce; }
	inline int GetNeedLevel() const { return m_iNeedLevel; }
	inline bool EnableSamePart() const { return m_bEnableSamePart; }
	inline bool EnableSameCode() const { return m_bEnableSameCode; }

public:
	static ioItemCompoundManager& GetSingleton();

public:
	ioItemCompoundManager();
	virtual ~ioItemCompoundManager();
};

#define g_CompoundMgr ioItemCompoundManager::GetSingleton()

