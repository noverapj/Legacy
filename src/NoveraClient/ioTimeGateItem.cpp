#include "StdAfx.h"
#include "ioTimeGateItem.h"
#include "ioZoneEffectWeapon.h"

ioTimeGateItem::ioTimeGateItem(void)
{
	ClearData();
}

ioTimeGateItem::ioTimeGateItem( const ioTimeGateItem& rhs ):
ioWeaponItem( rhs ),
m_BranchDelayAnimation( rhs.m_BranchDelayAnimation ),
m_FullChargeAni( rhs.m_FullChargeAni ), 
m_ReadyCircle( rhs.m_ReadyCircle ),
m_EnableCircle( rhs.m_EnableCircle ),
m_fRange( rhs.m_fRange ),
m_fRadius( rhs.m_fRadius ),
m_fMoveSpeed( rhs.m_fMoveSpeed ),
m_fHeightGap( rhs.m_fHeightGap ),
m_PortalStartEffect( rhs.m_PortalStartEffect ),
m_szStartPointDummyName( rhs.m_szStartPointDummyName ),
m_szEndPointDummyName( rhs.m_szEndPointDummyName ),
m_fMaxBullet( rhs.m_fMaxBullet ),
m_fIncreseGaugeTicDelay( rhs.m_fIncreseGaugeTicDelay ),
m_fIncreseGaugeTicRun( rhs.m_fIncreseGaugeTicRun ),
m_fIncreseGaugeTicDash( rhs.m_fIncreseGaugeTicDash ),
m_fIncreseGaugeTicAttack( rhs.m_fIncreseGaugeTicAttack ),
m_fIncreseGaugeTicWound( rhs.m_fIncreseGaugeTicWound ),
m_iChargeAttackNeedBullet( rhs.m_iChargeAttackNeedBullet ),
m_iJumpChargeAttackNeedBullet( rhs.m_iJumpChargeAttackNeedBullet ),
m_szCreateDummySound( rhs.m_szCreateDummySound )
{
	ClearData();
}

ioTimeGateItem::~ioTimeGateItem(void)
{
}

ioItem*	ioTimeGateItem::Clone()
{
	return new ioTimeGateItem( *this );
}

ioWeaponItem::WeaponSubType ioTimeGateItem::GetSubType() const
{
	return WST_TIMEGATE_ITEM;
}

void	ioTimeGateItem::LoadProperty( ioINILoader& rkLoader )
{
	ioWeaponItem::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "branch_delay_animation", "", szBuf, MAX_PATH );
	m_BranchDelayAnimation	= szBuf;
	rkLoader.LoadString_e( "full_charge_ani", "", szBuf, MAX_PATH );
	m_FullChargeAni	= szBuf;

	rkLoader.LoadString_e( "ready_circle", "", szBuf, MAX_PATH );
	m_ReadyCircle	= szBuf;
	rkLoader.LoadString_e( "enable_circle", "", szBuf, MAX_PATH );
	m_EnableCircle	= szBuf;

	rkLoader.LoadString_e( "portal_start_effect", "", szBuf, MAX_PATH );
	m_PortalStartEffect	= szBuf;

	rkLoader.LoadString_e( "start_point_dummy_name", "", szBuf, MAX_PATH );
	m_szStartPointDummyName	= szBuf;
	rkLoader.LoadString_e( "end_point_dummy_name", "", szBuf, MAX_PATH );
	m_szEndPointDummyName	= szBuf;

	rkLoader.LoadString_e( "create_dummy_sound", "", szBuf, MAX_PATH );
	m_szCreateDummySound	= szBuf;

	m_fRange		= rkLoader.LoadFloat_e( "magic_circle_range", FLOAT1000 );
	m_fRadius		= rkLoader.LoadFloat_e( "magic_circle_radius", FLOAT1000 );
	m_fMoveSpeed	= rkLoader.LoadFloat_e( "magic_circle_move_speed", 350.0f );
	m_fHeightGap	= rkLoader.LoadFloat_e( "magic_circle_height_gap", 20.0f );

	m_fMaxBullet	= rkLoader.LoadFloat_e( "max_gauge", FLOAT100 );
	m_fIncreseGaugeTicDelay	= rkLoader.LoadFloat_e( "increase_gauge_tic_delay", 0.0f );
	m_fIncreseGaugeTicRun	= rkLoader.LoadFloat_e( "increase_gauge_tic_run", 0.0f );
	m_fIncreseGaugeTicDash	= rkLoader.LoadFloat_e( "increase_gauge_tic_dash", 0.0f );
	m_fIncreseGaugeTicAttack	= rkLoader.LoadFloat_e( "increase_gauge_tic_attack", 0.0f );
	m_fIncreseGaugeTicWound	= rkLoader.LoadFloat_e( "increase_gauge_tic_wound", 0.0f );
	m_iChargeAttackNeedBullet	= rkLoader.LoadInt_e( "need_charging_attack_gauge", 0 );
	m_iJumpChargeAttackNeedBullet	= rkLoader.LoadInt_e( "need_jump_charging_attack_gauge", 0 );
}

void	ioTimeGateItem::SetNormalAttack( ioBaseChar* pOwner, int iCurCombo )
{
	ClearCancelInfo();
	ClearKeyReserve();

	int iMaxCombo = GetMaxCombo();
	if( COMPARE( iCurCombo, 0, iMaxCombo ) )
	{
		m_TimeGateState = TGS_CHARGING;
		m_dwChargeStartTime = FRAMEGETTIME();
		m_iCurCombo = iCurCombo;

		ioEntityGroup *pGrp = pOwner->GetGroup();
		pGrp->ClearAllActionAni( FLOAT100, true );
		pGrp->ClearAllLoopAni( FLOAT100, true );

		int iAniID = pGrp->GetAnimationIdx( m_BranchDelayAnimation );
		float fTimeRate = (float)m_dwChargeTime / pGrp->GetAnimationFullTime( iAniID );
		pGrp->SetActionAni( iAniID, FLOAT100, FLOAT100, FLOAT1, FLOAT1/fTimeRate, 0.0f, 0.0f, true );

		pOwner->SetState( CS_ATTACK );
		pOwner->SetChargingState( true );
	}
	else
	{
		ioAttackableItem::SetNormalAttack( pOwner, iCurCombo );
		pOwner->SetAttackMoveEnable( false );
		pOwner->ChangeDirectionByInputDir( true );
	}
}

void	ioTimeGateItem::CheckNormalAttack( ioBaseChar* pOwner )
{
	switch( m_TimeGateState )
	{
	case	TGS_CHARGING:
		{
			OnCharging( pOwner );
		}
		break;

	case	TGS_NORMAL_ATTACK:
		if( pOwner->IsNeedProcess() )
		{
			CheckCancelReserve( pOwner );

			if( ProcessCancel(pOwner) )
				return;
		}
		ioAttackableItem::CheckNormalAttack( pOwner );
		break;

	case	TGS_FIRST_CIRCLE_LOOP:
		{
			OnFirstCircleLoop( pOwner );
		}
		break;

	case	TGS_SECOND_CIRCLE_LOOP:
		{
			OnSecondCircleLoop( pOwner );
		}
		break;

	case	TGS_PORTAL_ACTION:
		{
			ChangeToDelay( pOwner );
		}
		break;
	}
}

void	ioTimeGateItem::OnItemGaugeOK( ioBaseChar* pOwner, SP2Packet& rkPacket )
{
	if( !pOwner ) return;

	int iState;
	rkPacket >> iState;
	rkPacket >> m_iCurCombo;
	m_TimeGateState	= (TimeGateState)iState;

	switch( iState )
	{
	case	TGS_NONE:
		{
			ioEntityGroup *pGrp = pOwner->GetGroup();
			if( pGrp )
			{
				pGrp->ClearAllLoopAni( FLOAT100, true );
				pGrp->ClearAllActionAni( FLOAT100, true );
			}

			pOwner->SetState( CS_DELAY );
		}
		break;

	case	TGS_NORMAL_ATTACK:
		{
			ChangeToNormalAttack( pOwner );
		}
		break;

	case	TGS_FIRST_CIRCLE_LOOP:
		{
			ChangeToFirstCircleLoop( pOwner );
		}
		break;

	case	TGS_SECOND_CIRCLE_LOOP:
		{
			ChangeToSecondCircleLoop( pOwner );
		}
		break;

	case	TGS_PORTAL_ACTION:
		{
			D3DXVECTOR3	vStart, vEnd;
			rkPacket >> vStart >> vEnd;

			SetPortalAction( pOwner, vStart, vEnd );
		}
	}
}

void	ioTimeGateItem::OnItemGaugeCancel( ioBaseChar* pOwner, SP2Packet& rkPacket )
{

}

void	ioTimeGateItem::OnCharging( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )	
		return;

	if( m_dwChargeStartTime == 0 )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( pOwner->IsAttackKeyDown() && m_fCurBullet >= m_iChargeAttackNeedBullet )
	{
		if( m_dwChargeStartTime + m_dwChargeTime <= dwCurTime )
		{
			ChangeToFirstCircleLoop( pOwner );
		}
	}
	else
	{
		pOwner->SetChargingState( false );
		pOwner->SetAttackMoveEnable( false );
		ChangeToNormalAttack( pOwner );
	}
}

void	ioTimeGateItem::OnFirstCircleLoop( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKey() )
	{
		ChangeToSecondCircleLoop( pOwner );
		return;
	}

	if( pOwner->CheckMagicCircle() )
	{
		pOwner->UpdateMagicCirclePos( m_fMoveSpeed, m_fRange, m_fHeightGap, false );

		D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
		D3DXVECTOR3 vCharDir = pOwner->GetMagicCirclePos() - vCharPos;
		vCharDir.y = 0.0f;

		D3DXVec3Normalize( &vCharDir, &vCharDir );

		pOwner->SetTargetRotToDir( vCharDir, true );

		//pOwner->CheckChangeDirectionForMagicCircle();		
		pOwner->ChangeDirectionByInputDir( true );
	}
}

void	ioTimeGateItem::OnSecondCircleLoop( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	if( !pOwner->IsNeedProcess() )
		return;

	if( pOwner->IsAttackKey() )
	{
		ChangeToPortalAction( pOwner );
		return;
	}

	if( pOwner->CheckMagicCircle() )
	{
		pOwner->UpdateMagicCirclePos( m_fMoveSpeed, m_fRadius, m_fRange, m_fHeightGap, false, m_vFirstCirclePos );

		D3DXVECTOR3 vCharPos = pOwner->GetWorldPosition();
		D3DXVECTOR3 vCharDir = pOwner->GetMagicCirclePos() - vCharPos;
		vCharDir.y = 0.0f;

		D3DXVec3Normalize( &vCharDir, &vCharDir );

		pOwner->SetTargetRotToDir( vCharDir, true );

		//pOwner->CheckChangeDirectionForMagicCircle();
		pOwner->ChangeDirectionByInputDir( true );
	}
}

void	ioTimeGateItem::ChangeToNormalAttack( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	ClearCancelInfo();

	DWORD dwCurTime = FRAMEGETTIME();
	const AttackAttribute *pAttribute = m_FormalAttack.GetAttribute( AT_NORMAL, m_iCurCombo );
	if( pAttribute )
	{
		pOwner->SetNormalAttackByAttribute( *pAttribute );

		m_szCurAni = pAttribute->m_AttackAnimation;
		m_fCurAniRate = pAttribute->m_fAttackAniRate;

		m_dwInputCancelCheckTime = dwCurTime;
		m_dwCancelCheckTime = dwCurTime;
	}
	pOwner->SetAutoTarget( ATT_NORMAL );
	pOwner->ApplyNormalAttackResult( m_iCurCombo );
	m_TimeGateState = TGS_NORMAL_ATTACK;

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_TimeGateState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTimeGateItem::ClearData()
{
	m_iCurCombo	= 0;
	m_dwChargeStartTime	= 0;
	m_TimeGateState	= TGS_NONE;
	m_dwPortalStartEffectIndex	= 0;
	m_dwStartPointDummyIndex	= 0;
	m_dwEndPointDummyIndex		= 0;
	m_fCurBullet	= 0.0f;
}

bool	ioTimeGateItem::IsEnoughJumpChargeAttackBullet()
{
	if( m_iJumpChargeAttackNeedBullet <= m_fCurBullet )
		return	true;

	return	false;
}

void	ioTimeGateItem::ChangeToFirstCircleLoop( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_TimeGateState	= TGS_FIRST_CIRCLE_LOOP;

	DecreaseBullet( m_iChargeAttackNeedBullet );

	int iAniID = pGrp->GetAnimationIdx( m_FullChargeAni );

	pGrp->ClearAllLoopAniExceptOne( iAniID, FLOAT100, true );
	pGrp->ClearAllActionAni( FLOAT10, true );
	pGrp->SetLoopAni( iAniID, FLOAT100 );
	pOwner->SetAttackMoveEnable( false );
	pOwner->ChangeDirectionByInputDir( true );

	ioHashString szDummyCircle;
	pOwner->CreateMagicCircle( m_ReadyCircle, m_EnableCircle, szDummyCircle, m_fMagicCircleOffSet );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_TimeGateState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTimeGateItem::ChangeToSecondCircleLoop( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	m_TimeGateState	= TGS_SECOND_CIRCLE_LOOP;

	if( pOwner->CheckMagicCircle() )
		pOwner->UpdateMagicCirclePos( m_fMoveSpeed, m_fRange, m_fHeightGap, false );

	m_vFirstCirclePos	= pOwner->GetMagicCirclePos();

	pOwner->DestroyMagicCircle();

	ioHashString szDummyCircle;
	pOwner->CreateMagicCircle( m_ReadyCircle, m_EnableCircle, szDummyCircle, m_vFirstCirclePos, m_fRadius );

	ioPlayStage* pStage = pOwner->GetCreator();
	if( pOwner->IsOwnerChar() || ( pStage && pStage->GetOwnerTeam() == pOwner->GetTeam() ) )
		m_dwPortalStartEffectIndex	= AddEffect( m_PortalStartEffect, m_vFirstCirclePos, pOwner );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_TimeGateState;
		kPacket << m_iCurCombo;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTimeGateItem::ChangeToPortalAction( ioBaseChar* pOwner )
{
	if( !pOwner )
		return;

	ioEntityGroup *pGrp = pOwner->GetGroup();
	if( !pGrp )
		return;

	m_TimeGateState	= TGS_PORTAL_ACTION;

	if( pOwner->CheckMagicCircle() )
		pOwner->UpdateMagicCirclePos( m_fMoveSpeed, m_fRadius, m_fRange, m_fHeightGap, false, m_vFirstCirclePos );

	m_vSecondCirclePos	= pOwner->GetMagicCirclePos();

	pOwner->DestroyMagicCircle();

	if ( m_dwStartPointDummyIndex != -1 )
		DestroyDummy( pOwner, m_dwStartPointDummyIndex );
	if ( m_dwEndPointDummyIndex != -1 )
		DestroyDummy( pOwner, m_dwEndPointDummyIndex );

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar* pDummy = NULL;
	pDummy = CreateDummy( pOwner, m_szStartPointDummyName, m_vFirstCirclePos, vDir, iCurIndex );
	if ( pDummy )
		m_dwStartPointDummyIndex = pDummy->GetDummyCharIdx();
	iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	vPos.y = pOwner->GetBottomHeight();
	pDummy = CreateDummy( pOwner, m_szEndPointDummyName, m_vSecondCirclePos, vDir, iCurIndex );
	if ( pDummy )
		m_dwEndPointDummyIndex = pDummy->GetDummyCharIdx();

	if( m_dwPortalStartEffectIndex != 0 )
		RemoveEffect( m_dwPortalStartEffectIndex, pOwner );

	if( !m_szCreateDummySound.IsEmpty() )
		g_SoundMgr.PlaySound( m_szCreateDummySound, pGrp->GetParentSceneNode() );

	if( pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_ITEM_GAUGE_OK );
		FillItemGaugePacketHeader( pOwner, kPacket );
		kPacket << (int)m_TimeGateState;
		kPacket << m_iCurCombo;
		kPacket << m_vFirstCirclePos;
		kPacket << m_vSecondCirclePos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void	ioTimeGateItem::ChangeToDelay( ioBaseChar* pOwner )
{
	pOwner->SetState( CS_DELAY );
	m_TimeGateState	= TGS_NONE;
}

void	ioTimeGateItem::SetPortalAction( ioBaseChar* pOwner, D3DXVECTOR3 vStart, D3DXVECTOR3 vEnd )
{
	if( !pOwner )
		return;

	m_TimeGateState	= TGS_PORTAL_ACTION;

	pOwner->DestroyMagicCircle();

	if ( m_dwStartPointDummyIndex != -1 )
		DestroyDummy( pOwner, m_dwStartPointDummyIndex );
	if ( m_dwEndPointDummyIndex != -1 )
		DestroyDummy( pOwner, m_dwEndPointDummyIndex );

	D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	int iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	ioDummyChar* pDummy = NULL;
	pDummy = CreateDummy( pOwner, m_szStartPointDummyName, vStart, vDir, iCurIndex );
	if ( pDummy )
		m_dwStartPointDummyIndex = pDummy->GetDummyCharIdx();
	iCurIndex = g_DummyCharMgr.GetCurLocalIndex();
	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();
	vPos.y = pOwner->GetBottomHeight();
	pDummy = CreateDummy( pOwner, m_szEndPointDummyName, vEnd, vDir, iCurIndex );
	if ( pDummy )
		m_dwEndPointDummyIndex = pDummy->GetDummyCharIdx();

	if( m_dwPortalStartEffectIndex != 0 )
		RemoveEffect( m_dwPortalStartEffectIndex, pOwner );
}

DWORD	ioTimeGateItem::AddEffect( const ioHashString& szEffectName, D3DXVECTOR3 vPos, ioBaseChar *pOwner )
{
	if ( !pOwner )
		return -1;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return -1;

	D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
	ioMapEffect *pMapEffect = pStage->CreateMapEffect( szEffectName, vPos, vScale );
	if ( !pMapEffect )
		return -1;

	return pMapEffect->GetUniqueID();
}

void	ioTimeGateItem::RemoveEffect( DWORD& rhID, ioBaseChar *pOwner )
{
	if ( rhID == -1 )
		return;

	if ( !pOwner )
		return;

	ioPlayStage* pStage = pOwner->GetCreator();
	if ( !pStage )
		return;

	ioMapEffect *pEff = pStage->FindMapEffect( rhID );
	if ( pEff )
		pEff->EndEffectForce();

	rhID = -1;
}

ioDummyChar*	ioTimeGateItem::CreateDummy( ioBaseChar* pOwner, const ioHashString& szName, const D3DXVECTOR3& vPos, const D3DXVECTOR3& vDir, const int iIndex )
{
	if ( !pOwner )
		return NULL;
	ioPlayStage *pStage	= pOwner->GetCreator();
	if ( !pStage )
		return NULL;


	ioDummyChar *pDummy = pStage->CreateDummyChar( szName, iIndex, pOwner->GetCharName(), vPos, 0.0f, 0 );
	return pDummy;
}

void	ioTimeGateItem::DestroyDummy( ioBaseChar* pOwner, DWORD& iDummyIndex )
{
	if( pOwner && iDummyIndex != -1 )
	{
		ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, iDummyIndex );
		if( pDummy )
			pDummy->SetDieState( true );

		iDummyIndex = -1;
	}
}

int ioTimeGateItem::GetCurBullet()
{
	return (int)m_fCurBullet;
}

int ioTimeGateItem::GetMaxBullet()
{
	return (int)m_fMaxBullet;
}

void ioTimeGateItem::SetCurBullet( int iCurBullet )
{
	if( iCurBullet < 0 )
		return;

	float fCurBullet = iCurBullet;
	fCurBullet = max( 0, min( fCurBullet, m_fMaxBullet ) );

	m_fCurBullet = fCurBullet;
}

int ioTimeGateItem::GetNeedBullet()
{
	ioPlayStage *pStage = g_ItemMaker.GetPlayStage();
	ioBaseChar *pOwner = NULL;
	if( pStage )
		pOwner = pStage->GetBaseChar( m_OwnerName );

	if( !pOwner )
		return m_iChargeAttackNeedBullet;

	if( pOwner->GetState() == CS_JUMP )
		return	m_iJumpChargeAttackNeedBullet;

	return	m_iChargeAttackNeedBullet;
}

void ioTimeGateItem::UpdateExtraData( ioBaseChar *pOwner )
{
	if( !pOwner )
		return;

	float	fTimePerSec	= g_FrameTimer.GetSecPerFrame();
	float	fIncreaseRate	= 0.0f;

	switch( pOwner->GetState() )
	{
	case	CS_DELAY:
		{
			fIncreaseRate	= m_fIncreseGaugeTicDelay;
		}
		break;

	case	CS_DASH:
		{
			fIncreaseRate	= m_fIncreseGaugeTicDash;
		}
		break;

	case	CS_RUN:
		{
			fIncreaseRate	= m_fIncreseGaugeTicRun;
		}
		break;

	case	CS_ATTACK:
		{
			if( m_TimeGateState == TGS_NONE || m_TimeGateState == TGS_NORMAL_ATTACK )
				fIncreaseRate	= m_fIncreseGaugeTicAttack;
		}
		break;

	case	CS_DASH_ATTACK:
		{
			fIncreaseRate	= m_fIncreseGaugeTicAttack;
		}
		break;

	case	CS_WOUNDED:
	case	CS_BLOW_WOUNDED:
		{
			fIncreaseRate	= m_fIncreseGaugeTicWound;
		}
		break;
	}

	if( m_fCurBullet < m_fMaxBullet )
	{
		float fGauge = fIncreaseRate * fTimePerSec;

		m_fCurBullet += fGauge;
		m_fCurBullet = min( m_fCurBullet, m_fMaxBullet );
	}
}

int		ioTimeGateItem::GetJumpChargeNeedBullet()
{
	return	m_iJumpChargeAttackNeedBullet;
}

bool	ioTimeGateItem::CheckWoundAfterUseExtendState( ioBaseChar* pOwner, BlowTargetType eType )
{
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	// 마땅히 처리할 곳이 없어서 여기에서 피격후 이펙트 삭제 처리를 한다.
	if( m_dwPortalStartEffectIndex != 0 )
		RemoveEffect( m_dwPortalStartEffectIndex, pOwner );

	return	false;
}

void	ioTimeGateItem::DecreaseBullet( int iBullet )
{
	m_fCurBullet	-= iBullet;
	m_fCurBullet	= max( m_fCurBullet, 0.0f );
}