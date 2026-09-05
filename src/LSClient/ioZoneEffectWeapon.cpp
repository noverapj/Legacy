

#include "stdafx.h"

#include "ioZoneEffectWeapon.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioOwnerControlDummy6.h"

ActionStopType ioZoneEffectWeapon::m_ActionStopType = AST_NONE;
ActionStopType ioZoneEffectWeapon::m_DieActionStopType = AST_NONE;

ioZoneEffectWeapon::ioZoneEffectWeapon( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	SetPeculiarity( WP_DEF_NO_SOUND );

	m_fZoneRange		= 0.0f;
	m_fZoneInnerRange	= 0.0f;
	m_fZoneAngle		= 0.0f;
	
	m_pWeaponEffect = NULL;

	m_vAttackDir	= ioMath::VEC3_ZERO;
	m_vCollisionDir = D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	m_szCollsionIgnoreChar.clear();

	m_dwCallAreaWeaponIndex = 0;

	m_bCalcFirstWorldBox = false;

	m_qtFirstRot = ioMath::QUAT_IDENTITY;

	m_nNowCollisionCnt = 0;

	m_stAttachTargetName.Clear();

	m_bNoCol = false;
	m_bOnce	 = true;
}

ioZoneEffectWeapon::~ioZoneEffectWeapon()
{
	if( m_pWeaponEffect )
	{
		LOG.PrintTimeAndLog( 0, "ioZoneEffectWeapon::~ioZoneEffectWeapon - UnReleased Resouce" );
	}
}

void ioZoneEffectWeapon::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	DWORD dwCurTime = FRAMEGETTIME();

	char szBuf[MAX_PATH] = "";

	m_OwnerAttachType = (OwnerAttachType)rkLoader.LoadInt_e( "zone_attach_type", OAT_NONE );

	rkLoader.LoadString_e( "weapon_effect2", "", szBuf, MAX_PATH );
	m_szWeaponEffect = szBuf;

	m_dwLiveTime = rkLoader.LoadInt_e( "zone_live_time", 0 );

	m_bEnableOwnerScale	= rkLoader.LoadBool_e( "enable_owner_scale", false );

	m_fZoneRange		= rkLoader.LoadFloat_e( "zone_range", 0.0f );
	m_fZoneMinRange		= rkLoader.LoadFloat_e( "zone_min_range", 0.0f );
	m_fZoneMaxRange		= rkLoader.LoadFloat_e( "zone_max_range", 0.0f );
	m_fZoneInnerRange	= rkLoader.LoadFloat_e( "zone_inner_range", 0.0f );
	m_fZoneAngle		= rkLoader.LoadFloat_e( "zone_angle", 0.0f );

	m_fUpHeight		= rkLoader.LoadFloat_e( "zone_up_height", 0.0f );
	m_fUnderHeight	= rkLoader.LoadFloat_e( "zone_under_height", 0.0f );
	m_fWidth		= rkLoader.LoadFloat_e( "zone_width", 0.0f );
	m_fMaxWidth		= rkLoader.LoadFloat_e( "zone_max_width", 0.0f );

	m_fOffSet		= rkLoader.LoadFloat_e( "zone_offset", 0.0f );
	m_fHeightOffSet	= rkLoader.LoadFloat_e( "zone_height_offset", 0.0f );

	rkLoader.LoadString_e( "extra_zone_check_buff", "", szBuf, MAX_PATH );
	m_ExtraZoneCheckBuff = szBuf;
	m_fExtraZoneAngle = rkLoader.LoadFloat_e( "extra_zone_angle", 0.0f );

	m_TargetColType		= (TargetColType)rkLoader.LoadInt_e( "zone_collision_type", 0 );
	m_bDisableColGrap	= rkLoader.LoadBool_e( "zone_collision_disable_grap", 0 );
	m_TargetWoundType	= (TargetWoundType)rkLoader.LoadInt_e( "zone_wound_type", 0 );

	rkLoader.LoadString_e( "boom_effect", "", szBuf, MAX_PATH );
	m_BoomEffect = szBuf;

	m_bDisableZoneMapIgnore = rkLoader.LoadBool_e( "zone_map_ignore", true );
	m_bEnableDefenseSound = rkLoader.LoadBool_e( "enable_defense_sound", false );
	m_bEnableProtectCol = rkLoader.LoadBool_e( "enable_protect_col", false );

	if( m_bEnableDefenseSound )
	{
		m_dwWeaponPeculiarity &= ~WP_DEF_NO_SOUND;
	}

	m_bAttachOnLand = rkLoader.LoadBool_e( "attach_on_land", false );
	m_bOwnerCharRotApply = rkLoader.LoadBool_e( "owenr_rot_apply", true );
	m_bOnlyOneRotApply = rkLoader.LoadBool_e( "only_one_rot_apply", false );
	m_bSelfRotApply = rkLoader.LoadBool_e( "self_rot_apply", false );

	rkLoader.LoadString_e( "call_area_weapon_name", "", szBuf, MAX_PATH );
	m_CallAreaWeaponName = szBuf;
	m_dwCallAreaWeaponTime = (DWORD)rkLoader.LoadInt_e( "call_area_weapon_time", 0 );

	m_bUseDelayCollisionTime = rkLoader.LoadBool_e( "use_delay_collision_time", false );
	m_dwDelayCollisionTime   = (DWORD)rkLoader.LoadInt_e( "max_delay_collision_time", 0 );

	m_iAttackSide = rkLoader.LoadInt_e("collision_attack_side",0 );

	m_bUseMaxCollisionCnt = rkLoader.LoadBool_e( "use_limit_collision_cnt", false );
	m_nMaxCollisionCnt = rkLoader.LoadInt_e( "max_limit_collision_cnt", 1 );

	m_wondItemType = (WoundItemType)rkLoader.LoadInt_e( "wound_item_type", WIT_NONE );

	m_bSetCheckTarget = rkLoader.LoadBool_e( "set_check_target", false );

	m_bTerrainHeightWeapon = rkLoader.LoadBool_e( "set_terrain_height_weapon", false );
	m_fTerrainHeightWeapon = rkLoader.LoadFloat_e( "terrain_height_weapon_y_offset", 0.f );

	m_bNotChangeLiveTime = rkLoader.LoadBool_e( "not_change_live_time", false );

	m_bTargetSelfControl = rkLoader.LoadBool_e( "target_self_control", false );

	m_bUseTestmapCol_ColBox = rkLoader.LoadBool_e( "use_test_map_col_box", false );

	m_bOwnerAttachNotDestroy = rkLoader.LoadBool_e( "owner_attack_not_destroy", false );

	m_fMoveSpeed = rkLoader.LoadFloat_e( "weapon_move_speed", 0.0f );
}

void ioZoneEffectWeapon::SetZoneValue( bool bAttached, bool bSkill )
{
	if( m_OwnerAttachType != OAT_NONE )
	{
		if( bAttached && bSkill )
			m_OwnerAttachType = OAT_SKILL;
	}
	else if( bAttached )
	{
		if( bSkill )
			m_OwnerAttachType = OAT_SKILL;
		else
			m_OwnerAttachType = OAT_NORMAL;
	}
	else
	{
		m_OwnerAttachType = OAT_NONE;
	}

	m_LocalAttackBox.SetExtents( 0, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 1, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 2, m_fZoneRange );

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		if( m_iDummyIndex <= 0 )
		{
			D3DXVECTOR3 vPos = GetPosition();
			D3DXVECTOR3 vMoveDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			vMoveDir = vMoveDir * m_fOffSet;
			vPos = vPos + vMoveDir;

			vPos.y += m_fHeightOffSet;

			if( m_bAttachOnLand )
			{
				ioPlayStage *pStage = pOwner->GetCreator();
				if( pStage )
					vPos.y = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y+3.0f );
			}
			SetPosition( vPos );
		}
	}

	m_fCurRange = m_fZoneRange;
	m_fCurWidth = m_fWidth;

	UpdateWorldAttackBox( 0.0f, false );

	m_dwWeaponEffectID = -1;
}

void ioZoneEffectWeapon::SetZoneValue( float fRange, float fAngle, float fUpHeight, float fUnderHeight, float fWidth, bool bAttached )
{
	m_fZoneRange = fRange;
	m_fZoneAngle = fAngle;

	m_fUpHeight    = fUpHeight;
	m_fUnderHeight = fUnderHeight;
	m_fWidth       = fWidth;

	if( bAttached )
	{
		m_OwnerAttachType = OAT_SKILL;
	}
	else
	{
		m_OwnerAttachType = OAT_NONE;
	}
	
	m_LocalAttackBox.SetExtents( 0, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 1, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 2, m_fZoneRange );
	UpdateWorldAttackBox( 0.0f, false );

	m_dwWeaponEffectID = -1;
}

void ioZoneEffectWeapon::SetZoneValue( float fRange, bool bAttached )
{
	m_fZoneRange		= fRange;

	if( bAttached )
	{
		m_OwnerAttachType = OAT_SKILL;
	}
	else
	{
		m_OwnerAttachType = OAT_NONE;
	}

	m_LocalAttackBox.SetExtents( 0, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 1, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 2, m_fZoneRange );
	UpdateWorldAttackBox( 0.0f, false );

	m_dwWeaponEffectID = -1;
}

void ioZoneEffectWeapon::SetZoneValue( D3DXVECTOR3 vPos, bool bTargetAttached )
{
	if( bTargetAttached )
	{
		m_OwnerAttachType = OAT_TARGET;
	}
	else
	{
		m_OwnerAttachType = OAT_NONE;
	}

	m_LocalAttackBox.SetExtents( 0, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 1, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 2, m_fZoneRange );

	if( vPos != ioMath::VEC3_ZERO )
	{
		vPos.y += m_fHeightOffSet;

		if( m_bAttachOnLand )
		{
			ioPlayStage *pStage = GetOwner()->GetCreator();
			if( pStage )
				vPos.y = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y+3.0f );
		}

		SetPosition( vPos );
	}

	UpdateWorldAttackBox( 0.0f, false );

	m_dwWeaponEffectID = -1;
}

void ioZoneEffectWeapon::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
	{
		m_vStartOffSet = vPos - pOwner->GetWorldPosition();
	}
	else
	{
		m_vStartOffSet = ioMath::VEC3_ZERO;
	}

	if( pOwner && m_bOnlyOneRotApply && !m_bCalcFirstWorldBox )
	{
		m_bCalcFirstWorldBox = true;
		m_qtFirstRot = pOwner->GetWorldOrientation();
	}

	SetPosition( vPos );
	m_LocalAttackBox.SetExtents( 0, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 1, m_fZoneRange );
	m_LocalAttackBox.SetExtents( 2, m_fZoneRange );
	UpdateWorldAttackBox( 0.0f, false );
}

void ioZoneEffectWeapon::SetZoneRange( float fRange )
{
	m_fZoneRange = fRange;
}

void ioZoneEffectWeapon::SetZoneWidth( float fWidth )
{
	m_fWidth = fWidth;
}

void ioZoneEffectWeapon::SetCollisionDir( D3DXVECTOR3 &vDir )
{
	m_vCollisionDir = vDir;
}

D3DXVECTOR3 ioZoneEffectWeapon::GetMoveDir()
{
	return m_vCollisionDir;
}

void ioZoneEffectWeapon::SetCollisionWoundType( int iColType, int iWoundType )
{
	m_TargetColType = (TargetColType)iColType;
	m_TargetWoundType = (TargetWoundType)iWoundType;
}

D3DXVECTOR3 ioZoneEffectWeapon::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	if( m_vAttackDir == ioMath::VEC3_ZERO )
		return ioWeapon::GetAttackDir( pEntity );
	else
		return m_vAttackDir;
}

void ioZoneEffectWeapon::SetAttackDir(  D3DXVECTOR3 &vDir )
{
	m_vAttackDir = vDir;
}

ioWeapon::WeaponType ioZoneEffectWeapon::GetType() const
{
	return WT_ZONE_EFFECT;
}

ActionStopType ioZoneEffectWeapon::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioZoneEffectWeapon::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioZoneEffectWeapon::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !m_CallAreaWeaponName.IsEmpty() &&
		m_dwCreatedTime+m_dwCallAreaWeaponTime < FRAMEGETTIME() )
	{
		CallAreaWeapon();
	}

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_dwCreatedTime > dwCurTime )
	{
		LOG.PrintTimeAndLog( 0, "ioWeapon::Process - Create Time Error(%d/%d)", m_dwCreatedTime, dwCurTime );
		SetWeaponDead();
		return;
	}

	if( !CheckPiercing() )
		return;

	ioBaseChar *pOwner = GetOwner();

	if( !m_szWeaponEffect.IsEmpty() && m_dwWeaponEffectID == -1 )
	{
		D3DXVECTOR3 vPos = pOwner->GetMidPositionByRate();
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
		if( m_bEnableOwnerScale )
		{
			vScale = pOwner->GetWorldScale();
		}

		vScale = vScale * m_fOwnerChargeScaleRate;
		ioMapEffect *pMapEffect = pOwner->CreateMapEffectBySkill( m_szWeaponEffect, vPos, vScale );
		if( pMapEffect )
		{
			m_dwWeaponEffectID = pMapEffect->GetUniqueID();
		}
	}

	DWORD dwLooseTime = 0;
	if( pOwner )
	{
		dwLooseTime = pOwner->GetActionStopTotalDelay();
	}

	if( m_dwCreatedTime + m_dwLiveTime + dwLooseTime < FRAMEGETTIME() && m_dwLiveTime != 0 )
	{
		SetWeaponDead();
	}
	else if( pOwner )
	{
		if( m_iDummyIndex > 0 )
		{
			ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iDummyIndex );
			if( pDummy && m_OwnerAttachType != OAT_NONE )
			{
				D3DXVECTOR3 vPos = pDummy->GetMidPositionByRate();
				D3DXVECTOR3 vDir = pDummy->GetWorldOrientation() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				D3DXVECTOR3 vOffSetAmt = vDir * m_fOffSet;
				vPos += vOffSetAmt;

				SetCollisionDir( vDir );
				SetPosition( vPos );
			}
		}
		else
		{
			if( m_OwnerAttachType == OAT_SKILL && pOwner->GetState() != CS_USING_SKILL )
			{
				SetWeaponDead();
				return;
			}
			
			bool bCalc = false;
			if( m_OwnerAttachType != OAT_NONE && m_OwnerAttachType != OAT_TARGET && m_OwnerAttachType != OAT_ONCE )
			{
				bCalc = true;
			}
			else if( m_OwnerAttachType == OAT_ONCE && m_bOnce )
			{
				bCalc = true;
			}

			if( bCalc )
			{
				D3DXVECTOR3 vDir = pOwner->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
				SetCollisionDir( vDir );

				D3DXVECTOR3 vPos = pOwner->GetWorldPosition() + m_vStartOffSet;
				D3DXVECTOR3 vOffSetAmt = vDir * m_fOffSet;
				vPos += vOffSetAmt;
				vPos.y += m_fHeightOffSet;

				//이비때 추가
				if( m_bTerrainHeightWeapon )
					vPos.y = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y + 3.0f ) + m_fTerrainHeightWeapon;

				SetPosition( vPos );
				SetOrientation( pOwner->GetTargetRot() );
				m_bOnce = false;
			}

			if( m_OwnerAttachType == OAT_TARGET && !m_stAttachTargetName.IsEmpty() )
			{
				ioBaseChar *pAttachTarget = pOwner->GetBaseChar( m_stAttachTargetName );
				if( pAttachTarget )
				{
					D3DXVECTOR3 vDir = pAttachTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
					SetCollisionDir( vDir );

					D3DXVECTOR3 vPos = pAttachTarget->GetWorldPosition() + m_vStartOffSet;
					D3DXVECTOR3 vOffSetAmt = vDir * m_fOffSet;
					vPos += vOffSetAmt;
					vPos.y += m_fHeightOffSet;

					if( m_bTerrainHeightWeapon )
						vPos.y = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y + 3.0f ) + m_fTerrainHeightWeapon;

					SetPosition( vPos );
					SetOrientation( pAttachTarget->GetTargetRot() );
				}
				else
				{
					SetWeaponDead();
					return;
				}
			}

			if( m_TargetColType == TCT_CONE )
			{
				// 시간 계산 및 범위 갱신.
				float fModifyRange = 0.0f;
				float fModifyWidth = 0.0f;
				if( m_dwLiveTime > 0 )
				{
					float fRate = ( fTimePerSec * 1000 ) / m_dwLiveTime;
					fModifyRange = m_fZoneMaxRange * fRate;
					fModifyWidth = m_fMaxWidth * fRate;
					m_fCurRange += fModifyRange;
					m_fCurRange = min( m_fCurRange, m_fZoneMaxRange );

					m_fCurWidth += fModifyWidth;
					m_fCurWidth = min( m_fCurWidth, m_fMaxWidth );
				}
				else
				{
					m_fCurRange = m_fZoneMaxRange;
					m_fCurWidth = m_fMaxWidth;
				}

				m_LocalAttackBox.SetExtents( 0, m_fCurRange );
				m_LocalAttackBox.SetExtents( 1, m_fCurRange );
				m_LocalAttackBox.SetExtents( 2, m_fCurRange );
			}
		}
	}

	if ( m_OwnerAttachType == OAT_NONE )
	{
		if ( m_fMoveSpeed > 0.0f )
		{
			float fMoveDist = m_fMoveSpeed * fTimePerSec;
			GetParentSceneNode()->Translate( GetMoveDir() * fMoveDist );
		}
	}


	UpdateWorldAttackBox( fTimePerSec, false );

	if( m_pWeaponEffect )
	{
		m_pWeaponEffect->Update( fTimePerSec * FLOAT1000 );
	}
}

bool ioZoneEffectWeapon::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	if( m_bNoCol )
		return false;

	//추가 윤태철
	if( m_bUseMaxCollisionCnt && m_nNowCollisionCnt >= m_nMaxCollisionCnt )
		return false;

	ioDummyChar *pDummy = NULL;
	if( m_iDummyIndex > 0 )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iDummyIndex );
			if( pDummy && !pDummy->IsActivateWeapon() )
				return false;
		}
	}

	bool bCollision = false;
	
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_bUseDelayCollisionTime && (m_dwCreatedTime + m_dwDelayCollisionTime) > dwCurTime )
		return false;

	if( m_TargetColType == TCT_SPHERE )
	{
		bCollision = CheckCollisionSphere( pEntity );
	}
	else if( m_TargetColType == TCT_CYLINDER )
	{
		bCollision = CheckCollisionCylinder( pEntity );
	}
	else if( m_TargetColType == TCT_BOX )
	{
		bCollision = CheckCollisionBox( pEntity );
	}
	else if( m_TargetColType == TCT_DONUT )
	{
		bCollision = CheckCollisionDonut( pEntity );
	}
	else if( m_TargetColType == TCT_CONE )
	{
		bCollision = CheckCollisionCone( pEntity );
	}

	ioBaseChar *pChar = ToBaseChar(pEntity);

	if( bCollision && pEntity->GetSubType() == ioPlayEntity::PST_CHAR )
	{
		if( !pChar ) return false;

		// 쓰러짐 관련 체크
		if( m_TargetWoundType == TWT_AIR_WOUND_EXCEPT ) // 쓰러짐중 공중상태는 패스
		{
			if( pChar->IsApplyDownState() && !pChar->IsApplyDownState(false) )
				return false;
		}
		else if( m_TargetWoundType == TWT_DOWN_WOUND_EXCEPT ) // 쓰러짐중 바닥상태는 패스
		{
			if( pChar->IsApplyDownState(false) )
				return false;
		}
		else if( m_TargetWoundType == TWT_ALL_WOUND_EXCEPT )
		{
			if( m_bDisableColGrap && pChar->IsApplyDownState() ||
				( pChar->GetState() == CS_GRAPPLING_WOUNDED ||
				  pChar->GetState() == CS_GRAPPLING_PUSHED ||
				  pChar->GetState() == CS_GRAPPLING_WOUNDED_SKILL ||
				  pChar->GetState() == CS_GRAPPLING_WOUNDED_SWING ||
				  pChar->GetState() == CS_GRAPPLING_WOUNDED_ITEM ||
				  pChar->GetState() == CS_GRAPPLING_WOUNDED_BUFF ) )
			{
				return false;
			}
			else if( pChar->IsApplyDownState() )
			{
				return false;
			}
			
		}
		else if( m_TargetWoundType == TWT_FLOATING_STATE )	// 공중에 뜬 상태가 아니면 패스
		{
			if( !pChar->IsFloating() )
				return false;
		}
		else if( m_TargetWoundType == TWT_DOWN_WOUNDED ) //바닥에 쓰러져 있지 않거나 공중에 뜬 상태면 패스
		{			
			if( !pChar->IsApplyDownState() || pChar->IsFloating() )
			{				
				return false;
			}
		}
	}

	if( bCollision && pChar && pChar->IsProtectState() && !m_bEnableProtectCol )
	{
		int iType = pChar->CheckBlockEmoticonType();
		if( iType >= 0 && !IsAlreadyBlocked( pChar ) )
		{
			AddBlockEntity( pChar );
			pChar->SetProtectWound( iType );
		}

		bCollision = false;
	}
	
	if( bCollision && pChar && 0 < m_iAttackSide )
	{
		D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVECTOR3 vAttack = -GetAttackDir( pEntity );

		float fAttackedCosine = D3DXVec3Dot( &vDir, &vAttack );
		if( fAttackedCosine >= pChar->GetFrontSideAngleCosine() )
		{
			if ( m_iAttackSide == AS_FRONT )
				bCollision = true;
			else
				bCollision = false;
		}
		else
		{
			if ( m_iAttackSide == AS_BACK )
				bCollision = true;
			else
				bCollision = false;
		}
	}
	
	//char Collsion ignore
	if( bCollision && pChar )
	{	
		if(!m_szCollsionIgnoreChar.empty() )
		{
			ioHashStringVec::iterator iter = m_szCollsionIgnoreChar.begin();
			for( ; iter != m_szCollsionIgnoreChar.end(); ++iter )
			{
				ioHashString szName = (*iter);
				if( pChar->GetCharName() == szName )
					bCollision = false;
			}
		}
	}

	if( bCollision && pDummy )
	{
		pDummy->CheckCollisionWeapon( GetOwner() );
		ioOwnerControlDummy6* pOwnerControlDummy6 = ToOwnerControlDummy6( pDummy );
		if( pOwnerControlDummy6 && pChar )
			pOwnerControlDummy6->CheckCollisionWeapon( pChar );
	}

	//추가 윤태철
	//최대 충돌 가능 숫자를 설정
	if( bCollision && m_bUseMaxCollisionCnt && m_nNowCollisionCnt >= m_nMaxCollisionCnt )
		return false;

	if( bCollision )
		m_nNowCollisionCnt++;

	return bCollision;
}

bool ioZoneEffectWeapon::CheckCollisionSphere( ioPlayEntity *pEntity )
{
	D3DXVECTOR3 vPos = GetWorldAttackBox().GetCenter();
	
	if( m_bAttachOnLand && GetOwner() )
	{
		ioPlayStage *pStage = GetOwner()->GetCreator();
		if( pStage )
			vPos.y = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y+3.0f );
	}

	float fZoneRange = m_fZoneRange * m_fOwnerChargeRangeRate;

	D3DXVECTOR3 vDiff = vPos - pEntity->GetMidPositionByRate();
	float fLength = D3DXVec3Length( &vDiff );
	if( fLength <= m_fZoneMinRange )
		return false;

	D3DXVECTOR3 vPoint = ioMath::VEC3_ZERO;
	if( !pEntity->CheckSpherePoint( vPos, fZoneRange, vPoint ) )
		return false;

	D3DXVECTOR3 vDir = vPoint - vPos;
	D3DXVec3Normalize( &vDir, &vDir );

	float fCurCheckAngle = m_fZoneAngle;
	if( !m_ExtraZoneCheckBuff.IsEmpty() && GetOwner() && ToBaseChar(pEntity) )
	{
		if( ToBaseChar(pEntity)->HasBuff( GetOwner()->GetCharName(), m_ExtraZoneCheckBuff ) )
		{
			fCurCheckAngle = m_fExtraZoneAngle;
		}
	}

	float fHalfCosine = cosf( DEGtoRAD( fCurCheckAngle ) * FLOAT05 );
	if( D3DXVec3Dot( &m_vCollisionDir, &vDir ) >= fHalfCosine )
		return true;

	return false;
}

bool ioZoneEffectWeapon::CheckCollisionCylinder( ioPlayEntity *pEntity )
{
	if( m_fUpHeight == 0.0f && m_fUnderHeight == 0.0f )
		return false;

	float fZoneRange = m_fZoneRange * m_fOwnerChargeRangeRate;

	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();
	D3DXVECTOR3 vTop, vBottom;

	if( m_bAttachOnLand && GetOwner() )
	{
		ioPlayStage *pStage = GetOwner()->GetCreator();
		if( pStage )
			vCenter.y = pStage->GetMapHeightNoneEntity( vCenter.x, vCenter.z, vCenter.y+3.0f );
	}

	vTop = vBottom = vCenter;
	vTop.y += m_fUpHeight;
	vBottom.y -= m_fUnderHeight;
	ioCylinder kCylinder( vTop, vBottom, fZoneRange );
	vCenter = kCylinder.GetCenter();

	// 사각형을 이루는 2점의 포인트를 구한다. 가장 먼거리의 대각선의 2점
	D3DXVECTOR3 vMin, vMax;
	vMax = vTop;
	vMin = vBottom;

	vMax.x += fZoneRange;
	vMax.z -= fZoneRange;

	vMin.x -= fZoneRange;
	vMin.z += fZoneRange;

	// 사각형 생성
	ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( vMin, vMax );

	#ifndef SHIPPING
	kBuffBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
	#endif


	D3DXVECTOR3 vCollisionPoint;
	if( !pEntity->CheckCylinderPoint( kCylinder, vCenter, vCollisionPoint ) )
		return false;

	// 수평 거리 체크
	D3DXVECTOR3 vDiff1 = vCollisionPoint - vCenter;
	vDiff1.y = 0.0f;

	float fTargetDist = D3DXVec3LengthSq( &vDiff1 );
	if( fTargetDist > fZoneRange * fZoneRange )
		return false;

	if( fTargetDist < m_fZoneMinRange * m_fZoneMinRange )
		return false;

	// 수평 각 체크
	float fCurCheckAngle = m_fZoneAngle;
	if( !m_ExtraZoneCheckBuff.IsEmpty() && GetOwner() && ToBaseChar(pEntity) )
	{
		if( ToBaseChar(pEntity)->HasBuff( GetOwner()->GetCharName(), m_ExtraZoneCheckBuff ) )
		{
			fCurCheckAngle = m_fExtraZoneAngle;
		}
	}

	float fHalfCosine = cosf( DEGtoRAD( fCurCheckAngle ) * FLOAT05 );
	D3DXVec3Normalize( &vDiff1, &vDiff1 );

	if( D3DXVec3Dot( &m_vCollisionDir, &vDiff1 ) < fHalfCosine )
		return false;

	// 높이 체크
	if( vCollisionPoint.y >= vBottom.y && vCollisionPoint.y <= vTop.y )
		return true;

	return false;
}

bool ioZoneEffectWeapon::CheckCollisionBox( ioPlayEntity *pEntity )
{
	if( m_fUpHeight == 0.0f && m_fUnderHeight == 0.0f )
		return false;

	if( m_fWidth == 0.0f )
		return false;

	ioBaseChar *pOwnerChar = GetOwner();
	if( !pOwnerChar )
		return false;

	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();

	// 사각형의 center을 vPos로 설정하게 되므로 여기서 캐릭터앞쪽에 생성하고 높이를 보정한다.
	if( m_OwnerAttachType != OAT_TARGET )
	{
		vCenter += m_vCollisionDir * m_fZoneRange * FLOAT05; 
		D3DXVECTOR3 vOffset( 0.0f, ( ( m_fUpHeight + m_fUnderHeight ) * FLOAT05 ) - m_fUpHeight, 0.0f );
		vCenter -= vOffset;
	}

	if( m_bAttachOnLand && GetOwner() )
	{
		ioPlayStage *pStage = GetOwner()->GetCreator();
		if( pStage )
			vCenter.y = pStage->GetMapHeightNoneEntity( vCenter.x, vCenter.z, vCenter.y+3.0f );
	}

	// 사각형을 이루는 2점의 포인트를 구한다. 가장 먼거리의 대각선의 2점
	D3DXVECTOR3 vMin, vMax;
	vMin = vMax = vCenter;
	vMin.x -= ( m_fWidth * FLOAT05 );
	vMin.y -= m_fUnderHeight;
	vMax.x += ( m_fWidth * FLOAT05 );
	vMax.y += m_fUpHeight;
	vMax.z += m_fZoneRange;

	// 사각형 생성
	ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( vMin, vMax );
	// 유저 방향으로 사각형 회전
	if( m_bOwnerCharRotApply )
	{
		if( m_iDummyIndex > 0 )
		{
			ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwnerChar, m_iDummyIndex );
			if( pDummy && m_OwnerAttachType != OAT_NONE )
			{
				kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, pDummy->GetWorldOrientation() );
			}
		}
		else
		{
			if( !m_bOnlyOneRotApply )
				kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, pOwnerChar->GetWorldOrientation() );
			else
				kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, m_qtFirstRot );
		}
	}
	else if ( m_bTargetSelfControl )
	{
		ioBaseChar *pOwner = GetOwner();
		ioBaseChar *pAttachTarget = pOwner->GetBaseChar( m_stAttachTargetName );
		if( pAttachTarget )
			kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, pAttachTarget->GetWorldOrientation() );
		else
			kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, m_qtFirstRot );
	}
	else if( !m_bOwnerCharRotApply && m_bSelfRotApply )
	{
		kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, GetOrientation() );
	}
	else
		kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, ioMath::QUAT_IDENTITY );

	// 회전후 위치가 변경되므로 중심을 다시 pos로 이동
	kBuffBox.SetCenter( vCenter );
#ifndef SHIPPING
	kBuffBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
#endif

	if( !pEntity )
		return false;

	if( !pEntity->IsWoundedCollision( kBuffBox ) )
		return false;

	return true;
}

bool ioZoneEffectWeapon::CheckCollisionDonut( ioPlayEntity *pEntity )
{
	if( m_fUpHeight == 0.0f && m_fUnderHeight == 0.0f )
		return false;

	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();
	if( m_bAttachOnLand && GetOwner() )
	{
		ioPlayStage *pStage = GetOwner()->GetCreator();
		if( pStage )
			vCenter.y = pStage->GetMapHeightNoneEntity( vCenter.x, vCenter.z, vCenter.y+3.0f );
	}

	D3DXVECTOR3 vTop, vBottom;
	vTop = vBottom = vCenter;
	vTop.y += m_fUpHeight;
	vBottom.y -= m_fUnderHeight;
	ioCylinder kCylinder( vTop, vBottom, m_fZoneRange );
	vCenter = kCylinder.GetCenter();


	// 사각형을 이루는 2점의 포인트를 구한다. 가장 먼거리의 대각선의 2점
	D3DXVECTOR3 vMin, vMax;
	vMax = vTop;
	vMin = vBottom;

	vMax.x += m_fZoneRange;
	vMax.z -= m_fZoneRange;

	vMin.x -= m_fZoneRange;
	vMin.z += m_fZoneRange;

	// 사각형 생성
	ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( vMin, vMax );

#ifndef SHIPPING
	kBuffBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
#endif


	D3DXVECTOR3 vCollisionPoint;
	if( !pEntity->CheckCylinderPoint( kCylinder, vCenter, vCollisionPoint ) )
		return false;

	// 수평 거리 체크
	D3DXVECTOR3 vDiff1 = vCollisionPoint - vCenter;
	vDiff1.y = 0.0f;

	float fTargetDist = D3DXVec3LengthSq( &vDiff1 );
	if( fTargetDist > m_fZoneRange * m_fZoneRange )
	{
		return false;
	}

	float fInnerRange = m_fZoneRange - m_fZoneInnerRange;
	fInnerRange = max( 0, fInnerRange );

	if( fTargetDist < fInnerRange * fInnerRange )
	{
		return false;
	}

	// 수평 각 체크
	float fCurCheckAngle = m_fZoneAngle;
	if( !m_ExtraZoneCheckBuff.IsEmpty() && GetOwner() && ToBaseChar(pEntity) )
	{
		if( ToBaseChar(pEntity)->HasBuff( GetOwner()->GetCharName(), m_ExtraZoneCheckBuff ) )
		{
			fCurCheckAngle = m_fExtraZoneAngle;
		}
	}

	float fHalfCosine = cosf( DEGtoRAD( fCurCheckAngle ) * FLOAT05 );
	D3DXVec3Normalize( &vDiff1, &vDiff1 );

	if( D3DXVec3Dot( &m_vCollisionDir, &vDiff1 ) < fHalfCosine )
		return false;

	// 높이 체크
	if( vCollisionPoint.y >= vBottom.y && vCollisionPoint.y <= vTop.y )
	{
		return true;
	}

	return false;
}

bool ioZoneEffectWeapon::CheckCollisionCone( ioPlayEntity *pEntity )
{
	if( m_fWidth == 0.0f )
		return false;

	ioBaseChar *pOwnerChar = GetOwner();
	if( !pOwnerChar )
		return false;

	float fCurRange		= m_fCurRange;
	float fZoneRange	= m_fZoneRange;
	float fZoneMaxRange	= m_fZoneMaxRange;
	float fCurWidth		= m_fCurWidth;
	float fWidth		= m_fWidth;
	float fMaxWidth		= m_fMaxWidth;

	fCurRange		*= pOwnerChar->GetWorldScale().x;
	fZoneRange		*= pOwnerChar->GetWorldScale().x;
	fZoneMaxRange	*= pOwnerChar->GetWorldScale().x;
	fCurWidth		*= pOwnerChar->GetWorldScale().x;
	fWidth			*= pOwnerChar->GetWorldScale().x;
	fMaxWidth		*= pOwnerChar->GetWorldScale().x;

	D3DXVECTOR3 vDiff = pEntity->GetMidPositionByRate() - pOwnerChar->GetMidPositionByRate();
	float fLengthSq = D3DXVec3LengthSq( &vDiff );
	float fLength = D3DXVec3Length( &vDiff );

	// 사거리 체크
	if( fLengthSq > fCurRange * fCurRange )
		return false;

	// 가로폭을 기준으로 충돌영역의 경사각을 구한다.
	D3DXVECTOR3 v0, v1, v2;
	v0 = ioMath::VEC3_ZERO;
	v1 = D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 ) * fCurRange;
	v2 = v1 + D3DXVECTOR3( fCurWidth*0.5f, 0.0f, 0.0f );
	v1 = v1 - v0;
	v2 = v2 - v0;
	D3DXVec3Normalize( &v1, &v1 );
	D3DXVec3Normalize( &v2, &v2 );
	float fDotValue = D3DXVec3Dot( &v1, &v2 );
	float fAngle = RADtoDEG( acosf( fDotValue ) );

	// 타겟과의 각도를 구한다.
	D3DXVECTOR3	vStart = pOwnerChar->GetMidPositionByRate() - ( m_vCollisionDir * m_fOffSet );
	D3DXVECTOR3 v3 = pEntity->GetMidPositionByRate();
	v3 = v3 - vStart;
	D3DXVec3Normalize( &v3, &v3 );
	float fDotValue2 = D3DXVec3Dot( &m_vCollisionDir, &v3 );
	float fAngle2 = RADtoDEG( acosf( fDotValue2 ) );

//////////////////////////////////////////////////////////////////////////
//바운드박스 체크용
#ifndef SHIPPING
	D3DXVECTOR3 vCenter = pOwnerChar->GetMidPositionByRate();

	D3DXVECTOR3	vArray[8];
	D3DXVECTOR3	vMinXAxis, vMinYAxis, vMinZAxis;
	D3DXVECTOR3	vMaxXAxis, vMaxYAxis, vMaxZAxis;

	vMinXAxis = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	vMinYAxis = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	vMinZAxis = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );

	vMaxXAxis = D3DXVECTOR3( m_fCurWidth*0.5f, 0.0f, 0.0f );
	vMaxYAxis = D3DXVECTOR3( 0.0f, m_fCurWidth*0.5f, 0.0f );
	vMaxZAxis = D3DXVECTOR3( 0.0f, 0.0f, -m_fCurRange );

	vMinXAxis = pOwnerChar->GetWorldOrientation() * vMinXAxis;
	vMinYAxis = pOwnerChar->GetWorldOrientation() * vMinYAxis;
	vMinZAxis = pOwnerChar->GetWorldOrientation() * vMinZAxis;
	vMaxXAxis = pOwnerChar->GetWorldOrientation() * vMaxXAxis;
	vMaxYAxis = pOwnerChar->GetWorldOrientation() * vMaxYAxis;
	vMaxZAxis = pOwnerChar->GetWorldOrientation() * vMaxZAxis;

	// 아래면
	vArray[0] = vCenter - vMinXAxis - vMinYAxis - vMinZAxis;
	vArray[1] = vCenter + vMinXAxis - vMinYAxis - vMinZAxis;
	vArray[2] = vCenter + vMaxXAxis - vMaxYAxis + vMaxZAxis;
	vArray[3] = vCenter - vMaxXAxis - vMaxYAxis + vMaxZAxis;

	// 윗면
	vArray[4] = vCenter - vMinXAxis + vMinYAxis - vMinZAxis;
	vArray[5] = vCenter + vMinXAxis + vMinYAxis - vMinZAxis;
	vArray[6] = vCenter + vMaxXAxis + vMaxYAxis + vMaxZAxis;
	vArray[7] = vCenter - vMaxXAxis + vMaxYAxis + vMaxZAxis;

	for( int i=0; i<8; ++i )
	{
		ioBaseChar::m_vRangeBoxArrayForTest[i] = vArray[i];
	}
#endif
//////////////////////////////////////////////////////////////////////////

	// 각도체크
	if( fAngle < fAngle2 )
	{
		return false;
	}

	return true;
}

void ioZoneEffectWeapon::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
{
	if( !IsLive() && bSetNull )
	{
		m_WorldAttackBox.SetNull();
		m_WorldAttackBoxForMap.SetNull();
		m_fCurRange = 0.0f;
		m_fCurWidth = 0.0f;
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
		if( m_bOwnerCharRotApply )
		{
			if( !m_bOnlyOneRotApply )
			{
				m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
				m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
			}
			else
			{
				m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, m_qtFirstRot );
				m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, m_qtFirstRot );
			}
			
		}
		else if( !m_bOwnerCharRotApply && m_bSelfRotApply )
		{
			m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
			m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, GetOrientation() );
		}
		else
		{
			m_WorldAttackBox.Transform( GetPosition(), ioMath::UNIT_ALL, ioMath::QUAT_IDENTITY );
			m_WorldAttackBoxForMap.Transform( GetPosition(), ioMath::UNIT_ALL, ioMath::QUAT_IDENTITY );
		}
	}
}

const ioOrientBox& ioZoneEffectWeapon::GetLocalAttackBox() const
{
	return m_LocalAttackBox;
}

bool ioZoneEffectWeapon::IsAlreadyBlocked( ioPlayEntity *pEntity ) const
{
	BlockEntityList::const_iterator iter;
	for( iter=m_vBlockEntityList.begin() ; iter!=m_vBlockEntityList.end() ; ++iter )
	{
		if( *iter == pEntity )
			return true;
	}
	return false;
}

void ioZoneEffectWeapon::AddBlockEntity( ioPlayEntity *pEntity )
{
	m_vBlockEntityList.push_back( pEntity );
}

bool ioZoneEffectWeapon::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		DestroyResource();

		if( !GetAttribute()->m_WeaponEffect.IsEmpty() )
		{
			CreateWeaponEffect( GetAttribute()->m_WeaponEffect );
		}

		return true;
	}

	return false;
}

bool ioZoneEffectWeapon::CreateWeaponEffect( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pWeaponEffect = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pWeaponEffect )	return false;
	
	D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );
	if( m_bEnableOwnerScale )
	{
		vScale = pOwner->GetWorldScale();
	}

	GetParentSceneNode()->AttachObject( m_pWeaponEffect );
	GetParentSceneNode()->SetScale( vScale );
	D3DXVECTOR3	vPos = GetPosition();

	AddEffectEvent( m_pWeaponEffect );

	return true;
}

void ioZoneEffectWeapon::CallAreaWeapon()
{
	if( m_CallAreaWeaponName.IsEmpty() )
		return;

	if( m_dwCallAreaWeaponIndex > 0 )
		return;

	if( m_dwCallAreaWeaponIndex == -1 )
		return;

	D3DXQUATERNION qtRot = ioMath::QUAT_IDENTITY;
	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		qtRot = pOwner->GetWorldOrientation();
	ioAreaWeapon *pArea = g_AreaWeaponMgr.CreateAreaWeapon( GetOwnerName(),
															"",
															"",
															m_CallAreaWeaponName,
															GetPosition(),
															qtRot,
															ioAreaWeapon::CT_NORMAL );

	if( pArea )
	{
		m_dwCallAreaWeaponIndex = pArea->GetAreaWeaponIndex();
		pArea->SetCreateWeaponIndex( m_dwWeaponIdx );
	}
}

void ioZoneEffectWeapon::DestroyResource( ioPlayStage *pStage )
{
	if( m_pWeaponEffect )
	{
		g_EffectFactory.DestroyEffect( m_pWeaponEffect );

		m_pWeaponEffect = NULL;
	}
}

void ioZoneEffectWeapon::SetLiveTime( DWORD dwTime )
{
	if( m_bNotChangeLiveTime )
		return;

	if( m_dwLiveTime == 0 )
	{
		m_dwLiveTime = dwTime;
		return;
	}

	if( dwTime > 1 )
	{
		m_dwLiveTime = dwTime;
	}
}

void ioZoneEffectWeapon::SetWeaponDead()
{
	if( !m_BoomEffect.IsEmpty() )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner && pOwner->GetCreator() )
		{
			D3DXVECTOR3 vPos = GetPosition();
			if( m_TargetColType != TCT_BOX )
				vPos += m_vCollisionDir * m_fOffSet;

			if( m_bAttachOnLand )
			{
				ioPlayStage *pStage = pOwner->GetCreator();
				if( pStage )
					vPos.y = pStage->GetMapHeightNoneEntity( vPos.x, vPos.z, vPos.y+3.0f );
			}
			D3DXVECTOR3 vScale = pOwner->GetWorldScale();
			ioMapEffect *pMapEffect = pOwner->GetCreator()->CreateMapEffect( m_BoomEffect, vPos, vScale );
			if( pMapEffect )
			{
				ioEffect *pEff = pMapEffect->GetEffect();
				if( pEff )
				{
					D3DXQUATERNION qtRot;
					ioMath::CalcDirectionQuaternion( qtRot, -m_vCollisionDir );
					pEff->GetParentNode()->SetOrientation( qtRot );
				}
			}
			ioWeapon::PlayExplosionSound();
		}
	}

	ioDummyChar* pDummy = NULL;
	if( m_iDummyIndex > 0 )
	{
		ioBaseChar *pOwner = GetOwner();
		if( pOwner )
		{
			pDummy = g_DummyCharMgr.FindDummyChar( pOwner, m_iDummyIndex );
			ioOwnerControlDummy6* pOwnerControlDummy6 = ToOwnerControlDummy6( pDummy );
			if( pOwnerControlDummy6 )
				pOwnerControlDummy6->RemoveAttachWeaponType( m_pConstAttr->m_iAttributeIndex );
		}
	}
	
	m_szCollsionIgnoreChar.clear();

	ioWeapon::SetWeaponDead();
}

bool ioZoneEffectWeapon::CheckCollisionLine( ioPlayEntity *pTarget, const D3DXVECTOR3 &vColPoint )
{
	if( GetPiercingType() == CPT_ALL || GetPiercingType() == CPT_ALL_LIMITE )
		return true;

	// 타겟이 캐릭터가 아니라면 충돌로 처리한다
	ioBaseChar *pTargetChar = ToBaseChar( pTarget );
	if( !pTargetChar )	return true;

	D3DXVECTOR3 vPos = GetPosition();

	ioBaseChar *pOwner = GetOwner();
	if( pOwner )
		return pOwner->CheckCollisionLine( vPos, vColPoint, true );

	return false;	// 주인이 없는 무기라면 충돌안한걸로 하자
}

bool ioZoneEffectWeapon::CheckDestroyOwnerAttach()
{
	switch( m_OwnerAttachType )
	{
	case OAT_NORMAL:
	case OAT_SKILL:
		if ( m_bOwnerAttachNotDestroy )
		{
			return false;
		}
		break;
	default:
		return false;
	}

	if( m_iDummyIndex > 0 )
		return false;

	return true;
}


void ioZoneEffectWeapon::GetColOrientBox( ioOrientBox& kBuffBox )
{
	ioBaseChar *pOwnerChar = GetOwner();
	if( !pOwnerChar )
		return;

	D3DXVECTOR3 vCenter = GetWorldAttackBox().GetCenter();

	// 사각형의 center을 vPos로 설정하게 되므로 여기서 캐릭터앞쪽에 생성하고 높이를 보정한다.
	if( m_OwnerAttachType != OAT_TARGET )
	{
		vCenter += m_vCollisionDir * m_fZoneRange * FLOAT05; 
		D3DXVECTOR3 vOffset( 0.0f, ( ( m_fUpHeight + m_fUnderHeight ) * FLOAT05 ) - m_fUpHeight, 0.0f );
		vCenter -= vOffset;
	}

	if( m_bAttachOnLand && GetOwner() )
	{
		ioPlayStage *pStage = GetOwner()->GetCreator();
		if( pStage )
			vCenter.y = pStage->GetMapHeightNoneEntity( vCenter.x, vCenter.z, vCenter.y+3.0f );
	}

	// 사각형을 이루는 2점의 포인트를 구한다. 가장 먼거리의 대각선의 2점
	D3DXVECTOR3 vMin, vMax;
	vMin = vMax = vCenter;
	vMin.x -= ( m_fWidth * FLOAT05 );
	vMin.y -= m_fUnderHeight;
	vMax.x += ( m_fWidth * FLOAT05 );
	vMax.y += m_fUpHeight;
	vMax.z += m_fZoneRange;

	// 사각형 생성
	//ioOrientBox kBuffBox;
	kBuffBox.SetBoxByMinMax( vMin, vMax );
	// 유저 방향으로 사각형 회전
	if( m_bOwnerCharRotApply )
	{
		if( m_iDummyIndex > 0 )
		{
			ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pOwnerChar, m_iDummyIndex );
			if( pDummy && m_OwnerAttachType != OAT_NONE )
			{
				kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, pDummy->GetWorldOrientation() );
			}
		}
		else
		{
			if( !m_bOnlyOneRotApply )
				kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, pOwnerChar->GetWorldOrientation() );
			else
				kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, m_qtFirstRot );
		}
	}
	else if ( m_bTargetSelfControl )
	{
		ioBaseChar *pOwner = GetOwner();
		ioBaseChar *pAttachTarget = pOwner->GetBaseChar( m_stAttachTargetName );
		kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, pAttachTarget->GetWorldOrientation() );
	}
	else if( !m_bOwnerCharRotApply && m_bSelfRotApply )
	{
		kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, GetOrientation() );
	}
	else
		kBuffBox.Transform( vCenter, ioMath::UNIT_ALL, ioMath::QUAT_IDENTITY );

	// 회전후 위치가 변경되므로 중심을 다시 pos로 이동
	kBuffBox.SetCenter( vCenter );
#ifndef SHIPPING
	kBuffBox.GetVertices( ioBaseChar::m_vRangeBoxArrayForTest );
#endif
}


bool ioZoneEffectWeapon::TestMapCollision( ioOpcodeShape *pShape, ioPlayStage *pStage, const D3DXMATRIX* pShapeWorldMat )
{
	//pShape에 대한 맵 충돌 검사를 중지 하고자 할때 TRUE
	//일반 모드의 경우 월드 메쉬가 1개이지만 하우징시스템의 경우 N개 이기 때문에 TestMapCollision() 함수가 여러번 호출됨

	if( m_bDisableZoneMapIgnore )
		return true;

	if( m_bNoCol )
		return true;

	if( m_bUseTestmapCol_ColBox && pStage )
	{
		ioOrientBox kBuffBox;
		GetColOrientBox( kBuffBox );

		if( pShape->TestIntersection( pShapeWorldMat, kBuffBox ) )
		{
			ioBaseChar *pOwner = GetOwner();
			if( pOwner )
			{
				ioAttackableItem *pItem = pOwner->GetPriorityItem( SPT_ATTACK );
				if( pItem )
					pItem->WeaponMapColDead( this );
			}
			SetWeaponDead();
		}
	}
	else if( pStage && pShape->TestIntersection( pShapeWorldMat, GetWorldAttackBox(true) ) )
	{
		SetWeaponDead();
	}

	return false;
}

void ioZoneEffectWeapon::CheckPreCollision( ioPlayEntity *pTarget, ioPlayStage *pStage )
{
	if( m_bDisableZoneMapIgnore )
		return;

	if( !pTarget )
		return;

	if( ToBaseChar(pTarget) )
		return;
	
	if( pTarget->IsWoundedCollision( GetWorldAttackBox() ) )
	{
		if( ToSymbolStruct(pTarget) || ToPushStruct(pTarget) || ToBallStruct(pTarget) || ToWallZone(pTarget) )
		{
			ioBaseChar *pOwner = GetOwner();
			if( pOwner )
			{
				SetWeaponDead();			
			}
		}
	}
}

void ioZoneEffectWeapon::SetIgnoreCollsionChar( ioHashString& szName )
{
	if( szName.IsEmpty() ) return;
	m_szCollsionIgnoreChar.push_back( szName );
}



int	ioZoneEffectWeapon::GetWoundItemType()
{
	return m_wondItemType;
}

//플래쉬 텔레포트 무기 확인을 위해 추가
bool ioZoneEffectWeapon::IsCheckTarget()
{
	return m_bSetCheckTarget;
}


void ioZoneEffectWeapon::SetAttachTargetName( const ioHashString& stTargetName )
{
	m_stAttachTargetName = stTargetName;
}

void ioZoneEffectWeapon::SetMoveSpeed( float fSpeed )
{
	m_fMoveSpeed = fSpeed;
}
