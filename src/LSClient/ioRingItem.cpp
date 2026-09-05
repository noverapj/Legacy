#include "StdAfx.h"
#include "ioRingItem.h"

ioRingItem::ioRingItem(void)
{
}

ioRingItem::ioRingItem( const ioRingItem &rhs ) :
ioItem( rhs ),
m_fLocalRandomNumber( rhs.m_fLocalRandomNumber ),
m_iManualNumber( rhs.m_iManualNumber ),
m_szToolTipText( rhs.m_szToolTipText )
{
}

ioRingItem::~ioRingItem(void)
{
}

ioItem::ItemType ioRingItem::GetType() const
{
	return IT_RING;
}

ioItem* ioRingItem::Clone()
{
	return new ioRingItem( *this );
}

void ioRingItem::LoadProperty( ioINILoader &rkLoader )
{
	ioItem::LoadProperty( rkLoader );
	char szBuf[MAX_PATH];
	SetItemType();      //Default.
	m_fLocalRandomNumber = rkLoader.LoadFloat_e( "local_random_num", 0.0f );

	m_iManualNumber = rkLoader.LoadInt_e( "manual_number", 0 );
	m_iAbilityMin = rkLoader.LoadInt_e( "ability_min", 0 );
	m_iAbilityMax = rkLoader.LoadInt_e( "ability_max", 0 );
	m_iAbilityGap = rkLoader.LoadInt_e( "ability_gap", 0 );
	m_bAbilitySign = rkLoader.LoadBool_e( "ability_sign", false );

	rkLoader.LoadString_e( "reinforce_text", "", szBuf, MAX_PATH );
	m_szReinforceText = szBuf;
	rkLoader.LoadString_e( "reinforce_result_text", "", szBuf, MAX_PATH );
	m_szReinforceResultText = szBuf;
	rkLoader.LoadString_e( "tooltip_text", "", szBuf, MAX_PATH );
	m_szToolTipText = szBuf;
}

void ioRingItem::InitializeBeforeEquip( ioBaseChar *pOwner, ioPlayStage* pStage )
{
	ioItem::InitializeBeforeEquip( pOwner, pStage );

	if( !P2PNetwork::IsNetworkPlaying() )
		SetAccessoryRandomNum( m_fLocalRandomNumber );
}

const int& ioRingItem::GetManualIndex() const
{
	return m_iManualNumber;
}

const ioHashString& ioRingItem::GetToolTipText() const
{
	return m_szToolTipText;
}

const int ioRingItem::GetAbilityMax() const
{
	if( m_bAbilitySign )
		return m_iAbilityMin;

	return m_iAbilityMax;
}