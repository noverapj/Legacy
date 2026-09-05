

#include "stdafx.h"
#include "ioBombMissileWeapon5.h"

ActionStopType ioBombMissileWeapon5::m_ActionStopType = AST_NONE;
ActionStopType ioBombMissileWeapon5::m_DieActionStopType = AST_NONE;

ioBombMissileWeapon5::ioBombMissileWeapon5( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
	: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_WOUND_NO_SOUND |	WP_DEF_NO_SOUND );

	m_pBombMesh = NULL;
	m_BombState = BS_FLOATING;

	m_bFirst = true;
	m_bResistance = false;
	m_fCurMoveSpeed = 0.0f;

	m_bTargetRot = false;
	m_CurDirKey = ioUserKeyInput::DKI_NONE;

	m_bSetUpKey = false;
	m_bSetDownKey = false;
	m_dwNextAttackKeyEnableTime = FRAMEGETTIME();
	m_dwNextCreateWeaponTime = FRAMEGETTIME();
}

ioBombMissileWeapon5::~ioBombMissileWeapon5()
{
	if( m_pBombMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioBombMissileWeapon5::~ioBombMissileWeapon5 - UnReleased Resouce" );
	}
}

bool ioBombMissileWeapon5::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		ioHashString szFireMesh = GetAttribute()->m_WeaponEffect;
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			TeamType eCurTeamType = pOwner->GetTeam();
			TeamType eCurSingleTeamType = pOwner->GetSingleConvertTeam();
			TeamType eTeamType = TEAM_NONE;
			switch( eCurTeamType )
			{
			case TEAM_BLUE:
				eTeamType = TEAM_BLUE;
				break;
			case TEAM_RED:
				eTeamType = TEAM_RED;
				break;
			default:
				if( eCurSingleTeamType == TEAM_BLUE )
					eTeamType = TEAM_BLUE;
				else
					eTeamType = TEAM_RED;
				break;
			}

			if( !m_szWeaponEffect_Blue.IsEmpty() && eTeamType == TEAM_BLUE )
				szFireMesh = m_szWeaponEffect_Blue;
			else if( !m_szWeaponEffect_Red.IsEmpty() && eTeamType == TEAM_RED )
				szFireMesh = m_szWeaponEffect_Red;
		}

		return SetBombMesh( szFireMesh );
	}

	return false;
}

void ioBombMissileWeapon5::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_dwLiveTime = (DWORD)rkLoader.LoadInt_e( "live_time", 0 );
	m_dwRotateSpeed = (DWORD)rkLoader.LoadInt_e( "rotate_speed", 0 );

	m_bDisableAutoMove = rkLoader.LoadBool_e( "disable_automove", false );
	m_bDisableUpDown = rkLoader.LoadBool_e( "disable_updown", false );
	m_bPlayWoundSound = rkLoader.LoadBool_e( "play_wound_sound", false );
	m_bDisableMapCollision = rkLoader.LoadBool_e( "disable_map_collision", false );

	// explosion cnt
	m_iMaxExplosionCnt = rkLoader.LoadInt_e( "max_explosion_cnt", 0 );
	m_iCurExplosionCnt = m_iMaxExplosionCnt;

	m_AttackKeyWeaponInfo.clear();
	int iAttackTypeCnt = rkLoader.LoadInt_e( "attack_key_explosion_cnt", 0 );
	if( 0 < iAttackTypeCnt )
		m_AttackKeyWeaponInfo.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "attack_key_explosion_index_%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_key_explosion_index_resistance_%d", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "attack_key_explosion_time_%d", i+1 );
		kInfo.m_dwCallTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_key_explosion_wound_ani_%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "attack_key_explosion_wound_duration_%d", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "attack_key_explosion_wound_loop_ani_%d", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_AttackKeyWeaponInfo.push_back( kInfo );
	}
	
	int iTicWeaponCnt = rkLoader.LoadInt_e( "tic_weapon_count", 0 );
	if( 0 < iTicWeaponCnt )
		m_TicWeaponInfo.reserve( iTicWeaponCnt );

	for( int i=0 ; i<iTicWeaponCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "tic_weapon_index_%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "tic_weapon_index_resistance_%d", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "tic_weapon_time_%d", i+1 );
		kInfo.m_dwCallTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "tic_weapon_wound_ani_%d", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "tic_weapon_wound_duration_%d", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "tic_weapon_wound_loop_ani_%d", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_TicWeaponInfo.push_back( kInfo );
	}

	m_dwAttackKeyEnableTicTime = (DWORD)rkLoader.LoadInt_e( "attackkey_enable_tic_time", 0 );
	m_dwCreateWeaponTicTime = (DWORD)rkLoader.LoadInt_e( "create_weapon_tic_time", 0 );

	rkLoader.LoadString_e( "weapon_effect_blue", "", szBuf, MAX_PATH );
	m_szWeaponEffect_Blue = szBuf;
	rkLoader.LoadString_e( "weapon_effect_red", "", szBuf, MAX_PATH );
	m_szWeaponEffect_Red = szBuf;
}

bool ioBombMissileWeapon5::SetBombMesh( const ioHashString &szFileName )
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

void ioBombMissileWeapon5::DestroyResource( ioPlayStage *pStage )
{
	if( m_pBombMesh )
	{
		g_EffectFactory.DestroyEffect( m_pBombMesh );
		m_pBombMesh = NULL;
	}
}

const ioOrientBox& ioBombMissileWeapon5::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

ioWeapon::WeaponType ioBombMissileWeapon5::GetType() const
{
	return WT_BOMB_MISSILE5;
}

ActionStopType ioBombMissileWeapon5::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioBombMissileWeapon5::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioBombMissileWeapon5::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioBombMissileWeapon5::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pBombMesh && m_pBombMesh->GetSkeletonName().IsEmpty() && m_pBombMesh->HasLinkedSystem() )
		return;

	SetPosition( vPos );
}

void ioBombMissileWeapon5::PlayWoundedSound( ioPlayEntity *pEntity )
{
	if( HasPeculiarity( WP_WOUND_NO_SOUND ) && !m_bPlayWoundSound )
		return;

	return;
}

void ioBombMissileWeapon5::Process( float fTimePerSec, ioPlayStage *pStage )
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

void ioBombMissileWeapon5::OnFloating( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_bFirst )
	{
		CheckMoveKey();
		CheckMoveDir();
		CheckAttackKeyExplosion();

		float fCurSpeed = GetMoveSpeed();
		float fMoveDist = fCurSpeed * fTimePerSec;
		GetParentSceneNode()->Translate( m_vMoveDir * fMoveDist );
	}

	m_bFirst = false;

	if( m_pBombMesh )
	{
		m_pBombMesh->Update( fTimePerSec * FLOAT1000 );
	}

	ProcessCreateTicWeapon( GetOwner(), pStage );
}

void ioBombMissileWeapon5::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

bool ioBombMissileWeapon5::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}

bool ioBombMissileWeapon5::IsFloating() const
{
	if( m_BombState == BS_FLOATING )
		return true;

	return false;
}

bool ioBombMissileWeapon5::IsExplosion() const
{
	return false;
}

bool ioBombMissileWeapon5::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	return false;
}

DWORD ioBombMissileWeapon5::GetCollisionEnableTime() const
{
	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

float ioBombMissileWeapon5::GetMoveSpeed()
{
	if( m_bDisableAutoMove && m_CurDirKey == ioUserKeyInput::DKI_NONE )
		return 0.f;	
	if( m_fCurMoveSpeed == 0.0f )
		m_fCurMoveSpeed = GetAttribute()->m_fMoveSpeed;

	return m_fCurMoveSpeed * m_fOwnerChargeSpeedRate;
}

void ioBombMissileWeapon5::CheckMoveKey()
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

void ioBombMissileWeapon5::CheckMoveDir()
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

	if( fDotValue >= FLOAT1 || m_bDisableAutoMove )
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

void ioBombMissileWeapon5::CheckAttackKeyExplosion()
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

void ioBombMissileWeapon5::ApplyWeaponSync( SP2Packet &rkPacket, ioPlayStage *pStage )
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
	case SSC_CREATE_TICWEAPON:
		{
			CreateTicWeapon( GetOwner(), pStage );
		}
		break;
	}
}

void ioBombMissileWeapon5::SetLiveTime( DWORD dwTime )
{
	if( m_dwLiveTime == 0 )
		m_dwLiveTime = dwTime;

	return;
}

void ioBombMissileWeapon5::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
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

bool ioBombMissileWeapon5::SetExplosionWeapon( bool bCharCol )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return false;

	if( m_iCurExplosionCnt <= 0 )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	if ( m_dwAttackKeyEnableTicTime > 0 )
	{
		if ( m_dwNextAttackKeyEnableTime + m_dwAttackKeyEnableTicTime > dwCurTime)
			return false;
		else
			m_dwNextAttackKeyEnableTime = dwCurTime;
	}
	for( int i = 0; i < (int)m_AttackKeyWeaponInfo.size(); i++ )
	{
		const WeaponInfo& info = m_AttackKeyWeaponInfo[i];
		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(), 
			info,
			GetPosition(),
			m_vMoveDir,
			dwCurTime + info.m_dwCallTime,
			m_dwWeaponIdx );
	}

	m_iCurExplosionCnt--;
	m_iCurExplosionCnt = max( 0, m_iCurExplosionCnt );

	return true;
}

void ioBombMissileWeapon5::ProcessCreateTicWeapon( ioBaseChar* pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pChar->IsNeedProcess() || !pStage || m_TicWeaponInfo.empty() )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if ( m_dwCreateWeaponTicTime > 0 && m_dwNextCreateWeaponTime + m_dwCreateWeaponTicTime < dwCurTime )
	{
		m_dwNextCreateWeaponTime = dwCurTime;
		CreateTicWeapon( pChar, pStage );
	}
}

void ioBombMissileWeapon5::CreateTicWeapon( ioBaseChar* pChar, ioPlayStage *pStage )
{
	if ( !pChar || !pStage || m_TicWeaponInfo.empty())
		return;
	DWORD dwCurTime = FRAMEGETTIME();
	for( int i = 0; i < (int)m_TicWeaponInfo.size(); i++ )
	{
		const WeaponInfo& info = m_TicWeaponInfo[i];
		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(), 
			info,
			GetPosition(),
			m_vMoveDir,
			dwCurTime + info.m_dwCallTime,
			m_dwWeaponIdx );
	}

	if( pChar->IsNeedSendNetwork() )
	{
		SP2Packet kPacket( CUPK_WEAPON_SYNC );
		kPacket << GetOwnerName();
		kPacket << m_dwWeaponIdx;
		kPacket << SSC_CREATE_TICWEAPON;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}