

#include "stdafx.h"

#include "ioThrowBomb2Weapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "WeaponDefine.h"

ActionStopType ioThrowBomb2Weapon::m_ActionStopType = AST_NONE;
ActionStopType ioThrowBomb2Weapon::m_DieActionStopType = AST_NONE;

ioThrowBomb2Weapon::ioThrowBomb2Weapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ATTACK_OWNER |
					WP_MINE_EXPLOSION |
					WP_WOUND_NO_SOUND |
					WP_DEF_NO_SOUND );

	m_pFireMesh = NULL;

	m_dwDuration = 0;
	m_dwExplosionStart = 0;
	m_dwResreveExplosionStartTime = 0;
	m_bReserveExplosion = false;
	m_fMoveSpeed = 0.0f;

	m_State = TS_MOVE;
	m_bObjectCollision = false;
	m_bResistance = false;
	m_bDropState = false;

	m_iCurPingPongCnt = 0;

	m_vMoveDir	= ioMath::VEC3_ZERO;
	m_vPreDir	= ioMath::VEC3_ZERO;
}

ioThrowBomb2Weapon::~ioThrowBomb2Weapon()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioThrowWeapon::~ioThrowWeapon - UnReleased Resource" );
	}
}

bool ioThrowBomb2Weapon::InitResource()
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
			{
				szFireMesh = m_szWeaponEffect_Blue;
			}
			else if( !m_szWeaponEffect_Red.IsEmpty() && eTeamType == TEAM_RED )
			{
				szFireMesh = m_szWeaponEffect_Red;
			}
		}
		return SetFireMesh( szFireMesh );
	}

	return false;
}

void ioThrowBomb2Weapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	DWORD dwCurTime = FRAMEGETTIME();

	char szBuf[MAX_PATH] = "";

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	rkLoader.LoadString_e( "explosion_delay_effect", "", szBuf, MAX_PATH );
	m_ExplosionDelayEffect = szBuf;

	m_bDirectlyBoom = rkLoader.LoadBool_e( "directly_boom", false );
	m_iDirectlyBoomToChar = rkLoader.LoadInt_e( "directly_boom_to_char", 0 );

	m_fExplosionRange = rkLoader.LoadFloat_e( "explosion_range", 0.0f );

	m_dwThrowDuration = rkLoader.LoadInt_e( "throw_explosion_duration", 5000 );
	m_dwDropDuration = rkLoader.LoadInt_e( "drop_explosion_duration", 5000 );
	m_dwExplosionDuration = rkLoader.LoadInt_e( "explosion_duration", 0 );
	m_dwResreveExplosionTime = (DWORD)rkLoader.LoadInt_e( "reserve_explosion_time", 0 );
	m_dwAutoReserveExplosiontTime = (DWORD)rkLoader.LoadInt_e( "auto_reserve_explosion_time", 0 );

	m_dwDuration = dwCurTime + m_dwThrowDuration;

	m_fJumpFloatingAngle = rkLoader.LoadFloat_e( "jump_floating_angle", 20.0f );
	
	m_fMoveSpeed = GetAttribute()->m_fMoveSpeed;
	m_fDropMoveSpeed = rkLoader.LoadFloat_e( "drop_move_speed", 0.0f );

	m_fFloatingPower = GetAttribute()->m_fFloatPower;
	m_fDropFloatingPower = rkLoader.LoadFloat_e( "drop_floating_power", 0.0f );

	m_fPingPongPower = rkLoader.LoadFloat_e( "ping_pong_power", 400.0f );
	m_fCurPingPongPower = m_fPingPongPower;
	m_iMaxPingPongCnt = rkLoader.LoadFloat_e( "max_ping_pong_cnt", 0 );

	m_fDefaultAngle = rkLoader.LoadFloat_e( "default_move_angle", 0.0f );

	m_bDisableRollOnMove = rkLoader.LoadBool_e( "disable_roll_on_move", false );

	m_bUseMeshLink = rkLoader.LoadBool_e( "use_weapon_mesh_link", false );

	m_bVisbleSameTeam = rkLoader.LoadBool_e( "visible_same_team", false );

	rkLoader.LoadString_e( "reserve_sound", "", szBuf, MAX_PATH );
	m_szReserveSound = szBuf;

	rkLoader.LoadString_e( "weapon_effect_blue", "", szBuf, MAX_PATH );
	m_szWeaponEffect_Blue = szBuf;
	rkLoader.LoadString_e( "weapon_effect_red", "", szBuf, MAX_PATH );
	m_szWeaponEffect_Red = szBuf;

	m_fOffSet_Look	= rkLoader.LoadFloat_e("weapon_offset_look", 0.0f);
	m_fOffSet_Y		= rkLoader.LoadFloat_e("weapon_offset_y", 0.0f);

	m_TargetWoundType = (TargetWoundType)rkLoader.LoadInt_e( "target_wound_type", TWT_ALL );
	m_fAimRange = rkLoader.LoadFloat_e( "aim_range", 0.0f );

	m_CallWeapon.m_iWeaponIdx = rkLoader.LoadInt_e( "call_weapon_type", NO_WEAPON_ATTRIBUTE );
	m_CallWeapon.m_iResistanceIdx = rkLoader.LoadInt_e( "call_weapon_type_resist", NO_WEAPON_ATTRIBUTE );
	rkLoader.LoadString_e( "call_weapon_type_wounded", "", szBuf, MAX_PATH );
	m_CallWeapon.m_WoundedAnimation = szBuf;
	m_CallWeapon.m_dwWoundedDuration = rkLoader.LoadInt_e( "call_weapon_type_wounded_time", 0 );
	m_CallWeapon.m_bWoundedLoopAni = rkLoader.LoadBool_e( "call_weapon_type_wounded_loop_ani", false );

	m_bDisableCheckTarget = rkLoader.LoadBool_e( "disable_check_target", false );
}

void ioThrowBomb2Weapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	if( m_vMoveDir == ioMath::VEC3_ZERO )
	{
		m_vOrigMoveDir = m_vMoveDir = vDir;
		D3DXVec3Cross( &m_vRollAxis, &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ), &m_vMoveDir );		
	}
}

void ioThrowBomb2Weapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_bUseMeshLink && m_pFireMesh && m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->HasLinkedSystem() )
	{
		SetOrientation( GetOwner()->GetTargetRot() );
		return;
	}

	SetPosition( vPos );

	if( m_fOffSet_Look > 0.0f || m_fOffSet_Y > 0.0f && GetOwner() )
	{
		D3DXVECTOR3 vTempPos = vPos;
		D3DXVECTOR3 vLook = GetOwner()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vLook, &vLook );

		vTempPos = vTempPos + vLook*m_fOffSet_Look + D3DXVECTOR3( 0, m_fOffSet_Y, 0 );
		SetPosition( vTempPos );
	}
}

void ioThrowBomb2Weapon::SetCurMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	D3DXVec3Cross( &m_vRollAxis, &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ), &m_vMoveDir );
}

void ioThrowBomb2Weapon::SetMoveSpeed( float fSpeed )
{
	m_fMoveSpeed = fSpeed;
}

bool ioThrowBomb2Weapon::SetFireMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pFireMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pFireMesh );
	GetParentSceneNode()->SetScale( FLOAT1, FLOAT1, FLOAT1 );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_pFireMesh->UpdateBounds( true );
	m_LocalAttackBox = m_pFireMesh->GetCollisionBox();

	m_fExtentsOffSet = m_LocalAttackBox.GetExtents( 1 );

	AddEffectEvent( m_pFireMesh );

	//
	if( m_fDefaultAngle != 0.0f )
	{
		float fCurAngle = ioMath::ArrangeHead( m_fDefaultAngle );

		D3DXVECTOR3 vMoveDir;
		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

		D3DXQUATERNION qtRotate = pOwner->GetTargetRot();
		vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vMoveDir = qtAngle * vMoveDir;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );

		m_vMoveDir = vMoveDir;
		D3DXVec3Cross( &m_vRollAxis, &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ), &m_vMoveDir );
	}

	return true;
}

void ioThrowBomb2Weapon::SetWallCollision( const D3DXVECTOR3 &vFaceNrm )
{
	if( m_State != TS_MOVE && m_State != TS_PINGPONG ) return;
	if( m_iMaxPingPongCnt > 0 && m_iCurPingPongCnt >= m_iMaxPingPongCnt )
		return;

	D3DXVECTOR3 vMoveDir = m_vMoveDir;
	vMoveDir.y = 0.0f;
	D3DXVec3Normalize( &vMoveDir, &vMoveDir );

	float fDotValue = D3DXVec3Dot( &vMoveDir, &vFaceNrm );
	if( fDotValue > 0.1f ) return;

	D3DXVECTOR3 vReflect = vMoveDir - 2.0f * fDotValue * vFaceNrm;
	vReflect.y = 0.0f;
	D3DXVec3Normalize( &vReflect, &vReflect );

	SetCurMoveDir( vReflect );

	m_State = TS_PINGPONG;

	m_iCurPingPongCnt++;
	
	if( m_iMaxPingPongCnt > 0 )
		m_fCurPingPongPower = m_fPingPongPower / m_iCurPingPongCnt;
}

void ioThrowBomb2Weapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_AreaWeaponName.IsEmpty() &&
		m_dwCreatedTime+m_dwAttachAreaWeaponTime < FRAMEGETTIME() )
	{
		CreateAttachAreaWeapon();
	}

	VisibleCheck( pStage );

	switch( m_State )
	{
	case TS_NONE:
		OnNoneState( fTimePerSec, pStage );
		break;
	case TS_MOVE:
		OnMoveState( fTimePerSec, pStage );
		break;
	case TS_PINGPONG:
		if( m_bDirectlyBoom && !m_bDropState )
			SetExplosionState( pStage );
		else
			OnPingPongState( fTimePerSec, pStage );
		break;
	case TS_EXPLOSION:
		OnExplosionState( fTimePerSec, pStage );
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

bool ioThrowBomb2Weapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_State != TS_MOVE && m_State != TS_PINGPONG )
		return true;

	int iFaceIndex = 0;
	if( !pShape->FindIntersection( pShapeWorldMat, GetWorldAttackBox(true), &iFaceIndex ) )
		return false;

	D3DXVECTOR3 vNormal;
	pShape->GetTriNormalInModelSpace( iFaceIndex, vNormal );

	SetWallCollision( vNormal );
	return true;
}

void ioThrowBomb2Weapon::OnNoneState( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_dwDuration > 0 && m_dwDuration < FRAMEGETTIME() )
	{
		SetExplosionState( pStage );
		return;
	}

	CheckReserveExplosion( pStage );

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioThrowBomb2Weapon::OnMoveState( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_dwDuration > 0 && m_dwDuration < FRAMEGETTIME() )
	{
		SetExplosionState( pStage );
		return;
	}

	if( !m_bDisableRollOnMove )
	{
		float fRotateAngle = 900.0f * fTimePerSec;
		GetParentSceneNode()->Rotate( m_vRollAxis, fRotateAngle );
	}

	//D3DXVECTOR3 vCurMove( 0.0, -FLOAT1, 0.0f );
	float fCurGravity = pStage->GetRoomGravity() * fTimePerSec;

	D3DXVECTOR3 vMove = m_vMoveDir * ( m_fMoveSpeed * fTimePerSec );

	//vCurMove *= fCurGravity;
	//vCurMove += vMove;
	//D3DXVec3Normalize( &vCurMove, &vCurMove );

	vMove.y += m_fFloatingPower * fTimePerSec;
	m_fFloatingPower -= fCurGravity;

	GetParentSceneNode()->Translate( vMove );

	if( m_bDisableRollOnMove )
	{
		D3DXVECTOR3 vTargetDir = vMove;
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );

		D3DXQUATERNION qtTargetRot = Help::ConvertDirToQuaternion( vTargetDir );

		SetOrientation( qtTargetRot );
	}

	m_vPreDir = vMove;

	D3DXVECTOR3 vPos = GetPosition();

	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+m_pFireMesh->GetBoundBox().GetExtents(1) );
	if( vPos.y < fMapHeight )
	{
		vPos.y = fMapHeight;
		vPos.y += m_fExtentsOffSet;

		SetPosition( vPos );

		if( fMapHeight == 0.0f )
			SetExplosionState( pStage );
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}

	CheckReserveExplosion( pStage );

	UpdateAttachAreaWeapon( pStage );
}

void ioThrowBomb2Weapon::OnPingPongState( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_dwDuration > 0 && m_dwDuration < FRAMEGETTIME() )
	{
		SetExplosionState( pStage );
		return;
	}

	float fRotateAngle = 900.0f * fTimePerSec;
	//GetParentSceneNode()->PitchX( fRotateAngle, ioNode::TS_LOCAL );
	GetParentSceneNode()->Rotate( m_vRollAxis, fRotateAngle );

	D3DXVECTOR3 vMove = m_vMoveDir * ( m_fMoveSpeed * fTimePerSec * 0.3f );
	float fCurGravity = pStage->GetRoomGravity() * fTimePerSec;

	vMove.y += m_fCurPingPongPower * fTimePerSec;
	m_fCurPingPongPower -= fCurGravity;
	GetParentSceneNode()->Translate( vMove );

	D3DXVECTOR3 vPos = GetPosition();

	float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+m_pFireMesh->GetBoundBox().GetExtents(1) );
	if( vPos.y < fMapHeight )
	{
		vPos.y = fMapHeight;
		vPos.y += m_fExtentsOffSet;

		D3DXQUATERNION qtRot;
		
		SetPosition( vPos );
		if( m_bDisableRollOnMove )
		{
			ioBaseChar *pOwner = GetOwner();
			if( pOwner )
				ioMath::CalcDirectionQuaternion( qtRot, -m_vOrigMoveDir );
			else
				qtRot = D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 );
		}
		else
		{
			qtRot = D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 );
		}
		
		SetOrientation( qtRot );
		m_fFloatingPower = 0.0f;

		if( fMapHeight > 0.0f )
		{
			m_State = TS_NONE;
		}
		else
			SetExplosionState( pStage );
	}
	else
	{
		SetPosition( vPos );
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}

	CheckReserveExplosion( pStage );

	UpdateAttachAreaWeapon( pStage );
}

void ioThrowBomb2Weapon::OnExplosionState( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_dwExplosionDuration == 0 )
	{
		SetWeaponDead();
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( dwCurTime - m_dwExplosionStart > m_dwExplosionDuration )
	{
		SetWeaponDead();
		return;
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioThrowBomb2Weapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}
}

ioWeapon::WeaponType ioThrowBomb2Weapon::GetType() const
{
	return WT_THROWBOMB2;
}

ActionStopType ioThrowBomb2Weapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioThrowBomb2Weapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

const ioOrientBox& ioThrowBomb2Weapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

void ioThrowBomb2Weapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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
		m_WorldAttackBox.Transform( GetPosition(),
									ioMath::UNIT_ALL,
									GetParentSceneNode()->GetDerivedOrientation() );

		m_WorldAttackBoxForMap.Transform( GetPosition(),
										  ioMath::UNIT_ALL,
										  GetParentSceneNode()->GetDerivedOrientation() );
	}
}

void ioThrowBomb2Weapon::SetExplosionState( ioPlayStage *pStage )
{
	if( m_State == TS_EXPLOSION )
		return;

	DestroyAttachAreaWeapon();

	m_State = TS_EXPLOSION;
	m_dwExplosionStart = FRAMEGETTIME();

	float fHalfRange = m_fExplosionRange;
	m_LocalAttackBox.SetExtents( 0, fHalfRange );
	m_LocalAttackBox.SetExtents( 1, fHalfRange );
	m_LocalAttackBox.SetExtents( 2, fHalfRange );
	UpdateWorldAttackBox( 0.0f, false );

	if( !m_BoomEffect.IsEmpty() )
	{
		D3DXVECTOR3 vPos = GetPosition();

		float fMapHeight = pStage->GetMapHeight( vPos.x, vPos.z, NULL, false, vPos.y+m_pFireMesh->GetBoundBox().GetExtents(1) );
		if( vPos.y < fMapHeight )
		{
			vPos.y = fMapHeight;
		}

		vPos.y += FLOAT1;

		pStage->CreateMapEffect( m_BoomEffect, vPos, GetScale() );
		ioWeapon::PlayExplosionSound();
	}

	if ( !m_bDisableCheckTarget )
	{
		CheckTarget();
	}
	else
	{
		GenerateNoTargetCallWeapon();
	}
}

void ioThrowBomb2Weapon::SetReserveExplosion( ioPlayStage *pStage )
{
	if( m_State == TS_EXPLOSION )
		return;

	if( m_bReserveExplosion )
		return;

	m_dwResreveExplosionStartTime = FRAMEGETTIME();
	m_bReserveExplosion = true;

	if( pStage )
	{
		D3DXVECTOR3 vPos = GetPosition();
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		pStage->CreateMapEffect( m_ExplosionDelayEffect, vPos, vScale );
	}

	g_SoundMgr.PlaySound( m_szReserveSound, GetParentSceneNode() );
}

bool ioThrowBomb2Weapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( !pEntity )
		return false;

	if( m_fExplosionRange <= 0.0f )
		return false;

	float fTargetDist = -FLOAT1;
	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();
	
	if( !pEntity->CheckSphereDistance( vCenter, m_fExplosionRange, fTargetDist ) )
		return false;

	if( COMPARE( fTargetDist, 0.0f, m_fExplosionRange ) )
	{
		switch( m_iDirectlyBoomToChar )
		{
		case DT_BOOM:
			if( ToBaseChar(pEntity) )
			{
				ioPlayStage *pStage = ToBaseChar(pEntity)->GetCreator();
				if(pStage)
					SetExplosionState(pStage);
			}
			return true;
		default:
			{
				if( m_State == TS_EXPLOSION )
					return true;
			}
		}
	}

	return false;
}

void ioThrowBomb2Weapon::SetBombDrop()
{
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDuration = dwCurTime + m_dwDropDuration;

	m_fMoveSpeed = m_fDropMoveSpeed;
	m_fFloatingPower = m_fDropFloatingPower;

	m_bDropState = true;
}

void ioThrowBomb2Weapon::SetJumpThrow()
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(m_fJumpFloatingAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;
}

void ioThrowBomb2Weapon::RotateMoveDir( float fAngle )
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;
}

void ioThrowBomb2Weapon::SetBoomEffect( const ioHashString &rkName )
{
	m_BoomEffect = rkName;
}

bool ioThrowBomb2Weapon::IsExplosion() const
{
	if( m_State == TS_EXPLOSION )
		return true;

	return false;
}

bool ioThrowBomb2Weapon::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}

void ioThrowBomb2Weapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_State != TS_MOVE && m_State != TS_PINGPONG )
		return;

	ioBaseChar *pChar = ToBaseChar( pTarget );
	if( pChar && pChar->GetCharName() == GetOwnerName() )
		return;

	if( pChar && pChar->IsApplyDownState( false ) )
		return;

	if( m_State == TS_PINGPONG && m_bObjectCollision )
		return;

	const ioOrientBox &kBombBox = GetWorldAttackBox();
	const ioOrientBox &kWoundedBox = pTarget->GetWorldCollisionBox();

	if( !pTarget->IsWoundedCollision( kBombBox ) )
		return;

	if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) || ToDummyChar(pTarget) || ToWallZone(pTarget) )
	{
		m_bObjectCollision = true;

		D3DXVECTOR3 vDiff = kBombBox.GetCenter() - kWoundedBox.GetCenter();
		vDiff.y = 0.0f;
		D3DXVec3Normalize( &vDiff, &vDiff );

		SetWallCollision( vDiff );
		return;
	}
	else if( ToBaseChar(pTarget) && GetPiercingType() == CPT_NONE )
	{
		if( ToBaseChar(pTarget)->IsNeedProcess() )
		{
			D3DXVECTOR3 vDiff = kBombBox.GetCenter() - kWoundedBox.GetCenter();
			vDiff.y = 0.0f;
			D3DXVec3Normalize( &vDiff, &vDiff );

			SetWallCollision( vDiff );

			if( ToBaseChar(pTarget)->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_THROWBOMB_BOUND );
				kPacket << GetOwnerName();
				kPacket << m_dwWeaponIdx;
				kPacket << GetPosition();
				kPacket << vDiff;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}
		return;
	}

	if( pTarget->GetTeam() == GetTeam() )
	{
		if( !pChar || !pChar->IsEnableTeamAttack() )
			return;
	}

	if( m_fExplosionRange <= 0.0f )
		return;

	CheckCurAttribute( pTarget );

	ApplyWeaponExp eExplosionType = pTarget->ApplyWeapon( this );
	pStage->AddWeaponExplosionEffect( pTarget, this, eExplosionType );

	AddCollisionEntity( pTarget );
}

DWORD ioThrowBomb2Weapon::GetCollisionEnableTime() const
{
	if( m_State == TS_EXPLOSION )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

void ioThrowBomb2Weapon::ApplyExtraReserveWeaponByWeaponInfo( ioWeapon* pWeapon )
{
	if( m_TargetNameList.empty() )
		return;

	ioTraceWeaponType2 *pTrace2 = ToTraceWeaponType2( pWeapon );
	if( pTrace2 )
		pTrace2->SetTraceTarget( m_TargetNameList[0] );

	ioHashStringVec::iterator iter = m_TargetNameList.begin();
	m_TargetNameList.erase( iter );
}

void ioThrowBomb2Weapon::SetFloatPower( float fFloatPower )
{
	m_fFloatingPower = fFloatPower;
}

void ioThrowBomb2Weapon::CheckReserveExplosion( ioPlayStage *pStage )
{
	if( !m_bReserveExplosion )
	{
		if( m_dwAutoReserveExplosiontTime > 0 && m_dwCreatedTime + m_dwAutoReserveExplosiontTime < FRAMEGETTIME() )
			SetReserveExplosion( pStage );

		return;
	}

	if( m_State == TS_EXPLOSION )
		return;

	if( m_dwResreveExplosionTime > 0 && FRAMEGETTIME() - m_dwResreveExplosionStartTime > m_dwResreveExplosionTime )
		SetExplosionState( pStage );	
}

void ioThrowBomb2Weapon::VisibleCheck( ioPlayStage *pStage )
{
	if( !pStage )
		return;

	if( !m_pFireMesh )	return;
	if( !m_bVisbleSameTeam )
		return;

	ioBaseChar *pTarget = pStage->GetCameraTargetChar();
	if( pTarget )
	{
		if( GetTeam() == pTarget->GetTeam() )
		{
			m_pFireMesh->SetVisible( true );
		}
		else
		{
			if( m_bReserveExplosion || m_State == TS_EXPLOSION )
			{
				if( !m_pFireMesh->IsVisible() )
					m_pFireMesh->SetVisible( true );
			}
			else
			{
				if( m_pFireMesh->IsVisible() )
					m_pFireMesh->SetVisible( false );
			}
		}
	}
	else
	{
		m_pFireMesh->SetVisible( true );
	}
}

void ioThrowBomb2Weapon::CheckTarget()
{
	ioPlayStage *pStage = g_WeaponMgr.GetCreator();
	if( !pStage )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	m_TargetNameList.clear();

	float fAImRangeSq = m_fAimRange*m_fAimRange;

	const BaseCharList &rkTargetList = pStage->GetBaseCharList();
	int iSize = rkTargetList.size();

	BaseCharList::const_iterator iter;
	for( iter = rkTargetList.begin(); iter != rkTargetList.end(); ++iter )
	{
		ioBaseChar *pTarget = (*iter);

		if( pOwner->GetCharName() == pTarget->GetCharName() )
			continue;

		if( pOwner->GetTeam() == pTarget->GetTeam() )
			continue;

		if( pTarget->IsSystemState() )
			continue;
			
		if( !pTarget->IsEnableTarget() )
			continue;

		if( !pTarget->CheckTargetWoundType( m_TargetWoundType ) )
			continue;

		D3DXVECTOR3 vDiff = GetPosition() - pTarget->GetWorldPosition();
		float fDistSq = D3DXVec3LengthSq( &vDiff );

		if( fDistSq < fAImRangeSq )	// Enemy
		{
			m_TargetNameList.push_back( pTarget->GetCharName() );
		}
	}

	GenerateCallWeapon();
}

void ioThrowBomb2Weapon::GenerateCallWeapon()
{
	ioPlayStage *pStage = g_WeaponMgr.GetCreator();
	if( !pStage )
		return;

	if( m_TargetNameList.empty() )
		return;

	int iCnt = m_TargetNameList.size();
	for( int i=0; i<iCnt; ++i )
	{
		ioBaseChar *pTarget = pStage->GetBaseChar( m_TargetNameList[i] );
		if( !pTarget )
			continue;
		
		D3DXVECTOR3 vCurPos = GetPosition();
		D3DXVECTOR3 vTargetPos = pTarget->GetMidPositionByRate();
		D3DXVECTOR3 vDir = vTargetPos - vCurPos;
		D3DXVec3Normalize( &vDir, &vDir );

		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(),
											  m_CallWeapon,
											  vCurPos,
											  vDir,
											  FRAMEGETTIME() + m_CallWeapon.m_dwCallTime,
											  m_dwWeaponIdx );
	}
}

void ioThrowBomb2Weapon::GenerateNoTargetCallWeapon()
{
	ioPlayStage *pStage = g_WeaponMgr.GetCreator();
	if( !pStage )
		return;

	D3DXVECTOR3 vCurPos = GetPosition();
	
	g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(),
										m_CallWeapon,
										vCurPos,
										m_vMoveDir,
										FRAMEGETTIME() + m_CallWeapon.m_dwCallTime,
										m_dwWeaponIdx );
}