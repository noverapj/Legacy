

#include "stdafx.h"

#include "ioThrowBombWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

ActionStopType ioThrowBombWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioThrowBombWeapon::m_DieActionStopType = AST_NONE;

ioThrowBombWeapon::ioThrowBombWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
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

	m_bArleadyCallWeapon = false;
}

ioThrowBombWeapon::~ioThrowBombWeapon()
{
	if( m_pFireMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioThrowWeapon::~ioThrowWeapon - UnReleased Resource" );
	}
}

bool ioThrowBombWeapon::InitResource()
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

void ioThrowBombWeapon::LoadProperty( ioINILoader &rkLoader )
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
	m_fOffSet_X		= rkLoader.LoadFloat_e("weapon_offset_x", 0.0f);

	LoadCallAttributeList( rkLoader );
}

void ioThrowBombWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	if( m_fDefaultAngle != 0.0f )
	{
		float fCurAngle = ioMath::ArrangeHead( m_fDefaultAngle );

		D3DXVECTOR3 vMoveDir;
		D3DXQUATERNION qtAngle;
		D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fCurAngle), 0.0f, 0.0f );

		D3DXQUATERNION qtRotate = ioMath::QUAT_IDENTITY;
		if( 0 < m_iDummyIndex )
		{
			ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( GetOwner(), m_iDummyIndex );
			if( pDummy )
				qtRotate = pDummy->GetWorldOrientation();
		}
		else if ( GetOwner() )
			qtRotate = GetOwner()->GetTargetRot();
		
		vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		vMoveDir = qtAngle * vMoveDir;
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );

		SetCurMoveDir( vMoveDir );
	}
	else
	{
		m_vOrigMoveDir = vDir;
		SetCurMoveDir( vDir );
	}
}

void ioThrowBombWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_bUseMeshLink && m_pFireMesh && m_pFireMesh->GetSkeletonName().IsEmpty() && m_pFireMesh->HasLinkedSystem() )
	{
		if ( !GetOwner() )
			return;
		SetOrientation( GetOwner()->GetTargetRot() );
		return;
	}

	SetPosition( vPos );

	if( m_fOffSet_Look != 0.0f || m_fOffSet_Y != 0.0f || m_fOffSet_X != 0.0f )
	{
		if( 0 < m_iDummyIndex )
		{
			ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( GetOwner(), m_iDummyIndex );
			if( pDummy )
			{				
				D3DXVECTOR3 vOffset = pDummy->GetWorldOrientation() * D3DXVECTOR3( m_fOffSet_X, m_fOffSet_Y, m_fOffSet_Look );
				D3DXVECTOR3 vTempPos = vPos + vOffset;
				SetPosition( vTempPos );
			}
		}
		else if ( GetOwner() )
		{
			D3DXVECTOR3 vTempPos = vPos;
			D3DXVECTOR3 vLook = GetOwner()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			D3DXVec3Normalize( &vLook, &vLook );
			D3DXVECTOR3 vRight;
			D3DXVec3Cross( &vRight, &ioMath::UNIT_Y, &vLook );

			vTempPos = vTempPos + vLook*m_fOffSet_Look + D3DXVECTOR3( 0, m_fOffSet_Y, 0 ) + vRight*m_fOffSet_X;
			SetPosition( vTempPos );
		}
	}
}

void ioThrowBombWeapon::SetCurMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;
	D3DXVec3Cross( &m_vRollAxis, &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ), &m_vMoveDir );
}

void ioThrowBombWeapon::SetMoveSpeed( float fSpeed )
{
	m_fMoveSpeed = fSpeed;
}

bool ioThrowBombWeapon::SetFireMesh( const ioHashString &szFileName )
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
	/*
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
	*/

	return true;
}

void ioThrowBombWeapon::SetWallCollision( const D3DXVECTOR3 &vFaceNrm )
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

void ioThrowBombWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
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
		{
			SetExplosionState( pStage );
		}
		else
		{
			OnTargetState( fTimePerSec, pStage );
			OnPingPongState( fTimePerSec, pStage );
		}
		break;
	case TS_EXPLOSION:
		OnExplosionState( fTimePerSec, pStage );
		break;
	}

	UpdateWorldAttackBox( fTimePerSec, false );
}

bool ioThrowBombWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
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

void ioThrowBombWeapon::OnNoneState( float fTimePerSec, ioPlayStage *pStage )
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

void ioThrowBombWeapon::OnMoveState( float fTimePerSec, ioPlayStage *pStage )
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

	/*
	if( m_bDisableRollOnMove )
	{
		D3DXVECTOR3 vPreDir;
		if( m_vPreDir != ioMath::VEC3_ZERO )
			vPreDir = m_vPreDir;
		else
			vPreDir = m_vMoveDir;

		D3DXVec3Normalize( &vPreDir, &vPreDir );
		D3DXVECTOR3 vCurDir = vMove;
		D3DXVec3Normalize( &vCurDir, &vCurDir );

		float fCos = D3DXVec3Dot( &vPreDir, &vCurDir );

		// 0도 일때
		if( fCos >= FLOAT1 )
		{
			// 처리 필요없음
		}
		else if( fCos <= -FLOAT1 ) // 180도 일때
		{
			D3DXVECTOR3 vAxis1, vAxis2;
			if( m_vMoveDir != ioMath::UNIT_Y )
			{
				D3DXVec3Cross( &vAxis1, &vCurDir, &ioMath::UNIT_Y );
				D3DXVec3Normalize( &vAxis1, &-vAxis1 );
			}
			else
			{
				D3DXVec3Cross( &vAxis1, &vCurDir, &ioMath::UNIT_X );
				D3DXVec3Normalize( &vAxis1, &-vAxis1 );
			}

			D3DXVec3Cross( &vAxis2, &vCurDir, &vAxis1 );
			D3DXVec3Normalize( &vAxis2, &-vAxis2 );

			GetParentSceneNode()->Rotate( vAxis2, FLOAT1 );
		}
		else // 그외
		{
			float fGapAngle = RADtoDEG( acos( fCos ) );

			D3DXVECTOR3 vAxis;
			D3DXVec3Cross( &vAxis, &vCurDir, &vPreDir );
			D3DXVec3Normalize( &vAxis, &-vAxis );

			GetParentSceneNode()->Rotate( vAxis, fGapAngle );
		}

	}
	*/

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

void ioThrowBombWeapon::OnTargetState( float fTimePerSec, ioPlayStage *pStage )
{
	if( !pStage )
		return;

	// Char Check	
	float fMinRange = ioMath::FLOAT_INFINITY;
	D3DXVECTOR3 vCurPos = GetPosition();

	const BaseCharList &rkCharList = pStage->GetBaseCharList();
	BaseCharList::const_iterator iter;

	m_szTargetName.Clear();
	if( !IsExperienceOwner() )
	{
		for( iter= rkCharList.begin() ; iter != rkCharList.end() ; ++iter )
		{
			ioBaseChar *pChar = *iter;

			if( pChar->GetTeam() == GetTeam() ) 
				continue;

			if( pChar->IsSystemState() )
				continue;
			
			if( !pChar->IsEnableTarget() )
				continue;

			D3DXVECTOR3 vDiff = vCurPos - pChar->GetWorldPosition();
			float fDiff = D3DXVec3LengthSq( &vDiff );
			if( fDiff < fMinRange )
			{
				m_szTargetName = pChar->GetCharName();	
				fMinRange = fDiff;
			}
		}
	}
}

void ioThrowBombWeapon::OnPingPongState( float fTimePerSec, ioPlayStage *pStage )
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

void ioThrowBombWeapon::OnExplosionState( float fTimePerSec, ioPlayStage *pStage )
{
	if( m_dwExplosionDuration == 0 )
	{
		CallAttribute();
		SetWeaponDead();
		return;
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( dwCurTime - m_dwExplosionStart > m_dwExplosionDuration )
	{
		CallAttribute();
		SetWeaponDead();
		return;
	}

	if( m_pFireMesh )
	{
		m_pFireMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioThrowBombWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireMesh )
	{
		g_EffectFactory.DestroyEffect( m_pFireMesh );
		m_pFireMesh = NULL;
	}
}

ioWeapon::WeaponType ioThrowBombWeapon::GetType() const
{
	return WT_THROWBOMB;
}

ActionStopType ioThrowBombWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioThrowBombWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

const ioOrientBox& ioThrowBombWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

void ioThrowBombWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

void ioThrowBombWeapon::SetExplosionState( ioPlayStage *pStage )
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
}

void ioThrowBombWeapon::SetReserveExplosion( ioPlayStage *pStage )
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

bool ioThrowBombWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
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
				ioPlayStage *pStage = ToBaseChar( pEntity )->GetCreator();
				if( pStage )
					SetExplosionState( pStage );

				return true;
			}
		case DT_ONE_TARGET:
			{
				return IsOneTargetCollision( pEntity, bMapCollision );
			}
		default:
			{
				if( m_State == TS_EXPLOSION )
					return true;
			}
		}
	}

	return false;
}

bool ioThrowBombWeapon::IsOneTargetCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_State == TS_EXPLOSION )
	{
		ioBaseChar* pChar = ToBaseChar( pEntity );
		if( pChar && pChar->IsNeedProcess() && !m_szTargetName.IsEmpty() && pChar->GetCharName() == m_szTargetName )
		{
			return true;
		}
	}
	return false;
}

void ioThrowBombWeapon::SetBombDrop()
{
	DWORD dwCurTime = FRAMEGETTIME();
	m_dwDuration = dwCurTime + m_dwDropDuration;

	m_fMoveSpeed = m_fDropMoveSpeed;
	m_fFloatingPower = m_fDropFloatingPower;

	m_bDropState = true;
}

void ioThrowBombWeapon::SetJumpThrow()
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(m_fJumpFloatingAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;
}

void ioThrowBombWeapon::RotateMoveDir( float fAngle )
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;
}

void ioThrowBombWeapon::SetBoomEffect( const ioHashString &rkName )
{
	m_BoomEffect = rkName;
}

bool ioThrowBombWeapon::IsExplosion() const
{
	if( m_State == TS_EXPLOSION )
		return true;

	return false;
}

bool ioThrowBombWeapon::IsCanMineExplosion()
{
	if( !ioWeapon::IsCanMineExplosion() )
		return false;

	if( !IsExplosion() )
		return false;

	return true;
}

void ioThrowBombWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
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

DWORD ioThrowBombWeapon::GetCollisionEnableTime() const
{
	if( m_State == TS_EXPLOSION )
		return m_dwCreatedTime;

	return m_dwCreatedTime + GetAttribute()->m_dwCollisionDelay;
}

void ioThrowBombWeapon::SetFloatPower( float fFloatPower )
{
	m_fFloatingPower = fFloatPower;
}

void ioThrowBombWeapon::CheckReserveExplosion( ioPlayStage *pStage )
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

void ioThrowBombWeapon::VisibleCheck( ioPlayStage *pStage )
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

void ioThrowBombWeapon::LoadCallAttributeList( ioINILoader &rkLoader )
{
	m_CallWeaponInfoList.clear();

	char szBuf[MAX_PATH], szKey[MAX_PATH];

	int iCnt = rkLoader.LoadInt_e( "call_attribute_cnt", 0 );
	if( iCnt == 0 )
		return;

	m_CallWeaponInfoList.reserve( iCnt );

	for( int i=0; i<iCnt; ++i )
	{
		WeaponFireInfo kCallAttribute;

		wsprintf_e( szKey, "call_attribute%d_index", i+1 );
		kCallAttribute.m_WeaponInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_resistance", i+1 );
		kCallAttribute.m_WeaponInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_time", i+1 );
		kCallAttribute.m_WeaponInfo.m_dwCallTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_ani", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kCallAttribute.m_WeaponInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "call_attribute%d_wound_duration", i+1 );
		kCallAttribute.m_WeaponInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_attribute%d_wound_loop_ani", i+1 );
		kCallAttribute.m_WeaponInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		wsprintf_e( szKey, "call_attribute%d_angle", i+1 );
		kCallAttribute.m_fWeaponFireAngle = rkLoader.LoadFloat( szKey, 0.0f );

		m_CallWeaponInfoList.push_back( kCallAttribute );
	}
}

void ioThrowBombWeapon::CallAttribute()
{
	if( m_bArleadyCallWeapon )
		return;

	if( m_CallWeaponInfoList.empty() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	DWORD dwCurGenerateTime = FRAMEGETTIME();

	WeaponFireInfoList::iterator iter;
	for( iter = m_CallWeaponInfoList.begin(); iter != m_CallWeaponInfoList.end(); ++iter )
	{
		WeaponFireInfo pInfo = (*iter);

		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(), 
			pInfo.m_WeaponInfo,
			GetPosition(),
			CalcRotateMoveDir( pInfo.m_fWeaponFireAngle ),
			dwCurGenerateTime + pInfo.m_WeaponInfo.m_dwCallTime,
			m_dwWeaponIdx );
	}

	m_bArleadyCallWeapon = true;
}

D3DXVECTOR3 ioThrowBombWeapon::CalcRotateMoveDir( float fAngle )
{
	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationYawPitchRoll( &qtRot, DEGtoRAD(fAngle), 0.0f, 0.0f );

	D3DXVECTOR3 vNewMoveDir = qtRot * m_vMoveDir;
	D3DXVec3Normalize( &vNewMoveDir, &vNewMoveDir );
	return vNewMoveDir;
}