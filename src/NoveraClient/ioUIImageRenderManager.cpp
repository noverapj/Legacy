
#include "stdafx.h"

#include "ioUIImageRenderManager.h"

template<> ioUIImageRenderManager* Singleton< ioUIImageRenderManager >::ms_Singleton = 0;

ioUIImageRenderManager::ioUIImageRenderManager()
{
}

ioUIImageRenderManager::~ioUIImageRenderManager()
{
	ReleaseUIGradeInfo();
}

ioUIImageRenderManager& ioUIImageRenderManager::GetSingleton()
{
	return Singleton< ioUIImageRenderManager >::GetSingleton();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioUIImageRenderManager::LoadProperty()
{
	ioINILoader_e kLoader( "config/sp2_uiimagerender.ini" );
	LoadUIGradeInfo( kLoader );
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioUIImageRenderManager::LoadUIGradeInfo( ioINILoader& kLoader )
{
	char szBuf[MAX_PATH], szKey[MAX_PATH];
	kLoader.SetTitle_e( "UIGrade" );

	m_UIGradeInfoList.clear();
	int nMaxGrade = kLoader.LoadInt_e( "grade_cnt", 0 );
	for ( int i=0; i<nMaxGrade; i++ )
	{
		wsprintf_e( szKey, "grade%d_type", i+1 );
		int nType = kLoader.LoadInt( szKey, 0 );
				
		UIGradeInfo sUIGradeInfo;

		wsprintf_e( szKey, "grade%d_icon_back", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		sUIGradeInfo.m_pGradeIconBack = g_UIImageSetMgr.CreateImageByFullName( szBuf );

		wsprintf_e( szKey, "grade%d_text_color", i+1 );
		kLoader.LoadString( szKey, "", szBuf, MAX_PATH);

		char *pTemp = NULL;	
		sUIGradeInfo.m_dwGradeTextColor = strtoul( szBuf, &pTemp, 16 );

		m_UIGradeInfoList.insert( std::make_pair( nType, sUIGradeInfo ) );
	}
}

void ioUIImageRenderManager::ReleaseUIGradeInfo()
{
	UIGradeInfoList::iterator Iter = m_UIGradeInfoList.begin();
	for ( ; Iter!=m_UIGradeInfoList.end(); ++Iter )
	{
		UIGradeInfo &sUIGradeInfo = Iter->second;
		SAFEDELETE( sUIGradeInfo.m_pGradeIconBack );
	}
	m_UIGradeInfoList.clear();
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int ioUIImageRenderManager::GetPowerUpGradeType( int nClassType )
{
	int nItemCode = nClassType;
	const ioClassExpert::CLASSSLOT* pClassSlot = g_MyInfo.GetClassExpertSlot( nClassType );
	if ( pClassSlot && pClassSlot->m_ePowerUpCharGrade != PUGT_NONE )
		nItemCode += ConvertPowerUpCharToItem( pClassSlot->m_ePowerUpCharGrade );

	const ioItem *pItem = g_ItemMaker.GetItemConst( nItemCode, __FUNCTION__ );
	if( pItem )
		return pItem->GetGradeType();

	return UGT_NONE;
}

int ioUIImageRenderManager::GetPowerUpGradeType( int nClassType, byte eCharPowerUpType )
{
	int nItemCode = nClassType;
	if ( eCharPowerUpType != PUGT_NONE )
		nItemCode += ConvertPowerUpCharToItem( eCharPowerUpType );

	const ioItem *pItem = g_ItemMaker.GetItemConst( nItemCode, __FUNCTION__ );
	if( pItem )
		return pItem->GetGradeType();

	return UGT_NONE;
}

DWORD ioUIImageRenderManager::GetGradeTextColor( int nGradeType )
{
	if ( nGradeType <= 0 || m_UIGradeInfoList.empty() )
		return TCT_DEFAULT_DARKGRAY;

	UIGradeInfoList::iterator FindIter = m_UIGradeInfoList.find( nGradeType );
	if ( FindIter == m_UIGradeInfoList.end() )
		return TCT_DEFAULT_DARKGRAY;

	UIGradeInfo &sUIGradeInfo = FindIter->second;
	return sUIGradeInfo.m_dwGradeTextColor;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioUIImageRenderManager::RenderGradeIconBack( int nGradeType, int nXpos, int nYpos, float fScale, UIRenderType eRenderType, TextureFilterOption eOption, int nRed, int nGreen, int nBlue, bool bNoMagicAlign, UIRenderType eGrayRenderType )
{
	if ( nGradeType <= 0 || m_UIGradeInfoList.empty() )
		return;
	
	UIGradeInfoList::iterator FindIter = m_UIGradeInfoList.find( nGradeType );
	if ( FindIter == m_UIGradeInfoList.end() )
		return;

	UIGradeInfo &sUIGradeInfo = FindIter->second;
	if ( sUIGradeInfo.m_pGradeIconBack )
	{
		sUIGradeInfo.m_pGradeIconBack->SetScale( fScale );

		if ( eRenderType == UI_RENDER_NORMAL_ADD_COLOR )
			sUIGradeInfo.m_pGradeIconBack->SetColor( nRed, nBlue, nGreen );

		if ( fScale != FLOAT1 )
			sUIGradeInfo.m_pGradeIconBack->Render( nXpos, nYpos, eRenderType, TFO_BILINEAR, bNoMagicAlign, eGrayRenderType );
		else
			sUIGradeInfo.m_pGradeIconBack->Render( nXpos, nYpos, eRenderType, eOption, bNoMagicAlign, eGrayRenderType );
	}
}