

#include "StdAfx.h"

#include "../io3DEngine/ioFontManager.h"
#include "../io3DEngine/ioINILoader.h" 
#include "../io3DEngine/ioWnd.h"
#include "../io3DEngine/ioScroll.h"
#include "../io3DEngine/ioUIImageSetManager.h"
#include "../io3DEngine/ioUIRenderImage.h"
#include "../io3DEngine/ioSoundManager.h"

#include "../TextColorTable.h"
#include "../EtcHelpFunc.h"
#include "../ioMyInfo.h"
#include "../ioQuestManager.h"
#include "../ioApplication.h"
#include "../WndID.h"

#include "ioSP2GUIManager.h"
#include "ManualWnd.h"
#include <strsafe.h>
#include "../io3DEngine/Safesprintf.h"
#include "../io3DEngine/ioStringManager.h"

ManualMgr::ManualMgr()
{
}

ManualMgr::~ManualMgr()
{
	ClearList();
}

void ManualMgr::LoadManual( const char* szININame )
{
	ClearList();

	ioINILoader kLoader( szININame );

	kLoader.SetTitle_e( "Info" );

	int iMaxManual = kLoader.LoadInt_e( "MaxManual", 0 );
	m_vManualList.reserve( iMaxManual );

	char szTitle[MAX_PATH] = "";

	char szIconKeyName[MAX_PATH] = "";
	char szColorKeyName[MAX_PATH] = "";
	char szTextKeyName[MAX_PATH] = "";

	char szIconValue[MAX_PATH]="";
	char szColorValue[MAX_PATH]="";
	char szTextValue[MAX_PATH]="";

	char szSpaceLineKeyName[MAX_PATH]="";

	for(int i = 0;i < iMaxManual;i++)
	{
		StringCbPrintf_e( szTitle, sizeof(szTitle), "Manual%d", i+1 );
		kLoader.SetTitle( szTitle );

		vLineText vLineTextList;
		vLineTextList.reserve(MAX_ITEM);

		for(int iLine = 0; iLine < MAX_ITEM; iLine++)
		{
			ColorText kColorText;
			vColorText vColorTextList;
			vColorTextList.reserve(MAX_ITEM);

			// title은 갭, icon이 없다.
			if(iLine != 0)
			{
				memset(szSpaceLineKeyName, 0, sizeof(szSpaceLineKeyName));
				StringCbPrintf_e( szSpaceLineKeyName, sizeof(szSpaceLineKeyName), "%dSpaceLine", iLine );
				kColorText.m_iSpaceLine = kLoader.LoadInt(szSpaceLineKeyName, 0);

				memset(szIconKeyName, 0, sizeof(szIconKeyName));
				memset(szIconValue, 0, sizeof(szIconValue));
				StringCbPrintf_e( szIconKeyName, sizeof(szIconKeyName), "%dIcon", iLine );
				kLoader.LoadString( szIconKeyName, "", szIconValue, MAX_PATH );
				kColorText.m_szIconName = szIconValue;
			}

			for(int iSentence = 0; iSentence < MAX_ITEM; iSentence++)
			{
				memset(szColorKeyName, 0, sizeof(szColorKeyName));
				memset(szTextKeyName, 0, sizeof(szTextKeyName));
				memset(szColorValue, 0, sizeof(szColorValue));
				memset(szTextValue, 0, sizeof(szTextValue));

				if(iLine == 0)
				{
					StringCbPrintf_e( szColorKeyName, sizeof(szColorKeyName), "TitleColor%d" ,iSentence+1 );
					StringCbPrintf_e( szTextKeyName, sizeof(szTextKeyName), "TitleText%d", iSentence+1 );
				}
				else
				{
					StringCbPrintf_e( szColorKeyName, sizeof(szColorKeyName), "%dColor%d" , iLine, iSentence+1  );
					StringCbPrintf_e( szTextKeyName, sizeof(szTextKeyName), "%dText%d", iLine, iSentence+1);
				}

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

			if( vColorTextList.empty() ) 
			{
				if( iLine == 0 ) // title이 없는 경우 기본값을 설정
				{
					kColorText.m_dwColor = 0;
					kColorText.m_szText  = "Untitled";
					vColorTextList.push_back( kColorText ); 
					kColorText.Clear();
				}
				else
					break;
			}

			vLineTextList.push_back(vColorTextList);
		}

		m_vManualList.push_back(vLineTextList);
	}

	// image
	m_ImginfoVecOfVec.reserve( iMaxManual );
	for(int i = 0;i < iMaxManual;i++)
	{
		StringCbPrintf_e( szTitle, sizeof(szTitle), "Manual%d", i+1 );
		kLoader.SetTitle( szTitle );

		ImgInfoVec vImgInfoVec;
		vImgInfoVec.reserve(MAX_ITEM);

		for(int iArray = 0; iArray < MAX_ITEM; iArray++)
		{
			ImgInfo kImgInfo;
			memset(szIconKeyName, 0, sizeof(szIconKeyName));
			memset(szIconValue, 0, sizeof(szIconValue));
			StringCbPrintf( szIconKeyName, sizeof(szIconKeyName), "%dImage", iArray+1 );
			kLoader.LoadString( szIconKeyName, "", szIconValue, MAX_PATH );
			kImgInfo.m_szImageName = szIconValue;

			memset(szSpaceLineKeyName, 0, sizeof(szSpaceLineKeyName));
			StringCbPrintf( szSpaceLineKeyName, sizeof(szSpaceLineKeyName), "%dImageDuration", iArray+1 );
			kImgInfo.m_dwDuration = kLoader.LoadInt(szSpaceLineKeyName, 0);

			if( kImgInfo.m_szImageName.IsEmpty() )
				break;

			vImgInfoVec.push_back( kImgInfo );
		}

		m_ImginfoVecOfVec.push_back( vImgInfoVec );
	}
}


const int ManualMgr::GetMaxManual()
{
	return (int) m_vManualList.size();
}

const int ManualMgr::GetMaxLine( const int iCurManual )
{
	if(! COMPARE(iCurManual, 0, (int) m_vManualList.size()))
		return 0;

	return (int) m_vManualList[iCurManual].size();
}

void ManualMgr::SetPrinter( const int iCurManual, const int iCurLine, float fScale, OUT ioComplexStringPrinter &rkPrinter )
{
	if(! COMPARE(iCurManual, 0, (int) m_vManualList.size()))
		return ;

	if(! COMPARE(iCurLine, 0, (int) m_vManualList[iCurManual].size()))
		return ;

	rkPrinter.SetTextStyle( TS_NORMAL );
	rkPrinter.SetBkColor( 0, 0, 0 );	

	vColorText &rvColorTextList = m_vManualList[iCurManual][iCurLine];

	int iMaxSentence = (int) rvColorTextList.size();
	for(int i = 0;i < iMaxSentence;i++)
	{
		rkPrinter.SetTextColor( rvColorTextList[i].m_dwColor );
		rkPrinter.AddTextPiece( fScale, rvColorTextList[i].m_szText.c_str() );
	}
}

void ManualMgr::GetStringAndColor( IN const int iCurManual,
								   IN const int iCurLine,
								   OUT char *szLineString ,
								   IN const int iLineStringArraySize,
								   OUT DWORD *pdwColor,
								   IN const int iColorArraySize )
{
	if(! COMPARE(iCurManual, 0, (int) m_vManualList.size()))
		return ;

	if(! COMPARE(iCurLine, 0, (int) m_vManualList[iCurManual].size()))
		return ;

	vColorText &rvColorTextList = m_vManualList[iCurManual][iCurLine];

	int iColorArray = 0;
	int iMaxSentence = (int) rvColorTextList.size();
	for(int i = 0;i < iMaxSentence;i++)
	{
		StringCbCat( szLineString, iLineStringArraySize, rvColorTextList[i].m_szText.c_str() );

		for ( int iStringLength = 0; iStringLength < rvColorTextList[i].m_szText.Length() ; iStringLength++)
		{
			if( iColorArray >= iColorArraySize )
				break;

			pdwColor[iColorArray] = rvColorTextList[i].m_dwColor;
			iColorArray++;
		}
	}
}

const ioHashString* ManualMgr::GetIconName( const int iCurManual, const int iCurLine )
{
	if(! COMPARE(iCurManual, 0, (int) m_vManualList.size()))
		return NULL;

	if(! COMPARE(iCurLine, 0, (int) m_vManualList[iCurManual].size()))
		return NULL;

	vColorText &rvColorTextList = m_vManualList[iCurManual][iCurLine];
	if(rvColorTextList.empty())
		return NULL;

	return &rvColorTextList[0].m_szIconName;
}

const ioHashString* ManualMgr::GetTitleName( const int iCurManual )
{
	if(! COMPARE(iCurManual, 0, (int) m_vManualList.size()))
		return NULL;

	if( m_vManualList[iCurManual].size() == 0)
		return NULL;

	if( m_vManualList[iCurManual][0].size() == 0 )
		return NULL;

	return &m_vManualList[iCurManual][0][0].m_szText;
}

void ManualMgr::ClearList()
{
	int iMaxManual = (int) m_vManualList.size();
	for (int iManual = 0; iManual < iMaxManual ; iManual++)
	{
		int iMaxLine = (int)m_vManualList[iManual].size();
		for (int iLine = 0; iLine < iMaxLine ; iLine++)
		{
			m_vManualList[iManual][iLine].clear();
		}
		m_vManualList[iManual].clear();
	}
	m_vManualList.clear();

	// image
	int iMax = (int) m_ImginfoVecOfVec.size();
	for (int i = 0; i < iMax ; i++)
	{
		m_ImginfoVecOfVec[i].clear();
	}
	m_ImginfoVecOfVec.clear();
}

const int ManualMgr::GetSpaceLine( const int iCurManual, const int iCurLine )
{
	if(! COMPARE(iCurManual, 0, (int) m_vManualList.size()))
		return 0;

	if(! COMPARE(iCurLine, 0, (int) m_vManualList[iCurManual].size()))
		return 0;

	vColorText &rvColorTextList = m_vManualList[iCurManual][iCurLine];
	if(rvColorTextList.empty())
		return 0;
	
	return rvColorTextList[0].m_iSpaceLine;
}

const int ManualMgr::GetMaxImage( const int iCurManual )
{
	if(! COMPARE(iCurManual, 0, (int) m_ImginfoVecOfVec.size()))
		return 0;

	return (int) m_ImginfoVecOfVec[iCurManual].size();
}

const ioHashString* ManualMgr::GetImageName( const int iCurManual, const int iCurArray )
{
	if(! COMPARE(iCurManual, 0, (int) m_ImginfoVecOfVec.size()))
		return NULL;

	if(! COMPARE(iCurArray, 0, (int) m_ImginfoVecOfVec[iCurManual].size()))
		return NULL;

	ImgInfo &rImgInfo = m_ImginfoVecOfVec[iCurManual][iCurArray];
	if(rImgInfo.m_szImageName.IsEmpty())
		return NULL;

	return &rImgInfo.m_szImageName;
}

const DWORD ManualMgr::GetImageDuration( const int iCurManual, const int iCurArray )
{
	if(! COMPARE(iCurManual, 0, (int) m_ImginfoVecOfVec.size()))
		return 0;

	if(! COMPARE(iCurArray, 0, (int) m_ImginfoVecOfVec[iCurManual].size()))
		return 0;

	ImgInfo &rImgInfo = m_ImginfoVecOfVec[iCurManual][iCurArray];

	return rImgInfo.m_dwDuration;
}
//------------------------------------------------------------------------
template<> ioItemManualMgr* Singleton< ioItemManualMgr >::ms_Singleton = 0;

ioItemManualMgr::ioItemManualMgr()
{
}

ioItemManualMgr::~ioItemManualMgr()
{
}

ioItemManualMgr& ioItemManualMgr::GetSingleton()
{
	return Singleton< ioItemManualMgr >::GetSingleton();
}

ioItemManualMgr *ioItemManualMgr::GetSingletonPtr()
{
	return Singleton< ioItemManualMgr >::GetSingletonPtr();
}
//--------------------------------------------------------------------------
template<> ioDecoManualMgr* Singleton< ioDecoManualMgr >::ms_Singleton = 0;

ioDecoManualMgr::ioDecoManualMgr()
{
}

ioDecoManualMgr::~ioDecoManualMgr()
{
}

ioDecoManualMgr& ioDecoManualMgr::GetSingleton()
{
	return Singleton< ioDecoManualMgr >::GetSingleton();
}

ioDecoManualMgr *ioDecoManualMgr::GetSingletonPtr()
{
	return Singleton< ioDecoManualMgr >::GetSingletonPtr();
}
//-------------------------------------------------------------------------
template<> ioEtcManualMgr* Singleton< ioEtcManualMgr >::ms_Singleton = 0;

ioEtcManualMgr::ioEtcManualMgr()
{
}

ioEtcManualMgr::~ioEtcManualMgr()
{
}

ioEtcManualMgr& ioEtcManualMgr::GetSingleton()
{
	return Singleton< ioEtcManualMgr >::GetSingleton();
}

ioEtcManualMgr *ioEtcManualMgr::GetSingletonPtr()
{
	return Singleton< ioEtcManualMgr >::GetSingletonPtr();
}
//-------------------------------------------------------------------------
template<> ioEventGoodsManualMgr* Singleton< ioEventGoodsManualMgr >::ms_Singleton = 0;

ioEventGoodsManualMgr::ioEventGoodsManualMgr()
{
}

ioEventGoodsManualMgr::~ioEventGoodsManualMgr()
{
}

ioEventGoodsManualMgr& ioEventGoodsManualMgr::GetSingleton()
{
	return Singleton< ioEventGoodsManualMgr >::GetSingleton();
}

ioEventGoodsManualMgr *ioEventGoodsManualMgr::GetSingletonPtr()
{
	return Singleton< ioEventGoodsManualMgr >::GetSingletonPtr();
}
//////////////////////////////////////////////////////////////////////////
template<> ioEtcInvenManualMgr* Singleton< ioEtcInvenManualMgr >::ms_Singleton = 0;

ioEtcInvenManualMgr::ioEtcInvenManualMgr()
{
}

ioEtcInvenManualMgr::~ioEtcInvenManualMgr()
{
}

ioEtcInvenManualMgr& ioEtcInvenManualMgr::GetSingleton()
{
	return Singleton< ioEtcInvenManualMgr >::GetSingleton();
}

ioEtcInvenManualMgr *ioEtcInvenManualMgr::GetSingletonPtr()
{
	return Singleton< ioEtcInvenManualMgr >::GetSingletonPtr();
}
//////////////////////////////////////////////////////////////////////////
template<> ioEtcInvenSubManualMgr* Singleton< ioEtcInvenSubManualMgr >::ms_Singleton = 0;

ioEtcInvenSubManualMgr::ioEtcInvenSubManualMgr()
{
}

ioEtcInvenSubManualMgr::~ioEtcInvenSubManualMgr()
{
}

ioEtcInvenSubManualMgr& ioEtcInvenSubManualMgr::GetSingleton()
{
	return Singleton< ioEtcInvenSubManualMgr >::GetSingleton();
}

ioEtcInvenSubManualMgr *ioEtcInvenSubManualMgr::GetSingletonPtr()
{
	return Singleton< ioEtcInvenSubManualMgr >::GetSingletonPtr();
}
//////////////////////////////////////////////////////////////////////////
template<> ioPresentInvenManualMgr* Singleton< ioPresentInvenManualMgr >::ms_Singleton = 0;

ioPresentInvenManualMgr::ioPresentInvenManualMgr()
{
}

ioPresentInvenManualMgr::~ioPresentInvenManualMgr()
{
}

ioPresentInvenManualMgr& ioPresentInvenManualMgr::GetSingleton()
{
	return Singleton< ioPresentInvenManualMgr >::GetSingleton();
}

ioPresentInvenManualMgr *ioPresentInvenManualMgr::GetSingletonPtr()
{
	return Singleton< ioPresentInvenManualMgr >::GetSingletonPtr();
}

void ioPresentInvenManualMgr::ClearList()
{
	ManualMgr::ClearList();

	m_PresentMentMap.clear();
}

void ioPresentInvenManualMgr::LoadManual( const char* szININame )
{
	ClearList();

	ioINILoader kLoader( szININame );

	kLoader.SetTitle_e( "Info" );

	int iMaxManual = kLoader.LoadInt_e( "MaxManual", 0 );
	iMaxManual++; // Ment0이 존재하므로 i+1을 하지 않음.
	m_vManualList.reserve( iMaxManual );

	char szTitle[MAX_PATH] = "";

	char szIconKeyName[MAX_PATH] = "";
	char szColorKeyName[MAX_PATH] = "";
	char szTextKeyName[MAX_PATH] = "";

	char szIconValue[MAX_PATH]="";
	char szColorValue[MAX_PATH]="";
	char szTextValue[MAX_PATH]="";

	char szSpaceLineKeyName[MAX_PATH]="";
	int  iManualListArray = 0;
	for(int i = 0;i < iMaxManual;i++)
	{
		StringCbPrintf_e( szTitle, sizeof(szTitle), "Manual%d", i );
		kLoader.SetTitle( szTitle );

		int iMentType = kLoader.LoadInt_e( "MentType", -1 );
		if( iMentType == -1 )
			continue;

		vLineText vLineTextList;
		vLineTextList.reserve(MAX_ITEM);

		for(int iLine = 0; iLine < MAX_ITEM; iLine++)
		{
			ColorText kColorText;
			vColorText vColorTextList;
			vColorTextList.reserve(MAX_ITEM);

			// title은 갭, icon이 없다.
			if(iLine != 0)
			{
				memset(szSpaceLineKeyName, 0, sizeof(szSpaceLineKeyName));
				StringCbPrintf_e( szSpaceLineKeyName, sizeof(szSpaceLineKeyName), "%dSpaceLine", iLine );
				kColorText.m_iSpaceLine = kLoader.LoadInt(szSpaceLineKeyName, 0);

				memset(szIconKeyName, 0, sizeof(szIconKeyName));
				memset(szIconValue, 0, sizeof(szIconValue));
				StringCbPrintf_e( szIconKeyName, sizeof(szIconKeyName), "%dIcon", iLine );
				kLoader.LoadString( szIconKeyName, "", szIconValue, MAX_PATH );
				kColorText.m_szIconName = szIconValue;
			}

			for(int iSentence = 0; iSentence < MAX_ITEM; iSentence++)
			{
				memset(szColorKeyName, 0, sizeof(szColorKeyName));
				memset(szTextKeyName, 0, sizeof(szTextKeyName));
				memset(szColorValue, 0, sizeof(szColorValue));
				memset(szTextValue, 0, sizeof(szTextValue));

				if(iLine == 0)
				{
					StringCbPrintf_e( szColorKeyName, sizeof(szColorKeyName), "TitleColor%d" ,iSentence+1 );
					StringCbPrintf_e( szTextKeyName, sizeof(szTextKeyName), "TitleText%d", iSentence+1 );
				}
				else
				{
					StringCbPrintf_e( szColorKeyName, sizeof(szColorKeyName), "%dColor%d" , iLine, iSentence+1  );
					StringCbPrintf_e( szTextKeyName, sizeof(szTextKeyName), "%dText%d", iLine, iSentence+1);
				}

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

			if( vColorTextList.empty() ) 
			{
				if( iLine == 0 ) // title이 없는 경우 기본값을 설정
				{
					kColorText.m_dwColor = 0;
					kColorText.m_szText  = "Untitled";
					vColorTextList.push_back( kColorText ); 
					kColorText.Clear();
				}
				else
					break;
			}

			vLineTextList.push_back(vColorTextList);
		}

		m_vManualList.push_back(vLineTextList);

		m_PresentMentMap.insert( PresentMentMap::value_type( iMentType, iManualListArray ) );
		iManualListArray++;
	}

	// image
	m_ImginfoVecOfVec.reserve( iMaxManual );
	for(int i = 0;i < iMaxManual;i++)
	{
		StringCbPrintf_e( szTitle, sizeof(szTitle), "Manual%d", i+1 );
		kLoader.SetTitle( szTitle );

		ImgInfoVec vImgInfoVec;
		vImgInfoVec.reserve(MAX_ITEM);

		for(int iArray = 0; iArray < MAX_ITEM; iArray++)
		{
			ImgInfo kImgInfo;
			memset(szIconKeyName, 0, sizeof(szIconKeyName));
			memset(szIconValue, 0, sizeof(szIconValue));
			StringCbPrintf_e( szIconKeyName, sizeof(szIconKeyName), "%dImage", iArray+1 );
			kLoader.LoadString( szIconKeyName, "", szIconValue, MAX_PATH );
			kImgInfo.m_szImageName = szIconValue;

			memset(szSpaceLineKeyName, 0, sizeof(szSpaceLineKeyName));
			StringCbPrintf_e( szSpaceLineKeyName, sizeof(szSpaceLineKeyName), "%dImageDuration", iArray+1 );
			kImgInfo.m_dwDuration = kLoader.LoadInt(szSpaceLineKeyName, 0);

			if( kImgInfo.m_szImageName.IsEmpty() )
				break;

			vImgInfoVec.push_back( kImgInfo );
		}

		m_ImginfoVecOfVec.push_back( vImgInfoVec );
	}
}

const int ioPresentInvenManualMgr::GetMaxLine( const int iCurMentType )
{
	PresentMentMap::iterator iter = m_PresentMentMap.find( iCurMentType );
	if( iter != m_PresentMentMap.end() )
	{
		int iCurManual = iter->second;

		if(! COMPARE(iCurManual, 0, (int) m_vManualList.size()))
			return 0;

		return (int) m_vManualList[iCurManual].size();
	}

	return 0;
}

const int ioPresentInvenManualMgr::GetSpaceLine( const int iCurMentType, const int iCurLine )
{
	PresentMentMap::iterator iter = m_PresentMentMap.find( iCurMentType );
	if( iter != m_PresentMentMap.end() )
	{
		int iCurManual = iter->second;

		if(! COMPARE(iCurManual, 0, (int) m_vManualList.size()))
			return 0;

		if(! COMPARE(iCurLine, 0, (int) m_vManualList[iCurManual].size()))
			return 0;

		vColorText &rvColorTextList = m_vManualList[iCurManual][iCurLine];
		if(rvColorTextList.empty())
			return 0;

		return rvColorTextList[0].m_iSpaceLine;
	}

	return 0;
}

void ioPresentInvenManualMgr::SetPrinter( const int iCurMentType, const int iCurLine, float fScale, OUT ioComplexStringPrinter &rkPrinter )
{
	PresentMentMap::iterator iter = m_PresentMentMap.find( iCurMentType );
	if( iter != m_PresentMentMap.end() )
	{
		int iCurManual = iter->second;

		if(! COMPARE(iCurManual, 0, (int) m_vManualList.size()))
			return ;

		if(! COMPARE(iCurLine, 0, (int) m_vManualList[iCurManual].size()))
			return ;

		rkPrinter.SetTextStyle( TS_NORMAL );
		rkPrinter.SetBkColor( 0, 0, 0 );	

		vColorText &rvColorTextList = m_vManualList[iCurManual][iCurLine];

		int iMaxSentence = (int) rvColorTextList.size();
		for(int i = 0;i < iMaxSentence;i++)
		{
			rkPrinter.SetTextColor( rvColorTextList[i].m_dwColor );
			rkPrinter.AddTextPiece( fScale, rvColorTextList[i].m_szText.c_str() );
		}
	}
}
//-------------------------------------------------------------------------
template<> ioExtraInvenManualMgr* Singleton< ioExtraInvenManualMgr >::ms_Singleton = 0;

ioExtraInvenManualMgr::ioExtraInvenManualMgr()
{
}

ioExtraInvenManualMgr::~ioExtraInvenManualMgr()
{
}

ioExtraInvenManualMgr& ioExtraInvenManualMgr::GetSingleton()
{
	return Singleton< ioExtraInvenManualMgr >::GetSingleton();
}

ioExtraInvenManualMgr *ioExtraInvenManualMgr::GetSingletonPtr()
{
	return Singleton< ioExtraInvenManualMgr >::GetSingletonPtr();
}
//-------------------------------------------------------------------------
template<> ioMedalInvenManualMgr* Singleton< ioMedalInvenManualMgr >::ms_Singleton = 0;

ioMedalInvenManualMgr::ioMedalInvenManualMgr()
{
}

ioMedalInvenManualMgr::~ioMedalInvenManualMgr()
{
}

ioMedalInvenManualMgr& ioMedalInvenManualMgr::GetSingleton()
{
	return Singleton< ioMedalInvenManualMgr >::GetSingleton();
}

ioMedalInvenManualMgr *ioMedalInvenManualMgr::GetSingletonPtr()
{
	return Singleton< ioMedalInvenManualMgr >::GetSingletonPtr();
}
//-------------------------------------------------------------------------
template<> ioDecoInvenManualMgr* Singleton< ioDecoInvenManualMgr >::ms_Singleton = 0;

ioDecoInvenManualMgr::ioDecoInvenManualMgr()
{
}

ioDecoInvenManualMgr::~ioDecoInvenManualMgr()
{
}

ioDecoInvenManualMgr& ioDecoInvenManualMgr::GetSingleton()
{
	return Singleton< ioDecoInvenManualMgr >::GetSingleton();
}

ioDecoInvenManualMgr *ioDecoInvenManualMgr::GetSingletonPtr()
{
	return Singleton< ioDecoInvenManualMgr >::GetSingletonPtr();
}
//////////////////////////////////////////////////////////////////////////
template<> ioPetInvenManualMgr* Singleton< ioPetInvenManualMgr >::ms_Singleton = 0;

ioPetInvenManualMgr::ioPetInvenManualMgr()
{
}

ioPetInvenManualMgr::~ioPetInvenManualMgr()
{
}

ioPetInvenManualMgr& ioPetInvenManualMgr::GetSingleton()
{
	return Singleton< ioPetInvenManualMgr >::GetSingleton();
}

ioPetInvenManualMgr *ioPetInvenManualMgr::GetSingletonPtr()
{
	return Singleton< ioPetInvenManualMgr >::GetSingletonPtr();
}

//////////////////////////////////////////////////////////////////////////
template<> ioCostumeInvenManualMgr* Singleton< ioCostumeInvenManualMgr >::ms_Singleton = 0;

ioCostumeInvenManualMgr::ioCostumeInvenManualMgr()
{
}

ioCostumeInvenManualMgr::~ioCostumeInvenManualMgr()
{
}

ioCostumeInvenManualMgr& ioCostumeInvenManualMgr::GetSingleton()
{
	return Singleton< ioCostumeInvenManualMgr >::GetSingleton();
}

ioCostumeInvenManualMgr *ioCostumeInvenManualMgr::GetSingletonPtr()
{
	return Singleton< ioCostumeInvenManualMgr >::GetSingletonPtr();
}

//////////////////////////////////////////////////////////////////////////
template<> ioCostumeShopManualMgr* Singleton< ioCostumeShopManualMgr >::ms_Singleton = 0;

ioCostumeShopManualMgr::ioCostumeShopManualMgr()
{
}

ioCostumeShopManualMgr::~ioCostumeShopManualMgr()
{
}

ioCostumeShopManualMgr& ioCostumeShopManualMgr::GetSingleton()
{
	return Singleton< ioCostumeShopManualMgr >::GetSingleton();
}

ioCostumeShopManualMgr *ioCostumeShopManualMgr::GetSingletonPtr()
{
	return Singleton< ioCostumeShopManualMgr >::GetSingletonPtr();
}

//////////////////////////////////////////////////////////////////////////
template<> ioPopupStorePresentItemManualMgr* Singleton< ioPopupStorePresentItemManualMgr >::ms_Singleton = 0;

ioPopupStorePresentItemManualMgr::ioPopupStorePresentItemManualMgr()
{
}

ioPopupStorePresentItemManualMgr::~ioPopupStorePresentItemManualMgr()
{
}

ioPopupStorePresentItemManualMgr& ioPopupStorePresentItemManualMgr::GetSingleton()
{
	return Singleton< ioPopupStorePresentItemManualMgr >::GetSingleton();
}

ioPopupStorePresentItemManualMgr *ioPopupStorePresentItemManualMgr::GetSingletonPtr()
{
	return Singleton< ioPopupStorePresentItemManualMgr >::GetSingletonPtr();
}

void ioPopupStorePresentItemManualMgr::ClearList()
{
	ManualMgr::ClearList();

	m_PresentMentMap.clear();
}

void ioPopupStorePresentItemManualMgr::LoadManual( const char* szININame )
{
	ClearList();

	ioINILoader kLoader( szININame );

	kLoader.SetTitle_e( "Info" );

	int iMaxManual = kLoader.LoadInt_e( "MaxManual", 0 );
	iMaxManual++; // Ment0이 존재하므로 i+1을 하지 않음.
	m_vManualList.reserve( iMaxManual );

	char szTitle[MAX_PATH] = "";

	char szIconKeyName[MAX_PATH] = "";
	char szColorKeyName[MAX_PATH] = "";
	char szTextKeyName[MAX_PATH] = "";

	char szIconValue[MAX_PATH]="";
	char szColorValue[MAX_PATH]="";
	char szTextValue[MAX_PATH]="";

	char szSpaceLineKeyName[MAX_PATH]="";
	int  iManualListArray = 0;
	for(int i = 0;i < iMaxManual;i++)
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

			// title은 갭, icon이 없다.
			if(iLine != 0)
			{
				memset(szSpaceLineKeyName, 0, sizeof(szSpaceLineKeyName));
				StringCbPrintf_e( szSpaceLineKeyName, sizeof(szSpaceLineKeyName), "%dSpaceLine", iLine );
				kColorText.m_iSpaceLine = kLoader.LoadInt(szSpaceLineKeyName, 0);

				memset(szIconKeyName, 0, sizeof(szIconKeyName));
				memset(szIconValue, 0, sizeof(szIconValue));
				StringCbPrintf_e( szIconKeyName, sizeof(szIconKeyName), "%dIcon", iLine );
				kLoader.LoadString( szIconKeyName, "", szIconValue, MAX_PATH );
				kColorText.m_szIconName = szIconValue;
			}

			for(int iSentence = 0; iSentence < MAX_ITEM; iSentence++)
			{
				memset(szColorKeyName, 0, sizeof(szColorKeyName));
				memset(szTextKeyName, 0, sizeof(szTextKeyName));
				memset(szColorValue, 0, sizeof(szColorValue));
				memset(szTextValue, 0, sizeof(szTextValue));

				if(iLine == 0)
				{
					StringCbPrintf_e( szColorKeyName, sizeof(szColorKeyName), "TitleColor%d" ,iSentence+1 );
					StringCbPrintf_e( szTextKeyName, sizeof(szTextKeyName), "TitleText%d", iSentence+1 );
				}
				else
				{
					StringCbPrintf_e( szColorKeyName, sizeof(szColorKeyName), "%dColor%d" , iLine, iSentence+1  );
					StringCbPrintf_e( szTextKeyName, sizeof(szTextKeyName), "%dText%d", iLine, iSentence+1);
				}

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

			if( vColorTextList.empty() ) 
			{
				if( iLine == 0 ) // title이 없는 경우 기본값을 설정
				{
					kColorText.m_dwColor = 0;
					kColorText.m_szText  = "Untitled";
					vColorTextList.push_back( kColorText ); 
					kColorText.Clear();
				}
				else
					break;
			}

			vLineTextList.push_back(vColorTextList);
		}

		m_vManualList.push_back(vLineTextList);

		m_PresentMentMap.insert( PresentMentMap::value_type( i, iManualListArray ) );
		iManualListArray++;
	}

	// image
	m_ImginfoVecOfVec.reserve( iMaxManual );
	for(int i = 0;i < iMaxManual;i++)
	{
		StringCbPrintf_e( szTitle, sizeof(szTitle), "Manual%d", i+1 );
		kLoader.SetTitle( szTitle );

		ImgInfoVec vImgInfoVec;
		vImgInfoVec.reserve(MAX_ITEM);

		for(int iArray = 0; iArray < MAX_ITEM; iArray++)
		{
			ImgInfo kImgInfo;
			memset(szIconKeyName, 0, sizeof(szIconKeyName));
			memset(szIconValue, 0, sizeof(szIconValue));
			StringCbPrintf_e( szIconKeyName, sizeof(szIconKeyName), "%dImage", iArray+1 );
			kLoader.LoadString( szIconKeyName, "", szIconValue, MAX_PATH );
			kImgInfo.m_szImageName = szIconValue;

			memset(szSpaceLineKeyName, 0, sizeof(szSpaceLineKeyName));
			StringCbPrintf_e( szSpaceLineKeyName, sizeof(szSpaceLineKeyName), "%dImageDuration", iArray+1 );
			kImgInfo.m_dwDuration = kLoader.LoadInt(szSpaceLineKeyName, 0);

			if( kImgInfo.m_szImageName.IsEmpty() )
				break;

			vImgInfoVec.push_back( kImgInfo );
		}

		m_ImginfoVecOfVec.push_back( vImgInfoVec );
	}
}

const int ioPopupStorePresentItemManualMgr::GetMaxLine( const int iCurMentType )
{
	PresentMentMap::iterator iter = m_PresentMentMap.find( iCurMentType );
	if( iter != m_PresentMentMap.end() )
	{
		int iCurManual = iter->second;

		if(! COMPARE(iCurManual, 0, (int) m_vManualList.size()))
			return 0;

		return (int) m_vManualList[iCurManual].size();
	}

	return 0;
}

const int ioPopupStorePresentItemManualMgr::GetSpaceLine( const int iCurMentType, const int iCurLine )
{
	PresentMentMap::iterator iter = m_PresentMentMap.find( iCurMentType );
	if( iter != m_PresentMentMap.end() )
	{
		int iCurManual = iter->second;

		if(! COMPARE(iCurManual, 0, (int) m_vManualList.size()))
			return 0;

		if(! COMPARE(iCurLine, 0, (int) m_vManualList[iCurManual].size()))
			return 0;

		vColorText &rvColorTextList = m_vManualList[iCurManual][iCurLine];
		if(rvColorTextList.empty())
			return 0;

		return rvColorTextList[0].m_iSpaceLine;
	}

	return 0;
}

void ioPopupStorePresentItemManualMgr::SetPrinter( const int iCurMentType, const int iCurLine, float fScale, OUT ioComplexStringPrinter &rkPrinter )
{
	PresentMentMap::iterator iter = m_PresentMentMap.find( iCurMentType );
	if( iter != m_PresentMentMap.end() )
	{
		int iCurManual = iter->second;

		if(! COMPARE(iCurManual, 0, (int) m_vManualList.size()))
			return ;

		if(! COMPARE(iCurLine, 0, (int) m_vManualList[iCurManual].size()))
			return ;

		rkPrinter.SetTextStyle( TS_NORMAL );
		rkPrinter.SetBkColor( 0, 0, 0 );	

		vColorText &rvColorTextList = m_vManualList[iCurManual][iCurLine];

		int iMaxSentence = (int) rvColorTextList.size();
		for(int i = 0;i < iMaxSentence;i++)
		{
			rkPrinter.SetTextColor( rvColorTextList[i].m_dwColor );
			rkPrinter.AddTextPiece( fScale, rvColorTextList[i].m_szText.c_str() );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
template<> ioAccessoryInvenManualMgr* Singleton< ioAccessoryInvenManualMgr >::ms_Singleton = 0;

ioAccessoryInvenManualMgr::ioAccessoryInvenManualMgr()
{
}

ioAccessoryInvenManualMgr::~ioAccessoryInvenManualMgr()
{
}

ioAccessoryInvenManualMgr& ioAccessoryInvenManualMgr::GetSingleton()
{
	return Singleton< ioAccessoryInvenManualMgr >::GetSingleton();
}

ioAccessoryInvenManualMgr *ioAccessoryInvenManualMgr::GetSingletonPtr()
{
	return Singleton< ioAccessoryInvenManualMgr >::GetSingletonPtr();
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ManualWnd::ManualWnd()
{
	m_iCurPos    = 0;
	m_iCurIndex  = 0;
}

ManualWnd::~ManualWnd()
{
	ManualHelpListMap::iterator iter = m_ManualHelpListMap.begin();
	for(;iter != m_ManualHelpListMap.end();++iter)
	{
		ManualHelpList &rkList = iter->second;
		for(int i = 0;i < (int)rkList.m_ManualHelp.size();i++)
		{
			ManualHelp *pHelp = rkList.m_ManualHelp[i];
			if( !pHelp ) continue;
			SAFEDELETE( pHelp->m_pImage );
			SAFEDELETE( pHelp );
		}
	}
	m_ManualHelpListMap.clear();
}

void ManualWnd::iwm_create()
{
	ioINILoader_e kLoader( "config/sp2_losa_help.ini" );
	kLoader.SetTitle_e( "common" );
	int iMaxHelp = kLoader.LoadInt_e( "max_help", 0 );
	for(int i = 0;i < iMaxHelp;i++)
	{
		ManualHelpList kManualHelpList;
		char szKey[MAX_PATH];
		sprintf_e( szKey, "help%d", i + 1 );
		kLoader.SetTitle( szKey );

		DWORD dwIndex = kLoader.LoadInt_e( "index", 0 );
		if( dwIndex == 0 ) continue;

		kManualHelpList.m_szKey = szKey;
		m_ManualHelpListMap.insert( ManualHelpListMap::value_type( dwIndex, kManualHelpList ) );
	}
}

void ManualWnd::ShowWndByCurManual( const int iCurManual )
{
	HideWnd();
	
	m_iCurIndex = iCurManual;
	ManualHelpListMap::iterator iter = m_ManualHelpListMap.find( m_iCurIndex );
	if( iter == m_ManualHelpListMap.end() )
		return;
	
	// 로드하지 않았으면 읽는다.
	if( !iter->second.m_bLoad )
	{
		ioINILoader_e kLoader( "config/sp2_losa_help.ini" );

		// 자동개행
		kLoader.SetTitle_e( "common" );
		float fSplitWidth   = kLoader.LoadFloat_e( "split_width", 0.0f );

		kLoader.SetTitle( iter->second.m_szKey.c_str() );

		ManualHelpList &rkManualHelpList = iter->second;

		char szKey[MAX_PATH], szBuf[MAX_PATH];
		kLoader.LoadString_e( "title", "", szBuf, MAX_PATH );
		rkManualHelpList.m_szTitle = szBuf;	

		int  iMaxLine = kLoader.LoadInt_e( "max_line", 0 );		
		for(int j = 0;j < iMaxLine;j++)
		{
			ManualHelp *pManualHelp = new ManualHelp;
			if( !pManualHelp ) continue;
			sprintf_e( szKey, "line%d_offsetx", j + 1 );
			pManualHelp->m_iOffSetX = kLoader.LoadInt( szKey, 0 );

			sprintf_e( szKey, "line%d_offsety", j + 1 );
			pManualHelp->m_iOffSetY = kLoader.LoadInt( szKey, 0 );

			sprintf_e( szKey, "line%d_image", j + 1 );
			kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
			pManualHelp->m_pImage = g_UIImageSetMgr.CreateImageByFullName( szBuf );

			sprintf_e( szKey, "line%d_size", j + 1 );
			float fSize = kLoader.LoadFloat( szKey, 0.0f );

			sprintf_e( szKey, "line%d_count", j + 1 );
			int iLineCount = kLoader.LoadInt( szKey, 0 );
			for(int k = 0;k < iLineCount;k++)
			{
				sprintf_e( szKey, "line%d_color%d", j + 1, k + 1 );
				kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
				char *pTemp = NULL;
				DWORD dwColor = strtoul( szBuf, &pTemp, 16 );					
				pManualHelp->m_szHelp.SetTextStyle( TS_NORMAL );
				pManualHelp->m_szHelp.SetTextColor( dwColor );

				sprintf_e( szKey, "line%d_space%d", j + 1, k + 1 );
				int iSpace = kLoader.LoadInt( szKey, 0 );
				for(int s = 0;s < iSpace;s++)
					pManualHelp->m_szHelp.AddTextPiece( fSize, " " );  //띄어쓰기

				sprintf_e( szKey, "line%d_text%d", j + 1, k + 1 );
				kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
				pManualHelp->m_szHelp.AddTextPiece( fSize, szBuf );
			}

			// 자동개행
			if( fSplitWidth != 0.0f  )
			{
				enum { MAX_LOOP = 20, };
				for (int k = 0; k < MAX_LOOP ; k++)
				{
					if( pManualHelp->m_szHelp.GetFullWidth() < fSplitWidth )
					{
						rkManualHelpList.m_ManualHelp.push_back( pManualHelp );
						break;
					}

					ioComplexStringPrinter kPrinterA, kPrinterB;
					pManualHelp->m_szHelp.SplitFullText( kPrinterA, kPrinterB, fSplitWidth );

					// 앞문장
					ManualHelp *pManualHelpA = new ManualHelp;
					if( !pManualHelpA )
						continue;
					pManualHelpA->m_iOffSetX = pManualHelp->m_iOffSetX;
					pManualHelpA->m_iOffSetY = pManualHelp->m_iOffSetY;
					kPrinterA.CopyFullText( pManualHelpA->m_szHelp );
					rkManualHelpList.m_ManualHelp.push_back( pManualHelpA ); 

					// 뒷문장
					kPrinterB.CopyFullText( pManualHelp->m_szHelp );
				}
			}
			else
				rkManualHelpList.m_ManualHelp.push_back( pManualHelp );

			rkManualHelpList.m_bLoad = true;
		}
		m_CurManualHelp = rkManualHelpList;
	}
	else
	{
		m_CurManualHelp = iter->second;
	}
	ShowWnd();
}

void ManualWnd::iwm_show()
{
	m_iCurPos = 0;	
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, m_CurManualHelp.m_ManualHelp.size() );
		pScroll->SetScrollPos( m_iCurPos );
	}
}

void ManualWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	m_iCurPos = curPos;
}

void ManualWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{ 
		// 휠 이벤트 처리를 했다면 부모로 이벤트를 날리지 않는다.
		if( zDelta == WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPos - 1 );
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPos + 1 );
		}
	}
}

void ManualWnd::iwm_hide()
{
	g_QuestMgr.QuestCompleteTerm( QC_MANUAL_CLOSE, m_iCurIndex );
}

void ManualWnd::OnRender()
{
 	ioWnd::OnRender();
 
 	int iXPos = GetDerivedPosX();
 	int iYPos = GetDerivedPosY();
	RenderManual( iXPos + 22, iYPos + 16 );
}

void ManualWnd::RenderManual( int iXPos, int iYPos )
{
	enum { YGAP = 21, };
	for(int i = 0, iLineCount = 0;i < (int)m_CurManualHelp.m_ManualHelp.size();i++)
	{
		if( iLineCount >= MAX_PAGE )
			return;

		ManualHelp *pManualHelp = m_CurManualHelp.m_ManualHelp[i];			
		if( !pManualHelp )
			continue;
		if( pManualHelp->m_pImage )
		{		
			int iOffsetArray = m_iCurPos - i;
			int iStartH = min( pManualHelp->m_pImage->GetHeight(), max( 0, iOffsetArray * YGAP ) );
			int iEndH   = min( ((MAX_PAGE + max( 0, iOffsetArray)) - iLineCount) * YGAP, pManualHelp->m_pImage->GetHeight() );
			pManualHelp->m_pImage->RenderHeightCut( iXPos, ( iYPos + (iLineCount * YGAP) ) - iStartH, iStartH, iEndH );
		}

		if( i < m_iCurPos ) continue;

		pManualHelp->m_szHelp.PrintFullText( iXPos + pManualHelp->m_iOffSetX, iYPos + (iLineCount * YGAP) + pManualHelp->m_iOffSetY, TAT_LEFT );
		iLineCount++;
	}
}

void ManualWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) 
		return;
}
//////////////////////////////////////////////////////////////////////////
ManualListWnd::ManualListWnd()
{
	m_pDarkLine	= NULL;
	m_pLightLine= NULL;
	m_pOverLine = NULL;
	m_pSelectIcon = NULL;
	m_pDotLine  = NULL;

	m_iSelectIndex   = 0;
	m_iOverListIndex = -1;
	m_iPushListIndex = -1;
}

ManualListWnd::~ManualListWnd()
{
	SAFEDELETE( m_pDarkLine );
	SAFEDELETE( m_pLightLine );
	SAFEDELETE( m_pOverLine );
	SAFEDELETE( m_pSelectIcon );
	SAFEDELETE( m_pDotLine );

	m_ManualTitleList.clear();
}

void ManualListWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "DarkLine" )
	{
		SAFEDELETE( m_pDarkLine );
		m_pDarkLine = pImage;
	}
	else if( szType == "LightLine" )
	{
		SAFEDELETE( m_pLightLine );
		m_pLightLine = pImage;
	}
	else if( szType == "OverLine" )
	{
		SAFEDELETE( m_pOverLine );
		m_pOverLine = pImage;
	}
	else if( szType == "SelectIcon" )
	{
		SAFEDELETE( m_pSelectIcon );
		m_pSelectIcon = pImage;
	}
	else if( szType == "DotLine" )
	{
		SAFEDELETE( m_pDotLine );
		m_pDotLine = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

void ManualListWnd::iwm_create()
{
	g_GUIMgr.AddWnd( "XML/ManualWnd.xml", this );

	// INI 로드
	ioINILoader_e kLoader( "config/sp2_losa_help.ini" );
	kLoader.SetTitle_e( "common" );
	m_iSelectIndex = kLoader.LoadInt_e( "default_select", 0 );
	int iMaxHelp = kLoader.LoadInt_e( "max_help", 0 );
	for(int i = 0;i < iMaxHelp;i++)
	{
		ManualTitle kManual;
		char szKey[MAX_PATH], szBuf[MAX_PATH];
		sprintf_e( szKey, "help%d", i + 1 );
		kLoader.SetTitle( szKey );

		kManual.m_dwIndex = kLoader.LoadInt_e( "index", 0 );
		if( kManual.m_dwIndex == 0 ) continue;
			
		kLoader.LoadString_e( "title", "", szBuf, MAX_PATH );
		kManual.m_szTitle = szBuf;
		m_ManualTitleList.push_back( kManual );
	}
}

void ManualListWnd::iwm_show()
{
	m_iOverListIndex = -1;
	m_iPushListIndex = -1;

	m_iCurPos = 0;
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		pScroll->SetScrollRange( 0, m_ManualTitleList.size() );
		pScroll->SetScrollPos( m_iCurPos );
	}

	ShowWndByCurManual( m_iSelectIndex );

#if defined( USE_GA )
	// WND_GUIDE
	g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FWND%2FGUIDE" );
#endif

}

void ManualListWnd::iwm_hide()
{
	HideChildWnd( ID_MANUAL_WND );
}

void ManualListWnd::iwm_command( ioWnd *pWnd, int cmd, DWORD param )
{
	DWORD dwID = pWnd->GetID();
	switch( dwID )
	{
	case ID_EXIT:
		if( cmd == IOBN_BTNUP )
		{
			HideWnd();
		}
		break;
	}
}

void ManualListWnd::iwm_vscroll( DWORD dwID, int curPos )
{
	m_iCurPos = curPos;
}

void ManualListWnd::iwm_wheel( int zDelta )
{
	if( !IsShow() )
		ioWnd::iwm_wheel( zDelta );
	else
	{ 
		// 휠 이벤트 처리를 했다면 부모로 이벤트를 날리지 않는다.
		if( zDelta == WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPos - 1 );
		}
		else if( zDelta == -WHEEL_DELTA )
		{
			ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
			if( pScroll )
				pScroll->SetScrollPos( m_iCurPos + 1 );
		}
	}
}

void ManualListWnd::ShowWndByCurManual( const int iCurManual )
{
	m_iSelectIndex = iCurManual;
	ManualWnd *pManualWnd = dynamic_cast<ManualWnd*> (FindChildWnd( ID_MANUAL_WND ) );
	if( pManualWnd )
		pManualWnd->ShowWndByCurManual( iCurManual );

	for(int iIndex = 0;iIndex < (int)m_ManualTitleList.size();iIndex++)
	{
		if( m_ManualTitleList[iIndex].m_dwIndex == (DWORD)m_iSelectIndex )
		{
			char szTitle[MAX_PATH] = "";
			SafeSprintf( szTitle, sizeof( szTitle ), STR(1), m_ManualTitleList[iIndex].m_szTitle.c_str() );
			SetTitleText( szTitle );
			break;
		}
	}

	if( !IsShow() )
	{
		ShowWnd();
	}
}

bool ManualListWnd::QuestGuide( DWORD dwIndex, float &rfXPos, float &rfYPos )
{
	int iXPos, iYPos;
	iXPos = GetDerivedPosX() + 17;
	iYPos = GetDerivedPosY() + 54;

	int iStartArray = m_iCurPos;
	for(int iIndex = 0;iIndex < MAX_PAGE;iIndex++,iStartArray++)
	{
		if( iStartArray >= (int)m_ManualTitleList.size() )
			break;

		RECT rcNameRect;
		rcNameRect.left = iXPos;
		rcNameRect.top  = iYPos + iIndex * 21;
		rcNameRect.right  = rcNameRect.left + 183;
		rcNameRect.bottom = rcNameRect.top + 21;
			
		if( m_ManualTitleList[iStartArray].m_dwIndex == dwIndex )
		{
			rfXPos = rcNameRect.left + 91;
			rfYPos = rcNameRect.top;
			return true;
		}
	}
	// 리스트에 없으면..
	ioScroll *pScroll = (ioScroll*)FindChildWnd( ID_VERT_SCROLL );
	if( pScroll )
	{
		rfXPos = pScroll->GetDerivedPosX() + ( pScroll->GetWidth() / 2 );
		rfYPos = pScroll->GetDerivedPosY();
		return true;
	}
	return false;
}

void ManualListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() )
		return;

	ioMovingWnd::OnProcess( fTimePerSec );	

	int  iPrevOverIndex = m_iOverListIndex;
	m_iOverListIndex = -1;

	if( g_GUIMgr.GetPreOverWnd() != this &&
		g_GUIMgr.GetPreClickWnd() != this )
		return;

	int iXPos, iYPos;
	iXPos = GetDerivedPosX() + 17;
	iYPos = GetDerivedPosY() + 54;

	ioLSMouse *pMouse = g_App.GetMouse();

	int iStartArray = m_iCurPos;
	for(int iIndex = 0;iIndex < MAX_PAGE;iIndex++,iStartArray++)
	{
		if( iStartArray >= (int)m_ManualTitleList.size() )
			break;

		RECT rcNameRect;
		rcNameRect.left = iXPos;
		rcNameRect.top  = iYPos + iIndex * 21;
		rcNameRect.right  = rcNameRect.left + 183;
		rcNameRect.bottom = rcNameRect.top + 21;

		if( ::PtInRect( &rcNameRect, pMouse->GetMousePos() ) )
		{
			if( iPrevOverIndex != (int)m_ManualTitleList[iStartArray].m_dwIndex &&
				m_iSelectIndex != iPrevOverIndex )
			{
				g_SoundMgr.PlaySound( "interface/public_mouseover_00.wav" );
			}

			m_iOverListIndex = m_ManualTitleList[iStartArray].m_dwIndex;
			if( pMouse->IsLBtnDown() && m_iPushListIndex == -1 )
			{
				if( m_iSelectIndex != (int)m_ManualTitleList[iStartArray].m_dwIndex )
					g_SoundMgr.PlaySound( "interface/public_mouseclick_00.wav" );

				ShowWndByCurManual( m_ManualTitleList[iStartArray].m_dwIndex );
				m_iPushListIndex = m_iSelectIndex;
			}
			break;
		}
	}

	if( pMouse->IsLBtnUp() )
	{
		m_iPushListIndex = -1;
	}
	else if( pMouse->IsLBtnDown() )
	{
		m_iPushListIndex = m_iSelectIndex;
	}
}

void ManualListWnd::OnRender()
{
	ioWnd::OnRender();

	if( !m_pDarkLine || !m_pLightLine || !m_pOverLine ) 
		return;

	int iXPos = GetDerivedPosX() + 17;
	int iYPos = GetDerivedPosY() + 54;

	g_FontMgr.SetAlignType( TAT_LEFT );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetBkColor( 0, 0, 0 );

	bool bDarkRender = false;
	int iStartArray = m_iCurPos;
	for(int iIndex = 0;iIndex < MAX_PAGE;iIndex++,iStartArray++)
	{
		bDarkRender = !bDarkRender;
		bool bOverLine = false;
		if( iStartArray < (int)m_ManualTitleList.size() )
		{
			if( m_ManualTitleList[iStartArray].m_dwIndex == (DWORD)m_iSelectIndex ||
				m_ManualTitleList[iStartArray].m_dwIndex == (DWORD)m_iOverListIndex )
				bOverLine = true;
		}

		if( bOverLine )
		{
			m_pOverLine->Render( iXPos, iYPos + ( iIndex * 21 ) );
			g_FontMgr.SetTextColor( 255, 255, 255 );
		}
		else if( bDarkRender )
		{
			m_pDarkLine->Render( iXPos, iYPos + ( iIndex * 21 ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}
		else
		{
			m_pLightLine->Render( iXPos, iYPos + ( iIndex * 21 ) );
			g_FontMgr.SetTextColor( TCT_DEFAULT_DARKGRAY );
		}

		if( iStartArray < (int)m_ManualTitleList.size() )
		{
			g_FontMgr.PrintTextWidthCut( iXPos + 5, iYPos + ( iIndex * 21 ) + 3, FONT_SIZE_12, 175.0f, m_ManualTitleList[iStartArray].m_szTitle.c_str() );

			if( m_ManualTitleList[iStartArray].m_dwIndex == (DWORD)m_iSelectIndex )
			{
				if( m_pSelectIcon )
				{
					m_pSelectIcon->SetColor( TCT_DEFAULT_WHITE );
					m_pSelectIcon->Render( iXPos + 170, iYPos + ( iIndex * 21 ) + 5, UI_RENDER_NORMAL_ADD_COLOR );
				}
			}
		}
	}

	// 우측 라인
	for(int i = 0;i < MAX_PAGE;i++)
	{
		if( m_pDotLine )
			m_pDotLine->Render( iXPos + 472, (iYPos + 18) + i * 21 );
	}
}