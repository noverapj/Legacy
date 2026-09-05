

#include "stdafx.h"

#include "ioDecorationPrice.h"

#include "iosalemanager.h"

template<> ioDecorationPrice* Singleton< ioDecorationPrice >::ms_Singleton = 0;

int ioDecorationPrice::m_RandTable[ioDecorationPrice::MAX_RAND_TABLE]=
{
	457,293,122,429,159,166,312,348,259,341,206,18,218,294,442,153,299,431,0,150,59,3,330,325,54,161,24,269,119,300,194,394,365,64,306,376,79,10,71,186,412,169,164,391,361,70,483,29,484,81,458,424,289,214,473,254,356,45,395,479,343,1,409,380,307,340,156,85,198,315,174,360,89,316,489,281,140,146,32,355,451,7,274,335,441,171,476,459,465,272,333,474,107,397,6,405,317,248,36,277,38,443,407,222,51,26,103,211,488,497,252,27,141,149,42,401,82,213,68,22,37,453,435,53,253,482,359,14,207,263,187,91,242,344,327,301,168,191,232,128,366,291,375,57,202,234,98,113,390,152,387,295,258,396,374,250,136,408,99,188,433,290,261,296,95,388,235,72,101,240,321,106,236,244,239,303,471,349,389,329,49,160,131,110,183,34,478,304,123,130,20,208,94,137,472,487,498,495,447,237,283,184,167,398,292,308,460,386,86,310,105,402,178,33,135,364,353,77,172,173,212,52,109,345,58,243,351,162,288,406,46,260,69,400,84,229,422,205,108,75,337,133,257,175,225,102,363,121,392,117,332,115,275,372,13,190,157,385,450,413,485,280,468,151,17,114,4,217,163,493,419,116,371,309,2,336,92,228,124,132,256,48,67,112,9,461,267,177,328,216,28,278,41,142,314,319,23,367,462,227,231,434,494,12,449,245,417,39,148,444,436,8,111,410,62,226,271,368,220,379,180,279,347,323,456,104,143,78,268,87,196,233,5,452,83,193,287,192,350,120,313,448,195,97,96,393,454,204,339,480,492,11,90,439,311,469,354,415,404,373,155,297,185,357,382,322,273,25,73,170,241,403,423,199,249,270,40,455,31,377,266,420,362,88,230,302,50,496,334,381,265,30,467,66,47,352,470,445,369,282,100,255,139,224,486,346,475,60,223,201,428,93,298,463,358,43,430,55,221,35,286,411,421,147,326,65,418,414,499,440,154,264,209,56,179,197,74,466,338,21,129,305,165,481,63,490,342,219,399,285,438,446,477,134,138,251,203,19,238,145,189,427,210,276,247,200,126,144,215,80,182,44,320,125,158,425,176,370,15,324,61,318,416,432,437,246,181,378,464,384,118,331,127,491,383,262,76,426,16,284,
};

ioDecorationPrice::ioDecorationPrice()
{
}

ioDecorationPrice::~ioDecorationPrice()
{	
	ClearSex();
}

ioDecorationPrice& ioDecorationPrice::GetSingleton()
{
	return Singleton< ioDecorationPrice >::GetSingleton();
}

void ioDecorationPrice::ClearSex()
{
	int iSexSize = m_vSexList.size();
	int i,j;
	for(i = 0;i < iSexSize;i++)
	{
		int iDecoTypeSize = m_vSexList[i].m_vList.size();
		for(j = 0;j < iDecoTypeSize;j++)
		{
			m_vSexList[i].m_vList[j].m_vList.clear();
		}
		m_vSexList[i].m_vList.clear();
	}
	m_vSexList.clear();
	
}

void ioDecorationPrice::LoadPriceInfo()
{
	ClearSex();

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	ioINILoader_e kLoader( "config/sp2_deco_sex.ini" );	
	kLoader.SetTitle_e( "Info" );

	int max_sex = kLoader.LoadInt_e( "max_sex", 0 );
	for( int i=0; i<max_sex; i++ )
	{
		sprintf_e( szTitle, "Sex%d", i + 1 );
		kLoader.SetTitle( szTitle );
		
		SexDecoList kSexList;

		kSexList.m_iSex = kLoader.LoadInt( "Sex", 0 );

		kLoader.LoadString_e( "Name", "", szBuf, MAX_PATH );
		kSexList.m_szName = szBuf;
				
		kLoader.LoadString_e( "Deco_ini", "", szBuf, MAX_PATH );
		kSexList.m_szINI = szBuf;
		m_vSexList.push_back( kSexList );
	}

	// 성별 치장 아이템
	LoadDecoInfo();
	LoadDefaultDecoCodeList();
}

void ioDecorationPrice::LoadDecoInfo()
{
	char szTitle[MAX_PATH];
	char szKey[MAX_PATH];
	char szBuf[MAX_PATH*2];

	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{	
		ioINILoader kLoader( m_vSexList[i].m_szINI.c_str() );	
		kLoader.SetTitle_e( "Info" );

		int iMaxDeco = kLoader.LoadInt_e( "max_Deco", 0 );
		for(int j = 0;j < iMaxDeco;j++)
		{
			sprintf_e( szTitle, "Deco%d", j + 1 );
			kLoader.SetTitle( szTitle );

			DecoList kDL;
			ZeroMemory( szBuf, sizeof(szBuf) );
			kLoader.LoadString_e( "Title", "", szBuf, MAX_PATH );
			kDL.m_szTitle   = szBuf;
			kDL.m_iDecoType = kLoader.LoadInt_e( "Type", 0 );
			kDL.m_iPackageKeepPeso = kLoader.LoadInt_e( "PackageKeepPeso", 3000 );

			m_vSexList[i].m_vList.push_back( kDL );
			int iDecoArray = m_vSexList[i].m_vList.size() - 1;

			int iMax = kLoader.LoadInt( "Max", 0 );
			for(int k = 0;k < iMax;k++)
			{
				DecoData kDD;

				ZeroMemory( szBuf, sizeof(szBuf) );
				sprintf_e( szKey, "Name_%d", k + 1 );
				kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
				kDD.m_szName = szBuf;

				ZeroMemory( szBuf, sizeof(szBuf) );
				sprintf_e( szKey, "Icon_%d", k + 1 );
				kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
				kDD.m_szIcon = szBuf;

				ZeroMemory( szBuf, sizeof(szBuf) );
				sprintf_e( szKey, "IconEx_%d", k + 1 );
				kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
				kDD.m_szIconEx = szBuf;

				sprintf_e( szKey, "Manual_%d", k + 1 );
				kDD.m_iManual = kLoader.LoadInt( szKey, 0 );

				sprintf_e( szKey, "InvenManual_%d", k + 1 );
				kDD.m_iInvenManual = kLoader.LoadInt( szKey, 0 );

				sprintf_e( szKey, "Code_%d", k + 1 );
				kDD.m_iDecoCode = kLoader.LoadInt( szKey, 0 );

				sprintf_e( szKey, "Peso_%d", k + 1 );
				kDD.m_iPeso     = kLoader.LoadInt( szKey, 0 );

				sprintf_e( szKey, "Cash_%d", k + 1 );
				kDD.m_iCash     = kLoader.LoadInt( szKey, 0 );

				sprintf_e( szKey, "BonusPeso_%d", k + 1 );
				kDD.m_iBonusPeso= kLoader.LoadInt( szKey, 0 );

				sprintf_e( szKey, "SellPeso_%d", k + 1 );
				kDD.m_iSellPeso= kLoader.LoadInt( szKey, 0 );

				sprintf_e( szKey, "ShowPeso_%d", k + 1 );
				kDD.m_bShowPeso = kLoader.LoadBool( szKey, true );

				sprintf_e( szKey, "LimitLevel_%d", k + 1 );
				kDD.m_iLimitLevel = kLoader.LoadInt( szKey, 0 );

				sprintf_e( szKey, "Active_%d", k + 1 );
				kDD.m_bActive   = kLoader.LoadBool( szKey, false );

				sprintf_e( szKey, "PackageActive_%d", k + 1 );
				kDD.m_bPackageActive   = kLoader.LoadBool( szKey, true );

				sprintf_e( szKey, "ShopOrder_%d", k + 1 );
				kDD.m_iShopOrder= kLoader.LoadInt( szKey, ORDER_DEFAULT );

				sprintf_e( szKey, "Recommended_Shop_Order_%d", k + 1 );
				kDD.m_iRecommendedShopOrder = kLoader.LoadInt( szKey, ORDER_DEFAULT );

				sprintf_e( szKey, "ShopMarkType_%d", k + 1 );
				kDD.m_iShopMarkType = kLoader.LoadInt( szKey, 0 ); // SHOP_MARK_TYPE_NONE

				sprintf_e( szKey, "LimitDate_%d", k + 1 );
				kDD.m_iLimitDate = kLoader.LoadInt( szKey, 0 );

				sprintf_e( szKey, "SubscriptionType_%d", k + 1 );
				kDD.m_iSubscriptionType = kLoader.LoadInt( szKey, SUBSCRIPTION_NONE );

				g_SaleMgr.LoadINI( true, kLoader, ioSaleManager::IT_DECO, kDL.m_iDecoType+(i*1000), kDD.m_iDecoCode, k ); // i*1000은 성별 정보를 넣는다.

				m_vSexList[i].m_vList[iDecoArray].m_vList.push_back( kDD );
			}
		}
	}
}

void ioDecorationPrice::LoadDefaultDecoCodeList()
{
	int iSexSize = m_vSexList.size();

	char szTitle[MAX_PATH];
	char szKey[MAX_PATH];
	for(int i = 0;i < iSexSize;i++)
	{
		ioINILoader kLoader( m_vSexList[i].m_szINI.c_str() );	
		kLoader.SetTitle_e( "Info" );
		int iMaxDeco = kLoader.LoadInt_e( "max_Deco", 0 );
		for(int j = 0;j < iMaxDeco;j++)
		{
			sprintf_e( szTitle, "Default%d", j + 1 );
			kLoader.SetTitle( szTitle );

			DefaultList kDL;
			kDL.m_iDecoType = kLoader.LoadInt_e( "Type", 0 );

			m_vSexList[i].m_vDefaultList.push_back( kDL );
			int iArray = m_vSexList[i].m_vDefaultList.size() - 1;

			int iMax = kLoader.LoadInt( "Max", 0 );
			for(int k = 0;k < iMax;k++)
			{
				sprintf_e( szKey, "Code_%d", k + 1 );
				int iDecoCode = kLoader.LoadInt( szKey, 0 );
				m_vSexList[i].m_vDefaultList[iArray].m_vDecoCodeList.push_back( iDecoCode );
			}
		}
	}
}

int ioDecorationPrice::GetDefaultDecoCode( int iSexType, int iDecoType, DWORD dwRandSeed )
{
	int iSexSize = m_vSexList.size();
	for (int i = 0; i < iSexSize ; i++)
	{
		if( m_vSexList[i].m_iSex != iSexType ) continue;

		int iDefualtSize =  m_vSexList[i].m_vDefaultList.size();
		for (int j = 0; j < iDefualtSize ; j++)
		{
			DefaultList &kList = m_vSexList[i].m_vDefaultList[j];
			if( kList.m_iDecoType != iDecoType )
				continue;
			int iCodeSize = kList.m_vDecoCodeList.size();
			DWORD dwArray = GetRandTableValue( dwRandSeed )%iCodeSize;
			return kList.m_vDecoCodeList[dwArray];
		}
	}

	LOG.PrintTimeAndLog( 0, "ioDecorationPrice::GetDefaultDecoCode Error : %d - %d - %u", iSexType, iDecoType, dwRandSeed );
	return 1;
}

DWORD ioDecorationPrice::GetRandTableValue( DWORD dwRandSeed )
{
	return m_RandTable[dwRandSeed%MAX_RAND_TABLE];
}

const DecoData &ioDecorationPrice::GetDecoData( int iSexType, int iDecoType, int iDecoCode )
{
	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{	
		if( m_vSexList[i].m_iSex == iSexType )
		{
			int iSize = m_vSexList[i].m_vList.size();			
			for(int j = 0;j < iSize;j++)
			{
				DecoList &kList = m_vSexList[i].m_vList[j];
				if( kList.m_iDecoType != iDecoType ) continue;

				int iSize2 = kList.m_vList.size();
				for(int k = 0;k < iSize2;k++)
				{
					DecoData &kDD = kList.m_vList[k];
					if( kDD.m_iDecoCode == iDecoCode )
						return kDD;
				}
			}
		}		
	}

	static DecoData kData;
	return kData;
}

const DecoData *ioDecorationPrice::GetDecoDataPtr( int iSexType, int iDecoType, int iDecoCode )
{
	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{	
		SexDecoList &kSex = m_vSexList[i];
		if( kSex.m_iSex == iSexType )
		{
			int iSize = kSex.m_vList.size();
			for(int j = 0;j < iSize;j++)
			{
				DecoList &kDecoList = kSex.m_vList[j];
				if( kDecoList.m_iDecoType == iDecoType )
				{
					int iDecoSize = kDecoList.m_vList.size();
					for (int k = 0; k < iDecoSize ; k++)
					{
						DecoData &kData = kDecoList.m_vList[k];
						if( kData.m_iDecoCode == iDecoCode )
							return &kData;
					}
				}
			}	
		}		
	}

	return NULL;
}

const int ioDecorationPrice::GetSexSize() const
{
	return m_vSexList.size();
}

const int ioDecorationPrice::GetDecoTypeSize( int iSexArray ) const
{
	if( COMPARE( iSexArray, 0, (int)m_vSexList.size() ) )
		return m_vSexList[iSexArray].m_vList.size();
	
	return 0;
}

const int ioDecorationPrice::GetDecoSize( int iSexArray, int iDecoTypeArray ) const
{
	if( COMPARE( iSexArray, 0, (int)m_vSexList.size() ) )
	{
		if( COMPARE( iDecoTypeArray, 0, (int)m_vSexList[iSexArray].m_vList.size() ) )
			return m_vSexList[iSexArray].m_vList[iDecoTypeArray].m_vList.size();
	}
	return 0;
}

const int ioDecorationPrice::GetSexType( int iSexArray ) const
{
	if( COMPARE( iSexArray, 0, (int)m_vSexList.size() ) )
		return m_vSexList[iSexArray].m_iSex;

	return 0;
}

const int ioDecorationPrice::GetDecoType( int iSexArray, int iDecoTypeArray ) const
{
	if( COMPARE( iSexArray, 0, (int)m_vSexList.size() ) )
	{
		if( COMPARE( iDecoTypeArray, 0, (int)m_vSexList[iSexArray].m_vList.size() ) )
			return m_vSexList[iSexArray].m_vList[iDecoTypeArray].m_iDecoType;
	}

	return 0;
}

const int ioDecorationPrice::GetDecoCode( int iSexArray, int iDecoTypeArray, int iDecoArray ) const
{
	if( COMPARE( iSexArray, 0, (int)m_vSexList.size() ) )
	{
		if( COMPARE( iDecoTypeArray, 0, (int)m_vSexList[iSexArray].m_vList.size() ) )
		{
			if( COMPARE( iDecoArray, 0, (int)m_vSexList[iSexArray].m_vList[iDecoTypeArray].m_vList.size() ) )
				return m_vSexList[iSexArray].m_vList[iDecoTypeArray].m_vList[iDecoArray].m_iDecoCode;
		}
	}

	return 0;
}

const int ioDecorationPrice::GetShopOrder( int iSexArray, int iDecoTypeArray, int iDecoArray ) const
{
	if( COMPARE( iSexArray, 0, (int)m_vSexList.size() ) )
	{
		if( COMPARE( iDecoTypeArray, 0, (int)m_vSexList[iSexArray].m_vList.size() ) )
		{
			if( COMPARE( iDecoArray, 0, (int)m_vSexList[iSexArray].m_vList[iDecoTypeArray].m_vList.size() ) )
				return m_vSexList[iSexArray].m_vList[iDecoTypeArray].m_vList[iDecoArray].m_iShopOrder;
		}
	}

	return  ORDER_DEFAULT;
}

const int ioDecorationPrice::GetRecommandedShopOrder( int iSexArray, int iDecoTypeArray, int iDecoArray ) const
{
	if( COMPARE( iSexArray, 0, (int)m_vSexList.size() ) )
	{
		if( COMPARE( iDecoTypeArray, 0, (int)m_vSexList[iSexArray].m_vList.size() ) )
		{
			if( COMPARE( iDecoArray, 0, (int)m_vSexList[iSexArray].m_vList[iDecoTypeArray].m_vList.size() ) )
				return m_vSexList[iSexArray].m_vList[iDecoTypeArray].m_vList[iDecoArray].m_iRecommendedShopOrder;
		}
	}

	return  ORDER_DEFAULT;
}

const int ioDecorationPrice::GetShopMarkType( int iSexArray, int iDecoTypeArray, int iDecoArray ) const
{
	if( COMPARE( iSexArray, 0, (int)m_vSexList.size() ) )
	{
		if( COMPARE( iDecoTypeArray, 0, (int)m_vSexList[iSexArray].m_vList.size() ) )
		{
			if( COMPARE( iDecoArray, 0, (int)m_vSexList[iSexArray].m_vList[iDecoTypeArray].m_vList.size() ) )
				return m_vSexList[iSexArray].m_vList[iDecoTypeArray].m_vList[iDecoArray].m_iShopMarkType;
		}
	}

	return  0; // SHOP_MARK_TYPE_NONE 
}

const int ioDecorationPrice::GetLimitDate( int iSexArray, int iDecoTypeArray, int iDecoArray ) const
{
	if( COMPARE( iSexArray, 0, (int)m_vSexList.size() ) )
	{
		if( COMPARE( iDecoTypeArray, 0, (int)m_vSexList[iSexArray].m_vList.size() ) )
		{
			if( COMPARE( iDecoArray, 0, (int)m_vSexList[iSexArray].m_vList[iDecoTypeArray].m_vList.size() ) )
				return m_vSexList[iSexArray].m_vList[iDecoTypeArray].m_vList[iDecoArray].m_iLimitDate;
		}
	}

	return  0; // SHOP_MARK_TYPE_NONE 
}

const bool ioDecorationPrice::GetActive( int iSexArray, int iDecoTypeArray, int iDecoArray ) const
{
	if( COMPARE( iSexArray, 0, (int)m_vSexList.size() ) )
	{
		if( COMPARE( iDecoTypeArray, 0, (int)m_vSexList[iSexArray].m_vList.size() ) )
		{
			if( COMPARE( iDecoArray, 0, (int)m_vSexList[iSexArray].m_vList[iDecoTypeArray].m_vList.size() ) )
				return m_vSexList[iSexArray].m_vList[iDecoTypeArray].m_vList[iDecoArray].m_bActive;
		}
	}

	return  false;
}

const bool ioDecorationPrice::GetPackageActive( int iSexArray, int iDecoTypeArray, int iDecoArray ) const
{
	if( COMPARE( iSexArray, 0, (int)m_vSexList.size() ) )
	{
		if( COMPARE( iDecoTypeArray, 0, (int)m_vSexList[iSexArray].m_vList.size() ) )
		{
			if( COMPARE( iDecoArray, 0, (int)m_vSexList[iSexArray].m_vList[iDecoTypeArray].m_vList.size() ) )
				return m_vSexList[iSexArray].m_vList[iDecoTypeArray].m_vList[iDecoArray].m_bPackageActive;
		}
	}

	return  false;
}

int ioDecorationPrice::GetDecoPackageKeepPeso( int iSexArray, int iDecoType )
{
	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{
		if( iSexArray != m_vSexList[i].m_iSex ) continue;

		int iDecoSize = m_vSexList[i].m_vList.size();
		for(int i2 = 0;i2 < iDecoSize;i2++)
		{
			DecoList &kDecoList = m_vSexList[i].m_vList[i2];
			if( kDecoList.m_iDecoType != iDecoType ) continue;

			return kDecoList.m_iPackageKeepPeso;
		}
	}
	LOG.PrintTimeAndLog( 0, "ioDecorationPrice::GetDecoPackageKeepPeso None Keep Peso(%d)!!", iDecoType );
	return 0;
}

void ioDecorationPrice::GetPackageBuyDefaultList( CHARACTER &rkCharInfo, ITEMSLOTVec &rkDecoList )
{
	rkDecoList.clear();
	enum 
	{
		MAX_DECO_ITEM = 5,
	};
	int i;
	int iDecoTypeList[MAX_DECO_ITEM]={ UID_HAIR, UID_HAIR_COLOR, UID_FACE, UID_SKIN_COLOR, UID_UNDERWEAR };
	ITEMSLOTVec vItemSlot;	

	{   
		for(i = 0;i < MAX_DECO_ITEM;i++)
		{
			ITEMSLOT kSlot;
			if( !g_MyInfo.IsClassTypeInventory( rkCharInfo.m_class_type, RDT_HUMAN_MAN, iDecoTypeList[i] ) )   // 이미 해당 부위의 치장을 보유하고 있으면 기본치장 지급 X
			{
				kSlot.m_item_type = ( rkCharInfo.m_class_type * 100000 ) + ( RDT_HUMAN_MAN * 1000 ) + iDecoTypeList[i];
				kSlot.m_item_code = GetDefaultDecoCode( RDT_HUMAN_MAN, iDecoTypeList[i], g_MyInfo.GetPrivateID().GetHashCode() + rkCharInfo.m_class_type + iDecoTypeList[i] );	
				if( !g_MyInfo.IsSlotItem( kSlot.m_item_type, kSlot.m_item_code ) )
				{				
					rkDecoList.push_back( kSlot );
				}
			}		

			// 유저가 여성을 구매한다면 여성 기본 치장도 지급한다.
			if( rkCharInfo.m_sex - 1 != RDT_HUMAN_WOMAN ) continue;					
			if( !g_MyInfo.IsClassTypeInventory( rkCharInfo.m_class_type, RDT_HUMAN_WOMAN, iDecoTypeList[i] ) )   // 이미 해당 부위의 치장을 보유하고 있으면 기본치장 지급 X
			{
				kSlot.m_item_type = ( rkCharInfo.m_class_type * 100000 ) + ( RDT_HUMAN_WOMAN * 1000 ) + iDecoTypeList[i];
				kSlot.m_item_code = GetDefaultDecoCode( RDT_HUMAN_WOMAN, iDecoTypeList[i], g_MyInfo.GetPrivateID().GetHashCode() + rkCharInfo.m_class_type + iDecoTypeList[i] );
				if( !g_MyInfo.IsSlotItem( kSlot.m_item_type, kSlot.m_item_code ) )
				{
					rkDecoList.push_back( kSlot );
				}
			}
		}
	}
}

ITEMSLOT ioDecorationPrice::GetItemSlot( CHARACTER &rkCharInfo, int iDecoType )
{
	//고유 특성 1111 22 333 ( 세트, 종족(성별), 치장타입 )
	ITEMSLOT kSlot;
	kSlot.m_item_type = ( rkCharInfo.m_class_type * 100000 ) + ( ( rkCharInfo.m_sex - 1 )  * 1000 );
	kSlot.m_item_code = 0;
	switch( iDecoType )
	{
	case UID_KINDRED:
		{
			kSlot.m_item_type += iDecoType;
			kSlot.m_item_code =  rkCharInfo.m_sex - 1;
		}
		break;
	case UID_FACE:
		{
			kSlot.m_item_type += iDecoType;
			kSlot.m_item_code =  rkCharInfo.m_face;
		}
		break;
	case UID_HAIR:
		{
			kSlot.m_item_type += iDecoType;
			kSlot.m_item_code =  rkCharInfo.m_hair;
		}
		break;
	case UID_SKIN_COLOR:
		{
			kSlot.m_item_type += iDecoType;
			kSlot.m_item_code =  rkCharInfo.m_skin_color;
		}
		break;
	case UID_HAIR_COLOR:
		{
			kSlot.m_item_type += iDecoType;
			kSlot.m_item_code =  rkCharInfo.m_hair_color;
		}
		break;
	case UID_UNDERWEAR:
		{
			kSlot.m_item_type += iDecoType;
			kSlot.m_item_code = rkCharInfo.m_underwear;
		}	
		break;
	}

	return kSlot;
}

const int ioDecorationPrice::GetShopOrderByType( int iType, int iCode )
{
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;

	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{
		if( iSex != m_vSexList[i].m_iSex ) continue;

		int iDecoSize = m_vSexList[i].m_vList.size();
		for(int i2 = 0;i2 < iDecoSize;i2++)
		{
			DecoList &kDecoList = m_vSexList[i].m_vList[i2];
			if( kDecoList.m_iDecoType != iDecoType ) continue;

			int iDecoCodeSize = kDecoList.m_vList.size();
			for(int i3 = 0;i3 < iDecoCodeSize;i3++)
			{
				DecoData &kDeco = kDecoList.m_vList[i3];
				if( kDeco.m_iDecoCode != iCode ) continue;

				return kDeco.m_iShopOrder;
			}
		}
	}
	
	return  ORDER_DEFAULT;
}

const int ioDecorationPrice::GetRecommendedShopOrderByType( int iType, int iCode )
{
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;

	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{
		if( iSex != m_vSexList[i].m_iSex ) continue;

		int iDecoSize = m_vSexList[i].m_vList.size();
		for(int i2 = 0;i2 < iDecoSize;i2++)
		{
			DecoList &kDecoList = m_vSexList[i].m_vList[i2];
			if( kDecoList.m_iDecoType != iDecoType ) continue;

			int iDecoCodeSize = kDecoList.m_vList.size();
			for(int i3 = 0;i3 < iDecoCodeSize;i3++)
			{
				DecoData &kDeco = kDecoList.m_vList[i3];
				if( kDeco.m_iDecoCode != iCode ) continue;

				return kDeco.m_iRecommendedShopOrder;
			}
		}
	}

	return  ORDER_DEFAULT;
}

const bool ioDecorationPrice::GetActiveByType( int iType, int iCode )
{
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;

	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{
		if( iSex != m_vSexList[i].m_iSex ) continue;

		int iDecoSize = m_vSexList[i].m_vList.size();
		for(int i2 = 0;i2 < iDecoSize;i2++)
		{
			DecoList &kDecoList = m_vSexList[i].m_vList[i2];
			if( kDecoList.m_iDecoType != iDecoType ) continue;

			int iDecoCodeSize = kDecoList.m_vList.size();
			for(int i3 = 0;i3 < iDecoCodeSize;i3++)
			{
				DecoData &kDeco = kDecoList.m_vList[i3];
				if( kDeco.m_iDecoCode != iCode ) continue;

				return kDeco.m_bActive;
			}
		}
	}

	return  false;
}

const bool ioDecorationPrice::GetPackageActiveByType( int iType, int iCode )
{
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;

	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{
		if( iSex != m_vSexList[i].m_iSex ) continue;

		int iDecoSize = m_vSexList[i].m_vList.size();
		for(int i2 = 0;i2 < iDecoSize;i2++)
		{
			DecoList &kDecoList = m_vSexList[i].m_vList[i2];
			if( kDecoList.m_iDecoType != iDecoType ) continue;

			int iDecoCodeSize = kDecoList.m_vList.size();
			for(int i3 = 0;i3 < iDecoCodeSize;i3++)
			{
				DecoData &kDeco = kDecoList.m_vList[i3];
				if( kDeco.m_iDecoCode != iCode ) continue;

				return kDeco.m_bPackageActive;
			}
		}
	}

	return  false;
}

const int ioDecorationPrice::GetDecoCashByType( int iType, int iCode )
{
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;

	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{
		if( iSex != m_vSexList[i].m_iSex ) continue;

		int iDecoSize = m_vSexList[i].m_vList.size();
		for(int i2 = 0;i2 < iDecoSize;i2++)
		{
			DecoList &kDecoList = m_vSexList[i].m_vList[i2];
			if( kDecoList.m_iDecoType != iDecoType ) continue;

			int iDecoCodeSize = kDecoList.m_vList.size();
			for(int i3 = 0;i3 < iDecoCodeSize;i3++)
			{
				DecoData &kDeco = kDecoList.m_vList[i3];
				if( kDeco.m_iDecoCode != iCode ) continue;

				if( !kDeco.m_bActive )
					return 0;
				else
					return kDeco.m_iCash;
			}
		}
	}

	return 0;
}


const int ioDecorationPrice::GetDecoPesoByType( int iType, int iCode )
{
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;

	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{
		if( iSex != m_vSexList[i].m_iSex ) continue;

		int iDecoSize = m_vSexList[i].m_vList.size();
		for(int i2 = 0;i2 < iDecoSize;i2++)
		{
			DecoList &kDecoList = m_vSexList[i].m_vList[i2];
			if( kDecoList.m_iDecoType != iDecoType ) continue;

			int iDecoCodeSize = kDecoList.m_vList.size();
			for(int i3 = 0;i3 < iDecoCodeSize;i3++)
			{
				DecoData &kDeco = kDecoList.m_vList[i3];
				if( kDeco.m_iDecoCode != iCode ) continue;

				if( !kDeco.m_bActive )
					return 0;
				else
					return kDeco.m_iPeso;
			}
		}
	}

	return 0;
}

const int ioDecorationPrice::GetShopMarkTypeByType( int iType, int iCode )
{
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;

	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{
		if( iSex != m_vSexList[i].m_iSex ) continue;

		int iDecoSize = m_vSexList[i].m_vList.size();
		for(int i2 = 0;i2 < iDecoSize;i2++)
		{
			DecoList &kDecoList = m_vSexList[i].m_vList[i2];
			if( kDecoList.m_iDecoType != iDecoType ) continue;

			int iDecoCodeSize = kDecoList.m_vList.size();
			for(int i3 = 0;i3 < iDecoCodeSize;i3++)
			{
				DecoData &kDeco = kDecoList.m_vList[i3];
				if( kDeco.m_iDecoCode != iCode ) continue;

				return kDeco.m_iShopMarkType;
			}
		}
	}

	return  0; // SHOP_MARK_TYPE_NONE 
}

void ioDecorationPrice::SetDecoCashByType( int iType, int iCode, int iCash )
{
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;

	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{
		if( iSex != m_vSexList[i].m_iSex ) continue;

		int iDecoSize = m_vSexList[i].m_vList.size();
		for(int i2 = 0;i2 < iDecoSize;i2++)
		{
			DecoList &kDecoList = m_vSexList[i].m_vList[i2];
			if( kDecoList.m_iDecoType != iDecoType ) continue;

			int iDecoCodeSize = kDecoList.m_vList.size();
			for(int i3 = 0;i3 < iDecoCodeSize;i3++)
			{
				DecoData &kDeco = kDecoList.m_vList[i3];
				if( kDeco.m_iDecoCode != iCode ) continue;

				kDeco.m_iCash = iCash;
				return;
			}
		}
	}
}

void ioDecorationPrice::SetDecoPesoByType( int iType, int iCode, int iPeso )
{
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;

	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{
		if( iSex != m_vSexList[i].m_iSex ) continue;

		int iDecoSize = m_vSexList[i].m_vList.size();
		for(int i2 = 0;i2 < iDecoSize;i2++)
		{
			DecoList &kDecoList = m_vSexList[i].m_vList[i2];
			if( kDecoList.m_iDecoType != iDecoType ) continue;

			int iDecoCodeSize = kDecoList.m_vList.size();
			for(int i3 = 0;i3 < iDecoCodeSize;i3++)
			{
				DecoData &kDeco = kDecoList.m_vList[i3];
				if( kDeco.m_iDecoCode != iCode ) continue;

				kDeco.m_iPeso = iPeso;
				return;
			}
		}
	}
}

void ioDecorationPrice::SetActiveByType( int iType, int iCode, bool bActive )
{
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;

	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{
		if( iSex != m_vSexList[i].m_iSex ) continue;

		int iDecoSize = m_vSexList[i].m_vList.size();
		for(int i2 = 0;i2 < iDecoSize;i2++)
		{
			DecoList &kDecoList = m_vSexList[i].m_vList[i2];
			if( kDecoList.m_iDecoType != iDecoType ) continue;

			int iDecoCodeSize = kDecoList.m_vList.size();
			for(int i3 = 0;i3 < iDecoCodeSize;i3++)
			{
				DecoData &kDeco = kDecoList.m_vList[i3];
				if( kDeco.m_iDecoCode != iCode ) continue;

				kDeco.m_bActive = bActive;
				return;
			}
		}
	}
}

void ioDecorationPrice::SetPackageActiveByType( int iType, int iCode, bool bActive )
{
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;

	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{
		if( iSex != m_vSexList[i].m_iSex ) continue;

		int iDecoSize = m_vSexList[i].m_vList.size();
		for(int i2 = 0;i2 < iDecoSize;i2++)
		{
			DecoList &kDecoList = m_vSexList[i].m_vList[i2];
			if( kDecoList.m_iDecoType != iDecoType ) continue;

			int iDecoCodeSize = kDecoList.m_vList.size();
			for(int i3 = 0;i3 < iDecoCodeSize;i3++)
			{
				DecoData &kDeco = kDecoList.m_vList[i3];
				if( kDeco.m_iDecoCode != iCode ) continue;

				kDeco.m_bPackageActive = bActive;
				return;
			}
		}
	}
}

void ioDecorationPrice::SetShopOrderByType( int iType, int iCode, int iShopOrder )
{
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;

	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{
		if( iSex != m_vSexList[i].m_iSex ) continue;

		int iDecoSize = m_vSexList[i].m_vList.size();
		for(int i2 = 0;i2 < iDecoSize;i2++)
		{
			DecoList &kDecoList = m_vSexList[i].m_vList[i2];
			if( kDecoList.m_iDecoType != iDecoType ) continue;

			int iDecoCodeSize = kDecoList.m_vList.size();
			for(int i3 = 0;i3 < iDecoCodeSize;i3++)
			{
				DecoData &kDeco = kDecoList.m_vList[i3];
				if( kDeco.m_iDecoCode != iCode ) continue;

				kDeco.m_iShopOrder = iShopOrder;
				return;
			}
		}
	}
}

void ioDecorationPrice::SetRecommendedShopOrderByType( int iType, int iCode, int iRecommendShopOrder )
{
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;

	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{
		if( iSex != m_vSexList[i].m_iSex ) continue;

		int iDecoSize = m_vSexList[i].m_vList.size();
		for(int i2 = 0;i2 < iDecoSize;i2++)
		{
			DecoList &kDecoList = m_vSexList[i].m_vList[i2];
			if( kDecoList.m_iDecoType != iDecoType ) continue;

			int iDecoCodeSize = kDecoList.m_vList.size();
			for(int i3 = 0;i3 < iDecoCodeSize;i3++)
			{
				DecoData &kDeco = kDecoList.m_vList[i3];
				if( kDeco.m_iDecoCode != iCode ) continue;

				kDeco.m_iRecommendedShopOrder = iRecommendShopOrder;
				return;
			}
		}
	}
}

void ioDecorationPrice::SetShopMarkTypeByType( int iType, int iCode, int iShopMarkType )
{
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;

	int iSexSize = m_vSexList.size();
	for(int i = 0;i < iSexSize;i++)
	{
		if( iSex != m_vSexList[i].m_iSex ) continue;

		int iDecoSize = m_vSexList[i].m_vList.size();
		for(int i2 = 0;i2 < iDecoSize;i2++)
		{
			DecoList &kDecoList = m_vSexList[i].m_vList[i2];
			if( kDecoList.m_iDecoType != iDecoType ) continue;

			int iDecoCodeSize = kDecoList.m_vList.size();
			for(int i3 = 0;i3 < iDecoCodeSize;i3++)
			{
				DecoData &kDeco = kDecoList.m_vList[i3];
				if( kDeco.m_iDecoCode != iCode ) continue;

				kDeco.m_iShopMarkType = iShopMarkType;
				return;
			}
		}
	}
}


int ioDecorationPrice::GetDecoRecommandedCnt()
{
	int iCount = 0;
	int iSexSize = m_vSexList.size();
	for( int iSex = 0; iSex < iSexSize; ++iSex )
	{
		int iDecoSize = m_vSexList[iSex].m_vList.size();
		for(int iDeco = 0; iDeco < iDecoSize; ++iDeco )
		{
			DecoList &kDecoList = m_vSexList[iSex].m_vList[iDeco];
			int iDecoCodeSize = kDecoList.m_vList.size();
			for(int iDecoCode = 0; iDecoCode < iDecoCodeSize; ++iDecoCode )
			{
				DecoData &kDeco = kDecoList.m_vList[iDecoCode];
				if( kDeco.m_iRecommendedShopOrder != ORDER_DEFAULT )
				{
					iCount++;
				}
			}
		}
	}

	return iCount;
}