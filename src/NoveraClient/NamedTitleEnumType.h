#pragma once

enum NamedTitleStatus
{
	TITLE_DISABLE = 0,
	TITLE_ACTIVE = 1,
	TITLE_NEW = 2,
};

//ÄªÈ£ ´Ü°è º°
typedef struct tagNamedTitleGrade
{
	DWORD m_dwColor;
	ioHashString m_szMark;

	void Init()
	{
		m_dwColor = 0;
		m_szMark.Clear();
	}

	tagNamedTitleGrade()
	{
		Init();
	}
}NamedTitleGrade;

typedef std::vector<NamedTitleGrade> NamedTitleGradeList;

//ÄªÈ£ Á¤º¸
typedef struct tagNamedTitleInfo
{
	DWORD m_dwCode;
	ioHashString m_szName;
	NamedTitleGradeList m_GradeList;

	void Init()
	{
		m_dwCode = 0;
		m_szName.Clear();
		m_GradeList.clear();
	}

	tagNamedTitleInfo()
	{
		Init();
	}
}NamedTitleInfo;

typedef std::map<DWORD, NamedTitleInfo> NamedTitleInfoList;

//ÄªÈ£ ½½·Ô
typedef struct tagNamedTitleSlot
{
	DWORD m_dwCode;
	int m_nLevel;
	__int64 m_nValue;

	BYTE m_byPremium;
	BYTE m_byEquip;
	BYTE m_byStatus;

	tagNamedTitleSlot()
	{
		Init();
	}

	void Init()
	{
		m_dwCode = 0;
		m_nLevel = 0;
		m_nValue = 0;
		
		m_byPremium = 0;
		m_byEquip = 0;
		m_byStatus = TITLE_DISABLE;
	}
}NamedTitleSlot;

//ÄªÈ£ ½½·Ô ¸®½ºÆ®
typedef std::vector<NamedTitleSlot> NamedTitleSlotList;

//ÄªÈ£ Á¤·Ä Á¤º¸
typedef struct tagNamedTitleSortInfo
{
	DWORD m_dwCode;
	ioHashString m_szNamedTitle;

	tagNamedTitleSortInfo()
	{
		Init();
	}

	void Init()
	{
		m_dwCode = 0;
		m_szNamedTitle.Clear();
	}
}NamedTitleSortInfo;

//ÄªÈ£ Á¤·Ä
class NamedTitleSortFunc : public std::binary_function< const NamedTitleSortInfo&, const NamedTitleSortInfo&, bool >
{
public:
	bool operator()( const NamedTitleSortInfo &lhs, const NamedTitleSortInfo &rhs ) const
	{
		if( strcmp( lhs.m_szNamedTitle.c_str(), rhs.m_szNamedTitle.c_str() ) < 0 )
			return true;

		return false;	
	}
};