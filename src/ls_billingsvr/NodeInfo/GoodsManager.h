#ifndef __GoodsManager_h__
#define __GoodsManager_h__

#include "../Util/Singleton.h"

#define PRESENT_SOLDIER             1
#define PRESENT_DECORATION          2
#define PRESENT_ETC_ITEM            3
#define PRESENT_PESO                4
#define PRESENT_EXTRAITEM           5
#define PRESENT_EXTRAITEM_BOX       6

class GoodsManager : public Singleton< GoodsManager >
{
public:
	enum  CharPeriodType
	{
		CPT_TIME     = 0,
		CPT_MORTMAIN = 1,
#ifdef DATE_CHAMP
		CPT_DATE = 2,
#endif
	};

protected:
	typedef std::map< DWORD, ioHashString > GoodsInfoMap;
	GoodsInfoMap m_GoodsInfoMap;
	
	typedef struct tagSoldierInfo
	{
		int m_iLimitSeconds;
		int m_iGoodsNoValue;

		tagSoldierInfo()
		{
			m_iLimitSeconds = 0;
			m_iGoodsNoValue = 0;
		}
	}SoldierInfo;

	typedef std::vector< SoldierInfo > vSoldierInfo;
	vSoldierInfo m_vSoldierInfo;

	int          m_iMortmainCharGoodsNoValue;
	DWORDVec     m_vPackageEtcItemShortTypeList;
	DWORDVec     m_vPackageExtraItemMachineCodeList;
	ioHashString m_szPresentAddGoodsName;

protected:
	int  GetGoodsNoValue( int iLimitSeconds );
	bool GetGoodsName( IN DWORD dwGoodsNo, OUT ioHashString &rszGoodsName );
	bool IsPackageItem( int iEtcItemShortType );
	bool IsPackageItemExtra( int iExtraItemMachineCode );

public:
	bool LoadINI( const char *szFileName, const char *szGoodsNameFile );
	bool ReloadINI( const char *szFileName, const char *szGoodsNameFile );
	bool GetGoodsInfoSoldier( IN int iClassType, IN int iLimitSeconds, IN int iPeriodType, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName );
	bool GetGoodsInfoDeco( IN int iType, IN int iDecoCode, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName );
	bool GetGoodsInfoEtc( IN DWORD dwType, IN int iArray, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName );
	bool GetGoodsInfoExtraBox( IN int iMachineCode, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName );
	bool GetGoodsInfoExtra( IN int iItemCode, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName );
	bool GetGoodsInfo( IN short iPresentType, IN int iBuyValue1, IN int iBuyValue2, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName );
	bool GetGoodsInfoPresent( IN short iPresentType, IN int iBuyValue1, IN int iBuyValue2, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName );
	bool GetGoodsInfoSubcription( IN short iPresentType, IN int iBuyValue1, IN int iBuyValue2, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName );
	bool GetGoodsInfoPresentMileage( IN short iPresentType, IN int iBuyValue1, IN int iBuyValue2, IN bool bPresent, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName );
	bool GetGoodsInfoPopup( IN short iPresentType, IN int iBuyValue3, OUT DWORD &rdwGoodsNo, OUT ioHashString &rszGoodsName );

public:
	static GoodsManager &GetSingleton();

public:
	GoodsManager(void);
	virtual ~GoodsManager(void);
};

#define g_GoodsMgr GoodsManager::GetSingleton()

#endif // __GoodsManager_h__
