#include "stdafx.h"

#include "ioDecorationPrice.h"
#include "iosalemanager.h"
#include <string.h>
#include "../EtcHelpFunc.h"

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

void ioDecorationPrice::LoadPriceInfo( bool bCreateLoad /*= true */ )
{
	if( bCreateLoad )
		ClearSex();

	ioINILoader kLoader;
	if( bCreateLoad )
		kLoader.LoadFile( "config/sp2_deco_sex.ini" );
	else
		kLoader.ReloadFile( "config/sp2_deco_sex.ini" );

	kLoader.SetTitle( "Info" );

	int i = 0;
	int max_sex = kLoader.LoadInt( "max_sex", 0 );

	char szTitle[MAX_PATH];
	char szBuf[MAX_PATH];
	for(i = 0;i < max_sex;i++)
	{
		sprintf_s( szTitle, "Sex%d", i + 1 );
		kLoader.SetTitle( szTitle );

		SexDecoList kSexList;

		kSexList.m_iSex = kLoader.LoadInt( "Sex", 0 );

		kLoader.LoadString( "Name", "", szBuf, MAX_PATH );
		kSexList.m_szName = szBuf;

		kLoader.LoadString( "Deco_ini", "", szBuf, MAX_PATH );
		kSexList.m_szINI = szBuf;

		if( bCreateLoad )
			m_vSexList.push_back( kSexList );
	}

	LoadDecoInfo( bCreateLoad );
	LoadDefaultDecoCodeList( bCreateLoad );
}

void ioDecorationPrice::LoadDecoInfo( bool bCreateLoad )
{
	int iSexSize = m_vSexList.size();

	char szTitle[MAX_PATH];
	char szKey[MAX_PATH];
	char szBuf[MAX_PATH];
	for(int i = 0;i < iSexSize;i++)
	{
		ioINILoader kLoader( m_vSexList[i].m_szINI.c_str() );	
		kLoader.SetTitle( "Info" );
		int iMaxDeco = kLoader.LoadInt( "max_Deco", 0 );
		for(int j = 0;j < iMaxDeco;j++)
		{
			sprintf_s( szTitle, "Deco%d", j + 1 );
			kLoader.SetTitle( szTitle );

			DecoList kDL;
			kDL.m_iDecoType = kLoader.LoadInt( "Type", 0 );
			kDL.m_iPackageKeepPeso = kLoader.LoadInt( "PackageKeepPeso", 3000 );

			if( bCreateLoad )
				m_vSexList[i].m_vList.push_back( kDL );
			else
			{
				if( COMPARE( j, 0 , (int)m_vSexList[i].m_vList.size() ) )
				{
					m_vSexList[i].m_vList[j].m_iDecoType        = kDL.m_iDecoType;
					m_vSexList[i].m_vList[j].m_iPackageKeepPeso = kDL.m_iPackageKeepPeso;
				}
			}

			int iDecoArray = j;

			int iMax = kLoader.LoadInt( "Max", 0 );
			for(int k = 0;k < iMax;k++)
			{
				DecoData kDD;

				sprintf_s( szKey, "Name_%d", k + 1 );
				kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
				kDD.m_szName = szBuf;

				sprintf_s( szKey, "Code_%d", k + 1 );
				kDD.m_iDecoCode = kLoader.LoadInt( szKey, 0 );

				sprintf_s( szKey, "Peso_%d", k + 1 );
				kDD.m_iPeso     = kLoader.LoadInt( szKey, 0 );

				sprintf_s( szKey, "Cash_%d", k + 1 );
				kDD.m_iCash     = kLoader.LoadInt( szKey, 0 );

				sprintf_s( szKey, "BonusPeso_%d", k + 1 );
				kDD.m_iBonusPeso= kLoader.LoadInt( szKey, 0 );

				sprintf_s( szKey, "SellPeso_%d", k + 1 );
				kDD.m_iSellPeso = kLoader.LoadInt( szKey, 0 );

				sprintf_s( szKey, "LimitLevel_%d", k + 1 );
				kDD.m_iLimitLevel = kLoader.LoadInt( szKey, 0 );

				sprintf_s( szKey, "SubscriptionType_%d", k + 1 );
				kDD.m_iSubscriptionType = kLoader.LoadInt( szKey, 0 );

				sprintf_s( szKey, "Active_%d", k + 1 );
				kDD.m_bActive   = kLoader.LoadBool( szKey, false );

				g_SaleMgr.LoadINI( bCreateLoad, kLoader, ioSaleManager::IT_DECO, kDL.m_iDecoType+(i*1000), kDD.m_iDecoCode, k ); // i*1000은 성별 타입을 넣는다.

				if( bCreateLoad )
					m_vSexList[i].m_vList[iDecoArray].m_vList.push_back( kDD );
				else
				{
					if( COMPARE( k , 0, (int) m_vSexList[i].m_vList[iDecoArray].m_vList.size() ) )
					{
						m_vSexList[i].m_vList[iDecoArray].m_vList[k].m_iDecoCode = kDD.m_iDecoCode;
						m_vSexList[i].m_vList[iDecoArray].m_vList[k].m_iPeso     = kDD.m_iPeso;
						m_vSexList[i].m_vList[iDecoArray].m_vList[k].m_iCash     = kDD.m_iCash;
						m_vSexList[i].m_vList[iDecoArray].m_vList[k].m_iBonusPeso= kDD.m_iBonusPeso;
						m_vSexList[i].m_vList[iDecoArray].m_vList[k].m_iSellPeso = kDD.m_iSellPeso;
						m_vSexList[i].m_vList[iDecoArray].m_vList[k].m_iLimitLevel= kDD.m_iLimitLevel;
						m_vSexList[i].m_vList[iDecoArray].m_vList[k].m_bActive   = kDD.m_bActive;
					}
				}
			}
		}
	}

	// test
	if( false )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Start LoadDecoInfo :%d", (int) bCreateLoad );

		int iSexSize = m_vSexList.size();
		for(int i = 0;i < iSexSize;i++)
		{
			int iMaxDeco = m_vSexList[i].m_vList.size();
			for(int j = 0;j < iMaxDeco;j++)
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Type            = %d", m_vSexList[i].m_vList[j].m_iDecoType );
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "PackageKeepPeso = %d", m_vSexList[i].m_vList[j].m_iPackageKeepPeso );

				int iMax = m_vSexList[i].m_vList[j].m_vList.size();
				for(int k = 0;k < iMax;k++)
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Name_%d      = %s", k + 1, m_vSexList[i].m_vList[j].m_vList[k].m_szName.c_str() );
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Code_%d      = %d", k + 1, m_vSexList[i].m_vList[j].m_vList[k].m_iDecoCode );
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Peso_%d      = %d", k + 1, m_vSexList[i].m_vList[j].m_vList[k].m_iPeso );
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Cash_%d      = %d", k + 1, m_vSexList[i].m_vList[j].m_vList[k].m_iCash );
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "BonusPeso_%d = %d", k + 1,  m_vSexList[i].m_vList[j].m_vList[k].m_iBonusPeso );
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "SellPeso_%d  = %d", k + 1, m_vSexList[i].m_vList[j].m_vList[k].m_iSellPeso );
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "LimitLevel_%d= %d", k + 1, m_vSexList[i].m_vList[j].m_vList[k].m_iLimitLevel );
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Active_%d    = %d", k + 1, m_vSexList[i].m_vList[j].m_vList[k].m_bActive );
				}
			}
		}

		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "End LoadDecoInfo :%d", (int) bCreateLoad );
	}
	//
}

void ioDecorationPrice::LoadDefaultDecoCodeList( bool bCreateLoad )
{
	int iSexSize = m_vSexList.size();

	char szTitle[MAX_PATH];
	char szKey[MAX_PATH];
	for(int i = 0;i < iSexSize;i++)
	{
		ioINILoader kLoader( m_vSexList[i].m_szINI.c_str() );	
		kLoader.SetTitle( "Info" );
		int iMaxDeco = kLoader.LoadInt( "max_Deco", 0 );
		for(int j = 0;j < iMaxDeco;j++)
		{
			sprintf_s( szTitle, "Default%d", j + 1 );
			kLoader.SetTitle( szTitle );

			DefaultList kDL;
			kDL.m_iDecoType = kLoader.LoadInt( "Type", 0 );

			if( bCreateLoad )
				m_vSexList[i].m_vDefaultList.push_back( kDL );
			else
			{
				if( COMPARE( j , 0, (int) m_vSexList[i].m_vDefaultList.size() ) )
				{
					m_vSexList[i].m_vDefaultList[j].m_iDecoType = kDL.m_iDecoType;
				}
			}

			int iArray = j;

			int iMax = kLoader.LoadInt( "Max", 0 );
			for(int k = 0;k < iMax;k++)
			{
				sprintf_s( szKey, "Code_%d", k + 1 );
				int iDecoCode = kLoader.LoadInt( szKey, 0 );
				if( bCreateLoad )
					m_vSexList[i].m_vDefaultList[iArray].m_vDecoCodeList.push_back( iDecoCode );
				else
				{
					if( COMPARE( k, 0, (int) m_vSexList[i].m_vDefaultList[iArray].m_vDecoCodeList.size() ) )
					{
						m_vSexList[i].m_vDefaultList[iArray].m_vDecoCodeList[k] = iDecoCode;
					}
				}
			}
		}

		SpecialDefaultDecoInfo stInfo;

		for( int k = 0; k < 1000; k++ )
		{
			wsprintf( szTitle, "special_default%d", k+1 );
			kLoader.SetTitle( szTitle );


			kLoader.LoadString("class_type", "", szKey, MAX_PATH);
			
			if( strcmp(szKey, "") == 0 )
				break;

			//value 파싱
			IntVec vValues;
			Help::TokenizeToINT(szKey, ",", vValues);

			stInfo.m_iFaceCode	= kLoader.LoadInt( "face_code", 0 );
			stInfo.m_iHairCode	= kLoader.LoadInt( "hair_code", 0 );
			stInfo.m_iSkinColor	= kLoader.LoadInt( "skin_color", 0 );
			stInfo.m_iHairColor	= kLoader.LoadInt( "hair_color", 0 );
			stInfo.m_iUnderwearCode	= kLoader.LoadInt( "underwear", 0 );

			for( int j = 0; j < (int)vValues.size(); j++ )
			{
				if( 0 == i )
					m_mSpecialManDecoInfo.insert( std::make_pair(vValues[j], stInfo) );
				else
					m_mSpecialWomanDecoInfo.insert( std::make_pair(vValues[j], stInfo) );
			}
				
		}
	}

	// test
	if( false )
	{
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Start LoadDefaultDecoCodeList :%d", (int) bCreateLoad );
		int iSexSize = m_vSexList.size();
		for(int i = 0;i < iSexSize;i++)
		{
			int iMaxDeco =  m_vSexList[i].m_vDefaultList.size();
			for(int j = 0;j < iMaxDeco;j++)
			{
				LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Type    = %d", m_vSexList[i].m_vDefaultList[j].m_iDecoType );
			
				int iArray = j;
				int iMax   = m_vSexList[i].m_vDefaultList[iArray].m_vDecoCodeList.size();
				for(int k = 0;k < iMax;k++)
				{
					LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Code_%d = %d", k + 1, m_vSexList[i].m_vDefaultList[iArray].m_vDecoCodeList[k] );	
				}
			}
		}
		LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "Start LoadDefaultDecoCodeList :%d", (int) bCreateLoad );
	}
	//
}

int ioDecorationPrice::GetDecoPackageKeepPeso( int iType )
{
	int iClassType = iType / 100000;
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

			return kDecoList.m_iPackageKeepPeso;
		}
	}
	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioDecorationPrice::GetDecoPackageKeepPeso None Keep Peso(%d)!!", iType );
	return 0;
}

int ioDecorationPrice::GetDecoPeso( int iType, int iCode, int iClassLevel )
{
	int iClassType = iType / 100000;
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;
	int iDecoCode  = iCode;

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
				if( kDeco.m_iDecoCode != iDecoCode ) continue;

				if( !kDeco.m_bActive )
					return -2;
				else if( iClassLevel < kDeco.m_iLimitLevel )
					return -1;
				else
					return kDeco.m_iPeso;
			}
		}
	}
	return 0;
}

int ioDecorationPrice::GetSubscriptionType( int iType, int iCode )
{
	int iClassType = iType / 100000;
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;
	int iDecoCode  = iCode;

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
				if( kDeco.m_iDecoCode != iDecoCode ) continue;

				if( !kDeco.m_bActive )
					return SUBSCRIPTION_NONE;
				else
					return kDeco.m_iSubscriptionType;
			}
		}
	}

	return SUBSCRIPTION_NONE;
}

int ioDecorationPrice::GetDecoCash( int iType, int iCode )
{
	int iClassType = iType / 100000;
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;
	int iDecoCode  = iCode;

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
				if( kDeco.m_iDecoCode != iDecoCode ) continue;

				if( !kDeco.m_bActive )
					return -2;
				else
					return kDeco.m_iCash;
			}
		}
	}
	return 0;
}

bool ioDecorationPrice::GetDecoActive( int iType, int iCode )
{
	int iClassType = iType / 100000;
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;
	int iDecoCode  = iCode;

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
				if( kDeco.m_iDecoCode != iDecoCode ) continue;

				return kDeco.m_bActive;
			}
		}
	}
	return false;
}

int ioDecorationPrice::GetBonusPeso( int iType, int iCode )
{
	int iClassType = iType / 100000;
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;
	int iDecoCode  = iCode;

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
				if( kDeco.m_iDecoCode != iDecoCode ) continue;

				if( !kDeco.m_bActive )
					return -2;
				else
					return kDeco.m_iBonusPeso;
			}
		}
	}
	return 0;
}

int ioDecorationPrice::GetSellPeso( int iType, int iCode )
{
	int iClassType = iType / 100000;
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;
	int iDecoCode  = iCode;

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
				if( kDeco.m_iDecoCode != iDecoCode ) continue;
/*팔때는 아무때나 팔려야한다.
				if( !kDeco.m_bActive )
					return -2;
				else
*/
					return kDeco.m_iSellPeso;
			}
		}
	}
	return 0;
}

int ioDecorationPrice::GetDefaultDecoCode( int iSexType, int iDecoType, DWORD dwRandSeed, int iClassType )
{
	SpecialDefaultDecoInfo stInfo;

	GetSpecialDecoInfo(iSexType, iClassType, stInfo);
	if( stInfo.m_iFaceCode	!= 0 )
	{
		switch( iDecoType )
		{
		case UID_FACE:
			return stInfo.m_iFaceCode;

		case UID_HAIR:
			return stInfo.m_iHairCode;

		case UID_SKIN_COLOR:
			return stInfo.m_iSkinColor;

		case UID_HAIR_COLOR:
			return stInfo.m_iHairColor;

		case UID_UNDERWEAR:
			return stInfo.m_iUnderwearCode;
		}
	}
	else
	{
		int iSexSize = m_vSexList.size();
		for (int i = 0; i < iSexSize ; i++)
		{
			if( m_vSexList[i].m_iSex != iSexType ) continue;

			int iDefualtSize =  m_vSexList[i].m_vDefaultList.size();
			for (int j = 0; j < iDefualtSize ; j++)
			{
				DefaultList &kList = m_vSexList[i].m_vDefaultList[j];			
				if( kList.m_iDecoType != iDecoType ) continue;

				int iCodeSize = kList.m_vDecoCodeList.size();
				DWORD dwArray = GetRandTableValue( dwRandSeed )%iCodeSize;
				return kList.m_vDecoCodeList[dwArray];
			}
		}
	}

	LOG.PrintTimeAndLog( LOG_DEBUG_LEVEL, "ioDecorationPrice::GetDefaultDecoCode Error : %d - %d - %u", iSexType, iDecoType, dwRandSeed );
	return 1;
}

DWORD ioDecorationPrice::GetRandTableValue( DWORD dwRandSeed )
{
	return m_RandTable[dwRandSeed%MAX_RAND_TABLE];
}

void ioDecorationPrice::SetDecoCash( int iType, int iCode, int iCash )
{
	int iClassType = iType / 100000;
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;
	int iDecoCode  = iCode;

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
				if( kDeco.m_iDecoCode != iDecoCode ) continue;

				kDeco.m_iCash = iCash;
				return;
			}
		}
	}
}

void ioDecorationPrice::SetDecoPeso( int iType, int iCode, int iPeso )
{
	int iClassType = iType / 100000;
	int iSex       = ( iType % 100000) / 1000;
	int iDecoType  = iType % 1000;
	int iDecoCode  = iCode;

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
				if( kDeco.m_iDecoCode != iDecoCode ) continue;

				kDeco.m_iPeso = iPeso;
				return;
			}
		}
	}
}

void ioDecorationPrice::SetDecoActive( int iType, int iCode, bool bActive )
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

void ioDecorationPrice::GetSpecialDecoInfo(int iGender, int iClassType, SpecialDefaultDecoInfo& stInfo)
{
	SPECIALDEFAULTINFO_iter it;

	if( iGender == 0 )
	{
		it	= m_mSpecialManDecoInfo.find(iClassType);
		if( it	== m_mSpecialManDecoInfo.end() )
			return;
	}
	else if( iGender == 1 )
	{
		it	= m_mSpecialWomanDecoInfo.find(iClassType);
		if( it	== m_mSpecialWomanDecoInfo.end() )
			return;
	}
	else
		return;

	stInfo = it->second;
}