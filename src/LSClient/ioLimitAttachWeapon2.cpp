

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"

#include "ioLimitAttachWeapon2.h"

ActionStopType ioLimitAttachWeapon2::m_ActionStopType = AST_NONE;
ActionStopType ioLimitAttachWeapon2::m_DieActionStopType = AST_NONE;

ioLimitAttachWeapon2::ioLimitAttachWeapon2( ioBaseChar *pOwner, const WeaponAttribute *pAttr )
: ioWeapon( pOwner, pAttr )
{
	m_dwWeaponEffectStand	= -1;
	m_dwWeaponEffectMove	= -1;
	m_dwWeaponEffectJump	= -1;
	m_dwWeaponEffectEnd		= -1;

	m_SkillSoundID = 0;

	m_vPreScale = ioMath::VEC3_ZERO;

	Init();
	SetPeculiarity( WP_DIVIDE_ACTION );
	m_pFireBall = NULL;
}

ioLimitAttachWeapon2::~ioLimitAttachWeapon2()
{
	if( m_pFireBall )
	{
		LOG.PrintTimeAndLog( 0, "ioLimitAttachWeapon2::~ioMissileWeapon - UnReleased Resouce" );
	}
}

void ioLimitAttachWeapon2::Init()
{
	m_vCurPosition = ioMath::VEC3_ZERO;
	m_vPrePosition = ioMath::VEC3_ZERO;
}

bool ioLimitAttachWeapon2::InitResource()
{
	if( ioWeapon::InitResource() )
	{
		Init();
		DestroyResource();
		return CreateFireBall( GetAttribute()->m_WeaponEffect );
	}

	return false;
}

void ioLimitAttachWeapon2::LoadProperty( ioINILoader &rkLoader )
{
	ioWeapon::LoadProperty( rkLoader );

	char szBuf[MAX_PATH];
	m_dwLiveTime = rkLoader.LoadInt_e( "live_time", 0 );

	rkLoader.LoadString_e( "reduce_collision_by_buff", "", szBuf, MAX_PATH );
	m_szReduceColByBuffName = szBuf;

	rkLoader.LoadString_e( "skill_sound", "", szBuf, MAX_PATH );
	m_szSkillSoundName = szBuf;

	rkLoader.LoadString_e( "weapon_effect_stand", "", szBuf, MAX_PATH );
	m_szWeaponEffectStand = szBuf;
	rkLoader.LoadString_e( "weapon_effect_move", "", szBuf, MAX_PATH );
	m_szWeaponEffectMove = szBuf;
	rkLoader.LoadString_e( "weapon_effect_jump", "", szBuf, MAX_PATH );
	m_szWeaponEffectJump = szBuf;
	rkLoader.LoadString_e( "weapon_effect_end", "", szBuf, MAX_PATH );
	m_szWeaponEffectEnd = szBuf;

	m_bIgnoreColOnDelay	= rkLoader.LoadBool_e( "ignore_collision_on_delay", false );
	m_bIgnoreColOnMove	= rkLoader.LoadBool_e( "ignore_collision_on_move", false );
	m_bIgnoreColOnJump	= rkLoader.LoadBool_e( "ignore_collision_on_jump", false );
}

bool ioLimitAttachWeapon2::CreateFireBall( const ioHashString &szFileName )
{
	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )	return false;

	m_pFireBall = g_EffectFactory.CreateEffect( szFileName );
	if( !m_pFireBall )	return false;

	if( !m_szSkillSoundName.IsEmpty() )
	{
		ioEntityGroup *pGrp = pOwner->GetGroup();
		if(pGrp)
			m_SkillSoundID = g_SoundMgr.PlaySound( m_szSkillSoundName, pGrp->GetParentSceneNode() );
	}

	GetParentSceneNode()->AttachObject( m_pFireBall );
	UpdateCurPosition( pOwner );

	AddEffectEvent( m_pFireBall );

	return true;
}

void ioLimitAttachWeapon2::SetStartPosition( const D3DXVECTOR3 &vPos, ioPlayStage *pStage )
{
	if( m_pFireBall && m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->HasLinkedSystem() )
		return;

	SetPosition( vPos );

	m_vMoveDir = GetOwner()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	SetOrientation( GetOwner()->GetTargetRot() );
}

void ioLimitAttachWeapon2::Process( float fTimePerSec, ioPlayStage *pStage )
{
	if( !CheckPiercing() )
		return;

	ioBaseChar *pOwner = GetOwner();
	if( !pOwner )
	{
		UpdateWorldAttackBox( fTimePerSec, true );
		SetWeaponDead();
		return;
	}

	if( !m_CreateBuffName.IsEmpty() )
	{
		if( !pOwner->HasBuff( m_CreateBuffName ) )
		{
			UpdateWorldAttackBox( fTimePerSec, true );
			WeaponEndEffect( pStage );
			SetWeaponDead();
			return;
		}
	}
	else
	{
		if( m_dwCreatedTime + m_dwLiveTime < FRAMEGETTIME() )
		{
			UpdateWorldAttackBox( fTimePerSec, true );
			WeaponEndEffect( pStage );
			SetWeaponDead();
			return;
		}
	}

	UpdateCurPosition( pOwner );

	m_vMoveDir = GetOwner()->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	SetOrientation( GetOwner()->GetTargetRot() );

	UpdateWorldAttackBox( fTimePerSec, false );

	UpdateWeaponEffect( pOwner );

	if( m_pFireBall )
	{
		m_pFireBall->Update( fTimePerSec * FLOAT1000 );
	}

	
}

void ioLimitAttachWeapon2::DestroyResource( ioPlayStage *pStage )
{
	if( m_pFireBall )
	{
		g_EffectFactory.DestroyEffect( m_pFireBall );
		m_pFireBall = NULL;
	}

	if( !m_szSkillSoundName.IsEmpty() )
	{
		g_SoundMgr.StopSound( m_szSkillSoundName, m_SkillSoundID );
		//g_SoundMgr.StopSound( m_szSkillSoundName, 0 );
	}

	if( m_dwWeaponEffectStand != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwWeaponEffectStand );
		if( pMapEffect )	pMapEffect->EndEffectForce();
		m_dwWeaponEffectStand = -1;
	}

	if( m_dwWeaponEffectMove != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwWeaponEffectMove );
		if( pMapEffect )	pMapEffect->EndEffectForce();
		m_dwWeaponEffectMove = -1;
	}

	if( m_dwWeaponEffectJump != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwWeaponEffectJump );
		if( pMapEffect )	pMapEffect->EndEffectForce();
		m_dwWeaponEffectJump = -1;
	}

	if( m_dwWeaponEffectEnd != -1 )
	{
		ioMapEffect *pMapEffect = pStage->FindMapEffect( m_dwWeaponEffectEnd );
		if( pMapEffect )	pMapEffect->EndEffectGrace();
		m_dwWeaponEffectEnd = -1;
	}

}

D3DXVECTOR3 ioLimitAttachWeapon2::GetAttackDir( const ioPlayEntity *pEntity ) const
{
	return ioWeapon::GetAttackDir(pEntity);
}

const ioOrientBox& ioLimitAttachWeapon2::GetLocalAttackBox() const
{
	if( m_pFireBall )
		return m_pFireBall->GetCollisionBox();

	return ioWeapon::GetLocalAttackBox();
}

ioWeapon::WeaponType ioLimitAttachWeapon2::GetType() const
{
	return WT_LIMIT_ATTACH2;
}

ActionStopType ioLimitAttachWeapon2::GetActionStopType() const
{
	return m_ActionStopType;
}

ActionStopType ioLimitAttachWeapon2::GetDieActionStopType() const
{
	return m_DieActionStopType;
}

void ioLimitAttachWeapon2::UpdateWorldAttackBox( float fTimePerSec, bool bSetNull )
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

const WeaponAttribute* ioLimitAttachWeapon2::GetAttribute() const
{
	if( m_bResistance )
	{
		if( m_pResistanceAttr )
			return m_pResistanceAttr;
	}

	return m_pConstAttr;
}

bool ioLimitAttachWeapon2::IsCollision( ioPlayEntity *pEntity, bool bMapCollision )
{
	bool bCollision = false;

	ioBaseChar* pTarget = ToBaseChar( pEntity );
	if( pTarget && !m_szReduceColByBuffName.IsEmpty() )
	{
		if( pTarget->HasBuff(m_szReduceColByBuffName) )
			return false;
	}

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	if( pEntity->IsWoundedCollision( GetWorldAttackBox(bMapCollision), &vColPoint ) )
	{
		bCollision = true;
	}

	if( bCollision )
	{
		if( m_bIgnoreColOnDelay && IsDelayStateOnBuff() )	bCollision = false;
		if( m_bIgnoreColOnMove && IsMoveStateOnBuff() )		bCollision = false;
		if( m_bIgnoreColOnJump && IsJumpStateOnBuff() )		bCollision = false;
	}

	CheckPiercing( bCollision );

	return bCollision;
}

void ioLimitAttachWeapon2::UpdateCurPosition( ioBaseChar *pOwner )
{
	if( !pOwner || !m_pFireBall ) return;

	ioHashString szLinkBiped;
	D3DXVECTOR3 vLinkPosOffset;
	D3DXQUATERNION qtLinkRotOffset;

	if( m_pFireBall->GetSkeletonName().IsEmpty() && m_pFireBall->GetFirstLinkOffset(szLinkBiped, vLinkPosOffset, qtLinkRotOffset ) )
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
	else
	{
		SetPosition( pOwner->GetWorldPosition() );
	}
}

void ioLimitAttachWeapon2::UpdateWeaponEffect( ioBaseChar *pOwner )
{
	if( !pOwner )	return;
	if( m_szWeaponEffectStand.IsEmpty() || m_szWeaponEffectMove.IsEmpty() || m_szWeaponEffectJump.IsEmpty() )	return;

	ioPlayStage* pStage = pOwner->GetCreator();

	if( pStage )
	{
		D3DXVECTOR3 vScale = pOwner->GetWorldScale();

		if( IsDelayStateOnBuff() )
		{
			if( m_dwWeaponEffectStand == -1 )
			{
				ioMapEffect* pWeaponStandEffect = NULL;
				pWeaponStandEffect = pOwner->CreateMapEffectBySkill( m_szWeaponEffectStand, GetPosition(), vScale );
				if( pWeaponStandEffect )	m_dwWeaponEffectStand = pWeaponStandEffect->GetUniqueID();
			}
			else
			{
				ioMapEffect *pWeaponStandEffect = pStage->FindMapEffect( m_dwWeaponEffectStand );
				if( pWeaponStandEffect )
				{
					pWeaponStandEffect->SetWorldPosition( GetPosition() );
					ioEffect *pEffect = pWeaponStandEffect->GetEffect();

					if( pEffect && m_vPreScale != vScale )
						pEffect->GetParentNode()->SetScale(vScale);
				}
					
			}

			if( m_dwWeaponEffectMove != -1 )
			{
				ioMapEffect *pWeaponMoveEffect = pStage->FindMapEffect( m_dwWeaponEffectMove );
				if( pWeaponMoveEffect )	pWeaponMoveEffect->EndEffectForce();
				m_dwWeaponEffectMove = -1;
			}

			if( m_dwWeaponEffectJump != -1 )
			{
				ioMapEffect *pWeaponJumpEffect = pStage->FindMapEffect( m_dwWeaponEffectJump );
				if( pWeaponJumpEffect )	pWeaponJumpEffect->EndEffectForce();
				m_dwWeaponEffectJump = -1;
			}
		}
		else if( IsMoveStateOnBuff() )
		{
			if( m_dwWeaponEffectStand != -1 )
			{
				ioMapEffect *pWeaponStandEffect = pStage->FindMapEffect( m_dwWeaponEffectStand );
				if( pWeaponStandEffect )	pWeaponStandEffect->EndEffectForce();
				m_dwWeaponEffectStand = -1;
			}

			if( m_dwWeaponEffectMove == -1 )
			{
				ioMapEffect* pWeaponMoveEffect = NULL;
				pWeaponMoveEffect = pOwner->CreateMapEffectBySkill( m_szWeaponEffectMove, GetPosition(), vScale );
				if( pWeaponMoveEffect )	m_dwWeaponEffectMove = pWeaponMoveEffect->GetUniqueID();
			}
			else
			{
				ioMapEffect *pWeaponMoveEffect = pStage->FindMapEffect( m_dwWeaponEffectMove );
				if( pWeaponMoveEffect )
				{
					pWeaponMoveEffect->SetWorldPosition( GetPosition() );
					
					ioEffect *pEffect = pWeaponMoveEffect->GetEffect();

					if( pEffect && m_vPreScale != vScale )
						pEffect->GetParentNode()->SetScale(vScale);
				}
			}

			if( m_dwWeaponEffectJump != -1 )
			{
				ioMapEffect *pWeaponJumpEffect = pStage->FindMapEffect( m_dwWeaponEffectJump );
				if( pWeaponJumpEffect )	pWeaponJumpEffect->EndEffectForce();
				m_dwWeaponEffectJump = -1;
			}
		}
		else if( IsJumpStateOnBuff() )
		{
			if( m_dwWeaponEffectStand != -1 )
			{
				ioMapEffect *pWeaponStandEffect = pStage->FindMapEffect( m_dwWeaponEffectStand );
				if( pWeaponStandEffect )	pWeaponStandEffect->EndEffectForce();
				m_dwWeaponEffectStand = -1;
			}

			if( m_dwWeaponEffectMove != -1 )
			{
				ioMapEffect *pWeaponMoveEffect = pStage->FindMapEffect( m_dwWeaponEffectMove );
				if( pWeaponMoveEffect )	pWeaponMoveEffect->EndEffectForce();
				m_dwWeaponEffectMove = -1;
			}

			if( m_dwWeaponEffectJump == -1 )
			{
				ioMapEffect* pWeaponJumpEffect = NULL;
				pWeaponJumpEffect = pOwner->CreateMapEffectBySkill( m_szWeaponEffectJump, GetPosition(), vScale );
				if( pWeaponJumpEffect )	m_dwWeaponEffectJump = pWeaponJumpEffect->GetUniqueID();
			}
			else
			{
				ioMapEffect *pWeaponJumpEffect = pStage->FindMapEffect( m_dwWeaponEffectJump );
				if( pWeaponJumpEffect )
				{
					pWeaponJumpEffect->SetWorldPosition( GetPosition() );

					ioEffect *pEffect = pWeaponJumpEffect->GetEffect();

					if( pEffect && m_vPreScale != vScale  )
						pEffect->GetParentNode()->SetScale(vScale);
				}
			}
		}

		ioMapEffect *pWeaponEffect = NULL;
		if( m_dwWeaponEffectStand != -1 )		pWeaponEffect = pStage->FindMapEffect( m_dwWeaponEffectStand );
		else if( m_dwWeaponEffectMove != -1 )	pWeaponEffect = pStage->FindMapEffect( m_dwWeaponEffectMove );
		else if( m_dwWeaponEffectJump != -1 )	pWeaponEffect = pStage->FindMapEffect( m_dwWeaponEffectJump );
		if( pWeaponEffect )
		{
			D3DXQUATERNION qtCharRot;
			D3DXVECTOR3 m_vAttackDir = pOwner->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, FLOAT1);
			ioMath::CalcDirectionQuaternion( qtCharRot, m_vAttackDir );			
			pWeaponEffect->SetWorldOrientation( qtCharRot );
		}
		
		if( m_vPreScale != vScale )	m_vPreScale = vScale;
	}
}

bool ioLimitAttachWeapon2::IsJumpStateOnBuff()
{
	ioBaseChar* pOwner = GetOwner();
	if( pOwner && pOwner->GetState() == CS_JUMP )	return true;
	return false;
}

bool ioLimitAttachWeapon2::IsMoveStateOnBuff()
{
	ioBaseChar* pOwner = GetOwner();
	if( pOwner && pOwner->GetState() == CS_RUN )	return true;
	return false;
}

bool ioLimitAttachWeapon2::IsDelayStateOnBuff()
{
	if( !IsJumpStateOnBuff() && !IsMoveStateOnBuff() )	return true;
	return false;
}

void ioLimitAttachWeapon2::WeaponEndEffect( ioPlayStage *pStage )
{
	if( !pStage )	return;
	if( m_szWeaponEffectEnd.IsEmpty() )	return;

	if( m_dwWeaponEffectEnd == -1 )
	{
		D3DXVECTOR3 vScale = D3DXVECTOR3( FLOAT1, FLOAT1, FLOAT1 );

		ioMapEffect* pWeaponEffect = NULL;
		pWeaponEffect = pStage->CreateMapEffect( m_szWeaponEffectEnd, GetPosition(), vScale );
		if( pWeaponEffect )	m_dwWeaponEffectEnd = pWeaponEffect->GetUniqueID();
	}
}