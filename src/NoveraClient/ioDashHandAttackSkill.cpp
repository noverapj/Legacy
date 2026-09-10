

#include "stdafx.h"

#include "ioBaseChar.h"
#include "ioPlayStage.h"


#include "ioDashHandAttackSkill.h"

#include "WeaponDefine.h"

ioDashHandAttackSkill::ioDashHandAttackSkill()
{
	InitSkillData();
}

ioDashHandAttackSkill::ioDashHandAttackSkill( const ioDashHandAttackSkill &rhs )
: ioAttackSkill( rhs ),
m_ContactRangeAttackInfoToChar( rhs.m_ContactRangeAttackInfoToChar ),
m_ContactRangeAttackInfoToWall( rhs.m_ContactRangeAttackInfoToWall ),
m_dwFireTime( rhs.m_dwFireTime ),
m_SkillMapEffect( rhs.m_SkillMapEffect ),
m_fCharEffectOffset( rhs.m_fCharEffectOffset ),
m_fWallEffectOffset( rhs.m_fWallEffectOffset ),
m_FireSound( rhs.m_FireSound ),
m_WallCollsionSound( rhs.m_WallCollsionSound )
{
	InitSkillData();
}

ioDashHandAttackSkill::~ioDashHandAttackSkill()
{
}

void ioDashHandAttackSkill::InitSkillData()
{
	m_State = SS_NONE;
	m_FireType = FT_NORMAL;

	m_bSetHandMesh = false;

	m_dwFireStartTime = 0;
	m_dwSkillMapEffect = -1;

	m_bOneTimeCharFire = false;
	m_bOneTimeWallFire = false;
	
	m_dwFireSoundID = 0;

	m_szCollsionIgoreCharList.clear();
}

void ioDashHandAttackSkill::LoadProperty( ioINILoader &rkLoader )
{
	ioAttackSkill::LoadProperty( rkLoader );
	
	m_dwFireTime = rkLoader.LoadInt_e( "contact_range_attack_fire_time", 100 );

	char szBuf[MAX_PATH];
		
	m_ContactRangeAttackInfoToChar.m_iWeaponIdx = 
		rkLoader.LoadInt_e( "contact_range_attack_char", NO_WEAPON_ATTRIBUTE );
	
	m_ContactRangeAttackInfoToChar.m_iResistanceIdx =
		rkLoader.LoadInt_e( "contact_range_attack_char_resist", NO_WEAPON_ATTRIBUTE );

	rkLoader.LoadString_e( "contact_range_attack_char_wounded", "", szBuf, MAX_PATH );
	m_ContactRangeAttackInfoToChar.m_WoundedAnimation = szBuf;
	
	m_ContactRangeAttackInfoToChar.m_dwWoundedDuration = 
		rkLoader.LoadInt_e( "contact_range_attack_char_wounded_time", 0 );
	
	m_ContactRangeAttackInfoToChar.m_bWoundedLoopAni = 
		rkLoader.LoadBool_e( "contact_range_attack_char_wounded_loop_ani", false );

	
	m_ContactRangeAttackInfoToWall.m_iWeaponIdx = 
		rkLoader.LoadInt_e( "contact_range_attack_wall", NO_WEAPON_ATTRIBUTE );

	m_ContactRangeAttackInfoToWall.m_iResistanceIdx =
		rkLoader.LoadInt_e( "contact_range_attack_wall_resist", NO_WEAPON_ATTRIBUTE );

	rkLoader.LoadString_e( "contact_range_attack_wall_wounded", "", szBuf, MAX_PATH );
	m_ContactRangeAttackInfoToWall.m_WoundedAnimation = szBuf;

	m_ContactRangeAttackInfoToWall.m_dwWoundedDuration = 
		rkLoader.LoadInt_e( "contact_range_attack_wall_wounded_time", 0 );

	m_ContactRangeAttackInfoToWall.m_bWoundedLoopAni = 
		rkLoader.LoadBool_e( "contact_range_attack_wall_wounded_loop_ani", false );
	
	rkLoader.LoadString_e( "skill_map_effect", "", szBuf, MAX_PATH );
	m_SkillMapEffect = szBuf;

	rkLoader.LoadString_e( "waepon_fire_sound", "", szBuf, MAX_PATH );
	m_FireSound = szBuf;

	rkLoader.LoadString_e( "waepon_wall_collision_sound", "", szBuf, MAX_PATH );
	m_WallCollsionSound = szBuf;

	m_fCharEffectOffset = rkLoader.LoadFloat_e( "contact_range_char_effect_offset", 0.0f );
	m_fWallEffectOffset = rkLoader.LoadFloat_e( "contact_range_wall_effect_offset", 0.0f );
}

ioSkill* ioDashHandAttackSkill::Clone()
{
	return new ioDashHandAttackSkill( *this );
}

bool ioDashHandAttackSkill::OnSkillStart( ioBaseChar *pChar )
{	
	if( !ioAttackSkill::OnSkillStart( pChar ) )
		return false;

	InitSkillData();

	if( m_dwSwitchSkill > 0 )
		m_dwEnableSwitchTime = FRAMEGETTIME() + m_dwSwitchSkill;
	if( m_dwSemiSwitchSkill > 0 )
		m_dwEnableSemiSwitchTime = FRAMEGETTIME() + m_dwSemiSwitchSkill;

	m_dwSkillStartTime = FRAMEGETTIME();

	pChar->AttachEffect( m_SkillEffectName );

	// 기본 무기 손에서 제거
	if( m_bUseItemMesh )
	{			
		pChar->ShowWeaponItemMesh( false );
	}

	pChar->SetSkillProtection();

	CheckSkillDesc( pChar );

	if( m_bInitJumpPower )
	{
		pChar->SetCurJumpPowerAmt( 0.0f );
		pChar->SetGravityAmt( 0.0f );
	}

	if( !StartPreDelayAnimation( pChar ) )	// Delay Animation이 없다
	{			
		AttackSkillFire(pChar);

	}

	return true;
}

void ioDashHandAttackSkill::OnSkillEnd( ioBaseChar *pChar )
{
	ioAttackSkill::OnSkillEnd( pChar );

	InitSkillData();

	// 기본무기 장착
	if( m_bUseItemMesh )
	{
		pChar->ShowWeaponItemMesh( true );
	}

	// 스킬 중간 동작에 사용된 메쉬들 제거	
	if( !m_HandMesh.IsEmpty() )
		pChar->RestoreSkillWeaponMesh( this, m_HandMesh, m_bVisibleEquipMesh );	
	
	ioPlayStage *pStage = g_SkillMaker.GetPlayStage();

	if( m_dwSkillMapEffect != -1 )
	{
		ioMapEffect* pMapEffect = pStage->FindMapEffect( m_dwSkillMapEffect );
		if( pMapEffect )	
		{				
			pStage->DestroyMapEffect( m_dwSkillMapEffect );
		}

		m_dwSkillMapEffect = -1;
	}
}

void ioDashHandAttackSkill::OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage )
{
	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE :
		{
			DWORD dwPreDelay = GetPreDelayTime();
			if( m_dwPreDelayStartTime + dwPreDelay < dwCurTime )
			{
				m_dwPreDelayStartTime = 0;
				AttackSkillFire(pChar);
			}
		}
		break;
		
	case SS_ACTION:
		{	
			if( 0 < m_dwFireStartTime && m_dwFireStartTime < dwCurTime )
			{	

				D3DXVECTOR3 vScale = pChar->GetWorldScale();
				D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3(0.0f, 0.0f, -FLOAT1);
				D3DXVec3Normalize( &vDir, &vDir );

				D3DXQUATERNION qtCharRot;
				ioMath::CalcDirectionQuaternion( qtCharRot, -vDir );
				
				D3DXVECTOR3 vPos = pChar->GetWorldPosition();
	

				switch( m_FireType )
				{
				case FT_NORMAL:
					{	
						if( m_bOneTimeCharFire )
							break;

						vPos += m_fCharEffectOffset * vDir;
						m_bOneTimeCharFire = true;

						pChar->SkillFireExplicit( m_ContactRangeAttackInfoToChar,
												  GetName(),
												  m_dwWeaponBaseIndex+1,
												  pChar->GetMidPositionByRate() );

						if( !m_FireSound.IsEmpty() && pChar )
						{
							ioEntityGroup *pGrp = pChar->GetGroup();
							m_dwFireSoundID = g_SoundMgr.PlaySound( m_FireSound, pGrp->GetParentSceneNode() );			
						}
						
						if( !m_SkillMapEffect.IsEmpty() )
						{
							
							ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_SkillMapEffect, vPos, vScale );

							if( pMapEffect )
							{
								m_dwSkillMapEffect = pMapEffect->GetUniqueID();
								pMapEffect->SetWorldOrientation( qtCharRot );
							}
						}
					}
				break;
				case FT_WALL:
					{	
						if( m_bOneTimeWallFire ) 
							break;

						vPos += m_fWallEffectOffset * vDir;
						m_bOneTimeWallFire = true;

						pChar->SkillFireExplicit( m_ContactRangeAttackInfoToWall,
												  GetName(),
												  m_dwWeaponBaseIndex+1,
												  pChar->GetMidPositionByRate() );

						if( !m_WallCollsionSound.IsEmpty() )
						{
							ioEntityGroup *pGrp = pChar->GetGroup();
							m_dwWallCollsionSoundID = g_SoundMgr.PlaySound( m_WallCollsionSound, pGrp->GetParentSceneNode() );						
						}

						if( !m_SkillMapEffect.IsEmpty() )
						{

							ioMapEffect* pMapEffect = pStage->CreateMapEffect( m_SkillMapEffect, vPos, vScale );

							if( pMapEffect )
							{
								m_dwSkillMapEffect = pMapEffect->GetUniqueID();			
								pMapEffect->SetWorldOrientation( qtCharRot );
							}
						}
					}
				break;
				}

				m_dwFireStartTime = 0;
			}			
		}
		break;
	}
}

void ioDashHandAttackSkill::AttackSkillFire( ioBaseChar *pChar )
{	
	if( !pChar ) return;
		
	ioAttackSkill::AttackSkillFire(pChar);
	m_State = SS_ACTION;

	ioEntityGroup *pGrp = pChar->GetGroup();

	int iAniID = pGrp->GetAnimationIdx( m_AttackAttribute.m_AttackAnimation );
	if( iAniID == -1 )	return;

	float fAniRate = m_AttackAttribute.m_fAttackAniRate;
	if( fAniRate <= 0.0f )
		fAniRate = FLOAT1;

	DWORD dwCurTime = FRAMEGETTIME();
}

void ioDashHandAttackSkill::SetInfoByApplyWeapon( ioBaseChar *pOwner, ioBaseChar *pTarget, DWORD dwWeaponBaseIndex, bool bBlock )
{
	if( !pOwner )	return;
	if( !pTarget )	return;
	if( pOwner->GetTeam() == pTarget->GetTeam() ) return;

	DWORD dwCurTime = FRAMEGETTIME();

	if( !m_szCollsionIgoreCharList.empty() )			
	{
		m_szCollsionIgoreCharList.push_back( pTarget->GetCharName() );
	}
	else
	{
		if( m_dwFireStartTime == 0 )
		{
			m_dwFireStartTime = dwCurTime;
			m_dwFireStartTime += m_dwFireTime;

			m_dwMotionEndTime += m_dwFireTime;
			m_FireType = FT_NORMAL;

			m_szCollsionIgoreCharList.push_back( pTarget->GetCharName() );
		}	
	}
}

void ioDashHandAttackSkill::SetCollisionByApplySkill()
{	
	if( m_dwFireStartTime == 0 )
	{
		DWORD dwCurTime = FRAMEGETTIME();

		m_dwFireStartTime = dwCurTime;
		m_dwFireStartTime += m_dwFireTime;

		m_dwMotionEndTime += m_dwFireTime;
		m_FireType = FT_WALL;
	}
	
}


void ioDashHandAttackSkill::ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage )
{	
	ioSkill::ApplyAttributeToWeapon( pWeapon, pStage );

	ioZoneEffectWeapon *pZone = ToZoneEffectWeapon( pWeapon );
	if( pZone )
	{		
		ioBaseChar *pChar = pZone->GetOwner();
		if( pChar )
		{
			D3DXVECTOR3 vDir = pChar->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
			pZone->SetCollisionDir( vDir );
			pZone->SetAttackDir( vDir );
			pZone->SetZoneValue( false, false );
			
			if( !m_szCollsionIgoreCharList.empty() )
			{				
				ioHashStringVec::iterator iter = m_szCollsionIgoreCharList.begin();
				for( ; iter != m_szCollsionIgoreCharList.end(); ++iter )
				{	
					ioHashString szName = (*iter);
					if( !szName.IsEmpty() )
					{							
						pZone->SetIgnoreCollsionChar( szName );
					}
				}
			}
		}
	}
}

bool ioDashHandAttackSkill::IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const
{	
	if( m_dwPreDelayStartTime > 0 )
		return false;

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_NONE:
		return false;
		break;

	case SS_ACTION:
		if( m_dwMotionEndTime == 0 )		
			return true;		

		if( 0 < m_dwMotionEndTime &&  m_dwMotionEndTime < dwCurTime )
		{	
			return true;
		}

		break;
	}

	return false;
}

bool ioDashHandAttackSkill::IsProtected( int iDefenseBreakType ) const
{	

	if( m_bDisableSkillProtect )
		return false;

	if( DBT_NONE != (DefenseBreakType)m_ProtectCancelType )
	{		
		if( iDefenseBreakType == m_ProtectCancelType )   // 스킬 무적 파괴
			return false;
	}

	if( IsPreDelayProtect() )
	{	
		if( m_dwPreDelayStartTime != 0 )
			return true;	
	}

	DWORD dwCurTime = FRAMEGETTIME();

	switch( m_State )
	{
	case SS_ACTION:
		if( 0 < m_dwFireMotionEndTime && dwCurTime <= m_dwFireMotionEndTime )
			return true;
	}

	return false;
}

bool ioDashHandAttackSkill::CheckUseItemMesh()
{
	return m_bUseItemMesh;
}
