

#ifndef _ioDecorationPrice_h_
#define _ioDecorationPrice_h_

#include "../Util/Singleton.h"
#include "NodeHelpStructDefine.h"

enum
{
	UID_FACE		 = 1,
	UID_HAIR		 = 2,
	UID_SKIN_COLOR	 = 3,
	UID_HAIR_COLOR	 = 4,
	UID_KINDRED      = 5,
	UID_CLASS        = 6,
	UID_UNDERWEAR    = 7,
	UID_WEAPON       = 10,
	UID_ARMOR        = 11,
	UID_HELM         = 12,
	UID_CLOAK        = 13,
};

typedef struct tagDecoData
{
	ioHashString m_szName;
	int          m_iDecoCode;
	int          m_iPeso;
	int          m_iCash;
	int          m_iBonusPeso;
	int          m_iSellPeso;
	bool         m_bActive;
	int          m_iLimitLevel;
	
	int m_iSubscriptionType;

	tagDecoData()
	{
		m_iDecoCode   = 999999999;
		m_iPeso		  = 0;
		m_iCash		  = 0;
		m_iBonusPeso  = 0;
		m_iSellPeso   = 0;
		m_bActive	  = false;
		m_iLimitLevel = 0;

		m_iSubscriptionType = SUBSCRIPTION_NONE;
	}
}DecoData;
typedef std::vector< DecoData > vDecoData; // vector을 다른것으로 대체할때 INI Reload 개선 필요
typedef struct tagDecoList
{
	int          m_iDecoType;
	int			 m_iPackageKeepPeso;
	vDecoData    m_vList;
	tagDecoList()
	{
		m_iDecoType = m_iPackageKeepPeso = 0;
	}

}DecoList;
typedef std::vector< DecoList > vDecoList;	 // vector을 다른것으로 대체할때 INI Reload 개선 필요


typedef struct tagDefaultList
{
	int    m_iDecoType;	
	IntVec m_vDecoCodeList;
	tagDefaultList()
	{
		m_iDecoType = 0;
	}
}DefaultList;
typedef std::vector< DefaultList > vDefaultList; // vector을 다른것으로 대체할때 INI Reload 개선 필요

struct SpecialDefaultDecoInfo
{
	int m_iFaceCode;
	int m_iHairCode;
	int m_iSkinColor;
	int m_iHairColor;
	int m_iUnderwearCode;

	SpecialDefaultDecoInfo()
	{
		m_iFaceCode			= 0;
		m_iHairCode			= 0;
		m_iSkinColor		= 0;
		m_iHairColor		= 0;
		m_iUnderwearCode	= 0;
	}
};

class ioDecorationPrice : public Singleton< ioDecorationPrice >
{
protected:
	struct SexDecoList
	{		
		ioHashString m_szName;
		ioHashString m_szINI;
		int          m_iSex;

		vDecoList    m_vList;
		vDefaultList m_vDefaultList;
		SexDecoList()
		{
			m_iSex			= 0;
		}
	};
	typedef std::vector< SexDecoList > vSexDecoList; // vector을 다른것으로 대체할때 INI Reload 개선 필요
	vSexDecoList m_vSexList;

	enum 
	{
		MAX_RAND_TABLE = 500,
	};
	static int m_RandTable[MAX_RAND_TABLE];

protected:
	void  ClearSex();

	void LoadDecoInfo( bool bCreateLoad );
	void LoadDefaultDecoCodeList( bool bCreateLoad );

	DWORD GetRandTableValue( DWORD dwRandSeed );
public:
	void LoadPriceInfo( bool bCreateLoad = true );

public:
	int  GetSubscriptionType( int iType, int iCode );

	int  GetDecoPackageKeepPeso( int iType );
	int  GetDecoPeso( int iType, int iCode, int iClassLevel );
	int  GetDecoCash( int iType, int iCode );
	bool GetDecoActive( int iType, int iCode );
	int  GetBonusPeso( int iType, int iCode );
	int  GetSellPeso( int iType, int iCode );
	int  GetDefaultDecoCode( int iSexType, int iDecoType, DWORD dwRandSeed, int iClassType ); 

	void SetDecoCash( int iType, int iCode, int iCash );
	void SetDecoPeso( int iType, int iCode, int iPeso );
	void SetDecoActive( int iType, int iCode, bool bActive );

	void GetSpecialDecoInfo(int iGender, int iClassType, SpecialDefaultDecoInfo& stInfo);

public:
	static ioDecorationPrice& GetSingleton();

public:
	ioDecorationPrice();
	virtual ~ioDecorationPrice();

protected:
	typedef boost::unordered_map<int, SpecialDefaultDecoInfo> SPECIALDEFAULTINFO; // <classType, 치장 정보들>
	typedef SPECIALDEFAULTINFO::iterator	SPECIALDEFAULTINFO_iter;

protected:
	SPECIALDEFAULTINFO m_mSpecialManDecoInfo;
	SPECIALDEFAULTINFO m_mSpecialWomanDecoInfo;
};

#define g_DecorationPrice ioDecorationPrice::GetSingleton()

#endif