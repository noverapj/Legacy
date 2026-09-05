#pragma once
#include "GUI/NewShopSortDefine.h"

#include "../io3DEngine/ioStringConverter.h"
#include "ioComplexStringPrinter.h"


class ioRenderSystem;
class ioUIRenderer;
class ioUIRenderImage;
class ioUIRenderFrame;

//15.03.06 기획팀에 요청에 의해 타입을 PRESENT 타입과 동일하게 변경
enum ADProductType
{
	ADPT_NONE		= 0,
	ADPT_SOLDIER	= 1,
	ADPT_EXTRA		= 5,
	ADPT_DECO		= 7,
	ADPT_ETC		= 3,
	ADPT_COSTUME	= 12,
};

struct ADCategory
{
	ioHashString	m_szName;	
	IntVec			m_ADProductIndexList;

	ADCategory()
	{
	}
};
typedef std::vector <ADCategory> ADCategoryList;

struct ADPeriod
{
	int	m_iYear;
	int	m_iMonth;
	int	m_iDay;
	int	m_iHour;
	int	m_iMin;

	ADPeriod()
	{
		m_iYear		= 0;
		m_iMonth	= 0;
		m_iDay		= 0;
		m_iHour		= 0;
		m_iMin		= 0;
	}
};

struct ADProductLink
{
	ADProductType m_eADProductType;

	int	m_iTab;
	int	m_iSubTab;

	int	m_iValue1;
	int	m_iValue2;
	int m_iValue3;

	ADProductLink()
	{
		Clear();
	}

	void Clear()
	{
		m_eADProductType	= ADPT_NONE;
		m_iTab				= 0;
		m_iSubTab			= 0;

		m_iValue1			= 0;
		m_iValue2			= 0;
		m_iValue3			= 0;
	}

	bool IsAvailableLink( int iValue1, int iValue2, int iValue3 ) const
	{
		switch( m_eADProductType )
		{
		case ADPT_SOLDIER:
			{
				if( m_iValue1 == iValue1 )
					return true;
			}
			break;
		case ADPT_EXTRA:
			{
				if( m_iValue1 == iValue1 )
					return true;
			}
			break;
		case ADPT_DECO:
			{
				if( m_iValue1 == iValue1 && m_iValue2 == iValue2 && m_iValue3 == iValue3 )
					return true;
			}
			break;
		case ADPT_ETC:
			{
				if( m_iValue1 == iValue1 && m_iValue2 == iValue2 )
					return true;
			}
			break;
		case ADPT_COSTUME:
			{
				if( m_iValue1 == iValue1 && m_iValue2 == iValue2 )
					return true;
			}
			break;
		}

		return false;
	}

	bool IsNoneLink() const
	{
		if( m_eADProductType == ADPT_NONE )
			return true;

		if( m_iTab == 0 && m_iSubTab == 0 )
			return true;

		return IsAvailableLink( 0, 0, 0 );
	}
};

struct ADText
{
	ADPeriod		m_ADStartPeriod;
	ADPeriod		m_ADEndPeriod;

	DWORD			m_dwColor;
	float			m_fADSpeed;
	ioHashString	m_Text;

	ADText()
	{
		m_fADSpeed	= 0.0f;
		m_dwColor	= TCT_DEFAULT_GRAY;
	}
};
typedef std::vector < ADText > ADTextList;

struct ADImage
{
	ADPeriod			m_ADStartPeriod;
	ADPeriod			m_ADEndPeriod;
	ADProductLink		m_Link;

	DWORD				m_dwHoldTime;
	DWORD				m_dPagingTime;
	ioUIRenderImage*	m_pImage;

	ADImage()
	{
		m_dwHoldTime	= 0;
		m_dPagingTime	= 0;
		m_pImage		= NULL;
	}
};
typedef std::vector < ADImage > ADImageList;

struct ADProduct
{
	enum
	{
		NONE_PRODUCT = 0,
	};

	ADProductLink	m_Link;
	
	ADProduct()
	{
	}
};
typedef std::vector < ADProduct > ADProductList;

struct NewShopFilterInfo
{
	NewShopFilter	m_eFiter;
	ioHashString	m_FilterName;
	IntVec			m_EnableTabList;

	NewShopFilterInfo()
	{
		m_eFiter =	NSF_NONE;
	}

	bool IsUseTab( int iTab )
	{
		for( IntVec::const_iterator iter = m_EnableTabList.begin(); iter != m_EnableTabList.end(); ++iter )
		{
			if( iTab == *iter )
				return true;
		}

		return false;
	}
};
typedef std::vector < NewShopFilterInfo > NewShopFilterInfoList;

class ioShopManager : public Singleton< ioShopManager >
{
protected:
	ADCategoryList			m_ADCategoryList;
	NewShopFilterInfoList	m_NewShopFilterInfoList;

	ADTextList				m_ADTextList;
	ADImageList				m_ADImageList;
	ADProductList			m_ADProductList;
	bool					m_bUnablePresentBonus;
	ioHashStringVec			m_UnablePresentBonusList;
	bool					m_bEnablePresentBonus;
	ioHashStringVec			m_EnablePresentBonusList;

public:
	static ioShopManager& GetSingleton();

public:
	void Initialize(); 
	
	void LoadCategory( ioINILoader& rkLoader, int iMax );
	void LoadFilter( ioINILoader& rkLoader );

	void LoadADText( ioINILoader& rkLoader, int iMax );
	void LoadADImage( ioINILoader& rkLoader, int iMax );
	void LoadADPeriod( ioINILoader& rkLoader, ADPeriod& rkPeriod, const char* szTitle );
	void LoadADProductLink( ioINILoader& rkLoader, ADProductLink& Link );
	void LoadADProduct( ioINILoader& rkLoader, int iMax );
	void LoadUnablePresentBonus( ioINILoader& rkLoader );
	void LoadEnablePresentBonus( ioINILoader& rkLoader );

protected:
	void Destroy();

public:
	void ReLoad();

public:
	int GetADCategoryCount();
	const ioHashString* GetADCategoryName( int Index );
	void GetADCategoryProductList( IN int Index, OUT IntVec& List );

public:
	int GetFilterCount();
	NewShopFilterInfo* GetFilter( int Index );

public:
	int GetTextADCount();
	bool HasTextAD( int Index );
	const ADText* ioShopManager::GetTextAD( int Index );

public:
	int GetImageADCount();
	bool HasImageAD( int Index );
	const ADImage* ioShopManager::GetImageAD( int Index );

public:
	int GetADProductCount();
	bool HasADProduct( int Index );	
	const ADProduct* GetADProduct( int Index );	

public:
	bool IsEndAD( const ADPeriod& rkPeriod );
	bool IsStartAD( const ADPeriod& rkPeriod );

public:
	bool IsUnablePresentBonus();
	bool GetUnablePresentBonus( int nPresentType, int nPresemtValue );
	bool IsEnablePresentBonus();
	bool GetEnablePresentBonus( int nPresentType, int nPresemtValue );

public:
	ioShopManager();
	virtual ~ioShopManager();

};

#define g_ShopManager ioShopManager::GetSingleton()

