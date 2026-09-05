#include "stdafx.h"

#include "ioSkillMaker.h"

#include "SkillTypeID.h"
#include "SkillDefine.h"

template<> ioSkillMaker* Singleton< ioSkillMaker >::ms_Singleton = 0;

ioSkillMaker::ioSkillMaker()
{
	m_pPlayStage = NULL;
	m_SkillMap.clear();
}

ioSkillMaker::~ioSkillMaker()
{
	m_pPlayStage = NULL;
	ClearSkillList();
}

void ioSkillMaker::ClearSkillList()
{
	SkillMap::iterator iter;
	for( iter=m_SkillMap.begin() ; iter!=m_SkillMap.end() ; ++iter )
	{
		SAFEDELETE(iter->second);
	}
	m_SkillMap.clear();
}

void ioSkillMaker::SetPlayStage( ioPlayStage *pStage )
{
	m_pPlayStage = pStage;
}

ioPlayStage* ioSkillMaker::GetPlayStage()
{
	return m_pPlayStage;
}

void ioSkillMaker::LoadSkillList( const char *szFileName )
{
	ioINILoader kLoader( szFileName );

	char szBuf[MAX_PATH] = "";
	char szGroupName[MAX_PATH] = "";
	char szTitle[MAX_PATH] = "";

	int i = 0;

	kLoader.SetTitle_e( "common_info" );
	int iGroupCnt = kLoader.LoadInt_e( "skill_group_cnt", 0 );

	for( i=0 ; i<iGroupCnt ; i++ )
	{
		// 그룹 호출
		wsprintf_e( szTitle, "skill_group%d", i );
		kLoader.LoadString( szTitle, "", szBuf, MAX_PATH );
		wsprintf_e( szGroupName, "config/%s", szBuf );

		ioINILoader kSetItemLoader( szGroupName );
		kSetItemLoader.SetTitle_e( "common" );

		int iSkillCnt = kSetItemLoader.LoadInt_e( "skill_cnt", 0 );
		for( int j=0; j < iSkillCnt; ++j )
		{
			// 그룹에 속한 스킬 추가
			wsprintf_e( szTitle, "skill%d", j+1 );
			kSetItemLoader.SetTitle( szTitle );

			ioSkill *pSkill = CreateSkillTemplete( kSetItemLoader );
			if( pSkill )
			{
				pSkill->SetGroupName( szGroupName );
				pSkill->SetTitleName( szTitle );
				pSkill->LoadProperty( kSetItemLoader );

				if( m_SkillMap.find( pSkill->GetName() ) == m_SkillMap.end() )
				{
					m_SkillMap.insert( SkillMap::value_type( pSkill->GetName(), pSkill ) );
				}
				else
				{
					LOG.PrintTimeAndLog( 0, "ioSkillMaker::LoadSkillList - %s Already Exist",
											pSkill->GetName().c_str() );

					delete pSkill;
				}
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioSkillMaker::LoadSkillList - Load Fail! %s, %s", szGroupName, szTitle );
			}
		}
	}
}

ioSkill* ioSkillMaker::CreateSkill( const ioHashString &rkName )
{
	SkillMap::iterator iter = m_SkillMap.find( rkName );
	if( iter!=m_SkillMap.end() )
	{
		return iter->second->Clone();
	}

	LOG.PrintTimeAndLog( 0, "ioSkillMaker::CreateSkill - %s Not Exist Skill", rkName.c_str() );
	return NULL;
}

const ioSkill* ioSkillMaker::GetSkillConst( const ioHashString &rkName ) const
{
	SkillMap::const_iterator iter = m_SkillMap.find( rkName );
	if( iter!=m_SkillMap.end() )
	{
		return iter->second;
	}

	LOG.PrintTimeAndLog( 0, "ioSkillMaker::GetSkillConst - %s Not Exist Skill", rkName.c_str() );
	return NULL;
}

bool ioSkillMaker::HasSkill( const ioHashString &rkName )
{
	SkillMap::iterator iter = m_SkillMap.find( rkName );
	if( iter!=m_SkillMap.end() )
		return true;

	return false;
}

ioSkill* ioSkillMaker::CreateSkillTemplete( ioINILoader &rkLoader )
{
	int iTypeID = rkLoader.LoadInt_e( "type", 0 );

	switch( iTypeID )
	{
	case ST_ATTACK:
		return CreateAttackSkillTemplete( rkLoader );
	case ST_BUFF:
		return CreateBuffSkillTemplete( rkLoader );
	case ST_NORMAL:
		return CreateNormalSkillTemplete( rkLoader );
	case ST_PASSIVE:
		return CreatePassiveSkillTemplete( rkLoader );
	case ST_RANGE:
		return CreateRangeSkillTemplete( rkLoader );
	case ST_MULTI:
		return new ioMultiSkill;
	case ST_COUNT:
		return CreateCountSkillTemplete( rkLoader );
	}

	return NULL;
}

ioSkill* ioSkillMaker::CreateAttackSkillTemplete( ioINILoader &rkLoader )
{
	int iSubType = rkLoader.LoadInt_e( "sub_type", 0 );
	
	switch( iSubType )
	{
	case AST_DEFAULT:
		return new ioAttackSkill;
	case AST_COW_RUN:
		return new ioCowRunSkill;
	case AST_BIG_HAMMER:
		return new ioBigHammerSkill;
	case AST_NUCLEAR:
		return new ioNuclearSkill;
	case AST_FLAME_8WAY:
		return new ioFlame8WaySkill;
	case AST_SCATTER_BOMB:
		return new ioScatterBombSkill;
	case AST_FULL_SHOT:
		return new ioFullShotSkill;
	case AST_MINE:
		return new ioMineSkill;
	case AST_METEOR:
		return new ioMeteorSkill;
	case AST_RANDOM_GENERATE:
		return new ioRandomGenerateSkill;
	case AST_THROW:
		return new ioThrowSkill;
	case AST_FIRE:
		return new ioFireSkill;
	case AST_MOVE_ATTACK:
		return new ioMoveAttackSkill;
	case AST_CONSECUTION:
		return new ioConsecutionSkill;
	case AST_ROT_CONSECUTION:
		return new ioRotateConsecutionSkill;
	case AST_CONTACT_CONSECUTION:
		return new ioContactConsecutionSkill;
	case AST_RUNNING_ATTACK:
		return new ioRunningAttackSkill;
	case AST_WHIP_ATTACK:
		return new ioWhipSkill;
	case AST_LINEAR_COMBO:
		return new ioLinearComboSkill;
	case AST_SPIN_CHARGING:
		return new ioSpinChargingSkill;
	case AST_CONTROL_FIRE:
		return new ioControlFireSkill;
	case AST_GRAPPLING_SWING:
		return new ioGrapplingSwingSkill;
	case AST_GRAPPLING_BLOW:
		return new ioGrapplingBlowSkill;
	case AST_DUMMY_CHAR:
		return new ioDummyCharSkill;
	case AST_PSYC_GRAPPLING:
		return new ioPsycGrapplingSkill;
	case AST_TRACE:
		return new ioTraceSkill;
	case AST_BREATH:
		return new ioBreathSkill;
	case AST_COMBOWHIP:
		return new ioComboWhipSkill;
	case AST_GRAPPLING:
		return new ioGrapplingSkill;
	case AST_RUNNING_GRAPPLING:
		return new ioRunningGrapplingSkill;
	case AST_MONSTER_SKILL:
		return new ioMonsterSkill;
	case AST_GRAPPLING2:
		return new ioGrapplingSkill2;
	case AST_RUNNING_GRAPPLING_JUMP:
		return new ioRunningGrapplingJumpSkill;
	case AST_ANGLE_FIRE:
		return new ioAngleFireSkill;
	case AST_COMBO_FORCE_EXPLOSION_FIRE:
		return new ioComboForceExplosionSkill;
	case AST_TELEPORT_GRAPPLING_SWING:
		return new ioTeleportGrapplingSwingSkill;
	case AST_CONTROL_DUMMY:
		return new ioControlDummySkill;
	case AST_AIR_COMBO_ATTACK:
		return new ioAirComboAttackSkill;
	case AST_UROBORUS:
		return new ioUroborusSkill;
	case AST_UROBORUS2:
		return new ioUroborusSkill2;
	case AST_DEFENSE_ATTACK:
		return new ioDefenseAttackSkill;
	case AST_MOVE_ATTACK2:
		return new ioMoveAttackSkill2;
	case AST_TOSS_SKILL:
		return new ioTossSkill;
	case AST_RUNNING_ATTACK2:
		return new ioRunningAttackSkill2;
	case AST_TRACE_WARP:
		return new ioTraceWarpSkill;
	case AST_CONSECUTION2:
		return new ioConsecution2Skill;
	case AST_TARGET_WARP:
		return new ioTargetWarpSkill;
	case AST_GRAPPLING_ATTACK:
		return new ioGrapplingAttackSkill;
	case AST_RUNNING_ATTACK3:
		return new ioRunningAttackSkill3;
	case AST_GRAPPLING_ATTACK2:
		return new ioGrapplingAttack2Skill;
	case AST_GRAPPLING3:
		return new ioGrapplingSkill3;
	case AST_ANGLE_FIRE2:
		return new ioAngleFire2Skill;
	case AST_CHANGE_POSITION:
		return new ioChangePositionSkill;
	case AST_GRAPPLING_ATTACK3:
		return new ioGrapplingAttack3Skill;
	case AST_GRAPPLING_ATTACK4:
		return new ioGrapplingAttack4Skill;
	case AST_MECHANICS_COMBO:
		return new ioMechanicsComboSkill;
	case AST_MECHANICS_FIRE:
		return new ioMechanicsFireSkill;
	case AST_MULTI_TELEPORT:
		return new ioMultiTeleportSkill;
	case AST_DEFENSE_ATTACK2:
		return new ioDefenseAttack2Skill;
	case AST_GUIDE_ATTACK:
		return new ioGuideAttackSkill;
	case AST_CANNON_CHARGE:
		return new ioCannonChargeSkill;
	case AST_THROW_CHAR:
		return new ioThrowCharSkill;
	case AST_DOUBLE_ICE_MISSILE:
		return new ioDoubleIceMissileSkill;
	case AST_FIRE2:
		return new ioFireSkill2;
	case AST_TARGET_CHANGE_GANGSI:
		return new ioTargetGangsiChangeSkill;
	case AST_DASH_HAND_ATTACK:
		return new ioDashHandAttackSkill;
	case AST_BLINDLY_SHOT:
		return new ioBlindlyShotSkill;
	case AST_CONSECUTION3:
		return new ioConsecution3Skill;
	case AST_GRAPPLING4:
		return new ioGrapplingSkill4;
	case AST_TWO_STATE_ATTACK:
		return new ioTwoStateAttack;
	case AST_ANGLE_FIRE3:
		return new ioAngleFire3Skill;
	case AST_MULTI_LIGHT:
		return new ioMultiLightAttack;
	case AST_LASER_CONTROL:
		return new ioLaserControlSkill;
	case AST_ANGLE_FIRE4:
		return new ioAngleFire4Skill;
	case AST_FULL_SHOT2:
		return new ioFullShotSkill2;
	case AST_SPAWN_NPC:
		return new ioNpcSpawnSkill;
	case AST_RUNNING_ATTACK4:
		return new ioRunningAttackSkill4;
	case AST_CHAIN_TRACE:
		return new ioChainTraceSkill;
	case AST_FIRE_WORKS:
		return new ioFireWorksSkill;
	case AST_RUNNING_GRAPPLING2:
		return new ioRunningGrapplingSkill2;
	case AST_CHARGE_LINE_COMBO:
		return new ioChargeLineComboSkill;
	case AST_CONTROL_FIRE2:
		return new ioControlFireSkill2;
	case AST_MOVE_ATTACK3:
		return new ioMoveAttackSkill3;
	case AST_FIRE3:
		return new ioFireSkill3;
	case AST_MOSESS:
		return new ioMosesSkill;
	case AST_LIGHT_ATTACK:
		return new ioLightAttack;
	case AST_SELECT_TRIPLE_WEAPON:
		return new ioSelectTripleWeaponSkill;
	case AST_AREA_CHECK_BUFF:
		return new ioAreaCheckBuffSkill;
	case AST_CRECENDO:
		return new ioCrecendoSkill;
	case AST_CHARGING_FIRE7:
		return new ioChargingFireSkill7;
	case AST_PROTECT_COUNT_AREA_ATTACK:
		return new ioProtectCountAreaAttackSkill;
	case AST_RUNNING_ATTACK5:
		return new ioRunningAttackSkill5;
	case AST_CONTROL_DUMMY2:
		return new ioControlDummySkill2;
	case AST_JUMPING_FIRE:
		return new ioJumpingFireSkill;
	case AST_RUNNING_ATTACK6:
		return new ioRunningAttackSkill6;
	case AST_RUNNING_GRAPPLING3:
		return new ioRunningGrapplingSkill3;
	case AST_CREATE_DUMMY:
		return new ioCreateDummySkill;
	case AST_CONTROL_FIRE3:
		return new ioControlFireSkill3;
	case AST_FIGURESKATING:
		return new ioFigureskatingSkill;
	case AST_GRAPPLING_SWING2:
		return new ioGrapplingSwing2Skill;
	case AST_DUMMY_CHAR2:
		return new ioDummyCharSkill2;
	case AST_RUNNING_ATTACK7:
		return new ioRunningAttackSkill7;
	case AST_CONSECUTION4:
		return new ioConsecution4Skill;
	case AST_GRAPPLING_BLOW_LOOP:
		return new ioGrapplingBlowLoopSkill;
	case AST_TRACE3:
		return new ioTraceSkill3;
	case AST_RANDOM_EFFECT_EXPLOSION:
		return new ioRandomEffectExplosionSkill;
	case AST_COMBO_JUMP_ATTACK:
		return new ioComboJumpAttackSkill;
	case AST_DUMMY_GRAPPLING_ATTACK:
		return new ioDummyGrapplingAttackSkill;
	case AST_REPEAT_MOVE_ATTACK:
		return new ioRepeatMoveAttack;
	case AST_MYSTERY_TARGET_POS_CHANGE:
		return new ioMysteryTargetPosChnageSkill;
	case AST_DASH_MOVE_CHARGE_ATTACK:
		return new ioDashMoveChargeAttack;
	case AST_RANDOM_GENERATE2:
		return new ioRandomGenerate2Skill;
	case AST_JUMP_AREA_ATTACK:
		return new ioJumpAreaAttackSkill;
	case AST_TARGET_MOVE_COMBO_ATTACK:
		return new ioTargetMoveComboAttackSkill;
	case AST_REPEAT_TELEPORT_ATTACK:
		return new ioRepeatTeleportAttack;
	case AST_REPEAT_MOVE_ATTACK2:
		return new ioRepeatMoveAttack2;
	case AST_JUMPING_FIRE2:
		return new ioJumpingFire2Skill;
	case AST_KEY_WAIT_SKILL:
		return new ioKeyWaitAttackSKill;
	case AST_CREAT_WEPON_BY_INPUT_KEY:
		return new ioCreatWeaponByKeyInputAttackSkill;
	case AST_CHARGING_FIRE8:
		return new ioChargingFireSkill8;
	case AST_REPEAT_TELEPORT_ATTACK2:
		return new ioRepeatTeleportAttack2;
	case AST_RUNNING_ATTACK8:
		return new ioRunningAttackSkill8;
	case AST_TARGET_FLOW_ATTACK:
		return new ioTargetFlowAttackSkill;
	case AST_WHIRLWND_ATTACK:
		return new ioWhirlwndAttack;
	case AST_LINEAR_COMBO2:
		return new ioLinearComboSkill2;
	case AST_GRAPPLING_BLOW_LOOP2:
		return new ioGrapplingBlowLoopSkill2;
	case AST_RUNNING_ATTACK9:
		return new ioRunningAttackSkill9;
	case AST_RUNNING_ATTACK10:
		return new ioRunningAttackSkill10;
	case AST_CIRCLE_RESERVE:
		return new ioCircleReserveAttackSkill;
	case AST_CREATE_WEAPON_ATTACK:
		return new ioCreateWeaponAttackSkill;
	case AST_TARGET_TIMING_ATTACK:
		return new ioTargetTimingAttackSkill;
	case AST_FIRE_AND_LINER:
		return new ioFireAndLinearSkill;
	case AST_SPACE_MASTER_ATTACK:
		return new ioSpaceMasterAttack;
	case AST_REPEAT_LOOP_ATTACK:
		return new ioRepeatLoopAttack;
	case AST_TARGET_REPEAT_ATTACK:
		return new ioTargetRepeatAttack;
	case AST_RANDOM_EXPLOSION:
		return new ioRandomExplosionSkill;
	case AST_TOIST_MAGIC_ATTACK:
		return new ioToistMagicAttack;
	case AST_JUMP_ATTACK:
		return new ioJumpFireSkill;
	case AST_REPEAT_LOOP_ATTACK2:
		return new ioRepeatLoopAttack2;
	case AST_TARGET_TELEPORT_ATTACK:
		return new ioTargetTeleportAttackSkill;
	case AST_CONTROL_FIRE4:
		return new ioControlFireSkill4;
	case AST_MULTIPLE_ELEMENT_ATTACK:
		return new ioMultipleElementAttackSkill;
	case AST_CONTROL_FIRE5:
		return new ioControlFireSkill5;
	case AST_GRAPPLING_ATTACK5:
		return new ioGrapplingAttack5Skill;
	case AST_RANDOM_GENERATE_MOVE:
		return new ioRandomGenerateMoveSkill;
	case AST_FIRE_AND_BUFF_CHECK:
		return new ioFireAndBuffCheckSkill;
	case AST_CONSECUTION5:
		return new ioConsecution5Skill;
	case AST_RUNNING_ATTACK11:
		return new ioRunningAttackSkill11;
	case AST_CIRCLE_MINE:
		return new ioCircleMineSkill;
	case AST_LINEAR_COMBO3:
		return new ioLinearComboSkill3;
	case AST_ROTATE_DIR_KEY:
		return new ioRotateDirKeyAttackSkill;
	case AST_TARGET_THROW_WEAPON:
		return new ioTargetThrowWeaponSkill;
	case AST_EFFECT_MOVE_ATTACK:
		return new ioEffectMoveAttackSkill;
	case AST_FURIOUS_TELEPORT_ATTACK:
		return new ioFuriousTeleportAttack;
	case AST_CREATE_TIC_WEAPON_ATTACK:
		return new ioCreateTicWeaponSkill;
	case AST_BINDING_FIRE_ATTACK:
		return new ioBindingFireAttackSkill;
	case AST_COMBO_AND_GRAPPLING_ATTACK:
		return new ioComboAndGrapplingAttackSkill;
	case AST_PSYC_GRAPPLING2:
		return new ioPsycGrapplingSkill2;
	case AST_CONTROL_DUMMY3:
		return new ioControlDummySkill3;
	case AST_LASER:
		return new ioLaserSkill;
	case AST_MOVE_ATTACK4:
		return new ioMoveAttackSkill4;
	case AST_ANGLE_FIRE5:
		return new ioAngleFire5Skill;
	case AST_CONTROL_FIRE6:
		return new ioControlFireSkill6;
	case AST_UROBORUS3:
		return new ioUroborusSkill3;
	case AST_TARGET_AIMING_ATTACK:
		return new ioTargetAimingAttackSkill;
	case AST_CHARGING_COMBO_ATTACK:				// 2020-05-20 권왕 무공서
		return new ioChargingComboSkill;
	case AST_GRAPPLING5:
		return new ioGrapplingSkill5;
	case AST_EVA_FIRE:
		return new ioEvaFireSkill;				// 2020-11-04 이브 풀 버스트
	}

	LOG.PrintTimeAndLog( 0, "ioSkillMaker::CreateAttackSkillTemplete - %d UnknownType", iSubType );
	return NULL;
}

ioSkill* ioSkillMaker::CreateBuffSkillTemplete( ioINILoader &rkLoader )
{
	int iSubType = rkLoader.LoadInt_e( "sub_type", 0 );
	switch( iSubType )
	{
	case BST_DEFAULT:
		return new ioBuffSkill;
	case BST_AUTO:
		return new ioAutoBuffSkill;
	case BST_CREATE_ITEM: //JSM. 동기화 문제 및 오브젝트 생성과 해제를 전부 TCP로 변경함에 따라 일부 문제가 있어 해당 스킬을 블럭
		return NULL;
	case BST_MULTI_ACTIVE:
		return new ioMultiActiveBuffSkill;
	case BST_TARGET:
		return new ioTargetBuffSkill;
	case BST_MULTI_ACTIVE2:
		return new ioMultiActiveBuffSkill2;
	case BST_TEAM_TARGET:
		return new ioTeamTargetBuffSkill;
	case BST_RELOAD:
		return new ioReloadBuffSkill;
	case BST_JUMP:
		return new ioJumpBuffSkill;
	case BST_CHAR_STATE:
		return new ioCharStateBuffSkill;
	case BST_MAGNETIC:
		return new ioMagneticBuffSkill;				// 2020-12-08 마그 네틱 포스 
	}

	LOG.PrintTimeAndLog( 0, "ioSkillMaker::CreateBuffSkillTemplete - %d UnknownType", iSubType );
	return NULL;
}

ioSkill* ioSkillMaker::CreateNormalSkillTemplete( ioINILoader &rkLoader )
{
	int iNormalType = rkLoader.LoadInt_e( "sub_type", 0 );
	switch( iNormalType )
	{
	case NST_BILDING:
		return new ioBildingSkill;
	case NST_TELEPORT:
		return new ioTeleportSkill;
	case NST_DODGE:
		return new ioCowDodgeSkill;
	case NST_OBJECT:
		return new ioObjectItemSkill;
	case NST_AREA_STRUCT:
		return new ioAreaStructSkill;
	case NST_AREA_CONTROL:
		return new ioAreaWeaponControlSkill;
	case NST_STATE_ESCAPE:
		return new ioStateEscapeSkill;
	case NST_ABSORB:
		return new ioAbsorbSkill;
	case NST_CREEP:
		return new ioCreepSkill;
	case NST_CIRCLE_MOVE:
		return new ioCircleMoveSkill;
	case NST_CIRCLE_MOVE_ATTACK:
		return new ioCircleMoveAttackSkill;
	case NST_RELOAD:
		return new ioReloadSkill;
	case NST_WEAPON_ACTION:
		return new ioWeaponActionSkill;
	case NST_CIRCLE_MOVE_WEAPON:
		return new ioCircleMoveWeaponSkill;
	case NST_LINEAR_FLOAT_WEAPON:
		return new ioLinearFloatWeaponSkill;
	case NST_TARGET_MOVE_ATTACK:
		return new ioTargetMoveAttackSkill;
	case NST_COMBO_MOVE_ATTACK:
		return new ioComboMoveAttackSkill;
	case NST_CHARGING_FIRE:
		return new ioChargingFireSkill;
	case NST_CHARGING_FIRE2:
		return new ioChargingFireSkill2;
	case NST_CIRCLE_MOVE_KEY:
		return new ioCircleMoveAndKeySkill;
	case NST_MULTI_AREA:
		return new ioMultiAreaSkill;
	case NST_CIRCLE_MOVE_ATTACK2:
		return new ioCircleMoveAttackSkill2;
	case NST_CHARGING_FIRE3:
		return new ioChargingFireSkill3;
	case NST_MULTI_DUMMYCHAR:
		return new ioMultiDummyCharSkill;
	case NST_COMBO_ATTACK:
		return new ioComboAttackSkill;
	case NST_TARGET_MISSILE:
		return new ioTargetMissileSkill;
	case NST_WEAPON_CONTROL:
		return new ioWeaponControlSkill;
	case NST_NORMAL_FIRE:
		return new ioNormalFireSkill;
	case NST_CHARGING_FIRE4:
		return new ioChargingFireSkill4;
	case NST_CHARGING_FIRE5:
		return new ioChargingFireSkill5;
	case NST_AREA_STRUCT2:
		return new ioAreaStruct2Skill;
	case NST_CIRCLE_MOVE2:
		return new ioCircleMove2Skill;
	case NST_MULTI_AREA2:
		return new ioMultiArea2Skill;
	case NST_CIRCLE_MOVE_WEAPON2:
		return new ioCircleMoveWeapon2Skill;
	case NST_FLYING_TELEPORT:
		return new ioFlyingTeleportSkill;
	case NST_CIRCLE_MOVE3:
		return new ioCircleMove3Skill;
	case NST_TELEPORT_ATTACK:
		return new ioTeleportAttackSkill;
	case NST_TARGET_DUMMY_CHAR:
		return new ioTargetDummyCharSkill;
	case NST_CIRCLE_MOVE_ATTACK3:
		return new ioCircleMoveAttackSkill3;
	case NST_COMBO_MOVE_ATTACK2:
		return new ioComboMoveAttack2Skill;
	case NST_TARGET_TELEPORT:
		return new ioTargetTeleportSkill;
	case NST_COMBO_MOVE_ATTACK3:
		return new ioComboMoveAttack3Skill;
	case NST_MULTI_AREA3:
		return new ioMultiArea3Skill;
	case NST_MULTIPLE_OBJECT:
		return new ioMultipleObjectItemSkill;
	case NST_FLASH_TRANSFORMATION:
		return new ioFlashTransformationAttackSkill;
	case NST_TARGET_DEBUFF:
		return new ioTargetDebuffSkill;
	case NST_TWO_CIRCLE:
		return new ioTwoMagicCircleSkill;
	case NST_SUMMON_WOOD:
		return new ioSummonWoodSkill;
	case NST_WEAPON_CONTROL2:
		return new ioWeaponControlSkill2;
	case NST_CIRCLE_MOVE4:
		return new ioCircleMove4Skill;
	case NST_MULTI_CIRCLE:
		return new ioMultiMagicCircleSkill;
	case NST_TARGET_SHOT_OWNER:
		return new ioTargetShotOwnerSkill;
	case NST_TRACE2:
		return new ioTraceSkill2;
	case NST_AREA_CONTROL2:
		return new ioAreaWeaponControl2Skill;
	case NST_CIRCLE_MOVE_WEAPON3:
		return new ioCircleMoveWeapon3Skill;
	case NST_CHARGING_FIRE6:
		return new ioChargingFireSkill6;
	case NST_CIRCLE_MOVE_WEAPON4:
		return new ioCircleMoveWeapon4Skill;
	case NST_TARGET_DEBUFF2:
		return new ioTargetDebuffSkill2;
	case NST_COMBO_JUMP:
		return new ioComboJumpSkill;
	case NST_MULTI_CIRCLE2:
		return new ioMultiCircleSkill;
	case NST_TARGET_DEBUFF3:
		return new ioTargetDebuffSkill3;
	case NST_TARGET_TELEPORT2:
		return new ioTargetTeleportSkill2;
	case NST_CIRCLE_MOVE_ATTACK4:
		return new ioCircleMoveAttackSkill4;
	case NST_TARGET_DEBUFF4:
		return new ioTargetDebuffSkill4;
	case NST_CIRCLE_MOVE_ATTACK5:
		return new ioCircleMoveAttackSkill5;
	case NST_CIRCLE_MOVE_ATTACK6:
		return new ioCircleMoveAttackSkill6;
	case NST_CIRCLE_MOVE_ATTACK7:
		return new ioCircleMoveAttackSkill7;
	case NST_TARGET_MOVE_ATTACK2:
		return new ioTargetMoveAttackSkill2;
	case NST_RANDOM_BUFF:
		return new ioRandomBuffSkill;
	case NST_COMBO_MOVE_ATTACK4:
		return new ioComboMoveAttack4Skill;
	case NST_COMBO_JUMP2:
		return new ioComboJumpSkill2;
	case NST_CIRCLE_MOVE5:
		return new ioCircleMove5Skill;
	case NST_TARGET_MISSILE2:
		return new ioTargetMissile2Skill;
	case NST_SCAB_SKILL:
		return new ioScabSkill;
	case NST_RANGE_TARGET_SHOT:
		return new ioRangeTargetShotSkill;
	case NST_DESTROY:
		return new ioDestroySkill;
	case NST_CIRCLE_MOVE6:
		return new ioCircleMove6Skill;
	case NST_DUMMY_COMBO_FIRE:
		return new ioDummyComboFireSkill;
	case NST_MULTI_AREA4:
		return new ioMultiArea4Skill;
	case NST_CIRCLE_MOVE_CREATE_DUMMY:
		return new ioCircleMoveCreateDummySkill;
	case NST_TELEPORT_ATTACK2:
		return new ioTeleportAttackSkill2;
	case NST_JUMP_LINEAR_COMBO:
		return new ioJumpLinearComboSkill;
	case NST_TELEPORT_ATTACK3:
		return new ioTeleportAttackSkill3;
	case NST_CIRCLE_MOVE_ATTACK8:
		return new ioCircleMoveAttackSkill8;
	case NST_ENGINEER_MACHINE_GUN:
		return new ioEngineerMachineGunAttack;
	case NST_WEAPON_CONTROL3:
		return new ioWeaponControlSkill3;
	case NST_FLY_MOVE_ATTACK:
		return new ioFlyMoveAttackSkill;
	case NST_TARGET_DEBUFF5:
		return new ioTargetDebuffSkill5;
	case NST_CIRCLE_MOVE_ATTACK9:
		return new ioCircleMoveAttackSkill9;
	case NST_TARGET_DEBUFF6:
		return new ioTargetDebuffSkill6;
	case NST_CIRCLE_MOVE7:
		return new ioCircleMove7Skill;
	case NST_CIRCLE_MOVE_WEAPON5:
		return new ioCircleMoveWeapon5Skill;
	case NST_CIRCLE_ATTACK:
		return new ioCircleAttackSkill;
	case NST_CIRCLE_MOVE_WEAPON6:
		return new ioCircleMoveWeapon6Skill;
	case NST_TARGET_MOVE_ATTACK3:
		return new ioTargetMoveAttackSkill3;
	case NST_CIRCLE_MOVE8:
		return new ioCircleMove8Skill;
	case NST_MULTI_CIRCLE_MOVE_ATTACK:
		return new ioMultiCircleMoveAttackSkill;
	case NST_CIRCLE_MOVE_AREA_WEAPON:
		return new ioCircleMoveCreateAreaWeaponSkill;
	case NST_JUMP_TARGET_SHOT_SKILL:
		return new ioJumpTargetShotSkill;
	case NST_TARGET_MISSILE3:
		return new ioTargetMissile3Skill;
	case NST_CIRCLE_MOVE_CREATE_DUMMY2:
		return new ioCircleMoveCreateDummySkill2;
	case NST_TARGET_CIRCLE_MOVE:
		return new ioTargetCircleMoveSkill;
	case NST_TARGET_MOVE_ATTACK_RUN:
		return new ioTargetMoveAttackRunSkill;
	case NST_CREATE_EQUIP_ITEM:
		return new ioCreateEquipItemSkill;
	case NST_CREATE_MULTIPLE_OBJECT_ITEM:
		return new ioCreateMultipleObjectItemSkill;
	case NST_TELEPORT_SPECIFIED_ENEMY:
		return new ioTeleportSpecifiedEnemySkill;
	case NST_COMBO_MOVE_ATTACK5:
		return new ioComboMoveAttack5Skill;
	}

	LOG.PrintTimeAndLog( 0, "ioSkillMaker::CreateNormalSkillTemplete - %d UnknownType", iNormalType );
	return NULL;
}

ioSkill* ioSkillMaker::CreateRangeSkillTemplete( ioINILoader &rkLoader )
{
	int iRangeType = rkLoader.LoadInt_e( "sub_type", 0 );
	switch( iRangeType )
	{
	case RST_BUFF:
		return new ioRangeBuffSkill;
	case RST_WEAPON:
		return new ioRangeAttackSkill;
	case RST_TELEPORT:
		return new ioRangeTeleportSkill;
	case RST_SYNC_BUFF:
		return new ioRangeSyncBuffSkill;
	case RST_TELEPORT2:
		return new ioRangeTeleportSkill2;
	case RST_POSITION_INVERSE:
		return new ioRangePositionInverseSkill;
	case RST_BUFF_ATTACK:
		return new ioRangeBuffAttackSkill;
	}

	LOG.PrintTimeAndLog( 0, "ioSkillMaker::CreateRangeSkillTemplete - %d UnknownType", iRangeType );
	return NULL;
}

ioSkill* ioSkillMaker::CreatePassiveSkillTemplete( ioINILoader &rkLoader )
{
	int iPassiveType = rkLoader.LoadInt_e( "sub_type", 0 );
	switch( iPassiveType )
	{
	case PST_DEFAULT:
		return new ioPassiveSkill;
	case PST_WOUND:
		return new ioWoundPassiveSkill;
	case PST_RANDOM:
		return new ioRandomPassiveSkill;
	case PST_ASSESSORY:
		return new ioAccessoryPassiveSkill;
	}

	LOG.PrintTimeAndLog( 0, "ioSkillMaker::CreatePassiveSkillTemplete - %d UnknownType", iPassiveType );
	return NULL;
}

ioSkill* ioSkillMaker::CreateCountSkillTemplete( ioINILoader &rkLoader )
{
	int iCountType = rkLoader.LoadInt_e( "sub_type", 0 );
	switch( iCountType )
	{
	case CST_ATTACK:
		return new ioCountSkill;
	case CST_GLIDE:
		return new ioGlideSkill;
	case CST_LIGHT:
		return new ioCountLightSkill;
	case CST_FORCE:
		return new ioCountForceSkill;
	case CST_ROLLING:
		return new ioCountRollingSkill;
	case CST_DASH_ATTACK:
		return new ioCountDashAttackSkill;
	case CST_TELEPORT_ATTACK:
		return new ioCountTeleportAttackSkill;
	case CST_WHIP_ATTACK:
		return new ioFantasticWhipAttack;
	case CST_BUFF_CHECK_ATTACK:
		return new ioBuffCheckAttackSkill;
	case CST_BUFF:
		return new ioBuffCountSkill;
	case CST_TELEPORT_ATTACK2:
		return new ioCountTeleportAttackSkill2;
	case CST_RUNNING_ATTACK:
		return new ioCountRunningAttackSkill;
	case CST_TOIST_MAGIC_ATTACK:
		return new ioCountToistMagicAttack;
	}

	LOG.PrintTimeAndLog( 0, "ioSkillMaker::CreateCountSkillTemplete - %d UnknownType", iCountType );
	return NULL;
}

ioSkillMaker& ioSkillMaker::GetSingleton()
{
	return Singleton<ioSkillMaker>::GetSingleton();
}
