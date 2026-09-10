#pragma once

class GashaponPresent
{
public:
	short	m_iType;
	int		m_iValue1;
	int		m_iValue2;
	int		m_iOrder;
	int		m_iSpecialAddType;
	bool	m_bLimited_Flag; // 2020-03-26 가차 선택 제한
	

public:
	void Clear();
	bool IsEmpty();

public:
	GashaponPresent();
	~GashaponPresent();
};

bool operator==( const GashaponPresent &lhs, const GashaponPresent &rhs );
bool operator!=( const GashaponPresent &lhs, const GashaponPresent &rhs );

typedef std::vector<GashaponPresent> vGashaponPresent;

struct GashaponPresentInfo
{
	int					m_iEtcItemType;
	vGashaponPresent	m_vPresent;
	int					m_iLimited_Quantity; // 2020-03-26 가차 선택 제한

	GashaponPresentInfo()
	{
		m_iEtcItemType		= 0;
		m_iLimited_Quantity	= 0;
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class GashaponPresentSortByOrder : public std::binary_function< const GashaponPresent&, const GashaponPresent&, bool >
{
public:
	bool operator()( const GashaponPresent &lhs , const GashaponPresent &rhs ) const
	{
		if( lhs.m_iOrder < rhs.m_iOrder )
			return true;

		return false;
	}
};

class GashaponPresentSortByValue : public std::binary_function< const GashaponPresent&, const GashaponPresent&, bool >
{
public:
	bool operator()( const GashaponPresent &lhs , const GashaponPresent &rhs ) const
	{
		if( lhs.m_iValue1 > rhs.m_iValue1 )
			return true;

		return false;
	}
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef std::map<DWORD, GashaponPresentInfo> GashaponPresentInfoMap;

class ioGashaponMgr : public Singleton< ioGashaponMgr >
{
protected:
	GashaponPresentInfoMap m_GashaponPresentInfoMap;

public:
	static ioGashaponMgr& GetSingleton();

public:
	virtual void Initialize();
	
	void LoadGashaponList( ioINILoader& rkLoader, GashaponPresentInfo& kInfo );	

public:
	const GashaponPresentInfo& FindGashaponPresentInfo( int iEtcItemType );
	const vGashaponPresent& FindGashaponPresentList( int iEtcItemType ) ;

public:
	ioGashaponMgr();
	virtual ~ioGashaponMgr();
};

#define g_GashaponMgr ioGashaponMgr::GetSingleton()