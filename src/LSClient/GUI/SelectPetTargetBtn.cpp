
#include "StdAfx.h"

#include "SelectPetTargetBtn.h"

//////////////////////////////////////////////////////////////////////////////

SelectPetTargetBtn::SelectPetTargetBtn()
{
	m_nCurSelectSlot = 0;
	m_szSelectText.Clear();
}

SelectPetTargetBtn::~SelectPetTargetBtn()
{
}

void SelectPetTargetBtn::iwm_show()
{
	m_nCurSelectSlot = 0;
	m_dwCurTitleColor = TCT_DEFAULT_DARKGRAY;
	m_szSelectText.Clear();
}

//////////////////////////////////////////////////////////////////////////////

void SelectPetTargetBtn::OnRender()
{
	ioButton::OnRender();

	ioUITitle *pTitle =  GetTitle();
	if( pTitle )
	{
		pTitle->SetColor( "Normal", m_dwCurTitleColor, 0xFF000000 );
		pTitle->SetColor( "Over", m_dwCurTitleColor, 0xFF000000 );
		pTitle->SetColor( "Push", m_dwCurTitleColor, 0xFF000000 );
	}

	if( m_nCurSelectSlot > 0 )
		SetTitleText( m_szSelectText.c_str() );
	else
		SetTitleText( m_szDefaultText.c_str() );
}

//////////////////////////////////////////////////////////////////////////////

void SelectPetTargetBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	ioButton::ParseExtraInfo( xElement );

	m_szDefaultText = xElement.GetStringAttribute_e( "DescText" );
}

//////////////////////////////////////////////////////////////////////////////

//초기화
void SelectPetTargetBtn::InitData()
{
	m_nCurSelectSlot = 0;
	m_dwCurTitleColor = TCT_DEFAULT_DARKGRAY;
	m_szSelectText.Clear();
}

//펫 설정
void SelectPetTargetBtn::SetSelectPetSlot( int nSelectSlot )
{
	m_nCurSelectSlot = 0;
	m_dwCurTitleColor = TCT_DEFAULT_DARKGRAY;

	ioUserPet *pUserPet = g_MyInfo.GetUserPet();
	if( pUserPet )
	{
		PetSlot sPetSlot;

		if( pUserPet->GetPetSlot( nSelectSlot, sPetSlot ) )
		{
			m_nCurSelectSlot = nSelectSlot;

			char szBuf[MAX_PATH] = "";
			SafeSprintf( szBuf, sizeof( szBuf ), STR(1), sPetSlot.m_Name.c_str() );

			m_szSelectText = szBuf;
		}
	}
}

//재료 설정
void SelectPetTargetBtn::SetSelectMaterial( int nSelectMaterial, ioHashString szTitle )
{
	m_nCurSelectSlot = nSelectMaterial;	
	m_szSelectText = szTitle;
}

//색 설정
void SelectPetTargetBtn::SetCurTitleColor( DWORD dwTitleColor )
{
	m_dwCurTitleColor = dwTitleColor;
}