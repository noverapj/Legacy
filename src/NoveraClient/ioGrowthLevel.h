#pragma once



class ioBaseChar;


enum
{
	TIG_NONE	= 0,
	TIG_WEAPON	= 1,
	TIG_ARMOR	= 2,
	TIG_HELMET	= 3,
	TIG_CLOAK	= 4,
	TIG_ATTACK	= 5,
	TIG_DEFENCE	= 6,
	TIG_MOVE	= 7,
	TIG_DROP	= 8,
};

class ioGrowthLevel
{
protected:
	ioHashString m_OwnerName;

protected:
	typedef std::map< int, int > GrowthPointMap;

	GrowthPointMap m_CharGrowthPoint;

	CharGrowthLevelMap m_CharGrowthLevel;
	ItemGrowthLevelMap m_ItemGrowthLevel;

	TimeGrowthInfoList m_vTimeGrowthInfoList;

	DWORD m_dwCheckTime;

	CEncrypt<int> m_iMaxCnt;
	CEncrypt<int> m_nInstantLevel;

public:
	void Initialize( const ioHashString& szName );

public:
	inline void SetOwnerName( const ioHashString &rkOwnerName ){ m_OwnerName = rkOwnerName; }

protected:
	void CheckGrowthPointClassType( int iClassType );
	int GetUseTimeGrowthPoint( int iClassType );

public:
	void ApplyGrowthLevelData( SP2Packet &rkPacket );
	void ApplyGrowthLevelDataRoomSync( int iClassType, SP2Packet &rkPacket );
	void CheckGrowthPoint();

	void ApplyCharGrowthLevelUp( int iClassType, int iSlot, int iUpLevel );
	void ApplyItemGrowthLevelUp( int iClassType, int iSlot, int iUpLevel );

	void ApplyCharGrowthLevelDown( int iClassType, int iSlot, int iDownLevel );
	void ApplyItemGrowthLevelDown( int iClassType, int iSlot, int iDownLevel );

	void SetCharGrowthPoint( int iClassType, int iPoint );
	void SetCharGrowthLevelInit( int iClassType, int iPoint );

	void SetExtraGrowthLevel(int nInstantLv) {m_nInstantLevel = nInstantLv;}
	void IncreaseExtraGrowthLevel() {m_nInstantLevel++;}
	void ResetInstantExtraGrowthLevel() {m_nInstantLevel = 0;}
	int	GetInstantLevel() {return m_nInstantLevel;}


public:
	BYTE GetCharGrowthLevel( int iClassType, int iSlot, bool bOriginal );
	BYTE GetItemGrowthLevel( int iClassType, int iSlot, bool bOriginal );

	void SetCharGrowthLevel( int iClassType, int iSlot, int iLevel );
	void SetItemGrowthLevel( int iClassType, int iSlot, int iLevel );

	BYTE GetGrowthLevelByTimeSlot( int iClassType, int iTimeSlot );

	int GetCharGrowthPoint( int iClassType );

	//
	int GetTimeGrowthRemainTime( int iClassType );
	void AddTimeGrowth( int iClassType, int iSlot, int iValue1, int iValue2 );
	void RemoveTimeGrowth( int iClassType );

	TimeGrowthInfo* FindTimeGrowthInfo( int iClassType );

	void CheckTimeGrowthEnd();

	int GetCurTimeGrowthClassCnt();
	bool CheckEnableTimeGrowthSlot();
	bool CheckEnableTimeGrowthClass( int iClassType );

	const ioHashString& GetOwnerName();
	void ClearCharGrowthLevel( int iClassType );

public:
	ioGrowthLevel();
	virtual ~ioGrowthLevel();
};

