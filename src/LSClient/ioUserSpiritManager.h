#pragma once

#include "GUI/MyInventoryWnd.h"

enum DecomposeType
{
	DT_SPIRIT = 1,
	DT_SPECIAL = 2,
};

struct SpiritInfo
{
	int m_iCode;
	int m_iTable;
	int m_iDecomposeType;
	ioHashString m_szIcon;
	ioHashString m_szName;
};
typedef std::map< int, SpiritInfo > SpiritInfoMap;

struct SpiritTable
{
	int m_iSellPeso;
	int m_iComposeMin;
	int m_iComposeMax;
	int m_iDecomposeType;
	int m_iDecomposeQuantity;
	float m_fDecomposeCriticalRate;
	int m_iDecomposeCriticalQuantity;
	float m_fConversionCriticalRate;
	int m_iConversionCriticalQuantity;

	SpiritTable()
	{
		Init();
	}

	void Init()
	{
		m_iSellPeso = 0;
		m_iComposeMin = 0;
		m_iComposeMax = 0;
		m_iDecomposeType = 0;
		m_iDecomposeQuantity = 0;
		m_fDecomposeCriticalRate = 0.f;
		m_iDecomposeCriticalQuantity = 0;
		m_fConversionCriticalRate = 0.f;
		m_iConversionCriticalQuantity = 0;
	}
};
typedef std::map< int, SpiritTable > SpiritTableMap;

struct RewardInfo
{
	int table_index;
	int random_value;
};
struct ModeSpiritReward
{
	ModeType mode_type;
	std::vector<RewardInfo> reward_list;
};
typedef std::vector<ModeSpiritReward> vModeSpiritReward;

class ioUserSpiritManager : public Singleton< ioUserSpiritManager >
{
public:
	enum SpiritType
	{
		SPT_SPECIAL = 0,
		SPT_UNIQUE = 1,
		SPT_PRIMIUM = 2,
		SPT_IDOL = 3,
		SPT_RARE = 4,
		SPT_NORMAL = 5,
	};

private:
	SpiritInfoMap m_SpiritInfoMap;
	SpiritTableMap m_SpiritTableMap;

	ioHashString m_szSpiritName;
	int m_iSpecialSpiritCode;

	ioHashString m_szPacketError[8];

public:
	void LoadINIInfo();
	int GetDecomposeQuantity( int iSpiritCode );
	int GetComposeMax( int iSpiritCode );
	int GetComposeMin( int iSpiritCode );
	int GetSpiritType( int iSpiritCode );
	int GetSellConst( int iSpiritCode );
	ioHashString GetIconName( int iSpiritCode );
	ioHashString GetName( int iSpiritCode );
	ioHashString GetDecomposeSpiritName( int iClassType );

	void GetSpiritInfo( vAlchemicItemInfoList &rkInfoList, int iSpiritType );
	bool CheckPacketResult( int iResult );

public:
	static ioUserSpiritManager& GetSingleton();

public:
	ioUserSpiritManager();
	virtual ~ioUserSpiritManager();
};

#define g_SpiritMgr ioUserSpiritManager::GetSingleton()