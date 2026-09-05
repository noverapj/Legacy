#include "stdafx.h"
#include "ioGashaponMgr.h"

GashaponPresent::GashaponPresent()
{
	Clear();
}

GashaponPresent::~GashaponPresent()
{
}

void GashaponPresent::Clear()
{
	m_iType			  = 0;
	m_iValue1		  = 0;
	m_iValue2		  = 0;
	m_iOrder		  = 0;
	m_iSpecialAddType = 0;
	m_bLimited_Flag	  = false;
}

bool GashaponPresent::IsEmpty()
{
	if( m_iType <= 0 && m_iValue1 <= 0 && m_iValue2 <= 0 )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool operator!=( const GashaponPresent &lhs, const GashaponPresent &rhs )
{
	if( lhs.m_iType == rhs.m_iType && lhs.m_iValue1 == rhs.m_iValue1 && lhs.m_iValue2 == rhs.m_iValue2 )
		return false;

	return true;
}

bool operator==( const GashaponPresent &lhs, const GashaponPresent &rhs )
{
	if( lhs.m_iType == rhs.m_iType && lhs.m_iValue1 == rhs.m_iValue1 && lhs.m_iValue2 == rhs.m_iValue2 )
		return true;

	return false;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template<> ioGashaponMgr* Singleton< ioGashaponMgr >::ms_Singleton = 0;
ioGashaponMgr& ioGashaponMgr::GetSingleton()
{
	return Singleton< ioGashaponMgr >::GetSingleton();
}

ioGashaponMgr::ioGashaponMgr()
{	
}

ioGashaponMgr::~ioGashaponMgr()
{
}

void ioGashaponMgr::Initialize()
{
	ioINILoader_e kLoader( "config/sp2_gashapon_present.ini" );
	kLoader.SetTitle_e( "Common" );
	int iMaxItem = kLoader.LoadInt_e( "MaxInfo", 0 );

	for( int i = 0; i < iMaxItem; ++i )
	{
		char szTtitle[MAX_PATH];
		wsprintf_e( szTtitle, "GashaponPresent%d", i+1 );
		kLoader.SetTitle( szTtitle );

		GashaponPresentInfo kInfo;
		int iEtcItemType = kLoader.LoadInt( "EtcItemType", 0 );
		int iPresentLimitedQuantity = kLoader.LoadInt( "PresentLimitedQuantity", 0 );	// 2020-03-26
		if( 0 < iEtcItemType )
		{
			kInfo.m_iEtcItemType = iEtcItemType;
			kInfo.m_iLimited_Quantity = iPresentLimitedQuantity;
			LoadGashaponList( kLoader, kInfo );

			m_GashaponPresentInfoMap.insert( GashaponPresentInfoMap::value_type( iEtcItemType, kInfo ) );
		}
	}
}

void ioGashaponMgr::LoadGashaponList( ioINILoader& rkLoader, GashaponPresentInfo& kInfo )
{	
	char szKey[MAX_PATH] = "";
	int iMaxItem = rkLoader.LoadInt( "MaxItem", 0 );
	for (int i = 0; i < iMaxItem ; i++ )
	{
		GashaponPresent Present;

		sprintf_s_e( szKey, "Present%d_Type", i + 1 );
		Present.m_iType = (short)rkLoader.LoadInt( szKey, 0 );

		sprintf_s_e( szKey, "Present%d_Value1", i + 1 );
		Present.m_iValue1 = rkLoader.LoadInt( szKey, 0 );

		sprintf_s_e( szKey, "Present%d_Value2", i + 1 );
		Present.m_iValue2 = rkLoader.LoadInt( szKey, 0 );

		sprintf_s_e( szKey, "Present%d_order", i + 1 );
		Present.m_iOrder = rkLoader.LoadInt( szKey, 0 );

		sprintf_s_e( szKey, "Present%d_Special_Type", i + 1 );
		Present.m_iSpecialAddType = rkLoader.LoadInt( szKey, 0 );

		// 2020-04-13
		sprintf_s_e( szKey, "Present%d_Limited", i + 1 );
		Present.m_bLimited_Flag = rkLoader.LoadBool( szKey, false );

		kInfo.m_vPresent.push_back( Present );
	}
}

const GashaponPresentInfo& ioGashaponMgr::FindGashaponPresentInfo( int iEtcItemType )
{
	static GashaponPresentInfo kNone;
	GashaponPresentInfoMap::iterator iter = m_GashaponPresentInfoMap.find( iEtcItemType );
	if( iter != m_GashaponPresentInfoMap.end() )
	{		
		return iter->second;
	}

	return kNone;
}

const vGashaponPresent& ioGashaponMgr::FindGashaponPresentList( int iEtcItemType ) 
{
	static vGashaponPresent kNone;
	GashaponPresentInfoMap::const_iterator iter = m_GashaponPresentInfoMap.find( iEtcItemType );
	if( iter != m_GashaponPresentInfoMap.end() )
	{
		const GashaponPresentInfo& kInfo = iter->second;
		return kInfo.m_vPresent;
	}

	return kNone;
}
