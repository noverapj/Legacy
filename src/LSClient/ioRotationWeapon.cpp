

#include "stdafx.h"

#include "ioRotationWeapon.h"

#include "ioBaseChar.h"

ActionStopType ioRotationWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioRotationWeapon::m_DieActionStopType = AST_NONE;

ioRotationWeapon::ioRotationWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_ONE_TIME_USE | WP_MISSILE_WEAPON | WP_DIVIDE_ACTION );

	m_pFireBall = NULL;

	m_iCreateItemIndex = 0;

	m_bEnableDownState = false;
	m_bRemoveByWoundState = false;

	m_bSetGraceClose = false;
	m_bFirst = true;
}

ioRotationWeapon::~ioRotationWeapon()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioRotationWeapon::~ioRotationWeapon - UnReleased Resouce" );
	}
}

bool ioRotationWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioRotationWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH] = "";

	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );
	m_fRotationTime = rkLoader.LoadFloat_e( "rotation_time", 0.0f );

	m_bEnableDownState = rkLoader.LoadBool_e( "enable_down_state", false );
	m_bRemoveByWoundState = rkLoader.LoadBool_e( "remove_by_wound", false );
	m_bRemoveBySkillEnd = rkLoader.LoadBool_e( "remove_by_skill_end", false );

	m_bIndependenceChar = rkLoader.LoadBool_e( "independence_char", false );
	m_bReverseRotation = rkLoader.LoadBool_e( "reverse_rotation", false );
}

bool ioRotationWeapon::CreateFireBall( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pFireBall = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireBall )	return false;

	GetParentSceneNode()->AttachObject( m_pFireBall );

	AddEffectEvent( m_pFireBall );
	return true;
}

void ioRotationWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() )
		return;

	SetPosition( vPos );

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
		return;

	m_vOwnerPos = pOwner->GetMidPositionByRate();
	m_vCenterPos = m_vOwnerPos;
	m_vDiffPos = vPos - m_vOwnerPos;
	
	m_qtStartRot = pOwner->GetTargetRot();
}

void ioRotationWeapon::SetMoveDir( const D3DXVECTOR3 &vDir )
{
	D3DXQUATERNION qtRot;
	ioMath::CalcDirectionQuaternion( qtRot, -vDir );
	SetOrientation( qtRot );
}

void ioRotationWeapon::SetGraceClose()
{
	if( m_bSetGraceClose )	return;

	if( m_pFireBall )
	{
		if( m_bEndEffectGrace )
			m_pFireBall->EndEffectGrace();
		else
			m_pFireBall->EndEffectForce();
	}

	StopLoopSound();
	m_bSetGraceClose = true;
}

void ioRotationWeapon::SetLiveTime( DWORD dwTime )
{
}

void ioRotationWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	DWORD dwLooseTime = 0;
	if( GetOwner() )
	{
		dwLooseTime = GetOwner()->GetActionStopTotalDelay();
	}

	if( m_bRemoveBySkillEnd )
	{
		ioHashString szSkillName = GetWeaponBySkillName();
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			ioSkill *pSkill = pOwner->GetCurActiveSkill();
			if( !pSkill )
				SetWeaponDead();
			else if( pSkill && pSkill->GetName() != szSkillName )
				SetWeaponDead();
		}
	}

	if( m_dwCreatedTime + m_dwLiveTime + dwLooseTime < FRAMEGETTIME() )
	{
		SetWeaponDead();
		return;
	}

	if( !CheckCreateItem() )
	{
		SetWeaponDead();
		return;
	}

	if( !CheckOwnerState() )
	{
		SetWeaponDead();
		return;
	}

	if( !CheckPiercing() )
		return;

	if( !m_bFirst )
	{
		// 캐릭터 기준으로 위치 갱신
		ioBaseChar *pOwner = GetOwner();
		if( !pOwner )
		{
			UpdateWorldAttackBox( fTimePerSec, true );
			SetWeaponDead();
			return;
		}

		m_vOwnerPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vCurCencter = m_vOwnerPos;
		if( m_bIndependenceChar )
			vCurCencter = m_vCenterPos;

		// 회전처리
		if( m_fRotationTime > 0.0f )
		{
			float fGapTime = fTimePerSec * FLOAT1000;
			float fRotationRate = fGapTime / m_fRotationTime;
			float fAngle = 0.0f;
			if ( !m_bReverseRotation )
				fAngle = 360.0f * fRotationRate;
			else
				fAngle = -360.0f * fRotationRate;

			fAngle = ioMath::ArrangeHead( fAngle );

			D3DXQUATERNION qtAngle;
			D3DXQuaternionRotationYawPitchRoll( &qtAngle, DEGtoRAD(fAngle), 0.0f, 0.0f );
			SetOrientation( qtAngle );

			m_vDiffPos = qtAngle * m_vDiffPos;

			D3DXVECTOR3 vNewPos = vCurCencter + m_vDiffPos;
			SetPosition( vNewPos );
		}
		else
		{
			D3DXQUATERNION qtCurRot = pOwner->GetTargetRot();

			D3DXVECTOR3 vStartDir = m_qtStartRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vStartDir.y = 0.0f;
			D3DXVec3Normalize( &vStartDir, &vStartDir );

			D3DXVECTOR3 vCurDir = qtCurRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vCurDir.y = 0.0f;
			D3DXVec3Normalize( &vCurDir, &vCurDir );

			D3DXVECTOR3 vDiff = m_vDiffPos;

			if( vCurDir != vStartDir )
			{
				float fDotValue = D3DXVec3Dot( &vStartDir, &vCurDir );
				float fGapAngle = acosf(fDotValue);

				D3DXVECTOR3 vAxis = ioMath::UNIT_Y;
				if( COMPARE( fDotValue, -0.98f, 0.98f ) )
				{
					D3DXVec3Cross( &vAxis, &vStartDir, &vCurDir );
					D3DXVec3Normalize( &vAxis, &vAxis );
				}

				D3DXQUATERNION qtNewRot;
				D3DXQuaternionRotationAxis( &qtNewRot, &vAxis, fGapAngle );

				vDiff = qtNewRot * m_vDiffPos;
			}

			D3DXVECTOR3 vNewPos = vCurCencter + vDiff;
			SetPosition( vNewPos );

			SetOrientation( qtCurRot );
		}

		UpdateWorldAttackBox( fTimePerSec, false );
	}

	m_bFirst = false;
	
	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}
}

void ioRotationWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}
}

D3DXVECTOR3 ioRotationWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	D3DXVECTOR3 vAttackDir;

	if( m_fRotationTime > 0.0f )
	{
		D3DXVec3Cross( &vAttackDir,
					   &D3DXVECTOR3( 0.0f, FLOAT1, 0.0f ),
					   &m_vDiffPos );
	}
	else
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			vAttackDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		}
	}

	D3DXVec3Normalize( &vAttackDir, &vAttackDir );
	
	return vAttackDir;
}

const ioOrientBox& ioRotationWeapon::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioRotationWeapon::GetType() const
{
	return WT_ROTATE;
}

ActionStopType ioRotationWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioRotationWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioRotationWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() || bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		return;
	}

	m_WorldAttackBox = GetLocalAttackBox();

	float fMapCollisionRate = GetAttribute()->m_fMapCollisionBoxRate;
	m_WorldAttackBoxForMap = m_WorldAttackBox;
	m_WorldAttackBoxForMap.SetExtents( 0, m_WorldAttackBox.GetExtents(0)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 1, m_WorldAttackBox.GetExtents(1)*fMapCollisionRate );
	m_WorldAttackBoxForMap.SetExtents( 2, m_WorldAttackBox.GetExtents(2)*fMapCollisionRate );

	if( !m_WorldAttackBox.IsNull() )
	{
		m_WorldAttackBox.Transform( GetPosition(),
									GetScale(),
									GetParentSceneNode()->GetDerivedOrientation() );

		m_WorldAttackBoxForMap.Transform( GetPosition(),
										  GetScale(),
										  GetParentSceneNode()->GetDerivedOrientation() );
	}
}

void ioRotationWeapon::CheckCurAttribute( const ioPlayEntity *pTarget )
{
	ioWeapon::CheckCurAttribute( pTarget );
}

const WeaponAttribute* ioRotationWeapon::GetAttribute() const
{
	if( m_bResistance )
	{
		if( m_pResistanceAttr )
			return m_pResistanceAttr;
	}

	return m_pConstAttr;
}

bool ioRotationWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
		bCollision = CheckCollisionLine( pEntity, vColPoint );

	CheckPiercing( bCollision );

	return bCollision;
}

bool ioRotationWeapon::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	// 타겟이 캐릭터가 아니라면 충돌로 처리한다
	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )	return true;

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( vColPoint );

	return false;	// 주인이 없는 무기라면 충돌안한걸로 하자
}

void ioRotationWeapon::SetCreateItem( const ioHashString &rkName )
{
	m_CreateItem = rkName;
	m_iCreateItemIndex = 0;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioItem *pItem = pOwner->GetEquipedItem( m_CreateItem );
	if( pItem )
	{
		m_iCreateItemIndex = pItem->GetItemCreateIndex();
	}

	//
	if( pItem && pItem->GetItemCreateIndex() == m_iCreateItemIndex )
	{
		int iItemType = (int)pItem->GetType();
		pOwner->CheckSkillItemHide( false, iItemType );
	}
}

void ioRotationWeapon::SetWeaponDead()
{
	StopLoopSound();
	m_bLive = false;

	// Live false 처리후 진행...
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return;

	ioItem *pItem = pOwner->GetEquipedItem( m_CreateItem );
	if( pItem && pItem->GetItemCreateIndex() == m_iCreateItemIndex )
	{
		if( !g_WeaponMgr.CheckRotationWeaponByItem(m_OwnerName, pItem->GetItemCreateIndex()) )
		{
			int iItemType = (int)pItem->GetType();
			pOwner->CheckSkillItemHide( true, iItemType );
		}
	}
}

bool ioRotationWeapon::CheckCreateItem()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return false;

	ioItem *pItem = pOwner->GetEquipedItem( m_CreateItem );
	if( pItem && pItem->GetItemCreateIndex() == m_iCreateItemIndex )
		return true;

	return false;
}

bool ioRotationWeapon::CheckOwnerState()
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner ) return false;

	if( !m_bEnableDownState && pOwner->IsApplyDownState( false ) )
		return false;

	return true;
}


