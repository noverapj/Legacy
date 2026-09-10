#pragma once
#include "GameEnumType.h"
#include "ItemDefine.h"

#include "ioBunkerDummy.h"

#include "StateClass/ioConstructHouseSpecialState.h"
#include "StateClass/ioBlockInterplaySpecialState.h"

class ioBaseChar;

namespace ioSpecialStateFactory
{
	ioSpecialStateBase* CreateSpecialState( ioBaseChar* pOwner, CharState eNewState, CharState eCurrState )
	{
		ioSpecialStateBase* pNewState = NULL;
		switch( eNewState )
		{
			// 용병
		case CS_UROBORUS:
			pNewState = new ioUroborusSpecialState;
			break;
		case CS_WEREWOLF:
			pNewState = new ioWerewolfSpecialState;
			break;
		case CS_WITCH_FLY:
			pNewState = new ioWitchFlySpecialState;
			break;
		case CS_MECHANICS_FLY:
			pNewState = new ioMechanicsFlySpecialState;
			break;
		case CS_PHANTOM_FLY:
			pNewState = new ioPhantomFlySpecialState;
			break;
		case CS_LUCIFER_FLY:
			pNewState = new ioLuciferFlySpecialState;
			break;
		case CS_NAKORURU_FLY:
			pNewState = new ioNakoruruFlySpecialState;
			break;
		case CS_NAKORURU_JUMP_ATTACK:
			pNewState = new ioNakoruruJumpAttackSpecialState;
			break;
		case CS_DESTROYER_FLY:
			pNewState = new ioDestroyerFlySpecialState;
			break;
		case CS_MICHAEL_FLY:
			pNewState = new ioMichaelFlySpecialState;
			break;
		case CS_GOOMIHO_LAND_FLY:
			pNewState = new ioGoomihoFlySpecialState;
			break;
		case CS_PETERPAN_FLY:
			pNewState = new ioPeterpanFlySpecialState;
			break;
		case CS_SUCCUBUS_ATTACK:
			pNewState = new ioSuccubusSpecialState;
			break;
		case CS_CYVOGUE_STATE:
			pNewState = new ioCyvogueSpecialState;
			break;
		case CS_TRINITY:
			pNewState = new ioTrinitySpecialState;
			break;
		case CS_TRINITY_SWALLOW:
			pNewState = new ioTrinitySwallowMoonSpecialState;
			break;
		case CS_GHOST_STEALER_STATE:
			pNewState = new ioGhostStealerSpecialState;
			break;
		case CS_DRACULA_FLY:
			pNewState = new ioDraculaFlySpecialState;
			break;
		case CS_HAKUMEN_DEFENSE:
			pNewState = new ioHakumenDefenseSpecialState;
			break;
		case CS_DEVIL_FLY:
			pNewState = new ioDevilFlySpecialState;
			break;
		case CS_FLASH_SPECIAL:
			pNewState = new ioFlashSpecialState;
			break;
		case CS_EVY_BLINK:
			pNewState = new ioEvyBlinkSpecialState;
			break;
		case CS_EVY_BOMB_STATE:
			pNewState = new ioEvyBombSpecialState;
			break;
		case CS_PSYCHO_SPECIAL:
			pNewState = new ioPsychoMetrySpecialState;
			break;
		case CS_SLASHER_SPECIAL:
			pNewState = new ioSlasherSpecialState;
			break;
		case CS_TITAN_EXTEND_MOVE:
			pNewState = new ioTitanExtendMoveSpecialState;
			break;
		case CS_SUPER_DIMENSION_FIRE:
			pNewState = new ioSuperDimensionFireSpecialState;
			break;
		case CS_SUPER_DIMENSION_DASH:
			pNewState = new ioSuperDimensionDashSpecialState;
			break;
		case CS_NANGIN_SPECIAL:
			pNewState = new ioNanginSpecialState;
			break;
		case CS_STRIKER_MOVE:
			pNewState = new ioStrikerMoveSpecialState;
			break;
		case CS_STRIKER_LAND_ATTACK:
			pNewState = new ioStrikerLandAttackSpecialState;
			break;
		case CS_DRAGONNEST_ASS_SPECIAL:
			pNewState = new ioDragonNestAssSpecialState;
			break;
		case CS_THUNDER_BIRD_OBJECT_SPECIAL:
			pNewState = new ioThunderBirdObjectSpecialState;
			break;
		case CS_THUNDER_BIRD_HOLD:
			pNewState = new ioThunderBirdHoldSpecialState;
			break;
		case CS_THUNDER_BIRD_BOMB_STATE:
			pNewState = new ioThunderBirdBombSpecialState;
			break;
		case CS_VALKYRIE_FLY:
			pNewState = new ioValkyrieFlySpecialState;
			break;
		case CS_EARTHQUAKE_SPECIAL:
			pNewState = new ioEarthQuakeSpecialState;
			break;
		case CS_SHADOW_RANGER_SPECIAL:
			pNewState = new ioShadowRangerSpecialState;
			break;
		case CS_DARKKNIGHT_SPECIAL:
			pNewState = new ioDarkKnightSpecialState;
			break;
		case CS_JUMPER_SPECIAL:
			pNewState = new ioJumperSpecialSpState;
			break;
		case CS_SABER_ATTACK:
			pNewState = new ioSaberAttackSpecialState;
			break;
		case CS_SABER_FLY:
			pNewState = new ioSaberFlySpecialState;
			break;
		case CS_SABER_DASH_ATTACK:
			pNewState = new ioSaberDashAttackSpecialState;
			break;
		case CS_HADES_ATTACK:
			pNewState = new ioHadesSpecialState;
			break;
		case CS_MAGIC_SWORD_ATTACK:
			pNewState = new ioMagicSwordSpecialState;
			break;
		case CS_NATA_COMBO:
			pNewState = new ioNataComboSpecialState;
			break;
		case CS_NATA_FLY:
			pNewState = new ioNataFlySpecialState;
			break;
		case CS_RESEARCHER_REAR_FLY:
			pNewState = new ioResearcherRearFlySpecialState;
			break;
		case CS_BLADE_FLY:
			pNewState = new ioBladeFlySpecialState;
			break;
		case CS_GRAPPLER_JUMP_GRAP:
			pNewState = new ioGrapplerJumpAttackSpecialState;
			break;
		case CS_SAWANG_STATE:
			pNewState = new ioSawangSpecialState;
			break;
		case CS_APOSTATE_MONK_SPECIAL:
			pNewState = new ioApostateMonkSpecialState;
			break;
		case CS_JEONWOOCHI_EXTEND_MOVE:
			pNewState = new ioJeonWooChiExtendMoveSpecialState;
			break;
		case CS_JEONWOOCHI_EXTEND_DASH:
			pNewState = new ioJeonWooChiExtendDashSpecialState;
			break;
		case CS_STRIDER_SPECIAL:
			pNewState = new ioStriderSpecialState;
			break;
		case CS_RED_HOOD_MOVE:
			pNewState = new ioRedHoodItemMoveSpecialState;
			break;
		case CS_MUTANT_SPECIAL_STATE:
			pNewState = new ioMutantItemSpecialState;
			break;
		case CS_NEPHILIM_PUSH_STATE:
			pNewState = new ioNephilimPushState;
			break;
		case CS_NEPHILIM_BOMB_STATE:
			pNewState = new ioNephilimBombState;
			break;
		case CS_NEPHILIM_TELEPORT_STATE:
			pNewState = new ioNephilimTeleportState;
			break;
		case CS_MIR_WARRIOR_DASH_STATE:
			pNewState = new ioMirWarriorDashSpecialState;
			break;
		case CS_CUCHULAIN_SPECIAL_STATE:
			pNewState = new ioCuchulainItemSpecialState;
			break;
		case CS_GALFORD_CMD_STATE:
			pNewState = new ioGalfordCmdSpecialState;
			break;
		case CS_ONIMUSHA_RUN_EXTRA_STATE:
			pNewState = new ioOnimushaRunExtraState;
			break;
		case CS_DRUID_CROUCH_GATHERING_STATE:
			pNewState = new ioDruidCrouchGatheringState;
			break;
		case CS_DRUID_EAGLE_FLY:
			pNewState = new ioDruidEagleFlyState;
			break;
		case CS_SASINMU_SPECIAL:
			pNewState = new ioSasinmuSpecialState;
			break;
		case CS_DARKSEAS_STATE:
			pNewState = new ioDarkSeasSpecialState;
			break;
		case CS_PREDATOR_SPECIAL:
			pNewState = new ioPredatorSpecialState;
			break;
		case CS_CROW_SPECIAL_STATE:
			pNewState = new ioCrowItemSpecialState;
			break;
		case CS_MEPHISTO_STATE:
			pNewState = new ioMephistoSpecialState;
			break;
		case CS_BANGTAN_STATE:
			pNewState = new ioBangtanSpecialState;
			break;
		case CS_SURFER_STATE:
			pNewState = new ioSurferSpecialState;
			break;
		case CS_URIEL_STATE:
			pNewState = new ioUrielSpecialState;
			break;
		case CS_WINDGOD_SPECIAL:
			pNewState = new ioWindGodSpecialState;
			break;
		case CS_WARP_SPECIAL:
			pNewState = new ioWarpSpecialState;
			break;
		case CS_STRANGER_SPECIAL:
			pNewState = new ioStrangerSpecialState;
			break;
		case CS_GARGOYLE_SPECIAL:
			pNewState = new ioGargoyleSpecialState;
			break;
		case CS_CANNON_SPECIAL:
			pNewState = new ioCannonSpecialState;
			break;
		case CS_MAGICENGINEER_SPECIAL:
			pNewState = new ioMagicEngineerSpecialState;
			break;
		case CS_JINRAH_SPECIAL:
			pNewState = new ioJinrahSpecialState;
			break;
		case CS_KUNGFU_SPECIAL:
			pNewState = new ioKungfuSpecialState;
			break;
		case CS_NIGHTMARE_SPECIAL:
			pNewState = new ioNightmareSpecialState;
			break;
		case CS_AZAZEL_SPECIAL:
			pNewState = new ioAzazelSpecialState;
			break;
		case CS_MEGA_AGENT_FLY:
			pNewState = new ioMegaAgentFlyState;
			break;
		case CS_MEGA_AGENT_SPECIAL:
			pNewState = new ioMegaAgentSpecialState;
			break;
		case CS_NEO_FLY:
			pNewState = new ioNeoFlySpecialState;
			break;
		case CS_NEO_ATTACK:
			pNewState = new ioNeoAttackSpecialState;
			break;
		case CS_SANTA_SPECIAL:
			pNewState = new ioSantaFlySpecialState;
			break;
		case CS_WILDBOXER_SPECIAL:
			pNewState = new ioWildBoxerSpecialState;
			break;
		case CS_AUTOMATON_SPECIAL:
			pNewState = new ioAutomatonSpecialState;
			break;
		case CS_GARDENER_DELAY_SPECIAL:
			pNewState = new ioGardenerDelaySpecialState;
			break;
		case CS_GARDENER_ATTACK_SPECIAL:
			pNewState = new ioGardenerAttackSpecialState;
			break;
		case CS_BUBBLE_OBJECT_SPECIAL:
			pNewState = new ioBubbleObjectSpecialState;
			break;
		case CS_CHANGE_ON_GRIFFIN_FLY:
			pNewState = new ioChangeOnGriffinFlyState;
			break;
		case CS_MURIM_GOSU_SPECIAL:
			pNewState = new ioMurimGosuSpecialState;
			break;
		case CS_GHOST_FLY:
			pNewState = new ioGhostFlySpecialState;
			break;
		case CS_SLOPE_SPECIAL:
			pNewState = new ioSlopeFlySpecialState;
			break;
		case CS_HANZO_SPECIAL:
			pNewState = new ioHanzoSpecialState;
			break;
		case CS_MIDNIGHT_SPECIAL:
			pNewState = new ioMidnightSpecialState;
			break;
		case CS_MIDNIGHT_SCYTHE:
			pNewState = new ioMidnightScytheState;
			break;
		case CS_MIDNIGHT_GUN:
			pNewState = new ioMidnightGunState;
			break;
		case CS_TYR_STATE:
			pNewState = new ioTyrSpecialState;
			break;
		case CS_TAOIST_SPECIAL:
			pNewState = new ioTaoistSpecialState;
			break;
			//진화용병
		case CS_NINJA_EXPANSION_MOVE:
			pNewState = new ioNinjaExpansionSpecialState;
			break;
		case CS_REAPER_EXPANSION:
			pNewState = new ioReaperExpansionSpecialState;
			break;
		case CS_MAGICSTAFF_SPECIAL:
			pNewState = new ioMagicStaffSpecialState;
			break;
		case CS_FIRE_SPECIAL:
			pNewState = new ioFireSpecialState;
			break;
		case CS_POWERCHARGE_SPECIAL:
			pNewState = new ioPowerChargeSpecialState;
			break;
		case CS_SUMMON_SKULLHERO_SPECIAL:
			pNewState = new ioSummonSkullHeroSpecialState;
			break;
		case CS_HAWKEYE_SPECIAL:
			pNewState = new ioHawkEyeSpecialState;
			break;
		case CS_SMILE_JOKER_FLY:
			pNewState = new ioSmileJokerFlyState;
			break;
			// 아이템
		case CS_OBJECT_RUNNING_ATTACK:
			pNewState = new ioObjectRunningAttackSpecialState;
			break;
		case CS_OBJECT_ATTACH:
			pNewState = new ioObjectAttachSpecialState;
			break;			
		case CS_HELICOPTER:
			pNewState = new ioHelicopterSpecialState;
			break;
		case CS_HELICOPTER_ATTACH:
			pNewState = new ioHelipCopterAttachSpecialState;
			break;
		case CS_VEHICLE_DRIVER:
			pNewState = new ioVehicleDirverState;
			break;
		case CS_PASSENGER:
			pNewState = new ioPassengerState;
			break;
		case CS_OBJECT_RUNNING_COMBO_ATTACK:
			pNewState = new ioObjectRunningComboSpecialState;
			break;
		case CS_FLY_BUFF_EXTRA_ATTACK:
			pNewState = new ioFlyJumpExtraAttackBuffSpecialState;
			break;
		case CS_BUNKER_STATE:
			pNewState = new ioBunkerAttachSpecialState;
			break;
			
		case CS_BUFF_AIR_WALK_ATTACK:
			pNewState = new ioBuffAirWalkAttackState;
			break;
		case CS_WARP_BUFF_SPECIAL:
			pNewState = new ioWarpBuffSpecialState;
			break;
		case CS_NUCLEARSOLDIER_SPECIAL:	// 2020-02-14 뉴클리어솔저
			pNewState = new ioNuClearSoldierSpecialState;
			break;
		case CS_POSEIDON_SPECIAL:
			pNewState = new ioPoseidonSpecialState;
			break;
		case CS_SKY_WALKER_SPECIAL:
			pNewState = new ioSkyWalkerSpecialState;
			break;
		case CS_GWONWANG_SPECIAL:	// 2020-05-021 신규 용병 레어(권왕) 
			pNewState = new ioGwonwangSpecialState;
			break;
		case CS_OBJECT_BANKRUPTCY_SPECIAL:
			pNewState = new ioObjectBankruptcySpecialState;	// 2020-06-08 레어(권왕 파산신공) 추가
			break;
		case CS_GENISYSARMY_SPECIAL:
			pNewState = new ioGenisysarmySpecialState;	// 2020-06-25 레어(제네시스) 추가
			break;
		case CS_LILLY_SPECIAL:
			pNewState = new ioLillySpecialState;
			break;
		case CS_EVA_SPECIAL:					
			pNewState = new ioEvaSpecialState;			// 2020-10-20 레어(이브) 추가
			break;
		case CS_MAGNETIC_SPECIAL:					
			pNewState = new ioMagneticSpecialState;		// 2020-11-20 레어(마그넥티) 추가
			break;

			// 기타
		case CS_BLOCK_INTEPLAY:
			pNewState = new ioBlockInterplaySpecialState;
			break;
		case CS_CONSTRUCT_HOUSE:
			pNewState = new ioConstructHouseSpecialState;
			break;
		}

		if( pNewState )
		{
			char szBuf[MAX_PATH];
			ioINILoader_e kLoader( "config/sp2_special_state.ini" );
			sprintf_s_e( szBuf, "state%d", (int)eNewState );
			kLoader.SetTitle( szBuf );

			pNewState->LoadProperty( kLoader );
			pNewState->Initialize( pOwner, eCurrState );
			if( pNewState->IsInitializeSync() && pOwner && pOwner->IsNeedSendNetwork() )
			{
				SP2Packet kPacket( CUPK_INITIALIZE_SPECIAL_STATE );
				kPacket << pOwner->GetCharName();
				kPacket << (int)eNewState;
				P2PNetwork::SendToAllPlayingUser( kPacket );
			}
		}

		return pNewState;
	}
};