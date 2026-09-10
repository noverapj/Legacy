

#include "stdafx.h"

#include "ioItemRight.h"

ioItemRight::ioItemRight()
{
	m_iLevel = -1;
	m_dwRightCode = 0;
	m_RightState = RS_LOCKED;
}

ioItemRight::~ioItemRight()
{
}

bool ioItemRight::IsRightValid() const
{
	if( m_iLevel == -1 )
		return false;

	if( m_dwRightCode != 0 && !m_Name.IsEmpty() )
		return true;

	return false;
}

void ioItemRight::LoadProperty( ioINILoader &rkLoader )
{
	m_iLevel = rkLoader.LoadInt_e( "level", -1 );
	m_dwRightCode = rkLoader.LoadInt_e( "code", 0 );
	m_dwPreConditionCode = rkLoader.LoadInt_e( "need_right_code", 0 );
	m_iSpendPeso  = rkLoader.LoadInt_e( "spend_peso", -1);

	char szBuf[MAX_PATH * 2];
	rkLoader.LoadString_e( "name", "", szBuf, MAX_PATH * 2 );
	m_Name = szBuf;

	rkLoader.LoadString_e( "desc", "", szBuf, MAX_PATH * 2 );
	m_Description = szBuf;
	
	rkLoader.LoadString_e( "type_desc", "", szBuf, MAX_PATH * 2 );
	m_TypeDesc = szBuf;

	rkLoader.LoadString_e( "desc_image_set", "", szBuf, MAX_PATH * 2 );
	m_DescImageSet = szBuf;

	rkLoader.LoadString_e( "desc_image_name", "", szBuf, MAX_PATH * 2 );
	m_DescImageName = szBuf;

	rkLoader.LoadString_e( "first_func", "", szBuf, MAX_PATH * 2 );
	m_FirstFunc = szBuf;

	rkLoader.LoadString_e( "second_func", "", szBuf, MAX_PATH * 2 );
	m_SecondFunc = szBuf;
}

bool ioItemRight::CheckPreCondition( ioWnd *pWnd ) const
{
	if( m_dwPreConditionCode == 0 )
		return true;

	if( m_iSpendPeso > g_MyInfo.GetMoney())
	{
		int iTemp = (int)g_MyInfo.GetMoney();
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), m_iSpendPeso-iTemp);
		return false;
	}

	return false;
}

void ioItemRight::SetRightState( RightState eState )
{
	m_RightState = eState;
}

bool ioItemRight::HasDescImage() const
{
	if( !m_DescImageSet.IsEmpty() && !m_DescImageName.IsEmpty() )
		return true;

	return false;
}

ioUIRenderImage* ioItemRight::CreateDescImage() const
{
	if( HasDescImage() )
		return g_UIImageSetMgr.CreateImage( m_DescImageSet, m_DescImageName );
	
	return NULL;
}