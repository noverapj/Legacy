

#include "stdafx.h"

#include "ioBombMissileWeapon4.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioBombMissileWeapon4::m_ActionStopType = AST_NONE;
ActionStopType ioBombMissileWeapon4::m_DieActionStopType = AST_NONE;

ioBombMissileWeapon4::ioBombMissileWeapon4( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_WOUND_NO_SOUND |
					WP_DEF_NO_SOUND );

	m_pBombMesh = NULL;
	m_BombState = BS_FLOATING;

	m_bFirst = true;
	m_bResistance = false;
	m_fCurMoveSpeed = 0.0f;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;
}

ioBombMissileWeapon4::~ioBombMissileWeapon4()
{
	if( m_pBombMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioBombMissileWeapon4::~ioBombMissileWeapon4 - UnReleased Resouce" );
	}
}

bool ioBombMissileWeapon4::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return SetBombMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioBombMissileWeapon4::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_dwLiveTime = (DWORD)rkLoader.LoadInt_e( "live_time", 0 );

	m_dwRotateSpeed = (DWORD)rkLoader.LoadInt_e( "rotate_speed", 0 );

	m_bDisableUpDown = rkLoader.LoadBool_e( "disable_updown", false );

	m_bPlayWoundSound = rkLoader.LoadBool_e( "play_wound_sound", false );
	m_bDisableMapCollision = rkLoader.LoadBool_e( "disable_map_collision", false );

	// explosion cnt
	m_iMaxExplosionCnt = rkLoader.LoadInt_e( "max_explosion_cnt", 0 );
	m_iCurExplosionCnt = m_iMaxExplosionCnt;

	// char colision explosion
	m_CharColExplosion.m_iWeaponIdx = rkLoader.LoadInt_e( "char_col_explosion_index", 0 );
	m_CharColExplosion.m_iResistanceIdx = rkLoader.LoadInt_e( "char_col_explosion_index_resistance", 0 );

	m_CharColExplosion.m_dwCallTime = rkLoader.LoadInt_e( "char_col_explosion_time", 0 );

	rkLoader.LoadString_e( "char_col_explosion_wound_ani", "", szBuf, MAX_PATH );
	m_CharColExplosion.m_WoundedAnimation = szBuf;
	m_CharColExplosion.m_dwWoundedDuration = rkLoader.LoadInt_e( "char_col_explosion_wound_duration", 0 );
	m_CharColExplosion.m_bWoundedLoopAni = rkLoader.LoadBool_e( "char_col_explosion_wound_loop_ani", false );

	// attack key explosion
	m_AttackKeyExplosion.m_iWeaponIdx = rkLoader.LoadInt_e( "attack_key_explosion_index", 0 );
	m_AttackKeyExplosion.m_iResistanceIdx = rkLoader.LoadInt_e( "attack_key_explosion_index_resistance", 0 );

	m_AttackKeyExplosion.m_dwCallTime = rkLoader.LoadInt_e( "attack_key_explosion_time", 0 );

	rkLoader.LoadString_e( "attack_key_explosion_wound_ani", "", szBuf, MAX_PATH );
	m_AttackKeyExplosion.m_WoundedAnimation = szBuf;
	m_AttackKeyExplosion.m_dwWoundedDuration = rkLoader.LoadInt_e( "attack_key_explosion_wound_duration", 0 );
	m_AttackKeyExplosion.m_bWoundedLoopAni = rkLoader.LoadBool_e( "attack_key_explosion_wound_loop_ani", false );
}

bool ioBombMissileWeapon4::SetBombMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pBombMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pBombMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pBombMesh );
	GetParentSceneNode()->SetScale( ioMath::UNIT_ALL );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pBombMesh->GetSkeletonName().IsEmpty() && m_pBombMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
	{
		ioBiped *pLinked = pOwner->GetGroup()->GetBiped( szLinkBiped );
		if( pLinked )
		{
			vLinkPosOffset = pLinked->GetDerivedOrientation() * vLinkPosOffset + pLinked->GetDerivedPosition();
			vLinkPosOffset = vLinkPosOffset * pOwner->GetWorldScale();
			vLinkPosOffset = pOwner->GetWorldOrientation() * vLinkPosOffset + pOwner->GetWorldPosition();
			SetPosition( vLinkPosOffset );
		}
	}

	m_pBombMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pBombMesh->GetCollisionBox();

	AddEffectEvent( m_pBombMesh );
	return true;
}

void ioBombMissileWeapon4::DestroyResource( ioPlayStage *pStage )
{
	if( m_pBombMesh )
	{
		g_EffectFactory.DestroyEffect( m_pBombMesh );
		m_pBombMesh = NULL;
	}
}

const ioOrientBox& ioBombMissileWeapon4::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioBombMissileWeapon4::GetType() const
{
	return WT_BOMB_MISSILE4;
}

ActionStopType ioBombMissileWeapon4::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioBombMissileWeapon4::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioBombMissileWeapon4::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioBombMissileWeapon4::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pBombMesh && m_pBombMesh->GetSkeletonName().IsEmpty() && m_pBombMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
}

void ioBombMissileWeapon4::PlayWoundedSound( ioPlayEntity *pEntity )
{
	if( HasPeculiarity( WP_WOUND_NO_SOUND ) && !m_bPlayWoundSound )
		return;

	return;
}

void ioBombMissileWeapon4::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_BombState )
	{
	case BS_FLOATING:
		{
			DWORD dwLooseTime = 0;
			if( GetOwner() )
				dwLooseTime = GetOwner()->GetActionStopCurAdd();

			if( m_dwCreatedTime + m_dwLiveTime + dwLooseTime < dwCurTime )
				SetWeaponDead();
			else if( m_iCurExplosionCnt <= 0 )
				SetWeaponDead();
			else
				OnFloating( fTimePerSec, pStage );
		}
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

void ioBombMissileWeapon4::OnFloating( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_bFirst )
	{
		float fCurSpeed = GetMoveSpeed();
		float fMoveDist = fCurSpeed * fTimePerSec;

		CheckMoveKey();
		CheckMoveDir();
		CheckAttackKeyExplosion();

		GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	}

	m_bFirst = false;
	
	if( m_pBombMesh )
	{
		m_pBombMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioBombMissileWeapon4::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() || bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_WorldAttackBox = GetLocalAttackBox();
	m_WorldAttackBoxForMap = m_WorldAttackBox;

	float fMapCollisionRate = GetAttribute()->m_fMapCollisionBoxRate;
	m_WorldAttackBoxForMap.SetExtents( 0, m_WorldAttackBox.GetExtents(0)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 1, m_WorldAttackBox.GetExtents(1)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 2, m_WorldAttackBox.GetExtents(2)*fMapCollisionRate );

	if( !m_WorldAttackBox.IsNull() )
	{
		if( m_fOwnerChargeScaleRate > FLOAT1 )
		{
			D3DXVECTOR3 vScale( m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate, m_fOwnerChargeScaleRate );
			m_WorldAttackBox.Transform( GetPosition(), vScale, GetOrientation() );
			m_WorldAttackBoxForMap.Transform( GetPosition(), vScale, GetOrientation() );
		}
		else
		{
			m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
			m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
		}
	}
}

bool ioBombMissileWeapon4::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}

bool ioBombMissileWeapon4::IsFloating() const
{
	if( m_BombState == BS_FLOATING )
		return true;

	return false;
}

bool ioBombMissileWeapon4::IsExplosion() const
{
	return false;
}

bool ioBombMissileWeapon4::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	return false;
}

DWORD ioBombMissileWeapon4::GetCollisionEnableTime() const
{
	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

float ioBombMissileWeapon4::GetMoveSpeed()
{
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed * m_fOwnerChargeSpeedRate;
}

void ioBombMissileWeapon4::CheckMoveKey()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	bool bChange = false;
	bool bAttackKey = false;

	ioUserKeyInput::DirKeyInput eNewDirKey = pOwner->CheckCurDirKey();
	if( eNewDirKey != m_CurDirKey )
	{
		bChange = true;
		m_bTargetRot = false;
		m_CurDirKey = eNewDirKey;
	}

	if( !m_bDisableUpDown )
	{
		if( !m_bSetDownKey && m_bSetUpKey != pOwner->IsJumpKeyDown() )
		{
			bChange = true;
			m_bTargetRot = false;
			m_bSetUpKey = pOwner->IsJumpKeyDown();
		}

		if( !m_bSetUpKey && m_bSetDownKey != pOwner->IsDefenseKeyDown() )
		{
			bChange = true;
			m_bTargetRot = false;
			m_bSetDownKey = pOwner->IsDefenseKeyDown();
		}
	}

	if( bChange && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << SSC_CHANGE_MOVE;
		kPacket << (int)m_CurDirKey;
		kPacket << m_bSetUpKey;
		kPacket << m_bSetDownKey;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBombMissileWeapon4::CheckMoveDir()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( m_dwRotateSpeed == 0 )
		return;

	if( m_bTargetRot )
		return;

	if( m_CurDirKey == ioUserKeyInput::DKI_NONE && !m_bSetUpKey && !m_bSetDownKey )
	{
		return;
	}

	float fFrameGap = g_FrameTimer.GetCurFrameGap();
	float fRate = fFrameGap / m_dwRotateSpeed;
	float fRotateAngle = 360.0f * fRate;

	D3DXVECTOR3 vCurMoveDir = m_vMoveDir;
	D3DXVec3Normalize( &vCurMoveDir, &vCurMoveDir );

	// 목표에 도달체크 및 처리
	D3DXVECTOR3 vTargetDir = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	if( m_CurDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( m_CurDirKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		vTargetDir = qtRotate * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
	}

	if( m_bSetUpKey )
	{
		vTargetDir += ioMath::UNIT_Y;
	}
	else if( m_bSetDownKey )
	{
		vTargetDir += (-ioMath::UNIT_Y);
	}

	D3DXVec3Normalize( &vTargetDir, &vTargetDir );

	float fDotValue = (float)D3DXVec3Dot( &vCurMoveDir, &vTargetDir );
	float fGapAngle = RADtoDEG( acosf(fDotValue) );

	if( fDotValue >= FLOAT1 )
	{
		m_bTargetRot = true;

		D3DXQUATERNION qtTargetRot;
		ioMath::CalcDirectionQuaternion( qtTargetRot, -vTargetDir );

		m_vMoveDir = qtTargetRot * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
		D3DXVec3Normalize( &m_vMoveDir, &m_vMoveDir );

		SetOrientation( qtTargetRot );
		return;
	}

	D3DXVECTOR3 vAxis;
	if( fDotValue <= -FLOAT1 )
	{
		D3DXVECTOR3 vTmpAxis;

		if( vCurMoveDir != ioMath::UNIT_Y )
		{
			D3DXVec3Cross( &vTmpAxis, &ioMath::UNIT_Y, &vCurMoveDir );
			D3DXVec3Normalize( &vTmpAxis, &vTmpAxis );
		}
		else
		{
			D3DXVec3Cross( &vTmpAxis, &ioMath::UNIT_X, &vCurMoveDir );
			D3DXVec3Normalize( &vTmpAxis, &vTmpAxis );
		}

		D3DXVec3Cross( &vAxis, &vTmpAxis, &vCurMoveDir );
		D3DXVec3Normalize( &vAxis, &vAxis );
	}
	else
	{
		D3DXVec3Cross( &vAxis, &vCurMoveDir, &vTargetDir );
		D3DXVec3Normalize( &vAxis, &vAxis );
	}

	float fCurRotateAngle = fRotateAngle;
	if( fGapAngle < fRotateAngle )
		fCurRotateAngle = fGapAngle;

	D3DXQUATERNION qtNewRot;
	D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, DEGtoRAD(fCurRotateAngle) );

	D3DXVECTOR3 vNewDir = qtNewRot * vCurMoveDir;
	D3DXVec3Normalize( &vNewDir, &vNewDir );

	m_vMoveDir = vNewDir;

	GetParentSceneNode()->Rotate( vAxis, fCurRotateAngle );
}

void ioBombMissileWeapon4::CheckAttackKeyExplosion()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	if( !pOwner->IsNeedProcess() )
		return;

	bool bChange = false;
	if( pOwner->IsAttackKey() )
	{
		bChange = SetExplosionWeapon( false );
	}

	if( bChange && pOwner->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << SSC_ATTACK_KEY;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioBombMissileWeapon4::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
{
	int iState;
	rkPacket >> iState;

	switch( iState )
	{
	case SSC_CHANGE_MOVE:
		{
			int iDirKey;
			rkPacket >> iDirKey;

			m_CurDirKey = (ioUserKeyInput::DirKeyInput)iDirKey;

			rkPacket >> m_bSetUpKey;
			rkPacket >> m_bSetDownKey;

			m_bTargetRot = false;
		}
		break;
	case SSC_CHAR_COL:
		{
			SetExplosionWeapon( true );
		}
		break;
	case SSC_ATTACK_KEY:
		{
			SetExplosionWeapon( false );
		}
		break;
	}
}

void ioBombMissileWeapon4::SetLiveTime( DWORD dwTime )
{
	if( m_dwLiveTime == 0 )
		m_dwLiveTime = dwTime;

	return;
}

void ioBombMissileWeapon4::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	return;

	if( m_BombState != BS_FLOATING )
		return;

	ioBaseChar *pChar = ToBaseChar( pTarget );
	if( !pChar || !pChar->IsNeedProcess() )
		return;

	bool bCollision = false;
	if( pChar->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( GetTeam() != pTarget->GetTeam() )
		{
			bCollision = true;
		}
	}
		
	if( !bCollision )
		return;

	// char col explosion
	if( SetExplosionWeapon( true ) )
	{
		AddCollisionEntity( pTarget );

		if( pChar->IsNeedSendNetwork() )
		{
			SP2Packet kPacket( CUPK_WEAPON_SYNC );
			kPacket << GetOwnerName();
			kPacket << m_dwWeaponIdx;
			kPacket << SSC_CHAR_COL;
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

bool ioBombMissileWeapon4::SetExplosionWeapon( bool bCharCol )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	if( m_iCurExplosionCnt <= 0 )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	if( bCharCol )
	{
		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(), 
											  m_CharColExplosion,
											  GetPosition(),
											  m_vMoveDir,
											  dwCurTime + m_CharColExplosion.m_dwCallTime,
											  m_dwWeaponIdx );
	}
	else
	{
		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(), 
											  m_AttackKeyExplosion,
											  GetPosition(),
											  m_vMoveDir,
											  dwCurTime + m_AttackKeyExplosion.m_dwCallTime,
											  m_dwWeaponIdx );
	}

	m_iCurExplosionCnt--;
	m_iCurExplosionCnt = max( 0, m_iCurExplosionCnt );

	return true;
}



