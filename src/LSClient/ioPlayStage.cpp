#include "stdafx.h"

#include "ioPlayStage.h"

#include "ioLadderTeamMgr.h"
#include "ioMapCollisionHelper.h"
#include "ioNpcChar.h"
#include "ioTreasureCardMgr.h"

#include "ioTournamentManager.h"
#include "ioTournamentTeamManager.h"

#include "ioTowerDefMode.h"
#include "ioDoubleCrownMode.h"
#include "ioHouseMode.h"
#include "ioRaidMode.h"
#include "ioTeamSurvivalAIMode.h"
#include "ioShuffleBonusMode.h"
#include "ioFlagCaptureMode.h"
#include "ioArenaMode.h"
#include "ioBattleMode.h"
#include "ioFarmingMode.h"
#include "ioClientAniEventHandler.h"
#include "ioReservedTask.h"

#include "ItemDefine.h"
#include "BuffDefine.h"
#include "SkillDefine.h"
#include "DummyCharDefine.h"
#include "RoomDefine.h"

#include "ioBingo.h"
#include "ioSuperGashaponMgr.h"
#include "ioMultiSuperGashaponMgr.h"
#include "ioItemRechargeManager.h"
#include "ioSubscriptionMgr.h"
#include "ioExcavationManager.h"
#include "ioSlotMgr.h"
#include "ioShuffleRoomMgr.h"
#include "ioRankBattleManager.h"
#include "ioPowerUpManager.h"
#include "ioNoviceGradeBenefitManager.h"
#include "ioFishingManager.h"
#include "ioRsoldierInfoManager.h"
#include "ioExtendSoundManager.h"
#include "ioBonusCashManager.h"
#include "ioAccessoryInfoManager.h"

#include "Housing/ioBlockWorld.h"
#include "Housing/ioUserBlockStorage.h"

#include "Channeling/ioChannelingNodeManager.h"
#include "Channeling/ioChannelingNodeParent.h"

#ifdef NPROTECT
#include "nProtect/ioNProtect.h"
#endif 

#include "GUI/ioMemoManager.h"
#include "GUI/HelpWnd.h"
#include "GUI/TowerDefWnd.h"
#include "GUI/TournamentMainWnd.h"
#include "GUI/MessengerWnd.h"
#include "GUI/ExPCRoomWnd.h"
#include "GUI/SelectGashaponWnd.h"
#include "GUI/RoundStateMainWnd.h"
#include "GUI/PlazaMainWnd.h"
#include "GUI/QuickLobbyWnd.h"
#include "GUI/TradeMainWnd.h"
#include "GUI/LobbyMainWnd.h"
#include "GUI/LadderTeamWnd.h"
#include "GUI/VoiceChatWnd.h"
#include "GUI/PartyVersusWnd.h"
#include "GUI/SoldierLimitWnd.h"
#include "GUI/SoldierSelectWnd.h"
#include "GUI/FishingInventoryWnd.h"
#include "GUI/GameOptionWnd.h"
#include "GUI/UserInfoToolTip.h"
#include "GUI/MannerValuationWnd.h"
#include "GUI/KickUserVoteWnd.h"
#include "GUI/ModeRankWnd.h"
#include "GUI/PCRoomWnd.h"
#include "GUI/GameOptionSettingTab.h"
#include "GUI/GameOptionSettingJoyKeyTab.h"
#include "GUI/TotalResultMainWnd.h"
#include "GUI/UISoldierSelectWnd.h"
#include "GUI/MiniSoldierSelectWnd.h"
#include "GUI/AbuseProblemWnd.h"
#include "GUI/TimeGateSearch.h"
#include "GUI/HeadquartersMainWnd.h"
#include "GUI/NewShopBoughtWnd.h"
#include "GUI/BillingWantWnd.h"
#include "GUI/MyInventoryInfoWnd.h"
#include "GUI/HeroMainWnd.h"
#include "GUI/ioGrowthSelectWnd.h"
#include "GUI/SendPresentWnd.h"
#include "GUI/TradeItemRegisterWnd.h"
#include "GUI/SelectExtraGashaponWnd.h"
#include "GUI/ioExpansionBox.h"
#include "GUI/PetWnd.h"
#include "GUI/RollBookWnd.h"
#include "GUI/TrainingFuncWnd.h"
#include "GUI/NewShopItemPurchaseLeedWnd.h"
#include "GUI/HouseMainWnd.h"
#include "GUI/DebugInfoWnd.h"
#include "GUI/PresentWnd.h"
#include "GUI/RankBattleWnd.h"
#include "GUI/ioSuccessionModeWnd.h"
#include "GUI/AlchemicChangeAlarmWnd.h"

#include "ioSuccessionMode.h"
#include "GameStage/Mode/Practice/ioPracticeMode.h"
#include "GUI/PracticemainWnd.h"
#include "GUI/GradeAdjustmentWnd.h"

bool ioPlayStage::sg_bEquipItemError = false;
ioPlayStage::ioPlayStage( ioGameSceneManager *pSceneMgr ) : ioGameStage( pSceneMgr )
{
	m_pPushStructMgr = NULL;
	m_pAreaWeaponMgr = NULL;
	m_pTalismanMaker = NULL;
	m_pObserverWndMgr = NULL;
	m_pWorldEntityFactory = NULL;

	m_pBallMgr = NULL;
	m_pMachineStructMgr = NULL;

	m_pDummyCharMgr = NULL;

	m_pPetMgr = NULL;
	m_pSoldierCacheManager = NULL;
	m_pRecvPacketLogManager = NULL;
	m_pNoviceGradeBenefitManager = NULL;

	m_pPlayMode = NULL;
	m_pBlockMgr	= NULL;
	m_pBlockWorld = NULL;

	m_pCameraEventMgr = NULL;
	m_pShakeCameraMgr = NULL;

	m_iModeSubNum = 0;
	m_iNextModeSubNum = 0;

	m_iModeMapIndex = 0;
	m_iNextModeMapIndex = 0;

	m_pGetupStick = NULL;
	m_pGetupStick2 = NULL;
	m_pPressButton = NULL;

	m_pBuffBlind = NULL;

	m_dwPreUpdatedTime = 0;
	m_dwCheckValidateNetTableTime = 0;

	m_fMapFriction = 400.0f;
	m_fMapGravity = 800.0f;
	m_dwMapClearColor = 0xff000000;

	m_iMaxSinglePlayer = 4;

	m_fCamLookAtHeightOffset = 200.0f;

	m_iCameraShakePriority = 0;
	m_dwCameraShakeStartTime = 0;
	m_dwCameraShakeCheckTime = 0;
	m_dwCameraShakeGapTime = 60;
	m_fCameraShakeMaxRate = 0.0f;
	m_fCameraShakeEndAmount = FLOAT1;
	m_fCameraShakeOffsetX = 15.0f;
	m_fCameraShakeOffsetY = 0.0f;

	m_fShadowEnableRange = 0.0f;

	m_dwViewMovingTime = 0;
	m_dwCamSlerpTime = 2000;

	m_dwViewMovingTimeItem = 0;
	m_dwCamZoomTimeItem = 0;

	m_fGapDistance = 0.0f;
	m_fCurGapDistance = 0.0f;
	m_fMaxGapDistance = 0.0f;
	m_fGapHeight = 0.0f;
	m_fCurGapHeight = 0.0f;
	m_fMaxGapHeight = 0.0f;
	m_fGapFov = 0.0f;
	m_fCurGapFov = 0.0f;
	m_fMaxGapFov = 0.0f;
	m_dwZoomStartTime = 0;
	m_dwZoomSlerpTime = 0;
	m_dwCurSlerpTime = 0;
	m_dwChargingStartTime = 0;
	m_dwBuffCamStartTime = 0;
	m_fArcadeCameraDistanceLimit = 0.0f;

	m_fDefCamHeight = 0.0f;
	m_fDefCamDistance = 0.0f;
	m_fDefCamFov = 0.0f;

	m_fStartDistance = 0.0f;
	m_fStartHeight = 0.0f;
	m_fStartFov = 0.0f;

	m_fCurCamHeight = 0.0f;
	m_fCurCamDistance = 0.0f;
	m_fCurCamFov = 0.0f;

	m_fMouseCameraZ = 0.0f;
	m_fMousePreTargetZ = 0.0f;

	m_vMouseCameraRot		   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vMousePreTargetRot	   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vCamMouseTargetPrevDir   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vCamMouseTargetPrevPos   = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_vCamMouseTargetCurLookAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_dwCamMouseLookAtMoveCurTime = 0;				
	m_dwCamMouseLookAtMoveTime = 1000;

	m_dwMouseCamCtrlTime = 0;

	m_dwPreCharCollisionSyncTime = 0;

	m_fPickItemRange = 0.0f;
	m_dwOwnerCameraLoseTime = 0;
	
	m_bRenderDamage = false;
	m_bShowCharName = true;
	m_bShowItemName = true;
	m_bShowItemIcon = true;

	m_fItemNameSize = FLOAT08;
	m_iItemNameColorRed = 255;
	m_iItemNameColorGreen = 255;
	m_iItemNameColorBlue = 255;

	m_bSpringDampUse = false;
	m_bCameraZooming = true;
	m_bCameraChanging = false;
	m_bCameraSkilling = false;

	m_dwCameraPauseTime = m_dwCurCameraPauseTime = 0;
	m_bEventSceneCamera = false;
	m_bEndSceneCamera = false;
	m_bNoneCameraWorking = false;
	m_bArcadeCameraWorking= false;
	m_bFirstEventSceneCamera = true;
	m_bEventSceneEndSendPacket = false;
	m_bFirstEventSceneEndSendCheck = false;
	m_dwEventSceneEndNextCameraEvent = 0;

	m_fCameraFixedFov = 0.0f;

	m_bSetCameraToNowOwner = false;
	m_bTargetLookAt = false;
	
	m_RoomStyle = RSTYLE_NONE;
	m_iRoomIndex = -1;

	m_iRedForBalance = 0;
	m_iBlueForBalance = 0;
	
	m_fMemberBalanceBlue = FLOAT1;
	m_fMemberBalanceRed = FLOAT1;

	m_fMemberBalanceBlueForGauge = FLOAT1;
	m_fMemberBalanceRedForGauge = FLOAT1;

	m_fMemberBalanceBlueForBlowTime = FLOAT1;
	m_fMemberBalanceRedForBlowTime = FLOAT1;

	m_fMemberBalanceRedForSpeed = FLOAT1;
	m_fMemberBalanceBlueForSpeed = FLOAT1;

	m_iMemberBalanceRedCntForBuff = 0;
	m_iMemberBalanceBlueCntForBuff = 0;

	m_iPreRedCnt = 0;
	m_iPreBlueCnt = 0;

	m_ReserveNextModeType = MT_NONE;

	m_dwCameraRollStartTime = 0;

	m_BaseCharList.reserve( MAX_PLAZA_PLAYER );

	m_ScreenBlindState = SBS_NONE;
	m_dwScreenBlindCurTime = 0;
	m_ScreenBlindType = SBT_NONE;
	m_fScreenBlindBuffRange = 0.0f;

	m_BuffBlindState = SBS_NONE;
	m_dwBuffBlindCurTime = 0;

	m_dwKillCameraEvent  = 0;
	m_dwDeathCameraEvent = 0;
	m_dwCatchKillCameraEvent  = 0;
	m_dwCatchDeathCameraEvent = 0;

	m_ePlazaType          = PT_NONE;
	m_bOpenPlaza          = false;
	m_bBlindAniEnd        = true;
	m_bCameraFixedBlock   = false;
	m_dwCameraFixedStyle  = CAMERA_STYLE_NORMAL;
	m_vCamMouseAmt		  = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_eVoiceChatType = VCT_TEAM;
	m_iRoomKickOutVoteProposal = 0;
	m_iBadNetworkCount = 0;
	m_bEnableScreenRedFilter = false;
	m_bEnableScreenOveray    = false;
	m_fGameEntityCollisionCheckRange = FLOAT1000;

	m_WoodSummonPositionList.clear();

	m_bDrawGlowOfTObectAll = 0;
	m_fRenderGlowness = FLOAT1;

	m_bUseRevisionSkillGauge = false;
	m_bReLoadRevisionSkillGauge = false;
	m_fRevisionDefaultSkillGauge = 0.f;
	ClearPieceToPesoInfo();

	m_vObserverLookAt = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	m_bSpecialObserver = true;
	m_bStartChangeObserver = false;
}

ioPlayStage::~ioPlayStage()
{
	ClearStage();
	ErrorReport::SetPosition( 1100, 827 );
	g_App.ClearAllNetworkTable( P2P_STYLE_PLAY ); 
	ErrorReport::SetPosition( 1100, 828 );
	if( !g_BattleRoomMgr.IsBattleRoom() && !g_LadderTeamMgr.IsLadderTeam() && 
		!g_ShuffleRoomMgr.IsShuffleRoom() && !g_RankBattleMgr.IsRankBattlePlaying() )
	{
		ErrorReport::SetPosition( 1100, 829 );
		g_App.ClearAllNetworkTable( P2P_STYLE_VOICE );
		ErrorReport::SetPosition( 1100, 830 );
		g_VoiceChat.RemoveAllSoundPlayer();
		ErrorReport::SetPosition( 1100, 831 );
		g_VoiceOffListManager.DeleteAll();
		ErrorReport::SetPosition( 1100, 832 );
#ifndef SHIPPING
		LOG.PrintTimeAndLog( 0, "%s | Delete all voice", __FUNCTION__ );
#endif
	}
	ErrorReport::SetPosition( 1100, 833 );
	g_CustomSoundMgr.ClearExceptMe();
	ErrorReport::SetPosition( 1100, 834 );
#ifdef XTRAP
	g_ioXtrap.EndPayment();
#endif 
	ErrorReport::SetPosition( 1100, 835 );
#ifdef NPROTECT
	g_ioNProtect.Check( true );
#endif 
	ErrorReport::SetPosition( 1100, 836 );
}

bool ioPlayStage::InitStage()
{
	ErrorReport::SetPosition( 1100, 1 );

	ioINILoader_e kLoader( "config/sp2.ini" );

	kLoader.SetTitle_e( "input" );
	m_iMaxSinglePlayer = kLoader.LoadInt_e( "max_single_player", 4 );
	m_bRenderDamage = kLoader.LoadBool_e( "render_damage", false );
	m_bShowCharName = kLoader.LoadBool_e( "show_char_name", true );
	m_bShowItemName = kLoader.LoadBool_e( "show_item_name", true );
	m_bShowItemIcon = kLoader.LoadBool_e( "show_item_icon", true );
	m_bShowBalanceValue = kLoader.LoadBool_e( "show_balance_value", false );

	m_fItemNameSize = kLoader.LoadFloat_e( "item_name_size", FLOAT08 );
	m_iItemNameColorRed = kLoader.LoadInt_e( "item_name_color_red", 255 );
	m_iItemNameColorGreen = kLoader.LoadInt_e( "item_name_color_green", 255 );
	m_iItemNameColorBlue = kLoader.LoadInt_e( "item_name_color_blue", 255 );

	m_fShadowEnableRange = kLoader.LoadFloat_e( "shadow_enable_range", FLOAT1000 );
	m_fObserverCameraSpeed = kLoader.LoadFloat_e( "observer_camera", FLOAT10 );

	//yunga 만들다만 블러효과
	kLoader.SetTitle_e( "glow" );
	m_bDrawGlowOfTObectAll = kLoader.LoadBool_e( "DrawTransObj", false );
	m_pSceneMgr->EnableDrawAllTransObjForGlow( m_bDrawGlowOfTObectAll );

	m_fRenderGlowness = kLoader.LoadFloat_e( "Glowness", FLOAT1 );
	m_fRenderGlowTexcelInc = kLoader.LoadFloat_e( "TexcelInc", 0.0039f );

	m_pSceneMgr->SetGlownessAndTexcelInc( m_fRenderGlowness, m_fRenderGlowTexcelInc );
	
	kLoader.SetTitle_e( "skill_gauge_revision" );
	m_bUseRevisionSkillGauge = kLoader.LoadBool_e( "enable", false );
	m_bReLoadRevisionSkillGauge = kLoader.LoadBool_e( "reload", false );
	m_fRevisionDefaultSkillGauge = kLoader.LoadFloat_e( "gauge", 0.f );

	ErrorReport::SetPosition( 1100, 2 );

	m_pCameraEventMgr = new ioCameraEventManager;
	m_pCameraEventMgr->LoadCameraEventList( "config/sp2_camera_event.ini" );

	ErrorReport::SetPosition( 1100, 6 );

	m_pShakeCameraMgr = new ioShakeCameraManager( this );
	m_pShakeCameraMgr->InitManager();

	ErrorReport::SetPosition( 1100, 7 );

	m_pPushStructMgr = new ioPushStructListManager( this );

	ErrorReport::SetPosition( 1100, 8 );

	m_pBallMgr = new ioBallManager( this );
	m_pMachineStructMgr = new ioMachineStructMgr( this );
	
	m_pDummyCharMgr = new ioDummyCharManager( this );
	if( m_pDummyCharMgr )
	{
		m_pDummyCharMgr->LoadDummyCharBaseInfo();
	}

	m_pAreaWeaponMgr = new ioAreaWeaponManager( this );
	m_pAreaWeaponMgr->LoadAreaWeaponList();

	m_pTalismanMaker = new ioTalismanMaker( this );
	m_pTalismanMaker->LoadTalismanList();

	m_pObserverWndMgr = new ioObserverWndManager( this );

	m_pPetMgr = new ioPetManager( this );
	if( m_pPetMgr )
		m_pPetMgr->LoadPetBaseInfo();

	m_pSoldierCacheManager = new ioSoldierCacheManager;
	if( m_pSoldierCacheManager )
		m_pSoldierCacheManager->Initialize();

#ifndef SHIPPING
	//패킷 로그 매니저 생성
	if ( g_MyInfo.GetPublicID() == "jch2305" )
	{
		m_pRecvPacketLogManager = new ioRecvPacketLogManager;
	}
#endif

	m_pNoviceGradeBenefitManager = new ioNoviceGradeBenefitManager;
	if ( m_pNoviceGradeBenefitManager )
		m_pNoviceGradeBenefitManager->LoadProperty();

	m_pWorldEntityFactory = new ioWorldEntityFactory( this );

	g_WeaponMgr.SetPlayStage( this );
	g_ItemMaker.SetPlayStage( this );
	g_SkillMaker.SetPlayStage( this );

	kLoader.SetTitle_e( "springdamp_camera" );
	m_fSpringConst = kLoader.LoadFloat_e( "spring_const", 1.5f );
	m_fDampConst = kLoader.LoadFloat_e( "damp_const", 0.25f );

	m_fSpringXOffset = 250.0f + kLoader.LoadFloat_e( "spring_offset_x", 150.0f );
	m_fSpringZOffset = 250.0f + kLoader.LoadFloat_e( "spring_offset_z", FLOAT100 );

	int iRed, iGreen, iBlue;
	kLoader.SetTitle_e( "screen_blind" );
	
	iRed   = kLoader.LoadInt_e( "skill_color_r", 0 );
	iGreen = kLoader.LoadInt_e( "skill_color_g", 0 );
	iBlue  = kLoader.LoadInt_e( "skill_color_b", 0 );
	m_SkillBlind.m_dwColor = D3DCOLOR_ARGB( 0, iRed, iGreen, iBlue );
	m_SkillBlind.m_iMaxAlpha = kLoader.LoadInt_e( "skill_max_alpha", 0 );

	iRed   = kLoader.LoadInt_e( "kill_color_r", 0 );
	iGreen = kLoader.LoadInt_e( "kill_color_g", 0 );
	iBlue  = kLoader.LoadInt_e( "kill_color_b", 0 );
	m_KillBlind.m_dwColor = D3DCOLOR_ARGB( 0, iRed, iGreen, iBlue );
	m_KillBlind.m_iMaxAlpha = kLoader.LoadInt_e( "kill_max_alpha", 0 );
	m_KillBlindTime.m_dwUpTime     = kLoader.LoadInt_e( "kill_blind_up_time", 0 );
	m_KillBlindTime.m_dwCenterTime = max( 1, kLoader.LoadInt_e( "kill_blind_center_time", 1 ) );
	m_KillBlindTime.m_dwDownTime   = max( 1, kLoader.LoadInt_e( "kill_blind_down_time", 1 ) );
	m_dwKillCameraEvent  = kLoader.LoadInt_e( "kill_camera_event", 1 );

	iRed   = kLoader.LoadInt_e( "death_color_r", 0 );
	iGreen = kLoader.LoadInt_e( "death_color_g", 0 );
	iBlue  = kLoader.LoadInt_e( "death_color_b", 0 );
	m_DeathBlind.m_dwColor = D3DCOLOR_ARGB( 0, iRed, iGreen, iBlue );
	m_DeathBlind.m_iMaxAlpha = kLoader.LoadInt_e( "death_max_alpha", 0 );
	m_DeathBlindTime.m_dwUpTime     = kLoader.LoadInt_e( "death_blind_up_time", 0 );
	m_DeathBlindTime.m_dwCenterTime = max( 1, kLoader.LoadInt_e( "death_blind_center_time", 1 ) );
	m_DeathBlindTime.m_dwDownTime   = max( 1, kLoader.LoadInt_e( "death_blind_down_time", 1 ) );
	m_dwDeathCameraEvent = kLoader.LoadInt_e( "death_camera_event", 1 );

	iRed   = kLoader.LoadInt_e( "catch_kill_color_r", 0 );
	iGreen = kLoader.LoadInt_e( "catch_kill_color_g", 0 );
	iBlue  = kLoader.LoadInt_e( "catch_kill_color_b", 0 );
	m_CatchKillBlind.m_dwColor = D3DCOLOR_ARGB( 0, iRed, iGreen, iBlue );
	m_CatchKillBlind.m_iMaxAlpha = kLoader.LoadInt_e( "catch_kill_max_alpha", 0 );

	iRed   = kLoader.LoadInt_e( "catch_death_color_r", 0 );
	iGreen = kLoader.LoadInt_e( "catch_death_color_g", 0 );
	iBlue  = kLoader.LoadInt_e( "catch_death_color_b", 0 );
	m_CatchDeathBlind.m_dwColor = D3DCOLOR_ARGB( 0, iRed, iGreen, iBlue );
	m_CatchDeathBlind.m_iMaxAlpha = kLoader.LoadInt_e( "catch_death_max_alpha", 0 );

	m_CatchKillBlindTime.m_dwUpTime     = kLoader.LoadInt_e( "catch_kill_blind_up_time", 0 );
	m_CatchKillBlindTime.m_dwCenterTime = max( 1, kLoader.LoadInt_e( "catch_kill_blind_center_time", 1 ) );
	m_CatchKillBlindTime.m_dwDownTime   = max( 1, kLoader.LoadInt_e( "catch_kill_blind_down_time", 1 ) );

	m_CatchDeathBlindTime.m_dwUpTime     = kLoader.LoadInt_e( "catch_death_blind_up_time", 0 );
	m_CatchDeathBlindTime.m_dwCenterTime = max( 1, kLoader.LoadInt_e( "catch_death_blind_center_time", 1 ) );
	m_CatchDeathBlindTime.m_dwDownTime   = max( 1, kLoader.LoadInt_e( "catch_death_blind_down_time", 1 ) );

	m_dwCatchKillCameraEvent  = kLoader.LoadInt_e( "catch_kill_camera_event", 1 );
	m_dwCatchDeathCameraEvent = kLoader.LoadInt_e( "catch_death_camera_event", 1 );

	m_dwStartSelectCharCameraEvent = kLoader.LoadInt_e( "start_select_char_camera_event", 0 );

	iRed   = kLoader.LoadInt_e( "fixed_color_r", 0 );
	iGreen = kLoader.LoadInt_e( "fixed_color_g", 0 );
	iBlue  = kLoader.LoadInt_e( "fixed_color_b", 0 );
	m_FixedBlind.m_dwColor = D3DCOLOR_ARGB( 0, iRed, iGreen, iBlue );
	m_FixedBlind.m_iMaxAlpha = kLoader.LoadInt_e( "fixed_max_alpha", 0 );

	m_FixedBlindTime.m_dwUpTime     = kLoader.LoadInt_e( "fixed_blind_up_time", 0 );
	m_FixedBlindTime.m_dwCenterTime = max( 1, kLoader.LoadInt_e( "fixed_blind_center_time", 1 ) );
	m_FixedBlindTime.m_dwDownTime   = max( 1, kLoader.LoadInt_e( "fixed_blind_down_time", 1 ) );

	kLoader.SetTitle_e( "voice_chat" );
	m_eVoiceChatType = (VoiceChatType) kLoader.LoadInt_e( "type", 0 );
	ErrorReport::SetPosition( 1100, 9 );

	g_MyInfo.LoadINIInfo( kLoader );

	ErrorReport::SetPosition( 1100, 10 );

	ioINILoader_e kBuff( "config/sp2_buff.ini" );
	ioBaseChar::InitStaticINIValue( Help::GetCharLoader(), kBuff );
	ioBaseChar::InitOwnerMark();

	ErrorReport::SetPosition( 1100, 11 );

	InitGUI();

	ErrorReport::SetPosition( 1100, 12 );

	InitUIImages();

	ErrorReport::SetPosition( 1100, 13 );

#ifndef SHIPPING
	if( !P2PNetwork::IsNetworkPlaying() )
	{
		ModeType eMode = (ModeType)Setting::GetReservedMode();
		m_iModeSubNum   = Setting::GetModeSub();
		m_iModeMapIndex = Setting::GetModeMap();
		ChangePlayMode( eMode, PT_NONE );
		EnableWorldEntityFunctions( true );
	}
#endif

	m_UserSyncTimer.InitTimer();
	m_PushStructTimer.InitTimer();
	m_P2PPingTimer.InitTimer();
	m_BallStructTimer.InitTimer();

	kLoader.SetFileName_e( "config/sp2_item.ini" );
	m_fPickItemRange = kLoader.LoadFloat_e( "common_info", "item_pick_range", 50.0f );

	ErrorReport::SetPosition( 1100, 15 );

	g_RouletteMgr.SetPlayStage( this );
	
	g_RequestCharMgr.ClearData();
	return true;
}

void ioPlayStage::InitUIImages()
{
	ioINILoader_e kLoader( "config/sp2_gui.ini" );

	char szBuf[MAX_PATH] = "";

	kLoader.SetTitle_e( "getup_stick" );
	DWORD dwFrameTime = kLoader.LoadInt_e( "frame_time", 200 );

	m_pGetupStick = new ioAnimate2DImageRender;
	m_pGetupStick->SetFrameTime( dwFrameTime );
	m_pGetupStick->SetAnimateType( ioAnimate2DImageRender::FT_ZIGZAG );

	kLoader.LoadString_e( "getup_stick1_left", "", szBuf, MAX_PATH );
	m_pGetupStick->AddImageByFullName( szBuf );
	kLoader.LoadString_e( "getup_stick1_right", "", szBuf, MAX_PATH );
	m_pGetupStick->AddImageByFullName( szBuf );
	m_pGetupStick->SetHorzAlign( "Center" );

	m_pGetupStick2 = new ioAnimate2DImageRender;
	m_pGetupStick2->SetFrameTime( dwFrameTime );
	m_pGetupStick2->SetAnimateType( ioAnimate2DImageRender::FT_ZIGZAG );

	kLoader.LoadString_e( "getup_stick2_left", "", szBuf, MAX_PATH );
	m_pGetupStick2->AddImageByFullName( szBuf );
	kLoader.LoadString_e( "getup_stick2_right", "", szBuf, MAX_PATH );
	m_pGetupStick2->AddImageByFullName( szBuf );
	m_pGetupStick2->SetHorzAlign( "Center" );

	kLoader.SetTitle_e( "press_button" );
	
	dwFrameTime = kLoader.LoadInt_e( "frame_time", 200 );

	m_pPressButton = new ioAnimate2DImageRender;

	m_pPressButton->SetFrameTime( dwFrameTime );
	m_pPressButton->SetAnimateType( ioAnimate2DImageRender::FT_DEFAULT );

	kLoader.LoadString_e( "press_button_up", "", szBuf, MAX_PATH );
	m_pPressButton->AddImageByFullName( szBuf );
	kLoader.LoadString_e( "press_button_down", "", szBuf, MAX_PATH );
	m_pPressButton->AddImageByFullName( szBuf );
	m_pPressButton->SetHorzAlign( "Center" );
	m_pPressButton->SetVertAlign( "Center" );

	kLoader.SetTitle_e( "char_mouse_over" );
	m_CharOverColor.r = kLoader.LoadInt_e( "over_outline_r", 0 ) / 255.0f;
	m_CharOverColor.g = kLoader.LoadInt_e( "over_outline_g", 0 ) / 255.0f;
	m_CharOverColor.b = kLoader.LoadInt_e( "over_outline_b", 0 ) / 255.0f;
	m_fCharOverThickness = kLoader.LoadFloat_e( "over_outline_thickness", 1.5f );

	kLoader.SetTitle_e( "push_struct_mouse_over" );
	m_PushOverColor.r = kLoader.LoadInt_e( "over_outline_r", 0 ) / 255.0f;
	m_PushOverColor.g = kLoader.LoadInt_e( "over_outline_g", 0 ) / 255.0f;
	m_PushOverColor.b = kLoader.LoadInt_e( "over_outline_b", 0 ) / 255.0f;
	m_fPushOverThickness = kLoader.LoadFloat_e( "over_outline_thickness", 0.0f );
}

void ioPlayStage::LoadMap( ioINILoader &rkLoader )
{
	m_pSceneMgr->LoadMeshMap( rkLoader, this );	

	rkLoader.SetTitle( "map" );
	m_fMapFriction = rkLoader.LoadFloat_e( "map_friction", 400.0f );
	m_fMapGravity  = rkLoader.LoadFloat_e( "map_gravity", 800.0f );

	m_vMapCenter.x = rkLoader.LoadFloat_e( "center_x", 0.0f );
	m_vMapCenter.z = rkLoader.LoadFloat_e( "center_z", 0.0f );
	m_vMapCenter.y = GetMapHeight( m_vMapCenter.x, m_vMapCenter.z );

	int iRed, iGreen, iBlue;
	rkLoader.SetTitle_e( "clear_color" );
	iRed   = rkLoader.LoadInt( "red", 0 );
	iGreen = rkLoader.LoadInt_e( "green", 0 );
	iBlue  = rkLoader.LoadInt_e( "blue", 0 );
	m_dwMapClearColor = D3DCOLOR_ARGB( 255, iRed, iGreen, iBlue );

	LoadItemCreatePos( rkLoader );
	LoadMapEffectList( rkLoader );
	LoadMapObject( rkLoader );
	LoadCameraFixedMapObject( rkLoader );
	LoadCustomMapObject( rkLoader );
	LoadCustomMapEffect( rkLoader );

	if( Setting::ShowMapEffect() )
	{
		EnableMapDefaultEffect( true );
	}

	SetMapDefaultObjectQuality( Setting::GetMapObjectQuality() );
}

void ioPlayStage::LoadItemCreatePos( ioINILoader &rkLoader )
{
	m_vItemCreatePosList.clear();
	m_vItemShufflePosList.clear();

	rkLoader.SetTitle_e( "item_generate" );

	char szBuf[MAX_PATH];
	int iPosCnt = rkLoader.LoadInt_e( "pos_cnt", 0 );
	for( int i=0 ; i<iPosCnt ; i++ )
	{
		D3DXVECTOR3 vPos;

		wsprintf_e( szBuf, "pos%d_x", i+1 );
		vPos.x = rkLoader.LoadFloat( szBuf, 0.0f );

		wsprintf_e( szBuf, "pos%d_z", i+1 );
		vPos.z = rkLoader.LoadFloat( szBuf, 0.0f );

		vPos.y = GetMapHeight( vPos.x, vPos.z );
		m_vItemCreatePosList.push_back( vPos );
	}
}

void ioPlayStage::LoadMapEffectList( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "effect_common" );

	char szCommonFile[MAX_PATH];
	rkLoader.LoadString_e( "common_file", "none", szCommonFile, MAX_PATH );

	m_MapDefaultEffectList.clear();

	int iTotalEffect = rkLoader.LoadInt_e( "total_count", 0 );
	for( int i=0 ; i<iTotalEffect ; i++ )
	{
		MapDefaultEffect kMapEffect;

		char szBuf[MAX_PATH];
		wsprintf_e( szBuf, "effect%d", i+1 );
		rkLoader.SetTitle( szBuf );

		kMapEffect.m_iEffectIndex = i + 1;

		kMapEffect.m_vPos.x = rkLoader.LoadFloat_e( "pos_x", 0.0f );
		kMapEffect.m_vPos.y = rkLoader.LoadFloat_e( "pos_y", 0.0f );
		kMapEffect.m_vPos.z = rkLoader.LoadFloat_e( "pos_z", 0.0f );

		rkLoader.LoadString_e( "effect_file", "none", szBuf, MAX_PATH );
		if( !strcmp( szBuf, "none" ) )
		{
			kMapEffect.m_FileName = szCommonFile;
		}
		else
		{
			kMapEffect.m_FileName = szBuf;
		}

		kMapEffect.m_UniqueID = -1;

		// 캐릭터와 그리는 우선순위 체크 : 1은 체크 0은 체크 안함. 기본값은 체크 안함이며 캐릭터와 겹칠때 어색한 부분이 발생하면 체크함.
		if( rkLoader.LoadBool_e( "check_render_priority", false ) )
			kMapEffect.m_eRenderGroupID = RENDER_MAIN;
		else
			kMapEffect.m_eRenderGroupID = RENDER_GROUP_3;

		m_MapDefaultEffectList.push_back( kMapEffect );
	}
}

void ioPlayStage::LoadMapObject( ioINILoader &rkLoader )
{
	m_MapDefaultObjectList.clear();

	int iTotalObject = rkLoader.LoadInt_e( "object_common", "total_count", 0 );
	if( iTotalObject <= 0 )	return;

	m_MapDefaultObjectList.reserve( iTotalObject );

	char szBuf[MAX_PATH];

	rkLoader.SetTitle_e( "object_common" );
	for( int i=0 ; i<iTotalObject ; i++ )
	{
		wsprintf_e( szBuf, "object%d", i+1 );
		rkLoader.SetTitle( szBuf );

		MapDefaultObject kMapObject;
		kMapObject.m_iMultiMapIndex = rkLoader.LoadInt_e( "multi_map_index", 0 );
		kMapObject.m_vPos.x = rkLoader.LoadFloat_e( "pos_x", 0.0f );
		kMapObject.m_vPos.y = rkLoader.LoadFloat_e( "pos_y", 0.0f );
		kMapObject.m_vPos.z = rkLoader.LoadFloat_e( "pos_z", 0.0f );

		rkLoader.LoadString_e( "model_file", "none", szBuf, MAX_PATH );
		if( !strcmp( szBuf, "none" ) )	continue;

		kMapObject.m_FileName		 = szBuf;
		kMapObject.m_Quality		 = (MapObjectQuality)rkLoader.LoadInt_e( "object_quality_level", MQ_LOW );
		kMapObject.m_iAnimationIndex = rkLoader.LoadInt_e( "animation_idx", -1 );
		kMapObject.m_UniqueID		 = -1;	// Not Loaded
		kMapObject.m_bShadowCast	 = rkLoader.LoadBool_e( "shadow_cast", false );

		kMapObject.m_bUseChangeAlphaCheck = rkLoader.LoadBool_e( "use_change_alpha", false );
		kMapObject.m_iChangeAlphaRate = rkLoader.LoadInt_e( "change_alpha_rate", MAX_ALPHA_RATE );

		// 캐릭터와 그리는 우선순위 체크 : 1은 체크 0은 체크 안함. 기본값은 체크 안함이며 캐릭터와 겹칠때 어색한 부분이 발생하면 체크함.
		if( rkLoader.LoadBool_e( "check_render_priority", false ) )
			kMapObject.m_eRenderGroupID = RENDER_MAIN;
		else
			kMapObject.m_eRenderGroupID = RENDER_GROUP_3;

		m_MapDefaultObjectList.push_back( kMapObject );
	}
}

void ioPlayStage::LoadCameraFixedMapObject( ioINILoader &rkLoader )
{
	m_MapCameraFixedObjectList.clear();

	int iTotalObject = rkLoader.LoadInt_e( "object_camera_fixed", "total_count", 0 );
	if( iTotalObject <= 0 )	return;

	m_MapCameraFixedObjectList.reserve( iTotalObject );

	char szBuf[MAX_PATH];

	rkLoader.SetTitle_e( "object_camera_fixed" );
	for( int i=0 ; i<iTotalObject ; i++ )
	{
		wsprintf_e( szBuf, "object_camera_fixed%d", i+1 );
		rkLoader.SetTitle( szBuf );

		MapCameraFixedObject kMapObject;

		kMapObject.m_vPos.x = rkLoader.LoadFloat_e( "pos_x", 0.0f );
		kMapObject.m_vPos.y = rkLoader.LoadFloat_e( "pos_y", 0.0f );
		kMapObject.m_vPos.z = rkLoader.LoadFloat_e( "pos_z", 0.0f );

		rkLoader.LoadString_e( "model_file", "none", szBuf, MAX_PATH );
		if( !strcmp( szBuf, "none" ) )	continue;

		kMapObject.m_FileName		 = szBuf;
		kMapObject.m_UniqueID		 = -1;	// Not Loaded
		kMapObject.m_bShadowCast	 = rkLoader.LoadBool_e( "shadow_cast", false );

		m_MapCameraFixedObjectList.push_back( kMapObject );
	}
}

void ioPlayStage::LoadCustomMapObject( ioINILoader &rkLoader )
{
	m_MapCustomObjectList.clear();

	int iTotalObject = rkLoader.LoadInt_e( "costom_object_common", "total_count", 0 );
	if( iTotalObject <= 0 )	return;

	m_MapCustomObjectList.reserve( iTotalObject );

	char szBuf[MAX_PATH];
	for(int i = 0;i < iTotalObject;i++)
	{
		sprintf_e( szBuf, "costom_object%d", i+1 );
		rkLoader.SetTitle( szBuf );

		MapCustomObject kMapObject;

		kMapObject.m_vPos.x = rkLoader.LoadFloat_e( "pos_x", 0.0f );
		kMapObject.m_vPos.y = rkLoader.LoadFloat_e( "pos_y", 0.0f );
		kMapObject.m_vPos.z = rkLoader.LoadFloat_e( "pos_z", 0.0f );

		rkLoader.LoadString_e( "model_file", "none", szBuf, MAX_PATH );
		if( !strcmp( szBuf, "none" ) )	continue;

		kMapObject.m_FileName		 = szBuf;
		kMapObject.m_UniqueID		 = -1;	// Not Loaded
		kMapObject.m_bShadowCast	 = rkLoader.LoadBool_e( "shadow_cast", false );
		kMapObject.m_iCustomIndex    = i + 1;

		// 캐릭터와 그리는 우선순위 체크 : 1은 체크 0은 체크 안함. 기본값은 체크 안함이며 캐릭터와 겹칠때 어색한 부분이 발생하면 체크함.
		if( rkLoader.LoadBool_e( "check_render_priority", false ) )
			kMapObject.m_eRenderGroupID = RENDER_MAIN;
		else
			kMapObject.m_eRenderGroupID = RENDER_GROUP_3;

		m_MapCustomObjectList.push_back( kMapObject );
	}
}

void ioPlayStage::LoadCustomMapEffect( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "costom_effect_common" );

	char szCommonFile[MAX_PATH];
	rkLoader.LoadString_e( "common_file", "none", szCommonFile, MAX_PATH );

	m_MapCustomEffectList.clear();

	int iTotalEffect = rkLoader.LoadInt_e( "total_count", 0 );
	for( int i=0 ; i<iTotalEffect ; i++ )
	{
		MapCustomEffect kMapEffect;

		char szBuf[MAX_PATH];
		wsprintf_e( szBuf, "costom_effect%d", i+1 );
		rkLoader.SetTitle( szBuf );

		kMapEffect.m_vPos.x = rkLoader.LoadFloat_e( "pos_x", 0.0f );
		kMapEffect.m_vPos.y = rkLoader.LoadFloat_e( "pos_y", 0.0f );
		kMapEffect.m_vPos.z = rkLoader.LoadFloat_e( "pos_z", 0.0f );

		rkLoader.LoadString_e( "effect_file", "none", szBuf, MAX_PATH );
		if( !strcmp( szBuf, "none" ) )
		{
			kMapEffect.m_FileName = szCommonFile;
		}
		else
		{
			kMapEffect.m_FileName = szBuf;
		}

		kMapEffect.m_UniqueID = -1;

		// 캐릭터와 그리는 우선순위 체크 : 1은 체크 0은 체크 안함. 기본값은 체크 안함이며 캐릭터와 겹칠때 어색한 부분이 발생하면 체크함.
		if( rkLoader.LoadBool_e( "check_render_priority", false ) )
			kMapEffect.m_eRenderGroupID = RENDER_MAIN;
		else
			kMapEffect.m_eRenderGroupID = RENDER_GROUP_3;

		kMapEffect.m_iCustomIndex = i + 1;
		m_MapCustomEffectList.push_back( kMapEffect );
	}
}

void ioPlayStage::LoadModeCustomBlind( ioINILoader &rkLoader )
{
	rkLoader.SetTitle_e( "screen_blind" );
	if( rkLoader.LoadBool_e( "custom_kill", false ) )
	{
		int iRed   = rkLoader.LoadInt_e( "kill_color_r", 0 );
		int iGreen = rkLoader.LoadInt_e( "kill_color_g", 0 );
		int iBlue  = rkLoader.LoadInt_e( "kill_color_b", 0 );
		m_KillBlind.m_dwColor = D3DCOLOR_ARGB( 0, iRed, iGreen, iBlue );
		m_KillBlind.m_iMaxAlpha = rkLoader.LoadInt_e( "kill_max_alpha", 0 );
		m_KillBlindTime.m_dwUpTime     = rkLoader.LoadInt_e( "kill_blind_up_time", 0 );
		m_KillBlindTime.m_dwCenterTime = max( 1, rkLoader.LoadInt_e( "kill_blind_center_time", 1 ) );
		m_KillBlindTime.m_dwDownTime   = max( 1, rkLoader.LoadInt_e( "kill_blind_down_time", 1 ) );
		m_dwKillCameraEvent  = rkLoader.LoadInt_e( "kill_camera_event", 1 );
	}
}

void ioPlayStage::EnableMapDefaultEffect( bool bEnable )
{
	if( bEnable )
	{
		MapDefaultEffectList::iterator iter=m_MapDefaultEffectList.begin();
		for( ; iter!=m_MapDefaultEffectList.end() ; ++iter )
		{
			MapDefaultEffect &rkEffect = *iter;

			if( rkEffect.m_UniqueID != -1 )
				continue;

			ioMapEffect *pMap = CreateMapEffect( rkEffect.m_FileName, rkEffect.m_vPos, ioMath::UNIT_ALL );
			if( pMap )
			{
				rkEffect.m_UniqueID = pMap->GetUniqueID();
				pMap->SetRenderQueueGroup( rkEffect.m_eRenderGroupID );
			}
		}
	}
	else
	{
		MapDefaultEffectList::iterator iter=m_MapDefaultEffectList.begin();
		for( ; iter!=m_MapDefaultEffectList.end() ; ++iter )
		{
			MapDefaultEffect &rkEffect = *iter;
			if( rkEffect.m_UniqueID == -1 )
				continue;

			DestroyMapEffect( rkEffect.m_UniqueID );
			rkEffect.m_UniqueID = -1;
		}
	}
}

void ioPlayStage::EnableMapDefaultEffect( bool bEnable, int iEffectIndex )
{
	int iCheckIndex = 1;
	if( bEnable )
	{
		MapDefaultEffectList::iterator iter=m_MapDefaultEffectList.begin();
		for( ; iter!=m_MapDefaultEffectList.end() ; ++iter )
		{
			MapDefaultEffect &rkEffect = *iter;

			if( rkEffect.m_UniqueID != -1 )
				continue;

			if( rkEffect.m_iEffectIndex != iEffectIndex )			// 해당 이펙트의 인덱스가 없다면 나감
				return;

			ioMapEffect *pMap = CreateMapEffect( rkEffect.m_FileName, rkEffect.m_vPos, ioMath::UNIT_ALL );
			if( pMap )
			{
				rkEffect.m_UniqueID = pMap->GetUniqueID();
				pMap->SetRenderQueueGroup( rkEffect.m_eRenderGroupID );
			}
		}
	}
	else
	{
		MapDefaultEffectList::iterator iter=m_MapDefaultEffectList.begin();
		for( ; iter!=m_MapDefaultEffectList.end() ; ++iter )
		{
			MapDefaultEffect &rkEffect = *iter;
			if( rkEffect.m_UniqueID == -1 )
				continue;

			if( rkEffect.m_iEffectIndex != iEffectIndex )			// 해당 이펙트의 인덱스가 없다면 나감
				return;

			DestroyMapEffect( rkEffect.m_UniqueID );
			rkEffect.m_UniqueID = -1;
		}
	}
}

void ioPlayStage::SetMapDefaultObjectQuality( MapObjectQuality eQuality )
{
	if( m_pSceneMgr == NULL ) return;

	/************************************************************************/
	/* 오브젝트 활성화 방식                                                 */
	/* 현재 맵 인덱스에서 +1 / -1 인덱스의 맵 오브젝트만 활성화 시킨다.     */
	/************************************************************************/		
	int iCurrentMapIndex = m_pSceneMgr->GetMultiMapCurrentIndex();
	MapDefaultObjectList::iterator iter = m_MapDefaultObjectList.begin();
	for(;iter != m_MapDefaultObjectList.end();++iter)
	{
		MapDefaultObject &rkObject = *iter;		
		if( rkObject.m_Quality > eQuality || iCurrentMapIndex != rkObject.m_iMultiMapIndex )	// 소멸
		{
			if( rkObject.m_UniqueID == -1 )	continue;

			GameEntityList::iterator iGame = m_GameEntityList.begin();
			for( ; iGame != m_GameEntityList.end() ; ++iGame )
			{
				ioWorldDecoration *pDeco = ToWorldDecoration( *iGame );
				if( pDeco && pDeco->GetUniqueID() == rkObject.m_UniqueID )
				{
					EraseGameEntity( iGame );
					rkObject.m_UniqueID = -1;
					break;
				}
			}
		}
		else	// 생성
		{
			if( rkObject.m_UniqueID != -1 )	continue;

			ioWorldDecoration *pDeco = CreateWorldDecoration( rkObject.m_FileName );
			if( !pDeco )	continue;

			pDeco->SetWorldPosition( rkObject.m_vPos );
			pDeco->SetShadowCastEnable( rkObject.m_bShadowCast );
			pDeco->SetRenderGroup( rkObject.m_eRenderGroupID );

			pDeco->SetCreator( this );
			pDeco->SetChangeAlphaRate( rkObject.m_bUseChangeAlphaCheck, rkObject.m_iChangeAlphaRate );

			if( rkObject.m_iAnimationIndex >= 0 )
			{
				pDeco->SetLoopAni( rkObject.m_iAnimationIndex );
			}

			rkObject.m_UniqueID = pDeco->GetUniqueID();
		}
	}
}

void ioPlayStage::SetMapCameraFixedObject( bool bFixed )
{
	MapCameraFixedObjectList::iterator iter=m_MapCameraFixedObjectList.begin();
	for( ; iter!=m_MapCameraFixedObjectList.end() ; ++iter )
	{
		MapCameraFixedObject &rkObject = *iter;

		if( !bFixed )	// 소멸
		{
			if( rkObject.m_UniqueID == -1 )
				continue;

			GameEntityList::iterator iGame = m_GameEntityList.begin();
			for( ; iGame != m_GameEntityList.end() ; ++iGame )
			{
				ioWorldDecoration *pDeco = ToWorldDecoration( *iGame );
				if( pDeco && pDeco->GetUniqueID() == rkObject.m_UniqueID )
				{
					EraseGameEntity( iGame );
					rkObject.m_UniqueID = -1;
					break;
				}
			}
		}
		else	// 생성
		{
			if( rkObject.m_UniqueID != -1 )
				continue;

			ioWorldDecoration *pDeco = CreateWorldDecoration( rkObject.m_FileName );
			if( !pDeco )	continue;

			pDeco->SetWorldPosition( rkObject.m_vPos );
			pDeco->SetShadowCastEnable( rkObject.m_bShadowCast );
			pDeco->SetRenderGroup( RENDER_GROUP_3 );

			rkObject.m_UniqueID = pDeco->GetUniqueID();
		}
	}
}

void ioPlayStage::SetMapCustomObject( bool bShow, int iCustomIndex )
{
	MapCustomObjectList::iterator iter = m_MapCustomObjectList.begin();
	for(;iter != m_MapCustomObjectList.end();++iter)
	{
		MapCustomObject &rkObject = *iter;

		if( iCustomIndex != -1 && rkObject.m_iCustomIndex != iCustomIndex ) continue;

		if( !bShow )	// 소멸
		{
			if( rkObject.m_UniqueID == -1 )
				continue;

			GameEntityList::iterator iGame = m_GameEntityList.begin();
			for( ; iGame != m_GameEntityList.end() ; ++iGame )
			{
				ioWorldDecoration *pDeco = ToWorldDecoration( *iGame );
				if( pDeco && pDeco->GetUniqueID() == rkObject.m_UniqueID )
				{
					EraseGameEntity( iGame );
					rkObject.m_UniqueID = -1;
					break;
				}
			}
		}
		else	// 생성
		{
			if( rkObject.m_UniqueID != -1 )
				continue;

			ioWorldDecoration *pDeco = CreateWorldDecoration( rkObject.m_FileName );
			if( !pDeco )	continue;

			pDeco->SetWorldPosition( rkObject.m_vPos );
			pDeco->SetShadowCastEnable( rkObject.m_bShadowCast );
			pDeco->SetRenderGroup( rkObject.m_eRenderGroupID );
			rkObject.m_UniqueID = pDeco->GetUniqueID();
		}
	}
}

void ioPlayStage::SetMapCustomEffect( bool bShow, int iCustomIndex )
{
	MapCustomEffectList::iterator iter=m_MapCustomEffectList.begin();
	for( ; iter!=m_MapCustomEffectList.end() ; ++iter )
	{
		MapCustomEffect &rkEffect = *iter;

		if( iCustomIndex != -1 && rkEffect.m_iCustomIndex != iCustomIndex ) continue;

		if( bShow )   //생성
		{
			if( rkEffect.m_UniqueID != -1 )
				continue;

			ioMapEffect *pMap = CreateMapEffect( rkEffect.m_FileName, rkEffect.m_vPos, ioMath::UNIT_ALL );
			if( pMap )
			{
				rkEffect.m_UniqueID = pMap->GetUniqueID();
				pMap->SetRenderQueueGroup( rkEffect.m_eRenderGroupID );
			}
		}
		else        // 소멸
		{
			if( rkEffect.m_UniqueID == -1 )
				continue;

			DestroyMapEffect( rkEffect.m_UniqueID );
			rkEffect.m_UniqueID = -1;
		}
	}
}

void ioPlayStage::StartBuffBlind( const ioHashString &szBlind, const ScreenBlindTime *pTime, int iMaxAlpha )
{
	SAFEDELETE( m_pBuffBlind );

	if( szBlind.IsEmpty() ) return;

	m_pBuffBlind = g_UIImageSetMgr.CreateImageByFullName( szBlind );
	if( !m_pBuffBlind  ) return;

	m_BuffBlindTime = *pTime;
	m_iBuffBlindMaxAlpha = iMaxAlpha;

	if( m_iBuffBlindMaxAlpha == 0 )
		return;

	if( m_BuffBlindTime.m_dwUpTime > 0 )
		m_BuffBlindState = SBS_UP;
	else
		m_BuffBlindState = SBS_CENTER;

	m_dwBuffBlindCurTime = FRAMEGETTIME();
}

void ioPlayStage::EndBuffBlind()
{
	if( m_BuffBlindState == SBS_NONE || !m_pSceneMgr )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	if( m_BuffBlindState != SBS_DOWN )
	{
		m_BuffBlindState = SBS_DOWN;
		m_dwBuffBlindCurTime = dwCurTime;
	}
}

void ioPlayStage::ProcessBuffBlind()
{
	if( m_BuffBlindState == SBS_NONE || !m_pSceneMgr )
		return;

	int iAlpha = 0;
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwBuffBlindCurTime;

	switch( m_BuffBlindState )
	{
	case SBS_UP:
		if( dwGapTime <= m_BuffBlindTime.m_dwUpTime )
		{
			float fRate = (float)dwGapTime / (float)m_BuffBlindTime.m_dwUpTime;
			iAlpha = max( m_iBuffBlindMaxAlpha * fRate, 1 );
		}
		else
		{
			m_BuffBlindState = SBS_CENTER;
			m_dwBuffBlindCurTime = dwCurTime;

			iAlpha = m_iBuffBlindMaxAlpha;
		}
		break;
	case SBS_CENTER:
		iAlpha = m_iBuffBlindMaxAlpha;

		if( dwGapTime > m_BuffBlindTime.m_dwCenterTime )
		{
			m_BuffBlindState = SBS_DOWN;
			m_dwBuffBlindCurTime = dwCurTime;
		}
		break;
	case SBS_DOWN:
		if( dwGapTime <= m_BuffBlindTime.m_dwDownTime )
		{
			float fRate = (float)dwGapTime / (float)m_BuffBlindTime.m_dwDownTime;
			iAlpha = m_iBuffBlindMaxAlpha * ( FLOAT1 - fRate );
		}
		else
		{
			m_BuffBlindState = SBS_NONE;
			m_dwBuffBlindCurTime = 0;
			iAlpha = 0;
		}
		break;
	}

	if(  m_pBuffBlind )
	{
		if( iAlpha )
		{
			m_pBuffBlind->SetAlpha( iAlpha );
		}
		else
		{
			m_BuffBlindState = SBS_NONE;
			m_dwBuffBlindCurTime = 0;
		}
	}
}

void ioPlayStage::StartScreenBlind( ScreenBlindType eType, const ScreenBlindTime *pTime, bool bCameraEvent )
{
	if( eType == SBT_SKILL && !pTime )	// 스킬은 시간을 외부에서 설정
		return;

	if( m_ScreenBlindType == SBT_FIXED_BLIND )     // 다른 블라인드로 부터 간섭 받지 않는다.
		return;

	if( m_ScreenBlindType == SBT_BUFF && eType == SBT_SKILL )	// 스킬에 의해 버프 블라인트 간섭받지 않게.
		return;

	DWORD dwEventCode = 0;
	switch( eType )
	{
	case SBT_SKILL:
		m_CurBlindSetting = m_SkillBlind;
		m_CurBlindTime = *pTime;
		break;
	case SBT_KILL:
		m_CurBlindSetting = m_KillBlind;
		m_CurBlindTime    = m_KillBlindTime;
		dwEventCode		  = m_dwKillCameraEvent;
		break;
	case SBT_DEATH:
		m_CurBlindSetting = m_DeathBlind;
		m_CurBlindTime    = m_DeathBlindTime;
		dwEventCode		  = m_dwDeathCameraEvent;
		break;
	case SBT_CATCH_KILL:
		m_CurBlindSetting = m_CatchKillBlind;
		m_CurBlindTime	  = m_CatchKillBlindTime;
		dwEventCode		  = m_dwCatchKillCameraEvent;
		break;
	case SBT_CATCH_DEATH:
		m_CurBlindSetting = m_CatchDeathBlind;
		m_CurBlindTime	  = m_CatchDeathBlindTime;
		dwEventCode		  = m_dwCatchDeathCameraEvent;
		break;
	case SBT_FIXED_BLIND:
		m_CurBlindSetting = m_FixedBlind;
		m_CurBlindTime	  = m_FixedBlindTime;
		break;
	default:
		m_CurBlindSetting.m_iMaxAlpha = 0;	/// 탈출용
		break;
	}

	if( m_CurBlindSetting.m_iMaxAlpha == 0 )
		return;

	m_ScreenBlindType = eType;
	if( m_CurBlindTime.m_dwUpTime > 0 )
		m_ScreenBlindState = SBS_UP;
	else
		m_ScreenBlindState = SBS_CENTER;

	m_dwScreenBlindCurTime = FRAMEGETTIME();

	if( bCameraEvent && dwEventCode > 0 )
	{
		g_CameraEventMgr.CreateCE( dwEventCode, this );
	}
}

void ioPlayStage::StartScreenBlindSkill( const ScreenBlindTime *pTime, const ScreenBlindSetting *pSetting, bool bCameraEvent )
{
	if( !pTime )
		return;

	if( m_ScreenBlindType == SBT_FIXED_BLIND )     // 다른 블라인드로 부터 간섭 받지 않는다.
		return;

	if( m_ScreenBlindType == SBT_BUFF )	// 스킬에 의해 버프 블라인트 간섭받지 않게.
		return;

	DWORD dwEventCode = 0;

	if( pSetting )
		m_CurBlindSetting = *pSetting;
	else
		m_CurBlindSetting = m_SkillBlind;

	m_CurBlindTime = *pTime;

	if( m_CurBlindSetting.m_iMaxAlpha == 0 )
		return;

	m_ScreenBlindType = SBT_SKILL;

	if( m_CurBlindTime.m_dwUpTime > 0 )
		m_ScreenBlindState = SBS_UP;
	else
		m_ScreenBlindState = SBS_CENTER;

	m_dwScreenBlindCurTime = FRAMEGETTIME();

	if( bCameraEvent && dwEventCode > 0 )
	{
		g_CameraEventMgr.CreateCE( dwEventCode, this );
	}
}

void ioPlayStage::EndScreenBlind()
{
	m_ScreenBlindType  = SBT_NONE;
	m_ScreenBlindState = SBS_NONE;
	m_dwScreenBlindCurTime = 0;
	m_fScreenBlindBuffRange = 0.0f;

	if( m_pSceneMgr )
	{
		m_pSceneMgr->SetScreenBlind( 0x0 );
	}
}

void ioPlayStage::SetScreenBlindDownState()
{
	switch( m_ScreenBlindState )
	{
	case SBS_UP:
	case SBS_CENTER:
		m_ScreenBlindState = SBS_DOWN;
		m_dwScreenBlindCurTime = FRAMEGETTIME();
		break;
	}
}

void ioPlayStage::ProcessScreenBlind()
{
	if( m_ScreenBlindState == SBS_NONE || !m_pSceneMgr )
		return;

	int iAlpha = 0;
	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwScreenBlindCurTime;

	switch( m_ScreenBlindState )
	{
	case SBS_UP:
		if( dwGapTime <= m_CurBlindTime.m_dwUpTime )
		{
			float fRate = (float)dwGapTime / (float)m_CurBlindTime.m_dwUpTime;
			iAlpha = max( m_CurBlindSetting.m_iMaxAlpha * fRate, 1 );
		}
		else
		{
			m_ScreenBlindState = SBS_CENTER;
			m_dwScreenBlindCurTime = dwCurTime;

			iAlpha = m_CurBlindSetting.m_iMaxAlpha;
		}
		break;
	case SBS_CENTER:
		iAlpha = m_CurBlindSetting.m_iMaxAlpha;

		if( dwGapTime > m_CurBlindTime.m_dwCenterTime )
		{
			m_ScreenBlindState = SBS_DOWN;
			m_dwScreenBlindCurTime = dwCurTime;
		}
		break;
	case SBS_DOWN:
		if( dwGapTime <= m_CurBlindTime.m_dwDownTime )
		{
			float fRate = (float)dwGapTime / (float)m_CurBlindTime.m_dwDownTime;
			iAlpha = m_CurBlindSetting.m_iMaxAlpha * ( FLOAT1 - fRate );
		}
		else
		{
			m_ScreenBlindState = SBS_NONE;
			m_dwScreenBlindCurTime = 0;
			iAlpha = 0;
		}
		break;
	}

	if( iAlpha > 0 )
	{
		DWORD dwFactor = (iAlpha<<24) | m_CurBlindSetting.m_dwColor;
		m_pSceneMgr->SetScreenBlind( dwFactor );
	}
	else
	{
		EndScreenBlind();
	}
}

void ioPlayStage::SetScreenRedFilter()
{
	if( m_pSceneMgr == NULL ) return;
	
	m_pSceneMgr->SetFilterAlphaValue( (float)MAX_ALPHA_RATE * 0.7f );
	m_pSceneMgr->EnableFilterProcess( true, "filter_red.fxo" );
	m_bEnableScreenRedFilter = true;
}

void ioPlayStage::ReloadScreenFilter()
{
	if( m_pSceneMgr == NULL ) return;
	if( !m_bEnableScreenRedFilter ) return;

	m_pSceneMgr->EnableFilter( Setting::GetFilterType() );
	m_bEnableScreenRedFilter = false;
}

void ioPlayStage::SetScreenOveray()
{
	if( m_pSceneMgr == NULL ) return;

	m_pSceneMgr->EnableOverayProcess( true, "channel2.dds", 200, 0, 0 );
	m_bEnableScreenOveray = true;
}

void ioPlayStage::ReloadScreenOveray()
{
	if( m_pSceneMgr == NULL ) return;
	if( !m_bEnableScreenOveray ) return;

	m_pSceneMgr->EnableOveray( Setting::GetOverayType() );
	m_bEnableScreenOveray = false;
}

void ioPlayStage::InitGUI()
{
	g_GUIMgr.CheckGUIPosSave();

	g_ChatMacroMgr.SetPlayStage( this );

	g_GUIMgr.AddWnd( "XML/ManualListWnd.xml" );
	
	VoiceChatWnd *pVoiceChatWnd = dynamic_cast<VoiceChatWnd*> ( g_GUIMgr.AddWnd( "XML/VoiceChatWnd.xml") );
	if( pVoiceChatWnd )
		pVoiceChatWnd->SetPlayStage( this );
	g_GUIMgr.AddWnd( "XML/VoiceChatOptionWnd.xml");

	g_GUIMgr.AddWnd( "XML/centerhelpwnd.xml" );
	g_GUIMgr.AddWnd( "XML/DisconnectWnd.xml" );
	g_GUIMgr.AddWnd( "XML/ResolutionConfirm.xml" );

	MessengerWnd *pMessengerWnd = dynamic_cast<MessengerWnd*>(g_GUIMgr.AddWnd( "XML/MessengerWnd.xml" ));
	if( pMessengerWnd )
		pMessengerWnd->SetPlayStage( this );

	PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.AddWnd( "XML/partyshambattlewnd.xml" ));
	if( pShamBattle )
		pShamBattle->SetPlayStage( this );

	g_GUIMgr.AddWnd( "XML/partyownerwnd.xml" );
	g_GUIMgr.AddWnd( "XML/partyinvitelistwnd.xml" );
	g_GUIMgr.AddWnd( "XML/MiddleJoinDelayWnd.xml" );
	g_GUIMgr.AddWnd( "XML/inviteprogresswnd.xml" );
	
	g_GUIMgr.AddWnd( "XML/partyoptionwnd.xml" );

	g_GUIMgr.AddWnd( "XML/MiniMap.xml" );
	g_GUIMgr.AddWnd( "XML/roulette/roulettecenterhelpwnd.xml" );

	//보너스 캐쉬
	g_GUIMgr.AddWnd( "XML/bonuscashwnd.xml" );

	SoldierLimitPopupWnd *pSoldierLimitPopup = dynamic_cast<SoldierLimitPopupWnd*>(g_GUIMgr.AddWnd( "XML/soldierlimitpopupwnd.xml" ));
	if( pSoldierLimitPopup )
		pSoldierLimitPopup->SetPlayStage( this );

	RentalSoldierLimitPopupWnd *pRentalSoldierLimitPopup = dynamic_cast<RentalSoldierLimitPopupWnd*>(g_GUIMgr.AddWnd( "XML/rentalsoldierlimitpopupwnd.xml" ));
	if( pRentalSoldierLimitPopup )
		pRentalSoldierLimitPopup->SetPlayStage( this );

	SoldierSelectWnd *pSoldierWnd = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.AddWnd( "XML/SoldierSelectWnd.xml" ));
	if(pSoldierWnd)
	{
		pSoldierWnd->SetPlayStage(this);
	}

	MyInventoryWnd *pMyInventoryWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.AddWnd( "XML/MyInventoryWnd.xml" ));
	if( pMyInventoryWnd )
	{
		pMyInventoryWnd->SetPlayStage( this );
	}
	g_GUIMgr.AddWnd( "XML/myinventorytooltip.xml" );
	g_GUIMgr.AddWnd( "XML/alchemicitemtooltip.xml" );

	NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*>( g_GUIMgr.AddWnd( "XML/newshopwnd.xml" ) );
	if( pNewShopWnd )
		pNewShopWnd->SetPlayStage( this );

	PracticeRankingWnd *pPracTiceRank = dynamic_cast<PracticeRankingWnd*>( g_GUIMgr.FindWnd(PRACTICE_RANKING_WND) );
	if(!pPracTiceRank)
		pPracTiceRank = dynamic_cast<PracticeRankingWnd*>( g_GUIMgr.AddWnd( "XML/Practice/PracticeRankingWnd.xml" ) );

	g_GUIMgr.AddWnd( "XML/newshoptooltip.xml" );
	g_GUIMgr.AddWnd( "XML/newshopneedlevelwnd.xml");
	g_GUIMgr.AddWnd( "XML/newshopneedpesownd.xml" );

	g_GUIMgr.AddWnd( "XML/soldierdismissalwnd.xml" );

	g_GUIMgr.AddWnd( "XML/GrowthSelectWnd.xml" );

	g_GUIMgr.AddWnd( "XML/ChangeTradeStateWnd.xml" );
	g_GUIMgr.AddWnd( "XML/ChangeTradeStateConfirmWnd.xml" );

	g_GUIMgr.AddWnd( "XML/TradeItemRegisterWnd.xml" );
	g_GUIMgr.AddWnd( "XML/TradeItemRegisterConfirmWnd.xml" );
	g_GUIMgr.AddWnd( "XML/TradeItemRegisterResultWnd.xml" );

	g_GUIMgr.AddWnd( "XML/TradeItemBuyWnd.xml" );
	g_GUIMgr.AddWnd( "XML/TradeItemBuyResultWnd.xml" );

	g_GUIMgr.AddWnd( "XML/TradeItemCancelWnd.xml" );
	g_GUIMgr.AddWnd( "XML/TradeItemCancelResultWnd.xml" );

	ExtraItemEquipConfirmWnd *pConfirmWnd = dynamic_cast<ExtraItemEquipConfirmWnd*>(g_GUIMgr.AddWnd( "XML/ExtraItemEquipConfirmWnd.xml" ));
	if( pConfirmWnd )
		pConfirmWnd->SetPlayStage( this );

	FishingInventoryWnd *pFishingInventoryWnd = dynamic_cast<FishingInventoryWnd*>(g_GUIMgr.AddWnd( "XML/FishingInventoryWnd.xml" ));
	if( pFishingInventoryWnd )
	{
		pFishingInventoryWnd->SetPlayStage( this );
	}
	g_GUIMgr.AddWnd( "XML/fishingbuywnd.xml" );

	GameOptionWnd *pGameOptionWnd = dynamic_cast<GameOptionWnd*>( g_GUIMgr.AddWnd( "XML/GameOptionWnd.xml" ) );
	if( pGameOptionWnd )
		pGameOptionWnd->SetPlayStage( this );

	g_GUIMgr.AddWnd( "XML/BriefingWnd.xml" );
	g_GUIMgr.AddWnd( "XML/PartyInfoWnd.xml" );
	g_GUIMgr.AddWnd( "XML/PartyInvitedWnd.xml" );

	g_GUIMgr.AddWnd( "XML/hackproblemwnd.xml" );
	g_GUIMgr.AddWnd( "XML/abuseproblemwnd.xml" );

	g_GUIMgr.AddWnd( "XML/DeveloperInfoWnd.xml" );
	g_GUIMgr.AddWnd( "XML/p2pdebugwnd.xml" );
	g_GUIMgr.AddWnd( "XML/framedebugwnd.xml" );
	g_GUIMgr.AddWnd( "XML/fonttexturetestwnd.xml" );

	HelpWnd *pHelpWnd = dynamic_cast<HelpWnd *>(g_GUIMgr.AddWnd( "XML/HelpWnd.xml" ));
	if( pHelpWnd ) 
	{
		pHelpWnd->SetPlayStage( this );
	}

	g_GUIMgr.AddWnd( "XML/battlequickstartoption.xml" );
	g_GUIMgr.AddWnd( "XML/MessengerPopup.xml" );
	g_GUIMgr.AddWnd( "XML/TimeGateSearch.xml" );
	g_GUIMgr.AddWnd( "XML/TimeGateSearchResult.xml" );
	g_GUIMgr.AddWnd( "XML/timegatesearchshuffle.xml" );
	g_GUIMgr.AddWnd( "XML/timegatesearchshuffleresult.xml" );

	/*UserInfoToolTip *pUserToolTip = dynamic_cast<UserInfoToolTip*>( g_GUIMgr.AddWnd( "XML/userinfotooltip.xml" ) );
	if( pUserToolTip )
		pUserToolTip->SetPlayStage( this );*/

	MannerValuationExtensionWnd *pManner = dynamic_cast<MannerValuationExtensionWnd *>( g_GUIMgr.AddWnd( "XML/MannerValuationExtensionWnd.xml" ) );
		if(pManner)
			pManner->SetPlayStage( this );	
	
	g_GUIMgr.AddWnd( "XML/mannervaluationchatwnd.xml" );

	KickVoteReasonWnd *pKickVoteReason = dynamic_cast<KickVoteReasonWnd *>( g_GUIMgr.AddWnd( "XML/KickVoteReasonWnd.xml" ) );
	if( pKickVoteReason )
		pKickVoteReason->SetPlayStage( this );

	KickVoteProgressWnd *pKickVoteProgress = dynamic_cast<KickVoteProgressWnd*>( g_GUIMgr.AddWnd( "XML/KickVoteProgressWnd.xml" ) ); 
	if( pKickVoteProgress )
		pKickVoteProgress->SetPlayStage( this );

	g_GUIMgr.AddWnd( "XML/mannertrialwnd.xml");
	g_GUIMgr.AddWnd( "XML/bankruptcycharwnd.xml");

	g_GUIMgr.AddWnd( "XML/friendrequestwnd.xml" );
	g_GUIMgr.AddWnd( "XML/friendapplicationwnd.xml" );
	g_GUIMgr.AddWnd( "XML/frienddeletewnd.xml" );

	g_GUIMgr.AddWnd( "XML/blacklistinsertwnd.xml" );
	g_GUIMgr.AddWnd( "XML/channelinvitewnd.xml" );
	
	g_GUIMgr.AddWnd( "XML/GuildCreateWnd.xml" );
	g_GUIMgr.AddWnd( "XML/guildeventpopup.xml" );
	g_GUIMgr.AddWnd( "XML/GuildMainWnd.xml" );
	g_GUIMgr.AddWnd( "XML/guildentrydelayuserwnd.xml" );
	g_GUIMgr.AddWnd( "XML/guildinvitationwnd.xml" );
	g_GUIMgr.AddWnd( "XML/guildinvitedwnd.xml" );
	g_GUIMgr.AddWnd( "XML/guildtitlechangewnd.xml" );
	g_GUIMgr.AddWnd( "XML/ladderteaminfownd.xml");
	g_GUIMgr.AddWnd( "XML/ladderteaminvitelistwnd.xml" );
	g_GUIMgr.AddWnd( "XML/ladderteaminvitedwnd.xml" );
	g_GUIMgr.AddWnd( "XML/laddercreatewnd.xml" );
	g_GUIMgr.AddWnd( "XML/ladderpasswordwnd.xml" );
	g_GUIMgr.AddWnd( "XML/ladderownerwnd.xml" );
	g_GUIMgr.AddWnd( "XML/guildherorankinglistwnd.xml" );
	g_GUIMgr.AddWnd( "XML/campjoinneedpopup.xml" );
	ModeRankWnd *pModeRankWnd = dynamic_cast<ModeRankWnd *>( g_GUIMgr.AddWnd( "XML/ModeRankWnd.xml" ) );
	if( pModeRankWnd )
		pModeRankWnd->SetPlayStage( this );

	g_GUIMgr.AddWnd( "XML/filelistwnd.xml");
	g_GUIMgr.AddWnd( "XML/backgroundfilelistwnd.xml" );
	g_GUIMgr.AddWnd( "XML/backgroundchangewnd.xml" );

	g_GUIMgr.AddWnd( "XML/presentalarmwnd.xml" );
	g_GUIMgr.AddWnd( "XML/presentdeletewnd.xml" );
	g_GUIMgr.AddWnd( "XML/presenttimecashalarmwnd.xml" );
	g_GUIMgr.AddWnd( "XML/presenttimecashreleasewnd.xml" );
	g_GUIMgr.AddWnd( "XML/NewMedalRandomBoxWnd.xml" );
	g_GUIMgr.AddWnd( "XML/TimeGateResultWnd.xml" );

	g_GUIMgr.AddWnd( "XML/campseasonbonus.xml" );
	g_GUIMgr.AddWnd( "XML/pcroomauthoritywnd.xml" );
	g_GUIMgr.AddWnd( "XML/expcroomauthoritywnd.xml" );

	PCRoomSoldierBonusWnd *pPCRoomSoldierBonus = dynamic_cast<PCRoomSoldierBonusWnd*>( g_GUIMgr.AddWnd( "XML/pcroomsoldierbonuswnd.xml" ) );
	if( pPCRoomSoldierBonus )
		pPCRoomSoldierBonus->SetPlayStage( this );

	ExPCRoomSoldierBonusWnd *pExPCRoomSoldierBonus = dynamic_cast<ExPCRoomSoldierBonusWnd*>( g_GUIMgr.AddWnd( "XML/expcroomsoldierbonuswnd.xml" ) );
	if( pExPCRoomSoldierBonus )
		pExPCRoomSoldierBonus->SetPlayStage( this );

	ExPCRoomSoldierBonusWnd *pExFreeDayUserSelectBonus = dynamic_cast<ExPCRoomSoldierBonusWnd*>( g_GUIMgr.AddWnd( "XML/userselectherobonuswnd.xml" ) );
	if( pExFreeDayUserSelectBonus )
		pExFreeDayUserSelectBonus->SetPlayStage( this );

	BillingWantWnd *pBillingWantWnd = dynamic_cast<BillingWantWnd*>( g_GUIMgr.AddWnd( "XML/billingwantwnd.xml" ) );
	if( pBillingWantWnd )
		pBillingWantWnd->SetPlayStage( this );

	g_GUIMgr.AddWnd( "XML/idchangewnd.xml" );

	GameOptionSettingKeyTab::SetPlayStage( this );
	GameOptionSettingJoyKeyTab::SetPlayStage( this );
	GameOptionMovieScreenShotTab::SetPlayStage( this );

	LadderBattleWnd *pLadderBattleWnd = dynamic_cast<LadderBattleWnd *>( g_GUIMgr.AddWnd( "XML/ladderbattlewnd.xml" ) );
	if( pLadderBattleWnd )
		pLadderBattleWnd->SetPlayStage( this );

	RankBattleWnd *pRankBattleWnd = dynamic_cast<RankBattleWnd *>( g_GUIMgr.AddWnd( "XML/rankbattlewnd.xml" ) );
	if( pRankBattleWnd )
		pRankBattleWnd->SetPlayStage( this );

	g_GUIMgr.AddWnd( "XML/RankBattleUpWnd.xml" );
	g_GUIMgr.AddWnd( "XML/ladderrankinglistwnd.xml" );
	g_GUIMgr.AddWnd( "XML/exitmsgbox.xml" );
	g_GUIMgr.AddWnd( "XML/battleroomexitmsgbox.xml" );
	g_GUIMgr.AddWnd( "XML/battleroomexitpopup.xml" );
	g_GUIMgr.AddWnd( "XML/campenterwnd.xml" );
	g_GUIMgr.AddWnd( "XML/guildetcitempopup.xml" );
	g_GUIMgr.AddWnd( "XML/guildnamechangewnd.xml" );
	g_GUIMgr.AddWnd( "XML/ExtraItemListWnd.xml" );
	g_GUIMgr.AddWnd( "XML/DecoItemListWnd.xml" );
	g_GUIMgr.AddWnd( "XML/PackageItemBuyAgreeWnd.xml" );
	g_GUIMgr.AddWnd( "XML/ItemResellWnd.xml" );
	g_GUIMgr.AddWnd( "XML/monstercoinusewnd.xml" );
	g_GUIMgr.AddWnd( "XML/GashaponPresentListWnd.xml" );
	g_GUIMgr.AddWnd( "XML/SendPresentIdWnd.xml" );
	g_GUIMgr.AddWnd( "XML/SendPresentConfirmWnd.xml" );
	g_GUIMgr.AddWnd( "XML/QuestProgressInfoMainWnd.xml" );
	g_GUIMgr.AddWnd( "XML/questcompleteinfownd.xml" );
	g_GUIMgr.AddWnd( "XML/QuestOccurStateMainWnd.xml" );
	g_GUIMgr.AddWnd( "XML/QuestAttainStateWnd.xml" );
	g_GUIMgr.AddWnd( "XML/QuestAttainSelectStateBrownWnd.xml" );
	g_GUIMgr.AddWnd( "XML/QuestAttainSelectStatePinkWnd.xml" );
	g_GUIMgr.AddWnd( "XML/questtooltip.xml" );
	g_GUIMgr.AddWnd( "XML/myladderteaminfownd.xml" );
	g_GUIMgr.AddWnd( "XML/partybattlesimplewnd.xml" );
	g_GUIMgr.AddWnd( "XML/partyobserverwnd.xml" );
	g_GUIMgr.AddWnd("XML/ExtraItemReleaseWnd.xml");
	g_GUIMgr.AddWnd( "XML/gradeupwnd.xml" );
	g_GUIMgr.AddWnd( "XML/soldierlevelupwnd.xml" );
	g_GUIMgr.AddWnd( "XML/uisoldierselectwnd.xml" );
	g_GUIMgr.AddWnd( "XML/questdeleteandfailwnd.xml" );
	g_GUIMgr.AddWnd( "XML/gameoptionsettingkeyinput.xml" );
	g_GUIMgr.AddWnd( "XML/moviemodehelpwnd.xml" );
	g_GUIMgr.AddWnd( "XML/newshopbuyconfirmwnd.xml" );
	g_GUIMgr.AddWnd( "XML/clauseviewwnd.xml" );
	g_GUIMgr.AddWnd( "XML/filewriteprocesswnd.xml" );
	g_GUIMgr.AddWnd( "XML/itemcustominfotooltipwnd.xml" );
	g_GUIMgr.AddWnd( "XML/itemcustomthumbnailtooltip.xml" );
	g_GUIMgr.AddWnd( "XML/tradeitemregtooltip.xml" );
	g_GUIMgr.AddWnd( "XML/tradeitemchangetooltip.xml" );
	g_GUIMgr.AddWnd( "XML/itemcustomglobaldetailviewwnd.xml" );
	g_GUIMgr.AddWnd( "XML/presentitemthumbnailtooltip.xml" );
	g_GUIMgr.AddWnd( "XML/myinventorycustomtooltip.xml" );
	g_GUIMgr.AddWnd( "XML/etcmotionselectwnd.xml" );
	g_GUIMgr.AddWnd( "XML/charrentalrequestwnd.xml" );
	g_GUIMgr.AddWnd( "XML/charrentalreturnwnd.xml" );
	g_GUIMgr.AddWnd( "XML/charrentalacquirewnd.xml" );	
	g_GUIMgr.AddWnd( "XML/soldierexperiencewnd.xml" );
	g_GUIMgr.AddWnd( "XML/invensoldierselectbuyagreewnd.xml" );
	g_GUIMgr.AddWnd( "XML/exmedalslotreleasewnd.xml" );
	g_GUIMgr.AddWnd( "XML/etcitemslotwnd.xml");

	// UI코스튬 관련 (코스튬 xml 추가)
	g_GUIMgr.AddWnd( "XML/costumecustomglobaldetailviewwnd.xml" );
	g_GUIMgr.AddWnd( "XML/myinventorycostumecustomtooltip.xml" );
	g_GUIMgr.AddWnd( "XML/presentcostumethumbnailtooltip.xml" );
	g_GUIMgr.AddWnd( "XML/costumelistwnd.xml" );
	g_GUIMgr.AddWnd("XML/costumereleasewnd.xml");

	//미션 및 퀘스트 관련
	g_GUIMgr.AddWnd( "XML/missionquestmainwnd.xml" );
	g_GUIMgr.AddWnd( "XML/missionattainstatewnd.xml" );

	//훈련소
	g_GUIMgr.AddWnd( "XML/headquartersinfownd.xml" );
	g_GUIMgr.AddWnd( "XML/headquartersjoinrequestwnd.xml" );
	g_GUIMgr.AddWnd( "XML/headquartersinvitedwnd.xml" );

	//개인본부
	g_GUIMgr.AddWnd( "XML/house/houseinfownd.xml" );
	g_GUIMgr.AddWnd( "XML/house/housejoinrequestwnd.xml" );
	g_GUIMgr.AddWnd( "XML/house/houseinvitedwnd.xml" );

	g_GUIMgr.AddWnd("XML/accessoryreleasewnd.xml");

	// 중간 현황
	RoundStateMainWnd *pRoundStateWnd = dynamic_cast<RoundStateMainWnd*>( g_GUIMgr.AddWnd( "XML/roundstatemainwnd.xml" ) );
	if( pRoundStateWnd )
	{
		pRoundStateWnd->SetPlayStage( this );
	}

	// 중간 결과
	RoundResultMainWnd *pResultStateWnd = dynamic_cast<RoundResultMainWnd*>( g_GUIMgr.AddWnd( "XML/roundresultmainwnd.xml" ) );
	if( pResultStateWnd )
	{
		pResultStateWnd->SetPlayStage( this );
	}

	// 시상식 + 최종 결과
	TotalResultMainWnd *pResultMainWnd = dynamic_cast<TotalResultMainWnd *>( g_GUIMgr.AddWnd( "XML/totalresultmainwnd.xml" ) );
	if( pResultMainWnd )
	{
		pResultMainWnd->SetPlayStage( this );
	}
		
	g_GUIMgr.AddWnd( "XML/joinrequestprogresswnd.xml" );
	g_GUIMgr.AddWnd( "XML/battlelockproblemwnd.xml" );
	g_GUIMgr.AddWnd( "XML/selectextragashaponsubwnd.xml" );
	g_GUIMgr.AddWnd( "XML/selectextragashaponwnd.xml" );
	g_GUIMgr.AddWnd( "XML/selectgashaponwnd.xml" );
	g_GUIMgr.AddWnd( "XML/selectgashaponsubwnd.xml" );	

	g_GUIMgr.AddWnd( "XML/tournamentteaminfownd.xml" );
	g_GUIMgr.AddWnd( "XML/tournamentteaminvitationwnd.xml" );
	g_GUIMgr.AddWnd( "XML/tournamentteaminvitedwnd.xml" );
	g_GUIMgr.AddWnd( "XML/tournamentteamentrydelayuserwnd.xml" );
	g_GUIMgr.AddWnd( "XML/tournamentteameventwnd.xml" );
	g_GUIMgr.AddWnd( "XML/tournamentteamalarmwnd.xml" );
	g_GUIMgr.AddWnd( "XML/tournamentbattleinvitewnd.xml" );
	g_GUIMgr.AddWnd( "XML/tournamentmatchwinalarmwnd.xml" );
	g_GUIMgr.AddWnd( "XML/tournamentrewardalarmwnd.xml" );
	g_GUIMgr.AddWnd( "XML/tournamentunearnedwinalarmwnd.xml" );

	g_GUIMgr.AddWnd( "XML/customtournamentdelaycreateteamlistwnd.xml" );	
	g_GUIMgr.AddWnd( "XML/tournamentjoinconfirmagreewnd.xml" );
	g_GUIMgr.AddWnd( "XML/tournamentinvitationwnd.xml" );

	g_GUIMgr.AddWnd( "XML/bingo/bingownd.xml" );
	g_GUIMgr.AddWnd( "XML/bingo/bingorewardwnd.xml" );

	g_GUIMgr.AddWnd( "XML/UsePresentEtcItemWnd.xml" );
	g_GUIMgr.AddWnd( "XML/UsePresentEtcItemConfirmWnd.xml" );

	g_GUIMgr.AddWnd( "XML/itemrechargewnd.xml" );
	g_GUIMgr.AddWnd( "XML/accessoryrechargewnd.xml" );

	g_GUIMgr.AddWnd( "XML/tournamentcheerpopupwnd.xml");
	g_GUIMgr.AddWnd( "XML/tournamentpredictpopupwnd.xml" );

	g_GUIMgr.AddWnd( "XML/attendancewnd.xml" );	
	PetWnd *pPetWnd = dynamic_cast<PetWnd*>( g_GUIMgr.AddWnd( "XML/petwnd.xml" ) );
	if( pPetWnd )
		pPetWnd->SetPlayStage( this );

	g_GUIMgr.AddWnd( "XML/pettooltip.xml" );
	g_GUIMgr.AddWnd( "XML/petresellwnd.xml" );
	g_GUIMgr.AddWnd( "XML/petemptywnd.xml" );
	g_GUIMgr.AddWnd( "XML/shopitemmoviewnd.xml" );
	
	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{
		g_GUIMgr.AddWnd( "XML/campenteralarmwnd.xml" );
		g_GUIMgr.AddWnd( "XML/campenterresultwnd.xml" );
		g_GUIMgr.AddWnd( "XML/userblocktypealarm.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustomselectwnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustommakewnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustommakedownloadwnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustommakeuploadwnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustommakeconfirmwnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustomdetailviewwnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustomchangeconfirmwnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustommakecompletewnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustomdeletewnd.xml" );

		g_GUIMgr.AddWnd( "XML/itemcustomtrainselectwnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustomtrainmakewnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustomwantetcitemwnd.xml" );
		g_GUIMgr.AddWnd( "XML/itemcustomselecttooltip.xml" );

		g_GUIMgr.AddWnd( "XML/lobbylockproblemwnd.xml" );

		g_GUIMgr.AddWnd( "XML/tournamentdetailmainwnd.xml" );
		g_GUIMgr.AddWnd( "XML/tournamentruleagreeconfirmwnd.xml" );
		g_GUIMgr.AddWnd( "XML/tournamentteamcreatewnd.xml" );
		g_GUIMgr.AddWnd( "XML/tournamentteamcreatecomplete.xml" );
		g_GUIMgr.AddWnd( "XML/tournamentetcitemneedpopup.xml" );
		g_GUIMgr.AddWnd( "XML/tournamentetcitemusepopup.xml" );
		g_GUIMgr.AddWnd( "XML/tournamentroomlistwnd.xml" );

		g_GUIMgr.AddWnd( "XML/customtournamentcreatewnd.xml" );
		g_GUIMgr.AddWnd( "XML/customtournamentcalendarwnd.xml" );
		g_GUIMgr.AddWnd( "XML/customtournamentcreateresultwnd.xml" );
		g_GUIMgr.AddWnd( "XML/customtournamentnoticewnd.xml" );
		g_GUIMgr.AddWnd( "XML/customtournamentalarmrewardwnd.xml" );
		g_GUIMgr.AddWnd( "XML/customtournamentallocatewnd.xml" );
		g_GUIMgr.AddWnd( "XML/customtournamentallocateresultwnd.xml" );
		g_GUIMgr.AddWnd( "XML/customtournamentrewardselectwnd.xml" );
		g_GUIMgr.AddWnd( "XML/tournamentcoinneedpopup.xml" );

		g_GUIMgr.AddWnd( "XML/customtournamententrylistwnd.xml" );

		g_GUIMgr.AddWnd( "XML/relativegradebonuswnd.xml" );

		// UI코스튬 관련 (코스튬 스킨 xml 추가)
		g_GUIMgr.AddWnd( "XML/costumecustomchangeconfirmwnd.xml" );
		g_GUIMgr.AddWnd( "XML/costumecustomdeletewnd.xml" );
		g_GUIMgr.AddWnd( "XML/costumecustomdetailviewwnd.xml" );
		g_GUIMgr.AddWnd( "XML/costumecustommakecompletewnd.xml" );
		g_GUIMgr.AddWnd( "XML/costumecustommakeconfirmwnd.xml" );
		g_GUIMgr.AddWnd( "XML/costumecustommakewnd.xml" );
		g_GUIMgr.AddWnd( "XML/costumecustomselecttooltip.xml" );
		g_GUIMgr.AddWnd( "XML/costumecustomselectwnd.xml" );
	}
		
	g_GUIMgr.AddWnd( "XML/awakereleasewnd.xml" );
	g_GUIMgr.AddWnd( "XML/QuickLobbyExitWnd.xml" );
	g_GUIMgr.AddWnd( "XML/guildmarkchangewnd.xml" );
	g_GUIMgr.AddWnd( "XML/partypasswordwnd.xml" );

	g_GUIMgr.AddWnd( "XML/reinforceselectwnd.xml" );
	g_GUIMgr.AddWnd( "XML/powerupselectwnd.xml" );
	g_GUIMgr.AddWnd( "XML/enablepoweruplistwnd.xml" );
	g_GUIMgr.AddWnd( "XML/soulstonebuywnd.xml" );
	g_GUIMgr.AddWnd( "XML/myitemselectewnd.xml" );
	g_GUIMgr.AddWnd( "XML/accessoryselectwnd.xml" );
	g_GUIMgr.AddWnd( "XML/MySpiritSelectWnd.xml" );

	RollBookWnd *pRollBookWnd = dynamic_cast<RollBookWnd*>( g_GUIMgr.AddWnd( "XML/rollbookwnd.xml" ) );
	if( pRollBookWnd )
		pRollBookWnd->SetPlayStage( this );

	g_GUIMgr.AddWnd( "XML/guildrewardwnd.xml" );
	g_GUIMgr.AddWnd( "XML/guildattendrewardwnd.xml" );
	g_GUIMgr.AddWnd( "XML/guildrankrewardwnd.xml" );

	g_GUIMgr.AddWnd( "XML/inventoryselectitemwnd.xml" );

	g_GUIMgr.AddWnd( "XML/newshopitempurchaseleedwnd.xml");
	g_GUIMgr.AddWnd( "XML/housingblocksetupwnd.xml" );
	g_GUIMgr.AddWnd( "XML/housingblockinvenwnd.xml" );
	g_GUIMgr.AddWnd( "XML/housingblockinventooltip.xml" );
	g_GUIMgr.AddWnd( "XML/guildhqetcitemneedpopup.xml" );
	g_GUIMgr.AddWnd( "XML/homeetcitemneedpopup.xml" );

#ifndef SHIPPING
	g_GUIMgr.AddWnd( "XML/CharGameInfoListWnd.xml" );
	g_GUIMgr.AddWnd( "XML/singlesettingwnd.xml" );
#endif

	g_GUIMgr.AddWnd( "XML/namedtitleselectwnd.xml" );
	g_GUIMgr.AddWnd( "XML/priaterulletwnd.xml" );
	g_GUIMgr.AddWnd( "XML/PriateRulletRenewalWnd.xml" );
	g_GUIMgr.AddWnd( "XML/privaterulletresetpopup.xml" );
	g_GUIMgr.AddWnd( "XML/successionbattlewnd.xml" );
	g_GUIMgr.AddWnd( "XML/successionbattlerequestwnd.xml" );
	g_GUIMgr.AddWnd( "XML/successionbattlereplywnd.xml" );
	g_GUIMgr.AddWnd( "XML/successionmatchversusscenewnd.xml" );
	g_GUIMgr.AddWnd( "XML/waiteffectwnd.xml" );
	g_GUIMgr.AddWnd( "XML/SpiritComposeResultWnd.xml" );
	g_GUIMgr.AddWnd( "XML/SpiritConversionResultWnd.xml" );
	g_GUIMgr.AddWnd( "XML/matchinggamewnd.xml" );
	g_GUIMgr.AddWnd( "XML/dicegamewnd.xml" );
	g_GUIMgr.AddWnd( "XML/gradeadjustmentwnd.xml" );

	AlchemicChangeAlarmWnd *pAlarmWnd = dynamic_cast<AlchemicChangeAlarmWnd*>( g_GUIMgr.AddWnd( "XML/AlchemicChangeAlarmWnd.xml" ) );
	if( pAlarmWnd )
		pAlarmWnd->SetPlayStage( this );

	// 싱글 모드 필요없는 인터페이스 Hide
	if( !P2PNetwork::IsNetworkPlaying() )
	{
		g_GUIMgr.HideWnd( SOLDIER_SELECT_WND );
		g_GUIMgr.HideWnd( MINI_SOLDIER_SELECT_WND );
	}
	
	g_QuestGuide.CreateGuideWnd();

	g_QuestMgr.SetPlayStage( this );
	g_GUIMgr.SetPlayStage( this );
	g_GUIMgr.SetNoneUIMode( false );
	g_ChatMgr.SetPlayStage( this );
	g_AlarmMgr.EnterStage();
}

void ioPlayStage::InitCameraController()
{
	// 공통 카메라	
	ioINILoader_e kLoader( "config/sp2.ini" );
	kLoader.SetTitle_e( "camera" );

	m_bNoneCameraWorking = kLoader.LoadBool_e( "none_camera_working", false );
	
	m_fCamLookAtHeightOffset = kLoader.LoadFloat_e( "lookat_height_offset", 200.0f );
	m_dwCamSlerpTime = kLoader.LoadInt_e( "camera_slerp_time", 2000 );

	m_fDefCamHeight = kLoader.LoadFloat_e( "default_height", 400.0f );
	m_fStartHeight = m_fCurCamHeight = m_fDefCamHeight;

	m_fDefCamDistance = kLoader.LoadFloat_e( "default_distance", FLOAT1000 );	
	m_fStartDistance = m_fCurCamDistance = m_fDefCamDistance;

	// 임시 : TPS
	m_bObserverTPSView = kLoader.LoadBool_e( "tps_observer_view", false );
	m_fTPSLookAtRate = kLoader.LoadFloat_e( "tps_lookat_rate", FLOAT1 );
	m_fTPSLookAtOffSet = kLoader.LoadFloat_e( "tps_lookat_offset", 0.0f );
	m_fTPSLookAtOffSetFr = kLoader.LoadFloat_e( "tps_lookat_offset_fr", 0.0f );

	m_fTPSHeight = kLoader.LoadFloat_e( "tps_height", 200.0f );
	m_fTPSDistance = kLoader.LoadFloat_e( "tps_distance", FLOAT100 );	
	m_fTPSFov = kLoader.LoadFloat_e( "tps_fov", 45.0f );

	m_fTPSOffSet = kLoader.LoadFloat_e( "tps_offset", 0.0f );

	m_fDefCamFov = kLoader.LoadFloat( "projection", "fov", 45.0f );
	m_fStartFov = m_fCurCamFov = m_fDefCamFov;

	kLoader.SetTitle_e( "mouse_camera" );

	m_vCamMouseDefMovie.x = kLoader.LoadFloat_e( "camera_movie_mouse_x", 0.0f );
	m_vCamMouseDefMovie.y = kLoader.LoadFloat_e( "camera_movie_mouse_y", 0.0f );
	m_vCamMouseDefMovie.z = kLoader.LoadFloat_e( "camera_movie_mouse_z", 0.0f );
	m_vCamMouseDefView.x  = kLoader.LoadFloat_e( "camera_view_mouse_x", 0.0f );
	m_vCamMouseDefView.y  = kLoader.LoadFloat_e( "camera_view_mouse_y", 0.0f );
	m_vCamMouseDefView.z  = kLoader.LoadFloat_e( "camera_view_mouse_z", 0.0f );
	m_fCameraFixedFov= kLoader.LoadFloat_e( "camera_fixed_mouse_fov", 35.0f );
	m_fCamMouseFixedLookAtDist = kLoader.LoadFloat_e( "camera_fixed_lookat_dist", 0.0f );	
	m_fCamMouseFixedLookAtDistHeight = kLoader.LoadFloat_e( "camera_fixed_lookat_dist_height", 42.5f );	
	m_dwCamMouseLookAtMoveTime = kLoader.LoadInt_e( "camera_fixed_lookat_move_time", 1000 );

	m_fMouseRotateAngle = kLoader.LoadFloat_e( "max_rotate_angle", 45.0f );	
	m_fMouseRotateSpeed = kLoader.LoadFloat_e( "cam_rotate_speed", FLOAT100 );

	m_fMouseRotateAngleByHousing = kLoader.LoadFloat_e( "housing_max_rotate_angle", 175.0f );
	m_fMouseRotateSpeedByHousing = kLoader.LoadFloat_e( "housing_cam_rotate_speed", 40.0f );

	m_fMouseZoomSpeed = kLoader.LoadFloat_e( "cam_zoom_speed", 36.0f );
	m_fMouseZoomMin = kLoader.LoadFloat_e( "cam_zoom_min", -500.0f );
	m_fMouseZoomMax = kLoader.LoadFloat_e( "cam_zoom_max", FLOAT500 );

	m_fMouseZoomSpeedByHousing	= kLoader.LoadFloat_e( "housing_cam_zoom_speed", 36.0f );
	m_fMouseZoomMinByHousing	= kLoader.LoadFloat_e( "housing_cam_zoom_min", -500.0f );
	m_fMouseZoomMaxByHousing	= kLoader.LoadFloat_e( "housing_cam_zoom_max", 1500.0f );

	m_fMouseZoomTotalMin = kLoader.LoadFloat_e( "total_zoom_min", FLOAT100 );
	m_fMouseZoomTotalMax = kLoader.LoadFloat_e( "total_zoom_max", 2000.0f );
	m_fMouseZoomRestoreSpeed = kLoader.LoadFloat_e( "zoom_restore_speed", 300.0f );

	m_dwMouseCamCtrlRestoreTime = kLoader.LoadInt_e( "mouse_cam_restore_time", 1000 );
	m_dwZoomKeyboardTick = kLoader.LoadInt_e( "zoom_keyboard_tick", 10 );
	
	// 모드 전용 카메라 
	if( m_pPlayMode )
	{
		if( m_pPlayMode->IsModeCameraUse() )
		{
			m_bArcadeCameraWorking =  true;  
			m_fDefCamFov = m_pPlayMode->GetModeCameraFov();;
			m_fStartFov = m_fCurCamFov = m_fDefCamFov;

			m_fDefCamHeight = m_pPlayMode->GetModeCameraHeight();
			m_fStartHeight = m_fCurCamHeight = m_fDefCamHeight;

			m_fDefCamDistance = m_pPlayMode->GetModeCameraDistance();	
			m_fStartDistance = m_fCurCamDistance = m_fDefCamDistance;

			m_fCamLookAtHeightOffset = m_pPlayMode->GetModeCameraLookAtHeightOffset();
			m_dwCamSlerpTime = m_pPlayMode->GetModeCameraSlerpTime();
			m_fArcadeCameraDistanceLimit = m_pPlayMode->GetModeCameraDistanceLimit();
		}
	}

	D3DXVECTOR3 vPos, vLookAt;
	vPos = m_vMapCenter;
	vPos.y += m_fDefCamHeight;
	vPos.z -= m_fDefCamDistance;

	vLookAt = m_vMapCenter;
	vLookAt.y += m_fCamLookAtHeightOffset;

	ioLookAtCameraController *pCtrl = new ioLookAtCameraController;
	pCtrl->SetLookAt( vLookAt );
	pCtrl->SetPosition( vPos );

	g_App.GetCamera()->SetController( pCtrl );
	g_App.GetCamera()->SetFov( m_fStartFov );

	m_vCurLookAt = vLookAt;
}

ioItem* ioPlayStage::CreateItem( const ioHashString &rkName )
{
	return g_ItemMaker.CreateItem( rkName );
}

ioItem* ioPlayStage::CreateItem( DWORD dwItemCode )
{
	return g_ItemMaker.CreateItem( dwItemCode );
}

void ioPlayStage::UpdatePreLoop( ioCamera *pCamera )
{
	if( P2PNetwork::IsNetworkPlaying() )
	{
		g_GlobalTimer.Process();
		g_BattleRoomMgr.ProcessBattleRoom();
		g_LadderTeamMgr.ProcessLadderTeam();
		g_ShuffleRoomMgr.ProcessShuffleRoom();
		g_CampMgr.ProcessCamp();
		g_AlarmMgr.ProcessAlarm();
		g_QuestMgr.Process();
 		g_QuestGuide.ProcessGuide();
		g_ModeUIController.ProcessController();
		g_GameAnnMgr.Process( GetModeType() );
		g_MyInfo.RentalCharTimeCheck();
		g_MyInfo.OneMinuteTimeCheckProcess();
		g_MyInfo.UpdateClover();
		g_MemoManager.ProcessMemo();
		g_TournamentMgr.Process();
		g_NamedTitleInfoMgr.Process();
	}
}

void ioPlayStage::UpdateLoop( ioCamera *pCamera )
{
	if( m_pPlayMode && !m_pPlayMode->IsEntityUpdateState() )
	{
		return;
	}

	bool bCheckChar = Help::CheckEnableTimeGapUser();

	DWORD dwStartTime = 0;
	DWORD dwGapTime = 0;

	ErrorReport::SetPosition( 1100, 92 );

	if( bCheckChar )
		dwStartTime = timeGetTime();

	if( m_pBlockWorld )
		m_pBlockWorld->ProcessUpdate();

	UpdateGameEntityList();

	if( bCheckChar )
	{
		dwGapTime = timeGetTime() - dwStartTime;

		if( dwGapTime > 30 )
			LOG.PrintTimeAndLog( 0, "FC - UpdateLoop: Entity: %d", dwGapTime );
	}

	ErrorReport::SetPosition( 1100, 93 );

	if( bCheckChar )
		dwStartTime = timeGetTime();

	g_AreaWeaponMgr.Update();

	if( bCheckChar )
	{
		dwGapTime = timeGetTime() - dwStartTime;

		if( dwGapTime > 100 )
			LOG.PrintTimeAndLog( 0, "FC - UpdateLoop: AreaWeapon: %d", dwGapTime );
	}

	ErrorReport::SetPosition( 1100, 94 );

	if( bCheckChar )
		dwStartTime = timeGetTime();

	ProcessCollision();

	if( bCheckChar )
	{
		dwGapTime = timeGetTime() - dwStartTime;

		if( dwGapTime > 30 )
			LOG.PrintTimeAndLog( 0, "FC - UpdateLoop: Collision: %d", dwGapTime );
	}

	ErrorReport::SetPosition( 1100, 95 );

	if( m_pPlayMode )
	{
		ErrorReport::SetPosition( 1100, 96 );
		m_pPlayMode->ProcessCharDropZoneCollision();
		ErrorReport::SetPosition( 1100, 97 );

		if( Help::CheckEnableTimeGapUser(1) && m_pPlayMode->GetModeState() == ioPlayMode::MS_PLAY )
		{
			CheckValidateNetTable();
		}
	}

	g_RequestCharMgr.OnProcess();
	g_RequestLagMgr.OnProcess( this );

	if( m_pBlockWorld )
		m_pBlockWorld->CheckAfterLoop( pCamera ); 
}

void ioPlayStage::UpdateAfterLoop( ioCamera *pCamera )
{
	ErrorReport::SetPosition( 1100, 20 );

	ProcessSyncOwnerPing();
	ErrorReport::SetPosition( 1100, 21 );

	UpdateAfterGameEntityList();
	ErrorReport::SetPosition( 1100, 23 );

	if( m_pBlockMgr )
		m_pBlockMgr->UpdateAfter();

	ProcessSyncControl();
	ErrorReport::SetPosition( 1100, 24 );

	ProcessSyncPushStruct();
	ErrorReport::SetPosition( 1100, 25 );

	ProcessLossWeaponWoundEvent();
	ErrorReport::SetPosition( 1100, 26 );

	UpdateCamera( pCamera );
	ErrorReport::SetPosition( 1100, 27 );

	if( m_pPlayMode )
	{
		m_pPlayMode->ProcessMode();
	}
	else
	{		
		return;
	}

	ErrorReport::SetPosition( 1100, 28 );

	g_ModeHelpMgr.Process( m_pPlayMode );
	ErrorReport::SetPosition( 1100, 29 );

	CheckOwnerCanPickItems();
	CheckOwnerCanTackMachine();
	ErrorReport::SetPosition( 1100, 31 );

	CheckShadowCastObjects( pCamera );
	ErrorReport::SetPosition( 1100, 32 );

	UpdateMapEffect();
	ErrorReport::SetPosition( 1100, 33 );

	CheckCharMouseClick( pCamera );
	ErrorReport::SetPosition( 1100, 34 );

	ProcessByOwner( pCamera );
	ErrorReport::SetPosition( 1100, 35 );

	ProcessFunctionKey();
	ErrorReport::SetPosition( 1100, 36 );

	ProcessScreenBlind();
	ErrorReport::SetPosition( 1100, 37 );

	ProcessBuffBlind();
	ErrorReport::SetPosition( 1100, 38 );

	g_AbuseMgr.ProcessPlayStage( this );
	ErrorReport::SetPosition( 1100, 39 );

	CheckOwnerCanStartRoulette();
	ErrorReport::SetPosition( 1100, 40 );
}

void ioPlayStage::UpdateMapEffect()
{
	float fTimeGap = g_FrameTimer.GetCurFrameGap();

	MapEffectList::iterator iter = m_EffectList.begin();
	while( iter != m_EffectList.end() )
	{
		ioMapEffect *pEffect = *iter;

		ErrorReport::SetPosition( 1110, 1 );
		if( pEffect && pEffect->IsLive() )
		{
			ErrorReport::SetPosition( 1110, 2 );
			pEffect->Update( fTimeGap );
			++iter;
		}
		else
		{
			ErrorReport::SetPosition( 1110, 3 );
			iter = m_EffectList.erase( iter );

			if( pEffect )
				delete pEffect;
		}
	}
}

void ioPlayStage::ProcessCollision()
{
	ErrorReport::SetPosition( 1120, 1 );
	if( m_pPlayMode && m_pPlayMode->IsCollisionCheck() )
	{
		ProcessWeaponVSEntityCollision();
	}

	ErrorReport::SetPosition( 1120, 2 );
	ProcessWeaponVSMapCollision();

	ErrorReport::SetPosition( 1120, 3 );
	ProcessWeaponVSMineCollision();

	ErrorReport::SetPosition( 1120, 4 );
	ProcessWeaponVSMine6Collision();

	ErrorReport::SetPosition( 1120, 5 );
	ProcessWeaponVSTrackingMineCollision();

	ErrorReport::SetPosition( 1120, 6 );
	g_WeaponMgr.ClearDeadWeaponList();

	ErrorReport::SetPosition( 1120, 7);
	DestroyDiePushStruct();

	ErrorReport::SetPosition( 1120, 8 );
	DestroyDieMachineStruct();

	ErrorReport::SetPosition( 1120, 9 );
	DestroyDieDummyChar();

	DestroyDiePet();

	ErrorReport::SetPosition( 1120, 10 );
	ProcessBallStructMapCollision();

	ErrorReport::SetPosition( 1120, 11 );
	ProcessPlayEntityMapCollision();

	ErrorReport::SetPosition( 1120, 12 );
	ProcessPlayEntityCollision();

	if( m_pPlayMode )
	{
		ErrorReport::SetPosition( 1120, 13 );
		m_pPlayMode->ProcessZoneCollision();
	}

	ErrorReport::SetPosition( 1120, 14 );
	ProcessWeaponVSAirMineCollision();
		
	ErrorReport::SetPosition( 1120, 15 );
	ProcessModeItemCollision();	

	ErrorReport::SetPosition( 1120, 16 );
	if( m_pDummyCharMgr )
		m_pDummyCharMgr->ProcessCheckDummyGroup();
}

void ioPlayStage::ProcessSyncControl()
{
	if( !P2PNetwork::IsNetworkPlaying() || !IsNetworkMode() )
		return;

	DWORD dwTime = 500;
	if( ioLocalManager::GetLocalType() == ioLocalManager::LCT_INDONESIA || 
		ioLocalManager::GetLocalType() == ioLocalManager::LCT_TAIWAN ) 
		dwTime = 1000;

	if( m_UserSyncTimer.IsTimePast( dwTime ) )
	{
		// 자신의 캐릭터를 0.5초마다 동기화 시킴
		ioBaseChar *pChar = GetOwnerChar();
		if( pChar )
		{
			pChar->SendCurrentPositionToAllUser();
		}

		// 자신이 관리하는 NPC를 1초마다 동기화 시킴
		ProcessSyncNpcControl();
	}
}

void ioPlayStage::ProcessLossWeaponWoundEvent()
{
	DWORD dwCurTime = FRAMEGETTIME();
	LossWeaponWoundEvent::iterator iter = m_LossWeaponWoundEventList.begin();
	while( iter != m_LossWeaponWoundEventList.end() )
	{
		WeaponWoundEvent* pEvent = &(*iter) ;
		if( pEvent )
		{
			ioBaseChar *pAttacker = GetBaseChar( pEvent->szAttacker );
			ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pAttacker, pEvent->dwWeaponIdx );
			if( pWeapon )
			{
				pWeapon->ApplyWeaponSync( pEvent->packet, this );
				iter = m_LossWeaponWoundEventList.erase( iter );
			}
			else if( pEvent->dwLifeEndTime < dwCurTime )
				iter = m_LossWeaponWoundEventList.erase( iter );
			else
				++iter;
		}
		else
			iter = m_LossWeaponWoundEventList.erase( iter );
	}
}

void ioPlayStage::ProcessSyncNpcControl()
{
	if( m_pPlayMode == NULL ) return;
	if( m_pPlayMode->GetModeState() != ioPlayMode::MS_PLAY ) return;

	BaseCharList::iterator iter;
	for(iter = m_BaseCharList.begin();iter != m_BaseCharList.end();++iter)
	{			
		ioNpcChar *pNpc = ToNpcChar( *iter );
		if( pNpc == NULL )	continue;
		if( !pNpc->IsNeedProcess() ) continue;
		if( pNpc->GetNpcType() >= NT_BIGTOWER && pNpc->GetNpcType() < END_FIXING_NPC )
			continue;
		if( pNpc->GetState() == CS_LOADING ||
			pNpc->GetState() == CS_DIE ||
			pNpc->GetState() == CS_VIEW ) continue;

		pNpc->SendCurrentPositionToAllUser();
	}
}

void ioPlayStage::ProcessSyncOwnerPing()
{
	if( !P2PNetwork::IsNetworkPlaying() )
		return;

	if( m_P2PPingTimer.IsTimePast( 5000 ) )
	{
		g_App.SendAllP2PPing();
	}
}

void ioPlayStage::NotifyOwnerCameraLoseTime( DWORD dwLoseTime )
{
	m_dwOwnerCameraLoseTime = dwLoseTime;
}

void ioPlayStage::NotifyOwnerRevival()
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner )	return;

	if( m_ViewChar != pOwner->GetCharName() )
	{
		int iCharCnt = m_pPlayMode->GetRecordCharCnt();
		ChangeCurViewChar( pOwner->GetCharName(), iCharCnt );
	}

	m_dwViewMovingTime = 0;
	NotifyOwnerCameraLoseTime( 0 );
	m_bSpringDampUse = false;
}

void ioPlayStage::ChangeCurViewChar( const ioHashString &rkName, int iMaxChar )
{
	if( iMaxChar <= 0 )
	{
		m_ViewChar.Clear();
		return;
	}

	// 배틀모드일때는 죽었을 경우 
	ioBaseChar* pMyChar = GetOwnerChar();
	if( m_pPlayMode->GetModeType() == MT_BATTLE && pMyChar->GetState() != CS_OBSERVER )
	{
		ioHashString szFightSameTeamUser = g_BattleRoomMgr.FindSameTeamUserInBattle( g_MyInfo.GetPublicID() );
		if( !szFightSameTeamUser.IsEmpty() )
			m_ViewChar = szFightSameTeamUser;
	}
	else
		m_ViewChar = rkName;
	m_dwViewMovingTime = FRAMEGETTIME();

	ioCamera *pCam = g_App.GetCamera();
	if( pCam )
	{
		ioLookAtCameraController *pCtrl = ToLookAtCtrl( pCam->GetController() );
		if( pCtrl )
		{
			m_vLerpStart = pCtrl->GetLookAt();
			m_vLerpStart.y -= m_fCamLookAtHeightOffset;
		}
	}
}

void ioPlayStage::ChangeCurViewCharByDie( ioBaseChar* pChar )
{
	if( !pChar || !pChar->IsOwnerChar() )
		return;

	if( pChar->IsNeedSendNetwork() && GetModeType() != MT_MYROOM )
	{
		SP2Packet kPacket( CTPK_REQUEST_REVIVAL_TIME );
		TCPNetwork::SendToServer( kPacket );
	}

	int iDieCnt = 0;
	int iUserCnt= 0;
	int iCharCnt = m_pPlayMode->GetRecordCharCnt();
	for( int i=0; i<iCharCnt; i++ )
	{
		ioBaseChar *pCurChar = m_pPlayMode->GetRecordChar( i );
		if( pCurChar )
		{
			if( ToNpcChar( pCurChar ) ) continue;

			iUserCnt++;

			if( pCurChar->GetState() == CS_DIE ||
				pCurChar->GetState() == CS_VIEW ||
				pCurChar->GetState() == CS_OBSERVER ||
				pCurChar->GetState() == CS_LOADING )
			{
				iDieCnt++;
			}
		}
	}

	if( iDieCnt == iUserCnt )
	{
		m_ViewChar = pChar->GetCharName();
		ChangeCurViewChar( m_ViewChar, iCharCnt );
		return;
	}

	ioBaseChar *pTarget = GetNextViewChar( pChar->GetCharName() );
	// 배틀모드일때는 기본 시점이 출전한 같은 팀 유저
	if( GetModeType() == MT_BATTLE )
	{
		ioHashString FightUserNick = g_BattleRoomMgr.FindSameTeamUserInBattle( g_MyInfo.GetPublicID() );
		if( !FightUserNick.IsEmpty() )
			pTarget = GetBaseChar( FightUserNick );
	}

	if( pTarget )
	{
		ChangeCurViewChar( pTarget->GetCharName(), iCharCnt );
	}
}

void ioPlayStage::ChangeCurViewCharByView( ioBaseChar *pChar )
{
	if( !pChar ) return;
	
	// 최종 ViewChar의 캐릭터가 관전으로 전환되면 다른 타겟을 찾는다.
	if( pChar->GetCharName() == m_ViewChar || pChar->IsOwnerChar() )
	{
		int i = 0;
		int iCharCnt = m_pPlayMode->GetRecordCharCnt();
		for(i = 0;i < iCharCnt;i++)
		{
			ioBaseChar *pCurChar = m_pPlayMode->GetRecordChar( i );
			if( pCurChar )
			{
				if( pCurChar->GetState() == CS_DIE ||
					pCurChar->GetState() == CS_VIEW ||
					pCurChar->GetState() == CS_OBSERVER ||
					pCurChar->GetState() == CS_LOADING ||
					ToNpcChar( pCurChar ) )
				{
					continue;
				}

				m_ViewChar = pCurChar->GetCharName();
				return;
			}
		}

		// 유저가 없으면 몬스터도 타겟으로 잡음
		for(i = 0;i < iCharCnt;i++)
		{
			ioBaseChar *pCurChar = m_pPlayMode->GetRecordChar( i );
			if( pCurChar )
			{
				if( pCurChar->GetState() == CS_DIE ||
					pCurChar->GetState() == CS_VIEW ||
					pCurChar->GetState() == CS_OBSERVER ||
					pCurChar->GetState() == CS_LOADING)
				{
					continue;
				}

				m_ViewChar = pCurChar->GetCharName();
				return;
			}
		}

		// 아무도 없으면 자신 !!
		ioBaseChar *pOwner = GetOwnerChar();
		if( pOwner )
			m_ViewChar = pOwner->GetCharName();
	}
}

bool ioPlayStage::IsNoCharInputWndOpen()
{
	if( g_GUIMgr.IsShow( SOLDIER_SELECT_WND ) )
		return true;

	if( g_GUIMgr.IsShowSettingKeyWnd() )
		return true;

	return false;
}

void ioPlayStage::NotifyOwnerSendControlInfo()
{
	m_UserSyncTimer.InitTimer();
}

void ioPlayStage::NotifyPushStructSendInfo()
{
	m_PushStructTimer.InitTimer();
}

void ioPlayStage::NotifyBallStructSendInfo()
{
	m_BallStructTimer.InitTimer();
}

void ioPlayStage::NotifySpringDampUseSet( bool bSpringDampUse )
{
	m_bSpringDampUse = bSpringDampUse;
}

ioBaseChar* ioPlayStage::GetModeCameraTarget()
{
	// 배틀모드이면 모두 자기것부터 보이게...
	if( GetModeType() == MT_BATTLE )
		GetOwnerChar();
	if( m_bArcadeCameraWorking )
	{
		ioFightClubMode *pFightClubMode = ToFightClubMode( m_pPlayMode );
		if( !pFightClubMode ) return NULL;

		const ioHashString &kChampionName   = pFightClubMode->GetChampionName();
		const ioHashString &kChallengerName = pFightClubMode->GetChallengerName();

		ioBaseChar *pOwnerChar= GetOwnerChar();
		ioBaseChar *pChallenger = GetBaseChar( kChallengerName );

		if( pChallenger == pOwnerChar )
		{
			if( pChallenger->GetState() != CS_VIEW &&
				pChallenger->GetState() != CS_LOADING &&
				pChallenger->GetState() != CS_OBSERVER )
			{
				return pChallenger;			
			}
		}
		else
		{
			ioBaseChar *pChampion = GetBaseChar( kChampionName );
			if( pChampion )
			{
				if( pChampion->GetState() != CS_VIEW &&
					pChampion->GetState() != CS_LOADING &&
					pChampion->GetState() != CS_OBSERVER )
				{
					return pChampion;			
				}
			}
		}
	}

	return GetOwnerChar();
}

ioBaseChar* ioPlayStage::CheckCameraTarget()
{
	ioBaseChar *pTarget = GetModeCameraTarget();
	if( !pTarget )	return NULL;

	if( ( pTarget->GetState() != CS_DIE &&
		pTarget->GetState() != CS_VIEW &&
		pTarget->GetState() != CS_LOADING &&
		pTarget->GetState() != CS_OBSERVER &&
		m_pPlayMode->GetModeType() != MT_BATTLE ))
		return pTarget;

	// 배틀모드일때
	if( m_pPlayMode->GetModeType() == MT_BATTLE )
	{
		if( g_BattleRoomMgr.CheckUserBattleFightFlag(pTarget->GetCharName()) )
			return pTarget;
	}

	// 죽을때 혼전상황에서 방어키를 눌러서 죽자마자 타겟이 바뀌는 상황을 위해
	if( m_dwOwnerCameraLoseTime > 0 && m_dwOwnerCameraLoseTime + 500 > FRAMEGETTIME() )
		return pTarget;

	// 아래 UI일때는 Camera target 변경 
	SoldierSelectWnd *pSelectWnd = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
	if( pSelectWnd && pSelectWnd->IsShow() )
	{
		if( !pSelectWnd->IsSettingWait() )
			return pTarget;
	}

	int iCharCnt = m_pPlayMode->GetRecordCharCnt();
	if( iCharCnt == 1 )
		return pTarget;

	bool bSpecialChange = false;
	if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
		bSpecialChange = pTarget->CheckSpecialViewChange();
	bool bNpcChar = false;
	if( ToNpcChar( m_pPlayMode->GetRecordChar( m_ViewChar ) ) && !m_bCameraFixedBlock )
		bNpcChar = true;

	if( m_bStartChangeObserver || ( !m_bSpecialObserver && ( pTarget->CheckViewChange() || bSpecialChange || !m_pPlayMode->GetRecordChar(m_ViewChar) || bNpcChar ) ) ||
		( m_bSpecialObserver && ( bSpecialChange || !m_pPlayMode->GetRecordChar(m_ViewChar) ) ) )
	{
		if( bSpecialChange || m_bStartChangeObserver )
		{
			m_bStartChangeObserver = false;

			m_bSpecialObserver = !m_bSpecialObserver;
			if( m_bSpecialObserver )
			{
				if( !m_ViewChar.IsEmpty() )
				{
					pTarget = m_pPlayMode->GetRecordChar( m_ViewChar );
					if( pTarget )
						m_vObserverLookAt = pTarget->GetWorldPosition();
				}
			}
			g_ObserverWndMgr.ObserverInfoShow( m_bSpecialObserver );
		}
		if( !m_bSpecialObserver )
		{
			int iDieCnt = 0;
			int iUserCnt= 0;
			for( int i=0; i<iCharCnt; i++ )
			{
				ioBaseChar *pChar = m_pPlayMode->GetRecordChar( i );
				if( pChar )
				{
					if( ToNpcChar( pChar ) ) continue;

					iUserCnt++;
					if( pChar->GetState() == CS_DIE ||
						pChar->GetState() == CS_VIEW ||
						pChar->GetState() == CS_OBSERVER ||
						pChar->GetState() == CS_LOADING )
					{
						iDieCnt++;
					}
				}
			}

			if( iDieCnt == iUserCnt )
			{
				m_ViewChar = pTarget->GetCharName();
				ChangeCurViewChar( m_ViewChar, iCharCnt );

				if( g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()) && pTarget )
					pTarget->ReSetOwnerMarkTeam();
				else if( IsStealthMode() && pTarget )
					pTarget->ReSetOwnerMarkTeam();

				return pTarget;
			}

			pTarget = GetNextViewChar( m_ViewChar );
			if( pTarget )
			{
				m_ViewChar = pTarget->GetCharName();
				ChangeCurViewChar( m_ViewChar, iCharCnt );
			}
			else
			{
				m_ViewChar = GetOwnerChar()->GetCharName();
			}
		}
		else
			m_ViewChar = GetOwnerChar()->GetCharName();
	}

	if( !m_ViewChar.IsEmpty() )
		pTarget = m_pPlayMode->GetRecordChar( m_ViewChar );

	if( g_BattleRoomMgr.IsObserverUser(g_MyInfo.GetPublicID()) && pTarget )
		pTarget->ReSetOwnerMarkTeam();
	else if( IsStealthMode() &&& pTarget )
		pTarget->ReSetOwnerMarkTeam();

	ioBattleMode* pBattleMode = ToBattleMode( m_pPlayMode );
	if( pBattleMode )
	{
		if( pBattleMode->GetFirstBlockCameraView() )
		{
			ioHashString FightUserNick = g_BattleRoomMgr.FindSameTeamUserInBattle( g_MyInfo.GetPublicID() );
			if( !FightUserNick.IsEmpty() )
			{
				pTarget = GetBaseChar( FightUserNick );
				m_vObserverLookAt = pTarget->GetWorldPosition();
				m_ViewChar = FightUserNick;
				ChangeCurViewChar( m_ViewChar, iCharCnt );
				pBattleMode->ChangeFirstBlockCameraView(false);
			}
		}
	}

	return pTarget;
}

ioBaseChar* ioPlayStage::GetNextViewChar( const ioHashString &rkName )
{
	// Find
	int iCharCnt = m_pPlayMode->GetRecordCharCnt();
	int iCurIndex = -1;

	for( int i=0; i<iCharCnt; i++ )
	{
		ioBaseChar *pChar = m_pPlayMode->GetRecordChar( i );
		if( pChar && pChar->GetCharName() == rkName )
		{
			iCurIndex = i;
			break;
		}
	}

	if( iCurIndex == -1 )
		return NULL;

	int iNextIndex = iCurIndex;
	ioBaseChar *pTarget = NULL;

	do
	{
		iNextIndex++;
		iNextIndex = iNextIndex % iCharCnt;

		pTarget = m_pPlayMode->GetRecordChar( iNextIndex );
		if( !pTarget )
			return NULL;

	} while( ToNpcChar( pTarget ) ||
		     pTarget->GetState() == CS_DIE ||
			 pTarget->GetState() == CS_VIEW || 
			 pTarget->GetState() == CS_OBSERVER ||
			 pTarget->GetState() == CS_LOADING );

	return pTarget;
}

void ioPlayStage::UpdateCamera( ioCamera *pCamera )
{
	if( IsCameraPause() ) return;

	ioLookAtCameraController *pCtrl = ToLookAtCtrl( pCamera->GetController() );
	if( !pCtrl )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::UpdateCamera - Not LookAt Controller" );
		return;
	}

	if( m_bEventSceneCamera )
	{
		UpdateEventSceneCamera( pCamera );
	}
	else if( m_bEndSceneCamera )
	{
		UpdateEndSceneCamera( pCamera );
	}
	else
	{
		ioBaseChar *pTarget = CheckCameraTarget();
		if( pTarget )
		{
			if( m_bObserverTPSView )
			{
				if( g_BattleRoomMgr.IsObserverUser( GetOwnerName() ) )
				{
					D3DXVECTOR3 vNewLookAt = UpdateNewTPSLookAt( pTarget, m_vCurLookAt );
					UpdateTPSCameraPos( pCamera, pTarget, vNewLookAt );
				}
				else
				{
					D3DXVECTOR3 vNewLookAt = UpdateNewLookAt( pTarget, m_vCurLookAt );
					UpdateCameraPos( pCamera, pTarget, vNewLookAt );

					if( !m_bNoneCameraWorking && !m_bArcadeCameraWorking )
					{
						UpdateCameraByMouse( pCamera );
					}
				}
			}
			else
			{
				if( g_BattleRoomMgr.IsObserverUser( pTarget->GetCharName() ) )
				{
					D3DXVECTOR3 vNewLookAt = UpdateNewObserverLookAt( pTarget, m_vCurLookAt );
					UpdateObserverCameraPos( pCamera, pTarget, vNewLookAt );
				}
				else
				{
					D3DXVECTOR3 vNewLookAt = UpdateNewLookAt( pTarget, m_vCurLookAt );
					UpdateCameraPos( pCamera, pTarget, vNewLookAt );

					if( !m_bNoneCameraWorking && !m_bArcadeCameraWorking && !pTarget->IsFixedExceptionCamera() )
					{
						UpdateCameraByMouse( pCamera );
					}
				}
			}

		}
	}
	ioLookAtCameraInput kInput;
	pCamera->Update( &kInput );

	m_bSetCameraToNowOwner = false;
	m_bTargetLookAt = false;
}

float ioPlayStage::MouseSpringDistDamp( float fSpringConst,	float fDampConst, float fCurDist, float fTargetDist, float fPreDist )
{
	float fDisp = fCurDist - fTargetDist;
	if( fabsf( fDisp ) <= FLOAT1 )
		return -fDisp;

	float fForceMag = fSpringConst * fDisp +
					  fDampConst * ( fPreDist - fCurDist );

	return fForceMag * g_FrameTimer.GetSecPerFrame();
}

bool ioPlayStage::IsNowMouseCamera() const
{
	if( m_vCamMouseAmt.x != 0.0f || m_vCamMouseAmt.y != 0.0f || m_vCamMouseAmt.z != 0.0f )
		return true;
	if( m_dwCameraFixedStyle == CAMERA_STYLE_NORMAL )
		return false;
	return true;
}

bool ioPlayStage::HasCameraFixedStyle( DWORD dwStyle )
{
	if( m_dwCameraFixedStyle & dwStyle )
		return true;
	return false; 
}	

void ioPlayStage::SetCameraModeFixedBlock( bool bBlock )
{
	if( bBlock )
	{
		if( HasCameraFixedStyle( CAMERA_STYLE_MOVIE ) )
			ClearCameraModeFixed( CAMERA_STYLE_MOVIE );
		if( HasCameraFixedStyle( CAMERA_STYLE_VIEW ) )
			ClearCameraModeFixed( CAMERA_STYLE_VIEW );
	}
	m_bCameraFixedBlock = bBlock;
}

void ioPlayStage::SetCameraModeFixed( DWORD dwStyle )
{
	if( !m_pPlayMode ) return;
	if( m_pPlayMode->GetModeState() != ioPlayMode::MS_PLAY ) return;
	if( m_bEventSceneCamera || m_bEndSceneCamera || m_bCameraFixedBlock ) return;
	if( HasCameraFixedStyle( dwStyle ) ) return;
	if( g_MyInfo.IsFirstCameraFixedHelp() ) return;

	m_dwCameraFixedStyle |= dwStyle;

	// 스타일별 적용
	switch( dwStyle )
	{
	case CAMERA_STYLE_MOVIE:
		if( Setting::MovieModeUIHide() )
			g_GUIMgr.SetNoneUIMode( true );			
		m_vCamMouseAmt = m_vCamMouseDefMovie;
		g_QuestMgr.QuestCompleteTerm( QC_MOVIE_MODE );
		break;
	case CAMERA_STYLE_VIEW:
		if( !HasCameraFixedStyle( CAMERA_STYLE_MOVIE ) )         //이미 영화모드중이라면 카메라 기본 위치로 이동시키지 않는다.
			m_vCamMouseAmt = m_vCamMouseDefView;			
		break;
	}		

	// 최초 적용
	if( !(HasCameraFixedStyle( CAMERA_STYLE_MOVIE ) && HasCameraFixedStyle( CAMERA_STYLE_VIEW )) )
	{
		m_vCamMouseTargetCurLookAt = m_vCamMouseTargetPrevPos = m_vCamMouseFixedTargetLookAt;
		m_dwCamMouseLookAtMoveCurTime = FRAMEGETTIME();

		if( Setting::MovieModeUIHide() )
			SetScreenShotMode( true );
		SetMapCameraFixedObject( true );
	}
}

void ioPlayStage::SetCameraModeFixedByHousing()
{
	if( !m_pPlayMode )
		return;

	if( m_pPlayMode->GetModeState() != ioPlayMode::MS_PLAY )
		return;

	if( m_bEventSceneCamera || m_bEndSceneCamera || m_bCameraFixedBlock )
		return;

	if( HasCameraFixedStyle( CAMERA_STYLE_HOUSING ) )
		return;

	if( HasCameraFixedStyle( CAMERA_STYLE_MOVIE ) )
		ClearCameraModeFixed( CAMERA_STYLE_MOVIE );
	if( HasCameraFixedStyle( CAMERA_STYLE_VIEW ) )
		ClearCameraModeFixed( CAMERA_STYLE_VIEW );

	m_dwCameraFixedStyle |= CAMERA_STYLE_HOUSING;

	// 최초 적용
	m_vCamMouseTargetCurLookAt = m_vCamMouseTargetPrevPos = m_vCamMouseFixedTargetLookAt;
	m_dwCamMouseLookAtMoveCurTime = FRAMEGETTIME();
}

void ioPlayStage::ClearCameraModeFixed( DWORD dwStyle )
{
	if( !HasCameraFixedStyle( dwStyle ) )
		return;

	m_dwCameraFixedStyle ^= dwStyle;
	switch( m_dwCameraFixedStyle )
	{
	case CAMERA_STYLE_NORMAL:
		{
			m_vCamMouseAmt.x = m_vCamMouseAmt.y = m_vCamMouseAmt.z = 0.0f;
			m_dwMouseCamCtrlTime = 0;

			g_GUIMgr.SetNoneUIMode( false );
			SetScreenShotMode( false );

			SetMapCameraFixedObject( false );
		}
		break;
	case CAMERA_STYLE_MOVIE:
		break;
	case CAMERA_STYLE_VIEW:
		g_GUIMgr.SetNoneUIMode( false );
		m_vCamMouseAmt = m_vCamMouseDefView;
		break;
	case CAMERA_STYLE_HOUSING:
		break;
	}
}

bool ioPlayStage::IsOwnerViewModeCameraFiexed()
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		if( pOwner->GetState() == CS_DIE ||	pOwner->GetState() == CS_VIEW )
		{
			if( GetModeType() != MT_MYROOM && GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
				return true;
		}	
	}	
	return false;
}

void ioPlayStage::UpdateCameraByMouse( ioCamera *pCamera )
{
	if( !pCamera )	return;
	if( m_bCameraFixedBlock ) return;

	static bool bDebug = false;

	ioLookAtCameraController *pCtrl = ToLookAtCtrl( pCamera->GetController() );
	if( !pCtrl )	return;

	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )	return;

	bool bDeskTopOver = g_GUIMgr.IsDeskTopOver();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	float fCurrRotateSpeed	= m_fMouseRotateSpeed;
	float fCurrRotateAngle	= m_fMouseRotateAngle;
	if( HasCameraFixedStyle( CAMERA_STYLE_HOUSING ) )
	{
		fCurrRotateSpeed = m_fMouseRotateSpeedByHousing;
		fCurrRotateAngle = m_fMouseRotateAngleByHousing;

		ioLSMouse* pMouse = g_App.GetMouse();
		if( pMouse )
			pMouse->SetHousingCameraCursor();
	}

	bool bRotateChanged = false;
	if( bDeskTopOver )
	{
		if( pMouse->IsMBtnDown() || pMouse->IsRBtnDown() )
		{
			POINT ptMove = g_App.GetMouseAmount();
			
			D3DXVECTOR3 vPrevMouseMove = m_vCamMouseAmt;
			if( abs( ptMove.x ) >= abs( ptMove.y ) )
			{
				if( ptMove.x != 0 )
					m_vCamMouseAmt.x += ( (float)ptMove.x * fCurrRotateSpeed * fTimePerSec );
			}
			else
			{
				if( ptMove.y != 0 )
					m_vCamMouseAmt.y += ( (float)ptMove.y * fCurrRotateSpeed * fTimePerSec );
			}

			m_vCamMouseAmt.x = max( -fCurrRotateAngle, min( m_vCamMouseAmt.x, fCurrRotateAngle ) );
			m_vCamMouseAmt.y = max( -fCurrRotateAngle, min( m_vCamMouseAmt.y, fCurrRotateAngle ) );
			
			if( vPrevMouseMove != m_vCamMouseAmt )
			{
				bRotateChanged = true;
				m_dwMouseCamCtrlTime = FRAMEGETTIME() + m_dwMouseCamCtrlRestoreTime;
			}
		}
	}

	int iWheelDelta = 0;
	if( bDeskTopOver && ( HasCameraFixedStyle( CAMERA_STYLE_MOVIE ) || HasCameraFixedStyle( CAMERA_STYLE_VIEW ) || HasCameraFixedStyle( CAMERA_STYLE_HOUSING ) ) )
	{
		iWheelDelta = pMouse->GetWheelDelta();
		if( iWheelDelta == 0 )
		{
			if( g_KeyManager.IsZoomInKeyRepeat() )
				iWheelDelta += m_dwZoomKeyboardTick;
			else if( g_KeyManager.IsZoomOutKeyRepeat() )
				iWheelDelta -= m_dwZoomKeyboardTick;
		}

		if( iWheelDelta != 0 )
		{
			float fAmtRate = (float)iWheelDelta / 120.0f;			

			if( HasCameraFixedStyle( CAMERA_STYLE_HOUSING ) )
			{
				m_vCamMouseAmt.z += fAmtRate * m_fMouseZoomSpeedByHousing;
				m_vCamMouseAmt.z = max( m_fMouseZoomMinByHousing, min( m_vCamMouseAmt.z, m_fMouseZoomMaxByHousing ) );

				m_dwMouseCamCtrlTime = FRAMEGETTIME() + m_dwMouseCamCtrlRestoreTime;

				SetCameraModeFixedByHousing();
			}
			else
			{
				m_vCamMouseAmt.z += fAmtRate * m_fMouseZoomSpeed;
				m_vCamMouseAmt.z = max( m_fMouseZoomMin, min( m_vCamMouseAmt.z, m_fMouseZoomMax ) );

				m_dwMouseCamCtrlTime = FRAMEGETTIME() + m_dwMouseCamCtrlRestoreTime;

				SetCameraModeFixed( CAMERA_STYLE_MOVIE );
			}
		}
	}	

	if( COMPARE( m_dwMouseCamCtrlTime, 1, FRAMEGETTIME() ) && m_dwCameraFixedStyle == CAMERA_STYLE_NORMAL )
	{
		if( !bRotateChanged )
		{
			m_vCamMouseAmt.x = ReduceMouseCamAmount( m_vCamMouseAmt.x, fCurrRotateSpeed * fTimePerSec );
			m_vCamMouseAmt.y = ReduceMouseCamAmount( m_vCamMouseAmt.y, fCurrRotateSpeed * fTimePerSec );
		}

		if( iWheelDelta == 0 )
		{
			m_vCamMouseAmt.z = ReduceMouseCamAmount( m_vCamMouseAmt.z, m_fMouseZoomRestoreSpeed * fTimePerSec );
		}

		m_vCamMouseAmt.x = m_vCamMouseAmt.y = m_vCamMouseAmt.z = 0.0f;
		if( m_vCamMouseAmt.x == 0.0f && m_vCamMouseAmt.y == 0.0f && m_vCamMouseAmt.z == 0.0f )
		{
			m_dwMouseCamCtrlTime = 0;
			return;
		}
	}

	if( bDebug )
	{
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_SHADOW );
		g_FontMgr.SetTextColor( 255, 255, 0 );
		g_FontMgr.SetBkColor( 0, 0, 0 );
		g_FontMgr.PrintText( FLOAT1, 101.0f, 0.7f, STR(1), m_vCamMouseAmt.x, m_vCamMouseAmt.y, m_vCamMouseAmt.z );
		g_FontMgr.PrintText( FLOAT1, 123.0f, 0.7f, STR(2), pCtrl->GetPosition().x, pCtrl->GetPosition().y, pCtrl->GetPosition().z );
		g_FontMgr.PrintText( FLOAT1, 145.0f, 0.7f, STR(3), m_vCamMouseTargetCurLookAt.x, m_vCamMouseTargetCurLookAt.y, m_vCamMouseTargetCurLookAt.z, m_fMouseCameraZ );
	}

	D3DXVECTOR3 vLookAt;
	if( IsNowMouseCamera() )
		vLookAt = m_vCamMouseFixedTargetLookAt;
	else
		vLookAt = pCtrl->GetLookAt();

	D3DXVECTOR3 vDiff   = pCtrl->GetPosition() - vLookAt;
	D3DXVECTOR3 vCamDir;
	D3DXVec3Normalize( &vCamDir, &vDiff );
	float fCurZ = D3DXVec3Length( &vDiff );

	if( fabs( m_fMouseCameraZ - m_fMousePreTargetZ ) > FLOAT05 || m_vCamMouseAmt.z != 0.0f )
	{
		float fTargetZ = fCurZ - m_vCamMouseAmt.z;
		fTargetZ = max( m_fMouseZoomTotalMin, min( m_fMouseZoomTotalMax, fTargetZ ) );
		float fSpringDistDamp = MouseSpringDistDamp( 5.0f, FLOAT05, m_fMouseCameraZ, fTargetZ, m_fMousePreTargetZ );
		if( fabs( fSpringDistDamp ) > FLOAT05 )
		{
			m_fMouseCameraZ -= fSpringDistDamp;
		}
		m_fMouseCameraZ = max( m_fMouseZoomTotalMin, min( m_fMouseZoomTotalMax, m_fMouseCameraZ ) );
		m_fMousePreTargetZ = fTargetZ;
		pCtrl->SetPosition( vLookAt + vCamDir * m_fMouseCameraZ );
	}
	else
	{
		m_fMouseCameraZ = m_fMousePreTargetZ = fCurZ;
	}
	
	D3DXVECTOR3 vCamMouseRot( m_vCamMouseAmt.x, m_vCamMouseAmt.y, 0.0f );
	m_vMouseCameraRot = SpringDamp( 5.0f, 0.5f,	m_vMouseCameraRot, vCamMouseRot, m_vMousePreTargetRot, 0.1f );

	if( m_vMouseCameraRot.x != 0.0f || m_vMouseCameraRot.y != 0.0f )
	{
		D3DXQUATERNION qtRotate;
		qtRotate = ioMath::MakeQuaternionByDegree( m_vMouseCameraRot.x,
												   m_vMouseCameraRot.y,
												   0.0f );

		D3DXVECTOR3 vCenterZ( 0.0f, 0.0f, -FLOAT1 );
		D3DXVECTOR3 vNewCamDir = qtRotate * vCamDir;

		float fNewCos   = D3DXVec3Dot( &vNewCamDir, &vCenterZ );
		float fLimitCos = cosf( DEGtoRAD(fCurrRotateAngle) );

		float fRotateR = acosf( max( fNewCos, fLimitCos ) );

		D3DXVECTOR3 vRotAxis;
		D3DXVec3Cross( &vRotAxis, &vCenterZ, &vNewCamDir );
		D3DXQuaternionRotationAxis( &qtRotate, &vRotAxis, fRotateR );
		pCtrl->SetPosition( vLookAt + ( qtRotate * vCenterZ ) * m_fMouseCameraZ );

		D3DXVECTOR3 vNewCamPos = pCtrl->GetPosition();
		if( vLookAt.y > vNewCamPos.y )
		{
			vNewCamPos.y = vLookAt.y;
			pCtrl->SetPosition( vNewCamPos );
		}
	}
	m_vMousePreTargetRot = vCamMouseRot;

	if( bDebug )
	{
		g_FontMgr.PrintText( FLOAT1, 168.0f, 0.7f, STR(4), pCtrl->GetPosition().x, pCtrl->GetPosition().y, pCtrl->GetPosition().z );
		g_FontMgr.PrintText( FLOAT1, 190.0f, 0.7f, STR(5), pCtrl->GetLookAt().x, pCtrl->GetLookAt().y, pCtrl->GetLookAt().z );
		g_FontMgr.PrintText( FLOAT1, 213.0f, 0.7f, STR(6), 
			m_vMouseCameraRot.x, m_vMouseCameraRot.y, m_vMouseCameraRot.z,
			m_vMousePreTargetRot.x, m_vMousePreTargetRot.y, m_vMousePreTargetRot.z );
	}
}

float ioPlayStage::ReduceMouseCamAmount( float fPreAmt, float fReduceAmt )
{
	if( fPreAmt == 0.0f )
		return 0.0f;

	float fNewAmt;
	if( fPreAmt > 0.0f )
		fNewAmt = fPreAmt - fReduceAmt;
	else
		fNewAmt = fPreAmt + fReduceAmt;

	// 부호가 달라졌다.
	if( fNewAmt * fPreAmt < 0.0f )
	{
		fNewAmt = 0.0f;
	}

	return fNewAmt;
}

D3DXVECTOR3 ioPlayStage::CheckMapCameraLimit( D3DXVECTOR3 vNewLookAt ) const
{
	if( m_pSceneMgr )
		return m_pSceneMgr->CheckMapCameraLimit( vNewLookAt );
	return vNewLookAt;
}

float ioPlayStage::GetArcadeCameraDistance( ioBaseChar *pTarget )
{
	if( !pTarget )
		return 0.0f;

	bool bMeHighPos;
	float fHalfXGap, fHalfYGap;
	GetArcadeCameraGapXY( pTarget, fHalfXGap, fHalfYGap, bMeHighPos, FLOAT1 );	
	float fDistance = fabs( fHalfXGap );
	fDistance = max( 0.0f, fDistance - m_fArcadeCameraDistanceLimit );
	return fDistance;
}

void ioPlayStage::GetArcadeCameraGapXY(  ioBaseChar *pTarget, float &rXGap, float &rYGap, bool &bMeHighPos, float fGapOffset )
{
	bMeHighPos = false;
	rXGap = rYGap = 0.0f;
	if( m_bArcadeCameraWorking == false )
		return;

	ioFightClubMode *pFightClubMode = ToFightClubMode( m_pPlayMode );
	if( !pFightClubMode ) return;
	
	const ioHashString &kChampionName   = pFightClubMode->GetChampionName();
	const ioHashString &kChallengerName = pFightClubMode->GetChallengerName();

	ioBaseChar *pTarget2    = NULL;
	ioBaseChar *pChampion   = GetBaseChar( kChampionName );
	ioBaseChar *pChallenger = GetBaseChar( kChallengerName );
	if( pChampion == pTarget )	// 챔피언이 타겟이면 도전자
	{
		if( pChallenger == NULL ) return;
		if( pChallenger->GetState() == CS_VIEW ||
			pChallenger->GetState() == CS_LOADING ||
			pChallenger->GetState() == CS_OBSERVER ) return;

		pTarget2 = pChallenger;
	}
	else // 도전자가 타겟이면 챔피언
	{
		if( pChampion == NULL ) return;
		if( pChampion->GetState() == CS_VIEW ||
			pChampion->GetState() == CS_LOADING ||
			pChampion->GetState() == CS_OBSERVER ) return;

		pTarget2 = pChampion;
	}

	if( pTarget == NULL || pTarget2 == NULL ) return;

	// X축, Y축만 변경
	rXGap = ( pTarget->GetWorldPosition().x - pTarget2->GetWorldPosition().x ) * fGapOffset;
	rYGap = ( pTarget->GetWorldPosition().y - pTarget2->GetWorldPosition().y ) * fGapOffset;	

	ioBaseChar *pOwnerChar = GetOwnerChar();
	if( pTarget->GetWorldPosition().y > pTarget2->GetWorldPosition().y )
	{
		if( pTarget == pOwnerChar )
		{
			bMeHighPos = true;	
		}
	}
	else if( pTarget2->GetWorldPosition().y > pTarget->GetWorldPosition().y )
	{
		if( pTarget2 == pOwnerChar )
		{
			bMeHighPos = true;	
		}
	}

	// 대기자는 아래에있는 유저 Y값을 따른다.
	if( pTarget != pOwnerChar && pTarget2 != pOwnerChar )
	{
		if( pTarget->GetWorldPosition().y > pTarget2->GetWorldPosition().y )
		{
			bMeHighPos = true;
			rYGap = pTarget->GetWorldPosition().y - pTarget2->GetWorldPosition().y;
		}
	}
}

D3DXVECTOR3 ioPlayStage::UpdateArcadeNewLookAt( ioBaseChar *pTarget, const D3DXVECTOR3 &vPos )
{
	if( m_bArcadeCameraWorking == false )
		return vPos;
	if( pTarget == NULL )
		return vPos;
	
	// 상대 유저와의 거리 중앙이 카메라 포지션이 된다.
	bool bMeHighPos;
	float fXPos, fYPos;
	GetArcadeCameraGapXY( pTarget, fXPos, fYPos, bMeHighPos, FLOAT05 );
	if( !bMeHighPos )
		fYPos = 0.0f;

	D3DXVECTOR3 vTargetPos = vPos;
	D3DXVECTOR3 vReturnPos = D3DXVECTOR3( vTargetPos.x - fXPos, vTargetPos.y - fYPos, vTargetPos.z );
	return vReturnPos;
}

D3DXVECTOR3 ioPlayStage::UpdateNewLookAt( ioBaseChar *pTarget, const D3DXVECTOR3 &vPreLookAt )
{	
	bool bLookAtMoving = true;
	D3DXVECTOR3 vNewLookAt;
	D3DXVECTOR3 vPos = pTarget->GetWorldPosition();
	vPos = UpdateArcadeNewLookAt( pTarget, vPos );

	if( m_dwViewMovingTime > 0 && m_dwCamSlerpTime > 0 )
	{
		DWORD dwGap = FRAMEGETTIME() - m_dwViewMovingTime;
		float fRate = (float)dwGap / (float)m_dwCamSlerpTime;
		fRate = max( 0.0f, min( fRate, FLOAT1 ) );
		if( fRate < FLOAT1 )
		{
			fRate = ( cosf( D3DX_PI * fRate ) + FLOAT1 ) * FLOAT05;
			fRate = FLOAT1 - fRate;

			D3DXVec3Lerp( &vNewLookAt, &m_vLerpStart, &vPos, fRate );
		}
		else
		{
			vNewLookAt = vPos;
			m_dwViewMovingTime = 0;
			bLookAtMoving = false;
		}
	}
	else
	{
		ioCameraEvent *pCameraEvent = g_CameraEventMgr.GetCurCameraEvent();
		if( pCameraEvent && pCameraEvent->IsForceLookTargetChar() )
		{
			vNewLookAt = vPos;
			bLookAtMoving = false;
		}
		else if( pTarget && pTarget->CheckMagicCircle() )
		{
			if( m_bSetCameraToNowOwner )
			{
				vNewLookAt = pTarget->GetMagicCirclePos();
				bLookAtMoving = false;
			}
			else
			{
				D3DXVECTOR3 vCurPos = vPreLookAt;
				vCurPos.y -= m_fCamLookAtHeightOffset;
				vNewLookAt = SpringDamp( m_fSpringConst, m_fDampConst, vCurPos, pTarget->GetMagicCirclePos(), pTarget->GetMagicCirclePrePos() );
				bLookAtMoving = false;
				m_bSpringDampUse = false;
			}
		}
		else if( pTarget && pTarget->CheckAreaWeaponControl() )
		{
			if( m_bSetCameraToNowOwner )
			{
				vNewLookAt = pTarget->GetAreaWeaponPos();
				bLookAtMoving = false;
			}
			else
			{
				D3DXVECTOR3 vCurPos = vPreLookAt;
				vCurPos.y -= m_fCamLookAtHeightOffset;
				vNewLookAt = SpringDamp( m_fSpringConst, m_fDampConst, vCurPos, pTarget->GetAreaWeaponPos(), pTarget->GetAreaWeaponPrePos() );
				bLookAtMoving = false;
				m_bSpringDampUse = false;
			}
		}
		else if( pTarget && pTarget->CheckDummyCharControl() )
		{
			vNewLookAt = pTarget->GetDummyCharControlPos();
			bLookAtMoving = false;
			m_bSpringDampUse = false;
		}
		else if( pTarget && pTarget->CheckWeaponCharControl() )
		{
			vNewLookAt = pTarget->GetWeaponCharControlPos();
			bLookAtMoving = false;
			m_bSpringDampUse = false;
		}
		else if( pTarget && pTarget->CheckTargetTeleport() )
		{
			vNewLookAt = pTarget->GetTargetTeleportPos();
			bLookAtMoving = false;
			m_bSpringDampUse = false;
		}
		else if( pTarget && pTarget->CheckPossessionChar() )
		{
			vNewLookAt = pTarget->GetPossessionPos();
			bLookAtMoving = false;
			m_bSpringDampUse = false;
		}
		else if( pTarget && pTarget->CheckExceptionSkillCamera() )
		{
			if ( pTarget->IsFixedExceptionCamera() )
				return pTarget->GetExceptionSkillCameraPos();
			else
			{
				D3DXVECTOR3 vCurPos = vPreLookAt;
				D3DXVECTOR3 vTarPos = pTarget->GetExceptionSkillCameraPos();
				D3DXVECTOR3 vPreTarPos = vPreLookAt;
				D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

				float fCurMaxSpeed = pTarget->GetMaxSpeed( false );
				float fCurSpeed = pTarget->GetCurSpeed();
				fCurSpeed = min( fCurSpeed, fCurMaxSpeed );
				if( fCurMaxSpeed > 0.0f )
				{
					float fSpeedRate = fCurSpeed / fCurMaxSpeed;

					if( fCurMaxSpeed >= m_fSpringXOffset )
						vDir.x *= m_fSpringXOffset * fSpeedRate;
					else
						vDir.x *= fCurMaxSpeed * fSpeedRate;

					if( fCurMaxSpeed >= m_fSpringZOffset )
						vDir.z *= m_fSpringZOffset * fSpeedRate;
					else
						vDir.z *= m_fSpringZOffset * fSpeedRate;
				}
				else
				{
					vDir.x = vDir.z = 0.0f;
				}

				vCurPos.y -= m_fCamLookAtHeightOffset;
				vTarPos += vDir;
				vPreTarPos += vDir;

				vNewLookAt = SpringDamp( m_fSpringConst, m_fDampConst, vCurPos, vTarPos, vPreTarPos );

				bLookAtMoving = false;
				m_bSpringDampUse = false;
			}
		}
		else
		{
			vNewLookAt = vPos;
			bLookAtMoving = false;
		}
	}

	if( m_bNoneCameraWorking || m_bSetCameraToNowOwner || m_bArcadeCameraWorking )
		return vNewLookAt;

	if( m_bTargetLookAt )
		return vNewLookAt;

	if( pTarget->GetState() == CS_DROP_ZONE_DOWN || 
		pTarget->GetState() == CS_FLY ||
		pTarget->GetState() == CS_WARP_STATE )
	{
		m_bSpringDampUse = false;
	}

	ioCameraEvent *pCameraEvent = g_CameraEventMgr.GetCurCameraEvent();
	if( pCameraEvent )
	{
		if( pCameraEvent->CheckCameraEventEnd() )
		{
			g_CameraEventMgr.NotifyCEDestroyed();
			pCameraEvent = NULL;
		}
		else
		{
			vNewLookAt = pCameraEvent->CheckOffSet( vNewLookAt, pTarget->GetTargetRot() );
		}
	}
	else if( !bLookAtMoving && m_bSpringDampUse )
	{
		D3DXVECTOR3 vCurPos = vPreLookAt;
		D3DXVECTOR3 vTarPos = vPos;
		D3DXVECTOR3 vPreTarPos = pTarget->GetPrePosition();
		D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		float fCurMaxSpeed = pTarget->GetMaxSpeed( false );
		float fCurSpeed = pTarget->GetCurSpeed();
		fCurSpeed = min( fCurSpeed, fCurMaxSpeed );
		if( fCurMaxSpeed > 0.0f )
		{
			float fSpeedRate = fCurSpeed / fCurMaxSpeed;

			if( fCurMaxSpeed >= m_fSpringXOffset )
				vDir.x *= m_fSpringXOffset * fSpeedRate;
			else
				vDir.x *= fCurMaxSpeed * fSpeedRate;

			if( fCurMaxSpeed >= m_fSpringZOffset )
				vDir.z *= m_fSpringZOffset * fSpeedRate;
			else
				vDir.z *= m_fSpringZOffset * fSpeedRate;
		}
		else
		{
			vDir.x = vDir.z = 0.0f;
		}

		vCurPos.y -= m_fCamLookAtHeightOffset;
		vTarPos += vDir;
		vPreTarPos += vDir;

		vNewLookAt = SpringDamp( m_fSpringConst, m_fDampConst, vCurPos, vTarPos, vPreTarPos );
	}
	
	m_vCamMouseFixedTargetLookAt = vPos;
	if( IsNowMouseCamera() )
	{
		vNewLookAt = UpdateCameraFixedLookAt( pTarget, vNewLookAt );
	}
	return vNewLookAt;
}

D3DXVECTOR3 ioPlayStage::UpdateNewObserverLookAt( ioBaseChar *pTarget, const D3DXVECTOR3 &vPreLookAt )
{
	D3DXVECTOR3 vNewLookAt;
	if( m_vObserverLookAt == D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) )
		m_vObserverLookAt = pTarget->GetWorldPosition();

	D3DXVECTOR3 vPos = m_vObserverLookAt;
	vPos = UpdateArcadeNewLookAt( pTarget, vPos );		
	vNewLookAt = vPos;

	if( m_bSpringDampUse )
	{
		D3DXVECTOR3 vCurPos = vPreLookAt;
		D3DXVECTOR3 vTarPos = vPos;
		D3DXVECTOR3 vPreTarPos = pTarget->GetPrePosition();
		D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		float fCurMaxSpeed = pTarget->GetMaxSpeed( false );
		float fCurSpeed = pTarget->GetCurSpeed();
		fCurSpeed = min( fCurSpeed, fCurMaxSpeed );
		if( fCurMaxSpeed > 0.0f )
		{
			float fSpeedRate = fCurSpeed / fCurMaxSpeed;

			if( fCurMaxSpeed >= m_fSpringXOffset )
				vDir.x *= m_fSpringXOffset * fSpeedRate;
			else
				vDir.x *= fCurMaxSpeed * fSpeedRate;

			if( fCurMaxSpeed >= m_fSpringZOffset )
				vDir.z *= m_fSpringZOffset * fSpeedRate;
			else
				vDir.z *= m_fSpringZOffset * fSpeedRate;
		}
		else
		{
			vDir.x = vDir.z = 0.0f;
		}

		vCurPos.y -= m_fCamLookAtHeightOffset;
		vTarPos += vDir;
		vPreTarPos += vDir;

		vNewLookAt = SpringDamp( m_fSpringConst, m_fDampConst, vCurPos, vTarPos, vPreTarPos );
	}

	m_vCamMouseFixedTargetLookAt = vPos;
	return vNewLookAt;
}

D3DXVECTOR3 ioPlayStage::UpdateCameraFixedLookAt( ioBaseChar *pTarget, D3DXVECTOR3 vNewLookAt )
{
	if( !IsNowMouseCamera() || !pTarget )
		return vNewLookAt;

	vNewLookAt = pTarget->GetWorldPosition();
	D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

	if( vDir != m_vCamMouseTargetPrevDir )
	{
		m_vCamMouseTargetCurLookAt		= m_vCamMouseTargetPrevPos;
		m_vCamMouseTargetPrevDir		= vDir;
		m_dwCamMouseLookAtMoveCurTime	= FRAMEGETTIME();
	}

	float fRate = FLOAT1;
	DWORD dwGap = FRAMEGETTIME() - m_dwCamMouseLookAtMoveCurTime;
	if( dwGap < m_dwCamMouseLookAtMoveTime )
		fRate = min( FLOAT1, sinf( ( D3DX_PI / 2 ) * (float)( dwGap ) / m_dwCamMouseLookAtMoveTime ) );

	D3DXVECTOR3 vNewPos = vNewLookAt + ( vDir * m_fCamMouseFixedLookAtDist ) + D3DXVECTOR3( 0.0f, m_fCamMouseFixedLookAtDistHeight,0.0f);		
	D3DXVECTOR3 vGapPos = vNewPos - m_vCamMouseTargetCurLookAt;
	m_vCamMouseTargetPrevPos = m_vCamMouseTargetCurLookAt + ( vGapPos * fRate );
	return m_vCamMouseTargetPrevPos;
}

void ioPlayStage::UpdateCameraPos( ioCamera *pCamera, ioBaseChar *pTarget, D3DXVECTOR3 vNewLookAt )
{
	if( !pCamera || !pTarget )
		return;

	ioLookAtCameraController *pCtrl = ToLookAtCtrl( pCamera->GetController() );
	if( !pCtrl )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::UpdateCtrlPos - Not LookAt Controller" );
		return;
	}

	vNewLookAt = CheckMapCameraLimit( vNewLookAt );

    D3DXVECTOR3 vCamLookAt;
	vCamLookAt.x = vNewLookAt.x;
	vCamLookAt.y = vNewLookAt.y + m_fCamLookAtHeightOffset;
	vCamLookAt.z = vNewLookAt.z;	

	m_vCurLookAt = vCamLookAt;
	if( !m_bNoneCameraWorking && !m_bArcadeCameraWorking )
		vCamLookAt += GetCameraShakeAmt();

	pCtrl->SetLookAt( vCamLookAt );
	if( (m_fCurCamHeight != pTarget->GetCamHeight() ) ||
		(m_fCurCamDistance != pTarget->GetCamDistance() ) ||
		(m_fCurCamFov != pTarget->GetCamFov() ) )
	{
		m_dwViewMovingTimeItem = FRAMEGETTIME();

		m_fStartHeight = m_fCurCamHeight;
		m_fStartDistance = m_fCurCamDistance;
		m_fStartFov = m_fCurCamFov;

		m_fCurCamHeight = pTarget->GetCamHeight();
		m_fCurCamDistance = pTarget->GetCamDistance();
		m_fCurCamFov = pTarget->GetCamFov();
	}
	
	float fCamDistance, fCamHeight, fCamFov;
	m_dwCamZoomTimeItem = pTarget->GetCamZoomTime();
	if( m_dwViewMovingTimeItem > 0 && m_dwCamZoomTimeItem > 0 )
	{
		DWORD dwGap = FRAMEGETTIME() - m_dwViewMovingTimeItem;
		float fRate = (float)dwGap / (float)m_dwCamZoomTimeItem;
		fRate = max( 0.0f, min( fRate, FLOAT1 ) );

		if( fRate < FLOAT1 )
		{
			fRate = ( cosf( D3DX_PI * fRate ) + FLOAT1 ) * FLOAT05;

			float fInvRate = FLOAT1 - fRate;
			fCamHeight = m_fStartHeight * fRate + pTarget->GetCamHeight() * fInvRate;
			fCamDistance = m_fStartDistance * fRate + pTarget->GetCamDistance() * fInvRate;
			fCamFov = m_fStartFov * fRate + pTarget->GetCamFov() * fInvRate;
		}
		else
		{
			fCamHeight = pTarget->GetCamHeight();
			fCamDistance = pTarget->GetCamDistance();
			fCamFov = pTarget->GetCamFov();

			m_dwViewMovingTimeItem = 0;
		}
	}
	else
	{
		fCamHeight = pTarget->GetCamHeight();
		fCamDistance = pTarget->GetCamDistance();
		fCamFov = pTarget->GetCamFov();
	}

	if( m_bNoneCameraWorking || m_bSetCameraToNowOwner )
	{		
		fCamHeight = m_fStartHeight;
		fCamDistance = m_fStartDistance;
		fCamFov = m_fStartFov;
	}
	else if( m_bArcadeCameraWorking )
	{
		DWORD  dwArcadeDistance = GetArcadeCameraDistance( pTarget );
		float  fHeightRate      = FLOAT1 + ( (float)dwArcadeDistance / m_fStartDistance );		
		fCamHeight = m_fStartHeight * fHeightRate;
		fCamDistance = m_fStartDistance + dwArcadeDistance;
		fCamFov = m_fStartFov;
	}

	D3DXVECTOR3 vNewCamPos = vCamLookAt;
	vNewCamPos.y += fCamHeight;
	vNewCamPos.z -= fCamDistance;
	if( IsNowMouseCamera() )
	{
		m_vCamMouseFixedTargetLookAt = CheckMapCameraLimit( m_vCamMouseFixedTargetLookAt );
		m_vCamMouseFixedTargetLookAt.y = m_vCamMouseFixedTargetLookAt.y + m_fCamLookAtHeightOffset;
		vNewCamPos    = m_vCamMouseFixedTargetLookAt;
		vNewCamPos.y += fCamHeight;
		vNewCamPos.z -= fCamDistance;
	}

	pCamera->SetFov( fCamFov );
	pCamera->SetCameraRoll( 0.0f );

	if( m_bNoneCameraWorking || m_bSetCameraToNowOwner || m_bArcadeCameraWorking )
	{
		pCamera->SetPosition( vNewCamPos );
		return;
	}

	D3DXVECTOR3 vNewGapPos = vNewCamPos;
	ioCameraEvent *pCameraEvent = g_CameraEventMgr.GetCurCameraEvent();
	if( pCameraEvent && !IsNowMouseCamera() )
	{
		m_dwZoomStartTime = 0;
		m_dwChargingStartTime = 0;

		m_bCameraZooming = true;
		m_bCameraChanging = false;

		m_fCurGapDistance = 0.0f;
		m_fCurGapHeight = 0.0f;
		m_fCurGapFov = 0.0f;

		if( pCameraEvent->CheckCameraEventEnd() )
		{
			g_CameraEventMgr.NotifyCEDestroyed();
			pCameraEvent = NULL;
		}
		else
		{
			vNewCamPos = pCameraEvent->ProcessCameraEvent( pCtrl->GetLookAt(),
														   vNewCamPos,
														   fCamFov,
														   pTarget->GetTargetRot() );

			pCamera->SetFov( pCameraEvent->GetCameraFov() );
			pCamera->SetCameraRoll( pCameraEvent->GetCameraRoll() );
		}
	}
	else 
	{
		if( !IsNowMouseCamera() )
		{
			D3DXVECTOR3 vLookAt = pCtrl->GetLookAt();
			vNewCamPos = RunDashCamera( vLookAt, vNewCamPos, fCamFov, pTarget );
			vNewCamPos = ChargingCamera( vLookAt, vNewCamPos, fCamFov, pTarget );
			vNewCamPos = BuffCamera( vLookAt, vNewCamPos, fCamFov, pTarget );
			vNewCamPos = SkillCamera( vLookAt, vNewCamPos, fCamFov, pTarget );	
		}		
		pCamera->SetFov( fCamFov );
		pCamera->SetCameraRoll( GetCameraRollAmt() );
	}
	
	if( IsNowMouseCamera() )
	{
		pCamera->SetFov( m_fCameraFixedFov );
        pCamera->SetPosition( vNewCamPos );
	}
	else
	{
		pCamera->SetPosition( vNewCamPos );	
	}
}

void ioPlayStage::UpdateObserverCameraPos( ioCamera *pCamera, ioBaseChar *pTarget, D3DXVECTOR3 vNewLookAt )
{
	if( !pCamera || !pTarget )
		return;

	ioLookAtCameraController *pCtrl = ToLookAtCtrl( pCamera->GetController() );
	if( !pCtrl )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::UpdateCtrlPos - Not LookAt Controller" );
		return;
	}

	vNewLookAt = CheckMapCameraLimit( vNewLookAt );

    D3DXVECTOR3 vCamLookAt;
	vCamLookAt.x = vNewLookAt.x;
	vCamLookAt.y = vNewLookAt.y + m_fCamLookAtHeightOffset;
	vCamLookAt.z = vNewLookAt.z;	

	m_vCurLookAt = vCamLookAt;
	pCtrl->SetLookAt( vCamLookAt );
	
	D3DXVECTOR3 vNewCamPos = vCamLookAt;
	vNewCamPos.y += pTarget->GetCamHeight();
	vNewCamPos.z -= pTarget->GetCamDistance();

	pCamera->SetFov( pTarget->GetCamFov() );
	pCamera->SetCameraRoll( 0.0f );
	pCamera->SetPosition( vNewCamPos );	

	ioUserKeyInput::DirKeyInput eDirKey = pTarget->CheckCurDirKey();
	if( eDirKey != ioUserKeyInput::DKI_NONE )
	{
		float fYawD = 180.0f + 45.0f * ( eDirKey - ioUserKeyInput::DKI_UP );
		fYawD = ioMath::ArrangeHead( fYawD );

		float fYawR = DEGtoRAD( fYawD );
		D3DXQUATERNION qtRotate;
		D3DXQuaternionRotationYawPitchRoll( &qtRotate, fYawR, 0.0f, 0.0f );

		D3DXVECTOR3 vMoveDir = qtRotate * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vMoveDir, &vMoveDir );
		m_vObserverLookAt += vMoveDir * m_fObserverCameraSpeed;
	}
	if( pTarget->IsJumpKeyDown() )
		m_vObserverLookAt.y += m_fObserverCameraSpeed;
	else if( pTarget->IsDefenseKeyDown() )
		m_vObserverLookAt.y -= m_fObserverCameraSpeed;

	m_vObserverLookAt = CheckMapCameraLimit( m_vObserverLookAt );
}

D3DXVECTOR3 ioPlayStage::RunDashCamera( const D3DXVECTOR3 &vLookAt,
										const D3DXVECTOR3 &vTarPos,
										float &fCamFov,
										ioBaseChar *pTarget )
{
	if( !pTarget )
		return vTarPos;

	int iCameraType = pTarget->CheckCameraType();
	if( iCameraType != CT_DEFAULT && iCameraType != CT_RUNDASH )
	{
		m_dwZoomStartTime = 0;
		m_bCameraZooming = true;
		return vTarPos;
	}

	D3DXVECTOR3 vNewPosition = vTarPos;

	if( pTarget->CheckCameraType() == CT_RUNDASH )
	{
		if( m_bCameraZooming )
		{
			m_bCameraZooming = false;
			m_bCameraSkilling = false;

			float fDistance = 0.0f, fHeight = 0.0f, fFov = 0.0f;
			DWORD dwZoomTime = 0;
			pTarget->GetRunDashCamera( fDistance, fHeight, fFov, dwZoomTime );

			m_fMaxGapDistance = fDistance - ( vLookAt.z-vTarPos.z );
			m_fGapDistance = m_fMaxGapDistance - m_fCurGapDistance;
			
			m_fMaxGapHeight = fHeight - ( vTarPos.y-vLookAt.y );
			m_fGapHeight = m_fMaxGapHeight - m_fCurGapHeight;

			m_fMaxGapFov = fFov - fCamFov;
			m_fGapFov = m_fMaxGapFov - m_fCurGapFov;

			m_dwZoomStartTime = FRAMEGETTIME();

			if( fabs(m_fMaxGapDistance) <= FLOAT1 && m_fCurGapDistance != 0.0f )
				m_dwZoomSlerpTime = dwZoomTime;
			else if( fabs(m_fMaxGapDistance) <= FLOAT1 && m_fCurGapDistance == 0.0f )
				m_dwZoomSlerpTime = 0;
			else
				m_dwZoomSlerpTime = dwZoomTime * fabs(m_fGapDistance / m_fMaxGapDistance);
		}

		if( m_dwZoomStartTime > 0 && m_dwZoomSlerpTime > 0 )
		{
			DWORD dwGap = FRAMEGETTIME() - m_dwZoomStartTime;
			float fRate = (float)dwGap / (float)m_dwZoomSlerpTime;
			fRate = max( 0.0f, min( fRate, FLOAT1 ) );

			if( fRate < FLOAT1 )
			{
				fRate = ( cosf( D3DX_PI * fRate ) + FLOAT1 ) * FLOAT05;

				m_fCurGapDistance = m_fMaxGapDistance - m_fGapDistance;
				m_fCurGapDistance +=  m_fGapDistance * (1.0f - fRate);
				m_fCurGapHeight = m_fMaxGapHeight - m_fGapHeight;
				m_fCurGapHeight += m_fGapHeight * (1.0f - fRate);
				m_fCurGapFov = m_fMaxGapFov - m_fGapFov;
				m_fCurGapFov += m_fGapFov * (1.0f - fRate);

				vNewPosition.z -= m_fCurGapDistance;
				vNewPosition.y += m_fCurGapHeight;
				fCamFov += m_fCurGapFov;
			}
			else
			{
				vNewPosition.z -= m_fMaxGapDistance;
				vNewPosition.y += m_fMaxGapHeight;
				fCamFov += m_fMaxGapFov;
				m_fCurGapDistance = m_fMaxGapDistance;
				m_fCurGapHeight = m_fMaxGapHeight;
				m_fCurGapFov = m_fMaxGapFov;
				m_dwZoomStartTime = 0;
			}
		}
		else
		{
			vNewPosition.z -= m_fMaxGapDistance;
			vNewPosition.y += m_fMaxGapHeight;
			fCamFov += m_fMaxGapFov;
			m_fCurGapDistance = m_fMaxGapDistance;
			m_fCurGapHeight = m_fMaxGapHeight;
			m_fCurGapFov = m_fMaxGapFov;
		}
	}
	else
	{
		if( !m_bCameraZooming )
		{
			m_bCameraZooming = true;

			float fDistance = 0.0f, fHeight = 0.0f, fFov = 0.0f;
			DWORD dwZoomTime = 0;
			pTarget->GetRunDashCamera( fDistance, fHeight, fFov, dwZoomTime );

			m_fMaxGapDistance = fDistance - ( vLookAt.z-vTarPos.z );
			m_fGapDistance = m_fCurGapDistance;
			m_fGapHeight = m_fCurGapHeight;
			m_fGapFov = m_fCurGapFov;

			m_dwZoomStartTime = FRAMEGETTIME();
			if( fabs(m_fMaxGapDistance) <= FLOAT1 && m_fCurGapDistance != 0.0f )
				m_dwZoomSlerpTime = dwZoomTime;
			else if( fabs(m_fMaxGapDistance) <= FLOAT1 && m_fCurGapDistance == 0.0f )
				m_dwZoomSlerpTime = 0;
			else
				m_dwZoomSlerpTime = dwZoomTime * fabs(m_fGapDistance / m_fMaxGapDistance);
		}
		
		if( m_dwZoomStartTime > 0 && m_dwZoomSlerpTime > 0 )
		{
			DWORD dwGap = FRAMEGETTIME() - m_dwZoomStartTime;
			float fRate = (float)dwGap / (float)m_dwZoomSlerpTime;
			fRate = max( 0.0f, min( fRate, FLOAT1 ) );

			if( fRate < FLOAT1 )
			{
				fRate = ( cosf( D3DX_PI * fRate ) + FLOAT1 ) * FLOAT05;

				m_fCurGapDistance = m_fGapDistance * fRate;
				m_fCurGapHeight = m_fGapHeight * fRate;
				m_fCurGapFov = m_fGapFov * fRate;

				vNewPosition.z -= m_fCurGapDistance;
				vNewPosition.y += m_fCurGapHeight;
				fCamFov += m_fCurGapFov;
			}
			else
			{
				m_fCurGapDistance = 0.0f;
				m_fCurGapHeight = 0.0f;
				m_fCurGapFov = 0.0f;
				m_dwZoomStartTime = 0;
			}
		}
	}

	return vNewPosition;
}

D3DXVECTOR3 ioPlayStage::ChargingCamera( const D3DXVECTOR3 &vLookAt,
										 const D3DXVECTOR3 &vTarPos,
										 float &fCamFov,
										 ioBaseChar *pTarget )
{
	if( !pTarget )
		return vTarPos;

	int iCameraType = pTarget->CheckCameraType();
	if( iCameraType != CT_DEFAULT && iCameraType != CT_EXTEND )
	{
		m_dwChargingStartTime = 0;
		m_bCameraChanging = false;
		return vTarPos;
	}

	D3DXVECTOR3 vNewPosition = vTarPos;

	if( pTarget->CheckCameraType() == CT_EXTEND )
	{
		if( !m_bCameraChanging )
		{
			m_bCameraChanging = true;

			float fDistance = 0.0f, fHeight = 0.0f, fFov = 0.0f;
			DWORD dwZoomTime = 0;
			if( !pTarget->GetChargingCamera( fDistance, fHeight, fFov, dwZoomTime ) )
			{
				m_bCameraChanging = false;
				m_dwChargingStartTime = 0;
			}

			m_fMaxGapDistance = fDistance - ( vLookAt.z-vTarPos.z );
			m_fGapDistance = m_fMaxGapDistance - m_fCurGapDistance;
			
			m_fMaxGapHeight = fHeight - ( vTarPos.y-vLookAt.y );
			m_fGapHeight = m_fMaxGapHeight - m_fCurGapHeight;

			m_fMaxGapFov = fFov - fCamFov;
			m_fGapFov = m_fMaxGapFov - m_fCurGapFov;

			m_dwChargingStartTime = FRAMEGETTIME();
			if( m_fMaxGapDistance <= FLOAT1 && m_fCurGapDistance != 0.0f )
				m_dwZoomSlerpTime = dwZoomTime;
			else if( m_fMaxGapDistance <= FLOAT1 && m_fCurGapDistance == 0.0f )
				m_dwZoomSlerpTime = 0;
			else
				m_dwZoomSlerpTime = dwZoomTime * fabs(m_fGapDistance / m_fMaxGapDistance);
		}

		if( m_dwChargingStartTime > 0 && m_dwZoomSlerpTime > 0 )
		{
			DWORD dwGap = FRAMEGETTIME() - m_dwChargingStartTime;
			float fRate = (float)dwGap / (float)m_dwZoomSlerpTime;
			fRate = max( 0.0f, min( fRate, FLOAT1 ) );

			if( fRate < FLOAT1 )
			{
				fRate = ( cosf( D3DX_PI * fRate ) + FLOAT1 ) * FLOAT05;

				m_fCurGapDistance = m_fMaxGapDistance - m_fGapDistance;
				m_fCurGapDistance +=  m_fGapDistance * (1.0f - fRate);
				m_fCurGapHeight = m_fMaxGapHeight - m_fGapHeight;
				m_fCurGapHeight += m_fGapHeight * (1.0f - fRate);
				m_fCurGapFov = m_fMaxGapFov - m_fGapFov;
				m_fCurGapFov += m_fGapFov * (1.0f - fRate);

				vNewPosition.z -= m_fCurGapDistance;
				vNewPosition.y += m_fCurGapHeight;
				fCamFov += m_fCurGapFov;
			}
			else
			{
				vNewPosition.z -= m_fMaxGapDistance;
				vNewPosition.y += m_fMaxGapHeight;
				fCamFov += m_fMaxGapFov;
				m_fCurGapDistance = m_fMaxGapDistance;
				m_fCurGapHeight = m_fMaxGapHeight;
				m_fCurGapFov = m_fMaxGapFov;
				m_dwChargingStartTime = 0;
			}
		}
		else
		{
			vNewPosition.z -= m_fMaxGapDistance;
			vNewPosition.y += m_fMaxGapHeight;
			fCamFov += m_fMaxGapFov;
			m_fCurGapDistance = m_fMaxGapDistance;
			m_fCurGapHeight = m_fMaxGapHeight;
			m_fCurGapFov = m_fMaxGapFov;
		}
	}
	else
	{
		if( m_bCameraChanging )
		{
			m_bCameraChanging = false;
			m_bCameraSkilling = false;

			float fDistance = 0.0f, fHeight = 0.0f, fFov = 0.0f;
			DWORD dwZoomTime = 0;
			if( !pTarget->GetChargingCamera( fDistance, fHeight, fFov, dwZoomTime ) )
			{
				m_dwChargingStartTime = 0;
			}

			m_fMaxGapDistance = fDistance - ( vLookAt.z-vTarPos.z );
			m_fGapDistance = m_fCurGapDistance;
			m_fGapHeight = m_fCurGapHeight;
			m_fGapFov = m_fCurGapFov;

			m_dwChargingStartTime = FRAMEGETTIME();
			if( fabs(m_fMaxGapDistance) <= FLOAT1 && m_fCurGapDistance != 0.0f )
				m_dwZoomSlerpTime = dwZoomTime;
			else if( fabs(m_fMaxGapDistance) <= FLOAT1 && m_fCurGapDistance == 0.0f )
				m_dwZoomSlerpTime = 0;
			else
				m_dwZoomSlerpTime = dwZoomTime * fabs(m_fGapDistance / m_fMaxGapDistance);
		}
		
		if( m_dwChargingStartTime > 0 && m_dwZoomSlerpTime > 0 )
		{
			DWORD dwGap = FRAMEGETTIME() - m_dwChargingStartTime;
			float fRate = (float)dwGap / (float)m_dwZoomSlerpTime;
			fRate = max( 0.0f, min( fRate, FLOAT1 ) );

			if( fRate < FLOAT1 )
			{
				fRate = ( cosf( D3DX_PI * fRate ) + FLOAT1 ) * FLOAT05;

				m_fCurGapDistance = m_fGapDistance * fRate;
				m_fCurGapHeight = m_fGapHeight * fRate;
				m_fCurGapFov = m_fGapFov * fRate;

				vNewPosition.z -= m_fCurGapDistance;
				vNewPosition.y += m_fCurGapHeight;
				fCamFov += m_fCurGapFov;
			}
			else
			{
				m_fCurGapDistance = 0.0f;
				m_fCurGapHeight = 0.0f;
				m_fCurGapFov = 0.0f;
				m_dwChargingStartTime = 0;
			}
		}
	}

	return vNewPosition;
}

D3DXVECTOR3 ioPlayStage::BuffCamera( const D3DXVECTOR3 &vLookAt,
									 const D3DXVECTOR3 &vTarPos,
									 float &fCamFov,
									 ioBaseChar *pTarget )
{
	if( !pTarget )
		return vTarPos;

	int iCameraType = pTarget->CheckCameraType();
	if( iCameraType != CT_DEFAULT && iCameraType != CT_BUFF )
	{
		m_dwBuffCamStartTime = 0;
		m_bCameraBuffing = false;
		return vTarPos;
	}

	D3DXVECTOR3 vNewPosition = vTarPos;

	if( pTarget->CheckCameraType( m_bCameraBuffing ) == CT_BUFF )
	{
		if( !m_bCameraBuffing )
		{
			m_bCameraBuffing = true;

			float fDistance = 0.0f, fHeight = 0.0f, fFov = 0.0f;
			DWORD dwZoomTime = 0;
			pTarget->GetBuffCamera( fDistance, fHeight, fFov, dwZoomTime );

			m_fMaxGapDistance = fDistance - ( vLookAt.z-vTarPos.z );
			m_fGapDistance = m_fMaxGapDistance - m_fCurGapDistance;

			m_fMaxGapHeight = fHeight - ( vTarPos.y-vLookAt.y );
			m_fGapHeight = m_fMaxGapHeight - m_fCurGapHeight;

			m_fMaxGapFov = fFov - fCamFov;
			m_fGapFov = m_fMaxGapFov - m_fCurGapFov;

			m_dwBuffCamStartTime = FRAMEGETTIME();
			if( fabs(m_fMaxGapDistance) <= FLOAT1 && m_fCurGapDistance != 0.0f )
				m_dwZoomSlerpTime = dwZoomTime;
			else if( fabs(m_fMaxGapDistance) <= FLOAT1 && m_fCurGapDistance == 0.0f )
				m_dwZoomSlerpTime = 0;
			else
			{
				ioWeaponItem* pWeaponItem = pTarget->GetWeapon();
				if( pWeaponItem && ToSuddenAttackItem( pWeaponItem ) )
					m_dwZoomSlerpTime = dwZoomTime;
				else 
					m_dwZoomSlerpTime = dwZoomTime * fabs(m_fGapDistance / m_fMaxGapDistance);	
			}
		}

		if( m_dwBuffCamStartTime > 0 && m_dwZoomSlerpTime > 0 )
		{
			DWORD dwGap = FRAMEGETTIME() - m_dwBuffCamStartTime;
			float fRate = (float)dwGap / (float)m_dwZoomSlerpTime;
			fRate = max( 0.0f, min( fRate, FLOAT1 ) );

			if( fRate < FLOAT1 )
			{
				fRate = ( cosf( D3DX_PI * fRate ) + FLOAT1 ) * FLOAT05;

				m_fCurGapDistance = m_fMaxGapDistance - m_fGapDistance;
				m_fCurGapDistance +=  m_fGapDistance * (1.0f - fRate);
				m_fCurGapHeight = m_fMaxGapHeight - m_fGapHeight;
				m_fCurGapHeight += m_fGapHeight * (1.0f - fRate);
				m_fCurGapFov = m_fMaxGapFov - m_fGapFov;
				m_fCurGapFov += m_fGapFov * (1.0f - fRate);

				vNewPosition.z -= m_fCurGapDistance;
				vNewPosition.y += m_fCurGapHeight;
				fCamFov += m_fCurGapFov;
			}
			else
			{
				vNewPosition.z -= m_fMaxGapDistance;
				vNewPosition.y += m_fMaxGapHeight;
				fCamFov += m_fMaxGapFov;
				m_fCurGapDistance = m_fMaxGapDistance;
				m_fCurGapHeight = m_fMaxGapHeight;
				m_fCurGapFov = m_fMaxGapFov;
				m_dwBuffCamStartTime = 0;
			}
		}
		else
		{
			vNewPosition.z -= m_fMaxGapDistance;
			vNewPosition.y += m_fMaxGapHeight;
			fCamFov += m_fMaxGapFov;
			m_fCurGapDistance = m_fMaxGapDistance;
			m_fCurGapHeight = m_fMaxGapHeight;
			m_fCurGapFov = m_fMaxGapFov;
		}
	}
	else
	{
		if( m_bCameraBuffing )
		{
			m_bCameraBuffing = false;

			float fDistance = 0.0f, fHeight = 0.0f, fFov = 0.0f;
			DWORD dwZoomTime = 0;
			pTarget->GetBuffCamera( fDistance, fHeight, fFov, dwZoomTime );

			m_fMaxGapDistance = fDistance - ( vLookAt.z-vTarPos.z );
			m_fGapDistance = m_fCurGapDistance;
			m_fGapHeight = m_fCurGapHeight;
			m_fGapFov = m_fCurGapFov;

			m_dwBuffCamStartTime = FRAMEGETTIME();
			if( fabs(m_fMaxGapDistance) <= FLOAT1 && m_fCurGapDistance != 0.0f )
				m_dwZoomSlerpTime = dwZoomTime;
			else if( fabs(m_fMaxGapDistance) <= FLOAT1 && m_fCurGapDistance == 0.0f )
				m_dwZoomSlerpTime = 0;
			else
				m_dwZoomSlerpTime = dwZoomTime * FLOAT05 * fabs(m_fGapDistance / m_fMaxGapDistance);
		}

		if( m_dwBuffCamStartTime > 0 && m_dwZoomSlerpTime > 0 )
		{
			DWORD dwGap = FRAMEGETTIME() - m_dwBuffCamStartTime;
			float fRate = (float)dwGap / (float)m_dwZoomSlerpTime;
			fRate = max( 0.0f, min( fRate, FLOAT1 ) );

			if( fRate < FLOAT1 )
			{
				fRate = ( cosf( D3DX_PI * fRate ) + FLOAT1 ) * FLOAT05;

				m_fCurGapDistance = m_fGapDistance * fRate;
				m_fCurGapHeight = m_fGapHeight * fRate;
				m_fCurGapFov = m_fGapFov * fRate;

				vNewPosition.z -= m_fCurGapDistance;
				vNewPosition.y += m_fCurGapHeight;
				fCamFov += m_fCurGapFov;
			}
			else
			{
				m_fCurGapDistance = 0.0f;
				m_fCurGapHeight = 0.0f;
				m_fCurGapFov = 0.0f;
				m_dwBuffCamStartTime = 0;
			}
		}
	}

	return vNewPosition;
}

D3DXVECTOR3 ioPlayStage::SkillCamera( const D3DXVECTOR3 &vLookAt,
					 				  const D3DXVECTOR3 &vTarPos,
									  float &fCamFov,
									  ioBaseChar *pTarget )
{
	if( !pTarget )
		return vTarPos;

	int iCameraType = pTarget->CheckCameraType();
	if( iCameraType != CT_DEFAULT && iCameraType != CT_SKILL )
		return vTarPos;

	D3DXVECTOR3 vNewPosition = vTarPos;

	if( pTarget->CheckCameraType() == CT_SKILL )
	{
		DWORD dwZoomTime = 0;
		float fDistance = 0.0f, fHeight = 0.0f, fFov = 0.0f;
		if( pTarget->GetSkillCamera( fDistance, fHeight, fFov, dwZoomTime ) )
		{
			vNewPosition = vLookAt;
			vNewPosition.y += fHeight;
			vNewPosition.z -= fDistance;
			fCamFov = fFov;

			m_fCurGapDistance = fDistance - ( vLookAt.z-vTarPos.z );
			m_fCurGapHeight = fHeight - ( vTarPos.y-vLookAt.y );
			m_fCurGapFov = fFov - fCamFov;
		}
		else
		{
			m_fCurGapDistance = 0.0f;
			m_fCurGapHeight = 0.0f;
			m_fCurGapFov = 0.0f;
		}

		m_bCameraSkilling = true;
	}
	else
	{
		// 스킬사용후 딱 한 프레임동안 default가 들어오는데 이때 깜빡임이 발생함
		if( m_bCameraSkilling )
		{
			m_bCameraSkilling = false;

			DWORD dwZoomTime = 0;
			float fDistance = 0.0f, fHeight = 0.0f, fFov = 0.0f;
			if( pTarget->GetSkillCamera( fDistance, fHeight, fFov, dwZoomTime ) )
			{
				vNewPosition = vLookAt;
				vNewPosition.y += fHeight;
				vNewPosition.z -= fDistance;
				fCamFov = fFov;

				m_fCurGapDistance = fDistance - ( vLookAt.z-vTarPos.z );
				m_fCurGapHeight = fHeight - ( vTarPos.y-vLookAt.y );
				m_fCurGapFov = fFov - fCamFov;
			}
			else
			{
				m_fCurGapDistance = 0.0f;
				m_fCurGapHeight = 0.0f;
				m_fCurGapFov = 0.0f;
			}
		}
	}

	return vNewPosition;
}

D3DXVECTOR3 ioPlayStage::SpringDamp( float fSpringConst,
									 float fDampConst,
									 const D3DXVECTOR3 &vCurPos,
									 const D3DXVECTOR3 &vTarPos,
									 const D3DXVECTOR3 &vPreTarPos,
									 float fEpsilon )
{
	D3DXVECTOR3 vDiff; // Displacement
	D3DXVECTOR3 vVelocity; // Velocity

	float fDeltaTime = g_FrameTimer.GetSecPerFrame();

	// Calculate Spring Force
	vDiff = vCurPos - vTarPos;
	float fDistance = D3DXVec3Length( &vDiff );
	if( fDistance < fEpsilon )
		return vTarPos;

	vVelocity = (vPreTarPos - vTarPos) * fDeltaTime;

	float fForceMag; // Force Magnitude
	fForceMag = fSpringConst * ( -fDistance ) + 
				fDampConst * ( D3DXVec3Dot( &vDiff, &vVelocity ) / fDistance );

	// Apply Spring Force
	D3DXVec3Normalize( &vDiff, &vDiff );
	vDiff *= fForceMag * fDeltaTime;

	return ( vCurPos + vDiff );
}

D3DXVECTOR3 ioPlayStage::GetCameraShakeAmt()
{
	D3DXVECTOR3 vShake( 0.0f, 0.0f, 0.0f );

	if( m_dwCameraShakeStartTime == 0 )
		return vShake;

	DWORD dwCurTime = FRAMEGETTIME();
	if( COMPARE( fabs(m_fCameraShakeOffsetX), 0.1f, m_fCameraShakeEndAmount ) )
	{
		m_iCameraShakePriority = 0;
		m_dwCameraShakeStartTime = 0;
	}
	else if( COMPARE( fabs(m_fCameraShakeOffsetY), 0.1f, m_fCameraShakeEndAmount ) )
	{
		m_iCameraShakePriority = 0;
		m_dwCameraShakeStartTime = 0;
	}
	else
	{
		DWORD dwDiff = dwCurTime - m_dwCameraShakeCheckTime;

		if( dwDiff >= m_dwCameraShakeGapTime )
		{
			m_dwCameraShakeCheckTime = dwCurTime - ( dwDiff - m_dwCameraShakeGapTime );

			if( m_fCameraShakeOffsetX != 0.0f )
			{
				m_fCameraShakeOffsetX = -(m_fCameraShakeOffsetX * ( FLOAT1 - m_fCameraShakeMaxRate ));
			}

			if( m_fCameraShakeOffsetY != 0.0f )
			{
				m_fCameraShakeOffsetY = -(m_fCameraShakeOffsetY * ( FLOAT1 - m_fCameraShakeMaxRate ));
			}
		}

		vShake.x = m_fCameraShakeOffsetX;
		vShake.y = m_fCameraShakeOffsetY;
	}

	return vShake;
}

float ioPlayStage::GetCameraRollAmt()
{
	float fRollAngle = 0.0f;

	if( m_dwCameraRollStartTime == 0 )
		return fRollAngle;

	DWORD dwCurTime = FRAMEGETTIME();
	if( fabs(m_fCameraRollValue) < m_fCameraRollEndAmount )
	{
		if( m_dwCameraRollEndSlerpTime == 0 )
		{
			m_dwCameraRollEndSlerpTime = dwCurTime + m_dwCameraRollGapTime + m_dwCameraRollEndSlerp;
		}

		if( m_dwCameraRollEndSlerpTime <= dwCurTime )
		{
			m_dwCameraRollStartTime = 0;
		}
		else
		{
			if( dwCurTime > (m_dwCameraRollEndSlerpTime - m_dwCameraRollEndSlerp) )
			{
				DWORD dwGap = m_dwCameraRollEndSlerpTime - dwCurTime;
				fRollAngle = m_fCameraRollValue * (float)dwGap/(float)m_dwCameraRollEndSlerp;
			}
			else
			{
				fRollAngle = m_fCameraRollValue;
			}
		}
	}
	else
	{
		DWORD dwDiff = dwCurTime - m_dwCameraRollCheckTime;

		if( dwDiff >= m_dwCameraRollGapTime )
		{
			m_dwCameraRollCheckTime = dwCurTime - ( dwDiff - m_dwCameraRollGapTime );

			if( m_fCameraRollValue != 0.0f )
			{
				m_fCameraRollValue = -(m_fCameraRollValue * ( FLOAT1 - m_fCameraRollMaxRate ));
			}
		}

		fRollAngle = m_fCameraRollValue;
	}

	return fRollAngle;
}

void ioPlayStage::StartCameraPause( DWORD dwPauseTime )
{
	m_dwCameraPauseTime = dwPauseTime;
	m_dwCurCameraPauseTime = FRAMEGETTIME();
}	

void ioPlayStage::EndCameraPause()
{
	m_dwCameraPauseTime = m_dwCurCameraPauseTime = 0;
}

bool ioPlayStage::IsCameraPause()
{
	if( m_dwCurCameraPauseTime == 0 ) return false;    // 정지 하지 않았다.
	if( m_dwCameraPauseTime == 0 ) return true;        // 무한 정지 상태.
	if( FRAMEGETTIME() - m_dwCurCameraPauseTime < m_dwCameraPauseTime ) return true;   // 정지중

	// 정지 시간 초과
	EndCameraPause();
	return false;
}

void ioPlayStage::CallBackCameraEventEnd( DWORD dwEndCode )
{
	// 맵 카메라 이벤트 후 바로 시작된 몬스터 카메라 이벤트 종료 처리
	if( m_dwEventSceneEndNextCameraEvent == dwEndCode )
	{
		ioMonsterSurvivalMode *pMonsterSurvival = ToMonsterSurvivalMode( m_pPlayMode );
		if( pMonsterSurvival )
		{
			pMonsterSurvival->StartTurnViewCharStart();

			// 옵저버 유저 처리 - 게임 턴 넘어가는 카메라 이벤트때 입장한 옵저버!!
			if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() )  || IsStealthMode() )
			{
				if( !m_bFirstEventSceneEndSendCheck )      // 입장시 패킷을 보내지 않았으면 보낸다.
				{
					m_bFirstEventSceneEndSendCheck = true;

					SP2Packet kPacket( CTPK_EVENT_SCENE_END );
					kPacket << true << GetRoomIndex();
					TCPNetwork::SendToServer( kPacket );

					g_ObserverWndMgr.ObserverWndShow();
				}
			}
		}
		m_szEventSceneEndNextTarget.Clear();
		m_dwEventSceneEndNextCameraEvent = 0;		
	}
}

void ioPlayStage::StartTurnStartEventCamera( DWORD dwEventSceneID, DWORD dwNextCameraEvent, const ioHashString &rkNextTarget )
{
	m_dwEventSceneEndNextCameraEvent = dwNextCameraEvent;
	m_szEventSceneEndNextTarget      = rkNextTarget;
	StartModeStartEventCamera( false, false, dwEventSceneID );
}

void ioPlayStage::StartModeStartEventCamera( bool bFirst, bool bSceneEndSendPacket /* = true */, DWORD dwEventSceneID /* = 0  */ )
{
	m_bEventSceneCamera = true;
	m_bFirstEventSceneCamera = bFirst;
	m_bEventSceneEndSendPacket = bSceneEndSendPacket;

	if( !P2PNetwork::IsNetworkPlaying() || !IsNetworkMode() )
		m_bEventSceneEndSendPacket = false;

	g_CameraEventMgr.NotifyCEDestroyed();

	if( m_pPlayMode )
	{
		m_pPlayMode->StartEventScene( bFirst, dwEventSceneID );
		UpdateEventSceneCamera( g_App.GetCamera() );
	}
}

void ioPlayStage::UpdateEventSceneCamera( ioCamera *pCamera )
{
	if( !m_pPlayMode ) return;

	DWORD dwCurTime = REALGETTIME();
	D3DXVECTOR3 vNewOffset = m_pPlayMode->UpdateEventSceneOffset( dwCurTime );
	D3DXVECTOR3 vNewLookAt;
	vNewLookAt = CheckMapCameraLimit( m_pPlayMode->UpdateEventScenePos( dwCurTime ) );
	vNewLookAt.y += m_fCamLookAtHeightOffset;

	D3DXVECTOR3 vNewCamPos;
	vNewCamPos.x = vNewLookAt.x + vNewOffset.x;
	vNewCamPos.y = ( vNewLookAt.y + m_fDefCamHeight ) + vNewOffset.y;
	vNewCamPos.z = ( vNewLookAt.z - m_fDefCamDistance )  + vNewOffset.z;

	ioLookAtCameraController *pCtrl = ToLookAtCtrl( pCamera->GetController() );
	if( pCtrl )
	{
		m_vCurLookAt = vNewLookAt;
		pCtrl->SetLookAt( vNewLookAt );
		pCtrl->SetPosition( vNewCamPos );
		pCamera->SetFov( m_fDefCamFov );
	}

	if( m_pPlayMode->IsEventSceneOver() )
	{
		EndModeStartEventCamera();
	}
}

void ioPlayStage::SetModeEndSceneCamera( bool bEndScene )
{
	if( !m_pPlayMode ) return;

	m_bEndSceneCamera  = bEndScene;
	m_dwViewMovingTime = 0;
	if( m_bEndSceneCamera )
	{
		g_CameraEventMgr.NotifyCEDestroyed();

		m_pPlayMode->CheckEndSceneLookAt();
		m_dwViewMovingTime = FRAMEGETTIME();

		ioCamera *pCam = g_App.GetCamera();
		if( pCam )
		{
			ioLookAtCameraController *pCtrl = ToLookAtCtrl( pCam->GetController() );
			if( pCtrl )
			{
				m_vLerpStart = pCtrl->GetLookAt();
				m_vLerpStart.y -= m_fCamLookAtHeightOffset;
			}
		}
		g_CameraEventMgr.CreateCE( m_pPlayMode->GetEndSceneCameraEvent(), this );		
	}
}

void ioPlayStage::UpdateEndSceneCamera( ioCamera *pCamera )
{
	if( !m_pPlayMode ) return;

	D3DXVECTOR3 vNewLookAt;
	D3DXVECTOR3 vPos = m_pPlayMode->GetEndSceneLookAt();
	
	// 타겟 캐릭터가 존재하면...
	if( !m_pPlayMode->GetEndSceneLookCharName().IsEmpty() )
	{
		ioBaseChar *pTarget = GetBaseChar( m_pPlayMode->GetEndSceneLookCharName() );
		if( pTarget )
		{
			if( pTarget->GetState() != CS_DIE &&
				pTarget->GetState() != CS_VIEW &&
				pTarget->GetState() != CS_LOADING &&
				pTarget->GetState() != CS_OBSERVER )
			{
                vPos = pTarget->GetWorldPosition();
			}
		}
	}

	vPos = CheckMapCameraLimit( vPos );
	vPos.y += m_fCamLookAtHeightOffset;

	float fEndSceneCamHeight, fEndSceneCamDistance, fEndSceneCamFov;	
	m_pPlayMode->GetEndSceneCameraPos( fEndSceneCamHeight, fEndSceneCamDistance, fEndSceneCamFov );	
	if( m_dwViewMovingTime > 0 && m_pPlayMode->GetEndSceneCameraMoveTime() > 0 )
	{
		DWORD dwGap = FRAMEGETTIME() - m_dwViewMovingTime;
		float fRate = (float)dwGap / (float)m_pPlayMode->GetEndSceneCameraMoveTime();
		fRate = max( 0.0f, min( fRate, FLOAT1 ) );
		if( fRate < FLOAT1 )
		{
			fRate = ( cosf( D3DX_PI * fRate ) + FLOAT1 ) * FLOAT05;
			fRate = FLOAT1 - fRate;

			D3DXVec3Lerp( &vNewLookAt, &m_vLerpStart, &vPos, fRate );
		}
		else
		{
			vNewLookAt = vPos;
			m_dwViewMovingTime = 0;
		}
	}
	else
	{
		vNewLookAt = vPos;
	}

	ioCameraEvent *pCameraEvent = g_CameraEventMgr.GetCurCameraEvent();
	if( pCameraEvent )
	{
		if( pCameraEvent->CheckCameraEventEnd() )
		{
			g_CameraEventMgr.NotifyCEDestroyed();
			pCameraEvent = NULL;
		}
		else
		{
			vNewLookAt = pCameraEvent->CheckOffSet( vNewLookAt, D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 ) );
		}
	}

	ioLookAtCameraController *pCtrl = ToLookAtCtrl( pCamera->GetController() );
	if( !pCtrl )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::UpdateEndSceneCamera - Not LookAt Controller" );
		return;
	}

	vNewLookAt = CheckMapCameraLimit( vNewLookAt );

	D3DXVECTOR3 vCamLookAt;
	vCamLookAt.x = vNewLookAt.x;
	vCamLookAt.y = vNewLookAt.y + m_fCamLookAtHeightOffset;
	vCamLookAt.z = vNewLookAt.z;

	m_vCurLookAt = vCamLookAt;
	if( !m_bNoneCameraWorking && !m_bArcadeCameraWorking )
		vCamLookAt += GetCameraShakeAmt();

	pCtrl->SetLookAt( vCamLookAt );

	if( ( m_fCurCamHeight != fEndSceneCamHeight ) ||
		( m_fCurCamDistance != fEndSceneCamDistance ) ||
		( m_fCurCamFov != fEndSceneCamFov ) )
	{
		m_dwViewMovingTimeItem = FRAMEGETTIME();

		m_fStartHeight = m_fCurCamHeight;
		m_fStartDistance = m_fCurCamDistance;
		m_fStartFov = m_fCurCamFov;
		m_fCurCamHeight = fEndSceneCamHeight;
		m_fCurCamDistance = fEndSceneCamDistance;
		m_fCurCamFov = fEndSceneCamFov;
	}

	float fCamDistance, fCamHeight, fCamFov;
	m_dwCamZoomTimeItem = m_pPlayMode->GetEndSceneZoomTime();
	if( m_dwViewMovingTimeItem > 0 && m_dwCamZoomTimeItem > 0 )
	{
		DWORD dwGap = FRAMEGETTIME() - m_dwViewMovingTimeItem;
		float fRate = (float)dwGap / (float)m_dwCamZoomTimeItem;
		fRate = max( 0.0f, min( fRate, FLOAT1 ) );

		if( fRate < FLOAT1 )
		{
			fRate = ( cosf( D3DX_PI * fRate ) + FLOAT1 ) * FLOAT05;

			float fInvRate = FLOAT1 - fRate;
			fCamHeight = m_fStartHeight * fRate + m_fCurCamHeight * fInvRate;
			fCamDistance = m_fStartDistance * fRate + m_fCurCamDistance * fInvRate;
			fCamFov = m_fStartFov * fRate + m_fCurCamFov * fInvRate;
		}
		else
		{
			fCamHeight = m_fCurCamHeight;
			fCamDistance = m_fCurCamDistance;
			fCamFov = m_fCurCamFov;

			m_dwViewMovingTimeItem = 0;
		}
	}
	else
	{
		fCamHeight = m_fCurCamHeight;
		fCamDistance = m_fCurCamDistance;
		fCamFov = m_fCurCamFov;
	}

	if( m_bNoneCameraWorking || m_bSetCameraToNowOwner || m_bArcadeCameraWorking )
	{
		fCamHeight = m_fStartHeight;
		fCamDistance = m_fStartDistance;
		fCamFov = m_fStartFov;
	}

	D3DXVECTOR3 vNewCamPos = vCamLookAt;
	vNewCamPos.y += fCamHeight;
	vNewCamPos.z -= fCamDistance;

	pCamera->SetFov( fCamFov );
	pCamera->SetCameraRoll( 0.0f );
	
	if( m_bNoneCameraWorking || m_bSetCameraToNowOwner || m_bArcadeCameraWorking )
	{
		pCamera->SetPosition( vNewCamPos );
		return;
	}

	if( pCameraEvent && !IsNowMouseCamera() )
	{
		m_dwZoomStartTime = 0;
		m_dwChargingStartTime = 0;

		m_bCameraZooming = true;
		m_bCameraChanging = false;

		m_fCurGapDistance = 0.0f;
		m_fCurGapHeight = 0.0f;
		m_fCurGapFov = 0.0f;

		if( pCameraEvent->CheckCameraEventEnd() )
		{
			g_CameraEventMgr.NotifyCEDestroyed();
			pCameraEvent = NULL;
		}
		else
		{
			vNewCamPos = pCameraEvent->ProcessCameraEvent( pCtrl->GetLookAt(), vNewCamPos, fCamFov, D3DXQUATERNION( 0.0f, 0.0f, 0.0f, FLOAT1 ) );
			pCamera->SetFov( pCameraEvent->GetCameraFov() );
			pCamera->SetCameraRoll( pCameraEvent->GetCameraRoll() );
		}
	}
	else
	{
		vNewCamPos.y += fCamHeight;
		vNewCamPos.z -= fCamDistance;

		pCamera->SetFov( fCamFov );
		pCamera->SetCameraRoll( GetCameraRollAmt() );
	}
	pCamera->SetPosition( vNewCamPos );
}

void ioPlayStage::EndModeStartEventCamera()
{
	if( !m_bEventSceneCamera )	return;

	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner ) return;

	if( m_ViewChar != pOwner->GetCharName() )
	{
		int iCharCnt = m_pPlayMode->GetRecordCharCnt();
		ChangeCurViewChar( pOwner->GetCharName(), iCharCnt );
	}

	if( m_bEventSceneEndSendPacket )
	{
		m_bFirstEventSceneEndSendCheck = true;

		SP2Packet kPacket( CTPK_EVENT_SCENE_END );
		kPacket << m_bFirstEventSceneCamera;
		kPacket << GetRoomIndex();
		TCPNetwork::SendToServer( kPacket );
		
		g_ObserverWndMgr.ObserverWndShow();

		if( ioPlayMode::GetModeType() == MT_MONSTER_SURVIVAL )
		{
			if( !g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) && !IsStealthMode() )
			{
				StartCameraPause();
			}
		}
		if( ioPlayMode::GetModeType() == MT_PRACTICE )
		{
			ioPracticeMode *pPracticeMode = ToPracticeMode( m_pPlayMode );
			if( pPracticeMode )
				pPracticeMode->ReadyPractice();
		}

	}

    if( m_dwEventSceneEndNextCameraEvent == 0 )
	{
		if( !g_GUIMgr.IsShow( HERO_MATCH_VERSUS_SCENE_WND ) )
			m_pPlayMode->SetBlindMode( false );		
	}
	else
	{
		// 카메라 이벤트 시전
		ioMonsterSurvivalMode *pMonsterSurvivalMode = ToMonsterSurvivalMode( m_pPlayMode );
		if( pMonsterSurvivalMode )
		{
			pMonsterSurvivalMode->MonsterEventMent( m_szEventSceneEndNextTarget );
		}
		ChangeCurViewChar( m_szEventSceneEndNextTarget, m_pPlayMode->GetRecordCharCnt() );
		g_CameraEventMgr.NotifyCEDestroyed();
		g_CameraEventMgr.CreateCE( m_dwEventSceneEndNextCameraEvent, this );		
	}
	m_bEventSceneCamera = false;
	m_bSpecialObserver = true;
	if( !g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
		m_bStartChangeObserver = true;
	g_ObserverWndMgr.ObserverInfoShow( m_bSpecialObserver );
}

void ioPlayStage::SetCameraToOwnerExplicit( ioBaseChar *pOwner )
{
	m_bSetCameraToNowOwner = true;
}

void ioPlayStage::SetTargetLookAt( bool bTarget )
{
	if( IsNowMouseCamera() )
	{
		m_bTargetLookAt = false;
		return;
	}

	m_bTargetLookAt = bTarget;
}

void ioPlayStage::HidingAllFieldItem()
{
	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldItem *pField = ToFieldItem( *iter );
		if( pField )
		{
			pField->ReserveHiding();
		}
	}
}

void ioPlayStage::HidingFieldItem( int iItemCreateIndex )
{
	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldItem *pField = ToFieldItem( *iter );
		if( pField && pField->GetItemCreateIndex() == iItemCreateIndex )
		{
			pField->ReserveHiding();
			break;
		}
	}
}

void ioPlayStage::ClearItemOwnerByOutUser( const ioHashString &rkName )
{
	GameEntityList::iterator iter = m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldItem *pField = ToFieldItem( *iter );
		if( pField )
		{
			pField->ClearItemOwnerByOutUser( rkName );
		}
		else
		{
			ioBaseChar *pChar = ToBaseChar( *iter );
			if( pChar )
			{
				pChar->ClearItemOwnerByOutUser( rkName );
			}
		}
	}
}

void ioPlayStage::ReleaseFieldItemByIndex( int itemIndex )
{
	GameEntityList::iterator iter = m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldItem *pField = ToFieldItem( *iter );
		if( pField )
		{
			pField->ReleaseFieldItemByIndex( itemIndex );
		}
	}

	DestroyUnAvailableFieldItem();
}

void ioPlayStage::UpdateGameEntityList()
{
	ErrorReport::SetPosition( 1100, 70 );

	DWORD dwCurTime = FRAMEGETTIME();
	float fTimePerSec = g_FrameTimer.GetSecPerFrame();

	bool bCheckChar = Help::CheckEnableTimeGapUser();

	DWORD dwStartTime = 0;
	DWORD dwGapTime = 0;

	GameEntityList::iterator iter = m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		if( bCheckChar )
			dwStartTime = timeGetTime();

		if( NULL == ToNpcChar( *iter ) )   //NPC는 마지막에함
			(*iter)->Update( fTimePerSec, m_dwPreUpdatedTime, dwCurTime );

		if( bCheckChar )
		{
			dwGapTime = timeGetTime() - dwStartTime;

			if( dwGapTime > 10 )
			{
				if( ToBaseChar( *iter ) )
					LOG.PrintTimeAndLog( 0, "FC - UpdateGameEntityList: BaseChar: %d", dwGapTime );
				else if( ToPushStruct( *iter ) )
					LOG.PrintTimeAndLog( 0, "FC - UpdateGameEntityList: PushStruct: %d, %d", dwGapTime, ToPushStruct(*iter)->GetStructNum() );
				else if( ToRoulette( *iter ) )
					LOG.PrintTimeAndLog( 0, "FC - UpdateGameEntityList: Roulette: %d", dwGapTime );
			}
		}
	}
	ErrorReport::SetPosition( 1100, 71 );
	for(iter = m_GameEntityList.begin(); iter!=m_GameEntityList.end() ; ++iter )
	{
		if( bCheckChar )
			dwStartTime = timeGetTime();

		if( ToNpcChar( *iter ) )   //NPC
			(*iter)->Update( fTimePerSec, m_dwPreUpdatedTime, dwCurTime );

		if( bCheckChar )
		{
			dwGapTime = timeGetTime() - dwStartTime;

			if( dwGapTime > 10 )
			{
				LOG.PrintTimeAndLog( 0, "FC - UpdateGameEntityList: NPC: %d", dwGapTime );
			}
		}
	}
	ErrorReport::SetPosition( 1100, 72 );

	AddReserveEntityList();
	ErrorReport::SetPosition( 1100, 73 );

	if( bCheckChar )
		dwStartTime = timeGetTime();

	g_WeaponMgr.ProcessWeaponList( fTimePerSec );
	g_AreaWeaponMgr.ProcessCallAreaWeaponInfo();

	if( bCheckChar )
	{
		dwGapTime = timeGetTime() - dwStartTime;

		if( dwGapTime > 10 )
		{
			LOG.PrintTimeAndLog( 0, "FC - UpdateGameEntityList: Weapon: %d", dwGapTime );
		}
	}

	ErrorReport::SetPosition( 1100, 74 );

	DestroyUnAvailableFieldItem();
	DestroyUnAvailableFieldItemByTime();
	ErrorReport::SetPosition( 1100, 75 );

	DestroyUnAvailableFieldRewardItem();
	ErrorReport::SetPosition( 1100, 76 );

	DestroyRouletteEntity();

	m_dwPreUpdatedTime = dwCurTime;
}

void ioPlayStage::AddReserveEntityList()
{
	if( m_ReserveAddEntityList.empty() )
		return;

	ReserveAddEntityList::iterator iter = m_ReserveAddEntityList.begin();
	for( ; iter!=m_ReserveAddEntityList.end() ; ++iter )
	{
		AddGameEntity( *iter );
	}

	m_ReserveAddEntityList.clear();
}

void ioPlayStage::UpdateAfterGameEntityList()
{
	float fFrameGap = g_FrameTimer.GetCurFrameGap();

	GameEntityList::iterator iter = m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		(*iter)->UpdateAfter( fFrameGap );
	}
}

void ioPlayStage::CheckShadowCastObjects( ioCamera *pCamera )
{
	if( !m_pSceneMgr )	return;
	if( m_pSceneMgr->GetShadowMapType() == SMT_NOT_ENABLE )
		return;

	ioLookAtCameraController *pCtrl = ToLookAtCtrl( pCamera->GetController() );
	if( !pCtrl )	return;

	D3DXVECTOR3 vCamLookAt = pCtrl->GetLookAt();
	float fShadowRangeSq = m_fShadowEnableRange * m_fShadowEnableRange;

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioGameEntity *pEntity = *iter;

		if( !pEntity->IsShadowCastEnable() )
			continue;

		D3DXVECTOR3 vDiff = vCamLookAt - pEntity->GetWorldPosition();
		if( D3DXVec3LengthSq( &vDiff ) > fShadowRangeSq )
			pEntity->ApplyShadowCast( false );
		else
			pEntity->ApplyShadowCast( true );
	}
}

void ioPlayStage::DestroyUnAvailableFieldItem()
{
	GameEntityList::iterator iter = m_GameEntityList.begin();
	while( iter != m_GameEntityList.end() )
	{
		ioFieldItem *pField = ToFieldItem( *iter );
		if( !pField )
		{
			++iter;
			continue;
		}

		if( pField->IsNeedDestroy() )
		{
			iter = EraseGameEntity( iter );
		}
		else
		{
			if( !P2PNetwork::IsNetworkPlaying() || !IsNetworkMode() )
			{
				D3DXVECTOR3 vItemPos = pField->GetWorldPosition();
				float fMinHeight = pField->GetWorldCollisionBox().GetExtents(1);
				if( vItemPos.y <= fMinHeight + FLOAT1 )
				{
					if( pField->HasOwnerName() )
					{
						D3DXVECTOR3 vPos = GetItemRandomPosition();
						vPos.y += fMinHeight;
						pField->SetWorldPosition( vPos );
						pField->SetAlphaRateDirect( 0 );
						pField->SetTargetAlphaRate( MAX_ALPHA_RATE );
					}
					else
					{
						pField->ReserveHiding();
					}
				}
			}

			++iter;
		}
	}
}

void ioPlayStage::DestroyUnAvailableFieldRewardItem()
{
	GameEntityList::iterator iter = m_GameEntityList.begin();
	while( iter != m_GameEntityList.end() )
	{
		ioFieldRewardItem *pField = ToFieldRewardItem( *iter );
		if( !pField )
		{
			++iter;
			continue;
		}

		if( pField->IsNeedDestroy() )
		{
			iter = EraseGameEntity( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioPlayStage::CheckOwnerCanPickItems()
{
	ioBaseChar *pOwnerChar = GetOwnerChar();
	if( !pOwnerChar )	return;

	bool bOwnerFieldItemOver = false;
	bool bOwnerCanPick = pOwnerChar->IsItemCanPickUp();

	ioFieldItem *pPickItem = FindUserPickFieldItem( pOwnerChar );
	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldItem *pItem = ToFieldItem( *iter );
		if( !pItem )	continue;
		if( pItem->GetItemTeamType() != TEAM_NONE && pOwnerChar->GetTeam() != pItem->GetItemTeamType() ) continue;

		if( bOwnerCanPick && pItem->IsCanPicked() && pPickItem == pItem )
		{
			pItem->SetOwnerCharOver( true );
			pOwnerChar->SetPickEmoticon();
			bOwnerFieldItemOver = true;
		}
		else if( pItem->IsCanPicked() )
		{
			pItem->SetOwnerCharOver( false );
		}
	}

	// 장착 아이템과 겹쳐있을 수 있으므로 필드 아이템 우선 체크
	if( !bOwnerFieldItemOver )
	{
		ioFieldRewardItem *pPickRewardItem = FindUserPickFieldRewardItem( pOwnerChar );
		iter = m_GameEntityList.begin();
		for(;iter != m_GameEntityList.end();++iter)
		{
			ioFieldRewardItem *pItem = ToFieldRewardItem( *iter );
			if( !pItem ) continue;

			if( bOwnerCanPick && pItem->IsCanPicked() && pPickRewardItem == pItem && 
				(pItem->GetOwnerCharName().IsEmpty() || pItem->GetOwnerCharName() == pOwnerChar->GetCharName()) )
			{
				pItem->SetOwnerCharOver( true );
				pOwnerChar->SetPickEmoticon();
			}
			else if( pItem->IsCanPicked() )
			{
				pItem->SetOwnerCharOver( false );
			}
		}
	}
}

void ioPlayStage::CheckCharMouseClick( ioCamera *pCamera )
{
	ioLSMouse *pMouse = g_App.GetMouse();
	if( !pMouse )	return;

	ioBaseChar *pPickChar = NULL;
	ioPushStruct *pPickPush = NULL;

	// Char
	if( pMouse->IsMouseShowing() && g_GUIMgr.IsDeskTopOver() )
	{
		RECT rcCollision;
		float fMinDistSq = std::numeric_limits<float>::infinity();

		BaseCharList::iterator iter;
		for( iter=m_BaseCharList.begin() ; iter!=m_BaseCharList.end() ; ++iter )
		{
			ioBaseChar *pChar = *iter;

			if( ToNpcChar( pChar ) )
			{
				ioNpcChar *pNpcChar = ToNpcChar( pChar );
				if( ToHeadquartersMode( GetPlayMode() ) == NULL )
				{	// 본부가 아니면 NPC 체크 안하마
					continue;	
				}
			}

			if( !pChar->GetGroup()->IsVisible() )
				continue;

			if( !pChar->GetRenderGaugeRect( &rcCollision ) )
				continue;

			if( PtInRect( &rcCollision, pMouse->GetMousePos() ) )
			{
				D3DXVECTOR3 vDiff = pChar->GetWorldPosition() - pCamera->GetPosition();
				float fCurLengthSq = D3DXVec3LengthSq( &vDiff );
				if( fCurLengthSq < fMinDistSq )
				{
					fMinDistSq = fCurLengthSq;
					pPickChar = pChar;
				}
			}
		}
	}

	// Push
	if( pMouse->IsMouseShowing() && g_GUIMgr.IsDeskTopOver() && GetModeType() == MT_MYROOM )
	{
		RECT rcCollision;
		float fMinDistSq = std::numeric_limits<float>::infinity();

		GameEntityList::iterator iter=m_GameEntityList.begin();
		for( ; iter!=m_GameEntityList.end() ; ++iter )
		{
			ioPushStruct *pPush = ToPushStruct( *iter );
			if( !pPush ) continue;
			if( !pPush->GetGroup()->IsVisible() ) continue;

			if( !pPush->IsEnableClick() )
				continue;

			if( !pPush->GetCollisionRect( &rcCollision ) )
				continue;

			if( PtInRect( &rcCollision, pMouse->GetMousePos() ) )
			{
				D3DXVECTOR3 vDiff = pPush->GetWorldPosition() - pCamera->GetPosition();
				float fCurLengthSq = D3DXVec3LengthSq( &vDiff );
				if( fCurLengthSq < fMinDistSq )
				{
					fMinDistSq = fCurLengthSq;
					pPickPush = pPush;
				}
			}
		}
	}

	// Char or Push
	if( pPickChar && pPickPush )
	{
		D3DXVECTOR3 vDiff1 = pPickChar->GetWorldPosition() - pCamera->GetPosition();
		D3DXVECTOR3 vDiff2 = pPickPush->GetWorldPosition() - pCamera->GetPosition();

		if( D3DXVec3LengthSq(&vDiff1) <= D3DXVec3LengthSq(&vDiff2) )
			pPickPush = NULL;
		else
			pPickChar = NULL;
	}

	// Char OutLine Check
	BaseCharList::iterator iter;
	for( iter=m_BaseCharList.begin() ; iter!=m_BaseCharList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = *iter;

		if( pPickChar != pCurChar )
		{
			if( pCurChar->HasOutLineType( COT_MOUSE_OVER ) )
			{
				pCurChar->RestoreOutLineChange( COT_MOUSE_OVER );
			}
		}
	}

	// Push OutLine Check
	GameEntityList::iterator iter_p = m_GameEntityList.begin();
	for( ; iter_p != m_GameEntityList.end() ; ++iter_p )
	{
		ioPushStruct *pPush = ToPushStruct( *iter_p );
		if( !pPush ) continue;

		if( !pPickPush || pPickPush != pPush )
		{
			if( pPush->HasOutLineType( COT_MOUSE_OVER ) )
			{
				pPush->RestoreOutLineChange( COT_MOUSE_OVER );
			}
		}
	}

	if( !pPickChar && !pPickPush )	return;

	if( pPickChar && !pPickChar->HasOutLineType( COT_MOUSE_OVER ) )
		pPickChar->ApplyOutLineChange( COT_MOUSE_OVER, m_CharOverColor, m_fCharOverThickness );
	else if( pPickPush && !pPickPush->HasOutLineType( COT_MOUSE_OVER ) && m_fPushOverThickness > 0.0f )
		pPickPush->ApplyOutLineChange( COT_MOUSE_OVER, m_PushOverColor, m_fPushOverThickness );


	if( pMouse->IsLBtnPress() )
	{
		if( pPickChar )
		{
			if( GetModeType() == MT_HEADQUARTERS )
			{
				if( pPickChar->GetIndex() != NPC_INDEX )
				{
					if( ToNpcChar( pPickChar ) )
					{
						ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( GetPlayMode() );
						if( pHeadquartersMode )
						{
							g_App.OpenMannerWnd( pHeadquartersMode->GetMasterName().c_str(), true );
						}
					}
					else
					{					
						g_App.OpenMannerWnd( pPickChar->GetCharName().c_str(), true );
					}
				}
			}
			else
			{
				g_App.OpenMannerWnd( pPickChar->GetCharName().c_str(), true );
			}
		}
		else if( pPickPush )
		{
			if( g_EventMgr.IsAlive( EVT_CHRISTMAS, g_MyInfo.GetChannelingType() ) )
			{
				ChristmasEventUserNode *pNode = dynamic_cast<ChristmasEventUserNode*> ( g_MyInfo.GetEventUserMgr().GetEventUserNode( EVT_CHRISTMAS ) );
				if( pNode )
					pNode->SendRequestGift();
			}
		}
	}
	else
	{
		if( pPickChar )
		{
			if( GetModeType() == MT_HEADQUARTERS )
			{
				if( pPickChar->GetIndex() != NPC_INDEX && !pMouse->GetTraining() )
				{
					pMouse->SetToolTipID( pPickChar->GetCharName(), UserInfoToolTip::TT_ROOM );
				}
			}
			else
			{
				pMouse->SetToolTipID( pPickChar->GetCharName(), UserInfoToolTip::TT_ROOM );
			}
		}
	}
}

void ioPlayStage::CheckOwnerCanStartRoulette()
{
	ioBaseChar *pOwnerChar = GetOwnerChar();
	if( !pOwnerChar )	return;

	if( pOwnerChar->EnableRoulette() )
		pOwnerChar->SetRouletteEmoticon();
}

void ioPlayStage::ProcessByOwner( ioCamera *pCamera )
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner )	return;

	ioLookAtCameraController *pCtrl = ToLookAtCtrl( pCamera->GetController() );
	if( !pCtrl )	return;

	if( pOwner->GetState() == CS_DIE ||
		pOwner->GetState() == CS_VIEW ||
		pOwner->GetState() == CS_LOADING ||
		pOwner->GetState() == CS_OBSERVER )
	{
		g_SoundMgr.UpdateListener( pCtrl->GetLookAt() );
	}
	else if( pOwner->CheckDummyCharControl() )
	{
		g_SoundMgr.UpdateListener( pCtrl->GetLookAt() );
	}
	else
	{
		g_SoundMgr.UpdateListener( pOwner->GetWorldPosition() );
	}

	if( !m_bSpecialObserver && pOwner->CheckMovieModeChange() )
	{
		if( m_dwOwnerCameraLoseTime > 0 && m_dwOwnerCameraLoseTime + 500 < FRAMEGETTIME() )
		{
			if( HasCameraFixedStyle( CAMERA_STYLE_MOVIE ) )
				ClearCameraModeFixed( CAMERA_STYLE_MOVIE );
			else
				SetCameraModeFixed( CAMERA_STYLE_MOVIE );
		}
		else if( pOwner->GetState() ==	CS_VIEW || pOwner->GetState() == CS_OBSERVER )
		{
			if( HasCameraFixedStyle( CAMERA_STYLE_MOVIE ) )
				ClearCameraModeFixed( CAMERA_STYLE_MOVIE );
			else
				SetCameraModeFixed( CAMERA_STYLE_MOVIE );
		}
	}	

	if( m_pPlayMode )
		m_pPlayMode->ProcessByOwner();

	if( IsNoCharInputWndOpen() )
		pOwner->EnableKeyInputCheck( false );
	else
		pOwner->EnableKeyInputCheck( true );

	if( !m_bNoneCameraWorking )
	{
		if( m_pGetupStick && pOwner->IsShowGetUpStick() )
		{
			if( m_pGetupStick2 )
				m_pGetupStick2->HideRender();
			if( m_pPressButton )
				m_pPressButton->HideRender();

			m_pGetupStick->ShowRender();
			m_pGetupStick->Update();
		}
		else if( m_pGetupStick2 && pOwner->IsShowGetUpStick2() )
		{
			if( m_pGetupStick )
				m_pGetupStick->HideRender();
			if( m_pPressButton )
				m_pPressButton->HideRender();

			m_pGetupStick2->ShowRender();
			m_pGetupStick2->Update();
		}
		else if( m_pPressButton && pOwner->IsShowPressButton() )
		{
			if( m_pGetupStick )
				m_pGetupStick->HideRender();
			if( m_pGetupStick2 )
				m_pGetupStick2->HideRender();

			m_pPressButton->ShowRender();
			m_pPressButton->Update();
		}
		else
		{
			if( m_pGetupStick )
				m_pGetupStick->HideRender();
			if( m_pGetupStick2 )
				m_pGetupStick2->HideRender();
			if( m_pPressButton )
				m_pPressButton->HideRender();
		}
	}
}

void ioPlayStage::StartSelectCharCameraEvent()
{
	if( m_dwStartSelectCharCameraEvent == 0 ) return;

	g_CameraEventMgr.NotifyCEDestroyed();
	g_CameraEventMgr.CreateCE( m_dwStartSelectCharCameraEvent, this );
}

void ioPlayStage::SoldierSelectWndForcedShow()
{
	if( !P2PNetwork::IsNetworkPlaying() )
		return;

	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner )	return;

	SoldierSelectWnd *pSoldier = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
	if( pSoldier && !pSoldier->IsShow() )
	{
		pSoldier->SetSelectWaitShow( pOwner->GetSelectCharArray(), IsNetworkMode() );
	}
}

void ioPlayStage::ProcessFunctionKey()
{
#ifdef _RELEASE
	if( g_Input.IsKeyPress( KEY_F5 ) )
	{
		SaveOwnerPositionToClipBoard();
	}
#endif

#ifndef SHIPPING
	if( !P2PNetwork::IsNetworkPlaying() && g_Input.IsKeyPress( KEY_F12 ) )
	{
		SetModeEndSceneCamera( false );
		StartModeStartEventCamera( true );
	}
#endif

	if( g_Input.IsKeyDown( KEY_F5) && g_Input.IsKeyDown( KEY_F6) )
		g_InputBox.SetUseClipboard( true );

	bool bPressEnter = g_InputBox.IsEnter();
	if( g_GUIMgr.IsNoneUIMode() || HasCameraFixedStyle( CAMERA_STYLE_MOVIE ) )
	{
		if( bPressEnter || g_Input.IsKeyUp( KEY_ESCAPE ) )
		{
			if( HasCameraFixedStyle( CAMERA_STYLE_MOVIE ) )
				ClearCameraModeFixed( CAMERA_STYLE_MOVIE );
			else
				g_GUIMgr.SetNoneUIMode( false );

			if( bPressEnter )
				bPressEnter = false;
		}
	}

	// 관전용 키 처리 성공하면 이하는 처리 않함
	if( CheckObserverKey() )
		return;

	ioBaseChar *pOwnerChar = GetOwnerChar();
	if( g_KeyManager.IsMovieCaptureKeyPress() )
	{
		bool bPrevMovieCapture = g_MovieMaker.IsMovieCapturing();
		bool bMovieCapture = g_MovieMaker.ToggleMovieCapture();
		if( bPrevMovieCapture != bMovieCapture )
		{
			if( bMovieCapture )
			{
				if ( !Setting::UseVoiceChat() || ( Setting::UseVoiceChat() && !Setting::UseMIC() ) ) // 마이크를 사용하지 않으면
					g_VoiceChat.SetStereoMix();
				
				if( pOwnerChar )
				{
					pOwnerChar->SetMovieStartEmoticon( g_MovieMaker.GetMovieStartTime() );
				}
			}
			else
			{
				if( pOwnerChar )
				{
					pOwnerChar->SetMovieCutEmoticon();
				}
				g_VoiceChat.SetMic();
			}
		}		
	}

	if( !g_App.IsMouseBusy() && !g_GUIMgr.IsShowSettingKeyWnd() && !g_GUIMgr.IsUIKeyBlock() )
	{
		if ( g_KeyManager.IsUsersCustomSoundKeyPress() )
		{
			bool bBlock = false;
			ioWnd *pOptionWnd = g_GUIMgr.FindWnd( GAME_OPTION_WND );
			if( pOptionWnd && pOptionWnd->IsShow())
			{
				ioWnd *pSettingTab = pOptionWnd->FindChildWnd( GameOptionWnd::ID_GAME_TAB_WND );
				if( pSettingTab && pSettingTab->IsShow() )
					bBlock = true;
			}

			if( !bBlock )
				Setting::SetBlockUsersCustomSound( !Setting::UseBlockUsersCustomSound() );
		}

		// macro
		CKeyInfo::KeyTypeArray eKeyTypeArray = g_KeyManager.GetMacroKeyPress();
		const CKeyInfo::MecroTextInfo *pInfo = g_KeyManager.GetMacroTextInfo( eKeyTypeArray );
		g_ChatMacroInfo.SetChatMacro( pOwnerChar, pInfo, GetModeType(),GetRoomIndex(), this);
		
		if( pOwnerChar )
		{
			int iArray = g_KeyManager.GetSoldierKeyPressArray();
			if( COMPARE(iArray, 0, MAX_CHARACTER_KEY ) )
			{				
				ExPCRoomSoldierBonusWnd* pBonusWnd = NULL; 
				if( g_MyInfo.GetPCRoomAuthority() == FREEDAY_EVENT_CODE )
					pBonusWnd = dynamic_cast<ExPCRoomSoldierBonusWnd*>( g_GUIMgr.FindWnd( USER_SELECT_HERO_BONUS_WND ) );
				else if( g_MyInfo.GetPCRoomAuthority() != 0 )
					pBonusWnd = dynamic_cast<ExPCRoomSoldierBonusWnd*>( g_GUIMgr.FindWnd( EX_PCROOM_SOLDIER_BONUS_WND ) );
				if( pBonusWnd && pBonusWnd->IsShow() )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "PC방 보너스 용병 선택 중에는#용병을 교체 할 수 없습니다." );
					return;
				}

				// 용병 선택 UI를 교체가 아닌 설정으로 오픈하는 경우에는 교체하지 않음
				if( m_pPlayMode && !m_pPlayMode->ShowSoldierSettingUI( pOwnerChar, iArray ) )
				{
					if( GetModeType() == MT_PRACTICE )
					{
						return;
					}

					int iSlotToArray = g_MyInfo.GetCharSlotIndexToArray( iArray );
					if( g_MyInfo.GetCharIndex( iSlotToArray ) == 0 )
					{
						if( pOwnerChar->IsCanChangeCharState( iSlotToArray, true ) && pOwnerChar->IsOwnerChar() )
						{
							if( P2PNetwork::IsNetworkPlaying() )
							{
								SoldierSelectWnd *pSoldier = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
								if( pSoldier && !pSoldier->IsShow() )
								{ 
									pSoldier->SetSelectWaitShow( iArray, IsNetworkMode() );
								}
							}
						}
					}
					else
					{
						SoldierSelectWnd *pSoldier = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
						if( pSoldier && pSoldier->IsShow() )
						{
							pSoldier->SetFunctionKeySelect( iSlotToArray, IsNetworkMode() );
						}
						else
						{
							if( pOwnerChar->IsCanChangeCharState( iSlotToArray, true ) && pOwnerChar->IsOwnerChar() )
							{
								if( P2PNetwork::IsNetworkPlaying() )
								{
									if( pSoldier && !pSoldier->IsShow() )
									{
										pSoldier->SetSelectWaitShow( iArray, IsNetworkMode() );
									}
								}
							}
						}
					}
				}				
			}

			int nConsumptionKey = g_KeyManager.GetConsumptionKeyPress();
			
			if( nConsumptionKey != -1)
			{
				if( m_pPlayMode && ( ToTowerDefMode(m_pPlayMode) || ToRaidMode(m_pPlayMode) ) )
				{
					if( m_pPlayMode->GetModeState() == ioPlayMode::MS_PLAY)
					{
						int nSlot = nConsumptionKey - CKeyInfo::KTA_CONSUMPTION1;

						DWORD dwType = g_SlotMgr.GetSlotItemType(nSlot);

						if(COMPARE(dwType, ioEtcItem::EIT_ETC_CONSUMPTION_BUFF01, ioEtcItem::EIT_ETC_CONSUMPTION_BUFF64+1))
						{
							if( pOwnerChar->IsOwnerChar() && pOwnerChar->IsCanSlotBuff() )
								g_SlotMgr.UseItem(nSlot);
						}
						else
						{
							if( dwType == ioEtcItem::EIT_ETC_CONSUMPTION_REVIVE)
							{
								if( pOwnerChar->IsOwnerChar() && pOwnerChar->IsCanSlotRevive() )
									g_SlotMgr.UseItem(nSlot);
							}
						}
					}
				}
			}
		}
	}
}

bool ioPlayStage::CheckObserverKey()
{
	// 키 셋팅 상황이면 처리 안함
	if( g_GUIMgr.IsShowSettingKeyWnd() )
		return false;

	// 모드가 없으면 처리 안함
	if( !m_pPlayMode )
		return false;

	// 관전 상태 아니면 처리 안함
	if( !g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) && !IsStealthMode() )
		return false;

	// 셔플모드면 처리안함.
	if( g_ShuffleRoomMgr.IsShuffleRoom() )
		return false;
	if( g_RankBattleMgr.IsRankBattlePlaying() )
		return false;

	// 플레이 상황 아니면 처리 안함
	if( m_pPlayMode->GetModeState() != ioPlayMode::MS_PLAY )
		return false;

    ioBaseChar *pOwner = GetOwnerChar();

	// CS_LOADING 상태일수 있음
	if( !pOwner || pOwner->GetState() != CS_OBSERVER )
		return false;

	ioBaseChar *pChar = NULL;
	
	int iKeyCnt = 8;
	DWORD dwBlueKey[8] = { KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8 };
	DWORD dwRedKey[8] = { KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I };

	int i = 0;
	int j = 0;
	bool bCheckKeyDown = false;

	// 블루팀 키 체크
	for( i=0; i < iKeyCnt; ++i )
	{
		pChar = NULL;

		if( g_Input.IsKeyDown( dwBlueKey[i] ) )
		{
			bCheckKeyDown = true;

			if( GetModeType() != MT_SURVIVAL && GetModeType() != MT_BOSS && GetModeType() != MT_MONSTER_SURVIVAL && GetModeType() != MT_GANGSI && 
				GetModeType() != MT_DUNGEON_A && GetModeType() != MT_FIGHT_CLUB && GetModeType() != MT_RAID && !Help::IsMonsterDungeonMode(GetModeType()) )
				pChar = m_pPlayMode->GetRecordCharTeamArray( TEAM_BLUE, i );
			else
				pChar = m_pPlayMode->GetRecordCharTeamArray( TEAM_NONE, i );

			if( pChar ) break;
		}
	}

	// 레드팀 키 체크
	if( !pChar )		// 블루팀 키가 이미 눌러졌으면 체크하지 않는다
	{
		for( j=0; j < iKeyCnt; ++j, ++i )
		{
			pChar = NULL;

			if( g_Input.IsKeyDown( dwRedKey[j] ) )
			{
				bCheckKeyDown = true;

				if( GetModeType() != MT_SURVIVAL && GetModeType() != MT_BOSS && GetModeType() != MT_MONSTER_SURVIVAL && 
					GetModeType() != MT_GANGSI && GetModeType() != MT_DUNGEON_A && GetModeType() != MT_FIGHT_CLUB && GetModeType() != MT_RAID && !Help::IsMonsterDungeonMode(GetModeType()))
					pChar = m_pPlayMode->GetRecordCharTeamArray( TEAM_RED, j );
				else		// 데스메치에서는 레드팀은 8번째 유저부터다
					pChar = m_pPlayMode->GetRecordCharTeamArray( TEAM_NONE, i );

				if( pChar ) break;
			}
		}
	}

	if( pChar )
	{
		if( pChar->GetState() != CS_DIE && pChar->GetState() != CS_VIEW && pChar->GetState() != CS_LOADING )
		{
			m_ViewChar = pChar->GetCharName().c_str();
			m_dwViewMovingTime = FRAMEGETTIME();
			m_bSpecialObserver = false;
			g_ObserverWndMgr.ObserverInfoShow( m_bSpecialObserver );

			pChar->ReSetOwnerMarkTeam();

			ioCamera *pCam = g_App.GetCamera();
			if( pCam )
			{
				ioLookAtCameraController *pCtrl = ToLookAtCtrl( pCam->GetController() );
				if( pCtrl )
				{
					m_vLerpStart = pCtrl->GetLookAt();
					m_vLerpStart.y -= m_fCamLookAtHeightOffset;
				}
			}
		}
		return true;
	}

	// 대상이 없어도 해당 키 기능은 동작하지 않게 처리
	// 중간에 인원 변동에 의해 시점이 바뀌던 키가 다른 기능이 나올 수 있기 때문
	return bCheckKeyDown;
}

void ioPlayStage::CheckObserverCameraTarget( const ioHashString &szName )
{
	if( szName.IsEmpty() ) return;

	ioBaseChar *pTarget = GetBaseChar( szName );
	if( pTarget )
	{
		if( pTarget->GetState() != CS_DIE && pTarget->GetState() != CS_VIEW && pTarget->GetState() != CS_LOADING )
		{
			m_ViewChar = szName;
			m_dwViewMovingTime = FRAMEGETTIME();
			m_bSpecialObserver = false;
			g_ObserverWndMgr.ObserverInfoShow( m_bSpecialObserver );

			pTarget->ReSetOwnerMarkTeam();

			ioCamera *pCam = g_App.GetCamera();
			if( pCam )
			{
				ioLookAtCameraController *pCtrl = ToLookAtCtrl( pCam->GetController() );
				if( pCtrl )
				{
					m_vLerpStart = pCtrl->GetLookAt();
					m_vLerpStart.y -= m_fCamLookAtHeightOffset;
				}
			}
		}
	}
}

bool ioPlayStage::IsStealthMode()
{
	if( GetModeType() == MT_NONE			|| 
		GetModeType() == MT_MYROOM			|| 
		GetModeType() == MT_TRAINING		||
		GetModeType() == MT_HEADQUARTERS	||
		GetModeType() == MT_SUCCESSION		||
		GetModeType() == MT_HOUSE )
	{
		return false;
	}

	if( g_App.IsMeDeveloper() )
	{
		if( !g_BattleRoomMgr.IsBattleRoom() && !g_LadderTeamMgr.IsLadderTeam() && 
			!g_ShuffleRoomMgr.IsShuffleRoom() && !g_RankBattleMgr.IsRankBattlePlaying() )
		{
			return true;
		}
	}

	return false;
}

void ioPlayStage::SaveOwnerPositionToClipBoard()
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner )	return;

	D3DXVECTOR3 vPos = pOwner->GetWorldPosition();

	char szCharPosition[100];
	sprintf( szCharPosition, "pos_x = %.1ff\r\npos_y = %.1ff\r\npos_z = %.1ff\r\n",
							 vPos.x, vPos.y, vPos.z );

	HGLOBAL hmem = GlobalAlloc( GHND, 100 );
	char *ptr = (char *)GlobalLock( hmem );
	memcpy( ptr, szCharPosition, strlen(szCharPosition)+1 );
	GlobalUnlock( hmem );

	if ( OpenClipboard( NULL ) )
	{
		EmptyClipboard();
		SetClipboardData( CF_TEXT, hmem );
		CloseClipboard();
	}
}

void ioPlayStage::ProcessWeaponVSMapCollision()
{
	ioOpcodeShape *pShape = m_pSceneMgr->GetCollisionShape();

	ioWeaponManager::WeaponIterator kIter = g_WeaponMgr.GetWeaponIterator();
	while( kIter.HasMoreElements() )
	{
		ioWeapon *pWeapon = kIter.Next();
		if( !pWeapon )
			continue;

		if( !pWeapon->IsLive() )
			continue;

		if( pWeapon->GetType() == AWT_NO_COLLISION_WEAPON )
			continue;

		if( pWeapon->IsIgnoreMapCollision() )
		{
			if( pWeapon->GetType() == AWT_TRACE5 || pWeapon->GetType() == AWT_TRACE6 || pWeapon->GetType() == AWT_MISSILE5 )
				continue;
		}

		if( !pWeapon->HasPeculiarity( WP_MISSILE_WEAPON ) )
		{
			if( m_pBlockWorld && m_pBlockWorld->CheckWeaponVSBlockCollision( this, pWeapon ) )
				continue;

			if( pShape )
				pWeapon->TestMapCollision( pShape, this, NULL );

			continue;
		}

		// WP_MISSILE_WEAPON only
		if( pWeapon->GetPiercingType() == CPT_ALL || pWeapon->GetPiercingType() == CPT_ALL_LIMITE )
		{
			if( pWeapon->GetPiercingType() == CPT_ALL_LIMITE )
			{
				if( m_pBlockWorld && m_pBlockWorld->CheckWeaponVSBlockCollision( this, pWeapon ) )
					continue;

				if( pShape )
					pWeapon->TestMapCollision( pShape, this, NULL );
			}
			continue;
		}

		if( m_pBlockWorld && m_pBlockWorld->CheckWeaponVSBlockCollision( this, pWeapon, true ) )
		{
			g_WeaponMgr.WeaponMapCollisionExecProcedure( this, pWeapon );
		}
		else
		{
			if( pShape )
			{
				if( ioMapCollisionHelper::CheckWeaponPiercingCollision( pWeapon, pShape ) || pShape->TestIntersection( NULL, pWeapon->GetWorldAttackBox( true ) ) )
				{
					g_WeaponMgr.WeaponMapCollisionExecProcedure( this, pWeapon );
				}
			}
		}
	}
}

void ioPlayStage::ProcessWeaponVSMineCollision()
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner )
		return;

	ioWeaponManager::WeaponIterator kMine = g_WeaponMgr.GetMineWeaponIterator();
	while( kMine.HasMoreElements() )
	{
		ioMineWeapon *pMine = ToMineWeapon( kMine.Next() );
		if( !pMine || !pMine->IsCanExplosion() ) continue;
		if( pMine->IsExperienceOwner() ) continue;

		const ioOrientBox &rkMineBox = pMine->GetWorldCollisionBox();

		ioWeaponManager::WeaponIterator kIter = g_WeaponMgr.GetWeaponIterator();
		while( kIter.HasMoreElements() )
		{
			ioWeapon *pOwnWeapon = kIter.Next();

			if( pMine == pOwnWeapon )
				continue;

			if( !pOwnWeapon->IsCanMineExplosion() )
				continue;

			if( pOwnWeapon->IsExperienceOwner() )
				continue;

			const ioOrientBox &rkOwnAttackBox = pOwnWeapon->GetWorldAttackBox();

			if( !ioMath::SphereTestLightly( rkOwnAttackBox, rkMineBox ) )
				continue;

			if( ioMath::TestIntersection( rkOwnAttackBox, rkMineBox, NULL ) )
			{
				if( pMine->IsNormalDead() )
					pMine->SetNormalDeadMine( this, true );
				else
				{
					pMine->SetWaitExplosionMine( this, true );
				}

				break;
			}
		}
	}
}

void ioPlayStage::ProcessWeaponVSMine6Collision()
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner )
		return;

	ioWeaponManager::WeaponIterator kMine = g_WeaponMgr.GetMineWeaponIterator();
	while( kMine.HasMoreElements() )
	{
		ioMineWeapon6 *pMine6 = ToMineWeapon6( kMine.Next() );
		if( !pMine6 || !pMine6->IsCanExplosion() ) continue;
		if( pMine6->IsExperienceOwner() ) continue;

		const ioOrientBox &rkMineBox = pMine6->GetWorldCollisionBox();

		ioWeaponManager::WeaponIterator kIter = g_WeaponMgr.GetWeaponIterator();
		while( kIter.HasMoreElements() )
		{
			ioWeapon *pOwnWeapon = kIter.Next();

			if( pMine6 == pOwnWeapon )
				continue;

			if( !pOwnWeapon->IsCanMineExplosion() )
				continue;

			if( pOwnWeapon->IsExperienceOwner() )
				continue;

			const ioOrientBox &rkOwnAttackBox = pOwnWeapon->GetWorldAttackBox();

			if( !ioMath::SphereTestLightly( rkOwnAttackBox, rkMineBox ) )
				continue;

			if( ioMath::TestIntersection( rkOwnAttackBox, rkMineBox, NULL ) )
			{
				if( pMine6->IsNormalDead() )
					pMine6->SetNormalDeadMine( this, true );
				else
				{
					pMine6->SetWaitExplosionMine( this, true );
				}

				break;
			}
		}
	}
}

void ioPlayStage::ProcessWeaponVSTrackingMineCollision()
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner )	return;

	ioWeaponManager::WeaponIterator kTrackingMine = g_WeaponMgr.GetTrackingMineWeaponIterator();
	while( kTrackingMine.HasMoreElements() )
	{
		ioMineWeapon2 *pMine = ToMineWeapon2( kTrackingMine.Next() );
		if( !pMine || !pMine->IsCanExplosion() ) continue;
		if( pMine->IsExperienceOwner() ) continue;

		const ioOrientBox &rkMineBox = pMine->GetWorldCollisionBox();

		ioWeaponManager::WeaponIterator kIter = g_WeaponMgr.GetWeaponIterator();
		while( kIter.HasMoreElements() )
		{
			ioWeapon *pOwnWeapon = kIter.Next();

			if( pMine == pOwnWeapon )	continue;
			if( !pOwnWeapon->IsCanMineExplosion() ) continue;
			if( pOwnWeapon->IsExperienceOwner() ) continue;

			const ioOrientBox &rkOwnAttackBox = pOwnWeapon->GetWorldAttackBox();

			if( !ioMath::SphereTestLightly( rkOwnAttackBox, rkMineBox ) )
				continue;

			if( ioMath::TestIntersection( rkOwnAttackBox, rkMineBox, NULL ) )
			{
				if( pMine->IsNormalDead() )
					pMine->SetNormalDeadMine( this, true );
				else
				{
					pMine->SetWaitExplosionMine( this, true );
				}
				break;
			}
		}
	}
}

void ioPlayStage::ProcessWeaponVSAirMineCollision()
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner )	return;

	ioWeaponManager::WeaponIterator kMine = g_WeaponMgr.GetMineWeaponIterator();
	while( kMine.HasMoreElements() )
	{
		ioMineWeapon4 *pMine4 = ToMineWeapon4( kMine.Next() );
		if( !pMine4 || !pMine4->IsCanExplosion() ) continue;
		if( pMine4->IsExperienceOwner() ) continue;

		const ioOrientBox &rkMineBox = pMine4->GetWorldCollisionBox();

		ioWeaponManager::WeaponIterator kIter = g_WeaponMgr.GetWeaponIterator();
		while( kIter.HasMoreElements() )
		{
			ioWeapon *pOwnWeapon = kIter.Next();

			if( pMine4 == pOwnWeapon )	continue;
			if( !pOwnWeapon->IsCanMineExplosion() ) continue;
			if( pOwnWeapon->IsExperienceOwner() ) continue;

			const ioOrientBox &rkOwnAttackBox = pOwnWeapon->GetWorldAttackBox();

			if( !ioMath::SphereTestLightly( rkOwnAttackBox, rkMineBox ) )
				continue;

			if( ioMath::TestIntersection( rkOwnAttackBox, rkMineBox, NULL ) )
			{
				if( pMine4->IsNormalDead() )
					pMine4->SetNormalDeadMine( this, true );
				else
				{
					pMine4->SetWaitExplosionMine( this, true );
				}

				break;
			}
		}
	}
}

void ioPlayStage::ProcessWeaponVSEntityCollision()
{
	ioWeaponManager::WeaponIterator kIter = g_WeaponMgr.GetWeaponIterator();

	ErrorReport::SetPosition( 1121, 1 );
	DWORD dwCurTime = FRAMEGETTIME();
	while( kIter.HasMoreElements() )
	{
		ioWeapon *pWeapon = kIter.Next();
		if( !pWeapon->IsLive() ) continue;
		if( pWeapon->IsExperienceOwner() ) continue;

		ErrorReport::SetPosition( 1121, 2 );
		// 사전 충돌체크(폭발전 상태 충돌, 관통이나 튕기기등이 처리)
		GameEntityList::iterator iter=m_GameEntityList.begin();
		for( ; iter!=m_GameEntityList.end() ; ++iter )
		{
			ioPlayEntity *pEntity = ToPlayEntity( *iter );
			if( !pEntity ) continue;
			if( !pEntity->IsCollisionAvailableWeapon( pWeapon ) ) continue;

			if( ToBaseChar(pEntity) && pWeapon->GetCollisionEnableTime() > dwCurTime )
				continue;

			ErrorReport::SetPosition( 1121, 3 );
			pWeapon->CheckPreCollision( pEntity, this );
		}

		// 실제 충돌체크(폭발등에 의한 실제적인 충돌체크)
		bool bCollisioned = false;
		bool bObjectCollisioned = false;
		bool bAllDefenceWeapon = false;

		ErrorReport::SetPosition( 1121, 4 );
		iter=m_GameEntityList.begin();
		for( ; iter!=m_GameEntityList.end() ; ++iter )
		{
			// ApplyWeapon 내부에서 유저가 죽는 경우 SetDeadWeapon()이 호출된다.
			if( !pWeapon->IsLive() )
				break;

			ioPlayEntity *pEntity = ToPlayEntity( *iter );
			ioBaseChar *pChar = ToBaseChar( pEntity );

			if( pChar && pChar->HasBuff( BT_NOWOUND ) )
				continue;

			if( !pEntity )
				continue;

			if (pWeapon->IsOnlyPushStructAttack() && !ToPushStruct(pEntity))
				continue;

			if( (pWeapon->GetPiercingType() == CPT_ALL || pWeapon->GetPiercingType() == CPT_ALL_LIMITE) &&
				!ToBaseChar(pEntity) && !ToBallStruct(pEntity) && !ToDummyChar(pEntity) )
				continue;

			if( !pEntity->IsCollisionAvailableWeapon( pWeapon ) )
				continue;

			if( ToBaseChar(pEntity) && pWeapon->GetCollisionEnableTime() > dwCurTime )
				continue;

			ErrorReport::SetPosition( 1121, 5 );
			if( ToBaseChar(pEntity) && !pWeapon->GetGrapplingTargetName().IsEmpty() &&  pWeapon->CheckGrapplingState() != GPT_NO_CHANGE3 )
			{
				if( pWeapon->CheckGrapplingState() == GPT_EXCEPT &&
					ToBaseChar(pEntity)->GetCharName() == pWeapon->GetGrapplingTargetName() )
				{
					continue;
				}
				else if( pWeapon->CheckGrapplingState() != GPT_EXCEPT &&
					ToBaseChar(pEntity)->GetCharName() != pWeapon->GetGrapplingTargetName() )
				{
					continue;
				}
			}

			bool bMapCollision = false;
			if( ToSymbolStruct( pEntity ) )
				bMapCollision = true;

			if( pWeapon->IsCollision( pEntity, bMapCollision ) )
			{
				ErrorReport::SetPosition( 1121, 6 );
				pWeapon->CheckCurAttribute( pEntity );

				ErrorReport::SetPosition( 1121, 7 );

				ApplyWeaponExp eExplosionType = pEntity->ApplyWeapon( pWeapon );

				if( ToBaseChar(pEntity) )
				{
					g_WeaponMgr.AddCheckWeaponInfo( ToBaseChar(pEntity)->GetCharName(), pWeapon, eExplosionType );
				}

				AddWeaponExplosionEffect( pEntity, pWeapon, eExplosionType );

				pWeapon->AddCollisionEntity( pEntity );
				bCollisioned = true;

				if( pEntity->IsMountAvailable() )
					bObjectCollisioned = true;

				if ( pEntity->IsAllWeaponDefence() )
					bAllDefenceWeapon = true;

				pWeapon->CheckStructAndDummy( pEntity, this );
			}
		}

		ErrorReport::SetPosition( 1121, 8 );
		if( bCollisioned )
		{
			if ( !bAllDefenceWeapon )
			{
				if( pWeapon->HasPeculiarity( WP_ONE_TIME_USE ) )
				{
					if( pWeapon->GetPiercingType() == CPT_NONE )
						pWeapon->SetWeaponDead();
					else if( (pWeapon->GetPiercingType() != CPT_ALL && pWeapon->GetPiercingType() != CPT_ALL_LIMITE) &&
						bObjectCollisioned )
						pWeapon->SetWeaponDead();
				}
				else if( pWeapon->CheckWeaponByBuff() )
				{
					pWeapon->SetWeaponDead();
				}
			}
			else
			{
				if( pWeapon->HasPeculiarity( WP_ONE_TIME_USE ) )
				{
					pWeapon->SetWeaponDead();
				}
			}
		}

		ErrorReport::SetPosition( 1121, 9 );
	}

	ErrorReport::SetPosition( 1121, 10 );
}

void ioPlayStage::ProcessModeItemCollision()
{
	if( !m_pPlayMode )
		return;

	ioOpcodeShape *pShape = m_pSceneMgr->GetCollisionShape();
	if( !pShape )
		return;
	
	m_pPlayMode->ProcessModeItemVSMapCollision( pShape, this, NULL );
	m_pPlayMode->ProcessModeItemVSCharCollision( GetBaseCharList() );
}

void ioPlayStage::DestroyWeaponList()
{
	g_WeaponMgr.DestroyWeaponList();
}

void ioPlayStage::AddWeaponExplosionEffect( ioPlayEntity *pEntity,
										    ioWeapon *pWeapon,
										    ApplyWeaponExp eType )
{
	if( !pEntity || !pWeapon ) return;
	if( eType == AWE_NO_EXPLOSION )	return;
	if( pWeapon->IsAlreadyCollisioned( pEntity ) ) return;
	
	ioBaseChar *pOwner = GetOwnerChar();
	ioBaseChar *pCurChar = ToBaseChar(pEntity);

	ioMapEffect *pExplosion = NULL;
	ioMapEffect *pExtraExplosion = NULL;

	bool bSkipCharEffect = false;
	if( ToBaseChar(pEntity) && pWeapon->IsNoExplosionEffectWithChar() )
		bSkipCharEffect = true;

	switch( eType )
	{
	case AWE_EXPLOSION:
	case AWE_MISSILE_EXPLOSION:
		if( !bSkipCharEffect )
			pExplosion = CreateMapEffect( pWeapon->GetExplosionEffectName(), pEntity->GetWorldScale() );

		pWeapon->PlayWoundedSound( pEntity );

		if( pOwner )
		{
			if( pCurChar && pOwner->GetCharName() == pCurChar->GetCharName())
			{
				pExtraExplosion = CreateMapEffect( pOwner->GetWoundExtraEffect(), pEntity->GetWorldScale() );
			}
			else if( pOwner->GetCharName() == pWeapon->GetOwnerName() )
			{
				pExtraExplosion = CreateMapEffect( pWeapon->GetExtraExplosionEffectName(), pEntity->GetWorldScale() );
			}
		}
		break;
	case AWE_DEFENSE_EXPLOSION:
		pExplosion = CreateMapEffect( "sp2_defense_explosion.txt", pEntity->GetWorldScale() );
		pWeapon->PlayDefensedSound(pEntity);
		break;
	}

	if( pExplosion )
	{
		if( eType == AWE_MISSILE_EXPLOSION )
			pExplosion->SetWorldPosition( pWeapon->GetPosition() );
		else
			pExplosion->SetWorldPosition( pEntity->GetMidPositionByRate() );

		D3DXVECTOR3 vAttackDir = pWeapon->GetAttackDir( pEntity );
		if( vAttackDir == ioMath::VEC3_ZERO )
			vAttackDir = ioMath::UNIT_X;

		pExplosion->SetEffectDirection( vAttackDir );
		pExplosion->ReserveEffectStop( pWeapon->GetActionStopDuration(), pWeapon->GetActionStopLooseRate() );
	}

	if( pExtraExplosion )
	{
		if( eType == AWE_MISSILE_EXPLOSION )
			pExtraExplosion->SetWorldPosition( pWeapon->GetPosition() );
		else
			pExtraExplosion->SetWorldPosition( pEntity->GetMidPositionByRate() );

		D3DXVECTOR3 vAttackDir = pWeapon->GetAttackDir( pEntity );
		if( vAttackDir == ioMath::VEC3_ZERO )
			vAttackDir = ioMath::UNIT_X;

		pExtraExplosion->SetEffectDirection( vAttackDir );
		pExtraExplosion->ReserveEffectStop( pWeapon->GetActionStopDuration(), pWeapon->GetActionStopLooseRate() );
	}
}

void ioPlayStage::AddWeaponExplosionEffect( ioPlayEntity *pEntity,
											const ioHashString &szEffName,
											const ioHashString &szSound,
											const D3DXVECTOR3 &vAttackDir,
											DWORD dwStopDuration,
											float fLooseRate )
{
	if( !pEntity )	return;

	ioMapEffect *pExplosion = CreateMapEffect( szEffName, pEntity->GetWorldScale() );
	if( !pExplosion )	return;

	pExplosion->SetWorldPosition( pEntity->GetMidPositionByRate() );
	pExplosion->SetEffectDirection( vAttackDir );
	pExplosion->ReserveEffectStop( dwStopDuration, fLooseRate );

	ioEffect *pExpEffect = pExplosion->GetEffect();
	if( pExpEffect )
	{
		g_SoundMgr.PlaySound( szSound, pExpEffect->GetParentSceneNode() );
	}
}

void ioPlayStage::AddWeaponExtraExplosionEffect( ioBaseChar *pAttackChar,
												 ioBaseChar *pWoundChar,
												 const ioHashString &szEffect,
												 const D3DXVECTOR3 &vAttackDir,
												 DWORD dwStopDuration,
												 float fLooseRate)
{
	if( !pWoundChar )	return;
	if( szEffect.IsEmpty() ) return;

	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner ) return;

	if( pOwner->GetCharName() != pAttackChar->GetCharName() &&
		pOwner->GetCharName() != pWoundChar->GetCharName() )
		return;

	ioMapEffect *pExplosion = CreateMapEffect( szEffect, pWoundChar->GetWorldScale() );
	if( pExplosion )
	{
		pExplosion->SetWorldPosition( pWoundChar->GetMidPositionByRate() );
		pExplosion->SetEffectDirection( vAttackDir );
		pExplosion->ReserveEffectStop( dwStopDuration, fLooseRate );
	}
}

bool ioPlayStage::ReCalculateMoveVectorByTerrain( ioBaseChar *pChar, D3DXVECTOR3 *pMove, OUT bool &bCol )
{
	bCol = false;
	ioAxisAlignBox kAxisBox = pChar->GetWorldAxisBox();

	D3DXVECTOR3 vUpMinPos = kAxisBox.GetMinPos();
	vUpMinPos.y += ( kAxisBox.GetMaxPos().y - kAxisBox.GetMinPos().y ) * 0.25f;
	kAxisBox.SetMinPos( vUpMinPos );

	ioAxisAlignBox kAxisMoveBox = kAxisBox;
	kAxisMoveBox.Translate( *pMove );
	kAxisMoveBox.Merge( kAxisBox );

	D3DXVECTOR3 vMoveDir;
	D3DXVec3Normalize( &vMoveDir, pMove );

	bool bMonsterVsMonsterCollision = true;
	if( m_pPlayMode )
	{
		bMonsterVsMonsterCollision = m_pPlayMode->IsMonsterVsMonsterCollision();
	}
	
	//탑승 가능한 엔티티 체크
	if( !ioMapCollisionHelper::ReCalculateMoveVectorByMountAvailableEntity( pChar, 
																			kAxisMoveBox,
																			kAxisBox,
																			bMonsterVsMonsterCollision,
																			m_MountAvailableList,
																			NULL,
																			vMoveDir,
																			pMove,
																			bCol ) )
	{
		return false;
	}

	//엔티티 체크
	if( !ioMapCollisionHelper::ReCalculateMoveVectorByEntity( pChar,
															  m_GameEntityList,
															  kAxisMoveBox,
															  kAxisBox,
															  vMoveDir,
															  pMove,
															  bCol ) )
	{
		return false;
	}

	bool bForceMove = false;
	if( pChar->GetForceAmt() > 0.0f )
		bForceMove = true;

	BaseCharList::const_iterator iChar;
	for( iChar=m_BaseCharList.begin() ; iChar!=m_BaseCharList.end() ; ++iChar )
	{
		ioBaseChar *pOther = *iChar;

		if( pChar == pOther )	continue;
		 
		if( !bMonsterVsMonsterCollision )
		{
			if( ToNpcChar( pChar ) && ToNpcChar( pOther ) )
				continue;
		}
		else
		{
			ioNpcChar *pMovingChar = ToNpcChar( pChar);
			ioNpcChar *pNpcChar = ToNpcChar( pOther );
			if(  pMovingChar && pNpcChar )
			{
				if(COMPARE(pMovingChar->GetNpcType(), NT_BIGTOWER, END_FIXING_NPC))
				{
					if(pMovingChar->GetTeam() == pNpcChar->GetTeam())
						continue;
				}

				if(COMPARE(pNpcChar->GetNpcType(), NT_BIGTOWER, END_FIXING_NPC))
				{
					if(pMovingChar->GetTeam() == pNpcChar->GetTeam())
						continue;
				}
			}
		}
		
		if( pChar->IsCharCollisionSkipState( pOther->GetTeam(), (int)pOther->GetState() ) )
			continue;

		if( pOther->IsEntityCollisionSkipState( pChar ) )
			continue;

		if( pOther->IsCharCollisionSkipState( pChar->GetTeam(), (int)pChar->GetState() ) )
			continue;

		if( !ioMapCollisionHelper::IsCollisionCheckRange( pChar->GetWorldPosition(), pOther->GetWorldPosition() ) )
			continue;

		if( pChar->IsSystemActivityState() && pOther->IsSystemActivityState() )
			continue;

		const ioAxisAlignBox &rkMountBox = pOther->GetWorldAxisBox();

		if( !ioMath::TestIntersection( kAxisMoveBox, rkMountBox ) )
			continue;

		D3DXVECTOR3 vCenterDir = rkMountBox.GetCenter() - kAxisBox.GetCenter();
		vCenterDir.y = 0.0f;

		if( D3DXVec3Dot( &vCenterDir, &vMoveDir ) < 0.0f )	// 후면
			continue;

		if( ioMapCollisionHelper::ModifyMoveDir( kAxisBox, kAxisMoveBox, rkMountBox, bForceMove, pMove ) )
		{
			kAxisMoveBox = kAxisBox;
			kAxisMoveBox.Translate( *pMove );
		}
		else
		{
			return false;
		}
	}

	//블럭 메쉬 체크
	if( m_pBlockWorld && !m_pBlockWorld->ReCalculateMoveVectorByBlock( pChar, kAxisMoveBox, vMoveDir, pChar->IsFloating(), pMove, bCol ) )
		return false;

	ioOpcodeShape *pShape = m_pSceneMgr->GetCollisionShape();
	if( !pShape )
		return true;

	//실제 지형 메쉬 체크
	ioOrientBox kCharBox;
	kCharBox.SetByAxisBox( kAxisMoveBox );
	return ioMapCollisionHelper::ReCalculateMoveVectorByWorldMesh( pChar, kCharBox, pShape, NULL, 0.0f, vMoveDir, pChar->IsFloating(), pMove, bCol );
}

void ioPlayStage::ShakeCameraStart( const ShakeCamera &rkShake, int iPriority )
{
	if( m_iCameraShakePriority > iPriority )
		return;

	m_iCameraShakePriority = iPriority;

	m_dwCameraShakeStartTime = FRAMEGETTIME();
	m_dwCameraShakeCheckTime = FRAMEGETTIME();

	m_dwCameraShakeGapTime	= rkShake.dwShakeChangeTime;
	m_fCameraShakeMaxRate	= rkShake.fShakeRate;
	m_fCameraShakeEndAmount = rkShake.fEndAmount;
	
	m_fCameraShakeOffsetX = rkShake.fShakeAmountX;
	m_fCameraShakeOffsetY = rkShake.fShakeAmountY;
}

void ioPlayStage::RollCameraStart( const ShakeCamera &rkShake )
{
	m_dwCameraRollStartTime = FRAMEGETTIME();
	m_dwCameraRollCheckTime = FRAMEGETTIME();
	m_dwCameraRollEndSlerpTime = 0;
	
	m_dwCameraRollGapTime  = rkShake.dwRollChangeTime;
	m_dwCameraRollEndSlerp = rkShake.dwRollEndSlerp;

	m_fCameraRollMaxRate   = rkShake.fRollRate;
	m_fCameraRollEndAmount = rkShake.fRollEndAmount;
	m_fCameraRollValue = rkShake.fRollAmount;
}

void ioPlayStage::ProcessPlayEntityCollision()
{
	ProcessDontMoveEntityCollision();
	ProcessMovingPushEntityCollision();
	ProcessDummyCharEntityCollision();
	ProcessNPCEntityCollision();

	if( P2PNetwork::IsNetworkPlaying() && IsNetworkMode() )
	{
		ProcessCharCollisionNetwork();
	}
	else
	{
		ProcessCharCollisionLocal();
	}
}

void ioPlayStage::ProcessDontMoveEntityCollision()
{
	bool bMonsterVsMonsterCollision = true;
	if( m_pPlayMode )
	{
		bMonsterVsMonsterCollision = m_pPlayMode->IsMonsterVsMonsterCollision();
	}

	float fPushAmt;
	D3DXVECTOR3 vPushDir;

	// 낚시터 PushStruct 생성 시 한번만 생성된 자리에 용병 위치 확인하기 위한 GameEntity객체들 수집
	GameEntityList	GameEntityList;
	GameEntityList.clear();

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioPlayEntity *pFirst = ToPlayEntity( *iter );

		if( !pFirst )
			continue;
			
		if( pFirst->DontMoveEntityLevel() < 1 )
			continue;

		ioBaseChar *pFirstChar = ToBaseChar(pFirst);
		ioNpcChar *pFirstNPCChar = ToNpcChar( pFirst);
		ioDummyChar* pFirstDummy  = ToDummyChar( pFirst );
		ioPushStruct* pFirstStruct = ToPushStruct(pFirst);
		D3DXVECTOR3 vFirstPos = pFirst->GetWorldPosition();
		GameEntityList::iterator iterIn=m_GameEntityList.begin();

		// 낚시터 GameEntity 수집
		if( pFirst->GetSubType() == ioPlayEntity::PST_PUSHSTRUCT )
		{
			if( pFirstStruct && pFirstStruct->GetStructType() == PSTT_FISHING_GROUND && !pFirstStruct->GetAreaCheckCharPos() )
				GameEntityList.push_back( pFirst );
		}

		for( ; iterIn!=m_GameEntityList.end() ; ++iterIn )
		{
			ioPlayEntity *pSecond = ToPlayEntity( *iterIn );

			if( !pSecond )
				continue;

			if( pFirst == pSecond )
				continue;

			if( !ioMapCollisionHelper::IsCollisionCheckRange( vFirstPos, pSecond->GetWorldPosition() ) )
				continue;

			if( !bMonsterVsMonsterCollision )
			{
				if( pFirstNPCChar && ToNpcChar(pSecond) ) 
					continue;
			}
			else
			{
				if(  pFirstNPCChar && pSecond->GetSubType() == ioPlayEntity::PST_CHAR )
				{
					ioNpcChar *pNpcChar = ToNpcChar( pSecond );
					if ( !pNpcChar )
						continue;
					
					if(COMPARE(pNpcChar->GetNpcType(), NT_BIGTOWER, END_FIXING_NPC) )
					{
						if(pFirstNPCChar->GetTeam() == pNpcChar->GetTeam())
							continue;
					}

					if(COMPARE(pFirstNPCChar->GetNpcType(), NT_BIGTOWER, END_FIXING_NPC) )
					{
						if(pFirstNPCChar->GetTeam() == pNpcChar->GetTeam())
							continue;
					}
				}
			}

			if( pSecond->IsEntityCollisionSkipState( pFirst ) )
			{
				pSecond->NotifyDontMoveCollisionPushingSkip( pFirst );
				continue;
			}
				 
			if( pFirst->IsEntityCollisionSkipState( pSecond ) )
			{
				pFirst->NotifyDontMoveCollisionPushingSkip( pSecond );
				continue;
			}
			
			ioBaseChar *pSecondChar = ToBaseChar(pSecond );
			ioDummyChar* pSecondDummy = ToDummyChar( pSecond );
			if( pFirstDummy && pFirstDummy->IsDummyCharCollisionSkipState( pSecondChar ) )
				continue;

			if( pSecondDummy && pSecondDummy->IsDummyCharCollisionSkipState( pFirstChar ) )
				continue;

			if( pFirstDummy && pFirstDummy->IsDummyVsDummyCollisionSkipState( pSecondDummy ) )
				continue;

			if( pSecondDummy && pSecondDummy->IsDummyVsDummyCollisionSkipState( pFirstDummy ) )
				continue;

			if( pFirstDummy && pFirstDummy->GetMountEnable() && pSecondChar )
			{
				if( pSecondChar->IsSystemActivityState() )
					continue;
			}

			if( pSecondDummy && pSecondDummy->GetMountEnable() && pFirstChar )
			{
				if( pFirstChar->IsSystemActivityState() )
					continue;
			}

			if( pSecondChar )
			{
				if( pFirst->IsCharCollisionSkipState( pSecondChar->GetTeam(), (int)pSecondChar->GetState() ) )
					continue;
			}
			else
			{
				if( pFirst->IsCharCollisionSkipState(pSecond->GetTeam(), 0 ) )
					continue;
			}

			if( pFirstChar )
			{
				if( pSecond->IsCharCollisionSkipState( pFirstChar->GetTeam(), (int)pFirstChar->GetState() ) )
					continue;
			}
			else
			{
				if( pSecond->IsCharCollisionSkipState( pFirst->GetTeam(), 0 ) )
					continue;
			}

			if( pFirst->DontMoveEntityLevel() < pSecond->DontMoveEntityLevel() )
				continue;

			if( pFirstChar && pSecondChar )
			{
				if( pFirstChar->IsSystemActivityState() && pSecondChar->IsSystemActivityState() )
					continue;
			}

			if( pSecond->GetSubType() == ioPlayEntity::PST_PUSHSTRUCT && pFirst->GetSubType() == ioPlayEntity::PST_PUSHSTRUCT )
			{
				if( pFirstStruct->GetStructType() == PSTT_BLOCK && ToPushStruct(pSecond)->GetStructType() == PSTT_BLOCK )
					continue;
			}
		
			if( CalcEntityCollisionPushAmt( pFirst, pSecond, vPushDir, fPushAmt ) )
			{	
				ioPushStruct* pSecondPushStruct = NULL;
				pSecondPushStruct = ToPushStruct(pSecond);
				if( pSecondPushStruct && pSecondPushStruct->GetStructType() == PSTT_FISHING_GROUND )
				{
					if( pFirstChar )
					{
						if( !pSecondPushStruct->GetAreaCheckCharPos() )
						{
							D3DXVECTOR3 vPos = pSecondPushStruct->GetWorldPosition();
							vPos.y += 150.0f;
							pFirstChar->SetWorldPosition( vPos );
							return;
						}
					}
				}
				else if( pFirstStruct && pFirstStruct->GetStructType() == PSTT_FISHING_GROUND )
				{
					ioBaseChar* pSecondChar = ToBaseChar(pSecond);
					if( pSecondChar )
					{
						if( !pFirstStruct->GetAreaCheckCharPos() )
						{
							D3DXVECTOR3 vPos = pFirstStruct->GetWorldPosition();
							vPos.y += 150.0f;
							pSecondChar->SetWorldPosition( vPos );
							return;
						}
					}
				}

				D3DXVECTOR3 vMoveAmt = -vPushDir * fPushAmt;
				pSecond->DontMoveCollisionPushingTranslate( pFirst, vMoveAmt );
								
				if( !pFirstChar && pSecondChar )
				{
					// Grappling Check
					if( pSecondChar->CheckGrapplingPushedCollision() )
						pSecondChar->CheckWoundGrapplingBuff( "" );
					else if( pSecondChar->CheckGrapplingPushedBuffCollision() )
						pSecondChar->CheckWoundGrapplingPushedBuff();
					else if( pSecondChar->CheckGrapplingSkillCollision() )
						pSecondChar->CheckSkillGrapplingCancelBuff( "", true );
					else if( pSecondChar->CheckGrapplingSwingCollision() )
						pSecondChar->CheckSkillGrapplingCancelBuff( "", true );

					// AdhesiveBuff Check
					pSecondChar->CheckAdhesiveStateCol();
				}
			}
		}
	}

	// 수집한 낚시터 오프젝트 한번 충돌 체크 후 용병 충돌 시 Y축으로 올리는 작업 중지
	for( iter = GameEntityList.begin(); iter!=GameEntityList.end() ; ++iter )
	{
		ioPlayEntity *pFirst = ToPlayEntity( *iter );

		if( !pFirst )
			continue;

		ioPushStruct* pFirstStruct = ToPushStruct(pFirst);

		if( pFirstStruct && pFirstStruct->GetStructType() == PSTT_FISHING_GROUND )
			pFirstStruct->CompleteAreaCheckCharPos();
	}
}

void ioPlayStage::ProcessMovingPushEntityCollision()
{
	bool bMonsterVsMonsterCollision = true;
	if( m_pPlayMode )
	{
		bMonsterVsMonsterCollision = m_pPlayMode->IsMonsterVsMonsterCollision();
	}

	float fPushAmt;
	D3DXVECTOR3 vPushDir;

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioPushStruct *pFirst = ToPushStruct( *iter );

		if( !pFirst )
			continue;		
		
		//Entity에 대한 체크는 pSecond에서 처리(어느한쪽에서만 Skip해주면 되기 때문)
		if( pFirst->IsEntityCollisionSkipState( NULL ) )
			continue;

		if( pFirst->DontMoveEntityLevel() > 0 )
			continue;

		GameEntityList::iterator iterIn=m_GameEntityList.begin();
		for( ; iterIn!=m_GameEntityList.end() ; ++iterIn )
		{
			ioPlayEntity *pSecond = ToPlayEntity( *iterIn );
			if( !pSecond )
				continue;

			if( pFirst == pSecond )
				continue;

			if( !bMonsterVsMonsterCollision )
			{
				if( ToNpcChar(pFirst) && ToNpcChar(pSecond) ) 
					continue;
			}

			ioBaseChar *pFirstChar = ToBaseChar(pFirst);
			ioBaseChar *pSecondChar = ToBaseChar(pSecond);

			ioDummyChar* pDummy = ToDummyChar( pFirst );
			if( pDummy && pDummy->IsDummyCharCollisionSkipState( pSecondChar ) )
				continue;

			if( pSecondChar )
			{
				if( pFirst->IsCharCollisionSkipState(pSecondChar->GetTeam(), (int)pSecondChar->GetState()) )
					continue;
			}
			else
			{
				if( pFirst->IsCharCollisionSkipState(pSecond->GetTeam(), 0 ) )
					continue;
			}

			if( pFirstChar )
			{
				if( pSecond->IsCharCollisionSkipState(pFirstChar->GetTeam(), (int)pFirstChar->GetState()) )
					continue;
			}
			else
			{
				if( pSecond->IsCharCollisionSkipState(pFirst->GetTeam(), 0 ) )
					continue;
			}
			
			if( pFirst->GetPushChar() == pSecond )
				continue;

			if( pSecond->IsEntityCollisionSkipState( pFirst ) )
				continue;

			ioPushStruct *pPush = ToPushStruct( pSecond );
			if( pPush )
			{
				if( pFirst->IsNoDropStruct() && pPush->IsNoDropStruct() )
					continue;
			}

			if( !ioMapCollisionHelper::IsCollisionCheckRange( pFirst->GetWorldPosition(), pSecond->GetWorldPosition() ) ) continue;

			if( !CalcEntityCollisionPushAmt( pFirst, pSecond, vPushDir, fPushAmt ) )
				continue;

			if( pSecondChar )
			{
				// Grappling Check
				if( pSecondChar->CheckGrapplingPushedCollision() )
					pSecondChar->CheckWoundGrapplingBuff( "" );
				else if( pSecondChar->CheckGrapplingPushedBuffCollision() )
					pSecondChar->CheckWoundGrapplingPushedBuff();
				else if( pSecondChar->CheckGrapplingSwingCollision() )
					pSecondChar->CheckSkillGrapplingCancelBuff( "", true );

				// AdhesiveBuff Check
				pSecondChar->CheckAdhesiveStateCol();
			}

			bool bCol = false;
			D3DXVECTOR3 vMoveAmt = vPushDir * fPushAmt;
			if( pSecond->DontMoveEntityLevel() > 0 )
			{
				pFirst->Translate( vMoveAmt );
			}
			else
			{
				D3DXVECTOR3 vInvMoveAmt = -vMoveAmt;

				if( pSecondChar )
				{
					if( ReCalculateMoveVectorByTerrain( pSecondChar, &vInvMoveAmt, bCol ) )
					{
						pSecondChar->Translate( vInvMoveAmt );
					}
				}
				else
				{
					pSecond->Translate( vInvMoveAmt );
				}

				ioFieldItem *pItem = ToFieldItem( *iterIn );
				if( pItem )
				{
					pItem->SetLastPushChar( pFirst->GetPushChar()->GetCharName() );
				}
			}
		}
	}
}

void ioPlayStage::ProcessBallEntityCollision()
{
	float fPushAmt;
	D3DXVECTOR3 vPushDir;

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioBall *pFirst = ToBallStruct(*iter);

		if( !pFirst )
			continue;

		GameEntityList::iterator iterIn=m_GameEntityList.begin();
		for( ; iterIn!=m_GameEntityList.end() ; ++iterIn )
		{
			ioPlayEntity *pSecond = ToPlayEntity( *iterIn );
			if( !pSecond )
				continue;

			if( pFirst == pSecond )
				continue;

			if( pSecond->IsEntityCollisionSkipState( pFirst ) )
			{
				pSecond->NotifyDontMoveCollisionPushingSkip( pFirst );
				continue;
			}

			if( pFirst->IsEntityCollisionSkipState( pSecond ) )
			{
				pFirst->NotifyDontMoveCollisionPushingSkip( pSecond );
				continue;
			}

			if( !CalcEntityCollisionPushAmt( pFirst, pSecond, vPushDir, fPushAmt ) )
				continue;

			D3DXVECTOR3 vMoveAmt = vPushDir * fPushAmt;

			bool bCol = false;
			ioBaseChar *pChar = ToBaseChar( pSecond );
			if( pChar )
			{
				pFirst->EntityCollision( this, vMoveAmt );

				// Grappling Check
				if( pChar->CheckGrapplingPushedCollision() )
					pChar->CheckWoundGrapplingBuff( "" );
				else if( pChar->CheckGrapplingPushedBuffCollision() )
					pChar->CheckWoundGrapplingPushedBuff();
				else if( pChar->CheckGrapplingSkillCollision() )
					pChar->CheckSkillGrapplingCancelBuff( "", true );
				else if( pChar->CheckGrapplingSwingCollision() )
					pChar->CheckSkillGrapplingCancelBuff( "", true );

				// AdhesiveBuff Check
				pChar->CheckAdhesiveStateCol();

				D3DXVECTOR3 vInvMoveAmt = -vMoveAmt;
				if( ReCalculateMoveVectorByTerrain( pChar, &vInvMoveAmt, bCol ) )
				{
					pChar->Translate( vInvMoveAmt );
				}
			}
			else
			{
				D3DXVECTOR3 vInvMoveAmt = -vMoveAmt;
				
				if( pFirst->DontMoveEntityLevel() < pSecond->DontMoveEntityLevel() )
					pFirst->EntityCollision( this, vMoveAmt );
				else
					pSecond->DontMoveCollisionPushingTranslate( pFirst, vInvMoveAmt );
			}
		}
	}
}

void ioPlayStage::ProcessDummyCharEntityCollision()
{
	float fPushAmt;
	D3DXVECTOR3 vPushDir;

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioDummyChar *pFirst = ToDummyChar(*iter);

		if( !pFirst )
			continue;

		GameEntityList::iterator iterIn=m_GameEntityList.begin();
		for( ; iterIn!=m_GameEntityList.end() ; ++iterIn )
		{
			ioPlayEntity *pSecond = ToPlayEntity( *iterIn );
			if( !pSecond )
				continue;

			if( pFirst == pSecond )
				continue;

			if( pSecond->IsEntityCollisionSkipState( pFirst ) )
			{
				pSecond->NotifyDontMoveCollisionPushingSkip( pFirst );
				continue;
			}

			if( pFirst->IsEntityCollisionSkipState( pSecond ) )
			{
				pFirst->NotifyDontMoveCollisionPushingSkip( pSecond );
				continue;
			}

			if( ToFieldItem( pSecond ) )
				continue;
						
			ioBaseChar *pSecondChar = ToBaseChar(pSecond);

			if( pFirst->IsDummyCharCollisionSkipState( pSecondChar ) )
				continue;

			//프레데터 등의 Entity들을 태우고 이동하는 더미캐릭 일 경우
			if( pSecondChar && pFirst && pFirst->GetMountEnable() )
			{
				if( pSecondChar->IsSystemActivityState() )
					continue;
			}

			if( pSecondChar )
			{
				if( pFirst->IsCharCollisionSkipState( pSecondChar->GetTeam(), (int)pSecondChar->GetState() ) )
					continue;
			}
			else
			{
				if( pFirst->IsCharCollisionSkipState(pSecond->GetTeam(), 0 ) )
					continue;
			}

			ioDummyChar* pSecondDummy = ToDummyChar( pSecond );
			if( pFirst && pFirst->IsDummyVsDummyCollisionSkipState( pSecondDummy ) )
				continue;

			if( pSecondDummy && pSecondDummy->IsDummyVsDummyCollisionSkipState( pFirst ) )
				continue;

			if( !CalcEntityCollisionPushAmt( pFirst, pSecond, vPushDir, fPushAmt ) )
				continue;

			D3DXVECTOR3 vMoveAmt = vPushDir * fPushAmt;

			bool bCol = false;
			if( pSecondChar )
			{
				// Grappling Check
				if( pSecondChar->CheckGrapplingPushedCollision() )
					pSecondChar->CheckWoundGrapplingBuff( "" );
				else if( pSecondChar->CheckGrapplingPushedBuffCollision() )
					pSecondChar->CheckWoundGrapplingPushedBuff();
				else if( pSecondChar->CheckGrapplingSkillCollision() )
					pSecondChar->CheckSkillGrapplingCancelBuff( "", true );
				else if( pSecondChar->CheckGrapplingSwingCollision() )
					pSecondChar->CheckSkillGrapplingCancelBuff( "", true );

				// AdhesiveBuff Check
				pSecondChar->CheckAdhesiveStateCol();

				D3DXVECTOR3 vInvMoveAmt = -vMoveAmt;
				if( pFirst->DontMoveEntityLevel() == pSecond->DontMoveEntityLevel() )
				{
					D3DXVECTOR3 vHalfMoveAmt = vPushDir * (fPushAmt * FLOAT05);

					if( pFirst->isOwnerAttached() )
					{
						if( ReCalculateMoveVectorByTerrain( pFirst, &vMoveAmt, bCol ) )
						{
							pFirst->GetOwner()->Translate( vMoveAmt );
						}

						if( ReCalculateMoveVectorByTerrain( pSecondChar, &vInvMoveAmt, bCol ) )
						{
							pSecond->Translate( vInvMoveAmt );
						}
					}
					else
					{
						pFirst->Translate( vHalfMoveAmt );
					}
				}
				else if( pFirst->DontMoveEntityLevel() < pSecond->DontMoveEntityLevel() )
				{
					pFirst->Translate( vMoveAmt );
				}
				else if( ReCalculateMoveVectorByTerrain( pSecondChar, &vInvMoveAmt, bCol ) )
				{
					pSecondChar->Translate( vInvMoveAmt );
				}
			}
			else
			{
				D3DXVECTOR3 vInvMoveAmt = -vMoveAmt;

				if( pFirst->DontMoveEntityLevel() <= pSecond->DontMoveEntityLevel() )
				{
					if( pFirst->isOwnerAttached() )
					{
						if( ReCalculateMoveVectorByTerrain( pFirst, &vMoveAmt, bCol ) )
						{
							pFirst->GetOwner()->Translate( vMoveAmt );
						}
					}
					else
					{
						pFirst->Translate( vMoveAmt );
					}
				}
				else
				{
					pSecond->DontMoveCollisionPushingTranslate( pFirst, vInvMoveAmt );
				}
			}
		}
	}
}

void ioPlayStage::ProcessCharCollisionNetwork()
{
	ioBaseChar *pOwner = GetOwnerChar();

	//Entity에 대한 체크는 pOther에서 처리(어느한쪽에서만 Skip해주면 되기 때문)
	if( !pOwner || pOwner->IsEntityCollisionSkipState( NULL ) )
		return;

	bool bCollisioned = false;
	int  iOwnerDontMoveLevel = pOwner->DontMoveEntityLevel();

	float fPushAmt;
	D3DXVECTOR3 vPushDir;
	D3DXVECTOR3 vMoveAmt, vRealMoveAmt;

	BaseCharList::iterator iter;
	for( iter=m_BaseCharList.begin() ; iter!=m_BaseCharList.end() ; ++iter )
	{
		ioBaseChar *pOther = *iter;
		if( pOwner == pOther || pOther->IsEntityCollisionSkipState( pOwner ) )
			continue;

		if( pOwner->IsCharCollisionSkipState(pOther->GetTeam(), (int)pOther->GetState() ) )
			continue;

		if( pOther->IsCharCollisionSkipState(pOwner->GetTeam(), (int)pOwner->GetState() ) )
			continue;

		if( !ioMapCollisionHelper::IsCollisionCheckRange( pOwner->GetWorldPosition(), pOther->GetWorldPosition() ) )
			continue;

		if( !CalcEntityCollisionPushAmt( pOwner, pOther, vPushDir, fPushAmt ) )
			continue;

		if( pOwner->IsSystemActivityState() && pOther->IsSystemActivityState() )
			continue;

		// Grappling Check
		if( pOwner->CheckGrapplingPushedCollision() )
		{
			pOwner->CheckWoundGrapplingBuff( pOther->GetCharName() );

			pOther->CheckCollisionGrapplingBuff( pOwner->GetGrapplingAttacker(),
												 pOwner->GetCollisionGrapplingBufList() );
		}
		else if( pOwner->CheckGrapplingPushedBuffCollision() )
		{
			pOwner->CheckWoundGrapplingPushedBuff();

			pOther->CheckCollisionGrapplingPushedBuff( pOwner->GetGrapplingAttacker(),
													   pOwner->GetCollisionGrapplingBufList() );
		}
		else if( pOwner->CheckGrapplingSwingCollision() )
		{
			pOwner->CheckSkillGrapplingCancelBuff( pOther->GetCharName(), true );

			pOther->CheckCollisionSwingGrapplingBuff( pOwner->GetGrapplingAttacker(),
													  pOwner->GetCollisionSwingGrapplingBufList() );
		}

		if( pOther->CheckGrapplingPushedCollision() )
		{
			pOther->CheckWoundGrapplingBuff( pOwner->GetCharName() );

			pOwner->CheckCollisionGrapplingBuff( pOther->GetGrapplingAttacker(),
												 pOther->GetCollisionGrapplingBufList() );
		}
		else if( pOther->CheckGrapplingPushedBuffCollision() )
		{
			pOther->CheckWoundGrapplingPushedBuff();

			pOwner->CheckCollisionGrapplingPushedBuff( pOther->GetGrapplingAttacker(),
													   pOther->GetCollisionGrapplingBufList() );
		}
		else if( pOther->CheckGrapplingSwingCollision() )
		{
			pOther->CheckSkillGrapplingCancelBuff( pOwner->GetCharName(), true );

			pOwner->CheckCollisionSwingGrapplingBuff( pOther->GetGrapplingAttacker(),
													  pOther->GetCollisionSwingGrapplingBufList() );
		}

		bool bCol = false;
		int iOtherDontMoveLevel = pOther->DontMoveEntityLevel();
		if( iOwnerDontMoveLevel == iOtherDontMoveLevel )
		{
			vMoveAmt = vPushDir * ( fPushAmt * FLOAT05 );

			vRealMoveAmt = vMoveAmt;
			if( ReCalculateMoveVectorByTerrain( pOwner, &vRealMoveAmt, bCol ) )
			{
				pOwner->Translate( vRealMoveAmt );
				bCollisioned = true;
			}

			vRealMoveAmt = -vMoveAmt;
			if( ReCalculateMoveVectorByTerrain( pOther, &vRealMoveAmt, bCol ) )
			{
				pOther->Translate( vRealMoveAmt );
			}
		}
		else if( iOwnerDontMoveLevel > iOtherDontMoveLevel )
		{
			vRealMoveAmt = -vPushDir * fPushAmt;
			if( ReCalculateMoveVectorByTerrain( pOther, &vRealMoveAmt, bCol ) )
			{
				pOther->Translate( vRealMoveAmt );
			}
		}
		else	// iOwnerDontMoveLevel < iOtherDontMoveLevel
		{
			vRealMoveAmt = vPushDir * fPushAmt;
			if( ReCalculateMoveVectorByTerrain( pOwner, &vRealMoveAmt, bCol ) )
			{
				pOwner->Translate( vRealMoveAmt );
				bCollisioned = true;
			}
		}
	}

	ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
	if( pLocal && pLocal->IsSendCollisionPos() )
	{
		if( bCollisioned )
		{
			// 고사양의 경우 초당 100회 이상의 메시지 전송이 일어날수 있다.
			// 그리고 그 메세지들을 모두 받아서 처리해야하는 저사양 유저에게는 지옥..

			DWORD dwCurRealTime = REALGETTIME();
			if( m_dwPreCharCollisionSyncTime + 100 < dwCurRealTime )
			{
				m_dwPreCharCollisionSyncTime = dwCurRealTime;
				pOwner->SendCurrentPositionToAllUser();
			}
		}
	}
}

void ioPlayStage::ProcessCharCollisionLocal()
{
	D3DXVECTOR3 vMoveAmt, vRealMoveAmt;

	BaseCharList::iterator iter;
	for( iter=m_BaseCharList.begin() ; iter!=m_BaseCharList.end() ; ++iter )
	{
		ioBaseChar *pFirst = *iter;

		//Entity에 대한 체크는 pSecond에서 처리(어느한쪽에서만 Skip해주면 되기 때문)
		if( pFirst->IsEntityCollisionSkipState( NULL ) )
			continue;

		BaseCharList::iterator iterIn;
		for( iterIn=m_BaseCharList.begin() ; iterIn!=m_BaseCharList.end() ; ++iterIn )
		{
			ioBaseChar *pSecond = *iterIn;
			if( pFirst == pSecond || pSecond->IsEntityCollisionSkipState( pFirst ) )
				continue;

			if( pFirst->IsCharCollisionSkipState(pSecond->GetTeam(), (int)pSecond->GetState()) )
				continue;

			if( pSecond->IsCharCollisionSkipState(pFirst->GetTeam(), (int)pFirst->GetState()) )
				continue;

			if( !ioMapCollisionHelper::IsCollisionCheckRange( pFirst->GetWorldPosition(), pSecond->GetWorldPosition() ) )
				continue;

			float fPushAmt;
			D3DXVECTOR3 vPushDir;
			if( !CalcEntityCollisionPushAmt( pFirst, pSecond, vPushDir, fPushAmt ) )
				continue;

			vMoveAmt = vPushDir * fPushAmt;

			int iFirstLevel  = pFirst->DontMoveEntityLevel();
			int iSecondLevel = pSecond->DontMoveEntityLevel();

			if( pFirst->IsSystemActivityState() && pSecond->IsSystemActivityState() )
				continue;

			// Grappling Check
			if( pFirst->CheckGrapplingPushedCollision() )
			{
				pFirst->CheckWoundGrapplingBuff( pSecond->GetCharName() );

				pSecond->CheckCollisionGrapplingBuff( pFirst->GetGrapplingAttacker(),
													  pFirst->GetCollisionGrapplingBufList() );
			}
			else if( pFirst->CheckGrapplingPushedBuffCollision() )
			{
				pFirst->CheckWoundGrapplingPushedBuff();

				pSecond->CheckCollisionGrapplingPushedBuff( pFirst->GetGrapplingAttacker(),
															pFirst->GetCollisionGrapplingBufList() );
			}
			else if( pFirst->CheckGrapplingSwingCollision() )
			{
				pFirst->CheckSkillGrapplingCancelBuff( pSecond->GetCharName(), true );

				pSecond->CheckCollisionSwingGrapplingBuff( pFirst->GetGrapplingAttacker(),
														   pFirst->GetCollisionSwingGrapplingBufList() );
			}

			if( pSecond->CheckGrapplingPushedCollision() )
			{
				pSecond->CheckWoundGrapplingBuff( pFirst->GetCharName() );

				pFirst->CheckCollisionGrapplingBuff( pSecond->GetGrapplingAttacker(),
													 pSecond->GetCollisionGrapplingBufList() );
			}
			else if( pSecond->CheckGrapplingPushedBuffCollision() )
			{
				pSecond->CheckWoundGrapplingPushedBuff();

				pFirst->CheckCollisionGrapplingPushedBuff( pSecond->GetGrapplingAttacker(),
														   pSecond->GetCollisionGrapplingBufList() );
			}
			else if( pSecond->CheckGrapplingSwingCollision() )
			{
				pSecond->CheckSkillGrapplingCancelBuff( pFirst->GetCharName(), true );

				pFirst->CheckCollisionSwingGrapplingBuff( pSecond->GetGrapplingAttacker(),
														  pSecond->GetCollisionSwingGrapplingBufList() );
			}

			bool bCol = false;
			if( iFirstLevel > iSecondLevel )
			{
				if( ReCalculateMoveVectorByTerrain( pSecond, &vMoveAmt, bCol ) )
				{
					pSecond->Translate( vMoveAmt );
				}
			}
			else if( iFirstLevel < iSecondLevel )
			{
				if( ReCalculateMoveVectorByTerrain( pFirst, &vMoveAmt, bCol ) )
				{
					pFirst->Translate( vMoveAmt );
				}
			}
			else
			{
				vMoveAmt *= FLOAT05;

				vRealMoveAmt = vMoveAmt;
				if( ReCalculateMoveVectorByTerrain( pFirst, &vRealMoveAmt, bCol ) )
				{
					pFirst->Translate( vRealMoveAmt );
				}

				vRealMoveAmt = -vMoveAmt;
				if( ReCalculateMoveVectorByTerrain( pSecond, &vRealMoveAmt, bCol ) )
				{
					pSecond->Translate( vRealMoveAmt );
				}
			}
		}
	}
}

bool ioPlayStage::CheckCollisionLine( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, bool bCheckStruct )
{
	D3DXVECTOR3 vColPos = ioMath::VEC3_ZERO;
	if( m_pBlockWorld && m_pBlockWorld->CheckCollLine( vStart, vEnd, vColPos ) )
		return true;

	ioOpcodeShape *pShape = m_pSceneMgr->GetCollisionShape();
	if( !pShape )
		return false;

	if( !ioMapCollisionHelper::CheckCollisionLine( vColPos, pShape, vStart, vEnd ) )
		return false;

	if( !bCheckStruct )	// 구조물 체크하지 않으면 여기서 return
		return true;

	ioSegment kLineSegment( vStart, vEnd );

	GameEntityList::iterator iter = m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioGameEntity *pGameEntity = *iter;

		ioPlayEntity *pEntity = ToPlayEntity( pGameEntity );
		if( !pEntity )
			continue;

		//변경 코드(가디언 방패-더미가 대신 맞게 설정)
		if( !pEntity->IsMountAvailable() && !pEntity->IsAllWeaponDefence() )
			continue;

		// 이 더미는 IsAllWeaponDefence가 true 이지만 weapon과 entity 사이에서 방해물이 되어선 안되기에 예외 처리를 한다.
		ioDummyChar* pDummy = ToDummyChar( pEntity );
		ioDummyInvinciblePillar* pDummyPillar = ToDummyInvinciblePillar( pDummy );
		if( pDummyPillar )
			continue;

		ioOrientBox kStructBox;
		kStructBox.SetByAxisBox( pEntity->GetWorldAxisBox() );

		if( ioMath::TestIntersection( kLineSegment, kStructBox ) )	// 중간에 오브젝트가 꼈다.
		{
			return false;
		}
	}

	return true;
}

bool ioPlayStage::CheckCollisionLine( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd, D3DXVECTOR3& vColPos, bool bCheckStruct, bool bColCharSkip )
{	
	if( m_pBlockWorld && m_pBlockWorld->CheckCollLine( vStart, vEnd, vColPos ) )
		return true;

	ioOpcodeShape *pShape = m_pSceneMgr->GetCollisionShape();
	if( !pShape )
		return false;

	if( !ioMapCollisionHelper::CheckCollisionLine( vColPos, pShape, vStart, vEnd ) )
		return false;

	if( !bCheckStruct )
		return true;

	ioSegment kLineSegment( vStart, vEnd );

	GameEntityList::iterator iter = m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioGameEntity *pGameEntity = *iter;

		ioPlayEntity *pEntity = ToPlayEntity( pGameEntity );
		if( !pEntity )
			continue;

		ioBaseChar *pChar = ToBaseChar( pEntity );
		if( !pChar || bColCharSkip )
			continue;
		
		ioOrientBox kStructBox;
		kStructBox.SetByAxisBox( pEntity->GetWorldAxisBox() );

		if( ioMath::TestIntersection( kLineSegment, kStructBox ) )
		{
			vColPos = pEntity->GetWorldPosition();
			return false;
		}		
	}

	return true;
}

bool ioPlayStage::CheckCollisionFishingGroundAndPoint( const D3DXVECTOR3 &vStart, const D3DXVECTOR3 &vEnd )
{
	D3DXVECTOR3 vColPos = ioMath::VEC3_ZERO;
	if( m_pBlockWorld && m_pBlockWorld->CheckCollLine( vStart, vEnd, vColPos ) )
		return true;

	ioOpcodeShape *pShape = m_pSceneMgr->GetCollisionShape();
	if( !pShape )
		return false;

	if( !ioMapCollisionHelper::CheckCollisionLine( vColPos, pShape, vStart, vEnd ) )
		return false;

	ioSegment kLineSegment( vStart, vEnd );

	bool bFindFishingGround = false;
	GameEntityList::iterator iter = m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioGameEntity *pGameEntity = *iter;

		ioPlayEntity *pEntity = ToPlayEntity( pGameEntity );
		if( !pEntity )
			continue;

		ioOrientBox kStructBox;
		kStructBox.SetByAxisBox( pEntity->GetWorldAxisBox() );

		// 낚시터 오브젝트를 찾지 못했으면 false반환
		ioPushStruct* pPushStruct = ToPushStruct( pEntity );
		if( pPushStruct && pPushStruct->GetStructType() == PSTT_FISHING_GROUND )
		{
			bFindFishingGround = true;
			D3DXVECTOR3 vEnd2 = vEnd;
			D3DXVECTOR3 vFishBoxCenter = pEntity->GetWorldCollisionBox().GetCenter();

			if( vEnd2.y >= vFishBoxCenter.y )
				vEnd2.y = vFishBoxCenter.y;
			if( ioMath::InBox( vEnd2, pEntity->GetWorldCollisionBox() ) )
			{
				ioBaseChar* pMyChar = GetOwnerChar();
				ioPushStruct* pFishGround = ToPushStruct( pEntity );
				if( pMyChar && pFishGround )
				{
					pMyChar->SetUseFishGroundOwnerInfo( pFishGround->GetOwnerName(), pFishGround->GetPushArrayIdx() );
					return true;
				}
				return false;
			}
			else
			{
				bFindFishingGround = false;
				continue;
			}
		}
	}

	if( !bFindFishingGround )
		return false;

	return true;
}

bool ioPlayStage::CheckCollisionFishingGroundAndFishingGround( ioPushStruct* pPushStruct )
{
	GameEntityList::iterator iter = m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioGameEntity *pGameEntity = *iter;

		ioPlayEntity *pEntity = ToPlayEntity( pGameEntity );
		if( !pEntity )
			continue;

		// 낚시터 오브젝트를 찾지 못했으면 false반환
		ioPushStruct* pCompPushStruct = ToPushStruct( pEntity );
		if( pCompPushStruct && 
			( pPushStruct->GetCreateEtcCode() != pCompPushStruct->GetCreateEtcCode() || pPushStruct->GetStructNum() != pCompPushStruct->GetStructNum() ) )
		{
			if( ioMath::TestIntersection( pPushStruct->GetWorldCollisionBox(), pEntity->GetWorldCollisionBox(), NULL ) )
				return true;
		}

		// symbol struct와의 충돌 체크
		ioSymbolStruct* pCompSymbolStruct = ToSymbolStruct( pEntity );
		if( pCompSymbolStruct )
		{
			if( ioMath::TestIntersection( pPushStruct->GetWorldCollisionBox(), pEntity->GetWorldCollisionBox(), NULL ) )
				return true;
		}
	}

	return false;
}

bool ioPlayStage::CalcEntityCollisionPushAmt( const ioPlayEntity *pFirst, const ioPlayEntity *pSecond, D3DXVECTOR3 &vPushDir, float &fPushAmt )
{
	ioAxisAlignBox kFirstAABB = pFirst->GetWorldAxisBox();
	ioAxisAlignBox kSecondAABB = pSecond->GetWorldAxisBox();

	if( pFirst->GetSubType() == ioPlayEntity::PST_CHAR )
	{
		D3DXVECTOR3 vMin = kFirstAABB.GetMinPos();
		vMin.y = pFirst->GetWorldPosition().y + 1.5f;
		kFirstAABB.SetMinPos( vMin );
	}

	if( pSecond->GetSubType() == ioPlayEntity::PST_CHAR )
	{
		D3DXVECTOR3 vMin = kSecondAABB.GetMinPos();
		vMin.y = pSecond->GetWorldPosition().y + 1.5f;
		kSecondAABB.SetMinPos( vMin );
	}

	if( pFirst->GetSubType() == ioPlayEntity::PST_ROULETTE || pSecond->GetSubType() == ioPlayEntity::PST_ROULETTE )
		return false;
	
	if( pFirst->GetSubType() == ioPlayEntity::PST_PUSHSTRUCT || pSecond->GetSubType() == ioPlayEntity::PST_PUSHSTRUCT )
	{
		ioOrientBox kFirstOrient = pFirst->GetWorldCollisionBox();
		kFirstOrient.SetCenter( kFirstAABB.GetCenter() );

		ioOrientBox kSecondOrient = pSecond->GetWorldCollisionBox();
		kSecondOrient.SetCenter( kSecondAABB.GetCenter() );

		if( pFirst->GetSubType() != ioPlayEntity::PST_PUSHSTRUCT )
			kFirstOrient.SetByAxisBox( kFirstAABB );

		if( pSecond->GetSubType() != ioPlayEntity::PST_PUSHSTRUCT )
			kSecondOrient.SetByAxisBox( kSecondAABB );

		float fColAmt = FLOAT1;
		if( !ioMath::TestIntersection( kFirstOrient, kSecondOrient, &fColAmt ) )
			return false;

		D3DXVECTOR3 vDir( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 vDiff = pFirst->GetWorldPosition() - pSecond->GetWorldPosition();
		vDiff.x *= kFirstAABB.GetExtents( 2 ) / kFirstAABB.GetExtents( 0 );

		if( fabsf( vDiff.x ) > fabsf( vDiff.z ) )
			vDir.x = (vDiff.x>0.0f) ? FLOAT1 : -FLOAT1;
		else
			vDir.z = (vDiff.z>0.0f) ? FLOAT1 : -FLOAT1;
		
		vPushDir = vDir;
		fPushAmt = max( fColAmt, FLOAT1 );
		return true;
	}

	if( pFirst->GetSubType() == ioPlayEntity::PST_WALL_ZONE || pSecond->GetSubType() == ioPlayEntity::PST_WALL_ZONE )
	{
		ioOrientBox kFirstOrient = pFirst->GetWorldCollisionBox();
		kFirstOrient.SetCenter( kFirstAABB.GetCenter() );

		ioOrientBox kSecondOrient = pSecond->GetWorldCollisionBox();
		kSecondOrient.SetCenter( kSecondAABB.GetCenter() );

		if( pFirst->GetSubType() != ioPlayEntity::PST_WALL_ZONE )
			kFirstOrient.SetByAxisBox( kFirstAABB );

		if( pSecond->GetSubType() != ioPlayEntity::PST_WALL_ZONE )
			kSecondOrient.SetByAxisBox( kSecondAABB );

		float fColAmt = FLOAT1;
		if( !ioMath::TestIntersection( kFirstOrient, kSecondOrient, &fColAmt ) )
			return false;

		D3DXVECTOR3 vDir( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 vDiff = pFirst->GetWorldPosition() - pSecond->GetWorldPosition();
		vDiff.x *= kFirstAABB.GetExtents( 2 ) / kFirstAABB.GetExtents( 0 );

		if( fabsf( vDiff.x ) > fabsf( vDiff.z ) )
			vDir.x = (vDiff.x>0.0f) ? FLOAT1 : -FLOAT1;
		else
			vDir.z = (vDiff.z>0.0f) ? FLOAT1 : -FLOAT1;

		vPushDir = vDir;
		fPushAmt = max( fColAmt, FLOAT1 );
		return true;
	}

	if( pFirst->GetSubType() == ioPlayEntity::PST_DUMMY_CHAR || pSecond->GetSubType() == ioPlayEntity::PST_DUMMY_CHAR )
	{
		ioOrientBox kFirstOrient = pFirst->GetWorldCollisionBox();
		kFirstOrient.SetCenter( kFirstAABB.GetCenter() );

		ioOrientBox kSecondOrient = pSecond->GetWorldCollisionBox();
		kSecondOrient.SetCenter( kSecondAABB.GetCenter() );

		if( pFirst->GetSubType() != ioPlayEntity::PST_DUMMY_CHAR )
			kFirstOrient.SetByAxisBox( kFirstAABB );

		if( pSecond->GetSubType() != ioPlayEntity::PST_DUMMY_CHAR )
			kSecondOrient.SetByAxisBox( kSecondAABB );

		float fColAmt = FLOAT1;
		if( !ioMath::TestIntersection( kFirstOrient, kSecondOrient, &fColAmt ) )
			return false;

		D3DXVECTOR3 vDir( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 vDiff = pFirst->GetWorldPosition() - pSecond->GetWorldPosition();
		vDiff.x *= kFirstAABB.GetExtents( 2 ) / kFirstAABB.GetExtents( 0 );

		if( fabsf( vDiff.x ) > fabsf( vDiff.z ) )
			vDir.x = (vDiff.x>0.0f) ? FLOAT1 : -FLOAT1;
		else
			vDir.z = (vDiff.z>0.0f) ? FLOAT1 : -FLOAT1;

		vPushDir = vDir;
		fPushAmt = max( fColAmt, FLOAT1 );
		return true;
	}

	if( pFirst->GetSubType() == ioPlayEntity::PST_BALL || pSecond->GetSubType() == ioPlayEntity::PST_BALL )
	{
		D3DXVECTOR3 vMin, vMax;
		D3DXVECTOR3 vCenter;

		if( pFirst->GetSubType() == ioPlayEntity::PST_BALL )
		{
			ioOrientBox kFirstOrient = pFirst->GetWorldCollisionBox();

			vCenter = kFirstAABB.GetCenter();

			vMin.x = vCenter.x - kFirstOrient.GetExtents(0);
			vMin.y = vCenter.y - kFirstOrient.GetExtents(1);
			vMin.z = vCenter.z - kFirstOrient.GetExtents(2);

			vMax.x = vCenter.x + kFirstOrient.GetExtents(0);
			vMax.y = vCenter.y + kFirstOrient.GetExtents(1);
			vMax.z = vCenter.z + kFirstOrient.GetExtents(2);
			
			kFirstAABB.SetMinMaxPos( vMin, vMax );
		}

		if( pSecond->GetSubType() == ioPlayEntity::PST_BALL )
		{
			ioOrientBox kSecondOrient = pSecond->GetWorldCollisionBox();

			vCenter = kSecondAABB.GetCenter();

			vMin.x = vCenter.x - kSecondOrient.GetExtents(0);
			vMin.y = vCenter.y - kSecondOrient.GetExtents(1);
			vMin.z = vCenter.z - kSecondOrient.GetExtents(2);

			vMax.x = vCenter.x + kSecondOrient.GetExtents(0);
			vMax.y = vCenter.y + kSecondOrient.GetExtents(1);
			vMax.z = vCenter.z + kSecondOrient.GetExtents(2);
			
			kSecondAABB.SetMinMaxPos( vMin, vMax );
		}
	}

	if( !ioMath::TestIntersection( kFirstAABB, kSecondAABB ) )
		return false;

	D3DXVECTOR3 vFirstC  = kFirstAABB.GetCenter();
	D3DXVECTOR3 vSecondC = kSecondAABB.GetCenter();

	D3DXVECTOR3 vDiff = pFirst->GetWorldPosition() - pSecond->GetWorldPosition();
	vDiff.x *= kFirstAABB.GetExtents( 2 ) / kFirstAABB.GetExtents( 0 );

	float fRadiusSum = 0.0f;
	D3DXVECTOR3 vDir( 0.0f, 0.0f, 0.0f );
	if( fabsf( vDiff.x ) > fabsf( vDiff.z ) )
	{
		if( vDiff.x > 0.0f )
			vDir.x = FLOAT1;
		else
			vDir.x = -FLOAT1;

		float fFirstExtent  = vFirstC.x - kFirstAABB.GetMinPos().x;
		float fSecondExtent = vSecondC.x - kSecondAABB.GetMinPos().x;
		fRadiusSum = ( fFirstExtent + fSecondExtent + FLOAT1 ) * 2.0f;

		if( vFirstC.x > vSecondC.x )
		{
			fRadiusSum -= ( vFirstC.x + fFirstExtent ) - ( vSecondC.x - fSecondExtent );
		}
		else
		{
			fRadiusSum -= ( vSecondC.x + fSecondExtent ) - ( vFirstC.x - fFirstExtent );
		}
	}
	else
	{
		if( vDiff.z > 0.0f )
			vDir.z = FLOAT1;
		else
			vDir.z = -FLOAT1;

		float fFirstExtent  = vFirstC.z - kFirstAABB.GetMinPos().z;
		float fSecondExtent = vSecondC.z - kSecondAABB.GetMinPos().z;
		fRadiusSum = ( fFirstExtent + fSecondExtent + FLOAT1 ) * 2.0f;

		if( vFirstC.z > vSecondC.z )
		{
			fRadiusSum -= ( vFirstC.z + fFirstExtent ) - ( vSecondC.z - fSecondExtent );
		}
		else
		{
			fRadiusSum -= ( vSecondC.z + fSecondExtent ) - ( vFirstC.z - fFirstExtent );
		}
	}

	vPushDir = vDir;
	fPushAmt = max( fRadiusSum, FLOAT1 );

	return true;
}

void ioPlayStage::ProcessPlayEntityMapCollision()
{
	ioOpcodeShape *pShape = m_pSceneMgr->GetCollisionShape();

	GameEntityList::iterator iter;
	for( iter = m_GameEntityList.begin(); iter != m_GameEntityList.end() ; ++iter )
	{
		ioGameEntity *pEntity = *iter;
		if( pEntity == NULL )
			continue;

		if( m_pBlockWorld )
		{
			m_pBlockWorld->CheckPlayEntityBlockCollision( pEntity, this );
		}
		
		ioMapCollisionHelper::CheckPlayEntityMapCollision( this, pEntity, pShape, 10 );
	}
}

void ioPlayStage::ProcessBallStructMapCollision()
{
	ioOpcodeShape *pShape = m_pSceneMgr->GetCollisionShape();
	if( !pShape )	return;

	Vector3Vec vPushNrmList;

	GameEntityList::iterator iter;
	for( iter=m_GameEntityList.begin(); iter!=m_GameEntityList.end() ; ++iter )
	{
		ioGameEntity *pGEntity = *iter;
		ioBall *pBall = ToBallStruct( pGEntity );

		if( !pBall ) continue;

		pBall->TestMapCollision( pShape, this, NULL );
	}
}

void ioPlayStage::FindCameraDistCharCheck( ioCamera *pCamera )
{
	ioBaseChar *pOwnerChar = GetOwnerChar();
	m_VisibleBaseList.clear();
	if( pCamera )
	{
		ioLookAtCameraController *pCtrl = ToLookAtCtrl( pCamera->GetController() );
		if( pCtrl )
		{
			D3DXVECTOR3 vCamPos = pCtrl->GetPosition();
			D3DXVECTOR3 vCamDir = pCtrl->GetDirection();
			vCamPos = vCamPos + ( vCamDir * ( pCamera->GetNearP() * 3 ) );

			ioPlane kPlane;
			kPlane.SetPlane( vCamDir, vCamPos );
			BaseCharList::iterator iter = m_BaseCharList.begin();
			for( ; iter!= m_BaseCharList.end() ; ++iter )
			{
				ioBaseChar *pChar = *iter;
				if( !pChar ) continue;

				pChar->SetBottomCircleVisible( true );

                if( !pChar->IsCanInvisible() && !pChar->IsVisibleChar() ) continue;
				if( pOwnerChar && pOwnerChar->GetCharName() == pChar->GetCharName() ) continue;

				const ioAxisAlignBox &kBox = pChar->GetWorldAxisBox();
				if( kPlane.GetSide( kBox.GetMinPos(), kBox.GetMaxPos() ) == ioPlane::NEGATIVE_SIDE )
				{
					pChar->SetBottomCircleVisible( false );
					pChar->SetSpecialHide( true );
					m_VisibleBaseList.push_back( pChar );
				}
				else if( pOwnerChar && pOwnerChar->IsOwnerChar() && m_ScreenBlindType == SBT_BUFF )
				{
					if( !pChar->IsVisibleChar() )
						continue;

					D3DXVECTOR3 vDiff = pOwnerChar->GetWorldPosition() - pChar->GetWorldPosition();
					float fLength = D3DXVec3Length( &vDiff );
					
					if( m_fScreenBlindBuffRange < fLength )
					{
						pChar->SetBottomCircleVisible( false );
						pChar->SetSpecialHide( true );
						m_VisibleBaseList.push_back( pChar );
					}
				}
			}
		}
	}	
}

void ioPlayStage::ClearCameraDistCharCheck()
{
	BaseCharList::iterator iter = m_VisibleBaseList.begin();
	for( ; iter!= m_VisibleBaseList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( pChar )
		{
			ioHelipCopterAttachBuff* pAttachBuff = ToHelipCopterAttachBuff( pChar->GetBuff( BT_HELICOPTER_ATTACH ) );
			if ( !pAttachBuff || pAttachBuff->IsCamCheck() )
			{
				switch( pChar->GetState() )
				{
				case CS_HELICOPTER:
				case CS_HELICOPTER_ATTACH:
					continue;
				}
			}

			pChar->SetSpecialHide( false );
		}
	}

	m_VisibleBaseList.clear();
}

void ioPlayStage::Render( ioRenderSystem *pSystem )
{
	ErrorReport::SetPosition( 1100, 110 );
	m_pSceneMgr->GenerateShadowMap();

	pSystem->ClearBack( D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, m_dwMapClearColor );
	pSystem->SetFogEnable( m_pSceneMgr->IsNowFog() );
	if( pSystem->BeginScene() )
	{
		ErrorReport::SetPosition( 1100, 118 );			

		m_pSceneMgr->RenderVisibleObject();
		ErrorReport::SetPosition( 1100, 119 );

		pSystem->EndScene();
		ErrorReport::SetPosition( 1100, 120 );
	}

	pSystem->SetFogEnable( false );
	ErrorReport::SetPosition( 1100, 121 );

	//Local Glow 출력
	if(  Setting::TrailGlow() )
		m_pSceneMgr->RenderLocalGlowProcess();

	m_pSceneMgr->RenderPostProcess();
	ErrorReport::SetPosition( 1100, 122 );
	
	bool bCheckMode = false;
	if( g_GUIMgr.IsNoneUIMode() ||
		HasCameraFixedStyle(CAMERA_STYLE_MOVIE) ||
		HasCameraFixedStyle(CAMERA_STYLE_VIEW) )
	{
		if( Setting::MovieModeUIHide() )
			bCheckMode = true;
	}

	if( !bCheckMode && m_bBlindAniEnd )
	{
		ErrorReport::SetPosition( 1100, 123 );
		RenderEntityDamage();

		if( m_pPlayMode )
		{
			ErrorReport::SetPosition( 1100, 124 );
			m_pPlayMode->RenderPreGUI();
		}

		ErrorReport::SetPosition( 1100, 125 );
		RenderOverdItemName();
		RenderAllFieldItemIcon();

		ErrorReport::SetPosition( 1100, 126 );
		RenderOwnerCharState();		
	}

	ErrorReport::SetPosition( 1100, 127 );
	UpdateEntityGauge();
	ErrorReport::SetPosition( 1100, 128 );
	RenderEntityGauge();

	ErrorReport::SetPosition( 1100, 129 );

#ifndef SHIPPING
	if( Setting::ShowBoundBox() )
	{
		if( pSystem->BeginScene() )
		{
			RenderCharBox();			
			RenderWeaponBox();
			RenderSceneNodeBox();
			RenderModeEntity();
			pSystem->EndScene();
		}
	}

	if( Setting::ShowBoundSphere() )
	{
		if( pSystem->BeginScene() )
		{
			RenderEntitySphere();
			RenderWeaponSphere();
			pSystem->EndScene();
		}
	}
#endif
	{
		if( pSystem->BeginScene() )
		{
			if( m_pBlockWorld )
				m_pBlockWorld->RenderObject( g_App.GetCamera() );
			pSystem->EndScene();
		}
	}

	UpdateCharEmoticon();
	UpdateCharChatBubble();
	UpdateShuffleModeUI();
	RenderShuffleModeUI();

	if( m_pBlockWorld )
		m_pBlockWorld->RenderUI();

	ErrorReport::SetPosition( 1100, 131 );

	if( !bCheckMode && m_bBlindAniEnd )
	{
		ErrorReport::SetPosition( 1100, 132 );

		RenderUnderUIEmoticon();

		ErrorReport::SetPosition( 1100, 133 );

		RenderBuffBlind();
	}
	
	if( m_pPlayMode )
	{
		m_pPlayMode->RenderPlayMode();
	}
	RenderCharChatBubble();   // 블라인드 위에 찍는다.
	ErrorReport::SetPosition( 1100, 134 );
	
	if( !bCheckMode && m_bBlindAniEnd )
	{
		g_GUIMgr.RenderBackMostWindows();

		ErrorReport::SetPosition( 1100, 135 );

		RenderUpperBlindEmoticon();

		ErrorReport::SetPosition( 1100, 136 );

		g_GUIMgr.RenderNotBackMostWindows();

		ErrorReport::SetPosition( 1100, 137 );

		if( m_pPlayMode )
		{
			m_pPlayMode->RenderAfterGUI();
		}
		g_GUIMgr.RenderToolTip();
		ErrorReport::SetPosition( 1100, 138 );
	}

	ErrorReport::SetPosition( 1100, 139 );
}

void ioPlayStage::UpdateCharChatBubble()
{
	BaseCharList::iterator iter = m_BaseCharList.begin();
	for( ; iter!= m_BaseCharList.end() ; ++iter )
	{
		(*iter)->UpdateChatBubble();
	}
}

void ioPlayStage::RenderCharChatBubble()
{
	ioBaseChar *pCameraTarget = GetCameraTargetChar();
	BaseCharList::iterator iter = m_BaseCharList.begin();
	for( ; iter!= m_BaseCharList.end() ; ++iter )
	{
		(*iter)->RenderChatBubble( pCameraTarget );
	}
}

void ioPlayStage::UpdateCharEmoticon()
{
	bool bSystemVisible = true;
	
	if( m_pPlayMode && m_pPlayMode->GetModeState() == ioPlayMode::MS_RESULT )
	{
		bSystemVisible = false;
	}

	ioBaseChar *pCameraTarget = GetCameraTargetChar();

	BaseCharList::iterator iter = m_BaseCharList.begin();
	for( ; iter!= m_BaseCharList.end() ; ++iter )
	{
		(*iter)->UpdateEmoticon( bSystemVisible, pCameraTarget );
	}
}

void ioPlayStage::RenderUnderUIEmoticon()
{
	if( !m_pPlayMode )	return;

	RECT rcTop, rcBottom;
	if( m_pPlayMode->GetBlindRect( rcTop, rcBottom ) )
	{
		BaseCharList::iterator iter = m_BaseCharList.begin();
		for( ; iter!= m_BaseCharList.end() ; ++iter )
		{
			(*iter)->RenderEmoticon( &rcTop, &rcBottom, false );
		}
	}
	else
	{
		RenderAllEmoticon();
	}
}

void ioPlayStage::RenderBuffBlind()
{
	if( !m_pBuffBlind ) return;
	if( m_BuffBlindState == SBS_NONE ) return;

	float fXPos, fYPos;
	fXPos = (float)Setting::Width() * FLOAT05;
	fYPos = (float)Setting::Height() * FLOAT05;

	m_pBuffBlind->SetScale( (float)Setting::Width(), (float)Setting::Height() );
	m_pBuffBlind->Render( fXPos, fYPos );
}

void ioPlayStage::RenderUpperBlindEmoticon()
{
	if( !m_pPlayMode )	return;

	RECT rcTop, rcBottom;
	if( m_pPlayMode->GetBlindRect( rcTop, rcBottom ) )
	{
		BaseCharList::iterator iter = m_BaseCharList.begin();
		for( ; iter!= m_BaseCharList.end() ; ++iter )
		{
			(*iter)->RenderEmoticon( &rcTop, &rcBottom, true );
		}
	}
}

void ioPlayStage::RenderAllEmoticon()
{
	BaseCharList::iterator iter = m_BaseCharList.begin();
	for( ; iter!= m_BaseCharList.end() ; ++iter )
	{
		(*iter)->RenderEmoticon( NULL, NULL, false );
	}
}

void ioPlayStage::UpdateShuffleModeUI()
{
	if( !m_pPlayMode )
		return;

	if( m_pPlayMode->GetModeType() != MT_SHUFFLE_BONUS )
		return;

	bool bSystemVisible = true;
	if( m_pPlayMode->GetModeState() == ioPlayMode::MS_RESULT )
	{
		bSystemVisible = false;
	}

	ioBaseChar *pCameraTarget = GetCameraTargetChar();

	BaseCharList::iterator iter = m_BaseCharList.begin();
	for( ; iter!= m_BaseCharList.end() ; ++iter )
	{
		(*iter)->UpdateShuffleModeUI( bSystemVisible, pCameraTarget );
	}
}

void ioPlayStage::RenderShuffleModeUI()
{
	if( !m_pPlayMode )
		return;

	if( m_pPlayMode->GetModeType() != MT_SHUFFLE_BONUS )
		return;

	BaseCharList::iterator iter = m_BaseCharList.begin();
	for( ; iter!= m_BaseCharList.end() ; ++iter )
	{
		(*iter)->RenderShuffleModeUI( NULL, NULL, false );
	}
}

void ioPlayStage::RenderOwnerCharState()
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner )	return;

	if( m_pGetupStick && m_pGetupStick->IsShow() )
	{
		D3DXVECTOR3 vCenter = pOwner->GetEntityCenterPos();
		vCenter.y += pOwner->GetWorldCollisionBox().GetExtents(1) * 1.5f;

		int xPos = 0, yPos = 0;
		if( Setting::Check2DInScreen( vCenter, xPos, yPos ) )
		{
			m_pGetupStick->Render( xPos, yPos );
		}
	}

	if( m_pGetupStick2 && m_pGetupStick2->IsShow() )
	{
		D3DXVECTOR3 vCenter = pOwner->GetEntityCenterPos();
		vCenter.y += pOwner->GetWorldCollisionBox().GetExtents(1) * 1.5f;

		int xPos = 0, yPos = 0;
		if( Setting::Check2DInScreen( vCenter, xPos, yPos ) )
		{
			m_pGetupStick2->Render( xPos, yPos );
		}
	}

	if( m_pPressButton && m_pPressButton->IsShow() )
	{
		D3DXVECTOR3 vCenter = pOwner->GetWorldPosition();
		int xPos = 0, yPos = 0;
		if( Setting::Check2DInScreen( vCenter, xPos, yPos ) )
		{
			m_pPressButton->Render( xPos, yPos );
		}
	}

	if( m_bShowBalanceValue )
	{
		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_SHADOW );
		g_FontMgr.SetTextColor( 255, 255, 0 );
		
		if( GetPlazaModeType() != PT_GUILD )
		{
			g_FontMgr.PrintText( FLOAT100, Setting::Height() - 70.0f, 0.7f, STR(1),
				GetMemberBalance(TEAM_RED)*FLOAT100,
				GetMemberBalance(TEAM_BLUE)*FLOAT100 );

			if( m_pPlayMode )
			{
				float fRedRate, fBlueRate;
				fRedRate = g_MathManager.GetScoreBalanceDamage( 0.0f, FLOAT1,
																m_pPlayMode->GetRoundWinCnt(TEAM_BLUE),
																m_pPlayMode->GetRoundWinCnt(TEAM_RED), 
																m_pPlayMode->GetModeType() );

				fBlueRate = g_MathManager.GetScoreBalanceDamage( 0.0f, FLOAT1,
																 m_pPlayMode->GetRoundWinCnt(TEAM_RED),
																 m_pPlayMode->GetRoundWinCnt(TEAM_BLUE), 
																 m_pPlayMode->GetModeType() );

				g_FontMgr.PrintText( FLOAT100, Setting::Height() - 50.0f, 0.7f, STR(2),
																			  fRedRate*FLOAT100,
																			  fBlueRate*FLOAT100 );
			}
		}
		else
		{
			for(int i = 0;i < (int)m_vSingleTeamBalance.size();i++)
			{
				SingleTeamBalance &rSingleTeamBalance = m_vSingleTeamBalance[i];

				g_FontMgr.PrintText( FLOAT100, Setting::Height() - ( 50.0f + (i*20) ), 0.56f, STR(3), 
																							(int)rSingleTeamBalance.m_eTeam,
																							rSingleTeamBalance.m_fDamageBalance * FLOAT100,
																							rSingleTeamBalance.m_fGaugeBalance * FLOAT100 );
			}
		}		
	}

#ifndef SHIPPING
	if( Setting::ShowExtraInfo() )
	{
		D3DXVECTOR3 vOwnerPos = pOwner->GetWorldPosition();

		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_SHADOW );
		g_FontMgr.SetTextColor( 255, 255, 0 );
		g_FontMgr.PrintText( FLOAT1, 340.0f, 0.7f, STR(4), vOwnerPos.x, vOwnerPos.y, vOwnerPos.z );

		g_FontMgr.SetAlignType( TAT_LEFT );
		g_FontMgr.SetTextStyle( TS_SHADOW );
		g_FontMgr.SetTextColor( 255, 255, 0 );
		g_FontMgr.PrintText( FLOAT1, 320.0f, 0.7f, pOwner->GetStateText() );

		int iIndex = 1;
		BaseCharList::iterator iter=m_BaseCharList.begin();
		for( ; iter!=m_BaseCharList.end() ; ++iter )
		{
			if( (*iter)->IsOwnerChar() )
				continue;
			 
			D3DXVECTOR3 vOtherPos = (*iter)->GetWorldPosition();

			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextStyle( TS_SHADOW );
			g_FontMgr.SetTextColor( 255, 255, 0 );
			g_FontMgr.PrintText( FLOAT1, 320.0f + ( iIndex * 40.0f ), 0.7f, (*iter)->GetCharName().c_str() );

			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextStyle( TS_SHADOW );
			g_FontMgr.SetTextColor( 255, 255, 0 );
			g_FontMgr.PrintText( FLOAT1 + 100.0f, 320.0f + ( iIndex * 40.0f ), 0.7f, (*iter)->GetStateText() );

			g_FontMgr.SetAlignType( TAT_LEFT );
			g_FontMgr.SetTextStyle( TS_SHADOW );
			g_FontMgr.SetTextColor( 255, 255, 0 );
			g_FontMgr.PrintText( FLOAT1, 340.0f + ( iIndex * 40.0f ), 0.7f, STR(4), vOtherPos.x, vOtherPos.y, vOtherPos.z );

			iIndex++;
		}
	}
#endif
}

void ioPlayStage::RenderEntityDamage()
{
	if( !m_bRenderDamage )	return;

	D3DXVECTOR3 vPos, v2DPos;

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioPlayEntity *pEntity = ToPlayEntity( *iter );
		if( !pEntity )	continue;

		vPos = pEntity->GetWorldCollisionBox().GetCenter();
		vPos.y += pEntity->GetWorldCollisionBox().GetExtents(1);

		int xPos = 0, yPos = 0;
		if( Setting::Check2DInScreen( vPos, xPos, yPos ) )
		{
			pEntity->RenderHeadInfoList( xPos, yPos );
		}
	}
}

void ioPlayStage::UpdateEntityGauge()
{
	if( !m_bShowCharName )	return;

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioPlayEntity *pEntity = ToPlayEntity( *iter );
		if( pEntity )
		{
			pEntity->UpdateGauge();
		}
	}
}

void ioPlayStage::RenderEntityGauge()
{
	if( !m_bShowCharName )	return;

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioPlayEntity *pEntity = ToPlayEntity( *iter );
		if( pEntity )
		{
			pEntity->RenderGauge();
		}
	}
}

void ioPlayStage::RenderOverdItemName()
{
	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_SHADOW );
	g_FontMgr.SetTextColor( m_iItemNameColorRed, m_iItemNameColorGreen, m_iItemNameColorBlue );

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldItem *pField = ToFieldItem( *iter );

		if( pField && pField->IsOwnerCharOver() )
		{
			D3DXVECTOR3 vPos = pField->GetWorldCollisionBox().GetCenter();

			int xPos = 0, yPos = 0;
			if( Setting::Check2DInScreen( vPos, xPos, yPos ) )
			{
				if( m_bShowItemIcon )
					pField->RenderItemIcon( xPos, yPos, FLOAT05 );

				if( m_bShowItemName )
					g_FontMgr.PrintText( xPos, yPos + 50.0f, m_fItemNameSize, pField->GetItemName().c_str() );
			}
		}
	}
}

void ioPlayStage::RenderAllFieldItemIcon()
{
	if( !g_Input.IsKeyDown( KEY_B ) )
		return;

	g_FontMgr.SetAlignType( TAT_CENTER );
	g_FontMgr.SetTextStyle( TS_NORMAL );
	g_FontMgr.SetTextColor( 240, 240, 240 );

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldItem *pField = ToFieldItem( *iter );
		if( !pField || pField->IsOwnerCharOver() )
			continue;

		D3DXVECTOR3 vPos = pField->GetWorldCollisionBox().GetCenter();

		int xPos = 0, yPos = 0;
		if( Setting::Check2DInScreen( vPos, xPos, yPos ) )
		{
			pField->RenderItemIcon( xPos, yPos, FLOAT05 );
		}
	}
}

void ioPlayStage::ClearStage()
{
	ErrorReport::SetPosition( 1100, 801 );

	ReloadScreenFilter();
	ReloadScreenOveray();
	m_MountAvailableList.clear();
	m_BaseCharList.clear();
	m_EventStructList.clear();
	m_NetCheckMap.clear();

	if( m_pPetMgr )
		m_pPetMgr->DestroyPetList();

	if( m_pSoldierCacheManager )
		m_pSoldierCacheManager->Finalize();

	// gameentitylist 제거보다 먼저할 필요있음.
	ErrorReport::SetPosition( 1100, 802 );
	if( m_pDummyCharMgr )
		m_pDummyCharMgr->DestroyDummyCharList();
	ErrorReport::SetPosition( 1100, 803 );
	if( m_pPushStructMgr )
		m_pPushStructMgr->DestroyStructList();
	ErrorReport::SetPosition( 1100, 804 );
	if( m_pAreaWeaponMgr )
		m_pAreaWeaponMgr->ClearAllAreaWeaponList();

	if( m_pMachineStructMgr )
		m_pMachineStructMgr->DestroyMachineStructList();

	g_RouletteMgr.Initialize();

	ErrorReport::SetPosition( 1100, 805 );
	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		delete *iter;
	}
	m_GameEntityList.clear();

	ErrorReport::SetPosition( 1100, 806 );
	ReserveAddEntityList::iterator iReserve = m_ReserveAddEntityList.begin();
	for( ; iReserve !=m_ReserveAddEntityList.end() ; ++iReserve )
	{
		delete *iReserve;
	}
	m_ReserveAddEntityList.clear();

	ErrorReport::SetPosition( 1100, 807 );

	SAFEDELETE( m_pBuffBlind );

	SAFEDELETE( m_pGetupStick );
	SAFEDELETE( m_pGetupStick2 );
	SAFEDELETE( m_pPressButton );
	ErrorReport::SetPosition( 1100, 808 );

	g_WeaponMgr.ClearWeaponInfo();
	
	SAFEDELETE( m_pAreaWeaponMgr );

	SAFEDELETE( m_pRecvPacketLogManager );
	SAFEDELETE( m_pNoviceGradeBenefitManager );

	ErrorReport::SetPosition( 1100, 812 );
	SAFEDELETE( m_pCameraEventMgr );
	ErrorReport::SetPosition( 1100, 813 );
	SAFEDELETE( m_pShakeCameraMgr );
	ErrorReport::SetPosition( 1100, 814 );
	SAFEDELETE( m_pPushStructMgr );
	ErrorReport::SetPosition( 1100, 815 );
	SAFEDELETE( m_pBallMgr );
	ErrorReport::SetPosition( 1100, 816 );
	SAFEDELETE( m_pMachineStructMgr );
	ErrorReport::SetPosition( 1100, 817 );
	SAFEDELETE( m_pDummyCharMgr );
	ErrorReport::SetPosition( 1100, 818 );
	SAFEDELETE( m_pTalismanMaker );
	ErrorReport::SetPosition( 1100, 819 );
	SAFEDELETE( m_pObserverWndMgr );
	ErrorReport::SetPosition( 1100, 820 );
	SAFEDELETE( m_pWorldEntityFactory );

	SAFEDELETE( m_pPetMgr );
	SAFEDELETE( m_pSoldierCacheManager );

	ErrorReport::SetPosition( 1100, 821 );
	MapEffectList::iterator iEffect = m_EffectList.begin();
	for( ; iEffect!=m_EffectList.end() ; ++iEffect )
	{
		delete *iEffect;
	}
	m_EffectList.clear();

	ErrorReport::SetPosition( 1100, 822 );

	ClearPlayMode();	// 모드는 오브젝트 이후에 삭제되어야 한다.

	g_TCPChecker.AddPlayModeChange();
	ErrorReport::SetPosition( 1100, 823 );
	ErrorReport::SetPosition( 1100, 824 );
	
	g_GUIMgr.ClearAllGUI();
	ErrorReport::SetPosition( 1100, 825 );
	ioBaseChar::ClearOwnerMark();
	ErrorReport::SetPosition( 1100, 826 );

	m_OnControlErr.clear();
	m_vOnControl_Err_SendName.clear();

	g_RequestCharMgr.ClearData();

	ioHomeBlockStorage* pStorage = dynamic_cast<ioHomeBlockStorage*>( g_App.GetHomeStorage() );
	if( pStorage )
		pStorage->ClearBlock();
}

ioGameStage::GameStage ioPlayStage::GetTypeID() const
{
	return GS_PLAY;
}

const char* ioPlayStage::GetTypeText() const
{
	return "GS_PLAY";
}

int ioPlayStage::GetScriptModeStyle()
{
	int iCurModeStyle = MST_NONE;

	if( m_RoomStyle == RSTYLE_BATTLEROOM || m_RoomStyle == RSTYLE_SHUFFLEROOM )
	{
		if( m_pPlayMode && m_pPlayMode->IsSafetyLevelRoom() )
			iCurModeStyle = MST_SAFETY;
		else
			iCurModeStyle = MST_BATTLE;
	}
	else if( m_RoomStyle == RSTYLE_MATCHROOM )
	{
		iCurModeStyle = MST_MATCH;
	}
	else if( m_RoomStyle == RSTYLE_PLAZA )
	{
		iCurModeStyle = MST_PLAZA;
	}
	else if( m_RoomStyle == RSTYLE_LADDERBATTLE )
	{
		iCurModeStyle = MST_LADDER;
	}
	else
	{
		iCurModeStyle = MST_NONE;
	}
	return iCurModeStyle;
}

void ioPlayStage::ClearPlayMode()
{
	SAFEDELETE( m_pPlayMode );
	SAFEDELETE( m_pBlockWorld );
	SAFEDELETE( m_pBlockMgr );
}

void ioPlayStage::ChangePlayMode( ModeType eType, PlazaType ePlazaType )
{
	ErrorReport::SetPosition( 1100, 50 );

	SAFEDELETE( m_pPlayMode );

	g_TCPChecker.AddPlayModeChange( (DWORD)eType );
	ErrorReport::SetPosition( 1100, 51 );

	switch( eType )
	{
	case MT_SYMBOL:
		m_pPlayMode = new ioSymbolMode( this );
		break;
	case MT_UNDERWEAR:
		m_pPlayMode = new ioUnderwearMode( this );
		break;
	case MT_CBT:
		m_pPlayMode = new ioCBTMode( this );
		break;
	case MT_CATCH:
		m_pPlayMode = new ioCatchMode( this );
		break;
	case MT_CATCH_RUNNINGMAN:
		m_pPlayMode = new ioCatchRunningManMode( this );
		break;
	case MT_KING:
		m_pPlayMode = new ioHiddenkingMode( this );
		break;
	case MT_TRAINING:
		m_pPlayMode = new ioTrainingMode( this );
		break;
	case MT_SURVIVAL:
		m_pPlayMode = new ioSurvivalMode( this );
		break;
	case MT_TEAM_SURVIVAL:
		m_pPlayMode = new ioTeamSurvivalMode( this );
		break;
	case MT_BOSS:
		m_pPlayMode = new ioBossMode( this );
		break;
	case MT_MYROOM:
		m_pPlayMode = new ioMyRoomMode( this );
		break;
	case MT_MONSTER_SURVIVAL:
		m_pPlayMode = new ioMonsterSurvivalMode( this );
		break;
	case MT_FOOTBALL:
		m_pPlayMode = new ioFootballMode( this );
		break;
	case MT_HEROMATCH:
		m_pPlayMode = new ioHeroMatchMode( this );
		break;
	case MT_GANGSI:
		m_pPlayMode = new ioGangsiMode( this );
		break;
	case MT_DUNGEON_A:
		m_pPlayMode = new ioDungeonAMode( this );
		break;
	case MT_HEADQUARTERS:
		m_pPlayMode = new ioHeadquartersMode( this );
		break;	
	case MT_FIGHT_CLUB:
		m_pPlayMode = new ioFightClubMode( this );
		break;
	case MT_TOWER_DEFENSE:
	case MT_DARK_XMAS:
	case MT_FIRE_TEMPLE:
	case MT_FACTORY:
		m_pPlayMode = new ioTowerDefMode(this, eType);
		break;
	case MT_DOUBLE_CROWN:
		m_pPlayMode = new ioDoubleCrownMode( this );
		break;
	case MT_SHUFFLE_BONUS:
		m_pPlayMode = new ioShuffleBonusMode( this );
		break;
	case MT_TEAM_SURVIVAL_AI:
		m_pPlayMode = new ioTeamSurvivalAIMode( this );
		break;
	case MT_HOUSE:
		m_pPlayMode = new ioHouseMode( this );
		break;
	case MT_RAID:
		m_pPlayMode = new ioRaidMode( this );
		break;
	case MT_SUCCESSION:
		m_pPlayMode = new ioSuccessionMode( this );
		break;
	case MT_PRACTICE:
		m_pPlayMode = new ioPracticeMode( this );
		g_App.SetPracticeState(true);
		break;
	case MT_FLAG_CAPTURE:
		m_pPlayMode = new ioFlagCaptureMode( this );
		break;
	case MT_ARENA:
		m_pPlayMode = new ioArenaMode( this );
		break;
	case MT_BATTLE:
		m_pPlayMode = new ioBattleMode( this );
		break;
	case MT_FARMING:
		m_pPlayMode = new ioFarmingMode( this );
		break;
	}

	ErrorReport::SetPosition( 1100, 52 );

	if( !m_pPlayMode )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::ChangePlayMode - %d Unknown ModeType", eType );
		return;
	}

	ErrorReport::SetPosition( 1100, 53 );
	g_GlobalTimer.SetWorkPing();

	ErrorReport::SetPosition( 1100, 54 );
	m_pPlayMode->SetModeSubNum( m_iModeSubNum );
	ErrorReport::SetPosition( 1100, 55 );

	g_FishingMgr.LoadFishingInfo();

	SAFEDELETE( m_pBlockWorld );	
	SAFEDELETE( m_pBlockMgr );
	m_pBlockMgr = new ioBlockManager;
	if( m_pBlockMgr )
	{
		m_pBlockMgr->Initialize( this );
	}
	else
	{
		LOG.PrintTimeAndLog(0, "[ERROR] - blockmanager create failed");
	}

	switch( eType )
	{
	case MT_TRAINING:
		{
			if( ePlazaType == PT_GUILD )
			{
				if( m_pBlockMgr )
				{
					m_pBlockMgr->SetBlockModeType( BMT_GUILD );
					m_pBlockMgr->SetTileMax( TILE_MAX_GUILD, HEIGHT_MAX );
				}
				g_FishingMgr.LoadGuildFishingInfo();
				CreateBlockWorld( BMT_GUILD );
			}
		}
		break;
	case MT_HOUSE:
		{
			if( m_pBlockMgr )
			{	
				m_pBlockMgr->SetBlockModeType( BMT_PERSONAL );
				m_pBlockMgr->SetTileMax( TILE_MAX_HOME, HEIGHT_MAX_HOME );
			}

			CreateBlockWorld( BMT_PERSONAL );
		}
		break;
	}

	if(eType != MT_PRACTICE)
	{
		UserInfoToolTip *pUserToolTip = dynamic_cast<UserInfoToolTip*>( g_GUIMgr.AddWnd( "XML/userinfotooltip.xml" ) );
		if( pUserToolTip )
			pUserToolTip->SetPlayStage( this );
	}

	m_pPlayMode->InitMode( m_iModeMapIndex );
	m_pPlayMode->SetModeCreateValueSet();

	ErrorReport::SetPosition( 1100, 56 );
	SetReservedExit( false );

	ErrorReport::SetPosition( 1100, 57 );
	InitCameraController();

	ErrorReport::SetPosition( 1100, 58 );
	g_AbuseMgr.Initialize();         // 어뷰즈 초기화
	ErrorReport::SetPosition( 1100, 59 );
	g_AwardMgr.RemoveAllUser();
	ErrorReport::SetPosition( 1100, 60 );
	g_GlobalSoldierInfo.ClearAwardSoldierMap();
	ErrorReport::SetPosition( 1100, 61 );
	g_QuestMgr.ProcessChangeStage();
	ErrorReport::SetPosition( 1100, 62 );
	g_PresentMgr.SendPresentDataToTime( ioPresentMgr::REQUEST_CHANGE_MODE );
	g_SubscriptionMgr.SendSubscriptionDataToTime();
	ErrorReport::SetPosition( 1100, 63 );
	g_CustomTextureMgr.SetPlayStage( this );	
	ErrorReport::SetPosition( 1100, 64 );

	if( eType == MT_MYROOM || eType == MT_TRAINING || eType == MT_HEADQUARTERS || eType == MT_HOUSE )
	{
		TCPNetwork::SetNagleAlgorithm( true );
	}
	else
	{
		TCPNetwork::SetNagleAlgorithm( false );
	}

	ErrorReport::SetPosition( 1100, 65 );

	g_CostumeInfoMgr.SetCurPlayMode( (int)m_pPlayMode->GetModeType() );
}

void ioPlayStage::CreateBlockWorld( int eType )
{
	m_pBlockWorld = new ioBlockWorld( this );
	if( m_pBlockWorld )
	{
		m_pBlockWorld->Initialize( (BlockModeType)eType );
		m_pBlockWorld->LoadPlayModeBlock();
	}
	else
	{
		LOG.PrintTimeAndLog(0, "[ERROR] - blockworld create failed");
	}
}

void ioPlayStage::ReserveNextModeType( ModeType eType, int iSubNum, int iMapIndex )
{
	m_ReserveNextModeType = eType;
	m_iNextModeMapIndex = iMapIndex;
	m_iNextModeSubNum = iSubNum;
}

ModeType ioPlayStage::GetModeType() const
{
	if( m_pPlayMode )
		return m_pPlayMode->GetModeType();

	return MT_NONE;
}

bool ioPlayStage::IsModeStateResult()
{
	if( !m_pPlayMode ) return false;
	if( m_pPlayMode->GetModeState() == ioPlayMode::MS_RESULT ) return true;

	return false;	
}

bool ioPlayStage::IsSafetyLevelRoom() const
{
	if( m_pPlayMode )
		return m_pPlayMode->IsSafetyLevelRoom();

	return false;
}

int ioPlayStage::GetRoomNum()
{
	if( m_pPlayMode )
		return m_pPlayMode->GetRoomNum();

	return -1;
}

PlazaType ioPlayStage::GetPlazaModeType()
{
	if( !m_pPlayMode || m_pPlayMode->GetModeType() != MT_TRAINING )
		return PT_NONE;

	return m_ePlazaType;
}

bool ioPlayStage::IsCustomPlazaMode()
{
	if( !m_pPlayMode || m_pPlayMode->GetModeType() != MT_TRAINING )
		return false;

	return !m_bOpenPlaza;
}

ioEntityGroup* ioPlayStage::CreateEntityGrpAndSceneNode( const ioHashString &kFileName,
														 bool bThread,
														 bool bTestObj )
{
	ioEntityGroup *pGrp = m_pSceneMgr->CreateEntityGrp();
	if( pGrp )
	{
		ioSceneNode *pSNode = m_pSceneMgr->GetRootSceneNode()->CreateChildSceneNode();
		if( pSNode )
		{
			if( !bTestObj )
				pSNode->AttachObject( pGrp );

			bool bCheckChar = Help::CheckEnableTimeGapUser();
			DWORD dwStartTime = 0;
			DWORD dwGapTime = 0;

			if( bCheckChar )
				dwStartTime = timeGetTime();

			if( !pGrp->LoadGroup( kFileName.c_str(), bThread ) )
			{
				pSNode->DetachObject( pGrp );
				pSNode->RemoveAndDestroyAllChildren();

				m_pSceneMgr->DestroySceneNode( pSNode->GetUniqueID() );
				m_pSceneMgr->DestroyEntityGrp( pGrp );

				pSNode = NULL;
				pGrp = NULL;
			}

			if( bCheckChar )
			{
				dwGapTime = timeGetTime() - dwStartTime;

				if( dwGapTime > 10 )
					LOG.PrintTimeAndLog( 0, "FC - CreateEntityGrp: %s, %d", kFileName.c_str(), dwGapTime );
			}
		}
	}

	return pGrp;
}

void ioPlayStage::AddGameEntity( ioGameEntity *pEntity )
{
	if( pEntity )
	{
		m_GameEntityList.push_back( pEntity );

		if( pEntity->IsMountAvailable() )
		{
			m_MountAvailableList.push_back( pEntity );
		}
	}
}

void ioPlayStage::AddBaseChar( ioBaseChar *pBaseChar )
{
	if( pBaseChar )
	{
		m_BaseCharList.push_back( pBaseChar );

		//개발자 정,부 캐라면 로그를 출력
		if( Help::CheckEnableTimeGapUser2( 1 ) )
			LOG.PrintTimeAndLog( 0, "%s:%s:%d", __FUNCTION__, pBaseChar->GetCharName().c_str(), (int)pBaseChar->GetControlType() );
	}
}

void ioPlayStage::AddAniEventHandler( ioBaseChar *pBaseChar, ioEntityGroup *pGrp )
{
	if( pBaseChar && pGrp )
	{
		ioClientAniEventHandler *pHandler = new ioClientAniEventHandler;
		pHandler->SetBaseChar( pBaseChar );
		pGrp->SetAniEventHandler( pHandler );
	}
}

GameEntityList::iterator ioPlayStage::EraseGameEntity( GameEntityList::iterator iter )
{
	GameEntityList::iterator it;
	ioGameEntity* pEntity = *iter;
	if( pEntity )
	{
		if( pEntity->IsMountAvailable() )
		{
			m_MountAvailableList.remove( pEntity );
		}

		it = m_GameEntityList.erase( iter );
		delete pEntity;
	}
	return it;
}

bool ioPlayStage::TestWorldMeshCollision( const ioRay &rkRay, D3DXVECTOR3 *pColPoint )
{
	if( m_pBlockWorld && m_pBlockWorld->TestBlockMeshCollision( rkRay, pColPoint ) )
		return true;

	ioOpcodeShape *pShape = m_pSceneMgr->GetCollisionShape();
	if( pShape )
		return pShape->TestIntersection( NULL, rkRay, NULL, pColPoint );

	return false;
}

bool ioPlayStage::TestWorldMeshCollision( const ioOrientBox &rkBox )
{
	if( m_pBlockWorld && m_pBlockWorld->TestBlockMeshCollision( rkBox ) )
		return true;

	ioOpcodeShape *pShape = m_pSceneMgr->GetCollisionShape();
	if( pShape )
		return pShape->TestIntersection( NULL, rkBox );

	return false;
}

ioPushStruct* ioPlayStage::CreatePushStruct( int iININum,
											 int iIndex,
											 const D3DXVECTOR3 &vTargetPos,
											 const D3DXQUATERNION &qtTargetRot,
											 const ioHashString &szOwnerName,
											 DWORD dwCreateGapTime,
											 DWORD dwSeed,
											 DWORD dwEtcCode,
											 bool bAddList )
{
	ioINILoader kLoader = g_PushStructListMgr.GetINILoader();

	char szTitle[MAX_PATH];
	wsprintf_e( szTitle, "push_struct%d", iININum );
	kLoader.SetTitle( szTitle );

	char szStruct[MAX_PATH];
	kLoader.LoadString_e( "push_struct", "", szStruct, MAX_PATH );

	ioPushStruct *pPush = g_PushStructListMgr.CreatePushStruct( szStruct, m_pPlayMode );
	if( !pPush )	return NULL;

	pPush->InitResource();
	pPush->LoadProperty( kLoader );
	pPush->SetShadowCastEnable( true );
	pPush->SetOwner( szOwnerName );

	D3DXVECTOR3 vScale;
	vScale.x = kLoader.LoadFloat_e( "scale_x", FLOAT1 );
	vScale.y = kLoader.LoadFloat_e( "scale_y", FLOAT1 );
	vScale.z = kLoader.LoadFloat_e( "scale_z", FLOAT1 );

	D3DXVECTOR3 vPos = vTargetPos;

	ioBaseChar *pOwner = GetBaseChar( szOwnerName );
	if( pOwner )
	{
		if( vPos.y <= 0.0f )
		{
			const ioAxisAlignBox &rkWorldBox = pOwner->GetWorldAxisBox();
			vPos.y = rkWorldBox.GetCenter().y + rkWorldBox.GetExtents( 1 );
		}

		if( pPush->IsNoDropStruct() )
		{
			vPos = vTargetPos;
			pPush->SetWorldPosition( vPos );
		}
		else if( pPush->GetStructType() == PSTT_BLOCK )
		{
			vPos = vTargetPos;
			pPush->SetWorldPosition( vPos );
		}
		else
		{
			pPush->SetWorldPosition( vPos );
			vPos.y = GetMapHeight( vPos.x, vPos.z, pPush, true );
		}

		pPush->SetWorldOrientation( qtTargetRot );
		pPush->InitPushStructInfo( iININum, iIndex, dwCreateGapTime, dwSeed, dwEtcCode, vPos, vScale );

		float fPushAmt;
		bool bCol = false;
		D3DXVECTOR3 vPushDir;
		if( CalcEntityCollisionPushAmt( pPush, pOwner, vPushDir, fPushAmt ) )
		{
			D3DXVECTOR3 vMoveAmt = -vPushDir * fPushAmt;
			if( ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
			{
				pOwner->Translate( vMoveAmt );
			}
		}
	}
	else
	{
		if( vPos.y < FLOAT1 )
		{
			vPos.y = GetMapHeight( vPos.x, vPos.z, pPush, false );
		}

		pPush->InitPushStructInfo( iININum, iIndex, dwCreateGapTime, dwSeed, dwEtcCode, vPos, vScale );
	}

	if( bAddList )
	{
		AddGameEntity( pPush );
	}
	else
	{
		m_ReserveAddEntityList.push_back( pPush );
	}

	return pPush;
}

ioPushStruct* ioPlayStage::CreateTestPushStruct( int iININum,
	int iIndex,
	const D3DXVECTOR3 &vTargetPos,
	const D3DXQUATERNION &qtTargetRot,
	const ioHashString &szOwnerName,
	DWORD dwEtcCode )
{
	ioINILoader kLoader = g_PushStructListMgr.GetINILoader();

	char szTitle[MAX_PATH];
	wsprintf_e( szTitle, "push_struct%d", iININum );
	kLoader.SetTitle( szTitle );

	char szStruct[MAX_PATH];
	kLoader.LoadString_e( "push_struct", "", szStruct, MAX_PATH );

	ioPushStruct *pPush = g_PushStructListMgr.CreatePushStruct( szStruct, m_pPlayMode, true );
	if( !pPush )	return NULL;

	pPush->InitResource();
	pPush->LoadProperty( kLoader );
	pPush->SetShadowCastEnable( true );
	pPush->SetOwner( szOwnerName );

	D3DXVECTOR3 vScale;
	vScale.x = kLoader.LoadFloat_e( "scale_x", FLOAT1 );
	vScale.y = kLoader.LoadFloat_e( "scale_y", FLOAT1 );
	vScale.z = kLoader.LoadFloat_e( "scale_z", FLOAT1 );

	D3DXVECTOR3 vPos = vTargetPos;

	ioBaseChar *pOwner = GetBaseChar( szOwnerName );
	if( pOwner )
	{
		if( vPos.y <= 0.0f )
		{
			const ioAxisAlignBox &rkWorldBox = pOwner->GetWorldAxisBox();
			vPos.y = rkWorldBox.GetCenter().y + rkWorldBox.GetExtents( 1 );
		}

		if( pPush->IsNoDropStruct() )
		{
			vPos = vTargetPos;
			pPush->SetWorldPosition( vPos );
		}
		else if( pPush->GetStructType() == PSTT_BLOCK )
		{
			vPos = vTargetPos;
			pPush->SetWorldPosition( vPos );
		}
		else
		{
			pPush->SetWorldPosition( vPos );
			vPos.y = GetMapHeight( vPos.x, vPos.z, pPush, true );
		}

		pPush->SetWorldOrientation( qtTargetRot );
		pPush->InitPushStructInfo( iININum, -1, 0, 0, dwEtcCode, vPos, vScale );

		float fPushAmt;
		bool bCol = false;
		D3DXVECTOR3 vPushDir;
		if( CalcEntityCollisionPushAmt( pPush, pOwner, vPushDir, fPushAmt ) )
		{
			D3DXVECTOR3 vMoveAmt = -vPushDir * fPushAmt;
			if( ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
			{
				pOwner->Translate( vMoveAmt );
			}
		}
	}
	else
	{
		if( vPos.y < FLOAT1 )
		{
			vPos.y = GetMapHeight( vPos.x, vPos.z, pPush, false );
		}

		pPush->InitPushStructInfo( iININum, -1, 0, 0, dwEtcCode, vPos, vScale );
	}

	return pPush;
}

ioBall* ioPlayStage::CreateBallStruct( int iININum, int iIndex,
									   const D3DXVECTOR3 &vTargetPos,
									   const D3DXQUATERNION &qtTargetRot,
									   bool bAddList )
{
	ioINILoader_e kLoader( "config/sp2_ball_struct.ini" );

	char szTitle[MAX_PATH];
	wsprintf_e( szTitle, "ball_struct%d", iININum );
	kLoader.SetTitle( szTitle );

	char szStruct[MAX_PATH];
	kLoader.LoadString_e( "ball_struct", "", szStruct, MAX_PATH );

	ioBall *pBall = g_BallStructMgr.CreateBallStruct( szStruct, m_pPlayMode );
	if( !pBall )	return NULL;

	pBall->LoadProperty( kLoader );
	pBall->SetShadowCastEnable( true );

	D3DXVECTOR3 vScale;
	vScale.x = kLoader.LoadFloat_e( "scale_x", FLOAT1 );
	vScale.y = kLoader.LoadFloat_e( "scale_y", FLOAT1 );
	vScale.z = kLoader.LoadFloat_e( "scale_z", FLOAT1 );

	D3DXVECTOR3 vPos = vTargetPos;
	if( vPos.y < FLOAT1 )
	{
		vPos.y = GetMapHeight( vPos.x, vPos.z, pBall, false );
	}

	pBall->InitStructInfo( iININum, iIndex, vPos, vScale );

	if( bAddList )
	{
		AddGameEntity( pBall );
	}
	else
	{
		m_ReserveAddEntityList.push_back( pBall );
	}

	return pBall;
}

ioWorldDecoration* ioPlayStage::CreateWorldDecoration( const ioHashString &kFileName )
{
	ioEntityGroup *pGrp = CreateEntityGrpAndSceneNode( kFileName, true );
	if( !pGrp )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::CreateWorldDecoration - %s Create Failed", kFileName.c_str() );
		return NULL;
	}
	
	pGrp->SetLightApplyMask( APPLY_SECOND_MAIN_LIGHT );

	ioWorldDecoration *pDeco = new ioWorldDecoration( pGrp );
	if( pDeco )
	{
		AddGameEntity( pDeco );
	}

	return pDeco;
}

ioPlayEntity* ioPlayStage::CreateWorldEntity( ioINILoader &rkLoader, int iCreateIndex )
{
	ioPlayEntity *pEntity = m_pWorldEntityFactory->CreateWorldEntity( rkLoader, iCreateIndex );
	if( pEntity )
	{
		AddGameEntity( pEntity );
	}

	return pEntity;
}

ioPlayEntity* ioPlayStage::GetWorldEntity( int iEntitySubType, int iCreateIndex )
{
	GameEntityList::iterator iter=m_GameEntityList.begin();
	for(;iter != m_GameEntityList.end();++iter)
	{
		ioPlayEntity *pPlayEntity = ToPlayEntity( *iter );

		if( !pPlayEntity ) continue;
		if( pPlayEntity->GetSubType() != iEntitySubType ) continue;
		if( m_pWorldEntityFactory->IsWorldEntity( pPlayEntity, iCreateIndex ) == false ) continue;

		return pPlayEntity;
	}

	return NULL;	
}

void ioPlayStage::RemoveWorldEntity( ioPlayEntity *pWorldEntity )
{
	if( pWorldEntity == NULL )
		return;

	m_pWorldEntityFactory->RemoveWorldEntity( pWorldEntity );

	if( pWorldEntity->IsMountAvailable() )
	{
		m_MountAvailableList.remove( pWorldEntity );
	}
	m_GameEntityList.remove( pWorldEntity );

	SAFEDELETE( pWorldEntity );	
}

ioBaseChar* ioPlayStage::CreateBaseChar( const ioHashString &kFileName )
{
	ioEntityGroup *pGrp = CreateEntityGrpAndSceneNode( kFileName, true );
	if( !pGrp )
	{
		return NULL;
	}

	ioBaseChar *pBaseChar = new ioBaseChar( pGrp, this, m_pPlayMode );
	if( !pBaseChar )
	{
		return NULL;
	}

	AddGameEntity( pBaseChar );

	pBaseChar->SetShadowCastEnable( true );
	pBaseChar->SetWorldPosition( D3DXVECTOR3( FLOAT10, FLOAT10, FLOAT10 ) );
	pBaseChar->FillMaxHP();

	AddAniEventHandler( pBaseChar, pGrp );
	AddBaseChar( pBaseChar );

	return pBaseChar;
}

ioFieldRewardItem* ioPlayStage::CreateFieldRewardItem( int iFieldRewardUniqueIndex, int iFieldRewardResourceIndex )
{
	const ioHashString kMeshName = g_FieldRewardItemMgr.GetMeshName( iFieldRewardResourceIndex );
	ioEntityGroup *pGrp = CreateEntityGrpAndSceneNode( kMeshName, true );
	if( !pGrp )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::CreateFieldRewardItem - %s Create Failed", kMeshName.c_str() );
		return NULL;
	}

	ioFieldRewardItem *pField = new ioFieldRewardItem( pGrp, m_pPlayMode, iFieldRewardUniqueIndex, iFieldRewardResourceIndex );
	pField->LoadResource();
	pField->SetShadowCastEnable( true );
	AddGameEntity( pField );
	return pField;
}

ioFieldRewardItem* ioPlayStage::FindUserPickFieldRewardItem( ioBaseChar *pChar )
{
	if( !pChar )
		return NULL;

	if( pChar->GetObject() || pChar->IsObejctEquipState() )
		return NULL;
	ioFieldRewardItem *pTargetItem = NULL;
	float fMostDist = std::numeric_limits<float>::infinity();

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldRewardItem *pItem = ToFieldRewardItem( *iter );
		if( !pItem || !pItem->IsCanPicked() ) continue;
		if( !pItem->GetOwnerCharName().IsEmpty() && pItem->GetOwnerCharName() != pChar->GetCharName() ) continue;

		float fTargetDist = -FLOAT1;
		D3DXVECTOR3 vCenter = pItem->GetWorldCollisionBox().GetCenter();
		if( !pChar->CheckSphereDistance( vCenter, m_fPickItemRange, fTargetDist ) )
			continue;

		if( COMPARE( fTargetDist, 0.0f, m_fPickItemRange ) )
		{
			float fItemMapHeight = GetMapHeight( vCenter.x, vCenter.z, pItem );
			if( fTargetDist < fMostDist && pChar->GetMidPositionByRate().y >= fItemMapHeight )
			{
				fMostDist = fTargetDist;
				pTargetItem = pItem;
			}
		}
	}
	return pTargetItem;
}

ioFieldRewardItem* ioPlayStage::GetFieldRewardItemByIndex( int iUniqueIndex )
{
	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldRewardItem *pItem = ToFieldRewardItem( *iter );
		if( pItem && !pItem->IsNeedDestroy() )
		{
			if( pItem->GetRewardUniqueIndex() == iUniqueIndex )
				return pItem;
		}
	}

	return NULL;
}

void ioPlayStage::HidingAllFieldRewardItem()
{
	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldRewardItem *pField = ToFieldRewardItem( *iter );
		if( pField )
		{
			pField->ReserveHiding();
		}
	}
}

void ioPlayStage::HidingFieldRewardItem( int iFieldRewardUniqueIndex )
{
	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldRewardItem *pField = ToFieldRewardItem( *iter );
		if( pField == NULL ) continue;

		if( pField->GetRewardUniqueIndex() == iFieldRewardUniqueIndex )
		{
			pField->ReserveHiding();
			break;
		}
	}
}

ioFieldItem* ioPlayStage::CreateFieldItem( ioItem *pItem, bool bAddList )
{
	ioEntityGroup *pGrp = CreateEntityGrpAndSceneNode( pItem->GetFieldEntityGrp(), true );
	if( !pGrp )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::CreateFieldItem - %s Create Failed",
								pItem->GetFieldEntityGrp().c_str() );
		return NULL;
	}

	ioFieldItem *pField = new ioFieldItem( pGrp, m_pPlayMode, pItem );
	pField->LoadResource();
	pField->SetShadowCastEnable( true );

	if( bAddList )
	{
		AddGameEntity( pField );
	}
	else
	{
		m_ReserveAddEntityList.push_back( pField );
	}

	return pField;
}

D3DXVECTOR3 ioPlayStage::GetItemRandomPosition()
{
	if( m_vItemCreatePosList.empty() )
		return m_vMapCenter;

	D3DXVECTOR3 vPos;
	if( !m_vItemShufflePosList.empty() )
	{
		vPos = m_vItemShufflePosList.front();
		m_vItemShufflePosList.pop_front();
		return vPos;
	}
	
	m_vItemShufflePosList.clear();
	m_vItemShufflePosList.insert( m_vItemShufflePosList.begin(),
								  m_vItemCreatePosList.begin(),
								  m_vItemCreatePosList.end() );

	std::random_shuffle( m_vItemShufflePosList.begin(),
						 m_vItemShufflePosList.end() );

	vPos = m_vItemShufflePosList.front();
	m_vItemShufflePosList.pop_front();
	return vPos;
}

void ioPlayStage::PickFieldItem( const D3DXVECTOR3 &vDropPos,
								 int iPickItemIndex,
								 int iEquipedBullet,
								 float fEquipedGauge,
								 ioBaseChar *pChar )
{
	ioFieldItem *pFieldItem = GetFieldItemByIndex( iPickItemIndex );
	if( !pFieldItem )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::PickFieldItem - %s/%d Not Exist",
								pChar->GetCharName().c_str(),
								iPickItemIndex );
		return;
	}

	pChar->AttachEffect( "sp2_pick_item_spark.txt" );

	ioItem *pPickItem = pFieldItem->ReleaseItem();

	int iSlot = (int)pPickItem->GetType() - 1;
	pChar->GetEquipSlot()->SetDropWaitSlot( iSlot, false );
	pChar->GetEquipSlot()->SetEquipWaitSlot( iSlot, false );

	// 코스튬 관련 (드랍 아이템 장착시 코스튬코드 설정)
	const CHARACTER& rkInfo = pChar->GetCharacterInfo();

	if ( pPickItem && COMPARE( iSlot, 0, MAX_INVENTORY ) )
	{
		pPickItem->SetCostumeCustom( rkInfo.m_EquipCostume[iSlot].m_Costume_Male_Custom, rkInfo.m_EquipCostume[iSlot].m_Costume_Female_Custom );
		pPickItem->SetCostume( rkInfo.m_EquipCostume[iSlot].m_CostumeCode, pChar->IsOwnerChar() );
	}

	ioItem *pPreItem = pChar->EquipItem( pPickItem );
	if( pPreItem )
	{
		pPreItem->SetCurBullet( iEquipedBullet );
		pPreItem->SetCurSkillGauge( fEquipedGauge );
		DropFieldItem( vDropPos, pPreItem, pChar );
	}

	if( pChar->IsOwnerChar() && pPickItem )
	{
		int iDungeonFloor = 0;
		ioMonsterSurvivalMode *pMonsterSurvival = ToMonsterSurvivalMode( m_pPlayMode );
		if( pMonsterSurvival )
			iDungeonFloor = pMonsterSurvival->GetDungeonFloor();
		g_QuestMgr.QuestCompleteTerm( QC_PICK_ITEM, pPickItem->GetItemCode(), iDungeonFloor );
	}

	if( m_pPlayMode )
	{
		m_pPlayMode->NotifyPickItem( pPickItem, pChar );
	}

	if( ToDrinkItem( pPickItem ) )
	{
		pChar->SetDrinkState();
	}
}

ioFieldItem* ioPlayStage::GetFieldItemByIndex( int iPickItemIndex )
{
	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldItem *pItem = ToFieldItem( *iter );
		if( pItem && !pItem->IsNeedDestroy() )
		{
			if( pItem->GetItemCreateIndex() == iPickItemIndex )
				return pItem;
		}
	}

	return NULL;
}

ioFieldItem* ioPlayStage::DropFieldItem( const D3DXVECTOR3 &vItemPos, ioItem *pItem, ioBaseChar *pChar )
{
	ioFieldItem *pNewItem = CreateFieldItem( pItem );
	if( pNewItem )
	{
		pNewItem->SetDropState( vItemPos );
		pNewItem->SetDropChar( pChar->GetCharName() );
	}

	if(m_pPlayMode)
	{
		m_pPlayMode->NotifyDropItem(pItem, pChar);
	}
	
	return pNewItem;
}

ioFieldItem* ioPlayStage::MoveDropFieldItem( const ioHashString &szAttacker, const ioHashString &szSkillName,
											 const D3DXVECTOR3 &vStartPos, const D3DXVECTOR3 &vTargetPos, float fMoveSpeed, ioItem *pItem, ioBaseChar *pChar )
{
	ioFieldItem *pNewItem = CreateFieldItem( pItem );
	if( pNewItem )
	{
		pNewItem->SetMoveDropState( szAttacker, szSkillName, vStartPos, vTargetPos, fMoveSpeed );

		if( pChar )
			pNewItem->SetDropChar( pChar->GetCharName() );
	}

	if(m_pPlayMode)
	{
		m_pPlayMode->NotifyDropItem(pItem, pChar);
	}

	return pNewItem;
}

D3DXVECTOR3 ioPlayStage::CheckDropItemPos( const D3DXVECTOR3 &vDropPos, ioBaseChar *pChar )
{
	D3DXVECTOR3 vCharPos = pChar->GetMidPositionByRate();
	vCharPos.y = vDropPos.y;

	if( ioMath::IsEqual( vCharPos, vDropPos ) )
		return vDropPos;

	D3DXVECTOR3 vPosDiff = vDropPos - vCharPos;
	float fDropLength = D3DXVec3Length( &vPosDiff );

	D3DXVECTOR3 vDropDir;
	D3DXVec3Normalize( &vDropDir, &vPosDiff );

	ioOpcodeShape *pShape = m_pSceneMgr->GetCollisionShape();

	// 앞 방향 Entity 체크
	ioSegment kFrontSegment( vCharPos, vDropPos );
	if( !ioMapCollisionHelper::CheckMountAvailableEntityVsSegment( m_MountAvailableList, kFrontSegment ) )
	{
		ioRay kFrontRay( vCharPos, vDropDir );

		if( m_pBlockWorld && m_pBlockWorld->CheckCollRayVsBlockByRange( kFrontRay, fDropLength ) )
		{
			// 앞쪽에 걸림
			return vCharPos;
		}

		if( !ioMapCollisionHelper::CheckRayVsMapCollisionByRange( kFrontRay, pShape, NULL, fDropLength ) )
		{
			// 앞쪽에 걸리는것 전혀 없음
			return vDropPos;
		}
	}

	D3DXVECTOR3 vBackDropPos = vCharPos - vDropDir * fDropLength;

	// 뒷 방향 Entity 체크
	ioSegment kBackSegment( vCharPos, vBackDropPos );
	if( !ioMapCollisionHelper::CheckMountAvailableEntityVsSegment( m_MountAvailableList, kBackSegment ) )
	{
		ioRay kBackRay( vCharPos, -vDropDir );

		if( m_pBlockWorld && m_pBlockWorld->CheckCollRayVsBlockByRange( kBackRay, fDropLength ) )
		{
			// 뒤쪽에 걸림
			return vCharPos;
		}
				
		if( !ioMapCollisionHelper::CheckRayVsMapCollisionByRange( kBackRay, pShape, NULL, fDropLength ) )
		{
			// 뒤쪽에 걸리는것 전혀 없음
			return vBackDropPos;
		}
	}

	// 앞/뒤 모두 걸림
	return vCharPos;
}

ioFieldItem* ioPlayStage::FindUserPickFieldItem( ioBaseChar *pChar )
{
	if( !pChar )
		return NULL;
	ioFieldItem *pTargetItem = NULL;
	float fMostDist = std::numeric_limits<float>::infinity();

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldItem *pItem = ToFieldItem( *iter );
		if( !pItem || !pItem->IsCanPicked() )
			continue;
		if( !pItem->IsFlagItem() && ( pChar->GetObject() || pChar->IsObejctEquipState() ) )
			continue;

		if( pChar )
		{
			int iOnlySexType = pItem->GetCharOnlySexType();
			if( pChar->IsMale() && iOnlySexType == ioSetItemInfo::CST_FEMALE )
				continue;
			else if( !pChar->IsMale() && iOnlySexType == ioSetItemInfo::CST_MALE )
				continue;

			TeamType eTeamType = pItem->GetItemTeamType();

			if( eTeamType != TEAM_NONE )
			{
				if( pChar->GetTeam() != eTeamType )
					continue;
			}
		}

		float fTargetDist = -FLOAT1;
		D3DXVECTOR3 vCenter = pItem->GetWorldCollisionBox().GetCenter();
		if( !pChar->CheckSphereDistance( vCenter, m_fPickItemRange, fTargetDist ) )
			continue;

		if( COMPARE( fTargetDist, 0.0f, m_fPickItemRange ) )
		{
			if( pItem->GetCrownItemType() != ioItem::MCT_NONE )
				return pItem;

			float fItemMapHeight = GetMapHeight( vCenter.x, vCenter.z, pItem );
			if( fTargetDist < fMostDist && pChar->GetMidPositionByRate().y >= fItemMapHeight )
			{
				fMostDist = fTargetDist;
				pTargetItem = pItem;
			}
		}
	}

	return pTargetItem;
}

int ioPlayStage::FindUserPushStruct( ioBaseChar *pChar )
{
	if( m_pPushStructMgr )
		return m_pPushStructMgr->FindUserPushStruct( pChar );

	return -1;
}

ioMapEffect* ioPlayStage::CreateMapEffect( const ioHashString &rkFileName,
										   const D3DXVECTOR3 &vScale )
{
	if( rkFileName.IsEmpty() )	return NULL;

	if( ioEffectFactory::GetSingletonPtr() == NULL )
		return NULL;

	ioEffect *pEffect = g_EffectFactory.CreateEffect( rkFileName );
	if( !pEffect )	return NULL;

	ioMapEffect *pMapEffect = new ioMapEffect( pEffect );
	if( pMapEffect )
	{
		if( m_pSceneMgr == NULL )
			return NULL;

		ioSceneNode *pSNode = m_pSceneMgr->GetRootSceneNode()->CreateChildSceneNode();
		pSNode->AttachObject( pEffect );
		pSNode->SetScale( vScale );

		m_EffectList.push_back( pMapEffect );
	}

	return pMapEffect;
}

ioMapEffect* ioPlayStage::CreateMapEffect( const ioHashString &rkFileName,
										   const D3DXVECTOR3 &vPos,
										   const D3DXVECTOR3 &vScale )
{
	ioMapEffect *pEffect = CreateMapEffect( rkFileName, vScale );
	if( pEffect )
	{
		pEffect->SetWorldPosition( vPos );
	}

	return pEffect;
}

ioBaseChar* ioPlayStage::FindUser( FindPredicate &rkPred )
{
	return rkPred.GetFindChar( m_BaseCharList );
}

bool ioPlayStage::FindEntityList( FindListPredicate &rkPred )
{
	return rkPred.FindList( m_GameEntityList, this );
}

ioBaseChar* ioPlayStage::GetBaseChar( const ioHashString &szCharName )
{
	if( szCharName.IsEmpty() )
		return NULL;

	BaseCharList::iterator iter=m_BaseCharList.begin();
	for( ; iter!=m_BaseCharList.end() ; ++iter )
	{
		if( (*iter) == NULL )
			continue;

		if( (*iter)->GetCharName() == szCharName )
			return *iter;
	}

	return NULL;
}

ioBaseChar* ioPlayStage::GetBaseChar( DWORD dwCharIndex )
{
	BaseCharList::iterator iter=m_BaseCharList.begin();
	for( ; iter!=m_BaseCharList.end() ; ++iter )
	{
		if( (*iter)->GetIndex() == dwCharIndex )
			return *iter;
	}

	return NULL;
}

ioBaseChar* ioPlayStage::GetBaseCharFirstTeam( TeamType eTeam )
{
	BaseCharList::iterator iter=m_BaseCharList.begin();
	for( ; iter!=m_BaseCharList.end() ; ++iter )
	{
		if( (*iter)->GetTeam() == eTeam )
			return *iter;
	}
	return NULL;
}

bool ioPlayStage::IsBaseChar( const ioHashString &szCharName )
{
	ioBaseChar *pChar = GetBaseChar( szCharName );
	if( pChar )
		return true;
	return false;
}

TeamType ioPlayStage::GetBaseCharTeam( const ioHashString &szCharName )
{
	ioBaseChar *pChar = GetBaseChar( szCharName );
	if( !pChar )
		return TEAM_NONE;
	return pChar->GetTeam();
}

void ioPlayStage::ChangeBaseCharName( const ioHashString &rszCharName, const ioHashString &rszNewCharName )
{
	ioBaseChar *pChar = GetBaseChar( rszCharName );
	if( pChar )
		pChar->SetCharName( rszNewCharName );
}

ioBaseChar* ioPlayStage::GetOwnerChar()
{
	if( P2PNetwork::IsNetworkPlaying() )
	{
		BaseCharList::iterator iter=m_BaseCharList.begin();
		for( ; iter!=m_BaseCharList.end() ; ++iter )
		{
			if( (*iter)->IsOwnerChar() )
				return *iter;
		}

		return NULL;
	}

#ifndef _SHIPPING
	SingleSettingWnd* pWnd = dynamic_cast<SingleSettingWnd*>( g_GUIMgr.FindWnd( SINGLE_SETTING_WND ) );
	if( pWnd && pWnd->IsShow() && !pWnd->GetOwnerChar().IsEmpty() )
	{
		return GetBaseChar( pWnd->GetOwnerChar() );
	}
#endif

	static ioHashString sName( "character1" );
	return GetBaseChar( sName );
}

const ioHashString& ioPlayStage::GetOwnerName()
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
		return pOwner->GetCharName();

	LOG.PrintTimeAndLog( 0, "ioPlayStage::GetOwnerName() - OwnerChar is not Exist" );

	static ioHashString szEmpty;
	return szEmpty;
}

TeamType ioPlayStage::GetOwnerTeam()
{
	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
		return pOwner->GetTeam();
	return TEAM_NONE;
}

ioAxisAlignBox ioPlayStage::GetWorldMapBox() const
{
	if( m_pSceneMgr )
		return m_pSceneMgr->GetWorldMapBox();

	return ioAxisAlignBox();
}

float ioPlayStage::GetMapHeight( float fXPos,
								 float fZPos,
								 const ioGameEntity *pEntity,
								 bool bMidRay,
								 float fDefaultRayHeight,
								 bool bIgonreBlockWorld,
								 bool bIgonreMountObject )
{
	if( !pEntity )
	{
		return GetMapHeightNoneEntity( fXPos, fZPos, fDefaultRayHeight, bIgonreBlockWorld, bIgonreMountObject );
	}
	else if( ToBaseCharConst(pEntity) && ToBaseCharConst(pEntity)->GetState() == CS_FALL )
	{
		return GetMapHeightByRay( fXPos, fZPos, pEntity, bMidRay, fDefaultRayHeight );
	}

	ioAxisAlignBox kAxisBox = pEntity->GetWorldAxisBox();
	ioOrientBox kOrientBox  = pEntity->GetWorldCollisionBox();
	D3DXVECTOR3 vMin, vMax;

	if( !ToPushStructConst(pEntity) )
	{
		if( bMidRay )
		{
			vMin = kAxisBox.GetMinPos();
			vMin.y = 0.0f;

			vMax = kAxisBox.GetMaxPos();
			vMax.y = kAxisBox.GetCenter().y;
		}
		else
		{
			vMin.x = fXPos - kAxisBox.GetExtents( 0 );
			vMin.z = fZPos - kAxisBox.GetExtents( 2 );
			vMin.y = 0.0f;

			vMax.x = fXPos + kAxisBox.GetExtents( 0 );			
			vMax.z = fZPos + kAxisBox.GetExtents( 2 );

			if( fDefaultRayHeight == 0.0f )
				vMax.y = GetBoundHeight();
			else
				vMax.y = fDefaultRayHeight;
		}

		kAxisBox.SetMinMaxPos( vMin, vMax );
	}
	else
	{
		kOrientBox.SetCenter(kAxisBox.GetCenter() );

		D3DXVECTOR3 vVtxArray[8];
		kOrientBox.GetVertices( vVtxArray );

		vMin = vVtxArray[0];
		vMax = vVtxArray[6];

		if( bMidRay )
		{
			vMin.y = 0.0f;
			vMax.y = kOrientBox.GetCenter().y;
		}
		else
		{
			vMin.y = 0.0f;

			if( fDefaultRayHeight == 0.0f )
				vMax.y = GetBoundHeight();
			else
				vMax.y = fDefaultRayHeight;
		}

		kOrientBox.SetBoxMinMaxForAxis( vMin, vMax );
	}

	float fBoxMaxY = vMax.y;
	float fCurYPos = -FLOAT1;

	MountAvailableList::iterator iter = m_MountAvailableList.begin();
	for( ; iter!=m_MountAvailableList.end() ; ++iter )
	{
		ioGameEntity *pCurMount = *iter;
		if( pCurMount == pEntity )
			continue;

		if( !pCurMount->IsNowMountEnable() )
			continue;

		const ioBaseChar* pChar = ToBaseCharConst( pEntity );
		ioDummyChar* pDummy = ToDummyChar( pCurMount );
		if( pDummy && pDummy->IsDummyCharCollisionSkipState( pChar ) )
			continue;

		if( pChar && pDummy && pDummy->GetMountEnable() )
		{
			// 멘티스 : 0001054 프레데터 탑승한 상태로 탐사 시작 시 이상현상 (오브젝트아이템 해제)
			if( pChar->GetState() == CS_FISHING || pChar->GetState() == CS_ROULETTE || pChar->GetState() == CS_CONSTRUCT_HOUSE )
				continue;
		}

		ioForkcraneDummy* pForkcrane = ToForkcraneDummy( pDummy );
		ioPushStruct* pPushStruct = ToPushStruct( pCurMount );
		if( bIgonreMountObject && ( pForkcrane || ( pPushStruct && pPushStruct->GetStructType() == PSTT_FISHING_GROUND ) ) )
			break;

		ioOrientBox kColBox = pCurMount->GetWorldCollisionBox();
		kColBox.SetCenter( pCurMount->GetWorldAxisBox().GetCenter() );

		if( !ToPushStructConst(pEntity) )
		{
			kOrientBox.SetByAxisBox( kAxisBox );
		}
		else
		{
			D3DXVECTOR3 vVtxArray[8];
			kColBox.GetVertices( vVtxArray );

			vMin = vVtxArray[0];
			vMax = vVtxArray[6];
		}

		if( ioMath::TestIntersection( kOrientBox, kColBox, NULL ) )
		{
			float fYHeight = kColBox.GetCenter().y + kColBox.GetExtents(1);
			if( fYHeight < fBoxMaxY )
			{
				fCurYPos = max( fCurYPos, fYHeight + FLOAT1 );
			}
		}
	}

	float fMapHeight = m_pSceneMgr->GetHeight( fXPos, fBoxMaxY, fZPos );

	if( m_pBlockWorld && !bIgonreBlockWorld )
	{
		float fHeight = m_pBlockWorld->GetHeight( fXPos, fBoxMaxY, fZPos );
		if( fMapHeight < fHeight )
		{
			if( fCurYPos > -FLOAT1 && fHeight < fCurYPos )
				return fCurYPos;

			return fHeight;
		}
	}

	if( fCurYPos > -FLOAT1 && fMapHeight < fCurYPos )
	{
		return fCurYPos;
	}

	return fMapHeight;
}

float ioPlayStage::GetMapHeightByRevial( float fXPos, float fZPos, const ioGameEntity *pEntity, bool bMidRay, float fDefaultRayHeight )
{
	float fBoundY = 0.0f;
	float fMapHeight = 0.0f;

	if( fDefaultRayHeight != 0.0f )
	{
		fBoundY = fDefaultRayHeight;
		fMapHeight = m_pSceneMgr->GetHeight( fXPos, fDefaultRayHeight, fZPos );

		if( m_pBlockWorld )
		{
			float fHeight = m_pBlockWorld->GetHeight( fXPos, fDefaultRayHeight, fZPos );
			if( fMapHeight < fHeight )
				fMapHeight = fHeight;
		}
	}
	else
	{
		fBoundY = GetBoundHeight();
		fMapHeight = m_pSceneMgr->GetHeight( fXPos, fZPos );

		if( m_pBlockWorld )
		{
			float fHeight = m_pBlockWorld->GetHeight( fXPos, fZPos );
			if( fMapHeight < fHeight )
				fMapHeight = fHeight;
		}
	}

	D3DXVECTOR3 vRayDir( 0.0f, -FLOAT1, 0.0f );
	D3DXVECTOR3 vRayOrig( fXPos, fBoundY, fZPos );
	ioRay kYRay( vRayOrig, vRayDir );
	float fCurYPos = -FLOAT1;

	MountAvailableList::iterator iter = m_MountAvailableList.begin();
	for( ; iter!=m_MountAvailableList.end() ; ++iter )
	{
		const ioGameEntity *pCurMount = *iter;
		if( !pCurMount->IsNowMountEnable() )
			continue;

		const ioDummyChar* pDummy = ToDummyCharConst( pCurMount );
		if( pDummy && pDummy->GetMountEnable() )
			continue;

		ioOrientBox kColBox = pCurMount->GetWorldCollisionBox();
		kColBox.SetCenter( pCurMount->GetWorldAxisBox().GetCenter() );
		if( ioMath::TestIntersection( kYRay, kColBox ) )
		{
			float fYHeight = kColBox.GetCenter().y + kColBox.GetExtents(1);
			fCurYPos = max( fCurYPos, fYHeight + FLOAT1 );
		}
	}

	if( fCurYPos > -FLOAT1 && fMapHeight < fCurYPos )
		return fCurYPos;

	return fMapHeight;
}

float ioPlayStage::GetMapHeightOnlyTerrain( float fXPos, float fZPos, float fDefaultRayHeight )
{
	float fMapHeight = 0.0f;

	if( fDefaultRayHeight != 0.0f )
	{
		fMapHeight = m_pSceneMgr->GetHeight( fXPos, fDefaultRayHeight, fZPos );

		if( m_pBlockWorld )
		{
			float fHeight = m_pBlockWorld->GetHeight( fXPos, fDefaultRayHeight, fZPos );
			if( fMapHeight < fHeight )
				fMapHeight = fHeight;
		}
	}
	else
	{
		fMapHeight = m_pSceneMgr->GetHeight( fXPos, fZPos );

		if( m_pBlockWorld )
		{
			float fHeight = m_pBlockWorld->GetHeight( fXPos, fZPos );
			if( fMapHeight < fHeight )
				fMapHeight = fHeight;
		}
	}

	return fMapHeight;
}

float ioPlayStage::GetMapHeightNoneEntity( float fXPos, float fZPos, float fDefaultRayHeight, bool bIgonreBlockWorld, bool bIgonreMountObject )
{
	float fBoundY = 0.0f;
	float fMapHeight = 0.0f;

	if( fDefaultRayHeight != 0.0f )
	{
		fBoundY = fDefaultRayHeight;
		fMapHeight = m_pSceneMgr->GetHeight( fXPos, fDefaultRayHeight, fZPos );

		if( m_pBlockWorld && !bIgonreBlockWorld )
		{
			float fHeight = m_pBlockWorld->GetHeight( fXPos, fDefaultRayHeight, fZPos );
			if( fMapHeight < fHeight )
				fMapHeight = fHeight;
		}
	}
	else
	{
		fBoundY = GetBoundHeight();
		fMapHeight = m_pSceneMgr->GetHeight( fXPos, fZPos );

		if( m_pBlockWorld && !bIgonreBlockWorld )
		{
			float fHeight = m_pBlockWorld->GetHeight( fXPos, fZPos );
			if( fMapHeight < fHeight )
				fMapHeight = fHeight;
		}
	}

	D3DXVECTOR3 vRayDir( 0.0f, -FLOAT1, 0.0f );
	D3DXVECTOR3 vRayOrig( fXPos, fBoundY, fZPos );
	ioRay kYRay( vRayOrig, vRayDir );
	float fCurYPos = -FLOAT1;

	MountAvailableList::iterator iter = m_MountAvailableList.begin();
	for( ; iter!=m_MountAvailableList.end() ; ++iter )
	{
		const ioGameEntity *pCurMount = *iter;
		if( !pCurMount->IsNowMountEnable() )
			continue;

		// 포크레인 무시
		ioDummyChar* pDummy = ToDummyChar( *iter );
		ioForkcraneDummy* pForkcrane = ToForkcraneDummy( pDummy );
		ioPushStruct* pPushStruct = ToPushStruct( *iter );
		if( bIgonreMountObject && ( pForkcrane || ( pPushStruct && pPushStruct->GetStructType() == PSTT_FISHING_GROUND ) ) )
			continue;

		ioOrientBox kColBox = pCurMount->GetWorldCollisionBox();
		kColBox.SetCenter( pCurMount->GetWorldAxisBox().GetCenter() );
		if( ioMath::TestIntersection( kYRay, kColBox ) )
		{
			float fYHeight = kColBox.GetCenter().y + kColBox.GetExtents(1);
			fCurYPos = max( fCurYPos, fYHeight + FLOAT1 );
		}
	}

	if( fCurYPos > -FLOAT1 && fMapHeight < fCurYPos )
		return fCurYPos;

	return fMapHeight;
}

float ioPlayStage::GetMapHeightByRay( float fXPos,
									  float fZPos,
									  const ioGameEntity *pEntity,
									  bool bMidRay,
									  float fDefaultRayHeight )
{
	float fBoundY = 0.0f;
	float fMapHeight = 0.0f;

	if( bMidRay )
	{
		fBoundY = pEntity->GetWorldAxisBox().GetCenter().y;
	}
	else
	{
		if( fDefaultRayHeight == 0.0f )
			fBoundY = GetBoundHeight();
		else
			fBoundY = fDefaultRayHeight;
	}
		
	fMapHeight = m_pSceneMgr->GetHeight( fXPos, fBoundY, fZPos );

	if( m_pBlockWorld )
	{
		float fHeight = m_pBlockWorld->GetHeight( fXPos, fBoundY, fZPos );
		if( fMapHeight < fHeight )
			fMapHeight = fHeight;
	}

	D3DXVECTOR3 vRayDir( 0.0f, -FLOAT1, 0.0f );
	D3DXVECTOR3 vRayOrig( fXPos, fBoundY, fZPos );
	ioRay kYRay( vRayOrig, vRayDir );
	float fCurYPos = -FLOAT1;

	MountAvailableList::iterator iter = m_MountAvailableList.begin();
	for( ; iter!=m_MountAvailableList.end() ; ++iter )
	{
		const ioGameEntity *pCurMount = *iter;
		if( pCurMount == pEntity )	continue;
		if( !pCurMount->IsNowMountEnable() )
			continue;
		const ioDummyChar* pDummy = ToDummyCharConst( pCurMount );
		const ioBaseChar* pBaseChar = ToBaseCharConst( pEntity );
		if( pDummy && pBaseChar && const_cast<ioDummyChar*>(pDummy)->IsDummyCharCollisionSkipState( pBaseChar ) )
			continue;

		ioOrientBox kColBox = pCurMount->GetWorldCollisionBox();
		kColBox.SetCenter( pCurMount->GetWorldAxisBox().GetCenter() );
		if( ioMath::TestIntersection( kYRay, kColBox ) )
		{
			float fYHeight = kColBox.GetCenter().y + kColBox.GetExtents(1);
			fCurYPos = max( fCurYPos, fYHeight + FLOAT1 );
		}
	}

	if( fCurYPos > -FLOAT1 && fMapHeight < fCurYPos )
		return fCurYPos;

	return fMapHeight;
}

float ioPlayStage::GetMapHeightByTargetMid( float fXPos, float fZPos, float fBaseY, float fMidY )
{
	float fMapHeight = 0.0f;

	float fMidMapY = GetMapHeightNoneEntity( fXPos, fZPos, fMidY );
	float fHighMapY = GetMapHeightNoneEntity( fXPos, fZPos );

	if( fMidMapY > FLOAT1 )			// 중간점에서 쏜 레이가 드랍존이 아닐때.
	{
		if( fMidMapY < fBaseY )		// 기준점 보다 아래에 맵이 있으면 기준점에.
			fMapHeight = fBaseY;
		else						// 기준점 보다 위에 맵이 있으면 맵 지점에.
			fMapHeight = fMidMapY;
	}
	else if( fHighMapY > FLOAT1 )		// 중간점이 드랍존이고, 일반맵 지점이 드랍존이 아닐때.
	{
		fMapHeight = fHighMapY;
	}
	else							// 나머지 상황은 모두 기준점에.
		fMapHeight = fBaseY;

	return fMapHeight;
}

float ioPlayStage::GetBoundHeight()
{
	if( m_pSceneMgr )
		return m_pSceneMgr->GetBoundHeight() * 1.5f;

	return 0.0f;
}

void ioPlayStage::RenderSceneNodeBox()
{
	if( !m_pSceneMgr )	return;

	ioSceneNode *pRoot = m_pSceneMgr->GetRootSceneNode();

	int iNumChild = pRoot->GetNumChildren();
	for( int i=0 ; i<iNumChild ; i++ )
	{
		ioSceneNode *pChild = dynamic_cast<ioSceneNode*>( pRoot->GetChildByIndex(i) );
		if( pChild )
		{
			RenderSceneNode( pChild );
		}
	}
}

void ioPlayStage::RenderSceneNode( ioSceneNode *pNode )
{
	if( !pNode )	return;

	D3DXVECTOR3 vVtxArray[8];
	const ioAxisAlignBox &rkAxisBox = pNode->GetWorldAABB();
	rkAxisBox.GetVertices( vVtxArray );
	RenderBox( vVtxArray, 0xff00ff00 );

	int iNumChild = pNode->GetNumChildren();
	for( int i=0 ; i<iNumChild ; i++ )
	{
		ioSceneNode *pChild = dynamic_cast<ioSceneNode*>( pNode->GetChildByIndex(i) );
		if( pChild )
		{
			RenderSceneNode( pChild );
		}
	}
}

void ioPlayStage::RenderCharBox()
{
	D3DXVECTOR3 vVtxArray[8];	
	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioPlayEntity *pEntity = ToPlayEntity( *iter );
		if( !pEntity )	continue;

		const ioAxisAlignBox &rkAxisBox = pEntity->GetWorldAxisBox();
		rkAxisBox.GetVertices( vVtxArray );
		RenderBox( vVtxArray, 0xff000000 );

		const ioOrientBox &rkOrientBox = pEntity->GetWorldCollisionBox();
		rkOrientBox.GetVertices( vVtxArray );
		RenderBox( vVtxArray, 0xff0000ff );

		ioBaseChar *pChar = ToBaseChar( *iter );
		if( pChar )
		{
			int iWoundedBoxCnt = pChar->GetWoundedBoxCount();
			for( int i=0 ; i<iWoundedBoxCnt ; i++ )
			{
				pChar->GetWoundedBoxVertices( i, vVtxArray );
				RenderBox( vVtxArray, 0xffffff00 );
			}
		}
	}

#ifndef SHIPPING
	RenderBox( ioBaseChar::m_vRangeBoxArrayForTest,  0x00FF0000 );
#endif
}

#ifndef SHIPPING
void ioPlayStage::RenderModeEntity()
{
	if( m_pPlayMode )
		m_pPlayMode->RenderModeEntity();
}
#endif

void ioPlayStage::RenderWeaponBox()
{
	ioWeaponManager::WeaponIterator kWeaponIter = g_WeaponMgr.GetWeaponIterator();
	std::vector< ioOrientBox > vAttackBoxList;
	vAttackBoxList.reserve( 3 );

	D3DXVECTOR3 vVtxArray[8];
	while( kWeaponIter.HasMoreElements() )
	{
		ioWeapon *pWeapon = kWeaponIter.Next();

		vAttackBoxList.erase( vAttackBoxList.begin(), vAttackBoxList.end() );
		pWeapon->GetWorldAttackBoxList( vAttackBoxList );

		int iNumBoxCount = vAttackBoxList.size();
		for( int i=0 ; i<iNumBoxCount ; i++ )
		{
			const ioOrientBox &rkBox = vAttackBoxList[i];
			if( !rkBox.IsNull() )
			{
				rkBox.GetVertices( vVtxArray );
				RenderBox( vVtxArray, 0xffffffff );				
			}
		}
	}
}

void ioPlayStage::RenderBox( const D3DXVECTOR3 *pVertices, DWORD dwLineColor )
{
	static WORD wBoxIndex[24] = { 0,1, 1,2, 2,3, 3,0, 0,4, 1,5, 2,6, 3,7, 4,5, 5,6, 6,7, 7,4 };

	ioRenderSystem &rkSystem = RenderSystem();
	rkSystem.SetModelMatrixIdentity();

	rkSystem.SetFixedFVF( D3DFVF_XYZ );
	rkSystem.DisableShader();

	rkSystem.SetCullingMode( CM_NO );
	rkSystem.SetDepthFunction( D3DCMP_LESSEQUAL, true, true );
	rkSystem.DisableAlphaBlend();
	rkSystem.DisableAlphaTest();

	rkSystem.SetTextureFactor( dwLineColor );
	rkSystem.SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TFACTOR, D3DTA_DIFFUSE );
	rkSystem.SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TFACTOR, D3DTA_DIFFUSE );
	rkSystem.DisableTextureUnitsFrom( 1 );
	rkSystem.ClearStreamDesc();

	rkSystem.GetDevice()->DrawIndexedPrimitiveUP( D3DPT_LINELIST, 0, 8, 12, wBoxIndex, D3DFMT_INDEX16, pVertices, sizeof(D3DXVECTOR3) );
}

void ioPlayStage::RenderGrid( const D3DXVECTOR3 *pVtxArray, DWORD dwLineColor )
{
	static WORD wGridIndex[8] = { 0,1, 1,2, 2,3, 3,0 };

	ioRenderSystem &rkSystem = RenderSystem();
	rkSystem.SetModelMatrixIdentity();

	rkSystem.SetFixedFVF( D3DFVF_XYZ );
	rkSystem.DisableShader();

	rkSystem.SetCullingMode( CM_NO );
	rkSystem.SetDepthFunction( D3DCMP_LESSEQUAL, true, true );
	rkSystem.DisableAlphaBlend();
	rkSystem.DisableAlphaTest();

	rkSystem.SetTextureFactor( dwLineColor );
	rkSystem.SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TFACTOR, D3DTA_DIFFUSE );
	rkSystem.SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TFACTOR, D3DTA_DIFFUSE );
	rkSystem.DisableTextureUnitsFrom( 1 );
	rkSystem.ClearStreamDesc();

	rkSystem.GetDevice()->DrawIndexedPrimitiveUP( D3DPT_LINELIST, 0, 4, 2, wGridIndex, D3DFMT_INDEX16, pVtxArray, sizeof(D3DXVECTOR3) );
}

void ioPlayStage::RenderLine( const D3DXVECTOR3 *pVertices, DWORD dwLineColor )
{
	static WORD wBoxIndex[2] = { 0, 1 };

	ioRenderSystem &rkSystem = RenderSystem();
	rkSystem.SetModelMatrixIdentity();

	rkSystem.SetFixedFVF( D3DFVF_XYZ );
	rkSystem.DisableShader();

	rkSystem.SetCullingMode( CM_NO );
	rkSystem.SetDepthFunction( D3DCMP_LESSEQUAL, true, true );
	rkSystem.DisableAlphaBlend();
	rkSystem.DisableAlphaTest();

	rkSystem.SetTextureFactor( dwLineColor );
	rkSystem.SetTextureStageColorBlend( 0, D3DTOP_SELECTARG1, D3DTA_TFACTOR, D3DTA_DIFFUSE );
	rkSystem.SetTextureStageAlphaBlend( 0, D3DTOP_SELECTARG1, D3DTA_TFACTOR, D3DTA_DIFFUSE );
	rkSystem.DisableTextureUnitsFrom( 1 );
	rkSystem.ClearStreamDesc();

	rkSystem.GetDevice()->DrawIndexedPrimitiveUP( D3DPT_LINELIST, 0, 2, 2, wBoxIndex, D3DFMT_INDEX16, pVertices, sizeof(D3DXVECTOR3) );
}

void ioPlayStage::RenderEntitySphere()
{
	RenderSystem().SetModelMatrixIdentity();

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioPlayEntity *pEntity = ToPlayEntity( *iter );
		if( !pEntity )	
			continue;

		RenderSphere( pEntity->GetWorldCollisionBox().GetBoxRadius(), pEntity->GetWorldOrientation(), pEntity->GetMidPositionByRate(), pEntity->GetWorldScale(), D3DCOLOR_XRGB( 0, 0, 0 ) );
	}
}

void ioPlayStage::RenderWeaponSphere()
{
	ioWeaponManager::WeaponIterator kWeaponIter = g_WeaponMgr.GetWeaponIterator();
	std::vector< ioOrientBox > vAttackBoxList;
	vAttackBoxList.reserve( 3 );

	D3DXVECTOR3 vVtxArray[8];
	while( kWeaponIter.HasMoreElements() )
	{
		ioWeapon *pWeapon = kWeaponIter.Next();

		vAttackBoxList.erase( vAttackBoxList.begin(), vAttackBoxList.end() );
		pWeapon->GetWorldAttackBoxList( vAttackBoxList );

		int iNumBoxCount = vAttackBoxList.size();
		for( int i=0 ; i<iNumBoxCount ; i++ )
		{
			const ioOrientBox &rkBox = vAttackBoxList[i];
			if( !rkBox.IsNull() )
			{
				rkBox.GetVertices( vVtxArray );
				RenderSphere( rkBox.GetBoxRadius(), pWeapon->GetOrientation(), rkBox.GetCenter(), pWeapon->GetScale(), D3DCOLOR_XRGB( 0, 0, 0 ) );
			}
		}
	}
}

void ioPlayStage::RenderSphere( float fRadius, const D3DXQUATERNION &qtOrientaton, const D3DXVECTOR3 &vPosition, const D3DXVECTOR3 &vScale, DWORD dwLineColor )
{
	ioRenderSystem &rkSystem = RenderSystem();
	rkSystem.SetModelMatrixIdentity();

	if ( fRadius <= 0.0f )
		return;

	rkSystem.SetFixedFVF( D3DFVF_XYZ );
	rkSystem.DisableShader();

	rkSystem.SetCullingMode( CM_CCW );
	rkSystem.SetDepthFunction( D3DCMP_LESSEQUAL, false, false );
	rkSystem.DisableAlphaBlend();
	rkSystem.DisableAlphaTest();
	
	rkSystem.DisableTextureUnitsFrom( 0 );
	rkSystem.ClearStreamDesc();

	LPD3DXMESH pTempMesh = NULL;
	D3DXCreateSphere( rkSystem.GetDevice(), fRadius, 20, 20, &pTempMesh, NULL);

	D3DXMATRIX matDest;
	ioMath::MakeTransform( qtOrientaton, vPosition, vScale, matDest );	
	rkSystem.SetModelMatrix( &matDest );

	rkSystem.GetDevice()->SetRenderState( D3DRS_LIGHTING, TRUE );
	rkSystem.GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
	rkSystem.GetDevice()->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	rkSystem.GetDevice()->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
	
	D3DMATERIAL9 mat;
	ZeroMemory( &mat, sizeof(D3DMATERIAL9) );
	mat.Diffuse = D3DXCOLOR( dwLineColor );
	mat.Diffuse.a = 0.2f;
	mat.Ambient = D3DXCOLOR( dwLineColor );

	rkSystem.GetDevice()->SetMaterial( &mat );
	rkSystem.GetDevice()->SetTexture( 0, 0 ); // disable texture
	pTempMesh->DrawSubset( 0 );

	pTempMesh->Release();

	rkSystem.GetDevice()->SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	rkSystem.GetDevice()->SetRenderState( D3DRS_LIGHTING, FALSE );
	rkSystem.SetCullingMode( CM_NO );
}

void ioPlayStage::ChangeCharModel( ioBaseChar *pChar, const ioHashString &rkName, DWORD dwCharIndex, SP2Packet &rkPacket, bool bFastSync, byte eAwakeType )
{
	CHARACTER kCharInfo;
	rkPacket >> kCharInfo.m_class_type >> kCharInfo.m_kindred >> kCharInfo.m_sex;
	
	int iSelArray;
	rkPacket >> iSelArray;    

	bool bMovieCapturing;
	rkPacket >> bMovieCapturing;
	
	if( bFastSync )
		return;

	kCharInfo = g_GlobalSoldierInfo.GetSoldierData( rkName, dwCharIndex, kCharInfo.m_class_type, kCharInfo.m_kindred, kCharInfo.m_sex );
	kCharInfo.m_iAwakeType = eAwakeType;

	ioINILoader &rkLoader = Help::GetCharLoader( kCharInfo.m_kindred, kCharInfo.m_sex );
	pChar->InitINIValue( rkLoader );
	pChar->InitRaceInfo( kCharInfo );
	pChar->SetDelayCameraBuff( true );	
	pChar->SetSelectCharArray( iSelArray );

	if( pChar->GetCharName() != g_MyInfo.GetPublicID() )
	{
		if( bMovieCapturing )
		{
			// 동영상 촬영 이모티콘 추가
			pChar->SetMovieStartEmoticon( 0 );       // 0이면 지속 이모티콘
		}
	}
}

ioBaseChar* ioPlayStage::ChangeCharModel( const ioHashString &rkName, DWORD dwCharIndex, TeamType eTeam, SP2Packet &rkPacket, byte eAwakeType )
{
	CHARACTER kCharInfo;
	rkPacket >> kCharInfo.m_class_type >> kCharInfo.m_kindred >> kCharInfo.m_sex;

	int iSelArray;
	rkPacket >> iSelArray;

	bool bMovieCapturing;
	rkPacket >> bMovieCapturing;

	kCharInfo = g_GlobalSoldierInfo.GetSoldierData( rkName, dwCharIndex, kCharInfo.m_class_type, kCharInfo.m_kindred, kCharInfo.m_sex );
	kCharInfo.m_iAwakeType = eAwakeType;

	ioINILoader &rkLoader = Help::GetCharLoader( kCharInfo.m_kindred, kCharInfo.m_sex );

	char szFileName[MAX_PATH];
	rkLoader.LoadString_e( "model_file", "", szFileName, MAX_PATH );

	ioBaseChar *pChar = CreateBaseChar( szFileName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::ChangeCharModel Failed" );
		return NULL;
	}

	pChar->SetCharName( rkName );
	pChar->InitINIValue( rkLoader );
	pChar->InitRaceInfo( kCharInfo );
	pChar->SetState( CS_DELAY );
	pChar->SetDelayCameraBuff( true );
	pChar->SetSelectCharArray(iSelArray);

	if( rkName == g_MyInfo.GetPublicID() )
	{
		ioUserKeyInput::UseKeyValue kKeys;
		kKeys.SetDefaultFirstKey();
		ioUserKeyInput::UseKeyValue kSecondKeys;
		kSecondKeys.SetDefaultSecondKey();
		ioUserKeyInput::UseKeyValue kJoyKeys;
		kJoyKeys.SetDefaultJoyKey();

		g_KeyManager.GetGameKey(kKeys);
		g_KeyManager.GetSecondGameKey(kSecondKeys);
		g_KeyManager.GetJoyGameKey(kJoyKeys);

		pChar->SetUpKeyValue( kKeys );
		pChar->SetUpSecondKeyValue( kSecondKeys );
		pChar->SetUpJoyKeyValue( kJoyKeys );
		pChar->SetIsOwnerChar( true );		
	}
	pChar->SetTeam( eTeam );

	if( pChar->GetCharName() != g_MyInfo.GetPublicID() )
	{
		if( bMovieCapturing )
		{
			// 동영상 촬영 이모티콘 추가
			pChar->SetMovieStartEmoticon( 0 );       // 0이면 지속 이모티콘
		}
	}
	return pChar;
}

void ioPlayStage::ChangeCharEquipItem( ioBaseChar *pChar, float fPrevSkillGauge[MAX_INVENTORY], SP2Packet &rkPacket, bool bFastSync, bool bBornChange )
{
	if( !pChar )
		return;

	if( !bFastSync )
	{
		pChar->DestroyAllItem();
	}

	DWORD dwItemCode;
	int iItemReinforce;
	DWORD dwItemMaleCustom, dwItemFemaleCustom;
	int i, iItemIndex;
	ioHashString szOwner;

	for( i = 0; i < MAX_EQUIP_SLOT; i++ )
	{
		rkPacket >> dwItemCode >> iItemReinforce >> dwItemMaleCustom >> dwItemFemaleCustom >> iItemIndex >> szOwner;

		if( bFastSync )   // 이미 UDP로 동기화 된 아이템이면 서버에서 준 고유 인덱스만 넣는다.
		{
			ioItem *pItem = pChar->GetEquipedItem( i );
			if( pItem )
			{
				if( pItem->GetItemCreateIndex() == MAX_INT_VALUE )
				{
					pItem->SetItemCreateIndex( iItemIndex );
				}
			}
		}
		else
		{
			if( dwItemCode > 0 )
			{
				ioItem *pItem = CreateItem( dwItemCode );
				if( pItem )
				{
					// 배틀모드 일때 벗겨진 장비가 있다면 장착하지 않고 넘어감
					ioBattleMode* pBattleMode = ToBattleMode(m_pPlayMode);
					if(pBattleMode && pBattleMode->GetModeState() == ioPlayMode::MS_PLAY)
					{
						if( !g_BattleRoomMgr.CheckUserBattleFightFlag( pChar->GetCharName() ) && pBattleMode->GetUnmountingEquipItem( pChar->GetCharName(), pItem->GetType() ) )
							continue;
					}
					pItem->ReLoadProperty();
					pItem->SetOwnerName( szOwner );
					pItem->SetItemCreateIndex( iItemIndex );
					pItem->SetItemReinforce( iItemReinforce );
					pItem->SetItemCustom( dwItemMaleCustom, dwItemFemaleCustom );

					if ( !pChar->IsGangsi() )
					{
						// 코스튬 관련 (코스튬 동기화 : 서버에서 받을수도 있음)
						pItem->SetCostumeCustom( pChar->GetCharacterInfo().m_EquipCostume[i].m_Costume_Male_Custom, pChar->GetCharacterInfo().m_EquipCostume[i].m_Costume_Female_Custom );
						pItem->SetCostume( pChar->GetCharacterInfo().m_EquipCostume[i].m_CostumeCode, pChar->IsOwnerChar() );
					}

					ioItem *pPreItem = pChar->EquipItem( pItem, true );
					SAFEDELETE( pPreItem );

					if( ToSuddenAttackItem( pItem ) )
						ToSuddenAttackItem( pItem )->SetInitialize( szOwner, this, pChar->GetSuddenMeshType() );
					else if( ToDummyCharItem2( pItem ) )
						ToDummyCharItem2( pItem )->SetInitialize( szOwner, this, pChar->GetSuddenMeshType() );
					else
					{
						ioBaseChar *pOwner = GetBaseChar( szOwner );
						pItem->Initialize( pOwner, this );
					}
					// VoiceType Set
					int iVoiceType = 0;
					if( i == 0 )
					{
						const ioSetItemInfo *pSetInfo = g_SetItemInfoMgr.GetSetInfoByIdx( dwItemCode - 1 );
						if( pSetInfo )
							iVoiceType = pSetInfo->GetVoiceType();

						pChar->SetVoiceType( iVoiceType );
					}
				}
			}
		}
	}

	if( bFastSync )
		return;

	pChar->RecalcStatusValue();
	pChar->FillMaxHP();
	float fCurRate = pChar->GetCurHPRate();
	float fNewHP = pChar->GetHP().m_fCurValue * fCurRate;
	pChar->SetCurHP( fNewHP );

	if( bBornChange )
	{
		for(i = 0;i < MAX_INVENTORY;i++)
		{
			ioItem *pItem = pChar->GetEquipedItem( i );
			if( pItem )
			{
				float fStartSkillGaugeRate = pItem->GetStartSkillGaugeRate();
				pItem->SetCurSkillGauge( m_fRevisionDefaultSkillGauge * fStartSkillGaugeRate  );
			}
		}
	}
	else
	{
		for(i = 0;i < MAX_INVENTORY;i++)
		{
			ioItem *pItem = pChar->GetEquipedItem( i );
			if( pItem )
				pItem->SetCurSkillGauge( fPrevSkillGauge[i] );
		}
	}
}

ioBaseChar* ioPlayStage::AddNewUser( const ioHashString &rkName, SP2Packet &rkPacket )
{
	CHARACTER kCharInfo;
	rkPacket >> kCharInfo;

	ioINILoader &rkLoader = Help::GetCharLoader( kCharInfo.m_kindred, kCharInfo.m_sex );

	char szFileName[MAX_PATH];
	rkLoader.LoadString_e( "model_file", "", szFileName, MAX_PATH );

	ioBaseChar *pChar = CreateBaseChar( szFileName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::AddNewUser Failed" );
		return NULL;
	}

	pChar->SetCharName( rkName );
	pChar->InitINIValue( rkLoader );
	pChar->InitRaceInfo( kCharInfo );
	pChar->SetState( CS_DELAY );
	pChar->SetDelayCameraBuff( true );
	
	int iTeamType;
	rkPacket >> iTeamType;
	
	int iSelArray;
	rkPacket >> iSelArray;
	pChar->SetSelectCharArray(iSelArray);

	if( rkName == g_MyInfo.GetPublicID() )
	{
		ioUserKeyInput::UseKeyValue kKeys;
		kKeys.SetDefaultFirstKey();
		ioUserKeyInput::UseKeyValue kSecondKeys;
		kSecondKeys.SetDefaultSecondKey();
		ioUserKeyInput::UseKeyValue kJoyKeys;
		kJoyKeys.SetDefaultJoyKey();

		g_KeyManager.GetGameKey(kKeys);
		g_KeyManager.GetSecondGameKey(kSecondKeys);
		g_KeyManager.GetJoyGameKey(kJoyKeys);

		pChar->SetUpKeyValue( kKeys );
		pChar->SetUpSecondKeyValue( kSecondKeys );
		pChar->SetUpJoyKeyValue( kJoyKeys );		
		pChar->SetIsOwnerChar( true );		
	}
	pChar->SetTeam( (TeamType)iTeamType );

	bool bMovieCapturing;
	rkPacket >> bMovieCapturing;
	if( pChar->GetCharName() != g_MyInfo.GetPublicID() )
	{
		if( bMovieCapturing )
		{
			// 동영상 촬영 이모티콘 추가
			pChar->SetMovieStartEmoticon( 0 );       // 0이면 지속 이모티콘
		}
	}

	return pChar;
}

void ioPlayStage::ClearUserCreateEntities( const ioHashString &rkName )
{
	g_WeaponMgr.DestroyReserveOwnerWeapon( rkName );

	ioBaseChar *pChar = GetBaseChar( rkName );
	if( pChar )
	{
		//서버상에서 이미 아이템 DB정보를 Clear 했기때문에 패킷을 보내지 않고 Release
		if ( pChar->GetState() != CS_DIE )
			pChar->OnReleaseObjectItem( CS_DELAY );
		else
			pChar->OnReleaseObjectItem();

		pChar->ReleaseObejctEquipState();
		pChar->SetDisableEquipObject( false );

		g_PushStructListMgr.ClearStructListByOwner( pChar );
		g_DummyCharMgr.ClearDummyCharByOwner( pChar );
	}

	g_AreaWeaponMgr.ClearAreaWeaponListByOwnerChar( rkName );
}

void ioPlayStage::RemoveUser( const ioHashString &rkName, bool isRoomOut )
{
	if( m_pPlayMode && isRoomOut )
	{
		ClearUserCreateEntities( rkName );

		ioBaseChar *pOwner = GetOwnerChar();
		if( pOwner )
		{
			pOwner->RemoveHasAreaWeaponBuff( rkName );
		}

		g_AwardMgr.RemoveAwardUser( rkName );
		m_pPlayMode->RemoveRecord( rkName );
	}

	{   // 카메라에 제외된 캐릭터 리스트도 제거
		BaseCharList::iterator iter = m_VisibleBaseList.begin();
		for( ; iter!=m_VisibleBaseList.end() ; ++iter )
		{
			ioBaseChar *pUser = ToBaseChar( *iter );
			if( !pUser )
				continue;

			if( pUser->GetCharName() != rkName )
				continue;

			m_VisibleBaseList.erase( iter );
			break;
		}
	}

	bool bNPC = false;
	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioBaseChar *pUser = ToBaseChar( *iter );
		if( !pUser )
			continue;

		if( pUser->GetCharName() != rkName )
			continue;

		BaseCharList::iterator iBase = std::find( m_BaseCharList.begin(), m_BaseCharList.end(), pUser );

		if( iBase != m_BaseCharList.end() )
		{
			if( Help::CheckEnableTimeGapUser2( 1 ) )
			{
				ioBaseChar* pLogBaseChar = (*iBase);
				if( pLogBaseChar )
					LOG.PrintTimeAndLog( 0, "%s:%s:%d", __FUNCTION__, pLogBaseChar->GetCharName().c_str(), (int)pLogBaseChar->GetControlType() );
			}

			if( ToNpcChar( *iBase ) != NULL )
				bNPC = true;

			m_BaseCharList.erase( iBase );
			g_EffectFactory.ClearWorldEffectEventOwner( (ioEntityEvent*)pUser );
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioPlayStage::RemoveUser - Critical Error - %s User Not Exist In BaseCharList", rkName.c_str() );
		}

		EraseGameEntity( iter );
		break;
	}
	
	if( !bNPC )
		g_App.EraseManualResourceByUserRemove();
}

void ioPlayStage::SendOwnerDamageList( bool bLast )
{
	if( m_pPlayMode )
	{
		m_pPlayMode->SendOwnerDamageList( bLast );
	}
}

void ioPlayStage::CheckFlyMapLimit( D3DXVECTOR3 *pFlyPos )
{
	if( m_pSceneMgr )
		m_pSceneMgr->CheckFlyMapLimit( pFlyPos );
}

bool ioPlayStage::CheckAxisLimit( D3DXVECTOR3 *pNewPos )
{
	if( m_pSceneMgr )
		return m_pSceneMgr->CheckAxisLimit( pNewPos );

	return false;
}

bool ioPlayStage::CheckAxisLimit( const D3DXVECTOR3& vNewPos )
{
	if( m_pSceneMgr )
		return m_pSceneMgr->CheckAxisLimit( vNewPos );

	return false;
}

void ioPlayStage::EnableWorldEntityFunctions( bool bEnable )
{
	if( m_pWorldEntityFactory )
	{
		m_pWorldEntityFactory->EnableWorldEntityFunctions( bEnable );
	}

	if( m_pSceneMgr )
	{
		m_pSceneMgr->StopWorldMeshTexEffect( !bEnable );
	}
	if( m_pBlockMgr )
		m_pBlockMgr->StopAllTextureEffect( !bEnable );
}

void ioPlayStage::SetScreenShotMode( bool bStart )
{
	if( m_pPlayMode == NULL ) return;

	ioBaseChar *pOwnerChar = GetOwnerChar();
	if( !bStart && pOwnerChar && ( pOwnerChar->GetState() == CS_DIE || pOwnerChar->GetState() == CS_VIEW ) )
	{
		if( pOwnerChar->GetState() == CS_VIEW )
		{
			if( m_pPlayMode->GetModeType() != MT_FIGHT_CLUB )      // 파이트클럽의 CS_VIEW는 다음 출전 대기자이다.
				return;
		}
		else
			return;
	}

	if( !bStart && ( g_GUIMgr.IsNoneUIMode() || HasCameraFixedStyle( CAMERA_STYLE_MOVIE ) || HasCameraFixedStyle( CAMERA_STYLE_VIEW ) ) ) 
		return;

	m_pPlayMode->SetBlindMode( bStart );
	if( !bStart )
		m_bBlindAniEnd = false;
}

void ioPlayStage::ContributeLOG()
{
	if( m_pPlayMode )
	{
		m_pPlayMode->ContributeLOG();
	}
}

int ioPlayStage::ContributeMyRank( int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
{
	if( m_pPlayMode )
		return m_pPlayMode->ContributeMyRank( rRankValue1, rRankValue2, rRankValue3, rRankValue4 );
	return 0;
}

int ioPlayStage::ContributeRank( const ioHashString &rkName, int &rRankValue1, int &rRankValue2, int &rRankValue3, int &rRankValue4 )
{
	if( m_pPlayMode )
		return m_pPlayMode->ContributeRank( rkName, rRankValue1, rRankValue2, rRankValue3, rRankValue4 );
	return 0;
}

void ioPlayStage::RecordLOG()
{
	if( m_pPlayMode )
		m_pPlayMode->RecordLOG();
}

bool ioPlayStage::IsNetworkMode()
{
	if( m_pPlayMode ) 
		return m_pPlayMode->IsNetworkMode();

	return false;
}

bool ioPlayStage::IsCollisionCheckByMode()
{
	if( m_pPlayMode )
		return m_pPlayMode->IsCollisionCheck();

	return false;
}

void ioPlayStage::AddMySoldier( DWORD dwIndex, bool bBuySoldier )
{
	if( !m_pPlayMode ) return;

	// 용병 선택 UI가 활성화 되어있으면 추가
	SoldierSelectWnd *pSelectWnd = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
	if( pSelectWnd && pSelectWnd->IsShow() )
	{
		pSelectWnd->AddMySoldier( dwIndex );
	}

	UISoldierSelectWnd *pUISelectWnd = dynamic_cast<UISoldierSelectWnd*>(g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ) );
	if( pUISelectWnd && pUISelectWnd->IsShow() )
	{
		pUISelectWnd->AddMySoldier( dwIndex );
	}

	MiniSoldierSelectWnd *pMiniSoldierWnd = dynamic_cast<MiniSoldierSelectWnd*>( g_GUIMgr.FindWnd( MINI_SOLDIER_SELECT_WND ) );
	if( pMiniSoldierWnd && pMiniSoldierWnd->IsShow() )
		pMiniSoldierWnd->AddMySoldier( dwIndex );

    m_pPlayMode->AddMySoldier( dwIndex, bBuySoldier );
}

void ioPlayStage::ReLoadMySoldier( int iCharArray )
{
	if( m_pPlayMode )
	{
		m_pPlayMode->ReLoadMySoldier( iCharArray );
	}
}

void ioPlayStage::ReLoadAllMySoldier()
{
	if( m_pPlayMode )
	{
		m_pPlayMode->ReLoadAllMySoldier();
	}
}

void ioPlayStage::ReLoadCharInfo( DWORD dwCharIndex, CHARACTER &rkInfo )
{
	BaseCharList::iterator iter=m_BaseCharList.begin();
	for( ; iter!=m_BaseCharList.end() ; ++iter )
	{
		if( (*iter)->GetIndex() == dwCharIndex )
		{
			(*iter)->SetReloadDecoration( rkInfo );
		}
	}
}

void ioPlayStage::RemoveMySoldier( DWORD dwIndex )
{
	if( !m_pPlayMode ) return;

	if( m_pPlayMode->GetModeType() != MT_MYROOM )
	{
		// 내 용병의 어레이를 바꾼다.
        ioBaseChar *pChar = GetOwnerChar();
		if( pChar )
			pChar->SetSelectCharArray( g_MyInfo.GetSelectCharArray() );
	}

	m_pPlayMode->RemoveMySoldier( dwIndex );
}

void ioPlayStage::MySoldierDefPos()
{
	if( m_pPlayMode )
	{
		m_pPlayMode->MySoldierDefPos();
	}
}

bool ioPlayStage::IsRoundSetEnd()
{
	if( m_pPlayMode )
		return m_pPlayMode->IsRoundSetEnd();

	return true;
}

void ioPlayStage::AddAllNPCSyncFirstUser()
{
	if( m_pPlayMode == NULL ) return;

	ioHashString szFirstUser;
	if( m_pPlayMode->GetModeType() == MT_MYROOM )
	{
		szFirstUser = g_MyInfo.GetPublicID();
	}
	else
	{
		BaseCharList::iterator iter;	
		for( iter=m_BaseCharList.begin() ; iter!=m_BaseCharList.end() ; ++iter )
		{
			ioBaseChar *pChar = *iter;
			if( pChar == NULL )	continue;
			if( ToNpcChar( pChar ) ) continue;

			szFirstUser = pChar->GetCharName();
			break;
		}

		if( szFirstUser.IsEmpty() )
		{
			// 동기화 유저가 없으면 NPC가 안움직이므로 자신이 주체가 된다. 
			szFirstUser = g_MyInfo.GetPublicID();
		}
	}

	{
		BaseCharList::iterator iter;	
		for( iter=m_BaseCharList.begin() ; iter!=m_BaseCharList.end() ; ++iter )
		{
			ioNpcChar *pNpc = ToNpcChar( *iter );
			if( pNpc == NULL )	continue;

			pNpc->SetSyncUser( szFirstUser );
			if( m_pPlayMode->IsProtoTypeMode() )
			{
				D3DXVECTOR3 vStartPos = m_pPlayMode->GetStartPos( pNpc->GetTeam() );
				pNpc->SetWorldPosition( vStartPos );
				pNpc->SetCreateMotionState();
				LOG.PrintTimeAndLog( 0, "AddAllNPCSyncFirstUser : %s Start!!", pNpc->GetCharName().c_str() );
			}
		}
	}
}

void ioPlayStage::SetMonsterTargetFixing( const ioHashString &rkTargetName, float fFixingTime )
{
	int iCharCnt = m_BaseCharList.size();
	for( int i=0; i < iCharCnt; i++ )
	{
		ioNpcChar *pNpcChar = ToNpcChar( m_BaseCharList[i] );
		if( pNpcChar == NULL ) continue;
		if( pNpcChar->GetState() == CS_VIEW || pNpcChar->GetState() == CS_OBSERVER ) continue;

		pNpcChar->SetFixingTarget( rkTargetName, fFixingTime );

		if( pNpcChar->GetState() != CS_LOADING && pNpcChar->GetState() != CS_DIE )
			pNpcChar->SetTargetCommandEmoticon();
	}

	ioBaseChar *pTargetChar = GetBaseChar( rkTargetName );
	if( pTargetChar )
		pTargetChar->SetTargetingEmoticon();
}

bool ioPlayStage::ProcessTCPPacket( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1100, 150 );
	switch( rkPacket.GetPacketID() )
	{
	case STPK_JOIN_ROOMDATA:
		ErrorReport::SetPosition( 1100, 151 );
		OnJoinRoomData( rkPacket );
		ErrorReport::SetPosition( 1100, 152 );
		return true;
	case STPK_MODE_INFO:
		ErrorReport::SetPosition( 1100, 153 );
		OnModeInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 154 );
		return true;
	case STPK_PLAYING_USER_DATA:
		ErrorReport::SetPosition( 1100, 155 );
		OnJoinPlayingUserData( rkPacket );
		ErrorReport::SetPosition( 1100, 156 );
		return true;
	case STPK_JOIN_NEW_USERDATA:
		ErrorReport::SetPosition( 1100, 3000 );
		OnJoinUserData( rkPacket );
		ErrorReport::SetPosition( 1100, 3001 );
		return true;
	case STPK_JOIN_USERDATA:
		ErrorReport::SetPosition( 1100, 157 );
		OnJoinUserData( rkPacket );
		ErrorReport::SetPosition( 1100, 158 );
		return true;
	case STPK_PASSAGE:
		ErrorReport::SetPosition( 1100, 159 );
		OnPassage( rkPacket );
		ErrorReport::SetPosition( 1100, 160 );
		return true;
	case STPK_LEAVE_USERDATA:
		ErrorReport::SetPosition( 1100, 161 );
		OnLeaveUserData( rkPacket );
		ErrorReport::SetPosition( 1100, 162 );
		return true;
	case STPK_EXIT_ROOM:
		ErrorReport::SetPosition( 1100, 163 );
		OnExitRoom( rkPacket );
		ErrorReport::SetPosition( 1100, 164 );
		return true;
	case STPK_CREATE_ROOM:
		ErrorReport::SetPosition( 1100, 165 );
		OnCreateRoom( rkPacket );
		ErrorReport::SetPosition( 1100, 166 );
		return true;
	case STPK_JOIN_ROOM:
		ErrorReport::SetPosition( 1100, 169 );
		OnJoinRoom( rkPacket );
		ErrorReport::SetPosition( 1100, 170 );
		return true;
	case STPK_CHANGE_STAGE:
		ErrorReport::SetPosition( 1100, 171 );
		OnChangeStage( rkPacket );
		ErrorReport::SetPosition( 1100, 172 );
		return true;
	case STPK_HOST_USERID:
		ErrorReport::SetPosition( 1100, 173 );
		OnHostUserID( rkPacket );
		ErrorReport::SetPosition( 1100, 174 );
		return true;
	case STPK_CHANGE_UDP_INFO:
		ErrorReport::SetPosition( 1100, 175 );
		OnChangeUDPInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 176 );
		return true;
	case STPK_PLAYRECORD_INFO:
		ErrorReport::SetPosition( 1100, 177 );
		OnPlayRecordInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 178 );
		return true;
	case STPK_CHAR_EQUIPDATA:
		ErrorReport::SetPosition( 1100, 179 );
		OnParentCharEquipData( rkPacket );
		ErrorReport::SetPosition( 1100, 180 );
		return true;
	case STPK_FIELD_ITEM_LIST:
		ErrorReport::SetPosition( 1100, 181 );
		OnFieldItemList( rkPacket );
		ErrorReport::SetPosition( 1100, 182 );
		return true;
	case STPK_FIELD_ITEM_SUPPLY:
		ErrorReport::SetPosition( 1100, 183 );
		OnFieldItemSupply( rkPacket );
		ErrorReport::SetPosition( 1100, 184 );
		return true;
	case STPK_DROP_ITEM:
		ErrorReport::SetPosition( 1100, 185 );
		OnDropItem( rkPacket );
		ErrorReport::SetPosition( 1100, 186 );
		return true;
	case STPK_REPOSITION_FIELDITEM:
		ErrorReport::SetPosition( 1100, 187 );
		OnRepositionFieldItem( rkPacket );
		ErrorReport::SetPosition( 1100, 188 );
		return true;
	case STPK_PICK_ITEM:
		ErrorReport::SetPosition( 1100, 189 );
		OnPickItem( rkPacket );
		ErrorReport::SetPosition( 1100, 190 );
		return true;
	case STPK_PICK_ITEM_FAIL:
		ErrorReport::SetPosition( 1100, 191 );
		OnPickItemFail( rkPacket );
		ErrorReport::SetPosition( 1100, 192 );
		return true;
	case STPK_DELETE_FIELD_ITEM:
		OnDeleteFieldItem( rkPacket );
		return true;
	case STPK_DELETE_FIELD_ITEM_FAIL:
		OnDeleteFieldItemFail( rkPacket );
		return true;
	case STPK_DROP_ITEM_FAIL:
		ErrorReport::SetPosition( 1100, 193 );
		OnDropItemFail( rkPacket );
		ErrorReport::SetPosition( 1100, 194 );
		return true;
	case STPK_DROP_DIE:
		ErrorReport::SetPosition( 1100, 195 );
		OnDropDie( rkPacket );
		ErrorReport::SetPosition( 1100, 196 );
		return true;
	case STPK_WEAPON_DIE:
		ErrorReport::SetPosition( 1100, 197 );
		OnWeaponDie( rkPacket );
		ErrorReport::SetPosition( 1100, 198 );
		return true;
	case STPK_ANSWER_REVIVAL_TIME:
		OnRevivalTime( rkPacket );
		return true;
	case STPK_CHANGE_CHAR:
		ErrorReport::SetPosition( 1100, 199 );
		OnParentChangeChar( rkPacket, false );          // TCP & UDP 패킷 동일
		ErrorReport::SetPosition( 1100, 200 );
		return true;
	case STPK_JOIN_ROOM_INFO_END:
		ErrorReport::SetPosition( 1100, 201 );
		OnJoinRoomInfoEnd( rkPacket );
		ErrorReport::SetPosition( 1100, 202 );
		return true;
	case STPK_START_SET_NEXT_MODE:
		ErrorReport::SetPosition( 1100, 203 );
		OnStartSetNextMode( rkPacket );
		ErrorReport::SetPosition( 1100, 204 );
		return true;
	case STPK_END_SET_NEXT_MODE:
		ErrorReport::SetPosition( 1100, 205 );
		OnEndSetNextMode( rkPacket );
		ErrorReport::SetPosition( 1100, 206 );
		return true;
	case STPK_BATTLE_ROOM_END:
		ErrorReport::SetPosition( 1100, 207 );
		OnBattleRoomRoundEnd( rkPacket );
		ErrorReport::SetPosition( 1100, 208 );
		return true;	
	case STPK_MOVING_ROOM:
		ErrorReport::SetPosition( 1100, 211 );
		OnMovingRoom( rkPacket );
		ErrorReport::SetPosition( 1100, 212 );
		return true;
	case STPK_CHAR_SLOT_CHANGE:
		ErrorReport::SetPosition( 1100, 213 );
		OnCharSlotChange( rkPacket );
		ErrorReport::SetPosition( 1100, 214 );
		return true;
	case STPK_VOICE_INFO:
		ErrorReport::SetPosition( 1100, 215 );
		OnVoiceInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 216 );
		return true;	
		// 파티
	case STPK_BATTLEROOM_LIST:
		ErrorReport::SetPosition( 1100, 219 );
		OnBattleRoomList( rkPacket );
		ErrorReport::SetPosition( 1100, 220 );
		return true;
	case STPK_CREATE_BATTLEROOM:
		ErrorReport::SetPosition( 1100, 221 );
		OnBattleRoomCreate( rkPacket );
		ErrorReport::SetPosition( 1100, 222 );
		return true;
	case STPK_BATTLEROOM_INFO:
		ErrorReport::SetPosition( 1100, 223 );
		OnBattleRoomInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 224 );
		return true;
	case STPK_BATTLEROOM_USER_INFO:
		ErrorReport::SetPosition( 1100, 225 );
		OnBattleRoomUserInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 226 );
		return true;
	case STPK_USER_BATTLEROOM_JOIN:
		ErrorReport::SetPosition( 1100, 227 );
		OnBattleRoomJoin( rkPacket );
		ErrorReport::SetPosition( 1100, 228 );
		return true;
	case STPK_BATTLEROOM_LEAVE:
		ErrorReport::SetPosition( 1100, 229 );
		OnBattleLeaveUser( rkPacket );
		ErrorReport::SetPosition( 1100, 230 );
		return true;
	case STPK_MACRO_COMMAND:
		ErrorReport::SetPosition( 1100, 231 );
		OnMacroCmd( rkPacket );
		ErrorReport::SetPosition( 1100, 232 );
		return true;
	case STPK_BATTLEROOM_INVITE_LIST:
		ErrorReport::SetPosition( 1100, 239 );
		OnBattleRoomInviteList( rkPacket );
		ErrorReport::SetPosition( 1100, 240 );
		return true;
	case STPK_BATTLEROOM_INVITE:
		ErrorReport::SetPosition( 1100, 241 );
		OnBattleRoomInvited( rkPacket );	
		ErrorReport::SetPosition( 1100, 242 );
		return true;
	case STPK_RELAY_CHAT:
		ErrorReport::SetPosition( 1100, 243 );
		OnRelayChat( rkPacket );
		ErrorReport::SetPosition( 1100, 244 );
		return true;
	case STPK_BATTLEROOM_JOIN_INFO:
		ErrorReport::SetPosition( 1100, 245 );
		OnBattleRoomJoinInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 246 );
		return true;
	case STPK_AWARDING_INFO:
		ErrorReport::SetPosition( 1100, 251 );
		OnAwardingInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 252 );
		return true;
	case STPK_AWARDING_RESULT:
		ErrorReport::SetPosition( 1100, 253 );
		OnAwardingResult( rkPacket );
		ErrorReport::SetPosition( 1100, 254 );
		return true;
	case STPK_FOLLOW_USER:
		ErrorReport::SetPosition( 1100, 255 );
		OnFollowUserResult( rkPacket );
		ErrorReport::SetPosition( 1100, 256 );
		return true;
	case STPK_REQUEST_BANKRUPT:
		ErrorReport::SetPosition( 1100, 257 );
		OnRequestBankruptResult( rkPacket );
		ErrorReport::SetPosition( 1100, 258 );
		return true;
	case STPK_ETCITEM_BUY:
		ErrorReport::SetPosition( 1100, 259 );
		OnEtcItemBuyResult( rkPacket );
		ErrorReport::SetPosition( 1100, 260 );
		return true;
	case STPK_BATTLEROOM_COMMAND:
		ErrorReport::SetPosition( 1100, 263 );
		OnBattleRoomCmd( rkPacket );
		ErrorReport::SetPosition( 1100, 264 );
		return true;
	case STPK_PLAZA_ROOM_LIST:
		ErrorReport::SetPosition( 1100, 269 );
		OnPlazaRoomList( rkPacket );
		ErrorReport::SetPosition( 1100, 270 );
		return true;
	case STPK_PLAZA_COMMAND:
		ErrorReport::SetPosition( 1100, 271 );
		OnPlazaCommand( rkPacket );
		ErrorReport::SetPosition( 1100, 272 );
		return true;
	case STPK_PLAZA_INVITE_LIST:
		ErrorReport::SetPosition( 1100, 273 );
		OnPlazaInviteList( rkPacket );
		ErrorReport::SetPosition( 1100, 274 );
		return true;
	case STPK_PLAZA_USER_INVITE:
		ErrorReport::SetPosition( 1100, 275 );
		OnPlazaInvite( rkPacket );
		ErrorReport::SetPosition( 1100, 276 );
		return true;
	case STPK_BATTLEROOM_INVITE_RESULT:
		ErrorReport::SetPosition( 1100, 277 );
		OnBattleRoomInviteResult( rkPacket );	
		ErrorReport::SetPosition( 1100, 278 );
		return true;
	case STPK_USER_POS_INDEX:
		ErrorReport::SetPosition( 1100, 279 );
		OnUserPosIndex( rkPacket );	
		ErrorReport::SetPosition( 1100, 280 );
		return true;
	case STPK_BATTLEROOM_ENTERUSER_SYNC:
		ErrorReport::SetPosition( 1100, 281 );
		OnBattleRoomEnterUserSync( rkPacket );	
		ErrorReport::SetPosition( 1100, 282 );
		return true;
	case STPK_GUILD_RANK_LIST:
		ErrorReport::SetPosition( 1100, 283 );
		OnGuildRankList( rkPacket );	
		ErrorReport::SetPosition( 1100, 284 );
		return true;
	case STPK_PLAZA_JOIN_INFO:
		ErrorReport::SetPosition( 1100, 287 );
		OnPlazaJoinInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 288 );
		return true;
	case STPK_LADDER_TEAM_LIST:
		ErrorReport::SetPosition( 1100, 289 );
		OnLadderTeamList( rkPacket );
		ErrorReport::SetPosition( 1100, 290 );
		return true;
	case STPK_CREATE_LADDERTEAM:
		ErrorReport::SetPosition( 1100, 291 );
		OnCreateLadderTeam( rkPacket );
		ErrorReport::SetPosition( 1100, 292 );
		return true;
	case STPK_LADDERTEAM_INFO:
		ErrorReport::SetPosition( 1100, 293 );
		OnLadderTeamInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 294 );
		return true;
	case STPK_LADDERTEAM_USER_INFO:
		ErrorReport::SetPosition( 1100, 295 );
		OnLadderTeamUserInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 296 );
		return true;
	case STPK_LADDERTEAM_LEAVE:
		ErrorReport::SetPosition( 1100, 297 );
		OnLadderTeamLeave( rkPacket );
		ErrorReport::SetPosition( 1100, 298 );
		return true;
	case STPK_LADDERTEAM_MACRO:
		ErrorReport::SetPosition( 1100, 299 );
		OnLadderTeamMacro( rkPacket );
		ErrorReport::SetPosition( 1100, 300 );
		return true;
	case STPK_LADDERTEAM_JOIN_INFO:
		ErrorReport::SetPosition( 1100, 301 );
		OnLadderTeamJoinInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 302 );
		return true;
	case STPK_JOIN_LADDERTEAM:
		ErrorReport::SetPosition( 1100, 303 );
		OnJoinLadderTeam( rkPacket );
		ErrorReport::SetPosition( 1100, 304 );
		return true;
	case STPK_LADDERTEAM_INVITE_LIST:
		ErrorReport::SetPosition( 1100, 305 );
		OnLadderTeamInviteList( rkPacket );
		ErrorReport::SetPosition( 1100, 306 );
		return true;
	case STPK_LADDERTEAM_INVITED:
		ErrorReport::SetPosition( 1100, 307 );
		OnLadderTeamInvited( rkPacket );
		ErrorReport::SetPosition( 1100, 308 );
		return true;
	case STPK_LADDER_TEAM_RANKING:
		ErrorReport::SetPosition( 1100, 309 );
		OnLadderTeamRanking( rkPacket );
		ErrorReport::SetPosition( 1100, 310 );
		return true;
	case STPK_LADDER_OTHER_TEAM_INFO:
		ErrorReport::SetPosition( 1100, 311 );
		OnLadderOtherTeamInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 312 );
		return true;
	case STPK_LADDER_OTHER_NAME_CHANGE:
		ErrorReport::SetPosition( 1100, 313 );
		OnLadderOtherNameChange( rkPacket );
		ErrorReport::SetPosition( 1100, 314 );
		return true;
	case STPK_LADDER_BATTLE_REQUEST_AGREE:
		return true;
	case STPK_IMMEDIATELY_EQUIP:
		ErrorReport::SetPosition( 1100, 319 );
		OnImmediatelyEquipItem( rkPacket );
		ErrorReport::SetPosition( 1100, 320 );
		return true;
	case STPK_ETCITEM_USE:
		ErrorReport::SetPosition( 1100, 321 );
		OnEtcItemUse( rkPacket );
		ErrorReport::SetPosition( 1100, 322 );
		return true;
	case STPK_MOVIE_CONTROL:
		ErrorReport::SetPosition( 1100, 323 );
		OnMovieControl( rkPacket );
		ErrorReport::SetPosition( 1100, 324 );
		return true;
	case STPK_ABSORB_APPLY:
		ErrorReport::SetPosition( 1100, 325 );
		OnAbsorbInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 326 );
		return true;
	case STPK_CHAT_MODE:
		ErrorReport::SetPosition( 1100, 327 );
		OnChatModeState( rkPacket );
		ErrorReport::SetPosition( 1100, 328 );
		return true;
	case STPK_GROWTH_LEVEL_UP:
		ErrorReport::SetPosition( 1100, 329 );
		OnGrowthLevelUp( rkPacket );
		ErrorReport::SetPosition( 1100, 330 );
		return true;
	case STPK_LEVEL_GROWTH:
		ErrorReport::SetPosition( 1100, 329 );
		OnLevelGrowth( rkPacket );
		ErrorReport::SetPosition( 1100, 330 );
		return true;
	case STPK_GROWTH_POINT_BONUS:
		ErrorReport::SetPosition( 1100, 331 );
		OnGrowthPointBonus( rkPacket );
		ErrorReport::SetPosition( 1100, 332 );
		return true;
	case STPK_GROWTH_LEVEL_INIT:
		ErrorReport::SetPosition( 1100, 333 );
		OnGrowthLevelInit( rkPacket );
		ErrorReport::SetPosition( 1100, 334 );
		return true;
	case STPK_USER_KICK_VOTE:
		ErrorReport::SetPosition( 1100, 335 );
		OnUserKickVote( rkPacket );
		ErrorReport::SetPosition( 1100, 336 );
		return true;
	case STPK_LADDER_TEAM_RANK_LIST:
		ErrorReport::SetPosition( 1100, 337 );
		OnLadderTeamRankList( rkPacket );
		ErrorReport::SetPosition( 1100, 338 );
		return true;
	case STPK_ETCITEM_ACTION:
		ErrorReport::SetPosition( 1100, 339 );
		OnEtcItemAction( rkPacket );
		ErrorReport::SetPosition( 1100, 340 );
		return true;
	case STPK_MYROOM_SERVER_CHANGE:
		ErrorReport::SetPosition( 1100, 341 );
		OnMyRoomServerChange( rkPacket );
		ErrorReport::SetPosition( 1100, 342 );
		return true;
	case STPK_SERVER_LOBBY_INFO:
		return true;
	case STPK_GROWTH_LEVEL_DOWN:
		ErrorReport::SetPosition( 1100, 345 );
		OnGrowthLevelDown( rkPacket );
		ErrorReport::SetPosition( 1100, 346 );
		return true;
	case STPK_BAD_PING_CROWN_DROP:
		ErrorReport::SetPosition( 1100, 347 );
		OnBadPingCrownDrop( rkPacket );
		ErrorReport::SetPosition( 1100, 348 );
		return true;
	case STPK_FISHING:
		ErrorReport::SetPosition( 1100, 349 );
		OnFishingState( rkPacket );
		ErrorReport::SetPosition( 1100, 350 );
		return true;
	case STPK_EXTRAITEM_BUY:
		ErrorReport::SetPosition( 1100, 351 );
		OnExtraItemBuyResult( rkPacket );
		ErrorReport::SetPosition( 1100, 352 );
		return true;
	case STPK_EXTRAITEM_CHANGE:
		ErrorReport::SetPosition( 1100, 353 );
		OnExtraItemChange( rkPacket );
		ErrorReport::SetPosition( 1100, 354 );
		return true;
	case STPK_EXTRAITEM_SELL:
		ErrorReport::SetPosition( 1100, 357 );
		OnExtraItemSellResult( rkPacket );
		ErrorReport::SetPosition( 1100, 358 );
		return true;
	case STPK_ETCITEM_SELL:
		ErrorReport::SetPosition( 1100, 359 );
		OnEtcItemSell( rkPacket );
		ErrorReport::SetPosition( 1100, 360 );
		return true;
	case STPK_REFILL_COIN:
		ErrorReport::SetPosition( 1100, 361 );
		OnRefillCoin( rkPacket );
		ErrorReport::SetPosition( 1100, 362 );
		return true;
	case STPK_TIME_GROWTH_ADD:
		ErrorReport::SetPosition( 1100, 363 );
		OnTimeGrowthAdd( rkPacket );
		ErrorReport::SetPosition( 1100, 364 );
		return true;
	case STPK_TIME_GROWTH_REMOVE:
		ErrorReport::SetPosition( 1100, 365 );
		OnTimeGrowthRemove( rkPacket );
		ErrorReport::SetPosition( 1100, 366 );
		return true;
	case STPK_ITEM_COMPOUND:
		ErrorReport::SetPosition( 1100, 367 );
		OnItemCompound( rkPacket );
		ErrorReport::SetPosition( 1100, 368 );
		return true;
	case STPK_ITEM_MOVE_DROP:
		ErrorReport::SetPosition( 1100, 369 );
		OnItemMoveDrop( rkPacket );
		ErrorReport::SetPosition( 1100, 370 );
		return true;
	case STPK_PRESENT_BUY:
		ErrorReport::SetPosition( 1100, 371 );
		OnPresentBuy( rkPacket );
		ErrorReport::SetPosition( 1100, 372 );
		return true;
	case STPK_ALL_ITEM_DROP:
		ErrorReport::SetPosition( 1100, 373 );
		OnAllItemDrop( rkPacket );
		ErrorReport::SetPosition( 1100, 374 );
		return true;
	case STPK_EXCAVATION_COMMAND:
		ErrorReport::SetPosition( 1100, 375 );
		OnExcavation( rkPacket );
		ErrorReport::SetPosition( 1100, 376 );
		return true;
	case STPK_LADDER_USER_HQ_MOVE:
		ErrorReport::SetPosition( 1100, 377 );
		OnLadderUserHQMove( rkPacket );
		ErrorReport::SetPosition( 1100, 378 );
		return true;
	case STPK_GRADE_SYNC:
		ErrorReport::SetPosition( 1100, 379 );
		OnGradeSync( rkPacket );
		ErrorReport::SetPosition( 1100, 380 );
		return true;
	case STPK_MEDALITEM_CHANGE:
		ErrorReport::SetPosition( 1100, 381 );
		OnMedalItemChange( rkPacket );
		ErrorReport::SetPosition( 1100, 382 );
		return true;
	case STPK_CUSTOM_MEDALITEM_CHANGE:
		OnCustomMedalItemChange( rkPacket );
		return true;
	case STPK_MEDALITEM_BUY:
		ErrorReport::SetPosition( 1100, 383 );
		OnMedalItemBuy( rkPacket );
		ErrorReport::SetPosition( 1100, 384 );
		return true;
	case STPK_HERO_MATCH_OTHER_NAME:
		ErrorReport::SetPosition( 1100, 385 );
		OnHeroMatchOtherName( rkPacket );
		ErrorReport::SetPosition( 1100, 386 );
		return true;
	case STPK_HERO_MATCH_OTHER_INFO:
		ErrorReport::SetPosition( 1100, 387 );
		OnHeroMatchOtherInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 388 );
		return true;
	case STPK_TRADE_STATE_CHANGE:
		ErrorReport::SetPosition( 1100, 389 );
		OnTradeStateChange( rkPacket );
		ErrorReport::SetPosition( 1100, 390 );
		return true;
	case STPK_TRADE_CREATE:
		ErrorReport::SetPosition( 1100, 393 );
		OnTradeCreate( rkPacket );
		ErrorReport::SetPosition( 1100, 394 );
		return true;
	case STPK_TRADE_ITEM_COMPLETE:
		ErrorReport::SetPosition( 1100, 395 );
		OnTradeItemBuy( rkPacket );
		ErrorReport::SetPosition( 1100, 396 );
		return true;
	case STPK_TRADE_CANCEL:
		ErrorReport::SetPosition( 1100, 397 );
		OnTradeItemCancel( rkPacket );
		ErrorReport::SetPosition( 1100, 398 );
		return true;
	case STPK_TRADE_TIME_OUT:
		ErrorReport::SetPosition( 1100, 399 );
		OnTradeTimeOut( rkPacket );
		ErrorReport::SetPosition( 1100, 400 );
		return true;
	case STPK_ETCITEM_SWITCH:
		ErrorReport::SetPosition( 1100, 401 );
		OnEtcItemSwitch( rkPacket );
		ErrorReport::SetPosition( 1100, 402 );
		return true;
	case STPK_ROOM_USER_ETCITEM_SYNC:
		ErrorReport::SetPosition( 1100, 403 );
		OnRoomUserEtcItemSync( rkPacket );
		ErrorReport::SetPosition( 1100, 404 );
		return true;
	case STPK_ETCITEM_SKELETON_DELETE:
		ErrorReport::SetPosition( 1100, 405 );
		OnEtcItemSkeletonDelete( rkPacket );
		ErrorReport::SetPosition( 1100, 406 );
		return true;
	case STPK_ETCITEM_MOTION_STATE:
		ErrorReport::SetPosition( 1100, 407 );
		OnEtcItemMotionStateTCP( rkPacket );
		ErrorReport::SetPosition( 1100, 408 );
		return true;
	case STPK_EXTRAITEM_GROWTH_CATALYST:
		ErrorReport::SetPosition( 1100, 409 );
		OnItemGrowthCatalyst( rkPacket );
		ErrorReport::SetPosition( 1100, 410 );
		return true;
	case STPK_ETCITEM_ITEM_COMPOUNDEX_RESULT:
		ErrorReport::SetPosition( 1100, 411 );
		OnItemCompoundEx( rkPacket );
		ErrorReport::SetPosition( 1100, 412 );
		return true;
	case STPK_ETCITEM_LUCKY_COIN_RESULT:
		ErrorReport::SetPosition( 1100, 413 );
		OnEtcItemLuckyCoinUseResult( rkPacket );
		ErrorReport::SetPosition( 1100, 414 );
		return true;
	case STPK_EXPERIENCE_MODE:
		ErrorReport::SetPosition( 1100, 415 );
		OnExperienceModeState( rkPacket );
		ErrorReport::SetPosition( 1100, 416 );
		return true;
	case STPK_HEADQUARTERS_INFO:
		ErrorReport::SetPosition( 1100, 419 );
		OnHeadquartersInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 420 );
		return true;
	case STPK_HEADQUARTERS_JOIN_REQUEST:
		ErrorReport::SetPosition( 1100, 421 );
		OnHeadquartersJoinRequest( rkPacket );
		ErrorReport::SetPosition( 1100, 422 );
		return true;
	case STPK_HEADQUARTERS_USER_INVITE:
		ErrorReport::SetPosition( 1100, 423 );
		OnHeadquartersInvite( rkPacket );
		ErrorReport::SetPosition( 1100, 424 );
		return true;
	case STPK_HEADQUARTERS_INVITE_LIST:
		ErrorReport::SetPosition( 1100, 425 );
		OnHeadquartersInviteList( rkPacket );
		ErrorReport::SetPosition( 1100, 426 );
		return true;
	case STPK_ETCITEM_RAINBOW_MIXER_RESULT:
		ErrorReport::SetPosition( 1100, 427 );
		OnEtcItemRainbowMixerResult( rkPacket );
		ErrorReport::SetPosition( 1100, 428 );
		return true;
	case STPK_FIELD_REWARD_ITEM_LIST:
		ErrorReport::SetPosition( 1100, 429 );
		OnFieldRewardItemList( rkPacket );
		ErrorReport::SetPosition( 1100, 430 );
		return true;
	case STPK_FIELD_REWARD_ITEM_CHANGE:
		ErrorReport::SetPosition( 1100, 431 );
		OnFieldRewardItemChange( rkPacket );
		ErrorReport::SetPosition( 1100, 432 );
		return true;
	case STPK_PICK_REWARD_ITEM:
		ErrorReport::SetPosition( 1100, 433 );
		OnPickRewardItem( rkPacket );
		ErrorReport::SetPosition( 1100, 434 );
		return true;
	case STPK_ETCITEM_LOSTSAGA_MIXER_RESULT:
		ErrorReport::SetPosition( 1100, 435 );
		OnEtcItemLostSagaMixerResult( rkPacket );
		ErrorReport::SetPosition( 1100, 436 );
		return true;
	case STPK_ETCITEM_TIME_GASHAPON_UPDATE:
		ErrorReport::SetPosition( 1100, 437 );
		OnEtcItemTimeGashaponUpdate( rkPacket );
		ErrorReport::SetPosition( 1100, 438 );
		return true;
	case STPK_ETCITEM_GOLD_BOX_RESULT:
		ErrorReport::SetPosition( 1100, 439 );
		OnEtcItemGoldBoxResult( rkPacket );
		ErrorReport::SetPosition( 1100, 440 );
		return true;
	case STPK_EXPAND_MEDAL_SLOT_OPEN:
		ErrorReport::SetPosition( 1100, 441 );
		OnExpandMedalSlotOpen( rkPacket );
		ErrorReport::SetPosition( 1100, 442 );
		return true;
	case STPK_EXTRAITEM_DISASSEMBLE:
		OnExtraItemDisassembleResult( rkPacket );
		return true;
	case STPK_BUY_SELECT_EXTRA_GASHAPON:
		ErrorReport::SetPosition( 1100, 443 );
		OnSelectExtraGashaponResult( rkPacket );
		ErrorReport::SetPosition( 1100, 444 );
		return true;
	case STPK_GROWTH_ALL_LEVEL_DOWN:
		ErrorReport::SetPosition( 1100, 465 );
		OnGrowthAllLevelDown( rkPacket );
		ErrorReport::SetPosition( 1100, 466 );
		return true;
	case STPK_ROULETTE_START:
		ErrorReport::SetPosition( 1100, 497 );
		OnRouletteStart( rkPacket );
		ErrorReport::SetPosition( 1100, 498 );
		return true;
	case STPK_ROULETTE_JOIN_END:
		ErrorReport::SetPosition( 1100, 499 );
		OnRouletteJoinEnd( rkPacket );
		ErrorReport::SetPosition( 1100, 500 );
		return true;
	case STPK_ROULETTE_END:
		ErrorReport::SetPosition( 1100, 501 );
		OnRouletteEnd( rkPacket );
		ErrorReport::SetPosition( 1100, 502 );
		return true;
	case STPK_ROULETTE:
		ErrorReport::SetPosition( 1100, 503 );
		OnRouletteStartAni( rkPacket );
		ErrorReport::SetPosition( 1100, 504 );
		return true;
	case STPK_ROULETTE_EXCEPTION:
		ErrorReport::SetPosition( 1100, 505 );
		OnRouletteException( rkPacket );
		ErrorReport::SetPosition( 1100, 506 );
		return true;
	case STPK_BINGO_DATA:
		ErrorReport::SetPosition( 1100, 507 );
		OnBingoData( rkPacket );
		ErrorReport::SetPosition( 1100, 508 );
		return true;
	case STPK_BINGO_NUMBER_INIT:
		ErrorReport::SetPosition( 1100, 509 );
		OnBingoNumberInit( rkPacket );
		ErrorReport::SetPosition( 1100, 510 );
		return true;	
	case STPK_ON_CONTROL:	
		ErrorReport::SetPosition( 1100, 511 );
		OnRelayControl(rkPacket);
		ErrorReport::SetPosition( 1100, 512 );
		return true;
	case STPK_MEDALITEM_SELL:
		ErrorReport::SetPosition( 1100, 513 );
		OnMedalItemSellResult( rkPacket );
		ErrorReport::SetPosition( 1100, 514 );
		return true;
	case STPK_CUSTOM_MEDALITEM_SELL:
		OnCustomMedalItemSellResult( rkPacket );
		return true;
	case STPK_ETC_ITEM_SEND_PRESENT_FAIL:
		ErrorReport::SetPosition( 1100, 515 );
		OnSendPresentByEtcItemFail( rkPacket );
		ErrorReport::SetPosition( 1100, 516 );
		return true;
	case STPK_SOLDIER_EXP_ADD:
		ErrorReport::SetPosition( 1100, 517 );
		OnSoldierExpAddUpdate( rkPacket );
		ErrorReport::SetPosition( 1100, 518 );
		return true;
	case STPK_EXTRA_ITEM_RECHARGE_TIME:
		ErrorReport::SetPosition( 1100, 519 );
		OnRechargeExtraItem( rkPacket );
		ErrorReport::SetPosition( 1100, 520 );
		return true;
	case STPK_SUBSCRIPTION_BUY:
		ErrorReport::SetPosition( 1100, 523 );
		OnSubscriptionBuy( rkPacket );
		ErrorReport::SetPosition( 1100, 524 );
		return true;
	case STPK_SUBSCRIPTION_RETR_CHECK:
		ErrorReport::SetPosition( 1100, 525 );
		OnSubscriptionRetrCheck( rkPacket );
		ErrorReport::SetPosition( 1100, 526 );
		return true;
	case STPK_SHUFFLEROOM_JOIN_CANCEL:
		ErrorReport::SetPosition( 1100, 527 );
		OnShuffleRoomJoinCancel( rkPacket );
		ErrorReport::SetPosition( 1100, 528 );
		return true;
	case STPK_SHUFFLEROOM_COMMAND:
		ErrorReport::SetPosition( 1100, 529 );
		OnShuffleRoomCmd( rkPacket );
		ErrorReport::SetPosition( 1100, 530 );
		return true;
	case STPK_SHUFFLEROOM_INFO:
		ErrorReport::SetPosition( 1100, 531 );
		OnShuffleRoomInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 532 );
		return true;
	case STPK_SHUFFLEROOM_USER_INFO:
		ErrorReport::SetPosition( 1100, 533 );
		OnShuffleRoomUserInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 534 );
		return true;
	case STPK_SHUFFLEROOM_ENTERUSER_SYNC:
		ErrorReport::SetPosition( 1100, 535 );
		OnShuffleRoomEnterUserSync( rkPacket );
		ErrorReport::SetPosition( 1100, 536 );
		return true;
	case STPK_SHUFFLEROOM_LEAVE:
		ErrorReport::SetPosition( 1100, 537 );
		OnShuffleRoomLeaveUser( rkPacket );
		ErrorReport::SetPosition( 1100, 538 );
		return true;
	case STPK_BUY_SELECT_GASHAPON:
		ErrorReport::SetPosition( 1100, 539 );
		OnSelectGashaponResult( rkPacket );
		ErrorReport::SetPosition( 1100, 540 );
		return true;
	case STPK_BINGO_TYPE:
		ErrorReport::SetPosition( 1100, 541 );
		OnBingoType( rkPacket );
		ErrorReport::SetPosition( 1100, 542 );
		return true;
	case STPK_ITEM_MATERIAL_COMPOUND:
		ErrorReport::SetPosition( 1100, 543 );
		OnItemMaterialCompound( rkPacket );
		ErrorReport::SetPosition( 1100, 544 );
		return true;
	case STPK_PET_CHANGE:
		ErrorReport::SetPosition( 1100, 549 );
		g_PetMgr.OnPetChangeData( rkPacket );
		ErrorReport::SetPosition( 1100, 550 );
		return true;
	case STPK_PET_EQUIP_INFO:
		ErrorReport::SetPosition( 1100, 551 );
		g_PetMgr.OnPetEquipData( rkPacket );
		ErrorReport::SetPosition( 1100, 552 );
		return true;
	case STPK_PET_SELL:
		ErrorReport::SetPosition( 1100, 553 );
		g_PetMgr.OnPetRemoveData( rkPacket );
		ErrorReport::SetPosition( 1100, 554 );
		return true;
	case STPK_PET_ADD:
		ErrorReport::SetPosition( 1100, 555 );
		g_PetMgr.OnPetAdd( rkPacket );
		ErrorReport::SetPosition( 1100, 556 );
		return true;
	case STPK_PET_NURTURE:
		ErrorReport::SetPosition( 1100, 557 );
		g_PetMgr.OnPetNurture( rkPacket );
		ErrorReport::SetPosition( 1100, 558 );
		return true;
	case STPK_PET_NURTURE_LEVELUP:
		ErrorReport::SetPosition( 1100, 559 );
		g_PetMgr.OnPetNurtureLevelUp( rkPacket );
		ErrorReport::SetPosition( 1100, 560 );
		return true;
	case STPK_PET_COMPOUND:
		ErrorReport::SetPosition( 1100, 561 );
		g_PetMgr.OnPetCompound( rkPacket );
		ErrorReport::SetPosition( 1100, 562 );
		return true;
	case STPK_PLAYING_USERDATA_INFO:
		ErrorReport::SetPosition( 1100, 563 );
		OnPlayingUserDataInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 564 );
		return true;
	case STPK_POWER_UP_INFO:
		ErrorReport::SetPosition( 1100, 563 );
		OnPowerUpInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 564 );
		return true;
	case STPK_OBJECT_ITEM_RELEASE:
		ErrorReport::SetPosition( 1100, 565 );
		OnObjectItemRelease( rkPacket );
		ErrorReport::SetPosition( 1100, 566 );
		return true;
	case STPK_COSTUME_BUY:
		ErrorReport::SetPosition( 1100, 565 );
		g_CostumeInfoMgr.OnCostumeBuyResult( rkPacket );
		ErrorReport::SetPosition( 1100, 566 );
		return true;
	case STPK_COSTUME_CHANGE:
		ErrorReport::SetPosition( 1100, 567 );
		g_CostumeInfoMgr.OnCostumeChange( rkPacket, this );
		ErrorReport::SetPosition( 1100, 568 );
		return true;
	case STPK_COSTUME_SELL:
		ErrorReport::SetPosition( 1100, 569 );
		g_CostumeInfoMgr.OnCostumeSellResult( rkPacket );
		ErrorReport::SetPosition( 1100, 570 );
		return true;
	case STPK_COSTUME_PRESENT:
		ErrorReport::SetPosition( 1100, 571 );
		g_CostumeInfoMgr.OnCostumePresentResult( rkPacket );
		ErrorReport::SetPosition( 1100, 572 );
		return true;
	case STPK_USE_SELECT_GASHAPON:
		ErrorReport::SetPosition( 1100, 573 );
		OnSelectGashaponsUseResult( rkPacket );
		ErrorReport::SetPosition( 1100, 574 );
		return true;
	case STPK_PERSONAL_HQ_INFO:
		ErrorReport::SetPosition( 1100, 575 );
		OnHouseInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 576 );
		return true;
	case STPK_PERSONAL_HQ_INVITE:
		ErrorReport::SetPosition( 1100, 577 );
		OnHouseInvite( rkPacket );
		ErrorReport::SetPosition( 1100, 578 );
		return true;
	case STPK_PERSONAL_HQ_INVITE_LIST:
		ErrorReport::SetPosition( 1100, 579 );
		OnHouseInviteList( rkPacket );
		ErrorReport::SetPosition( 1100, 560 );
		return true;
	case STPK_PERSONAL_HQ_JOIN_REQUEST:
		ErrorReport::SetPosition( 1100, 561 );
		OnHouseJoinRequest( rkPacket );
		ErrorReport::SetPosition( 1100, 562 );
		return true;
	case STPK_TIME_CASH_INFO:
		ErrorReport::SetPosition( 1100, 563 );
		OnTimeCashInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 564 );
		return true;
	case STPK_TITLE_UPDATE:
		ErrorReport::SetPosition( 1100, 565 );
		g_NamedTitleInfoMgr.OnNamedTitleUpdate( rkPacket, this );
		ErrorReport::SetPosition( 1100, 566 );
		return true;
	case STPK_TITLE_PREMIUM:
		ErrorReport::SetPosition( 1100, 567 );
		g_NamedTitleInfoMgr.OnNamedTitlePremium( rkPacket );
		ErrorReport::SetPosition( 1100, 568 );
		return true;
	case STPK_TITLE_ETC_USE:
		ErrorReport::SetPosition( 1100, 569 );
		g_NamedTitleInfoMgr.OnNamedTitleEtcUse( rkPacket );
		ErrorReport::SetPosition( 1100, 570 );
		return true;
	case STPK_TITLE_EQUIP_INFO:
		ErrorReport::SetPosition( 1100, 571 );
		g_NamedTitleInfoMgr.OnNamedTitleEquip( rkPacket, this );
		ErrorReport::SetPosition( 1100, 572 );
		return true;
	case STPK_ETC_ITEM_ADD:
		ErrorReport::SetPosition( 1100, 573 );
		OnEtcItemAdd( rkPacket );
		ErrorReport::SetPosition( 1100, 574 );
		return true;
	case STPK_TRADE_LIST_REQ:
		ErrorReport::SetPosition( 1100, 575 );
		g_TradeInfoMgr.ApplyTradeList( rkPacket );
		ErrorReport::SetPosition( 1100, 576 );
		return true;
	case STPK_BONUS_CASH_ADD:
	case STPK_BONUS_CASH_UPDATE:
	case STPK_BONUS_CAHS_INFO:
	case STPK_EXPIRED_BONUS_CASH:
		ErrorReport::SetPosition( 1100, 577 );
		g_BonusCashMgr.ApplyBonusCashData( rkPacket );
		ErrorReport::SetPosition( 1100, 578 );
		return true;
	case STPK_RAID_COIN_INFO:
		ErrorReport::SetPosition( 1100, 579 );
		OnRaidCoinInfo( rkPacket );
		ErrorReport::SetPosition( 1100, 580 );
		return true;
	case STPK_ACCESSORY_CHANGE:
		ErrorReport::SetPosition( 1100, 579 );
		g_AccessoryInfoMgr.OnAccessoryChange( rkPacket, this );
		ErrorReport::SetPosition( 1100, 580 );
		return true;
	case STPK_ACCESSORY_BUY:
		ErrorReport::SetPosition( 1100, 581 );
		g_AccessoryInfoMgr.OnAccessoryBuyResult( rkPacket );
		ErrorReport::SetPosition( 1100, 582 );
		return true;
	case STPK_ACCESSORY_PRESENT:
		ErrorReport::SetPosition( 1100, 583 );
		g_AccessoryInfoMgr.OnAccessoryPresentResult( rkPacket );
		ErrorReport::SetPosition( 1100, 584 );
		return true;
	case STPK_ACCESSORY_SELL:
		ErrorReport::SetPosition( 1100, 569 );
		g_AccessoryInfoMgr.OnAccessorySellResult( rkPacket );
		ErrorReport::SetPosition( 1100, 570 );
		return true;
	case STPK_USER_MATCH_GAMESTART:
		g_RankBattleMgr.OnGameStart( rkPacket );
		return true;
	case STPK_USER_MATCH_GAMEEND:
		g_RankBattleMgr.OnGameEnd( rkPacket );
		return true;
	case STPK_ACCESSORY_RECHARGE_TIME:
		ErrorReport::SetPosition( 1100, 571 );
		OnRechargeAccessory( rkPacket );
		ErrorReport::SetPosition( 1100, 572 );
		return true;
	case STPK_BAD_PING_FLAG_DROP:
		ErrorReport::SetPosition( 1100, 573 );
		OnBadPingFlagDrop( rkPacket );
		ErrorReport::SetPosition( 1100, 574 );
		return true;
	case STPK_ARENA_MODE_CHAR_CREATE:
		ErrorReport::SetPosition( 1100, 575 );
		OnArenaModeCharCreate( rkPacket );  
		ErrorReport::SetPosition( 1100, 576 );
		return true;
	case STPK_ACCESSORY_COMPOSE:
		OnCompoundAccessory( rkPacket );
		return true;
	case STPK_ACCESSORY_REINFORCE:
		OnReinforceAccessory( rkPacket );
		return true;
	case STPK_EXTRAITEM_REINFORCE:
		OnReinforceExtraItem( rkPacket );
		return true;
	case STPK_SUCCESSION_DATA:
		OnSuccessionData( rkPacket );
		return true;
	case STPK_SUCCESSION_MATCHING_REQUEST:
		OnSuccessionMatchingResult( rkPacket );
		return true;
	case STPK_SUCCESSION_MATCHING_CANCEL:
		OnSuccessionMatchingCancel( rkPacket );
		return true;
	case STPK_SUCCESSION_MATCHING_GAMESTART:
		OnSuccessionGameStart( rkPacket );
		return true;
	case STPK_SUCCESSION_MATCHING_GAMEEND:
		OnSuccessionGameEnd( rkPacket );
		return true;
	case STPK_SUCCESSION_REQUEST_REVENGE:
		OnSuccessionRevengeRequest( rkPacket );
		return true;
	case STPK_SUCCESSION_OTHER_USER_INFO:
		OnSuccessionOtherUserInfo( rkPacket );
		return true;
	case STPK_PIECE_TO_PESO:
		OnPieceToPesoPacket( rkPacket );
		break;
	default:
		{
			if( m_pBlockWorld && m_pBlockWorld->ProcessTCPPacket( rkPacket ) )
			{
				return true;
			}

			if( m_pPlayMode )
			{
				return m_pPlayMode->ProcessTCPPacket( rkPacket );
			}
			else
			{
				LOG.PrintTimeAndLog( 0, "ioPlayStage::ProcessTCPPacket - PlayMode is NULL(%x)",
					rkPacket.GetPacketID() );
			}
		}
		return false;
	}

	return false;
}

void ioPlayStage::OnModeInfo( SP2Packet &rkPacket )
{
	int iModeType;
	rkPacket >> iModeType;

	if( m_pPlayMode )
	{
		m_pPlayMode->ApplyModeInfo( rkPacket );
	}
}

void ioPlayStage::OnJoinRoomData( SP2Packet &rkPacket )
{	
	int iRoomStyle, iRoomNum;
	int iPlazaType = 0;
	bool bOpenPlaza = false;
	bool bCharChangeToUDP = false;
	bool bOnlyServerRelay = false;
	rkPacket >> m_iRoomIndex;
	rkPacket >> iRoomNum;
	rkPacket >> iRoomStyle;
	rkPacket >> bCharChangeToUDP;
	rkPacket >> bOnlyServerRelay;
	rkPacket >> m_iModeSubNum;
	rkPacket >> m_iModeMapIndex;

	Help::SetCharChangeToUDP( bCharChangeToUDP );
	Help::SetOnlyServerRelay( bOnlyServerRelay );
	bool bZeroHP, bSafetyRoom;
	rkPacket >> bZeroHP;
	rkPacket >> bSafetyRoom;

	if( iRoomStyle == RSTYLE_PLAZA )
	{
		rkPacket   >> iPlazaType;
		rkPacket   >> bOpenPlaza;
	}

	int iModeType;
	rkPacket >> iModeType;
	
	m_ePlazaType     = (PlazaType)iPlazaType;
	m_bOpenPlaza     = bOpenPlaza;
	g_App.SetRoomNum( iRoomNum );
	m_RoomStyle = (RoomStyle)iRoomStyle;
	ChangePlayMode( (ModeType)iModeType, m_ePlazaType );

	// 다른 오브젝트 정보를 받을때까지 동기화대기
	m_pWorldEntityFactory->EnableElevationSync( false );

	if( m_pPlayMode )
	{
		m_pPlayMode->ApplyModeInfo( rkPacket );
		m_pPlayMode->SetZeroHP( bZeroHP );
		m_pPlayMode->SetSafetyLevelRoom( bSafetyRoom );

		if( g_MyInfo.IsDeleteExerciseChar( m_pPlayMode->GetModeType() , m_pPlayMode->IsSafetyLevelRoom() ) )
		{
			g_MyInfo.DeleteExerciseChar( EXERCISE_GENERAL );
			g_MyInfo.FixSelectChar();
		}
	}

	g_MyInfo.SetCurSafetyLevelRoom( bSafetyRoom );
	g_GUIMgr.ShowReserveUI();
	g_GUIMgr.ShowReserveMsgBox();

	if( Help::IsOnlyServerRelay() )
	{
		// 무조건 서버 중계
		g_App.AllNetworkTableServerRelay();
	}

	//UJ 130910, 최적화: 스레드로딩이 끝날때까지 기다린다(DDS 제외)
	if( g_ResourceLoader.IsNowRunningThread() )
	{
		g_ResourceLoader.StopLoadingThread();
		g_ResourceLoader.ResumeLoadingThread();
	}

#if defined( USE_GA )
	char chClassType[32]	= {0,};
	char chLabel[32]		= {0,};
	char chSlash[16]		= {0,};
	char chPostData[256]	= {0,};
	char chMapIndex[32]		= {0,};
	char chBattleType[32]	= {0,};
	
	// CHAR_SELECT_MAJOR
	for( int i = 0; i < 3; ++i )
	{

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		{
			sprintf_e( chSlash, "%%2F" );
			sprintf_e( chLabel, "%d", i );
			sprintf_e( chClassType, "%d", g_MyInfo.GetClassType( g_MyInfo.GetCharSlotIndexToArray( i ) ) );
			sprintf_e( chPostData, "%sCHAR%sSELECT_MAJOR%s%s", chSlash, chSlash, chSlash, chClassType );
		}
		else
		{
			SafeSprintf( chSlash, sizeof(chSlash), "%2F" );
			SafeSprintf( chLabel, sizeof(chLabel), "%1", i );
			SafeSprintf( chClassType, sizeof(chClassType), "%1", g_MyInfo.GetClassType( g_MyInfo.GetCharSlotIndexToArray( i ) ) );
			SafeSprintf( chPostData, sizeof(chPostData), "%1CHAR%2SELECT_MAJOR%3%4", chSlash, chSlash, chSlash, chClassType );
		}
		
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Character"
			, "Pick"
			, chLabel
			, 1
			, chPostData );
	}	

	// PLAY_CAMP_<MODE>
	if( g_LadderTeamMgr.IsLadderTeam() )
		SafeSprintf( chBattleType, sizeof(chBattleType), "CAMP" );
	// PLAY_LADDER_<MODE>
	else if( g_LadderTeamMgr.IsHeroMatchMode() )
		SafeSprintf( chBattleType, sizeof(chBattleType), "LADDER" );
	// PLAY_BATTLE_<MODE>
	else
		SafeSprintf( chBattleType, sizeof(chBattleType), "BATTLE" );

	// battle type setting
	g_HttpMng.SetBattleType( chBattleType );

	// <MapIndex SubMapIndex>
	if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		sprintf_e( chMapIndex, "%s%d%s%d", chSlash, m_iModeMapIndex, chSlash, m_iModeSubNum );
	else
		SafeSprintf( chMapIndex, sizeof(chMapIndex), "%1%2%3%4", chSlash, m_iModeMapIndex, chSlash, m_iModeSubNum );

	// set battle start time;
	int iTime = (int)REALGETTIME() / 1000;
	g_HttpMng.SetBattleStartTime( iTime );

	// battle mode initialize
	g_HttpMng.SetBattleMode( "NONE" );

	// choice hero duration time, index initialize;
	g_HttpMng.SetHeroStartTime( 0 );
	g_HttpMng.SetHeroEndTime( 0 );
	g_HttpMng.SetHeroIndex( 0 );

	// 용병 교체 관련 데이터 전송 flag
	g_HttpMng.SetSendHeroChange( true );

	switch( iModeType )
	{
	case MT_SYMBOL:
		{
			// PLAY_CAMP_POWERSTONE_<MapIndex-SubMapIndex>
			// PLAY_LADDER_POWERSTONE_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_POWERSTONE_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sPOWERSTONE%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4POWERSTONE%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "POWERSTONE" );
		}
		break;
	case MT_CATCH:
		{
			// PLAY_CAMP_PRISONER_<MapIndex-SubMapIndex>
			// PLAY_LADDER_PRISONER_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_PRISONER_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sPRISONER%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4PRISONER%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "PRISONER" );
		}
		break;
	case MT_UNDERWEAR:
		{
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sUNDERWEAR%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4UNDERWEAR%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "UNDERWEAR" );
		}
		break;

	case MT_CBT:
		{
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sCBT%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4CBT%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "CBT" );
		}
		break;

	case MT_KING:
		{
			// PLAY_CAMP_HIDENCROWN_<MapIndex-SubMapIndex>
			// PLAY_LADDER_HIDENCROWN_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_HIDENCROWN_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sHIDENCROWN%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4HIDENCROWN%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "HIDENCROWN" );
		}
		break;
	case MT_TRAINING:
		{
			// PLAY_PLAZA_CONNECT_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%sPLAZA%sCONNECT%s", chSlash, chSlash, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2PLAZA%3CONNECT%4", chSlash, chSlash, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "PLAZA" );

			// 용병 교체 관련 데이터 전송 flag
			g_HttpMng.SetSendHeroChange( false );
		}
		break;
	case MT_SURVIVAL:
		{
			// PLAY_CAMP_DEATH_<MapIndex-SubMapIndex>
			// PLAY_LADDER_DEATH_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_DEATH_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sDEATH%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4DEATH%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "DEATH" );
		}
		break;
	case MT_TEAM_SURVIVAL:
		{
			// PLAY_CAMP_TEAMDEATH_<MapIndex-SubMapIndex>
			// PLAY_LADDER_TEAMDEATH_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_TEAMDEATH_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sTEAMDEATH%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4TEAMDEATH%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "TEAMDEATH" );
		}
		break;
	case MT_BOSS:
		{
			// PLAY_CAMP_BOSSRAID_<MapIndex-SubMapIndex>
			// PLAY_LADDER_BOSSRAID_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_BOSSRAID_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sBOSSRAID%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4BOSSRAID%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "BOSSRAID" );
		}
		break;
	case MT_MONSTER_SURVIVAL:
		{
			// PLAY_BATTLE_SKELETON
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FPLAY%2FBATTLE%2FSKELETON" );

			// battle mode setting
			g_HttpMng.SetBattleMode( "SKELETON" );
		}
		break;
	case MT_FOOTBALL:
		{
			// PLAY_CAMP_SOCCER_<MapIndex-SubMapIndex>
			// PLAY_LADDER_SOCCER_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_SOCCER_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sSOCCER%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4SOCCER%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "SOCCER" );
		}
		break;
	case MT_HEROMATCH:
		{
			// PLAY_CAMP_LADDER_<MapIndex-SubMapIndex>
			// PLAY_LADDER_LADDER_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_LADDER_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sLADDER%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4LADDER%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "LADDER" );
		}
		break;
	case MT_GANGSI:
		{
			// PLAY_CAMP_GANGSI_<MapIndex-SubMapIndex>
			// PLAY_LADDER_GANGSI_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_GANGSI_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sGANGSI%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4GANGSI%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "GANGSI" );
		}
		break;
	case MT_HEADQUARTERS:
		{
			// PLAY_HQ_CONNECT
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FPLAY%2FHQ%2FCONNECT" );

			// battle mode setting
			g_HttpMng.SetBattleMode( "HQ" );

			// 용병 교체 관련 데이터 전송 flag
			g_HttpMng.SetSendHeroChange( false );
		}
		break;
	case MT_CATCH_RUNNINGMAN:
		{
			// PLAY_CAMP_DWARF_<MapIndex-SubMapIndex>
			// PLAY_LADDER_DWARF_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_DWARF_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sDWARF%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4DWARF%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "DWARF" );
		}
		break;
	case MT_FIGHT_CLUB:
		{
			// PLAY_CAMP_CHAMP_<MapIndex-SubMapIndex>
			// PLAY_LADDER_CHAMP_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_CHAMP_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sCHAMP%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4CHAMP%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "CHAMP" );
		}
		break;
	case MT_TOWER_DEFENSE:
		{
			// PLAY_BATTLE_FOREST
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FPLAY%2FBATTLE%2FFOREST" );

			// battle mode setting
			g_HttpMng.SetBattleMode( "FOREST" );
		}
		break;
	case MT_DARK_XMAS:
		{
			// PLAY_BATTLE_SNOW
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FPLAY%2FBATTLE%2FSNOW" );

			// battle mode setting
			g_HttpMng.SetBattleMode( "SNOW" );
		}
		break;
	case MT_FIRE_TEMPLE:
		{
			// PLAY_BATTLE_FIRE
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FPLAY%2FBATTLE%2FFIRE" );

			// battle mode setting
			g_HttpMng.SetBattleMode( "FIRE" );
		}
		break;
	case MT_DOUBLE_CROWN:
		{
			// PLAY_CAMP_DOUBLECROWN_<MapIndex-SubMapIndex>
			// PLAY_LADDER_DOUBLECROWN_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_DOUBLECROWN_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sDOUBLECROWN%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4DOUBLECROWN%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "DOUBLECROWN" );
		}
		break;
	case MT_SHUFFLE_BONUS:
		{
			// PLAY_CAMP_BONUS_<MapIndex-SubMapIndex>
			// PLAY_LADDER_BONUS_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_BONUS_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sBONUS%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4BONUS%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "BONUS" );
		}
		break;
	case MT_FACTORY:
		{
			// PLAY_BATTLE_FACTORY
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FPLAY%2FBATTLE%2FFACTORY" );

			// battle mode setting
			g_HttpMng.SetBattleMode( "FACTORY" );
		}
		break;
	case MT_TEAM_SURVIVAL_AI:
		{
			// PLAY_CAMP_TEAMDEATH_AI_<MapIndex-SubMapIndex>
			// PLAY_LADDER_TEAMDEATH_AI_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_TEAMDEATH_AI_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sTEAMDEATH_AI%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4TEAMDEATH_AI%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "TEAMDEATH_AI" );
		}
		break;
	case MT_HOUSE:
		break;
	case MT_RAID:
		break;
	case MT_FLAG_CAPTURE:
		{
			// PLAY_CAMP_DOUBLECROWN_<MapIndex-SubMapIndex>
			// PLAY_LADDER_DOUBLECROWN_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_DOUBLECROWN_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sFLAGCAPTURE%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4FLAGCAPTURE%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "FLAGCAPTURE" );
		}
		break;
	case MT_ARENA:
		{
			// PLAY_CAMP_PRISONER_<MapIndex-SubMapIndex>
			// PLAY_LADDER_PRISONER_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_PRISONER_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sPRISONER%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4PRISONER%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "ARENA" );
		}
		break;
	case MT_BATTLE:
		{
			// PLAY_CAMP_TEAMDEATH_<MapIndex-SubMapIndex>
			// PLAY_LADDER_TEAMDEATH_<MapIndex-SubMapIndex>
			// PLAY_BATTLE_TEAMDEATH_<MapIndex-SubMapIndex>
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sBATTLE%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4BATTLE%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "BATTLE" );
		}
		break;
	case MT_FARMING:
		{
			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chPostData, "%sPLAY%s%s%sFARMING%s", chSlash, chSlash, chBattleType, chSlash, chMapIndex );
			else
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4FARMING%5", chSlash, chSlash, chBattleType, chSlash, chMapIndex );

			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData );

			// battle mode setting
			g_HttpMng.SetBattleMode( "FARMING" );
		}
		break;
	}
#endif
}

void ioPlayStage::OnHostUserID( SP2Packet &rkPacket )
{
	ioHashString szHostName;
	rkPacket >> szHostName;

	g_MyInfo.SetHostName(szHostName);

	if( g_MyInfo.IsHost() )
	{
		if( m_pWorldEntityFactory )
		{
			// 최초 룸생성시 내가 호스트로 시작하는 경우에 작동한다.
			// 중간 호스트변경시에는 작동안함 ( 이미 가동시작했을것이므로.. )
			EnableWorldEntityFunctions( true );
		}
	}
}

void ioPlayStage::OnJoinPlayingUserData( SP2Packet &rkPacket )
{
	OnJoinUserData( rkPacket );
	OnCharEquipData( rkPacket );
	OnCharEquipMedalItemData( rkPacket );
	OnCharGrowthLevelData( rkPacket );
	OnExpandMedalSlotData( rkPacket );

	ioHashString szName;
	if( m_pPlayMode )
	{
		rkPacket >> szName;	
		m_pPlayMode->ApplyCharModeInfo( szName, rkPacket, true );

		ioBaseChar *pPlayingChar = GetBaseChar( szName );
		if( pPlayingChar )
		{
			switch( pPlayingChar->GetState() )
			{
			case CS_VEHICLE_DRIVER:
			case CS_PASSENGER:			
				return;
			}

			if( pPlayingChar->IsSystemState() )
				return; 

			if( !pPlayingChar->IsPrisonerMode() && !pPlayingChar->IsCatchMode() && !pPlayingChar->IsExcavating() )
			{
				pPlayingChar->SetState( CS_DELAY );
			}
		}
	}
}

void ioPlayStage::OnJoinUserData( SP2Packet &rkPacket )
{
	int iPort, iTransferPort;
	int iLevel,iClassLevel,iRCharCount, nSoldierSetCnt, iExcavationLevel;
	DWORD dwUserIndex,dwCharIndex, dwGuildIndex, dwGuildMark;
	ioHashString szName, szPublicIP, szPrivateIP, szTransferIP;

	rkPacket >> szName >> iRCharCount >> nSoldierSetCnt >> dwGuildIndex >> dwGuildMark >> dwCharIndex >> iLevel >> iClassLevel >> iExcavationLevel;
	rkPacket >> dwUserIndex >> szPublicIP >> iPort >> szPrivateIP >> szTransferIP >> iTransferPort;

	//서버가 중복으로 주는경우가 있는거 같다. (윤태철)
	if( m_pPlayMode )
	{
		ioBaseChar* pRecordChar = m_pPlayMode->GetRecordChar( szName );
		if( pRecordChar )
		{
			LOG.PrintTimeAndLog( 0, "ioPlayStage::OnJoinUserData - OverLap AddNewUer( %s )", szName );
			return;
		}
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnJoinUserData - No Mode" );
	}

	ioBaseChar *pChar = GetBaseChar( szName );
	if( pChar )
	{
		LOG.PrintTimeAndLog( 0, "~~~~~~~~~~~~~~~~ioPlayStage::OnJoinUserData - OverLap AddNewUer2( %s )~~~~~~~~~~~~~", szName );
		return;
	}
	
	g_GuildInfo.SetGuildMark( szName, dwGuildIndex, dwGuildMark );

	g_VictoriesMgr.EnterUser( szName );
	g_PlayBonusMgr.EnterUser( szName );

	bool bPlayState = true;
	bool bChatMode	= false;

	ioBaseChar *pJoinChar = AddNewUser( szName, rkPacket );
	if( pJoinChar )
	{
		pJoinChar->SetIndex( dwCharIndex );
		pJoinChar->SetLevel( iLevel );
		pJoinChar->SetClassLevel( iClassLevel );

		TeamType eTeam = pJoinChar->GetTeam();
		D3DXVECTOR3 vStartPos( 0.0f, 0.0f, 0.0f );
		if( m_pPlayMode )
		{
			vStartPos = m_pPlayMode->GetStartPos( eTeam );
			// 배틀모드 일때
			if(m_pPlayMode->GetModeType() == MT_BATTLE)
			{
				// 첫번째 출전이 아닐 경우
				if(g_BattleRoomMgr.GetUserBattleOrder(szName) != 1)
				{
					ioBattleMode* pBattleMode = ToBattleMode(m_pPlayMode);
					if(pBattleMode)
						vStartPos = pBattleMode->GetWaitingPos(pJoinChar, eTeam);
				}
			}

		}
		pJoinChar->SetWorldPosition( vStartPos );
		pJoinChar->FillMaxHP();

		int iMyVictories;
		rkPacket >> iMyVictories;

		if( g_LadderTeamMgr.IsLadderTeam() )
			g_VictoriesMgr.SetVictories( szName, iMyVictories );

		int iAbilityLevel, iLadderPoint;
		rkPacket >> bPlayState >> iAbilityLevel >> iLadderPoint;

		if( g_MyInfo.GetPublicID() == pJoinChar->GetCharName() )
		{
			g_MyInfo.SetAbilityLevel( iAbilityLevel );
		}

		if( !bPlayState )
		{
			pJoinChar->SetState( CS_LOADING );
		}

		bool bFishingState;
		rkPacket >> bChatMode >> bFishingState;

		int iRodType, iBaitType;
		rkPacket >> iRodType >> iBaitType;

		if( bFishingState )
		{
			pJoinChar->ApplyFishingLoopState( iRodType, iBaitType );
		}

		g_ExcavationMgr.CheckUserExcavationLv( szName, iExcavationLevel );

		bool bStealth;
		rkPacket >> bStealth;
		if( bStealth )
			pJoinChar->SetState( CS_OBSERVER );

		bool bExperienceMode;
		rkPacket >> bExperienceMode;
		if( bExperienceMode )
		{
			int  iExperienceClassType;
			ioHashString szExperienceID;
			rkPacket >> szExperienceID >> iExperienceClassType;
			pJoinChar->SetExperienceMode( EMS_EXPERIENCE, szExperienceID, iExperienceClassType );
		}

		if( m_pPlayMode )
			m_pPlayMode->AddNewRecord( pJoinChar );

		//R 용병 정보 추가
		pJoinChar->SetCurRSoldierCount( iRCharCount );		
		pJoinChar->SetCurSoldierSetCnt( nSoldierSetCnt );

		// 용병 정보 수집
		if( g_MyInfo.GetPublicID() != szName )
		{
			g_GlobalSoldierInfo.SyncSoldierData( pJoinChar->GetIndex(), pJoinChar->GetCharacterInfo() );
			pJoinChar->SetAwakeChangeEffect();
		}

		if( g_MyInfo.GetPublicID() != szName )
		{
			if( m_pPlayMode )
				m_pPlayMode->AddGrowthLevel( pJoinChar );
		}

		g_UserInfoMgr.SetEnterRoomUserSync( szName, iLevel, iLadderPoint );
	}

	if( g_MyInfo.GetPublicID() == szName )
	{
		if( bChatMode && pJoinChar )
		{
			pJoinChar->SetChatMode( CMS_NONE );
			pJoinChar->SetChatModeReserve( true );
		}

		g_MyInfo.SetMyAddress( szPublicIP, iPort );
	}
	else
	{
		if( bChatMode && pJoinChar )
		{
			pJoinChar->SetChatMode( CMS_CHAT );
		}

		g_App.AddNetworkTable( dwUserIndex, szName, szPublicIP, szPrivateIP, szTransferIP, iPort, iTransferPort, P2P_STYLE_PLAY, 4 );
		if( ( GetModeType() == MT_HEADQUARTERS || GetModeType() == MT_HEADQUARTERS || IsCustomPlazaMode() ) && 
			!g_BattleRoomMgr.IsBattleRoom() && !g_LadderTeamMgr.IsLadderTeam() && 
			!g_ShuffleRoomMgr.IsShuffleRoom() && !g_RankBattleMgr.IsRankBattlePlaying() )
		{
			g_App.AddNetworkTable( dwUserIndex, szName, szPublicIP, szPrivateIP, szTransferIP, iPort, iTransferPort, P2P_STYLE_VOICE, 5 );

#ifndef SHIPPING
			LOG.PrintTimeAndLog( 0, "%s Add Voice Table | %s", __FUNCTION__, szName.c_str() );
#endif
			if ( Setting::UseVoiceChat() && Setting::UseMIC() )
			{
				SP2Packet kPacket( CTPK_VOICE_INFO );
				kPacket << ID_VOICE_ON;
				kPacket << szName;
				TCPNetwork::SendToServer( kPacket );
#ifndef SHIPPING
				LOG.PrintTimeAndLog( 0, "%s CTPK_VOICE_INFO | ID_VOICE_ON | %s", __FUNCTION__, szName.c_str() );
#endif
			}
		}

		ioBaseChar *pOwnerChar = GetOwnerChar();
		if( pOwnerChar && pJoinChar )
		{
			if( ioWeaponManager::GetSingletonPtr() )
			{
				for( int i = MIT_MINE; i < MIT_MAX_CNT; i++ )
				{
					int iMineCnt = g_WeaponMgr.GetCurMineWeaponCnt( pOwnerChar, (MineType)i );
					if( iMineCnt > 0 )
					{
						SP2Packet kPacket( CTPK_PASSAGE );
						kPacket << pJoinChar->GetCharName();
						kPacket << pOwnerChar->GetCharName();
						kPacket << PAS_MINE_INFO;
						kPacket << iMineCnt;
						g_WeaponMgr.FillMineInfo( pOwnerChar, kPacket, (MineType)i );
						TCPNetwork::SendToServer( kPacket );
					}
				}
			}

			//if( pOwnerChar->GetCharName() == g_BattleRoomMgr.GetOwnerName() )
			{
				SP2Packet kDummyCharIndexPacket( CTPK_PASSAGE );
				kDummyCharIndexPacket << pJoinChar->GetCharName();
				kDummyCharIndexPacket << pOwnerChar->GetCharName();
				kDummyCharIndexPacket << PAS_DUMMY_INDEX_INFO;
				kDummyCharIndexPacket << g_DummyCharMgr.GetCurLocalIndex( false );
				TCPNetwork::SendToServer( kDummyCharIndexPacket );
			}

			int iDummyCharCnt = g_DummyCharMgr.GetDummyCharCnt( pOwnerChar, true );
			if( iDummyCharCnt > 0 )
			{
				SP2Packet kDummyCharPacket( CTPK_PASSAGE );
				kDummyCharPacket << pJoinChar->GetCharName();
				kDummyCharPacket << pOwnerChar->GetCharName();
				kDummyCharPacket << PAS_DUMMY_CHAR_INFO;
				kDummyCharPacket << iDummyCharCnt;
				g_DummyCharMgr.FillDummyCharInfo( pOwnerChar, kDummyCharPacket );

				ioDummyCharItem *pDummyItem = ToDummyCharItem( pOwnerChar->GetWeapon() );
				if( pDummyItem )
				{
					kDummyCharPacket << true;
					kDummyCharPacket << pDummyItem->GetCurDummyCharIndex();
				}
				else
				{
					kDummyCharPacket << false;
				}
				TCPNetwork::SendToServer( kDummyCharPacket );
			}

			iDummyCharCnt = g_DummyCharMgr.GetDummyCharCnt( pOwnerChar, true );
			if( iDummyCharCnt > 0 )
			{
				SP2Packet kPacket( CTPK_PASSAGE );
				kPacket << pJoinChar->GetCharName();
				kPacket << pOwnerChar->GetCharName();
				kPacket << PAS_DUMMY_MID_SYNC;
				g_DummyCharMgr.FillDummyCharMidSync( pOwnerChar, kPacket );
				TCPNetwork::SendToServer( kPacket );
			}

			int iDummyGroupCnt = g_DummyCharMgr.GetDummyGroupCount( pOwnerChar->GetCharName() );
			if( iDummyGroupCnt > 0 )
			{
				SP2Packet kPacket( CTPK_PASSAGE );
				kPacket << pJoinChar->GetCharName();
				kPacket << pOwnerChar->GetCharName();
				kPacket << PAS_GROUP_DUMMY;				
				g_DummyCharMgr.FillDummyGroupInfo( pOwnerChar, kPacket );
				TCPNetwork::SendToServer( kPacket );
			}

			if( pOwnerChar->IsObejctEquipState( OES_WEREWOLF ) )
			{
				SP2Packet kWereWolfPacket( CTPK_PASSAGE );
				kWereWolfPacket << pJoinChar->GetCharName();
				kWereWolfPacket << pOwnerChar->GetCharName();
				kWereWolfPacket << PAS_WEREWOLF_INFO;
				pOwnerChar->FillWereWolfStateInfo( kWereWolfPacket );
				TCPNetwork::SendToServer( kWereWolfPacket );
			}

			if( pOwnerChar->IsSuddenAttackState() )
			{
				SP2Packet kSuddenPacket( CTPK_PASSAGE );
				kSuddenPacket << pJoinChar->GetCharName();
				kSuddenPacket << pOwnerChar->GetCharName();
				kSuddenPacket << PAS_SUDDEN_ATTACK_INFO;
				pOwnerChar->FillSuddenAttackStateInfo( kSuddenPacket );
				TCPNetwork::SendToServer( kSuddenPacket );
			}

			if( pOwnerChar->IsTopBlAttackState() )
			{
				SP2Packet kSuddenPacket( CTPK_PASSAGE );
				kSuddenPacket << pJoinChar->GetCharName();
				kSuddenPacket << pOwnerChar->GetCharName();
				kSuddenPacket << PAS_TOPBL_EXTEND_MODE;
				TCPNetwork::SendToServer( kSuddenPacket );
			}

			if( pOwnerChar->IsSuperDimensionWeaponIncState() )
			{
				SP2Packet kSuperDimensionPacket( CTPK_PASSAGE );
				kSuperDimensionPacket << pJoinChar->GetCharName();
				kSuperDimensionPacket << pOwnerChar->GetCharName();
				kSuperDimensionPacket << PAS_SUPER_DIMENSION;
				pOwnerChar->FillSuperDimensionWeaponInfo( kSuperDimensionPacket );
				TCPNetwork::SendToServer( kSuperDimensionPacket );
			}

			if( pOwnerChar->IsObejctEquipState( OES_GP ) )
			{
				SP2Packet kGPPacket( CTPK_PASSAGE );
				kGPPacket << pJoinChar->GetCharName();
				kGPPacket << pOwnerChar->GetCharName();
				kGPPacket << PAS_YUMIRA_INFO;
				pOwnerChar->FillGPStateInfo( kGPPacket );
				TCPNetwork::SendToServer( kGPPacket );
			}

			if( pOwnerChar->IsObejctEquipState( OES_GALAXY ) )
			{
				SP2Packet kGPPacket( CTPK_PASSAGE );
				kGPPacket << pJoinChar->GetCharName();
				kGPPacket << pOwnerChar->GetCharName();
				kGPPacket << PAS_GALAXY;
				pOwnerChar->FillGalaxyInfo( kGPPacket );
				TCPNetwork::SendToServer( kGPPacket );
			}

			if( pOwnerChar->IsObejctEquipState( OES_THUNDER_BIRD ) )
			{
				SP2Packet kGPPacket( CTPK_PASSAGE );
				kGPPacket << pJoinChar->GetCharName();
				kGPPacket << pOwnerChar->GetCharName();
				kGPPacket << PAS_THUNDER_BIRD;
				pOwnerChar->FillThunderBirdInfo( kGPPacket );
				TCPNetwork::SendToServer( kGPPacket );
			}

			if( pOwnerChar->IsObejctEquipState( OES_ALICE ) )
			{
				SP2Packet kGPPacket( CTPK_PASSAGE );
				kGPPacket << pJoinChar->GetCharName();
				kGPPacket << pOwnerChar->GetCharName();
				kGPPacket << PAS_ALICE;
				pOwnerChar->FillAliceInfo( kGPPacket );
				TCPNetwork::SendToServer( kGPPacket );
			}

			if( pOwnerChar->IsObejctEquipState( OES_PARKILPYO ) )
			{
				SP2Packet kGPPacket( CTPK_PASSAGE );
				kGPPacket << pJoinChar->GetCharName();
				kGPPacket << pOwnerChar->GetCharName();
				kGPPacket << PAS_PARKILPYO;
				pOwnerChar->FillParkilpyoInfo( kGPPacket );
				TCPNetwork::SendToServer( kGPPacket );
			}

			if( pOwnerChar->IsObejctEquipState( OES_ONIMUSHA ) )
			{
				SP2Packet kGPPacket( CTPK_PASSAGE );
				kGPPacket << pJoinChar->GetCharName();
				kGPPacket << pOwnerChar->GetCharName();
				kGPPacket << PAS_ONIMUSHA;
				pOwnerChar->FillOnimushaInfo( kGPPacket );
				TCPNetwork::SendToServer( kGPPacket );
			}

			ioDarkKnightItem* pDarkKnight = ToDarkKnightItem( pOwnerChar->GetWeapon() );
			if( pDarkKnight )
			{				
				SP2Packet kStatePacket( CTPK_PASSAGE );
				kStatePacket << pJoinChar->GetCharName();
				kStatePacket << pOwnerChar->GetCharName();
				kStatePacket << PAS_DARKKNIGHT;
				//다크나이트의 오브젝트 아이템 장착에 따른 중간 동기화는 FillInfo안에서 체크
				pDarkKnight->FillInfo( pOwnerChar, kStatePacket );
				TCPNetwork::SendToServer( kStatePacket );
			}			

			if( pOwnerChar->IsObejctEquipState( OES_DRUID ) )
			{
				SP2Packet kGPPacket( CTPK_PASSAGE );
				kGPPacket << pJoinChar->GetCharName();
				kGPPacket << pOwnerChar->GetCharName();
				kGPPacket << PAS_DRUID;
				pOwnerChar->FillDruidInfo( kGPPacket );
				TCPNetwork::SendToServer( kGPPacket );
			}

			ioGeomjeItem* pGeomJe = ToGeomjeItem( pOwnerChar->GetWeapon() );
			if( pGeomJe )
			{
				SP2Packet kStatePacket( CTPK_PASSAGE );
				kStatePacket << pJoinChar->GetCharName();
				kStatePacket << pOwnerChar->GetCharName();
				kStatePacket << PAS_GEOMJE;
				pGeomJe->FillInfo( pOwnerChar, kStatePacket );
				TCPNetwork::SendToServer( kStatePacket );
			}

			ioResearcher* pResearcher = ToResearcher( pOwnerChar->GetWeapon() );
			if( pResearcher )
			{
				SP2Packet kStatePacket( CTPK_PASSAGE );
				kStatePacket << pJoinChar->GetCharName();
				kStatePacket << pOwnerChar->GetCharName();
				kStatePacket << PAS_RESSEARCHER;
				pResearcher->FillInfo( pOwnerChar, kStatePacket );
				TCPNetwork::SendToServer( kStatePacket );
			}

			if( pOwnerChar->IsObejctEquipState( OES_MIDNIGHT_SCYTHE ) )
			{
				SP2Packet kGPPacket( CTPK_PASSAGE );
				kGPPacket << pJoinChar->GetCharName();
				kGPPacket << pOwnerChar->GetCharName();
				kGPPacket << PAS_MIDNIGHT_SCYTHE;
				pOwnerChar->FillMidnightScytheInfo( kGPPacket );
				TCPNetwork::SendToServer( kGPPacket );
			}

			if( pOwnerChar->IsObejctEquipState( OES_MIDNIGHT_GUN ) )
			{
				SP2Packet kGPPacket( CTPK_PASSAGE );
				kGPPacket << pJoinChar->GetCharName();
				kGPPacket << pOwnerChar->GetCharName();
				kGPPacket << PAS_MIDNIGHT_GUN;
				pOwnerChar->FillMidnightGunInfo( kGPPacket );
				TCPNetwork::SendToServer( kGPPacket );
			}

			// 이브 관전 OR 입장 2020-11-24 
			ioEvaItem* pEvaItem = ToEvaItem( pOwnerChar->GetWeapon() );
			if( pEvaItem )
			{
				SP2Packet kGPPacket( CTPK_PASSAGE );
				kGPPacket << pJoinChar->GetCharName();
				kGPPacket << pOwnerChar->GetCharName();
				kGPPacket << PAS_EVA_SUB_WEAPON;
				pOwnerChar->FillEvaSubWeaponInfo( kGPPacket );
				TCPNetwork::SendToServer( kGPPacket );
			}

			// 마그네틱 관전 OR 입장 2020-12-16 
			ioMagneticItem* pMagneticItem = ToMagneticItem( pOwnerChar->GetWeapon() );
			if( pMagneticItem )
			{
				SP2Packet kGPPacket( CTPK_PASSAGE );
				kGPPacket << pJoinChar->GetCharName();
				kGPPacket << pOwnerChar->GetCharName();
				kGPPacket << PAS_MAGNETIC_WEAPON;
				pOwnerChar->FillMagneticWeaponInfo( kGPPacket );
				TCPNetwork::SendToServer( kGPPacket );
			}

			ioSpecialStateBase* pBase = pOwnerChar->GetSpecialState();
			if( pBase && pBase->IsJoinSync() )
			{
				SP2Packet kStatePacket( CTPK_PASSAGE );
				kStatePacket << pJoinChar->GetCharName();
				kStatePacket << pOwnerChar->GetCharName();
				kStatePacket << PAS_SPECIAL_STATE;
				kStatePacket << (int)pOwnerChar->GetState();
				pBase->FillJoinSync( kStatePacket, pOwnerChar );
				TCPNetwork::SendToServer( kStatePacket );
			}

			pOwnerChar->SendItemGauge( pJoinChar->GetCharName() );
			pOwnerChar->SendBuffList( pJoinChar->GetCharName() );
			pOwnerChar->SendEtcItemMotionState( pJoinChar->GetCharName() );
		}

		// 친구 정보 갱신
		if( GetRoomStyle() == RSTYLE_LADDERBATTLE )
			g_FriendManager.ClientFriendInfoUpdate( szName, iLevel, UP_LADDER_TEAM );
		else if( GetRoomStyle() == RSTYLE_BATTLEROOM || GetRoomStyle() == RSTYLE_SHUFFLEROOM || GetRoomStyle() == RSTYLE_MATCHROOM )
			g_FriendManager.ClientFriendInfoUpdate( szName, iLevel, UP_BATTLE_ROOM );
	}

	// 서버에서 세팅된 팀으로 변경
	if( pJoinChar )
	{
		if( g_BattleRoomMgr.IsBattleRoom() )
		{
			if( GetModeType() != MT_SURVIVAL && GetModeType() != MT_BOSS && GetModeType() != MT_MONSTER_SURVIVAL && 
				GetModeType() != MT_GANGSI && GetModeType() != MT_DUNGEON_A && GetModeType() != MT_FIGHT_CLUB && GetModeType() != MT_RAID && 
				!(Help::IsMonsterDungeonMode(GetModeType())) )
				g_BattleRoomMgr.SetBattleTeam( pJoinChar->GetCharName(), pJoinChar->GetTeam() );
		}
		else if( g_ShuffleRoomMgr.IsShuffleRoom() )
		{
			if( GetModeType() != MT_SURVIVAL && GetModeType() != MT_BOSS && GetModeType() != MT_MONSTER_SURVIVAL && 
				GetModeType() != MT_GANGSI && GetModeType() != MT_DUNGEON_A && GetModeType() != MT_FIGHT_CLUB && GetModeType() != MT_SHUFFLE_BONUS && GetModeType() != MT_RAID && 
				!(Help::IsMonsterDungeonMode(GetModeType())) )
				g_ShuffleRoomMgr.SetShuffleTeam( pJoinChar->GetCharName(), pJoinChar->GetTeam() );
		}
	}

	if( m_pPlayMode )
	{
		// 보스 유저로 시작한 유저 기록
		if( GetModeType() == MT_BOSS )
		{
			if( pJoinChar && pJoinChar->GetTeam() == TEAM_BLUE )
			{
				ioBossMode *pBossMode = ToBossMode( m_pPlayMode );
				if( pBossMode )
					pBossMode->InsertBossHistory( pJoinChar->GetCharName() );
			}
		}

		//신규 유저가 들어오면 기여도 정산을 다시한다.
		m_pPlayMode->ContributeConvert();
		m_pPlayMode->ContributeRoundAllPercent();
		m_pPlayMode->UpdateRoundInfoList();
		m_pPlayMode->CheckTotalChamp( false );

		if( g_MyInfo.IsHost() && pJoinChar )
		{
			// 내가 방장이고, 조인한 것이 내가 아니면
			if( pJoinChar->GetCharName() != g_MyInfo.GetPublicID() )
			{
				//신규 유저에게 현재까지의 기여도 진행상황을 전송한다.
				SP2Packet kPacket( CTPK_PLAYRECORD_INFO );
				kPacket << pJoinChar->GetCharName();
				m_pPlayMode->FillPlayRecordInfo( kPacket );
				TCPNetwork::SendToServer( kPacket );

				g_AwardMgr.SendAwardInfoSync( pJoinChar->GetCharName() );

				if( rkPacket.GetPacketID() == STPK_JOIN_NEW_USERDATA )
				{
					SP2Packet kModeInfo( CTPK_PASSAGE );
					kModeInfo << pJoinChar->GetCharName();
					kModeInfo << g_MyInfo.GetPublicID();
					kModeInfo << PAS_MODE_SPECIAL_INFO;

					if( m_pAreaWeaponMgr )
						m_pAreaWeaponMgr->GetAreaWeaponInfo( kModeInfo );
					if( m_pPushStructMgr ) 
						m_pPushStructMgr->GetPushStructInfo( kModeInfo );
					if( m_pBallMgr )
						m_pBallMgr->GetBallStructInfo( kModeInfo );

					m_pPlayMode->GetModeSpecialInfo( kModeInfo );

					kModeInfo << m_iRedForBalance;
					kModeInfo << m_iBlueForBalance;
					TCPNetwork::SendToServer( kModeInfo );
				}
			}			
		}
	}

	// 이 패킷은 여러군데서 사용하므로 STPK_JOIN_NEW_USERDATA 패킷만 아래 구문을 사용한다.
	if( rkPacket.GetPacketID() == STPK_JOIN_NEW_USERDATA )
	{
		if( pJoinChar && m_pPlayMode )
			m_pPlayMode->ApplyNewUserModeInfo( pJoinChar->GetCharName(), rkPacket );
	}
}

void ioPlayStage::OnLeaveUserData( SP2Packet &rkPacket )
{
	if( GetModeType() == MT_MYROOM )
		return;

	ioHashString szName;
	rkPacket >> szName;

	BYTE eKickType = 0;
	rkPacket >> eKickType;

	ioBaseChar *pExitUser = GetBaseChar( szName );
	if( pExitUser )
	{
		if( m_szRoomKickOutUser != pExitUser->GetCharName() )
		{
			if( !g_BattleRoomMgr.IsObserverUser( pExitUser->GetCharName() ) && pExitUser->GetTeam() != TEAM_NONE )
			{
				switch( (RoomLeaveType)eKickType )
				{
				case RLT_KICK:
					g_ChatMgr.SetKickUserChatMsg( pExitUser->GetCharName().c_str(), pExitUser->GetTeam() );
					break;
				case RLT_BADNETWORK:
					g_ChatMgr.SetBadNetworkKickUserChatMsg( pExitUser->GetCharName().c_str(), pExitUser->GetTeam() );
					break;
				case RLT_HIGH_LEVEL:
					g_ChatMgr.SetKickHighLevelUserChatMsg( pExitUser->GetCharName().c_str(), pExitUser->GetTeam() );
					break;
				case RLT_LOW_LEVEL:
					g_ChatMgr.SetKickLowLevelUserChatMsg( pExitUser->GetCharName().c_str(), pExitUser->GetTeam() );
					break;
				case RLT_SYS_ERROR:
					g_ChatMgr.SetKickSysError( pExitUser->GetCharName().c_str(), pExitUser->GetTeam() );
					break;
				default:
					if( GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE)
					{
						g_ChatMgr.SetOutUserChatMsg( pExitUser->GetCharName().c_str(), pExitUser->GetTeam() );
					}
					break;
				}					
			}
		}		
		else
		{
			m_szRoomKickOutUser.Clear();
		}

		if( pExitUser->GetState() != CS_DIE &&
			pExitUser->GetState() != CS_VIEW &&
			pExitUser->GetState() != CS_LOADING &&
			pExitUser->GetState() != CS_OBSERVER )
		{
			DecreaseTeamCntForBalance( pExitUser->GetTeam() );
		}

		if( m_pPlayMode )
		{
			m_pPlayMode->RemoveGrowthLevel( pExitUser );
		}

		//펫 엔드 상태로 만듬
		g_PetMgr.SetPetDieState( pExitUser );
	}

	if( Help::CheckEnableTimeGapUser2( 1 ) )
		LOG.PrintTimeAndLog( 0, "%s:%s", __FUNCTION__, szName.c_str());

	RemoveUser( szName, true );
	g_VictoriesMgr.LeaveUser( szName );
	g_PlayBonusMgr.LeaveUser( szName );
	g_ExcavationMgr.EraseUserExcavationLv( szName );

	bool bTeamSurvival;
	rkPacket >> bTeamSurvival;
	if( bTeamSurvival )
	{
		ioTeamSurvivalMode *pTeamSurvivalMode = ToTeamSurvivalMode( m_pPlayMode );
		if( pTeamSurvivalMode )
		{
			pTeamSurvivalMode->ApplyExtraModeInfo( rkPacket );
		}
	}

	ClearItemOwnerByOutUser( szName );

	g_App.RemoveNetworkTable( szName, P2P_STYLE_PLAY );
	if( ( GetModeType() == MT_HEADQUARTERS || GetModeType() == MT_HOUSE || IsCustomPlazaMode() ) && 
		!g_BattleRoomMgr.IsBattleRoom() && !g_LadderTeamMgr.IsLadderTeam() && 
		!g_ShuffleRoomMgr.IsShuffleRoom() && !g_RankBattleMgr.IsRankBattlePlaying() )
	{
		g_App.RemoveNetworkTable( szName, P2P_STYLE_VOICE );
		g_VoiceChat.RemoveSoundPlayer( szName.c_str() );
		g_VoiceOffListManager.Delete( szName );

		VoiceChatWnd *pVoice = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
		if( pVoice )
			pVoice->SetVoiceOff( szName );
#ifndef SHIPPING
		LOG.PrintTimeAndLog( 0, "%s | Remove user custom plaza | %s ", __FUNCTION__ , szName.c_str()  );
#endif
	}
	g_CustomSoundMgr.DeleteCustomSoundInfo( szName );

	ReCalculateDamageRate( false );

	if( m_pPlayMode )
	{
		m_pPlayMode->ContributeRoundAllPercent();
		m_pPlayMode->UpdateRoundInfoList();
		m_pPlayMode->CheckTotalChamp( false );
	}

	if( GetModeType() == MT_FLAG_CAPTURE )
	{
		ioFlagCaptureMode* pFlag = ToFlagCaptureMode( m_pPlayMode );
		if( pFlag )
			pFlag->ResetResultChar();
	}
}

void ioPlayStage::OnExitRoom( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	int iResult;
	int iSubNum;
	int iRoomNum, iMapNum, iPlazaType;
	bool bPenalty;
	ioHashString szName;

	rkPacket >> iResult;
	rkPacket >> iSubNum;
	rkPacket >> bPenalty;

#if defined( USE_GA )

	if( GetModeType() != MT_NONE && 
		GetModeType() != MT_TRAINING &&
		GetModeType() != MT_HEADQUARTERS &&
		GetModeType() != MT_HOUSE &&
		g_HttpMng.GetSendHeroChange() == true )
	{
		if( g_HttpMng.GetHeroStartTime() != 0 )
		{
			// end choice hero duration time;
			int iTime = (int)REALGETTIME() / 1000;
			g_HttpMng.SetHeroEndTime( iTime );

			char chSlash[16]		= {0,};
			char chType[64]			= {0,};
			char chMode[64]			= {0,};
			char chPostData[256]	= {0,};

			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
			{
				sprintf_e( chSlash, "%%2F" );
				sprintf_e( chType, "%s%s", chSlash, g_HttpMng.GetBattleType() );
				sprintf_e( chMode, "%s%s", chSlash, g_HttpMng.GetBattleMode() );
				sprintf_e( chPostData, "%sPLAY%s%s%sHERO_CHANGE", chSlash, chType, chMode, chSlash );
			}
			else
			{
				SafeSprintf( chSlash, sizeof(chSlash), "%2F" );
				SafeSprintf( chType, sizeof(chType), "%1%2", chSlash, g_HttpMng.GetBattleType() );
				SafeSprintf( chMode, sizeof(chMode), "%1%2", chSlash, g_HttpMng.GetBattleMode() );
				SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4HERO_CHANGE", chSlash, chType, chMode, chSlash );
			}

			// PLAY_<TYPE>_<MODE>_HERO_CHANGE
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData, 9 );

			g_HttpMng.SetSendHeroChange( false );
			g_HttpMng.SetHeroStartTime( 0 );
			g_HttpMng.SetHeroEndTime( 0 );
			g_HttpMng.SetHeroIndex( 0 );
		}
		else
		{
			// start choice hero duration time;
			int iTime = (int)REALGETTIME() / 1000;
			g_HttpMng.SetHeroStartTime( iTime );				
			g_HttpMng.SetHeroIndex( g_MyInfo.GetClassType() );
		}
	}	

#endif

	char szCall[MAX_PATH];
	sprintf_s( szCall, "%s - (%d) - (%d)" ,__FUNCTION__, iResult, iSubNum );

	switch( iResult )
	{
	case EXIT_ROOM_LOBBY:
		{
			if( GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
			{
				g_BattleRoomMgr.LeaveRoom();
				g_LadderTeamMgr.LeaveTeam();
				g_ShuffleRoomMgr.LeaveRoom();
				g_RankBattleMgr.SetRankBattlePlaying( false );
			}

			if( GetRoomKickOutUser() == g_MyInfo.GetPublicID() )
			{
				if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
				{
					g_App.SetNextStage( ioGameStage::GS_LOBBY, szCall );
					g_App.SetModeType( 0, 0, 0, PT_NONE );
				}
				else
				{
					g_App.SendSearchModeJoin();
				}
			}
			else
			{
				//로비/광장 병합버전의 경우 해당 루트로 들어오는 케이스는 전투대기방/래더대기방/대회대기방 등으로만 들어옴
				g_App.SetNextStage( ioGameStage::GS_LOBBY, szCall );
				g_App.SetModeType( 0, 0, 0, PT_NONE );
			}

			if( bPenalty )
			{
				g_GUIMgr.SetReserveMsgBox( MB_INFOMATION_PENALTY );
			}

#if defined( USE_GA )
			// PLAY_LOBBY_CONNECT
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FPLAY%2FLOBBY%2FCONNECT" );
#endif
		}
		break;		
	case EXIT_ROOM_CHAR_LIMIT:
		{
			if( GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
			{
				g_BattleRoomMgr.LeaveRoom();
				g_LadderTeamMgr.LeaveTeam();
				g_ShuffleRoomMgr.LeaveRoom();
				g_RankBattleMgr.SetRankBattlePlaying( false );
			}
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				g_App.SetNextStage( ioGameStage::GS_LOBBY, szCall );
				g_App.SetModeType( 0, 0, 0, PT_NONE );
			}
			else
			{				
				g_App.SetNextStage( ioGameStage::GS_PLAY, szCall );
				g_App.SetModeType( MT_HEADQUARTERS, 1, 1, PT_NONE );

				SP2Packet kPacket( CTPK_JOIN_HEADQUARTERS );
				kPacket << g_MyInfo.GetPublicID() << g_ModeSelectInfo.CheckHeadquartersMapIndex( g_ModeSelectInfo.GetHeadquartersModeIndex(), 1 ) << false;
				TCPNetwork::SendToServer( kPacket );
			}
		}
		break;
	case EXIT_ROOM_MONSTER_COIN_LACK:
	case EXIT_ROOM_RAID_COIN_LACK:
		{
			if( GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
			{
				g_BattleRoomMgr.LeaveRoom();
				g_LadderTeamMgr.LeaveTeam();
				g_ShuffleRoomMgr.LeaveRoom();
				g_RankBattleMgr.SetRankBattlePlaying( false );
			}

			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				g_App.SetNextStage( ioGameStage::GS_LOBBY, szCall );
				g_App.SetModeType( 0, 0, 0, PT_NONE );
			}
			else
			{
				g_App.SendSearchModeJoin();
			}

			if (GetModeType() == MT_RAID )
				g_GUIMgr.SetReserveMsgBox( MB_RAID_COIN_LACK );
			else
				g_GUIMgr.SetReserveMsgBox( MB_MONSTER_COIN_LACK );
		}
		break;
	case EXIT_ROOM_NOT_CHAR:
		{
			if( GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
			{
				g_BattleRoomMgr.LeaveRoom();
				g_LadderTeamMgr.LeaveTeam();
				g_ShuffleRoomMgr.LeaveRoom();
				g_RankBattleMgr.SetRankBattlePlaying( false );
			}
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				g_App.SetNextStage( ioGameStage::GS_LOBBY, szCall );
				g_App.SetModeType( 0, 0, 0, PT_NONE );
			}
			else
			{
				g_App.SendSearchModeJoin();
			}
			g_GUIMgr.SetReserveMsgBox( MB_SOLDIER_LIMIT );
		}
		break;
	case EXIT_ROOM_ENTRY_NOT_FORMALITY:
		{
			if( GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
			{
				g_BattleRoomMgr.LeaveRoom();
				g_LadderTeamMgr.LeaveTeam();
				g_ShuffleRoomMgr.LeaveRoom();
				g_RankBattleMgr.SetRankBattlePlaying( false );
			}

			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				g_App.SetNextStage( ioGameStage::GS_LOBBY, szCall );
				g_App.SetModeType( 0, 0, 0, PT_NONE );
			}
			else
			{
				g_App.SendSearchModeJoin();
			}
			g_GUIMgr.SetReserveMsgBox( MB_ENTRY_NOT_FORMALITY );
		}
		break;
	case EXIT_ROOM_RESERVED_CHANGE_ID:
		{
			if( GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
			{
				g_BattleRoomMgr.LeaveRoom();
				g_LadderTeamMgr.LeaveTeam();
				g_ShuffleRoomMgr.LeaveRoom();
				g_RankBattleMgr.SetRankBattlePlaying( false );
			}
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				g_App.SetNextStage( ioGameStage::GS_LOBBY, szCall );
				g_App.SetModeType( 0, 0, 0, PT_NONE );
			}
			else
			{
				g_App.SendSearchModeJoin();
			}
			g_GUIMgr.SetReserveMsgBox( MB_RESERVED_CHANGE_ID );
		}
		break;
	case EXIT_ROOM_OK:
		{
			if( GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
			{
				g_BattleRoomMgr.LeaveRoom();
				g_LadderTeamMgr.LeaveTeam();
				g_ShuffleRoomMgr.LeaveRoom();
				g_RankBattleMgr.SetRankBattlePlaying( false );
			}

			if( iSubNum == -1 )
			{
				if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
				{
					g_App.SetNextStage( ioGameStage::GS_LOBBY, szCall );
					g_App.SetModeType( 0, 0, 0, PT_NONE );					
				}
				else
				{
					g_App.SendSearchModeJoin();
				}
			}
			else
			{
				ASSERT( false );
				LOG.PrintTimeAndLog(0, "%s - Invalid packet parameter!!!!!", __FUNCTION__ );
			}

			if( bPenalty )
			{
				g_GUIMgr.SetReserveMsgBox( MB_INFOMATION_PENALTY );
			}

#if defined( USE_GA )
			// PLAY_LOBBY_CONNECT
			g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FPLAY%2FLOBBY%2FCONNECT" );
#endif
		}
		break;
	case EXIT_ROOM_SAFETY_KICK:
		{
			//SAFTY LEVEL - 초보 유저들에 대한 처리부분
			if( GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
			{
				g_BattleRoomMgr.LeaveRoom();
				g_LadderTeamMgr.LeaveTeam();
				g_ShuffleRoomMgr.LeaveRoom();
				g_RankBattleMgr.SetRankBattlePlaying( false );
			}

			if( GetModeType() == MT_HEADQUARTERS || GetModeType() == MT_TRAINING || GetModeType() == MT_HOUSE || iSubNum == -1 )
			{
				if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
				{
					g_App.SetNextStage( ioGameStage::GS_LOBBY, szCall );
					g_App.SetModeType( 0, 0, 0, PT_NONE );
				}
				else
				{
					g_App.SendSearchModeJoin();
				}
			}
			else
			{
				rkPacket >> iRoomNum >> iMapNum >> iPlazaType;

				char szDetailCall[MAX_PATH];
				sprintf_s( szDetailCall, "%s - (%d) - (%d) - (%d) - (%d)" ,__FUNCTION__, iResult, iSubNum, iMapNum, iPlazaType );

				g_App.SetNextStage( ioGameStage::GS_PLAY, szDetailCall );
				g_App.SetModeType( MT_TRAINING, iSubNum, iMapNum, (PlazaType)iPlazaType );
				g_App.SetRoomNum( iRoomNum );
			}

			g_GUIMgr.SetReserveMsgBox( MB_LEAVE_SAFE_ROOM );
		}
		break;
	case EXIT_ROOM_RESERVED:
		g_ChatMgr.SetSystemMsg( STR(1) );
		SetReservedExit( true );
		return;
	case EXIT_ROOM_CANCELED:
		g_ChatMgr.SetSystemMsg( STR(2) );
		SetReservedExit( false );
		return;
	case EXIT_SURRENDER:
		{
			rkPacket >> szName;
			g_ChatMgr.SetSystemMsg("%s님 도전포기", szName.c_str() );

			TowerDefWnd *pUserGauge = dynamic_cast<TowerDefWnd*>( g_GUIMgr.FindWnd( TOWERDEF_USER_GAUGE_WND ));
			if(pUserGauge)
				pUserGauge->SetSurrender(szName.c_str(), true);

			if(g_MyInfo.GetPublicID() == szName.c_str())
				SetReservedExit( true );
		}
		return;
	case EXIT_NEVER_SURRENDER:
		{
			rkPacket >> szName;
			g_ChatMgr.SetSystemMsg("%s님 도전포기 취소", szName.c_str() );

			TowerDefWnd *pUserGauge = dynamic_cast<TowerDefWnd*>( g_GUIMgr.FindWnd( TOWERDEF_USER_GAUGE_WND ));
			if(pUserGauge)
				pUserGauge->SetSurrender(szName.c_str(), false);

			if(g_MyInfo.GetPublicID() == szName.c_str())
				SetReservedExit( false );
		}
		return;
	case EXIT_ROOM_NO_ROOM:
		if( GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
		{
			g_BattleRoomMgr.LeaveRoom();
			g_LadderTeamMgr.LeaveTeam();
			g_ShuffleRoomMgr.LeaveRoom();
			g_RankBattleMgr.SetRankBattlePlaying( false );
		}

		if( iSubNum == -1 )
		{
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				g_App.SetNextStage( ioGameStage::GS_LOBBY, szCall );
				g_App.SetModeType( 0, 0, 0, PT_NONE );
			}
			else
			{
				g_App.SendSearchModeJoin();
			}
		}
		else
		{
			rkPacket >> iRoomNum >> iMapNum >> iPlazaType;
			char szDetailCall[MAX_PATH];
			sprintf_s( szDetailCall, "%s - (%d) - (%d) - (%d) - (%d)" ,__FUNCTION__, iResult, iSubNum, iMapNum, iPlazaType );

			g_App.SetNextStage( ioGameStage::GS_PLAY, szCall );
			g_App.SetModeType( MT_TRAINING, iSubNum, iMapNum, (PlazaType)iPlazaType );
			g_App.SetRoomNum( iRoomNum );
		}

		g_GUIMgr.SetReserveMsgBox( MB_RESERVE_NO_ROOM );
		break;
	case EXIT_ROOM_KEY_ABUSE:
	case EXIT_ROOM_DAMAGE_ABUSE:
	case EXIT_ROOM_SPEEDHACK:
	case EXIT_ROOM_MACRO_OUT:
		g_BattleRoomMgr.LeaveRoom();
		g_LadderTeamMgr.LeaveTeam();
		g_ShuffleRoomMgr.LeaveRoom();
		g_RankBattleMgr.SetRankBattlePlaying( false );

		if( iSubNum == -1 )
		{
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				g_App.SetNextStage( ioGameStage::GS_LOBBY, szCall );
				g_App.SetModeType( 0, 0, 0, PT_NONE );
			}
			else
			{
				g_App.SendSearchModeJoin();
			}
		}
		else
		{
			rkPacket >> iRoomNum >> iMapNum >> iPlazaType;

			char szDetailCall[MAX_PATH];
			sprintf_s( szDetailCall, "%s - (%d) - (%d) - (%d) - (%d)" ,__FUNCTION__, iResult, iSubNum, iMapNum, iPlazaType );

			if( iRoomNum == -1 )
			{
				if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
				{
					g_App.SetNextStage( ioGameStage::GS_LOBBY, szDetailCall );
					g_App.SetModeType( 0, 0, 0, PT_NONE );
				}
				else
				{
					g_App.SendSearchModeJoin();					
				}
			}
			else
			{
				g_App.SetNextStage( ioGameStage::GS_PLAY, szDetailCall );
				g_App.SetModeType( MT_TRAINING, iSubNum, iMapNum, (PlazaType)iPlazaType );
				g_App.SetRoomNum( iRoomNum );
			}
		}

		if( iResult == EXIT_ROOM_SPEEDHACK )
		{
			if( bPenalty )
				g_MyInfo.SetLobbyLockType( LobbyLockProblemWnd::LT_HACK, MB_RESERVE_SPEEDHACK_PENALTY );
			else
				g_MyInfo.SetLobbyLockType( LobbyLockProblemWnd::LT_HACK, MB_RESERVE_SPEEDHACK );	
		}
		else if( iResult == EXIT_ROOM_KEY_ABUSE && !bPenalty )	// 광장 키 어뷰즈
		{
			g_MyInfo.SetLobbyLockType( LobbyLockProblemWnd::LT_ABUSE, MB_RESERVE_OPEN_SPACE_ABUSE );
		}
		else if( iResult == EXIT_ROOM_MACRO_OUT )
		{
			g_MyInfo.SetLobbyLockType( LobbyLockProblemWnd::LT_MACRO, MB_RESERVE_MACRO_OUT );
		}
		else
		{
			g_MyInfo.SetLobbyLockType( LobbyLockProblemWnd::LT_ABUSE, MB_RESERVE_ABUSE );
		}
		break;
	case EXIT_ROOM_BAD_NETWORK:
		g_BattleRoomMgr.LeaveRoom();
		g_LadderTeamMgr.LeaveTeam();
		g_ShuffleRoomMgr.LeaveRoom();
		g_RankBattleMgr.SetRankBattlePlaying( false );

		if( iSubNum == -1 )
		{
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				g_App.SetNextStage( ioGameStage::GS_LOBBY, szCall );
				g_App.SetModeType( 0, 0, 0, PT_NONE );
			}
			else
			{
				g_App.SetConnectedMoveMode( ioApplication::CMM_HEADQUARTER );
				g_App.SendSearchModeJoin( ioApplication::LOADING_HEADQUARTER );
				g_App.ResetConnectedMoveMode();
			}
		}
		else
		{
			rkPacket >> iRoomNum >> iMapNum >> iPlazaType;

			char szDetailCall[MAX_PATH];
			sprintf_s( szDetailCall, "%s - (%d) - (%d) - (%d) - (%d)" ,__FUNCTION__, iResult, iSubNum, iMapNum, iPlazaType );

			g_App.SetNextStage( ioGameStage::GS_PLAY, szDetailCall );
			g_App.SetModeType( MT_TRAINING, iSubNum, iMapNum, (PlazaType)iPlazaType );
			g_App.SetRoomNum( iRoomNum );
		}
		g_GUIMgr.SetReserveMsgBox( MB_RESERVE_BAD_NETWORK_KICK );
		break;
	case EXIT_ROOM_CONTINUE_END:
		{
			g_BattleRoomMgr.LeaveRoom();
			g_LadderTeamMgr.LeaveTeam();
			g_ShuffleRoomMgr.LeaveRoom();
			g_RankBattleMgr.SetRankBattlePlaying( false );

			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				g_App.SetNextStage( ioGameStage::GS_LOBBY, szCall );
				g_App.SetModeType( 0, 0, 0, PT_NONE );
			}
			else
			{
				g_App.SendSearchModeJoin();
			}
		}
		break;
	case EXIT_BATTLEROOM_MOVE:
		{
			if( GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
			{
				g_BattleRoomMgr.LeaveRoom();
				g_LadderTeamMgr.LeaveTeam();
				g_ShuffleRoomMgr.LeaveRoom();
				g_RankBattleMgr.SetRankBattlePlaying( false );
			}

			if( bPenalty )
			{
				g_GUIMgr.SetReserveMsgBox( MB_INFOMATION_PENALTY );
			}
		}
		break;
	case EXIT_ROOM_PARTY_JOIN:
	case EXIT_ROOM_DELAY:
		break;
	case EXIT_ROOM_RELAYSVR_DISCONNECTED: //for relay
		{
			g_BattleRoomMgr.LeaveRoom();
			g_LadderTeamMgr.LeaveTeam();
			g_ShuffleRoomMgr.LeaveRoom();
			g_RankBattleMgr.SetRankBattlePlaying( false );

			if( iSubNum == -1 )
			{
				if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
				{
					g_App.SetNextStage( ioGameStage::GS_LOBBY, szCall );
					g_App.SetModeType( 0, 0, 0, PT_NONE );
				}
				else
				{
					g_App.SendSearchModeJoin();
				}
			}
			else
			{
				rkPacket >> iRoomNum >> iMapNum >> iPlazaType;

				char szDetailCall[MAX_PATH];
				sprintf_s( szDetailCall, "%s - (%d) - (%d) - (%d) - (%d)" ,__FUNCTION__, iResult, iSubNum, iMapNum, iPlazaType );

				g_App.SetNextStage( ioGameStage::GS_PLAY, szDetailCall );
				g_App.SetModeType( MT_TRAINING, iSubNum, iMapNum, (PlazaType)iPlazaType );
				g_App.SetRoomNum( iRoomNum );
			}
			g_GUIMgr.SetReserveMsgBox( MB_RELAYSVR_DISCONNECTED );

		}
		break;
	case EXIT_ROOM_EXCEPTION:
		{
			if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
			{
				g_App.SendSearchModeJoin();
				g_GUIMgr.SetReserveMsgBox( MB_ROOM_EXCEPTION );
			}
			else
			{
				return;
			}
		}
		break;
	}

	g_MyInfo.SetArenaModeCharacter( MT_NONE );
	
	// 룸 이탈 처리. . . . 나가기 예약은 여기까지 안옴.
	g_VictoriesMgr.Initialize();
	g_PlayBonusMgr.Initialize();
	g_ExcavationMgr.ClearExcavationUserInfo();

	if( g_App.GetGuildStorage() )
		g_App.GetGuildStorage()->Initialize();

	if( g_App.GetHomeStorage() )
		g_App.GetHomeStorage()->Initialize();

	// 예약작업 처리
	if( g_App.IsNextStageLobby() )
	{
		g_RserveTask.ExecuteReservedTask( RESERVEDCMD_WHEN_ENTERLOBBY );
	}
}

void ioPlayStage::OnLadderUserHQMove( SP2Packet &rkPacket )
{
	//래더/진영전은 로비로 이동
	ioMyRoomMode::SetMyRoomType( MRT_LADDER );
	g_App.SetNextStage( ioGameStage::GS_LOBBY, __FUNCTION__ );
	g_App.SetModeType( 0, 0, 0, PT_NONE );
	
	g_VictoriesMgr.Initialize();
	g_PlayBonusMgr.Initialize();

	// 마지막으로 플레이한 캐릭터 동기화
	g_MyInfo.SyncBattleNLadderCharIndex();
}

void ioPlayStage::OnGradeSync( SP2Packet &rkPacket )
{
	ioHashString szUserID;
	int iGradeLevel, iGradeRate;
	rkPacket >> szUserID >> iGradeLevel >> iGradeRate;

	g_UserInfoMgr.SetGrade( szUserID, iGradeLevel, iGradeRate );

	ioBaseChar *pChar = GetBaseChar( szUserID );
	if( pChar )
	{
		pChar->SetLevel( iGradeLevel );
	}

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		g_BattleRoomMgr.UpdateUserLevel( szUserID, iGradeLevel );
	}
	else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		g_LadderTeamMgr.UpdateUserData( szUserID, iGradeLevel, 0 );
	}
	else if( g_ShuffleRoomMgr.IsShuffleRoom() )
	{
		g_ShuffleRoomMgr.UpdateUserLevel( szUserID, iGradeLevel );
	}
}

void ioPlayStage::OnCreateRoom( SP2Packet &rkPacket )
{
	int iResult = 0;
	int iModeType = 0;
	int iSubNum = 0;
	int iRoomNum = -1;

	rkPacket >> iResult;
	rkPacket >> iModeType;
	rkPacket >> iSubNum;
	rkPacket >> iRoomNum;	

	switch( iResult )
	{
	// OK 신호는 오지않는다. OK 대신 JOIN_ROOM이 온다.
	case CREATE_ROOM_NOT:
		g_GUIMgr.HideWnd( TIMEGATE_SEARCH );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		break;
	case CREATE_ROOM_FULL:
		g_GUIMgr.HideWnd( TIMEGATE_SEARCH );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		break;
	case CREATE_ROOM_NOT_SOLDIER:
		g_GUIMgr.HideWnd( TIMEGATE_SEARCH );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		break;
	}

	g_App.SetMouseBusy( false );
}

void ioPlayStage::OnPlazaRoomList( SP2Packet &rkPacket )
{
	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
		if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )
			pLobbyMainWnd->ApplyPlazaList( rkPacket );

		TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
		if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )
			pTrainingFuncWnd->ApplyPlazaList( rkPacket );
	}
	else
	{
		QuickLobbyWnd* pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
		if( pWnd )
			pWnd->ApplyPlazaList( rkPacket );		
	}
}

void ioPlayStage::OnPlazaCommand( SP2Packet &rkPacket )
{
	int iCommand;
	rkPacket >> iCommand;
	switch( iCommand )
	{
	case PLAZA_CMD_INFO:
		{
			PlazaInfo kPlazaInfo;
			int iPlazaType;
			rkPacket >> kPlazaInfo.m_iRoomIndex >> kPlazaInfo.m_iRoomNum >> kPlazaInfo.m_szRoomName >> kPlazaInfo.m_szMasterName >> kPlazaInfo.m_bRoomPW >> kPlazaInfo.m_iMaxUserCount 
					 >> kPlazaInfo.m_iRoomLevel >> kPlazaInfo.m_iJoinUserCount >> kPlazaInfo.m_iPlayUserCount >> kPlazaInfo.m_iMasterLevel >> iPlazaType;

			m_ePlazaType = (PlazaType)iPlazaType;
			kPlazaInfo.m_ePlazaType = m_ePlazaType;
			
			if( kPlazaInfo.m_szMasterName == g_MyInfo.GetPublicID() )
				rkPacket >> kPlazaInfo.m_szRoomPW;

			PlazaMainWnd *pMainWnd = dynamic_cast<PlazaMainWnd*>(g_GUIMgr.FindWnd( PLAZA_MAIN_WND ));
			if( pMainWnd )
			{
				pMainWnd->UpdatePlazaInfo( kPlazaInfo );
			}
		}
		break;
	case PLAZA_CMD_MASTER_CHANGE:
		{
			ioHashString szMasterName;
			rkPacket >> szMasterName;
			PlazaMainWnd *pMainWnd = dynamic_cast<PlazaMainWnd*>(g_GUIMgr.FindWnd( PLAZA_MAIN_WND ));
			if( pMainWnd )
			{
				pMainWnd->SetPlazaMaster( szMasterName );
			}
		}
		break;
	case PLAZA_CMD_ROOM_LEVEL:       //유저가 입장하거나 나가면 레벨이 변경된다.
		{
			int iRoomLevel;
			rkPacket >> iRoomLevel;
			PlazaMainWnd *pMainWnd = dynamic_cast<PlazaMainWnd*>( g_GUIMgr.FindWnd( PLAZA_MAIN_WND ) );
			if( pMainWnd )
			{
				pMainWnd->SetPlazaLevel( iRoomLevel );
				if( m_pPlayMode )
					pMainWnd->SetJoinUserCount( m_pPlayMode->GetRecordCharCnt() );
			}
		}
		break;
	case PLAZA_CMD_NAME_PW_CHANGE:
		{
			ioHashString szRoomName, szRoomPW;
			rkPacket >> szRoomName >> szRoomPW;
			PlazaMainWnd *pMainWnd = dynamic_cast<PlazaMainWnd*>(g_GUIMgr.FindWnd( PLAZA_MAIN_WND ));
			if( pMainWnd )
			{
				pMainWnd->SetPlazaName( szRoomName );
				pMainWnd->SetPlazaPW( !szRoomPW.IsEmpty() );
			}
		}
		break;
	case PLAZA_CMD_MAXPLAYER_CHANGE:
		{
			TCPNetwork::MouseBusy( false );

			int iMaxPlayer;
			rkPacket >> iMaxPlayer;
			PlazaMainWnd *pMainWnd = dynamic_cast<PlazaMainWnd*>(g_GUIMgr.FindWnd( PLAZA_MAIN_WND ));
			if( pMainWnd )
			{
				pMainWnd->SetMaxUserCount( iMaxPlayer );
			}
		}
		break;
	case PLAZA_CMD_KICK_OUT:
		{
			rkPacket >> m_szRoomKickOutUser;
			if( m_szRoomKickOutUser == g_MyInfo.GetPublicID() )
			{
				int iRoomIndex;
				rkPacket >> iRoomIndex;
				g_MyInfo.SetKickOutPlaza( iRoomIndex );
				g_GUIMgr.SetReserveMsgBox( MB_PLAZA_KICK_OUT );
			}
			else
			{
				ioComplexStringPrinter kPrinter;
				g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
				kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), m_szRoomKickOutUser.c_str() );
				kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2));
				g_ChatMgr.SetChatComplexString( STR(3), kPrinter );
			}
		}
		break;
	}
}

void ioPlayStage::OnPlazaInviteList( SP2Packet &rkPacket )
{
	if( GetModeType() != MT_TRAINING ) return;

	PlazaInviteListWnd *pInviteList = dynamic_cast<PlazaInviteListWnd*>(g_GUIMgr.FindWnd( PLAZA_INVITE_LIST_WND ));
	if( pInviteList )
	{
		pInviteList->SetUserList( rkPacket );
	}
}

void ioPlayStage::OnHeadquartersInviteList( SP2Packet &rkPacket )
{
	if( GetModeType() != MT_HEADQUARTERS )
		return;

	HeadquartersInviteListWnd *pInviteList = dynamic_cast<HeadquartersInviteListWnd*>( g_GUIMgr.FindWnd( HEADQUARTERS_INVITE_LIST_WND ) );
	if( pInviteList )
	{
		pInviteList->SetUserList( rkPacket );
	}
}

void ioPlayStage::OnHouseInviteList( SP2Packet &rkPacket )
{
	if( GetModeType() != MT_HOUSE )
		return;

	HouseInviteListWnd *pInviteList = dynamic_cast<HouseInviteListWnd*>( g_GUIMgr.FindWnd( HOUSE_INVITE_LIST_WND ) );
	if( pInviteList )
	{
		pInviteList->SetUserList( rkPacket );
	}
}

void ioPlayStage::OnPlazaInvite( SP2Packet &rkPacket )
{
	if( g_MyInfo.IsTutorialUser() )
		return;

	if( g_QuestMgr.IsInvitedPass() )
		return;

	if( GetModeType() != MT_MYROOM && GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
		return;

	if( g_MyInfo.GetActiveCharCount() == 0 )
		return;

	if( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 )
		return;

	if( g_BattleRoomMgr.IsBattleRoom() || g_LadderTeamMgr.IsLadderTeam() || 
		g_ShuffleRoomMgr.IsShuffleRoom() || g_RankBattleMgr.IsRankBattlePlaying() )
		return;

	if( g_GUIMgr.IsShow( TIMEGATE_SEARCH ) )
		return;

	int iRoomNum;
	rkPacket >> iRoomNum;
	if( GetRoomNum() == iRoomNum ) return;

	PlazaInvitedWnd *pInvitedWnd = dynamic_cast<PlazaInvitedWnd*>(g_GUIMgr.FindWnd( PLAZA_INVITED_WND ));
	if( pInvitedWnd )
		pInvitedWnd->SetInfo( iRoomNum, rkPacket );
}

void ioPlayStage::OnJoinRoom( SP2Packet &rkPacket )
{
	int iResult = 0;
	int iModeType = 0;
	int iSubNum = 0;
	int iMapNum = 0;
	int iRoomNum = -1;
	int iPlazaType= PT_NONE;

	rkPacket >> iResult;

	switch( iResult )
	{
	case JOIN_ROOM_OK:
		{
			rkPacket >> iModeType;
			rkPacket >> iSubNum;
			rkPacket >> iMapNum;
			rkPacket >> iRoomNum;
			rkPacket >> iPlazaType;

			char szCall[MAX_PATH];
			sprintf_s( szCall, "%s - (%d), (%d), (%d), (%d)" ,__FUNCTION__, iModeType, iSubNum, iMapNum, iPlazaType );

			g_App.SetNextStage( ioGameStage::GS_PLAY, szCall );
			g_App.SetModeType( iModeType, iSubNum, iMapNum, (PlazaType)iPlazaType );
			g_App.SetRoomNum( iRoomNum );
			g_P2PChecker.Initialize();

			if( iModeType != MT_TRAINING && iModeType != MT_HEADQUARTERS && iModeType != MT_HOUSE )
			{
				TimeGateSearch *pSearchWnd = dynamic_cast<TimeGateSearch*>( g_GUIMgr.FindWnd( TIMEGATE_SEARCH ) );
				if( pSearchWnd )
					pSearchWnd->ClearReserve();
			}
		}
		break;
	case JOIN_ROOM_NOT:
		g_GUIMgr.HideWnd( TIMEGATE_SEARCH );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		break;
	case JOIN_ROOM_FULL:
		g_GUIMgr.HideWnd( TIMEGATE_SEARCH );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		break;
	case JOIN_ROOM_NOT_SOLDIER:
		g_GUIMgr.HideWnd( TIMEGATE_SEARCH );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		break;
	}

	g_App.SetMouseBusy( false );
}

void ioPlayStage::OnChangeStage( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	if( g_App.GetConnectedMoveMode() != ioApplication::CMM_LOBBY )
	{
		g_App.SendSearchModeJoin();
	}
	else
	{
		g_App.SetNextStage( ioGameStage::GS_LOBBY, __FUNCTION__ );
		g_App.SetModeType( 0, 0, 0, PT_NONE );	
	}
}

void ioPlayStage::OnChangeUDPInfo( SP2Packet &rkPacket )
{
	ioHashString szName, szPublicIP, szPrivateIP, szTransferIP;
	int iPort, iTransferPort;
	rkPacket >> szName >> szPublicIP >> iPort >> szPrivateIP >> szTransferIP >> iTransferPort;

	if( g_MyInfo.GetPublicID() == szName )
	{
		g_MyInfo.SetMyAddress( szPublicIP, iPort );
	}
	else
	{
		g_App.ServerToClientPortChange( szName, szPublicIP, szPrivateIP, szTransferIP, iPort, iTransferPort );
	}
}

void ioPlayStage::OnPlayRecordInfo( SP2Packet &rkPacket )
{
	if( m_pPlayMode )
	{
		ioHashString szName;
		rkPacket >> szName;
		if( g_MyInfo.GetPublicID() == szName )
			m_pPlayMode->ApplyPlayRecordInfo( rkPacket );
		m_pPlayMode->ContributeRoundAllPercent();
	}
}

void ioPlayStage::OnParentCharEquipData( SP2Packet &rkPacket )
{
	OnCharEquipData( rkPacket );
	OnCharEquipMedalItemData( rkPacket );
}

void ioPlayStage::OnCharEquipData( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnCharEquipData - %s Not Exist", szName.c_str() );
		return;
	}

	pChar->DestroyAllItem();

	DWORD dwItemCode;
	int iItemReinforce;
	DWORD dwItemMaleCustom, dwItemFemaleCustom;
	int   iItemIndex;
	ioHashString szOwner;
	ITEM_DATA kEquipItem[MAX_INVENTORY];
	for( int i=0 ; i<MAX_EQUIP_SLOT ; i++ )
	{
		rkPacket >> dwItemCode >> iItemReinforce >> dwItemMaleCustom >> dwItemFemaleCustom >> iItemIndex >> szOwner;

		if( i < MAX_INVENTORY )
		{
			kEquipItem[i].m_item_code = dwItemCode;
			kEquipItem[i].m_item_male_custom = dwItemMaleCustom;
			kEquipItem[i].m_item_female_custom = dwItemFemaleCustom;
		}

		if( dwItemCode > 0 )
		{
			ioItem *pItem = CreateItem( dwItemCode );
			if( pItem )
			{
				pItem->ReLoadProperty();
				pItem->SetOwnerName( szOwner );
				pItem->SetItemCreateIndex( iItemIndex );
				pItem->SetItemReinforce( iItemReinforce );
				pItem->SetItemCustom( dwItemMaleCustom, dwItemFemaleCustom );
				if( i == ES_OBJECT )
				{
					ioExcavationItem* pExcavationItem = ToExcavationItem( pItem );
					if( pExcavationItem )
					{
						pExcavationItem->CheckMeshAndAni( pChar );
					}
				}

				// 코스튬 관련 (코스튬 동기화 : 서버에서 받을수도 있음)
				pItem->SetCostumeCustom( pChar->GetCharacterInfo().m_EquipCostume[i].m_Costume_Male_Custom, pChar->GetCharacterInfo().m_EquipCostume[i].m_Costume_Female_Custom );
				pItem->SetCostume( pChar->GetCharacterInfo().m_EquipCostume[i].m_CostumeCode, pChar->IsOwnerChar() );

				ioItem *pPreItem = pChar->EquipItem( pItem, true );
				SAFEDELETE( pPreItem );

				if( ToSuddenAttackItem( pItem ) )
					ToSuddenAttackItem( pItem )->SetInitialize( szOwner, this, pChar->GetSuddenMeshType() );
				else
				{
					ioBaseChar *pOwner = GetBaseChar( szOwner );
					pItem->Initialize( pOwner, this );
				}
			}
		}
	}

	if( g_MyInfo.GetPublicID() != szName )
	{
		g_GlobalSoldierInfo.SyncSoldierItem( pChar->GetIndex(), (const ITEM_DATA*)&kEquipItem );
	}

	pChar->CheckDefaultReinforce();

	pChar->FillMaxHP();
	float fCurRate = pChar->GetCurHPRate();
	float fNewHP = pChar->GetHP().m_fCurValue * fCurRate;
	pChar->SetCurHP( fNewHP );

	if( pChar->GetState() == CS_FISHING )
	{
		pChar->ShowWeaponItemMesh( false );
	}

	if( pChar->IsExcavating() )
	{
		if( szName != g_MyInfo.GetPublicID() )
			pChar->SetLongTimeLiveEmoticonAndScale( g_ExcavationMgr.GetExcavatingEmoticon(), false );
	}

    // 최초 생성
	if( szName == g_MyInfo.GetPublicID() )
	{
		ioBaseChar *pOwner = GetOwnerChar();
		if( pOwner )
		{
			SP2Packet kPacket( CUPK_NEW_USER_INFO );
			kPacket << szName;
			pOwner->GetCharInfoInNetwork( kPacket );
			P2PNetwork::SendToAllPlayingUser( kPacket );
		}
	}
}

void ioPlayStage::OnFieldItemList( SP2Packet &rkPacket )
{
	int iItemCnt;
	rkPacket >> iItemCnt;

	int iItemIndex;
	DWORD dwItemCode;
	int iItemReinforce;
	DWORD dwItemMaleCustom, dwItemFemaleCustom;
	ioHashString szOwnerName;
	D3DXVECTOR3 vItemPos;
	float fCurItemGauge;

	for( int i=0 ; i<iItemCnt ; i++ )
	{
		rkPacket >> dwItemCode >> iItemReinforce >> dwItemMaleCustom >> dwItemFemaleCustom >> iItemIndex >> szOwnerName >> vItemPos >> fCurItemGauge;

		ioItem *pItem = CreateItem( dwItemCode );
		if( pItem )
		{
			pItem->ReLoadProperty();
			pItem->SetItemCreateIndex( iItemIndex );
			pItem->SetOwnerName( szOwnerName );
			pItem->SetItemReinforce( iItemReinforce );
			pItem->SetItemCustom( dwItemMaleCustom, dwItemFemaleCustom );
			pItem->SetCurSkillGauge( fCurItemGauge );

			ioFieldItem *pField = CreateFieldItem( pItem );
			if( pField )
			{
				pField->SetWorldPositionByServerSide( vItemPos );
				pField->CreateItemEffect();
			}
		}
	}
}

void ioPlayStage::OnFieldItemSupply( SP2Packet &rkPacket )
{
	// 2008.08.01 임시 광장에서 왕관쓰고 다른 광장으로 이동하는 유저 다운
	if( !m_pPlayMode )	return;

	int iNewItemCnt;
	rkPacket >> iNewItemCnt;

	int iCreateIndex;
	DWORD dwItemCode;
	int iItemReinforce;
	DWORD dwItemMaleCustom, dwItemFemaleCustom;
	D3DXVECTOR3 vItemPos;
	ioHashString szOwnerName;
	ioHashString szCreateItem;

	for( int i=0 ; i<iNewItemCnt ; i++ )
	{
		rkPacket >> dwItemCode >> iItemReinforce >> dwItemMaleCustom >> dwItemFemaleCustom >> iCreateIndex >> vItemPos >> szOwnerName >> szCreateItem;

		ioItem *pItem = CreateItem( dwItemCode );
		if( !pItem )	continue;

		pItem->ReLoadProperty();
		pItem->SetItemCreateIndex( iCreateIndex );
		pItem->SetOwnerName( szOwnerName );
		pItem->SetItemReinforce( iItemReinforce );
		pItem->SetItemCustom( dwItemMaleCustom, dwItemFemaleCustom );

		ioObjectItem *pObject = ToObjectItem(pItem);
		if( pObject && !szCreateItem.IsEmpty() )
		{
			pObject->SetObjectCreateItem(szCreateItem);
		}

		ioBaseChar *pOwner = GetBaseChar( szOwnerName );
		ioFieldItem *pField = CreateFieldItem( pItem );
		if( pField )
		{
			pField->SetWorldPositionByServerSide( vItemPos );
			pField->SetDropState( pField->GetWorldPosition() );
			pField->CreateDropEffect();
			if( pOwner )
			{
				bool bTeamSet = pOwner->AddCreateObjectItemInfo( iCreateIndex );
				if( bTeamSet )
					pField->SetTeamType( pOwner->GetTeam() );
			}

			m_pPlayMode->NotifyFieldItemSupply( pItem );
		}
		else
		{
			LOG.PrintTimeAndLog( 0, "ioPlayStage::OnFieldItemSupply - CreateFieldItem Fail: %s", szCreateItem.c_str() );
			delete pItem;
		}
	}
}

void ioPlayStage::OnFieldRewardItemList( SP2Packet &rkPacket )
{
	int iRewardSIze;
	rkPacket >> iRewardSIze;
	for(int i = 0;i < iRewardSIze;i++)
	{
		D3DXVECTOR3 vItemPos;
		ioHashString kOwnerName;
		int iUniqueIndex, iResourceValue;
		rkPacket >> iUniqueIndex >> iResourceValue >> kOwnerName >> vItemPos;

		ioFieldRewardItem *pFieldRewardItem = CreateFieldRewardItem( iUniqueIndex, iResourceValue );
		if( pFieldRewardItem )
		{
			pFieldRewardItem->SetOwnerCharName( kOwnerName );
			pFieldRewardItem->SetWorldPositionByServerSide( vItemPos );
			pFieldRewardItem->SetDropState( pFieldRewardItem->GetWorldPosition() );
			pFieldRewardItem->CreateDropEffect();
		}
	}
}

void ioPlayStage::OnFieldRewardItemChange( SP2Packet &rkPacket )
{
	int iDeleteItem;
	rkPacket >> iDeleteItem;

	int i = 0;
	for(i = 0;i < iDeleteItem;i++)
	{
		int iUniqueIndex;
		rkPacket >> iUniqueIndex;
		ioFieldRewardItem *pFieldRewardItem = GetFieldRewardItemByIndex( iUniqueIndex );
		if( pFieldRewardItem )
		{
			pFieldRewardItem->ReserveHiding(); // 소멸
		}
	}

	int iOwnerClearItem;
	rkPacket >> iOwnerClearItem;
	for(i = 0;i < iOwnerClearItem;i++)
	{
		int iUniqueIndex;
		rkPacket >> iUniqueIndex;
		ioFieldRewardItem *pFieldRewardItem = GetFieldRewardItemByIndex( iUniqueIndex );
		if( pFieldRewardItem )
		{
			pFieldRewardItem->SetOwnerCharName( "" );
		}
	}
}

void ioPlayStage::OnBadPingCrownDrop( SP2Packet &rkPacket )
{
	if( !m_pPlayMode )	return;

	int iCreateIndex;
	DWORD dwItemCode;
	int iItemReinforce;
	DWORD dwItemMaleCustom, dwItemFemaleCustom;
	D3DXVECTOR3 vItemPos;
	
	ioHashString szDroper;
	ioHashString szOwnerName;

	rkPacket >> szDroper;
	ioBaseChar *pChar = GetBaseChar( szDroper );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnBadPingCrownDrop - %s Not Exist User",
								szDroper.c_str() );
	}

	rkPacket >> dwItemCode >> iItemReinforce >> dwItemMaleCustom >> dwItemFemaleCustom >> iCreateIndex >> vItemPos >> szOwnerName;

	if( pChar )
	{
		ioItem *pDropItem = pChar->ReleaseItemExplicit( dwItemCode, iCreateIndex, ES_WEAR );
		if( !pDropItem )
			LOG.PrintTimeAndLog( 0, "ioPlayStage::OnBadPingCrownDrop - Crown Drop Fail!" );
		else
			SAFEDELETE( pDropItem );

		pChar->SetNetworkBadEmoticon();
	}

	ioItem *pItem = CreateItem( dwItemCode );
	if( !pItem )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnBadPingCrownDrop - Crown Create Fail!" );
		return;
	}

	pItem->ReLoadProperty();
	pItem->SetItemCreateIndex( iCreateIndex );
	pItem->SetOwnerName( szOwnerName );
	pItem->SetItemReinforce( iItemReinforce );
	pItem->SetItemCustom( dwItemMaleCustom, dwItemFemaleCustom );

	ioFieldItem *pField = CreateFieldItem( pItem );
	if( pField )
	{
		pField->SetWorldPositionByServerSide( vItemPos );
		pField->SetDropState( pField->GetWorldPosition() );
		pField->CreateDropEffect();

		switch( pItem->GetCrownType() )
		{
		case ioItem::MCT_HIDDEN_KING:
			{
				ioHiddenkingMode *pHiddenKing = ToHiddenkingMode( m_pPlayMode );
				if( pHiddenKing )				
					pHiddenKing->CrownBadPingDrop( szDroper );				
			}
			break;
		case ioItem::MCT_DOUBLE_CROWN:
			{
				ioDoubleCrownMode *pDouble = ToDoubleCrownMode( m_pPlayMode );
				if( pDouble )				
					pDouble->CrownBadPingDrop( szDroper );				
			}
			break;
		}

	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnBadPingCrownDrop - Crown Field Drop Fail!" );
	}
}

void ioPlayStage::OnFishingState( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnFishingState - %s Not Exist User", szName.c_str() );
		return;
	}

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case FISHING_START:
		if( GetModeType() == MT_TRAINING )
		{
			pChar->OnFishingState( rkPacket );
		}
		break;
	case FISHING_RESULT:
		pChar->OnFishingResult( rkPacket );

#if defined( USE_GA )
		// PLAY_PLAZA_FISHING_END
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Play"
			, "End"
			, ""
			, 1
			, "%2FPLAY%2FPLAZA%2FFISHING%2FEND" );
#endif
		break;
	case FISHING_END:
		if( GetModeType() == MT_TRAINING )
		{
			pChar->OnFishingEndState( rkPacket );
			pChar->ClearUseFishGroundOwnerInfo();
		}
		break;
	case FISHING_SELL:
		pChar->OnFishingSell( rkPacket );
		break;
	case FISHING_OPEN:
		{
			int iFishingLevel, iFishingExp;
			rkPacket >> iFishingLevel >> iFishingExp;

			g_MyInfo.SetFishingLevel( iFishingLevel, iFishingExp );

			bool bFirst;
			rkPacket >> bFirst;

			if( bFirst )
			{
				// MyInfo에 정보추가
				g_MyInfo.ApplyFishItemData( rkPacket );
			}

			// 낚시가방 열기
			g_MyInfo.CheckCurFishingTackle( true );
			FishingInventoryWnd *pWnd = dynamic_cast<FishingInventoryWnd*> (g_GUIMgr.FindWnd(FISHING_INVENTORY_WND));
			if( pWnd && !pWnd->IsShow() )
			{
				pWnd->ShowWnd();
				if( pChar->IsOwnerChar() && pChar->EnableFishing( true, true ) )
				{
					pChar->SetFishingState( true );
				}
			}
		}
		break;
	case FISHING_ERROR:
		{
			int iNum;
			rkPacket >> iNum;

			if( pChar->IsOwnerChar() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), iNum );
			}

			if( GetModeType() == MT_TRAINING )
			{
				pChar->OnFishingEndState( rkPacket );
			}
		}
		break;
	case FISHING_NONEXIST_BLOCK:
		{
			if( pChar->IsOwnerChar() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "낚시터를 찾을 수 없습니다." );
			}

			if( GetModeType() == MT_TRAINING )
			{
				pChar->OnFishingEndState( rkPacket );
			}
		}
		break;
	case FISHING_NONEXIST_STRUCT:
		{
			if( pChar->IsOwnerChar() )
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "낚시터를 찾을 수 없습니다." );
			}

			if( GetModeType() == MT_TRAINING )
			{
				pChar->OnFishingEndState( rkPacket );
			}
		}
		break;
	}
}

void ioPlayStage::OnDropItem( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnDropItem - %s Not Exist User",
								szName.c_str() );
		return;
	}

	int iSlot;
	rkPacket >> iSlot;

	int iItemCreateIndex;
	DWORD dwItemCode;
	int iItemReinforce;
	DWORD dwItemMaleCustom, dwItemFemaleCustom;
	rkPacket >> dwItemCode >> iItemReinforce >> dwItemMaleCustom >> dwItemFemaleCustom >> iItemCreateIndex;

	ioSkill *pSkill = pChar->GetEquipedSkill( iSlot );
	ioHashStringVec vExBuffList;
	vExBuffList.clear();
	float fNewGauge = 0.0f;
		
	if( pSkill &&  pSkill->IsCanUseSkillWithCrown( pChar ) )
	{
		bool bAddBuff = true;
		if ( pSkill->EnableExBuffOnHPZero() && pChar->GetHP().m_fCurValue > 0.0f )
		{
			bAddBuff = false;
		}

		if( pSkill->CheckGaugeMax( pChar->GetCharName() ) && !pChar->HasBuff( BT_SEAL_SKILL ) && bAddBuff )
		{
			vExBuffList = pSkill->GetExBuffList();
			if( !vExBuffList.empty() )
				fNewGauge = pChar->ReduceNeedGaugeBySkill( pSkill, pSkill->GetNeedGauge(pChar->GetCharName()) );
		}
	}

	// 팀 서바 AI 모드에선 NPC랑 유저가 착용할 코드가 다르기 때문에 강제로 벗기고 삭제, 생성되는 아이템은 서버에서 보내준걸로..
	// ai는 아이템을 줍지 않음
	ioItem *pPreItem = NULL;
	ioNpcChar* pNPC = ToNpcChar( pChar );
	if( m_pPlayMode->GetModeType() == MT_TEAM_SURVIVAL_AI && pNPC )
		pPreItem = pChar->ReleaseNCreateExplict( dwItemCode, iItemCreateIndex, iSlot );
	else
		pPreItem = pChar->ReleaseItemExplicit( dwItemCode, iItemCreateIndex, iSlot );
	if( !pPreItem )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnDropItem - %s Not Has Item(%d)",
								szName.c_str(), dwItemCode );
		return;
	}
	// 아이템이 벗겨 진 이후 스킬포인터의 안정성을 보장할 수 없으므로 쓰지 말것.
	pSkill = NULL;

	ioHashString szOwnerName;
	D3DXVECTOR3 vDropStartPos;
	float fCurItemSkillGauge;
	rkPacket >> szOwnerName >> vDropStartPos >> fCurItemSkillGauge;

	int iCurBullet;
	rkPacket >> iCurBullet;

	pPreItem->SetItemReinforce( iItemReinforce );
	pPreItem->SetItemCustom( dwItemMaleCustom, dwItemFemaleCustom );
	pPreItem->SetCurSkillGauge( fCurItemSkillGauge );
	pPreItem->SetCurBullet( iCurBullet );

	if( !vExBuffList.empty() )
	{
		bool bGrappling = true;
		if( !pChar->CheckGrapplingExceptionState() )
		{
			bGrappling = false;
		}
		if ( bGrappling )
		{
			int iCnt = vExBuffList.size();
			for( int i=0; i<iCnt; ++i )
			{
				ioHashString szCreator = pChar->GetCharName();
				pChar->AddNewBuff( vExBuffList[i], szCreator, "", NULL );
			}

			pPreItem->SetCurSkillGauge( fNewGauge );
		}
	}

	ioFieldItem *pDropItem = DropFieldItem( vDropStartPos, pPreItem, pChar );
	if( !pDropItem )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnDropItem - Create FieldItem Error" );
		delete pPreItem;
		return;
	}
	
	switch( pPreItem->GetCrownType() )
	{
	case ioItem::MCT_HIDDEN_KING:
		{
			ioHiddenkingMode *pHiddenKing = ToHiddenkingMode( m_pPlayMode );
			if( pHiddenKing )
			{
				pHiddenKing->CrownDrop( pChar->GetLastAttackerName(), pChar->GetCharName() );
			}
		}
		break;
	case ioItem::MCT_DOUBLE_CROWN:
		{
			ioDoubleCrownMode *pDoubleCrown = ToDoubleCrownMode( m_pPlayMode );
			if( pDoubleCrown )
			{
				pDoubleCrown->CrownDrop( pChar->GetLastAttackerName(), pChar->GetCharName() );
			}
		}
		break;
	default:
		{
			ioHeadquartersMode *pHeadquarters = ToHeadquartersMode( m_pPlayMode );
			if( pHeadquarters && pHeadquarters->GetCharState() == ioHeadquartersMode::STATE_DISPLAY_CHAR )
			{
				if( ToNpcChar( pChar ) )
					pDropItem->SetDroppingEndHide();
			}
		}
		break;
	}
}

void ioPlayStage::OnItemMoveDrop( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemMoveDrop - %s Not Exist User", szName.c_str() );
		return;
	}

	int iSlot;
	rkPacket >> iSlot;

	int iItemCreateIndex;
	DWORD dwItemCode;
	int iItemReinforce;
	DWORD dwItemMaleCustom, dwItemFemaleCustom;
	rkPacket >> dwItemCode >> iItemReinforce >> dwItemMaleCustom >> dwItemFemaleCustom >> iItemCreateIndex;

	ioItem *pPreItem = pChar->ReleaseItemExplicit( dwItemCode, iItemCreateIndex, iSlot );
	if( !pPreItem )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemMoveDrop - %s Not Has Item(%d)", szName.c_str(), dwItemCode );
		return;
	}

	ioHashString szOwnerName;
	D3DXVECTOR3 vDropStartPos;
	float fCurItemSkillGauge;
	rkPacket >> szOwnerName >> vDropStartPos >> fCurItemSkillGauge;

	int iCurBullet;
	rkPacket >> iCurBullet;

	pPreItem->SetItemReinforce( iItemReinforce );
	pPreItem->SetItemCustom( dwItemMaleCustom, dwItemFemaleCustom );
	pPreItem->SetCurSkillGauge( fCurItemSkillGauge );
	pPreItem->SetCurBullet( iCurBullet );

	// Drop Info
	ioHashString szAttacker, szSkillName;
	D3DXVECTOR3 vTargetPos;
	float fMoveSpeed;

	rkPacket >> szAttacker >> szSkillName >> vTargetPos >> fMoveSpeed;

	ioFieldItem *pDropItem = MoveDropFieldItem( szAttacker, szSkillName, vDropStartPos, vTargetPos, fMoveSpeed, pPreItem, pChar );
	if( !pDropItem )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemMoveDrop - Create FieldItem Error" );
		delete pPreItem;
		return;
	}
}

void ioPlayStage::OnDropItemFail( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnDropItemFail - %s Not Exist User",
								szName.c_str() );
		return;
	}

	int iItemSlot;
	rkPacket >> iItemSlot;

	pChar->GetEquipSlot()->SetDropWaitSlot( iItemSlot, false );
}

void ioPlayStage::OnAllItemDrop( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnAllItemDrop - %s Not Exist User", szName.c_str() );
		return;
	}

	int iItemCnt;
	rkPacket >> iItemCnt;

	for( int i=0; i < iItemCnt; ++i )
	{
		int iSlot;
		rkPacket >> iSlot;

		int iItemCreateIndex;
		DWORD dwItemCode;
		int iItemReinforce;
		DWORD dwItemMaleCustom, dwItemFemaleCustom;
		rkPacket >> dwItemCode >> iItemReinforce >> dwItemMaleCustom >> dwItemFemaleCustom >> iItemCreateIndex;

		ioHashString szOwnerName;
		D3DXVECTOR3 vDropStartPos;
		float fCurItemSkillGauge;
		int iCurBullet;
		rkPacket >> szOwnerName >> vDropStartPos >> fCurItemSkillGauge;
		rkPacket >> iCurBullet;

		if(	dwItemCode == 0 )
		{
			LOG.PrintTimeAndLog( 0, "ioPlayStage::OnAllItemDrop - %s Not Has Item(%d)", szName.c_str(), dwItemCode );
			continue;
		}

		ioItem *pPreItem = pChar->ReleaseItemExplicit( dwItemCode, iItemCreateIndex, iSlot );
		if( !pPreItem )
		{
			LOG.PrintTimeAndLog( 0, "ioPlayStage::OnAllItemDrop - %s Not Has Item(%d)", szName.c_str(), dwItemCode );
			continue;
		}

		pPreItem->SetItemReinforce( iItemReinforce );
		pPreItem->SetItemCustom( dwItemMaleCustom, dwItemFemaleCustom );
		pPreItem->SetCurSkillGauge( fCurItemSkillGauge );
		pPreItem->SetCurBullet( iCurBullet );
				
		if( !pChar->IsEnableDropObjectItem( pPreItem ) )
		{
			delete pPreItem;
			continue;
		}

		ioFieldItem *pDropItem = DropFieldItem( vDropStartPos, pPreItem, pChar );
		if( !pDropItem )
		{
			LOG.PrintTimeAndLog( 0, "ioPlayStage::OnAllItemDrop - Create FieldItem Error" );
			delete pPreItem;
			continue;
		}
		
		switch( pDropItem->GetCrownItemType() )
		{
		case ioItem::MCT_HIDDEN_KING:
			{
				ioHiddenkingMode *pHiddenKing = ToHiddenkingMode( m_pPlayMode );
				if( pHiddenKing )
				{
					pHiddenKing->CrownDrop( pChar->GetLastAttackerName(), pChar->GetCharName() );
				}
			}
			break;
		case ioItem::MCT_DOUBLE_CROWN:
			{
				ioDoubleCrownMode *pDoubleCrown = ToDoubleCrownMode( m_pPlayMode );
				if( pDoubleCrown )
				{
					pDoubleCrown->CrownDrop( pChar->GetLastAttackerName(), pChar->GetCharName() );
				}
			}
			break;
		}
	}
}

void ioPlayStage::OnImmediatelyEquipItem( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnImmediatelyEquipItem - %s Not Exist User", szName.c_str() );
		return;
	}

	if( pChar->GetState() == CS_DIE )
		return;

	int iCreateIndex;
	DWORD dwItemCode;
	int iItemReinforce;
	DWORD dwItemMaleCustom, dwItemFemaleCustom;
	ioHashString szCreateItem;
	int eObjectCreateType = ioObjectItem::OCT_NONE;

	rkPacket >> dwItemCode >> iItemReinforce >> dwItemMaleCustom >> dwItemFemaleCustom >> iCreateIndex;
	rkPacket >> szCreateItem;
	rkPacket >> eObjectCreateType;

	ioHashString szControlBuffName;
	if( eObjectCreateType == ioObjectItem::OCT_EQUIP_BUFF1 || eObjectCreateType == ioObjectItem::OCT_EQUIP_BUFF2 )
	{
		rkPacket >> szControlBuffName;
	}

	bool bIsExistItem = false;
	ioItem* pItem = NULL;
	pItem = pChar->GetEquipedItemByItemCode( dwItemCode );
	if( !pItem || ( pItem && pItem->GetItemCreateIndex() != 0 ) )
		pItem = CreateItem( dwItemCode );
	else
		bIsExistItem = true;

	if( !pItem )
		return;

	pItem->ReLoadProperty();
	pItem->SetItemCreateIndex( iCreateIndex );
	pItem->SetOwnerName( pChar->GetCharName() );
	if( eObjectCreateType == 5 )
	{
		pItem->SetItemReinforce( 0 );
	}
	else
	{
		pItem->SetItemReinforce( iItemReinforce );
		pItem->SetItemCustom( dwItemMaleCustom, dwItemFemaleCustom );
	}

	if( eObjectCreateType <= ioObjectItem::OCT_EQUIP_BUFF2 )
	{
		ioObjectItem *pObject = ToObjectItem( pItem );
		if( pObject )
		{
			if( !szControlBuffName.IsEmpty() )
			{
				//컨트롤 버프 지정, 해당 아이템(OCT_EQUIP_BUFF1,2타입)을 장착한 캐릭터에 지정된 버프가 없으면 CheckReleaseProcess()에 의해서 제거됨
				pObject->SetControlBuff( szControlBuffName );

				switch( eObjectCreateType )
				{
				case ioObjectItem::OCT_EQUIP_BUFF1:
					{
						//OCT_EQUIP_BUFF1
						ioTargetEquipObjectItemBuff* pEquipBuff = ToTargetEquipObjectItemBuff( pChar->GetBuff( szControlBuffName ) );
						if( pEquipBuff )
						{
							pEquipBuff->SetEquipState();
						}
					}
					break;
				case ioObjectItem::OCT_EQUIP_BUFF2:
					{
						//OCT_EQUIP_BUFF2
						ioCreateObjectItemBuff* pCraeteBuff = ToCreateObjectItemBuff( pChar->GetBuff( szControlBuffName ) );
						if( pCraeteBuff )
						{
							pCraeteBuff->SetEquipState();
						}
					}
					break;
				}
			}

			pChar->GetEquipSlot()->SetEquipWaitSlot( ES_OBJECT, false );
			pObject->SetObejctCreateType( static_cast<ioObjectItem::ObejctCreatorType>( eObjectCreateType ) );

			if( !szCreateItem.IsEmpty() )
			{
				pObject->SetObjectCreateItem( szCreateItem );

				ioAttackableItem* pAttackItem = ToAttackableItem( pChar->GetEquipedItem( ES_WEAPON ) );
				if( pAttackItem )
					pAttackItem->EquipObjectItemByAttakable( pChar, pItem );
			}
		}

		ioItem *pPreItem = NULL;
		if( !bIsExistItem )
			pPreItem = pChar->EquipItem( pItem );
		if( pPreItem )
		{
			LOG.PrintTimeAndLog( 0, "ioPlayStage::OnImmediatelyEquipItem() - PreItem is Exist : %d", pPreItem->GetItemCode() );
		}
	}
	else if( eObjectCreateType == ioObjectItem::OCT_STEAL_EQUIP_BUFF )
	{
		ioItem *pPreItem = pChar->EquipItem( pItem );
		ioBaseChar* pOwner = GetBaseChar( pChar->GetCharName() );
		pItem->Initialize( pOwner, this );

		if( pItem && pPreItem )
			pItem->SetCurSkillGauge( pPreItem->GetCurSkillGuage() );

		SAFEDELETE( pPreItem );
	}
}

void ioPlayStage::OnEtcItemUse( SP2Packet &rkPacket )
{
	int   iResult = 0;
	int   iType   = 0;
	int   iValue1 = 0;
	int   iValue2 = 0;

	rkPacket >> iResult;

	// MyInventoryWnd에 대한 업데이트를 각 상황별로 따로 처리
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	
	// warning
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		if( pInvenWnd && pInvenWnd->IsShow() )
			pInvenWnd->UpdateTab( false, true );

		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnEtcITemUse - UserEtcItem == NULL" );
		return;
	}

	if( iResult == ETCITEM_USE_EXCEPTION )
	{
		TCPNetwork::MouseBusy( false );

		if( pInvenWnd && pInvenWnd->IsShow() )
			pInvenWnd->UpdateTab( false, true );

		// 임시 : 특별아이템 없어지는 경우가 자주 발생하여 일단 차단.
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnEtcITemUse - Exception" );
		return;
	}
	else if( iResult == ETCITEM_USE_DONT_HAVE )
	{
		TCPNetwork::MouseBusy( false );

		if( pInvenWnd && pInvenWnd->IsShow() )
			pInvenWnd->UpdateTab( false, true );

		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}
	else if( iResult == ETCITEM_USE_WRONG_ID )
	{
		TCPNetwork::MouseBusy( false );

		if( pInvenWnd && pInvenWnd->IsShow() )
		{
			pInvenWnd->UpdateTab( false, false );
			pInvenWnd->OnErrorEdit( STR(3) );
		}
		return;
	}
	else if( iResult == ETCITEM_USE_MOVING_SERVER )
	{
		TCPNetwork::MouseBusy( false );

		if( pInvenWnd && pInvenWnd->IsShow() )
		{
			pInvenWnd->UpdateTab( false, false );
			pInvenWnd->OnErrorEdit( STR(4) );
		}
		return;
	}
	else if( iResult == ETCITEM_USE_EXIST_ID )
	{
		TCPNetwork::MouseBusy( false );

		if( pInvenWnd && pInvenWnd->IsShow() )
		{
			pInvenWnd->UpdateTab( false, false );
			pInvenWnd->OnErrorEdit( STR(5) );
		}
		return;
	}
	else if( iResult == ETCITEM_USE_RESERVED_CHANGE_ID )
	{
		TCPNetwork::MouseBusy( false );

		if( pInvenWnd && pInvenWnd->IsShow() )
		{
			pInvenWnd->UpdateTab( false, false );
			pInvenWnd->OnErrorEdit( STR(6) );
		}
		return;
	}
	else if( iResult == ETCITEM_USE_ALREADY_CHAR )
	{
		TCPNetwork::MouseBusy( false );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
		return;
	}
	else if( iResult == ETCITEM_USE_CHAR_BEGINNING )
	{
		TCPNetwork::MouseBusy( false );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );
		return;
	}
	else if( iResult == ETCITEM_USE_CHAR_SLOT_FULL )
	{
		TCPNetwork::MouseBusy( false );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10) );
		return;
	}
	else if( iResult == ETCITEM_USE_CHAR_NONE )
	{
		TCPNetwork::MouseBusy( false );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11) );
		return;
	}
	else if( iResult == ETCITEM_USE_CHAR_RENTAL )
	{
		TCPNetwork::MouseBusy( false );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(12) );
		return;
	}
	else if( iResult == ETCITEM_USE_CHAR_LIMIT_LEVEL )
	{
		TCPNetwork::MouseBusy( false );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(13) );
		return;
	}
	else if( iResult == ETCITEM_USE_DECO_SLOT_FULL )
	{
		TCPNetwork::MouseBusy( false );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(14) );
		return;
	}
	else if( iResult != ETCITEM_USE_OK )
	{
		// 임시 : 특별아이템 없어지는 경우가 자주 발생하여 일단 차단.
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnEtcITemUse - Error :%d", iResult );
		return;
	}
	
	rkPacket >>iType;

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iType );
	if( !pEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "EtcItemUse pEtcItem == NULL (Type = %d)", iType );
		return;
	}

	pEtcItem->OnUse( iType, rkPacket, pUserEtcItem, this );

	SoldierSelectWnd *pSoldierWnd = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
	if( pSoldierWnd && pSoldierWnd->IsShow() )
	{
		pSoldierWnd->SettingSoldierBtn();
	}

	MiniSoldierSelectWnd *pMiniSoldierWnd = dynamic_cast<MiniSoldierSelectWnd*>( g_GUIMgr.FindWnd( MINI_SOLDIER_SELECT_WND ));
	if( pMiniSoldierWnd && pMiniSoldierWnd->IsShow() )
	{
		pMiniSoldierWnd->SettingMiniSoldierBtn();
	}

	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		bool bInfo = false;
		if( iType == ioEtcItem::EIT_ETC_FISHING_PACKAGE  ||   // 선물로 발송하는 패키지는 갱신
			iType == ioEtcItem::EIT_ETC_PESO_EXP_PACKAGE ||
			iType == ioEtcItem::EIT_ETC_SPECIAL_PACKAGE1 ) 
			bInfo = true;

		pInvenWnd->UpdateTab( false, bInfo );
	}

	//소울 스톤 추가
	// UI상에 소울스톤 개수가 존재. ETC Item이 갱신된 후에 UI를 Show 해준다.
	//if( iType == ioEtcItem::EIT_ETC_SOUL_STONE )
	//{
	//	if( g_PowerUpManager.GetReinforcedCode() != 0 )
	//	{
	//		if( pInvenWnd && pInvenWnd->IsShow() )
	//			pInvenWnd->ShowHeroReinforceDoneWnd( g_PowerUpManager.GetReinforcedCode() );
	//	}
	//}	
}

void ioPlayStage::OnEtcItemSell( SP2Packet &rkPacket )
{
	int   iResult = 0;
	int   iType   = 0;
	rkPacket >> iResult;

	// 성공 아닌 경우를 위한 '내아이템' 업데이트
	if( iResult != ETCITEM_SELL_OK )
	{
		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
		if( pInvenWnd && pInvenWnd->IsShow() )
		{
			pInvenWnd->UpdateTab( true, true );
		}
	}

	TCPNetwork::MouseBusy( false );
	// warning
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnEtcItemSell - UserEtcItem == NULL" );
		return;
	}

	if( iResult == ETCITEM_SELL_EXCEPTION )
	{
		// 임시 : 특별아이템 없어지는 경우가 자주 발생하여 일단 차단.
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	else if( iResult == ETCITEM_SELL_DONT_HAVE )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		return;
	}
	else if( iResult == ETCITEM_SELL_MOVING_SERVER )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}
	else if( iResult == ETCITEM_SELL_DONT_SELL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}
	else if( iResult != ETCITEM_SELL_OK )
	{
		// 임시 : 특별아이템 없어지는 경우가 자주 발생하여 일단 차단.
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5), iResult );
		return;
	}

	rkPacket >> iType;
	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iType );
	if( !pEtcItem )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnEtcItemSell pEtcItem == NULL" );
		return;
	}
	pEtcItem->OnSell( iType, rkPacket, pUserEtcItem );
}

void ioPlayStage::OnMovieControl( SP2Packet &rkPacket )
{
	int iMovieType;
	ioHashString szCharName;
	rkPacket >> iMovieType >> szCharName;

	ioBaseChar *pChar = GetBaseChar( szCharName );
	if( !pChar ) return;

	switch( iMovieType )
	{
	case MOVIE_CAPTURE_START:
		pChar->SetMovieStartEmoticon( g_MovieMaker.GetMovieStartTime() );
		break;
	case MOVIE_CAPTURE_CUT:
		pChar->SetMovieCutEmoticon();
		break;
	}
}

void ioPlayStage::OnAbsorbInfo( SP2Packet &rkPacket )
{
	int iMineCnt, iFieldCnt, iStructCnt;
	int i = 0;

	rkPacket >> iMineCnt;
	for( i=0; i < iMineCnt; ++i )
	{
		int iMineIndex;
		rkPacket >> iMineIndex;

		ioWeaponManager::WeaponIterator kMine = g_WeaponMgr.GetMineWeaponIterator();
		while( kMine.HasMoreElements() )
		{
			ioMineWeapon *pMine = ToMineWeapon( kMine.Next() );
			if( !pMine || !pMine->IsLive() )
				continue;

			if( iMineIndex == pMine->GetWeaponIndex() )
			{
				pMine->SetWeaponDead();
			}
		}
	}

	rkPacket >> iFieldCnt;
	for( i=0; i < iFieldCnt; ++i )
	{
		int iCreateIndex;
		rkPacket >> iCreateIndex;
		
		HidingFieldItem( iCreateIndex );
	}

	rkPacket >> iStructCnt;
	for( i=0; i < iStructCnt; ++i )
	{
		int iStructIndex;
		rkPacket >> iStructIndex;

		ioPushStruct *pPush = g_PushStructListMgr.FindPushStruct( iStructIndex );

		if( pPush )
		{
			pPush->StructDie( true );
		}
	}

	int iTotalCnt = iMineCnt + iFieldCnt + iStructCnt;

	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;


	ioHashString szSkillName;
	rkPacket >> szSkillName;

	if( pChar->IsCurUsingSkill( szSkillName ) )
	{
		for( int k=0; k < ES_OBJECT; k++ )
		{
			ioAbsorbSkill *pAbsorb = ToAbsorbSkill( pChar->GetEquipedSkill( k ) );

			if( !pAbsorb ) continue;
			if( pAbsorb->GetName() == szSkillName )
			{
				pAbsorb->ApplyAbsorbInfo( pChar, iTotalCnt );
				break;
			}
		}
	}
}

void ioPlayStage::OnChatModeState( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnChatModeState - %s Not Exist", szName.c_str() );
		return;
	}

	int iSubType;
	bool bChatMode;
	rkPacket  >> iSubType >> bChatMode;

	switch( iSubType )
	{
	case CHAT_MODE_NONE:
		if( bChatMode )
		{
			pChar->SetChatMode( CMS_CHAT );
		}
		else
		{
			pChar->SetChatMode( CMS_NONE );
		}
		break;
	case CHAT_MODE_CHAT:
		pChar->SetChatMode( CMS_CHAT );
		break;
	case CHAT_MODE_CHANGE_CHAT:
		pChar->SetChatMode( CMS_CHANGE_CHAT );
		break;
	case CHAT_MODE_CHANGE_NORMAL:
		pChar->SetChatMode( CMS_CHANGE_NORMAL );
		break;
	}
}

void ioPlayStage::OnExperienceModeState( SP2Packet &rkPacket )
{
	ioHashString kName;
	rkPacket >> kName;

	ioBaseChar *pChar = GetBaseChar( kName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnExperienceModeState - %s Not Exist", kName.c_str() );
		return;
	}

	int iSubType;
	rkPacket >> iSubType;	

	int  iExperienceClassType = 0;
	ioHashString szExperienceID;
	rkPacket >> szExperienceID >> iExperienceClassType;
	if( pChar->IsOwnerChar() == false )
	{
		pChar->SetExperienceMode( iSubType, szExperienceID, iExperienceClassType );
	}

	if( iSubType == EMS_EXPERIENCE )
	{
		pChar->SetNoChangeCharOnExperienceChange( false );
		if( pChar->IsOwnerChar() )
			pChar->SetOnlyExperienceValue( EMS_EXPERIENCE );
	}

	bool bExperienceChar;
	rkPacket >> bExperienceChar;
	
	if( bExperienceChar )
	{
		// 치장 정보 교체
		const CHARACTER &rkOriginal = pChar->GetCharacterInfo();
		CHARACTER kExperienceChar = rkOriginal;
		rkPacket >> kExperienceChar.m_kindred >> kExperienceChar.m_sex >> kExperienceChar.m_face >> kExperienceChar.m_hair >> kExperienceChar.m_hair_color
				 >> kExperienceChar.m_skin_color >> kExperienceChar.m_beard >> kExperienceChar.m_accessories >> kExperienceChar.m_underwear;
		
		for ( int i=0; i<MAX_INVENTORY; i++ )
		{
			kExperienceChar.m_EquipCostume[i].Init();
			rkPacket >> kExperienceChar.m_EquipCostume[i].m_CostumeCode;
		}

		ReLoadCharInfo( pChar->GetIndex(), kExperienceChar );	

		// 아이템 착용
		float fPrevSkillGauge[MAX_INVENTORY] = { 0.0f, 0.0f, 0.0f, 0.0f };
		for( int i = 0;i < MAX_INVENTORY;i++)
		{
			ioItem *pItem = pChar->GetEquipedItem( i );
			if( pItem )
			{
				fPrevSkillGauge[i] = pItem->GetCurSkillGuage();
			}
		}
		ChangeCharEquipItem( pChar, fPrevSkillGauge, rkPacket, false );

		if( pChar->IsOwnerChar() )
			g_GUIMgr.ShowWnd( SOLDIER_EXPERIENCE_WND );
	}
}

void ioPlayStage::OnHeadquartersInfo( SP2Packet &rkPacket )
{
	HeadquartersInfoWnd *pInfoWnd = dynamic_cast<HeadquartersInfoWnd*>(g_GUIMgr.FindWnd( HEADQUARTERS_INFO_WND ));
	if( pInfoWnd )
		pInfoWnd->SetInfo( rkPacket );
}

void ioPlayStage::OnHouseInfo( SP2Packet &rkPacket )
{
	HouseInfoWnd *pInfoWnd = dynamic_cast<HouseInfoWnd*>(g_GUIMgr.FindWnd( HOUSE_INFO_WND ));
	if( pInfoWnd )
		pInfoWnd->SetInfo( rkPacket );
}

void ioPlayStage::OnHeadquartersJoinRequest( SP2Packet &rkPacket )
{
	ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( GetPlayMode() );
	if( pHeadquartersMode == NULL )
		return;
	if( pHeadquartersMode->GetMasterName() != g_MyInfo.GetPublicID() )
		return;
	if( g_MyInfo.IsTutorialUser() )
		return;

	HeadquartersJoinRequestWnd *pJoinRequestWnd = dynamic_cast< HeadquartersJoinRequestWnd * >( g_GUIMgr.FindWnd( HEADQUARTERS_JOIN_REQUEST_WND ) );
	if( pJoinRequestWnd )
	{
		ioHashString kRequestName;
		rkPacket >> kRequestName;
		pJoinRequestWnd->SetRequestName( kRequestName );
		pJoinRequestWnd->ShowWnd();
	}
}

void ioPlayStage::OnHouseJoinRequest( SP2Packet &rkPacket )
{
	ioHouseMode *pHouseMode = ToHouseMode( GetPlayMode() );
	if( pHouseMode == NULL )
		return;
	if( pHouseMode->GetMasterName() != g_MyInfo.GetPublicID() )
		return;
	if( g_MyInfo.IsTutorialUser() )
		return;

	HouseJoinRequestWnd* pJoinRequestWnd = dynamic_cast< HouseJoinRequestWnd* >( g_GUIMgr.FindWnd( HOUSE_JOIN_REQUEST_WND ) );
	if( pJoinRequestWnd )
	{
		ioHashString kRequestName;
		rkPacket >> kRequestName;
		pJoinRequestWnd->SetRequestName( kRequestName );
		pJoinRequestWnd->ShowWnd();
	}
}

void ioPlayStage::OnHeadquartersInvite( SP2Packet &rkPacket )
{
	if( g_BattleRoomMgr.IsBattleRoom() )
		return;
	if( g_LadderTeamMgr.IsLadderTeam() )
		return;
	if( g_ShuffleRoomMgr.IsShuffleRoom() )
		return;
	if( g_RankBattleMgr.IsRankBattlePlaying() )
		return;

	if( g_MyInfo.IsTutorialUser() )
		return;

	ioHashString kMasterName;
	rkPacket >> kMasterName;

	ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( GetPlayMode() );
	if( pHeadquartersMode )
	{
		if( kMasterName == pHeadquartersMode->GetMasterName() )
			return;
	}

	HeadquartersInvitedWnd *pInvitedWnd = dynamic_cast< HeadquartersInvitedWnd * >( g_GUIMgr.FindWnd( HEADQUARTERS_INVITED_WND ) );
	if( pInvitedWnd )
	{
		pInvitedWnd->SetInfo( kMasterName, rkPacket );
	}
}

void ioPlayStage::OnHouseInvite( SP2Packet &rkPacket )
{
	if( g_BattleRoomMgr.IsBattleRoom() )
		return;
	if( g_LadderTeamMgr.IsLadderTeam() )
		return;
	if( g_ShuffleRoomMgr.IsShuffleRoom() )
		return;
	if( g_RankBattleMgr.IsRankBattlePlaying() )
		return;

	if( g_MyInfo.IsTutorialUser() )
		return;

	ioHashString kMasterName;
	rkPacket >> kMasterName;

	ioHouseMode *pHouseMode = ToHouseMode( GetPlayMode() );
	if( pHouseMode )
	{
		if( kMasterName == pHouseMode->GetMasterName() )
			return;
	}

	HouseInvitedWnd* pInvitedWnd = dynamic_cast< HouseInvitedWnd* >( g_GUIMgr.FindWnd( HOUSE_INVITED_WND ) );
	if( pInvitedWnd )
	{
		pInvitedWnd->SetInfo( kMasterName, rkPacket );
	}
}

void ioPlayStage::OnPickItem( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPickItem - %s Not Exist User", szName.c_str() );
		return;
	}

	if( !pChar->CheckGrapplingExceptionState() )
		return;

	int iCreateIndex;
	rkPacket >> iCreateIndex;

	int iEquipedBullet;
	float fEquipedGauge;
	rkPacket >> iEquipedBullet;
	rkPacket >> fEquipedGauge;

	D3DXVECTOR3 vDropPos;
	rkPacket >> vDropPos;

	PickFieldItem( vDropPos, iCreateIndex, iEquipedBullet, fEquipedGauge, pChar );
}

void ioPlayStage::OnPickItemFail( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPickItemFail - %s Not Exist User",
								szName.c_str() );
		return;
	}

	int iCreateIndex;
	int iItemSlot;
	rkPacket >> iCreateIndex;
	rkPacket >> iItemSlot;

	pChar->GetEquipSlot()->SetDropWaitSlot( iItemSlot, false );
	pChar->GetEquipSlot()->SetEquipWaitSlot( iItemSlot, false );

	ioFieldItem *pFieldItem = GetFieldItemByIndex( iCreateIndex );
	if( !pFieldItem )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPickItemFail - %s/%d Not Exist",
								pChar->GetCharName().c_str(), iCreateIndex );
		return;
	}
}

void ioPlayStage::OnDeleteFieldItem( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnDeleteFieldItem - %s Not Exist User", szName.c_str() );
		return;
	}

	int iCreateIndex;
	rkPacket >> iCreateIndex;
	ReleaseFieldItemByIndex( iCreateIndex );
}

void ioPlayStage::OnDeleteFieldItemFail( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnDeleteFieldItemFail - %s Not Exist User",
								szName.c_str() );
		return;
	}

	int iCreateIndex;
	rkPacket >> iCreateIndex;
	ioFieldItem *pFieldItem = GetFieldItemByIndex( iCreateIndex );
	if( !pFieldItem )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnDeleteFieldItemFail - %s/%d Not Exist",
								pChar->GetCharName().c_str(), iCreateIndex );
		return;
	}
}

void ioPlayStage::OnPickRewardItem( SP2Packet &rkPacket )
{
	bool bResult;
	ioHashString kCharName;
	int iFieldRewardUniqueIndex;
	rkPacket >> bResult >> kCharName >> iFieldRewardUniqueIndex;

	if( bResult )
	{
		ioFieldRewardItem *pFieldRewardItem = GetFieldRewardItemByIndex( iFieldRewardUniqueIndex );
		if( !pFieldRewardItem )
		{
			LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPickRewardItem - %s/%d Not Exist",
									kCharName.c_str(), iFieldRewardUniqueIndex );
			return;
		}
		pFieldRewardItem->SetDestroy();
	}
}

//떨어져 죽음
void ioPlayStage::OnDropDie( SP2Packet &rkPacket )
{
	int iAttackerTeam;
	ioHashString szDier, szAttacker, szBestAttacker, szAttackerSkill;
	DWORD dwAttackerWeaponItemCode;
	rkPacket >> szDier >> szAttacker >> szAttackerSkill >> dwAttackerWeaponItemCode >> iAttackerTeam >> szBestAttacker;

	float fLastRate, fBestRate;
	rkPacket >> fLastRate >> fBestRate;

	if( !m_pPlayMode )
		return;

	ioBaseChar *pChar = GetBaseChar( szDier );
	if( !pChar )	return;

	pChar->SetDieLastAttackerInfo( szAttacker, szAttackerSkill, (TeamType)iAttackerTeam, dwAttackerWeaponItemCode );

	ioBaseChar *pKillChar = GetBaseChar( szAttacker );
	ioBaseChar *pBestAttacker = GetBaseChar( szBestAttacker );

	m_pPlayMode->NotifyCharDropDie( pChar, pKillChar, pBestAttacker, fLastRate, fBestRate );

	pChar->SetDieState();

	m_pPlayMode->ApplyCharModeInfo( szDier, rkPacket );
	m_pPlayMode->ApplyCharModeInfo( szAttacker, rkPacket );

	ClearUserCreateEntities( szDier );
 
	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( pAttacker )
	{
		if( pAttacker->IsOwnerChar() )
		{
			g_ShakeCameraMgr.SetShakeCamera( pAttacker->GetKillShakeCamera(), ShakeCamera::SP_HIGH );

			if( pAttacker->GetState() == CS_USING_SKILL )
				StartScreenBlind( SBT_KILL, NULL, true );
			else
				StartScreenBlind( SBT_KILL, NULL, true );
		}

		//팀 제외
		if( m_pPlayMode && pAttacker->GetTeam() != pChar->GetTeam() )
		{
			if( m_pPlayMode && m_pPlayMode->GetModeType() != MT_FOOTBALL )
				g_AwardMgr.AddDrop( szAttacker, FLOAT100 * m_pPlayMode->ContributeTeamPerA( pAttacker->GetTeam(), 8.0f, FLOAT1 ) );
		}

		if ( pAttacker->IsNeedProcess() )
			g_RSoldierInfoManager.PlaySoldierSound( pAttacker, SST_Kill );
	}	

	m_pPlayMode->KillContribute( szAttacker, szDier, dwAttackerWeaponItemCode );
	m_pPlayMode->CheckOwnerNonePlayTime();
	if( !pChar->IsOwnerChar() )
	{
		// 죽은 유저의 데미지 리스트는 All에 넣고 부활 후 부터 다시 체크한다.
		m_pPlayMode->AttachOwnerDamageList( pChar->GetCharName() );
	}

	// 강시 모드에서 죽으면 강시됨
	if( GetModeType() == MT_GANGSI )
	{
		pChar->SetTeam( TEAM_BLUE );
	}

	// 배틀모드 스코어 갱신
	if( GetModeType() == MT_BATTLE )
	{
		ioBattleMode* pBattleMode = ToBattleMode( m_pPlayMode );
		if( pBattleMode )
			pBattleMode->ChangeScore( pChar->GetTeam() );
	}
}

//맞아 죽음.
void ioPlayStage::OnWeaponDie( SP2Packet &rkPacket )
{
	int iAttackerTeam;
	ioHashString szDier, szAttacker, szBestAttacker, szAttackerSkill;
	DWORD dwAttackerWeaponItemCode;
	rkPacket >> szDier >> szAttacker >> szAttackerSkill >> dwAttackerWeaponItemCode >> iAttackerTeam >> szBestAttacker;

	float fLastRate, fBestRate;
	rkPacket >> fLastRate >> fBestRate;

	ioBaseChar *pChar = GetBaseChar( szDier );
	if( !pChar )	return;

	pChar->SetDieLastAttackerInfo( szAttacker, szAttackerSkill, (TeamType)iAttackerTeam, dwAttackerWeaponItemCode );

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	ioBaseChar *pBestAttacker = GetBaseChar( szBestAttacker );
	m_pPlayMode->NotifyEntityDie( pChar, pAttacker, pBestAttacker, fLastRate, fBestRate );
	
	pChar->SetDieState();

	m_pPlayMode->ApplyCharModeInfo( szDier, rkPacket );
	m_pPlayMode->ApplyCharModeInfo( szAttacker, rkPacket );

	ClearUserCreateEntities( szDier );

	m_pPlayMode->KillContribute( szAttacker, szDier, dwAttackerWeaponItemCode );
	m_pPlayMode->CheckOwnerNonePlayTime();

	if( pAttacker )
	{
		if( pAttacker->IsOwnerChar() )
		{
			g_AbuseMgr.AddDamage( 1 );

			g_ShakeCameraMgr.SetShakeCamera( pAttacker->GetKillShakeCamera(), ShakeCamera::SP_HIGH );

			if( pAttacker->GetState() == CS_USING_SKILL )
				StartScreenBlind( SBT_KILL, NULL, true );
			else
				StartScreenBlind( SBT_KILL, NULL, true );
		}

		if ( pAttacker->IsNeedProcess() )
			g_RSoldierInfoManager.PlaySoldierSound( pAttacker, SST_Kill );
	}

	if( !pChar->IsOwnerChar() )
	{
		// 죽은 유저의 데미지 리스트는 All에 넣고 부활 후 부터 다시 체크한다.
		m_pPlayMode->AttachOwnerDamageList( pChar->GetCharName() );
	}

	// 강시 모드에서 죽으면 강시됨
	if( GetModeType() == MT_GANGSI )
	{
		pChar->SetTeam( TEAM_BLUE );
	}

	// 배틀모드 스코어 갱신
	if( GetModeType() == MT_BATTLE )
	{
		ioBattleMode* pBattleMode = ToBattleMode( m_pPlayMode );
		if( pBattleMode )
			pBattleMode->ChangeScore( pChar->GetTeam() );
	}
}

void ioPlayStage::OnRevivalTime( SP2Packet &rkPacket )
{
	ioHashString szDier;
	rkPacket >> szDier;

	DWORD dwRevivalGapTime;
	rkPacket >> dwRevivalGapTime;

	if( szDier.IsEmpty() )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnRevivalTime() - Dier Name is NULL" );
		return;
	}

	if( m_pPlayMode )
	{
		m_pPlayMode->UpdateRevivalTime( szDier, dwRevivalGapTime );
	}
}

void ioPlayStage::OnParentChangeChar( SP2Packet &rkPacket, bool bUdp )
{
	ioBaseChar *pChar = _OnChangeChar( rkPacket );
	if( pChar )
	{
		OnCharEquipAccessoryData( rkPacket );
		OnCharEquipMedalItemData( rkPacket );
		OnCharGrowthLevelData( rkPacket );

		if( bUdp )
		{
			bool	bCharacterImmediatelyChange;
			rkPacket >> bCharacterImmediatelyChange;

			if( bCharacterImmediatelyChange )
			{
				pChar->ClearAttackFireTimeList();
				g_WeaponMgr.DestroyReserveOwenrWeaponExceptCharChange( pChar->GetCharName() );
			}
		}

		if( pChar->GetImmediatelyChange() )
		{
			pChar->SetCharacterChangeImmediatelyTime( 0.0f );
			pChar->SetImmediatelyChange( false );
		}

		OnCharNamedTitleData( rkPacket );
		OnExpandMedalSlotData( rkPacket );
		// 용병 교체 패킷보다 먼저 도착한 패킷 처리
		pChar->ChangeCharReservePacketProcess();

		if(GetModeType() == MT_PRACTICE)
		{
			ioPracticeMode* pMode = ToPracticeMode( m_pPlayMode );
			if(pMode)
			{
				pMode->SelectRegularSoldier();
			}
		}

		// 배틀모드 중 대기자 용병 변경 시 용병 아이콘도 변경
		ioBattleMode* pBattleMode = ToBattleMode( m_pPlayMode );
		if( pBattleMode )
		{
			pBattleMode->ChangeCharIcon(pChar);

			pBattleMode->AddBattleModeUserInfoToUI( pChar ); 

			if( pChar->GetState() != CS_BATTLE_WAITING )
				pChar->SetState( CS_DELAY );
		}
		///////////////////////////////

		if( bUdp )
		{
			SP2Packet kPacket( CTPK_CHANGE_CHAR_CHECK );
			kPacket << pChar->GetCharName();
			kPacket << pChar->GetIndex();

			CHARACTER rkCharData = pChar->GetCharacterInfo();
			kPacket << rkCharData.m_class_type;
			for( int i = ES_WEAPON; i < 4; ++i )
			{
				if( pChar->GetEquipedItem( i ) )
				{
					kPacket << pChar->GetEquipedItem( i )->GetItemCode();
					kPacket << pChar->GetEquipedItem( i )->GetItemReinforce();
				}
				else
				{
					kPacket << 0;
					kPacket << 0;
				}
			}
			TCPNetwork::SendToServer( kPacket );
		}
	}

#if defined( USE_GA )
	if( pChar )
	{
		if( pChar->GetCharName() == g_MyInfo.GetPublicID() )
		{
			if( g_HttpMng.GetHeroIndex() != g_MyInfo.GetClassType() )
			{
				if( GetModeType() == MT_NONE			|| 
					GetModeType() == MT_TRAINING		||
					GetModeType() == MT_HEADQUARTERS	||
					GetModeType() == MT_HOUSE			||
					GetModeType() == MT_PRACTICE )
					return;

				if( g_HttpMng.GetHeroStartTime() != 0 )
				{
					// end choice hero duration time;
					int iTime = (int)REALGETTIME() / 1000;
					g_HttpMng.SetHeroEndTime( iTime );
				}
				else
				{
					// start choice hero duration time;
					int iTime = (int)REALGETTIME() / 1000;
					g_HttpMng.SetHeroStartTime( iTime );				
					g_HttpMng.SetHeroIndex( g_MyInfo.GetClassType() );

					return;
				}			

				char chSlash[16]		= {0,};
				char chType[64]			= {0,};
				char chMode[64]			= {0,};
				char chPostData[256]	= {0,};

				if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				{
					sprintf_e( chSlash, "%%2F" );
					sprintf_e( chType, "%s%s", chSlash, g_HttpMng.GetBattleType() );
					sprintf_e( chMode, "%s%s", chSlash, g_HttpMng.GetBattleMode() );
					sprintf_e( chPostData, "%sPLAY%s%s%sHERO_CHANGE", chSlash, chType, chMode, chSlash );
				}
				else
				{
					SafeSprintf( chSlash, sizeof(chSlash), "%2F" );
					SafeSprintf( chType, sizeof(chType), "%1%2", chSlash, g_HttpMng.GetBattleType() );
					SafeSprintf( chMode, sizeof(chMode), "%1%2", chSlash, g_HttpMng.GetBattleMode() );
					SafeSprintf( chPostData, sizeof(chPostData), "%1PLAY%2%3%4HERO_CHANGE", chSlash, chType, chMode, chSlash );
				}

				// PLAY_<TYPE>_<MODE>_HERO_CHANGE
				g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), chPostData, 9 );
			}		
		}
	}	
#endif
}

ioBaseChar *ioPlayStage::_OnChangeChar( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1100, 500 );

	TCPNetwork::MouseBusy( false );

	int i = 0;
	int iSuccess;
	ioHashString szName;
	rkPacket >> szName >> iSuccess;

	int iRCharCount;
	rkPacket >> iRCharCount;

	int nSoldierSetCnt;
	rkPacket >> nSoldierSetCnt;

	if( iSuccess == CHANGE_CHAR_FAIL )
	{
		ioBaseChar *pFailChar = GetBaseChar( szName );
		if( pFailChar )
		{
			pFailChar->SetState( CS_DELAY );
			pFailChar->SetState( CS_ENDMOTION );
		}

		return NULL;
	}

	bool bCharChangeWait;
	rkPacket >> bCharChangeWait;

	DWORD dwCharChangeIndex;
	rkPacket >> dwCharChangeIndex;

	DWORD dwCharIndex;
	int iClassLevel, iHeroTitle;
	byte eAwakeType;
	rkPacket >> dwCharIndex >> eAwakeType >> iClassLevel >> iHeroTitle;
    
	ErrorReport::SetPosition( 1100, 501 );

	// 기존 정보 재사용
	bool bPreCharChangeSync = false;
	bool bPreCharExisted    = false;
	D3DXVECTOR3 vPreCharPosition;
	float fPrevSkillGauge[MAX_INVENTORY] = { 0.0f, 0.0f, 0.0f, 0.0f };
	int   iGradeLevel, iTeamType;
	ioHashString szCharBubbleChat;
	bool         bWholeChat = false;
	DWORD        dwCharBubbleTime = 0;
	ioBaseChar *pPreChar = GetBaseChar( szName );

	bool bBornChange = false;
	bool bDisableEquipObject = false;
	if( pPreChar )
	{
		bDisableEquipObject = pPreChar->IsDisableEquipObject();

		//서버상에서 이미 아이템 DB정보를 Clear 했기때문에 패킷을 보내지 않고 Release
		pPreChar->OnReleaseObjectItem( CS_DELAY );
		pPreChar->ReleaseObejctEquipState();
		pPreChar->SetDisableEquipObject( false );

		if( bBornChange && m_bReLoadRevisionSkillGauge )
		{
			ioINILoader_e kLoader( "config/sp2.ini" );

			kLoader.SetTitle_e( "skill_gauge_revision" );
			m_bUseRevisionSkillGauge = kLoader.LoadBool_e( "enable", false );
			m_bReLoadRevisionSkillGauge = kLoader.LoadBool_e( "reload", false );
			m_fRevisionDefaultSkillGauge = kLoader.LoadFloat_e( "gauge", 0.f );
		}

		if( m_bUseRevisionSkillGauge &&	pPreChar->GetState() == CS_READY )
			bBornChange = true;

		if( pPreChar->IsOwnerChar() )
		{
			if( pPreChar->GetSendChangeCharTime() != 0 )
			{
				DWORD dwDelayTime = REALGETTIME() - pPreChar->GetSendChangeCharTime();
				if( dwDelayTime >= 500 )         // 0.5초 이상 걸리면 렉으로 느낄듯하다.
				{
					// 로그 서버로 전송
					char szLOG[2048] = "";
					sprintf( szLOG, "[%s] Change Char %dms Delay : Ping[%d] - Mode[%d] - UDP Lose[%d]", 
									pPreChar->GetCharName().c_str(), dwDelayTime, g_GlobalTimer.GetServerDelayMS(),  
									(int)GetModeType(), g_GlobalTimer.GetPacketloseCnt() );

					SP2Packet kPacket( LUPK_LOG );
					kPacket << "CheckError";  // 로그 파일 타입
					kPacket << szLOG;
					kPacket << 3182;   // 오류번호:3182
					kPacket << false; // write db
					P2PNetwork::SendLog( kPacket );
				}
#ifndef SHIPPING 
				LOG.PrintTimeAndLog( 0, "Change Char Delay : %dms", dwDelayTime );
#endif
				pPreChar->SetSendChangeCharTime( 0 );
			}
		}

		if( pPreChar->GetCharChangeUniqueIndex() == dwCharChangeIndex &&
			dwCharChangeIndex != MAX_INT_VALUE )
		{
			bPreCharChangeSync = true;
		}

		vPreCharPosition = pPreChar->GetWorldPosition();
		bPreCharExisted  = true;

		if( bCharChangeWait )
		{
			for(i = 0;i < MAX_INVENTORY;i++)
			{
				ioItem *pItem = pPreChar->GetEquipedItem( i );
				if( pItem )
					fPrevSkillGauge[i] = pItem->GetCurSkillGuage();
			}
		}
		iGradeLevel = pPreChar->GetLevel();
		iTeamType   = (int)pPreChar->GetTeam();

		ioCharChatBubble *pCharChatBubble = pPreChar->GetCharChatBubble();
		if( pCharChatBubble && pCharChatBubble->IsBubbleChat() )
		{
			szCharBubbleChat = pCharChatBubble->GetChatBubble();
			bWholeChat       = pCharChatBubble->IsWholeChat();
			dwCharBubbleTime = pCharChatBubble->GetCurrentTime();
		}
		pPreChar = NULL; // 주의
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "용병 교체 패킷에서 기존 캐릭터 찾지 못함!! : %s", szName.c_str() );
		if( rkPacket.GetPacketID() == CUPK_CHANGE_CHAR )
			return NULL;      // UDP 패킷은 첫 용병보다 먼저 도착할 수 있으므로 용병을 생성하지 않는다.
	}

	ErrorReport::SetPosition( 1100, 502 );

	//자신의 캐릭터 변경
	int iPreCharArray = -1;
	if( g_MyInfo.GetPublicID() == szName )
	{
		if( !bPreCharChangeSync )
		{
			iPreCharArray = g_MyInfo.GetSelectCharArray();

			g_MyInfo.SetChangeChar( dwCharIndex );
			g_MyInfo.SetCharJoined( dwCharIndex, true );
		}
	}
	ErrorReport::SetPosition( 1100, 503 );

	float fHPRate = FLOAT1;
	ioBaseChar *pChar = NULL;
	if( bPreCharExisted && bCharChangeWait )
	{
		ErrorReport::SetPosition( 1100, 504 );

		pChar = GetBaseChar( szName );
		if( !pChar )
		{
			LOG.PrintTimeAndLog( 0, "ioPlayStage::OnChangeChar - %s Add Failed(1)", szName.c_str() );
			return NULL;
		}
		StatusValue eHP = pChar->GetHP();
		if( eHP.m_fMaxValue > 0.0f )
		{
			fHPRate = eHP.m_fCurValue / eHP.m_fMaxValue;
		}

		if( !bPreCharChangeSync )
		{
			pChar->SetDisableEquipObject( bDisableEquipObject );
			pChar->DestroyAllItem();       //아이템을 먼저 벗기고 모델을 바꾼다.
			pChar->ClearAniEvent();
			pChar->SetDisableEquipObject( false );
		}
		ErrorReport::SetPosition( 1100, 505 );		
		ChangeCharModel( pChar, szName, dwCharIndex, rkPacket, bPreCharChangeSync, eAwakeType );
		pChar->SetCurRSoldierCount( iRCharCount );
		pChar->SetCurSoldierSetCnt( nSoldierSetCnt );
		ErrorReport::SetPosition( 1100, 506 );		
	}
	else
	{
		ErrorReport::SetPosition( 1100, 508 );

		if( Help::CheckEnableTimeGapUser2( 1 ) )
			LOG.PrintTimeAndLog( 0, "%s:%s", __FUNCTION__, szName.c_str() );

		RemoveUser( szName, false );
		ErrorReport::SetPosition( 1100, 509 );

		pChar = ChangeCharModel( szName, dwCharIndex, (TeamType)iTeamType, rkPacket, eAwakeType );
		ErrorReport::SetPosition( 1100, 510 );
		if( !pChar )
		{
			ErrorReport::SetPosition( 9000, 1 );
			// 로그 서버 전송
			char szErrorPos[MAX_PATH] = "";
			sprintf_s( szErrorPos, sizeof( szErrorPos ), STR(1), g_MyInfo.GetMyIP().c_str(),
				g_MyInfo.GetPrivateID().c_str(),
				g_MyInfo.GetPublicID().c_str(),
				ErrorReport::GetMajorPosition(),
				ErrorReport::GetMinorPosition(),
				ErrorReport::GetEngineMajorPos(),
				ErrorReport::GetEngineMinorPos(),
				ErrorReport::GetString() );


			SP2Packet kPacket( LUPK_LOG );
			kPacket << "ClientError";  // 로그 파일 타입
			kPacket << szErrorPos;
			kPacket << 317;  // 오류번호:317
			kPacket << false; // write db
			P2PNetwork::SendLog(kPacket);

			LOG.PrintTimeAndLog( 0, "ExitProgram - 13" );

#if defined( USE_GA )
			if( g_App.GetGAStart() == true )
			{
				char chLabel[32] = {0,};
				if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
					sprintf_e( chLabel, "%d", 13 );
				else
					SafeSprintf( chLabel, sizeof(chLabel), "%1", 13 );

				// GAME_END_ERR_POS
				g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
					, "Game"
					, "Error"
					, chLabel
					, 1
					, "%2FGAME%2FOVER%2FERR"
					, 1 );

				g_HttpMng.GA_PageVIewTracking( g_MyInfo.GetUserIndex(), "%2FGAME%2FOVER%2FERR", 6, chLabel );
			}			
#endif
			g_App.SetExitProgram();
			return NULL;
		}

		pChar->SetCurRSoldierCount( iRCharCount );
		pChar->SetCurSoldierSetCnt( nSoldierSetCnt );

		//펫 장착 정보 요청 (캐릭터 새로 만들었을 때 - 새 캐릭터가 처음 방에 입장시 적용하기 위해)
		SP2Packet kPacket( CTPK_PET_EQUIP_INFO );
		kPacket << pChar->GetCharName();
		TCPNetwork::SendToServer( kPacket );

		if( bPreCharExisted )
		{
			pChar->SetWorldPosition( vPreCharPosition );
		}
	}
	ErrorReport::SetPosition( 1100, 511 );

	if( !bPreCharChangeSync )
	{
		pChar->SetIndex( dwCharIndex );
		pChar->SetLevel( iGradeLevel );
		pChar->SetHeroTitle( iHeroTitle );
		pChar->SetClassLevel( iClassLevel );

		// 풍선 채팅 내용이 있으면 넣어준다.
		if( !szCharBubbleChat.IsEmpty() )
		{
			pChar->SetChatBubble( szName.c_str(), szCharBubbleChat.c_str(), bWholeChat, dwCharBubbleTime );
			SetChatIcon( szName, szCharBubbleChat, false, false );
		}

		ErrorReport::SetPosition( 1100, 512 );

		if( m_pPlayMode )
		{
			m_pPlayMode->AddNewRecord( pChar );
			ErrorReport::SetPosition( 1100, 513 );
			m_pPlayMode->ResetTotalChamp();	
			ErrorReport::SetPosition( 1100, 514 );
		}

		ErrorReport::SetPosition( 1100, 515 );

		if( ioPlayMode::IsZeroHP() )
		{
			pChar->SetCurHP( 0.0f);
			pChar->SetCurHPRate( 0.0f );
		}
		else
		{
			if( bCharChangeWait )
			{
				pChar->SetCurHPRate( fHPRate );
			}
			else if( m_pPlayMode )
			{
				pChar->SetCurHPRate( m_pPlayMode->GetRevivalHPRateByMode() );
			}
		}
		ErrorReport::SetPosition( 1100, 516 );
	}

	// 아이템 장착
	pChar->SetDisableEquipObject( bDisableEquipObject );
	if( ToHeadquartersMode( m_pPlayMode ) )
	{
		ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayMode );
		if( pHeadquartersMode->IsCharChangeEquipItemReload() )
			ChangeCharEquipItem( pChar, fPrevSkillGauge, rkPacket, false, bBornChange );
		else
			ChangeCharEquipItem( pChar, fPrevSkillGauge, rkPacket, bPreCharChangeSync, bBornChange );
	}
	else
	{
		ChangeCharEquipItem( pChar, fPrevSkillGauge, rkPacket, bPreCharChangeSync, bBornChange );
	}
	pChar->SetDisableEquipObject( false );
	pChar->SetCharChangeUniqueIndex( dwCharChangeIndex );

	ErrorReport::SetPosition( 1100, 517 );

	if( bPreCharChangeSync )
		return pChar; // 

	pChar->SetCharChangeEffect();

	if( bCharChangeWait )
	{
		CharState eState = pChar->GetState();

		if( eState == CS_CHANGE_WAIT || eState == CS_DELAY || eState == CS_SPECIAL_MOTION || eState == CS_ETCITEM_MOTION )
		{
			pChar->SetState( CS_DELAY );
		}
		else if( eState == CS_RUN )
		{
			pChar->SetState( CS_RUN );
		}

		// 용병교체시 총알, 레이져 충전량 관련 처리
		if( pChar->IsOwnerChar() )
		{
			ioItem *pItem = pChar->GetEquipedItem( ES_WEAPON );

			ITEM_DATA &rkItem = g_MyInfo.GetCharWeaponItem();
			int iCurCharArray = g_MyInfo.GetSelectCharArray();

			int iCurBullet = rkItem.m_item_bullet;
			if( iCurBullet >= 0 )
			{
				ioWeaponItem *pWeapon = ToWeaponItem( pItem );
				if( pWeapon )
				{
					switch( pWeapon->GetSubType() )
					{
					case ioWeaponItem::WST_FIRE:
					case ioWeaponItem::WST_AUTO_SHOT:
					case ioWeaponItem::WST_WEREWOLF:
					case ioWeaponItem::WST_EXCALIBUR:
					case ioWeaponItem::WST_CHARGE_COMMAND:
					case ioWeaponItem::WST_CHARGE_COMBO_BOW:
					case ioWeaponItem::WST_WIND_LORD:
					case ioWeaponItem::WST_EVY_ITEM:
					case ioWeaponItem::WST_SUDDEN_ATTACK_ITEM:
					case ioWeaponItem::WST_DUMMY_CHAR2:
					case ioWeaponItem::WST_TRACE_ITEM:
					case ioWeaponItem::WST_LEE_ITEM:
					case ioWeaponItem::WST_JORO_EXPANSION_ITEM:
					case ioWeaponItem::WST_EARTHQUAKE:
					case ioWeaponItem::WST_RESEARCHER:
					//case ioWeaponItem::WST_BULLFIGHT_ITEM:
					case ioWeaponItem::WST_RED_HOOD_ITEM:
					//case ioWeaponItem::WST_STRIDER_ITEM:
					case ioWeaponItem::WST_CUCHULAIN_ITEM:
					case ioWeaponItem::WST_WESTGUN_EXTEND_ITEM:
					case ioWeaponItem::WST_SASINMU:
					//case ioWeaponItem::WST_GUNNER:
					case ioWeaponItem::WST_BUBBLE_FIGHTER:
					//case ioWeaponItem::WST_DRILL_ITEM:
					case ioWeaponItem::WST_STRANGER_ITEM:
					case ioWeaponItem::WST_MEGA_AGENT_ITEM:
					case ioWeaponItem::WST_MICHAEL:
					case ioWeaponItem::WST_GOOMIHO:
					case ioWeaponItem::WST_MECHANICS:
					case ioWeaponItem::WST_SOCCER_ITEM:
					case ioWeaponItem::WST_VULCAN_ITEM:
					case ioWeaponItem::WST_MURIMGOSU_ITEM:
					case ioWeaponItem::WST_TIMEGATE_ITEM:
					case ioWeaponItem::WST_ROCKET_LAUNCHER_ITEM:
					case ioWeaponItem::WST_PAPARAZZI_ITEM:
					case ioWeaponItem::WST_FROZEN_ITEM:	// 2020-03-05 신규용병 얼음신 추가 
					case ioWeaponItem::WST_SKY_WALKER_ITEM:	// 2020-05-27 신규용병 스카이워커 추가 
					case ioWeaponItem::WST_EVA_ITEM:		// 2020-10-27 신규용병 이브 추가 	
					case ioWeaponItem::WST_MAGNETIC_ITEM:	// 2020-11-30신규용병 레어(마그네틱) 추가
						{
							pWeapon->SetCurBullet( iCurBullet );
						}
						break;
					
					case ioWeaponItem::WST_CHARGE_CONTACT:
					case ioWeaponItem::WST_YUMIRA_ITEM:
					case ioWeaponItem::WST_PETERPAN:
					case ioWeaponItem::WST_SHAOLIN:
					case ioWeaponItem::WST_JINMORI_ITEM:
					case ioWeaponItem::WST_DRACULA:
					case ioWeaponItem::WST_PROJECT_K:
					case ioWeaponItem::WST_HAKUMEN:
					case ioWeaponItem::WST_FLASHITEM:
					case ioWeaponItem::WST_PSYCO_ITEM:
					case ioWeaponItem::WST_SLASHER_ITEM:
					case ioWeaponItem::WST_SUPER_DIMENSION_ITEM:
					case ioWeaponItem::WST_KAELTHAS_ITEM:
					case ioWeaponItem::WST_GALAXY_ITEM:
					case ioWeaponItem::WST_DRAGON_SLAYER_ITEM:
					case ioWeaponItem::WST_THUNDER_BIRD_ITEM:
					case ioWeaponItem::WST_ALICE_ITEM:
					case ioWeaponItem::WST_PARKILPYO_ITEM:
					case ioWeaponItem::WST_JACK_ITEM:
					case ioWeaponItem::WST_SHADOWRANGER_ITEM:
					case ioWeaponItem::WST_GEOMJE_ITEM:
					case ioWeaponItem::WST_NATA_ITEM:
					case ioWeaponItem::WST_BLADE_ITEM:
					case ioWeaponItem::WST_CHAIN_MAGICIAN_ITEM:
					case ioWeaponItem::WST_GANESA:
					case ioWeaponItem::WST_ONIMUSHA_ITEM:
					case ioWeaponItem::WST_PREDATOR_ITEM:
					case ioWeaponItem::WST_STRIDER_ITEM:
					case ioWeaponItem::WST_GUNNER:
						{
							if( iCurCharArray == iPreCharArray )
								pWeapon->SetCurBullet( iCurBullet );
							else
								pWeapon->InitExtraGauge();
						}
						break;

					case ioWeaponItem::WST_BOW:
						{
							ioBowItem *pBow = ToBowItem( pItem );
							if( pBow )
							{
								if( pBow->IsChargeBulletType() )
								{
									if( iCurCharArray == iPreCharArray )
										pBow->SetCurBullet( iCurBullet );
									else
										pBow->MaxCoolTime();
								}
								else
									pBow->SetCurBullet( iCurBullet );
							}
						}
						break;

					case ioWeaponItem::WST_LASER:
						{
							ioLaserItem *pLaser = ToLaserItem( pItem );
							if( pLaser )
							{
								if( iCurCharArray == iPreCharArray )
									pLaser->SetCurBullet( iCurBullet );
								else
									pLaser->MaxCoolTime();
							}
						}
						break;
					
					case ioWeaponItem::WST_UROBORUS:
						{
							ioUroborusItem *pUroborus = ToUroborusItem( pItem );
							if( pUroborus )
							{
								if( iCurCharArray == iPreCharArray )
									pUroborus->SetCurBullet( iCurBullet );
								else
									pUroborus->InitCoolTime();
							}
						}
						break;
					
					case ioWeaponItem::WST_WITCH:
						{
							ioWitchItem *pWitchItem = ToWitchItem( pItem );
							if( pWitchItem )
							{
								if( iCurCharArray == iPreCharArray )
									pWitchItem->SetCurBullet( iCurBullet );
								else
									pWitchItem->InitWitchGauge();
							}
						}
						break;

					case ioWeaponItem::WST_NAKORURU:
						{
							ioNakoruruItem *pNakoruru = ToNakoruruItem( pItem );
							if( pNakoruru )
							{
								if( iCurCharArray == iPreCharArray )
									pNakoruru->SetCurBullet( iCurBullet );
								else
									pNakoruru->InitWitchGauge();
							}
						}
						break;
					
					case ioWeaponItem::WST_DESTROYER:
						{
							ioDestroyerItem *pDestroyer = ToDestroyerItem( pItem );
							if( pDestroyer )
							{
								if( iCurCharArray == iPreCharArray )
									pDestroyer->SetCurBullet( iCurBullet );
								else
									pDestroyer->InitDestroyerGauge();
							}
						}
						break;
					case ioWeaponItem::WST_PHANTOM:
						{
							ioPhantomItem *pPhantom = ToPhantomItem( pItem );
							if( pPhantom )
							{
								if( iCurCharArray == iPreCharArray )
									pPhantom->SetCurBullet( iCurBullet );
								else
									pPhantom->InitPhantomGauge();
							}
						}
						break;

					case ioWeaponItem::WST_CYVOGUE:
						{
							ioCyvogueItem *pCyvogueItem = ToCyvogueItem(pItem);
							if( pCyvogueItem )
							{
								if( iCurCharArray == iPreCharArray )
									pCyvogueItem->SetCurBullet( iCurBullet );
								else
									pCyvogueItem->InitCyvogueGauge();
							}
						}
						break;

					case ioWeaponItem::WST_DARKSEAS_ITEM:
						{
							ioDarkSeasItem *pDarkSeasItem = ToDarkSeasItem(pItem);
							if( pDarkSeasItem )
							{
								if( iCurCharArray == iPreCharArray )
									pDarkSeasItem->SetCurBullet( iCurBullet );
								else
									pDarkSeasItem->InitGauge();
							}
						}
						break;
					}
				}
			}
			else
			{
				ioWeaponItem *pWeapon = ToWeaponItem( pItem );
				if( pWeapon )
				{
					switch( pWeapon->GetSubType() )
					{
					case ioWeaponItem::WST_LASER:
						{
							ioLaserItem *pLaser = ToLaserItem(pWeapon);
							if( pLaser )
								pLaser->MaxCoolTime();
						}
						break;
					case ioWeaponItem::WST_BOW:
						{
							ioBowItem *pBow = ToBowItem(pWeapon);
							if( pBow && pBow->IsChargeBulletType() )
								pBow->MaxCoolTime();
						}
						break;
					case ioWeaponItem::WST_UROBORUS:
						{
							ioUroborusItem *pUroborus = ToUroborusItem( pWeapon );
							if( pUroborus )
								pUroborus->InitCoolTime();
						}
						break;
					case ioWeaponItem::WST_WEREWOLF:
						{
							ioWereWolfItem *pWereWolf = ToWereWolfItem( pWeapon );
							if( pWereWolf )
								pWereWolf->InitChangeDamage();
						}
						break;
					case ioWeaponItem::WST_WITCH:
						{
							ioWitchItem *pWitchItem = ToWitchItem( pWeapon );
							if( pWitchItem )
								pWitchItem->InitWitchGauge();
						}
						break;
					case ioWeaponItem::WST_NAKORURU:
						{
							ioNakoruruItem *pNakoruru = ToNakoruruItem( pWeapon );
							if( pNakoruru )
								pNakoruru->InitWitchGauge();
						}
						break;
					case ioWeaponItem::WST_PHANTOM:
						{
							ioPhantomItem *pPhantom = ToPhantomItem( pWeapon );
							if( pPhantom )
								pPhantom->InitPhantomGauge();
						}
						break;
					case ioWeaponItem::WST_YUMIRA_ITEM:
					case ioWeaponItem::WST_CHARGE_CONTACT:
					case ioWeaponItem::WST_PETERPAN:
						{
							pWeapon->InitExtraGauge();
						}
						break;
					case ioWeaponItem::WST_DR_SPIDER_ITEM:
						{
							ioDrSpiderItem *pDrSpider = ToDrSpiderItem( pWeapon );
							if( pDrSpider )
								pDrSpider->InitChangeDamage();
						}
						break;
					}
				}
			}

			for( int i = ES_ARMOR; i < MAX_INVENTORY; i++ )
			{
				const ITEM_DATA &rkItem = g_MyInfo.GetCharItem( i );
				int iCurBullet = rkItem.m_item_bullet;

				ioSkill* pSkill = NULL;
				if( pChar->GetEquipSlot() )
					pSkill = pChar->GetEquipSlot()->GetEquipedSkill( i );

				ioPassiveSkill* pPassiveSkill = ToPassiveSkill( pSkill );
				if( pPassiveSkill && pPassiveSkill->GetMaintainPassiveGauge() )
					pPassiveSkill->SetCurPassiveGauge( (float)iCurBullet );
			}
		}
	}
	else
	{
		// 용병교체시 총알, 레이져 충전량 관련 처리
		if( pChar->IsOwnerChar() )
		{
			g_MyInfo.InitCharWeaponItem();

			ioItem *pItem = pChar->GetEquipedItem( ES_WEAPON );
			ioLaserItem *pLaser = ToLaserItem( pItem );
			ioBowItem *pBow = ToBowItem( pItem );
			ioUroborusItem *pUroborus = ToUroborusItem( pItem );

			if( pLaser )
				pLaser->MaxCoolTime();
			else if( pBow && pBow->IsChargeBulletType() )
				pBow->MaxCoolTime();
			else if( pUroborus )
				pUroborus->InitCoolTime();
		}

		pChar->SetStartMotionState();

		if( GetPlazaModeType() == PT_COMMUNITY )
			pChar->SetChatMode( CMS_NONE );

		if ( pChar->IsNeedProcess() )
			g_RSoldierInfoManager.PlaySoldierSound( pChar, SST_Start );
	}

	pChar->CheckDefaultReinforce();

	if( m_pPlayMode )
	{
		m_pPlayMode->SetMapPassiveBuff( pChar );
	}

	ErrorReport::SetPosition( 1100, 518 );

	if( pChar && pChar->IsOwnerChar() )
	{
		CHARACTER backup = (CHARACTER)pChar->GetCharacterInfo();
		// 유저 동기화
		SP2Packet kPacket( CUPK_CHANGE_CHARINFO );
		kPacket << pChar->GetIndex() << (CHARACTER)pChar->GetCharacterInfo();
		g_MyInfo.FillCharItemData( pChar->GetIndex(), kPacket );
		P2PNetwork::SendToAllUser( kPacket );
	}

	ErrorReport::SetPosition( 1100, 519 );
	if( m_pPlayMode )
	{
		m_pPlayMode->OnChangeCharNextProcess( pChar, ( !bPreCharExisted || !bCharChangeWait ) );
	}

	ErrorReport::SetPosition( 1100, 520 );

	return pChar;
}

void ioPlayStage::OnJoinRoomInfoEnd( SP2Packet &rkPacket )
{
	if( GetModeType() == MT_TRAINING )
	{
		if( g_App.CheckFirstPlaza() )
		{
			SetModeEndSceneCamera( false );
			StartModeStartEventCamera( true );

			if( m_pPlayMode )
				m_pPlayMode->SetModeMapSound(true);
		}
		else
		{
			if( P2PNetwork::IsNetworkPlaying() && IsNetworkMode() )
			{
				SP2Packet kPacket( CTPK_EVENT_SCENE_END );
				kPacket << true;
				kPacket << GetRoomIndex();
				TCPNetwork::SendToServer( kPacket );
			}

			if( m_pPlayMode )
			{
				m_pPlayMode->ReserveMyStartPositionEvent( false );
				m_pPlayMode->SetModeMapSound( true );
			}
		}
	}
	else
	{
		SetModeEndSceneCamera( false );
		StartModeStartEventCamera( true );	// 초기접속시 이벤트 스타트

		if( m_pPlayMode )
			m_pPlayMode->SetModeMapSound();
	}
}

void ioPlayStage::OnStartSetNextMode( SP2Packet &rkPacket )
{
	int iModeType = 0;
	int iSubNum = 0;
	int iMapNum = 0;
	int iPlazaType = PT_NONE;

	rkPacket >> iModeType;
	rkPacket >> iSubNum;
	rkPacket >> iMapNum;
	rkPacket >> iPlazaType;

	char szCall[MAX_PATH];
	sprintf_s( szCall, "%s - (%d), (%d), (%d), (%d)" ,__FUNCTION__, iModeType, iSubNum, iMapNum, iPlazaType );

	g_App.SetNextStage( ioGameStage::GS_PLAY, szCall );
	g_App.SetModeType( iModeType, iSubNum, iMapNum, (PlazaType)iPlazaType );
}

void ioPlayStage::OnEndSetNextMode( SP2Packet &rkPacket )
{
	if( !m_pPlayMode ) return;

	D3DXVECTOR3 vStartPos;
	BaseCharList::iterator iter;
	for( iter=m_BaseCharList.begin() ; iter!=m_BaseCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;
		if( pChar == NULL ) continue;
		if( ToNpcChar( pChar ) ) continue;

		vStartPos = m_pPlayMode->GetStartPos( pChar->GetTeam() );
		// 배틀모드 일때
		if(m_pPlayMode->GetModeType() == MT_BATTLE)
		{
			ioHashString szName = pChar->GetCharName();
			// 첫번째 출전이 아닐 경우
			if(g_BattleRoomMgr.GetUserBattleOrder(szName) != 1 && !szName.IsEmpty())
			{
				ioBattleMode* pBattleMode = ToBattleMode(m_pPlayMode);
				if(pBattleMode)
					vStartPos = pBattleMode->GetWaitingPos(pChar, pChar->GetTeam());
			}
		}
		pChar->SetWorldPosition( vStartPos );

		if( !g_BattleRoomMgr.IsObserverUser( pChar->GetCharName() ) )
			pChar->SetReadyState();

		m_pPlayMode->AddNewRecord( pChar );
	}

	// 모두 함께 시작하자
	EnableWorldEntityFunctions( true );

	// 세트완료후 모드 교체시 이벤트 스타트
	SetModeEndSceneCamera( false );
	StartModeStartEventCamera( true );

	if( m_pPlayMode )
		m_pPlayMode->SetModeMapSound();
}

void ioPlayStage::OnAwardingInfo( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	if( g_MyInfo.GetPublicID() == szName )
	{
		g_AwardMgr.ApplyAwardInfo( rkPacket );
	}
}

void ioPlayStage::OnAwardingResult( SP2Packet &rkPacket )
{
	TotalResultMainWnd *pResultMainWnd = dynamic_cast<TotalResultMainWnd *>(g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ));
	if( pResultMainWnd )
	{
		pResultMainWnd->SetAwarding( rkPacket );
	}
}

void ioPlayStage::OnFollowUserResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult, iUserPos;
	ioHashString szName;
	rkPacket >> iResult >> szName >> iUserPos;

	switch( iResult )
	{
	case FOLLOW_USER_OK:
		return;
	case FOLLOW_USER_PARTY_PW:
		{
			// 파티 UI Show.
			ioHashString szName;
			int iPartyIndex, iPartyLevel, iCurJoiner, iCurPlayer, iMaxPlayerBlue, iMaxPlayerRed;
			rkPacket >> iPartyIndex >> iPartyLevel >> iCurJoiner >> iCurPlayer >> iMaxPlayerBlue >> iMaxPlayerRed >> szName;

			PartyPasswordWnd *pPasswordWnd = dynamic_cast<PartyPasswordWnd*>(g_GUIMgr.FindWnd(PARTY_PASSWORD_WND));
			if( pPasswordWnd )
				pPasswordWnd->SetBattleRoomInfo( iPartyIndex, iPartyLevel, szName, iCurJoiner, iCurPlayer, iMaxPlayerBlue, iMaxPlayerRed, false );
		}
		return;	
	case FOLLOW_USER_PLAZA_PW:
		{
			// 광장 비번UI Show
			int iRoomIndex, iRoomLevel, iCurJoiner, iCurPlayer, iMaxPlayer, iPlazaType;
			ioHashString szName;
			rkPacket >> iRoomIndex >> iRoomLevel >> iCurJoiner >> iCurPlayer >> iMaxPlayer  >> szName >> iPlazaType;

			PlazaPasswordWnd *pPasswordWnd = dynamic_cast<PlazaPasswordWnd*>(g_GUIMgr.FindWnd( PLAZA_PASSWORD_WND ));
			if( pPasswordWnd )
				pPasswordWnd->SetPlazaInfo( iRoomIndex, iRoomLevel, szName, iCurJoiner, iCurPlayer, iMaxPlayer, (PlazaType)iPlazaType );
		}
		return;
	}

	// 예외 처리
	char szException[MAX_PATH] = "";

	ioINILoader_e kLoader( "config/sp2_exception_help.ini" );
	kLoader.SetTitle_e( "MOVING_USER" );
	
	char szKey[MAX_PATH] = "";
	sprintf_e( szKey, "exception_%d", iResult - FOLLOW_USER_ERROR_1 );	
	kLoader.LoadString( szKey, "", szException, MAX_PATH );
	if( strcmp( szException, "" ) == 0 )
		sprintf( szException, "(Code:%d)", iResult - FOLLOW_USER_ERROR_1  );

	switch( iResult )
	{
	case FOLLOW_USER_ERROR_3:         //유저 없음
		{
			g_FriendManager.OnLogOutUser( szName );
		}
		break;
	case FOLLOW_USER_ERROR_4:         //유저 이동중
		{
			int iNextUserPos;
			rkPacket >> iNextUserPos;

			g_GuildInfo.SetMyGuildUserPos( szName, iNextUserPos, -1 );
			g_FriendManager.OnUserPos( szName, iNextUserPos, -1 );
		}
		break;
	}

	switch( iUserPos )
	{
	case UP_BATTLE_ROOM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), szName.c_str(), szException );
		break;
	case UP_TRAINING:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3), szName.c_str(), szException );
		break;
	case UP_LADDER_TEAM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4), szName.c_str(), szException );
		break;
	}
}

void ioPlayStage::OnUserPosIndex( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	ioHashString szUserName;
	int iUserPos, iIndex;
	rkPacket >> szUserName >> iUserPos >> iIndex;
	switch( iUserPos )
	{
	case UP_TRAINING:
		if( g_MyInfo.IsKickOutPlaza( iIndex ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			return;
		}
		break;
	case UP_BATTLE_ROOM:
		if( g_BattleRoomMgr.IsKickOutRoom( iIndex ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			return;
		}
		break;
	case UP_LADDER_TEAM:
		/*강퇴체크필요*/
		break;
	}

	//따라가기 실행
	TCPNetwork::MouseBusy( true );
	SP2Packet kPacket( CTPK_FOLLOW_USER );
	kPacket << szUserName << iUserPos;
	TCPNetwork::SendToServer( kPacket );
}

void ioPlayStage::OnRequestBankruptResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	int iResult;
	rkPacket >> iResult ;

	if( iResult == REQUEST_BANKRUPT_OK )
	{
		g_MyInfo.SetMoney( 0 );
	}
	else if( iResult == REQUEST_BANKRUPT_EXCEPTION )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
	else if( iResult == REQUEST_BANKRUPT_OUT_OF_MONEY )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
	}
}

void ioPlayStage::OnEtcItemBuyResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult = 0;
	int iType = 0;
	int iValue1      = 0;
	int iValue2      = 0;
	__int64 iMoney   = 0;
	int iCash        = 0;
	int iChannelingCash = 0;
	int iBonusPeso      = 0;
	bool bShowAlarmPopup= true;

	rkPacket >> iResult;

	if( iResult == ETCITEM_BUY_OK )
	{
		DWORD dwCompensation;
		byte eEtcBuyResultType;
		rkPacket >> iType >> iValue1 >> iValue2 >> iMoney >> iCash >> iChannelingCash >> iBonusPeso >> bShowAlarmPopup;
		rkPacket >> dwCompensation >> eEtcBuyResultType;
		
#if defined( USE_GA )
		bool bGACash	= false;
		bool bGAMoney	= false;
		int iPesoPrice	= g_MyInfo.GetMoney() - iMoney;
		int iCashPrice	= g_MyInfo.GetCash() - iCash;
		if( iCashPrice > 0 && iCashPrice < 1000000 )
			bGACash		= true;
		if( iPesoPrice > 0 )
			bGAMoney	= true;
		int iSubCode	= 0;
#endif

		ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
		if( pUserEtcItem )
		{
			ioUserEtcItem::ETCITEMSLOT kSlot;

#if defined( USE_GA )
			ioUserEtcItem::ETCITEMSLOT kNowSlot;
			pUserEtcItem->GetEtcItem( iType, kNowSlot );
			iSubCode = iValue1 - kNowSlot.m_iValue1;
#endif

			kSlot.m_iType   = iType;
			kSlot.m_iValue1 = iValue1;
			kSlot.m_iValue2 = iValue2;
			pUserEtcItem->AddEtcItem( kSlot );

			FishingInventoryWnd *pWnd = dynamic_cast<FishingInventoryWnd*>(g_GUIMgr.FindWnd(FISHING_INVENTORY_WND));
			if ( pWnd && pWnd->IsShow() )
			{
				pWnd->UpdatePage( false, true );
				pWnd->UpdateSelecItemInfo();
			}
		}

		if( !g_SuperGashaponMgr.IsSuperGashpon( iType ) )
		{
			if( iMoney != g_MyInfo.GetMoney() || iCash != g_MyInfo.GetCash() )
				g_ExSoundMgr.PlaySound( ExSound::EST_SHOP_BUY_SUCCESS );
		}

		if( iType == ioEtcItem::EIT_ETC_CHAR_SLOT_EXTEND )
		{
			g_MyInfo.CheckCurMaxCharSlot();

			// 용병 슬롯수 변경되서 갱신 필요
			SoldierSelectWnd *pSoldier = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
			if( pSoldier && pSoldier->IsShow() )
			{
				pSoldier->ReSetScroll();
			}

			UISoldierSelectWnd *pUISoldierWnd = dynamic_cast<UISoldierSelectWnd*>( g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ) );
			if( pUISoldierWnd && pUISoldierWnd->IsShow() )
			{
				pUISoldierWnd->ReSetScroll();
			}
		}
		else if( iType == ioEtcItem::EIT_ETC_FISHING_SLOT_EXTEND )
		{
			// 낚시슬롯수 변경되서 갱신 필요, Wnd 쪽에서 갱신할 것이다.
			FishingInventoryWnd *pFishingWnd = dynamic_cast<FishingInventoryWnd*>(g_GUIMgr.FindWnd( FISHING_INVENTORY_WND ));
			if( pFishingWnd && pFishingWnd->IsShow() )
			{
				pFishingWnd->CheckPageData( false );
				pFishingWnd->UpdatePage();
			}
		}
		else if( g_SuperGashaponMgr.IsSuperGashpon( iType ) )
		{			
			g_SuperGashaponMgr.SendAutoUse( iType, eEtcBuyResultType );
		}
		else if( g_MultiSuperGashaponMgr.IsMultiSuperGashpon( iType ) )
		{			
			g_MultiSuperGashaponMgr.SendAutoUse( iType, eEtcBuyResultType );
		}

		g_MyInfo.SetMoney( iMoney );
		g_MyInfo.SetCash( iCash );
		g_MyInfo.SetChannelingCash( iChannelingCash );

		if( dwCompensation > 0 )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), dwCompensation );
		}

		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
		if( pInvenWnd && pInvenWnd->IsShow() )
		{
			pInvenWnd->UpdateTab( true, true );
		}

		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iType );
		if( pEtcItem )
		{
			pEtcItem->OnBuy();
		}
		else
			LOG.PrintTimeAndLog( 0, "BuyResult pEtcItem == NULL." );

		// 몬스터 코인과 실버 코인은 클라이언트에서 구매하는게 아니다.
		if( iType != ioEtcItem::EIT_ETC_SILVER_COIN && iType != ioEtcItem::EIT_ETC_CLOVER  && iType != ioEtcItem::EIT_ETC_MILEAGE_COIN )
		{
			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShopWnd && pNewShopWnd->IsShow() )
			{
				pNewShopWnd->SetBoughtMark( NewShopWnd::ID_ETC_TAB_BTN, iType, 0 );
				NewShopBuyWnd *pNewShopBuyWnd = dynamic_cast<NewShopBuyWnd*> ( pNewShopWnd->FindChildWnd( NewShopWnd::ID_BUY_WND ) );
				if( pNewShopBuyWnd && pNewShopBuyWnd->IsShow() )
					pNewShopBuyWnd->ShowOnlyBoughtBtns();
			}
		}
		g_QuestMgr.QuestCompleteTerm( QC_BUY_ETC_ITEM, iType );

#if defined( USE_GA )
		if( bGACash )
		{
			if( iSubCode > 0 )
			{
				iCashPrice = g_App.GetGAEtcCash( iType, iSubCode );
				if( iCashPrice > 0 )
				{
					// GOLD_USE_ETC
					g_HttpMng.GA_ItemHitTracking( g_MyInfo.GetUserIndex() 
						, "Etc"
						, iCashPrice
						, 1
						, iType
						, "Gold"
						, "%2FGOLD%2FUSE%2FETC"
						, true );
				}
			}						
		}
		if( bGAMoney )
		{
			// PESO_USE_ETC
			g_HttpMng.GA_ItemHitTracking( g_MyInfo.GetUserIndex() 
				, "Etc"
				, iPesoPrice
				, 1
				, iType
				, "Peso"
				, "%2FPESO%2FUSE%2FETC" );
		}	
#endif
	}
	else if( iResult == ETCITEM_BUY_EXCEPTION )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
	}
	else if( iResult == ETCITEM_BUY_OUT_OF_MONEY )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
	}
	else if( iResult == ETCITEM_BUY_BILLING_WAIT)
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
	}
	else if( iResult == ETCITEM_BUY_BILLING_DISCONNECT )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
	}
	else if( iResult == ETCITEM_BUY_BILLING_FAIL )
	{
		bool bBillingError    = false;
		ioHashString  sBillingError;
		rkPacket >> bBillingError;
		if( bBillingError )
			rkPacket >> sBillingError;

		ioHashString sErrorMent = STR(6);
		if( !bBillingError )
			g_GUIMgr.SetMsgBox( MB_OK, NULL, sErrorMent.c_str() );
		else
		{
			ioHashString sError = "Billing Error : ";
			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal ) 
				sError = pLocal->GetBillingErrorMent(sBillingError);

			char szErrorFullMent[MAX_PATH]="";
			ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
			if( pNode && pNode->GetType() == CNT_NEXON )
				StringCbPrintf( szErrorFullMent, sizeof( szErrorFullMent ), "%s",  sError.c_str() ); //Except Extracting Hangeul
			else
				StringCbPrintf( szErrorFullMent, sizeof( szErrorFullMent ), "%s#%s%s", sErrorMent.c_str(), sError.c_str(), sBillingError.c_str() ); //Except Extracting Hangeul


			g_GUIMgr.SetMsgBox( MB_OK, NULL, szErrorFullMent );
		}
	}
	else if( iResult == ETCITEM_BUY_BILLING_WANT_OF_CASH )
	{
		g_GUIMgr.ShowWnd( BILLING_WANT_WND );
	}
	else if( iResult == ETCITEM_BUY_BILLING_EXCESS_BUY )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
	}
	else if( iResult == ETCITEM_BUY_SPECIAL_SHOP_CLOSE )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "기간한정 상품 판매기간이 종료 되었습니다." );
	}
	else if( iResult == ETCITEM_BUY_SPECIAL_SHOP_SOLD_OUT )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "해당 상품은 매진 되었습니다." );
	}
	else if( iResult == ETCITEM_BUY_SPECIAL_SHOP_BUY_LIMIT )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "1인 한정구매 제한 갯수를 초과 하였습니다." );
	}
	else if( iResult == ETCITEM_BUY_NOT_GUILD_USER )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "길드 아이템은 길드 가입 유저만# 구입 할 수 있습니다." );
	}
	else if( iResult == ETCITEM_BUY_GUILD_ROOM_DISABLE )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "길드 아이템은 길드 본부가 활성화 되어야# 구입 할 수 있습니다." );
	}
	else if( iResult == ETCITEM_BUY_BLOCK )
	{
		rkPacket >> iType >> iMoney >> iCash >> iChannelingCash >> iBonusPeso;

		if( iMoney != g_MyInfo.GetMoney() || iCash != g_MyInfo.GetCash() )
			g_ExSoundMgr.PlaySound( ExSound::EST_SHOP_BUY_SUCCESS );

		g_MyInfo.SetMoney( iMoney );
		g_MyInfo.SetCash( iCash );
		g_MyInfo.SetChannelingCash( iChannelingCash );
		g_QuestMgr.QuestCompleteTerm( QC_BUY_ETC_ITEM, iType );

		ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
		if( pUserEtcItem )
			pUserEtcItem->DeleteEtcItem( iType );

		NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
		if( pNewShopWnd && pNewShopWnd->IsShow() )
		{
			pNewShopWnd->SetBoughtMark( NewShopWnd::ID_ETC_TAB_BTN, iType, 0 );
			NewShopBuyWnd *pNewShopBuyWnd = dynamic_cast<NewShopBuyWnd*> ( pNewShopWnd->FindChildWnd( NewShopWnd::ID_BUY_WND ) );
			if( pNewShopBuyWnd && pNewShopBuyWnd->IsShow() )
				pNewShopBuyWnd->ShowOnlyBoughtBtns();
		}
	}
	else if( iResult == ETCITEM_BUY_PERSONAL_HQ_DISABLE )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "개인본부 아이템은 개인본부가 활성화 되어야# 구입 할 수 있습니다." );
	}
	else if( iResult == ETCITEM_BUY_BILLING_SHORTAGE_REAL_CASH )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "해당 아이템은 보너스 골드로# 구매가 불가능한 상품입니다.." );
	}
}

void ioPlayStage::OnEtcItemAdd( SP2Packet &rkPacket )
{
	int eType, iValue1, iValue2;
	rkPacket >> eType >> iValue1 >> iValue2;

	ioUserEtcItem *pUserEtcItem =  g_MyInfo.GetUserEtcItem();
	if( pUserEtcItem )
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		kSlot.m_iType   = eType;
		kSlot.m_iValue1 = iValue1;
		kSlot.m_iValue2 = iValue2;
		pUserEtcItem->AddEtcItem( kSlot );
	}

	if( eType == ioEtcItem::EIT_ETC_CHAR_SLOT_EXTEND )
	{
		g_MyInfo.CheckCurMaxCharSlot();

		// 용병 슬롯수 변경되서 갱신 필요
		SoldierSelectWnd *pSoldier = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
		if( pSoldier && pSoldier->IsShow() )
		{
			pSoldier->ReSetScroll();
		}

		UISoldierSelectWnd *pUISoldierWnd = dynamic_cast<UISoldierSelectWnd*>( g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ) );
		if( pUISoldierWnd && pUISoldierWnd->IsShow() )
		{
			pUISoldierWnd->ReSetScroll();
		}
	}
	else if( eType == ioEtcItem::EIT_ETC_FISHING_SLOT_EXTEND )
	{
		// 낚시슬롯수 변경되서 갱신 필요, Wnd 쪽에서 갱신할 것이다.
		FishingInventoryWnd *pFishingWnd = dynamic_cast<FishingInventoryWnd*>(g_GUIMgr.FindWnd( FISHING_INVENTORY_WND ));
		if( pFishingWnd && pFishingWnd->IsShow() )
		{
			pFishingWnd->CheckPageData( false );
			pFishingWnd->UpdatePage();
		}
	}

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( true, true );
	}
}

void ioPlayStage::OnRefillCoin( SP2Packet &rkPacket )
{
	int iRefillData, iRefillCount;
	rkPacket >> iRefillData >> iRefillCount;
	g_MyInfo.SetRefillCoinData( iRefillData, iRefillCount );
}

void ioPlayStage::OnExtraItemBuyResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult = 0;

	int iItemCode = 0;
	int iReinforce = 0;
	int iSlotIndex = 0;
	int iTradeType = 0;
	int iPeriodType = 0;
	int iValue1 = 0;
	int iValue2 = 0;
	DWORD dwMaleCustom = 0;
	DWORD dwFemaleCustom = 0;

	int iPeriodTime = 0;

	int iMachineCode = 0;

	__int64 iPeso;
	int iCash = 0;
	int iChannelingCash = 0;
	bool bAlarm         = false;

	rkPacket >> iResult;

	switch( iResult )
	{
	case EXTRAITEM_BUY_OK:
	case EXTRAITEM_PRESENT:
	case EXTRAITEM_BOX_PRESENT:
		{
			rkPacket >> iItemCode >> iReinforce >> iSlotIndex >> iTradeType >> iPeriodType >> iValue1 >> iValue2 >> dwMaleCustom >> dwFemaleCustom;
			rkPacket >> iPeriodTime >> iPeso >> iCash >> iChannelingCash >> iMachineCode >> bAlarm;

			int iBonusPeso;
			rkPacket >> iBonusPeso;

#if defined( USE_GA )
			bool bGACash	= false;
			bool bGAMoney	= false;
			int iPesoPrice	= g_MyInfo.GetMoney() - iPeso;
			int iCashPrice	= g_MyInfo.GetCash() - iCash;
			if( iCashPrice > 0 && iCashPrice < 1000000 )
				bGACash		= true;
			if( iPesoPrice > 0 )
				bGAMoney	= true;
#endif

			ioUserExtraItem *pUserExtraItem = g_MyInfo.GetUserExtraItem();
			if( pUserExtraItem )
			{
				EXTRAITEMSLOT kSlot;
				kSlot.m_iItemCode = iItemCode;
				kSlot.m_iReinforce = iReinforce;
				kSlot.m_iIndex = iSlotIndex;
				kSlot.m_iTradeState = iTradeType;
				kSlot.m_PeriodType = iPeriodType;
				kSlot.m_iValue1 = iValue1;
				kSlot.m_iValue2 = iValue2;
				kSlot.m_dwMaleCustom = dwMaleCustom;
				kSlot.m_dwFemaleCustom = dwFemaleCustom;

				pUserExtraItem->AddExtraItemData( kSlot );
			}
			
			g_QuestMgr.QuestCompleteTerm( QC_BUY_EXTRA_ITEM, iMachineCode );
			
			// UI 처리	
			NewShopBoughtWnd *pNewShopBoughtWnd = NULL;
			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShopWnd )
				pNewShopBoughtWnd = dynamic_cast<NewShopBoughtWnd*> ( pNewShopWnd->FindChildWnd( NewShopWnd::ID_BOUGHT_WND ) );
			
			if( iResult == EXTRAITEM_BUY_OK )
			{
				if( pNewShopBoughtWnd && pNewShopBoughtWnd->IsShow() )
					pNewShopBoughtWnd->SetResultInfo( PRESENT_EXTRAITEM, iItemCode, (iReinforce*10000) + iPeriodTime, false, iTradeType );
			}
			else if( iResult == EXTRAITEM_BOX_PRESENT )
			{
				MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
				if( pInvenWnd && pInvenWnd->IsShow() )
				{
					pInvenWnd->ShowExtraItemBoxWnd( iMachineCode );
					pInvenWnd->OnExtraItemBoxResultInfo( iItemCode, iPeriodTime, iReinforce, iTradeType );
					pInvenWnd->UpdateTab( false, false );
				}
			}

			g_MyInfo.SetMoney( iPeso );
			g_MyInfo.SetCash( iCash );
			g_MyInfo.SetChannelingCash( iChannelingCash );

#if defined( USE_GA )
			if( bGACash )
			{
				RandomMachineInfo *pInfo = g_ExtraItemInfoMgr.GetRandomMachineInfo( iMachineCode );
				if( pInfo )
				{
					iCashPrice = pInfo->GetNeedCash();

					if( iCashPrice > 0 )
					{
						// GOLD_USE_GEAR
						g_HttpMng.GA_ItemHitTracking( g_MyInfo.GetUserIndex() 
							, "Gear"
							, iCashPrice
							, 1
							, iItemCode
							, "Gold"
							, "%2FGOLD%2FUSE%2FGEAR"
							, true );
					}					
				}				
			}
			if( bGAMoney )
			{
				// PESO_USE_GEAR
				g_HttpMng.GA_ItemHitTracking( g_MyInfo.GetUserIndex() 
					, "Gear"
					, iPesoPrice
					, 1
					, iItemCode
					, "Peso"
					, "%2FPESO%2FUSE%2FGEAR" );
			}			
#endif
		}
		return;
	case EXTRAITEM_PACKAGE_BUY_OK:
		{
			rkPacket >> iPeso >> iCash >> iChannelingCash >> iMachineCode;

			g_MyInfo.SetMoney( iPeso );
			g_MyInfo.SetCash( iCash );
			g_MyInfo.SetChannelingCash( iChannelingCash );

			g_QuestMgr.QuestCompleteTerm( QC_BUY_EXTRA_ITEM, iMachineCode );

			// UI 처리	
			NewShopBoughtWnd *pNewShopBoughtWnd = NULL;
			NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
			if( pNewShopWnd )
				pNewShopBoughtWnd = dynamic_cast<NewShopBoughtWnd*> ( pNewShopWnd->FindChildWnd( NewShopWnd::ID_BOUGHT_WND ) );

			if( pNewShopBoughtWnd && pNewShopBoughtWnd->IsShow() )
				pNewShopBoughtWnd->SetResultInfo( PRESENT_EXTRAITEM_BOX, iMachineCode, 0, false, EET_DISABLE );
		}
		return;
	case EXTRAITEM_EXCEPTION:
		{
			int iErrorNum = 0;
			rkPacket >> iErrorNum;
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), iErrorNum );
		}
		break;
	case EXTRAITEM_SLOT_FULL:
		{
			NewShopItemPurchaseLeedWnd *pPurchaseLeedWnd = dynamic_cast<NewShopItemPurchaseLeedWnd*> ( g_GUIMgr.FindWnd( ITEM_PURCHASE_LEED_WND ) );
			if ( pPurchaseLeedWnd )
			{
				ioEtcItem *pItem = g_EtcItemMgr.FindEtcItem( ioEtcItem::EIT_ETC_EXTRAITEM_SLOT_EXTEND );
				if ( pItem )
				{
					ioHashString sIcon = pItem->GetIconName();
					ioHashString sSubIcon = pItem->GetSubIconName();
					ioHashString sTitle= pItem->GetName();
					ioHashStringVec sDescVec;
					sDescVec.push_back( "[장비슬롯확장]이 필요합니다." );
					sDescVec.push_back( "부족한 장비 슬롯을 위해 구입해주세요." );
					sDescVec.push_back( "상점 - 특별에서 구입하실 수 있습니다." );
					if( pPurchaseLeedWnd->SetInfo( ioEtcItem::EIT_ETC_EXTRAITEM_SLOT_EXTEND, sIcon, sSubIcon, sTitle, sDescVec ) )
						pPurchaseLeedWnd->ShowWnd();
				}
			}
		}
		break;
	case EXTRAITEM_PESO:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		}
		break;
	case EXTRAITEM_PERIOD:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		}
		break;
	case EXTRAITEM_LEVEL:
		{
			int iNeedLevel;
			rkPacket >> iNeedLevel;

			char szHelp[MAX_PATH]="";
			g_LevelMgr.GetGradeName( iNeedLevel, szHelp, sizeof( szHelp ), false );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4), szHelp );
		}
		break;
	case EXTRAITEM_BILLING_WAIT:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		}
		break;
	case EXTRAITEM_BILLING_DISCONNECT:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );	
		}
		break;
	case EXTRAITEM_BILLING_FAIL:
		{
			bool bBillingError    = false;
			ioHashString  sBillingError;
			rkPacket >> bBillingError;
			if( bBillingError )
				rkPacket >> sBillingError;

			ioHashString sErrorMent = STR(7);
			if( !bBillingError )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, sErrorMent.c_str() );
			else
			{
				ioHashString sError = "Billing Error : ";
				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
				if( pLocal ) 
					sError = pLocal->GetBillingErrorMent(sBillingError);

				char szErrorFullMent[MAX_PATH]="";
				ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
				if( pNode && pNode->GetType() == CNT_NEXON )					
					StringCbPrintf( szErrorFullMent, sizeof( szErrorFullMent ), "%s",  sError.c_str() ); //Except Extracting Hangeul
				else
					StringCbPrintf( szErrorFullMent, sizeof( szErrorFullMent ), "%s#%s%s", sErrorMent.c_str(), sError.c_str(), sBillingError.c_str() ); //Except Extracting Hangeul

				g_GUIMgr.SetMsgBox( MB_OK, NULL, szErrorFullMent );
			}
		}
		break;
	case EXTRAITEM_BUY_BILLING_EXCESS_BUY:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
		}
		break;
	}

	// 실패 상황만...
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true );
	}
}

void ioPlayStage::OnExtraItemChange( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	ioHashString szOwner;
	int iResult;

	rkPacket >> szOwner;
	rkPacket >> iResult;

	if( iResult == EXTRAITEM_CHANGE_FAIL )
	{
		int iNum;
		rkPacket >> iNum;
		//처음 에러나는 단 한번만 출력
		if ( !sg_bEquipItemError )
		{
			sg_bEquipItemError = true;
			int iType, iCharArray,iSlot,iNewSlotIndex;
			g_MyInfo.GetExtraItemChange( iType, iCharArray, iSlot, iNewSlotIndex );

			//ExtraItem 존재 유무 체크
			ioUserExtraItem* pUserExtraItem = g_MyInfo.GetUserExtraItem();
			if( !pUserExtraItem )
				return;

			EXTRAITEMSLOT rkExtraItem;
			char szError[MAX_PATH*2] = "";
			if( !pUserExtraItem->GetExtraItem( iNewSlotIndex, rkExtraItem ) )
			{
				SafeSprintf( szError, sizeof( szError ), "%s Not Exist SlotIndex, Fail Num : %d, Type : %d, Send( Char Num : %d, Slot : %d, SlotIndex : %d )",g_MyInfo.GetPublicID().c_str(), iNum, iType, iCharArray, iSlot, iNewSlotIndex );
			}
			else
			{
				bool bEquip = rkExtraItem.m_bCharEquip;
				int iCount = 0;
				int iCharIndex = -1;
				CHARACTER EquipCharData;
				EquipCharData.Init();
				for ( int i=0 ; i<g_MyInfo.GetCharCount() ; ++i )
				{
					//해당 아이템이 장착중인지 체크
					CHARACTER CharData = g_MyInfo.GetCharacter( i );
					if ( CharData.m_extra_item[iSlot] == iNewSlotIndex )
					{
						EquipCharData = CharData;
						iCharIndex = i;
						iCount++;
					}
				}
				if ( iCount == 0 )
					SafeSprintf( szError, sizeof( szError ), "%s - Character Not Equip, Fail Num : %d, Type : %d, Send( Char Num : %d, Slot : %d, SlotIndex : %d ), Item( ItemCode : %d, ItemEquip : %d )"
								, g_MyInfo.GetPublicID().c_str(), iNum, iType, iCharArray, iSlot, iNewSlotIndex, rkExtraItem.m_iItemCode, rkExtraItem.m_bCharEquip );
				else
				{
					int iClassType = EquipCharData.m_class_type;
					SafeSprintf( szError, sizeof( szError ), "%s - Already Equip, Fail Num : %d, Type : %d, Send( Char Num : %d, Slot : %d, SlotIndex : %d ), Char( Char Index : %d, CharType : %d, cnt : %d), Item( ItemCode : %d, ItemEquip : %d )"
						, g_MyInfo.GetPublicID().c_str(), iNum, iType, iCharArray, iSlot, iNewSlotIndex, iCharIndex, iClassType, iCount, rkExtraItem.m_iItemCode, rkExtraItem.m_bCharEquip );
				}
			}

			SP2Packet kPacket( LUPK_LOG );
			kPacket << "ChangeEquipError";	// 로그 파일 타입
			kPacket << szError;
			kPacket << 8500;				// 오류번호:8500
			kPacket << true;				// write db
			P2PNetwork::SendLog(kPacket);
		}

		ioBaseChar *pOwner = GetBaseChar( szOwner );
		if( GetModeType() != MT_MYROOM && !pOwner )
			return;

		if( pOwner && pOwner->IsOwnerChar() )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), iNum );

			if( pOwner->GetState() == CS_EXTRAITEM_CHANGE )
				pOwner->SetState( CS_DELAY );
		}
	}
	else if( iResult == EXTRAITEM_CHANGE_OK )
	{
		int iCharArray, iSlot, iNewSlotIndex;
		rkPacket >> iCharArray >> iSlot >> iNewSlotIndex;

		if( GetModeType() == MT_MYROOM || szOwner == g_MyInfo.GetPublicID() )
		{
			g_MyInfo.ApplyChangeExtraItem( iCharArray, iSlot, iNewSlotIndex );

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->UpdateTab( false, true );
			}

			SoldierSelectWnd *pSoldierWnd = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
			if( pSoldierWnd && pSoldierWnd->IsShow() )
			{
				pSoldierWnd->SettingSoldierBtn();
			}

			MiniSoldierSelectWnd *pMiniSoldierWnd = dynamic_cast<MiniSoldierSelectWnd*>( g_GUIMgr.FindWnd( MINI_SOLDIER_SELECT_WND ));
			if( pMiniSoldierWnd && pMiniSoldierWnd->IsShow() )
			{
				pMiniSoldierWnd->SettingMiniSoldierBtn();
			}

			// 진화or되돌리기 후 인벤 UI가 앞으로 나오는 문제때문에 진화UI를 다시 앞으로 나오게 함
			GradeAdjustmentWnd *pWnd = dynamic_cast<GradeAdjustmentWnd*>( g_GUIMgr.FindWnd( GRADE_ADJUSTMENT_WND ) );
			if( pWnd )
			{
				if( pWnd->IsShow() )
					pWnd->GoToTop();
			}
		}

		if( GetModeType() == MT_MYROOM )
		{
			if( m_pPlayMode )
			{
				m_pPlayMode->SetChangeExtraItem( iCharArray, iSlot );
			}
		}
		else
		{
			int iGameIndex, iNewItemCode, iNewReinforce;
			DWORD dwItemMaleCustom, dwItemFemaleCustom;
			rkPacket >> iGameIndex >> iNewItemCode >> iNewReinforce >> dwItemMaleCustom >> dwItemFemaleCustom;

			// 필드에 출전중인 캐릭터면 장비 교체
			ioBaseChar *pOwner = GetBaseChar( szOwner );
			bool bSelectChar = true;
			if( pOwner && pOwner->IsOwnerChar() && g_MyInfo.GetSelectCharArray() != iCharArray )
				bSelectChar = false;

			if( pOwner && bSelectChar && !IsModeStateResult() )
			{
				ioItem *pItem = NULL;
				if( iNewItemCode > 0 )
					pItem = CreateItem( iNewItemCode );

				if( pItem )
				{
					pItem->ReLoadProperty();
					pItem->SetOwnerName( szOwner );
					pItem->SetItemCreateIndex( iGameIndex );
					pItem->SetItemReinforce( iNewReinforce );
					pItem->SetItemCustom( dwItemMaleCustom, dwItemFemaleCustom );
					
					// 코스튬 관련 (코스튬 동기화 : 서버에서 받을수도 있음)
					int nSubType = pItem->GetType()-1;
					if ( COMPARE( nSubType, 0, MAX_INVENTORY ) )
					{
						pItem->SetCostumeCustom( pOwner->GetCharacterInfo().m_EquipCostume[nSubType].m_Costume_Male_Custom, pOwner->GetCharacterInfo().m_EquipCostume[nSubType].m_Costume_Female_Custom );
						pItem->SetCostume( pOwner->GetCharacterInfo().m_EquipCostume[nSubType].m_CostumeCode, pOwner->IsOwnerChar() );
					}
					
					float fPreSkillGauge = 0.0f;
					ioItem *pPreItem = pOwner->EquipItem( pItem, true );
					if( pPreItem )
						fPreSkillGauge = pPreItem->GetCurSkillGuage();

					pItem->SetCurSkillGauge( fPreSkillGauge );

					// 용병교체시 총알, 레이져 충전량 관련 처리
					if( pPreItem && pOwner->IsOwnerChar() && pItem->GetType() == ioItem::IT_WEAPON )
					{
						int iCurBullet = pPreItem->GetCurBullet();
						pItem->SetCurBullet( iCurBullet );
					}

					SAFEDELETE( pPreItem );
				}
				else if( !pItem && iNewItemCode > 0 )
				{
					// 예외상황
					if( pOwner->GetState() == CS_EXTRAITEM_CHANGE )
						pOwner->SetState( CS_DELAY );

					pOwner->CheckDefaultReinforce();

					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), iNewItemCode );
					return;
				}

				// iNewItemCode 값이 0 이라도 아래처리는 해야함
				if( pOwner->GetState() == CS_EXTRAITEM_CHANGE )
					pOwner->SetState( CS_DELAY );

				pOwner->CheckDefaultReinforce();
			}
		}
	}
	else
	{
		ioBaseChar *pOwner = GetBaseChar( szOwner );
		if( pOwner && pOwner->IsOwnerChar() )
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
	}

	if( szOwner == g_MyInfo.GetPublicID() )
		g_MyInfo.SetUserInfoMgr();
}

void ioPlayStage::OnExtraItemSellResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult = 0;
	rkPacket >> iResult;

	if( iResult == EXTRAITEM_SELL_OK )
	{
		int iSlotIndex = 0;
		__int64 iAddPeso, iPeso;
		rkPacket >> iSlotIndex;
		rkPacket >> iAddPeso;
		rkPacket >> iPeso;

		g_MyInfo.SetMoney( iPeso );

		ioHashString szItemName;
		ioHashString szIconName;
		ioHashString szSubIconName;
		int iItemReinforce = 0;
		bool bExtraItemCustom = false;
		int nGradeType = 0;

#if defined( USE_GA )
		int iGAItemIndex = 0;
#endif

		ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
		if( pExtraItem )
		{
			EXTRAITEMSLOT kSlot;
			if( pExtraItem->GetExtraItem( iSlotIndex, kSlot ) )
			{
				szItemName = kSlot.m_Name;
				szIconName = kSlot.m_IconName;
				iItemReinforce = kSlot.m_iReinforce;
				if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
					bExtraItemCustom = true;

				const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
				if( pItem )
					nGradeType = pItem->GetGradeType();

#if defined( USE_GA )
				iGAItemIndex = kSlot.m_iItemCode;
#endif
			}
		}

		if( !g_MyInfo.DeleteExtraItem( iSlotIndex ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			return;
		}

		// UI 처리
		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*> ( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
		if( pInvenWnd && pInvenWnd->IsShow() )
		{
			pInvenWnd->UpdateTab( false, true );

			char szConvertNum[MAX_PATH] = "";
			Help::ConvertNumToStrComma( iAddPeso, szConvertNum, sizeof( szConvertNum ) );

			ComplexStringPrinterVec vTitle;
			ComplexStringPrinterVec vDesc;

			ioComplexStringPrinter kTitle1;
			kTitle1.SetTextStyle( TS_NORMAL );
			kTitle1.SetBkColor( 0, 0, 0 );	
			kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kTitle1.AddTextPiece( FONT_SIZE_17, STR(5), szItemName.c_str(), iItemReinforce );
			vTitle.push_back( kTitle1 );

			ioComplexStringPrinter kTitle2;
			kTitle2.SetTextStyle( TS_NORMAL );
			kTitle2.SetBkColor( 0, 0, 0 );
			kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kTitle2.AddTextPiece( FONT_SIZE_17, STR(6), szConvertNum );
			vTitle.push_back( kTitle2 );

			ioComplexStringPrinter kDesc1;
			kDesc1.SetTextStyle( TS_NORMAL );
			kDesc1.SetBkColor( 0, 0, 0 );	
			kDesc1.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kDesc1.AddTextPiece( FONT_SIZE_12, STR(7), szItemName.c_str(), iItemReinforce );
			vDesc.push_back( kDesc1 ); 

			ioComplexStringPrinter kDesc2;
			kDesc2.SetTextStyle( TS_NORMAL );
			kDesc2.SetBkColor( 0, 0, 0 );
			kDesc2.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kDesc2.AddTextPiece( FONT_SIZE_12, STR(8), szConvertNum );
			vDesc.push_back( kDesc2 );

			pInvenWnd->ShowItemRecvSellInfoWnd( ItemRecvSellInfoWnd::ITEM_SELL, ItemRecvSellInfoWnd::ITEM_EXTRAITEM, szIconName, szSubIconName, vTitle, vDesc, -1, iItemReinforce, bExtraItemCustom, false, nGradeType );
		}

#if defined( USE_GA )
		char chItemIndex[32]	= {0,};
		char chSlash[16]		= {0,};
		char chPostData[256]	= {0,};

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		{
			sprintf_e( chItemIndex, "%d", iGAItemIndex );
			sprintf_e( chSlash, "%%2F" );
			sprintf_e( chPostData, "%sPESO_GET_BUY_GEAR_ITEM%s%s", chSlash, chSlash, chItemIndex );
		}
		else
		{
			SafeSprintf( chItemIndex, sizeof(chItemIndex), "%1", iGAItemIndex );
			SafeSprintf( chSlash, sizeof(chSlash), "%2F" );
			SafeSprintf( chPostData, sizeof(chPostData), "%1PESO_GET_BUY_GEAR_ITEM%2%3", chSlash, chSlash, chItemIndex );
		}		

		// PESO_GET_BUY_GEAR_ITEM
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Peso"
			, "Gear"
			, ""
			, iAddPeso
			, chPostData );
#endif
	}
	else if( iResult == EXTRAITEM_SELL_FAIL )
	{
		int iNum;
		rkPacket >> iNum;

		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4), iNum );
	}
}

void ioPlayStage::OnCharExtraItemChange( SP2Packet &rkPacket )
{
	ioHashString szCharName;
	rkPacket >> szCharName;

	ioBaseChar *pChar = GetBaseChar( szCharName );
	if( pChar )
	{
		pChar->SetExtraItemChangeState( true );
	}
}

void ioPlayStage::OnBattleRoomRoundEnd( SP2Packet &rkPacket )
{	
	if( GetRoomStyle() == RSTYLE_SHUFFLEROOM )
	{
		// 셔플모드 종료시 로비로 이탈 처리
		SP2Packet kPacket( CTPK_EXIT_ROOM );
		kPacket << EXIT_ROOM_QUICK_OUT << 0 << true;
		TCPNetwork::SendToServer( kPacket );
		g_ShuffleRoomMgr.LeaveRoom();
	}
	else if( g_BattleRoomMgr.IsTournamentModeType() )
	{
		// 대회 경기 종료시 로비로 이탈 처리
		SP2Packet kPacket( CTPK_EXIT_ROOM );
		kPacket << EXIT_ROOM_QUICK_OUT << 0 << true;
		TCPNetwork::SendToServer( kPacket );
		g_BattleRoomMgr.LeaveRoom();
	}
	else
	{
		g_BattleRoomMgr.SetBattleModePlaying( false );
		int iBlue,iRed;
		rkPacket >> iBlue >> iRed;
		if( iBlue != 0 && iRed != 0 )
		{
			g_SoundMgr.AllSoundStop();
			g_SoundMgr.StopStream();

			g_GUIMgr.HideWnd( TIME_SCORE_WND );
			g_GUIMgr.HideWnd( KO_CHAT_MSG_WND );
		}
		else 
		{
			if( iBlue + iRed > 1 )
			{
				g_SoundMgr.AllSoundStop();
				g_SoundMgr.StopStream();

				g_GUIMgr.HideWnd( TIME_SCORE_WND );
				g_GUIMgr.HideWnd( KO_CHAT_MSG_WND );
			}
		}	

		g_VoiceChat.RemoveAllSoundPlayer();
		g_VoiceOffListManager.DeleteAll();
		VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
		if(pWnd)
			pWnd->AllOtherVoiceOff();
		if ( Setting::UseVoiceChat() && Setting::UseMIC() )
		{
			SP2Packet kPacket( CTPK_VOICE_INFO );
			kPacket << ID_VOICE_ON;
			ioHashString szEmptyString; // 빈스트링이면 파티원 모드에게 전송
			kPacket << szEmptyString;
			TCPNetwork::SendToServer( kPacket );
#ifndef SHIPPING
			LOG.PrintTimeAndLog( 0, "%s CTPK_VOICE_INFO | ID_VOICE_ON | %s", __FUNCTION__, szEmptyString.c_str() );
#endif
		}

		if ( Setting::UseVoiceChat() )
			g_ChatMgr.SetSystemMsg( STR(1) );

		HelpWnd* pHelpWnd = dynamic_cast<HelpWnd*>( g_GUIMgr.FindWnd( HELP_WND ) );
		if( pHelpWnd && m_pPlayMode->GetModeType() == MT_ARENA )
			pHelpWnd->ReCreateBtn();
	}
	g_MyInfo.SetArenaModeCharacter( MT_NONE );
}

void ioPlayStage::OnMovingRoom( SP2Packet &rkPacket )
{
	int iModeType = 0;
	int iSubNum = 0;
	int iMapNum = 0;
	int iPlazaType = PT_NONE;
	rkPacket >> iModeType >> iSubNum >> iMapNum >> iPlazaType;

	char szCall[MAX_PATH];
	sprintf_s( szCall, "%s - (%d), (%d), (%d), (%d)" ,__FUNCTION__, iModeType, iSubNum, iMapNum, iPlazaType );

	g_App.SetNextStage( ioGameStage::GS_PLAY, szCall );
	g_App.SetModeType( iModeType, iSubNum, iMapNum, (PlazaType)iPlazaType );

	if( (ModeType)iModeType == MT_TRAINING || (ModeType)iModeType == MT_HEADQUARTERS|| (ModeType)iModeType == MT_HOUSE )
		g_VictoriesMgr.Initialize();

	//g_MyInfo.SetArenaModeCharacter( (ModeType)iModeType );
}

void ioPlayStage::OnCharSlotChange( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	int iType = 0;
	rkPacket >> iType;

	if( iType == CHAR_SLOT_CHANGE_FAIL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}

	int iLeftSlotIndex = 0, iRightSlotIndex = 0;
	rkPacket >> iLeftSlotIndex >> iRightSlotIndex;

	int iLeftArray, iRightArray;
	if( m_pPlayMode->GetModeType() == MT_ARENA && g_BattleRoomMgr.IsBattleModePlaying() )
	{
		iLeftArray = g_MyInfo.GetCharSlotIndexToArray( iLeftSlotIndex - g_MyInfo.GetOriginalCharacterCount() );
		iRightArray= g_MyInfo.GetCharSlotIndexToArray( iRightSlotIndex - g_MyInfo.GetOriginalCharacterCount() );
	}
	else
	{
		iLeftArray = g_MyInfo.GetCharSlotIndexToArray( iLeftSlotIndex );
		iRightArray= g_MyInfo.GetCharSlotIndexToArray( iRightSlotIndex );
	}

	if( g_MyInfo.CharSlotChange( iLeftArray, iLeftSlotIndex, iRightArray, iRightSlotIndex ) )
	{
		SoldierSelectWnd *pSoldier = dynamic_cast<SoldierSelectWnd*>(g_GUIMgr.FindWnd( SOLDIER_SELECT_WND ));
		if( pSoldier && pSoldier->IsShow() )
		{
			// 용병 윈도우에서 위치를 변경한다.
			pSoldier->ChangeSoldierSlot( iLeftArray, iRightArray );			
		}						

		UISoldierSelectWnd *pUISoldierWnd = dynamic_cast<UISoldierSelectWnd*>( g_GUIMgr.FindWnd( UI_SOLDIERSELECT_WND ) );
		if( pUISoldierWnd && pUISoldierWnd->IsShow() )
			pUISoldierWnd->ChangeSoldierSlot( iLeftArray, iRightArray );

		MiniSoldierSelectWnd *pMiniSoldierWnd = dynamic_cast<MiniSoldierSelectWnd*>( g_GUIMgr.FindWnd( MINI_SOLDIER_SELECT_WND ) );
		if( pMiniSoldierWnd && pMiniSoldierWnd->IsShow() )
			pMiniSoldierWnd->ChangeSoldierSlot( iLeftArray, iRightArray );

		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
		if( pInvenWnd && pInvenWnd->IsShow() )
		{
			pInvenWnd->UpdateTab( false, true );
		}
	}
}

void ioPlayStage::OnMedalItemBuy( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult = 0;
	rkPacket >> iResult;

	if( iResult  == MEDALITEM_BUY_PRESENT )
	{	
		ioUserMedalItem::MEDALITEMSLOT kSlot;

		__int64 iPeso = 0;
		int iCash     = 0;
		int iChannelingCash = 0;

		rkPacket >> kSlot.m_iItemType >> kSlot.m_iEquipClass >> kSlot.m_iPeriodType >> kSlot.m_iLimitDate >> kSlot.m_iLimitTime >> iPeso >> iCash >> iChannelingCash ;

		ioUserMedalItem *pUserMedalItem = g_MyInfo.GetUserMedalItem();
		if( pUserMedalItem )
			pUserMedalItem->AddMedalItemData( kSlot );

		if( iPeso != g_MyInfo.GetMoney() || iCash != g_MyInfo.GetCash() )
			g_ExSoundMgr.PlaySound( ExSound::EST_SHOP_BUY_SUCCESS );

		g_MyInfo.SetMoney( iPeso );
		g_MyInfo.SetCash( iCash );
		g_MyInfo.SetChannelingCash( iChannelingCash );
	}
	else
		LOG.PrintTimeAndLog( 0, "%s Error (%d)", __FUNCTION__, iResult );
}

void ioPlayStage::OnMedalItemChange( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	ioHashString szPublicID;
	int iResult;

	rkPacket >> szPublicID;
	rkPacket >> iResult;

	if( iResult == MEDALITEM_CHANGE_SKIP )
		return;

	if( iResult == MEDALITEM_CHANGE_FAIL )
	{
		if( szPublicID == g_MyInfo.GetPublicID() )
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}
	
	if ( iResult == MEDALITEM_ALREADY_EQUIPPED )
	{
		if ( szPublicID == g_MyInfo.GetPublicID() )
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "이미 장착 중입니다." );
		return;
	}

	if( iResult != MEDALITEM_CHANGE_OK )
	{
		if( szPublicID == g_MyInfo.GetPublicID() )
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), iResult );
		return;
	}
	
	// MEDALITEM_CHANGE_OK
	int iCharArray = 0;
	int iItemType  = 0;
	bool bEquip    = false;
	rkPacket >> iCharArray >> iItemType >> bEquip;

	if( szPublicID == g_MyInfo.GetPublicID() )
	{
		int iCharIndex = g_MyInfo.GetCharIndex( iCharArray );
		ioBaseChar *pChar = GetBaseChar( iCharIndex );
		if( pChar )
			pChar->SetMedalItem( iItemType, bEquip );

		g_MyInfo.ApplyChangeMedalItem( iCharArray, iItemType, 0, bEquip );

		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
		if( pInvenWnd && pInvenWnd->IsShow() )
			pInvenWnd->UpdateTab( false, true );

		g_MyInfo.SetUserInfoMgr();

		if( pChar )
			pChar->RecalcStatusValue();
	}
	else
	{
		ioBaseChar *pChar = GetBaseChar( szPublicID );
		if( pChar )
		{
			if( pChar->GetSelectCharArray() == iCharArray )
			{
				pChar->SetMedalItem( iItemType, bEquip );
				pChar->RecalcStatusValue();
			}
		}
	}
}

void ioPlayStage::OnCustomMedalItemChange( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	ioHashString szPublicID;
	int iResult;

	rkPacket >> iResult;
	rkPacket >> szPublicID;

	if( iResult == MEDALITEM_CHANGE_SKIP )
		return;
	if( iResult == MEDALITEM_CHANGE_FAIL )
	{
		if( szPublicID == g_MyInfo.GetPublicID() )
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		return;
	}	
	if ( iResult == MEDALITEM_ALREADY_EQUIPPED )
	{
		if ( szPublicID == g_MyInfo.GetPublicID() )
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "이미 장착 중입니다." );
		return;
	}
	if( iResult != MEDALITEM_CHANGE_OK )
	{
		if( szPublicID == g_MyInfo.GetPublicID() )
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), iResult );
		return;
	}
	
	// MEDALITEM_CHANGE_OK
	int iEquipClass = 0;
	int iItemType  = 0;
	int iCustomIndex = 0;
	bool bEquip    = false;
	int iStat[8];
	rkPacket >> iItemType >> iCustomIndex >> iEquipClass >> bEquip;
	for( int i = 0; i < 8; i++ )
		rkPacket >> iStat[i];

	if( szPublicID == g_MyInfo.GetPublicID() )
	{
		int iCharArray = g_MyInfo.GetClassArray( iEquipClass );
		int iCharIndex = g_MyInfo.GetCharIndexByClassType( iEquipClass );
		ioBaseChar *pChar = GetBaseChar( iCharIndex );
		if( pChar )
			pChar->SetCustomMedalItem( iItemType, iCustomIndex, iStat, bEquip );

		g_MyInfo.ApplyChangeMedalItem( iCharArray, iItemType, iCustomIndex, bEquip );

		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
		if( pInvenWnd && pInvenWnd->IsShow() )
			pInvenWnd->UpdateTab( false, true );

		g_MyInfo.SetUserInfoMgr();

		if( pChar )
			pChar->RecalcStatusValue();
	}
	else
	{
		ioBaseChar *pChar = GetBaseChar( szPublicID );
		if( pChar )
		{
			if( pChar->GetCharacterInfo().m_class_type == iEquipClass )
			{
				pChar->SetCustomMedalItem( iItemType, iCustomIndex, iStat, bEquip );
				pChar->RecalcStatusValue();
			}
		}
	}
}

void ioPlayStage::OnExpandMedalSlotOpen( SP2Packet &rkPacket )
{
	int iClassType, iLimitTime;
	BYTE iSlotNumber;

	rkPacket >> iClassType;
	rkPacket >> iSlotNumber;
	rkPacket >> iLimitTime;

	g_ExpandMedalSlotMgr.AddExpandMedalSlot( iSlotNumber, iClassType, iLimitTime );

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->OnExpandMedalSlotResultWnd( iClassType, iSlotNumber );
		pInvenWnd->UpdateTab( false, false );
	}
}

void ioPlayStage::OnExpandMedalSlotData( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );

	ioExpandMedalSlotManager::ExpandMedalSlotVec kMedalSlotVec;
	IntVec vExpandMedalSlot;
	vExpandMedalSlot.clear();
	int iSize = 0;
	vExpandMedalSlot.reserve( iSize );
	rkPacket >> iSize;
	for( int i=0; i<iSize; ++i )
	{
		int iSlot;
		rkPacket >> iSlot;
		vExpandMedalSlot.push_back( iSlot );
	}

	if( pChar )
		pChar->SetExMedalSlot( vExpandMedalSlot );
}

void ioPlayStage::OnHeroMatchOtherName( SP2Packet &rkPacket )
{
	ioHashString kNickName; 
	rkPacket >> kNickName;

	g_LadderTeamMgr.SetLadderOtherTeamName( kNickName );

	SP2Packet kPacket( CTPK_HERO_MATCH_OTHER_INFO );
	kPacket << kNickName;
	TCPNetwork::SendToServer( kPacket );
}

void ioPlayStage::OnHeroMatchOtherInfo( SP2Packet &rkPacket )
{
	g_LadderTeamMgr.ApplyHeroMatchOtherInfo( rkPacket );	

	HeroMatchVersusSceneWnd *pVersusScene = dynamic_cast<HeroMatchVersusSceneWnd*>(g_GUIMgr.FindWnd( HERO_MATCH_VERSUS_SCENE_WND ));
	if( pVersusScene && pVersusScene->IsShow() )
		pVersusScene->OtherHeroInfoUpdate();
}

void ioPlayStage::OnCharEquipMedalItemData( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
		return;

	pChar->ClearMedalItem();

	for (int i = 0; i < ioMedalItemInfoManager::MAX_SLOT_NUM ; i++)
	{
		int iItemType = 0;
		rkPacket >> iItemType;
		if( iItemType != 0 )
			pChar->SetMedalItem( iItemType, true );	
	}
	for (int i = 0; i < ioMedalItemInfoManager::MAX_SLOT_NUM ; i++)
	{
		int  iMedalType;
		int iCustomIndex;
		int iStat[8];
		rkPacket >> iMedalType >> iCustomIndex;
		if( iCustomIndex != 0 )
		{
			for( int j = 0; j < 8; j++ )
				rkPacket >> iStat[j];

			pChar->SetCustomMedalItem( iMedalType, iCustomIndex, iStat, true );	
		}
	}
	if( szName == g_MyInfo.GetPublicID() )
		g_MyInfo.SetUserInfoMgr();
}

void ioPlayStage::OnEtcItemMotionStateTCP( SP2Packet &rkPacket )
{
	ioHashString kName;
	rkPacket >> kName;

	ioBaseChar *pChar = GetBaseChar( kName );
	if( !pChar ) return;

	if( pChar->GetState() == CS_DIE )
		return;

	int iEtcItemCode;
	rkPacket >> iEtcItemCode;

	pChar->SetEtcItemMotionState( iEtcItemCode );
}

void ioPlayStage::OnItemGrowthCatalyst( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iType;
	rkPacket >> iType;
	switch( iType )
	{
	case EXTRAITEM_GROWTH_CATALYST_OK:
		{
			bool bGrowthOK;
			int iEtcType;
			int iTargetSlot, iTargetPeriodType;
			int iTargetReinforce;

			rkPacket >> iEtcType;
			rkPacket >> bGrowthOK;
			rkPacket >> iTargetSlot >> iTargetReinforce >> iTargetPeriodType;

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->OnItemGrowthCatalystResult( bGrowthOK, iTargetSlot, iTargetReinforce, iTargetPeriodType, iEtcType );
				pInvenWnd->UpdateTab( false, false );
			}

			ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
			if( pExtraItem )
			{
				EXTRAITEMSLOT kTargetSlot;
				if( pExtraItem->GetExtraItem( iTargetSlot, kTargetSlot ) )
				{
					kTargetSlot.m_iReinforce = iTargetReinforce;
					kTargetSlot.m_PeriodType = iTargetPeriodType;
					pExtraItem->SetExtraItem( kTargetSlot );
				}
				else
				{
					LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemGrowthCatalyst - Target Error" );
				}
			}
		}
		return;
	case EXTRAITEM_GROWTH_CATALYST_NONE_ITEM:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		}
		break;
	case EXTRAITEM_GROWTH_CATALYST_NONE_TYPE:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		}
		break;
	}

	// UI 갱신
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true );
	}
}

void ioPlayStage::OnItemCompoundEx( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iType;
	rkPacket >> iType;
	switch( iType )
	{
	case EXTRAITEM_COMPOUNDEX_OK:
		{
			int iEtcType;
			bool bSuccess;
			int iTargetSlot, iTargetReinforce;

			rkPacket >> iEtcType;
			rkPacket >> bSuccess >> iTargetSlot >> iTargetReinforce;

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->OnItemCompoundExResult( bSuccess, iTargetSlot, iTargetReinforce, iEtcType );
				pInvenWnd->UpdateTab( false, false );
			}

			ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
			if( pExtraItem )
			{
				EXTRAITEMSLOT kTargetSlot;
				if( pExtraItem->GetExtraItem( iTargetSlot, kTargetSlot ) )
				{
					kTargetSlot.m_iReinforce = iTargetReinforce;
					if ( bSuccess )
						kTargetSlot.m_failExp = 0;
					pExtraItem->SetExtraItem( kTargetSlot );
				}
				else
				{
					LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemCompoundEx - Target Error" );
				}
			}
		}
		return;
	case EXTRAITEM_COMPOUNDEX_NONE_ITEM:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		}
		break;
	case EXTRAITEM_COMPOUNDEX_NONE_TYPE:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		}
		break;
	case EXTRAITEM_COMPOUNDEX_EXCEPTION:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		}
		break;
	case EXTRAITEM_COMPOUNDEX_REINFORCE_LIMIT:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		}
		break;
	}

	// UI 갱신
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true );
	}
}

void ioPlayStage::OnRouletteStartAni( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnRouletteStartAni - %s Not Exist User", szName.c_str() );
		return;
	}

	pChar->OnRouletteState( rkPacket );
}

void ioPlayStage::OnRouletteStart( SP2Packet &rkPacket )
{
	if( g_RouletteMgr.IsRouletteEvent() )
		g_RouletteMgr.OnRouletteStart( rkPacket );
}

void ioPlayStage::OnRouletteJoinEnd( SP2Packet &rkPacket )
{
	if( g_RouletteMgr.IsRouletteEvent() )
		g_RouletteMgr.OnRouletteJoinEnd( rkPacket );
}

void ioPlayStage::OnRouletteEnd( SP2Packet &rkPacket )
{
	if( g_RouletteMgr.IsRouletteEvent() )
		g_RouletteMgr.OnRouletteEnd( rkPacket );
}

void ioPlayStage::OnRouletteException( SP2Packet &rkPacket )
{
	if( g_RouletteMgr.IsRouletteEvent() )
		g_RouletteMgr.OnRouletteException( rkPacket );
}

void ioPlayStage::OnBingoData( SP2Packet &rkPacket )
{
	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		pBingo->ApplyAllData( rkPacket, false );
	}
}

void ioPlayStage::OnBingoNumberInit( SP2Packet &rkPacket )
{
	ioBingo *pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		pBingo->ApplyAllData( rkPacket, true );
	}
}

bool ioPlayStage::ProcessUDPPacket( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 2100, 300 );
	
	g_P2PChecker.AddRecvPacket( rkPacket.GetPacketID() );

	switch( rkPacket.GetPacketID() )
	{
	case CUPK_CHAT:
		ErrorReport::SetPosition( 2100, 301 );
		OnChat( rkPacket );
		ErrorReport::SetPosition( 2100, 302 );
		return true;
	case CUPK_HOLE_CHECK:
		ErrorReport::SetPosition( 2100, 303 );
		OnHoleCheck( rkPacket );
		ErrorReport::SetPosition( 2100, 304 );
		return true;
	case CUPK_NEW_USER_INFO:
		ErrorReport::SetPosition( 2100, 307 );
		OnNewUserInfo( rkPacket );
		ErrorReport::SetPosition( 2100, 308 );
		return true;
	case CUPK_EXIST_USER_INFO:
		ErrorReport::SetPosition( 2100, 309 );
		OnExistUserInfo( rkPacket );
		ErrorReport::SetPosition( 2100, 310 );
		return true;
	case CUPK_CHAR_CONTROL:
		ErrorReport::SetPosition( 2100, 311 );
		OnControl( rkPacket );
		ErrorReport::SetPosition( 2100, 312 );
		return true;
	case CUPK_ATTACK:
		ErrorReport::SetPosition( 2100, 313 );
		OnAttack( rkPacket );
		ErrorReport::SetPosition( 2100, 314 );
		return true;
	case CUPK_DEFENSE:
		ErrorReport::SetPosition( 2100, 315 );
		OnDefense( rkPacket );
		ErrorReport::SetPosition( 2100, 316 );
		return true;
	case CUPK_DEFENSE_RELEASE:
		ErrorReport::SetPosition( 2100, 317 );
		OnDefenseRelease( rkPacket );
		ErrorReport::SetPosition( 2100, 318 );
		return true;
	case CUPK_PRE_DEFENSE_RELEASE:
		ErrorReport::SetPosition( 2100, 319 );
		OnPreDefenseRelease( rkPacket );
		ErrorReport::SetPosition( 2100, 320 );
		return true;
	case CUPK_JUMP:
		ErrorReport::SetPosition( 2100, 321 );
		OnJump( rkPacket );
		ErrorReport::SetPosition( 2100, 322 );
		return true;
	case CUPK_FLY_JUMP:
		ErrorReport::SetPosition( 2100, 323 );
		OnFlyJump( rkPacket );
		ErrorReport::SetPosition( 2100, 324 );
		return true;
	case CUPK_DOUBLE_JUMP:
		ErrorReport::SetPosition( 2100, 325 );
		OnDoubleJump( rkPacket );
		ErrorReport::SetPosition( 2100, 326 );
		return true;
	case CUPK_BUFF_JUMP:
		ErrorReport::SetPosition( 2100, 327 );
		OnBuffJump( rkPacket );
		ErrorReport::SetPosition( 2100, 328 );
		return true;
	case CUPK_JUMP_ATTACK:
		ErrorReport::SetPosition( 2100, 329 );
		OnJumpAttack( rkPacket );
		ErrorReport::SetPosition( 2100, 330 );
		return true;
	case CUPK_EXTEND_JUMP:
		ErrorReport::SetPosition( 2100, 331 );
		OnExtendJump( rkPacket );
		ErrorReport::SetPosition( 2100, 332 );
		return true;
	case CUPK_JUMPPING:
		ErrorReport::SetPosition( 2100, 333 );
		OnJumpping( rkPacket );
		ErrorReport::SetPosition( 2100, 334 );
		return true;
	case CUPK_DASH_ATTACK:
		ErrorReport::SetPosition( 2100, 335 );
		OnDashAttack( rkPacket );
		ErrorReport::SetPosition( 2100, 336 );
		return true;
	case CUPK_PICK_ITEM_TRY:
		ErrorReport::SetPosition( 2100, 337 );
		OnPickItemTry( rkPacket );
		ErrorReport::SetPosition( 2100, 338 );
		return true;
	case CUPK_DELETE_ITEM_TRY:
		OnDeleteFieldItemTry( rkPacket );
		return true;
	case CUPK_USE_SKILL:
		ErrorReport::SetPosition( 2100, 339 );
		OnUseSkill( rkPacket );
		ErrorReport::SetPosition( 2100, 340 );
		return true;
	case CUPK_WEAPON_DEFENSE:
		ErrorReport::SetPosition( 2100, 341 );
		OnWeaponDefense( rkPacket );
		ErrorReport::SetPosition( 2100, 342 );
		return true;
	case CUPK_WEAPON_WOUNDED:
		ErrorReport::SetPosition( 2100, 343 );
		OnWeaponWounded( rkPacket );
		ErrorReport::SetPosition( 2100, 344 );
		return true;
	case CUPK_EXTEND_DEFENSE:
		ErrorReport::SetPosition( 2100, 345 );
		OnExtendDefense( rkPacket );
		ErrorReport::SetPosition( 2100, 346 );
		return true;
	case CUPK_PUSHSTRUCT_MOVE:
		ErrorReport::SetPosition( 2100, 347 );
		OnPushStructMove( rkPacket );
		ErrorReport::SetPosition( 2100, 348 );
		return true;
	case CUPK_PUSHSTRUCT_DELAY:
		ErrorReport::SetPosition( 2100, 349 );
		OnPushStructDelay( rkPacket );
		ErrorReport::SetPosition( 2100, 350 );
		return true;
	case CUPK_DROP_DIE_MOTION:
		ErrorReport::SetPosition( 2100, 351 );
		OnDropDieMotion( rkPacket );
		ErrorReport::SetPosition( 2100, 352 );
		return true;
	case CUPK_BLOW_GET_UP:
		ErrorReport::SetPosition( 2100, 353 );
		OnBlowGetUp( rkPacket );
		ErrorReport::SetPosition( 2100, 354 );
		return true;
	case CUPK_DROP_DOWN_GET_UP:
		ErrorReport::SetPosition( 2100, 355 );
		OnDropDownGetUp( rkPacket );
		ErrorReport::SetPosition( 2100, 356 );
		return true;
	case CUPK_SYSTEM_DAMAGE:
		ErrorReport::SetPosition( 2100, 357 );
		OnSystemDamage( rkPacket );
		ErrorReport::SetPosition( 2100, 358 );
		return true;
	case CUPK_MINE_WAIT_EXPLOSION:
		ErrorReport::SetPosition( 2100, 359 );
		OnMineWaitExplosion( rkPacket );
		ErrorReport::SetPosition( 2100, 360 );
		return true;
	case CUPK_MINE_EXPLOSION:
		ErrorReport::SetPosition( 2100, 361 );
		OnMineExplosion( rkPacket );
		ErrorReport::SetPosition( 2100, 362 );
		return true;
	case CUPK_COUNTER_ATTACK:
		ErrorReport::SetPosition( 2100, 363 );
		OnCounterAttack( rkPacket );
		ErrorReport::SetPosition( 2100, 364 );
		return true;
	case CUPK_EXPLOSION_ON_HAND:
		ErrorReport::SetPosition( 2100, 365 );
		OnExplosionOnHand( rkPacket );
		ErrorReport::SetPosition( 2100, 366 );
		return true;
	case CUPK_EXPLOSION_DROP:
		ErrorReport::SetPosition( 2100, 367 );
		OnExplosionDrop( rkPacket );
		ErrorReport::SetPosition( 2100, 368 );
		return true;
	case CUPK_ITEM_GAUGE_OK:
	case CUPK_ITEM_GAUGE_CANCEL:
		ErrorReport::SetPosition( 2100, 369 );
		OnItemGauge( rkPacket );
		ErrorReport::SetPosition( 2100, 370 );
		return true;
	case CUPK_FALL_DAMAGE:
		ErrorReport::SetPosition( 2100, 371 );
		OnFallDamage( rkPacket );
		ErrorReport::SetPosition( 2100, 372 );
		return true;
	case CUPK_REQUEST_CHAR_CONTROL:
		ErrorReport::SetPosition( 2100, 373 );
		OnRequestControl( rkPacket );
		ErrorReport::SetPosition( 2100, 374 );
		return true;
	case CUPK_CHANGE_DASH_RUN:
		ErrorReport::SetPosition( 2100, 375 );
		OnChangeDashRun( rkPacket );
		ErrorReport::SetPosition( 2100, 376 );
		return true;
	case CUPK_UPDATE_MAGIC_CIRCLE:
		ErrorReport::SetPosition( 2100, 377 );
		OnMagicCircle( rkPacket );
		ErrorReport::SetPosition( 2100, 378 );
		return true;
	case SUPK_SYNCTIME:
		ErrorReport::SetPosition( 2100, 381 );
		OnSyncTime( rkPacket );
		ErrorReport::SetPosition( 2100, 382 );
		return true;
	case CUPK_VOICE_DATA:
		ErrorReport::SetPosition( 2100, 383 );
		OnPushVoiceData( rkPacket );
		ErrorReport::SetPosition( 2100, 384 );
		return true;
	case CUPK_SKILL_EXTRA_INFO:
		ErrorReport::SetPosition( 2100, 385 );
		OnSkillExtraInfo( rkPacket );
		ErrorReport::SetPosition( 2100, 386 );
		return true;
	case CUPK_PUSHSTRUCT_INFO:
		ErrorReport::SetPosition( 2100, 387 );
		OnPushStructInfo( rkPacket );
		ErrorReport::SetPosition( 2100, 388 );
		return true;
	case CUPK_PUSHSTRUCT_CREATEWEAPON:
		OnPushStructCreateWeapon( rkPacket );
		return true;
	case CUPK_PUSHSTRUCT_SEARCH_STATE:
		OnPushStructSearchState( rkPacket );
		return true;
	case CUPK_PUSHSTRUCT_TRACING_STATE:
		OnPushStructTracingState( rkPacket );
		return true;
	case CUPK_PUSHSTRUCT_FIRE_STATE:
		OnPushStructFireState( rkPacket );
		return true;
	case CUPK_FIELDITEM_INFO:
		ErrorReport::SetPosition( 2100, 389 );
		OnFieldItemInfo( rkPacket );
		ErrorReport::SetPosition( 2100, 390 );
		return true;
	case CUPK_AIM_FIRE:
		ErrorReport::SetPosition( 2100, 391 );
		OnAimFire( rkPacket );
		ErrorReport::SetPosition( 2100, 392 );
		return true;
	case CUPK_DRINK:
		ErrorReport::SetPosition( 2100, 393 );
		OnDrink( rkPacket );
		ErrorReport::SetPosition( 2100, 394 );
		return true;
	case CUPK_DROP_THROWBOMB:
		ErrorReport::SetPosition( 2100, 395 );
		OnDropThrowBomb( rkPacket );
		ErrorReport::SetPosition( 2100, 396 );
		return true;
	case CUPK_RELEASE_FROZEN:
		ErrorReport::SetPosition( 2100, 397 );
		OnFrozenRelease( rkPacket );
		ErrorReport::SetPosition( 2100, 398 );
		return true;
	case CUPK_END_BUFF:
		ErrorReport::SetPosition( 2100, 399 );
		OnEndBuff( rkPacket );
		ErrorReport::SetPosition( 2100, 400 );
		return true;
	case CUPK_SKILL_END:
		ErrorReport::SetPosition( 2100, 401 );
		OnSkillEnd( rkPacket );
		ErrorReport::SetPosition( 2100, 402 );
		return true;
	case CUPK_ADD_NEW_BUFF:
		ErrorReport::SetPosition( 2100, 405 );
		OnAddBuffBySkill( rkPacket );
		ErrorReport::SetPosition( 2100, 406 );
		return true;
	case CUPK_P2P_PING:
		ErrorReport::SetPosition( 2100, 407 );
		OnP2PPing( rkPacket );
		ErrorReport::SetPosition( 2100, 408 );
		return true;
	case CUPK_SHOPPING:
		ErrorReport::SetPosition( 2100, 409 );
		OnShopping( rkPacket );
		ErrorReport::SetPosition( 2100, 410 );
		return true;
	case CUPK_GUILD_INFO_CHANGE:
		ErrorReport::SetPosition( 2100, 411 );
		OnGuildInfoChange( rkPacket );
		ErrorReport::SetPosition( 2100, 412 );
		return true;
	case CUPK_REMOVE_BUFF:
		ErrorReport::SetPosition( 2100, 413 );
		OnRemoveBuff( rkPacket );
		ErrorReport::SetPosition( 2100, 414 );
		return true;
	case CUPK_PREVENT_TARGET_BUFF:
		OnPreventTargetBuff( rkPacket );
		return true;
	case CUPK_ELEVATOR_CHANGED:
		ErrorReport::SetPosition( 2100, 415 );
		OnElevatorChanged( rkPacket );
		ErrorReport::SetPosition( 2100, 416 );
		return true;
	case CUPK_CLOSEORDER_DIRECTION:
		ErrorReport::SetPosition( 2100, 417 );
		OnCloseOrderDirection( rkPacket );
		ErrorReport::SetPosition( 2100, 418 );
		return true;	
	case CUPK_BREAK_FALL:
		ErrorReport::SetPosition( 2100, 421 );
		OnBreakFallState( rkPacket );
		ErrorReport::SetPosition( 2100, 422 );
		return true;
	case CUPK_EMOTICON_SYNC:
		ErrorReport::SetPosition( 2100, 423 );
		OnEmoticonSync( rkPacket );
		ErrorReport::SetPosition( 2100, 424 );
		return true;
	case CUPK_RELEASE_FLOAT:
		ErrorReport::SetPosition( 2100, 427 );
		OnFloatRelease( rkPacket );
		ErrorReport::SetPosition( 2100, 428 );
		return true;
	case CUPK_RELEASE_CLOSEORDER:
		ErrorReport::SetPosition( 2100, 429 );
		OnCloseOrderRelease( rkPacket );
		ErrorReport::SetPosition( 2100, 430 );
		return true;
	case CUPK_TELEPORT_STATE:
		ErrorReport::SetPosition( 2100, 431 );
		OnTeleportState( rkPacket );
		ErrorReport::SetPosition( 2100, 432 );
		return true;
	case CUPK_CUSTOM_SOUND_INFO:
		ErrorReport::SetPosition( 2100, 433 );
		OnCustomSoundInfo( rkPacket );
		ErrorReport::SetPosition( 2100, 434 );
		return true;
	case CUPK_CUSTOM_SOUND_DATA:
		ErrorReport::SetPosition( 2100, 435 );
		OnCustomSoundData( rkPacket );
		ErrorReport::SetPosition( 2100, 436 );
		return true;
	case CUPK_FALL_STATE:
		ErrorReport::SetPosition( 2100, 437 );
		OnFallState( rkPacket );
		ErrorReport::SetPosition( 2100, 438 );
		return true;
	case CUPK_END_AREA:
		ErrorReport::SetPosition( 2100, 439 );
		OnEndArea( rkPacket );
		ErrorReport::SetPosition( 2100, 440 );
		return true;
	case CUPK_AUTO_SKILL:
		ErrorReport::SetPosition( 2100, 441 );
		OnUseAutoSkill( rkPacket );
		ErrorReport::SetPosition( 2100, 442 );
		return true;
	case CUPK_CHANGE_CHARINFO:
		ErrorReport::SetPosition( 2100, 443 );
		OnChangeCharInfo( rkPacket );
		ErrorReport::SetPosition( 2100, 444 );
		return true;
	case CUPK_NONE_PLAY_STATE:
		ErrorReport::SetPosition( 2100, 445 );
		OnNonePlayState( rkPacket );
		ErrorReport::SetPosition( 2100, 446 );
		return true;
	case CUPK_SPECIAL_MOTION:
		ErrorReport::SetPosition( 2100, 447 );
		OnSpecialMotionState( rkPacket );
		ErrorReport::SetPosition( 2100, 448 );
		return true;
	case CUPK_USER_INFO_REFRESH:
		ErrorReport::SetPosition( 2100, 449 );
		OnUserInfoRefresh( rkPacket );
		ErrorReport::SetPosition( 2100, 450 );
		return true;
	case CUPK_USER_INFO_SYNC:
		ErrorReport::SetPosition( 2100, 451 );
		OnUserInfoSync( rkPacket );
		ErrorReport::SetPosition( 2100, 452 );
		return true;
	case CUPK_EXTRAITEM_CHANGE:
		ErrorReport::SetPosition( 2100, 453 );
		OnCharExtraItemChange( rkPacket );
		ErrorReport::SetPosition( 2100, 454 );
		return true;
	case CUPK_NPC_STUCK_STATE:
		ErrorReport::SetPosition( 2100, 455 );
		OnNpcStuckState( rkPacket );
		ErrorReport::SetPosition( 2100, 456 );
		return true;
	case CUPK_MONSTERCOIN_FIRST_SYNC:
		ErrorReport::SetPosition( 2100, 457 );
		OnMonsterCoinFirstSync( rkPacket );
		ErrorReport::SetPosition( 2100, 458 );
		return true;
	case CUPK_MONSTERCOIN_SYNC:
		ErrorReport::SetPosition( 2100, 459 );
		OnMonsterCoinSync( rkPacket );
		ErrorReport::SetPosition( 2100, 460 );
		return true;
	case CUPK_MONSTER_GLOBAL_TARGET:
		ErrorReport::SetPosition( 2100, 461 );
		OnMonsterGlobalTarget( rkPacket );
		ErrorReport::SetPosition( 2100, 462 );
		return true;
	case CUPK_RETURN_ATTACK_EXPLICIT:
		ErrorReport::SetPosition( 2100, 463 );
		OnReturnAttackExplicit( rkPacket );
		ErrorReport::SetPosition( 2100, 464 );
		return true;
	case CUPK_CHAIN_COLLISION:
		ErrorReport::SetPosition( 2100, 465 );
		OnChainCollision( rkPacket );
		ErrorReport::SetPosition( 2100, 466 );
		return true;
	case CUPK_CHAIN_CHANGE:
		ErrorReport::SetPosition( 2100, 467 );
		OnChainChange( rkPacket );
		ErrorReport::SetPosition( 2100, 468 );
		return true;
	case CUPK_DUMMY_EXPLICIT:
		ErrorReport::SetPosition( 2100, 469 );
		OnDummyExplicit( rkPacket );
		ErrorReport::SetPosition( 2100, 470 );
		return true;
	case CUPK_BUFF_FLY_JUMP:
		ErrorReport::SetPosition( 2100, 471 );
		OnBuffFlyJump( rkPacket );
		ErrorReport::SetPosition( 2100, 472 );
		return true;
	case CUPK_JUMP_AUTO_SHOT:
		ErrorReport::SetPosition( 2100, 473 );
		OnJumpAutoShot( rkPacket );
		ErrorReport::SetPosition( 2100, 474 );
		return true;
	case CUPK_EXCAVATING:
		ErrorReport::SetPosition( 2100, 475 );
		OnExcavationUDP( rkPacket );
		ErrorReport::SetPosition( 2100, 476 );
		return true;
	case CUPK_DODGE_DEFENSE:
		ErrorReport::SetPosition( 2100, 477 );
		OnDodgeDefense( rkPacket );
		ErrorReport::SetPosition( 2100, 478 );
		return true;
	case CUPK_FIRST_SYNC_CHARINFO:
		ErrorReport::SetPosition( 2100, 479 );
		OnFirstSyncCharInfo( rkPacket );
		ErrorReport::SetPosition( 2100, 480 );
		return true;
	case CUPK_BALL_WEAPON_COL:
		ErrorReport::SetPosition( 2100, 481 );
		OnBallWeaponCollision( rkPacket );
		ErrorReport::SetPosition( 2100, 482 );
		return true;
	case CUPK_BALL_WEAPON_CHANGE:
		ErrorReport::SetPosition( 2100, 483 );
		OnBallWeaponChange( rkPacket );
		ErrorReport::SetPosition( 2100, 484 );
		return true;
	case CUPK_SYNC_CHARINFO:
		ErrorReport::SetPosition( 2100, 485 );
		OnSyncCharInfo( rkPacket );
		ErrorReport::SetPosition( 2100, 486 );
		return true;
	case CUPK_TRACE_WEAPON:
		ErrorReport::SetPosition( 2100, 487 );
		OnTraceWeaponTrace( rkPacket );
		ErrorReport::SetPosition( 2100, 488 );
		return true;
	case CUPK_JUMP_DASH:
		ErrorReport::SetPosition( 2100, 489 );
		OnJumpDash( rkPacket );
		ErrorReport::SetPosition( 2100, 490 );
		return true;
	case CUPK_TRACE_WEAPON_WAIT:
		ErrorReport::SetPosition( 2100, 491 );
		OnTraceWeaponWait( rkPacket );
		ErrorReport::SetPosition( 2100, 492 );
		return true;
	case CUPK_AREAWEAPON:
		ErrorReport::SetPosition( 2100, 493 );
		OnAreaWeaponInfo( rkPacket );
		ErrorReport::SetPosition( 2100, 494 );
		return true;
	case CUPK_DOWN_ROLLING:
		ErrorReport::SetPosition( 2100, 495 );
		OnDownRolling( rkPacket );
		ErrorReport::SetPosition( 2100, 496 );
		return true;
	case CUPK_BUFF_AIR_ATTACK:
		ErrorReport::SetPosition( 2100, 497 );
		OnBuffAirAttack( rkPacket );
		ErrorReport::SetPosition( 2100, 498 );
		return true;
	case CUPK_NORMAL_DEAD_MINE:
		ErrorReport::SetPosition( 2100, 499 );
		OnNormalDeadMine( rkPacket );
		ErrorReport::SetPosition( 2100, 500 );
		return true;
	case CUPK_ETCITEM_MOTION:
		ErrorReport::SetPosition( 2100, 501 );
		OnEtcItemMotionStateUDP( rkPacket );
		ErrorReport::SetPosition( 2100, 502 );
		return true;
	case CUPK_PUSHSTRUCT_BOMBER:
		ErrorReport::SetPosition( 2100, 503 );
		OnPushStructBomber( rkPacket );
		ErrorReport::SetPosition( 2100, 504 );
		return true;
	case CUPK_WEAPON_ATTACK_EXPLICIT:
		ErrorReport::SetPosition( 2100, 505 );
		OnWeaponByWeaponExplicit( rkPacket );
		ErrorReport::SetPosition( 2100, 506 );
	case CUPK_WEAPON_ATTACK_EXPLICIT_LIST:
		ErrorReport::SetPosition( 2100, 507 );
		OnWeaponByWeaponExplicitList( rkPacket );
		ErrorReport::SetPosition( 2100, 508 );
		return true;
	case CUPK_COLLECT_JUMP_ATTACK:
		ErrorReport::SetPosition( 2100, 509 );
		OnCollectJumpAttack( rkPacket );
		ErrorReport::SetPosition( 2100, 510 );
		return true;
	case CUPK_NEW_JUMP_ATTACK:
		ErrorReport::SetPosition( 2100, 511 );
		OnNewJumpAttack( rkPacket );
		ErrorReport::SetPosition( 2100, 512 );
		return true;
	case CUPK_THROWBOMB_BOUND:
		ErrorReport::SetPosition( 2100, 513 );
		OnThrowBombBound( rkPacket );
		ErrorReport::SetPosition( 2100, 514 );
		return true;
	case CUPK_DEFENSE_ATTACK:
		ErrorReport::SetPosition( 2100, 515 );
		OnDefenseAttack( rkPacket );
		ErrorReport::SetPosition( 2100, 516 );
		return true;
	case CUPK_CREATE_STRUCT_STATE:
		ErrorReport::SetPosition( 2100, 517 );
		OnCreateStructState( rkPacket );
		ErrorReport::SetPosition( 2100, 518 );
		return true;
	case CUPK_WEAPON_SYNC:
		ErrorReport::SetPosition( 2100, 521 );
		OnWeaponSync( rkPacket );
		ErrorReport::SetPosition( 2100, 522 );
		return true;
	case CUPK_EXTEND_DASH:
		ErrorReport::SetPosition( 2100, 523 );
		OnExtendDash( rkPacket );
		ErrorReport::SetPosition( 2100, 524 );
		return true;
	case CUPK_ETCITEM_MOTION_OPTION_FIRST_SYNC:
		ErrorReport::SetPosition( 2100, 525 );
		OnEtcItemMotionOptionFirstSync( rkPacket );
		ErrorReport::SetPosition( 2100, 526 );
		return true;
	case CUPK_ETCITEM_MOTION_OPTION_SYNC:
		ErrorReport::SetPosition( 2100, 527 );
		OnEtcItemMotionOptionSync( rkPacket );
		ErrorReport::SetPosition( 2100, 528 );
		return true;
	case CUPK_GRAPPLING_SYNC:
		ErrorReport::SetPosition( 2100, 529 );
		OnGrapplingSync( rkPacket );
		ErrorReport::SetPosition( 2100, 530 );
		return true;
	case CUPK_DUMMY_CHAR_SYNC:
		ErrorReport::SetPosition( 2100, 531 );
		OnDummyCharSync( rkPacket );
		ErrorReport::SetPosition( 2100, 532 );
		return true;
	case CUPK_CHANGE_CHAR:
		ErrorReport::SetPosition( 2100, 533 );
		OnParentChangeChar( rkPacket, true );          // TCP & UDP 패킷 동일
		ErrorReport::SetPosition( 2100, 534 );
		return true;
	case CUPK_JUMP_RELOAD:
		ErrorReport::SetPosition( 2100, 535 );
		OnJumpReload( rkPacket );
		ErrorReport::SetPosition( 2100, 536 );
		return true;
	case CUPK_CROSS_BOW_JUMP:
		ErrorReport::SetPosition( 2100, 537 );
		OnCrossBowJump( rkPacket );
		ErrorReport::SetPosition( 2100, 538 );
		return true;
	case CUPK_CROSS_BOW_JUMP_AIM:
		ErrorReport::SetPosition( 2100, 539 );
		OnCrossBowJumpAim( rkPacket );
		ErrorReport::SetPosition( 2100, 540 );
		return true;
	case CUPK_CROSS_BOW_AIM:
		ErrorReport::SetPosition( 2100, 541 );
		OnCrossBowAim( rkPacket );
		ErrorReport::SetPosition( 2100, 542 );
		return true;
	case CUPK_BLOW_DASH:
		ErrorReport::SetPosition( 2100, 543 );
		OnBlowDash( rkPacket );
		ErrorReport::SetPosition( 2100, 544 );
		return true;
	case CUPK_PICK_REWARD_ITEM_TRY:
		ErrorReport::SetPosition( 2100, 545 );
		OnPickRewardItemTry( rkPacket );
		ErrorReport::SetPosition( 2100, 546 );
		return true;		
	case CUPK_WEREWOLF_STATE:
		ErrorReport::SetPosition( 2100, 547 );
		OnWereWolfState( rkPacket );
		ErrorReport::SetPosition( 2100, 548 );
		return true;
	case CUPK_CHAR_ACTION_INFO:
		ErrorReport::SetPosition( 2100, 551 );
		OnCharActionInfo( rkPacket );
		ErrorReport::SetPosition( 2100, 552 );
		return true;
	case CUPK_PASSIVE_GAUGE:
		ErrorReport::SetPosition( 2100, 553 );
		CheckPassiveGauge( rkPacket );
		ErrorReport::SetPosition( 2100, 554 );
		return true;
	case CUPK_WITCH_STATE:
		ErrorReport::SetPosition( 2100, 555 );
		OnWitchState( rkPacket );
		ErrorReport::SetPosition( 2100, 556 );
		return true;
	case CUPK_MACHINESTRUCT:
		ErrorReport::SetPosition( 2100, 557 );
		OnMachineStructUDP( rkPacket );
		ErrorReport::SetPosition( 2100, 558 );
		return true;
	case CUPK_DROP_SMARTBOOM:
		ErrorReport::SetPosition( 2100, 559 );
		OnDropSmartBomb( rkPacket );
		ErrorReport::SetPosition( 2100, 560 );
		return true;
	case CUPK_SMARTBOMB_BOUND:
		ErrorReport::SetPosition( 2100, 561 );
		OnSmartBombBound( rkPacket );
		ErrorReport::SetPosition( 2100, 562 );
		return true;
	case CUPK_SUMMON_POS_SYNC:
		ErrorReport::SetPosition( 2100, 563 );
		OnSummonDummyInfo( rkPacket );
		ErrorReport::SetPosition( 2100, 564 );
		return true;
	case CUPK_TRACE_MINE_WAIT_EXPLOSION:
		ErrorReport::SetPosition( 2100, 565 );
		OnTraceMineWaitExplosion( rkPacket );
		ErrorReport::SetPosition( 2100, 566 );
		return true;
	case CUPK_TRACE_MINE_NORMAL_DEAD:
		ErrorReport::SetPosition( 2100, 567 );
		OnTraceMineNormalDead( rkPacket );
		ErrorReport::SetPosition( 2100, 568 );
		return true;
	case CUPK_TRACE_MINE_FLY_TRACE:
		ErrorReport::SetPosition( 2100, 569 );
		OnTraceMineFlyTraceStart( rkPacket );
		ErrorReport::SetPosition( 2100, 570 );
		return true;
	case CUPK_TRACE_MINE_EXPLOSION:
		ErrorReport::SetPosition( 2100, 571 );
		OnTraceMineExplosion( rkPacket );
		ErrorReport::SetPosition( 2100, 572 );
		return true;
	case CUPK_COUNTER_ATTACK_SYNC:
		ErrorReport::SetPosition( 2100, 573 );
		OnCounterAttackSync( rkPacket );
		ErrorReport::SetPosition( 2100, 574 );
		return true;
	case CUPK_POWEREDARMOR_WOUNDED:
		ErrorReport::SetPosition( 2100, 575 );
		OnWoundedPoweredArmor( rkPacket );
		ErrorReport::SetPosition( 2100, 576 );
		return true;
	case CUPK_GP_STATE:
		ErrorReport::SetPosition( 2100, 577 );
		OnGPState( rkPacket );
		ErrorReport::SetPosition( 2100, 578 );
		return true;
	case CUPK_APPLY_EXTRA_BUFF_INFO:
		ErrorReport::SetPosition( 2100, 579 );
		OnBuffExtraInfo( rkPacket );
		ErrorReport::SetPosition( 2100, 580 );
		return true;
	case CUPK_CHECK_ESCAPE_SKILL:
		ErrorReport::SetPosition( 2100, 581 );
		OnCheckEscapeSkill( rkPacket );
		ErrorReport::SetPosition( 2100, 582 );
		return true;
	case CUPK_SMART_BOOM_GENERATE_WEAPON:
		ErrorReport::SetPosition( 2100, 583 );
		OnSmartBoomObjectItemGenerateWeapon( rkPacket );
		ErrorReport::SetPosition( 2100, 584 );
		return true;
	case CUPK_BUFF_MULTI_FLY:
		OnBuffMultiFlyJump( rkPacket );
		return true;
	case CUPK_TRACE2_MINE_DELAY:
		ErrorReport::SetPosition( 2100, 585 );
		OnTrace2MineDelay( rkPacket);
		ErrorReport::SetPosition( 2100, 586 );
		return true;
	case CUPK_TRACE2_MINE_NORMAL_DEAD:
		ErrorReport::SetPosition( 2100, 587 );
		OnTrace2MineNormalDead( rkPacket );
		ErrorReport::SetPosition( 2100, 588 );
		return true;
	case CUPK_TRACE2_MINE_WAIT_EXPLOSION:
		ErrorReport::SetPosition( 2100, 589 );
		OnTrace2MineWaitExplosion( rkPacket );
		ErrorReport::SetPosition( 2100, 590 );
		return true;
	case CUPK_TRACE2_MINE_EXPLOSION:
		ErrorReport::SetPosition( 2100, 591 );
		OnTrace2MineExplosion( rkPacket );
		ErrorReport::SetPosition( 2100, 592 );
		return true;
	case CUPK_REFLECT_ATTACK_EXPLICIT:
		ErrorReport::SetPosition( 2100, 591 );
		OnReflectAttackExplicit( rkPacket );
		ErrorReport::SetPosition( 2100, 592 );
		return true;
	case CUPK_AIR_MINE_WAIT_EXPLOSION:
		ErrorReport::SetPosition( 2100, 593 );
		OnAirMineWaitExplosion( rkPacket );
		ErrorReport::SetPosition( 2100, 594 );
		return true;
	case CUPK_AIR_MINE_EXPLOSION:
		ErrorReport::SetPosition( 2100, 595 );
		OnAirMineExplosion( rkPacket );
		ErrorReport::SetPosition( 2100, 596 );
		return true;
	case CUPK_AIR_MINE_NORMAL_DEAD:
		ErrorReport::SetPosition( 2100, 597 );
		OnAirNormalDeadMine( rkPacket );
		ErrorReport::SetPosition( 2100, 598 );
		return true;
	case CUPK_EQUIP_SKILL_EXTRA_INFO:
		ErrorReport::SetPosition( 2100, 599 );
		OnEquipSkillExtraInfo( rkPacket );
		ErrorReport::SetPosition( 2100, 600 );
		return true;		
	case CUPK_PET_SYNC:
		ErrorReport::SetPosition( 2100, 601 );
		g_PetMgr.OnPetSync( rkPacket );
		ErrorReport::SetPosition( 2100, 602 );
		return true;
	case CUPK_NAKORURU_STATE:
		ErrorReport::SetPosition( 2100, 603 );
		OnNakoruruState( rkPacket );
		ErrorReport::SetPosition( 2100, 604 );
		return true;
	case CUPK_LAG_ACTION:
		ErrorReport::SetPosition( 2100, 605 );
		OnLagAction( rkPacket );
		ErrorReport::SetPosition( 2100, 606 );
		return true;
	case CUPK_INITIALIZE_SPECIAL_STATE:
		ErrorReport::SetPosition( 2100, 607 );
		OnInitializeSpecialState( rkPacket );
		ErrorReport::SetPosition( 2100, 608 );
		return true;
	case CUPK_SPECIAL_STATE_SYNC:
		ErrorReport::SetPosition( 2100, 609 );
		OnSpecialStateSync( rkPacket );
		ErrorReport::SetPosition( 2100, 610 );
		return true;
	case CUPK_RANDOM_APPLY_BUFF:
		ErrorReport::SetPosition( 2100, 611 );
		g_RSoldierInfoManager.OnRandomBuff( rkPacket, this );
		ErrorReport::SetPosition( 2100, 612 );
		return true;
	case CUPK_PASSIVE_SKILL_EXTRA_INFO:
		ErrorReport::SetPosition( 2100, 613 );
		OnPassiveSkillExtraInfo( rkPacket );
		ErrorReport::SetPosition( 2100, 614 );
		return true;
	case CUPK_SYNC_SKILL_GAUGE:
		ErrorReport::SetPosition( 2100, 615 );
		OnShareSkillGauge( rkPacket );
		ErrorReport::SetPosition( 2100, 616 );
		return true;
	case CUPK_BOOST_JUMP:
		ErrorReport::SetPosition( 2100, 617 );
		OnBoostJump( rkPacket );
		ErrorReport::SetPosition( 2100, 618 );
		return true;
	case CUPK_HIDE_OBJECT_WEAPON_INDEX_SYNC:
		ErrorReport::SetPosition( 2100, 619 );
		OnHideObjectWeaponIndexSync( rkPacket );
		ErrorReport::SetPosition( 2100, 620 );
		return true;
	default:
		{
			if( m_pBlockWorld && m_pBlockWorld->ProcessUDPPacket( rkPacket ) )
			{
				return true;
			}

			if( m_pPlayMode )
			{
				return m_pPlayMode->ProcessUDPPacket( rkPacket );
			}
		}
	}

	return false;
}

void ioPlayStage::OnChat( SP2Packet &rkPacket )
{
	int iChatType;
	rkPacket >> iChatType;

	if( iChatType != CT_ANNOUNCE )
	{
		//HARDCODE: 게임에서 보낸 메시지가 룸까지 도착못하게 하기 위함
		//			일반적인 경우는 문제가 안되는데 게임에서 나갈때 받은 메시지를 걸러내기 위함이다.
		if( ToMyRoomMode( m_pPlayMode ) )
			return;
	}

	int iTeam;
	ioHashString szID, szChat;
	bool bNoChatSound;
	bool bCusomSound;

	char szChatBuf[MAX_PATH] = "";
	if( iChatType == CT_ALL )
	{
		rkPacket >> iTeam;
		rkPacket >> szID;
		rkPacket >> szChat;
		rkPacket >> bNoChatSound;
		rkPacket >> bCusomSound;

		if( !g_App.IsNetworkTableUser( szID ) )        //UDP 정보가 없는 유저로부터의 채팅은 무시 
			return; 

		if( Setting::UseBlockUsersCustomSound() )
			bCusomSound = false;

		g_MannerTrialChatMgr.Add( ioMannerTrialChatManager::TT_NORMAL_CHAT, szID.c_str(), szChat.c_str() ); // 욕필터전에 할것

		StringCbCopy( szChatBuf, MAX_PATH, g_SlangUtil.ConvertStringWithPreLine( szChat.c_str() ) );
		szChat = szChatBuf;

		SetChatIcon( szID, szChat, bNoChatSound, bCusomSound );
		SetChatMsg( (TeamType)iTeam, szID.c_str(), szChat.c_str(), bNoChatSound, bCusomSound );
	}
	else if( iChatType == CT_TEAM )
	{
		rkPacket >> iTeam;
		rkPacket >> szID;
		rkPacket >> szChat;
		rkPacket >> bNoChatSound;
		rkPacket >> bCusomSound;

		if( !g_App.IsNetworkTableUser( szID ) )        //UDP 정보가 없는 유저로부터의 채팅은 무시 
			return; 

		if( Setting::UseBlockUsersCustomSound() )
			bCusomSound = false;

		if( GetOwnerTeam() == (TeamType)iTeam )
		{
			g_MannerTrialChatMgr.Add( ioMannerTrialChatManager::TT_NORMAL_CHAT, szID.c_str(), szChat.c_str() ); // 욕필터전에 할것

			StringCbCopy( szChatBuf, MAX_PATH, g_SlangUtil.ConvertStringWithPreLine( szChat.c_str() ) );
			szChat = szChatBuf;

			SetChatIcon( szID, szChat, bNoChatSound, bCusomSound );
			SetTeamChatMsg( (TeamType)iTeam, szID.c_str(), szChat.c_str(), bNoChatSound, bCusomSound );
		}
	}
	else if( iChatType == CT_ANNOUNCE )
	{
		rkPacket >> szID;
		rkPacket >> szChat;

		char szAnn[MAX_PATH] = "";
		sprintf( szAnn, "%s", szChat.c_str() );
		g_ChatMgr.SetSystemMsg( szAnn );
	}
}

void ioPlayStage::OnHoleCheck( SP2Packet &rkPacket )
{
	if( Help::IsOnlyServerRelay() )
		return;     // 서버 중계

	ioHashString szUserID, szUserIP;
	int iPort;
	rkPacket >> szUserID >> szUserIP >> iPort;
	bool bHoleRecvComplete = g_App.IsHoleRecvComplete( szUserID );

	if( g_App.IsMeDeveloper() )
	{
		char szLog[MAX_PATH*2] = "";
		sprintf( szLog, "OnHoleCheck : %s(%d) : %s:%d", szUserID.c_str(), (int)bHoleRecvComplete, szUserIP.c_str(), iPort );
		LOG.PrintTimeAndLog( 0, "%s", szLog );
		g_ChatMgr.SetSystemMsg( szLog );
	}

	g_App.SetHoleRecvComplete( szUserID, true, szUserIP, iPort );
	if( !bHoleRecvComplete )
	{
		// 홀 패킷 받았다고 서버로 전송
		SP2Packet kPacket( CTPK_HOLE_SEND_COMPLETE );
		kPacket << szUserID << szUserIP << iPort;
		TCPNetwork::SendToServer( kPacket );
	}	
}

void ioPlayStage::OnSyncTime( SP2Packet &rkPacket )
{
#ifdef NPROTECT
	g_ioNProtect.Check( false );
#endif 

	// 외침 가능 여부
	bool bWholeChatOn;
	rkPacket >> bWholeChatOn;
	Help::SetWholeChatOn( bWholeChatOn );
	Help::SetWholeRainbowChatOn( bWholeChatOn );

	// 핑 시간
	int iServerDelayMS = g_GlobalTimer.SetGlobalTime( rkPacket );
	ioBaseChar *pOwnerChar = GetOwnerChar();
	if( !pOwnerChar )
		return;

	if( pOwnerChar->CheckAndSetNetworkBadEmoticon( iServerDelayMS ) )
	{
		if( GetModeType() == MT_NONE || GetModeType() == MT_MYROOM )
			return;

		// 배드핑 발생
		if( g_BattleRoomMgr.IsBattleRoom() && g_BattleRoomMgr.IsBattleModePlaying() )
		{
			if( g_BattleRoomMgr.IsBadPingKick() )
			{
				if( m_iBadNetworkCount++ >= Help::GetBadNetworkKickCount() )
				{
					// 전투방 자동 이탈
					SendOwnerDamageList();

					SP2Packet kPacket( CTPK_EXIT_ROOM );
					kPacket << EXIT_ROOM_BAD_NETWORK << 0 << true;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
					g_BattleRoomMgr.LeaveRoom();
				}
			}
		}
		else if( g_LadderTeamMgr.IsLadderTeam() && g_LadderTeamMgr.IsLadderBattleModePlaying() )
		{
			if( g_LadderTeamMgr.IsBadPingKick() )
			{
				if( m_iBadNetworkCount++ >= Help::GetBadNetworkKickCount() )
				{
					// 레더팀 자동 이탈
					SendOwnerDamageList();

					SP2Packet kPacket( CTPK_EXIT_ROOM );
					kPacket << EXIT_ROOM_BAD_NETWORK << 0 << true;
					TCPNetwork::SendToServer( kPacket );
					TCPNetwork::MouseBusy( true );
					g_LadderTeamMgr.LeaveTeam();
				}
			}
		}
	}
}

void ioPlayStage::OnP2PPing( SP2Packet &rkPacket )
{
	DWORD dwPingTime;
	ioHashString szSendID;
	rkPacket >> szSendID >> dwPingTime;
	P2PNetwork::SetLastRecvTime( szSendID, dwPingTime );
}

void ioPlayStage::OnShopping( SP2Packet &rkPacket )
{
	int iBuyPeso;
	ioHashString szBuyID;
	rkPacket >> szBuyID >> iBuyPeso;
	g_AwardMgr.AddShopping( szBuyID, iBuyPeso );
}

void ioPlayStage::OnGuildInfoChange( SP2Packet &rkPacket )
{
	DWORD dwGuildIndex, dwGuildMark;
	ioHashString szUserID;
	rkPacket >> szUserID >> dwGuildIndex >> dwGuildMark;

	g_GuildInfo.SetGuildMark( szUserID, dwGuildIndex, dwGuildMark );
}

void ioPlayStage::OnNewUserInfo( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pNewUser = GetBaseChar( szName );
	if( pNewUser )
	{
		pNewUser->ApplyCharInfoInNetwork( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnNewUserInfo - %s Not Exist", szName.c_str() );
		return;
	}

	ioBaseChar *pMyChar = GetOwnerChar();
	if( !pMyChar )	return;

	SP2Packet kCharInfo( CUPK_EXIST_USER_INFO );
	kCharInfo << pMyChar->GetCharName();
	pMyChar->GetCharInfoInNetwork( kCharInfo );
	P2PNetwork::SendToUser( pNewUser->GetCharName(), kCharInfo );
}

void ioPlayStage::OnExistUserInfo( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pUser = GetBaseChar( szName );
	if( pUser )
	{
		pUser->ApplyCharInfoInNetwork( rkPacket );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnExistUserInfo - %s Not Exist", szName.c_str() );
	}
}

void ioPlayStage::OnControl( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar* pChar = GetBaseChar( szName );
	if( !pChar )
	{
		g_RequestCharMgr.AddData( szName );
		g_RequestCharMgr.IncreaseValue( szName );

		if ( g_RequestCharMgr.IsCheckValue( szName ) )
		{
			g_RequestCharMgr.DelData( szName );

			SP2Packet kPacket( CTPK_PLAYING_USERDATA_INFO );
			kPacket << szName;
			TCPNetwork::SendToServer( kPacket );
		}
		return;
	}

	int iSendIndex;
	rkPacket >> iSendIndex;

	if( ToNpcChar( pChar ) )
	{
		pChar->ApplyCharControl( rkPacket );
		g_P2PChecker.SetControlLog( rkPacket.GetPacketID(), szName, iSendIndex, -1, true );
	}
	else
	{
		int iPreIndex = P2PNetwork::GetLastControlIndex( szName );
		if( iPreIndex >= 0 && iPreIndex <= iSendIndex )
		{
			P2PNetwork::SetLastControlIndex( szName, iSendIndex );
			pChar->ApplyCharControl( rkPacket );
			g_P2PChecker.SetControlLog( rkPacket.GetPacketID(), szName, iSendIndex, iPreIndex, true );
		}
		else
		{
			g_P2PChecker.SetControlLog( rkPacket.GetPacketID(), szName, iSendIndex, iPreIndex, false );
		} 
	}
}

void ioPlayStage::OnAttack( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;

	pChar->ApplyNormalAttack( rkPacket );
}

void ioPlayStage::OnDefense( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;

	if( pChar->GetState() == CS_DIE )
		return;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;
	pChar->SetTargetRotAndMoveDirChange( qtRot );

	int iDefenseDirKey;
	rkPacket >> iDefenseDirKey;
	pChar->SetCurDefenseDirKey( (ioUserKeyInput::DirKeyInput)iDefenseDirKey );
	pChar->SetDefenseState();

	float fCurHP;
	rkPacket >> fCurHP;
	pChar->SetCurHP( fCurHP );
}

void ioPlayStage::OnDefenseRelease( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ReleaseDefenseState();

	float fCurHP;
	rkPacket >> fCurHP;
	pChar->SetCurHP( fCurHP );
}

void ioPlayStage::OnPreDefenseRelease( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->PreReleaseDefenseState();
}

void ioPlayStage::OnJump( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyJumpState( rkPacket );
}

void ioPlayStage::OnCrossBowJump( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyCrossBowJumpState( rkPacket );
}

void ioPlayStage::OnCrossBowJumpAim( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	int iCurJumpType;
	rkPacket >> iCurJumpType;

	pChar->SetCurJumpType( (JumpType) iCurJumpType );
	pChar->SetCrossBowJumpAimState();
}

void ioPlayStage::OnFlyJump( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyFlyJumpState( rkPacket );
}

void ioPlayStage::OnBoostJump( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyBoostJumpState( rkPacket );
}

void ioPlayStage::OnDoubleJump( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;
	
	pChar->ApplyDoubleJump( rkPacket );
}

void ioPlayStage::OnJumpDash( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyJumpDash( rkPacket );
}

void ioPlayStage::OnBuffJump( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;
	
	pChar->ApplyBuffJump( rkPacket );
}

void ioPlayStage::OnBuffFlyJump( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyBuffFlyJump( rkPacket );
}

void ioPlayStage::OnBuffMultiFlyJump( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;

	pChar->ApplyBuffMultiFlyJump( rkPacket );
}

void ioPlayStage::OnJumpAttack( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;

	int iJumpType;
	rkPacket >> iJumpType;
	pChar->SetCurJumpType( (JumpType)iJumpType );

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;
	pChar->SetWeaponIndexBase( dwWeaponIdx );

	float fCurHP;
	rkPacket >> fCurHP;
	pChar->SetCurHP( fCurHP );

	D3DXQUATERNION qtRotate;
	rkPacket >> qtRotate;

	D3DXVECTOR3 vMoveDir, vInputDir;
	rkPacket >> vMoveDir >> vInputDir;

	pChar->SetJumpAttckTargetRot( qtRotate, vMoveDir, vInputDir );

	pChar->SetJumpAttack();
	pChar->ApplyAutoTargetInfo( rkPacket );
}

void ioPlayStage::OnDashAttack( SP2Packet &rkPacket )
{
	ErrorReport::SetPosition( 1100, 551 );

	ioHashString szName;
	rkPacket >> szName;

	ErrorReport::SetPosition( 1100, 552 );

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;

	ErrorReport::SetPosition( 1100, 553 );

	pChar->ApplyDashAttack( rkPacket );

	ErrorReport::SetPosition( 1100, 554 );
}

void ioPlayStage::OnPickItemTry( SP2Packet &rkPacket )
{	
	ioHashString szName;
	rkPacket >> szName;
	
	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	if( pChar->GetState() == CS_DRINK )
		return;

	pChar->SetPickItemState( NULL );
}

void ioPlayStage::OnDeleteFieldItemTry( SP2Packet &rkPacket )
{	
	ioHashString szName;
	ioHashString szDeleteAni;
	rkPacket >> szName >> szDeleteAni;
	
	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->SetDeleteFieldItemState( NULL, szDeleteAni );
}

void ioPlayStage::OnPickRewardItemTry( SP2Packet &rkPacket )
{	
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	if( pChar->GetState() == CS_DRINK )
		return;

	pChar->SetPickRewardItemState( NULL );
}

void ioPlayStage::OnUseSkill( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	// Index Check
	DWORD dwCharIndex;
	rkPacket >> dwCharIndex;
	if( pChar->GetIndex() != dwCharIndex )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnUseSkill(%s) Not Match CharIndex : %d - %d", 
								pChar->GetCharName().c_str(), pChar->GetIndex(), dwCharIndex );

		pChar->AddChangeCharPacketReserve( dwCharIndex, rkPacket );
		return;
	}

	P2PNetwork::SetLastControlIndex( szName, rkPacket );

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;

	pChar->SetWeaponIndexBase( dwWeaponIdx );
	pChar->CheckComboAddTime();

	int iSkillNum;
	BYTE bySkillNum;
	rkPacket >> bySkillNum;
	iSkillNum = bySkillNum;

	float fCurHP, fCurGauge;
	ioHashString szSkillName;
	HALF halfTemp;
	BYTE byCurGauge;
	rkPacket >> halfTemp >> byCurGauge;
	fCurHP = HALFToFloat(halfTemp);
	fCurGauge = byCurGauge;
	rkPacket >> szSkillName;
	
	if( IsNpcChar( pChar ) )
	{
		ioNpcChar *pNpcChar = ToNpcChar( pChar );
		pNpcChar->ChangeItemSkill( (EquipSlot)iSkillNum, szSkillName );
	}

	D3DXQUATERNION qtRot;
	D3DXVECTOR3 vRot;
	HALF half[3];
	rkPacket >> half[0] >> half[1] >> half[2];
	InputHalf2Vec( half, vRot );
	D3DXQuaternionRotationYawPitchRoll( &qtRot, vRot.y, vRot.x, vRot.z );

	int iSkillUseType;
	BYTE bySkillUseType;
	rkPacket >> bySkillUseType;
	iSkillUseType = bySkillUseType;

	DWORD dwSeed;
	rkPacket >> dwSeed;
	pChar->SetRandomSeed( dwSeed );

	bool bCancelSkill;
	rkPacket >> bCancelSkill;

	// 스킬이 바뀌었는지를 먼저 체크한다
	pChar->ApplyCurSkillName( iSkillNum, szSkillName );

	// 캐릭터 방향을 먼저 동기화 시킴
	pChar->SetTargetRotAndMoveDirChange( qtRot );

	pChar->ApplyAutoTargetInfo( rkPacket );

	if( !pChar->SetUseSkill( iSkillNum, (SkillUseType)iSkillUseType, true, bCancelSkill ) )
	{
		LOG.PrintTimeAndLog( 0, "OnUseSkill Function Failed! %s", pChar->GetCharName().c_str() );
	}
	pChar->SetCurHP( fCurHP );
	pChar->SetCurItemSkillGauge( iSkillNum, fCurGauge );
}

void ioPlayStage::OnUseAutoSkill( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	int iSkillNum;
	rkPacket >> iSkillNum;

	float fCurHP, fCurGauge;
	rkPacket >> fCurHP >> fCurGauge;

	pChar->ApplyUseAutoSkill( iSkillNum );
	pChar->SetCurHP( fCurHP );
	pChar->SetCurItemSkillGauge( iSkillNum, fCurGauge );
}

void ioPlayStage::OnWeaponDefense( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	P2PNetwork::SetLastControlIndex( szName, rkPacket );

	//
	int iWeaponIndex, iAttributeIndex;
	rkPacket >> iWeaponIndex >> iAttributeIndex;

	ioHashString szAttacker;
	ioHashString szItemName;
	rkPacket >> szAttacker;
	rkPacket >> szItemName;

	pChar->SetAttackerName( szAttacker );
	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pAttacker, iWeaponIndex );
	const WeaponAttribute *pAttr = g_WeaponMgr.FindAttribute( iAttributeIndex );

	ioMissileWeapon2 *pMissile2 = ToMissileWeapon2( pWeapon );
	if( pMissile2 )
	{
		pMissile2->CheckTargetColState( pAttacker );
	}

	float fDamage;
	fDamage = pChar->ApplyWeaponDefenseInNetwork( rkPacket, szAttacker, szItemName, iWeaponIndex, pAttr );

	if( Help::CheckEnableTimeGapUser(2) )
		g_WeaponMgr.CheckApplyWeaponDefense( szName, iWeaponIndex, iAttributeIndex );

	bool bPushAttacker, bSkipBlock;
	rkPacket >> bPushAttacker >> bSkipBlock;

	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnWeaponDefense - %s Not Exist", szAttacker.c_str() );
		return;
	}

	if( pAttr )
	{
		if( pAttr->m_WeaponExtraGaugeType != WGT_NONE )
		{
			pAttacker->SetExtraGaugeTypeByWeapon( EGT_ATTACK_DEF, pAttr->m_WeaponExtraGaugeType, pAttr->m_fDamageRate );
		}
	}

	bool bDashDefense;
	rkPacket >> bDashDefense;

	if( bDashDefense )
	{
		const ioAttackableItem *pDashItem = ToAttackableItemConst( g_ItemMaker.GetItemConst( szItemName, __FUNCTION__ ) );
		if( pDashItem )
		{
			ioHashStringVec vTotalBuff;
			vTotalBuff.clear();

			const ioPowerChargeDash2 *pPowerDash2 = ToPowerChargeDash2Const( pDashItem->GetCurExtendDashConst() );
			if( pPowerDash2 )
			{
				vTotalBuff = pPowerDash2->GetDashDefenseBuff();
			}

			for( int i=0; i < (int)vTotalBuff.size(); ++i )
			{
				ioHashString szBuff = vTotalBuff[i];
				if( !szBuff.IsEmpty() )
				{
					pAttacker->AddNewBuff( szBuff, szName, szItemName, NULL );
				}
			}
		}
	}

	ioHashString szAttackerSkill;
	rkPacket >> szAttackerSkill;

	pChar->SetLastAttackerName( szAttacker, szAttackerSkill );

	D3DXVECTOR3 vAttackDir;
	float fAttackPushPower;
	rkPacket >> vAttackDir >> fAttackPushPower;

	if( bPushAttacker )
	{
		if( !bSkipBlock )
		{
			ioAttackableItem *pItem = pChar->GetPriorityItem( SPT_DEFENSE );
			if( pItem && !pItem->GetDefenseWeakAttackPushBuff().IsEmpty() )
			{
				pAttacker->AddNewBuff( pItem->GetDefenseWeakAttackPushBuff(), pChar->GetCharName(), "", NULL );
			}

			pAttacker->ReverseReflectPush( vAttackDir, fAttackPushPower * FLOAT05 );
		}
	}

	int iBlockType, iProtectionType;
	bool bOnlyEmoticon;
	float fPushPower;
	rkPacket >> iBlockType >> iProtectionType >> bOnlyEmoticon >> fPushPower;

	AttackBlockType eBlockType = (AttackBlockType)iBlockType;
	ProtectionMode eProtectionType = (ProtectionMode)iProtectionType;
	if( eBlockType != ATB_NONE && !pAttacker->IgnoreBlock() )
	{
		if( !bOnlyEmoticon )
			pAttacker->WeakAttackPush( vAttackDir, fPushPower );

		// 블럭 이모티콘 처리
		pChar->CheckBlockState( eBlockType, eProtectionType );
	}

	bool bCollisionCancel;
	rkPacket >> bCollisionCancel;
	pAttacker->CheckRunChargeCancel( bCollisionCancel );

	bool bChangeJumpping;
	DWORD dwWeaponPeculiarity;
	float fChangeJumppingRate;
	int iLimitCnt;

	rkPacket >> bChangeJumpping >> dwWeaponPeculiarity;
	rkPacket >> fChangeJumppingRate >> iLimitCnt;

	int iActionStop = 0;
	rkPacket >> iActionStop;

	int iShakeCamera = 0;
	rkPacket >> iShakeCamera;

	bool bTargetDie = false;
	if( pChar->GetState() == CS_DIE )
		bTargetDie = true;

	if( pWeapon && !pWeapon->GetCalcActionDelayPassType() )
	{
		switch( iActionStop )
		{
		case AST_BOTH:
			pAttacker->SetActionStopDelay( fDamage, bTargetDie, iShakeCamera );
			pChar->SetActionStopDelay( fDamage, bTargetDie, iShakeCamera );
			break;
		case AST_ATTACKER:
			pAttacker->SetActionStopDelay( fDamage, bTargetDie, iShakeCamera );
			break;
		case AST_TARGET:
			pChar->SetActionStopDelay( fDamage, bTargetDie, iShakeCamera );
			break;

			if( pWeapon )
			{
				pWeapon->SetActionStopDuration( pChar->CalcActionStopDelay( fDamage, bTargetDie ),
					pChar->GetActionStopLooseRate( bTargetDie ) );
			}
		}
	}

	if( pWeapon )
	{
		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, pChar, iDummyIndex, true );
	}

	if( pWeapon && bPushAttacker )
	{
		if( bSkipBlock )
		{
			if( pAttacker->IsCharCollisionSkipUse() )
			{
				ioAttackableItem *pAttackerItem = pAttacker->GetPriorityItem( SPT_ATTACK );
				if( pAttackerItem && !pWeapon->GetCharCollisionBlockBuff().IsEmpty() )
				{
					pAttacker->AddNewBuff( pWeapon->GetCharCollisionBlockBuff(), pChar->GetCharName(), "", NULL );
					pAttacker->ReverseReflectPush( vAttackDir, fAttackPushPower * FLOAT05 );
				}
			}
		}
	}

	if( pAttr )
	{
		float fLooseRate = 0.0f;
		DWORD dwStopDuration = 0;		
		if( iActionStop != AST_NONE )
		{
			fLooseRate = ioBaseChar::GetActionStopLooseRate( bTargetDie );
			dwStopDuration = ioBaseChar::CalcActionStopDelay( fDamage, bTargetDie );
		}

		if( dwWeaponPeculiarity & WP_DEF_NO_SOUND )
		{
			AddWeaponExplosionEffect( pChar,
									  "sp2_defense_explosion.txt",
									  "",
									  vAttackDir,
									  dwStopDuration,
									  fLooseRate );
		}
		else
		{
			std::string szNewSound = pChar->GetDefensedSoundName(pAttr->m_DefensedSound);

			AddWeaponExplosionEffect( pChar,
									  "sp2_defense_explosion.txt",
									  ioHashString(szNewSound.c_str()),
									  vAttackDir,
									  dwStopDuration,
									  fLooseRate );
		}
	}

	if( pWeapon )
	{
		pWeapon->AddCollisionEntity( pChar );

		if( pWeapon->HasPeculiarity( WP_ONE_TIME_USE ) && pWeapon->GetPiercingType() == CPT_NONE )
		{
			pWeapon->SetWeaponDead();
		}
		else if( pWeapon->CheckWeaponByBuff() )
		{
			pWeapon->SetWeaponDead();
		}
	}

	//팀 제외
	if( pChar->GetTeam() != pAttacker->GetTeam() )
	{
		if( eBlockType == ATB_NONE && fDamage == 0.0f && m_pPlayMode )
		{
			if( m_pPlayMode->GetModeType() != MT_FOOTBALL )
			{
				g_AwardMgr.AddDefense( szName, FLOAT100 * m_pPlayMode->ContributeTeamPerA( pChar->GetTeam(), FLOAT1, 0.1f ) );
				if( pChar->IsOwnerChar()  )
					g_QuestMgr.QuestCompleteTerm( QC_ATTACK_DEFENSE );
			}
		}
	}

	if( pAttacker->IsOwnerChar() )
	{
		// ChangeJumpping
		if( bChangeJumpping )
		{
			pAttacker->SetChangeJumppingState( fChangeJumppingRate, true, iLimitCnt );
		}

		if( pWeapon && pWeapon->IsChangeNextItemType( true ) )
		{
			ioAttackableItem* pItem = pAttacker->GetPriorityItem( SPT_ATTACK );
			if( pItem )
				pItem->SetNextItemState( pChar, pAttacker, pWeapon, true, fDamage );
		}
	}

	// damagedefense effect
	if( pAttacker->GetDamageDefenseBuffValue() > 0.0f )
	{
		ioHashString szValue;
		pAttacker->GetDamageDefenseBuffStringValue( szValue );
		if( fDamage > 0.0f && !szValue.IsEmpty() )
		{
			pChar->AttachEffect( szValue );
		}
	}
}

void ioPlayStage::OnWeaponWounded( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	P2PNetwork::SetLastControlIndex( szName, rkPacket );

	ioHashString szSkill;
	rkPacket >> szSkill;

	BYTE byWeaponType;
	int iWeaponType;
	DWORD dwAttackerWeaponItemCode;
	rkPacket >> byWeaponType >> dwAttackerWeaponItemCode;
	iWeaponType = byWeaponType;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	DWORD dwWeaponIndex, iAttributeIndex;
	rkPacket >> dwWeaponIndex >> iAttributeIndex;

	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
		return;
	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pAttacker, dwWeaponIndex );
	const WeaponAttribute *pAttr = g_WeaponMgr.FindAttribute( iAttributeIndex );
	
	ioMissileWeapon2 *pMissile2 = ToMissileWeapon2( pWeapon );
	if( pMissile2 )
	{
		pMissile2->CheckTargetColState( pAttacker );
	}

	if( pAttr && pAttacker )
	{
		pAttacker->IncreaseChangeDamageByWeapon( pAttr->m_fWereWolfChangeDamage );
	}
	
	DefenseBreakType iDefenseBreakType = DBT_NONE;
	bool bISCreateBuff = false;
	float fDamage = pChar->ApplyWeaponWoundedInNetwork( rkPacket,
														szAttacker,
														szSkill,
														dwWeaponIndex,
														iAttributeIndex,
														iDefenseBreakType,
														bISCreateBuff );

	if( Help::CheckEnableTimeGapUser(2) )
		g_WeaponMgr.CheckApplyWeaponWounded( szName, dwWeaponIndex, iAttributeIndex );

	if( pAttr && pAttacker )
	{
		if( pAttr->m_WeaponExtraGaugeType != WGT_NONE && fDamage > 0.0f && pAttr->m_fDamageRate > 0.0f )
		{
			pAttacker->SetExtraGaugeTypeByWeapon( EGT_ATTACK, pAttr->m_WeaponExtraGaugeType, pAttr->m_fDamageRate );
		}
	}

	//Block & Reflect Start
	BYTE bPack8;
	bool bApplyPushPower;
	int iBlockType, iProtectionType;
	bool bOnlyEmoticon;
	float fPushPower;

	rkPacket >> bPack8;
	bApplyPushPower = ((bPack8 >> 1) & 1) == 1;
	bOnlyEmoticon = (bPack8 & 1) == 1;
	BYTE byBloackType,byProtectionType;
	rkPacket >> byBloackType >> byProtectionType;
	iBlockType = byBloackType;
	iProtectionType = byProtectionType;

	HALF half;
	rkPacket >> half;
	fPushPower = HALFToFloat(half);

	bool bPushAttacker, bStateProtectIgnoreReflect, bApplyStateProtect;
	rkPacket >> bPack8;
	bPushAttacker = ((bPack8 >> 2) & 1) == 1;
	bStateProtectIgnoreReflect = ((bPack8 >> 1) & 1) == 1;
	bApplyStateProtect = (bPack8 & 1) == 1;

	D3DXVECTOR3 vAttackDir;
	float fAttackPushPower;
	float fReflectAniRate;
	ioHashString szReflectAni;

	float compactX, compactY;
	rkPacket >> compactX >> compactY;
	decodeXYZ( vAttackDir, compactX, compactY );
	rkPacket >> half;
	fAttackPushPower = HALFToFloat(half);
	rkPacket >> szReflectAni;
	rkPacket >> half;
	fReflectAniRate = HALFToFloat(half);

	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnWeaponWounded - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	if( pWeapon )
	{
		if( bApplyPushPower )
			pWeapon->CheckWeaponCollision();

		int iDummyIndex = pWeapon->GetWeaponByDummyChar();
		g_DummyCharMgr.CheckEntityCollision( pAttacker, pChar, iDummyIndex, false );
	}

	// 괴도 루팡의 무기 스킬 웨폰 충돌 체크(피격된 상대 정보가 필요해 여기서 웨폰 호출함)
	ioItem *pItem = pAttacker->GetEquipedItem( ES_WEAPON );
	iolupinItem *pLupinItem = ToLupinItem( pItem );
	if( pAttacker && pChar && pLupinItem && pAttacker->GetCharName() == g_MyInfo.GetPublicID() )
	{
		pLupinItem->GetTargetCharByWeaponSkill( pAttacker, pChar, iAttributeIndex );
	}


	// last applyweapon check
	if( pAttr && (bApplyPushPower || fDamage > 0.0f) )
	{
		pChar->SetLastApplyWeapon( iAttributeIndex, pAttr->m_dwIgnoreWeaponTime );
	}

	AttackBlockType eBlockType = (AttackBlockType)iBlockType;
	ProtectionMode eProtectionType = (ProtectionMode)iProtectionType;
		
	if( bStateProtectIgnoreReflect && !pAttacker->IgnoreBlock() )
	{
		if( bApplyStateProtect )
		{
			if( !bOnlyEmoticon )
				pAttacker->WeakAttackPush( vAttackDir, fPushPower );

			pChar->CheckBlockState( eBlockType, eProtectionType );			
		}
	}
	else if( !pAttacker->IgnoreBlock() )
	{
		if( eBlockType != ATB_NONE )
		{
			if( !bOnlyEmoticon )
				pAttacker->WeakAttackPush( vAttackDir, fPushPower );

			pChar->CheckBlockState( eBlockType, eProtectionType );
		}
	}

	bool bCollisionCancel, bNoChangeGrappling, bChangeJumpping;
	rkPacket >> bPack8;	
	bCollisionCancel = ((bPack8 >> 2) & 1) == 1;
	bNoChangeGrappling = ((bPack8 >> 1) & 1) == 1;
	bChangeJumpping = (bPack8 & 1) == 1;

	pAttacker->CheckRunChargeCancel( bCollisionCancel );

	int iGrapplingType;
	BYTE byTemp;
	rkPacket >> byTemp;
	iGrapplingType = byTemp;

	if( pAttacker->IsNeedProcess() )
	{
		switch( iGrapplingType )
		{
		case GPT_WRESTLING:
		case GPT_WRESTLING_ALL:
			{
				pAttacker->SetGrapplingType( (GrapplingType)iGrapplingType );

				if( pAttacker->GetState() == CS_ATTACK )
					pAttacker->SetAttackGrapplingState( szName );
				else if( pAttacker->GetState() == CS_USING_SKILL )
					pAttacker->SetAttackSkillGrapplingState( szName );
				else if( pAttacker->GetState() == CS_JUMP )
					pAttacker->SetJumpGrapplingState( szName );
			}
			break;
		case GPT_PSYCHIC:
			{
				pAttacker->SetGrapplingType( (GrapplingType)iGrapplingType );

				if( pAttacker->GetState() == CS_USING_SKILL )
					pAttacker->SetPsycGrappling( szName );
			}
			break;
		case GPT_TOSS:
		case GPT_ATTACK:
		case GPT_COMPETE:
			{
				pAttacker->SetGrapplingType( (GrapplingType)iGrapplingType );

				if( pAttacker->GetState() == CS_USING_SKILL )
					pAttacker->SetAttackSkillGrapplingState( szName );
			}
			break;
		case GPT_MULTI:
			{
				if( bApplyPushPower && eBlockType == ATB_NONE && eProtectionType == PM_NONE )
				{
					pAttacker->SetGrapplingType( (GrapplingType)iGrapplingType );
					pChar->SetGrapplingType( (GrapplingType)iGrapplingType );
					if( pAttacker->GetState() == CS_USING_SKILL )
						pAttacker->SetAttackSkillGrapplingState( szName );
				}
			}
			break;
		case GPT_LUCIFER_AIR:
			{
				pAttacker->SetGrapplingType( (GrapplingType)iGrapplingType );
				if( pAttacker->GetState() == CS_LUCIFER_FLY || 
					pAttacker->GetState() == CS_SANTA_SPECIAL ||
					pAttacker->GetState() == CS_AUTOMATON_SPECIAL )
					pAttacker->SetAttackItemGrapplingState( szName );
				else if( pAttacker->GetState() == CS_JUMP )
					pAttacker->SetJumpGrapplingState( szName );
			}
			break;
		case GPT_NAKORURU:
			{
				pAttacker->SetGrapplingType( (GrapplingType)iGrapplingType );
				if( pAttacker->GetState() ==  CS_JUMP )
					pAttacker->SetJumpGrapplingState( szName );
			}
			break;
		case GPT_GRAPPLER_JUMP:
			{
				pAttacker->SetGrapplingType( (GrapplingType)iGrapplingType );
				if( pAttacker->GetState() ==  CS_JUMP )
					pAttacker->SetJumpGrapplingState( szName );
			}
			break;
		case GPT_MICHAEL:
			{
				pAttacker->SetGrapplingType( (GrapplingType)iGrapplingType );
				if( pAttacker->GetState() == CS_MICHAEL_FLY )
					pAttacker->SetAttackItemGrapplingState( szName );
			}
			break;
		case GPT_SUPER_DIMENSION:
			{
				pAttacker->SetGrapplingType( (GrapplingType)iGrapplingType );
				if( pAttacker->GetState() == CS_SUPER_DIMENSION_DASH )
					pAttacker->SetAttackItemGrapplingState( szName );
			}
			break;
		case GPT_STRIKER_AIR:
			{
				ioAttackableItem *pItem = pAttacker->GetPriorityItem( SPT_JUMP );
				if( pItem )
				{
					ioExtendJump *pExtendJump = pItem->GetCurExtendJump();
					if( pExtendJump && ToChargeComboJump3( pExtendJump) && 
						ToChargeComboJump3( pExtendJump )->GetCurChargeComboState() == ioChargeComboJump3::CCS_STRIKER_MOVE_ADD_ATTACK )
					{
						pAttacker->SetAttackItemGrapplingState( szName );
					}
				}
			}
			break;
		case GPT_JUMPER:
			{
				pAttacker->SetGrapplingType( (GrapplingType)iGrapplingType );
				if( pAttacker->GetState() == CS_JUMPER_SPECIAL )
					pAttacker->SetAttackItemGrapplingState( szName );
			}
			break;
		case GPT_FLY_JUMP_EXTRA_ATTACK:
			{
				pAttacker->SetGrapplingType( (GrapplingType)iGrapplingType );
				if( pAttacker->GetState() == CS_FLY_BUFF_EXTRA_ATTACK )
					pAttacker->SetAttackBuffGrapplingState( szName, BT_FLY_JUMP_EXTRA_ATTACK );
			}
			break;
		case GPT_GALFORD:
			{
				pAttacker->SetGrapplingType( (GrapplingType)iGrapplingType );
				if( pAttacker->GetState() == CS_GALFORD_CMD_STATE )
					pAttacker->SetAttackItemGrapplingState( szName );
			}
			break;
		case GPT_DRUID:
			pAttacker->SetGrapplingType( (GrapplingType)iGrapplingType );
			if( pAttacker->GetState() == CS_ATTACK )
				pAttacker->SetAttackGrapplingState( szName );
			break;
		case GPT_GUNNER:
			pAttacker->SetGrapplingType( (GrapplingType)iGrapplingType );
			if( pAttacker->GetState() == CS_CMD_STATE )
				pAttacker->SetAttackItemGrapplingState( szName );
			break;
		case GPT_WARP:
			{
				pAttacker->SetGrapplingType( (GrapplingType)iGrapplingType );
				if( pAttacker->GetState() == CS_WARP_SPECIAL )
					pAttacker->SetAttackItemGrapplingState( szName );
			}
			break;
		case GPT_CHANGE_ON_GRIFFIN:
			{
				pAttacker->SetGrapplingType( (GrapplingType)iGrapplingType );
				if( pAttacker->GetState() == CS_CHANGE_ON_GRIFFIN_FLY )
					pAttacker->SetAttackItemGrapplingState( szName );
			}
			break;
		}
	}

	if( bPushAttacker )
	{
		if( pAttacker->IsEnableAttackCancelState() && !pAttacker->IgnoreBlock() )
			pAttacker->SetEtcState( szReflectAni, fReflectAniRate );

		pAttacker->SetLastAttackerName( pChar->GetCharName().c_str(), szSkill );
		pAttacker->ReverseReflectPush( vAttackDir, fAttackPushPower );
			
		//Reflect 이펙트 고정 때문에 추가 (jch)
		if ( pChar->HasBuff( BT_REFLECT ) )
		{
			pChar->CreateReflectEffect();
		}
		else
		{
			CreateMapEffect( "sp2_auto_defense_explosion.txt",
				pChar->GetMidPositionByRate(),
				pChar->GetWorldScale() );
		}
	}
	//Refelct End

	ioHashString szFrontBackAttackerBuff;
	rkPacket >> szFrontBackAttackerBuff;
	if( !szFrontBackAttackerBuff.IsEmpty() )
	{
		pAttacker->AddNewBuff( szFrontBackAttackerBuff, "", "", NULL );
	}

	DWORD dwWeaponPeculiarity;
	float fChangeJumppingRate;
	int iLimitCnt;

	rkPacket >> dwWeaponPeculiarity;
	rkPacket >> half;
	fChangeJumppingRate = HALFToFloat(half);
	rkPacket >> byTemp;
	iLimitCnt = byTemp;

	int iActionStop;
	rkPacket >> byTemp;
	iActionStop = byTemp;

	int iShakeCamera = 0;
	rkPacket >> byTemp;
	iShakeCamera = byTemp;

	bool bBuffCreate, bTargetDie, bEnableHitCount;
	rkPacket >> bPack8;	
	bBuffCreate = ((bPack8 >> 2) & 1) == 1;
	bTargetDie = ((bPack8 >> 1) & 1) == 1;
	bEnableHitCount = (bPack8 & 1) == 1;

	if( !bBuffCreate )
		pChar->SetLastAttackerName( szAttacker, szSkill );

	if( bTargetDie )
	{
		pChar->OnWeaponDie( pChar->GetLastAttackerName(), pChar->GetLastAttackerSkillName() );
	}

	// 기여도
	if( pAttacker->GetTeam() != pChar->GetTeam() && bEnableHitCount )
	{
		pAttacker->CheckComboSkillHit();

		if( m_pPlayMode && m_pPlayMode->GetModeType() != MT_FOOTBALL )
		{
			g_AwardMgr.AddWounded( szName, FLOAT100 * m_pPlayMode->ContributeTeamPerA( pChar->GetTeam(), FLOAT1, 0.1f ) );
		}

		if( m_pPlayMode )
		{
			m_pPlayMode->AttackCntContribute( pAttacker->GetCharName(),
				pChar->GetCharName(),
				dwAttackerWeaponItemCode );
		}
		if( pAttacker->IsOwnerChar() )
		{
			// 팀 히트 제외
			if( pChar->GetTeam() != pAttacker->GetTeam() )
			{
				g_QuestMgr.QuestCompleteTerm( QC_HIT_ATTACK_ATTRIBUTE, iAttributeIndex );
			}
		}
	}

	if( fDamage > 0.0f && m_pPlayMode )
	{
		m_pPlayMode->DamageContribute( pAttacker->GetCharName(),
									   pChar->GetCharName(),
									   fDamage, dwAttackerWeaponItemCode, iWeaponType );
	}

	// 팀 히트 제외
	if( pChar->GetTeam() != pAttacker->GetTeam() )
	{
		if( m_pPlayMode && m_pPlayMode->GetModeType() != MT_FOOTBALL )
			g_AwardMgr.AddHit( pAttacker->GetCharName() );
	}

	if( pWeapon )
	{
		if( !pWeapon->GetCalcActionDelayPassType() )
		{
			switch( iActionStop )
			{
			case AST_BOTH:
				pAttacker->SetActionStopDelay( fDamage, bTargetDie, iShakeCamera );

				if( bApplyPushPower )
					pChar->SetActionStopDelay( fDamage, bTargetDie, iShakeCamera );
				break;
			case AST_ATTACKER:
				pAttacker->SetActionStopDelay( fDamage, bTargetDie, iShakeCamera );
				break;
			case AST_TARGET:
				if( bApplyPushPower )
					pChar->SetActionStopDelay( fDamage, bTargetDie, iShakeCamera );
				break;

				if( pWeapon )
				{
					pWeapon->SetActionStopDuration( pChar->CalcActionStopDelay( fDamage, bTargetDie ),
						pChar->GetActionStopLooseRate( bTargetDie ) );
				}
			}
		}
	}
	else
	{
		switch( iActionStop )
		{
		case AST_BOTH:
			pAttacker->SetActionStopDelay( fDamage, bTargetDie, iShakeCamera );

			if( bApplyPushPower )
				pChar->SetActionStopDelay( fDamage, bTargetDie, iShakeCamera );
			break;
		case AST_ATTACKER:
			pAttacker->SetActionStopDelay( fDamage, bTargetDie, iShakeCamera );
			break;
		case AST_TARGET:
			if( bApplyPushPower )
				pChar->SetActionStopDelay( fDamage, bTargetDie, iShakeCamera );
			break;

			if( pWeapon )
			{
				pWeapon->SetActionStopDuration( pChar->CalcActionStopDelay( fDamage, bTargetDie ),
					pChar->GetActionStopLooseRate( bTargetDie ) );
			}
		}
	}

	if( pAttr )
	{		
		float fLooseRate = 0.0f;
		DWORD dwStopDuration = 0;
		if( iActionStop != AST_NONE )
		{
			fLooseRate = ioBaseChar::GetActionStopLooseRate( bTargetDie );
			dwStopDuration = ioBaseChar::CalcActionStopDelay( fDamage, bTargetDie );
		}

		if( !pAttr->m_bNoExplosionEffectWithChar )
		{
			if( dwWeaponPeculiarity & WP_WOUND_NO_SOUND )
			{

				AddWeaponExplosionEffect( pChar,
										  pAttr->m_ExplosionEffect,
										  "",
										  vAttackDir,
										  dwStopDuration,
										  fLooseRate );
			}
			else
			{
				std::string szNewSound = pChar->GetWoundedSoundName(pAttr->m_WoundedSound);

				AddWeaponExplosionEffect( pChar,
										  pAttr->m_ExplosionEffect,
										  ioHashString(szNewSound.c_str()),
										  vAttackDir,
										  dwStopDuration,
										  fLooseRate );
			}
		}

		AddWeaponExtraExplosionEffect( pAttacker,
									   pChar,
									   pAttr->m_ExtraExplosionEffect,
									   vAttackDir,
									   dwStopDuration,
									   fLooseRate );
	}

	pAttacker->SetWarpTargetList( szAttacker, szName );
	if( pWeapon )
	{
		pWeapon->AddCollisionEntity( pChar );

		if( pWeapon->HasPeculiarity( WP_ONE_TIME_USE ) && pWeapon->GetPiercingType() == CPT_NONE )
		{	
			ioMissileWeapon* pMissile = ToMissileWeapon( pWeapon );
			if( pMissile )
			{
				if ( pMissile->isCallAttribute() )
					pMissile->CallAttribute( pChar );

				pMissile->SetCreateDummy( true );				
			}
			pWeapon->SetWeaponDead();
		}
		else if( pWeapon->CheckWeaponByBuff() )
		{			
			pWeapon->SetWeaponDead();
		}
	}

	if( pAttacker->IsOwnerChar() )
	{
		g_AbuseMgr.AddDamage( (int)fDamage );

		if( bChangeJumpping )
		{
			pAttacker->SetChangeJumppingState( fChangeJumppingRate, true, iLimitCnt );
		}
	

		//yunga
		if( pWeapon && pWeapon->IsChangeNextItemType( false ) )
		{
			ioAttackableItem* pItem = pAttacker->GetPriorityItem( SPT_ATTACK );
			if( pItem )
			{
				pItem->SetNextItemState( pChar, pAttacker, pWeapon, false, fDamage );				
	
				if( bISCreateBuff )
					pItem->WeaponWounded( pWeapon, pChar );
			}
		}
	}

	if( bNoChangeGrappling && 
		(iGrapplingType == GPT_NO_CHANGE || iGrapplingType == GPT_NO_CHANGE2 || iGrapplingType == GPT_NO_CHANGE3) )
	{
		pChar->SetNoChangeGrapplingState( iGrapplingType );
	}

	// for need target name skill
	if( pAttacker )
	{
		bool bBlock = true;
		if( eBlockType == ATB_NONE && eProtectionType == PM_NONE )
			bBlock = false;

		ioSkill *pSkill = pAttacker->GetCurActiveSkill();
		if( pSkill )
			pSkill->SetInfoByApplyWeapon( pAttacker, pChar, dwWeaponIndex, bBlock );

		if( pAttacker->GetTeam() != pChar->GetTeam() )
		{
			for( int i = 0; i < 4; i++ )
			{
				ioSkill *pSkill = pChar->GetEquipedSkill( i );
				ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pSkill );
				if( pPassiveSkill )
					pPassiveSkill->CheckReducePassiveSkillGrade( pChar, pAttacker );
			}
		}

		if( eBlockType == ATB_NONE && eProtectionType == PM_NONE )
		{
			if( bApplyPushPower && pAttr && pAttr->m_bEnablePossession && pChar->HasEnablePossessionBuff() && pChar->GetPossessionChar().IsEmpty() )
			{
				pAttacker->CheckEnablePossessionState( szName );
			}

			if( bApplyPushPower && fDamage > 0.0f )
			{
				pChar->CheckWoundPassiveSkill( pAttacker );

				for( int i=0; i < 4; i++ )
				{
					ioSkill *pSkill = NULL;
					pSkill = pAttacker->GetEquipedSkill( i );

					ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pSkill );
					if( pPassiveSkill && pPassiveSkill->UsePassiveGauge() )
					{
						if( pAttacker->HasBuff( BT_CALL_BUFF_ON_ATTACK ) )
						{
							ioHashStringVec vGaugeBuffName = pPassiveSkill->GetOwnerGaugeBuffName( pAttacker );
							if( !vGaugeBuffName.empty() )
							{
								int iBuffCnt = vGaugeBuffName.size();
								for( int j=0; j<iBuffCnt; ++j )
								{
									pPassiveSkill->CheckAttackProcess( pChar, pAttacker );
									pChar->AddNewBuff( vGaugeBuffName[j], pAttacker->GetCharName(), "", NULL );
								}
							}
						}
												
						if( iDefenseBreakType == DBT_CHANGE &&
							pPassiveSkill->CheckReducePassiveSkillGaugeBuff( pAttacker, BT_DEFENSE_BREAK_BUFF ))
						{
							pPassiveSkill->CheckAttackProcess( pChar, pAttacker );
						}
					}
				}

				if( pAttacker->HasBuff( BT_CALL_BUFF_ON_ATTACK2 ) )
				{
					ioCallBuffOnAttackBuff2 *pCallBuff2 = ToCallBuffOnAttackBuff2( pAttacker->GetBuff( BT_CALL_BUFF_ON_ATTACK2 ) );
					if( pCallBuff2 )
					{
						ioHashString vCallBuffName = pCallBuff2->GetCallBuffName();
						if( !vCallBuffName.IsEmpty() )
							pChar->AddNewBuff( vCallBuffName, pAttacker->GetCharName(), "", NULL );
					}
				}
			}
		}
	}

	// damagedefense effect
	if( (!bApplyPushPower || pChar->IsDamageProtect()) && fDamage > 0.0f && pAttacker && pAttacker->GetDamageDefenseBuffValue() > 0.0f )
	{
		ioHashString szValue;
		pAttacker->GetDamageDefenseBuffStringValue( szValue );
		if( !szValue.IsEmpty() )
		{
			pChar->AttachEffect( szValue );
		}
	}

	//썬더버드 
	if ( pChar->GetState() == CS_THUNDER_BIRD_HOLD )
	{
		ioThunderBirdHoldBuff *pThunderBirdHoldBuff = ToThunderBirdHoldBuff( pChar->GetBuff( BT_THUNDER_BIRD_HOLD ) );
		if ( pThunderBirdHoldBuff )
		{
			//같은 팀 일경우
			if ( pChar->GetTeam() == pAttacker->GetTeam() )
				pChar->CheckThunderBirdSpecialBuff( true );
			else
			{
				//썬더버드 아이템이 아니라면 버프에서 특정 공격 자동 발생
				ioThunderBirdItem *pThunderBirdItem = ToThunderBirdItem( pAttacker->GetWeapon() );
				if ( !pThunderBirdItem )
				{
					ioThunderBirdHoldBuff *pThunderBirdHoldBuff = ToThunderBirdHoldBuff( pChar->GetBuff( BT_THUNDER_BIRD_HOLD ) );
					if ( pThunderBirdHoldBuff && !pThunderBirdHoldBuff->GetIgnoreWeaponItem() )
						pChar->CheckThunderBirdSpecialBuff( false );
				}
			}
		}
		else
			pChar->SetState( CS_DELAY );
	}
}

void ioPlayStage::OnExtendDefense( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	float fCurHP;
	rkPacket >> fCurHP;
	pChar->SetCurHP( fCurHP );
	pChar->ApplyExtendDefenseState( rkPacket );
}

void ioPlayStage::OnDodgeDefense( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	D3DXQUATERNION qtCharRot;
	rkPacket >> qtCharRot;

	pChar->SetTargetRot( qtCharRot );
	pChar->ApplyDodgeDefenseState( rkPacket );
}

void ioPlayStage::OnPushStructMove( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar ) return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->SetState( CS_STRUCT_PUSH );

	D3DXQUATERNION qtRotate;
	rkPacket >> qtRotate;

	pChar->SetMoveDirByRotate( qtRotate );

	int iPushIndex;
	rkPacket >> iPushIndex;

	ioPushStruct *pPushStruct = g_PushStructListMgr.FindPushStruct( iPushIndex );
	if( !pPushStruct )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPushStructMove - %d PushStruct Not Exist", iPushIndex );
		return;
	}

	pChar->SetCurPushStructByNetwork( iPushIndex );

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pPushStruct->SetWorldPosition( vPos );
	pPushStruct->SetMoveState( pChar );
}

void ioPlayStage::OnPushStructDelay( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar ) return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->SetState( CS_DELAY );

	int iPushIndex;
	rkPacket >> iPushIndex;

	ioPushStruct *pPushStruct = g_PushStructListMgr.FindPushStruct( iPushIndex );
	if( !pPushStruct )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPushStructDelay - %d PushStruct Not Exist", iPushIndex );
		return;
	}

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;
	pPushStruct->SetWorldPosition( vPos );
	pPushStruct->SetDelayState();
}

void ioPlayStage::OnDropDieMotion( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnDropDieMotion() - %s is not Exist", szName.c_str() );
		return;
	}

	D3DXVECTOR3 vRecvPos;
	rkPacket >> vRecvPos;
	pChar->SetWorldPosition( vRecvPos );
	pChar->SetDieState();
}

void ioPlayStage::OnBlowGetUp( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->SetBlowWoundedGetUp();
}

void ioPlayStage::OnDropDownGetUp( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;
	
	D3DXVECTOR3 vNewPos;
	rkPacket >> vNewPos;

	pChar->SetBlowWoundedGetUp();
	pChar->SetWorldPosition( vNewPos );
	pChar->SetUserRankEmoticon();

#ifndef SHIPPING
	//패킷 로그 확인 (드랍존에서 부활 후)
	if ( g_MyInfo.GetPublicID() == "jch2305" )
	{
		g_RecvPacketLogManager.PrintCharRecvInfo( 0xFF00A1A1, szName, ioRecvPacketLogManager::RPT_Skill, "", true );
		g_RecvPacketLogManager.PrintCharRecvInfo( 0xFFFF00A1, szName, ioRecvPacketLogManager::RPT_Buff, "", true );
		g_RecvPacketLogManager.PrintCharRecvInfo( 0xFFFFFF00, szName, ioRecvPacketLogManager::RPT_Dummy, "", true );
		g_RecvPacketLogManager.PrintCharRecvInfo( 0xFFA1FF00, szName, ioRecvPacketLogManager::RPT_Special, "", true );
	}
#endif
}

void ioPlayStage::OnMineWaitExplosion( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnMineWaitExplosion - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;

	ioMineWeapon *pMine = ToMineWeapon( g_WeaponMgr.FindWeapon( pAttacker, dwWeaponIdx ) );
	ioMineWeapon6 *pMine6 = ToMineWeapon6( g_WeaponMgr.FindWeapon( pAttacker, dwWeaponIdx ) );
	if( pMine )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		pMine->SetPosition( vPos );
		pMine->SetWaitExplosionMine( this, false );
	}
	else if( pMine6 )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		pMine6->SetPosition( vPos );
		pMine6->SetWaitExplosionMine( this, false );
	}
	else
	{
		g_WeaponMgr.RegisterMissingMineIndex( dwWeaponIdx );
	}
}

void ioPlayStage::OnNormalDeadMine( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnNormalDeadMine - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;

	ioMineWeapon *pMine = ToMineWeapon( g_WeaponMgr.FindWeapon( pAttacker, dwWeaponIdx ) );
	if( pMine )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		pMine->SetPosition( vPos );
		pMine->SetNormalDeadMine( this, false );
	}
	else
	{
		g_WeaponMgr.RegisterMissingMineIndex( dwWeaponIdx );
	}
}

void ioPlayStage::OnMineExplosion( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	int iWeaponIdx;
	rkPacket >> szAttacker >> iWeaponIdx;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnMineExplosion - Attacker %s Not Exist",
								szAttacker.c_str() );
		return;
	}

	ioMineWeapon *pMine = ToMineWeapon( g_WeaponMgr.FindWeapon( pAttacker, iWeaponIdx ) );
	if( pMine )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		pMine->SetPosition( vPos );
		pMine->ExplosionMine( this );
	}
	else
	{
		g_WeaponMgr.RegisterMissingMineIndex( iWeaponIdx );
	}
}

void ioPlayStage::OnPassage( SP2Packet &rkPacket )
{
	ioHashString szTarget, szSender;
	rkPacket >> szTarget >> szSender;

	if( szTarget != g_MyInfo.GetPublicID() )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPassage - %s Not Me", szTarget.c_str() );
		return;
	}

	int iSubType;
	rkPacket >> iSubType;
	if( iSubType == PAS_MODE_SPECIAL_INFO )
	{
		m_pAreaWeaponMgr->ApplyAreaWeaponInfo( rkPacket );
		m_pPushStructMgr->ApplyPushStructInfo( rkPacket );
		m_pBallMgr->ApplyBallStructInfo( rkPacket );

		if( m_pPlayMode )
		{
			m_pPlayMode->OnModeSpecialInfo( rkPacket );
		}

		rkPacket >> m_iRedForBalance;
		rkPacket >> m_iBlueForBalance;

		m_pWorldEntityFactory->EnableElevationSync( true );
	}
	else if( iSubType == PAS_ELEVATOR_INFO )
	{
		if( m_pWorldEntityFactory )
		{
			m_pWorldEntityFactory->ApplyElevatorStateInfo( rkPacket );
		}
	}
	else
	{
		ioBaseChar *pChar = GetBaseChar( szSender );
		if( !pChar )
		{
			LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPassage - Sender %s Not Exist", szSender.c_str() );
			return;
		}

		switch( iSubType )
		{
		case PAS_MINE_INFO:
			pChar->SetMineInfo( rkPacket );
			break;
		case PAS_BUFF_INFO:
			pChar->ReceiveBuffList( rkPacket );
			break;
		case PAS_ITEM_GAUGE_INFO:
			pChar->ReceiveItemGauge( rkPacket );
			break;
		case PAS_ATTACH_WEAPON_INFO:
			pChar->SetMapAttachInfo( rkPacket );
			break;
		case PAS_DUMMY_CHAR_INFO:
			pChar->SetDummyCharInfo( rkPacket );
			break;
		case PAS_WEREWOLF_INFO:
			pChar->SetWereWolfStateInfo( rkPacket );
			break;
		case PAS_YUMIRA_INFO:
			pChar->SetGPStateInfo( rkPacket );
			break;
		case PAS_SUDDEN_ATTACK_INFO:
			pChar->SetSuddenAttackStateInfo( rkPacket );
			break;
		case PAS_TOPBL_EXTEND_MODE:
			pChar->SetTopBlExtendStateInfo( rkPacket );
			break;
		case PAS_SUPER_DIMENSION:
			pChar->SetSuperDimensionWeaponInfo( rkPacket );
			break;
		case PAS_GALAXY:
			pChar->SetGalaxyInfo( rkPacket );
			break;
		case PAS_THUNDER_BIRD:
			pChar->SetThunderBirdInfo( rkPacket );
			break;
		case PAS_ALICE:
			pChar->SetAliceInfo( rkPacket );
			break;
		case PAS_PARKILPYO:
			pChar->SetParkilpyoInfo( rkPacket );
			break;
		case PAS_DARKKNIGHT:
			{
				ioDarkKnightItem* pDarkKnight = ToDarkKnightItem( pChar->GetWeapon() );
				if( pDarkKnight )
					pDarkKnight->SetInfo( pChar, rkPacket );
			}
			break;
		case PAS_DUMMY_MID_SYNC:
			g_DummyCharMgr.ApplyDummyCharMidSync( pChar, rkPacket );
			break;
		case PAS_GROUP_DUMMY:
			g_DummyCharMgr.ApplyDummyGroupInfo( pChar, rkPacket );
			break;
		case PAS_GEOMJE:
			{
				ioGeomjeItem* pGeomJe = ToGeomjeItem( pChar->GetWeapon() );
				if( pGeomJe )
					pGeomJe->SetInfo( pChar, rkPacket );
			}
			break;
		case PAS_RESSEARCHER:
			{
				ioResearcher* pResearcher = ToResearcher( pChar->GetWeapon() );
				if( pResearcher )
					pResearcher->SetInfo( pChar, rkPacket );
			}
			break;
		case PAS_SPECIAL_STATE:
			{
				int eState;
				rkPacket >> eState;
				pChar->SetState( eState );
				ioSpecialStateBase* pBase = pChar->GetSpecialState();
				if( pBase )
					pBase->ApplyJoinSync( rkPacket, pChar );
			}
			break;
		case PAS_ONIMUSHA:
			pChar->SetOnimushaInfo( rkPacket );
			break;
		case PAS_DRUID:
			pChar->SetDruidInfo( rkPacket );
			break;
		case PAS_MIDNIGHT_SCYTHE:
			pChar->SetMidnightScytheInfo( rkPacket );
			break;
		case PAS_MIDNIGHT_GUN:
			pChar->SetMidnightGunInfo( rkPacket );
			break;

		case PAS_DUMMY_INDEX_INFO:
			int iIndex;
			rkPacket >> iIndex;
			g_DummyCharMgr.SetCurLocalIndex( iIndex );
			break;

		case PAS_EVA_SUB_WEAPON:
			pChar->SetEvaSubWeaponInfoList( rkPacket );
			break;

		case PAS_MAGNETIC_WEAPON:	// 마그네틱 관전 OR 입장 관련 변수 2020-12-16
			pChar->SetMagneticWeaponInfoList( rkPacket );
			break;
		}
	}
}

void ioPlayStage::OnCounterAttack( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;
	pChar->SetWeaponIndexBase( dwWeaponIdx );

	float fCurHP;
	rkPacket >> fCurHP;

	int iKeyType;
	rkPacket >> iKeyType;

	pChar->SetCurHP( fCurHP );
	pChar->SetCounterAttack( (CharCounterAttackKeyState)iKeyType);
}

void ioPlayStage::OnItemGauge( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )	return;
	if( !pAttacker->CheckGrapplingExceptionState() )
		return;

   	pAttacker->ApplyItemGauge( rkPacket );
}

void ioPlayStage::OnMagicCircle( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )	return;
	if( pAttacker->GetState() == CS_DIE )
		return;

	pAttacker->ApplyMagicCircle( rkPacket );
}

void ioPlayStage::OnSystemDamage( SP2Packet &rkPacket )
{
	ioHashString szDamagedChar;
	rkPacket >> szDamagedChar;

	ioBaseChar *pDamagedChar = GetBaseChar( szDamagedChar );
	if( !pDamagedChar )	return;
	if( pDamagedChar->GetState() == CS_DIE )
		return;

	int iDamageType = 0;
	rkPacket >> iDamageType;

	float fCurHP, fDropDamage;
	rkPacket >> fCurHP >> fDropDamage;

	pDamagedChar->ApplyDamage( fDropDamage, true );
	pDamagedChar->SetCurHP( fCurHP );

	ioHashString szLastAttacker;
	DWORD dwLastAttackerWeaponCode;
	rkPacket >> szLastAttacker >> dwLastAttackerWeaponCode;

	if( iDamageType == SYSTEM_DROP_DAMAGE )
	{
		pDamagedChar->SetDropZoneDownState();

		D3DXVECTOR3 vRecvPos;
		rkPacket >> vRecvPos;
		pDamagedChar->SetWorldPosition( vRecvPos );

		if( !szLastAttacker.IsEmpty() )
		{
			ioBaseChar *pAttacker = GetBaseChar( szLastAttacker );
			if( pAttacker && pAttacker->IsOwnerChar() )
			{
				g_ShakeCameraMgr.SetShakeCamera( ioBaseChar::m_iDroppedShakeCamera, ShakeCamera::SP_HIGH );
			}
		}
	}
	else if( iDamageType == SYSTEM_FIRE_DAMAGE )
	{
		BlowWoundAniInfo kInfo;
		pDamagedChar->SetBlowWoundedState( false, kInfo );

		bool bResistantBuff = false;
		rkPacket >> bResistantBuff;

		ioHashString szBuff;
		rkPacket >> szBuff;

		if( !bResistantBuff && !szBuff.IsEmpty() )
		{
			pDamagedChar->AddNewBuff( szBuff, "", "", NULL );
		}
	}

	m_pPlayMode->DamageContribute( szLastAttacker, szDamagedChar, fDropDamage, dwLastAttackerWeaponCode );
	m_pPlayMode->NotifyCharDrop( pDamagedChar, fDropDamage );
}

void ioPlayStage::OnExplosionOnHand( SP2Packet &rkPacket )
{
	ioHashString szOwner;
	rkPacket >> szOwner;

	ioBaseChar *pChar = GetBaseChar( szOwner );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	int iWeaponIdx;
	rkPacket >> iWeaponIdx;
	pChar->SetWeaponIndexBase( iWeaponIdx );
	pChar->CheckExplosionOnHand( true );

	float fCurHP;
	rkPacket >> fCurHP;
	pChar->SetCurHP( fCurHP );

	// weaponindex 맞춰주기
	pChar->IncreaseWeaponIndexBase();
}

void ioPlayStage::OnExplosionDrop( SP2Packet &rkPacket )
{
	ioHashString szOwner;
	rkPacket >> szOwner;
	
	ioBaseChar *pChar = GetBaseChar( szOwner );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;
	
	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;
	pChar->SetWeaponIndexBase( dwWeaponIdx );

	float fCurHP;
	rkPacket >> fCurHP;
	pChar->SetCurHP( fCurHP );

	D3DXVECTOR3 vDropDir;
	rkPacket >> vDropDir;
	pChar->CheckExplosionItemWoundedDrop( vDropDir, true );
}

void ioPlayStage::OnFallDamage( SP2Packet &rkPacket )
{
	ioHashString szFallChar;
	rkPacket >> szFallChar;

	ioBaseChar *pFallChar = GetBaseChar( szFallChar );
	if( !pFallChar )	return;
	if( pFallChar->GetState() == CS_DIE )
		return;

	m_pPlayMode->NotifyCharFall(pFallChar);	

	D3DXVECTOR3 vFallPos;
	rkPacket >> vFallPos;

	float fCurHP;
	rkPacket >> fCurHP;

	pFallChar->SetWorldPosition( vFallPos );
	pFallChar->ApplyFallDamage( true );

	pFallChar->SetCurHP( fCurHP );
}

void ioPlayStage::OnRequestControl( SP2Packet &rkPacket )
{
	ioHashString szRequester;
	rkPacket >> szRequester;

	ioBaseChar *pRequest = GetBaseChar( szRequester );
	if( !pRequest )	return;

	ioBaseChar *pOwner = GetOwnerChar();
	if( pOwner )
	{
		pOwner->SendControlInfoToUser( szRequester );
	}
}

void ioPlayStage::OnChangeDashRun( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;

	if( !pChar->CheckGrapplingExceptionState() )
		return;

	pChar->ApplyChangeDashRun( rkPacket );
}

void ioPlayStage::OnReturnAttackExplicit( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	WeaponInfo kInfo;
	rkPacket >> kInfo.m_iWeaponIdx;
	rkPacket >> kInfo.m_iResistanceIdx;
	rkPacket >> kInfo.m_WoundedAnimation;
	rkPacket >> kInfo.m_dwWoundedDuration;
	rkPacket >> kInfo.m_bWoundedLoopAni;

	DWORD dwWeaponIndex;
	ioHashString szTarget;
	D3DXVECTOR3 vFirePos;
	rkPacket >> szTarget >> dwWeaponIndex >> vFirePos;

	pChar->SetWeaponIndexBase( dwWeaponIndex );
	pChar->IncreaseWeaponIndexBase();

	pChar->ReturnAttackExplicit( kInfo, dwWeaponIndex, vFirePos, szTarget );
}

void ioPlayStage::OnDummyExplicit( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;

	WeaponInfo kInfo;
	rkPacket >> kInfo.m_iWeaponIdx;
	rkPacket >> kInfo.m_iResistanceIdx;
	rkPacket >> kInfo.m_WoundedAnimation;
	rkPacket >> kInfo.m_dwWoundedDuration;
	rkPacket >> kInfo.m_bWoundedLoopAni;

	DWORD dwWeaponIndex;
	D3DXVECTOR3 vFirePos;
	rkPacket >> dwWeaponIndex >> vFirePos;

	ioHashString szCharName, szBuffName;
	rkPacket >> szCharName >> szBuffName;

	bool bUseWeaponCol;
	rkPacket >> bUseWeaponCol;

	pChar->SetWeaponIndexBase( dwWeaponIndex - 1 );
	pChar->DummyExplicit( kInfo, vFirePos, dwWeaponIndex, false, szCharName, szBuffName, bUseWeaponCol );

	// weaponindex 맞춰주기
	pChar->IncreaseWeaponIndexBase();
}

void ioPlayStage::OnChainCollision( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;

	int iWeaponIndex, iNext;
	D3DXVECTOR3 vPos, vDir;
	rkPacket >> iWeaponIndex >> iNext;
	rkPacket >> vPos >> vDir;

	if( !pChar->ApplyChainWeaponChange( iWeaponIndex, iNext, vPos, vDir ) )
		return;

	// 참여한 다른 유저들에게 전달
	SP2Packet kPacket( CUPK_CHAIN_CHANGE );
	kPacket << szName;
	kPacket << iWeaponIndex;
	kPacket << iNext;
	kPacket << vPos;
	kPacket << vDir;
	P2PNetwork::SendToAllPlayingUser( kPacket );
}

void ioPlayStage::OnChainChange( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;

	int iWeaponIndex, iNext;
	D3DXVECTOR3 vPos, vDir;
	rkPacket >> iWeaponIndex >> iNext;
	rkPacket >> vPos >> vDir;

	if( !pChar->ApplyChainWeaponChange( iWeaponIndex, iNext, vPos, vDir ) )
		return;
}

void ioPlayStage::OnBallWeaponCollision( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;

	int iWeaponIndex, iNext;
	D3DXVECTOR3 vPos, vMoveDir, vAttackDir;
	rkPacket >> iWeaponIndex >> iNext;
	rkPacket >> vPos >> vMoveDir >> vAttackDir;

	if( !pChar->ApplyBallWeaponChange( iWeaponIndex, iNext, vPos, vMoveDir, vAttackDir ) )
		return;

	// 참여한 다른 유저들에게 전달
	SP2Packet kPacket( CUPK_BALL_WEAPON_CHANGE );
	kPacket << szName;
	kPacket << iWeaponIndex;
	kPacket << iNext;
	kPacket << vPos;
	kPacket << vMoveDir;
	kPacket << vAttackDir;
	P2PNetwork::SendToAllPlayingUser( kPacket );
}

void ioPlayStage::OnBallWeaponChange( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;

	int iWeaponIndex, iNext;
	D3DXVECTOR3 vPos, vMoveDir, vAttackDir;
	rkPacket >> iWeaponIndex >> iNext;
	rkPacket >> vPos >> vMoveDir >> vAttackDir;

	if( !pChar->ApplyBallWeaponChange( iWeaponIndex, iNext, vPos, vMoveDir, vAttackDir ) )
		return;
}

void ioPlayStage::OnPushVoiceData( SP2Packet &rkPacket )
{
	ioHashString szName;
	VOICEDATA    vd;

	rkPacket >> szName;
	rkPacket >> vd;

	VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
	if(pWnd && pWnd->IsDisable(szName) ) return;

	g_VoiceChat.PushData( szName.c_str(), vd );
}

void ioPlayStage::OnVoiceInfo(SP2Packet &rkPacket )
{
	int iType;
	ioHashString szID;
	int iTeamType = 0;

	rkPacket >> iType;
	rkPacket >> szID;
	if( g_BattleRoomMgr.IsBattleRoom() )
		rkPacket >> iTeamType;

	ioHashStringVC szTemp = szID.c_str();

	// team check
	if( m_eVoiceChatType == VCT_TEAM   &&
		g_BattleRoomMgr.IsBattleRoom() &&
		g_BattleRoomMgr.IsBattleModePlaying() &&
		GetModeType() != MT_MONSTER_SURVIVAL ) // 해골영웅은 유저끼리 무조건 대화 가능
	{
		if( g_BattleRoomMgr.GetBattleUserTeam( g_MyInfo.GetPublicID() ) != (TeamType) iTeamType ||
			GetModeType() == MT_SURVIVAL ) // 데스매치 모드는 배틀팀이 이전 모드와 같아서 무조건 return;
		{
#ifndef SHIPPING
			LOG.PrintTimeAndLog( 0, "%s | SURVIVAL RETURN | %d | %s | %d", __FUNCTION__, iType, szID.c_str(), iTeamType  );
#endif
			return;
		}
	}

	// 전투룸 enter / exit event
	if( iType == ID_VOICE_REMOVE_TABLE )
	{
		if( !g_BattleRoomMgr.IsBattleRoom() && !g_LadderTeamMgr.IsLadderTeam() )
		{
			g_App.RemoveNetworkTable( szID, P2P_STYLE_VOICE );
#ifndef SHIPPING
			LOG.PrintTimeAndLog( 0, "%s | ID_VOICE_REMOVE_TABLE | %d | %s | %d", __FUNCTION__, iType, szID.c_str(), iTeamType  );
#endif
		}
	}
	else if( iType == ID_VOICE_ON_ADD_TABLE_REQUEST || 
		     iType == ID_VOICE_OFF_ADD_TABLE_REQUEST )
	{
		if( !g_BattleRoomMgr.IsBattleRoom() && !g_LadderTeamMgr.IsLadderTeam() )
		{
			g_App.AddNetworkTableChangeStyle( szID, P2P_STYLE_VOICE );	

			SP2Packet kPacket( CTPK_VOICE_INFO_USER );
			int iReturnMsg = ID_VOICE_OFF_ADD_TABLE;
			if ( Setting::UseVoiceChat() && Setting::UseMIC() )
				iReturnMsg = ID_VOICE_ON_ADD_TABLE;
			kPacket << iReturnMsg;
			kPacket << szID;
			TCPNetwork::SendToServer( kPacket );
#ifndef SHIPPING
			LOG.PrintTimeAndLog( 0, "%s | CTPK_VOICE_INFO_USER | RE:%d | %d | %s | %d", __FUNCTION__, iReturnMsg, iType, szID.c_str(), iTeamType );
#endif
		}
	}
	else if( iType == ID_VOICE_ON_ADD_TABLE ||
		     iType == ID_VOICE_OFF_ADD_TABLE )
	{
		g_App.AddNetworkTableChangeStyle( szID, P2P_STYLE_VOICE );	
#ifndef SHIPPING
		LOG.PrintTimeAndLog( 0, "%s | Change Style  P2P_STYLE_VOICE | %d | %s | %d", __FUNCTION__, iType, szID.c_str(), iTeamType );
#endif
	}

	if(iType == ID_VOICE_ON || 
	   iType == ID_VOICE_ON_ADD_TABLE_REQUEST || 
	   iType == ID_VOICE_ON_ADD_TABLE )
	{
		if(!g_VoiceChat.IsSoundPlayer(szTemp))
			g_VoiceChat.AddSoundPlayer(szTemp);

		bool bVoiceOn = true;
		VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
		if(pWnd)
		{
			pWnd->SetVoiceOn(szID);
			if(!Setting::UseOtherVoiceChat() || g_VoiceDisableListManager.IsVoiceDisable( szID ))
			{
				pWnd->SetVoiceDisable(szID);
				bVoiceOn = false;
			}
				
		}

		g_VoiceChat.Stop(szTemp);
		g_VoiceChat.Reset(szTemp);
		g_VoiceOffListManager.Delete( szID );

#ifndef SHIPPING
		LOG.PrintTimeAndLog( 0, "%s | ID_VOICE_ON ETC | %d | %d | %s | %d", __FUNCTION__, (int)bVoiceOn, iType, szID.c_str(), iTeamType );
#endif
	}
	else if( iType == ID_VOICE_OFF || 
		     iType == ID_VOICE_REMOVE_TABLE )
	{
		VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
		if(pWnd)
		{
			pWnd->SetVoiceOff(szID);
		}
		g_VoiceChat.RemoveSoundPlayer( szTemp );
		g_VoiceOffListManager.Add( szID );

#ifndef SHIPPING
		LOG.PrintTimeAndLog( 0, "%s | ID_VOICE_OFF OR ID_VOICE_REMOVE_TABLE | %d | %s | %d", __FUNCTION__, iType, szID.c_str(), iTeamType );
#endif
	}
	else if(iType == ID_VOICE_REFUSE)
	{
		g_VoiceChat.SetRefuse(szTemp, true);
#ifndef SHIPPING
		LOG.PrintTimeAndLog( 0, "%s | ID_VOICE_REFUSE | %d | %s | %d", __FUNCTION__, iType, szID.c_str(), iTeamType );
#endif
	}
	else if(iType == ID_VOICE_PERMIT)
	{
		g_VoiceChat.SetRefuse(szTemp, false);
#ifndef SHIPPING
		LOG.PrintTimeAndLog( 0, "%s | ID_VOICE_PERMIT | %d | %s | %d", __FUNCTION__, iType, szID.c_str(), iTeamType );
#endif
	}
}

void ioPlayStage::OnSkillExtraInfo( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;

	ioHashString szSkillName;
	rkPacket >> szSkillName;

	pChar->ApplySkillExtraInfo( szSkillName, rkPacket );

#ifndef SHIPPING
	//패킷 로그 확인 (스킬)
	if ( g_MyInfo.GetPublicID() == "jch2305" )
	{
		g_RecvPacketLogManager.AddCharRecvInfo( szName, ioRecvPacketLogManager::RPT_Skill, szSkillName );
	}
#endif
}

void ioPlayStage::OnEquipSkillExtraInfo( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	
		return;

	if( !pChar->CheckGrapplingExceptionState() )
		return;

	ioHashString szSkillName;
	rkPacket >> szSkillName;

	pChar->ApplyEquipSlotSkillExtraInfo( szSkillName, rkPacket );

#ifndef SHIPPING
	//패킷 로그 확인 (스킬)
	if ( g_MyInfo.GetPublicID() == "jch2305" )
	{
		g_RecvPacketLogManager.AddCharRecvInfo( szName, ioRecvPacketLogManager::RPT_Skill, szSkillName );
	}
#endif
}

void ioPlayStage::OnFrozenRelease( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->SetCurFrozenState( false );
}

void ioPlayStage::OnFloatRelease( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	BlowWoundAniInfo kInfo;
	pChar->SetBlowWoundedState( true, kInfo );
}

void ioPlayStage::OnFallState( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	bool bChange;
	rkPacket >> bChange;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	bool bBugCheck;
	rkPacket >> bBugCheck;

	if( bBugCheck )
	{
		bool bCheck1, bCheck2;
		float fCheck1, fCheck2;
		rkPacket >> bCheck1 >> bCheck2;
		rkPacket >> fCheck1 >> fCheck2;
	}

	pChar->SetFallState( bChange );
}

void ioPlayStage::OnCloseOrderRelease( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->SetState( CS_DELAY );
}

void ioPlayStage::OnEndBuff( SP2Packet &rkPacket )
{
	ioHashString szName, szBuffName;
	int iBuffCnt;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;

	if( pChar->GetState() == CS_DIE )
		return;

	bool bUseObjectItem;
	rkPacket >> bUseObjectItem;

	if( bUseObjectItem )
	{
		int iObjectItem;
		rkPacket >> iObjectItem;

		pChar->ObjectItemUse( (DWORD)iObjectItem, pChar->GetState() );
	}

	rkPacket >> iBuffCnt;

	for( int i=0; i < iBuffCnt; ++i )
	{
		rkPacket >> szBuffName;

		pChar->ApplyEndBuff( szBuffName );
	}
}

void ioPlayStage::OnEndArea( SP2Packet &rkPacket )
{
	ioHashString szOwnerName, szSkillName;

	rkPacket >> szOwnerName;

	ioBaseChar *pOwner = GetBaseChar( szOwnerName );
	if( !pOwner ) return;

	rkPacket >> szSkillName;

	ioSkill *pSkill = NULL;
	if( !szSkillName.IsEmpty() )
	{
		for( int k=0; k < ES_OBJECT; k++ )
		{
			pSkill = pOwner->GetEquipedSkill( k );
			if( !pSkill ) continue;

			if( pSkill->GetName() == szSkillName )
				break;

			pSkill = NULL;
		}

		ioBuffSkill *pBuffSkill = ToBuffSkill( pSkill );
		if( pBuffSkill )
		{
			pBuffSkill->RemoveAreaWeapon( pOwner );
		}
	}
}

void ioPlayStage::OnChangeCharInfo( SP2Packet &rkPacket )
{
	DWORD dwCharIndex;
	CHARACTER kCharInfo;
	bool bSpecificUserItemSync = false;
	rkPacket >> dwCharIndex >> kCharInfo;

	ITEM_DATA kItemData[MAX_INVENTORY];
	for(int i = 0;i < MAX_INVENTORY;i++)
	{
		rkPacket >> kItemData[i].m_item_code >> kItemData[i].m_item_male_custom >> kItemData[i].m_item_female_custom;
	}

	ReLoadCharInfo( dwCharIndex, kCharInfo );

	g_GlobalSoldierInfo.SyncSoldierData( dwCharIndex, kCharInfo );
	g_GlobalSoldierInfo.SyncSoldierItem( dwCharIndex, (const ITEM_DATA*)&kItemData );
}

void ioPlayStage::OnRemoveBuff( SP2Packet &rkPacket )
{
	ioHashString szTargetName;
	rkPacket >> szTargetName;

	ioBaseChar *pChar = GetBaseChar( szTargetName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	int iCnt;
	rkPacket >> iCnt;

	for( int i=0; i<iCnt; i++ )
	{
		ioHashString szBuffName;
		rkPacket >> szBuffName;
		
		if( !szBuffName.IsEmpty() )
		{
			pChar->RemoveBuff( szBuffName );
		}
	}
}

void ioPlayStage::OnPreventTargetBuff( SP2Packet &rkPacket )
{
	ioHashString szTargetName, szBuffName;
	rkPacket >> szTargetName;

	ioBaseChar *pChar = GetBaseChar( szTargetName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	rkPacket >> szBuffName;

	ioBuff* pBuff = pChar->GetBuff( szBuffName );
	if( pBuff )
		pBuff->PlayPassiveBuffEffect();
}

void ioPlayStage::OnAddBuffBySkill( SP2Packet &rkPacket )
{
	ioHashString szCreator, szTargetName, szItemName;
	rkPacket >> szTargetName >> szCreator >> szItemName;

	ioBaseChar *pChar = GetBaseChar( szTargetName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;

	ioBaseChar *pCreator = GetBaseChar( szCreator );

	DWORD dwSeed;
	rkPacket >> dwSeed;

	pChar->SetRandomSeed( dwSeed );

	bool bAttackCnt;
	rkPacket >> bAttackCnt;

	float fCoolValue;
	float fDurationValue;
	float fTicValue;

	rkPacket >> fCoolValue >> fDurationValue >> fTicValue;

	DWORD dwCheckCloseOrderBuff;
	rkPacket >> dwCheckCloseOrderBuff;

	ioHashString szWeaponOwner;
	ioHashString szAreaWeaponName;
	DWORD dwWeaponIdx;
	DWORD dwCreateItemCode = 0;

	switch( dwCheckCloseOrderBuff )
	{
	case COWT_AREAWEAPON:
		{
			rkPacket >> szWeaponOwner;
			rkPacket >> szAreaWeaponName;
		}
		break;
	case COWT_WEAPON:
		{
			rkPacket >> szWeaponOwner;
			rkPacket >> dwWeaponIdx;
		}
		break;
	case COWT_BUFFSKILL_OWNER:
		{
			rkPacket >> dwCreateItemCode;
		}
		break;
	}

	int iBuffCount;
	rkPacket >> iBuffCount;

	ioHashString szBuffName;
	for( int i=0; i<iBuffCount; i++ )
	{
		rkPacket >> szBuffName;

		if( !szBuffName.IsEmpty() )
		{
			ioBuff *pBuff = pChar->AddNewBuffWithAnimateTime( szBuffName, szCreator, szItemName, NULL, 0 );
			if( pBuff )
			{
				//Area무기의 인덱스 얻어서 버프에 넣기
				if ( dwCheckCloseOrderBuff == COWT_AREAWEAPON )
				{
					ioAreaWeapon *pArea = g_AreaWeaponMgr.FindAreaWeaponByName( szAreaWeaponName, szWeaponOwner );
					if( pArea )
					{
						DWORD dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();
						pBuff->SetAreaWeaponIndex( dwAreaWeaponIndex );
					}
				}

				// 쿨타임 성장요소 관련
				pBuff->SetTicGaugeRateByGrowth( fCoolValue );

				// 유지시간 성장요소 관련
				pBuff->SetExtraDurationByGrowth( fDurationValue );

				// 틱 성장요소 관련
				pBuff->SetTicTimeByGrowth( fTicValue );

				if( dwCheckCloseOrderBuff == COWT_AREAWEAPON )
				{
					ioCloseOrderBuff *pCloseOrder = ToCloseOrderBuff(pBuff);
					if( pCreator && pCloseOrder && pCloseOrder->GetCloseOrderType() == ioCloseOrderBuff::COT_AREA_TRACE )
					{
						ioAreaWeapon *pArea = g_AreaWeaponMgr.FindAreaWeaponByName( szAreaWeaponName, szWeaponOwner );
						if( pArea )
						{
							DWORD dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();
							pCloseOrder->SetTargetAreaWeapon( dwAreaWeaponIndex );
						}
					}

					ioPullBuff *pPullBuff = ToPullBuff(pBuff);
					if( pCreator && pPullBuff )
					{
						ioAreaWeapon *pArea = g_AreaWeaponMgr.FindAreaWeaponByName( szAreaWeaponName, szWeaponOwner );
						if( pArea )
						{
							DWORD dwAreaWeaponIndex = pArea->GetAreaWeaponIndex();
							pPullBuff->SetTargetAreaWeapon( dwAreaWeaponIndex );
						}
					}
				}
				else if( dwCheckCloseOrderBuff == COWT_WEAPON )
				{
					ioCloseOrderBuff *pCloseOrder = ToCloseOrderBuff(pBuff);
					if( pCreator && pCloseOrder && pCloseOrder->GetCloseOrderType() == ioCloseOrderBuff::COT_AREA_TRACE )
					{
						ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( GetBaseChar(szWeaponOwner), dwWeaponIdx );
						if( pWeapon )
						{
							pCloseOrder->SetTargetWeapon( dwWeaponIdx );
						}
					}
				}
				else if( dwCheckCloseOrderBuff == COWT_BUFFSKILL_OWNER )
				{
					if( dwCreateItemCode > 0 )
						pBuff->SetItemCode( dwCreateItemCode );
				}
			}
		}
	}

	bool bUseForce;
	rkPacket >> bUseForce;

	if( bUseForce )
	{
		float fForceAmt, fForceFriction;
		D3DXVECTOR3 vDir;
		rkPacket >> fForceAmt >> fForceFriction >> vDir;

		if( fForceAmt > 0.0f )
		{
			D3DXQUATERNION qtCharRot;
			ioMath::CalcDirectionQuaternion( qtCharRot, vDir );
			pChar->SetTargetRot( qtCharRot );
			pChar->SetForcePower( vDir, fForceAmt, fForceFriction, true );
		}

		D3DXVECTOR3 vEffectPos;
		ioHashString szEffectName;
		rkPacket >> vEffectPos >> szEffectName;

		if( !szEffectName.IsEmpty() )
		{
			D3DXVECTOR3 vScale( FLOAT1, FLOAT1, FLOAT1 );
			CreateMapEffect( szEffectName, vEffectPos, vScale );
		}
	}

	// Owner 가 NULL 이라도 위에것 까지는 처리해야함
	ioBaseChar *pOwner = GetBaseChar( szCreator );
	if( !pOwner ) return;

	if( pChar->GetTeam() != pOwner->GetTeam() )
	{
		if( bAttackCnt )
		{
			pOwner->CheckComboSkillHit();
			pOwner->CheckAttackCnt( pChar );
		}
	}
	else if( pChar->GetTeam() == pOwner->GetTeam() )
	{
		if( bAttackCnt )
			pOwner->CheckTeamBuffAttackCnt( pChar );
	}
}

void ioPlayStage::OnElevatorChanged( SP2Packet &rkPacket )
{
	if( m_pWorldEntityFactory )
	{
		m_pWorldEntityFactory->OnElevatorChanged( rkPacket );
	}
}

void ioPlayStage::OnCloseOrderDirection( SP2Packet &rkPacket )
{
	ioHashString szOwnerName;
	rkPacket >> szOwnerName;

	ioBaseChar *pChar = GetBaseChar( szOwnerName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	float fCurAngle;
	rkPacket >> fCurAngle;

	pChar->SetCloseOrderDir( fCurAngle, true );
}

void ioPlayStage::OnBreakFallState( SP2Packet &rkPacket )
{
	ioHashString szOwnerName;
	rkPacket >> szOwnerName;

	ioBaseChar *pChar = GetBaseChar( szOwnerName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	ioHashString szBuffName;
	rkPacket >> szBuffName;

	D3DXQUATERNION qtRot;
	rkPacket >> qtRot;

	pChar->SetTargetRotToRotate( qtRot, false );
	pChar->SetBreakFallState( szBuffName );
}

void ioPlayStage::OnSkillEnd( SP2Packet &rkPacket )
{
	ioHashString szName, szBuffName;
	int iNextState;
	rkPacket >> szName;
	rkPacket >> iNextState;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	ioHashString szSkillEndBuff;
	rkPacket >> szSkillEndBuff;

#ifndef SHIPPING
	//패킷 로그 확인 (스킬 종료 후)
	if ( g_MyInfo.GetPublicID() == "jch2305" && pChar->GetCurActiveSkill() )
	{
		g_RecvPacketLogManager.PrintCharRecvInfo( 0xFF00A1A1, szName, ioRecvPacketLogManager::RPT_Skill, pChar->GetCurActiveSkill()->GetName(), true );
	}
#endif

	ErrorReport::SetPosition( 2101, 1 );
	pChar->ClearAttackFireTimeAndSkill();

	ErrorReport::SetPosition( 2101, 2 );
	pChar->ClearReservedSliding();
	ErrorReport::SetPosition( 2101, 3 );

	if( iNextState == CS_FALL )
		pChar->SetFallState( true );
	else
		pChar->SetState( iNextState );
	ErrorReport::SetPosition( 2101, 4 );

	pChar->AddNewBuff( szSkillEndBuff, pChar->GetCharName(), "", NULL );
	ErrorReport::SetPosition( 2101, 5 );
}

void ioPlayStage::OnEmoticonSync( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	int iType;
	rkPacket >> iType;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;

	switch( iType )
	{
	case EMOTICON_HOME_PAGE:
		pChar->SetHomePageEmoticon();
		break;
	case EMOTICON_SCREEN_SHOT:
		pChar->SetScreenShotEmoticon();
		break;
	case EMOTICON_PROTECT_WOUND:
		{
			int iEmoticonType;
			rkPacket >> iEmoticonType;
			pChar->SetProtectWound( iEmoticonType );
		}
		break;
	case EMOTICON_COIN_EVENT:
		pChar->SetCoinEventEmoticon();
		break;
	case EMOTICON_NETWORK_BAD:
		pChar->SetNetworkBadEmoticon();
		break;	
	case EMOTICON_FISHING_GRADE:
		{
			ioHashString szIcon, szGrade;
			rkPacket >> szIcon >> szGrade;

			pChar->SetFishingGradeEmoticon( szIcon, szGrade );
		}
		break;
	case EMOTICON_NO_INPUT:
		{
			if( pChar->GetState() == CS_BLOW_WOUNDED )
			{
				pChar->SetState( CS_DELAY );
			}

			DWORD dwDuration;
			rkPacket >> dwDuration;
			pChar->SetNoInputEmoticon( dwDuration );
		}
		break;
	}
}

void ioPlayStage::OnTeleportState( SP2Packet &rkPacket )
{
	ioHashString szCharName;
	rkPacket >> szCharName;

	ioBaseChar *pChar = GetBaseChar( szCharName );
	if( !pChar )	return;

	ioHashString szStart, szEnd;
	float fStartRate, fEndRate;
	D3DXVECTOR3 vEndPos;

	rkPacket >> szStart >> fStartRate;
	rkPacket >> szEnd >> fEndRate;
	rkPacket >> vEndPos;

	pChar->SetTeleportState( szStart, fStartRate, szEnd, fEndRate, vEndPos );
}

void ioPlayStage::OnNonePlayState( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;

	if( pChar->GetState() == CS_DIE )
		return;

	bool bNonePlay;
	rkPacket >> bNonePlay;

	pChar->ApplyNonePlayStateBuff( bNonePlay );
}

void ioPlayStage::OnSpecialMotionState( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;

	if( pChar->GetState() == CS_DIE )
		return;

	pChar->SetSpecialMotionState();
}

void ioPlayStage::OnEtcItemMotionStateUDP( SP2Packet &rkPacket )
{
	ioHashString kName;
	rkPacket >> kName;

	ioBaseChar *pChar = GetBaseChar( kName );
	if( !pChar ) return;

	if( pChar->GetState() == CS_DIE )
		return;

	int iEtcItemCode;
	rkPacket >> iEtcItemCode;

	pChar->SetEtcItemMotionState( iEtcItemCode );
}

void ioPlayStage::OnEtcItemMotionOptionFirstSync( SP2Packet &rkPacket )
{
	ioHashString kName;
	int iMotionOption, iEtcItemCode;
	rkPacket >> kName >> iMotionOption >> iEtcItemCode;

	// 전투 대기방과 래더 대기방만 최초 동기화 필요
	switch( iMotionOption )
	{
	case ioEtcItemMotion::MOTION_OPTION_5:			// 전투대기방 포즈
		{
			if( g_BattleRoomMgr.IsBattleRoom() )
			{
				g_BattleRoomMgr.UpdateUserCharMotion( kName, iEtcItemCode );

				// 첫번째 동기화이므로 해당 유저한테 내 모션도 전송한다.
				SP2Packet kPacket( CUPK_ETCITEM_MOTION_OPTION_SYNC );
				kPacket << g_MyInfo.GetPublicID() << ioEtcItemMotion::MOTION_OPTION_5 << g_BattleRoomMgr.GetUserCharMotion( g_MyInfo.GetPublicID() );
				P2PNetwork::SendToUser( kName, kPacket );
			}
		}
		break;
	case ioEtcItemMotion::MOTION_OPTION_6:			// 진영전 대기방 포즈
		{
			if( g_LadderTeamMgr.IsLadderTeam() )
			{
				g_LadderTeamMgr.UpdateUserCharMotion( kName, iEtcItemCode );

				// 첫번째 동기화이므로 해당 유저한테 내 모션도 전송한다.
				SP2Packet kPacket( CUPK_ETCITEM_MOTION_OPTION_SYNC );
				kPacket << g_MyInfo.GetPublicID() << ioEtcItemMotion::MOTION_OPTION_6 << g_LadderTeamMgr.GetUserCharMotion( g_MyInfo.GetPublicID() );
				P2PNetwork::SendToUser( kName, kPacket );
			}
		}
		break;
	}	
}

void ioPlayStage::OnEtcItemMotionOptionSync( SP2Packet &rkPacket )
{
	ioHashString kName;
	int iMotionOption, iEtcItemCode;
	rkPacket >> kName >> iMotionOption >> iEtcItemCode;

	switch( iMotionOption )
	{
	case ioEtcItemMotion::MOTION_OPTION_1:          // 게임 승리 포즈
	case ioEtcItemMotion::MOTION_OPTION_2:			// 게임 패배 포즈
		{
			ioBaseChar *pChar = GetBaseChar( kName );
			if( !pChar ) return;

			pChar->SetRoundEndCharMotionItem( iEtcItemCode, false );
		}
		break;
	case ioEtcItemMotion::MOTION_OPTION_3:			// 좋은 수상 포즈
	case ioEtcItemMotion::MOTION_OPTION_4:			// 나쁜 수상 포즈
		{
			int iAwardType;
			rkPacket >> iAwardType;
			TotalResultMainWnd *pResultMainWnd = dynamic_cast<TotalResultMainWnd *>(g_GUIMgr.FindWnd( TOTAL_RESULT_MAIN_WND ));
			if( pResultMainWnd )
			{
				pResultMainWnd->SetAwardingCharMotion( kName, iAwardType, iEtcItemCode );
			}		
		}
		break;
	case ioEtcItemMotion::MOTION_OPTION_5:			// 전투대기방 포즈
		{
			if( g_BattleRoomMgr.IsBattleRoom() )
				g_BattleRoomMgr.UpdateUserCharMotion( kName, iEtcItemCode );
		}
		break;
	case ioEtcItemMotion::MOTION_OPTION_6:			// 진영전 대기방 포즈
		{
			if( g_LadderTeamMgr.IsLadderTeam() )
				g_LadderTeamMgr.UpdateUserCharMotion( kName, iEtcItemCode );
		}
		break;
	}
}

void ioPlayStage::OnUserInfoRefresh( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	int iClassType = g_MyInfo.GetClassType();
	SP2Packet kSend( CUPK_USER_INFO_SYNC );
	kSend << g_MyInfo.GetPublicID();
	kSend << g_MyInfo.GetGradeLevel() << g_MyInfo.GetGradeExpRate();
	kSend << g_MyInfo.GetPartyExp() << g_MyInfo.GetLadderExp() << g_MyInfo.GetAwardLevel() << g_MyInfo.GetAwardExp();
	kSend << g_MyInfo.GetKillDeathLevel() << g_MyInfo.GetFishingLevel() << g_MyInfo.GetFishingExpert() << g_MyInfo.GetExcavationLevel() << g_MyInfo.GetExcavationExpert();
	kSend << g_MyInfo.GetRecordWin( RECORD_HEROMATCH ) << g_MyInfo.GetRecordLose( RECORD_HEROMATCH ) << g_MyInfo.GetHeroTitle() << g_MyInfo.GetHeroRank();
	kSend << (int)g_MyInfo.GetBlockType() << g_MyInfo.GetUserRanking() << g_MyInfo.GetUserCampPos() << g_MyInfo.GetLadderPoint() << g_MyInfo.GetClassLevel( iClassType, false ) << iClassType;

	//펫 정보 보냄
	ioUserPet *pPet = g_MyInfo.GetUserPet();
	PetSlot sPetSlot;
	if ( pPet )
		pPet->GetPetSlotEquip( sPetSlot );

	kSend << sPetSlot.m_nPetCode << sPetSlot.m_nCurLevel;

	//각성 정보
	kSend << static_cast<byte>( g_MyInfo.GetAwakeType( iClassType ) );

	//개인본부 오픈여부
	kSend << g_MyInfo.IsHouseOpen();

	// medal
	int i = 0;
	MedalSyncInfoVec kMedal;
	g_MyInfo.GetMedalItemTypeVec( kMedal, g_MyInfo.GetClassType() );
	for( i = 0;i < ioMedalItemInfoManager::MAX_SLOT_NUM;i++ )
	{
		if( i < (int)kMedal.size() )
		{
			kSend << kMedal[i].m_iItemType;
			kSend << kMedal[i].m_iCustomIndex;
			if( kMedal[i].m_iCustomIndex != 0 )
			{
				for( int j = 0; j < 8; j++ )
					kSend << kMedal[i].m_iStat[j];
			}
		}
		else
			kSend << 0 << 0;
	}

	// growth
	MedalSyncInfoVec vMedalTypeVec;
	IntVec vGrowthLevelVec;
	g_MyInfo.GetMedalTypeAndGrowthLevel( iClassType, vMedalTypeVec, vGrowthLevelVec );

	int iSize = vGrowthLevelVec.size();
	int iMax  = ( MAX_CHAR_GROWTH+MAX_ITEM_GROWTH );
	for (int i = 0; i < iMax ; i++)
	{
		if( COMPARE( i, 0, iSize ) )
		{
			int iGrowthLevel = vGrowthLevelVec[i];
			kSend << iGrowthLevel;
		}
		else
			kSend << 0;
	}

	kSend << g_MyInfo.GetNamedTitleCode();
	kSend << g_MyInfo.GetNamedTitleLevel();
	kSend << g_MyInfo.GetNamedTitlePremium();

	IntVec vSoldierSetVec;
	g_MyInfo.GetSoldierSetCnt( vSoldierSetVec );

	int nSoldierSetCnt = (int)vSoldierSetVec.size();
	kSend << nSoldierSetCnt;

	for ( int i=0; i<nSoldierSetCnt; i++ )
		kSend << vSoldierSetVec[i];

	kSend << g_MyInfo.GetCurSuccessionCount();
	kSend << g_MyInfo.GetMaxSuccessionCount();

	P2PNetwork::SendP2PUserInfoSync( szName, kSend );
}

void ioPlayStage::OnUserInfoSync( SP2Packet &rkPacket )
{
	ioHashString UserID;
	int iGradeLevel, iGradeExpPer, iAwardLevel, iAwardExp, iKillDeathLevel, iFishingLevel, iFishingExp, iExcavationLevel, iExcavationExp, iBlockType, iRanking, iCampPosition, iLadderPoint, iClassLevel, iClassType;
	int iHeroWin, iHeroLose, iHeroTitle, iHeroRank;
	__int64 iPartyExp, iLadderExp;
	int nPetCode, nPetLevel;
	byte eAwakeType = AWAKE_NONE;
	int iCurSuccessionCnt,iMaxSuccessionCnt;

	rkPacket >> UserID >> iGradeLevel >> iGradeExpPer >> iPartyExp >> iLadderExp >> iAwardLevel >> iAwardExp
		     >> iKillDeathLevel >> iFishingLevel >> iFishingExp >> iExcavationLevel >> iExcavationExp >> iHeroWin >> iHeroLose >> iHeroTitle >> iHeroRank
			 >> iBlockType >> iRanking >> iCampPosition >> iLadderPoint >> iClassLevel >> iClassType
			 >> nPetCode >> nPetLevel >> eAwakeType;
	
	MedalSyncInfoVec vMedalItemTypeVec;
	for (int i = 0; i < ioMedalItemInfoManager::MAX_SLOT_NUM ; i++)
	{
		MedalSyncInfo cInfo;
		rkPacket >> cInfo.m_iItemType >> cInfo.m_iCustomIndex;
		if( cInfo.m_iCustomIndex != 0 )
		{
			for( int j = 0; j < 8; j++ )
				rkPacket >> cInfo.m_iStat[j];
		}
		vMedalItemTypeVec.push_back( cInfo );
	}

	bool bHouseOpen;
	rkPacket >> bHouseOpen;

	IntVec vGrowthLevelVec;
	int iMax = ( MAX_CHAR_GROWTH+MAX_ITEM_GROWTH );
	for (int i = 0; i < iMax ; i++)
	{
		int iGrowthLevel = 0;
		rkPacket >> iGrowthLevel;
		vGrowthLevelVec.push_back( iGrowthLevel );
	}

	DWORD dwNamedTitleCode;
	int nNamedTitleLevel;
	BYTE byNamedTitlePremium;
	rkPacket >> dwNamedTitleCode;
	rkPacket >> nNamedTitleLevel;
	rkPacket >> byNamedTitlePremium;

	int nSoldierSetCnt;
	rkPacket >> nSoldierSetCnt;

	IntVec vSoldierSetVec;
	for ( int i=0; i<nSoldierSetCnt; i++ )
	{
		int nSetCnt;
		rkPacket >> nSetCnt;
		vSoldierSetVec.push_back( nSetCnt );
	}
	rkPacket >> iCurSuccessionCnt;
	rkPacket >> iMaxSuccessionCnt;

	g_UserInfoMgr.SetUserInfo(	UserID, iGradeLevel, iGradeExpPer, iPartyExp, iLadderExp, iAwardLevel, iAwardExp, iKillDeathLevel, iFishingLevel, iFishingExp,
								iExcavationLevel, iExcavationExp, iHeroWin, iHeroLose, iHeroTitle, iHeroRank, iBlockType, iRanking, iCampPosition, iLadderPoint, iClassLevel, iClassType, 
								vMedalItemTypeVec, vGrowthLevelVec, nPetCode, nPetLevel, bHouseOpen, dwNamedTitleCode, nNamedTitleLevel, byNamedTitlePremium, vSoldierSetVec, iCurSuccessionCnt, iMaxSuccessionCnt );	

	g_UserInfoMgr.SetCharAwake( UserID, iClassType, static_cast<int>( eAwakeType ) );
	g_UserInfoMgr.SetSyncTime( UserID, FRAMEGETTIME() );
}

void ioPlayStage::OnNpcStuckState( SP2Packet &rkPacket )
{
	ioHashString kNpcName;
	rkPacket >> kNpcName;
	
	ioNpcChar *pNpcChar = ToNpcChar( GetBaseChar( kNpcName ) );
	if( !pNpcChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnNpcStuckState None Npc : %s", kNpcName.c_str() );
		return;
	}

	pNpcChar->ActiveStuck();
}

void ioPlayStage::OnMonsterCoinFirstSync( SP2Packet &rkPacket )
{
	DWORD dwUserIndex = 0;
	int iMonsterCoin, iGoldMonsterCoin, iRaidCoin = 0;
	ioHashString szUserID;
	rkPacket >> dwUserIndex >> szUserID >> iMonsterCoin >> iGoldMonsterCoin >> iRaidCoin;

	if( g_BattleRoomMgr.IsBattleRoom() )
		g_BattleRoomMgr.UpdateUserMonsterCoin( szUserID, iMonsterCoin, iGoldMonsterCoin, iRaidCoin );

	// 첫번째 동기화이므로 해당 유저한테 내 코인도 전송한다.
	SP2Packet kPacket( CUPK_MONSTERCOIN_SYNC );
	kPacket << g_MyInfo.GetPublicID() << g_MyInfo.GetEtcMonsterCoin() << g_MyInfo.GetEtcGoldMonsterCoin() << g_MyInfo.GetEtcRaidCoin();
	P2PNetwork::SendToUserByIndex(  dwUserIndex, kPacket );
}

void ioPlayStage::OnMonsterCoinSync( SP2Packet &rkPacket )
{
	int iMonsterCoin, iGoldMonsterCoin, iRaidCoin;
	ioHashString szUserID;	
	rkPacket >> szUserID >> iMonsterCoin >> iGoldMonsterCoin >> iRaidCoin;

	if( g_BattleRoomMgr.IsBattleRoom() )
		g_BattleRoomMgr.UpdateUserMonsterCoin( szUserID, iMonsterCoin, iGoldMonsterCoin, iRaidCoin );
}

void ioPlayStage::OnMonsterGlobalTarget( SP2Packet &rkPacket )
{
	ioHashString szGlobalTarget;
	float fGlobalTargetTime;
	rkPacket >> szGlobalTarget >> fGlobalTargetTime;

	SetMonsterTargetFixing( szGlobalTarget, fGlobalTargetTime );
}

void ioPlayStage::OnFirstSyncCharInfo( SP2Packet &rkPacket )
{
	ioHashString szUserID;
	DWORD dwCharIndex;
	CHARACTER kCharInfo;
	rkPacket >> szUserID >> dwCharIndex >> kCharInfo;
	
	ITEM_DATA kItemData[MAX_INVENTORY];
	for(int i = 0;i < MAX_INVENTORY;i++)
	{
		rkPacket >> kItemData[i].m_item_code >> kItemData[i].m_item_male_custom >> kItemData[i].m_item_female_custom;
	}

	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		g_BattleRoomMgr.UpdateUserCharIndex( szUserID, dwCharIndex, false );

		CHARACTER CharInfo;
		DWORD dwMyCharIndex = g_BattleRoomMgr.GetUserCharIndex( g_MyInfo.GetPublicID() );
		g_MyInfo.CopyCharacter( g_MyInfo.GetCharArray( dwMyCharIndex ), &CharInfo );
		SP2Packet kPacket( CUPK_SYNC_CHARINFO );
		kPacket << g_MyInfo.GetPublicID() << dwMyCharIndex << CharInfo;
		g_MyInfo.FillCharItemData( dwMyCharIndex, kPacket );
		P2PNetwork::SendToUser( szUserID, kPacket );
	}
	else if( g_LadderTeamMgr.IsLadderTeam() )
	{
		g_LadderTeamMgr.UpdateUserCharIndex( szUserID, dwCharIndex, false );

		CHARACTER CharInfo;
		DWORD dwMyCharIndex = g_LadderTeamMgr.GetUserCharIndex( g_MyInfo.GetPublicID() );
		g_MyInfo.CopyCharacter( g_MyInfo.GetCharArray( dwMyCharIndex ), &CharInfo );
		SP2Packet kPacket( CUPK_SYNC_CHARINFO );
		kPacket << g_MyInfo.GetPublicID() << dwMyCharIndex << CharInfo;
		g_MyInfo.FillCharItemData( dwMyCharIndex, kPacket );
		P2PNetwork::SendToUser( szUserID, kPacket );
	}
	else if( g_ShuffleRoomMgr.IsShuffleRoom() )
	{
		g_ShuffleRoomMgr.UpdateUserCharIndex( szUserID, dwCharIndex, false );

		CHARACTER CharInfo;
		DWORD dwMyCharIndex = g_ShuffleRoomMgr.GetUserCharIndex( g_MyInfo.GetPublicID() );
		g_MyInfo.CopyCharacter( g_MyInfo.GetCharArray( dwMyCharIndex ), &CharInfo );
		SP2Packet kPacket( CUPK_SYNC_CHARINFO );
		kPacket << g_MyInfo.GetPublicID() << dwMyCharIndex << CharInfo;
		g_MyInfo.FillCharItemData( dwMyCharIndex, kPacket );
		P2PNetwork::SendToUser( szUserID, kPacket );
	}
	g_GlobalSoldierInfo.SyncSoldierData( dwCharIndex, kCharInfo );	
	g_GlobalSoldierInfo.SyncSoldierItem( dwCharIndex, (const ITEM_DATA*)&kItemData );	
}

void ioPlayStage::OnSyncCharInfo( SP2Packet &rkPacket )
{
	ioHashString szUserID;
	DWORD dwCharIndex;
	CHARACTER kCharInfo;
	rkPacket >> szUserID >> dwCharIndex >> kCharInfo;

	ITEM_DATA kItemData[MAX_INVENTORY];
	for(int i = 0;i < MAX_INVENTORY;i++)
	{
		rkPacket >> kItemData[i].m_item_code >> kItemData[i].m_item_male_custom >> kItemData[i].m_item_female_custom;
	}

	if( g_BattleRoomMgr.IsBattleRoom() )
		g_BattleRoomMgr.UpdateUserCharIndex( szUserID, dwCharIndex, true );
	else if( g_LadderTeamMgr.IsLadderTeam() )
		g_LadderTeamMgr.UpdateUserCharIndex( szUserID, dwCharIndex, true );
	else if( g_ShuffleRoomMgr.IsShuffleRoom() )
		g_ShuffleRoomMgr.UpdateUserCharIndex( szUserID, dwCharIndex, true );
	else if ( m_pPlayMode )
	{
		// 코스튬 관련 (모드에 따른 동기화 - 체험하기 관련 툴팁 동기화 적용)
		if ( m_pPlayMode->GetModeType() == MT_HEADQUARTERS || m_pPlayMode->GetModeType() == MT_TRAINING|| m_pPlayMode->GetModeType() == MT_HOUSE )
		{
			ioBaseChar *pChar = GetBaseChar( szUserID );
			if ( !pChar )
				return;
			
			g_UserInfoMgr.SetEquipCostume( szUserID, kCharInfo.m_class_type, kCharInfo.m_EquipCostume );
		}
	}

	g_GlobalSoldierInfo.SyncSoldierData( dwCharIndex, kCharInfo );
	g_GlobalSoldierInfo.SyncSoldierItem( dwCharIndex, (const ITEM_DATA*)&kItemData );	
}

void ioPlayStage::DestroyDiePushStruct()
{
	ErrorReport::SetPosition( 1125, 1 );
	g_PushStructListMgr.ClearDeadStructList();
	ErrorReport::SetPosition( 1125, 2 );

	GameEntityList::iterator iter = m_GameEntityList.begin();
	while ( iter != m_GameEntityList.end() )
	{
		ioPushStruct *pPush = ToPushStruct( *iter );
		if( pPush && pPush->GetState() == ioPushStruct::PS_DIE )
		{
			ErrorReport::SetPosition( 1125, 3 );
			iter = EraseGameEntity( iter );
			ErrorReport::SetPosition( 1125, 4 );
		}
		else
		{
			++iter;
		}
	}

	ErrorReport::SetPosition( 1125, 5 );
}

void ioPlayStage::DestroyAllPushStruct()
{
	GameEntityList::iterator iter = m_GameEntityList.begin();
	while ( iter != m_GameEntityList.end() )
	{
		ioPushStruct *pPush = ToPushStruct( *iter );
		if( pPush )
		{
			iter = EraseGameEntity( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioPlayStage::DestroyPushStructByEtcCode( DWORD dwEtcCode )
{
	GameEntityList::iterator iter = m_GameEntityList.begin();
	while ( iter != m_GameEntityList.end() )
	{
		ioPushStruct *pPush = ToPushStruct( *iter );
		if( pPush && pPush->GetCreateEtcCode() == dwEtcCode )
		{
			iter = EraseGameEntity( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioPlayStage::DestroyAllBallStruct()
{
	GameEntityList::iterator iter = m_GameEntityList.begin();
	while ( iter != m_GameEntityList.end() )
	{
		ioBall *pBall = ToBallStruct( *iter );
		if( pBall )
		{
			iter = EraseGameEntity( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioPlayStage::DestroyAllDummyChar()
{
	GameEntityList::iterator iter = m_GameEntityList.begin();
	while ( iter != m_GameEntityList.end() )
	{
		ioDummyChar *pDummy = ToDummyChar( *iter );
		if( pDummy )
		{
			iter = EraseGameEntity( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioPlayStage::DestroyAllRouletteEntity()
{
	GameEntityList::iterator iter = m_GameEntityList.begin();
	while ( iter != m_GameEntityList.end() )
	{
		ioRoulette *pRoulette = ToRoulette( *iter );
		if( pRoulette )
		{
			iter = EraseGameEntity( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioPlayStage::DestroyDieMachineStruct()
{
	g_MachineStructMgr.ClearDeadStructList();

	GameEntityList::iterator iter = m_GameEntityList.begin();
	while ( iter != m_GameEntityList.end() )
	{
		ioMachineStruct *pMachine = ToMachineStruct( *iter );
		if( pMachine && !pMachine->IsLive() )
		{
			iter = EraseGameEntity( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioPlayStage::DestroyAllMachineStruct()
{
	GameEntityList::iterator iter = m_GameEntityList.begin();
	while ( iter != m_GameEntityList.end() )
	{
		ioMachineStruct *pMachine = ToMachineStruct( *iter );
		if( pMachine )
		{
			iter = EraseGameEntity( iter );
		}
		else
		{
			++iter;
		}
	}
}

bool ioPlayStage::CheckEnableEntity( ioGameEntity *pEntity )
{
	if( !pEntity ) return false;

	GameEntityList::iterator iter = m_GameEntityList.begin();
	while ( iter != m_GameEntityList.end() )
	{
		if( pEntity == (*iter) )
			return true;

		++iter;
	}

	return false;
}

bool ioPlayStage::UpdateMountedEntityList( ioGameEntity *pEntity, MountedEntityList &vMountEntityList, ioBaseChar* pOwner )
{
	vMountEntityList.erase( vMountEntityList.begin(), vMountEntityList.end() );

	D3DXVECTOR3 vEntityPos, vGameEntityPos;
	vEntityPos = pEntity->GetWorldPosition();

	ioDummyChar* pDummy = ToDummyChar( pEntity );

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioGameEntity *pGameEntity = *iter;
		if( pGameEntity == pEntity )
			continue;

		if( !pGameEntity->IsMountingAvailable() )
			continue;

		ioBaseChar* pChar = ToBaseChar( pGameEntity );
		if( pOwner && pOwner == pChar )
			continue;

		if( pDummy && pDummy->IsDummyCharCollisionSkipState( pChar ) )
			continue;
		
		vGameEntityPos = pGameEntity->GetWorldPosition();
		if( vEntityPos.y >= vGameEntityPos.y )
			continue;
				
		if( pChar )
		{
			ioSkill *pSkill = pChar->GetCurActiveSkill();
			if( pSkill )
			{
				ioDashHandAttackSkill* pHandSkill = ToDashHandAttackSkill( pSkill );				
				if( pHandSkill )
					continue;
			}

			//낚시 하는 유저는 높이 체크를 하지 말자
			if( pChar->GetState() == CS_FISHING )
				continue;
		}

		ioAxisAlignBox kAxisBox = pGameEntity->GetWorldAxisBox();
		ioOrientBox kOrientBox = pGameEntity->GetWorldCollisionBox();

		D3DXVECTOR3 vMin, vMax;
		if( !pGameEntity->IsMountAvailable() )
		{
			vMin = kAxisBox.GetMinPos();
			vMin.y = 0.0f;
			vMax = kAxisBox.GetMaxPos();
			vMax.y = kAxisBox.GetCenter().y;

			kAxisBox.SetMinMaxPos( vMin, vMax );
		}
		else
		{
			kOrientBox.SetCenter(kAxisBox.GetCenter() );

			D3DXVECTOR3 vVtxArray[8];
			kOrientBox.GetVertices( vVtxArray );

			vMin = vVtxArray[0];
			vMin.y = 0.0f;
			vMax = vVtxArray[6];
			vMax.y = kOrientBox.GetCenter().y;

			kOrientBox.SetBoxMinMaxForAxis( vMin, vMax );
		}

		float fBoxMaxY = vMax.y;
		float fCurYPos = -FLOAT1;

		ioOrientBox kColBox = pEntity->GetWorldCollisionBox();
		kColBox.SetCenter( pEntity->GetWorldAxisBox().GetCenter() );

		if( !pGameEntity->IsMountAvailable() )
		{
			kOrientBox.SetByAxisBox( kAxisBox );
		}

		if( ioMath::TestIntersection( kOrientBox, kColBox, NULL ) )
		{
			float fYHeight = kColBox.GetCenter().y + kColBox.GetExtents(1);
			if( fYHeight < fBoxMaxY )
			{
				fCurYPos = max( fCurYPos, fYHeight + FLOAT1 );				
			}
		}
		
		if( m_pBlockWorld )
		{
			float fHeight = m_pBlockWorld->GetHeight( vGameEntityPos.x, fBoxMaxY, vGameEntityPos.z );
			if( fCurYPos > -FLOAT1 && fHeight < fCurYPos )
			{
				vMountEntityList.push_back( pGameEntity );
				return true;
			}
		}

		float fMapHeight = m_pSceneMgr->GetHeight( vGameEntityPos.x, fBoxMaxY, vGameEntityPos.z );
		if( fCurYPos > -FLOAT1 && fMapHeight < fCurYPos )
		{
			vMountEntityList.push_back( pGameEntity );
		}
	}

	return !vMountEntityList.empty();
}

bool ioPlayStage::UpdateMountedEntityListByDummyChar( ioGameEntity *pEntity, MountedEntityList &vMountEntityList, ioBaseChar* pOwner )
{
	vMountEntityList.erase( vMountEntityList.begin(), vMountEntityList.end() );

	D3DXVECTOR3 vEntityPos, vGameEntityPos;
	vEntityPos = pEntity->GetWorldPosition();

	ioDummyChar* pDummy = ToDummyChar( pEntity );
	GameEntityList::iterator iter = m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioGameEntity *pGameEntity = *iter;
		if( pGameEntity == pEntity )
			continue;
		
		if( !pGameEntity->IsMountingAvailable() )
			continue;

		ioBaseChar* pChar = ToBaseChar( pGameEntity );
		if( pOwner && pOwner == pChar )
			continue;

		if( pDummy && pDummy->IsDummyCharCollisionSkipState( pChar ) )
			continue;

		vGameEntityPos = pGameEntity->GetWorldPosition();
		if( vEntityPos.y + pEntity->GetWorldCollisionBox().GetExtents( 1 ) >= vGameEntityPos.y )
			continue;

		if( pChar )
		{
			if( pChar->IsSystemActivityState() )
			{
				continue;
			}

			ioSkill *pSkill = pChar->GetCurActiveSkill();
			if( pSkill )
			{
				ioDashHandAttackSkill* pHandSkill = ToDashHandAttackSkill( pSkill );
				if( pHandSkill )
					continue;
			}
		}

		ioAxisAlignBox kAxisBox = pGameEntity->GetWorldAxisBox();
		ioOrientBox kOrientBox = pGameEntity->GetWorldCollisionBox();

		D3DXVECTOR3 vMin, vMax;
		if( !pGameEntity->IsMountAvailable() )
		{
			vMin = kAxisBox.GetMinPos();
			vMin.y = 0.0f;
			vMax = kAxisBox.GetMaxPos();
			vMax.y = kAxisBox.GetCenter().y;

			kAxisBox.SetMinMaxPos( vMin, vMax );
		}
		else
		{
			kOrientBox.SetCenter(kAxisBox.GetCenter() );

			D3DXVECTOR3 vVtxArray[8];
			kOrientBox.GetVertices( vVtxArray );

			vMin = vVtxArray[0];
			vMin.y = 0.0f;
			vMax = vVtxArray[6];
			vMax.y = kOrientBox.GetCenter().y;

			kOrientBox.SetBoxMinMaxForAxis( vMin, vMax );
		}

		ioOrientBox kColBox = pEntity->GetWorldCollisionBox();
		kColBox.SetCenter( pEntity->GetWorldAxisBox().GetCenter() );

		if( !pGameEntity->IsMountAvailable() )
		{
			kOrientBox.SetByAxisBox( kAxisBox );
		}
		
		float fBoxMaxY = vMax.y;
		float fCurYPos = -FLOAT1;

		if( ioMath::TestIntersection( kOrientBox, kColBox, NULL ) )
		{
			float fYHeight = kColBox.GetCenter().y + kColBox.GetExtents(1);
			if( fYHeight < fBoxMaxY )
			{
				fCurYPos = max( fCurYPos, fYHeight + FLOAT1 );				
			}
		}

		float fMapHeight = GetNearMountEntity( pGameEntity );
		float fgap = fabs( fMapHeight - fCurYPos );
		if( fCurYPos > -FLOAT1 && fMapHeight == fCurYPos )
		{		
			vMountEntityList.push_back( pGameEntity );
		}
	}

	return !vMountEntityList.empty();
}


float ioPlayStage::GetNearMountEntity( ioGameEntity* pEntity )
{
	ioAxisAlignBox kAxisBox = pEntity->GetWorldAxisBox();
	ioOrientBox kOrientBox  = pEntity->GetWorldCollisionBox();
	D3DXVECTOR3 vMin, vMax;

	if( !ToPushStructConst(pEntity) )
	{

		vMin = kAxisBox.GetMinPos();
		vMin.y = 0.0f;

		vMax = kAxisBox.GetMaxPos();
		vMax.y = kAxisBox.GetCenter().y;

		kAxisBox.SetMinMaxPos( vMin, vMax );
	}
	else
	{
		kOrientBox.SetCenter(kAxisBox.GetCenter() );

		D3DXVECTOR3 vVtxArray[8];
		kOrientBox.GetVertices( vVtxArray );

		vMin = vVtxArray[0];
		vMax = vVtxArray[6];		
		vMin.y = 0.0f;
		vMax.y = kOrientBox.GetCenter().y;
	
		kOrientBox.SetBoxMinMaxForAxis( vMin, vMax );
	}

	float fBoxMaxY = vMax.y;
	float fCurYPos = -FLOAT1;
	
	MountAvailableList::iterator iter = m_MountAvailableList.begin();
	for( ; iter!=m_MountAvailableList.end() ; ++iter )
	{
		ioGameEntity *pCurMount = *iter;
		if( !pCurMount->IsNowMountEnable() )
			continue;

		ioDummyChar* pDummy = ToDummyChar( pCurMount );
		if( !pDummy )
			continue;

		if( !pDummy->GetMountEnable() )
			continue;
		
		ioOrientBox kColBox = pCurMount->GetWorldCollisionBox();
		kColBox.SetCenter( pCurMount->GetWorldAxisBox().GetCenter() );

		if( !ToPushStructConst(pEntity) )
		{
			kOrientBox.SetByAxisBox( kAxisBox );
		}
		else
		{
			D3DXVECTOR3 vVtxArray[8];
			kColBox.GetVertices( vVtxArray );

			vMin = vVtxArray[0];
			vMax = vVtxArray[6];
		}

		if( ioMath::TestIntersection( kOrientBox, kColBox, NULL ) )
		{
			float fYHeight = kColBox.GetCenter().y + kColBox.GetExtents(1);
			if( fYHeight < fBoxMaxY )
			{
				fCurYPos = max( fCurYPos, fYHeight + FLOAT1 );
			}
		}
	}

	float fMapHeight = m_pSceneMgr->GetHeight( pEntity->GetWorldPosition().x, fBoxMaxY, pEntity->GetWorldPosition().z );

	if( m_pBlockWorld )
	{
		float fHeight = m_pBlockWorld->GetHeight( pEntity->GetWorldPosition().x, fBoxMaxY, pEntity->GetWorldPosition().z );
		if( fMapHeight < fHeight )
		{
			if( fCurYPos > -FLOAT1 && fHeight < fCurYPos )
				return fCurYPos;

			return fHeight;
		}
	}

	if( fCurYPos > -FLOAT1 && fMapHeight < fCurYPos )
	{
		return fCurYPos;
	}

	return fMapHeight;
}

bool ioPlayStage::UpdateMountedWeaponList( ioGameEntity *pEntity, MountedWeaponList &vMountWeaponList )
{
	vMountWeaponList.erase( vMountWeaponList.begin(), vMountWeaponList.end() );

	D3DXVECTOR3 vWeaponPos, vGameEntityPos;
	vGameEntityPos = pEntity->GetWorldPosition();

	ioWeaponManager::WeaponIterator kIter = g_WeaponMgr.GetWeaponIterator();
	while( kIter.HasMoreElements() )
	{
		ioMineWeapon *pMine = ToMineWeapon( kIter.Next() );
		ioMineWeapon2 *pMine2 = ToMineWeapon2( kIter.Next() );
		ioMineWeapon3 *pMine3 = ToMineWeapon3( kIter.Next() );
		ioMineWeapon4 *pMine4 = ToMineWeapon4( kIter.Next() );
		ioMineWeapon6 *pMine6 = ToMineWeapon6( kIter.Next() );

		if( !pMine && !pMine2 && !pMine3 ) continue;

		if( pMine )
			vWeaponPos = pMine->GetPosition();
		else if( pMine2 )
			vWeaponPos = pMine2->GetPosition();
		else if( pMine3 )
			vWeaponPos = pMine3->GetPosition();
		else if( pMine4 )
			vWeaponPos = pMine4->GetPosition();
		else if( pMine6 )
			vWeaponPos = pMine6->GetPosition();

		if( vGameEntityPos.y >= vWeaponPos.y )
			continue;

		D3DXVECTOR3 vRayDir( 0.0f, -FLOAT1, 0.0f );
		D3DXVECTOR3 vRayOrig( vWeaponPos.x, vWeaponPos.y, vWeaponPos.z );

		ioOrientBox kColBox = pEntity->GetWorldCollisionBox();
		kColBox.SetCenter( pEntity->GetWorldAxisBox().GetCenter() );

		ioRay kYRay( vRayOrig, vRayDir );
		if( ioMath::TestIntersection( kYRay, kColBox ) )
		{
			if( pMine )
				vMountWeaponList.push_back( pMine );
			else if( pMine2 )
				vMountWeaponList.push_back( pMine2 );
			else if( pMine3 )
				vMountWeaponList.push_back( pMine3 );
			else if( pMine4 )
				vMountWeaponList.push_back( pMine4 );
			else if( pMine6 )
				vMountWeaponList.push_back( pMine6 );
		}
	}

	return !vMountWeaponList.empty();
}

bool ioPlayStage::UpdateMountedWeaponListByBox( ioGameEntity *pEntity,
											    MountedWeaponList &vMountWeaponList )
{
	vMountWeaponList.erase( vMountWeaponList.begin(), vMountWeaponList.end() );

	const ioOrientBox &rkEntityBox = pEntity->GetWorldCollisionBox();

	ioWeaponManager::WeaponIterator kIter = g_WeaponMgr.GetWeaponIterator();
	while( kIter.HasMoreElements() )
	{
		ioWeapon *pWeapon = kIter.Next();
		ioMineWeapon *pMine = ToMineWeapon( pWeapon );
		ioMineWeapon2 *pMine2 = ToMineWeapon2( pWeapon );
		ioMineWeapon3 *pMine3 = ToMineWeapon3( pWeapon );
		ioMineWeapon6 *pMine6 = ToMineWeapon6( pWeapon );
		ioBoundBombWeapon *pBound = ToBoundBombWeapon( pWeapon );
		ioMapAttachWeapon *pAttach = ToMapAttachWeapon( pWeapon );

		if( !pMine && !pBound && !pAttach &&!pMine2 )	continue;

		if( ToPushStruct( pEntity ) )
		{
			if( pMine )
			{
				if( rkEntityBox.GetCenter().y > pMine->GetWorldCollisionBox().GetCenter().y )
					continue;
			}
			else if( pBound )
			{
				if( !pBound->IsExplosion() )
					continue;

				if( rkEntityBox.GetCenter().y > pBound->GetPosition().y )
					continue;
			}
			else if( pAttach )
			{
				if( rkEntityBox.GetCenter().y > pAttach->GetWorldAttackBox().GetCenter().y )
					continue;
			}
			else if( pMine2 )
			{
				if( rkEntityBox.GetCenter().y > pMine2->GetWorldCollisionBox().GetCenter().y )
					continue;
			}
			else if( pMine3 )
			{
				if( rkEntityBox.GetCenter().y > pMine3->GetWorldCollisionBox().GetCenter().y )
					continue;
			}
			else if( pMine6 )
			{
				if( rkEntityBox.GetCenter().y > pMine6->GetWorldCollisionBox().GetCenter().y )
					continue;
			}
		}

		if( pMine )
		{
			if( ioMath::TestIntersection( rkEntityBox, pMine->GetWorldCollisionBox(), NULL ) )
			{
				vMountWeaponList.push_back( pMine );
			}
		}
		else if( pBound )
		{
			if( ioMath::TestIntersection( rkEntityBox, pBound->GetWorldAttackBox(), NULL ) )
			{
				vMountWeaponList.push_back( pBound );
			}
		}
		else if( pAttach )
		{
			if( ioMath::TestIntersection( rkEntityBox, pAttach->GetWorldAttackBox(), NULL ) )
			{
				vMountWeaponList.push_back( pAttach );
			}
		}
		else if( pMine2 )
		{
			if( ioMath::TestIntersection( rkEntityBox, pMine2->GetWorldCollisionBox(), NULL ) )
			{
				vMountWeaponList.push_back( pMine2 );
			}
		}
		else if( pMine3 )
		{
			if( ioMath::TestIntersection( rkEntityBox, pMine3->GetWorldCollisionBox(), NULL ) )
			{
				vMountWeaponList.push_back( pMine3 );
			}
		}
		else if( pMine6 )
		{
			if( ioMath::TestIntersection( rkEntityBox, pMine6->GetWorldCollisionBox(), NULL ) )
			{
				vMountWeaponList.push_back( pMine6 );
			}
		}
	}

	return !vMountWeaponList.empty();
}

void ioPlayStage::ProcessSyncPushStruct()
{
	if( !P2PNetwork::IsNetworkPlaying() )	return;
	if( !m_PushStructTimer.IsTimePast( 3000 ) )	return;

	ioBaseChar *pOwner = GetOwnerChar();
	if( !pOwner )	return;

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioPushStruct *pPush = ToPushStruct( *iter );
		if( !pPush ) continue;

		if( pPush->GetState() == ioPushStruct::PS_MOVE )
		{
			if( pPush->GetPushChar() == pOwner )
			{
				pPush->SendPushStructInfo();
			}
		}
		else
		{
			const ioHashString& szPushChar = pPush->GetLastPushChar();
			if( !szPushChar.IsEmpty() )
			{
				if( szPushChar == pOwner->GetCharName() )
					pPush->SendPushStructInfo();
			}
			else
			{
				if( pPush->GetPushTime() > 0 && P2PNetwork::IsHost() )
				{
					pPush->SetPushTime( 0 );
					pPush->SendPushStructInfo();
				}
			}
		}
	}

	for( iter=m_GameEntityList.begin() ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldItem *pItem = ToFieldItem( *iter );
		if( !pItem ) continue;
		if( !pItem->GetLastPushChar().IsEmpty() )
		{
			if( pItem->GetLastPushChar() == pOwner->GetCharName() )
			{
				pItem->SendFieldItemInfo();
			}
		}
		else
		{
			if( pItem->GetPushTime() > 0 && P2PNetwork::IsHost() )
			{
				pItem->SetPushTime( 0 );
				pItem->SendFieldItemInfo();
			}
		}
	}
}

void ioPlayStage::OnPushStructInfo( SP2Packet &rkPacket )
{
	int iIndex;
	rkPacket >> iIndex;

	ioPushStruct *pPushStruct = g_PushStructListMgr.FindPushStruct( iIndex );
	if( !pPushStruct )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPushStructInfo - %d PushStruct Not Exist", iIndex );
		return;
	}

	pPushStruct->OnPushStructInfo( rkPacket );
}

void ioPlayStage::OnPushStructCreateWeapon( SP2Packet &rkPacket )
{
	int iIndex;
	rkPacket >> iIndex;

	ioPushStruct *pPushStruct = g_PushStructListMgr.FindPushStruct( iIndex );
	if( !pPushStruct )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPushStructCreateWeapon - %d PushStruct Not Exist", iIndex );
		return;
	}

	if( pPushStruct->GetState() == ioPushStruct::PS_DIE )
		return;

	float fAngle = 0.0f;
	rkPacket >> fAngle;

	D3DXQUATERNION qtAngle;
	rkPacket >> qtAngle;

	DWORD dwBaseWeaponIndex;
	rkPacket >> dwBaseWeaponIndex;

	pPushStruct->SetWorldOrientation( qtAngle );
	pPushStruct->SetBaseWeaponIndex( dwBaseWeaponIndex );
	pPushStruct->CreateWeapon( fAngle );
}

void ioPlayStage::OnPushStructSearchState( SP2Packet &rkPacket )
{
	int iIndex;
	rkPacket >> iIndex;

	ioPushStruct *pPushStruct = g_PushStructListMgr.FindPushStruct( iIndex );
	if( !pPushStruct )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPushStructSearchState - %d PushStruct Not Exist", iIndex );
		return;
	}

	if( pPushStruct->GetState() == ioPushStruct::PS_DIE )
		return;

	D3DXQUATERNION qtAngle;
	rkPacket >> qtAngle;

	pPushStruct->SetWorldOrientation( qtAngle );
	pPushStruct->ChangeToSearchingState();
}

void ioPlayStage::OnPushStructTracingState( SP2Packet &rkPacket )
{
	int iIndex;
	rkPacket >> iIndex;

	ioPushStruct *pPushStruct = g_PushStructListMgr.FindPushStruct( iIndex );
	if( !pPushStruct )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPushStructTracingState - %d PushStruct Not Exist", iIndex );
		return;
	}

	if( pPushStruct->GetState() == ioPushStruct::PS_DIE )
		return;

	D3DXQUATERNION qtAngle;
	rkPacket >> qtAngle;

	pPushStruct->SetWorldOrientation( qtAngle );

	ioHashString szTargetName;
	D3DXVECTOR3 vStartDir;
	rkPacket >> szTargetName >> vStartDir;

	pPushStruct->ChangeToTracingState( szTargetName, vStartDir );
}

void ioPlayStage::OnPushStructFireState( SP2Packet &rkPacket )
{
	int iIndex;
	rkPacket >> iIndex;

	ioPushStruct *pPushStruct = g_PushStructListMgr.FindPushStruct( iIndex );
	if( !pPushStruct )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPushStructFireState - %d PushStruct Not Exist", iIndex );
		return;
	}

	if( pPushStruct->GetState() == ioPushStruct::PS_DIE )
		return;

	D3DXQUATERNION qtAngle;
	rkPacket >> qtAngle;

	pPushStruct->SetWorldOrientation( qtAngle );
	pPushStruct->ChangeToFireState();
}

bool ioPlayStage::CheckSensingMine( ioMineWeapon *pMine, float fSensingRange )
{
	if( !pMine || !pMine->IsCanExplosion() )
		return false;
	if( pMine->IsExperienceOwner() )
		return false;
	if( !pMine->IsCanMineSensing() )
		return false;

	ioBaseChar *pOwner = pMine->GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	D3DXVECTOR3 vMinePos = pMine->GetPosition();
	float fSensingRangeSq = fSensingRange * fSensingRange;

	ioWeaponManager::WeaponIterator kInner = g_WeaponMgr.GetWeaponIterator();
	while( kInner.HasMoreElements() )
	{
		ioMineWeapon *pTarget = ToMineWeapon( kInner.Next() );

		if( pMine == pTarget ) continue;
		if( !pTarget || !pTarget->IsCanExplosion() ) continue;
		if( !pTarget->IsCanMineSensing() ) continue;
		if( pTarget->IsExperienceOwner() ) continue;

		D3DXVECTOR3 vDiff = vMinePos - pTarget->GetPosition();
		if( D3DXVec3LengthSq( &vDiff ) < fSensingRangeSq )
			return true;
	}

	return false;
}

bool ioPlayStage::CheckSensingMine2( ioMineWeapon2 *pMine, float fSensingRange )
{
	if( !pMine || !pMine->IsCanExplosion() )
		return false;
	if( pMine->IsExperienceOwner() )
		return false;
	if( !pMine->IsCanMineSensing() )
		return false;

	ioBaseChar *pOwner = pMine->GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	D3DXVECTOR3 vMinePos = pMine->GetPosition();
	float fSensingRangeSq = fSensingRange * fSensingRange;

	ioWeaponManager::WeaponIterator kInner = g_WeaponMgr.GetWeaponIterator();
	while( kInner.HasMoreElements() )
	{
		ioMineWeapon2 *pTarget = ToMineWeapon2( kInner.Next() );

		if( pMine == pTarget ) continue;
		if( !pTarget || !pTarget->IsCanExplosion() ) continue;
		if( !pTarget->IsCanMineSensing() ) continue;
		if( pTarget->IsExperienceOwner() ) continue;

		D3DXVECTOR3 vDiff = vMinePos - pTarget->GetPosition();
		if( D3DXVec3LengthSq( &vDiff ) < fSensingRangeSq )
			return true;
	}

	return false;
}

bool ioPlayStage::CheckSensingMine3( ioMineWeapon3 *pMine, float fSensingRange )
{
	if( !pMine || !pMine->IsCanExplosion() )
		return false;
	if( pMine->IsExperienceOwner() )
		return false;
	if( !pMine->IsCanMineSensing() )
		return false;

	ioBaseChar *pOwner = pMine->GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	D3DXVECTOR3 vMinePos = pMine->GetPosition();
	float fSensingRangeSq = fSensingRange * fSensingRange;

	ioWeaponManager::WeaponIterator kInner = g_WeaponMgr.GetWeaponIterator();
	while( kInner.HasMoreElements() )
	{
		ioMineWeapon3 *pTarget = ToMineWeapon3( kInner.Next() );

		if( pMine == pTarget ) continue;
		if( !pTarget || !pTarget->IsCanExplosion() ) continue;
		if( !pTarget->IsCanMineSensing() ) continue;
		if( pTarget->IsExperienceOwner() ) continue;

		D3DXVECTOR3 vDiff = vMinePos - pTarget->GetPosition();
		if( D3DXVec3LengthSq( &vDiff ) < fSensingRangeSq )
			return true;
	}

	return false;
}

bool ioPlayStage::CheckSensingMine4( ioMineWeapon4 *pMine, float fSensingRange )
{
	if( !pMine || !pMine->IsCanExplosion() )
		return false;
	if( pMine->IsExperienceOwner() )
		return false;
	if( !pMine->IsCanMineSensing() )
		return false;

	ioBaseChar *pOwner = pMine->GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	D3DXVECTOR3 vMinePos = pMine->GetPosition();
	float fSensingRangeSq = fSensingRange * fSensingRange;

	ioWeaponManager::WeaponIterator kInner = g_WeaponMgr.GetWeaponIterator();
	while( kInner.HasMoreElements() )
	{
		ioMineWeapon4 *pTarget = ToMineWeapon4( kInner.Next() );

		if( pMine == pTarget ) continue;
		if( !pTarget || !pTarget->IsCanExplosion() ) continue;
		if( !pTarget->IsCanMineSensing() ) continue;
		if( pTarget->IsExperienceOwner() ) continue;

		D3DXVECTOR3 vDiff = vMinePos - pTarget->GetPosition();
		if( D3DXVec3LengthSq( &vDiff ) < fSensingRangeSq )
			return true;
	}

	return false;
}

bool ioPlayStage::CheckSensingMine5( ioMineWeapon5 *pMine, float fSensingRange )
{
	if( !pMine || !pMine->IsCanExplosion() )
		return false;
	if( pMine->IsExperienceOwner() )
		return false;
	if( !pMine->IsCanMineSensing() )
		return false;

	ioBaseChar *pOwner = pMine->GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	D3DXVECTOR3 vMinePos = pMine->GetPosition();
	float fSensingRangeSq = fSensingRange * fSensingRange;

	ioWeaponManager::WeaponIterator kInner = g_WeaponMgr.GetWeaponIterator();
	while( kInner.HasMoreElements() )
	{
		ioMineWeapon5 *pTarget = ToMineWeapon5( kInner.Next() );

		if( pMine == pTarget ) continue;
		if( !pTarget || !pTarget->IsCanExplosion() ) continue;
		if( !pTarget->IsCanMineSensing() ) continue;
		if( pTarget->IsExperienceOwner() ) continue;

		D3DXVECTOR3 vDiff = vMinePos - pTarget->GetPosition();
		if( D3DXVec3LengthSq( &vDiff ) < fSensingRangeSq )
			return true;
	}

	return false;
}

bool ioPlayStage::CheckSensingMine6( ioMineWeapon6 *pMine, float fSensingRange )
{
	if( !pMine || !pMine->IsCanExplosion() )
		return false;
	if( pMine->IsExperienceOwner() )
		return false;
	if( !pMine->IsCanMineSensing() )
		return false;

	ioBaseChar *pOwner = pMine->GetOwner();
	if( !pOwner || !pOwner->IsNeedProcess() )
		return false;

	D3DXVECTOR3 vMinePos = pMine->GetPosition();
	float fSensingRangeSq = fSensingRange * fSensingRange;

	ioWeaponManager::WeaponIterator kInner = g_WeaponMgr.GetWeaponIterator();
	while( kInner.HasMoreElements() )
	{
		ioMineWeapon6 *pTarget = ToMineWeapon6( kInner.Next() );

		if( pMine == pTarget ) continue;
		if( !pTarget || !pTarget->IsCanExplosion() ) continue;
		if( !pTarget->IsCanMineSensing() ) continue;
		if( pTarget->IsExperienceOwner() ) continue;

		D3DXVECTOR3 vDiff = vMinePos - pTarget->GetPosition();
		if( D3DXVec3LengthSq( &vDiff ) < fSensingRangeSq )
			return true;
	}

	return false;
}

void ioPlayStage::OnExtendJump( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyExtendJump( rkPacket );
}

void ioPlayStage::OnComboJump( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyExtendJump( rkPacket );
}

void ioPlayStage::OnAimFire( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	int iCurDashType, iCurJumpType;
	rkPacket >> iCurDashType >> iCurJumpType;

	pChar->SetCurDashType( (DashType) iCurDashType );
	pChar->SetCurJumpType( (JumpType) iCurJumpType );

	pChar->ApplyAimState( rkPacket );
}

void ioPlayStage::OnCrossBowAim( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	int iCurDashType, iCurJumpType;
	rkPacket >> iCurDashType >> iCurJumpType;

	pChar->SetCurDashType( (DashType) iCurDashType );
	pChar->SetCurJumpType( (JumpType) iCurJumpType );

	pChar->ApplyCrossBowAimState( rkPacket );
}

void ioPlayStage::OnJumpping( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	bool bFullTime, bChangeJumpping;
	rkPacket >> bFullTime;
	rkPacket >> bChangeJumpping;

	if( bChangeJumpping )
		pChar->ApplyChangeJumppingState( rkPacket );
	else
		pChar->SetJumpping( bFullTime );
}

void ioPlayStage::OnRepositionFieldItem( SP2Packet &rkPacket )
{
	int iItemIdx;
	D3DXVECTOR3 vPos;
	rkPacket >> iItemIdx;
	rkPacket >> vPos;

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldItem *pItem = ToFieldItem( *iter );
		if( !pItem )	continue;

		if( pItem->GetItemCreateIndex() == iItemIdx )
		{
			pItem->ReserveRePosition( vPos );
			break;
		}
	}
}

void ioPlayStage::OnFieldItemInfo( SP2Packet &rkPacket )
{
	DWORD dwItemIdx;
	D3DXVECTOR3 vPos;
	rkPacket >> dwItemIdx;
	rkPacket >> vPos;

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldItem *pItem = ToFieldItem( *iter );
		if( !pItem )	continue;

		if( pItem->GetItemCreateIndex() == dwItemIdx )
		{
			pItem->ReserveRePosition( vPos );
			break;
		}
	}
}

void ioPlayStage::OnDrink( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;

	if( pChar->GetState() == CS_DIE )
		return;

	pChar->SetDrinkState();
}

void ioPlayStage::OnDropThrowBomb( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;
	
	D3DXVECTOR3 vDropDir;
	rkPacket >> vDropDir;

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;
	pChar->SetWeaponIndexBase( dwWeaponIdx );

	pChar->CheckDropThrowBomb( vDropDir, true );
}

void ioPlayStage::OnDropSmartBomb( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
		return;

	if( !pChar->CheckGrapplingExceptionState() )
		return;

	D3DXVECTOR3 vDropDir;
	rkPacket >> vDropDir;

	pChar->CheckDropSmartBomb( vDropDir );
}

void ioPlayStage::OnWoundedPoweredArmor( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
		return;

	if( !pChar->CheckGrapplingExceptionState() )
		return;

	pChar->CheckPowerArmorWounded();
}

void ioPlayStage::OnRelayChat( SP2Packet &rkPacket )
{
	int iChatType;
	rkPacket >> iChatType;
	switch( iChatType )
	{
	case CT_PARTY:     //전투방 유저 전체
		{
			char szChatBuf[MAX_PATH] = "";
			int iTeam;
			ioHashString szID, szChat;
			bool bNoSound;

			rkPacket >> iTeam;
			rkPacket >> szID;
			rkPacket >> szChat;
			rkPacket >> bNoSound;

			g_MannerTrialChatMgr.Add( ioMannerTrialChatManager::TT_BATTLE_CHAT, szID.c_str(), szChat.c_str() ); // 욕필터전에 할것

			StringCbCopy( szChatBuf, MAX_PATH, g_SlangUtil.ConvertStringWithPreLine( szChat.c_str() ) );
			szChat = szChatBuf;

			SetPartyChatMsg( (TeamType)iTeam, szID.c_str(), szChat.c_str(), bNoSound );
		}
		break;
	case CT_LADDER:    //진영팀 유저 전체
		{
			char szChatBuf[MAX_PATH] = "";
			ioHashString szID, szChat;
			bool bNoSound;

			rkPacket >> szID;
			rkPacket >> szChat;
			rkPacket >> bNoSound;

			g_MannerTrialChatMgr.Add( ioMannerTrialChatManager::TT_LADDER_CHAT, szID.c_str(), szChat.c_str() ); // 욕필터전에 할것

			StringCbCopy( szChatBuf, MAX_PATH, g_SlangUtil.ConvertStringWithPreLine( szChat.c_str() ) );
			szChat = szChatBuf;

			SetLadderChatMsg( szID.c_str(), szChat.c_str(), bNoSound );
		}
		break;
	}
}

void ioPlayStage::OnBattleRoomList( SP2Packet &rkPacket )
{
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		ioBattleRoomExitMsgBox *pExitBox = dynamic_cast<ioBattleRoomExitMsgBox*>( g_GUIMgr.FindWnd( BATTLEROOM_EXIT_MSG_BOX ) );
		if( pExitBox && pExitBox->IsShow() )
		{
			pExitBox->ApplyBattleRoomList( rkPacket );
		}
	}
	else
	{
		if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
		{
			LobbyMainWnd *pLobbyMainWnd = dynamic_cast< LobbyMainWnd* >( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
			if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )
			{
				pLobbyMainWnd->ApplyNormalTeamList( rkPacket );
			}

			TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
			if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )
				pTrainingFuncWnd->ApplyNormalTeamList( rkPacket );
		}
		else
		{
			QuickLobbyWnd* pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
			if( pWnd && pWnd->IsShow() )
				pWnd->ApplyNormalTeamList( rkPacket );			
		}
	}
}

void ioPlayStage::OnBattleRoomCreate( SP2Packet &rkPacket )
{
	int iResult;
	rkPacket >> iResult;

	if( iResult != CREATE_BATTLEROOM_OK )
	{
		TCPNetwork::MouseBusy( false );
		g_GUIMgr.SetMsgBox(MB_OK, NULL, STR(1));
	}
}

void ioPlayStage::OnBattleRoomInfo( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	g_GUIMgr.HideWnd( PARTY_INVITED_WND );
	g_GUIMgr.HideWnd( PARTY_INFO_WND );
	g_GUIMgr.HideWnd( TIMEGATE_SEARCH );

	
	bool bBattleRoom = false; //현재 룸에 속해있는지 여부
	bool bFristEnter = g_BattleRoomMgr.ApplyBattleRoomInfo( rkPacket, bBattleRoom );

	if( bFristEnter && ( IsCustomPlazaMode() || GetModeType() == MT_HEADQUARTERS || GetModeType() == MT_HOUSE ) )
	{
		// 전투룸에 첫 접속
		g_App.ClearAllNetworkTable( P2P_STYLE_VOICE ); 
		g_VoiceChat.RemoveAllSoundPlayer();
		g_VoiceOffListManager.DeleteAll();
		VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
		if(pWnd)
			pWnd->AllOtherVoiceOff();

		SP2Packet kPacket( CTPK_VOICE_INFO_USER );
		kPacket << ID_VOICE_REMOVE_TABLE;
		ioHashString szEmptyString; // 빈스트링이면 모두에게 전송
		kPacket << szEmptyString;
		TCPNetwork::SendToServer( kPacket );
#ifndef SHIPPING
		LOG.PrintTimeAndLog( 0, "%s Deleate all voice and CTPK_VOICE_INFO_USER | ID_VOICE_REMOVE_TABLE | %s", __FUNCTION__, szEmptyString.c_str() );
#endif
	}

	if ( bFristEnter && Setting::UseVoiceChat() )
		g_ChatMgr.SetSystemMsg( STR(1) );

	//_PLAZA_CONNECT_가 발동된 경우에도 래더, 배틀, 대회 전투방 등은 로비로 이동시킴

	//로비, 광장, 본부에서 전투방으로 이동했을 경우
	if( bFristEnter && !bBattleRoom )
	{
		// 전투방
		ioMyRoomMode::SetMyRoomType( MRT_BATTLE );

		// 로비로 이탈
		if( GetModeType() == MT_TRAINING || GetModeType() == MT_HEADQUARTERS|| GetModeType() == MT_HOUSE )
		{
			SP2Packet kPacket( CTPK_EXIT_ROOM );
			kPacket << EXIT_ROOM_LOBBY << 0 << true;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );
		}
		else
		{
			g_App.SetNextStage( ioGameStage::GS_LOBBY, __FUNCTION__ );
			g_App.SetModeType( 0, 0, 0, PT_NONE );
		}
		g_BattleRoomMgr.SetBattleModePlaying( bBattleRoom );
	}
	else
	{
		// 게임 UI 갱신
		PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
		if( pShamBattle )
			pShamBattle->UpdateShamBattle();
	}

	if( bFristEnter )
		g_MyInfo.SetArenaModeCharacter( MT_NONE );
}

void ioPlayStage::OnBattleRoomUserInfo( SP2Packet &rkPacket, bool bCompleteCheck )
{
	ioHashString szJoinName = g_BattleRoomMgr.ApplyJoinUserInfo( rkPacket );

	PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
	if( pShamBattle )
		pShamBattle->UpdateShamBattle();
}

void ioPlayStage::OnBattleRoomJoin( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	byte bServerMoveType = RET_PLAZA_TO_ROOM;
	int iResult = 0;
	rkPacket >> iResult;
	rkPacket >> bServerMoveType;
	
	ModeType eModeType = GetModeType();
	if( bServerMoveType == RET_MOVE_PLAZA_TO_ROOM || bServerMoveType == RET_MOVE_ROOM_TO_ROOM )
	{
		ExceptionExitRoom();
	}

	switch( iResult )
	{
	case USER_BATTLEROOM_JOIN_SEARCH_FAILED:
		{
			if( bServerMoveType == RET_MOVE_PLAZA_TO_ROOM || bServerMoveType == RET_MOVE_ROOM_TO_ROOM  )
				g_GUIMgr.SetReserveMsgBox( MB_BATTLEROOM_JOIN_SEARCH_FAILED );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		}
		break;
	case USER_BATTLEROOM_JOIN_NOT_NODE:
		{
			if( bServerMoveType == RET_MOVE_PLAZA_TO_ROOM || bServerMoveType == RET_MOVE_ROOM_TO_ROOM )
				g_GUIMgr.SetReserveMsgBox( MB_BATTLEROOM_JOIN_NOT_NODE );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		}
		break;
	case USER_BATTLEROOM_JOIN_FULL_USER:
		{
			if( bServerMoveType == RET_MOVE_PLAZA_TO_ROOM || bServerMoveType == RET_MOVE_ROOM_TO_ROOM )
				g_GUIMgr.SetReserveMsgBox( MB_BATTLEROOM_JOIN_FULL_USER );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );

			// 리스트 갱신
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
				if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )
					pLobbyMainWnd->RefreshBattleList();

				TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
				if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )
					pTrainingFuncWnd->RefreshBattleList();
			}
			else
			{
				QuickLobbyWnd* pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
				if( pWnd && pWnd->IsShow() )
					pWnd->RefreshBattleList();
			}
		}
		break;
	case USER_BATTLEROOM_JOIN_NOT_PASSWORD:
		{
			if( bServerMoveType == RET_MOVE_PLAZA_TO_ROOM || bServerMoveType == RET_MOVE_ROOM_TO_ROOM )
				g_GUIMgr.SetReserveMsgBox( MB_BATTLEROOM_JOIN_NOT_PASSWORD );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		}
		break;
	case USER_BATTLEROOM_JOIN_EXCEPTION:
		{
			if( bServerMoveType == RET_MOVE_PLAZA_TO_ROOM || bServerMoveType == RET_MOVE_ROOM_TO_ROOM )
				g_GUIMgr.SetReserveMsgBox( MB_BATTLEROOM_JOIN_EXCEPTION );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		}
		break;
	case USER_BATTLEROOM_JOIN_TIME_CLOSE:
		{
			if( bServerMoveType == RET_MOVE_PLAZA_TO_ROOM || bServerMoveType == RET_MOVE_ROOM_TO_ROOM )
				g_GUIMgr.SetReserveMsgBox( MB_BATTLEROOM_JOIN_TIME_CLOSE );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		}
		break;
	case USER_BATTLEROOM_JOIN_START_ROOM_ENTER_X:
		{
			if( bServerMoveType == RET_MOVE_PLAZA_TO_ROOM || bServerMoveType == RET_MOVE_ROOM_TO_ROOM )
				g_GUIMgr.SetReserveMsgBox( MB_BATTLEROOM_JOIN_START_ROOM_ENTER_X );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
		}
		break;
	case USER_BATTLEROOM_JOIN_SAFETY_ROOM:
		{
			if( bServerMoveType == RET_MOVE_PLAZA_TO_ROOM || bServerMoveType == RET_MOVE_ROOM_TO_ROOM )
				g_GUIMgr.SetReserveMsgBox( MB_BATTLEROOM_JOIN_SAFETY_ROOM );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
		}
		break;
	case USER_BATTLEROOM_JOIN_MAP_LIMIT_PLAYER:
		{
			if( bServerMoveType == RET_MOVE_PLAZA_TO_ROOM || bServerMoveType == RET_MOVE_ROOM_TO_ROOM )
				g_GUIMgr.SetReserveMsgBox( MB_BATTLEROOM_JOIN_MAP_LIMIT_PLAYER );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );

			// 리스트 갱신
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
				if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )
					pLobbyMainWnd->RefreshBattleList();

				TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
				if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )
					pTrainingFuncWnd->RefreshBattleList();
			}
			else
			{
				QuickLobbyWnd* pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
				if( pWnd && pWnd->IsShow() )
					pWnd->RefreshBattleList();
			}
		}
		break;
	case USER_BATTLEROOM_JOIN_MAP_LIMIT_GRADE:
		{			
			PartyInfoWnd *pPartyInfo = dynamic_cast<PartyInfoWnd*>(g_GUIMgr.FindWnd( PARTY_INFO_WND ));
			if( pPartyInfo )
			{
				char szGradeName[MAX_PATH] = "";
				g_LevelMgr.GetGradeName( g_ModeSelectInfo.GetSelectQuickStartModeLimitGrade( pPartyInfo->GetModeSelectIndex() ), szGradeName, MAX_PATH, true, true );
				
				if( bServerMoveType == RET_MOVE_PLAZA_TO_ROOM || bServerMoveType == RET_ROOM_TO_ROOM || bServerMoveType == RET_MOVE_ROOM_TO_ROOM )
					g_GUIMgr.SetReserveMsgBox( MB_BATTLEROOM_JOIN_MAP_LIMIT_GRADE );
				else
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10), g_ModeSelectInfo.GetSelectQuickStartModeTitle( pPartyInfo->GetModeSelectIndex() ).c_str(), szGradeName );
			}

			// 리스트 갱신
			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
				if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )
					pLobbyMainWnd->RefreshBattleList();

				TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
				if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )
					pTrainingFuncWnd->RefreshBattleList();
			}
			else
			{
				QuickLobbyWnd* pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
				if( pWnd && pWnd->IsShow() )
					pWnd->RefreshBattleList();
			}
		}
		break;
	case USER_BATTLEROOM_JOIN_NO_CHALLENGER:
		{
			if( bServerMoveType == RET_MOVE_PLAZA_TO_ROOM || bServerMoveType == RET_MOVE_ROOM_TO_ROOM )
				g_GUIMgr.SetReserveMsgBox( MB_BATTLEROOM_JOIN_NO_CHALLENGER );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "도전자를 받지 않습니다." );
		}
		break;
	}
}

void ioPlayStage::OnBattleLeaveUser( SP2Packet &rkPacket )
{
	ioHashString szLeaveUser = g_BattleRoomMgr.ApplyLeaveUserInfo( rkPacket, GetModeType() );
	
	TournamentNode* pNode =  g_TournamentMgr.GetTournament( g_BattleRoomMgr.GetTournamentIndex(), false );
	if( pNode && pNode->IsLogSend() && g_BattleRoomMgr.IsTournamentModeType() )
	{
		char szLog[MAX_PATH];
		sprintf( szLog, "[대회로그] 방 나감 패킷 받음 - %s", szLeaveUser.c_str() );

		SP2Packet kPacket2( LUPK_LOG );
		kPacket2 << "TournamentLog";  // 로그 파일 타입
		kPacket2 << szLog;
		kPacket2 << 4000;  // 오류번호:4000
		kPacket2 << false; // write db
		P2PNetwork::SendLog(kPacket2);

		if( g_App.IsMeDeveloper() )
			LOG.PrintTimeAndLog(0, "%s - %s", __FUNCTION__, szLog );
	}

	// 자신이 나감
	if( g_MyInfo.GetPublicID() == szLeaveUser )
	{
		TCPNetwork::MouseBusy( false );

		if( g_BattleRoomMgr.IsBattleRoom() )
		{
			g_App.ClearAllNetworkTable( P2P_STYLE_VOICE );
			g_VoiceChat.RemoveAllSoundPlayer();
			g_VoiceOffListManager.DeleteAll();
#ifndef SHIPPING
			LOG.PrintTimeAndLog( 0, "%s | Delete All voice", __FUNCTION__  );
#endif
			VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
			if(pWnd)
				pWnd->AllOtherVoiceOff();
			if( IsCustomPlazaMode() || GetModeType() == MT_HEADQUARTERS|| GetModeType() == MT_HOUSE )
			{
				SP2Packet kPacket( CTPK_VOICE_INFO_USER );
				if ( Setting::UseVoiceChat() && Setting::UseMIC() )
					kPacket << ID_VOICE_ON_ADD_TABLE_REQUEST;
				else
					kPacket << ID_VOICE_OFF_ADD_TABLE_REQUEST;
				ioHashString szEmptyString; // 빈스트링이면 모두에게 전송
				kPacket << szEmptyString;
				TCPNetwork::SendToServer( kPacket );

				if ( Setting::UseVoiceChat() )
					g_ChatMgr.SetSystemMsg( STR(1) );
#ifndef SHIPPING
				LOG.PrintTimeAndLog( 0, "%s CTPK_VOICE_INFO_USER | ID_VOICE_ON_ADD_TABLE_REQUEST OR ID_VOICE_OFF_ADD_TABLE_REQUEST | %s", __FUNCTION__, szEmptyString.c_str() );
#endif
			}
			else if( m_bOpenPlaza )
			{
				if ( Setting::UseVoiceChat() )
					g_ChatMgr.SetSystemMsg( STR(2) );
			}
		}

		g_BattleRoomMgr.LeaveRoom();

		g_GUIMgr.HideWnd( PARTY_SHAM_BATTLE_WND );
		g_GUIMgr.HideWnd( PARTY_INVITE_LIST_WND );
		g_GUIMgr.HideWnd( TIMEGATE_SEARCH );

		// 예약작업 처리
		if( g_App.IsNextStageLobby() )
			g_RserveTask.ExecuteReservedTask( RESERVEDCMD_WHEN_ENTERLOBBY );
	}
	else
	{
		PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
		if( pShamBattle )
		{
			pShamBattle->UpdateShamBattle();
		}

		g_App.RemoveNetworkTable( szLeaveUser , P2P_STYLE_VOICE );
		g_VoiceChat.RemoveSoundPlayer( szLeaveUser.c_str() );
		g_VoiceOffListManager.Delete( szLeaveUser );

		VoiceChatWnd *pVoice = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
		if( pVoice )
			pVoice->SetVoiceOff( szLeaveUser );
#ifndef SHIPPING
		LOG.PrintTimeAndLog( 0, "%s | Delete voice | %s", __FUNCTION__, szLeaveUser.c_str()  );
#endif
	}
}

void ioPlayStage::OnMacroCmd( SP2Packet &rkPacket )
{
	int iResult;
	rkPacket >>  iResult;
	switch( iResult )
	{
	case MACRO_ROOM_NAME_PW:
		{
			OnMacroCmdByRoomNamePw( rkPacket );
		}
		break;
	case MACRO_KICK_OUT:
		{
			OnMacroCmdByKickOut( rkPacket );
		}
		return;
	case MACRO_PLAYER_CNT:
		{
			OnMacroCmdByPayCnt( rkPacket );
		}
		break;
	case MACRO_OPTION_CHANGE:
		{
			g_BattleRoomMgr.ApplyBattleRoomOption( rkPacket, true );
			PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pShamBattle )
				pShamBattle->FixBattleModeOrderByRandomTeamOpt();
		}
		break;
	case MACRO_AI_LEVEL_CHANGE:
		{
			int iLevel;
			rkPacket >> iLevel;
			g_BattleRoomMgr.SetAiLevel( iLevel );
		}
		break;
	}

	PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
	if( pShamBattle )
	{
		pShamBattle->UpdateShamBattle();
	}
}

void ioPlayStage::OnMacroCmdByRoomNamePw( SP2Packet &rkPacket )
{
			ioHashString szName, szPW;
			rkPacket >> szName >> szPW;
			g_BattleRoomMgr.SetName( szName );
			g_BattleRoomMgr.SetPW( szPW );
		}

void ioPlayStage::OnMacroCmdByKickOut( SP2Packet &rkPacket )
		{
			ioHashString kName;
			rkPacket >> kName;
			if( g_MyInfo.GetPublicID() == kName )
			{
				ioHashString szPartyName = g_BattleRoomMgr.GetName();

				ioComplexStringPrinter kPrinter;
				g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );
				kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "%s", szPartyName.c_str() );
				kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), "전투방에서 강제퇴장.");
				g_ChatMgr.SetChatComplexString( "정보", kPrinter );
				g_BattleRoomMgr.SetKickOutRoom( g_BattleRoomMgr.GetIndex() );		
				
				if( g_BattleRoomMgr.IsBattleRoom() )
				{
					g_App.ClearAllNetworkTable( P2P_STYLE_VOICE );
					g_VoiceChat.RemoveAllSoundPlayer();
					g_VoiceOffListManager.DeleteAll();
#ifndef SHIPPING
					LOG.PrintTimeAndLog( 0, "%s | Delete All voice", __FUNCTION__  );
#endif
					VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
					if(pWnd)
						pWnd->AllOtherVoiceOff();
					if( IsCustomPlazaMode() || GetModeType() == MT_HEADQUARTERS || GetModeType() == MT_HOUSE )
					{
						SP2Packet kPacket( CTPK_VOICE_INFO_USER );
						if ( Setting::UseVoiceChat() && Setting::UseMIC() )
							kPacket << ID_VOICE_ON_ADD_TABLE_REQUEST;
						else
							kPacket << ID_VOICE_OFF_ADD_TABLE_REQUEST;
						ioHashString szEmptyString; // 빈스트링이면 모두에게 전송
						kPacket << szEmptyString;
						TCPNetwork::SendToServer( kPacket );

						if ( Setting::UseVoiceChat() )
							g_ChatMgr.SetSystemMsg( "광장 유저와 음성채팅상태로 변경." );

#ifndef SHIPPING
						LOG.PrintTimeAndLog( 0, "%s CTPK_VOICE_INFO_USER | ID_VOICE_ON_ADD_TABLE_REQUEST OR ID_VOICE_OFF_ADD_TABLE_REQUEST | %s", __FUNCTION__, szEmptyString.c_str() );
#endif
					}
					else if( m_bOpenPlaza )
					{
						if ( Setting::UseVoiceChat() )
							g_ChatMgr.SetSystemMsg( "관리자 없는 광장에서는 음성채팅 불가능." );
					}
				}

				g_BattleRoomMgr.LeaveRoom();

				g_GUIMgr.HideWnd( PARTY_SHAM_BATTLE_WND );
				g_GUIMgr.HideWnd( PARTY_INVITE_LIST_WND );

				//모드 룸에서 강퇴당하면 광장으로 이동하게되므로 
				//메시지 박스로 강퇴를 알려주지 않으면 창섭씨처럼 혼란스러워한다.
				if( GetModeType() != MT_MYROOM && GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
				{ 
					g_GUIMgr.SetReserveMsgBox( MB_BATTLE_ROOM_KICK_OUT );
				}
			}
			else
				g_BattleRoomMgr.SetKickOutUser( kName );				
		}

void ioPlayStage::OnMacroCmdByPayCnt( SP2Packet &rkPacket )
		{
			TCPNetwork::MouseBusy( false );
			int iBlueCount, iRedCount, iObserver;
			rkPacket >> iBlueCount >> iRedCount >> iObserver;
			g_BattleRoomMgr.SetMaxPlayer( iBlueCount, iRedCount, iObserver );
		}

void ioPlayStage::OnBattleRoomInviteList( SP2Packet &rkPacket )
{
	PartyInviteListWnd *pInviteWnd = dynamic_cast<PartyInviteListWnd *>(g_GUIMgr.FindWnd( PARTY_INVITE_LIST_WND ));
	if( pInviteWnd )
		pInviteWnd->SetUserList( rkPacket );
}

void ioPlayStage::OnBattleRoomInvited( SP2Packet &rkPacket )
{
	if( g_MyInfo.IsTutorialUser() ) return;
	if( g_QuestMgr.IsInvitedPass() ) return;
	if( g_BattleRoomMgr.IsBattleRoom() ) return;
	if( g_LadderTeamMgr.IsLadderTeam() ) return;
	if( g_ShuffleRoomMgr.IsShuffleRoom() ) return;
	if( g_RankBattleMgr.IsRankBattlePlaying() ) return;
	if( g_MyInfo.GetActiveCharCount() == 0 ) return;
	if( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 ) return;
	if( g_GUIMgr.IsShow( TIMEGATE_SEARCH ) ) return;

	PartyInvitedWnd *pWnd = dynamic_cast<PartyInvitedWnd*> (g_GUIMgr.FindWnd( PARTY_INVITED_WND ) );
	if( pWnd )
	{
		pWnd->SetInfo( rkPacket, this );
	}
}

void ioPlayStage::OnBattleRoomInviteResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	if( !g_BattleRoomMgr.IsBattleRoom() ) return;

	PartyInviteListWnd *pInviteWnd = dynamic_cast<PartyInviteListWnd *>(g_GUIMgr.FindWnd( PARTY_INVITE_LIST_WND ));
	if( pInviteWnd )
	{
		int iResult;
		rkPacket >> iResult;
		switch( iResult )
		{
		case BATTLEROOM_INVITE_OK:
			{
				int iSize;
				rkPacket >> iSize;
				if( iSize == 1 )
				{
					ioHashString szName;
					DWORD dwLevel, dwGuildIndex, dwGuildMark;
					rkPacket >> szName >> dwLevel >> dwGuildIndex >> dwGuildMark;
					pInviteWnd->StartInivteWait( dwLevel, szName );
				}
				else
					pInviteWnd->StartInivteWait( iSize, "" );
			}
			break;
		case BATTLEROOM_INVITE_TIME_CLOSE:
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			break;
		case BATTLEROOM_INVITE_USER_FULL:
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			break;
		}
	}
}

void ioPlayStage::OnBattleRoomEnterUserSync( SP2Packet &rkPacket )
{
	g_BattleRoomMgr.ApplyBattleRoomRecord( rkPacket );
	g_BattleRoomMgr.ApplyBattleRoomOption( rkPacket, false );
	g_BattleRoomMgr.ApplyBattleRoomEventType( rkPacket );

	PartyShamBattleWnd *pBattleRoomWnd = dynamic_cast<PartyShamBattleWnd*>( g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ) );
	if( pBattleRoomWnd )
	{
		pBattleRoomWnd->UpdateShamBattle();
	}
}

void ioPlayStage::OnShuffleRoomJoinCancel( SP2Packet &rkPacket )
{
	if( g_GUIMgr.IsShow( TIMEGATE_SEARCH_SHUFFLE ) )
		g_GUIMgr.HideWnd( TIMEGATE_SEARCH_SHUFFLE );

	if( !g_GUIMgr.IsShow( TIMEGATE_SEARCH_SHUFFLE_RESULT ) )
		g_GUIMgr.ShowWnd( TIMEGATE_SEARCH_SHUFFLE_RESULT );

	g_ShuffleRoomMgr.LeaveRoom();
}

void ioPlayStage::OnShuffleRoomCmd( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	if( g_GUIMgr.IsShow( SHUFFLE_BONUS_RESULT_WND ) )
		g_GUIMgr.HideWnd( SHUFFLE_BONUS_RESULT_WND );

	int iResult;
	rkPacket >> iResult;

	switch( iResult )
	{
	case SHUFFLEROOM_CREATE_OK:
		{
			OnShuffleRoomCmdByCreateOK( rkPacket );
		}
		break;
	case SHUFFLEROOM_READY_GO_OK:
		{
			OnShuffleRoomCmdByReadyGoOK( rkPacket );
		}
		break;
	case SHUFFLEROOM_PHASE_INFO:
		{
			g_ShuffleRoomMgr.ApplyShufflePhase( rkPacket );
		}
		break;
	case SHUFFLEROOM_MACRO_KICK_OUT:
		{
			OnShuffleRoomCmdByKickOut( rkPacket );
		}
		break;
	}
}

void ioPlayStage::OnShuffleRoomCmdByCreateOK( SP2Packet &rkPacket )
{
	g_ShuffleRoomMgr.InitShuffleCancelCount();
	g_ShuffleRoomMgr.InitGlobalSearchingShuffleCount();
	int iUserCount;
	rkPacket >> iUserCount;

	for( int i=0; i<iUserCount; ++i )
	{
		ioHashString szName;
		int iTeamType;
		rkPacket >> szName >> iTeamType;
		g_ShuffleRoomMgr.SetShuffleTeam( szName, (TeamType)iTeamType );
	}
}

void ioPlayStage::OnShuffleRoomCmdByReadyGoOK( SP2Packet &rkPacket )
{
	int iModeType = 0;
	int iSubNum = 0;
	int iMapNum = 0;
	int iPhase = 0;
	rkPacket >> iModeType >> iSubNum >> iMapNum >> iPhase;

	char szCall[MAX_PATH];
	sprintf_s( szCall, "%s - (%d), (%d), (%d), (%d)" ,__FUNCTION__, iModeType, iSubNum, iMapNum, iPhase );

	g_App.SetNextStage( ioGameStage::GS_PLAY, szCall );
	g_App.SetModeType( iModeType, iSubNum, iMapNum, PT_NONE );
	g_ShuffleRoomMgr.SetPhase( iPhase );

	g_VoiceChat.RemoveAllSoundPlayer();
	g_VoiceOffListManager.DeleteAll();
	VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
	if(pWnd)
		pWnd->AllOtherVoiceOff();
}

void ioPlayStage::OnShuffleRoomCmdByKickOut( SP2Packet &rkPacket )
{
	ioHashString kName;
	rkPacket >> kName;
	
	if( g_MyInfo.GetPublicID() == kName )
	{
		ioHashString szPartyName = g_ShuffleRoomMgr.GetName();
		g_ShuffleRoomMgr.SetKickOutRoom( g_ShuffleRoomMgr.GetIndex() );
		if( g_ShuffleRoomMgr.IsShuffleRoom() )
		{
			g_App.ClearAllNetworkTable( P2P_STYLE_VOICE );
			g_VoiceChat.RemoveAllSoundPlayer();
			g_VoiceOffListManager.DeleteAll();

			VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd( VOICE_CHAT_WND ) );
			if( pWnd )
				pWnd->AllOtherVoiceOff();
		}
		g_ShuffleRoomMgr.LeaveRoom();
		g_ChatMgr.SetKickHighLevelUserChatMsg( kName.c_str(), TEAM_BLUE );
		g_GUIMgr.SetReserveMsgBox( MB_SHUFFLE_ROOM_KICK_OUT );
	}
}

void ioPlayStage::OnShuffleRoomInfo( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	g_GUIMgr.HideWnd( TIMEGATE_SEARCH );

	bool bShuffleRoom = false;
	bool bFirstEnter = g_ShuffleRoomMgr.ApplyShuffleRoomInfo( rkPacket, bShuffleRoom );

	if ( bFirstEnter && Setting::UseVoiceChat() )
		g_ChatMgr.SetSystemMsg( "전투방 유저와 음성채팅상태로 변경." );

	if( bFirstEnter )
	{
		// 전투방
		ioMyRoomMode::SetMyRoomType( MRT_SHUFFLE );
	}
}

void ioPlayStage::OnShuffleRoomUserInfo( SP2Packet &rkPacket )
{
	g_ShuffleRoomMgr.ApplyJoinUserInfo( rkPacket );
}

void ioPlayStage::OnShuffleRoomEnterUserSync( SP2Packet &rkPacket )
{
	g_ShuffleRoomMgr.ApplyShuffleRoomRecord( rkPacket );
}

void ioPlayStage::OnShuffleRoomLeaveUser( SP2Packet &rkPacket )
{
	ioHashString szLeaveUser = g_ShuffleRoomMgr.ApplyLeaveUserInfo( rkPacket, GetModeType() );

	// 자신이 나감
	if( g_MyInfo.GetPublicID() == szLeaveUser )
	{
		TCPNetwork::MouseBusy( false );

		if( g_ShuffleRoomMgr.IsShuffleRoom() )
		{
			g_App.ClearAllNetworkTable( P2P_STYLE_VOICE );
			g_VoiceChat.RemoveAllSoundPlayer();
			g_VoiceOffListManager.DeleteAll();
#ifndef SHIPPING
			LOG.PrintTimeAndLog( 0, "%s | Delete All voice", __FUNCTION__  );
#endif
			VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
			if(pWnd)
				pWnd->AllOtherVoiceOff();
		}

		g_ShuffleRoomMgr.LeaveRoom();

		g_GUIMgr.HideWnd( TIMEGATE_SEARCH );
	}
	else
	{
		g_App.RemoveNetworkTable( szLeaveUser , P2P_STYLE_VOICE );
		g_VoiceChat.RemoveSoundPlayer( szLeaveUser.c_str() );
		g_VoiceOffListManager.Delete( szLeaveUser );

		VoiceChatWnd *pVoice = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
		if( pVoice )
			pVoice->SetVoiceOff( szLeaveUser );
#ifndef SHIPPING
		LOG.PrintTimeAndLog( 0, "%s | Delete voice | %s", __FUNCTION__, szLeaveUser.c_str()  );
#endif
	}
}

void ioPlayStage::OnBattleRoomJoinInfo( SP2Packet &rkPacket )
{
	if( g_LadderTeamMgr.IsLadderTeam() ) return;

	PartyInfoWnd *pInfoWnd = dynamic_cast<PartyInfoWnd*>(g_GUIMgr.FindWnd( PARTY_INFO_WND ));
	if( pInfoWnd )
		pInfoWnd->SetInfo( rkPacket );
}

void ioPlayStage::OnPlazaJoinInfo( SP2Packet &rkPacket )
{
	PlazaInfoWnd *pInfoWnd = dynamic_cast<PlazaInfoWnd*>(g_GUIMgr.FindWnd( PLAZA_INFO_WND ));
	if( pInfoWnd )
		pInfoWnd->SetInfo( rkPacket );
}

void ioPlayStage::OnBattleRoomCmd( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	int iResult;
	rkPacket >> iResult;
	switch( iResult )
	{
	case BATTLEROOM_CREATE_OK:
		{
			g_BattleRoomMgr.SetBattleModePlaying( false );
			g_BattleRoomMgr.InitBattleCancelCount();
			int iUserCount;
			rkPacket >> iUserCount;
			for(int i = 0;i < iUserCount;i++)
			{
				ioHashString szName;
				int iTeamType;
				rkPacket >> szName >> iTeamType;
				g_BattleRoomMgr.SetBattleTeam( szName, (TeamType)iTeamType );
			}
			PartyShamBattleWnd *pBattleRoomWnd = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pBattleRoomWnd )
			{
				pBattleRoomWnd->UpdateShamBattle();
			}
		}
		break;
	case BATTLEROOM_CREATE_FAIL:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		}
		break;
	case BATTLEROOM_TEAM_CHANGE_OK:
		{
			ioHashString szName;
			int iTeamType;
			bool bMovedOtherTeamSubSlot = false;
			rkPacket >> szName >> iTeamType;
			rkPacket >> bMovedOtherTeamSubSlot;
			g_BattleRoomMgr.SetBattleTeam( szName, (TeamType)iTeamType, bMovedOtherTeamSubSlot );

			PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pShamBattle )
			{
				pShamBattle->UpdateShamBattle();
			}
		}
		break;
	case BATTLEROOM_TEAM_CHANGE_FULL:
		{
			int iTeamType;
			rkPacket >> iTeamType;
			if( iTeamType == TEAM_BLUE )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			else if( iTeamType == TEAM_RED )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			else if( iTeamType == TEAM_NONE )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		}
		break;
	case BATTLEROOM_TEAM_CHANGE_NOT_LEVEL:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		}
		break;
	case BATTLEROOM_TEAM_CHANGE_MAP_LIMIT:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11) );
		}
		break;
	case BATTLEROOM_EVENT_TYPE_CHANGE:
		{
			bool bSafetyLevelRoom;
			int iBattleEventType, iModeIndex, iMapIndex;
			rkPacket >> iBattleEventType >> iModeIndex >> iMapIndex >> bSafetyLevelRoom;
			if( g_BattleRoomMgr.IsBattleRoom() )
			{
				g_BattleRoomMgr.SetBattleEventType( iBattleEventType );				
				g_BattleRoomMgr.SetSelectModeNMap( iModeIndex, iMapIndex, bSafetyLevelRoom );
				PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
				if( pShamBattle )
					pShamBattle->ChangeModeType();
			}
		}
		break;
	case BATTLEROOM_UI_SHOW_OK:
		{
			g_SoundMgr.StopStream();

			bool bModePlaying, bAutoStart;
			rkPacket >> bModePlaying >> bAutoStart;
			g_BattleRoomMgr.SetBattleModePlaying( bModePlaying );
			g_BattleRoomMgr.InitBattleCancelCount();
			g_GUIMgr.HideWnd( MIDDLE_JOINDELAY_WND );		

			// 마지막으로 플레이한 캐릭터 동기화
			g_MyInfo.SyncBattleNLadderCharIndex();

			PartyShamBattleWnd *pBattleRoomWnd = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pBattleRoomWnd )
			{
				pBattleRoomWnd->UpdateShamBattle();
				pBattleRoomWnd->ShowWnd();

				//자동 시작
				if( bAutoStart && g_BattleRoomMgr.GetOwnerName() == g_MyInfo.GetPublicID() )
				{
					int iBlueTeam = g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_BLUE );
					int iRedTeam  = g_BattleRoomMgr.GetBattleTeamUserCount( TEAM_RED );	
					if( GetModeType() == MT_SURVIVAL || GetModeType() == MT_BOSS || GetModeType() == MT_GANGSI || GetModeType() == MT_FIGHT_CLUB )
					{
						if( iBlueTeam + iRedTeam < 2 )      // 2인 이하면 불가능
							return;
					}
					else if( GetModeType() == MT_MONSTER_SURVIVAL || GetModeType() == MT_DUNGEON_A || GetModeType() == MT_RAID ||
						Help::IsMonsterDungeonMode(GetModeType()))
					{
						if( iBlueTeam + iRedTeam < 1 )      // 1인 이하면 불가능
							return;
					}
					else // 팀플
					{
						if( iBlueTeam + iRedTeam < 2 ) 
							return; // 최소 2명 이상이어야함

						// 양 팀에 최소 1명 이상이 있어야 가능
						if( iBlueTeam == 0 || iRedTeam == 0 )
						{
							// 서바이벌이 아니고 한쪽팀에 유저수가 0명인데 랜덤 팀이 설정되지 않음.
							if( !g_BattleRoomMgr.IsRandomTeamMode() )
								return;
						}
					}
					pBattleRoomWnd->SetModeStart( true, false );
				}
			}
		}
		break;
	case BATTLEROOM_CANCEL_OK:
		{
			g_GUIMgr.HideWnd( PARTY_SHAM_BATTLE_WND );
			
			PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pShamBattle )
				pShamBattle->UpdateShamBattle();
		}
		break;
	case BATTLEROOM_MODE_SEL_OK:
	case BATTLEROOM_MODE_SEL_FAIL:
		{
			bool bSafetyLevelRoom;
			int  iModeIndex, iMapIndex;
			rkPacket >> iModeIndex >> iMapIndex >> bSafetyLevelRoom;
			if( bSafetyLevelRoom != g_BattleRoomMgr.IsSafetyLevelRoom() )
			{
				if( bSafetyLevelRoom && g_GUIMgr.IsShow( PARTY_INVITE_LIST_WND ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
					g_GUIMgr.HideWnd( PARTY_INVITE_LIST_WND );
				}
			}

			int iPrevModeIndex = g_BattleRoomMgr.GetPreSelectModeIndex();
			int iPrevMapIndex  = g_BattleRoomMgr.GetPreSelectMapIndex();
			g_BattleRoomMgr.SetSelectModeNMap( iModeIndex, iMapIndex, bSafetyLevelRoom );
			PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pShamBattle )
			{
				pShamBattle->ChangeModeType();
				if( pShamBattle->IsChangeMaxPlayerLimit( iPrevModeIndex, iPrevMapIndex ) )
					pShamBattle->SetMaxPlayerLimit();

				PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
				if( pShamBattle && pShamBattle->IsShow() )
				{
					pShamBattle->UpdateShamBattle();
				}
			}
			if( iResult == BATTLEROOM_MODE_SEL_FAIL )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
			else if ( iResult == BATTLEROOM_MODE_SEL_OK )
			{
				// 방장이 아닐때 아래 if문 이후 함수에서 배틀모드에서 다른 모드로 넘어갈 시 유저 슬롯 정리
				if( g_BattleRoomMgr.GetOwnerName() != g_MyInfo.GetPublicID() )
				{
					PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
					if( pShamBattle )
						pShamBattle->SwitchUserSlot();			// 모드가 배틀 모드로 변경되면 일부 슬롯을 열고 닫는다.
						
				}
			}
		}
		break;
	case BATTLEROOM_START_COUNT_OK:
		{
			if( !g_BattleRoomMgr.IsBattleRoom() ) return;

			PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pShamBattle )
				pShamBattle->SetModeStart( true, false );
		}
		break;
	case BATTLEROOM_STOP_COUNT_OK:
		{
			if( !g_BattleRoomMgr.IsBattleRoom() ) return;

			PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pShamBattle )
			{
				pShamBattle->SetModeStart( false, false );
				ioHashString szName;
				rkPacket >> szName;

				ioComplexStringPrinter kPrinter;
				g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );

				if( g_BattleRoomMgr.GetBattleUserTeam( szName ) == TEAM_BLUE )
					kPrinter.SetTextColor( TCT_DEFAULT_BLUE );	
				else
					kPrinter.SetTextColor( TCT_DEFAULT_RED );	
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), szName.c_str() );
				
				kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(9) );
				g_ChatMgr.SetChatComplexString( STR(8), kPrinter );
			}
		}
		break;
	case BATTLEROOM_READY_GO_OK:
		{
			int iModeType = 0;
			int iSubNum = 0;
			int iMapNum = 0;
			rkPacket >> iModeType >> iSubNum >> iMapNum;

			char szCall[MAX_PATH];
			sprintf_s( szCall, "%s - (%d), (%d), (%d)" ,__FUNCTION__, iModeType, iSubNum, iMapNum );

			g_App.SetNextStage( ioGameStage::GS_PLAY, szCall );
			g_App.SetModeType( iModeType, iSubNum, iMapNum, PT_NONE );
			g_BattleRoomMgr.SetBattleModePlaying( true );

			//g_MyInfo.SetArenaModeCharacter( (ModeType)iModeType );
			
			// voice chat 초기화
			g_VoiceChat.RemoveAllSoundPlayer();
			g_VoiceOffListManager.DeleteAll();
			VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
			if(pWnd)
				pWnd->AllOtherVoiceOff();

			PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pShamBattle )
			{
				pShamBattle->OnBattleReadyGO();
			}
		}
		break;
	case BATTLEROOM_TOURNAMENT_INFO:
		{
			DWORD dwTourIndex, dwBlueIndex, dwRedIndex, dwStartTimer;
			rkPacket >> dwTourIndex >> dwBlueIndex >> dwRedIndex >> dwStartTimer;
			g_BattleRoomMgr.SetTournamentData( dwTourIndex, dwBlueIndex, dwRedIndex, dwStartTimer );

			PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pShamBattle && pShamBattle->IsShow() )
			{
				pShamBattle->UpdateShamBattle();
			}

			if( g_BattleRoomMgr.IsObserverUser( g_MyInfo.GetPublicID() ) )
			{
				// 내가 옵저버이면 양팀의 데이터를 가지고 옴
				g_TournamentTeamMgr.GetTeamData( dwBlueIndex );
				g_TournamentTeamMgr.GetTeamData( dwRedIndex );
			}
		}
		break;
	case BATTLEROOM_TOURNAMENT_DRAW_BYE:
		{
			TournamentMatchWinAlarmWnd* pWin = dynamic_cast<TournamentMatchWinAlarmWnd*>( g_GUIMgr.FindWnd( TOURNAMENT_MATCH_ALARM_WIN_WND ) );
			if( pWin )
			{
				pWin->SetModeType( GetModeType() );
				pWin->ShowWnd();
			}

			PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pShamBattle && pShamBattle->IsShow() )
			{
				pShamBattle->SetTournamentDrawBye();
			}		

			// 부전승이 발생했다.
			g_BattleRoomMgr.SetTournamentStartSender( true );            // 다시는 결과를 보내지 않음.

			TournamentNode* pNode =  g_TournamentMgr.GetTournament( g_BattleRoomMgr.GetTournamentIndex(), false );
			if( pNode && pNode->IsLogSend() && g_BattleRoomMgr.IsTournamentModeType() )
			{
				char szLog[MAX_PATH];
				sprintf( szLog, "[대회로그] %s : 부전승으로 인한 다음라운드 진출", g_MyInfo.GetPublicID().c_str() );

				SP2Packet kPacket2( LUPK_LOG );
				kPacket2 << "TournamentLog";  // 로그 파일 타입
				kPacket2 << szLog;
				kPacket2 << 4000;   // 오류번호:4000
				kPacket2 << false;  // write db
				P2PNetwork::SendLog(kPacket2);

				if( g_App.IsMeDeveloper() )
					LOG.PrintTimeAndLog(0, "%s - %s", __FUNCTION__, szLog );
			}
		}
		break;
	case BATTLEROOM_TOURNAMENT_START_CHEAT:
		{
			// 치트 의심 유저 이탈 시킴
			if( GetModeType() != MT_MYROOM && GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
			{
				SP2Packet kPacket( CTPK_EXIT_ROOM );
				kPacket << EXIT_ROOM_QUICK_OUT << 0 << true;
				TCPNetwork::SendToServer( kPacket );
				g_BattleRoomMgr.LeaveRoom();
			}
			else
			{
				TCPNetwork::MouseBusy( true );
				SP2Packet kPacket( CTPK_USER_BATTLEROOM_LEAVE );
				TCPNetwork::SendToServer( kPacket );
			}

			TournamentNode* pNode =  g_TournamentMgr.GetTournament( g_BattleRoomMgr.GetTournamentIndex(), false );
			if( pNode && pNode->IsLogSend() && g_BattleRoomMgr.IsTournamentModeType() )
			{
				char szLog[MAX_PATH];
				sprintf( szLog, "[대회로그] %s : 치트 유저로 의심되어 룸 이탈 처리", g_MyInfo.GetPublicID().c_str() );

				SP2Packet kPacket2( LUPK_LOG );
				kPacket2 << "TournamentLog";  // 로그 파일 타입
				kPacket2 << szLog;
				kPacket2 << 4000;   // 오류번호:4000
				kPacket2 << false;  // write db
				P2PNetwork::SendLog(kPacket2);

				if( g_App.IsMeDeveloper() )
					LOG.PrintTimeAndLog(0, "%s - %s", __FUNCTION__, szLog );
			}
		}
		break;
	case BATTLEROOM_TOURNAMENT_DELETE_TEAM:
		{
			// 예비 엔트리 선정 대기
			DWORD dwDropTeam, dwStartTimer;
			rkPacket >> dwDropTeam >> dwStartTimer; 
			g_BattleRoomMgr.SetTournamentDeleteTeam( dwDropTeam, dwStartTimer );

			PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pShamBattle && pShamBattle->IsShow() )
			{
				pShamBattle->UpdateShamBattle();
				pShamBattle->SetEntrySerchShow( true );
			}

			TournamentNode* pNode =  g_TournamentMgr.GetTournament( g_BattleRoomMgr.GetTournamentIndex(), false );
			if( pNode && pNode->IsLogSend() && g_BattleRoomMgr.IsTournamentModeType() )
			{
				char szLog[MAX_PATH];
				sprintf( szLog, "[대회로그] %s - 상대가 존재 하지 않아 예비엔트리 선정 대기", g_MyInfo.GetPublicID().c_str() );

				SP2Packet kPacket2( LUPK_LOG );
				kPacket2 << "TournamentLog";  // 로그 파일 타입
				kPacket2 << szLog;
				kPacket2 << 4000;   // 오류번호:4000
				kPacket2 << false;  // write db
				P2PNetwork::SendLog(kPacket2);

				if( g_App.IsMeDeveloper() )
					LOG.PrintTimeAndLog(0, "%s - %s", __FUNCTION__, szLog );
			}
		}
		break;
	case BATTLEROOM_TOURNAMENT_NEW_TEAM:
		{
			// 예비 엔트리 선정
			DWORD dwNewTeam;
			rkPacket >> dwNewTeam; 
			g_BattleRoomMgr.SetTournamentNewTeam( dwNewTeam );

			PartyShamBattleWnd *pShamBattle = dynamic_cast<PartyShamBattleWnd*>(g_GUIMgr.FindWnd( PARTY_SHAM_BATTLE_WND ));
			if( pShamBattle && pShamBattle->IsShow() )
			{
				pShamBattle->UpdateShamBattle();
			}

			TournamentNode* pNode =  g_TournamentMgr.GetTournament( g_BattleRoomMgr.GetTournamentIndex(), false );
			if( pNode && pNode->IsLogSend() && g_BattleRoomMgr.IsTournamentModeType() )
			{
				char szLog[MAX_PATH];
				sprintf( szLog, "[대회로그] %s : 예비엔트리 선정된 팀 : %d", g_MyInfo.GetPublicID().c_str(), dwNewTeam );

				SP2Packet kPacket2( LUPK_LOG );
				kPacket2 << "TournamentLog";  // 로그 파일 타입
				kPacket2 << szLog;
				kPacket2 << 4000;  // 오류번호:4000
				kPacket2 << false;  // write db
				P2PNetwork::SendLog(kPacket2);

				if( g_App.IsMeDeveloper() )
					LOG.PrintTimeAndLog(0, "%s - %s", __FUNCTION__, szLog );
			}
		}
		break;
	case BATTLEROOM_RUNNER_SELECT_OK:
		{
			int iBattleOrder;
			ioHashString szUserPublicID;
			rkPacket >> szUserPublicID >> iBattleOrder; 
			g_BattleRoomMgr.UpdateUserBattleOrder(szUserPublicID, iBattleOrder);
		}
		break;
	case BATTLEROOM_RUNNER_SELECT_DONE:
		{
			int iUserCount = 0;
			int iBattleOrder2;
			ioHashString szUserPublicID2;
			rkPacket >> iUserCount; 
			for(int i = 0; i < iUserCount; i++)
			{
				rkPacket >> szUserPublicID2 >> iBattleOrder2; 
				g_BattleRoomMgr.UpdateUserBattleOrder(szUserPublicID2, iBattleOrder2);
			}
		}
		break;
	}
}

void ioPlayStage::OnGuildRankList( SP2Packet &rkPacket )
{
	GuildHeroRankingListWnd *pGuildList = dynamic_cast<GuildHeroRankingListWnd*>(g_GUIMgr.FindWnd( GUILD_HERO_RANKING_LIST_WND ));
	if( pGuildList && pGuildList->IsShow() )
		pGuildList->ApplyGuildList( rkPacket );
}

void ioPlayStage::OnLadderTeamList( SP2Packet &rkPacket )
{
	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
		if( pLobbyMainWnd )
			pLobbyMainWnd->ApplyLadderTeamList( rkPacket );
	}
	else
	{
		QuickLobbyWnd* pWnd = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
		if( pWnd && pWnd->IsShow() )
			pWnd->ApplyLadderTeamList( rkPacket );
	}
}

void ioPlayStage::OnCreateLadderTeam( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult;
	rkPacket >> iResult;
	switch( iResult )
	{
	case LADDERTEAM_CREATE_NOT:
		break;
	case LADDERTEAM_CREATE_CAMP_END:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), g_CampMgr.GetPlayPreceedMent().c_str() );
		}
		break;
	}
}

void ioPlayStage::OnLadderTeamInfo( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	g_GUIMgr.HideWnd( PARTY_INVITED_WND );
	g_GUIMgr.HideWnd( PARTY_INFO_WND );
	g_GUIMgr.HideWnd( TIMEGATE_SEARCH );

	bool bFristEnter = g_LadderTeamMgr.ApplyLadderTeamInfo( rkPacket );

	if( bFristEnter && ( IsCustomPlazaMode() || GetModeType() == MT_HEADQUARTERS || GetModeType() == MT_HOUSE ) )
	{
		// 룸에 첫 접속
		g_App.ClearAllNetworkTable( P2P_STYLE_VOICE ); 
		g_VoiceChat.RemoveAllSoundPlayer();
		g_VoiceOffListManager.DeleteAll();
		VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
		if(pWnd)
			pWnd->AllOtherVoiceOff();

		SP2Packet kPacket( CTPK_VOICE_INFO_USER );
		kPacket << ID_VOICE_REMOVE_TABLE;
		ioHashString szEmptyString; // 빈스트링이면 모두에게 전송
		kPacket << szEmptyString;
		TCPNetwork::SendToServer( kPacket );
#ifndef SHIPPING
		LOG.PrintTimeAndLog( 0, "%s Delate all voice and CTPK_VOICE_INFO_USER | ID_VOICE_REMOVE_TABLE | %s", __FUNCTION__, szEmptyString.c_str() );
#endif
	}

	if ( bFristEnter && Setting::UseVoiceChat() )
		g_ChatMgr.SetSystemMsg( STR(1) );

	if( bFristEnter )
	{
		//래더, 배틀, 대회 전투방 등은 로비로 이동시킴
		// 진영팀
		ioMyRoomMode::SetMyRoomType( MRT_LADDER );

		// 로비로 이탈
		if( GetModeType() == MT_TRAINING || GetModeType() == MT_HEADQUARTERS || GetModeType() == MT_HOUSE )
		{
			SP2Packet kPacket( CTPK_EXIT_ROOM );
			kPacket << EXIT_ROOM_LOBBY << 0 << true;
			TCPNetwork::SendToServer( kPacket );
			TCPNetwork::MouseBusy( true );
		}
		else
		{
			g_App.SetNextStage( ioGameStage::GS_LOBBY, __FUNCTION__ );
			g_App.SetModeType( 0, 0, 0, PT_NONE );

		}		
	}
	else
	{
		LadderBattleWnd *pLadderBattleWnd = dynamic_cast<LadderBattleWnd *>(g_GUIMgr.FindWnd(LADDER_BATTLE_WND));
		if( pLadderBattleWnd )
		{
			pLadderBattleWnd->ShowWnd();
			pLadderBattleWnd->UpdateLadderBattle();
		}
	}
}

void ioPlayStage::OnLadderTeamUserInfo( SP2Packet &rkPacket )
{
	ioHashString szJoinName = g_LadderTeamMgr.ApplyJoinUserInfo( rkPacket );
	
	LadderBattleWnd *pLadderBattleWnd = dynamic_cast<LadderBattleWnd *>(g_GUIMgr.FindWnd(LADDER_BATTLE_WND));
	if( pLadderBattleWnd )
		pLadderBattleWnd->UpdateLadderBattle();
}

void ioPlayStage::OnLadderTeamLeave( SP2Packet &rkPacket )
{
	ioHashString szLeaveUser = g_LadderTeamMgr.ApplyLeaveUserInfo( rkPacket );

	// 자신이 나감
	if( g_MyInfo.GetPublicID() == szLeaveUser )
	{
		TCPNetwork::MouseBusy( false );

		if( g_LadderTeamMgr.IsLadderTeam() )
		{
			g_App.ClearAllNetworkTable( P2P_STYLE_VOICE );
			g_VoiceChat.RemoveAllSoundPlayer();
			g_VoiceOffListManager.DeleteAll();
#ifndef SHIPPING
			LOG.PrintTimeAndLog( 0, "%s | Delete All voice", __FUNCTION__  );
#endif
			VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
			if(pWnd)
				pWnd->AllOtherVoiceOff();
			if( IsCustomPlazaMode() || GetModeType() == MT_HEADQUARTERS || GetModeType() == MT_HOUSE )
			{
				SP2Packet kPacket( CTPK_VOICE_INFO_USER );
				if ( Setting::UseVoiceChat() && Setting::UseMIC() )
					kPacket << ID_VOICE_ON_ADD_TABLE_REQUEST;
				else
					kPacket << ID_VOICE_OFF_ADD_TABLE_REQUEST;
				ioHashString szEmptyString; // 빈스트링이면 모두에게 전송
				kPacket << szEmptyString;
				TCPNetwork::SendToServer( kPacket );

				if ( Setting::UseVoiceChat() )
					g_ChatMgr.SetSystemMsg( STR(1) );
#ifndef SHIPPING
				LOG.PrintTimeAndLog( 0, "%s CTPK_VOICE_INFO_USER | ID_VOICE_ON_ADD_TABLE_REQUEST OR ID_VOICE_OFF_ADD_TABLE_REQUEST | %s", __FUNCTION__, szEmptyString.c_str() );
#endif
			}
			else if( m_bOpenPlaza )
			{
				if ( Setting::UseVoiceChat() )
					g_ChatMgr.SetSystemMsg( STR(2) );
			}
		}
		g_LadderTeamMgr.LeaveTeam();
	}
	else
	{
		LadderBattleWnd *pLadderBattleWnd = dynamic_cast<LadderBattleWnd *>(g_GUIMgr.FindWnd(LADDER_BATTLE_WND));
		if( pLadderBattleWnd )
			pLadderBattleWnd->UpdateLadderBattle();

		g_App.RemoveNetworkTable( szLeaveUser , P2P_STYLE_VOICE );
		g_VoiceChat.RemoveSoundPlayer( szLeaveUser.c_str() );
		g_VoiceOffListManager.Delete( szLeaveUser );

		VoiceChatWnd *pVoice = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
		if( pVoice )
			pVoice->SetVoiceOff( szLeaveUser );

#ifndef SHIPPING
		LOG.PrintTimeAndLog( 0, "%s | Delete voice | %s ", __FUNCTION__, szLeaveUser.c_str()  );
#endif
	}
}

void ioPlayStage::OnLadderTeamMacro( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	int iMacro;
	rkPacket >> iMacro;
	switch( iMacro )
	{
	case LADDERTEAM_MACRO_CHANGE_INFO:
		{
			ioHashString szName, szPW;
			DWORD dwJoinGuildIndex;
			rkPacket >> szName >> szPW >> dwJoinGuildIndex;
			g_LadderTeamMgr.SetLadderTeamName( szName );
			g_LadderTeamMgr.SetLadderTeamPW( szPW );
			g_LadderTeamMgr.SetJoinGuildIndex( dwJoinGuildIndex );
		}
		break;
	case LADDERTEAM_MACRO_MAX_PLAYER:
		{	
			int iChangeMaxPlayer;
			rkPacket >> iChangeMaxPlayer;
			g_LadderTeamMgr.SetMaxPlayer( iChangeMaxPlayer );
		}
		break;
	case LADDERTEAM_MACRO_KICK_OUT:
		{
			ioHashString kName;
			rkPacket >> kName;
			if( g_MyInfo.GetPublicID() == kName )
			{
				ioComplexStringPrinter kPrinter;
				g_ChatMgr.PresetChatComplexString( MSG_INFO_CHAT, kPrinter );	

				kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1), g_LadderTeamMgr.GetLadderTeamName().c_str() );
				kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2) );
				g_ChatMgr.SetChatComplexString( STR(5), kPrinter );
				g_LadderTeamMgr.SetKickOutRoom( g_LadderTeamMgr.GetLadderTeamIndex() );	
				if( g_LadderTeamMgr.IsLadderTeam() )
				{
					g_App.ClearAllNetworkTable( P2P_STYLE_VOICE );
					g_VoiceChat.RemoveAllSoundPlayer();
					g_VoiceOffListManager.DeleteAll();
#ifndef SHIPPING
					LOG.PrintTimeAndLog( 0, "%s | Delete All voice", __FUNCTION__  );
#endif
					VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
					if( pWnd )
						pWnd->AllOtherVoiceOff();
					if( IsCustomPlazaMode() || GetModeType() == MT_HEADQUARTERS || GetModeType() == MT_HOUSE )
					{
						SP2Packet kPacket( CTPK_VOICE_INFO_USER );
						if ( Setting::UseVoiceChat() && Setting::UseMIC() )
							kPacket << ID_VOICE_ON_ADD_TABLE_REQUEST;
						else
							kPacket << ID_VOICE_OFF_ADD_TABLE_REQUEST;
						ioHashString szEmptyString; // 빈스트링이면 모두에게 전송
						kPacket << szEmptyString;
						TCPNetwork::SendToServer( kPacket );

						if ( Setting::UseVoiceChat() )
							g_ChatMgr.SetSystemMsg( STR(6) );

#ifndef SHIPPING
						LOG.PrintTimeAndLog( 0, "%s CTPK_VOICE_INFO_USER | ID_VOICE_ON_ADD_TABLE_REQUEST OR ID_VOICE_OFF_ADD_TABLE_REQUEST | %s", __FUNCTION__, szEmptyString.c_str() );
#endif
					}
					else if( m_bOpenPlaza )
					{
						if ( Setting::UseVoiceChat() )
							g_ChatMgr.SetSystemMsg( STR(7) );
					}
				}
				g_LadderTeamMgr.LeaveTeam();
				
				//모드 룸에서 강퇴당하면 광장으로 이동하게되므로 
				//메시지 박스로 강퇴를 알려주지 않으면 창섭씨처럼 혼란스러워한다.
				if( GetModeType() != MT_MYROOM && GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
				{ 
					g_GUIMgr.SetReserveMsgBox( MB_LADDER_TEAM_KICK_OUT );
				}
			}
			else
			{
				if( !kName.IsEmpty() )
					g_LadderTeamMgr.SetKickOutUser( kName );

                if( g_LadderTeamMgr.GetLadderTeamOwner() == g_MyInfo.GetPublicID() )
					TCPNetwork::MouseBusy( false );
			}
		}
		break;
	case LADDERTEAM_MACRO_BATTLE_INFO:
		{
			DWORD dwJoinGuildIndex;
			bool bSearchLevelMatch, bSearchSameUser, bBadPingKick;
			int iModeSelectIndex, iMapSelectIndex;
			rkPacket >> iModeSelectIndex >> iMapSelectIndex >> dwJoinGuildIndex >> bSearchLevelMatch >> bSearchSameUser >> bBadPingKick;
			
			g_LadderTeamMgr.SetSelectModeNMap( iModeSelectIndex, iMapSelectIndex );
			g_LadderTeamMgr.SetJoinGuildIndex( dwJoinGuildIndex );
			g_LadderTeamMgr.SetSearchLevelMatch( bSearchLevelMatch );
			g_LadderTeamMgr.SetSearchSameUser( bSearchSameUser );
			g_LadderTeamMgr.SetBadPingKick( bBadPingKick );
		}
		break;
	case LADDERTEAM_MACRO_MODE_SEL:
		{
			int iModeSelectIndex, iMapSelectIndex;
			rkPacket >> iModeSelectIndex >> iMapSelectIndex;
			g_LadderTeamMgr.SetSelectModeNMap( iModeSelectIndex, iMapSelectIndex );
		}
		break;
	case LADDERTEAM_MACRO_CHANGE_OPTION:
		{
			bool bSearchLevelMatch, bSearchSameUser, bBadPingKick;
			rkPacket >> bSearchLevelMatch >> bSearchSameUser >> bBadPingKick;
			if( g_LadderTeamMgr.IsLadderTeam() )
			{
				g_LadderTeamMgr.SetSearchLevelMatch( bSearchLevelMatch );
				g_LadderTeamMgr.SetSearchSameUser( bSearchSameUser );
				g_LadderTeamMgr.SetBadPingKick( bBadPingKick );
			}
		}
		break;
	case LADDERTEAM_MACRO_TEAM_STATE:
		{
			TCPNetwork::MouseBusy( false );
			DWORD dwState;
			int iChangeMaxPlayer;
			rkPacket >> dwState >> iChangeMaxPlayer;
			g_LadderTeamMgr.SetTeamState( dwState );
			g_LadderTeamMgr.SetMaxPlayer( iChangeMaxPlayer );

			if( dwState == ioLadderTeamMgr::TMS_SEARCH_RESERVE )
			{
				DWORD dwFullTime;
				rkPacket >> dwFullTime;
				LadderBattleWnd *pLadderBattleWnd = dynamic_cast<LadderBattleWnd *>(g_GUIMgr.FindWnd(LADDER_BATTLE_WND));
				if( pLadderBattleWnd )
					pLadderBattleWnd->StartSearch( dwFullTime );
			}
		}
		break;
	case LADDERTEAM_MACRO_SEARCH_CANCEL_FAILED:
		{
			TCPNetwork::MouseBusy( false );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		}
		break;
	case LADDERTEAM_MACRO_SEARCH_FAILED:
		{
			int iChangeMaxPlayer;
			rkPacket >> iChangeMaxPlayer;
			g_LadderTeamMgr.SetMaxPlayer( iChangeMaxPlayer );
			g_LadderTeamMgr.MatchSearchFailed();
		}
		break;
	case LADDERTEAM_MACRO_CHANGE_GUILD:
		{
			DWORD dwGuildIndex;
			rkPacket >> dwGuildIndex;
			g_LadderTeamMgr.SetGuildIndex( dwGuildIndex );
		}
		break;
	case LADDERTEAM_MACRO_MODE_READY_GO:
		{
			int iModeType = 0;
			int iSubNum = 0;
			int iMapNum = 0;
			int iCampType = CAMP_NONE;
			int iTeamType = TEAM_NONE;
			rkPacket >> iModeType >> iSubNum >> iMapNum >> iCampType >> iTeamType;

			char szCall[MAX_PATH];
			sprintf_s( szCall, "%s - (%d), (%d), (%d)" ,__FUNCTION__, iModeType, iSubNum, iMapNum );

			g_App.SetNextStage( ioGameStage::GS_PLAY, szCall );
			g_App.SetModeType( iModeType, iSubNum, iMapNum, PT_NONE );
			g_LadderTeamMgr.SetLadderBattleModePlaying( true );
			LOG.PrintTimeAndLog( 0, "%s g_LadderTeamMgr.SetOtherTypeBeforeLoading CampType : %d TeamType : %d", __FUNCTION__, iCampType, iTeamType );
			g_LadderTeamMgr.SetOtherTypeBeforeLoading( iCampType, iTeamType );

			// voice chat 초기화
			g_VoiceChat.RemoveAllSoundPlayer();
			g_VoiceOffListManager.DeleteAll();
			VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
			if(pWnd)
				pWnd->AllOtherVoiceOff();			
		}
		break;
	case LADDERTEAM_MACRO_MODE_END:
		{
			int iBlue,iRed;
			rkPacket >> iBlue >> iRed;
			if( iBlue != 0 && iRed != 0 )
			{
				g_GUIMgr.HideWnd( TOTAL_RESULT_MAIN_WND );
				g_GUIMgr.HideWnd( TIME_SCORE_WND );
				g_GUIMgr.HideWnd( KO_CHAT_MSG_WND );
				g_GUIMgr.ShowWnd( LADDER_BATTLE_WND );

				if( g_LadderTeamMgr.GetLadderTeamOwner() == g_MyInfo.GetPublicID() )
				{
					//임시 진영
					g_GUIMgr.SetMsgBox( MB_YESNO, g_GUIMgr.FindWnd( LADDER_TEAM_INVITE_LIST_WND ), "restart?" );
				}
			}
			else
			{
				g_LadderTeamMgr.SetLadderBattleModePlaying( false );

				// 로비 진영 스테이지로 이동
				ioMyRoomMode::SetMyRoomType( MRT_LADDER );
				SP2Packet kPacket( CTPK_LADDER_USER_HQ_MOVE );
				TCPNetwork::SendToServer( kPacket );
			}
		}
		break;
	case LADDERTEAM_MACRO_CAMP_BATTLE_END:
		{
			SP2Packet kPacket( CTPK_CAMP_BATTLE_END_LEAVE_TEAM );
			TCPNetwork::SendToServer( kPacket );			
			g_LadderTeamMgr.LeaveTeam();
			if( GetRoomStyle() == RSTYLE_LADDERBATTLE )
				g_GUIMgr.SetReserveMsgBox( MB_CAMP_BATTLE_END );
			else
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4), g_CampMgr.GetPlayPreceedMent().c_str() );
			}
			return;
		}
		break;
	}

	LadderBattleWnd *pLadderBattleWnd = dynamic_cast<LadderBattleWnd *>(g_GUIMgr.FindWnd(LADDER_BATTLE_WND));
	if( pLadderBattleWnd )
		pLadderBattleWnd->UpdateLadderBattle();
}

void ioPlayStage::OnLadderTeamJoinInfo( SP2Packet &rkPacket )
{
	if( GetModeType() != MT_TRAINING && GetModeType() != MT_MYROOM && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE ) return;	

	LadderTeamInfoWnd *pInfoWnd = dynamic_cast<LadderTeamInfoWnd *>(g_GUIMgr.FindWnd(LADDER_TEAM_INFO_WND));
	if( pInfoWnd )
		pInfoWnd->SetInfo( rkPacket );
}

void ioPlayStage::OnJoinLadderTeam( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	int iResult;
	rkPacket >> iResult;
	switch( iResult )
	{
	case LADDERTEAM_ALREADY_TEAM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		break;
	case LADDERTEAM_ALREADY_ROOM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		break;
	case LADDERTEAM_NOT_CHAR:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		break;
	case LADDERTEAM_FORMALITY:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		break;
	case LADDERTEAM_FULL:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		break;
	case LADDERTEAM_NOT_GUILD:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		break;
	case LADDERTEAM_NOT_LADDERTEAM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
		break;
	case LADDERTEAM_JOIN_MISS_PASSWORD:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
		break;
	case LADDERTEAM_JOIN_NOT_LEVELMATCH:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );
		break;
	case LADDERTEAM_JOIN_MATCH_PLAY:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10) );
		break;
	case LADDERTEAM_JOIN_NOT_CAMP:
		if( g_MyInfo.GetUserCampPos() == 0 )
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(11) );
		else
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(12) );
		break;
	case LADDERTEAM_JOIN_CAMP_END:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(13), g_CampMgr.GetPlayPreceedMent().c_str() );
		break;
	}
}

void ioPlayStage::OnLadderTeamInviteList( SP2Packet &rkPacket )
{
	LadderTeamInviteListWnd *pInviteWnd = dynamic_cast<LadderTeamInviteListWnd *>(g_GUIMgr.FindWnd( LADDER_TEAM_INVITE_LIST_WND ));
	if( pInviteWnd )
		pInviteWnd->SetUserList( rkPacket );
}

void ioPlayStage::OnLadderTeamInvited( SP2Packet &rkPacket )
{
	if( g_MyInfo.GetUserCampPos() == 0 ) return;
	if( g_MyInfo.IsTutorialUser() ) return;
	if( g_QuestMgr.IsInvitedPass() ) return;
	if( g_BattleRoomMgr.IsBattleRoom() ) return;
	if( g_LadderTeamMgr.IsLadderTeam() ) return;
	if( g_ShuffleRoomMgr.IsShuffleRoom() ) return;
	if( g_RankBattleMgr.IsRankBattlePlaying() ) return;
	if( g_MyInfo.GetActiveCharCount() == 0 ) return;
	if( g_MyInfo.GetActiveCharCount() == 1 && g_MyInfo.GetActiveExerciseCharCount() == 1 ) return;
	if( g_MyInfo.GetGradeLevel() < g_LadderTeamMgr.GetLadderTeamLimitGrade() ) return;
	if( g_GUIMgr.IsShow( TIMEGATE_SEARCH ) ) return;

	LadderTeamInvitedWnd *pInvitedWnd = dynamic_cast<LadderTeamInvitedWnd*> (g_GUIMgr.FindWnd( LADDER_TEAM_INVITED_WND ) );
	if( pInvitedWnd )
	{
		pInvitedWnd->SetInfo( rkPacket );
	}
}

void ioPlayStage::OnLadderTeamRanking( SP2Packet &rkPacket )
{
	int iTeamRanking;
	DWORD dwTeamIndex;
	rkPacket >> dwTeamIndex >> iTeamRanking;
	g_LadderTeamMgr.SetTeamRanking( dwTeamIndex, iTeamRanking );

	LadderBattleWnd *pLadderBattleWnd = dynamic_cast<LadderBattleWnd *>(g_GUIMgr.FindWnd(LADDER_BATTLE_WND));
	if( pLadderBattleWnd )
		pLadderBattleWnd->UpdateLadderBattle();	
}

void ioPlayStage::OnLadderOtherTeamInfo( SP2Packet &rkPacket )
{
	g_LadderTeamMgr.FillOtherTeamInfo( rkPacket );
}

void ioPlayStage::OnLadderOtherNameChange( SP2Packet &rkPacket )
{
	DWORD dwTeamIndex;
	ioHashString szTeamName;
	rkPacket >> dwTeamIndex >> szTeamName;
	g_LadderTeamMgr.SetLadderOtherTeamName( dwTeamIndex, szTeamName );
}

void ioPlayStage::OnLadderTeamRankList( SP2Packet &rkPacket )
{
	LadderRankingListWnd *pRankList = dynamic_cast<LadderRankingListWnd*>(g_GUIMgr.FindWnd( LADDER_RANKING_LIST_WND ));
	if( pRankList && pRankList->IsShow() )
		pRankList->ApplyRankingList( rkPacket );
}

void ioPlayStage::OnCustomSoundInfo( SP2Packet &rkPacket )
{
	int   iType = 0;
	DWORD dwUserIndex = 0;
	rkPacket >> iType;
	rkPacket >> dwUserIndex;

	ioHashString szPubicID;
	g_App.GetNetworkTablePublicID( dwUserIndex, szPubicID );
	if( szPubicID.IsEmpty() )
		return;
	if( g_CustomSoundMgr.IsDisable( szPubicID ) )
		return;
	if( Setting::UseBlockUsersCustomSound() )
		return;

	if( iType == HAVE_CUSTOM_SOUND )
	{
		DWORD dwCustomSoundIndex = 0;
		DWORD dwCustomSoundCRC   = 0;
		DWORD dwCustomSoundSize  = 0;
		rkPacket >> dwCustomSoundIndex;
		rkPacket >> dwCustomSoundCRC;
		rkPacket >> dwCustomSoundSize;

		char szRootDir[MAX_PATH]="";
		::GetCurrentDirectory(MAX_PATH,szRootDir);
		char szFullPath[MAX_PATH*2]="";
		StringCbPrintf( szFullPath, sizeof(szFullPath), "%s\\Usersound\\_Usersoundother\\%s\\%d.wav", szRootDir, szPubicID.c_str(), dwCustomSoundIndex );

		DWORD dwCurCRC = 0;
		CCrc32Static::FileCrc32Streams( szFullPath, dwCurCRC );

		if( dwCustomSoundCRC == dwCurCRC )
		{
			g_CustomSoundMgr.PlayByPath( szFullPath );
		}
		else
		{
			g_CustomSoundMgr.AddCustomSoundData( dwUserIndex, szPubicID, dwCustomSoundIndex, dwCustomSoundSize );
			SP2Packet kPacket( CUPK_CUSTOM_SOUND_INFO );
			kPacket << DONT_HAVE_CUSTOM_SOUND;
			kPacket << g_MyInfo.GetUserIndex();
			kPacket << dwCustomSoundIndex;
			P2PNetwork::SendToUserByIndex( dwUserIndex, kPacket );			
		}
	}
	else if( iType == DONT_HAVE_CUSTOM_SOUND )
	{
		DWORD dwCustomSoundIndex = 0;
		rkPacket >> dwCustomSoundIndex;
		g_CustomSoundMgr.AddSendNetworkInfo( dwCustomSoundIndex, dwUserIndex );
	}
}

void ioPlayStage::OnCustomSoundData( SP2Packet &rkPacket )
{
	DWORD dwUserIndex = 0;
	DWORD dwCustomSoundIndex = 0;
	rkPacket >> dwUserIndex;
	rkPacket >> dwCustomSoundIndex;

	ioHashString szPubicID;
	g_App.GetNetworkTablePublicID( dwUserIndex, szPubicID );
	if( szPubicID.IsEmpty() )
		return;

	CustomSoundPacket kData;
	rkPacket >> kData;
	if( g_CustomSoundMgr.UpdateCustomSoundData( dwUserIndex, dwCustomSoundIndex, kData ) )
	{
		if( g_CustomSoundMgr.IsDisable( szPubicID ) )
			return;
		if( Setting::UseBlockUsersCustomSound() )
			return;

		char szRootDir[MAX_PATH]="";
		::GetCurrentDirectory(MAX_PATH,szRootDir);
		char szFullPath[MAX_PATH*2]="";
		StringCbPrintf( szFullPath, sizeof(szFullPath), "%s\\Usersound\\_Usersoundother\\%s\\%d.wav", szRootDir, szPubicID.c_str(), dwCustomSoundIndex );
		g_CustomSoundMgr.PlayByPath( szFullPath );	
	}
}

void ioPlayStage::OnJumpAutoShot( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	int iCurJumpType;
	rkPacket >> iCurJumpType;

	pChar->SetCurJumpType( (JumpType) iCurJumpType );
	pChar->SetJumpAutoShotState();
}

void ioPlayStage::OnExcavationUDP( SP2Packet &rkPacket )
{
	g_ExcavationMgr.OnExcavationUDPPacket( rkPacket, this );
}

void ioPlayStage::ReCalculateSymbolHP( int iCharCnt )
{
	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioSymbolStruct *pSymbol = ToSymbolStruct( *iter );
		if( pSymbol )
		{
			pSymbol->SetAddHP( iCharCnt );
		}
	}
}

void ioPlayStage::SetMemberBalanceMsg()
{
	if( !m_pPlayMode ) return;

	ModeType eType = GetModeType();

	if( eType == MT_TRAINING || eType == MT_MYROOM ||  eType == MT_HEADQUARTERS || eType == MT_HOUSE || 
		eType == MT_MONSTER_SURVIVAL || eType == MT_DUNGEON_A || eType == MT_RAID || 
		Help::IsMonsterDungeonMode(eType))
		return;

	if( g_BattleRoomMgr.IsTournamentModeType() )
		return;        // 경기방은 보정 없음

	if( m_pPlayMode->GetModeState() != ioPlayMode::MS_PLAY )
		return;

	DWORD dwCurTime = FRAMEGETTIME();

	int iRedTeamCnt, iBlueTeamCnt;
	iRedTeamCnt = iBlueTeamCnt = 0;

	if( eType == MT_CATCH || eType == MT_CATCH_RUNNINGMAN || eType == MT_ARENA ||
		eType == MT_UNDERWEAR || eType == MT_CBT || eType == MT_FARMING )
	{
		iRedTeamCnt = m_iRedForBalance;
		iBlueTeamCnt = m_iBlueForBalance;
	}
	else
	{
		int iCharCnt = m_pPlayMode->GetRecordCharCnt();
		for( int i=0; i < iCharCnt; i++ )
		{
			ioBaseChar *pChar = m_pPlayMode->GetRecordChar( i );
			if( !pChar ) continue;
			if( pChar->GetState() == CS_VIEW ) continue;
			if( pChar->GetState() == CS_LOADING ) continue;
			if( pChar->GetState() == CS_OBSERVER ) continue;

			if( pChar->GetTeam() == TEAM_RED )
				iRedTeamCnt++;
			else if( pChar->GetTeam() == TEAM_BLUE )
				iBlueTeamCnt++;
		}
	}

	// 배틀모드일때는 아래 팀 보정 로직 돌아가지 않음
	if(m_pPlayMode->GetModeType() == MT_BATTLE)
		return;

	if( iRedTeamCnt != 0 && iBlueTeamCnt != 0 )
	{
		if( m_pPlayMode->IsEnableBalanceMsg() && (iRedTeamCnt != m_iPreRedCnt || iBlueTeamCnt != m_iPreBlueCnt) )
		{
			if( iRedTeamCnt != iBlueTeamCnt && m_iPreRedCnt != 0 && m_iPreBlueCnt != 0 )
			{
				int iGap = abs( m_iPreRedCnt - m_iPreBlueCnt );
				if( iRedTeamCnt > iBlueTeamCnt )
				{
					if( iRedTeamCnt-iBlueTeamCnt > iGap )
						g_ChatMgr.SetMemberBalanceMsg( TEAM_BLUE );
				}
				else if( iRedTeamCnt < iBlueTeamCnt )
				{
					if( iBlueTeamCnt-iRedTeamCnt > iGap )
						g_ChatMgr.SetMemberBalanceMsg( TEAM_RED );
				}
			}
		}

		m_iPreRedCnt = iRedTeamCnt;
		m_iPreBlueCnt = iBlueTeamCnt;
	}
}

void ioPlayStage::ReCalculateDamageRate( bool bRestart )
{
	m_fMemberBalanceBlue = m_fMemberBalanceRed = FLOAT1;
	m_fMemberBalanceBlueForGauge = m_fMemberBalanceRedForGauge = FLOAT1;
	m_fMemberBalanceBlueForBlowTime = m_fMemberBalanceRedForBlowTime = FLOAT1;
	m_iMemberBalanceRedCntForBuff = m_iMemberBalanceBlueCntForBuff = 1;
	m_fMemberBalanceRedForSpeed = m_fMemberBalanceBlueForSpeed = FLOAT1;
    m_vSingleTeamBalance.clear();

	if( !m_pPlayMode )	return;
	if( g_BattleRoomMgr.IsTournamentModeType() )
		return;        // 경기방은 보정 없음

	int iCharCnt = m_pPlayMode->GetRecordCharCnt();
	if( GetModeType() == MT_TRAINING && GetPlazaModeType() == PT_GUILD )       //길드 광장 밸런스...
	{
		// 팀 추출
		int i = 0;
		int iTeamCount = 0;
		for(i = 0;i < iCharCnt;i++)
		{
			ioBaseChar *pChar = m_pPlayMode->GetRecordChar( i );
			if( !pChar ) continue;
			if( pChar->GetState() == CS_VIEW ) continue;
			if( pChar->GetState() == CS_LOADING ) continue;
			if( pChar->GetState() == CS_OBSERVER ) continue;
			
			// 싱글팀 ..
			bool bNewTeam = true;
			for(iTeamCount = 0;iTeamCount < (int)m_vSingleTeamBalance.size();iTeamCount++)
			{
				SingleTeamBalance &rSingleTeam = m_vSingleTeamBalance[iTeamCount];
				if( rSingleTeam.m_eTeam == pChar->GetTeam() )
				{
					rSingleTeam.m_iUserCount++;
					bNewTeam = false;
					break;
				}
			}			
			if( bNewTeam )
			{
				SingleTeamBalance kSingleTeam;
				kSingleTeam.m_eTeam = pChar->GetTeam();
				kSingleTeam.m_iUserCount = 1;
				m_vSingleTeamBalance.push_back( kSingleTeam );
			}
		}
		
		// 가장 인원이 많은 팀
		int isBigTeamUser = 0;
		for(iTeamCount = 0;iTeamCount < (int)m_vSingleTeamBalance.size();iTeamCount++)
		{
			SingleTeamBalance &rSingleTeam = m_vSingleTeamBalance[iTeamCount];
			if( rSingleTeam.m_iUserCount >= isBigTeamUser )
			{
				isBigTeamUser = rSingleTeam.m_iUserCount;
			}
		}			

		// 팀 인원에 따른 비율
		// 데미지 배율 = 최대인원팀의 인원수 / 자기팀원수
		// 게이지 배율 = 최대인원팀의 인원수 / 자기팀원수
		for(iTeamCount = 0;iTeamCount < (int)m_vSingleTeamBalance.size();iTeamCount++)
		{
			SingleTeamBalance &rSingleTeam = m_vSingleTeamBalance[iTeamCount];
			if( rSingleTeam.m_iUserCount <= 0 ) continue;

			rSingleTeam.m_fDamageBalance = (float)isBigTeamUser / (float)rSingleTeam.m_iUserCount;
			rSingleTeam.m_fGaugeBalance  = (float)isBigTeamUser / (float)rSingleTeam.m_iUserCount;
		}			
	}
	else             //기존 모드 밸런스...
	{
		int iRedTeamCnt, iBlueTeamCnt;
		iRedTeamCnt = iBlueTeamCnt = 0;
		if( GetModeType() == MT_TRAINING || GetModeType() == MT_SURVIVAL || GetModeType() == MT_HEADQUARTERS || GetModeType() == MT_HOUSE || GetModeType() == MT_FIGHT_CLUB || GetModeType() == MT_SHUFFLE_BONUS )
		{
			iRedTeamCnt = iBlueTeamCnt = 1;

			m_iRedForBalance = iRedTeamCnt;
			m_iBlueForBalance = iBlueTeamCnt;
		}
		else if( ( GetModeType() == MT_CATCH || GetModeType() == MT_CATCH_RUNNINGMAN || GetModeType() == MT_ARENA
			|| GetModeType() == MT_UNDERWEAR || GetModeType() == MT_CBT || GetModeType() == MT_FARMING ) && !bRestart )
		{
			iRedTeamCnt = m_iRedForBalance;
			iBlueTeamCnt = m_iBlueForBalance;
		}
		else
		{
			if( m_pPlayMode->GetModeType() == MT_MONSTER_SURVIVAL )
			{
				for( int i=0; i < m_pPlayMode->GetRecordCharCnt(); i++ )
				{
					ioBaseChar *pChar = m_pPlayMode->GetRecordChar( i );
					if( !pChar ) continue;
					if( pChar->GetState() == CS_LOADING ) continue;
					if( pChar->GetState() == CS_OBSERVER ) continue;

					if( pChar->GetTeam() == TEAM_BLUE )
						iBlueTeamCnt++;
				}
				iRedTeamCnt = 1;         //몬스터는 무조건 1명
			}
			else if( m_pPlayMode->GetModeType() == MT_DUNGEON_A )
			{
				for( int i=0; i < m_pPlayMode->GetRecordCharCnt(); i++ )
				{
					ioBaseChar *pChar = m_pPlayMode->GetRecordChar( i );
					if( !pChar ) continue;
					if( pChar->GetState() == CS_LOADING ) continue;
					if( pChar->GetState() == CS_OBSERVER ) continue;

					if( pChar->GetTeam() == TEAM_BLUE )
						iBlueTeamCnt++;
				}
				iRedTeamCnt = 1;         //몬스터는 무조건 1명
			}
			else if( m_pPlayMode->GetModeType() == MT_PRACTICE )				
			{
				iRedTeamCnt = 1;
				iBlueTeamCnt = 1;
			}
			else
			{
				for( int i=0; i < m_pPlayMode->GetRecordCharCnt(); i++ )
				{
					ioBaseChar *pChar = m_pPlayMode->GetRecordChar( i );
					if( !pChar ) continue;
					if( pChar->GetState() == CS_VIEW ) continue;
					if( pChar->GetState() == CS_LOADING ) continue;
					if( pChar->GetState() == CS_OBSERVER ) continue;

					if( pChar->GetTeam() == TEAM_RED )
						iRedTeamCnt++;
					else if( pChar->GetTeam() == TEAM_BLUE )
						iBlueTeamCnt++;
				}
			}

			m_iRedForBalance = iRedTeamCnt;
			m_iBlueForBalance = iBlueTeamCnt;
		}

		if( iRedTeamCnt != 0 && iBlueTeamCnt != 0 )
		{
			int iCurModeStyle = GetScriptModeStyle();

			m_fMemberBalanceRed = g_MathManager.GetMemberBalance( iRedTeamCnt, iBlueTeamCnt, iCurModeStyle );
			m_fMemberBalanceBlue = g_MathManager.GetMemberBalance( iBlueTeamCnt, iRedTeamCnt, iCurModeStyle );			
			
			m_fMemberBalanceRedForBlowTime = g_MathManager.GetMemberBalanceForBlowTime( iRedTeamCnt, iBlueTeamCnt, (int)GetModeType(), iCurModeStyle );
			m_fMemberBalanceBlueForBlowTime = g_MathManager.GetMemberBalanceForBlowTime( iBlueTeamCnt, iRedTeamCnt, (int)GetModeType(), iCurModeStyle );

			m_fMemberBalanceRedForGauge = g_MathManager.GetMemberBalanceForGauge( iRedTeamCnt, iBlueTeamCnt, (int)GetModeType(), iCurModeStyle );
			m_fMemberBalanceBlueForGauge = g_MathManager.GetMemberBalanceForGauge( iBlueTeamCnt, iRedTeamCnt, (int)GetModeType(), iCurModeStyle );

			m_fMemberBalanceRedForSpeed = g_MathManager.GetMemberBalanceForMoveSpeed( iRedTeamCnt, iBlueTeamCnt, (int)GetModeType(), iCurModeStyle );
			m_fMemberBalanceBlueForSpeed = g_MathManager.GetMemberBalanceForMoveSpeed( iBlueTeamCnt, iRedTeamCnt, (int)GetModeType(), iCurModeStyle );

			m_iMemberBalanceRedCntForBuff = iRedTeamCnt;
			m_iMemberBalanceBlueCntForBuff = iBlueTeamCnt;

			
			// 보스 레이드 보정
			ioBossMode *pBossMode = ToBossMode( m_pPlayMode );
			if( pBossMode )
			{
				//보스 모드 보정 적용
				if( m_fMemberBalanceRed != m_fMemberBalanceBlue )
				{
					m_fMemberBalanceRed  = FLOAT1;
					m_fMemberBalanceBlue = m_fMemberBalanceBlue * pBossMode->GetCurBossLevel();

					m_fMemberBalanceRedForBlowTime  = FLOAT1;
					m_fMemberBalanceBlueForBlowTime = m_fMemberBalanceBlueForBlowTime / max( 1, pBossMode->GetMaxBossLevel() - pBossMode->GetCurBossLevel() );
				}
			}

			// 해골 영웅 보정
			ioMonsterSurvivalMode *pMonsterMode = ToMonsterSurvivalMode( m_pPlayMode );
			if( pMonsterMode )
			{
				m_fMemberBalanceRed  = m_fMemberBalanceBlue = FLOAT1;

				// 블루(유저)팀은 최대 인원 평균에 따라 낮아진다.
				if( iBlueTeamCnt > 0 )
					m_fMemberBalanceBlue = min( (float)pMonsterMode->GetAveragePlayer() / (float)iBlueTeamCnt, pMonsterMode->GetBalanceMaxRate() );     

				// 나머지는 전부 1.
				m_fMemberBalanceBlueForGauge = m_fMemberBalanceRedForGauge = FLOAT1;
				m_fMemberBalanceBlueForBlowTime = m_fMemberBalanceRedForBlowTime = FLOAT1;
				m_iMemberBalanceRedCntForBuff = m_iMemberBalanceBlueCntForBuff = 1;
				m_fMemberBalanceRedForSpeed = m_fMemberBalanceBlueForSpeed = FLOAT1;
				m_iBlueForBalance = m_iRedForBalance = 1;
			}

			// 던전 보정
			ioDungeonAMode *pDungeonAMode = ToDungeonAMode( m_pPlayMode );
			if( pDungeonAMode )
			{
				m_fMemberBalanceRed  = m_fMemberBalanceBlue = FLOAT1;
				m_fMemberBalanceBlueForGauge = m_fMemberBalanceRedForGauge = FLOAT1;
				m_fMemberBalanceBlueForBlowTime = m_fMemberBalanceRedForBlowTime = FLOAT1;
				m_iMemberBalanceRedCntForBuff = m_iMemberBalanceBlueCntForBuff = 1;
				m_fMemberBalanceRedForSpeed = m_fMemberBalanceBlueForSpeed = FLOAT1;
				m_iBlueForBalance = m_iRedForBalance = 1;
			}

			ioTowerDefMode *pTowerDefMode = ToTowerDefMode( m_pPlayMode );
			if( pTowerDefMode )
			{
				m_fMemberBalanceRed  = m_fMemberBalanceBlue = FLOAT1;
				m_fMemberBalanceBlueForGauge = m_fMemberBalanceRedForGauge = FLOAT1;
				m_fMemberBalanceBlueForBlowTime = m_fMemberBalanceRedForBlowTime = FLOAT1;
				m_iMemberBalanceRedCntForBuff = m_iMemberBalanceBlueCntForBuff = 1;
				m_fMemberBalanceRedForSpeed = m_fMemberBalanceBlueForSpeed = FLOAT1;
				m_iBlueForBalance = m_iRedForBalance = 1;
			}
			
			ioRaidMode *pRaidMode = ToRaidMode( m_pPlayMode );
			if( pRaidMode )
			{
				m_fMemberBalanceRed  = m_fMemberBalanceBlue = FLOAT1;
				m_fMemberBalanceBlueForGauge = m_fMemberBalanceRedForGauge = FLOAT1;
				m_fMemberBalanceBlueForBlowTime = m_fMemberBalanceRedForBlowTime = FLOAT1;
				m_iMemberBalanceRedCntForBuff = m_iMemberBalanceBlueCntForBuff = 1;
				m_fMemberBalanceRedForSpeed = m_fMemberBalanceBlueForSpeed = FLOAT1;
				m_iBlueForBalance = m_iRedForBalance = 1;
			}

			// 강시 모드 보정
			ioGangsiMode *pGangsiMode = ToGangsiMode( m_pPlayMode );
			if( pGangsiMode )
			{
				m_fMemberBalanceRed  = m_fMemberBalanceBlue = FLOAT1;
				if( iRedTeamCnt > 0 )
				{
					m_fMemberBalanceRed = min( FLOAT1, (float)iBlueTeamCnt / (float)iRedTeamCnt );
				}

				// 나머지는 전부 1.
				m_fMemberBalanceBlueForGauge = m_fMemberBalanceRedForGauge = FLOAT1;
				m_iMemberBalanceRedCntForBuff = m_iMemberBalanceBlueCntForBuff = 1;
				m_fMemberBalanceRedForSpeed = m_fMemberBalanceBlueForSpeed = FLOAT1;
				m_iBlueForBalance = m_iRedForBalance = 1;
			}

			SetMemberBalanceMsg();
		}
	}	
}

void ioPlayStage::ReCalculateDamageRateByMyRoom()
{
	m_fMemberBalanceBlue = m_fMemberBalanceRed = FLOAT1;
	m_fMemberBalanceBlueForGauge = m_fMemberBalanceRedForGauge = FLOAT1;
	m_iMemberBalanceRedCntForBuff = m_iMemberBalanceBlueCntForBuff = 1;
	m_fMemberBalanceRedForSpeed = m_fMemberBalanceBlueForSpeed = FLOAT1;

	if( !m_pPlayMode )	return;

	if( g_BattleRoomMgr.IsTournamentModeType() )
		return;        // 경기방은 보정 없음

	m_fMemberBalanceRedForGauge = g_MathManager.GetMemberBalanceForGauge( 1, 1, (int)GetModeType(), MST_NONE );
	m_fMemberBalanceBlueForGauge = g_MathManager.GetMemberBalanceForGauge( 1, 1, (int)GetModeType(), MST_NONE );

	m_fMemberBalanceRedForSpeed = g_MathManager.GetMemberBalanceForMoveSpeed( 1, 1, (int)GetModeType(), MST_NONE );
	m_fMemberBalanceBlueForSpeed = g_MathManager.GetMemberBalanceForMoveSpeed( 1, 1, (int)GetModeType(), MST_NONE );
}

ioFieldItem* ioPlayStage::GetCrownFieldItem( ioItem::ModeCrownType Type )
{
	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldItem *pField = ToFieldItem( *iter );

		if( pField && pField->GetCrownItemType() == Type )
			return pField;
	}

	return NULL;
}

void ioPlayStage::GetCrownFieldItemByAll( ioItem::ModeCrownType Type, ioFieldItemVec& ItemVec )
{
	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldItem *pField = ToFieldItem( *iter );

		if( pField && pField->GetCrownItemType() == Type )
		{
			ItemVec.push_back( pField );
		}		
	}	
}

ioFieldItem* ioPlayStage::GetFlagFieldItem()
{
	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldItem *pField = ToFieldItem( *iter );

		if( pField && pField->IsFlagItem() )
			return pField;
	}

	return NULL;
}

void ioPlayStage::DestroyMapEffect( ioMapEffect *pEffect )
{
	if( pEffect )
	{
		DestroyMapEffect( pEffect->GetUniqueID() );
	}
}

void ioPlayStage::DestroyMapEffect( UniqueObjID eUniqueID )
{
	if( eUniqueID == -1 )	return;

	ioMapEffect *pMapEffect = NULL;
	MapEffectList::iterator iter = m_EffectList.begin();
	while( iter != m_EffectList.end() )
	{
		pMapEffect = *iter;

		if( pMapEffect )
		{
			if( pMapEffect->GetUniqueID() == eUniqueID )
			{
				m_EffectList.erase( iter );
				delete pMapEffect;
				break;
			}
			else
				++iter;
		}
	}
}

ioMapEffect* ioPlayStage::FindMapEffect( UniqueObjID eUniqueID )
{
	if( eUniqueID == -1 )	return NULL;

	ioMapEffect *pMapEffect = NULL;
	MapEffectList::iterator iter = m_EffectList.begin();
	while( iter != m_EffectList.end() )
	{
		pMapEffect = *iter;

		if( !pMapEffect )
			++iter;

		if( pMapEffect->GetUniqueID() != eUniqueID )
			++iter;
		else
			return pMapEffect;
	}

	return NULL;
}

float ioPlayStage::GetMemberBalance( TeamType eTeam )
{
	float fRate = FLOAT1;	
	
	if( g_BattleRoomMgr.IsTournamentModeType() )
		return fRate;        // 경기방은 보정 없음

	if( GetModeType() == MT_HEROMATCH )
	{
		// 영웅전은 HP 차이에 따른 공식으로 처리한다.
		TeamType eOtherTeam = TEAM_BLUE;
		if( eTeam == TEAM_BLUE )
			eOtherTeam = TEAM_RED;

		ioBaseChar *pMyChar = GetBaseCharFirstTeam( eTeam );
		ioBaseChar *pOtherChar = GetBaseCharFirstTeam( eOtherTeam );
		if( pMyChar && pOtherChar )
		{
			StatusValue kMyHP = pMyChar->GetHP();
			StatusValue kOtherHP = pOtherChar->GetHP();
			float fGapHP = max( 0.0f, kMyHP.m_fCurValue - kOtherHP.m_fCurValue );
			float fMaxHP = max( FLOAT1, kMyHP.m_fMaxValue );
			float fBalance = min( FLOAT05, fGapHP / fMaxHP );
			fRate = max( 0.01f, FLOAT1 - fBalance );
		}
	}
	else
	{
		switch( eTeam )
		{
		case TEAM_RED:
			fRate = m_fMemberBalanceRed;
			break;
		case TEAM_BLUE:
			fRate = m_fMemberBalanceBlue;
			break;
		default:
			if( GetPlazaModeType() == PT_GUILD )
			{
				for(int i = 0;i < (int)m_vSingleTeamBalance.size();i++)
				{
					SingleTeamBalance &rSingleTeam = m_vSingleTeamBalance[i];
					if( rSingleTeam.m_eTeam != eTeam ) continue;

					fRate = rSingleTeam.m_fDamageBalance;
					break;
				}			
			}
			break;		
		}
	}
	return fRate;
}

float ioPlayStage::GetMemberBalanceForGauge( TeamType eTeam )
{
	float fRate = FLOAT1;

	if( g_BattleRoomMgr.IsTournamentModeType() )
		return fRate;        // 경기방은 보정 없음

	switch( eTeam )
	{
	case TEAM_RED:
		fRate = m_fMemberBalanceRedForGauge;
		break;
	case TEAM_BLUE:
		fRate = m_fMemberBalanceBlueForGauge;
		break;
	default:
		if( GetPlazaModeType() == PT_GUILD )
		{
			for(int i = 0;i < (int)m_vSingleTeamBalance.size();i++)
			{
				SingleTeamBalance &rSingleTeam = m_vSingleTeamBalance[i];
				if( rSingleTeam.m_eTeam != eTeam ) continue;

				fRate = rSingleTeam.m_fGaugeBalance;
				break;
			}	
		}
		else
		{
			fRate = m_fMemberBalanceBlueForGauge;
		}
		break;
	}

	return fRate;
}

float ioPlayStage::GetMemberBalanceForStateProtectionTime( TeamType eTeam, int iType )
{
	float fRate = FLOAT1;

	if( g_BattleRoomMgr.IsTournamentModeType() )
		return fRate;        // 경기방은 보정 없음

	switch( eTeam )
	{
	case TEAM_RED:
		fRate = g_MathManager.GetStateProtectionTimeRate( m_iMemberBalanceRedCntForBuff, m_iMemberBalanceBlueCntForBuff, iType );
		break;
	case TEAM_BLUE:
		fRate = g_MathManager.GetStateProtectionTimeRate( m_iMemberBalanceBlueCntForBuff, m_iMemberBalanceRedCntForBuff, iType );
		break;
	default:
		if( GetPlazaModeType() == PT_GUILD )
		{
			for(int i = 0;i < (int)m_vSingleTeamBalance.size();i++)
			{
				SingleTeamBalance &rSingleTeam = m_vSingleTeamBalance[i];
				if( rSingleTeam.m_eTeam != eTeam ) continue;

				fRate = rSingleTeam.m_fGaugeBalance;
				break;
			}	
		}
		else
		{
			fRate = g_MathManager.GetStateProtectionTimeRate( 1, 1, iType );
		}
		break;
	}

	return fRate;
}

float ioPlayStage::GetMemberBalanceForBuffItemDrop( TeamType eTeam )
{
	float fRate = FLOAT1;

	if( g_BattleRoomMgr.IsTournamentModeType() )
		return fRate;        // 경기방은 보정 없음

	switch( eTeam )
	{
	case TEAM_RED:
		fRate = g_MathManager.GetBuffItemDropRate( m_iMemberBalanceRedCntForBuff, m_iMemberBalanceBlueCntForBuff );
		break;
	case TEAM_BLUE:
		fRate = g_MathManager.GetBuffItemDropRate( m_iMemberBalanceBlueCntForBuff, m_iMemberBalanceRedCntForBuff );
		break;
	default:
		if( GetPlazaModeType() == PT_GUILD )
		{
			for(int i = 0;i < (int)m_vSingleTeamBalance.size();i++)
			{
				SingleTeamBalance &rSingleTeam = m_vSingleTeamBalance[i];
				if( rSingleTeam.m_eTeam != eTeam ) continue;

				fRate = rSingleTeam.m_fGaugeBalance;
				break;
			}	
		}
		else
		{
			fRate = g_MathManager.GetBuffItemDropRate( 1, 1 );
		}
		break;
	}

	return fRate;
}

float ioPlayStage::GetMemberBalanceForBlowTime( TeamType eTeam )
{
	float fRate = FLOAT1;

	if( g_BattleRoomMgr.IsTournamentModeType() )
		return fRate;        // 경기방은 보정 없음

	switch( eTeam )
	{
	case TEAM_RED:
		fRate = m_fMemberBalanceRedForBlowTime;
		break;
	case TEAM_BLUE:
		fRate = m_fMemberBalanceBlueForBlowTime;
		break;
	}

	return fRate;
}

float ioPlayStage::GetMemberBalanceForDropDamage( TeamType eTeam, float fDamage )
{
	float fNewDamage = fDamage;

	if( g_BattleRoomMgr.IsTournamentModeType() )
		return fNewDamage;        // 경기방은 보정 없음

	if( GetModeType() == MT_HEROMATCH )
	{
		// 영웅전은 HP 차이에 따른 공식으로 처리한다.
		TeamType eOtherTeam = TEAM_BLUE;
		if( eTeam == TEAM_BLUE )
			eOtherTeam = TEAM_RED;

		ioBaseChar *pMyChar = GetBaseCharFirstTeam( eTeam );
		ioBaseChar *pOtherChar = GetBaseCharFirstTeam( eOtherTeam );
		if( pMyChar && pOtherChar )
		{
			StatusValue kMyHP = pMyChar->GetHP();
			StatusValue kOtherHP = pOtherChar->GetHP();
			float fGapHP = max( 0.0f, kOtherHP.m_fCurValue - kMyHP.m_fCurValue );   // 이부분은 데미지와 반대
			float fMaxHP = max( FLOAT1, kMyHP.m_fMaxValue );
			float fBalance = min( FLOAT05, fGapHP / fMaxHP );
			fNewDamage = fDamage * max( 0.01f, FLOAT1 - fBalance );
		}
	}
	else
	{
		int iOwnerCnt, iEnemyCnt;
		iOwnerCnt = iEnemyCnt = 1;
		switch( eTeam )
		{
		case TEAM_RED:
			iOwnerCnt = m_iRedForBalance;
			iEnemyCnt = m_iBlueForBalance;
			break;
		case TEAM_BLUE:
			iOwnerCnt = m_iBlueForBalance;
			iEnemyCnt = m_iRedForBalance;
			break;
		}

		if( iOwnerCnt == 0 || iEnemyCnt == 0 )
		{
			iOwnerCnt = iEnemyCnt = 1;
		}

		fNewDamage = g_MathManager.GetMemberBalanceForDropDamage( iOwnerCnt, iEnemyCnt, fDamage );
	}
	return fNewDamage;
}

float ioPlayStage::GetMemberBalanceForMoveSpeed( TeamType eTeam )
{
	float fRate = FLOAT1;

	if( g_BattleRoomMgr.IsTournamentModeType() )
		return fRate;        // 경기방은 보정 없음

	switch( eTeam )
	{
	case TEAM_RED:
		fRate = m_fMemberBalanceRedForSpeed;
		break;
	case TEAM_BLUE:
		fRate = m_fMemberBalanceBlueForSpeed;
		break;
	default:
		fRate = FLOAT1;
		break;
	}

	return fRate;
}

void ioPlayStage::SetChatIcon( const ioHashString &szName,
							   const ioHashString &szChat,
							   bool bNoSound,
							   bool bCusomSound )
{
	if( g_BlackListManager.IsBlackList( szName ) ) return;

	if( Setting::UseBlockUsersChat() &&
		!g_FriendManager.IsFriend( szName )  &&
		!g_GuildInfo.IsMyGuildUser( szName ) && 
		g_MyInfo.GetPublicID() != szName ) return;

	if( g_LadderTeamMgr.IsLadderTeam()    && 
		g_LadderTeamMgr.IsOtherChatHide() && 
		g_MyInfo.GetPublicID() != szName ) return;

	ioBaseChar *pChar = NULL;
	if( GetModeType() == MT_MYROOM )
		pChar = GetOwnerChar();
	else
		pChar = GetBaseChar( szName );

	if( pChar )
	{
		ioHashString szChatIcon;

		if( bCusomSound )
			szChatIcon = g_CustomSoundMgr.GetDefaultChatIcon();
		else 
			szChatIcon = g_ChatMacroInfo.GetChatIcon( szChat );

		if( szChatIcon == "UIIconPack2#chat_default" )
		{
			if( Setting::ChatBubbleAlarm() )
				pChar->Set3DChatBubbleEmoticon( szChat.Length(), bNoSound );
			else
				pChar->Set3DChatEmoticon( szChat.Length(), bNoSound );
		}
		else
		{
			if( Setting::ChatBubbleAlarm() )
				pChar->SetChatBubbleEmoticon( szChatIcon );
			else
				pChar->SetEmoticon( szChatIcon, EMT_CHAT_ICON );
		}
	}
}

ioBaseChar* ioPlayStage::GetCameraTargetChar()
{
	ioBaseChar *pTarget = GetOwnerChar();
	if( !pTarget )	return NULL;

	if( pTarget->GetState() == CS_DIE ||
		pTarget->GetState() == CS_VIEW ||
		pTarget->GetState() == CS_LOADING ||
		pTarget->GetState() == CS_OBSERVER )
	{
		int iCharCnt = m_BaseCharList.size();
		if( iCharCnt == 1 )
			return pTarget;

		int iDieCnt = 0;
		for( int i=0; i < iCharCnt; i++ )
		{
			ioBaseChar *pChar = m_BaseCharList[i];
			if( pChar )
			{
				if( pChar->GetState() == CS_DIE ||
					pChar->GetState() == CS_VIEW ||
					pChar->GetState() == CS_LOADING ||
					pChar->GetState() == CS_OBSERVER )
				{
					iDieCnt++;
				}
			}
		}
		if( iDieCnt == iCharCnt || m_ViewChar.IsEmpty() )
			return pTarget;

		ioBaseChar *pViewChar = m_pPlayMode->GetRecordChar( m_ViewChar );
		if( pViewChar )
			pTarget = pViewChar;
	}

	return pTarget;
}

bool ioPlayStage::CheckMapCollision( ioBaseChar *pChar, const ioRay &rkRay, const D3DXVECTOR3 &vMovePos )
{
	ioOpcodeShape *pShape = m_pSceneMgr->GetCollisionShape();
	if( !pShape )
		return false;

	ioOrientBox kEntityBox;
	kEntityBox.SetByAxisBox( pChar->GetWorldAxisBox() );

	float fNewExtents = kEntityBox.GetExtents(1) * FLOAT05;
	kEntityBox.SetCenter( kEntityBox.GetCenter() + kEntityBox.GetAxis(1) * fNewExtents );
	kEntityBox.SetExtents( 1, fNewExtents );

	D3DXVECTOR3 vColPoint( 0.0f, 0.0f, 0.0f );
	float fCurHeight = kEntityBox.GetCenter().y + fNewExtents;

	if( pShape->TestIntersection( NULL, rkRay, NULL, &vColPoint ) )
	{
		if( vColPoint.y > 0.0f && vColPoint.y < fCurHeight )
			return true;
	}

	return false;
}

void ioPlayStage::SetOneMyChar()
{
	if( m_pPlayMode )
	{
		m_pPlayMode->SetOneMyChar();
	}
}

void ioPlayStage::SetAllMyChar()
{
	if( m_pPlayMode )
	{
		m_pPlayMode->SetAllMyChar();
	}
}

bool ioPlayStage::IsOneMyChar()
{
	if( m_pPlayMode )
	{
		return m_pPlayMode->IsOneMyChar();
	}

	return true;
}

void ioPlayStage::TreasureCardRandom( short &rPresentType, int &rPresentValue1, int &rPresentValue2 )
{
	rPresentType = 0;
	rPresentValue1 = rPresentValue2 = 0;

	switch( GetModeType() )
	{
	case MT_DUNGEON_A:
		{
			ioDungeonAMode *pMode = ToDungeonAMode( m_pPlayMode );
			if( pMode )
			{
				g_TreasureCardMgr.TreasureCardRandom( pMode->GetTreasureCardRandomIndex(), rPresentType, rPresentValue1, rPresentValue2 );
			}
		}
		break;
	case MT_MONSTER_SURVIVAL:
		{
			ioMonsterSurvivalMode *pMode = ToMonsterSurvivalMode( m_pPlayMode );
			if( pMode )
			{
				g_TreasureCardMgr.TreasureCardRandom( pMode->GetTreasureCardRandomIndex(), rPresentType, rPresentValue1, rPresentValue2 );
			}
		}
		break;
	case MT_TOWER_DEFENSE:
	case MT_DARK_XMAS:
	case MT_FIRE_TEMPLE:
	case MT_FACTORY:
		{
			ioTowerDefMode *pMode = ToTowerDefMode( m_pPlayMode );
			if( pMode )
			{
				g_TreasureCardMgr.TreasureCardRandom( pMode->GetTreasureCardRandomIndex(), rPresentType, rPresentValue1, rPresentValue2 );
			}
		}
		break;
	case MT_RAID:
		{
			ioRaidMode *pMode = ToRaidMode( m_pPlayMode );
			if( pMode )
			{
				g_TreasureCardMgr.TreasureCardRandom( pMode->GetTreasureCardRandomIndex(), rPresentType, rPresentValue1, rPresentValue2 );
			}
		}
		break;
	}
}

void ioPlayStage::IncreaseTeamCntForBalance( TeamType eTeam )
{
	switch( eTeam )
	{
	case TEAM_RED:
		m_iRedForBalance++;
		break;
	case TEAM_BLUE:
		m_iBlueForBalance++;
		break;
	}
}

void ioPlayStage::DecreaseTeamCntForBalance( TeamType eTeam )
{
	switch( eTeam )
	{
	case TEAM_RED:
		m_iRedForBalance--;
		m_iRedForBalance = max( 0, m_iRedForBalance );
		break;
	case TEAM_BLUE:
		m_iBlueForBalance--;
		m_iBlueForBalance = max( 0, m_iBlueForBalance );
		break;
	}
}

ioPlayStage::GameEntityIterator ioPlayStage::GetFieldItemIterator()
{
	m_FieldItemBufferList.clear();

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioFieldItem *pFieldItem = ToFieldItem( *iter );
		if( pFieldItem && !pFieldItem->IsNeedDestroy() )
		{
			m_FieldItemBufferList.push_back( pFieldItem );
		}
	}

	return GameEntityIterator( m_FieldItemBufferList.begin(), m_FieldItemBufferList.end() );
}

void ioPlayStage::UpdateGrowthData()
{
	BaseCharList::iterator iter;
	for( iter=m_BaseCharList.begin() ; iter!=m_BaseCharList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( pCurChar )
		{
			// 방어력에 성장정보 반영
			pCurChar->UpdateCharArmorClass();

			// 속보 반영
			pCurChar->UpdateCharSpeedClass();
		}
	}
}

void ioPlayStage::UpdateGrowthDataOne( const ioHashString &szName )
{
	BaseCharList::iterator iter;
	for( iter=m_BaseCharList.begin() ; iter!=m_BaseCharList.end() ; ++iter )
	{
		ioBaseChar *pCurChar = *iter;
		if( pCurChar && pCurChar->GetCharName() == szName )
		{
			// 방어력에 성장정보 반영
			pCurChar->UpdateCharArmorClass();

			// 속보 반영
			pCurChar->UpdateCharSpeedClass();

			break;
		}
	}
}

void ioPlayStage::OnCharGrowthLevelData( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( pChar == NULL || m_pPlayMode == NULL || szName.IsEmpty() )
	{
		BYTE iLevel;
		for(int i =0;i < MAX_CHAR_GROWTH + MAX_ITEM_GROWTH;++i)
			rkPacket >> iLevel;
	}
	else
	{    
		ioGrowthLevel *pLevel = GetGrowthLevel( szName );
		if( pLevel == NULL )
		{
			BYTE iLevel;
			for(int i =0;i < MAX_CHAR_GROWTH + MAX_ITEM_GROWTH;++i)
				rkPacket >> iLevel;
			LOG.PrintTimeAndLog( 0, "OnCharGrowthLevelData : %s - None Growth", szName.c_str() );
		}
		else
		{
			pLevel->ApplyGrowthLevelDataRoomSync( pChar->GetCharacterInfo().m_class_type, rkPacket );
		}
	}
	UpdateGrowthDataOne( szName );
}

void ioPlayStage::OnCharNamedTitleData( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	DWORD dwCode;
	BYTE byPremium;
	int nLevel;
	rkPacket >> dwCode >> byPremium >> nLevel;

	// 유저 노드 적용
	UserInfoNode *pInfo = g_UserInfoMgr.GetUserInfoNode( szName, false );
	if ( pInfo )
		pInfo->SetNamedTitle( dwCode, nLevel, byPremium );

	// 캐릭터 적용
	ioBaseChar *pOwner = GetBaseChar( szName );
	if ( pOwner )
		pOwner->SetNamedTitle( dwCode, nLevel, byPremium );
}

ioGrowthLevel* ioPlayStage::GetGrowthLevel( const ioHashString& szName )
{
	ioGrowthLevel *pLevel = NULL;

	if( szName.IsEmpty() )
		return pLevel;

	if( GetModeType() == MT_MYROOM )
		pLevel = g_MyInfo.GetGrowthLevel();
	else if( szName == g_MyInfo.GetPublicID() )
		pLevel = g_MyInfo.GetGrowthLevel();
	else if( m_pPlayMode )
		pLevel = m_pPlayMode->GetGrowthLevel( szName );

	return pLevel;
}

void ioPlayStage::OnGrowthLevelInit( SP2Packet &rkPacket )
{
	int iSuccess;
	ioHashString szOwner;

	rkPacket >> szOwner;
	rkPacket >> iSuccess;

	if( szOwner.IsEmpty() )
		return;

	if( szOwner == g_MyInfo.GetPublicID() )
		g_App.SetMouseBusy( false );

	if( iSuccess == GROWTH_INIT_SUCESS )
	{
		int iClassType, iCurPoint;
		__int64 iPeso;
		rkPacket >> iClassType >> iCurPoint >> iPeso;

		ioGrowthLevel *pLevel = GetGrowthLevel( szOwner );

		if( pLevel )
			pLevel->SetCharGrowthLevelInit( iClassType, iCurPoint );

		if( m_pPlayMode && m_pPlayMode->GetModeType() == MT_MYROOM )
		{
			DWORD dwIndex = g_MyInfo.GetCharIndexByClassType( iClassType );
			ioBaseChar *pChar = GetBaseChar( dwIndex );
			if( pChar )
			{
				UpdateGrowthDataOne( pChar->GetCharName() );
			}
		}
		else
		{
			UpdateGrowthDataOne( szOwner );
		}

		if( szOwner == g_MyInfo.GetPublicID() )
		{
			g_MyInfo.SetMoney( iPeso );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->UpdateTab( false, true );
			}
		}
	}
	else if( iSuccess == GROWTH_INIT_FAIL_PESO )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
	}
}

void ioPlayStage::OnGrowthLevelUp( SP2Packet &rkPacket )
{
	int iSuccess;
	ioHashString szOwner;

	rkPacket >> szOwner;
	rkPacket >> iSuccess;

	if( szOwner.IsEmpty() ) return;

	if( szOwner == g_MyInfo.GetPublicID() )
		g_App.SetMouseBusy( false );

	// 성공/실패 처리, 실패는 서버에서 당사자에게만 전달
	if( iSuccess == GROWTH_UP_SUCESS )
	{
		bool bItem;
		bool bRemove;
		int iClassType, iSlot, iPoint, iUpLevel;

		__int64 iPeso;
		iSlot = 0;

		rkPacket >> iClassType;
		rkPacket >> bItem;
		rkPacket >> iSlot;
	
		rkPacket >> bRemove;
		
		rkPacket >> iPoint;
		rkPacket >> iUpLevel;
		rkPacket >> iPeso;

		ioGrowthLevel *pLevel = GetGrowthLevel( szOwner );
		if( pLevel )
		{
			if( bRemove )
				pLevel->RemoveTimeGrowth( iClassType );

			if( bItem )
				pLevel->ApplyItemGrowthLevelUp( iClassType, iSlot, iUpLevel );
			else
				pLevel->ApplyCharGrowthLevelUp( iClassType, iSlot, iUpLevel );

			pLevel->SetCharGrowthPoint( iClassType, iPoint );
		}

		ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayMode );
		if( pHeadquartersMode )
		{
			pHeadquartersMode->OnModeCharGrowthUpdate( szOwner, iClassType, iSlot, bItem, iUpLevel );
		}
		else
		{
			UpdateGrowthDataOne( szOwner );
		}

		if( szOwner == g_MyInfo.GetPublicID() )
		{
			g_MyInfo.SetMoney( iPeso );

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->UpdateTab( false, true );
			}

			ioGrowthSelectWnd *pSelectWnd = dynamic_cast<ioGrowthSelectWnd*>(g_GUIMgr.FindWnd(GROWTH_SELECT_WND));
			if( pSelectWnd && pSelectWnd->IsShow() )
			{
				pSelectWnd->UpdateInfo();
			}

			g_QuestMgr.QuestCompleteTerm( QC_PESO_GROWTH_TRY );

#if defined( USE_GA )
			char chLabel[32] = {0,};

			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chLabel, "%d", iClassType );
			else
				SafeSprintf( chLabel, sizeof(chLabel), "%1", iClassType );

			// CHAR_GROWTH_LV_UP
			g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
				, "Character"
				, "LevelUp"
				, chLabel
				, 1
				, "%2FCHAR%2FGROWTH%2FLV%2FUP" );
#endif

		}

		return;
	}
	else if( iSuccess == GROWTH_UP_FAIL_POINT )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(1) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_13,STR(2) );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		return;
	}
	else if( iSuccess == GROWTH_UP_FAIL_PESO )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}
	else if( iSuccess == GROWTH_UP_FAIL_GOLD )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		return;
	}
	else if( iSuccess == GROWTH_UP_FAIL )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(6) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(7) );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		return;
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return;
	}
}

void ioPlayStage::OnLevelGrowth( SP2Packet &rkPacket )
{
	int iSuccess;
	ioHashString szOwner;

	rkPacket >> szOwner;
	rkPacket >> iSuccess;

	if( szOwner.IsEmpty() ) return;

	if( szOwner == g_MyInfo.GetPublicID() )
		g_App.SetMouseBusy( false );

	// 성공/실패 처리, 실패는 서버에서 당사자에게만 전달
	if( iSuccess == LEVEL_GROWTH_SUCESS )
	{
		int iClassType, iPoint;
		__int64 iPeso;
		int iCharLevelUp[CUR_GROWTH_CNT];
		int iItemLevelUp[CUR_GROWTH_CNT];

		rkPacket >> iClassType;
		rkPacket >> iPoint;
		for( int i = 0; i < CUR_GROWTH_CNT; i++ )
			rkPacket >> iCharLevelUp[i];
		for( int i = 0; i < CUR_GROWTH_CNT; i++ )
			rkPacket >> iItemLevelUp[i];

		rkPacket >> iPeso;

		ioGrowthLevel *pLevel = GetGrowthLevel( szOwner );
		if( pLevel )
		{
			for( int i = 0; i < CUR_GROWTH_CNT; i++ )
			{
				pLevel->ApplyItemGrowthLevelUp( iClassType, i, iItemLevelUp[i] );
				pLevel->ApplyCharGrowthLevelUp( iClassType, i, iCharLevelUp[i] );
			}
			pLevel->SetCharGrowthPoint( iClassType, iPoint );
		}

		ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayMode );
		if( pHeadquartersMode )
		{
			for( int i = 0; i < CUR_GROWTH_CNT; i++ )
			{
				pHeadquartersMode->OnModeCharGrowthUpdate( szOwner, iClassType, i, true, iItemLevelUp[i] );
				pHeadquartersMode->OnModeCharGrowthUpdate( szOwner, iClassType, i, false, iCharLevelUp[i] );
			}
		}
		else
		{
			UpdateGrowthDataOne( szOwner );
		}

		if( szOwner == g_MyInfo.GetPublicID() )
		{
			g_MyInfo.SetMoney( iPeso );

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->UpdateTab( false, true );
			}

			ioGrowthSelectWnd *pSelectWnd = dynamic_cast<ioGrowthSelectWnd*>(g_GUIMgr.FindWnd(GROWTH_SELECT_WND));
			if( pSelectWnd && pSelectWnd->IsShow() )
			{
				pSelectWnd->UpdateInfo();
			}

			g_QuestMgr.QuestCompleteTerm( QC_PESO_GROWTH_TRY );

#if defined( USE_GA )
			char chLabel[32] = {0,};

			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chLabel, "%d", iClassType );
			else
				SafeSprintf( chLabel, sizeof(chLabel), "%1", iClassType );

			g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
				, "Character"
				, "GrowthLevel"
				, chLabel
				, 1
				, "%2FCHAR%2FGROWTH%2FLV%2FUP" );
#endif
		}

		return;
	}
	else if( iSuccess == LEVEL_GROWTH_FAIL_POINT )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(1) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_13,STR(2) );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		return;
	}
	else if( iSuccess == LEVEL_GROWTH_FAIL_PESO )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		return;
	}
	else if( iSuccess == LEVEL_GROWTH_FAIL )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(6) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(7) );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		return;
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		return;
	}
}

void ioPlayStage::OnTimeGrowthAdd( SP2Packet &rkPacket )
{
	g_App.SetMouseBusy( false );

	int iSuccess;
	rkPacket >> iSuccess;

	if( iSuccess == TIME_GROWTH_ADD_FAIL )
	{
		int iErrorNum;
		rkPacket >> iErrorNum;

		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), iErrorNum );
		return;
	}
	else if( iSuccess == TIME_GROWTH_ADD_POINT )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(2) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(3) );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		return;
	}
	else if( iSuccess == TIME_GROWTH_ADD_LIMIT_CNT )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(4) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(5) );

		kPrinter[2].SetTextStyle( TS_NORMAL );
		kPrinter[2].SetBkColor( 0, 0, 0 );	
		kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(6) );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		return;
	}
	else if( iSuccess == TIME_GROWTH_ADD_LIMIT_TOTAL_CNT )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(7), g_GrowthInfoMgr.GetTimeGrowthEnableTotalCnt() );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(8) );

		kPrinter[2].SetTextStyle( TS_NORMAL );
		kPrinter[2].SetBkColor( 0, 0, 0 );	
		kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[2].AddTextPiece( FONT_SIZE_13, STR(9) );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		return;
	}
	else if( iSuccess == TIME_GROWTH_ADD_LIMIT_LEVEL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10), g_GrowthInfoMgr.GetTimeGrowthLimitLevel() );
		return;
	}
	else if( iSuccess == TIME_GROWTH_ADD_OK )
	{
		ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
		if( pLevel )
		{
			int iClassType, iSlot, iValue1, iValue2, iPoint;
			rkPacket >> iClassType >> iSlot;
			rkPacket >> iValue1 >> iValue2;
			rkPacket >> iPoint;

			pLevel->AddTimeGrowth( iClassType, iSlot, iValue1, iValue2 );
			pLevel->SetCharGrowthPoint( iClassType, iPoint );

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->UpdateTab( false, false );
			}

			ioGrowthSelectWnd *pSelectWnd = dynamic_cast<ioGrowthSelectWnd*>(g_GUIMgr.FindWnd(GROWTH_SELECT_WND));
			if( pSelectWnd && pSelectWnd->IsShow() )
			{
				pSelectWnd->UpdateInfo();
			}

#if defined( USE_GA )
			char chLabel[32] = {0,};

			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chLabel, "%d", iClassType );
			else
				SafeSprintf( chLabel, sizeof(chLabel), "%1", iClassType );

			// CHAR_GROWTH_TIME_START
			g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
				, "Character"
				, "Start"
				, chLabel
				, 1
				, "%2FCHAR%2FGROWTH%2FTIME%2FSTART" );
#endif
		}

		g_QuestMgr.QuestCompleteTerm( QC_TIME_GROWTH_TRY );
	}
}

void ioPlayStage::OnTimeGrowthRemove( SP2Packet &rkPacket )
{
	g_App.SetMouseBusy( false );

	int iSuccess;
	rkPacket >> iSuccess;

	if( iSuccess == TIME_GROWTH_REMOVE_FAIL )
	{
		int iErrorNum;
		rkPacket >> iErrorNum;

		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), iErrorNum );
		return;
	}
	else if( TIME_GROWTH_REMOVE_OK )
	{
		ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
		if( pLevel )
		{
			int iClassType, iPoint;
			rkPacket >> iClassType;
			rkPacket >> iPoint;

			pLevel->RemoveTimeGrowth( iClassType );
			pLevel->SetCharGrowthPoint( iClassType, iPoint );

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->UpdateTab( false, false );
			}

			ioGrowthSelectWnd *pSelectWnd = dynamic_cast<ioGrowthSelectWnd*>(g_GUIMgr.FindWnd(GROWTH_SELECT_WND));
			if( pSelectWnd && pSelectWnd->IsShow() )
			{
				pSelectWnd->UpdateInfo();
			}

#if defined( USE_GA )
			char chLabel[32] = {0,};

			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chLabel, "%d", iClassType );
			else
				SafeSprintf( chLabel, sizeof(chLabel), "%1", iClassType );

			// CHAR_GROWTH_TIME_CANCEL
			g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			 	, "Character"
			 	, "Cancel"
			 	, chLabel
			 	, 1
			 	, "%2FCHAR%2FGROWTH%2FTIME%2FCANCEL" );
#endif
		}
	}
}

void ioPlayStage::OnGrowthLevelDown( SP2Packet &rkPacket )
{
	int iSuccess;
	ioHashString szOwner;

	rkPacket >> szOwner;
	rkPacket >> iSuccess;

	if( szOwner.IsEmpty() ) return;

	if( szOwner == g_MyInfo.GetPublicID() )
		g_App.SetMouseBusy( false );

	// 성공/실패 처리, 실패는 서버에서 당사자에게만 전달
	if( iSuccess == GROWTH_DOWN_SUCESS )
	{
		bool bItem;
		int iClassType, iSlot, iPoint, iDownLevel, iReturnPoint;

		rkPacket >> iClassType;
		rkPacket >> bItem;
		rkPacket >> iSlot;

		rkPacket >> iPoint;
		rkPacket >> iReturnPoint;
		rkPacket >> iDownLevel;

		ioGrowthLevel *pLevel = GetGrowthLevel( szOwner );
		if( pLevel )
		{
			if( bItem )
				pLevel->ApplyItemGrowthLevelDown( iClassType, iSlot, iDownLevel );
			else
				pLevel->ApplyCharGrowthLevelDown( iClassType, iSlot, iDownLevel );

			pLevel->SetCharGrowthPoint( iClassType, iPoint );
		}

		ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayMode );
		if( pHeadquartersMode )
		{
			pHeadquartersMode->OnModeCharGrowthUpdate( szOwner, iClassType, iSlot, bItem, -iDownLevel );
		}
		else
		{
			UpdateGrowthDataOne( szOwner );
		}

		if( szOwner == g_MyInfo.GetPublicID() )
		{		
			// 성공 팝업
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->OnGrowthDownResult( iClassType, iReturnPoint );
				pInvenWnd->UpdateTab( false, false );
			}

#if defined( USE_GA )
			char chLabel[32] = {0,};

			if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
				sprintf_e( chLabel, "%d", iClassType );
			else
				SafeSprintf( chLabel, sizeof(chLabel), "%1", iClassType );

			// CHAR_GROWTH_LV_DOWN
			g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
				, "Character"
				, "LevelDown"
				, chLabel
				, 1
				, "%2FCHAR%2FGROWTH%2FLV%2FDOWN" );
#endif
		}

		return;
	}
	else if( iSuccess == GROWTH_DOWN_FAIL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
	else if( iSuccess == GROWTH_DOWN_TIME_GROWTH )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(3) );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(4) );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
	}
	else
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
	}

	// 실패해도 업데이트는 해줘야 한다
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true );
	}
}

void ioPlayStage::OnGrowthAllLevelDown( SP2Packet &rkPacket )
{
	int iResult;
	ioHashString szOwner;

	rkPacket >> szOwner;
	rkPacket >> iResult;

	if( szOwner.IsEmpty() ) return;

	if( szOwner == g_MyInfo.GetPublicID() )
		g_App.SetMouseBusy( false );

	// 성공/실패 처리, 실패는 서버에서 당사자에게만 전달
	if( iResult == GROWTH_ALL_LEVEL_DOWN_SUCESS )
	{
		int iClassType = 0;
		int iCharLevels[MAX_CHAR_GROWTH]={ 0, 0, 0, 0 };
		int iItemLevels[MAX_ITEM_GROWTH]={ 0, 0, 0, 0 };
		int iTotoalPoint = 0;
		__int64 iReturnPeso = 0;
		__int64 iMoney = 0;

		rkPacket >> iClassType;
		for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
			rkPacket >> iCharLevels[i];
		for( int i = 0; i < MAX_ITEM_GROWTH; i++ )
			rkPacket >> iItemLevels[i];
		rkPacket >> iTotoalPoint;
		rkPacket >> iReturnPeso;
		rkPacket >> iMoney;

		ioGrowthLevel *pLevel = GetGrowthLevel( szOwner );
		if( pLevel )
		{
			for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
			{
				if( iCharLevels[i] <= 0 )
					continue;
				pLevel->ApplyCharGrowthLevelDown( iClassType, i, iCharLevels[i] );
			}

			for( int i = 0; i < MAX_ITEM_GROWTH; i++ )
			{
				if( iItemLevels[i] <= 0 )
					continue;
				pLevel->ApplyItemGrowthLevelDown( iClassType, i, iItemLevels[i] );
			}

			pLevel->SetCharGrowthPoint( iClassType, iTotoalPoint );
		}

		ioHeadquartersMode *pHeadquartersMode = ToHeadquartersMode( m_pPlayMode );
		if( pHeadquartersMode )
		{
			for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
			{
				if( iCharLevels[i] <= 0 )
					continue;
				pHeadquartersMode->OnModeCharGrowthUpdate( szOwner, iClassType, i, false, -iCharLevels[i] );
			}

			for( int i = 0; i < MAX_ITEM_GROWTH; i++ )
			{
				if( iItemLevels[i] <= 0 )
					continue;
				pHeadquartersMode->OnModeCharGrowthUpdate( szOwner, iClassType, i, true, -iItemLevels[i] );
			}
		}
		else
		{
			UpdateGrowthDataOne( szOwner );
		}

		if( szOwner == g_MyInfo.GetPublicID() )
		{	
			g_MyInfo.SetMoney( iMoney ); 

			int iReturnPoint = 0;
			for( int i = 0; i < MAX_CHAR_GROWTH; i++ )
				iReturnPoint += ( iCharLevels[i] * g_GrowthInfoMgr.GetGrowthUpNeedPoint( true ) );
			for( int i = 0; i < MAX_ITEM_GROWTH; i++ )
				iReturnPoint += ( iItemLevels[i] * g_GrowthInfoMgr.GetGrowthUpNeedPoint( false ) );

			// check
			if( iReturnPoint > iTotoalPoint )
				iReturnPoint = iTotoalPoint;

			// 성공 팝업
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->OnGrowthAllDownResult( iClassType, iReturnPoint, iReturnPeso );
				pInvenWnd->UpdateTab( false, false );
			}
		}

		return;////////////////////////////////////////////////////////////
	}
	else if( iResult == GROWTH_ALL_LEVEL_DOWN_FAIL )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "예외 오류가 발생하였습니다." );
	}
	else if( iResult == GROWTH_ALL_LEVEL_DOWN_TIME_GROWTH )
	{
		ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];
		kPrinter[0].SetTextStyle( TS_NORMAL );
		kPrinter[0].SetBkColor( 0, 0, 0 );	
		kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[0].AddTextPiece( FONT_SIZE_13, "선택하신 용병이 시간육성중이라 복구가 불가능합니다." );

		kPrinter[1].SetTextStyle( TS_NORMAL );
		kPrinter[1].SetBkColor( 0, 0, 0 );	
		kPrinter[1].SetTextColor( TCT_DEFAULT_DARKGRAY );
		kPrinter[1].AddTextPiece( FONT_SIZE_13, "시간육성을 취소 또는 완료후 다시 시도하여 주십시오." );

		g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
	}

	// 실패해도 업데이트는 해줘야 한다
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true );
	}
}

void ioPlayStage::OnGrowthPointBonus( SP2Packet &rkPacket )
{
	ioGrowthLevel *pLevel = g_MyInfo.GetGrowthLevel();
	if( pLevel )
	{
		int iCnt, iClassType, iPoint;

		rkPacket >> iCnt;

		for( int i=0; i < iCnt; ++i )
		{
			rkPacket >> iClassType >> iPoint;

			if( iClassType <= 0 ) continue;

			pLevel->SetCharGrowthPoint( iClassType, iPoint );
		}

		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
		if( pInvenWnd && pInvenWnd->IsShow() )
		{
			pInvenWnd->UpdateTab( false, false );
		}
	}
}

void ioPlayStage::OnUserKickVote( SP2Packet &rkPacket )
{
	int iVoteType;
	rkPacket >> iVoteType;
	
	switch( iVoteType )
	{
	case USER_KICK_VOTE_PROPOSAL:  //투표 제안
		{
			DWORD dwProcessTime;
			ioHashString szProposalName, szKickName, szKickReason;
			rkPacket >> szProposalName >> szKickName >> szKickReason >> dwProcessTime;
			
			if( szProposalName == g_MyInfo.GetPublicID() )
			{
				m_iRoomKickOutVoteProposal++;
				KickVoteResultHelp( this, iVoteType, szKickName, (int)dwProcessTime / 1000, 0, 0 );
			}
			else if( szKickName == g_MyInfo.GetPublicID() )
			{
			}
			else
			{
				// 투표 진행 UI Show
				KickVoteProgressWnd *pKickVoteProgress = dynamic_cast<KickVoteProgressWnd*>(g_GUIMgr.FindWnd( KICK_VOTE_PROGRESS_WND ));
				if( pKickVoteProgress )
					pKickVoteProgress->SetKickVoteProgress( szProposalName, szKickName, szKickReason, dwProcessTime );
			}
		}
		break;
	case USER_KICK_VOTE_ADOPTION:       //투표 가결
		{
			g_GUIMgr.HideWnd( KICK_VOTE_PROGRESS_WND );

			ioHashString szKickName;
			rkPacket >> szKickName;
			m_szRoomKickOutUser = szKickName;

			if( szKickName == g_MyInfo.GetPublicID() )
			{
				int iRoomIndex;
				rkPacket >> iRoomIndex;

				ioHashString szKickReason;
				rkPacket >> szKickReason;

				g_MyInfo.SetVoteKickReason( szKickReason );
				g_GUIMgr.SetReserveMsgBox( MB_VOTE_KICK_OUT );

				switch( GetRoomStyle() )
				{
				case RSTYLE_PLAZA:
					{
						g_MyInfo.SetKickOutPlaza( iRoomIndex );
					}
					break;
				case RSTYLE_BATTLEROOM:
					{
						g_BattleRoomMgr.SetKickOutRoom( g_BattleRoomMgr.GetIndex() );
                        g_BattleRoomMgr.LeaveRoom();
					}
					break;
				case RSTYLE_SHUFFLEROOM:
					{
						g_ShuffleRoomMgr.SetKickOutRoom( g_ShuffleRoomMgr.GetIndex() );
						g_ShuffleRoomMgr.LeaveRoom();
					}
					break;
				case RSTYLE_LADDERBATTLE:
				case RSTYLE_MATCHROOM:
					break;
				}
			}
			else
			{				
				KickVoteResultHelp( this, iVoteType, szKickName, 0, 0, 0 );
			}
		}
		break;
	case USER_KICK_VOTE_REJECTION:       //투표 부결
		{
			g_GUIMgr.HideWnd( KICK_VOTE_PROGRESS_WND );

			ioHashString szKickName;
			rkPacket >> szKickName;
			KickVoteResultHelp( this, iVoteType, szKickName, 0, 0, 0 );
		}
		break;
	case USER_KICK_VOTE_CANCEL_PROPOSAL:       //투표 취소 : 제안자 이탈
	case USER_KICK_VOTE_CANCEL_TARGET:         //투표 취소 : 대상자 이탈
		{
			g_GUIMgr.HideWnd( KICK_VOTE_PROGRESS_WND );

			int iTeamType;
			ioHashString szName;
			rkPacket >> szName >> iTeamType;
			KickVoteResultHelp( this, iVoteType, szName, iTeamType, 0, 0 );
		}
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_1:       //투표 제안 자신의 닉네임
	case USER_KICK_VOTE_PROPOSAL_ERROR_2:       //투표 제안 개발자 & 마스터 & 매니저
	case USER_KICK_VOTE_PROPOSAL_ERROR_3:       //투표 제안 명성이 90% 미만
	case USER_KICK_VOTE_PROPOSAL_ERROR_4:       //투표 제안 관전자는 투표 제안 X
	case USER_KICK_VOTE_PROPOSAL_ERROR_5:       //투표 제안 현재 투표가 진행중
	case USER_KICK_VOTE_PROPOSAL_ERROR_6:       //투표 제안 1회 투표 제안을 했던 유저
	case USER_KICK_VOTE_PROPOSAL_ERROR_7:       //투표 제안 대상자가 동일한 룸에 없음
	case USER_KICK_VOTE_PROPOSAL_ERROR_8:       //투표 제안 대상자가 MC
	case USER_KICK_VOTE_PROPOSAL_ERROR_11:      //투표 제안 투표 참여자 부족( 싱글 )
	case USER_KICK_VOTE_PROPOSAL_ERROR_12:      //투표 제안 투표 참여자 부족( 팀전 )
		KickVoteResultHelp( this, iVoteType, "", 0, 0, 0 );
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_9:       //투표 제안 강퇴 건의 시간 지남( 싱글 )
		{
			int iKickVoteUserPool, iKickVoteRoundWin;
			DWORD dwKickVoteRoundTime;
			rkPacket >> iKickVoteUserPool >> iKickVoteRoundWin >> dwKickVoteRoundTime;
			KickVoteResultHelp( this, iVoteType, "", (int)dwKickVoteRoundTime / 1000, 0, 0 );
		}		
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_10:      //투표 제안 강퇴 건의 라운드 지남( 팀전 )
		{
			int iKickVoteUserPool, iKickVoteRoundWin;
			DWORD dwKickVoteRoundTime;
			rkPacket >> iKickVoteUserPool >> iKickVoteRoundWin >> dwKickVoteRoundTime;
			KickVoteResultHelp( this, iVoteType, "", iKickVoteRoundWin, 0, 0 );
		}		
		break;
	case USER_KICK_VOTE_PROPOSAL_ERROR_14:
		{
			KickVoteResultHelp( this, iVoteType, "", 0, 0, 0 );
		}
		break;
	}
}

void ioPlayStage::OnEtcItemAction( SP2Packet &rkPacket )
{
	int iResult = 0;
	rkPacket >> iResult;

	if( iResult != ETCITEM_ACTION_OK )
	{
		if( iResult == ETCITEM_ACTION_DONT_HAVE )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			LOG.PrintTimeAndLog( 0, "%s - Don't have", __FUNCTION__ );
		}
		else if( iResult == ETCITEM_ACTION_EXCEPTION )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			LOG.PrintTimeAndLog( 0, "%s - Exception", __FUNCTION__ );
		}
		else
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			LOG.PrintTimeAndLog( 0, "%s - %d UNKNOWN", __FUNCTION__, iResult  );
		}
		return;
	}

	int iType = 0;
	rkPacket >> iType;

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iType );
	if( pEtcItem )
		pEtcItem->OnAction( rkPacket, this );
	else
		LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL.", __FUNCTION__ );
}

void ioPlayStage::OnEtcItemSwitch( SP2Packet &rkPacket )
{
	int iResult = 0;
	rkPacket >> iResult;

	if( iResult != ETCITEM_SWITCH_OK )
	{
		if( iResult == ETCITEM_SWITCH_DONT_HAVE )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
			LOG.PrintTimeAndLog( 0, "%s - Don't have", __FUNCTION__ );
		}
		else if( iResult == ETCITEM_SWITCH_EXCEPTION )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
			LOG.PrintTimeAndLog( 0, "%s - Exception", __FUNCTION__ );
		}
		else
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
			LOG.PrintTimeAndLog( 0, "%s - %d UNKNOWN", __FUNCTION__, iResult  );
		}
		return;
	}

	int iType = 0;
	rkPacket >> iType;

	ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iType );
	if( pEtcItem )
		pEtcItem->OnUseSwitch( rkPacket, this );
	else
		LOG.PrintTimeAndLog( 0, "%s pEtcItem == NULL.", __FUNCTION__ );
}

void ioPlayStage::OnEtcItemLuckyCoinUseResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult = 0;
	rkPacket >> iResult;

	switch( iResult )
	{
	case ETCITEM_LUCKY_COIN_RESULT_OK:
		{
			int iEtcItemCode, iSelectIndex;
			rkPacket >> iEtcItemCode >> iSelectIndex;

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->OnItemLuckyCoinResult( iEtcItemCode, iSelectIndex );
				pInvenWnd->UpdateTab( false, false );
			}
		}
		return;
	case ETCITEM_LUCKY_COIN_RESULT_NONE_ITEM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		break;
	case ETCITEM_LUCKY_COIN_RESULT_NONE_COIN:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		break;
	}

	// UI 갱신
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true );
	}
}

void ioPlayStage::OnEtcItemRainbowMixerResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult = 0;
	rkPacket >> iResult;

	switch( iResult )
	{
	case ETCITEM_RAINBOW_MIXER_RESULT_OK:
		{
			int iEtcItemCode, iSelectIndex;
			rkPacket >> iEtcItemCode >> iSelectIndex;

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->OnRainbowMixerResult( iEtcItemCode, iSelectIndex );
				pInvenWnd->UpdateTab( false, false );
			}
		}
		return;
	case ETCITEM_RAINBOW_MIXER_RESULT_NONE_ITEM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		break;
	case ETCITEM_RAINBOW_MIXER_RESULT_NONE_MIXER:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		break;
	case ETCITEM_RAINBOW_MIXER_RESULT_NONE_ETCITEM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		break;
	}

	// UI 갱신
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true );
	}
}

void ioPlayStage::OnEtcItemLostSagaMixerResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult = 0;
	rkPacket >> iResult;

	switch( iResult )
	{
	case ETCITEM_LOSTSAGA_MIXER_RESULT_OK:
		{
			int iEtcItemCode, iSelectIndex;
			rkPacket >> iEtcItemCode >> iSelectIndex;

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->OnLostSagaMixerResult( iEtcItemCode, iSelectIndex );
				pInvenWnd->UpdateTab( false, false );
			}
		}
		return;
	case ETCITEM_LOSTSAGA_MIXER_RESULT_NONE_ITEM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		break;
	case ETCITEM_LOSTSAGA_MIXER_RESULT_NONE_MIXER:
	case ETCITEM_LOSTSAGA_MIXER_RESULT_NONE_ETCITEM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		break;
	}

	// UI 갱신
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true );
	}
}

void ioPlayStage::OnEtcItemGoldBoxResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult = 0;
	rkPacket >> iResult;

	switch( iResult )
	{
	case ETCITEM_GOLD_BOX_RESULT_OK:
		{
			int iEtcItemCode, iSelectIndex;
			rkPacket >> iEtcItemCode >> iSelectIndex;

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->OnRainbowMixerResult( iEtcItemCode, iSelectIndex );
				pInvenWnd->UpdateTab( false, false );
			}
		}
		return;
	case ETCITEM_GOLD_BOX_RESULT_NONE_ITEM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		break;
	case ETCITEM_GOLD_BOX_RESULT_NONE_MIXER:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		break;
	case ETCITEM_GOLD_BOX_RESULT_NONE_ETCITEM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		break;
	}

	// UI 갱신
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true );
	}
}

void ioPlayStage::OnEtcItemTimeGashaponUpdate( SP2Packet &rkPacket )
{
	ioUserEtcItem *pUserEtcItem = g_MyInfo.GetUserEtcItem();
	if( !pUserEtcItem ) return;

	int i = 0;
	int iUpdateCount = 0;
	rkPacket >> iUpdateCount;
	for(i = 0;i < iUpdateCount;i++)
	{
		ioUserEtcItem::ETCITEMSLOT kSlot;
		rkPacket >> kSlot.m_iType >> kSlot.m_iValue1 >> kSlot.m_iValue2;
		pUserEtcItem->SetEtcItem( kSlot );
	}

	int iDeleteCount = 0;
	rkPacket >> iDeleteCount;
	for(i = 0;i < iDeleteCount;i++)
	{
		int iEtcItem;
		rkPacket >> iEtcItem;
		pUserEtcItem->DeleteEtcItem( iEtcItem );
	}

	// UI 갱신
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true );
	}
}

void ioPlayStage::OnRoomUserEtcItemSync( SP2Packet &rkPacket )
{
	int iSize;
	rkPacket >> iSize;
	for(int i = 0;i < iSize;i++)
	{
		ioHashString kName;
		rkPacket >> kName;

		int iEtcItemSize;
		rkPacket >> iEtcItemSize;
		for(int j = 0;j < iEtcItemSize;j++)
		{
			int iEtcItemType;
			rkPacket >> iEtcItemType;       
			
			// 권한 아이템에 따라 다른 작동
			switch( iEtcItemType )
			{
			case ioEtcItem::EIT_ETC_SKELETON_BIG:
			case ioEtcItem::EIT_ETC_SKELETON_BIGHEAD:
			case ioEtcItem::EIT_ETC_SKELETON_SMALL:
				{
					SkeletonEtcItemSync( kName, iEtcItemType );
				}
				break;
			default:
				{
					LOG.PrintTimeAndLog( 0, "%s Error %s : %d", __FUNCTION__, kName.c_str(), iEtcItemType );
				}
				break;
			}
		}
	}
}

void ioPlayStage::OnEtcItemSkeletonDelete( SP2Packet &rkPacket )
{
	int iEtcItem;
	rkPacket >> iEtcItem;

	ioHashString kName;
	rkPacket >> kName; 

	SkeletonEtcItemSync( kName, 0 );

	if( kName == g_MyInfo.GetPublicID() )
	{
		MyInventoryWnd *pMyInventoryWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd( MY_INVENTORY_WND ));
		if( pMyInventoryWnd && pMyInventoryWnd->IsShow() )
			pMyInventoryWnd->UpdateTab( false, true );

		ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iEtcItem );
		if( pEtcItem )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), pEtcItem->GetName().c_str() );
		}
	}
}

void ioPlayStage::OnMyRoomServerChange( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	int iResult;
	rkPacket >> iResult;
	switch( iResult )
	{
	case MYROOM_SERVER_CHANGE_NOT_MYROOM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		break;
	case MYROOM_SERVER_CHANGE_RESERVE_MOVE:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		break;
	case MYROOM_SERVER_CHANGE_ALREADY_SERVER:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		break;
	case MYROOM_SERVER_CHANGE_NOT_SERVER:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		break;
	case MYROOM_SERVER_CHANGE_BATTLE_ROOM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		break;
	case MYROOM_SERVER_CHANGE_LADDER_TEAM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		break;
	}
}

void ioPlayStage::OnPresentBuy( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult = 0;
	rkPacket >> iResult;

	if( iResult == PRESENT_BUY_BILLING_WAIT )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
	else if( iResult == PRESENT_BUY_WANT_OF_CASH )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
	}
	else if( iResult == PRESENT_BUY_BILLING_DISCONNECT )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
	}
	else if( iResult == PRESENT_BUY_BILLING_FAIL )
	{	
		bool bBillingError    = false;
		ioHashString  sBillingError;
		rkPacket >> bBillingError;
		if( bBillingError )
			rkPacket >> sBillingError;

		ioHashString sErrorMent = STR(4);
		if( !bBillingError )
			g_GUIMgr.SetMsgBox( MB_OK, NULL, sErrorMent.c_str() );
		else
		{
			ioHashString sError = "Billing Error : ";
			ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
			if( pLocal ) 
				sError = pLocal->GetBillingErrorMent(sBillingError);

			char szErrorFullMent[MAX_PATH]="";
			ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
			if( pNode && pNode->GetType() == CNT_NEXON )					
				StringCbPrintf( szErrorFullMent, sizeof( szErrorFullMent ), "%s",  sError.c_str() ); //Except Extracting Hangeul
			else
				StringCbPrintf( szErrorFullMent, sizeof( szErrorFullMent ), "%s#%s%s", sErrorMent.c_str(), sError.c_str(), sBillingError.c_str() ); //Except Extracting Hangeul

			g_GUIMgr.SetMsgBox( MB_OK, NULL, szErrorFullMent );
		}
	}
	else if( iResult == PRESENT_BUY_BILLING_EXCESS_BUY )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
	}
	else if( iResult == PRESENT_BUY_ERROR_RECEIVERID )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
	}
	else if( iResult == PRESENT_BUY_NOT_RECEIVERID )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
	}
	else if( iResult == PRESENT_BUY_EXCEED_PRESENT_CNT )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
	}
	else if( iResult == PRESENT_BUY_DISABLE_SEND )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "해당 상품은 선물할 수 없습니다" );
	}
	else if( iResult == PRESENT_BUY_SUCCESS )
	{
		__int64 iMoney      = 0;
		int iCash           = 0;
		int iPurchasedCash  = 0;
		int iChannelingCash = 0;

		rkPacket >> iMoney >> iCash >> iPurchasedCash >> iChannelingCash;
		
#if defined( USE_GA )
		int iCashPrice	= g_MyInfo.GetCash() - iCash;
#endif

		//사운드
		if( iMoney != g_MyInfo.GetMoney() || iCash != g_MyInfo.GetCash() )
			g_ExSoundMgr.PlaySound( ExSound::EST_SHOP_BUY_SUCCESS );

		g_MyInfo.SetMoney( iMoney );
		g_MyInfo.SetCash( iCash );
		g_MyInfo.SetPurchasedCash( iPurchasedCash );
		g_MyInfo.SetChannelingCash( iChannelingCash );

		SendPresentConfirmWnd *pConfirmWnd = dynamic_cast< SendPresentConfirmWnd* > ( g_GUIMgr.FindWnd( SEND_PRESENT_CONFIRM_WND ) );
		if( !pConfirmWnd )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(9) );
		}
		else
		{
			pConfirmWnd->SendPresentComplete();
		}

		LOG.PrintTimeAndLog( 0, "Success present buy : %I64d|%d|%d|%d", iMoney, iCash, iPurchasedCash, iChannelingCash );

#if defined( USE_GA )
		if( iCashPrice > 0 && iCashPrice < 1000000 )
		{
			// GOLD_USE_GIFT
			g_HttpMng.GA_ItemHitTracking( g_MyInfo.GetUserIndex() 
				, "Gift"
				, g_HttpMng.GetGiftCash()
				, 1
				, g_HttpMng.GetGiftIndex()
				, "Gold"
				, "%2FGOLD%2FUSE%2FGIFT"
				, true );

			g_HttpMng.SetGiftIndex( 0 );
			g_HttpMng.SetGiftCash( 0 );
		}		
#endif
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Type is Error(%d).", __FUNCTION__, iResult );
	}
}

void ioPlayStage::OnExcavation( SP2Packet &rkPacket )
{
	g_ExcavationMgr.OnExcavationCommand( rkPacket );
}

void ioPlayStage::CheckEventStruct( int iEventType )			// 훈련소 들어갈때
{
	if( GetModeType() != MT_HEADQUARTERS )
		return;

	bool bAlive = g_EventMgr.IsAlive( (EventType)iEventType, g_MyInfo.GetChannelingType() );
	bool bEventStructExist = IsExistEventStruct( iEventType );

	// 이벤트 유효 체크, 인벤트 구조물 이미 존제하는지 체크
	if( bAlive && !bEventStructExist )
	{
		ioINILoader kStructLoader = g_PushStructListMgr.GetINILoader();

		ioINILoader_e kEventLoader( "config/sp2_event_struct.ini" );
		kEventLoader.SetTitle_e( "common" );

		char szKey[MAX_PATH] = "";
		char szTitle[MAX_PATH] = "";
		wsprintf_e( szKey, "event%d_struct_cnt", iEventType );

		int iStructCnt = kEventLoader.LoadInt( szKey, 0 );
		if( iStructCnt == 0 ) return;

		EventStruct kEventStruct;
		kEventStruct.m_iEventType = iEventType;

		for( int i=0; i < iStructCnt; ++i )
		{
			// 구조물 번호, 위치 읽어오기
			D3DXVECTOR3 vStartPos = ioMath::VEC3_ZERO;

			wsprintf_e( szKey, "event%d_struct%d_num", iEventType, i+1 );
			int iStructNum = kEventLoader.LoadInt( szKey, 0 );
			if( iStructNum == 0 ) continue;

			wsprintf_e( szKey, "event%d_struct%d_pos_x", iEventType, i+1 );
			vStartPos.x = kEventLoader.LoadFloat( szKey, 0.0f );
			wsprintf_e( szKey, "event%d_struct%d_pos_y", iEventType, i+1 );
			vStartPos.y = kEventLoader.LoadFloat( szKey, 0.0f );
			wsprintf_e( szKey, "event%d_struct%d_pos_z", iEventType, i+1 );
			vStartPos.z = kEventLoader.LoadFloat( szKey, 0.0f );

			// 구조물 실제 생성
			wsprintf_e( szTitle, "push_struct%d", iStructNum );
			kStructLoader.SetTitle( szTitle );

			char szStruct[MAX_PATH];
			kStructLoader.LoadString_e( "push_struct", "", szStruct, MAX_PATH );

			ioPushStruct *pPush = g_PushStructListMgr.CreatePushStruct( szStruct, m_pPlayMode );
			if( pPush )
			{
				pPush->InitResource();
				pPush->LoadProperty( kStructLoader );
				pPush->SetShadowCastEnable( true );

				D3DXVECTOR3 vScale;
				vScale.x = kStructLoader.LoadFloat_e( "scale_x", FLOAT1 );
				vScale.y = kStructLoader.LoadFloat_e( "scale_y", FLOAT1 );
				vScale.z = kStructLoader.LoadFloat_e( "scale_z", FLOAT1 );

				if( vStartPos.y < FLOAT1 )
				{
					vStartPos.y = GetMapHeight( vStartPos.x, vStartPos.z, pPush, false );
				}

				DWORD dwSeed = timeGetTime();
				int iCurIndex = g_PushStructListMgr.GetCurLocalIndex();
				pPush->InitPushStructInfo( iStructNum, iCurIndex, 0, dwSeed, 0, vStartPos, vScale );
				AddGameEntity( pPush );

				int iIndex = pPush->GetPushArrayIdx();
				kEventStruct.m_vStructList.push_back( iIndex );
			}
		}

		// 구조물 존재하면 추가
		if( !kEventStruct.m_vStructList.empty() )
		{
			m_EventStructList.push_back( kEventStruct );
		}
	}
}

void ioPlayStage::CheckEventStruct( int iEventType, bool bAlive )		// 서버로부터 정보 받았을때
{
	if( GetModeType() != MT_HEADQUARTERS )
		return;

	bool bEventStructExist = IsExistEventStruct( iEventType );

	// 이벤트 유효 체크, 인벤트 구조물 이미 존제하는지 체크
	if( bAlive && !bEventStructExist )
	{
		ioINILoader kStructLoader = g_PushStructListMgr.GetINILoader();

		ioINILoader_e kEventLoader( "config/sp2_event_struct.ini" );
		kEventLoader.SetTitle_e( "common" );

		char szKey[MAX_PATH] = "";
		char szTitle[MAX_PATH] = "";
		wsprintf_e( szKey, "event%d_struct_cnt", iEventType );

		int iStructCnt = kEventLoader.LoadInt( szKey, 0 );
		if( iStructCnt == 0 ) return;

		EventStruct kEventStruct;
		kEventStruct.m_iEventType = iEventType;

		for( int i=0; i < iStructCnt; ++i )
		{
			// 구조물 번호, 위치 읽어오기
			D3DXVECTOR3 vStartPos = ioMath::VEC3_ZERO;

			wsprintf_e( szKey, "event%d_struct%d_num", iEventType, i+1 );
			int iStructNum = kEventLoader.LoadInt( szKey, 0 );
			if( iStructNum == 0 ) continue;

			wsprintf_e( szKey, "event%d_struct%d_pos_x", iEventType, i+1 );
			vStartPos.x = kEventLoader.LoadFloat( szKey, 0.0f );
			wsprintf_e( szKey, "event%d_struct%d_pos_y", iEventType, i+1 );
			vStartPos.y = kEventLoader.LoadFloat( szKey, 0.0f );
			wsprintf_e( szKey, "event%d_struct%d_pos_z", iEventType, i+1 );
			vStartPos.z = kEventLoader.LoadFloat( szKey, 0.0f );

			// 구조물 실제 생성
			wsprintf_e( szTitle, "push_struct%d", iStructNum );
			kStructLoader.SetTitle( szTitle );

			char szStruct[MAX_PATH];
			kStructLoader.LoadString_e( "push_struct", "", szStruct, MAX_PATH );

			ioPushStruct *pPush = g_PushStructListMgr.CreatePushStruct( szStruct, m_pPlayMode );
			if( pPush )
			{
				pPush->InitResource();
				pPush->LoadProperty( kStructLoader );
				pPush->SetShadowCastEnable( true );

				D3DXVECTOR3 vScale;
				vScale.x = kStructLoader.LoadFloat_e( "scale_x", FLOAT1 );
				vScale.y = kStructLoader.LoadFloat_e( "scale_y", FLOAT1 );
				vScale.z = kStructLoader.LoadFloat_e( "scale_z", FLOAT1 );

				if( vStartPos.y < FLOAT1 )
				{
					vStartPos.y = GetMapHeight( vStartPos.x, vStartPos.z, pPush, false );
				}

				DWORD dwSeed = timeGetTime();
				int iCurIndex = g_PushStructListMgr.GetCurLocalIndex();
				pPush->InitPushStructInfo( iStructNum, iCurIndex, 0, dwSeed, 0, vStartPos, vScale );
				AddGameEntity( pPush );

				int iIndex = pPush->GetPushArrayIdx();
				kEventStruct.m_vStructList.push_back( iIndex );
			}
		}

		// 구조물 존재하면 추가
		if( !kEventStruct.m_vStructList.empty() )
		{
			m_EventStructList.push_back( kEventStruct );
		}
	}
	else if( !bAlive && bEventStructExist )			// 이벤트 종료상황 체크, 구조물 존재 체크
	{
		int iSize = m_EventStructList.size();
		for( int i=0; i < iSize; ++i )
		{
			if( m_EventStructList[i].m_iEventType != iEventType )
				continue;

			int iStructSize = m_EventStructList[i].m_vStructList.size();
			for( int j=0; j < iStructSize; ++j )
			{
				int iStructIndex = m_EventStructList[i].m_vStructList[j];
				ioPushStruct *pPush = g_PushStructListMgr.FindPushStruct( iStructIndex );
				if( pPush )
				{
					pPush->SetStructDie( NULL, true );					
				}
			}

			m_EventStructList.erase( m_EventStructList.begin() + i );
			break;
		}
	}
}

bool ioPlayStage::IsExistEventStruct( int iEventType )
{
	int iSize = m_EventStructList.size();

	for( int i=0; i < iSize; ++i )
	{
		if( m_EventStructList[i].m_iEventType == iEventType )
			return true;
	}

	return false;
}

void ioPlayStage::SkeletonEtcItemSync( const ioHashString &rkName, int iEtcItem )
{
	if( m_pPlayMode )
		m_pPlayMode->SkeletonEtcItemSync( rkName, iEtcItem );
}

void ioPlayStage::ChangeMultiMapIndex( int iMapIndex )
{
	if( m_pSceneMgr )
	{
		m_pSceneMgr->SetMultiMapCurrentIndex( iMapIndex );
	}
	SetMapDefaultObjectQuality( Setting::GetMapObjectQuality() );
}

void ioPlayStage::OnItemCompound( SP2Packet &rkPacket )
{
	int iType;
	rkPacket >> iType;

	TCPNetwork::MouseBusy( false );

	// HARDCODE : 성공 상황에서는 break가 아닌 return을 사용한다.

	switch( iType )
	{
	case ITEM_COMPOUND_OK:
		{
			bool bSuccess;
			int iEtcType;
			int iTargetSlot, iVictimSlot;
			int iTargetReinforce;

			rkPacket >> iEtcType;
			rkPacket >> bSuccess;
			rkPacket >> iTargetSlot >> iTargetReinforce;
			rkPacket >> iVictimSlot;

			// HARDCODE : 갱신전 정보값 활용때문에 UI쪽을 먼저 진행.
			// UI 출력
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->OnItemCompoundResult( bSuccess, iTargetSlot, iTargetReinforce, iEtcType );
				pInvenWnd->UpdateTab( false, false );
			}

			ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
			if( pExtraItem )
			{
				if( !pExtraItem->DeleteExtraItemData( iVictimSlot ) )
				{
					LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemCompound - Victim Error" );
				}

				EXTRAITEMSLOT kTargetSlot;
				if( pExtraItem->GetExtraItem( iTargetSlot, kTargetSlot ) )
				{
					kTargetSlot.m_iReinforce = iTargetReinforce;
					kTargetSlot.m_failExp = 0;
					pExtraItem->SetExtraItem( kTargetSlot );
				}
				else
				{
					LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemCompound - Target Error" );
				}
			}
		}
		return;
	case ITEM_COMPOUND_ERROR:
		{
			int iErrorNum;
			rkPacket >> iErrorNum;

			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), iErrorNum );
		}
		break;
	case ITEM_MULTIPLE_COMPOUND_EXCEPTION:
		{
			int iErrorNum;
			rkPacket >> iErrorNum;

			int iItem1, iItem2, iItem3;
			rkPacket >> iItem1 >> iItem2 >> iItem3;
			ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
			if( iItem1 > 0 && !pExtraItem->DeleteExtraItemData( iItem1 ) )
				LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemCompound - ItemDel Error(1)" );

			if( iItem2 > 0 && !pExtraItem->DeleteExtraItemData( iItem2 ) )
				LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemCompound - ItemDel Error(2)" );

			if( iItem3 > 0 && !pExtraItem->DeleteExtraItemData( iItem3 ) )
				LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemCompound - ItemDel Error(3)" );

			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2), iErrorNum );
		}
		break;
	case ITEM_MULTIPLE_COMPOUND_ITEM_ERROR:
		{
			int iItemSlot;
			rkPacket >> iItemSlot;

			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3), iItemSlot );
		}
		break;
	case ITEM_MULTIPLE_COMPOUND_EQUAL_ERROR:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		}
		break;
	case ITEM_MULTIPLE_COMPOUND_DEL_ERROR:
		{
			int iItem1, iItem2, iItem3;
			bool bItem1, bItem2, bItem3;
			rkPacket >> iItem1 >> iItem2 >> iItem3;
			rkPacket >> bItem1 >> bItem2 >> bItem3;

			ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
			if( !bItem1 )
			{
				if( !pExtraItem->DeleteExtraItemData( iItem1 ) )
				{
					LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemCompound - ItemDel Error(1)" );
				}
			}

			if( !bItem2 )
			{
				if( !pExtraItem->DeleteExtraItemData( iItem2 ) )
				{
					LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemCompound - ItemDel Error(2)" );
				}
			}

			if( !bItem3 )
			{
				if( !pExtraItem->DeleteExtraItemData( iItem3 ) )
				{
					LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemCompound - ItemDel Error(3)" );
				}
			}

			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5), (int)bItem1, (int)bItem2, (int)bItem3 );
		}
		break;
	case ITEM_MULTIPLE_COMPOUND_OK:
		{
			int iEtcType;
			int iItem1, iItem2, iItem3;
			int iItemCode, iReinforce, iSlotIndex, iPeriodType, iValue1, iValue2, iPeriodTime;

			rkPacket >> iEtcType;
			rkPacket >> iItem1 >> iItem2 >> iItem3;
			rkPacket >> iItemCode >> iReinforce >> iSlotIndex >> iPeriodType >> iValue1 >> iValue2 >> iPeriodTime;

			ioUserExtraItem *pUserExtraItem = g_MyInfo.GetUserExtraItem();
			if( pUserExtraItem )
			{
				// 삭제
				if( iItem1 > 0 && !pUserExtraItem->DeleteExtraItemData( iItem1 ) )
					LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemCompound - ItemDel Error(1)" );

				if( iItem2 > 0 && !pUserExtraItem->DeleteExtraItemData( iItem2 ) )
					LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemCompound - ItemDel Error(2)" );

				if( iItem3 > 0 && !pUserExtraItem->DeleteExtraItemData( iItem3 ) )
					LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemCompound - ItemDel Error(3)" );

				// 추가
				EXTRAITEMSLOT kSlot;
				kSlot.m_iItemCode = iItemCode;
				kSlot.m_iReinforce = iReinforce;
				kSlot.m_iIndex = iSlotIndex;
				kSlot.m_PeriodType = iPeriodType;
				kSlot.m_iValue1 = iValue1;
				kSlot.m_iValue2 = iValue2;

				pUserExtraItem->AddExtraItemData( kSlot );

				// UI 처리
				MyInventoryWnd *pWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
				if( pWnd && pWnd->IsShow() )
				{
					pWnd->OnMultipleCompoundResult( iEtcType, kSlot.m_iItemCode, kSlot.m_iReinforce );
					pWnd->UpdateTab( false, false );
				}
			}
		}
		return;
	case ITEM_NEW_MULTIPLE_COMPOUND_OK:
		{
			int iPresentType;
			rkPacket >> iPresentType;
			
			if ( iPresentType == PRESENT_MEDALITEM )
			{
				int iItemIndex1, iItemIndex2, iItemIndex3;
				rkPacket >> iItemIndex1 >> iItemIndex2 >> iItemIndex3 ;

				// 데이터 삭제
				if( !g_MyInfo.DeleteMedalItem( iItemIndex1, 0 ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s 메달 삭제 예외오류, %d", __FUNCTION__, iItemIndex1 );
					return;
				}

				if( !g_MyInfo.DeleteMedalItem( iItemIndex2, 0 ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s 메달 삭제 예외오류, %d", __FUNCTION__, iItemIndex2 );
					return;
				}

				if( !g_MyInfo.DeleteMedalItem( iItemIndex3, 0 ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s 메달 삭제 예외오류, %d", __FUNCTION__, iItemIndex3 );
					return;
				}
			}
			else if ( iPresentType == PRESENT_COSTUME )
			{
				int iItemIndex1, iItemIndex2, iItemIndex3;
				rkPacket >> iItemIndex1 >> iItemIndex2 >> iItemIndex3 ;
				// 데이터 삭제
				if( !g_MyInfo.DeleteCostume( iItemIndex1 ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s 코스튬 삭제 예외오류 %d", __FUNCTION__, iItemIndex1 );
					return;
				}

				if( !g_MyInfo.DeleteCostume( iItemIndex2 ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s 코스튬 삭제 예외오류 %d", __FUNCTION__, iItemIndex2 );
					return;
				}

				if( !g_MyInfo.DeleteCostume( iItemIndex3 ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, "%s 코스튬 삭제 예외오류 %d", __FUNCTION__, iItemIndex3 );
					return;
				}
			}
			else if ( iPresentType == PRESENT_EXTRAITEM )
			{
				int iItemIndex1, iItemIndex2, iItemIndex3;
				rkPacket >> iItemIndex1 >> iItemIndex2 >> iItemIndex3 ;
				// 데이터 삭제
				if( !g_MyInfo.DeleteExtraItem( iItemIndex1 ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4), __FUNCTION__, iItemIndex1 );
					return;
				}

				if( !g_MyInfo.DeleteExtraItem( iItemIndex2 ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5), __FUNCTION__, iItemIndex2 );
					return;
				}

				if( !g_MyInfo.DeleteExtraItem( iItemIndex3 ) )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6), __FUNCTION__, iItemIndex3 );
					return;
				}
			}
		}
		return;
	}

	// UI 갱신
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, true );
	}
}

float ioPlayStage::GetRoomGravity()
{
	if( g_BattleRoomMgr.IsBattleRoom() &&
		m_pPlayMode &&
		m_pPlayMode->GetModeType() != MT_TRAINING		&&
		m_pPlayMode->GetModeType() != MT_MYROOM			&&
		m_pPlayMode->GetModeType() != MT_HEADQUARTERS	&&
		m_pPlayMode->GetModeType() != MT_HOUSE )
	{
		if( g_BattleRoomMgr.IsUseExtraOption() )
		{
			int iType = g_BattleRoomMgr.GetGravityType();
			return m_fMapGravity * g_RoomOptionMgr.GetGravityRate( iType );
		}
	}
	return m_fMapGravity;
}

float ioPlayStage::GetRoomFriction()
{
	return m_fMapFriction;
}

bool ioPlayStage::CheckRoomChangeChar()
{
	if( g_BattleRoomMgr.IsBattleRoom() &&
		m_pPlayMode &&
		m_pPlayMode->GetModeType() != MT_TRAINING		&&
		m_pPlayMode->GetModeType() != MT_MYROOM			&&
		m_pPlayMode->GetModeType() != MT_HEADQUARTERS	&&
		m_pPlayMode->GetModeType() != MT_HOUSE )
	{
		if( g_BattleRoomMgr.IsUseExtraOption() )
		{
			int iType = g_BattleRoomMgr.GetChangeCharType();
			return g_RoomOptionMgr.CheckChangeCharType( iType );
		}
	}
	return true;
}

float ioPlayStage::GetRoomCoolTimeRate()
{
	if( g_BattleRoomMgr.IsBattleRoom() &&
		m_pPlayMode &&
		m_pPlayMode->GetModeType() != MT_TRAINING		&&
		m_pPlayMode->GetModeType() != MT_MYROOM			&&
		m_pPlayMode->GetModeType() != MT_HEADQUARTERS	&&
		m_pPlayMode->GetModeType() != MT_HOUSE )
	{
		if( g_BattleRoomMgr.IsUseExtraOption() )
		{
			int iType = g_BattleRoomMgr.GetCoolTimeType();
			return g_RoomOptionMgr.GetCoolTimeRate( iType );
		}
	}
	return FLOAT1;
}

float ioPlayStage::GetRoomHPRate( TeamType eTeam )
{
	if( g_BattleRoomMgr.IsBattleRoom() &&
		m_pPlayMode &&
		m_pPlayMode->GetModeType() != MT_TRAINING		&&
		m_pPlayMode->GetModeType() != MT_MYROOM			&&
		m_pPlayMode->GetModeType() != MT_HEADQUARTERS	&&
		m_pPlayMode->GetModeType() != MT_HOUSE )
	{
		if( g_BattleRoomMgr.IsUseExtraOption() )
		{
			int iIndex = 0;
			if( eTeam == TEAM_RED )
			{
				iIndex = g_BattleRoomMgr.GetRedHPType();
				return g_RoomOptionMgr.GetRedHPRate( iIndex );
			}
			else if( eTeam == TEAM_BLUE )
			{
				iIndex = g_BattleRoomMgr.GetBlueHPType();
				return g_RoomOptionMgr.GetBlueHPRate( iIndex );
			}
		}
	}
	return FLOAT1;
}

float ioPlayStage::GetRoomDropDamageRate()
{
	if( g_BattleRoomMgr.IsBattleRoom() &&
		m_pPlayMode &&
		m_pPlayMode->GetModeType() != MT_TRAINING		&&
		m_pPlayMode->GetModeType() != MT_MYROOM			&&
		m_pPlayMode->GetModeType() != MT_HEADQUARTERS	&&
		m_pPlayMode->GetModeType() != MT_HOUSE )
	{
		if( g_BattleRoomMgr.IsUseExtraOption() )
		{
			int iType = g_BattleRoomMgr.GetDropDamageType();
			return g_RoomOptionMgr.GetDropDamageRate( iType );
		}
	}
	return FLOAT1;
}

bool ioPlayStage::CheckRoomFastGetUp()
{
	if( g_BattleRoomMgr.IsBattleRoom() &&
		m_pPlayMode &&
		m_pPlayMode->GetModeType() != MT_TRAINING		&&
		m_pPlayMode->GetModeType() != MT_MYROOM			&&
		m_pPlayMode->GetModeType() != MT_HEADQUARTERS	&&
		m_pPlayMode->GetModeType() != MT_HOUSE )
	{
		if( g_BattleRoomMgr.IsUseExtraOption() )
		{
			int iType = g_BattleRoomMgr.GetGetUpType();
			float fRate = g_RoomOptionMgr.GetGetUpRate( iType );

			if( fRate <= 0.0f )
				return true;
		}
	}
	return false;
}

bool ioPlayStage::CheckRoomWeakWounded( TeamType eTeam )
{
	if( g_BattleRoomMgr.IsBattleRoom() && GetModeType() != MT_TRAINING && GetModeType() != MT_MYROOM && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
	{
		if( g_BattleRoomMgr.IsUseExtraOption() )
		{
			int iIndex = 0;
			float fRate = 0.0f;
			if( eTeam == TEAM_RED )
			{
				iIndex = g_BattleRoomMgr.GetRedBlowType();
				fRate = g_RoomOptionMgr.GetRedBlowRate( iIndex );
			}
			else if( eTeam == TEAM_BLUE )
			{
				iIndex = g_BattleRoomMgr.GetBlueBlowType();
				fRate = g_RoomOptionMgr.GetBlueBlowRate( iIndex );
			}

			if( fRate < 0.0f )
				return true;
		}

		if( GetModeType() == MT_GANGSI )
		{
			if( eTeam == TEAM_BLUE )  // 강시팀 강타를 받지 않는다.
				return true;
		}
	}
	return false;
}

float ioPlayStage::GetRoomGetUpRate()
{
	if( g_BattleRoomMgr.IsBattleRoom() &&
		m_pPlayMode &&
		m_pPlayMode->GetModeType() != MT_TRAINING		&&
		m_pPlayMode->GetModeType() != MT_MYROOM			&&
		m_pPlayMode->GetModeType() != MT_HEADQUARTERS	&&
		m_pPlayMode->GetModeType() != MT_HOUSE )
	{
		if( g_BattleRoomMgr.IsUseExtraOption() )
		{
			int iType = g_BattleRoomMgr.GetGetUpType();
			float fRate = g_RoomOptionMgr.GetGetUpRate( iType );

			if( fRate > 0.0f )
				return fRate;
		}
	}
	return FLOAT1;
}

float ioPlayStage::GetRoomMoveSpeedRate( TeamType eTeam )
{
	if( g_BattleRoomMgr.IsBattleRoom() &&
		m_pPlayMode &&
		m_pPlayMode->GetModeType() != MT_TRAINING		&&
		m_pPlayMode->GetModeType() != MT_MYROOM			&&
		m_pPlayMode->GetModeType() != MT_HEADQUARTERS	&&
		m_pPlayMode->GetModeType() != MT_HOUSE )
	{
		if( g_BattleRoomMgr.IsUseExtraOption() )
		{
			int iIndex = 0;
			if( eTeam == TEAM_RED )
			{
				iIndex = g_BattleRoomMgr.GetRedMoveSpeedType();
				return g_RoomOptionMgr.GetRedMoveSpeedRate( iIndex );
			}
			else if( eTeam == TEAM_BLUE )
			{
				iIndex = g_BattleRoomMgr.GetBlueMoveSpeedType();
				return g_RoomOptionMgr.GetBlueMoveSpeedRate( iIndex );
			}
		}
	}
	return FLOAT1;
}

bool ioPlayStage::CheckRoomZeroKO()
{
	if( g_BattleRoomMgr.IsBattleRoom() &&
		m_pPlayMode &&
		m_pPlayMode->GetModeType() != MT_TRAINING		&&
		m_pPlayMode->GetModeType() != MT_MYROOM			&&
		m_pPlayMode->GetModeType() != MT_HEADQUARTERS	&&
		m_pPlayMode->GetModeType() != MT_HOUSE )
	{
		if( Help::IsMonsterDungeonMode(m_pPlayMode->GetModeType()) || m_pPlayMode->GetModeType() == MT_RAID )
		{
			return true;
		}
		else
		{
			if( g_BattleRoomMgr.IsUseExtraOption() )
			{
				int iType = g_BattleRoomMgr.GetKOType();
				float fRate = g_RoomOptionMgr.GetKORate( iType );

				if( fRate < 0.0f )
					return true;
			}
		}
	}
	return false;
}

bool ioPlayStage::CheckRoomKOEffect()
{
	if( g_BattleRoomMgr.IsBattleRoom() &&
		m_pPlayMode &&
		m_pPlayMode->GetModeType() != MT_TRAINING		&&
		m_pPlayMode->GetModeType() != MT_MYROOM			&&
		m_pPlayMode->GetModeType() != MT_HEADQUARTERS	&&
		m_pPlayMode->GetModeType() != MT_HOUSE )
	{
		if( g_BattleRoomMgr.IsUseExtraOption() )
		{
			int iType = g_BattleRoomMgr.GetKOEffectType();
			float fRate = g_RoomOptionMgr.GetKOEffectRate( iType );

			if( fRate < 0.0f )
				return false;
		}
	}
	return true;
}

bool ioPlayStage::CheckRoomEquip( TeamType eTeam )
{
	if( g_BattleRoomMgr.IsBattleRoom() &&
		m_pPlayMode &&
		m_pPlayMode->GetModeType() != MT_TRAINING		&&
		m_pPlayMode->GetModeType() != MT_MYROOM			&&
		m_pPlayMode->GetModeType() != MT_HEADQUARTERS	&&
		m_pPlayMode->GetModeType() != MT_HOUSE )
	{
		if( g_BattleRoomMgr.IsUseExtraOption() )
		{
			int iIndex = 0;
			float fRate = 0.0f;
			if( eTeam == TEAM_RED )
			{
				iIndex = g_BattleRoomMgr.GetRedEquipType();
				fRate = g_RoomOptionMgr.GetRedEquipRate( iIndex );
			}
			else if( eTeam == TEAM_BLUE )
			{
				iIndex = g_BattleRoomMgr.GetBlueEquipType();
				fRate = g_RoomOptionMgr.GetBlueEquipRate( iIndex );
			}

			if( fRate < 0.0f )
				return false;
		}
	}
	return true;
}

bool ioPlayStage::CheckRoomGrowthUse()
{
	if( g_BattleRoomMgr.IsBattleRoom() &&
		m_pPlayMode &&
		m_pPlayMode->GetModeType() != MT_TRAINING		&&
		m_pPlayMode->GetModeType() != MT_MYROOM			&&
		m_pPlayMode->GetModeType() != MT_HEADQUARTERS	&&
		m_pPlayMode->GetModeType() != MT_HOUSE )
	{
		if( g_BattleRoomMgr.IsUseExtraOption() )
		{
			int iType = g_BattleRoomMgr.GetGrowthUseType();
			return g_RoomOptionMgr.CheckEnableGrowthUse( iType );
		}
	}

	return true;
}

bool ioPlayStage::CheckRoomExtraItemUse()
{
	if( g_BattleRoomMgr.IsBattleRoom() &&
		m_pPlayMode &&
		m_pPlayMode->GetModeType() != MT_TRAINING		&&
		m_pPlayMode->GetModeType() != MT_MYROOM			&&
		m_pPlayMode->GetModeType() != MT_HEADQUARTERS	&&
		m_pPlayMode->GetModeType() != MT_HOUSE )
	{
		if( g_BattleRoomMgr.IsUseExtraOption() )
		{
			int iType = g_BattleRoomMgr.GetExtraItemUseType();
			return g_RoomOptionMgr.CheckEnableExtraItemUse( iType );
		}
	}

	return true;
}

void ioPlayStage::OnTraceWeaponTrace( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnTraceWeaponTrace - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	int iWeaponIdx;
	rkPacket >> iWeaponIdx;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pAttacker, iWeaponIdx );

	ioTraceWeapon *pTrace = ToTraceWeapon( pWeapon );
	ioTraceWeapon4 *pTrace4 = ToTraceWeapon4( pWeapon );
	ioGroundTraceWeapon *pGroundTrace = ToGroundTraceWeapon( pWeapon );
	ioTraceWeapon6 *pTrace6 = ToTraceWeapon6( pWeapon );
	ioGroundTraceWeapon2 *pGroundTrace2 = ToGroundTraceWeapon2( pWeapon );
	ioGroundTraceWeapon3 *pGroundTrace3 = ToGroundTraceWeapon3( pWeapon );

	if( pTrace || pTrace4 || pGroundTrace || pTrace6 || pGroundTrace2 || pGroundTrace3 )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		ioHashString szTargetName;
		rkPacket >> szTargetName;

		if( pTrace )
		{
			pTrace->SetPosition( vPos );
			pTrace->SetTraceTarget( szTargetName );
			pTrace->SetTraceState( this, false );
		}
		else if( pTrace4 )
		{
			pTrace4->SetPosition( vPos );
			pTrace4->SetTraceTarget( szTargetName );
			pTrace4->SetTraceState( this, false );
		}
		else if( pGroundTrace )
		{
			pGroundTrace->SetPosition( vPos );
			pGroundTrace->SetTraceTarget( szTargetName );
			pGroundTrace->SetTraceState( this, false );
		}
		else if( pTrace6 )
		{
			pTrace6->SetPosition( vPos );
			pTrace6->SetTraceTarget( szTargetName );
			pTrace6->SetTraceState( this, false );
		}
		else if( pGroundTrace2 )
		{
			pGroundTrace2->SetPosition( vPos );
			pGroundTrace2->SetTraceTarget( szTargetName );
			pGroundTrace2->SetTraceState( this, false );
		}
		else if( pGroundTrace3 )
		{
			pGroundTrace3->SetPosition( vPos );
			pGroundTrace3->SetTraceTarget( szTargetName );
			pGroundTrace3->SetTraceState( this, false );
		}
	}
}

void ioPlayStage::OnTradeStateChange( SP2Packet &rkPacket )
{
	int iResult;
	rkPacket >> iResult;

	g_GUIMgr.HideWnd(CHANGE_TRADE_STATE_WND);
	g_GUIMgr.HideWnd(CHANGE_CONFIRM_WND);

	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));

	bool bInfo = true;
	switch( iResult )
	{
	case TRADE_STATE_CHANGE_FAIL:
		{
			TCPNetwork::MouseBusy( false );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		}
		break;
	case TRADE_STATE_CHANGE_NO_ITEM:
		{
			TCPNetwork::MouseBusy( false );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
		}
		break;
	case TRADE_STATE_CHANGE_EQUIP_ITEM:
		{
			TCPNetwork::MouseBusy( false );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		}
		break;
	case TRADE_STATE_CHANGE_ERROR:
		{
			TCPNetwork::MouseBusy( false );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(4) );
		}
		break;
	case TRADE_STATE_CHANGE_OK:
		{
			TCPNetwork::MouseBusy( false );

			int iSlotIndex;
			rkPacket >> iSlotIndex;

			ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
			if( pItem )
			{
				EXTRAITEMSLOT kSlot;
				if( pItem->GetExtraItem( iSlotIndex, kSlot ) )
				{
					kSlot.m_iTradeState = EET_ENABLE;
					pItem->SetExtraItem( kSlot );
				}
			}

			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->OnChangeTradeStateWnd( iSlotIndex );
				bInfo = false;
			}
		}
		break;
	}

	if( pInvenWnd && pInvenWnd->IsShow() )
	{
		pInvenWnd->UpdateTab( false, bInfo );
	}
}

void ioPlayStage::OnTradeCreate( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case TRADE_CREATE_OK:
		{
			TradeItemInfo Info;
			rkPacket >> Info.m_dwRegisterUserIndex;
			rkPacket >> Info.m_dwTradeIndex;
			rkPacket >> Info.m_dwItemType;
			rkPacket >> Info.m_dwItemMagicCode;
			rkPacket >> Info.m_dwItemType;
			rkPacket >> Info.m_dwItemMaleCustom;
			rkPacket >> Info.m_dwItemFemaleCustom;
			rkPacket >> Info.m_iItemPrice;
			rkPacket >> Info.m_dwRegisterStartDate;
			rkPacket >> Info.m_dwRegisterEndDate;
			rkPacket >> Info.m_dwPeriodTime;
			rkPacket >> Info.m_szRegisterUserNick;
			g_TradeInfoMgr.AddTradeItem( Info );

			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
				if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )				
					pLobbyMainWnd->OnRefreshTradeItemList();

				TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
				if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )
					pTrainingFuncWnd->RefreshTradeItemList();
			}
			else
			{
				QuickLobbyWnd *pQuick = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
				if( pQuick && pQuick->IsShow() )
					pQuick->RefreshTradeItemList();				
			}
			
			TradeItemRegisterResultWnd *pResultWnd = dynamic_cast<TradeItemRegisterResultWnd*>(g_GUIMgr.FindWnd(TRADE_REGISTER_RESULT_WND));
			if( pResultWnd )
			{
				pResultWnd->SetRegisterResultInfo(	Info.m_dwItemType,
													Info.m_dwItemMagicCode,
													Info.m_dwItemType,
													Info.m_dwItemMaleCustom,
													Info.m_dwItemFemaleCustom,
													Info.m_iItemPrice );
			}
		}
		break;
	case TRADE_CREATE_FAIL:
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];

			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );	
			kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(1) );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );	
			kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(2) );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}
		break;
	case TRADE_CREATE_DEL:
		{
			int iSlotIndex;
			__int64 iPeso;
			rkPacket >> iSlotIndex >> iPeso;

			g_MyInfo.SetMoney(iPeso);

			ioUserExtraItem *pItem = g_MyInfo.GetUserExtraItem();
			if( pItem )
			{
				if( pItem->DeleteExtraItemData( iSlotIndex ) )
				{
					MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
					if( pInvenWnd && pInvenWnd->IsShow() )
					{
						pInvenWnd->UpdateTab( false, true );
					}
				}
			}
		}
		break;
	case TRADE_CREATE_NO_ITEM:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
		}
		break;
	case TRADE_CREATE_DISABLE:
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];

			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );	
			kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(4) );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );	
			kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(5) );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}
		break;
	case TRADE_CREATE_PESO:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );
		}
		break;
	case TRADE_CREATE_LEVEL:
		{
			char szName[MAX_PATH] = "";
			g_LevelMgr.GetGradeName( g_TradeInfoMgr.GetRegisterLimitLv(), szName, sizeof( szName ), false );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7), szName );
		}
		break;
	case TRADE_CREATE_ERROR:
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];

			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );	
			kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(8) );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );	
			kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(9) );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}
		break;
	}
}

void ioPlayStage::OnTradeItemBuy( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case TRADE_BUY_OK:
		{
			DWORD dwTradeIndex, dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom;
			__int64 iItemPrice, iPeso;
			rkPacket >> dwTradeIndex >> dwItemType >> dwItemMagicCode >> dwItemValue >> dwItemMaleCustom >> dwItemFemaleCustom;
			rkPacket >> iItemPrice;
			rkPacket >> iPeso;
			g_TradeInfoMgr.RemoveTradeItem( dwTradeIndex );
			g_MyInfo.SetMoney( iPeso );

			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
				if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )				
					pLobbyMainWnd->OnRefreshTradeItemList();

				TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
				if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )
					pTrainingFuncWnd->RefreshTradeItemList();
			}
			else
			{
				QuickLobbyWnd *pQuick = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
				if( pQuick && pQuick->IsShow() )
					pQuick->RefreshTradeItemList();				
			}

			TradeItemBuyResultWnd *pResultWnd = dynamic_cast<TradeItemBuyResultWnd*>(g_GUIMgr.FindWnd(TRADE_ITEM_BUY_RESULT_WND));
			if( pResultWnd )
			{
				pResultWnd->SetBuyResultInfo( dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom, iItemPrice );
			}

#if defined( USE_GA )
			// PESO_MARKET_BUY
			g_HttpMng.GA_ItemHitTracking( g_MyInfo.GetUserIndex() 
				, "Gear"
				, iItemPrice
				, 1
				, dwItemMagicCode
				, "Peso"
				, "%2FPESO%2FMARKET%2FBUY" );
#endif
		}
		break;
	case TRADE_SELL_OK:
		{
			DWORD dwTradeIndex, dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom;
			__int64 iItemPrice;
			rkPacket >> dwTradeIndex >> dwItemType >> dwItemMagicCode >> dwItemValue >> dwItemMaleCustom >> dwItemFemaleCustom;
			rkPacket >> iItemPrice;

			g_TradeInfoMgr.RemoveTradeItem( dwTradeIndex );
			const ioItem *pItem = g_ItemMaker.GetItemConst( dwItemMagicCode, __FUNCTION__ );
			if( pItem )
			{
				ioComplexStringPrinter kPrinter;
				g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

				kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1) );

				kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2), pItem->GetName().c_str(), dwItemValue );

				kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
				kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3) );

				g_ChatMgr.SetChatComplexString( STR(4), kPrinter );
			}

			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
				if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )				
					pLobbyMainWnd->OnRefreshTradeItemList();

				TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
				if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )
					pTrainingFuncWnd->RefreshTradeItemList();
			}
			else
			{
				QuickLobbyWnd *pQuick = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
				if( pQuick && pQuick->IsShow() )
					pQuick->RefreshTradeItemList();
			}

#if defined( USE_GA )
			// PESO_MARKET_SELL
			g_HttpMng.GA_ItemHitTracking( g_MyInfo.GetUserIndex() 
				, "Gear"
				, iItemPrice
				, 1
				, dwItemMagicCode
				, "Peso"
				, "%2FPESO%2FMARKET%2FSELL" );
#endif
		}
		break;
	case TRADE_BUY_PESO:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
		}
		break;
	case TRADE_BUY_NO_ITEM:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(6) );

			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
				if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )				
					pLobbyMainWnd->OnRefreshTradeItemList();

				TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
				if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )
					pTrainingFuncWnd->RefreshTradeItemList();
			}
			else
			{
				QuickLobbyWnd *pQuick = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
				if( pQuick && pQuick->IsShow() )
					pQuick->RefreshTradeItemList();
			}
		}
		break;
	case TRADE_BUY_LEVEL:
		{
			char szName[MAX_PATH] = "";
			g_LevelMgr.GetGradeName( g_TradeInfoMgr.GetItemBuyLimitLv(), szName, sizeof( szName ), false );
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7), szName );
		}
		break;
	case TRADE_BUY_ERROR:
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];

			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );	
			kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(8) );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );	
			kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(9) );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}
		break;
	case TRADE_BUY_OWNER:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(10), g_MyInfo.GetPublicID().c_str() );
		}
		break;
	}
}

void ioPlayStage::OnTradeItemCancel( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iType;
	rkPacket >> iType;

	g_GUIMgr.HideWnd(TRADE_ITEM_CANCEL_WND);

	switch( iType )
	{
	case TRADE_CANCEL_COMPLETE:
		{
			DWORD dwTradeIndex, dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom;;
			__int64 iItemPrice;
			rkPacket >> dwTradeIndex >> dwItemType >> dwItemMagicCode >> dwItemValue >> dwItemMaleCustom >> dwItemFemaleCustom;
			rkPacket >> iItemPrice;

			g_TradeInfoMgr.RemoveTradeItem( dwTradeIndex );

			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
				if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )				
					pLobbyMainWnd->OnRefreshTradeItemList();

				TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
				if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )
					pTrainingFuncWnd->RefreshTradeItemList();
			}
			else
			{
				QuickLobbyWnd *pQuick = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
				if( pQuick && pQuick->IsShow() )
					pQuick->RefreshTradeItemList();
			}
			
			TradeItemCancelResultWnd *pResultWnd = dynamic_cast<TradeItemCancelResultWnd*>(g_GUIMgr.FindWnd(TRADE_ITEM_CANCEL_RESULT_WND));
			if( pResultWnd )
			{
				pResultWnd->SetCancelResultInfo( dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom, iItemPrice );
			}
		}
		break;
	case TRADE_CANCEL_NOT_OWNER:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
		}
		break;
	case TRADE_CANCEL_NO_ITEM:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );

			if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
			{
				LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
				if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )				
					pLobbyMainWnd->OnRefreshTradeItemList();

				TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
				if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )
					pTrainingFuncWnd->RefreshTradeItemList();
			}
			else
			{
				QuickLobbyWnd *pQuick = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
				if( pQuick && pQuick->IsShow() )
					pQuick->RefreshTradeItemList();
			}
		}
		break;
	case TRADE_CANCEL_ERROR:
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];

			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );	
			kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(3) );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );	
			kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(4) );

			g_GUIMgr.SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
		}
		break;
	}
}

void ioPlayStage::OnTradeTimeOut( SP2Packet &rkPacket )
{
	DWORD dwTradeIndex, dwItemType, dwItemMagicCode, dwItemValue, dwItemMaleCustom, dwItemFemaleCustom;
	__int64 iItemPrice;

	rkPacket >> dwTradeIndex >> dwItemType >> dwItemMagicCode >> dwItemValue >> dwItemMaleCustom >> dwItemFemaleCustom;
	rkPacket >> iItemPrice;
	g_TradeInfoMgr.RemoveTradeItem( dwTradeIndex );

	const ioItem *pItem = g_ItemMaker.GetItemConst( dwItemMagicCode, __FUNCTION__ );
	if( pItem )
	{
		ioComplexStringPrinter kPrinter;
		g_ChatMgr.PresetChatComplexString( MSG_ANNOUNCE_CHAT, kPrinter );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(1) );

		kPrinter.SetTextColor( TCT_DEFAULT_VIOLET );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(2), pItem->GetName().c_str(), dwItemValue );

		kPrinter.SetTextColor( g_ChatMgr.GetChatColor() );
		kPrinter.AddTextPiece( g_ChatMgr.GetChatFontScale(), STR(3) );

		g_ChatMgr.SetChatComplexString( STR(4), kPrinter );
	}
	
	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{
		LobbyMainWnd *pLobbyMainWnd = dynamic_cast<LobbyMainWnd*>( g_GUIMgr.FindWnd( LOBBY_MAIN_WND ) );
		if( pLobbyMainWnd && pLobbyMainWnd->IsShow() )				
			pLobbyMainWnd->OnRefreshTradeItemList();

		TrainingFuncWnd *pTrainingFuncWnd = dynamic_cast<TrainingFuncWnd*>( g_GUIMgr.FindWnd( TRAINING_FUNC_WND ) );
		if( pTrainingFuncWnd && pTrainingFuncWnd->IsShow() )
			pTrainingFuncWnd->RefreshTradeItemList();
	}
	else
	{
		QuickLobbyWnd *pQuick = dynamic_cast<QuickLobbyWnd*>( g_GUIMgr.FindWnd( LOBBY_QUICK_WND ) );
		if( pQuick && pQuick->IsShow() )
			pQuick->RefreshTradeItemList();
	}
}

D3DXVECTOR3 ioPlayStage::UpdateNewTPSLookAt( ioBaseChar *pTarget, const D3DXVECTOR3 &vPreLookAt )
{
	bool bLookAtMoving = true;
	D3DXVECTOR3 vNewLookAt;

	float fRate = min( m_fTPSLookAtRate, FLOAT1 );
	D3DXVECTOR3 vPos = pTarget->GetMidPositionByRate( fRate );

	if( m_fTPSLookAtOffSet != 0.0f )
		vPos.y += m_fTPSLookAtOffSet;

	if( m_fTPSLookAtOffSetFr != 0.0f )
	{
		D3DXQUATERNION qtRot = pTarget->GetWorldOrientation();
		D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
		D3DXVec3Normalize( &vDir, &vDir );

		vDir = m_fTPSLookAtOffSetFr * vDir;
		vPos += vDir;
	}

	return vPos;

	if( m_dwViewMovingTime > 0 && m_dwCamSlerpTime > 0 )
	{
		DWORD dwGap = FRAMEGETTIME() - m_dwViewMovingTime;
		float fRate = (float)dwGap / (float)m_dwCamSlerpTime;
		fRate = max( 0.0f, min( fRate, FLOAT1 ) );
		if( fRate < FLOAT1 )
		{
			fRate = ( cosf( D3DX_PI * fRate ) + FLOAT1 ) * FLOAT05;
			fRate = FLOAT1 - fRate;

			D3DXVec3Lerp( &vNewLookAt, &m_vLerpStart, &vPos, fRate );
		}
		else
		{
			vNewLookAt = vPos;
			m_dwViewMovingTime = 0;
			bLookAtMoving = false;
		}
	}
	else
	{
		vNewLookAt = vPos;
		bLookAtMoving = false;
	}

	if( m_bNoneCameraWorking || m_bSetCameraToNowOwner || m_bArcadeCameraWorking )
		return vNewLookAt;

	if( pTarget->GetState() == CS_DROP_ZONE_DOWN || 
		pTarget->GetState() == CS_FLY ||
		pTarget->GetState() == CS_WARP_STATE )
	{
		m_bSpringDampUse = false;
	}

	if( !bLookAtMoving && m_bSpringDampUse )
	{
		D3DXVECTOR3 vCurPos = vPreLookAt;
		D3DXVECTOR3 vTarPos = vPos;
		D3DXVECTOR3 vPreTarPos = pTarget->GetPrePosition();
		D3DXVECTOR3 vDir = pTarget->GetTargetRot() * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );

		float fCurMaxSpeed = pTarget->GetMaxSpeed( false );
		float fCurSpeed = pTarget->GetCurSpeed();
		fCurSpeed = min( fCurSpeed, fCurMaxSpeed );
		if( fCurMaxSpeed > 0.0f )
		{
			float fSpeedRate = fCurSpeed / fCurMaxSpeed;

			if( fCurMaxSpeed >= m_fSpringXOffset )
				vDir.x *= m_fSpringXOffset * fSpeedRate;
			else
				vDir.x *= fCurMaxSpeed * fSpeedRate;

			if( fCurMaxSpeed >= m_fSpringZOffset )
				vDir.z *= m_fSpringZOffset * fSpeedRate;
			else
				vDir.z *= m_fSpringZOffset * fSpeedRate;
		}
		else
		{
			vDir.x = vDir.z = 0.0f;
		}

		vCurPos.y -= m_fCamLookAtHeightOffset;
		vTarPos += vDir;
		vPreTarPos += vDir;

		vNewLookAt = SpringDamp( m_fSpringConst, m_fDampConst, vCurPos, vTarPos, vPreTarPos );
	}

	m_vCamMouseFixedTargetLookAt = vPos;

	return vNewLookAt;
}

void ioPlayStage::UpdateTPSCameraPos( ioCamera *pCamera, ioBaseChar *pTarget, D3DXVECTOR3 vNewLookAt )
{
	if( !pCamera || !pTarget )
		return;

	ioLookAtCameraController *pCtrl = ToLookAtCtrl( pCamera->GetController() );
	if( !pCtrl )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::UpdateCtrlPos - Not LookAt Controller" );
		return;
	}

	D3DXVECTOR3 vCamLookAt;
	vCamLookAt.x = vNewLookAt.x;
	vCamLookAt.y = vNewLookAt.y;
	vCamLookAt.z = vNewLookAt.z;	

	m_vCurLookAt = vCamLookAt;

	pCtrl->SetLookAt( vCamLookAt );

	if( (m_fCurCamHeight != pTarget->GetCamHeight() ) ||
		(m_fCurCamDistance != pTarget->GetCamDistance() ) ||
		(m_fCurCamFov != pTarget->GetCamFov() ) )
	{
		m_fStartHeight = m_fCurCamHeight;
		m_fStartDistance = m_fCurCamDistance;
		m_fStartFov = m_fCurCamFov;

		m_fCurCamHeight = pTarget->GetCamHeight();
		m_fCurCamDistance = pTarget->GetCamDistance();
		m_fCurCamFov = pTarget->GetCamFov();
	}

	float fCamDistance, fCamHeight, fCamFov;

	fCamHeight = m_fTPSHeight;
	fCamDistance = m_fTPSDistance;
	fCamFov = m_fTPSFov;

	D3DXVECTOR3 vNewCamPos = vCamLookAt;
	vNewCamPos.y += fCamHeight;

	D3DXQUATERNION qtRot = pTarget->GetWorldOrientation();
	D3DXVECTOR3 vDir = qtRot * D3DXVECTOR3( 0.0f, 0.0f, -FLOAT1 );
	vDir.y = 0.0f;

	D3DXVec3Normalize( &vDir, &vDir );

	vNewCamPos = vNewCamPos - (fCamDistance * vDir);

	if( m_fTPSOffSet != 0.0f )
	{
		D3DXVECTOR3 vLookDir = vCamLookAt - vNewCamPos;
		D3DXVECTOR3 vAxis;
		D3DXVec3Cross( &vAxis, &vLookDir, &ioMath::UNIT_Y );
		D3DXVec3Normalize( &vAxis, &-vAxis );

		vNewCamPos = vNewCamPos + (m_fTPSOffSet * vAxis);
	}

	pCamera->SetFov( fCamFov );
	pCamera->SetCameraRoll( 0.0f );
	pCamera->SetPosition( vNewCamPos );
}

void ioPlayStage::OnTraceWeaponWait( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnTraceWeaponWait - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	int iWeaponIdx;
	rkPacket >> iWeaponIdx;

	D3DXVECTOR3 vPos;
	rkPacket >> vPos;

	ioWeapon* pWeapon = g_WeaponMgr.FindWeapon( pAttacker, iWeaponIdx );

	if( !pWeapon )
		return;

	switch( pWeapon->GetType() )
	{
	case ioWeapon::WT_TRACE:
		{
			ioTraceWeapon *pTrace = ToTraceWeapon( pWeapon );
			if( pTrace )
			{
				pTrace->SetPosition( vPos );
				pTrace->SetExplosionWaitState( this, false );
			}
		}
		break;
	case ioWeapon::WT_TRACE3:
		{
			ioTraceWeaponType3 *pTrace = ToTraceWeaponType3( pWeapon );
			if( pTrace )
			{
				pTrace->SetPosition( vPos );
				pTrace->SetExplosionWaitState( this, false );
			}
		}
		break;
	case ioWeapon::WT_TRACE5:
		{
			ioTraceWeapon5 *pTrace = ToTraceWeapon5( pWeapon );
			if( pTrace )
			{
				pTrace->SetPosition( vPos );
				pTrace->SetExplosionState( this, false );
			}
		}
		break;
	case ioWeapon::WT_GROUND_TRACE:
		{
			ioGroundTraceWeapon *pTrace = ToGroundTraceWeapon( pWeapon );
			if( pTrace )
			{
				pTrace->SetPosition( vPos );
				pTrace->SetExplosionWaitState( this, false );
			}
		}
		break;
	case ioWeapon::WT_GROUND_TRACE2:
		{
			ioGroundTraceWeapon2 *pTrace = ToGroundTraceWeapon2( pWeapon );
			if( pTrace )
			{
				bool bWeaponDead;
				rkPacket >> bWeaponDead;
				pTrace->SetPosition( vPos );
				pTrace->SetExplosionWaitState( this, bWeaponDead, false );
			}
		}
		break;
	}
}

void ioPlayStage::OnAreaWeaponInfo( SP2Packet &rkPacket )
{
	int iSyncType;
	rkPacket >> iSyncType;

	DWORD dwMapIndex;
	rkPacket >> dwMapIndex;
	DWORD dwWeaponIndex;
	rkPacket >> dwWeaponIndex;

	ioAreaWeapon *pAreaWeapon = NULL;
	pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByMapIndex( dwMapIndex );
	if( !pAreaWeapon )
	{
		pAreaWeapon = g_AreaWeaponMgr.FindAreaWeaponByIndex( dwWeaponIndex );
		if ( !pAreaWeapon )
			return;
	}

	switch( iSyncType )
	{
	case AREAWEAPON_MOVE_INFO:
		{	
			pAreaWeapon->ApplyMoveInfo( rkPacket, this );			
		}
		break;
	case AREAWEAPON_SUB_INFO:
		{
			pAreaWeapon->ApplySubInfo( rkPacket, this );
		}
		break;
	}
}

void ioPlayStage::OnCurseRolling( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyCurseRolling( rkPacket );
}

void ioPlayStage::OnDownRolling( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyDownRollingState( rkPacket );
}

void ioPlayStage::OnBuffAirAttack( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;

	pChar->ApplyBuffAirAttackState( rkPacket );
}

ioMachineStruct* ioPlayStage::CreateMachineStruct( int iININum,
												   int iArrayNum,
												   const D3DXVECTOR3 &vTargetPos,
												   const D3DXVECTOR3 &vScale,
												   const D3DXQUATERNION &qtTargetRot,
												   DWORD dwCreateGapTime,
												   DWORD dwSeed,
												   bool bAddList )
{
	ioINILoader kLoader = g_MachineStructMgr.GetINILoader();

	char szTitle[MAX_PATH];
	wsprintf_e( szTitle, "machine_struct%d", iININum );
	kLoader.SetTitle( szTitle );

	ioMachineStruct *pMachine = g_MachineStructMgr.CreateMachineStruct( iININum, m_pPlayMode );
	if( !pMachine )	return NULL;

	pMachine->LoadProperty( kLoader );
	pMachine->SetShadowCastEnable( true );

	D3DXVECTOR3 vPos = vTargetPos;
	if( vPos.y < FLOAT1 )
	{
		vPos.y = GetMapHeight( vPos.x, vPos.z, pMachine, false );
	}
	
	pMachine->InitMachineStructInfo( iININum, iArrayNum, dwCreateGapTime, dwSeed, vPos, vScale );
	pMachine->SetWorldOrientation( qtTargetRot );

	if( bAddList )
	{
		AddGameEntity( pMachine );
	}
	else
	{
		m_ReserveAddEntityList.push_back( pMachine );
	}

	return pMachine;
}

void ioPlayStage::OnMachineStruct( SP2Packet &rkPacket )
{
	if( !m_pMachineStructMgr )
		return;

	int iSubType;

	rkPacket >> iSubType;

	if( iSubType == MACHINE_INFO )
	{
		ioINILoader kStructLoader = g_MachineStructMgr.GetINILoader();
		char szTitle[MAX_PATH] = "";

		int iStructCnt;
		rkPacket >> iStructCnt;

		for( int i=0; i<iStructCnt; i++ )
		{
			int iNum, iIndex;
			D3DXVECTOR3 vPos;
			float fAngle;

			rkPacket >> iNum >> iIndex;
			rkPacket >> vPos;
			rkPacket >> fAngle;

			DWORD dwSeed = timeGetTime();

			wsprintf_e( szTitle, "machine_struct%d", iNum );
			kStructLoader.SetTitle( szTitle );

			D3DXVECTOR3 vScale;
			vScale.x = kStructLoader.LoadFloat_e( "scale_x", FLOAT1 );
			vScale.y = kStructLoader.LoadFloat_e( "scale_y", FLOAT1 );
			vScale.z = kStructLoader.LoadFloat_e( "scale_z", FLOAT1 );

			D3DXQUATERNION qtRot;
			fAngle = ioMath::ArrangeHead( fAngle );
			D3DXQuaternionRotationAxis( &qtRot, &ioMath::UNIT_Y, DEGtoRAD(fAngle) );

			CreateMachineStruct( iNum, iIndex, vPos, vScale, qtRot, 0, dwSeed );
		}
	}
	else
	{
		ioHashString szName;
		rkPacket >> szName;

		ioBaseChar *pChar = GetBaseChar( szName );
		if( !pChar )
		{
			return;
		}


		switch( iSubType )
		{
		case MACHINE_TAKE_OK:
			{
				if( pChar->GetState() == CS_MACHINE_TAKE_WAIT )
				{
					pChar->ApplyMachineTake( rkPacket );
				}
			}
			break;
		case MACHINE_TAKE_FAIL:
			{
				if( pChar->GetState() == CS_MACHINE_TAKE_WAIT )
					pChar->SetState( CS_DELAY );
			}
			break;
		case MACHINE_RELEASE_OK:
			{
				if( pChar->GetState() == CS_MACHINE_TAKE )
					pChar->ApplyMachineRelease( rkPacket );
			}
			break;
		case MACHINE_RELEASE_FAIL:
			break;
		case MACHINE_DIE:
			{
				int iIndex;
				bool bEffect;
				rkPacket >> iIndex;
				rkPacket >> bEffect;

				ioMachineStruct *pMachine = g_MachineStructMgr.FindMachineStruct( iIndex );
				if( pMachine )
				{
					pMachine->StructDie( bEffect );
				}
			}
			break;
		}
	}
}

void ioPlayStage::OnPushStructBomber( SP2Packet &rkPacket )
{
	int iIndex;
	rkPacket >> iIndex;

	// HARDCODE :: TestCode Check
	int iNum, iTestValue;
	rkPacket >> iNum >> iTestValue;

	ioPushStruct *pPushStruct = g_PushStructListMgr.FindPushStruct( iIndex );
	if( !pPushStruct )
	{
		if( Help::CheckEnableTimeGapUser() )
		{
			LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPushStructBomber - %d : %d, %d", iIndex, iNum, iTestValue );
		}
		return;
	}

	if( pPushStruct->GetState() == ioPushStruct::PS_DIE )
		return;

	pPushStruct->OnPushStructBomber( rkPacket );
}

void ioPlayStage::OnWeaponByWeaponExplicit( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyWeaponByWeaponExplicit( rkPacket );
}

void ioPlayStage::OnWeaponByWeaponExplicitList( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyWeaponByWeaponExplicitList( rkPacket );
}

void ioPlayStage::OnCollectJumpAttack( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;

	int iJumpType;
	rkPacket >> iJumpType;
	pChar->SetCurJumpType( (JumpType)iJumpType );

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;
	pChar->SetWeaponIndexBase( dwWeaponIdx );

	float fCurHP;
	rkPacket >> fCurHP;
	pChar->SetCurHP( fCurHP );

	D3DXQUATERNION qtRotate;
	rkPacket >> qtRotate;

	D3DXVECTOR3 vMoveDir, vInputDir;
	rkPacket >> vMoveDir >> vInputDir;

	pChar->SetJumpAttckTargetRot( qtRotate, vMoveDir, vInputDir );

	int iType;
	rkPacket >> iType;
	pChar->SetCollectJumpAttack( iType );
	pChar->ApplyAutoTargetInfo( rkPacket );
}

void ioPlayStage::OnNewJumpAttack( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;

	int iJumpType;
	rkPacket >> iJumpType;
	pChar->SetCurJumpType( (JumpType)iJumpType );

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;
	pChar->SetWeaponIndexBase( dwWeaponIdx );

	float fCurHP;
	rkPacket >> fCurHP;
	pChar->SetCurHP( fCurHP );

	D3DXQUATERNION qtRotate;
	rkPacket >> qtRotate;

	bool bExtendAttack;
	rkPacket >> bExtendAttack;

	pChar->SetTargetRotAndMoveDirChange( qtRotate, false );
	pChar->SetInputDirection( pChar->GetMoveDir() );

	pChar->SetNewJumpAttackState( bExtendAttack );
	pChar->ApplyAutoTargetInfo( rkPacket );
}

void ioPlayStage::OnThrowBombBound( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnThrowBombBound - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	int iWeaponIdx;
	rkPacket >> iWeaponIdx;

	D3DXVECTOR3  vPos, vDir;
	rkPacket >> vPos;
	rkPacket >> vDir;

	ioThrowBombWeapon *pBomb = ToThrowBombWeapon( g_WeaponMgr.FindWeapon( pAttacker, iWeaponIdx ) );
	if( pBomb )
	{
		pBomb->SetPosition( vPos );
		pBomb->SetWallCollision( vDir );
	}

	ioThrowBomb2Weapon *pBomb2 = ToThrowBomb2Weapon( g_WeaponMgr.FindWeapon( pAttacker, iWeaponIdx ) );
	if( pBomb2 )
	{
		pBomb2->SetPosition( vPos );
		pBomb2->SetWallCollision( vDir );
	}
}

void ioPlayStage::OnSmartBombBound( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnSmartBombBound - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	int iWeaponIdx;
	rkPacket >> iWeaponIdx;

	D3DXVECTOR3  vPos, vDir;
	rkPacket >> vPos;
	rkPacket >> vDir;

	ioSmartBombWeapon *pSmart = ToSmartBombWeapon( g_WeaponMgr.FindWeapon( pAttacker, iWeaponIdx ) );
	if( pSmart )
	{
		pSmart->SetPosition( vPos );
		pSmart->SetWallCollision( vDir );
	}
}

void ioPlayStage::DestroyAreaWeapon( DWORD dwIndex )
{
	if( m_pAreaWeaponMgr )
	{
		m_pAreaWeaponMgr->DestroyAreaWeapon( dwIndex );
	}
}

void ioPlayStage::OnDefenseAttack( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;

	D3DXQUATERNION qtCharRot;
	rkPacket >> qtCharRot;

	int iWeaponIndex;
	rkPacket >> iWeaponIndex;

	pChar->SetWeaponIndexBase( iWeaponIndex );
	pChar->SetTargetRotAndMoveDirChange( qtCharRot );
	pChar->ApplyDefenseAttackState( rkPacket );

	// weaponindex 맞춰주기
	pChar->IncreaseWeaponIndexBase();
}

void ioPlayStage::OnCreateStructState( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyCreateStructState( rkPacket );
}

float ioPlayStage::GetMapHeightForBlockGuid( const D3DXVECTOR3 &vCurPos, const ioOrientBox &rkBox )
{
	ioAxisAlignBox kAxisBox;
	D3DXVECTOR3 vMin, vMax;

	D3DXVECTOR3 vCenter = vCurPos + rkBox.GetCenter();
	ioOrientBox kOrientBox  = rkBox;
	kOrientBox.SetCenter( vCenter );

	D3DXVECTOR3 vVtxArray[8];
	kOrientBox.GetVertices( vVtxArray );

	vMin = vVtxArray[0];
	vMax = vVtxArray[6];

	vMin.y = 0.0f;
	vMax.y = GetBoundHeight();

	kOrientBox.SetBoxMinMaxForAxis( vMin, vMax );

	float fBoxMaxY = vMax.y;
	float fCurYPos = -FLOAT1;

	MountAvailableList::iterator iter = m_MountAvailableList.begin();
	for( ; iter!=m_MountAvailableList.end() ; ++iter )
	{
		const ioGameEntity *pCurMount = *iter;
		if( !pCurMount->IsNowMountEnable() )
			continue;

		ioOrientBox kColBox = pCurMount->GetWorldCollisionBox();
		kColBox.SetCenter( pCurMount->GetWorldAxisBox().GetCenter() );

		D3DXVECTOR3 vVtxArray[8];
		kColBox.GetVertices( vVtxArray );

		vMin = vVtxArray[0];
		vMax = vVtxArray[6];

		if( ioMath::TestIntersection( kOrientBox, kColBox, NULL ) )
		{
			float fYHeight = kColBox.GetCenter().y + kColBox.GetExtents(1);
			if( fYHeight < fBoxMaxY )
			{
				fCurYPos = max( fCurYPos, fYHeight + FLOAT1 );
			}
		}
	}

	float fMapHeight = m_pSceneMgr->GetHeight( vCurPos.x, fBoxMaxY, vCurPos.z );

	if( m_pBlockWorld )
	{
		float fHeight = m_pBlockWorld->GetHeight( vCurPos.x, fBoxMaxY, vCurPos.z );
		if( fMapHeight < fHeight )
		{
			if( fCurYPos > -FLOAT1 && fHeight < fCurYPos )
				return fCurYPos;

			return fHeight;
		}
	}

	if( fCurYPos > -FLOAT1 && fMapHeight < fCurYPos )
		return fCurYPos;

	return fMapHeight;
}

void ioPlayStage::OnWeaponSync( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnWeaponSync - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;

	ioWeapon *pWeapon = g_WeaponMgr.FindWeapon( pAttacker, dwWeaponIdx );
	if( pWeapon )
	{
		pWeapon->ApplyWeaponSync( rkPacket, this );
	}
	else
	{
		if( m_LossWeaponWoundEventList.size() <= MAX_LOSS_WEAPON_WOUND_EVENT_COUNT )
		{
			WeaponWoundEvent loss_event;
			loss_event.szAttacker = szAttacker;
			loss_event.dwWeaponIdx = dwWeaponIdx;
			loss_event.dwLifeStartTime = FRAMEGETTIME();
			loss_event.dwLifeEndTime = FRAMEGETTIME() + 1000;
			loss_event.packet = rkPacket;

			m_LossWeaponWoundEventList.push_back( loss_event );
		}
	}
}

void ioPlayStage::OnExtendDash( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyExtendDash( rkPacket );
}

void ioPlayStage::OnGrapplingSync( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
		return;

	if( pChar->IsSystemState() )
		return;

	int iType;
	rkPacket >> iType;

	switch( iType )
	{
	case GRAPPLING_ATTACKER:
		pChar->ApplyAttackGrapplingState( rkPacket );
		break;
	case GRAPPLING_WOUNDED:
		pChar->ApplyWoundGrapplingState( rkPacket );
		break;
	case GRAPPLING_PUSHED:
		pChar->ApplyPushedGrapplingState( rkPacket );
		break;
	case GRAPPLING_WOUNDED_SKILL:
		pChar->ApplySkillGrapplingState( rkPacket );
		break;
	case GRAPPLING_WOUNDED_SWING:
		pChar->ApplyGrapplingWoundSwingState( rkPacket );
		break;
	case GRAPPLING_PSYC:
		pChar->ApplyWoundPsycGrappling( rkPacket );
		break;
	case GRAPPLING_PSYC_ACTION:
		pChar->ApplyWoundPsycGrapplingAction( rkPacket );
		break;
	case GRAPPLING_JUMP_ATTACK:
		pChar->ApplyJumpGrapplingState( rkPacket );
		break;
	case GRAPPLING_JUMP_WOUND:
		pChar->ApplyJumpGrapplingWoundState( rkPacket );
		break;
	case GRAPPLING_PUSHED_BUFF:
		pChar->ApplyPushedBuffGrapplingState( rkPacket );
		break;
	}
}

void ioPlayStage::OnDummyCharSync( SP2Packet &rkPacket )
{
	ioHashString szCreator;
	int iIndex;
	rkPacket >> szCreator >> iIndex;

	ioBaseChar *pCreator = GetBaseChar( szCreator );
	if( !pCreator )
		return;

	ioDummyChar *pDummy = g_DummyCharMgr.FindDummyChar( pCreator, iIndex );
	if( pDummy )
	{
		pDummy->ApplyDummyCharSync( this, rkPacket );

#ifndef SHIPPING
		//패킷 로그 확인 (더미)
		if ( g_MyInfo.GetPublicID() == "jch2305" )
		{
			g_RecvPacketLogManager.AddCharRecvInfo( szCreator, ioRecvPacketLogManager::RPT_Dummy, pDummy->GetDummyCharName() );
		}
#endif
	}	
}

void ioPlayStage::DestroyDieDummyChar()
{
	g_DummyCharMgr.ClearDeadDummyChar();

	GameEntityList::iterator iter = m_GameEntityList.begin();
	while ( iter != m_GameEntityList.end() )
	{
		ioDummyChar *pDummy = ToDummyChar( *iter );
		if( pDummy && pDummy->GetState() == DCS_DIE )
		{

#ifndef SHIPPING
			if ( g_MyInfo.GetPublicID() == "jch2305" )
			{
				g_RecvPacketLogManager.PrintCharRecvInfo( 0xFFFFFF00, pDummy->GetOwnerName(), ioRecvPacketLogManager::RPT_Dummy, pDummy->GetDummyCharName(), true );
			}
#endif
			iter = EraseGameEntity( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioPlayStage::DestroyRouletteEntity()
{
	GameEntityList::iterator iter = m_GameEntityList.begin();
	while( iter != m_GameEntityList.end() )
	{
		ioRoulette *pRoulette = ToRoulette( *iter );
		if( pRoulette && pRoulette->IsReserveDestroy() )
		{
			iter = EraseGameEntity( iter );
		}
		else
		{
			++iter;
		}
	}
}

ioDummyChar* ioPlayStage::CreateDummyChar( const ioHashString &szDummyCharName,
										   int iIndex,
										   const ioHashString &szOwnerName,
										   const D3DXVECTOR3 &vTargetPos,
										   float fStartAngle,
										   DWORD dwCreateGapTime,
										   bool bAddList )
{
	char szTitle[MAX_PATH] = "";

	DummyCharBaseInfo kBaseInfo;
	if( !g_DummyCharMgr.GetDummyCharBaseInfo( szDummyCharName, kBaseInfo ) )
	{
		// 기본정보 찾지 못함
		return NULL;
	}

	ioINILoader kLoader;
	kLoader.SetFileName( kBaseInfo.m_DummyCharPath.c_str() );
	kLoader.SetTitle( kBaseInfo.m_DummyCharTitle.c_str() );

	ioBaseChar *pOwner = GetBaseChar( szOwnerName );

	bool bMale = true;
	char szModelName[MAX_PATH] = "";
	if( pOwner && !pOwner->IsMale() )
	{
		bMale = false;
		wsprintf_e( szModelName, "%s", kBaseInfo.m_DummyCharModelW.c_str() );
	}
	else
	{
		bMale = true;
		wsprintf_e( szModelName, "%s", kBaseInfo.m_DummyCharModel.c_str() );
	}

	ioDummyChar *pDummy = NULL;

	int iType = kBaseInfo.m_iDummyCharType;
	if( iType == DCT_COPY_OWNER || iType == DCT_COPY_OWNER2 || iType == DCT_COPY_OWNER3 || iType == DCT_COPY_OWNER4 || iType == DCT_COPY_OWNER5 )
	{
		pDummy = g_DummyCharMgr.CreateDummyCharByOwnerGroup( pOwner, m_pPlayMode, iType );
	}
	else
	{
		pDummy = g_DummyCharMgr.CreateDummyChar( szModelName, m_pPlayMode, iType );
	}

	if( !pDummy )
	{
		// 생성실패
		return NULL;
	}

	pDummy->InitResource();
	pDummy->LoadProperty( kLoader );
	pDummy->SetShadowCastEnable( true );
	pDummy->SetOwner( szOwnerName );
	pDummy->SetMale( bMale );
	g_DummyCharMgr.SetCurLocalIndex( iIndex );

	ioCopyOwnerDummyChar *pCopyOwnerDummy = ToCopyOwnerDummyChar( pDummy );
	if( pCopyOwnerDummy )
	{
		pCopyOwnerDummy->SetCopyOwnerInfo( pOwner );
		pCopyOwnerDummy->LoadPropertyByChar();
	}

	ioCopyOwnerDummyChar2 *pCopyOwnerDummy2 = ToCopyOwnerDummyChar2( pDummy );
	if( pCopyOwnerDummy2 )
	{
		pCopyOwnerDummy2->SetCopyOwnerInfo( pOwner );
		pCopyOwnerDummy2->LoadPropertyByChar();
	}

	ioCopyOwnerDummyChar3 *pCopyOwnerDummy3 = ToCopyOwnerDummyChar3( pDummy );
	if( pCopyOwnerDummy3 )
	{
		pCopyOwnerDummy3->SetCopyOwnerInfo( pOwner );
		pCopyOwnerDummy3->LoadPropertyByChar();
	}

	ioCopyOwnerDummyChar4 *pCopyOwnerDummy4 = ToCopyOwnerDummyChar4( pDummy );
	if( pCopyOwnerDummy4 )
	{
		pCopyOwnerDummy4->SetCopyOwnerInfo( pOwner );
	}

	ioCopyOwnerDummyChar5 *pCopyOwnerDummy5 = ToCopyOwnerDummyChar5( pDummy );
	if( pCopyOwnerDummy5 )
		pCopyOwnerDummy5->SetCopyOwnerInfo( pOwner );

	ioEntityGroup *pGrp = pDummy->GetGroup();
	if( pGrp )
	{
		ioDummyCharAniEventHandler *pHandler = new ioDummyCharAniEventHandler;
		pHandler->SetCreateChar( pDummy );
		pGrp->SetAniEventHandler( pHandler );
	}

	D3DXVECTOR3 vScale = kBaseInfo.m_vScale;
	D3DXVECTOR3 vPos = vTargetPos;
	if( pOwner )
	{
		if( vPos.y <= 0.0f )
		{
			const ioAxisAlignBox &rkWorldBox = pOwner->GetWorldAxisBox();
			vPos.y = rkWorldBox.GetCenter().y + rkWorldBox.GetExtents( 1 );
		}

		pDummy->SetWorldPosition( vPos );

		switch(pDummy->GetType())
		{
		case DCT_NO_CONTROL:
		case DCT_SUB_CRANE:
		case DCT_OWNER_TRACE2:
			break;
		case DCT_RUSH:
		case DCT_ATTACK_TOWER2:
		case DCT_MOVE_ATTACK:
		case DCT_OWNER_CONTROL:
		case DCT_OWNER_CONTROL3:
		case DCT_OWNER_CONTROL5:
		case DCT_OWNER_CONTROL6:
		case DCT_OWNER_ATTACH4:
		case DCT_OWNER_ATTACH5:
		case DCT_CROW:
		case DCT_MINE:
		case DCT_ATTACH_WEAPON:
		case DCT_MOVE_ATTACK3:
		case DCT_MOVE_ATTACK4:
		case DCT_WARP_MINE:
		case DCT_HELICOPTER:
		case DCT_CRANE:
		case DCT_TARGET_TRACE9:
		case DCT_TARGET_TRACE10:
		case DCT_TARGET_TRACE14:
		case DCT_STRUCT:
		case DCT_CHAIN:
		case DCT_COPY_OWNER5:
		case DCT_TARGET_TRACE17:
		case DCT_SHIELD_DOME:
		case DCT_OWNER_TRACE_WOUND_ANI:
			pDummy->SetWorldOrientation( pOwner->GetTargetRot() );
			break;
		case DCT_OWNER_CONTROL2:
			vPos.y = GetMapHeightNoneEntity( vPos.x, vPos.z );
			pDummy->SetWorldOrientation( pOwner->GetTargetRot() );
			break;
		case DCT_TARGET_TRACE7:
		case DCT_TARGET_TRACE4:
			{
				float fTargetY = vPos.y;
				vPos.y = GetMapHeightByTargetMid( vPos.x, vPos.z, fTargetY, pOwner->GetMidHeightByRate( FLOAT1 ) );
				pDummy->SetWorldOrientation( pOwner->GetTargetRot() );
			}
			break;
		case DCT_TARGET_TRACE5:
			{
				vPos.y = GetMapHeight( vPos.x, vPos.z, pDummy, false );
				pDummy->SetWorldOrientation( pOwner->GetTargetRot() );
			}
			break;
		case DCT_STEEL:
			{
				ioDummySteel *pSteel = ToDummySteel( pDummy );
				if ( !pSteel->IsNoChangeHeight() )
					vPos.y = GetMapHeight( vPos.x, vPos.z, pDummy, true );

				pDummy->SetWorldOrientation( pOwner->GetTargetRot() );
			}
			break;
		default:
			vPos.y = GetMapHeight( vPos.x, vPos.z, pDummy, true );
			pDummy->SetWorldOrientation( pOwner->GetTargetRot() );
			break;
		}

		pDummy->InitDummyCharInfo( iIndex, dwCreateGapTime, vPos, vScale, fStartAngle );

		if( !pDummy->IsEntityCollisionSkipState( NULL ) && !pDummy->IsCharCollisionSkipState( pOwner->GetTeam() ) )
		{
			float fPushAmt;
			bool bCol = false;
			D3DXVECTOR3 vPushDir;
			if( CalcEntityCollisionPushAmt( pDummy, pOwner, vPushDir, fPushAmt ) )
			{
				D3DXVECTOR3 vMoveAmt = -vPushDir * fPushAmt;
				if( ReCalculateMoveVectorByTerrain( pOwner, &vMoveAmt, bCol ) )
				{
					pOwner->Translate( vMoveAmt );
				}
			}
		}
	}
	else
	{
		if( vPos.y < FLOAT1 )
		{
			vPos.y = GetMapHeight( vPos.x, vPos.z, pDummy, false );
		}

		pDummy->InitDummyCharInfo( iIndex, dwCreateGapTime, vPos, vScale, fStartAngle );
	}

	if( bAddList )
	{
		AddGameEntity( pDummy );
	}
	else
	{
		m_ReserveAddEntityList.push_back( pDummy );
	}

	return pDummy;
}

ioDummyChar* ioPlayStage::ApplyCreateDummyChar( const ioHashString &szDummyCharName,
												int iIndex,
												const ioHashString &szOwnerName,
												const D3DXVECTOR3 &vTargetPos,
												float fStartAngle,
												DWORD dwGapTime,
												bool bAddList,  bool bNetWork )
{
	char szTitle[MAX_PATH] = "";

	DummyCharBaseInfo kBaseInfo;
	if( !g_DummyCharMgr.GetDummyCharBaseInfo( szDummyCharName, kBaseInfo ) )
	{
		// 기본정보 찾지 못함
		return NULL;
	}

	ioINILoader kLoader;
	kLoader.SetFileName( kBaseInfo.m_DummyCharPath.c_str() );
	kLoader.SetTitle( kBaseInfo.m_DummyCharTitle.c_str() );

	ioBaseChar *pOwner = GetBaseChar( szOwnerName );

	bool bMale = true;
	char szModelName[MAX_PATH] = "";
	if( pOwner && !pOwner->IsMale() )
	{
		bMale = false;
		wsprintf_e( szModelName, "%s", kBaseInfo.m_DummyCharModelW.c_str() );
	}
	else
	{
		bMale = true;
		wsprintf_e( szModelName, "%s", kBaseInfo.m_DummyCharModel.c_str() );
	}

	ioDummyChar *pDummy = NULL;

	int iType = kBaseInfo.m_iDummyCharType;
	if( iType == DCT_MAMAHAHA )
	{
		if( g_DummyCharMgr.HasMamaHahaDummy( pOwner ) )
			return NULL;

		pDummy = g_DummyCharMgr.CreateDummyChar( szModelName, m_pPlayMode, iType );
	}
	else if( iType == DCT_PAPPY )
	{
		if( g_DummyCharMgr.FindDummyCharToName( pOwner, szDummyCharName ) != NULL )
			return NULL;

		pDummy = g_DummyCharMgr.CreateDummyChar( szModelName, m_pPlayMode, iType );
	}
	else if( iType == DCT_COPY_OWNER || iType == DCT_COPY_OWNER2 || iType == DCT_COPY_OWNER3 || iType == DCT_COPY_OWNER4 || iType == DCT_COPY_OWNER5 )
	{
		pDummy = g_DummyCharMgr.CreateDummyCharByOwnerGroup( pOwner, m_pPlayMode, iType );
	}
	else
	{
		pDummy = g_DummyCharMgr.CreateDummyChar( szModelName, m_pPlayMode, iType );
	}
	if( !pDummy )	return NULL;

	pDummy->InitResource();
	pDummy->LoadProperty( kLoader );
	pDummy->SetShadowCastEnable( true );
	pDummy->SetOwner( szOwnerName );
	pDummy->SetMale( bMale );

	ioCopyOwnerDummyChar *pCopyOwnerDummy = ToCopyOwnerDummyChar( pDummy );
	if( pCopyOwnerDummy )
	{
		pCopyOwnerDummy->SetCopyOwnerInfo( pOwner );
		pCopyOwnerDummy->LoadPropertyByChar();
	}

	ioCopyOwnerDummyChar2 *pCopyOwnerDummy2 = ToCopyOwnerDummyChar2( pDummy );
	if( pCopyOwnerDummy2 )
	{
		pCopyOwnerDummy2->SetCopyOwnerInfo( pOwner );
		pCopyOwnerDummy2->LoadPropertyByChar();
	}

	ioCopyOwnerDummyChar3 *pCopyOwnerDummy3 = ToCopyOwnerDummyChar3( pDummy );
	if( pCopyOwnerDummy3 )
	{
		pCopyOwnerDummy3->SetCopyOwnerInfo( pOwner );
		pCopyOwnerDummy3->LoadPropertyByChar();
	}

	ioCopyOwnerDummyChar4 *pCopyOwnerDummy4 = ToCopyOwnerDummyChar4( pDummy );
	if( pCopyOwnerDummy4 )
	{
		pCopyOwnerDummy4->SetCopyOwnerInfo( pOwner );
	}

	ioEntityGroup *pGrp = pDummy->GetGroup();
	if( pGrp )
	{
		ioDummyCharAniEventHandler *pHandler = new ioDummyCharAniEventHandler;
		pHandler->SetCreateChar( pDummy );
		pGrp->SetAniEventHandler( pHandler );
	}

	D3DXVECTOR3 vScale = kBaseInfo.m_vScale;
	pDummy->InitDummyCharInfo( iIndex, 0, vTargetPos, vScale, fStartAngle, bNetWork );

	if( bAddList )
		AddGameEntity( pDummy );
	else
		m_ReserveAddEntityList.push_back( pDummy );

	return pDummy;
}

bool ioPlayStage::ReCalculateMoveVectorByTerrain( ioDummyChar *pDummy, D3DXVECTOR3 *pMove, OUT bool &bCol )
{
	bCol = false;

	ioOpcodeShape *pShape = m_pSceneMgr->GetCollisionShape();
	if( !pShape )
		return true;

	// 로켓 런처 용병이 사용하는 D~ DCT_OWNER_CONTROL6 타입 더미가 PushStruct 충돌 처리 안함
	if( pDummy && !pDummy->CheckStructCollision() )
		return true;

	ioAxisAlignBox kAxisBox = pDummy->GetWorldAxisBox();

	D3DXVECTOR3 vUpMinPos = kAxisBox.GetMinPos();
	if( !pDummy->IsDisableRecalculateByterrain() )
		vUpMinPos.y += ( kAxisBox.GetMaxPos().y - kAxisBox.GetMinPos().y ) * 0.25f;
	kAxisBox.SetMinPos( vUpMinPos );

	ioAxisAlignBox kAxisMoveBox = kAxisBox;
	kAxisMoveBox.Translate( *pMove );
	kAxisMoveBox.Merge( kAxisBox );

	D3DXVECTOR3 vMoveDir, vCenterDir;
	D3DXVec3Normalize( &vMoveDir, pMove );

	bool bMonsterVsMonsterCollision = true;
	if( m_pPlayMode )
	{
		bMonsterVsMonsterCollision = m_pPlayMode->IsMonsterVsMonsterCollision();
	}

	//탑승 가능한 엔티티 체크
	if( !ioMapCollisionHelper::ReCalculateMoveVectorByMountAvailableEntity( pDummy, 
																			kAxisMoveBox,
																			kAxisBox,
																			bMonsterVsMonsterCollision,
																			m_MountAvailableList,
																			NULL,
																			vMoveDir,
																			pMove,
																			bCol ) )
	{
		return false;
	}

	//엔티티 체크
	if( !ioMapCollisionHelper::ReCalculateMoveVectorByEntity( pDummy,
															  m_GameEntityList,
															  kAxisMoveBox,
															  kAxisBox,
															  vMoveDir,
															  pMove,
															  bCol ) )
	{
		return false;
	}

	bool bForceMove = false;
	if( pDummy->GetForceAmt() > 0.0f )
		bForceMove = true;

	//ioBaseChar 및 NPC 체크
	BaseCharList::const_iterator iChar;
	for( iChar=m_BaseCharList.begin() ; iChar!=m_BaseCharList.end() ; ++iChar )
	{
		ioBaseChar *pOther = *iChar;

		if( !bMonsterVsMonsterCollision )
		{
			if( ToNpcChar( pDummy ) && ToNpcChar( pOther ) )
				continue;
		}

		if( pOther->IsEntityCollisionSkipState( pDummy ) )
			continue;

		if( pDummy->IsCharCollisionSkipState( pOther->GetTeam() ) )
			continue;
				
		if( pDummy->IsDummyCharCollisionSkipState( pOther ) )
			continue;


		if( !ioMapCollisionHelper::IsCollisionCheckRange( pDummy->GetWorldPosition(), pOther->GetWorldPosition() ) ) continue;

		const ioAxisAlignBox &rkMountBox = pOther->GetWorldAxisBox();

		if( !ioMath::TestIntersection( kAxisMoveBox, rkMountBox ) )
			continue;

		vCenterDir = rkMountBox.GetCenter() - kAxisBox.GetCenter();
		vCenterDir.y = 0.0f;

		if( D3DXVec3Dot( &vCenterDir, &vMoveDir ) < 0.0f )	// 후면
			continue;

		if( ioMapCollisionHelper::ModifyMoveDir( kAxisBox, kAxisMoveBox, rkMountBox, bForceMove, pMove ) )
		{
			kAxisMoveBox = kAxisBox;
			kAxisMoveBox.Translate( *pMove );
		}
		else
		{
			return false;
		}
	}
	
	//블럭 메쉬 체크
	if( m_pBlockWorld && !m_pBlockWorld->ReCalculateMoveVectorByBlock( pDummy, kAxisMoveBox, vMoveDir, false, pMove, bCol ) )
		return false;

	ioOrientBox kCharBox;
	kCharBox.SetByAxisBox( kAxisMoveBox );
	return ioMapCollisionHelper::ReCalculateMoveVectorByWorldMesh( pDummy, kCharBox, pShape, NULL, 0.0f, vMoveDir, false, pMove, bCol );
}

bool ioPlayStage::ReCalculateMoveVectorByTerrain( ioDummyChar *pDummy, D3DXVECTOR3 *pMove, OUT bool &bCol, const MountedEntityList& rkMount )
{
	bCol = false;
	ioOpcodeShape *pShape = m_pSceneMgr->GetCollisionShape();
	if( !pShape )
		return true;

	ioAxisAlignBox kAxisBox = pDummy->GetWorldAxisBox();

	D3DXVECTOR3 vUpMinPos = kAxisBox.GetMinPos();
	if( !pDummy->IsDisableRecalculateByterrain() )
		vUpMinPos.y += ( kAxisBox.GetMaxPos().y - kAxisBox.GetMinPos().y ) * 0.25f;
	kAxisBox.SetMinPos( vUpMinPos );

	ioAxisAlignBox kAxisMoveBox = kAxisBox;
	kAxisMoveBox.Translate( *pMove );
	kAxisMoveBox.Merge( kAxisBox );

	D3DXVECTOR3 vMoveDir, vCenterDir;
	D3DXVec3Normalize( &vMoveDir, pMove );

	bool bMonsterVsMonsterCollision = true;
	if( m_pPlayMode )
	{
		bMonsterVsMonsterCollision = m_pPlayMode->IsMonsterVsMonsterCollision();
	}

	//탑승 가능한 엔티티 체크
	if( !ioMapCollisionHelper::ReCalculateMoveVectorByMountAvailableEntity( pDummy, 
																			kAxisMoveBox,
																			kAxisBox,
																			bMonsterVsMonsterCollision,
																			m_MountAvailableList,
																			&rkMount,
																			vMoveDir,
																			pMove,
																			bCol ) )
	{
		return false;
	}

	//엔티티 체크
	if( !ioMapCollisionHelper::ReCalculateMoveVectorByEntity( pDummy,
															  m_GameEntityList,
															  kAxisMoveBox,
															  kAxisBox,
															  vMoveDir,
															  pMove,
															  bCol ) )
	{
		return false;
	}

	bool bForceMove = false;
	if( pDummy->GetForceAmt() > 0.0f )
		bForceMove = true;

	BaseCharList::const_iterator iChar;
	for( iChar=m_BaseCharList.begin() ; iChar!=m_BaseCharList.end() ; ++iChar )
	{
		ioBaseChar *pOther = *iChar;

		if( !bMonsterVsMonsterCollision )
		{
			if( ToNpcChar( pDummy ) && ToNpcChar( pOther ) )
				continue;
		}

		bool bContinute = false;
		MountedEntityList::const_iterator iter = rkMount.begin();
		for( ; iter != rkMount.end(); ++iter )
		{
			ioGameEntity* pMountedEntity = *iter;
			if( pMountedEntity == pOther )
			{
				bContinute = true;
				break;
			}
		}

		if( bContinute )
			continue;

		if( pOther->IsEntityCollisionSkipState( pDummy ) )
			continue;

		if( pDummy->IsDummyCharCollisionSkipState( pOther ) )
			continue;

		if( pDummy->IsCharCollisionSkipState(pOther->GetTeam()) )
			continue;

		if( !ioMapCollisionHelper::IsCollisionCheckRange( pDummy->GetWorldPosition(), pOther->GetWorldPosition() ) )
			continue;

		const ioAxisAlignBox &rkMountBox = pOther->GetWorldAxisBox();

		if( !ioMath::TestIntersection( kAxisMoveBox, rkMountBox ) )
			continue;

		vCenterDir = rkMountBox.GetCenter() - kAxisBox.GetCenter();
		vCenterDir.y = 0.0f;

		if( D3DXVec3Dot( &vCenterDir, &vMoveDir ) < 0.0f )	// 후면
			continue;

		if( ioMapCollisionHelper::ModifyMoveDir( kAxisBox, kAxisMoveBox, rkMountBox, bForceMove, pMove ) )
		{
			kAxisMoveBox = kAxisBox;
			kAxisMoveBox.Translate( *pMove );
		}
		else
		{
			return false;
		}
	}

	//블럭 메쉬 체크
	if( m_pBlockWorld && !m_pBlockWorld->ReCalculateMoveVectorByBlock( pDummy, kAxisMoveBox, vMoveDir, false, pMove, bCol ) )
		return false;

	ioOrientBox kCharBox;
	kCharBox.SetByAxisBox( kAxisMoveBox );
	return ioMapCollisionHelper::ReCalculateMoveVectorByWorldMesh( pDummy, kCharBox, pShape, NULL, 0.0f, vMoveDir, false, pMove, bCol );
}

void ioPlayStage::OnFastChangeChar( SP2Packet &rkPacket )
{
	OnParentChangeChar( rkPacket, true );
}

void ioPlayStage::OnJumpReload( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyJumpReload( rkPacket );
}

void ioPlayStage::OnBlowDash( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyBlowDash( rkPacket );
}

void ioPlayStage::OnWereWolfState( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyWereWolfState( rkPacket );
}

void ioPlayStage::OnGPState( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;

	pChar->ApplyGPState( rkPacket );
}

void ioPlayStage::OnBuffExtraInfo( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;

	ioHashString szBuffName;
	rkPacket >> szBuffName;

	pChar->ApplyExtraBuffInfo( szBuffName, rkPacket );

#ifndef SHIPPING
	//패킷 로그 확인 (버프)
	if ( g_MyInfo.GetPublicID() == "jch2305" )
	{
		g_RecvPacketLogManager.AddCharRecvInfo( szName, ioRecvPacketLogManager::RPT_Buff, szBuffName );
	}
#endif
}

void ioPlayStage::OnCheckEscapeSkill( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;

	pChar->ApplyCheckEscapeSkill( rkPacket );
}

void ioPlayStage::OnSmartBoomObjectItemGenerateWeapon( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;

	ioObjectItem *pObject = pChar->GetObject();
	ioSmartBoomObjectItem *pSmart = ToSmartBoomObjectItem( pObject );
	if( pSmart )
	{
		pSmart->ApplySmartBoomObjectItem( this, rkPacket );
	}
}

void ioPlayStage::CheckPassiveGauge( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;

	pChar->ApplyPassiveGauge( rkPacket );
}

void ioPlayStage::OnWitchState( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;

	if( !pChar->CheckGrapplingExceptionState() )
		return;

	pChar->ApplyWitchState( rkPacket );
}

void ioPlayStage::OnNakoruruState( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;

	if( !pChar->CheckGrapplingExceptionState() )
		return;

	pChar->ApplyNakoruruState( rkPacket );
}

void ioPlayStage::CheckOwnerCanTackMachine()
{
	ioBaseChar *pOwnerChar = GetOwnerChar();
	if( !pOwnerChar )	return;

	if( !m_pMachineStructMgr )
		return;

	ioMachineStruct *pMachine = m_pMachineStructMgr->FindTakeEnableMachine( pOwnerChar );

	bool bOwnerEnableMachineTake = false;
	bool bOwnerCanTack = pOwnerChar->IsMachineTakeEnableState( false );

	GameEntityList::iterator iter=m_GameEntityList.begin();
	for( ; iter!=m_GameEntityList.end() ; ++iter )
	{
		ioMachineStruct *pTarget = ToMachineStruct( *iter );
		if( !pTarget )	continue;

		if( bOwnerCanTack && pTarget->IsTakeEnableState() && pMachine == pTarget )
		{
			if( !pTarget->IsEnableTakeEffect() )
				pTarget->SetEnableTakeEffect( true );

			bOwnerEnableMachineTake = true;
		}
		else if( pTarget->IsEnableTakeEffect() )
		{
			pTarget->SetEnableTakeEffect( false );
		}
	}
}

void ioPlayStage::OnMachineStructUDP( SP2Packet &rkPacket )
{
	int iIndex;
	rkPacket >> iIndex;

	ioMachineStruct *pMachineStruct = g_MachineStructMgr.FindMachineStruct( iIndex );
	if( !pMachineStruct )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnMachineStructUDP - %d MachineStruct Not Exist", iIndex );
		return;
	}

	pMachineStruct->ApplyMachineStructExtraInfo( rkPacket );
}

void ioPlayStage::ProcessNPCEntityCollision()
{	
	bool bCollisioned = false;
	float fPushAmt;
	D3DXVECTOR3 vPushDir;

	BaseCharList::iterator iter;
	for( iter=m_BaseCharList.begin() ; iter!=m_BaseCharList.end() ; ++iter )
	{
		
		ioBaseChar *pFirst = *iter;
		if( !ToNpcChar(pFirst) )
			continue;

		if( !ToNpcChar(pFirst)->IsNeedProcess() )
			continue;

		BaseCharList::iterator iter_sec;
		for( iter_sec=m_BaseCharList.begin() ; iter_sec!=m_BaseCharList.end() ; ++iter_sec )
		{	
			ioBaseChar *pSecond = *iter_sec;
			if( !ToNpcChar(pSecond) )
				continue;

			if( pFirst == pSecond || pSecond->IsEntityCollisionSkipState( pFirst ) )
				continue;

			if( pFirst->IsCharCollisionSkipState(pSecond->GetTeam(), (int)pSecond->GetState()) )
				continue;

			if( pSecond->IsCharCollisionSkipState(pFirst->GetTeam(), (int)pFirst->GetState()) )
				continue;

			if( !ioMapCollisionHelper::IsCollisionCheckRange( pFirst->GetWorldPosition(), pSecond->GetWorldPosition() ) )
				continue;

			if( !CalcEntityCollisionPushAmt( pFirst, pSecond, vPushDir, fPushAmt ) )
				continue;

			if( pFirst->IsSystemActivityState() && pSecond->IsSystemActivityState() )
				continue;
	
			// Grappling Check
			if( pFirst->CheckGrapplingPushedCollision() )
			{	
								
				pFirst->CheckWoundGrapplingBuff( pSecond->GetCharName() );

				pSecond->CheckCollisionGrapplingBuff( pFirst->GetGrapplingAttacker(),
													  pFirst->GetCollisionGrapplingBufList() );
			}
			else if( pFirst->CheckGrapplingPushedBuffCollision() )
			{	
				pFirst->CheckWoundGrapplingPushedBuff();

				pSecond->CheckCollisionGrapplingPushedBuff( pFirst->GetGrapplingAttacker(),
															pFirst->GetCollisionGrapplingBufList() );
			}
			else if( pFirst->CheckGrapplingSwingCollision() )
			{
				
				pFirst->CheckSkillGrapplingCancelBuff( pSecond->GetCharName(), true );

				pSecond->CheckCollisionSwingGrapplingBuff( pFirst->GetGrapplingAttacker(),
														   pFirst->GetCollisionSwingGrapplingBufList() );
			}

			if( pSecond->CheckGrapplingPushedCollision() )
			{
			
				pSecond->CheckWoundGrapplingBuff( pFirst->GetCharName() );

				pFirst->CheckCollisionGrapplingBuff( pSecond->GetGrapplingAttacker(),
													 pSecond->GetCollisionGrapplingBufList() );
			}
			else if( pSecond->CheckGrapplingPushedBuffCollision() )
			{
				pSecond->CheckWoundGrapplingPushedBuff();

				pFirst->CheckCollisionGrapplingPushedBuff( pSecond->GetGrapplingAttacker(),
														   pSecond->GetCollisionGrapplingBufList() );
			}
			else if( pSecond->CheckGrapplingSwingCollision() )
			{				
			
				pSecond->CheckSkillGrapplingCancelBuff( pFirst->GetCharName(), true );

				pFirst->CheckCollisionSwingGrapplingBuff( pSecond->GetGrapplingAttacker(),
														  pSecond->GetCollisionSwingGrapplingBufList() );
			}
		}
	}
}

void ioPlayStage::SetSummonDummyInfo( ioHashString pOwnerName, int iDummyIndex, D3DXVECTOR3 vPos, bool bSend )
{
	DummyPosInfo Info;
	Info.m_OwnerName = pOwnerName;
	Info.m_iDummyIndex = iDummyIndex;
	Info.m_vPos = vPos;

	m_WoodSummonPositionList.push_back( Info );

	if( bSend )
	{
		SP2Packet kPacket( CUPK_SUMMON_POS_SYNC );
		kPacket << SPS_ADD;
		kPacket << pOwnerName;
		kPacket << iDummyIndex;
		kPacket << vPos;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

void ioPlayStage::RemoveSummonDummyInfo( int iDummyIndex, bool bSend )
{
	SummonList::iterator iter = m_WoodSummonPositionList.begin();
	for( ; iter != m_WoodSummonPositionList.end(); ++iter )
	{
		if( (*iter).m_iDummyIndex == iDummyIndex )
		{
			m_WoodSummonPositionList.erase( iter );
			return;
		}
	}

	if( bSend )
	{
		SP2Packet kPacket( CUPK_SUMMON_POS_SYNC );
		kPacket << SPS_REMOVE;
		kPacket << iDummyIndex;
		P2PNetwork::SendToAllPlayingUser( kPacket );
	}
}

SummonList ioPlayStage::GetSummonDummyInfoList()
{
	return m_WoodSummonPositionList;
}

void ioPlayStage::OnSummonDummyInfo( SP2Packet &rkPacket )
{
	int iSyncState;
	rkPacket >> iSyncState;

	switch( iSyncState )
	{
	case SPS_ADD:
		{
			ioHashString pOwnerName;
			int iDummyIndex;

			D3DXVECTOR3 vPos;
			rkPacket >> pOwnerName;
			rkPacket >> iDummyIndex;
			rkPacket >> vPos;

			SetSummonDummyInfo( pOwnerName, iDummyIndex, vPos, false );
		}
		break;
	case SPS_REMOVE:
		{
			int iDummyIndex;
			rkPacket >> iDummyIndex;

			RemoveSummonDummyInfo( iDummyIndex, false );
		}
		break;
	}
}

void ioPlayStage::OnTraceMineWaitExplosion( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnTraceMineWaitExplosion - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;

	ioMineWeapon2 *pMine2 = ToMineWeapon2( g_WeaponMgr.FindWeapon( pAttacker, dwWeaponIdx ) );
	if( pMine2 )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		pMine2->SetPosition( vPos );
		pMine2->SetWaitExplosionMine( this, false );
	}
}

void ioPlayStage::OnTraceMineFlyTraceStart( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnTraceMineFlyTraceStart - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;

	ioMineWeapon2 *pMine2 = ToMineWeapon2( g_WeaponMgr.FindWeapon( pAttacker, dwWeaponIdx ) );
	if( pMine2 )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		ioHashString szTarget;
		rkPacket >> szTarget;

		pMine2->SetPosition( vPos );
		pMine2->SetTraceTarget( szTarget );
		pMine2->SetWaitFlyMine( this, false );
		
	}
}

void ioPlayStage::OnTraceMineNormalDead( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnNormalDeadMine2 - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;

	ioMineWeapon2 *pMine2 = ToMineWeapon2( g_WeaponMgr.FindWeapon( pAttacker, dwWeaponIdx ) );
	if( pMine2 )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		pMine2->SetPosition( vPos );
		pMine2->SetNormalDeadMine( this, false );
	}
}

void ioPlayStage::OnTraceMineExplosion( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnTraceMineExplosion - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;

	ioMineWeapon2 *pMine2 = ToMineWeapon2( g_WeaponMgr.FindWeapon( pAttacker, dwWeaponIdx ) );
	if( pMine2 )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		pMine2->SetPosition( vPos );
		pMine2->ExplosionMine( this, true, false );
	}
}


void ioPlayStage::OnTrace2MineDelay( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnTrace2MineDelay - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;

	ioMineWeapon3 *pMine3 = ToMineWeapon3( g_WeaponMgr.FindWeapon( pAttacker, dwWeaponIdx ) );
	if( pMine3 )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		pMine3->SetPosition( vPos );
		pMine3->SetDelay( this, false, false );
	}
}


void ioPlayStage::OnTrace2MineNormalDead( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnTrace2MineNormalDead - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;

	ioMineWeapon3 *pMine3 = ToMineWeapon3( g_WeaponMgr.FindWeapon( pAttacker, dwWeaponIdx ) );
	if( pMine3 )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		pMine3->SetPosition( vPos );
		pMine3->SetNormalDeadMine( this, false );
	}
}

void ioPlayStage::OnTrace2MineWaitExplosion( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnTrace2MineWaitExplosion - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;

	ioMineWeapon3 *pMine3 = ToMineWeapon3( g_WeaponMgr.FindWeapon( pAttacker, dwWeaponIdx ) );
	if( pMine3 )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		pMine3->SetPosition( vPos );
		pMine3->SetWaitExplosionMine( this, false );
	}
}

void ioPlayStage::OnTrace2MineExplosion( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnTrace2MineExplosion - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;

	ioMineWeapon3 *pMine3 = ToMineWeapon3( g_WeaponMgr.FindWeapon( pAttacker, dwWeaponIdx ) );
	if( pMine3 )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		pMine3->SetPosition( vPos );
		pMine3->ExplosionMine( this, true, false );
	}
}

void ioPlayStage::OnAirMineWaitExplosion( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnAirMineWaitExplosion - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;

	ioMineWeapon4 *pMine4 = ToMineWeapon4( g_WeaponMgr.FindWeapon( pAttacker, dwWeaponIdx ) );
	if( pMine4 )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		pMine4->SetPosition( vPos );
		pMine4->SetWaitExplosionMine( this, false );
	}
}

void ioPlayStage::OnAirNormalDeadMine( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	rkPacket >> szAttacker;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnAirNormalDeadMine - Attacker %s Not Exist", szAttacker.c_str() );
		return;
	}

	DWORD dwWeaponIdx;
	rkPacket >> dwWeaponIdx;

	ioMineWeapon4 *pMine4 = ToMineWeapon4( g_WeaponMgr.FindWeapon( pAttacker, dwWeaponIdx ) );
	if( pMine4 )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		pMine4->SetPosition( vPos );
		pMine4->SetNormalDeadMine( this, false );
	}
}

void ioPlayStage::OnAirMineExplosion( SP2Packet &rkPacket )
{
	ioHashString szAttacker;
	int iWeaponIdx;
	rkPacket >> szAttacker >> iWeaponIdx;

	ioBaseChar *pAttacker = GetBaseChar( szAttacker );
	if( !pAttacker )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnAirMineExplosion - Attacker %s Not Exist",
			szAttacker.c_str() );
		return;
	}

	ioMineWeapon4 *pMine4 = ToMineWeapon4( g_WeaponMgr.FindWeapon( pAttacker, iWeaponIdx ) );
	if( pMine4 )
	{
		D3DXVECTOR3 vPos;
		rkPacket >> vPos;

		pMine4->SetPosition( vPos );
		pMine4->ExplosionMine( this );
	}
}

void ioPlayStage::OnReflectAttackExplicit( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( pChar->GetState() == CS_DIE )
		return;

	pChar->ApplyReflectFireExplicit( rkPacket );
}

void ioPlayStage::DestroyUnAvailableFieldItemByTime()
{
	if( !ToTrainingMode(m_pPlayMode) )
		return;

	if( !P2PNetwork::IsNetworkPlaying() || !IsNetworkMode() )
		return;

	GameEntityList::iterator iter = m_GameEntityList.begin();
	while( iter != m_GameEntityList.end() )
	{
		ioFieldItem *pField = ToFieldItem( *iter );
		if( !pField )
		{
			++iter;
			continue;
		}

		if( pField->GetCrownItemType() != ioItem::MCT_NONE || pField->IsObjectItem() )
		{
			++iter;
			continue;
		}

		if( pField->IsNeedDestroyByTime() )
		{
			iter = EraseGameEntity( iter );
		}
		else
		{
			++iter;
		}
	}
}

void ioPlayStage::OnCharActionInfo( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;

	pChar->ApplyCharActionInfo( rkPacket );
}

void ioPlayStage::StartScreenBlindBuff( const ScreenBlindTime *pTime, const ScreenBlindSetting *pSetting, float fRange )
{
	if( !pTime )
		return;

	if( m_ScreenBlindType == SBT_FIXED_BLIND )     // 다른 블라인드로 부터 간섭 받지 않는다.
		return;

	DWORD dwEventCode = 0;

	if( pSetting )
		m_CurBlindSetting = *pSetting;
	else
		m_CurBlindSetting = m_SkillBlind;

	m_CurBlindTime = *pTime;

	if( m_CurBlindSetting.m_iMaxAlpha == 0 )
		return;

	m_ScreenBlindType = SBT_BUFF;
	m_fScreenBlindBuffRange = fRange;

	if( m_CurBlindTime.m_dwUpTime > 0 )
		m_ScreenBlindState = SBS_UP;
	else
		m_ScreenBlindState = SBS_CENTER;

	m_dwScreenBlindCurTime = FRAMEGETTIME();
}

void ioPlayStage::EndScreenBlindBuff()
{
	if( m_ScreenBlindType == SBT_NONE )
		return;

	if( m_ScreenBlindType != SBT_BUFF )
		return;

	DWORD dwCurTime = FRAMEGETTIME();
	m_BuffBlindState = SBS_DOWN;
	m_dwBuffBlindCurTime = dwCurTime;

	if(  m_pBuffBlind )
	{
		if( m_iBuffBlindMaxAlpha )
		{
			m_pBuffBlind->SetAlpha( m_iBuffBlindMaxAlpha );
		}
		else
		{
			m_ScreenBlindType  = SBT_NONE;
			m_ScreenBlindState = SBS_NONE;
			m_dwScreenBlindCurTime = 0;
			m_fScreenBlindBuffRange = 0.0f;

			if( m_pSceneMgr )
			{
				m_pSceneMgr->SetScreenBlind( 0x0 );
			}
		}
	}
}

void ioPlayStage::OnCounterAttackSync( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( !pChar->CheckGrapplingExceptionState() )
		return;

	pChar->ApplyCounterAttack( rkPacket );
}

void ioPlayStage::OnHideObjectWeaponIndexSync( SP2Packet &rkPacket )
{
	ioHashString	szName;
	DWORD			dwWeaponIndex = 0;
	rkPacket >> szName;
	rkPacket >> dwWeaponIndex;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )	return;
	if( dwWeaponIndex == 0 )	return;

	pChar->SetWeaponIndexBase( dwWeaponIndex );
}

void ioPlayStage::OnExtraItemDisassembleResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult = 0;
	rkPacket >> iResult;

	if( iResult == EXTRAITEM_DISASSEMBLE_OK )
	{
		int iSlotIndex = 0;
		int iGainCode, iGainCnt;

		rkPacket >> iSlotIndex;
		rkPacket >> iGainCode >> iGainCnt;

		ioHashString szItemName;
		ioHashString szIconName;
		ioHashString szSubIconName;
		int iItemReinforce = 0;
		bool bExtraItemCustom = false;
		int nGradeType = 0;

#if defined( USE_GA )
		int iGAItemIndex = 0;
#endif
		ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
		if( pExtraItem )
		{
			EXTRAITEMSLOT kSlot;
			if( pExtraItem->GetExtraItem( iSlotIndex, kSlot ) )
			{
				szItemName = kSlot.m_Name;
				szIconName = kSlot.m_IconName;
				iItemReinforce = kSlot.m_iReinforce;
				if( kSlot.m_dwMaleCustom > 0 || kSlot.m_dwFemaleCustom > 0 )
					bExtraItemCustom = true;

				const ioItem *pItem = g_ItemMaker.GetItemConst( kSlot.m_iItemCode, __FUNCTION__ );
				if( pItem )
					nGradeType = pItem->GetGradeType();

#if defined( USE_GA )
				iGAItemIndex = kSlot.m_iItemCode;
#endif

			}
		}

		// 제거
		if( !g_MyInfo.DeleteExtraItem( iSlotIndex ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "장비분해 예외오류1" );
			return;
		}

		// UI 처리
		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*> ( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
		if( pInvenWnd && pInvenWnd->IsShow() )
		{
			pInvenWnd->UpdateTab( false, true );

			char szConvertNum[MAX_PATH] = "";
			Help::ConvertNumToStrComma( iGainCnt, szConvertNum, sizeof( szConvertNum ) );

			ComplexStringPrinterVec vTitle;
			ComplexStringPrinterVec vDesc;

			ioComplexStringPrinter kTitle1;
			kTitle1.SetTextStyle( TS_NORMAL );
			kTitle1.SetBkColor( 0, 0, 0 );	
			kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kTitle1.AddTextPiece( FONT_SIZE_17, "%s +%d", szItemName.c_str(), iItemReinforce );
			vTitle.push_back( kTitle1 );

			ioComplexStringPrinter kTitle2;
			kTitle2.SetTextStyle( TS_NORMAL );
			kTitle2.SetBkColor( 0, 0, 0 );
			kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );

			char szKey[MAX_PATH] = "";
			ioEtcItem *pEtcItem = g_EtcItemMgr.FindEtcItem( iGainCode );
			if( pEtcItem )
				sprintf_e( szKey, "%s %s개", pEtcItem->GetName().c_str(), szConvertNum );
			
			kTitle2.AddTextPiece( FONT_SIZE_17, szKey );

			vTitle.push_back( kTitle2 );

			ioComplexStringPrinter kDesc1;
			kDesc1.SetTextStyle( TS_NORMAL );
			kDesc1.SetBkColor( 0, 0, 0 );	
			kDesc1.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kDesc1.AddTextPiece( FONT_SIZE_12, "★ [%s +%d] 분해 완료.", szItemName.c_str(), iItemReinforce );
			vDesc.push_back( kDesc1 );

			ioComplexStringPrinter kDesc2;
			kDesc2.SetTextStyle( TS_NORMAL );
			kDesc2.SetBkColor( 0, 0, 0 );
			kDesc2.SetTextColor( TCT_DEFAULT_DARKGRAY );
			
			if( pEtcItem )
				sprintf_e( szKey, "★ %s %s개 지급.", pEtcItem->GetName().c_str(), szConvertNum );
			
			kDesc2.AddTextPiece( FONT_SIZE_12, szKey );			
			vDesc.push_back( kDesc2 );

			pInvenWnd->ShowItemRecvSellInfoWnd( ItemRecvSellInfoWnd::ITEM_DISASSEMBLE, ItemRecvSellInfoWnd::ITEM_PRESENT, szIconName, szSubIconName, vTitle, vDesc, -1, iItemReinforce, bExtraItemCustom, false, nGradeType );
		}

#if defined( USE_GA )
		char chLabel[32] = {0,};

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
			sprintf_e( chLabel, "%d", iGAItemIndex );
		else
			SafeSprintf( chLabel, sizeof(chLabel), "%1", iGAItemIndex );

		// ITEM_DISASSEMBLE
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Item"
			, "Disassemble"
			, chLabel
			, 1
			, "%2FITEM%2FDISASSEMBLE" );
#endif
	}
	else if( iResult == EXTRAITEM_DISASSEMBLE_FAIL )
	{
		int iNum;
		rkPacket >> iNum;

		g_GUIMgr.SetMsgBox( MB_OK, NULL, "장비분해 실패 : %d", iNum );
	}
}

void ioPlayStage::OnMedalItemSellResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult = 0;
	rkPacket >> iResult;

	if( iResult == MEDALITEM_SELL_OK )
	{
		int iMedalIndex = 0;
		__int64 iAddPeso, iPeso;
		rkPacket >> iMedalIndex;
		rkPacket >> iAddPeso;
		rkPacket >> iPeso;
		g_MyInfo.SetMoney( iPeso );

		ioHashString szItemName;
		ioHashString szIconName;
		ioHashString szSubIconName;

		ioUserMedalItem *pMedaltem = g_MyInfo.GetUserMedalItem();
		if( pMedaltem )
		{
			ioUserMedalItem::MEDALITEMSLOT kSlot;
			if( pMedaltem->GetMedalItem( iMedalIndex, 0, kSlot, true ) )
			{
				const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( iMedalIndex );
				if( pItemInfo )
				{
					szItemName = pItemInfo->m_sName;
					szIconName = pItemInfo->m_sIcon;
					szSubIconName = pItemInfo->m_sSubIcon;
				}
			}
		}

		if( !g_MyInfo.DeleteMedalItem( iMedalIndex, 0 ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "메달 팔기 예외오류, %d", iMedalIndex );
			return;
		}

		// UI 처리
		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*> ( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
		if( pInvenWnd && pInvenWnd->IsShow() )
		{
			pInvenWnd->UpdateTab( false, true );

			char szConvertNum[MAX_PATH] = "";
			Help::ConvertNumToStrComma( iAddPeso, szConvertNum, sizeof( szConvertNum ) );

			ComplexStringPrinterVec vTitle;
			ComplexStringPrinterVec vDesc;

			ioComplexStringPrinter kTitle1;
			kTitle1.SetTextStyle( TS_NORMAL );
			kTitle1.SetBkColor( 0, 0, 0 );	
			kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kTitle1.AddTextPiece( FONT_SIZE_17, "%s", szItemName.c_str() );
			vTitle.push_back( kTitle1 );

			ioComplexStringPrinter kTitle2;
			kTitle2.SetTextStyle( TS_NORMAL );
			kTitle2.SetBkColor( 0, 0, 0 );
			kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kTitle2.AddTextPiece( FONT_SIZE_17, "%s PESO", szConvertNum );
			vTitle.push_back( kTitle2 );

			ioComplexStringPrinter kDesc1;
			kDesc1.SetTextStyle( TS_NORMAL );
			kDesc1.SetBkColor( 0, 0, 0 );	
			kDesc1.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kDesc1.AddTextPiece( FONT_SIZE_12, "%s 판매 완료.", szItemName.c_str() );
			vDesc.push_back( kDesc1 );

			ioComplexStringPrinter kDesc2;
			kDesc2.SetTextStyle( TS_NORMAL );
			kDesc2.SetBkColor( 0, 0, 0 );
			kDesc2.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kDesc2.AddTextPiece( FONT_SIZE_12, "%s페소가 지급되었습니다.", szConvertNum );
			vDesc.push_back( kDesc2 );

			pInvenWnd->ShowItemRecvSellInfoWnd( ItemRecvSellInfoWnd::ITEM_SELL, ItemRecvSellInfoWnd::ITEM_MEDAL, szIconName, szSubIconName, vTitle, vDesc, -1, 0, false, false, 0 );
		}

#if defined( USE_GA )
		char chItemIndex[32]	= {0,};
		char chSlash[16]		= {0,};
		char chPostData[256]	= {0,};

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		{
			sprintf_e( chItemIndex, "%d", iMedalIndex );
			sprintf_e( chSlash, "%%2F" );
			sprintf_e( chPostData, "%sPESO_GET_BUY_MEDAL_ITEM%s%s", chSlash, chSlash, chItemIndex );
		}
		else
		{
			SafeSprintf( chItemIndex, sizeof(chItemIndex), "%1", iMedalIndex );
			SafeSprintf( chSlash, sizeof(chSlash), "%2F" );
			SafeSprintf( chPostData, sizeof(chPostData), "%1PESO_GET_BUY_MEDAL_ITEM%2%3", chSlash, chSlash, chItemIndex );
		}		

		// PESO_GET_BUY_MEDAL_ITEM
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Peso"
			, "Medal"
			, ""
			, iAddPeso
			, chPostData );
#endif
	}
	else if( iResult == MEDALITEM_SELL_FAIL )
	{
		int iNum;
		rkPacket >> iNum;

		g_GUIMgr.SetMsgBox( MB_OK, NULL, "메달 팔기 실패 (%d)", iNum );
	}
}

void ioPlayStage::OnCustomMedalItemSellResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult = 0;
	rkPacket >> iResult;

	if( iResult == MEDALITEM_SELL_OK )
	{
		int iMedalIndex = 0;
		int iCustomIndex = 0;
		__int64 iAddPeso, iPeso;
		rkPacket >> iMedalIndex;
		rkPacket >> iCustomIndex;
		rkPacket >> iAddPeso;
		rkPacket >> iPeso;
		g_MyInfo.SetMoney( iPeso );

		ioHashString szItemName;
		ioHashString szIconName;
		ioHashString szSubIconName;

		ioUserMedalItem *pMedaltem = g_MyInfo.GetUserMedalItem();
		if( pMedaltem )
		{
			ioUserMedalItem::MEDALITEMSLOT kSlot;
			if( pMedaltem->GetMedalItem( iMedalIndex, iCustomIndex, kSlot, true ) )
			{
				const ioMedalItemInfoManager::ItemInfo *pItemInfo = g_MedalItemMgr.GetItemInfo( iMedalIndex );
				if( pItemInfo )
				{
					szItemName = pItemInfo->m_sName;
					szIconName = pItemInfo->m_sIcon;
					szSubIconName = pItemInfo->m_sSubIcon;
				}
			}
		}

		if( !g_MyInfo.DeleteMedalItem( iMedalIndex, iCustomIndex ) )
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "메달 팔기 예외오류, %d", iMedalIndex );
			return;
		}

		// UI 처리
		MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*> ( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
		if( pInvenWnd && pInvenWnd->IsShow() )
		{
			pInvenWnd->UpdateTab( false, true );

			char szConvertNum[MAX_PATH] = "";
			Help::ConvertNumToStrComma( iAddPeso, szConvertNum, sizeof( szConvertNum ) );

			ComplexStringPrinterVec vTitle;
			ComplexStringPrinterVec vDesc;

			ioComplexStringPrinter kTitle1;
			kTitle1.SetTextStyle( TS_NORMAL );
			kTitle1.SetBkColor( 0, 0, 0 );	
			kTitle1.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kTitle1.AddTextPiece( FONT_SIZE_17, "%s", szItemName.c_str() );
			vTitle.push_back( kTitle1 );

			ioComplexStringPrinter kTitle2;
			kTitle2.SetTextStyle( TS_NORMAL );
			kTitle2.SetBkColor( 0, 0, 0 );
			kTitle2.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kTitle2.AddTextPiece( FONT_SIZE_17, "%s PESO", szConvertNum );
			vTitle.push_back( kTitle2 );

			ioComplexStringPrinter kDesc1;
			kDesc1.SetTextStyle( TS_NORMAL );
			kDesc1.SetBkColor( 0, 0, 0 );	
			kDesc1.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kDesc1.AddTextPiece( FONT_SIZE_12, "%s 판매 완료.", szItemName.c_str() );
			vDesc.push_back( kDesc1 );

			ioComplexStringPrinter kDesc2;
			kDesc2.SetTextStyle( TS_NORMAL );
			kDesc2.SetBkColor( 0, 0, 0 );
			kDesc2.SetTextColor( TCT_DEFAULT_DARKGRAY );
			kDesc2.AddTextPiece( FONT_SIZE_12, "%s페소가 지급되었습니다.", szConvertNum );
			vDesc.push_back( kDesc2 );

			pInvenWnd->ShowItemRecvSellInfoWnd( ItemRecvSellInfoWnd::ITEM_SELL, ItemRecvSellInfoWnd::ITEM_MEDAL, szIconName, szSubIconName, vTitle, vDesc, -1, 0, false, false, 0 );
		}

#if defined( USE_GA )
		char chItemIndex[32]	= {0,};
		char chSlash[16]		= {0,};
		char chPostData[256]	= {0,};

		if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
		{
			sprintf_e( chItemIndex, "%d", iMedalIndex );
			sprintf_e( chSlash, "%%2F" );
			sprintf_e( chPostData, "%sPESO_GET_BUY_MEDAL_ITEM%s%s", chSlash, chSlash, chItemIndex );
		}
		else
		{
			SafeSprintf( chItemIndex, sizeof(chItemIndex), "%1", iMedalIndex );
			SafeSprintf( chSlash, sizeof(chSlash), "%2F" );
			SafeSprintf( chPostData, sizeof(chPostData), "%1PESO_GET_BUY_MEDAL_ITEM%2%3", chSlash, chSlash, chItemIndex );
		}		

		// PESO_GET_BUY_MEDAL_ITEM
		g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
			, "Peso"
			, "Medal"
			, ""
			, iAddPeso
			, chPostData );
#endif
	}
	else if( iResult == MEDALITEM_SELL_FAIL )
	{
		int iNum;
		rkPacket >> iNum;

		g_GUIMgr.SetMsgBox( MB_OK, NULL, "메달 팔기 실패 (%d)", iNum );
	}
}

void ioPlayStage::OnSendPresentByEtcItemFail( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iLog;
	rkPacket >> iLog;
	switch( iLog )
	{
	case INCORRECT_ITEM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "보낼 수 없는 아이템입니다." );
		break;
	case INCORRECT_USER:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "잘못된 닉네임입니다." );
		break;
	}
}

void ioPlayStage::OnSoldierExpAddUpdate( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	ioHashString szPublicID;
	int iClassType;
	int iClassLevel;
	rkPacket >> szPublicID;
	rkPacket >> iClassType;
	rkPacket >> iClassLevel;

	if( szPublicID.IsEmpty() )
		return;

	BaseCharList::iterator iter;
	for( iter=m_BaseCharList.begin() ; iter!=m_BaseCharList.end() ; ++iter )
	{
		ioBaseChar *pChar = *iter;		
		if( pChar && pChar->GetCharName() == szPublicID && pChar->GetClassLevel() != iClassLevel )
		{
			CHARACTER rkInfo = pChar->GetCharacterInfo();
			if( rkInfo.m_class_type == iClassType )
			{
				pChar->SetClassLevel( iClassLevel );
				g_UserInfoMgr.SetCharacter( szPublicID, rkInfo, iClassLevel );
				break;
			}
		}
	}
}

void ioPlayStage::OnRechargeExtraItem( SP2Packet &rkPacket )
{
	g_ItemRechargeMgr.ApplyRechargeExtraItem( rkPacket );
}

void ioPlayStage::OnRechargeAccessory( SP2Packet &rkPacket )
{
	g_AccRechargeMgr.ApplyRechargeAccessory(rkPacket);
}

void ioPlayStage::OnCompoundAccessory( SP2Packet &rkPacket )
{
	g_AccessoryInfoMgr.OnAccessoryCompound( rkPacket );
}

void ioPlayStage::OnReinforceAccessory( SP2Packet &rkPacket )
{
	g_AccessoryInfoMgr.OnAccessoryReinforce( rkPacket );
}

void ioPlayStage::OnReinforceExtraItem( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int eType;
	rkPacket >> eType;
	switch( eType )
	{
	case ITEM_COMPOUND_OK:
		{
			bool bSuccess;
			int nTargetSlot;
			int nTargetReinforce;
			WORD wFailExp;
			int nMaterialType;
			int nMaterialCnt;

			rkPacket >> bSuccess;
			rkPacket >> nTargetSlot >> nTargetReinforce;
			rkPacket >> wFailExp;
			rkPacket >> nMaterialType >> nMaterialCnt;

			// HARDCODE : 갱신전 정보값 활용때문에 UI쪽을 먼저 진행.
			// UI 출력
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
				pInvenWnd->SetReinforceResultInfo( bSuccess, nTargetSlot, nTargetReinforce );

			__int64 iMoney;
			rkPacket >> iMoney;
			g_MyInfo.SetMoney( iMoney );

			ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
			if( pExtraItem )
			{
				EXTRAITEMSLOT kTargetSlot;
				if( pExtraItem->GetExtraItem( nTargetSlot, kTargetSlot ) )
				{
					//이전 실패 경험치
					int nPreFailExp = (int)kTargetSlot.m_failExp;

					//강화 레벨 설정
					kTargetSlot.m_iReinforce = nTargetReinforce;

					//성공시 실패경험치를 0으로, 실패시 해당 경험치를 반영
					if ( bSuccess || nTargetReinforce >= g_CompoundMaterialMgr.GetFixedFailReinforce() )
						kTargetSlot.m_failExp = 0;
					else
						kTargetSlot.m_failExp = wFailExp;

					//결과에 적용
					if ( pInvenWnd )
						pInvenWnd->SetReinforceResultFailExp( nPreFailExp, (int)kTargetSlot.m_failExp );

					pExtraItem->SetExtraItem( kTargetSlot );
				}
				else
					LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemMaterialCompound - Target Error" );
			}
		}
		return;
	case ITEM_COMPOUND_ERROR:
		{
			int iErrorNum;
			rkPacket >> iErrorNum;

			g_GUIMgr.SetMsgBox( MB_OK, NULL, "장비 강화중 오류가 발생하였습니다. : %d", iErrorNum );
		}
		break;
	}
}

void ioPlayStage::OnSelectExtraGashaponResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult = 0;

	int iItemCode = 0;
	int iReinforce = 0;
	int iSlotIndex = 0;
	int iTradeType = 0;
	int iPeriodType = 0;
	int iValue1 = 0;
	int iValue2 = 0;
	DWORD dwMaleCustom = 0;
	DWORD dwFemaleCustom = 0;
	int iPeriodTime = 0;
	__int64 iPeso;
	int iCash = 0;
	int iChannelingCash = 0;

	rkPacket >> iResult;

	switch( iResult )
	{
	case BUY_SELECT_EXTRA_GASHAPON_OK:
		{
			rkPacket >> iItemCode >> iReinforce >> iSlotIndex >> iTradeType >> iPeriodType >> iValue1 >> iValue2 >> dwMaleCustom >> dwFemaleCustom;
			rkPacket >> iPeriodTime >> iPeso >> iCash >> iChannelingCash;

			ioUserExtraItem *pUserExtraItem = g_MyInfo.GetUserExtraItem();
			if( pUserExtraItem )
			{
				EXTRAITEMSLOT kSlot;
				kSlot.m_iItemCode = iItemCode;
				kSlot.m_iReinforce = iReinforce;
				kSlot.m_iIndex = iSlotIndex;
				kSlot.m_iTradeState = iTradeType;
				kSlot.m_PeriodType = iPeriodType;
				kSlot.m_iValue1 = iValue1;
				kSlot.m_iValue2 = iValue2;
				kSlot.m_dwMaleCustom = dwMaleCustom;
				kSlot.m_dwFemaleCustom = dwFemaleCustom;

				pUserExtraItem->AddExtraItemData( kSlot );
			}

			g_MyInfo.SetMoney( iPeso );
			g_MyInfo.SetCash( iCash );
			g_MyInfo.SetChannelingCash( iChannelingCash );

			SelectExtraGashaponWnd *pMainWnd = dynamic_cast<SelectExtraGashaponWnd*>(g_GUIMgr.FindWnd( SELECT_EXTRA_GASHAPON_WND ));
			if( pMainWnd )
				pMainWnd->SetResult( iItemCode );
		}
		return; ///////////////////////////////////////////////////////////////
	case BUY_SELECT_EXTRA_GASHAPON_EXCEPTION:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "예외 오류가 발생되었습니다." );
		}
		break;
	case BUY_SELECT_EXTRA_GASHAPON_BILLING_WAIT:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "골드 처리중입니다.#잠시만 기다려 주세요." );
		}
		break;
	case BUY_SELECT_EXTRA_GASHAPON_EXCESS_MAX:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "해당 상품을 구입하면#이번달 최대 골드 사용 한도를 초과합니다.#(구매 한도:청소년 55,000원, 성인 330,000원)# #다음달 부터 상품 구매가 가능합니다." );
		}
		break;
	case BUY_SELECT_EXTRA_GASHAPON_BILLING_WANT_OF_CASH:
		{
			g_GUIMgr.ShowWnd( BILLING_WANT_WND );
		}
		break;
	case BUY_SELECT_EXTRA_GASHAPON_BILLING_DISCONNECT:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "골드 처리가 불가능합니다.#잠시만 기다려 주세요." );	
		}
		break;
	case BUY_SELECT_EXTRA_GASHAPON_BILLING_FAIL:
		{
			bool bBillingError    = false;
			ioHashString  sBillingError;
			rkPacket >> bBillingError;
			if( bBillingError )
				rkPacket >> sBillingError;

			ioHashString sErrorMent = STR(7);
			if( !bBillingError )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, sErrorMent.c_str() );
			else
			{
				ioHashString sError = "Billing Error : ";
				ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
				if( pLocal ) 
					sError = pLocal->GetBillingErrorMent(sBillingError);

				char szErrorFullMent[MAX_PATH]="";
				ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
				if( pNode && pNode->GetType() == CNT_NEXON )					
					StringCbPrintf( szErrorFullMent, sizeof( szErrorFullMent ), "%s",  sError.c_str() ); //Except Extracting Hangeul
				else
					StringCbPrintf( szErrorFullMent, sizeof( szErrorFullMent ), "%s#%s%s", sErrorMent.c_str(), sError.c_str(), sBillingError.c_str() ); //Except Extracting Hangeul

				g_GUIMgr.SetMsgBox( MB_OK, NULL, szErrorFullMent );
			}
		}
		break;
	}

	g_GUIMgr.HideWnd( SELECT_EXTRA_GASHAPON_WND );
}

void ioPlayStage::OnRelayControl( SP2Packet & rkPacket )
{
	int ctype;
	rkPacket >> ctype;
	switch(ctype)
	{
	case RC_USE_RELAYSVR:
		{
			char publicIP[64];
			int port = 0;
			rkPacket >> publicIP;
			rkPacket >> port;
			if( port == 0 )
	   			g_App.SetRelayServerState(false);	
			else		  
				g_App.SetRelayServerState(true);
			g_App.SetRelayServerIpaddr(publicIP,port);
			for(int i= 0; i<5; ++i)
			{
				SP2Packet kSendPacket( CUPK_CONNECT );
				kSendPacket << g_MyInfo.GetPublicID();
				P2PNetwork::SendToServer(kSendPacket);
			}
		}
		break;
	case RC_NOTUSE_RELAYSVR:
		{
			g_App.SetRelayServerState(false);
		}
		break; 
	}
}

void ioPlayStage::ExceptionExitRoom()
{
	SP2Packet kPacket( CTPK_EXIT_ROOM );
	if( g_BattleRoomMgr.IsBattleRoomMovePenaltyPeso() )
	{
		kPacket << EXIT_ROOM_PENALTY << g_MyInfo.GetCurPenaltyPeso() << true;
	}
	else
	{
		kPacket << EXIT_ROOM_EXCEPTION << 0 << true;
	}
	TCPNetwork::SendToServer( kPacket );

	if( GetModeType() != MT_TRAINING && GetModeType() != MT_HEADQUARTERS && GetModeType() != MT_HOUSE )
	{
		g_BattleRoomMgr.LeaveRoom();
		g_LadderTeamMgr.LeaveTeam();
		g_ShuffleRoomMgr.LeaveRoom();
		g_RankBattleMgr.SetRankBattlePlaying( false );
	}

	if( g_App.GetConnectedMoveMode() == ioApplication::CMM_LOBBY )
	{	
		g_App.SetNextStage( ioGameStage::GS_LOBBY, __FUNCTION__ );
		g_App.SetModeType( 0, 0, 0, PT_NONE );

		g_VictoriesMgr.Initialize();
		g_PlayBonusMgr.Initialize();

		// 예약작업 처리
		if( g_App.IsNextStageLobby() )
		{
			g_RserveTask.ExecuteReservedTask( RESERVEDCMD_WHEN_ENTERLOBBY );
			g_GUIMgr.SetReserveMsgBox( MB_RESERVE_EXCEPTION_BATTLE_ROOM );
		}		
	}	
}

void ioPlayStage::CheckValidateNetTable()
{
	DWORD dwCurTime = FRAMEGETTIME();

	if( m_dwCheckValidateNetTableTime == 0 ||
		(m_dwCheckValidateNetTableTime + 5000 < dwCurTime) )
	{
		m_dwCheckValidateNetTableTime = dwCurTime;

		g_App.CheckValidateNetTable( this );
	}
}

bool ioPlayStage::CheckValidateMember( ioHashString &rszCharName )
{
	bool bCheckMember = true;

	if( g_BattleRoomMgr.IsBattleRoom() && !g_BattleRoomMgr.IsHaveUser(rszCharName) )
		bCheckMember = false;
	else if( g_LadderTeamMgr.IsLadderTeam() && !g_LadderTeamMgr.IsHaveUser(rszCharName) )
		bCheckMember = false;
	if( g_ShuffleRoomMgr.IsShuffleRoom() && !g_ShuffleRoomMgr.IsHaveUser(rszCharName) )
		bCheckMember = false;

	if( !bCheckMember )
	{
		NetCheckMap::iterator iter = m_NetCheckMap.find( rszCharName );
		if( iter != m_NetCheckMap.end() )
		{
			int iCurCnt = iter->second + 1;

			if( iCurCnt > 10 )
				return true;
			else if( iCurCnt == 10 )
			{
				iter->second = iCurCnt;
				return false;
			}
			else
			{
				iter->second = iCurCnt;
				return true;
			}
		}
		else
		{
			m_NetCheckMap.insert( NetCheckMap::value_type(rszCharName, 1) );
			return true;
		}
	}

	return true;
}

void ioPlayStage::OnSubscriptionBuy( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult = 0;
	rkPacket >> iResult;

	if( iResult == SUBSCRIPTION_BUY_BILLING_WAIT )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1) );
	}
	else if( iResult == SUBSCRIPTION_BUY_WANT_OF_CASH )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(2) );
	}
	else if( iResult == SUBSCRIPTION_BUY_BILLING_DISCONNECT )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(3) );
	}
	else if( iResult == SUBSCRIPTION_BUY_BILLING_FAIL )
	{
		ioHashString sErrorMent = STR(4);
		g_GUIMgr.SetMsgBox( MB_OK, NULL, sErrorMent.c_str() );
	}
	else if( iResult == SUBSCRIPTION_BUY_BILLING_EXCESS_BUY )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(5) );
	}
	else if( iResult == SUBSCRIPTION_BUY_DISABLE_SUBSCRIPTION )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(7) );
	}
	else if( iResult == SUBSCRIPTION_BUY_EXCEED_PRESENT_CNT )
	{
		g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(8) );
	}
	else if( iResult == SUBSCRIPTION_BUY_SUCCESS )
	{
		__int64 iMoney      = 0;
		int iCash           = 0;
		int iPurchasedCash  = 0;
		int iChannelingCash = 0;

		rkPacket >> iMoney >> iCash >> iPurchasedCash >> iChannelingCash;

#if defined( USE_GA )
		bool bGACash	= false;
		bool bGAMoney	= false;
		int iPesoPrice	= g_MyInfo.GetMoney() - iMoney;
		int iCashPrice	= g_MyInfo.GetCash() - iCash;
		if( iCashPrice > 0 && iCashPrice < 1000000 )
			bGACash		= true;
		if( iPesoPrice > 0 )
			bGAMoney	= true;
#endif

		//사운드
		if( iMoney != g_MyInfo.GetMoney() || iCash != g_MyInfo.GetCash() )
			g_ExSoundMgr.PlaySound( ExSound::EST_SHOP_BUY_SUCCESS );

		g_MyInfo.SetMoney( iMoney );
		g_MyInfo.SetCash( iCash );
		g_MyInfo.SetPurchasedCash( iPurchasedCash );
		g_MyInfo.SetChannelingCash( iChannelingCash );

		ioHashString szSubscriptionID;
		int iSubscriptionGold;
		int iType, iValue1, iValue2;

		rkPacket >> szSubscriptionID >> iSubscriptionGold;
		rkPacket >> iType >> iValue1 >> iValue2;

		// UI처리
		NewShopWnd *pNewShopWnd = dynamic_cast<NewShopWnd*> ( g_GUIMgr.FindWnd( NEW_SHOP_WND ) );
		if( pNewShopWnd && pNewShopWnd->IsShow() )
		{
			NewShopBuyWnd *pNewShopBuyWnd = dynamic_cast<NewShopBuyWnd*> ( pNewShopWnd->FindChildWnd( NewShopWnd::ID_BUY_WND ) );
			if( pNewShopBuyWnd && pNewShopBuyWnd->IsShow() )
				pNewShopBuyWnd->ShowOnlyBoughtBtns( true );
		}

#if defined( USE_GA )
		if( bGACash )
		{
			int iLimitDate = g_HttpMng.GetScripHeroLimitDate();

			if( iLimitDate == 0 )
				iCashPrice = g_ClassPrice.GetMortmainCharCash( g_HttpMng.GetScripHeroIndex() );
			else
				iCashPrice = g_ClassPrice.GetClassBuyCash( g_HttpMng.GetScripHeroIndex(), iLimitDate );

			// GOLD_USE_HERO
			g_HttpMng.GA_ItemHitTracking( g_MyInfo.GetUserIndex() 
				, "Hero"
				, iCashPrice
				, 1
				, g_HttpMng.GetScripHeroIndex()
				, "Gold"
				, "%2FGOLD%2FUSE%2FHERO"
				, true );
		}
		if( bGAMoney )
		{
			// PESO_USE_HERO
			g_HttpMng.GA_ItemHitTracking( g_MyInfo.GetUserIndex() 
				, "Hero"
				, iPesoPrice
				, 1
				, g_HttpMng.GetScripHeroIndex()
				, "Peso"
				, "%2FPESO%2FUSE%2FHERO" );
		}
#endif
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "%s Type is Error(%d).", __FUNCTION__, iResult );
	}
}

void ioPlayStage::OnSubscriptionRetrCheck( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResultType;
	rkPacket >> iResultType;

	switch( iResultType )
	{
	case SUBSCRIPTION_RETR_CHECK_OK:
		{
			int iIndex;
			ioHashString szSubscriptionID;
			rkPacket >> iIndex >> szSubscriptionID;

			int iSubscriptionGold;
			int iRetraGold;
			rkPacket >> iSubscriptionGold >> iRetraGold;

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*> ( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->ShowSubscriptionRetrCheck( iIndex, szSubscriptionID, iSubscriptionGold, iRetraGold );
			}
		}
		break;
	case SUBSCRIPTION_RETR_CHECK_EXCEPTION:
		{
			int iErrorNum;
			rkPacket >> iErrorNum;
		}
		break;
	case SUBSCRIPTION_RETR_CHECK_NONE_INDEX:
	case SUBSCRIPTION_RETR_CHECK_NONE_ITEM:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "상품이 존재하지 않습니다." );
		break;
	case SUBSCRIPTION_RETR_CHECK_LIMIT_DATE:
		{
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*> ( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->ShowSubscriptionRetrCheckLimit();
			}
		}
		break;
	case SUBSCRIPTION_RETR_CHECK_NOT_SUPPORTED:
		{
			int iIndex;
			ioHashString szSubscriptionID;
			rkPacket >> iIndex >> szSubscriptionID;

			int iSubscriptionGold = 0;
			int iRetraGold = 0;

			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*> ( g_GUIMgr.FindWnd( MY_INVENTORY_WND ) );
			if( pInvenWnd && pInvenWnd->IsShow() )
			{
				pInvenWnd->ShowSubscriptionRetrCheck( iIndex, szSubscriptionID, iSubscriptionGold, iRetraGold );
			}
		}
		break;
	case SUBSCRIPTION_RETR_NOT_SUPPORTED:
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "청약철회 지원 안함" );
		break;
	}
}

void ioPlayStage::OnSelectGashaponResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	
	int iResult = 0;
	rkPacket >> iResult;
	
	if( iResult == BUY_SELECT_GASHAPON_OK )
	{
		__int64 iPeso = 0;
		int iCash = 0;
		int iChannelingCash = 0;
		GashaponPresent ReusltPresent;
		rkPacket >> ReusltPresent.m_iType;
		rkPacket >> ReusltPresent.m_iValue1;
		rkPacket >> ReusltPresent.m_iValue2;
		//rkPacket >> ReusltPresent.m_bLimited_Flag;	// 2020-03-26
		rkPacket >> iPeso;
		rkPacket >> iCash;
		rkPacket >> iChannelingCash;		
		
		g_MyInfo.SetMoney( iPeso );
		g_MyInfo.SetCash( iCash );
		g_MyInfo.SetChannelingCash( iChannelingCash );
		
		SelectGashaponWnd* pWnd = dynamic_cast<SelectGashaponWnd*>( g_GUIMgr.FindWnd( SELECT_GASHAPON_WND ) );
		if( pWnd )
			pWnd->SetResult( ReusltPresent );
	}
	else
	{
		switch( iResult )
		{
		case BUY_SELECT_GASHAPON_EXCEPTION:
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "예외 오류가 발생되었습니다." );
			}
			break;
		case BUY_SELECT_GASHAPON_BILLING_WAIT:
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "골드 처리중입니다.#잠시만 기다려 주세요." );
			}
			break;
		case BUY_SELECT_GASHAPON_EXCESS_MAX:
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "해당 상품을 구입하면#이번달 최대 골드 사용 한도를 초과합니다.#(구매 한도:청소년 55,000원, 성인 330,000원)# #다음달 부터 상품 구매가 가능합니다." );
			}
			break;
		case BUY_SELECT_GASHAPON_BILLING_WANT_OF_CASH:
			{
				g_GUIMgr.ShowWnd( BILLING_WANT_WND );
			}
			break;
		case BUY_SELECT_GASHAPON_BILLING_DISCONNECT:
			{
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "골드 처리가 불가능합니다.#잠시만 기다려 주세요." );	
			}
			break;
		case BUY_SELECT_GASHAPON_BILLING_FAIL:
			{
				bool bBillingError    = false;
				ioHashString  sBillingError;
				rkPacket >> bBillingError;
				if( bBillingError )
					rkPacket >> sBillingError;

				ioHashString sErrorMent = "골드 처리가 실패하였습니다.#관리자에게 문의하세요.";
				if( !bBillingError )
				{
					g_GUIMgr.SetMsgBox( MB_OK, NULL, sErrorMent.c_str() );
				}
				else
				{
					ioHashString sError = "Billing Error : ";
					ioLocalParent *pLocal = g_LocalMgr.GetLocal( ioLocalManager::GetLocalType() );
					if( pLocal ) 
						sError = pLocal->GetBillingErrorMent(sBillingError);

					char szErrorFullMent[MAX_PATH]="";
					ioChannelingNodeParent *pNode = g_ChannelingMgr.GetNode( g_MyInfo.GetChannelingType() );
					if( pNode && pNode->GetType() == CNT_NEXON )					
						StringCbPrintf( szErrorFullMent, sizeof( szErrorFullMent ), "%s",  sError.c_str() ); //Except Extracting Hangeul
					else
						StringCbPrintf( szErrorFullMent, sizeof( szErrorFullMent ), "%s#%s%s", sErrorMent.c_str(), sError.c_str(), sBillingError.c_str() ); //Except Extracting Hangeul

					g_GUIMgr.SetMsgBox( MB_OK, NULL, szErrorFullMent );
				}
			}
			break;
		case BUY_SELECT_GASHAPON_LIMITED_COUNT_OVER:	// 2020-03-26 가차 선택 제한
			{
				ioHashString sErrorMent = "처리가 실패하였습니다.#관리자에게 문의하세요.";
				g_GUIMgr.SetMsgBox( MB_OK, NULL, sErrorMent.c_str() );
			}
			break;
		}

		g_GUIMgr.HideWnd( SELECT_GASHAPON_WND );
	}	
}

void ioPlayStage::OnSelectGashaponsUseResult( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int iResult = 0;
	rkPacket >> iResult;

	if( iResult == EXTRA_SELECT_GASHAPON )
	{
		int iResult = 0;
		int iItemCode = 0;
		int iReinforce = 0;
		int iSlotIndex = 0;
		int iTradeType = 0;
		int iPeriodType = 0;
		int iValue1 = 0;
		int iValue2 = 0;
		DWORD dwMaleCustom = 0;
		DWORD dwFemaleCustom = 0;
		int iPeriodTime = 0;

		rkPacket >> iItemCode >> iReinforce >> iSlotIndex >> iTradeType >> iPeriodType >> iValue1 >> iValue2 >> dwMaleCustom >> dwFemaleCustom;
		rkPacket >> iPeriodTime;

		ioUserExtraItem *pUserExtraItem = g_MyInfo.GetUserExtraItem();
		if( pUserExtraItem )
		{
			EXTRAITEMSLOT kSlot;
			kSlot.m_iItemCode = iItemCode;
			kSlot.m_iReinforce = iReinforce;
			kSlot.m_iIndex = iSlotIndex;
			kSlot.m_iTradeState = iTradeType;
			kSlot.m_PeriodType = iPeriodType;
			kSlot.m_iValue1 = iValue1;
			kSlot.m_iValue2 = iValue2;
			kSlot.m_dwMaleCustom = dwMaleCustom;
			kSlot.m_dwFemaleCustom = dwFemaleCustom;

			pUserExtraItem->AddExtraItemData( kSlot );
		}

		SelectExtraGashaponWnd *pMainWnd = dynamic_cast<SelectExtraGashaponWnd*>(g_GUIMgr.FindWnd( SELECT_EXTRA_GASHAPON_WND ));
		if( pMainWnd )
			pMainWnd->SetResult( iItemCode );
	}
	else if( iResult == BESIDES_SELECT_GASHAPON )
	{
		GashaponPresent ReusltPresent;
		rkPacket >> ReusltPresent.m_iType;
		rkPacket >> ReusltPresent.m_iValue1;
		rkPacket >> ReusltPresent.m_iValue2;

		SelectGashaponWnd* pWnd = dynamic_cast<SelectGashaponWnd*>( g_GUIMgr.FindWnd( SELECT_GASHAPON_WND ) );
		if( pWnd )
			pWnd->SetResult( ReusltPresent );
	}
	else
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "예외 오류가 발생되었습니다." );
}

void ioPlayStage::OnBingoType( SP2Packet &rkPacket )
{
	ioBingo* pBingo = g_MyInfo.GetBingo();
	if( pBingo )
	{
		int iType = (int)ioBingo::BT_RAND;
		rkPacket >>iType;
		pBingo->SetBingoType( iType );
	}
}

void ioPlayStage::OnItemMaterialCompound( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	int eType;
	rkPacket >> eType;

	// HARDCODE : 성공 상황에서는 break가 아닌 return을 사용한다.
	switch( eType )
	{
	case ITEM_COMPOUND_OK:
		{
			int nEtcType;
			bool bSuccess;
			int nTargetSlot;
			int nTargetReinforce;
			WORD wFailExp;
			int nMaterialType;
			int nMaterialCnt;

			rkPacket >> nEtcType;
			rkPacket >> bSuccess;
			rkPacket >> nTargetSlot >> nTargetReinforce;
			rkPacket >> wFailExp;
			rkPacket >> nMaterialType >> nMaterialCnt;

			// HARDCODE : 갱신전 정보값 활용때문에 UI쪽을 먼저 진행.
			// UI 출력
			MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
			if( pInvenWnd )
			{
				pInvenWnd->OnItemMaterialCompoundResult( bSuccess, nTargetSlot, nTargetReinforce, nEtcType );
				pInvenWnd->UpdateTab( false, false );
			}

			__int64 iMoney;
			rkPacket >> iMoney;
			g_MyInfo.SetMoney( iMoney );

			ioUserExtraItem *pExtraItem = g_MyInfo.GetUserExtraItem();
			if( pExtraItem )
			{
				EXTRAITEMSLOT kTargetSlot;
				if( pExtraItem->GetExtraItem( nTargetSlot, kTargetSlot ) )
				{
					//이전 실패 경험치
					int nPreFailExp = (int)kTargetSlot.m_failExp;

					//강화 레벨 설정
					kTargetSlot.m_iReinforce = nTargetReinforce;

					//성공시 실패경험치를 0으로, 실패시 해당 경험치를 반영
					if ( bSuccess || nTargetReinforce >= g_CompoundMaterialMgr.GetFixedFailReinforce() )
						kTargetSlot.m_failExp = 0;
					else
						kTargetSlot.m_failExp = wFailExp;

					//결과에 적용
					if ( pInvenWnd )
						pInvenWnd->OnItemMaterialCompoundResultFailExp( nPreFailExp, (int)kTargetSlot.m_failExp, kTargetSlot.m_iItemCode );

					pExtraItem->SetExtraItem( kTargetSlot );
				}
				else
					LOG.PrintTimeAndLog( 0, "ioPlayStage::OnItemMaterialCompound - Target Error" );
			}
		}
		return;
	case ITEM_COMPOUND_ERROR:
		{
			int iErrorNum;
			rkPacket >> iErrorNum;

			g_GUIMgr.SetMsgBox( MB_OK, NULL, STR(1), iErrorNum );
		}
		break;
	}

	// UI 갱신
	MyInventoryWnd *pInvenWnd = dynamic_cast<MyInventoryWnd*>(g_GUIMgr.FindWnd(MY_INVENTORY_WND));
	if( pInvenWnd && pInvenWnd->IsShow() )
		pInvenWnd->UpdateTab( false, true );
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//펫 생성
ioPet* ioPlayStage::CreatePet( const ioHashString &szOwnerName, const D3DXVECTOR3 &vTargetPos, bool bAddList )
{
	//엔티티 생성
	ioPet *pPet = g_PetMgr.CreatePet( szOwnerName, vTargetPos );
	//엔티티 생성 실패
	if( !pPet )
	{
		LOG.PrintTimeAndLog( 0, "ioPet* ioPlayStage::CreatePet : Create Fail" );
		return NULL;
	}
	
	//엔티티에 바로 추가 확인
	if( bAddList )
		AddGameEntity( pPet );
	else
		m_ReserveAddEntityList.push_back( pPet );

	if( m_pPlayMode->GetModeType() == MT_ARENA )
	{
		pPet->SetForceHide( true );
	}

	return pPet;
}

//모든 펫 제거
void ioPlayStage::DestroyAllPet()
{
	GameEntityList::iterator iter = m_GameEntityList.begin();
	while ( iter != m_GameEntityList.end() )
	{
		ioPet *pPet = ToPet( *iter );
		if( pPet )
		{
			iter = EraseGameEntity( iter );
		}
		else
		{
			++iter;
		}
	}
}

//엔드 펫 제거(엔티티 리스트에서 삭제)
void ioPlayStage::DestroyDiePet()
{
	g_PetMgr.ClearDeadPet();

	GameEntityList::iterator iter = m_GameEntityList.begin();
	while ( iter != m_GameEntityList.end() )
	{
		ioPet *pPet = ToPet( *iter );
		if( pPet && !pPet->IsLive() )
			iter = EraseGameEntity( iter );
		else
			++iter;
	}
}

//펫 현재 정보 얻기
bool ioPlayStage::GetCurPetInfo( ioHashString szCharName, CurPetInfo &sCurPetInfo )
{
	return g_PetMgr.GetCurPetInfo( szCharName, sCurPetInfo );
}

void ioPlayStage::OnPlayingUserDataInfo( SP2Packet &rkPacket )
{
	int nState;
	rkPacket >> nState;

	switch ( nState )
	{
	case PLAYING_USER_INFO:
		{
			ioHashString szNewUser;
			rkPacket >> szNewUser;
			RemoveUser( szNewUser, true );
			OnJoinUserData( rkPacket );
			LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPlayingUserDataInfo - PLAYING_USER_INFO" );
		}
		break;
	case NOT_PLAYING_ROOM:
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPlayingUserDataInfo - NOT_PLAYING_ROOM" );
		break;
	case NOT_PLAYING_USER:
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnPlayingUserDataInfo - NOT_PLAYING_USER" );
		break;
	}
}

void ioPlayStage::OnPowerUpInfo( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );
	
	byte eTargetType = 0;
	rkPacket >> eTargetType;

	int iState = 0;
	rkPacket >> iState;	

	switch( iState )
	{
	case POWER_UP_SUCCESS:
		{
			ioHashString szName;			
			rkPacket >> szName;

			int iIndex = 0;
			rkPacket >> iIndex;

			//iCode가 용병의 경우 ClassType, 아이템의 경우 아이템 코드
			int iCode = 0;
			rkPacket >> iCode;

			byte eGrade = 0;
			rkPacket >> eGrade;

			g_PowerUpManager.SetPowerUp(eTargetType);

			if( eTargetType == PUTT_CHAR || eTargetType == PDTT_CHAR )
			{
				if( szName == g_MyInfo.GetPublicID() )
				{
					g_PowerUpManager.ApplyCharPowerUp( iIndex, iCode, eGrade );
				}
				else
				{
					g_UserInfoMgr.SetCharPowerUp( szName, iCode, eGrade );					

					ioBaseChar* pChar = GetBaseChar( szName );
					if( pChar && pChar->GetCharacterInfo().m_class_type == iCode )
					{
						pChar->ChangePowerUpCharGrade( eGrade );
					}
				}
			}
			else
			{
				g_PowerUpManager.ApplyItemPowerUp( iIndex, iCode );
			}
		}
		break;
	case POWER_UP_NO_TARGET:
	case POWER_DOWN_NO_TARGET:
		{
			if( eTargetType == PUTT_CHAR )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "강화 할 용병을 가지고 있지 않습니다." );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "강화 할 아이템을 가지고 있지 않습니다." );
		}
		break;
	case POWER_UP_DISABLE_TARGET:
	case POWER_DOWN_DISABLE_TARGET:
		{
			if( eTargetType == PUTT_CHAR )
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "강화 할 수 없는 용병입니다." );
			else
				g_GUIMgr.SetMsgBox( MB_OK, NULL, "강화 할 수 없는 아이템입니다." );
		}
		break;	
	case POWER_UP_MATERIAL_SHORTAGE:
	case POWER_DOWN_MATERIAL_SHORTAGE:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "소울 스톤이 부족합니다." );
		}
		break;
	default:
		{
			g_GUIMgr.SetMsgBox( MB_OK, NULL, "알 수없는 오류 : %s, State : %d", __FUNCTION__, iState );
		}
	}
	if(iState != POWER_UP_SUCCESS)
		LOG.PrintTimeAndLog( 0, "%s - EXCEPTION Value : %d ", __FUNCTION__, iState );
}

void ioPlayStage::OnObjectItemRelease( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "%s - %s Not Exist User", __FUNCTION__, szName.c_str() );
		return;
	}

	if( pChar->GetState() == CS_DIE )
		return;

	int iCreateIndex;
	int eState;
	DWORD dwItemCode;	
	rkPacket >> iCreateIndex;
	rkPacket >> dwItemCode;
	rkPacket >> eState;

	ioObjectItem* pObject = pChar->GetObject();
	if( pObject && pObject->GetItemCreateIndex() == iCreateIndex && pObject->GetItemCode() == dwItemCode )
	{
		pChar->OnReleaseObjectItem( eState );
	}
}

void ioPlayStage::OnLagAction( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
		return;

	if( pChar->GetState() == CS_DIE )
		return;

	g_RequestLagMgr.AddLagActionBuff( pChar );
}

void ioPlayStage::OnInitializeSpecialState( SP2Packet &rkPacket )
{
	ioHashString szName;
	int eState;
	rkPacket >> szName >> eState;
	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar || pChar->GetState() == eState )
		return;

	if( pChar->IsSystemState() )
		return;

	pChar->SetState( eState );
}

void ioPlayStage::OnSpecialStateSync( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
		return;

	ioSpecialStateBase* pBase = pChar->GetSpecialState();
	if( pBase )
		pBase->ApplyStateSync( rkPacket, pChar );
}

void ioPlayStage::OnTimeCashInfo( SP2Packet &rkPacket )
{
	int nResult;
	rkPacket >> nResult;

	if ( nResult == TIME_CASH_GIVE_SUCCESS )
	{
		int nAddCash, nCash, nChannelingCash;
		rkPacket >> nAddCash >> nCash >> nChannelingCash;

		g_MyInfo.SetCash( nCash );
		g_MyInfo.SetChannelingCash( nChannelingCash );

		PresentTimeCashAlarmWnd *pAlarmWnd = dynamic_cast<PresentTimeCashAlarmWnd*>(g_GUIMgr.FindWnd(PRESENT_TIME_CASH_ALARM_WND));
		if( pAlarmWnd )
			pAlarmWnd->ShowPresentTimeCashAlarmWnd( nAddCash );
	}
	else if ( nResult == TIME_CASH_ITEM_EXPIRE )
	{
		DWORD dwEtcItemCode;
		rkPacket >> dwEtcItemCode;

		PresentTimeCashReleaseWnd *pReleaseWnd = dynamic_cast<PresentTimeCashReleaseWnd*>(g_GUIMgr.FindWnd(PRESENT_TIME_CASH_RELEASE_WND));
		if( pReleaseWnd )
			pReleaseWnd->ShowPresentTimeCashReleaseWnd( dwEtcItemCode );
	}
}

void ioPlayStage::OnRaidCoinInfo( SP2Packet &rkPacket )
{
	byte bType;
	rkPacket >> bType;

	switch(bType)
	{
	case RAID_COIN_INFO_SYNC:
	case RAID_COIN_INFO_USE:
		{
			int nRaidCoinCnt;
			rkPacket >> nRaidCoinCnt;

			g_MyInfo.SetEtcRaidCoin( nRaidCoinCnt );
			g_BattleRoomMgr.SyncMonsterCoin();
		}
		break;
	}
}

void ioPlayStage::OnCharEquipAccessoryData( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar )
		return;

	pChar->DestroyAllAccessoryItem();

	DWORD dwItemIndex = 0;
	DWORD dwItemCode = 0;
	int   iRandomNum;
	int iCompoundCode = 0;
	int iCompoundValue = 0;
	for (int i = 0; i < MAX_ACC_INVENTORY ; i++)
	{
		rkPacket >> dwItemIndex;
		rkPacket >> dwItemCode;
		rkPacket >> iRandomNum;
		rkPacket >> iCompoundCode;
		rkPacket >> iCompoundValue;
		if( dwItemIndex > 0 && dwItemCode > 0 )
		{
			ioItem *pItem = CreateItem( dwItemCode );
			if( pItem )
			{
				pItem->ReLoadProperty();
				pItem->SetOwnerName( szName );
				pItem->SetItemCreateIndex( dwItemIndex );
				pItem->SetAccessoryRandomNum( iRandomNum );
				pItem->SetAccessoryCompoundInfo( iCompoundCode, iCompoundValue );
				ioItem *pPreItem = pChar->EquipItem( pItem, true );
				SAFEDELETE( pPreItem );

				pItem->Initialize( pChar, this );
			}
		}
	}

	pChar->RecalcStatusValue();
}

void ioPlayStage::OnBadPingFlagDrop( SP2Packet &rkPacket )
{
	if( !m_pPlayMode )	return;

	int iCreateIndex;
	DWORD dwItemCode;
	int iItemReinforce;
	DWORD dwItemMaleCustom, dwItemFemaleCustom;
	D3DXVECTOR3 vItemPos;

	ioHashString szDroper;
	ioHashString szOwnerName;

	rkPacket >> szDroper;
	ioBaseChar *pChar = GetBaseChar( szDroper );
	if( !pChar )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnBadPingFlagDrop - %s Not Exist User",
			szDroper.c_str() );
	}

	rkPacket >> dwItemCode >> iItemReinforce >> dwItemMaleCustom >> dwItemFemaleCustom >> iCreateIndex >> vItemPos >> szOwnerName;

	if( pChar )
	{
		ioItem *pDropItem = pChar->ReleaseItemExplicit( dwItemCode, iCreateIndex, ES_WEAR );
		if( !pDropItem )
			LOG.PrintTimeAndLog( 0, "ioPlayStage::OnBadPingFlagDrop - Flag Drop Fail!" );
		else
			SAFEDELETE( pDropItem );

		pChar->SetNetworkBadEmoticon();
	}

	ioItem *pItem = CreateItem( dwItemCode );
	if( !pItem )
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnBadPingFlagDrop - Flag Create Fail!" );
		return;
	}

	pItem->ReLoadProperty();
	pItem->SetItemCreateIndex( iCreateIndex );
	pItem->SetOwnerName( szOwnerName );
	pItem->SetItemReinforce( iItemReinforce );
	pItem->SetItemCustom( dwItemMaleCustom, dwItemFemaleCustom );

	ioFieldItem *pField = CreateFieldItem( pItem );
	if( pField )
	{
		pField->SetWorldPositionByServerSide( vItemPos );
		pField->SetDropState( pField->GetWorldPosition() );
		pField->CreateDropEffect();

		ioFlagCaptureMode *pFlagCapture = ToFlagCaptureMode( m_pPlayMode );
		if( pFlagCapture )
			pFlagCapture->FlagBadPingDrop( szDroper );
	}
	else
	{
		LOG.PrintTimeAndLog( 0, "ioPlayStage::OnBadPingFlagDrop - Flag Field Drop Fail!" );
	}
}

void ioPlayStage::OnArenaModeCharCreate( SP2Packet &rkPacket )
{
	int iCmd, iCount, iType, iSlot;
	DWORD dwIndex;
	bool bIsWaitMode;
	rkPacket >> iCmd >> iCount;

	g_MyInfo.SetArenaModeCharacter( MT_ARENA );
	for( int i = 0; i < iCount; i++ )
	{
		rkPacket >> iType >> iSlot >> dwIndex;

		g_MyInfo.SetArenaCharacterIndex( iType, dwIndex );
		g_MyInfo.SetArenaCharacterSlot( iType, iSlot );
	}

	MiniSoldierSelectWnd *pMiniSoldierWnd = dynamic_cast<MiniSoldierSelectWnd*>( g_GUIMgr.FindWnd( MINI_SOLDIER_SELECT_WND ));
	if( pMiniSoldierWnd )
	{
		pMiniSoldierWnd->SettingMiniSoldierBtn();
	}

	rkPacket >> bIsWaitMode;

	if( bIsWaitMode )
		g_MyInfo.SetArenaModeCharacter( MT_NONE );
}

void ioPlayStage::OnPassiveSkillExtraInfo( SP2Packet &rkPacket )
{
	ioHashString szName;
	rkPacket >> szName;

	ioBaseChar *pChar = GetBaseChar( szName );
	if( !pChar || !pChar->CheckGrapplingExceptionState() )
		return;

	ioHashString szSkillName;
	rkPacket >> szSkillName;

	pChar->ApplyPassiveSkillExtraInfo( szSkillName, rkPacket );
}

void ioPlayStage::OnSuccessionData( SP2Packet &rkPacket )
{
	int iMMR, iCur,iMax;
	rkPacket >> iMMR >> iCur >> iMax;
	g_MyInfo.SetSuccessionMMRPoint( iMMR );
	g_MyInfo.SetCurSuccessionCount( iCur );
	g_MyInfo.SetMaxSuccessionCount( iMax );

	UserInfoNode *pNode = g_UserInfoMgr.GetUserInfoNode( g_MyInfo.GetPublicID() );
	if( pNode )
	{
		pNode->SetCurSuccessionCount( iCur );
		pNode->SetMaxSuccessionCount( iMax );
	}
}

void ioPlayStage::OnSuccessionMatchingResult( SP2Packet &rkPacket )
{
	//매칭이 실패 했을 시에만 들어온다.
	int iResult;
	rkPacket >> iResult;

	if( iResult != MATCH_SUCCESS )
	{
		SuccessionBattleWnd *pSuccessionWnd = dynamic_cast<SuccessionBattleWnd*>(g_GUIMgr.FindWnd( SUCCESSION_BATTLE_WND ));
		if( pSuccessionWnd )
		{
			pSuccessionWnd->MatchingFail();
		}
	}
}

void ioPlayStage::OnSuccessionMatchingCancel( SP2Packet &rkPacket )
{
	TCPNetwork::MouseBusy( false );

	SuccessionBattleWnd *pSuccessionWnd = dynamic_cast<SuccessionBattleWnd*>(g_GUIMgr.FindWnd( SUCCESSION_BATTLE_WND ));
	if ( !pSuccessionWnd )
		return;

	pSuccessionWnd->MatchingCancel();

	int iResult;
	rkPacket >> iResult;
}

void ioPlayStage::OnSuccessionGameStart( SP2Packet &rkPacket )
{
	SuccessionBattleWnd *pSuccessionWnd = dynamic_cast<SuccessionBattleWnd*>(g_GUIMgr.FindWnd( SUCCESSION_BATTLE_WND ));
	if( pSuccessionWnd )
	{
		pSuccessionWnd->MatchingSuccess();
	}

	int iModeType = 0;
	int iSubNum = 0;
	int iMapNum = 0;
	bool bRevenge = false;
	rkPacket >> iModeType >> iSubNum >> iMapNum >> bRevenge;
	g_MyInfo.SetSuccessionRevengeMode( bRevenge );

	char szCall[MAX_PATH];
	sprintf_s( szCall, "%s - (%d), (%d), (%d)" ,__FUNCTION__, iModeType, iSubNum, iMapNum );

	g_App.SetNextStage( ioGameStage::GS_PLAY, szCall );
	g_App.SetModeType( iModeType, iSubNum, iMapNum, PT_NONE );

	// voice chat 초기화
	g_VoiceChat.RemoveAllSoundPlayer();
	g_VoiceOffListManager.DeleteAll();
	VoiceChatWnd *pWnd = dynamic_cast<VoiceChatWnd*>( g_GUIMgr.FindWnd(VOICE_CHAT_WND) );
	if(pWnd)
		pWnd->AllOtherVoiceOff();
}

void ioPlayStage::OnSuccessionGameEnd( SP2Packet &rkPacket )
{
	if( P2PNetwork::IsNetworkPlaying() )
	{
		SendOwnerDamageList();

		// 로비로 이동
		SP2Packet kPacket( CTPK_EXIT_ROOM );
		kPacket << EXIT_ROOM_WANT << 0 << true;
		TCPNetwork::SendToServer( kPacket );
		TCPNetwork::MouseBusy( true );
	}
	g_MyInfo.SetSuccessionRevengeMode( false );
}

void ioPlayStage::OnSuccessionRevengeRequest( SP2Packet &rkPacket )
{
	int iFlag;
	rkPacket >> iFlag;
	if ( iFlag == REVENGE_REQUEST )
	{
		WaitEffectWnd *pEffect = dynamic_cast<WaitEffectWnd*>( g_GUIMgr.FindWnd( WAIT_EFFECT_WND ) );
		if ( pEffect && pEffect->IsShow() )
			pEffect->HideWnd();

		g_GUIMgr.ShowWnd( SUCCESSION_BATTLE_REPLY_WND );
	}
	else if ( iFlag == REVENGE_REJECT )
	{
		g_GUIMgr.HideWnd( SUCCESSION_BATTLE_REQUEST_WND );
		g_GUIMgr.SetMsgBox( MB_OK, NULL, "상대방이 거절하였습니다." );
	}
}

void ioPlayStage::OnSuccessionOtherUserInfo( SP2Packet &rkPacket )
{
	SuccessionMatchVersusSceneWnd* pVersusScene = dynamic_cast<SuccessionMatchVersusSceneWnd*>(g_GUIMgr.FindWnd( SUCCESSION_BATTLE_VERSUS_SCENE_WND ));
	if ( pVersusScene )
	{
		int iOtherTeamType;
		rkPacket >> iOtherTeamType;
		pVersusScene->OtherHeroInfoUpdate( iOtherTeamType, rkPacket );
		pVersusScene->MyHeroInfoUpdate( iOtherTeamType );
	}
}

void ioPlayStage::OnPieceToPesoPacket( SP2Packet &rkPacket )
{
	int iCount, iPeso;
	rkPacket >> iCount >> iPeso;

	m_iPieceChangeCount = iCount;
	m_iPieceChangePeso = iPeso;
	AlchemicChangeAlarmWnd *pAlarmWnd = dynamic_cast<AlchemicChangeAlarmWnd*>( g_GUIMgr.FindWnd( ALCHEMIC_CHANGE_ALARM_WND ));
	if( pAlarmWnd )
		pAlarmWnd->SetInfo( iPeso, iCount );
}

void ioPlayStage::OnShareSkillGauge( SP2Packet &rkPacket )
{
	ioHashString szSkillOwner;
	int iItemType = 0;
	float fSkillGauge = 0.0;
	rkPacket >> szSkillOwner >> iItemType >> fSkillGauge;

	// 스킬 주인이 자신이 아니면 갱신
	if( szSkillOwner != g_MyInfo.GetPublicID() )
	{
		// 배틀모드이고 스킬 주인이 같은 팀이고 대기자이면 동기화 시킴
		ioBaseChar* pApplyChar = GetBaseChar(g_MyInfo.GetPublicID());
		if( pApplyChar && GetModeType() == MT_BATTLE )
		{
			// 같은 팀이고 난입하지않았으면 스킬 게이지 공유
			if( g_BattleRoomMgr.CheckSameTeam( szSkillOwner, g_MyInfo.GetPublicID() ) && !g_BattleRoomMgr.CheckUserEntryState( g_MyInfo.GetPublicID() ) )
			{
				ioItem *pItem = pApplyChar->GetEquipedItem( iItemType );
				if( pItem )
				{
					// 태그 게이지를 사용하는 패시브 스킬 초기화, 일반 스킬은 게이지 공유
					ioSkill *pSkill = pApplyChar->GetEquipedSkill( iItemType );
					ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pSkill );
					if( pPassiveSkill && pPassiveSkill->UsePassiveGauge() )
					{
						pPassiveSkill->SetCurPassiveGauge( 0.0f );
						pItem->SetCurSkillGauge( fSkillGauge );
					}
					else
						pItem->SetCurSkillGauge( fSkillGauge );
				}
			}
		}
	}
}

void ioPlayStage::ClearPieceToPesoInfo()
{
	m_iPieceChangeCount = 0;
	m_iPieceChangePeso = 0;
}

void ioPlayStage::GetPieceToPesoInfo( int &iPeso, int &iCount )
{
	iCount = m_iPieceChangeCount;
	iPeso = m_iPieceChangePeso;
}