

#include "stdafx.h"

#include "ioFloatThrowWeapon.h"

#include "ioPlayStage.h"
#include "ioBaseChar.h"

ActionStopType ioFloatThrowWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioFloatThrowWeapon::m_DieActionStopType = AST_NONE;

ioFloatThrowWeapon::ioFloatThrowWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_MISSILE_WEAPON );

	m_pThrowMesh = NULL;
	m_fFloatingPower = 0.0f;

	m_bArleadyCallWeapon = false;
	m_bCharCollision = false;	
}

ioFloatThrowWeapon::~ioFloatThrowWeapon()
{
	if( m_pThrowMesh )
	{
		LOG.PrintTimeAndLog( 0, "ioFloatThrowWeapon::~ioFloatThrowWeapon - UnReleased Resouce" );
	}
}

bool ioFloatThrowWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		m_fFloatingPower = GetAttribute()->m_fFloatPower;
		m_fMoveSpeed = GetAttribute()->m_fMoveSpeed;

		ioBaseChar *pOwner = GetOwner();
		if( pOwner && !m_szRedEffect.IsEmpty() && !m_szBlueEffect.IsEmpty() )
		{
			if( pOwner->GetTeam() == TEAM_RED )
				return SetThrowMesh( m_szRedEffect );
			else
				return SetThrowMesh( m_szBlueEffect );
		}
		else
			return SetThrowMesh( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioFloatThrowWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	m_fSpeedWeightRate = rkLoader.LoadFloat_e( "speed_weight_rate", FLOAT1 );
	m_fGravityRate = rkLoader.LoadFloat_e( "move_gravity_rate", FLOAT1 );

	m_vStartOffSet.x = rkLoader.LoadFloat_e( "start_offset_x", 0.0f );
	m_vStartOffSet.y = rkLoader.LoadFloat_e( "start_offset_y", 0.0f );
	m_vStartOffSet.z = rkLoader.LoadFloat_e( "start_offset_z", 0.0f );

	m_bCheckCharCollision = rkLoader.LoadBool_e( "enable_char_collision_call_attribute", false );
	// 새로 이 웨폰을 쓰시려면 true 해주세요. 찌그러짐이 없어집니다. by sms
	m_bDisableDistort = rkLoader.LoadBool_e( "disable_distort", false );

	char szBuf[MAX_PATH];
	rkLoader.LoadString_e( "weapon_effect_red", "", szBuf, MAX_PATH );
	m_szRedEffect = szBuf;
	rkLoader.LoadString_e( "weapon_effect_blue", "", szBuf, MAX_PATH );
	m_szBlueEffect = szBuf;

	LoadCallProperty( rkLoader );
}

void ioFloatThrowWeapon::LoadCallProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";
	char szKey[MAX_PATH] = "";

	m_bCallWeaponOnEnd = rkLoader.LoadBool_e( "call_weapon_on_weapon_dead", false );
	int iAttackTypeCnt = rkLoader.LoadInt_e( "call_weapon_type_count", 0 );

	m_CallWeaponInfoList.clear();

	if( 0 < iAttackTypeCnt )
		m_CallWeaponInfoList.reserve( iAttackTypeCnt );

	for( int i=0 ; i<iAttackTypeCnt ; i++ )
	{
		WeaponInfo kInfo;

		wsprintf_e( szKey, "call_weapon_type%d", i+1 );
		kInfo.m_iWeaponIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "call_weapon_type%d_resist", i+1 );
		kInfo.m_iResistanceIdx = rkLoader.LoadInt( szKey, NO_WEAPON_ATTRIBUTE );

		wsprintf_e( szKey, "call_weapon_type%d_time", i+1 );
		kInfo.m_dwCallTime = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_weapon_type%d_wounded", i+1 );
		rkLoader.LoadString( szKey, "", szBuf, MAX_PATH );
		kInfo.m_WoundedAnimation = szBuf;

		wsprintf_e( szKey, "call_weapon_type%d_wounded_time", i+1 );
		kInfo.m_dwWoundedDuration = rkLoader.LoadInt( szKey, 0 );

		wsprintf_e( szKey, "call_weapon_type%d_wounded_loop_ani", i+1 );
		kInfo.m_bWoundedLoopAni = rkLoader.LoadBool( szKey, false );

		m_CallWeaponInfoList.push_back( kInfo );
	}
}

bool ioFloatThrowWeapon::SetThrowMesh( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pThrowMesh = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pThrowMesh )	return false;

	GetParentSceneNode()->AttachObject( m_pThrowMesh );

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pThrowMesh->GetSkeletonName().IsEmpty() && m_pThrowMesh->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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

	m_pThrowMesh->UpdateBounds( true );

	AddEffectEvent( m_pThrowMesh );
	return true;
}
 
void ioFloatThrowWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pThrowMesh )
	{
		g_EffectFactory.DestroyEffect( m_pThrowMesh );
		m_pThrowMesh = NULL;
	}
}

void ioFloatThrowWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	m_vMoveDir = vDir;

	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );

	D3DXVec3Cross( &m_vRollAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );
	D3DXVec3Normalize( &m_vRollAxis, &m_vRollAxis );
}

void ioFloatThrowWeapon::SetMoveSpeed( float fSpeed )
{
	m_fMoveSpeed = fSpeed;
}

void ioFloatThrowWeapon::SetFloatPower( float fFloatPower )
{
	m_fFloatingPower = fFloatPower * m_fOwnerChargeFloatRate;
}

void ioFloatThrowWeapon::RotateMoveDir( float fAngle )
{
	D3DXVECTOR3 vAxis;
	D3DXVec3Cross( &vAxis, &m_vMoveDir, &D3DXVECTOR3( 0.0f, -FLOAT1, 0.0f ) );

	D3DXQUATERNION qtRot;
	D3DXQuaternionRotationAxis( &qtRot, &vAxis, DEGtoRAD(fAngle) );
	m_vMoveDir = qtRot * m_vMoveDir;

	ioMath::CalcDirectionQuaternion( qtRot, -m_vMoveDir );
	SetOrientation( qtRot );
}

void ioFloatThrowWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;
	if ( !m_bDisableDistort )
	{
		D3DXVECTOR3 vMoveH = m_vMoveDir * m_fSpeedWeightRate * ( GetMoveSpeed() * fTimePerSec );
		D3DXVECTOR3 vMoveV = D3DXVECTOR3( 0.0f, FLOAT1, 0.0f );
		vMoveV *= m_fFloatingPower * fTimePerSec;

		D3DXVECTOR3 vDir = vMoveH + vMoveV;
		D3DXVec3Normalize( &vDir, &vDir );

		//D3DXQUATERNION qtRot;
		//ioMath::CalcDirectionQuaternion( qtRot, -vDir );
		D3DXQUATERNION qtRot = Help::ConvertDirToQuaternion( vDir );
		SetOrientation( qtRot );

		D3DXVECTOR3 vMove = m_vMoveDir * ( GetMoveSpeed() * fTimePerSec );
		vMove.y += m_fFloatingPower * fTimePerSec;

		m_fFloatingPower -= (pStage->GetRoomGravity() * m_fGravityRate) * fTimePerSec;
		GetParentSceneNode()->Translate( vMove );
	}
	else
	{
		float fCurGravity = pStage->GetRoomGravity() * fTimePerSec;

		D3DXVECTOR3 vMove = m_vMoveDir * m_fSpeedWeightRate * ( GetMoveSpeed() * fTimePerSec );

		vMove.y += m_fFloatingPower * fTimePerSec;
		m_fFloatingPower -= fCurGravity * m_fGravityRate;
		GetParentSceneNode()->Translate( vMove );

		D3DXVECTOR3 vTargetDir = vMove;
		D3DXVec3Normalize( &vTargetDir, &vTargetDir );

		D3DXQUATERNION qtTargetRot = Help::ConvertDirToQuaternion( vTargetDir );

		SetOrientation( qtTargetRot );
	}	

	float fMapHeight = pStage->GetMapHeight( GetPosition().x,
											 GetPosition().z,
											 NULL,
											 false,
											 GetPosition().y+m_pThrowMesh->GetBoundBox().GetExtents(1) );

	if( GetPosition().y <= fMapHeight )
	{
		D3DXVECTOR3 vPos = GetPosition();
		CharPiercingType eType = GetAttribute()->m_PiercingType;

		switch( eType )
		{
		case CPT_NONE:
		case CPT_NORMAL:
			vPos.y = fMapHeight+1.0f;
			SetPosition( vPos );
			break;
		}

		if( fMapHeight == 0.0f )
		{
			pStage->CreateMapEffect( GetExplosionEffectName(), vPos, GetScale() );
			PlayWoundedSound( NULL );
			SetWeaponDead();
			return;
		}
	}

	UpdateWorldAttackBox( fTimePerSec, false );

	if( m_pThrowMesh )
	{
		m_pThrowMesh->Update( fTimePerSec * FLOAT1000 );
	}
}

D3DXVECTOR3 ioFloatThrowWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return m_vMoveDir;
}

const ioOrientBox& ioFloatThrowWeapon::GetLocalAttackBox() const
{
	if( m_pThrowMesh )
		return m_pThrowMesh->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioFloatThrowWeapon::GetType() const
{
	return WT_FLOAT_THROW;
}

ActionStopType ioFloatThrowWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioFloatThrowWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

float ioFloatThrowWeapon::GetMoveSpeed()
{
	return m_fMoveSpeed * m_fOwnerChargeSpeedRate;
}

bool ioFloatThrowWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	if( m_bNoWoundedWeapon )
	{
		if( ToBaseChar(pEntity) )
			return false;

		D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
		bCollision = pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint );

		CheckPiercing( bCollision );
		return bCollision;
	}

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	bCollision = pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint );

	CheckPiercing( bCollision );
	
	if( bCollision )
		m_bCharCollision = true;

	return bCollision;
}

bool ioFloatThrowWeapon::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	// 타겟이 캐릭터가 아니라면 충돌로 처리한다
	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )	return true;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( vColPoint );

	return false;	// 주인이 없는 무기라면 충돌안한걸로 하자
}

void ioFloatThrowWeapon::SetOwnerChargeFloatRate( float fRate )
{
	m_fOwnerChargeFloatRate = fRate;

	m_fFloatingPower = GetAttribute()->m_fFloatPower * m_fOwnerChargeFloatRate;
}

void ioFloatThrowWeapon::SetGravityRate( float fGravityRate )
{
	m_fGravityRate = fGravityRate;
}

void ioFloatThrowWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pThrowMesh && m_pThrowMesh->GetSkeletonName().IsEmpty() && m_pThrowMesh->HasLinkedSystem() )
	{
		SetPosition( vPos );
		return;
	}

	ioBaseChar *pOwner = GetOwner();
	D3DXVECTOR3 vNewPos = vPos;
	if( pOwner && m_vStartOffSet != ioMath::VEC3_ZERO )
	{
		D3DXQUATERNION qtRot = pOwner->GetTargetRot();
		D3DXVECTOR3 vOffSet = qtRot * m_vStartOffSet;

		vNewPos += vOffSet;
	}

	SetPosition( vNewPos );
}

bool ioFloatThrowWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		CheckPiercing( true );
		return true;
	}

	return false;
}

void ioFloatThrowWeapon::CallAttribute()
{	
	if( m_bArleadyCallWeapon ) return;
	if( !m_bCallWeaponOnEnd ) return;
	if( !m_bCheckCharCollision && m_bCharCollision ) return;
	if( m_CallWeaponInfoList.empty() ) return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return;
	
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwCurGenerateTime = dwCurTime;
	WeaponInfoList::iterator iter;
	
	for( iter = m_CallWeaponInfoList.begin(); iter != m_CallWeaponInfoList.end(); ++iter )
	{
		WeaponInfo pInfo = (*iter);
		g_WeaponMgr.SetReserveCallWeaponInfo( GetOwnerName(),
											  pInfo,
											  GetPosition(),
											  m_vMoveDir,
											  dwCurTime + pInfo.m_dwCallTime,
											  m_dwWeaponIdx );

	}
	m_bArleadyCallWeapon = true;
}

void ioFloatThrowWeapon::SetWeaponDead()
{
	StopLoopSound();
	m_bLive = false;

	CallAttribute();
}

const D3DXVECTOR3& ioFloatThrowWeapon::GetAttchBoxPosition()
{
	return m_WorldAttackBox.GetCenter();
}
