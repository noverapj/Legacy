//----------------------------------------------------------------------------------
/*
Filename	: ioPaperDoll.cpp
Author		: megamegy
Date		: 2013.11.21
*/
//----------------------------------------------------------------------------------

#include "stdafx.h"
#include "ioPaperDoll.h"
//#include "ioUI3DRender.h"
//#include "ioCreateChar.h"

//----------------------------------------------------------------------------------
/*
»ý¼º / ¼Ò¸ê
*/
ioPaperDoll::ioPaperDoll()
	: ioWndEX()
	, m_charrenderer( 0 )
	, m_character( 0 )
{
	//ioCreateChar *pChar = m_charrenderer->CreateChar( rkInfo, "ShopChar" );
	//if( !pChar )	return;

	//if( m_aMyChar )
	//{
	//	m_fPreYaw = m_aMyChar->GetYaw();
	//}

	//D3DXVECTOR3 vCharPos( 0.0f, 0.0f, m_fZPosition );
	//pChar->SetPosition( vCharPos );
	//pChar->SetYaw( m_fPreYaw );

	//m_pUI3DRender->SetLookAt( vCharPos );

	//SAFEDELETE( m_aMyChar );
	//m_aMyChar    = pChar;
	//	if( m_aMyChar )
	//		m_aMyChar->AttachEffect( "shop_delay_eff.txt", NULL );

	//if( m_iClassType != rkInfo.m_class_type )
	//{
	//	m_iClassType = rkInfo.m_class_type;
	//}



	//SAFEDELETE( m_pUI3DRender );

	//m_pUI3DRender = new ioUI3DRender;
	//m_pUI3DRender->Initialize( "config/sp2_stage_charselect_map.ini", "shop_cam" );
	
}

ioPaperDoll::~ioPaperDoll()
{
//	SAFEDELETE( m_character );
//	SAFEDELETE( m_charrenderer );	
}