#include "stdafx.h"

#include "ioEtcItem.h"
#include "ioSlotMgr.h"

///////////////////////////////////////////////////////////////////////////
//소모품  
//////////////////////////////////////////////////////////////////////////

ioEtcItemConsumption::ioEtcItemConsumption()
{

}

ioEtcItemConsumption::~ioEtcItemConsumption()
{

}

void ioEtcItemConsumption::OnInit( ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}
}

void ioEtcItemConsumption::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	if( kSlot.GetUse() == 0 )
	{
	}

	g_SlotMgr.UpdateItem(m_dwType);
}

void ioEtcItemConsumption::OnBuy()
{
	g_SlotMgr.UpdateItem(m_dwType);
}

bool ioEtcItemConsumption::OnUseBtnPush( int iType, ioUserEtcItem *pUserEtcItem, bool bPresentLimitCheck )
{
	return true;
}

void ioEtcItemConsumption::OnSell( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem )
{
	ioEtcItem::OnSell( iType, rkPacket, pUserEtcItem );
}


void ioEtcItemConsumption::LoadProperty( ioINILoader &rkLoader )
{
	ioEtcItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "sloticon_img", "", szBuf, MAX_PATH );
	m_strIcon = szBuf;

	
	strcpy(szBuf, "");
	rkLoader.LoadString_e( "sloticon_backimg", "", szBuf, MAX_PATH );
	m_strBackImg = szBuf;

	strcpy(szBuf, "");
	rkLoader.LoadString_e( "slot_buff", "", szBuf, MAX_PATH );
	m_strSlotBuff = szBuf;

	m_dwCoolTime = rkLoader.LoadInt_e( "cooltime", 100000 );
}


//////////////////////////////////////////////////////////////////////////
//즉시 부활 아이템 
//////////////////////////////////////////////////////////////////////////

ioEtcItemRevive::ioEtcItemRevive()
{

}

ioEtcItemRevive::~ioEtcItemRevive()
{

}

void ioEtcItemRevive::OnUse( int iType, SP2Packet &rkPacket, ioUserEtcItem *pUserEtcItem, ioPlayStage *pPlayStage )
{
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "%s pUserEtcItem == NULL.", __FUNCTION__ );
		return;
	}

	ioUserEtcItem::ETCITEMSLOT kSlot;
	kSlot.m_iType = iType;

	rkPacket >> kSlot.m_iValue1;
	rkPacket >> kSlot.m_iValue2;

	if( kSlot.m_iValue1 == 0 )
	{
		pUserEtcItem->DeleteEtcItem( kSlot.m_iType );
	}
	else
	{
		pUserEtcItem->SetEtcItem( kSlot );
	}

	if( kSlot.GetUse() == 0 )
	{
	}

	g_SlotMgr.UpdateItem(m_dwType);
}


void ioEtcItemRevive::OnBuy()
{
	g_SlotMgr.UpdateItem(m_dwType);
}