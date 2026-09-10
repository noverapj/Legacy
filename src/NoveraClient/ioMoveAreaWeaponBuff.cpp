
#include "stdafx.h"

#include "ioMoveAreaWeaponBuff.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ioMoveAreaWeaponBuff::ioMoveAreaWeaponBuff()
{
}

ioMoveAreaWeaponBuff::ioMoveAreaWeaponBuff( const ioMoveAreaWeaponBuff &rhs )
	: ioBuff( rhs ),
	  m_szAttachedAni( rhs.m_szAttachedAni ),
	  m_fAttachedAniRate( rhs.m_fAttachedAniRate ),
	  m_bDisableDownState( rhs.m_bDisableDownState ),
	  m_OwnerBuffList( rhs.m_OwnerBuffList ),
	  m_bColSkip( rhs.m_bColSkip ),
	  m_szAreaWeapon( rhs.m_szAreaWeapon ),
	  m_fOffsetHeight( rhs.m_fOffsetHeight )
{
}

ioMoveAreaWeaponBuff::~ioMoveAreaWeaponBuff()
{
}

void ioMoveAreaWeaponBuff::LoadProperty( ioINILoader &rkLoader )
{
	ioBuff::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "attached_ani", "", szBuf, MAX_PATH );
	m_szAttachedAni = szBuf;
	m_fAttachedAniRate = rkLoader.LoadFloat_e( "attached_ani_rate", FLOAT1 );

	m_bDisableDownState  = rkLoader.LoadBool_e( "disable_down_state", false );	
	m_bColSkip = rkLoader.LoadBool_e( "set_col_skip", false );

	rkLoader.LoadString_e( "area_weapon_name", "", szBuf, MAX_PATH );
	m_szAreaWeapon = szBuf;

	m_fOffsetHeight = rkLoader.LoadFloat_e( "offset_height", 0.0f );

	LoadOwnerBuffList( rkLoader );
}

ioBuff* ioMoveAreaWeaponBuff::Clone()
{
	return new ioMoveAreaWeaponBuff( *this );
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioMoveAreaWeaponBuff::StartBuff( ioBaseChar *pOwner )
{
	ioBuff::StartBuff( pOwner );

	Init();
}

bool ioMoveAreaWeaponBuff::ApplyStartBuff(ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime )
{
	if( !ioBuff::ApplyStartBuff( pOwner, dwGlobalTime, dwGapTime ) )
		return false;	

	Init();
	return true;
}

void ioMoveAreaWeaponBuff::ProcessBuff( float fTimePerSec )
{
	switch( m_iOperationType )
	{
	case OT_SWITCH:
		if( !CheckSwitchType( fTimePerSec ) )
		{
			SetReserveEndBuff();
			return;
		}

		CheckCallingProcess();
		break;
	case OT_DELAY:
	case OT_ACTIVE:
		ioBuff::ProcessBuff( fTimePerSec );
		break;
	case OT_PASSIVE:
		CheckCallingAreaWeapon();
		break;
	}

	if ( CheckOwnerStateCheck( m_pOwner ) || !CheckAreaWeapon( m_pOwner ) )
		m_BuffState = BS_End;
	
	switch( m_BuffState )
	{
	case BS_None:
		SetTeleportState( m_pOwner );
		break;
	case BS_End:
		SetReserveEndBuff();
		break;
	}
}

void ioMoveAreaWeaponBuff::EndBuff()
{
	ioBuff::EndBuff();

	SetOwnerBuffList( m_pOwner );
	//RemoveOwnerBuffList( m_pOwner );
}

void ioMoveAreaWeaponBuff::ApplyExtraBuffInfo( SP2Packet &rkPacket )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case BS_End:
		{
			SetReserveEndBuff();
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//버프 리스트 읽기
void ioMoveAreaWeaponBuff::LoadOwnerBuffList( ioINILoader &rkLoader )
{
	int iCnt = rkLoader.LoadInt_e( "owner_buff_cnt", 0 );
	m_OwnerBuffList.clear();

	if( iCnt <= 0 ) 
		return;

	m_OwnerBuffList.reserve( iCnt );

	char szTitle[MAX_PATH], szBuf[MAX_PATH];
	for( int i=0; i < iCnt; ++i )
	{
		wsprintf_e( szTitle, "owner_buff%d", i+1 );
		rkLoader.LoadString( szTitle, "", szBuf, MAX_PATH );

		m_OwnerBuffList.push_back( szBuf );
	}
}

void ioMoveAreaWeaponBuff::SetOwnerBuffList( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;
	
	if( m_OwnerBuffList.empty() ) 
		return;

	int nBuffCnt = m_OwnerBuffList.size();
	for( int i=0; i < nBuffCnt; ++i )
	{
		ioHashString szBuffName = m_OwnerBuffList[i];
		if( !szBuffName.IsEmpty() )
			pOwner->ReserveAddNewBuff( szBuffName, m_CreateChar, "", NULL );
	}
}

void ioMoveAreaWeaponBuff::RemoveOwnerBuffList( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	if( m_OwnerBuffList.empty() ) 
		return;

	int nBuffCnt = m_OwnerBuffList.size();
	for( int i=0; i < nBuffCnt; ++i )
	{
		ioBuff *pBuff = pOwner->GetBuff( m_OwnerBuffList[i], pOwner->GetCharName() );
		if ( pBuff )
			pBuff->SetReserveEndBuff();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ioMoveAreaWeaponBuff::Init()
{
	m_BuffState = BS_None;
}

void ioMoveAreaWeaponBuff::SetTeleportState( ioBaseChar *pOwner )
{
	if ( !pOwner )
		return;

	ioPlayStage *pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	if( m_szAttachedAni.IsEmpty() )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();

	pGrp->ClearAllLoopAni( FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT100, true );

	pGrp->SetLoopAni( m_szAttachedAni, FLOAT100, FLOAT1, FLOAT1/m_fAttachedAniRate );
	
	pOwner->SetState( CS_BUFF_ACTION );

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByName( m_szAreaWeapon, m_CreateChar );
	if ( pAreaWeapon )
	{
		D3DXVECTOR3 vAreaPos = pAreaWeapon->GetStartPos();
		vPos.x = vAreaPos.x;
		vPos.z = vAreaPos.z;
	}

	if ( !pOwner->IsFloating() )
		vPos.y = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z ) + m_fOffsetHeight;

	pOwner->SetWorldPosition( vPos );
	
	m_BuffState = BS_Teleport;
}

bool ioMoveAreaWeaponBuff::CheckOwnerStateCheck( ioBaseChar *pOwner )
{
	if( !pOwner ) 
		return false;

	CharState eState = pOwner->GetState();

	bool bDown = false;
	if( m_bDisableDownState )
	{
		if( eState == CS_FROZEN && pOwner->GetBlowFrozenState() )
			bDown = true;
		else if( eState == CS_BLOW_WOUNDED && !pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED && !pOwner->IsFloatingState() )
			bDown = true;
		else if( eState == CS_ETC_STATE && pOwner->IsApplyDownState(false) )
			bDown = true;
		else if( eState == CS_STOP_MOTION && pOwner->GetBlowStopMotionState() )
			bDown = true;
		else if( eState == CS_DROP_ZONE_DOWN )
			bDown = true;
		else if( eState == CS_ICE_STATE && pOwner->GetBlowStopMotionState() )
			bDown = true;

		if( bDown )
			return true;
	}

	bool bWound = false;
	if( m_bEnableWoundCancel )
	{
		if( eState == CS_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_BOUND_BLOW_WOUNDED )
			bWound = true;
		else if( eState == CS_WOUNDED )
			bWound = true;
		else if( eState == CS_ETC_STATE )
			bWound = true;

		if( bWound )
			return true;
	}

	return false;
}

bool ioMoveAreaWeaponBuff::CheckAreaWeapon( ioBaseChar *pOwner )
{
	ioAreaWeapon *pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByName( m_szAreaWeapon, m_CreateChar );
	if ( pAreaWeapon )
		return true;

	return false;
}

bool ioMoveAreaWeaponBuff::IsCharCollisionSkipState()
{
	return m_bColSkip;
}