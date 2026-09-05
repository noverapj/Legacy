
#include "stdafx.h"
#include "ioBingo.h"
#include "GUI/BingoWnd.h"
#include "GUI/BingoRightWnd.h"
#include "GUI/BingoToolTip.h"

ioBingo::ioBingo()
{
	Initialize();
}

ioBingo::~ioBingo()
{
	Initialize();
	ClearDummyInfo();
}

void ioBingo::Initialize()
{
	m_vNumberList.clear();
	m_vBingoRewardList.clear();

	// 이하 테스트 코드
	for( int i=0; i<BINGO_NUMBER_MAX_COUNT; ++i )
	{
		BingoInfo kInfo;
		kInfo.m_iIndex = i+1;
		kInfo.m_bClear = false;		
		m_vNumberList.push_back( kInfo );
	}

	for( int i=0; i<REWARD_MAX_COUNT; ++i )
	{
		RewardInfo kInfo( i+1, "", "", true, false, 0, 0, 0, 0 );
		m_vBingoRewardList.push_back( kInfo );
	}

	m_vBingoNumber.clear();

	for( int i=0; i<12; ++i )
		m_aBingoLine[i] = false;

	m_iAllBingoRewardIndex = 1;

	m_bSettedData = false;
}

void ioBingo::CreateIconMap()
{
	ClearIconMap();

	int iSize = m_vRewardTable.size();
	for( int i=0; i<iSize; ++i )
	{
		RewardInfo kInfo = m_vRewardTable[i];
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( kInfo.m_IconName );
		m_RewardIconMap.insert( RewardIconMap::value_type( kInfo.m_iIndex, pImg ) );
	}

	iSize = m_vAllBingoRewardTable.size();
	for( int i=0; i<iSize; ++i )
	{
		RewardInfo kInfo = m_vAllBingoRewardTable[i];
		ioUIRenderImage *pImg = g_UIImageSetMgr.CreateImageByFullName( kInfo.m_IconName );
		m_AllBingoRewardIconMap.insert( RewardIconMap::value_type( kInfo.m_iIndex, pImg ) );
	}
}

void ioBingo::ClearIconMap()
{
	RewardIconMap::iterator iterReward;
	for( iterReward = m_RewardIconMap.begin() ; iterReward != m_RewardIconMap.end() ; ++iterReward )
	{
		SAFEDELETE( iterReward->second );
	}
	m_RewardIconMap.clear();

	RewardIconMap::iterator iterAllBingo;
	for( iterAllBingo = m_AllBingoRewardIconMap.begin() ; iterAllBingo != m_AllBingoRewardIconMap.end() ; ++iterAllBingo )
	{
		SAFEDELETE( iterAllBingo->second );
	}
	m_AllBingoRewardIconMap.clear();
}

void ioBingo::ClearDummyInfo()
{
	DummyInfoMap::iterator iter;
	for( iter = m_DummyInfoMap.begin() ; iter != m_DummyInfoMap.end() ; ++iter )
	{		
		SAFEDELETE( iter->second.m_pUnCheckIconImg );
		SAFEDELETE( iter->second.m_pCheckIconImg );
		SAFEDELETE( iter->second.m_pDummyIcon );
		SAFEDELETE( iter->second.m_pSubIcon );
	}
	m_DummyInfoMap.clear();
}

void ioBingo::CheckBingoNumber( OUT int &rkiNumber, OUT bool &rkbClear )
{
	if( rkiNumber > 100 )
		rkbClear = true;
	else
		rkbClear = false;

	rkiNumber = rkiNumber%100;
}

void ioBingo::CheckReward( OUT int &rkiNumber, OUT bool &rkbClear )
{
	if( rkiNumber > 100 )
		rkbClear = true;
	else
		rkbClear = false;

	rkiNumber = rkiNumber%100;
}

void ioBingo::LoadINI()
{
	ioINILoader_e kLoader( "config/sp2_bingo.ini" );
	
	LoadCommon( kLoader );
	LoadDummyInfo( kLoader );
	LoadToolTipManual();
	LoadUIManual();
}

void ioBingo::LoadCommon( ioINILoader_e& rkLoader )
{
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";
	rkLoader.SetTitle_e( "common" );

	m_iMaxNumber = rkLoader.LoadInt_e( "max_bingo_number", 0 );

	rkLoader.LoadString_e( "hidden_icon", "", szBuf, MAX_PATH );
	m_HiddenIconName = szBuf;

	m_iRewardCount = rkLoader.LoadInt_e( "reward_count", 0 );
	m_vRewardTable.clear();
	m_vRewardTable.reserve( m_iRewardCount );

	for( int i = 0; i < m_iRewardCount; ++i )
	{
		wsprintf_e( szBuf, "reward%d", i+1 );
		rkLoader.SetTitle( szBuf );

		char szName[MAX_PATH] = "";
		char szIconName[MAX_PATH] = "";

		rkLoader.LoadString_e( "reward_name", "", szName, MAX_PATH );
		rkLoader.LoadString_e( "reward_icon", "", szIconName, MAX_PATH );

		bool bHidden = rkLoader.LoadBool_e( "reward_hidden", false );
		int iType    = rkLoader.LoadInt_e( "reward_type", 0 );
		int iValue1  = rkLoader.LoadInt_e( "reward_value1", 0 );
		int iValue2  = rkLoader.LoadInt_e( "reward_value2", 0 );

		int iManualIndex = rkLoader.LoadInt_e( "reward_manual", 0 );

		RewardInfo kInfo( i+1, szName, szIconName, bHidden, false, iType, iValue1, iValue2, iManualIndex );
		m_vRewardTable.push_back( kInfo );
	}

	rkLoader.SetTitle_e( "common" );

	m_iAllBingoRewardCount = rkLoader.LoadInt_e( "allbingo_count", 0 );
	m_vAllBingoRewardTable.clear();
	m_vAllBingoRewardTable.reserve(m_iAllBingoRewardCount);

	for( int i=0; i < m_iAllBingoRewardCount; ++i )
	{
		wsprintf_e( szBuf, "allbingo_%d", i+1 );
		rkLoader.SetTitle( szBuf );

		char szName[MAX_PATH] = "";
		char szIconName[MAX_PATH] = "";

		rkLoader.LoadString_e( "reward_name", "", szName, MAX_PATH );
		rkLoader.LoadString_e( "reward_icon", "", szIconName, MAX_PATH );

		bool bHidden = rkLoader.LoadBool_e( "reward_hidden", false );

		int iType   = rkLoader.LoadInt_e( "reward_type", 0 );
		int iValue1 = rkLoader.LoadInt_e( "reward_value1", 0 );
		int iValue2 = rkLoader.LoadInt_e( "reward_value2", 0 );

		int iManualIndex = rkLoader.LoadInt_e( "reward_manual", 0 );

		RewardInfo kInfo( i+1, szName, szIconName, bHidden, false, iType, iValue1, iValue2, iManualIndex );
		m_vAllBingoRewardTable.push_back( kInfo );
	}
}

void ioBingo::LoadDummyInfo( ioINILoader_e& rkLoader )
{
	ClearDummyInfo();
	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	for( int i = 0; i < m_iMaxNumber; ++i )
	{
		wsprintf_e( szKey, "dummy%d", i+1 );
		rkLoader.SetTitle( szKey );
		
		DummyInfo Info;
		rkLoader.LoadString_e( "uncheck_icon", "", szBuf, MAX_PATH );
		SAFEDELETE( Info.m_pUnCheckIconImg );
		Info.m_pUnCheckIconImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );

		rkLoader.LoadString_e( "check_icon", "", szBuf, MAX_PATH );
		SAFEDELETE( Info.m_pCheckIconImg );
		Info.m_pCheckIconImg = g_UIImageSetMgr.CreateImageByFullName( szBuf );

		Info.m_iDummyItemCode = rkLoader.LoadInt_e( "code", 0 );
		Info.m_iManual        = rkLoader.LoadInt_e( "manual", 0 );

		ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( Info.m_iDummyItemCode );
		if( pEtcItem )
		{
			SAFEDELETE( Info.m_pDummyIcon );
			SAFEDELETE( Info.m_pSubIcon );
			Info.m_pDummyIcon = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetIconName() );
			Info.m_pSubIcon = g_UIImageSetMgr.CreateImageByFullName( pEtcItem->GetSubIconName() );
		}

		m_DummyInfoMap.insert( DummyInfoMap::value_type( i+1, Info ) );
	}
}

void ioBingo::LoadToolTipManual()
{
	enum 
	{
		MAX_ITEM = 30,
	};

	ioINILoader_e kLoader( "config/sp2_bingo_manual.ini" );

	kLoader.SetTitle_e( "Info" );

	int iMaxManual = kLoader.LoadInt_e( "MaxManual", 0 );
	iMaxManual++; // 0번은 히든용으로 사용함.
	m_vManualList.clear();
	m_vManualList.reserve( iMaxManual );

	char szTitle[MAX_PATH] = "";

	char szColorKeyName[MAX_PATH] = "";
	char szTextKeyName[MAX_PATH] = "";

	char szColorValue[MAX_PATH]="";
	char szTextValue[MAX_PATH]="";

	char szSpaceLineKeyName[MAX_PATH]="";

	for( int i=0; i<iMaxManual; ++i )
	{
		StringCbPrintf_e( szTitle, sizeof(szTitle), "Manual%d", i );
		kLoader.SetTitle( szTitle );

		vLineText vLineTextList;
		vLineTextList.reserve(MAX_ITEM);

		for(int iLine = 0; iLine < MAX_ITEM; iLine++)
		{
			ColorText kColorText;
			vColorText vColorTextList;
			vColorTextList.reserve(MAX_ITEM);

			wsprintf_e( szSpaceLineKeyName, "%dSpaceLine", iLine+1 );
			kColorText.m_iSpaceLine = kLoader.LoadInt(szSpaceLineKeyName, 0);

			for(int iSentence = 0; iSentence < MAX_ITEM; iSentence++)
			{
				wsprintf_e( szColorKeyName, "%dColor%d", iLine+1, iSentence+1 );
				wsprintf_e( szTextKeyName, "%dText%d", iLine+1, iSentence+1 );
				
				kLoader.LoadString(szColorKeyName, "", szColorValue, MAX_PATH);
				char *pTemp = NULL;
				kColorText.m_dwColor = strtoul(szColorValue, &pTemp, 16);

				kLoader.LoadString( szTextKeyName, "", szTextValue, MAX_PATH );	
				if( strcmp( szTextValue, "" ) == 0) 
					break;
				Help::ChangeCharSet( szTextValue, '#', ' ' );
				kColorText.m_szText = szTextValue;

				vColorTextList.push_back(kColorText);
				kColorText.Clear();
			}
			
			vLineTextList.push_back(vColorTextList);
		}

		m_vManualList.push_back(vLineTextList);
	}
}

void ioBingo::LoadUIManual()
{
	enum 
	{
		MAX_ITEM = 30,
	};

	ioINILoader_e kLoader( "config/sp2_bingo_ui_manual.ini" );
	kLoader.SetTitle_e( "Info" );

	int iMaxManual = kLoader.LoadInt_e( "MaxManual", 0 );
	iMaxManual++; // 0번은 히든용으로 사용함.
	m_vUIManualList.clear();
	m_vUIManualList.reserve( iMaxManual );

	char szTitle[MAX_PATH] = "";

	char szColorKeyName[MAX_PATH] = "";
	char szTextKeyName[MAX_PATH] = "";

	char szColorValue[MAX_PATH]="";
	char szTextValue[MAX_PATH]="";

	char szSpaceLineKeyName[MAX_PATH]="";

	for( int i=0; i<iMaxManual; ++i )
	{
		StringCbPrintf_e( szTitle, sizeof(szTitle), "Manual%d", i );
		kLoader.SetTitle( szTitle );

		vLineText vLineTextList;
		vLineTextList.reserve(MAX_ITEM);

		for(int iLine = 0; iLine < MAX_ITEM; iLine++)
		{
			ColorText kColorText;
			vColorText vColorTextList;
			vColorTextList.reserve(MAX_ITEM);

			wsprintf_e( szSpaceLineKeyName, "%dSpaceLine", iLine+1 );
			kColorText.m_iSpaceLine = kLoader.LoadInt(szSpaceLineKeyName, 0);

			for(int iSentence = 0; iSentence < MAX_ITEM; iSentence++)
			{
				wsprintf_e( szColorKeyName, "%dColor%d", iLine+1, iSentence+1 );
				wsprintf_e( szTextKeyName, "%dText%d", iLine+1, iSentence+1 );

				kLoader.LoadString(szColorKeyName, "", szColorValue, MAX_PATH);
				char *pTemp = NULL;
				kColorText.m_dwColor = strtoul(szColorValue, &pTemp, 16);

				kLoader.LoadString( szTextKeyName, "", szTextValue, MAX_PATH );	
				if( strcmp( szTextValue, "" ) == 0) 
					break;
				Help::ChangeCharSet( szTextValue, '#', ' ' );
				kColorText.m_szText = szTextValue;

				vColorTextList.push_back(kColorText);
				kColorText.Clear();
			}

			vLineTextList.push_back(vColorTextList);
		}

		m_vUIManualList.push_back(vLineTextList);
	}
	
}

void ioBingo::ApplyAllData( SP2Packet &rkPacket, bool bOnlyNumberRoll )
{

	m_vNumberList.clear();
	m_vBingoRewardList.clear();
	m_bSettedData = true;

	for( int i=0; i<12; ++i )
		m_aBingoLine[i] = false;

	int iBingoType;
	rkPacket >> iBingoType;
	m_iBingoType = static_cast<BingoType>( iBingoType );

	int iResultType;
	rkPacket >> iResultType;

	for( int i = 0; i < BINGO_NUMBER_MAX_COUNT; ++i )
	{
		BYTE byNum = 0;
		bool bClear = false;
		int iDummyItemCode = 0;

		rkPacket >> byNum;
		rkPacket >> iDummyItemCode;
		int iNum = static_cast<int>( byNum );
		
		CheckBingoNumber( iNum, bClear );

		BingoInfo kInfo;
		kInfo.m_iIndex = iNum;
		kInfo.m_bClear = bClear;
		kInfo.m_iDummyItemCode = iDummyItemCode;		
		m_vNumberList.push_back( kInfo );
	}

	for( int i=0; i<BINGO_NUMBER_MAX_COUNT; ++i )
	{
		CheckBingoLine( m_vNumberList[i].m_iIndex );
	}

	for( int i=0; i<REWARD_MAX_COUNT; ++i )
	{
		BYTE byRewardIndex = 0;
		rkPacket >> byRewardIndex;
		int iRewardIndex = static_cast<int>( byRewardIndex );
		bool bClear = false;

		CheckReward( iRewardIndex, bClear );

		RewardInfo &kInfo = GetRewardInfo( iRewardIndex );
		kInfo.m_iIndex = iRewardIndex;
		kInfo.m_bClear = bClear;
		m_vBingoRewardList.push_back( kInfo );
	}

	BYTE byRewardIndex = 0;
	bool bClear = false;
	rkPacket >> byRewardIndex;
	m_iAllBingoRewardIndex = static_cast<int>( byRewardIndex );
	CheckReward( m_iAllBingoRewardIndex, bClear );

	CheckBingo( false );
	
	BingoWnd *pWnd = dynamic_cast<BingoWnd*>( g_GUIMgr.FindWnd( BINGO_WND ) );
	if( pWnd )
	{
		pWnd->SetNumber( true );
		pWnd->SetReward( true );

		if( CheckAllBingo() )
		{
			BingoRightWnd *pChild = dynamic_cast<BingoRightWnd*>( pWnd->FindChildWnd( BingoWnd::ID_RIGHT_WND ) );
			if( pChild )
				pChild->SetWindowType( BingoRightWnd::RWT_ALL_BINGO );
		}
		else
		{
			if( iResultType == RESTART_ROLLING )
			{
				pWnd->SetRollingNumber();
				if( !bOnlyNumberRoll )
					pWnd->SetRollingReward();
			}
		}
	}
}

void ioBingo::ApplyGetNumber( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	BYTE byNum = 0;
	rkPacket >> byNum;
	int iNum = static_cast<int>( byNum );

	for( int i=0; i<BINGO_NUMBER_MAX_COUNT; ++i )
	{
		BingoInfo &kInfo = m_vNumberList[i];
		if( kInfo.m_iIndex == iNum )
		{			
			kInfo.m_iIndex = iNum;
			kInfo.m_bClear = true;
		}
	}

	CheckBingo( true );
	BingoWnd *pWnd = dynamic_cast<BingoWnd*>( g_GUIMgr.FindWnd(BINGO_WND) );
	if( pWnd )
	{
		pWnd->SetNumber( false );

		BingoRightWnd *pChild = dynamic_cast<BingoRightWnd*>( pWnd->FindChildWnd( BingoWnd::ID_RIGHT_WND ) );

		if( pChild )
		{
			ioHashString szNewBallImage;
			ioHashString szSubImage;
			int iDummyCode = GetBingoDummyCode( iNum );
			if( 0 < iDummyCode && GetBingoType() == BT_FIXED )
			{
				ioEtcItem* pItem = g_EtcItemMgr.FindEtcItem( iDummyCode );
				if( pItem )
				{
					szNewBallImage = pItem->GetIconName();
					szSubImage = pItem->GetSubIconName();
				}
			}

			pChild->SetBallAni( iNum, this, szNewBallImage, szSubImage );

			if( CheckAllBingo() )
			{
				pChild->SetWindowType( BingoRightWnd::RWT_ALL_BINGO );
			}
		}
	}
}

void ioBingo::ApplyGetAllNumber( SP2Packet &rkPacket )
{
	BingoWnd *pWnd = dynamic_cast<BingoWnd*>( g_GUIMgr.FindWnd(BINGO_WND) );
	if( pWnd )
		pWnd->SetRollingNumber();

	bool bInit = true;

	m_vNumberList.clear();
	for( int i=0; i<BINGO_NUMBER_MAX_COUNT; ++i )
	{
		bool bClear = false;
		BYTE byNum = 0;
		rkPacket >> byNum;
		int iNum = static_cast<int>( byNum );
		
		CheckBingoNumber( iNum, bClear );

		BingoInfo kInfo;
		kInfo.m_iIndex = iNum;
		kInfo.m_bClear = bClear;
		//빙고 이미지 추가

		m_vNumberList.push_back( kInfo );

		if( bClear )
			bInit = false;
	}

	if( bInit )
	{
		for( int i=0; i<12; ++i )
			m_aBingoLine[i] = false;
	}

	CheckBingo( true );

	if( pWnd )
		pWnd->SetNumber( true );
}

void ioBingo::ApplyShuffleReward( SP2Packet &rkPacket )
{
	m_vBingoRewardList.clear();

	for( int i=0; i<REWARD_MAX_COUNT; ++i )
	{
		BYTE byRewardIndex = 0;
		rkPacket >> byRewardIndex;
		int iRewardIndex = static_cast<int>( byRewardIndex );
		bool bClear = false;

		CheckReward( iRewardIndex, bClear );

		RewardInfo &kInfo = GetRewardInfo( iRewardIndex );
		kInfo.m_iIndex = iRewardIndex;
		kInfo.m_bClear = bClear;
		m_vBingoRewardList.push_back( kInfo );
	}

	BYTE byRewardIndex = 0;
	rkPacket >> byRewardIndex;
	m_iAllBingoRewardIndex = static_cast<int>( byRewardIndex );

	BingoWnd *pWnd = dynamic_cast<BingoWnd*>( g_GUIMgr.FindWnd(BINGO_WND) );
	if( pWnd )
	{
		pWnd->SetReward( true );
		pWnd->SetRollingReward();
	}
}

void ioBingo::GetBingoInfo( OUT BingoInfoList &rkNumberInfo )
{
	rkNumberInfo.clear();
	for( int i=0; i<BINGO_NUMBER_MAX_COUNT; ++i )
	{
		rkNumberInfo.push_back( m_vNumberList[i] );
	}
}

void ioBingo::GetAllRewardTable( OUT RewardInfoList &rkRewardTable )
{
	rkRewardTable.clear();
	for( int i=0; i<REWARD_MAX_COUNT; ++i )
	{
		rkRewardTable.push_back( m_vRewardTable[i] );
	}
}

void ioBingo::GetRewardList( OUT RewardInfoList &rkRewardList )
{
	rkRewardList.clear();
	for( int i=0; i<REWARD_MAX_COUNT; ++i )
	{
		rkRewardList.push_back( m_vBingoRewardList[i] );
	}
}

const ioHashString &ioBingo::GetRewardName( int iRewardIndex )
{
	int iSize = m_vRewardTable.size();
	for( int i=0; i<iSize; ++i )
	{
		RewardInfo &kInfo = m_vRewardTable[i];
		if( kInfo.m_iIndex == iRewardIndex )
			return kInfo.m_Name;
	}

	static ioHashString szTemp;
	return szTemp;
}

ioBingo::RewardInfo ioBingo::GetRewardInfo( int iIndex )
{
	int iSize = m_vRewardTable.size();
	for( int i=0; i<iSize; ++i )
	{
		RewardInfo &kInfo = m_vRewardTable[i];
		if( kInfo.m_iIndex == iIndex )
			return kInfo;
	}

	static RewardInfo kInfo( 0, "", "", false, false, 0, 0, 0, 0 );
	return kInfo;
}

const ioBingo::RewardInfo ioBingo::GetAllBingoRewardInfo()
{
	int iSize = m_vAllBingoRewardTable.size();
	for( int i=0; i<iSize; ++i )
	{
		const RewardInfo &kInfo = m_vAllBingoRewardTable[i];
		if( kInfo.m_iIndex == m_iAllBingoRewardIndex )
			return kInfo;
	}

	static RewardInfo kInfo( 0, "", "", false, false, 0, 0, 0, 0 );
	return kInfo;
}

const ioBingo::RewardInfo& ioBingo::GetAllBingoRewardInfo( int Index )
{
	int iSize = m_vAllBingoRewardTable.size();
	for( int i=0; i<iSize; ++i )
	{
		const RewardInfo &kInfo = m_vAllBingoRewardTable[i];
		if( kInfo.m_iIndex == Index )
			return kInfo;
	}

	static RewardInfo kInfo( 0, "", "", false, false, 0, 0, 0, 0 );
	return kInfo;
}

const ioHashString &ioBingo::GetAllBingoRewardName( int iRewardIndex )
{
	int iSize = m_vAllBingoRewardTable.size();
	for( int i=0; i<iSize; ++i )
	{
		RewardInfo &kInfo = m_vAllBingoRewardTable[i];
		if( kInfo.m_iIndex == iRewardIndex )
			return kInfo.m_Name;
	}

	static ioHashString szTemp;
	return szTemp;
}

ioUIRenderImage *ioBingo::GetRewardIcon( int iIndex )
{
	RewardIconMap::iterator iter = m_RewardIconMap.find( iIndex );
	if( iter != m_RewardIconMap.end() )
		return iter->second;
	else
	{
		LOG.PrintTimeAndLog( 0, "ioBingo::GetRewardIcon Not Image : %d", iIndex );
		return NULL;
	}
}

ioUIRenderImage *ioBingo::GetAllBingoRewardIcon( int iIndex /* = 0 */ )
{
	if( iIndex == 0 )
		iIndex = m_iAllBingoRewardIndex;

	RewardIconMap::iterator iter = m_AllBingoRewardIconMap.find( iIndex );
	if( iter != m_AllBingoRewardIconMap.end() )
		return iter->second;
	else
	{
		LOG.PrintTimeAndLog( 0, "ioBingo::GetAllBingoRewardIcon Not Image : %d", iIndex );
		return NULL;
	}
}

bool ioBingo::IsNumberClear( int iIndex )
{
	for( int i=0; i<BINGO_NUMBER_MAX_COUNT; ++i )
	{
		if( m_vNumberList[i].m_iIndex == iIndex )
			return m_vNumberList[i].m_bClear;
	}
	return false;
}

void ioBingo::CheckBingo( bool bEffect )
{
	if( m_vNumberList.empty() )
		return;

	m_vBingoNumber.clear();

	// 편의상 하드코딩으로..

	// 가로 다섯줄 체크
	for( int i=0; i<5; ++i )
	{
		if( m_vNumberList[i*5].m_bClear &&
			m_vNumberList[i*5+1].m_bClear &&
			m_vNumberList[i*5+2].m_bClear &&
			m_vNumberList[i*5+3].m_bClear &&
			m_vNumberList[i*5+4].m_bClear )
		{
			for( int j=0; j<5; ++j )
			{
				int iArray = i * 5 + j;

				IntVec::iterator iter = std::find( m_vBingoNumber.begin(), m_vBingoNumber.end(), m_vNumberList[iArray].m_iIndex );
				if( iter == m_vBingoNumber.end() )
					m_vBingoNumber.push_back( m_vNumberList[iArray].m_iIndex );
			}

			if( !m_vBingoRewardList[i+1].m_bClear )
			{
				m_vBingoRewardList[i+1].m_bClear = true;
			}
		}
	}

	// 세로 다섯줄 체크
	for( int i=0; i<5; ++i )
	{
		if( m_vNumberList[i].m_bClear    &&
			m_vNumberList[i+5].m_bClear  &&
			m_vNumberList[i+10].m_bClear &&
			m_vNumberList[i+15].m_bClear &&
			m_vNumberList[i+20].m_bClear )
		{
			for( int j=0; j<5; ++j )
			{
				int iArray = i + j * 5;

				IntVec::iterator iter = std::find( m_vBingoNumber.begin(), m_vBingoNumber.end(), m_vNumberList[iArray].m_iIndex );
				if( iter == m_vBingoNumber.end() )
					m_vBingoNumber.push_back( m_vNumberList[iArray].m_iIndex );
			}

			if( !m_vBingoRewardList[i+7].m_bClear )
			{
				m_vBingoRewardList[i+7].m_bClear = true;
			}
		}
	}

	// 대각선 두줄 체크
	if( m_vNumberList[0].m_bClear  &&
		m_vNumberList[6].m_bClear  &&
		m_vNumberList[12].m_bClear &&
		m_vNumberList[18].m_bClear &&
		m_vNumberList[24].m_bClear )
	{
		for( int i=0; i<5; ++i )
		{
			int iArray = i * 6;
			
			IntVec::iterator iter = std::find( m_vBingoNumber.begin(), m_vBingoNumber.end(), m_vNumberList[iArray].m_iIndex );
			if( iter == m_vBingoNumber.end() )
				m_vBingoNumber.push_back( m_vNumberList[iArray].m_iIndex );
		}

		if( !m_vBingoRewardList[0].m_bClear )
		{
			m_vBingoRewardList[0].m_bClear = true;
		}
	}
	
	if( m_vNumberList[4].m_bClear  &&
		m_vNumberList[8].m_bClear  &&
		m_vNumberList[12].m_bClear &&
		m_vNumberList[16].m_bClear &&
		m_vNumberList[20].m_bClear )
	{
		for( int i=0; i<5; ++i )
		{
			int iArray = ( i + 1 ) * 4;
			
			IntVec::iterator iter = std::find( m_vBingoNumber.begin(), m_vBingoNumber.end(), m_vNumberList[iArray].m_iIndex );
			if( iter == m_vBingoNumber.end() )
				m_vBingoNumber.push_back( m_vNumberList[iArray].m_iIndex );
		}

		if( !m_vBingoRewardList[6].m_bClear )
		{
			m_vBingoRewardList[6].m_bClear = true;
		}
	}
}

bool ioBingo::CheckAllBingo()
{
	int iSize = m_vBingoNumber.size();
	if( iSize == 25 )
		return true;

	return false;
}

bool ioBingo::CheckBingoLine( int iNumber )
{
	if( iNumber == 0 )
		return false;

	// 가로 다섯줄 체크
	bool bBingo = false;
	for( int i=0; i<5; ++i )
	{
		if( m_vNumberList[i*5].m_bClear   &&
			m_vNumberList[i*5+1].m_bClear &&
			m_vNumberList[i*5+2].m_bClear &&
			m_vNumberList[i*5+3].m_bClear &&
			m_vNumberList[i*5+4].m_bClear )
		{
			for( int j=0; j<5; ++j )
			{
				int iArray = i * 5 + j;
				if( m_vNumberList[iArray].m_iIndex == iNumber )
					bBingo = true;
			}

			if( bBingo )
			{
				SetBingoLine( BINGO_LINE_HORZ1 + i );
				bBingo = false;
			}
		}
	}

	// 세로 다섯줄 체크
	bBingo = false;
	for( int i = 0; i < 5; ++i )
	{
		if( m_vNumberList[i].m_bClear    &&
			m_vNumberList[i+5].m_bClear  &&
			m_vNumberList[i+10].m_bClear &&
			m_vNumberList[i+15].m_bClear &&
			m_vNumberList[i+20].m_bClear )
		{
			for( int j = 0; j < 5; ++j )
			{
				int iArray = i + j * 5;

				if( m_vNumberList[iArray].m_iIndex == iNumber )
					bBingo = true;
			}

			if( bBingo )
			{
				SetBingoLine( BINGO_LINE_VERT1 + i );
				bBingo = false;
			}
		}
	}

	// 대각선 두줄 체크
	bBingo = false;
	if( m_vNumberList[0].m_bClear  &&
		m_vNumberList[6].m_bClear  &&
		m_vNumberList[12].m_bClear &&
		m_vNumberList[18].m_bClear &&
		m_vNumberList[24].m_bClear )
	{
		for( int i=0; i<5; ++i )
		{
			int iArray = i * 6;

			if( m_vNumberList[iArray].m_iIndex == iNumber )
				bBingo = true;
		}

		if( bBingo )
		{
			SetBingoLine( BINGO_LINE_LEFT_TOP );
		}
	}

	bBingo = false;
	if( m_vNumberList[4].m_bClear  &&
		m_vNumberList[8].m_bClear  &&
		m_vNumberList[12].m_bClear &&
		m_vNumberList[16].m_bClear &&
		m_vNumberList[20].m_bClear )
	{
		for( int i = 0; i < 5; ++i )
		{
			int iArray = ( i + 1 ) * 4;

			if( m_vNumberList[iArray].m_iIndex == iNumber )
				bBingo = true;
		}

		if( bBingo )
		{
			SetBingoLine( BINGO_LINE_LEFT_BOTTOM );
		}
	}

	return true;
}

void ioBingo::SetBingoLine( int iLineIndex )
{
	if( iLineIndex > BINGO_LINE_VERT5 )
		return;

	if( !m_aBingoLine[iLineIndex] )
	{
		m_aBingoLine[iLineIndex] = true;

		// 빙고 연출..
		BingoWnd *pWnd = dynamic_cast<BingoWnd*>( g_GUIMgr.FindWnd(BINGO_WND) );
		if( pWnd )
		{
			pWnd->SetBingo( iLineIndex );

			RewardButton *pBtn = dynamic_cast<RewardButton*>( pWnd->FindChildWnd(BingoWnd::ID_REWARD_BTN_1+iLineIndex) );
			if( pBtn )
				pBtn->SetClear();
		}
	}
}

bool ioBingo::IsNumberBingo( int iIndex )
{
	IntVec::iterator iter = m_vBingoNumber.begin();
	for( ; iter != m_vBingoNumber.end(); ++iter )
	{
		if( iIndex == (*iter) )
			return true;
	}

	return false;
}

bool ioBingo::IsRewardHidden( int iIndex )
{
	RewardInfoList::iterator iter = m_vBingoRewardList.begin();
	for( ; iter != m_vBingoRewardList.end(); ++iter )
	{
		RewardInfo kInfo = *iter;
		if( kInfo.m_iIndex == iIndex && kInfo.m_bClear == false )
			return kInfo.m_bHidden;
	}

	return false;
}

bool ioBingo::IsRewardClear( int iIndex )
{
	RewardInfoList::iterator iter = m_vBingoRewardList.begin();
	for( ; iter != m_vBingoRewardList.end(); ++iter )
	{
		RewardInfo kInfo = *iter;
		if( kInfo.m_iIndex == iIndex )
			return kInfo.m_bClear;
	}

	return false;
}

int ioBingo::GetToolTipMaxLine( int iIndex )
{
	if(! COMPARE( iIndex, 0, (int)m_vManualList.size() ) )
		return 0;

	return (int)m_vManualList[iIndex].size();
}

int ioBingo::GetToolTipGetSpaceLine( int iIndex, int iCurLine )
{
	if(! COMPARE( iIndex, 0, (int)m_vManualList.size() ) )
		return 0;

	if(! COMPARE( iCurLine, 0, (int)m_vManualList[iIndex].size() ) )
		return 0;

	vColorText &rvColorTextList = m_vManualList[iIndex][iCurLine];
	if(rvColorTextList.empty())
		return 0;

	return rvColorTextList[0].m_iSpaceLine;
}

void ioBingo::SetPrinter( int iIndex, int iCurLine, float fScale, OUT ioComplexStringPrinter &rkPrinter )
{
	if( !COMPARE( iIndex, 0, (int) m_vManualList.size() ) )
		return;

	if( !COMPARE( iCurLine, 0, (int) m_vManualList[iIndex].size() ) )
		return;

	rkPrinter.SetTextStyle( TS_NORMAL );
	rkPrinter.SetBkColor( 0, 0, 0 );	

	vColorText &rvColorTextList = m_vManualList[iIndex][iCurLine];

	int iMaxSentence = (int) rvColorTextList.size();
	for( int i=0; i<iMaxSentence; ++i )
	{
		rkPrinter.SetTextColor( rvColorTextList[i].m_dwColor );
		rkPrinter.AddTextPiece( fScale, rvColorTextList[i].m_szText.c_str() );
	}
}

void ioBingo::SetUIPrinter( int iType, int iCurLine, float fScale, OUT ioComplexStringPrinter &rkPrinter )
{
	if( !COMPARE( iType, 0, (int) m_vManualList.size() ) )
		return;

	if( !COMPARE( iCurLine, 0, (int) m_vUIManualList[iType].size() ) )
		return;

	rkPrinter.SetTextStyle( TS_NORMAL );
	rkPrinter.SetBkColor( 0, 0, 0 );	

	vColorText &rvColorTextList = m_vUIManualList[iType][iCurLine];

	int iMaxSentence = (int) rvColorTextList.size();
	for( int i=0; i<iMaxSentence; ++i )
	{
		char szString[MAX_PATH] = "";
		strcpy_s( szString, sizeof(szString), rvColorTextList[i].m_szText.c_str() );
		ParseString( szString );

		rkPrinter.SetTextColor( rvColorTextList[i].m_dwColor );
		rkPrinter.AddTextPiece( fScale, szString );
	}
}

void ioBingo::ParseString( char *szText )
{
	char szLeft[MAX_PATH] = "", szRight[MAX_PATH] = "", szTag[MAX_PATH] = "";
	bool bCheck = false;
	int iOffset = 0;
	int iLen = strlen( szText );
	
	for( int i=0; i<iLen; ++i )
	{
		if( szText[i] == '<' )
			break;
		szLeft[iOffset] = szText[i];
		iOffset++;
	}
	iOffset = 0;

	for( int i=0; i<iLen; ++i )
	{
		if( szText[i] == '>' )
		{
			bCheck = true;
			continue;
		}

		if( bCheck )
		{
			szRight[iOffset] = szText[i];
			iOffset++;
		}
	}
	iOffset = 0;

	bCheck = false;
	for( int i=0; i<iLen; ++i )
	{
		if( !bCheck && szText[i] == '<' )
		{
			bCheck = true;
			continue;
		}
		else if( bCheck && szText[i] == '>' )
		{
			bCheck = false;
			continue;
		}

		if( bCheck )
		{
			szTag[iOffset] = szText[i];
			iOffset++;
		}
	}

	int iTag = strlen( szTag );
	ioHashString szRewardName;
	if( iTag > 0 )
	{
		if( strcmp( szTag, "ITEM" ) == 0 )
			szRewardName = GetAllBingoRewardName( m_iAllBingoRewardIndex );
		else
			szRewardName = "<UnknownTag>";
	}

	memset( szText, 0, sizeof(szText) );
	wsprintf_e( szText, "%s%s%s", szLeft, szRewardName.c_str(), szRight );
}

ioUIRenderImage* ioBingo::GetBingoDummyCheckIcon( int iNumber )
{
	DummyInfoMap::iterator iter = m_DummyInfoMap.find( iNumber );
	if( iter != m_DummyInfoMap.end() )
		return iter->second.m_pCheckIconImg;

	return NULL;
}

ioUIRenderImage* ioBingo::GetBingoDummyUnCheckIcon( int iNumber )
{
	DummyInfoMap::iterator iter = m_DummyInfoMap.find( iNumber );
	if( iter != m_DummyInfoMap.end() )
		return iter->second.m_pUnCheckIconImg;

	return NULL;
}

ioUIRenderImage* ioBingo::GetBingoDummyIcon( int iNumber )
{
	DummyInfoMap::iterator iter = m_DummyInfoMap.find( iNumber );
	if( iter != m_DummyInfoMap.end() )
		return iter->second.m_pDummyIcon;

	return NULL;
}

ioUIRenderImage* ioBingo::GetBingoSubIcon( int iNumber )
{
	DummyInfoMap::iterator iter = m_DummyInfoMap.find( iNumber );
	if( iter != m_DummyInfoMap.end() )
		return iter->second.m_pSubIcon;

	return NULL;
}

const ioHashString& ioBingo::GetBingoDummyName( int iNumber )
{
	static ioHashString szNullStr;
	DummyInfoMap::iterator iter = m_DummyInfoMap.find( iNumber );
	if( iter != m_DummyInfoMap.end() )
	{
		const DummyInfo& rkInfo = iter->second;
		ioEtcItem* pEtcItem = g_EtcItemMgr.FindEtcItem( rkInfo.m_iDummyItemCode );
		if( pEtcItem )
		{
			return pEtcItem->GetName();
		}
	}	
	
	return szNullStr;
}

int ioBingo::GetBingoDummyCode( int iNumber )
{
	DummyInfoMap::iterator iter = m_DummyInfoMap.find( iNumber );
	if( iter != m_DummyInfoMap.end() )
		return iter->second.m_iDummyItemCode;

	return 0;
}

int ioBingo::GetBingoDummyManualIndex( int iNumber )
{
	DummyInfoMap::iterator iter = m_DummyInfoMap.find( iNumber );
	if( iter != m_DummyInfoMap.end() )
		return iter->second.m_iManual;

	return 0;
}

int ioBingo::GetBingoDummyNumberOfCurrent()
{
	ioUserEtcItem* pUserEtc = g_MyInfo.GetUserEtcItem();
	if( !pUserEtc )
		return 0;

	int iCount = 0;
	for( DummyInfoMap::iterator iter = m_DummyInfoMap.begin(); iter != m_DummyInfoMap.end(); ++iter )
	{
		if( !IsNumberClear( iter->first ) )
		{
			const DummyInfo& rkInfo = iter->second;

			ioUserEtcItem::ETCITEMSLOT rkEtcItem;
			if( pUserEtc->GetEtcItem( rkInfo.m_iDummyItemCode, rkEtcItem ) )
			{
				iCount++;
			}
		}
	}

	return iCount;
}

bool ioBingo::HasBingoDummy( int iNumber )
{
	ioUserEtcItem* pUserEtc = g_MyInfo.GetUserEtcItem();
	if( !pUserEtc )
		return false;
	
	DummyInfoMap::iterator iter = m_DummyInfoMap.find( iNumber );
	if( iter != m_DummyInfoMap.end() )
	{
		const DummyInfo& rkInfo = iter->second;
		ioUserEtcItem::ETCITEMSLOT rkEtcItem;
		if( pUserEtc->GetEtcItem( rkInfo.m_iDummyItemCode, rkEtcItem ) )
		{
			return true;
		}
	}

	return false;
}