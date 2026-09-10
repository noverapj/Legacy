

#include "stdafx.h"

#include "ioDrinkItem.h"
#include "ioBaseChar.h"

#include "WeaponDefine.h"

ioDrinkItem::ioDrinkItem()
{
	m_dwDrinkStartTime = 0;
	m_dwDrinkEndTime = 0;
}

ioDrinkItem::ioDrinkItem( const ioDrinkItem &rhs )
: ioObjectItem( rhs ),
  m_DrinkAnimation( rhs.m_DrinkAnimation ),
  m_fDrinkAnimationRate( rhs.m_fDrinkAnimationRate ),
  m_dwDrinkAnimationDelay( rhs.m_dwDrinkAnimationDelay ),
  m_bRandom( rhs.m_bRandom )
{
	m_dwDrinkStartTime = 0;
	m_dwDrinkEndTime = 0;
}

ioDrinkItem::~ioDrinkItem()
{
}

void ioDrinkItem::LoadProperty( ioINILoader &rkLoader )
{
	ioObjectItem::LoadProperty( rkLoader );
	
	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "drink_animation", "", szBuf, MAX_PATH );
	m_DrinkAnimation = szBuf;

	m_fDrinkAnimationRate = rkLoader.LoadFloat_e( "drink_animation_rate", FLOAT1 );
	m_dwDrinkAnimationDelay = rkLoader.LoadInt_e( "drink_animation_delay", 0 );

	m_bRandom = rkLoader.LoadBool_e( "owner_buff_rnd", false );
}

ioItem* ioDrinkItem::Clone()
{
	return new ioDrinkItem( *this );
}

ioObjectItem::ObjectSubType ioDrinkItem::GetObjectSubType() const
{
	return OST_DRINK;
}

void ioDrinkItem::SetDrinkState( ioBaseChar *pChar )
{
	ioEntityGroup *pGrp = pChar->GetGroup();
	int iAniID = pGrp->GetAnimationIdx( m_DrinkAnimation );
	if( iAniID == -1 ) return;

	if( m_OwnerBuffList.empty() )
		return;

	pChar->SetState( CS_DRINK );

	pGrp->ClearAllActionAni( FLOAT100, true );
	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->SetActionAni( m_DrinkAnimation, FLOAT100, FLOAT100, FLOAT1, FLOAT1/m_fDrinkAnimationRate );

	m_dwDrinkStartTime = m_dwDrinkEndTime = FRAMEGETTIME();
	m_dwDrinkEndTime += pGrp->GetAnimationFullTime( iAniID ) * m_fDrinkAnimationRate + m_dwDrinkAnimationDelay;

	int iCnt = m_OwnerBuffList.size();

	if( m_bRandom )
	{
		IORandom random;
		DWORD dwSeed = 0;
		int iRand = 0;
		dwSeed = pChar->GetRandomSeed();
		random.SetRandomSeed( dwSeed );

		iRand = random.Random( iCnt );
		if( COMPARE( iRand, 0, iCnt ) )
		{
			pChar->AddNewBuffWithAnimateTime( m_OwnerBuffList[iRand].m_BuffName, "", "", NULL,
											  m_dwDrinkEndTime - m_dwDrinkStartTime );
		}
	}
	else
	{
		for( int i=0; i < iCnt; ++i )
		{
			pChar->AddNewBuffWithAnimateTime( m_OwnerBuffList[i].m_BuffName, "", "", NULL,
											  m_dwDrinkEndTime - m_dwDrinkStartTime );
		}
	}
}

bool ioDrinkItem::ProcessDrinkState( ioBaseChar *pChar )
{
	if( m_dwDrinkEndTime + pChar->GetActionStopTotalDelay() < FRAMEGETTIME() )
		return true;

	return false;
}