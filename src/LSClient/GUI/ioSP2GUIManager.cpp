#include "StdAfx.h"
#include "../ioWndBoundary.h"
#include "../ioPlayStage.h"

#include "ChatMsgWnd.h"

#include "RunningManNameWnd.h"

#include "ioMessageBox4.h"
#include "ioExpansionBox.h"
#include "MiniMap.h"

#include "HelpWnd.h"

#include "BriefingWnd.h"
#include "UserListWnd.h"
#include "MemoListWnd.h"
#include "ChannelChatWnd.h"

#include "DisconnectWnd.h"
 
#include "HackProblemWnd.h"
#include "DebugInfoWnd.h"

#include "ResolutionList.h"
#include "FileListWnd.h"
#include "ModeMapSelectList.h"
#include "TotalResultRewardWnd.h"

#include "GradeUpBonusEtcitemWnd.h"
#include "GrowthDownSelectWnd.h"
#include "CenterHelpWnd.h"
#include "ioExtraItemListWnd.h"
#include "ioExtraItemReleaseWnd.h"
#include "ioTimeGrowthEndWnd.h"

#include "FirstIDChangeWnd.h"
#include "MonsterCoinWnd.h"
#include "BossMonsterGaugeWnd.h"
#include "PlazaNPCGaugeWnd.h"
#include "GashaponWnd.h"
#include "gashaponpresentlistwnd.h"
#include "guildmarkchangewnd.h"
#include "RandomDecoWnd.h"
#include "RandomDecoSelectWnd.h"
#include "ioNewMedalRandomBox.h"

#include "ItemMultipleCompoundWnd.h"
#include "ItemMultipleCompoundResultWnd.h"
#include "ioDecoItemListWnd.h"
#include "ExcavationWnd.h"
#include "ExcavationResultWnd.h"
#include "ExcavationShovelWnd.h"
#include "ExcavationLevelUpWnd.h"
#include "QuestWnd.h"
#include "GlobalWnd.h"
#include "NewShopBuyWnd.h"
#include "NewShopBoughtWnd.h"
#include "NewShopWnd.h"
#include "SmallTabButton.h"
#include "NewShopIconBtn.h"
#include "NewShopItemInfoWnd.h"
#include "NewShopToolTip.h"
#include "PracticeToolTip.h"
#include "MyInventoryWnd.h"
#include "MyInventoryInfoWnd.h"
#include "SoldierPracticeWnd.h"
#include "SoldierDisplayWnd.h"
#include "ItemResellWnd.h"
#include "ChangeNameWnd.h"
#include "RecordInitWnd.h"
#include "UISoldierSelectWnd.h"
#include "HeadquartersMainWnd.h"
#include "ExpandMedalSlotWnd.h"
#include "ExpandMedalSlotResultWnd.h"
#include "ioExMedalSlotReleaseWnd.h"

#include "TradeMainWnd.h"
#include "ChangeTradeStateWnd.h"
#include "TradeItemRegisterWnd.h"
#include "gameoptionsettingkeyinput.h"
#include "ItemCustomWnd.h"
#include "ItemCustomTrainWnd.h"
#include "firstsoldierselectwnd.h"
#include "FightClubWnd.h"
#include "BattleModeWnd.h"
#include "TowerDefWnd.h"
#include "TournamentMainWnd.h"
#include "TournamentDetailMainWnd.h"
#include "CustomTournamentCreateWnd.h"
#include "CustomTournamentWnd.h"
#include "UsePresentEtcItemWnd.h"
#include "ItemRechargeWnd.h"

#include "QuickModeToolTip.h"

#include "MessengerWnd.h"

#include "AlchemicFuncWnd.h"
#include "EtcItemSlotWnd.h"

#include "AniTextWnd.h"

#include "../ioBrowserManager.h"
#include "../ioUIGuide.h"

#include "invensoldierselectwnd.h"
#include "InvenFourExtraCompoundWnd.h"
#include "PCRoomLocalBenefitWnd.h"
#include "SelectExtraGashaponWnd.h"

#include "CloverToolTip.h"

#include "RouletteWnd.h"
#include "RouletteHelpWnd.h"

#include "BingoWnd.h"
#include "BingoRightWnd.h"
#include "BingoHelpToolTip.h"
#include "BingoToolTip.h"
#include "BingoMsgBox.h"
#include "ioBingoRewardWnd.h"

#include "RelativeGradeBonusWnd.h"
#include "SuperGashaponWnd.h"
#include "MultiSuperGashaponWnd.h"

#include "TimeGateSearchShuffle.h"
#include "ShuffleBonusResultWnd.h"

#include "ExPCRoomWnd.h"
#include "AttendanceWnd.h"
#include "SelectGashaponWnd.h"

#include "ioGUIRegister.h"
#include "ShuffleRankWnd.h"

#include "CampMainWnd.h"
#include "PlazaMainWnd.h"
#include "RoundStateMainWnd.h"
#include "LobbyMainWnd.h"
#include "ItemMaterialCompoundWnd.h"
#include "ItemMaterialCompoundResultWnd.h"
#include "SelectMaterialItemBtn.h"

#include "PetWnd.h"
#include "PetToolTip.h"
#include "PetViewWnd.h"
#include "PetIconBtn.h"
#include "PetResellWnd.h"
#include "PetEggResultWnd.h"
#include "PetEatWnd.h"
#include "SelectPetTargetBtn.h"
#include "PetCompoundWnd.h"
#include "PetResellResultWnd.h"
#include "PetEmptyWnd.h"
#include "ShopItemMovieWnd.h"
#include "RankBattleWnd.h"
#include "PartyMenuWnd.h"
#include "TimeGateSearch.h"
#include "PartyVersusWnd.h"
#include "ChatModeWnd.h"
#include "VoiceChatWnd.h"
#include "CenterInfoWnd.h"
#include "SoldierLimitWnd.h"
#include "SoldierExerciseWnd.h"
#include "SoldierSelectWnd.h"
#include "MiniSoldierSelectWnd.h"
#include "GameOptionWnd.h"
#include "TutorialWnd.h"
#include "ioGrowthSelectWnd.h"
#include "ioFishingMessageBox.h"
#include "ioObserverWnd.h"
#include "ExtendedButton.h"
#include "AbuseProblemWnd.h"
#include "GameOptionSettingTab.h"
#include "GameOptionSettingJoyKeyTab.h"
#include "UserInfoToolTip.h"
#include "MannerValuationWnd.h"
#include "KickUserVoteWnd.h"
#include "ModeRankWnd.h"
#include "EventWnd.h"
#include "GuildWnd.h"
#include "HeroMainWnd.h"
#include "LadderTeamWnd.h"
#include "PresentWnd.h"
#include "PCRoomWnd.h"
#include "IDChangeWnd.h"
#include "ItemCompoundResultWnd.h"
#include "GrowthDownDoneWnd.h"
#include "ioExtraItemRandomBox.h"
#include "FishingInventoryWnd.h"
#include "SendPresentWnd.h"
#include "TotalResultMainWnd.h"
#include "TimeScoreWnd.h"
#include "SkillListWnd.h"
#include "FireBulletWnd.h"
#include "ioPullDownList.h"
#include "ioPullDownItemFactory.h"
#include "ioBasicToolTip.h"
#include "ioAwakeReleaseWnd.h"
#include "ReinforceSelectWnd.h"
#include "HeroReinforceDownWnd.h"
#include "ItemReinforceDownWnd.h"
#include "PowerUpSelectWnd.h"
#include "GradeAdjustmentWnd.h"
#include "EnablePowerupListWnd.h"
#include "MyItemSelecteWnd.h"
#include "CostumeCustomWnd.h"
#include "ioCostumeRandomBox.h"
#include "ioCostumeListWnd.h"
#include "ioCostumeReleaseWnd.h"
#include "MissionWnd.h"
#include "RollBookWnd.h"
#include "GuildRewardWnd.h"
#include "PopUpStoreWnd.h"
#include "ADTextPanelWnd.h"
#include "ADImagePanelWnd.h"
#include "NewShopStartWnd.h"
#include "NewShopSearchWnd.h"
#include "BillingWantWnd.h"
#include "CenterKillInfoWnd.h"
#include "NoviceGradeBuffBtn.h"
#include "InventorySelectItemWnd.h"
#include "ItemNewMultipleCompoundWnd.h"
#include "NewShopItemPurchaseLeedWnd.h"
#include "TrainingFuncMenuWnd.h"
#include "TrainingFuncWnd.h"
#include "ioHousingBlockSetupWnd.h"
#include "ioHousingBlockInvenWnd.h"
#include "GuildHomeAdminInfoListWnd.h"
#include "GuildHQEtcItemNeedPopup.h"
#include "HomeEtcItemNeedPopup.h"
#include "HouseMainWnd.h"
#include "NamedTitleBtn.h"
#include "NamedTitleResultWnd.h"
#include "NamedTitlePremiumSelectWnd.h"
#include "NamedTitleSelectWnd.h"
#include "PriateRulletWnd.h"
#include "BonusCashWnd.h"
#include "AccessorySkillListWnd.h"
#include "ioAccessoryReleseWnd.h"
#include "PriateRulletRenewalWnd.h"
#include "ioSuccessionModeWnd.h"
#include "AlchemicChangeAlarmWnd.h"
#include "AccessoryFuncWnd.h"
#include "ExtraItemFuncWnd.h"
#include "MatchingGameWnd.h"
#include "DiceGameWnd.h"

#include "CountUpWnd.h"
#include "StartingSignalWnd.h"
#include "Practice/PracticeInfoWnd.h"
#include "Practice/PracticeResultWnd.h"
#include "PracticemainWnd.h"

#include "NewSearchWnd.h"	//2019-05-21

template<> ioSP2GUIManager* Singleton< ioSP2GUIManager >::ms_Singleton = 0;

ioSP2GUIManager::ioSP2GUIManager()
{
	m_pMsgReturn = NULL;
	memset( m_szMsgBuf, 0, MSG_BUF_SIZE );
	
	m_iPenaltyPeso = 0;
	m_iPenaltyLadderPoint = 0;
	m_dwReserveShowUI = 0;
	m_bSkipUIByBlind = false;
	m_bNoneUIMode = false;
	m_bEnableShopSound = false;
	m_pModalBackGround = NULL;
	m_pPlayStage = NULL;
	m_pPullDownFactory = NULL;
}

ioSP2GUIManager::~ioSP2GUIManager()
{
	WindowCreateTable::iterator iter;
	for( iter=m_CreateTable.begin() ; iter!=m_CreateTable.end() ; ++iter )
	{
		delete iter->second;
	}
	m_CreateTable.clear();

	SAFEDELETE( m_pModalBackGround );
}

void ioSP2GUIManager::InitManager()
{
	ioGUIManager::InitManager();

	LoadImageSetFile( "XML/UIImageSet.xml" );
	LoadFrameFile( "XML/DefaultUIFrame.xml" );

	RegisterWndCreator();

	m_pModalBackGround = g_UIImageSetMgr.CreateImage( "BaseImage001", "strip_black" );
	m_pPullDownFactory = new ioPullDownItemFactory;
}

void ioSP2GUIManager::ClearAllGUI()
{
	ErrorReport::SetPosition( 1300, 1 );
	SAFEDELETE( m_pDeskTopWnd );

	ErrorReport::SetPosition( 1300, 2 );
	ClearPreState();

	ErrorReport::SetPosition( 1300, 3 );
	ClearDragState();

	ErrorReport::SetPosition( 1300, 4 );
	DestroyBoundary();
}

void ioSP2GUIManager::RegisterWndCreator()
{
	m_CreateTable["TOOLTIP_BASE"] = new WindowCreator<ToolTip_Base>;

	m_CreateTable["LobbyMainWnd"]    = new WindowCreator<LobbyMainWnd>;
	m_CreateTable["LobbyWnd"]        = new WindowCreator<LobbyWnd>;
	m_CreateTable["LobbyBtn"]		 = new WindowCreator<LobbyBtn>;
	m_CreateTable["LobbyChatWnd"]    = new WindowCreator<LobbyChatWnd>;
	m_CreateTable["LobbyBattleInfoWnd"]      = new WindowCreator<LobbyBattleInfoWnd>;
	m_CreateTable["TimeGateBtn"]      = new WindowCreator<TimeGateBtn>;
	m_CreateTable["TimeGateResultWnd"]      = new WindowCreator<TimeGateResultWnd>;
	m_CreateTable["LobbyCampInfoWnd"]       = new WindowCreator<LobbyCampInfoWnd>;
	m_CreateTable["LobbyPlazaInfoWnd"]      = new WindowCreator<LobbyPlazaInfoWnd>;
	m_CreateTable["IconBtn"]		 = new WindowCreator<IconBtn>;

	m_CreateTable["MySoldierWnd"]    = new WindowCreator<MySoldierWnd>;
	m_CreateTable["MSCharacterWnd"]  = new WindowCreator<MSCharacterWnd>;
	m_CreateTable["MSExpWnd"]        = new WindowCreator<MSExpWnd>;
	m_CreateTable["MSCrossWnd"]      = new WindowCreator<MSCrossWnd>;
	m_CreateTable["HelpWnd"]         = new WindowCreator<HelpWnd>;
	m_CreateTable["MenuBase"]         = new WindowCreator<MenuBase>;
	m_CreateTable["MenuBtn"]         = new WindowCreator<MenuBtn>;
	m_CreateTable["MessengerBtn"]    = new WindowCreator<MessengerBtn>;
	m_CreateTable["PlazaStateBtn"]   = new WindowCreator<PlazaStateBtn>;
	m_CreateTable["TimeGateSearch"]  = new WindowCreator<TimeGateSearch>;
	m_CreateTable["TimeGateSearchResult"] = new WindowCreator<TimeGateSearchResult>;

	m_CreateTable["PartyCreateWnd"]   = new WindowCreator<PartyCreateWnd>;
	m_CreateTable["PartyPasswordWnd"] = new WindowCreator<PartyPasswordWnd>;

	m_CreateTable["PartyOwnerWnd"]     = new WindowCreator<PartyOwnerWnd>;
	m_CreateTable["PartyInfoBtn"]      = new WindowCreator<PartyInfoBtn>;
	m_CreateTable["PartyInfoWnd"]      = new WindowCreator<PartyInfoWnd>;
	m_CreateTable["PlazaInfoWnd"]      = new WindowCreator<PlazaInfoWnd>;
	m_CreateTable["PartyMemberList"]   = new WindowCreator<PartyMemberList>;
	m_CreateTable["PartyModeNMapBtn"]  = new WindowCreator<PartyModeNMapBtn>;
	m_CreateTable["PartyInviteList"]   = new WindowCreator<PartyInviteList>;
	m_CreateTable["PartyInviteListWnd"]= new WindowCreator<PartyInviteListWnd>;
	m_CreateTable["PartyOptionWnd"]= new WindowCreator<PartyOptionWnd>;
	m_CreateTable["PartyOptionBtn"]= new WindowCreator<PartyOptionBtn>;

	m_CreateTable["ActiveScreenBtn"]   = new WindowCreator<ActiveScreenBtn>;
	m_CreateTable["PartyShamBattleWnd"] = new WindowCreator<PartyShamBattleWnd>;
	m_CreateTable["PartyBattleSimpleWnd"] = new WindowCreator<PartyBattleSimpleWnd>;
	m_CreateTable["PartyBattleSimpleBtn"] = new WindowCreator<PartyBattleSimpleBtn>;
	m_CreateTable["PartyObserverBtn"] = new WindowCreator<PartyObserverBtn>;
	m_CreateTable["PartyObserverWnd"] = new WindowCreator<PartyObserverWnd>;	
	m_CreateTable["BattleUserOptionWnd"]= new WindowCreator<BattleUserOptionWnd>;
	m_CreateTable["BattleChatWnd"]      = new WindowCreator<BattleChatWnd>;
	m_CreateTable["MiddleJoinDelayWnd"] = new WindowCreator<MiddleJoinDelayWnd>;
	m_CreateTable["PartyCatalogueBtn"]  = new WindowCreator<PartyCatalogueBtn>;
	m_CreateTable["PartyQuickJoinBtn"]  = new WindowCreator<PartyQuickJoinBtn>;
	m_CreateTable["PartyCatalogueWnd"]  = new WindowCreator<PartyCatalogueWnd>;
	m_CreateTable["PartyInvitedBtn"]    = new WindowCreator<PartyInvitedBtn>;
	m_CreateTable["PartyInvitedWnd"]    = new WindowCreator<PartyInvitedWnd>;
	m_CreateTable["BattleQuickStartPlayerOption"] = new WindowCreator<BattleQuickStartPlayerOption>;
	m_CreateTable["BattleQuickStartOption"] = new WindowCreator<BattleQuickStartOption>;

	m_CreateTable["SkillListWnd"]	 = new WindowCreator<SkillListWnd>;
	m_CreateTable["SkillListBtn"]    = new WindowCreator<SkillListBtn>;
	m_CreateTable["SkillCharIcon"]    = new WindowCreator<SkillCharIcon>;
	m_CreateTable["SkillCharBtn"]    = new WindowCreator<SkillCharBtn>;

	m_CreateTable["FireBulletWnd"]	 = new WindowCreator<FireBulletWnd>;

	m_CreateTable["ChatModeWnd"]	= new WindowCreator<ChatModeWnd>;

	m_CreateTable["GameChatInput"]	 = new WindowCreator<GameChatInput>;
	m_CreateTable["ChatMsgWnd"]		 = new WindowCreator<ChatMsgWnd>;
	m_CreateTable["KOChatMsgWnd"]    = new WindowCreator<KOChatMsgWnd>;

	m_CreateTable["VoiceChatItem"]	 = new WindowCreator<VoiceChatItem>;

	m_CreateTable["CenterInfoWnd"] = new WindowCreator<CenterInfoWnd>;

	m_CreateTable["VoiceChatWnd"]	     = new WindowCreator<VoiceChatWnd>;
	m_CreateTable["VoiceChatOptionWnd"]	 = new WindowCreator<VoiceChatOptionWnd>;
	m_CreateTable["VoiceChatOptionBtn"]  = new WindowCreator<VoiceChatOptionBtn>;

	m_CreateTable["SoldierLimitPopupWnd"] = new WindowCreator<SoldierLimitPopupWnd>;
	m_CreateTable["RentalSoldierLimitPopupWnd"] = new WindowCreator<RentalSoldierLimitPopupWnd>;

	m_CreateTable["SoldierExerciseBtn"]	 = new WindowCreator<SoldierExerciseBtn>;

	m_CreateTable["CrossWnd"]        = new WindowCreator<CrossWnd>;
	m_CreateTable["CharOverWnd"]	 = new WindowCreator<CharOverWnd>;
	m_CreateTable["MiniMap"]		 = new WindowCreator<MiniMap>;

	m_CreateTable["MyInventoryWnd"]		= new WindowCreator<MyInventoryWnd>;
	m_CreateTable["InventoryIconBtn"]	= new WindowCreator<InventoryIconBtn>;
	m_CreateTable["InventoryEtcMotionBtn"] = new WindowCreator<InventoryEtcMotionBtn>;
	m_CreateTable["InventoryPresentBtn"] = new WindowCreator<InventoryPresentBtn>;
	m_CreateTable["InventoryItemWnd"]	= new WindowCreator<InventoryItemWnd>;
	m_CreateTable["GrowthSlotBtn"]		= new WindowCreator<GrowthSlotBtn>;
	m_CreateTable["SlotIconBtn"]		= new WindowCreator<SlotIconBtn>;
	m_CreateTable["InventorySoldierItemWnd"] = new WindowCreator<InventorySoldierItemWnd>;
	m_CreateTable["MyInventoryToolTip"] = new WindowCreator<MyInventoryToolTip>;
	m_CreateTable["ItemResellWnd"]		= new WindowCreator<ItemResellWnd>;

	m_CreateTable["MyInventoryInfoWnd"]	= new WindowCreator<MyInventoryInfoWnd>;
	m_CreateTable["ItemRecvSellInfoWnd"] = new WindowCreator<ItemRecvSellInfoWnd>;
	m_CreateTable["SoldierDismissalWnd"] = new WindowCreator<SoldierDismissalWnd>;
	m_CreateTable["SoldierSelectWnd"]       = new WindowCreator<SoldierSelectWnd>;
	m_CreateTable["SoldierBtn"]				= new WindowCreator<SoldierBtn>;
	m_CreateTable["SoldierChar"]			= new WindowCreator<SoldierChar>;
	m_CreateTable["SoldierInfo"]			= new WindowCreator<SoldierInfo>;
	m_CreateTable["SoldierChargeBtn"]       = new WindowCreator<SoldierChargeBtn>;
	m_CreateTable["SoldierRentalBtn"]		= new WindowCreator<SoldierRentalBtn>;

	m_CreateTable["SpiritItemWnd"]			= new WindowCreator<SpiritItemWnd>;
	m_CreateTable["SpiritFuncWnd"]		= new WindowCreator<SpiritFuncWnd>;
	m_CreateTable["SpiritComposeResultWnd"]		= new WindowCreator<SpiritComposeResultWnd>;
	m_CreateTable["SpiritConversionResultWnd"]		= new WindowCreator<SpiritConversionResultWnd>;

	m_CreateTable["AlchemicItemToolTip"] = new WindowCreator<AlchemicItemToolTip>;
	
	m_CreateTable["AccessoryFuncWnd"]		= new WindowCreator<AccessoryFuncWnd>;
	m_CreateTable["ExtraItemFuncWnd"]		= new WindowCreator<ExtraItemFuncWnd>;

	// 용병교체UI 작업 (2012.3.16)
	m_CreateTable["MiniSoldierSelectWnd"]       = new WindowCreator<MiniSoldierSelectWnd>;
	m_CreateTable["MiniSoldierBtn"]				= new WindowCreator<MiniSoldierBtn>;
	m_CreateTable["CharChangeGauge"]			= new WindowCreator<CharChangeGauge>;
	m_CreateTable["CharChangeGaugeHelp"]			= new WindowCreator<CharChangeGaugeHelp>;

	m_CreateTable["BackgroundChangeBtn"]    = new WindowCreator<BackgroundChangeBtn>;
	m_CreateTable["BackgroundChangeWnd"]    = new WindowCreator<BackgroundChangeWnd>;
	m_CreateTable["GameOptionWnd"]			= new WindowCreator<GameOptionWnd>;
	m_CreateTable["TutorialWnd"]            = new WindowCreator<TutorialWnd>;
	m_CreateTable["TutorialBtn"]            = new WindowCreator<TutorialBtn>;
	m_CreateTable["TutorialKeyboardWnd"]	= new WindowCreator<TutorialKeyboardWnd>;
	m_CreateTable["PracticeHelpWnd"]        = new WindowCreator<PracticeHelpWnd>;
	m_CreateTable["SoldierPracticeSelectBtn"] = new WindowCreator<SoldierPracticeSelectBtn>;
	m_CreateTable["SoldierPracticeSelectWnd"] = new WindowCreator<SoldierPracticeSelectWnd>;

	m_CreateTable["SoldierDisplaySelectBtn"] = new WindowCreator<SoldierDisplaySelectBtn>;
	m_CreateTable["SoldierDisplaySelectWnd"] = new WindowCreator<SoldierDisplaySelectWnd>;
	m_CreateTable["SoldierDisplayAlarmWnd"]  = new WindowCreator<SoldierDisplayAlarmWnd>;
	m_CreateTable["SoldierDisplayChangeWnd"] = new WindowCreator<SoldierDisplayChangeWnd>;

	m_CreateTable["BriefingWnd"]            = new WindowCreator<BriefingWnd>;

	m_CreateTable["ioMessageBoxBtn"]		= new WindowCreator<ioMessageBoxBtn>;
	m_CreateTable["ioMessageBox"]			= new WindowCreator<ioMessageBox>;
	m_CreateTable["ioMessageBox3"]			= new WindowCreator<ioMessageBox3>;
	m_CreateTable["ioMessageBox4"]			= new WindowCreator<ioMessageBox4>;
	m_CreateTable["ioMessageBox4Btn"]			= new WindowCreator<ioMessageBox4Btn>;

	m_CreateTable["ioExitMsgBox"]           = new WindowCreator<ioExitMsgBox>;
	m_CreateTable["ioBattleRoomExitMsgBox"] = new WindowCreator<ioBattleRoomExitMsgBox>;
	m_CreateTable["ioBattleRoomExitMsgBoxBtn"] = new WindowCreator< ioBattleRoomExitMsgBoxBtn >;
	m_CreateTable["ioBattleRoomExitPopup"]     = new WindowCreator< ioBattleRoomExitPopup >;
	m_CreateTable["ioBattleRoomExitRoomListWnd"]  = new WindowCreator< ioBattleRoomExitRoomListWnd >;

	m_CreateTable["ioGrowthSelectWnd"]		= new WindowCreator<ioGrowthSelectWnd>;

	m_CreateTable["FishingAutoSellQuestion"] = new WindowCreator<FishingAutoSellQuestion>;
	m_CreateTable["ioFishingMessageBox"]	 = new WindowCreator<ioFishingMessageBox>;
	m_CreateTable["ioFishingLevelUpWnd"]	 = new WindowCreator<ioFishingLevelUpWnd>;
	m_CreateTable["ioFishingItemMessageBox"] = new WindowCreator<ioFishingItemMessageBox>;
	m_CreateTable["FishingDropZoneInduceWnd"]= new WindowCreator<FishingDropZoneInduceWnd>;

	m_CreateTable["ManualWnd"]				= new WindowCreator<ManualWnd>;
	m_CreateTable["ManualListWnd"]			= new WindowCreator<ManualListWnd>;

	m_CreateTable["ioObserverRedWnd"]		= new WindowCreator<ioObserverRedWnd>;
	m_CreateTable["ioObserverBlueWnd"]		= new WindowCreator<ioObserverBlueWnd>;
	m_CreateTable["ioObserverInfoWnd"]		= new WindowCreator<ioObserverInfoWnd>;

	m_CreateTable["TwinkleButton"]          = new WindowCreator<TwinkleButton>;

	m_CreateTable["GameOptionSpecTab"]      = new WindowCreator<GameOptionSpecTab>;
	m_CreateTable["GameSpecRadioBtn"]       = new WindowCreator<GameSpecRadioBtn>;
	m_CreateTable["GameOptionVolumeTab"]    = new WindowCreator<GameOptionVolumeTab>;

	m_CreateTable["MessengerWnd"]           = new WindowCreator<MessengerWnd>;
	m_CreateTable["MessengerListBtn"]		= new WindowCreator<MessengerListBtn>;
	m_CreateTable["UserListWnd"]            = new WindowCreator<UserListWnd>;
	m_CreateTable["CloverButton"]			= new WindowCreator<CloverButton>;
	m_CreateTable["CloverToolTip"]			= new WindowCreator<CloverToolTip>;
	m_CreateTable["UserInfoListWnd"]        = new WindowCreator<UserInfoListWnd>;
	m_CreateTable["UserInfoListOptionWnd"]  = new WindowCreator<UserInfoListOptionWnd>;
	m_CreateTable["MemoListWnd"]            = new WindowCreator<MemoListWnd>;
	m_CreateTable["MemoInfoListWnd"]        = new WindowCreator<MemoInfoListWnd>;
	m_CreateTable["ChannelChatWnd"]         = new WindowCreator<ChannelChatWnd>;
	m_CreateTable["GuildChatWnd"]           = new WindowCreator<GuildChatWnd>;
	m_CreateTable["MessengerPopup"]         = new WindowCreator<MessengerPopup>;
	m_CreateTable["FriendRequestWnd"]       = new WindowCreator<FriendRequestWnd>;
	m_CreateTable["FriendApplicationWnd"]   = new WindowCreator<FriendApplicationWnd>;
	m_CreateTable["FriendDeleteWnd"]        = new WindowCreator<FriendDeleteWnd>;
	m_CreateTable["BlackListInsertWnd"]     = new WindowCreator<BlackListInsertWnd>;
	m_CreateTable["ChannelInviteWnd"]       = new WindowCreator<ChannelInviteWnd>;

	//UJ 140120, 보안강화: 밴/디스커넥 방지 치트 대응
	_ENCSTR( "DisconnectWnd", szEncDisconnectWnd );
	_ENCSTR( "HackProblemWnd", szEncHackProblemWnd );
	_ENCSTR( "AbuseProblemWnd", szEncAbuseProblemWnd );
	_ENCSTR( "LobbyLockProblemWnd", szEncLobbyLockProblemWnd );
	_ENCSTR( "BattleLockProblemWnd", szEncBattleLockProblemWnd );
	m_CreateTable[szEncDisconnectWnd]			= new WindowCreator<DisconnectWnd>;
	m_CreateTable[szEncHackProblemWnd]			= new WindowCreator<HackProblemWnd>;
	m_CreateTable[szEncAbuseProblemWnd]			= new WindowCreator<AbuseProblemWnd>;
	m_CreateTable[szEncLobbyLockProblemWnd]     = new WindowCreator<LobbyLockProblemWnd>;
	m_CreateTable[szEncBattleLockProblemWnd]    = new WindowCreator<BattleLockProblemWnd>;

	m_CreateTable["GameOptionSettingTab"]   = new WindowCreator<GameOptionSettingTab>;
	m_CreateTable["GameOptionSettingGameTab"]   = new WindowCreator<GameOptionSettingGameTab>;
	m_CreateTable["GameOptionSettingKeyTab"]    = new WindowCreator<GameOptionSettingKeyTab>;
	m_CreateTable["GameOptionMovieScreenShotTab"]= new WindowCreator<GameOptionMovieScreenShotTab>;
	m_CreateTable["SettingBtn"]                 = new WindowCreator<SettingBtn>;
	m_CreateTable["SettingKeyItemWnd"]          = new WindowCreator<SettingKeyItemWnd>;
	m_CreateTable["SettingMacroItemBtn"]        = new WindowCreator<SettingMacroItemBtn>;
	m_CreateTable["SettingMacroItemWnd"]        = new WindowCreator<SettingMacroItemWnd>;
	m_CreateTable["MacroEdit"]                  = new WindowCreator<MacroEdit>;
	m_CreateTable["UserInfoToolTip"]			= new WindowCreator<UserInfoToolTip>;

	m_CreateTable["SettingJoyBtn"]				= new WindowCreator<SettingJoyBtn>;
	m_CreateTable["SettingJoyKeyItemWnd"]		= new WindowCreator<SettingJoyKeyItemWnd>;
	m_CreateTable["GameOptionSettingJoyKeyTab"]	= new WindowCreator<GameOptionSettingJoyKeyTab>;

	m_CreateTable["DeveloperInfoWnd"]			= new WindowCreator<DeveloperInfoWnd>;
	m_CreateTable["DeveloperP2PWnd"]            = new WindowCreator<DeveloperP2PWnd>;
	m_CreateTable["DeveloperFrameWnd"]			= new WindowCreator<DeveloperFrameWnd>;
	m_CreateTable["FontTextureTestWnd"]         = new WindowCreator<FontTextureTestWnd>;
	m_CreateTable["CharGameInfoListWnd"]         = new WindowCreator<CharGameInfoListWnd>;

	m_CreateTable["ResolutionList"]		= new WindowCreator<ResolutionList>;
	m_CreateTable["StringSelectList"]	= new WindowCreator<StringSelectList>;
	m_CreateTable["ResolutionConfirm"]	= new WindowCreator<ResolutionConfirm>;
	m_CreateTable["MovieResolutionList"]= new WindowCreator<MovieResolutionList>;
	m_CreateTable["ModeMapSelectList"]  = new WindowCreator<ModeMapSelectList>;
	m_CreateTable["CreateModeSelectList"] = new WindowCreator<CreateModeSelectList>;
	m_CreateTable["RoomOptionTypeList"]= new WindowCreator<RoomOptionTypeList>;

	m_CreateTable["MannerValuationExtensionWnd"] = new WindowCreator<MannerValuationExtensionWnd>;
	m_CreateTable["MannerValuationSlotBtn"] = new WindowCreator<MannerValuationSlotBtn>;
	m_CreateTable["MannerValuationLeaderSlotWnd"] = new WindowCreator<MannerValuationLeaderSlotWnd>;
	m_CreateTable["MannerValuationControlWnd"] = new WindowCreator<MannerValuationControlWnd>;
	m_CreateTable["MannerValuationChatWnd"] = new WindowCreator<MannerValuationChatWnd>;
	m_CreateTable["MannerValuationSlotTooltip"] = new WindowCreator<MannerValuationSlotTooltip>;
	
	m_CreateTable["KickVoteReasonEdit"] = new WindowCreator<KickVoteReasonEdit>;
	m_CreateTable["KickVoteReasonWnd"]  = new WindowCreator<KickVoteReasonWnd>;
	m_CreateTable["KickVoteProgressWnd"]= new WindowCreator<KickVoteProgressWnd>;

	m_CreateTable["PlazaListBtn"] = new WindowCreator<PlazaListBtn>;
	m_CreateTable["PlazaListWnd"] = new WindowCreator<PlazaListWnd>;
	m_CreateTable["PlazaPasswordWnd"] = new WindowCreator<PlazaPasswordWnd>;
	m_CreateTable["PlazaOwnerWnd"]= new WindowCreator<PlazaOwnerWnd>;
	m_CreateTable["PlazaInviteList"] = new WindowCreator<PlazaInviteList>;
	m_CreateTable["PlazaInviteListWnd"] = new WindowCreator<PlazaInviteListWnd>;
	m_CreateTable["PlazaMainWnd"] = new WindowCreator<PlazaMainWnd>;
	m_CreateTable["PlazaMainOptionWnd"] = new WindowCreator<PlazaMainOptionWnd>;
	m_CreateTable["PlazaInvitedBtn"] = new WindowCreator<PlazaInvitedBtn>;
	m_CreateTable["PlazaInvitedWnd"] = new WindowCreator<PlazaInvitedWnd>;
	m_CreateTable["PlazaModeNMapListWnd"] = new WindowCreator<PlazaModeNMapListWnd>;
	m_CreateTable["PlazaCreateWnd"] = new WindowCreator<PlazaCreateWnd>;
	m_CreateTable["MannerTrialWnd"] = new WindowCreator<MannerTrialWnd>;
	m_CreateTable["TimeScoreWnd"] = new WindowCreator<TimeScoreWnd>;
	m_CreateTable["BossMonsterGaugeWnd"] = new WindowCreator<BossMonsterGaugeWnd>;
	m_CreateTable["PlazaNPCGaugeWnd"] = new WindowCreator<PlazaNPCGaugeWnd>;
	m_CreateTable["ModeRankWnd"] = new WindowCreator<ModeRankWnd>;

	m_CreateTable["LineChatWnd"] = new WindowCreator<LineChatWnd>;
	m_CreateTable["GuildLineChatWnd"] = new WindowCreator<GuildLineChatWnd>;
	m_CreateTable["ComplexTitleBtn"] = new WindowCreator<ComplexTitleBtn>;
	m_CreateTable["GuildRankingListBtn"]	= new WindowCreator<GuildRankingListBtn>;
	m_CreateTable["GuildRankingListWnd"]	= new WindowCreator<GuildRankingListWnd>;
	m_CreateTable["GuildHeroRankingListWnd"]= new WindowCreator<GuildHeroRankingListWnd>;
	m_CreateTable["GuildCreateWnd"]			= new WindowCreator<GuildCreateWnd>;
	m_CreateTable["GuildMarkBtn"]			= new WindowCreator<GuildMarkBtn>;
	m_CreateTable["GuildEventPopup"]        = new WindowCreator<GuildEventPopup>;
	m_CreateTable["GuildInfoWnd"]			= new WindowCreator<GuildInfoWnd>;
	m_CreateTable["GuildMainWnd"]           = new WindowCreator<GuildMainWnd>;
	m_CreateTable["GuildInfoUserListWnd"]   = new WindowCreator<GuildInfoUserListWnd>;
	m_CreateTable["GuildInfoUserBtn"]       = new WindowCreator<GuildInfoUserBtn>;
	m_CreateTable["GuildInfoUserOptionWnd"] = new WindowCreator<GuildInfoUserOptionWnd>;
	m_CreateTable["GuildEntryDelayUserWnd"] = new WindowCreator<GuildEntryDelayUserWnd>;
	m_CreateTable["GuildEntryDelayUserBtn"] = new WindowCreator<GuildEntryDelayUserBtn>;
	m_CreateTable["GuildInvitationWnd"]     = new WindowCreator<GuildInvitationWnd>;
	m_CreateTable["GuildInvitationFriendWnd"] = new WindowCreator<GuildInvitationFriendWnd>;
	m_CreateTable["GuildInvitedWnd"]        = new WindowCreator<GuildInvitedWnd>;
	m_CreateTable["GuildTitleChangeWnd"]    = new WindowCreator<GuildTitleChangeWnd>;
	m_CreateTable["GuildEtcItemPopup"]      = new WindowCreator<GuildEtcItemPopup>;
	m_CreateTable["GuildNameChangeWnd"]     = new WindowCreator<GuildNameChangeWnd>;
	m_CreateTable["CampMyInfoBtn"]          = new WindowCreator<CampMyInfoBtn>;
	m_CreateTable["CampMainWnd"]            = new WindowCreator<CampMainWnd>;
	m_CreateTable["InfluenceWnd"]           = new WindowCreator<InfluenceWnd>;
	m_CreateTable["InfluenceBtn"]           = new WindowCreator<InfluenceBtn>;
	m_CreateTable["CampEnterWnd"]           = new WindowCreator<CampEnterWnd>;
	m_CreateTable["CampEnterAlarmWnd"]      = new WindowCreator<CampEnterAlarmWnd>;
	m_CreateTable["CampEnterResultWnd"]     = new WindowCreator<CampEnterResultWnd>;
	m_CreateTable["CampSeasonBonus"]        = new WindowCreator<CampSeasonBonus>;
	m_CreateTable["CampEnterModalWnd"]		= new WindowCreator<CampEnterModalWnd>;

	m_CreateTable["LadderTeamListWnd"]		= new WindowCreator<LadderTeamListWnd>;
	m_CreateTable["LadderTeamCreateBtn"]	= new WindowCreator<LadderTeamCreateBtn>;
	m_CreateTable["LadderTeamListBtn"]		= new WindowCreator<LadderTeamListBtn>;
	m_CreateTable["LadderTeamInfoBtn"]		= new WindowCreator<LadderTeamInfoBtn>;
	m_CreateTable["LadderTeamInfoWnd"]		= new WindowCreator<LadderTeamInfoWnd>;
	m_CreateTable["MyLadderTeamInfoWnd"]    = new WindowCreator<MyLadderTeamInfoWnd>;
	m_CreateTable["LadderCreateWnd"]        = new WindowCreator<LadderCreateWnd>;
	m_CreateTable["LadderPasswordWnd"]      = new WindowCreator<LadderPasswordWnd>;
	m_CreateTable["LadderOwnerWnd"]         = new WindowCreator<LadderOwnerWnd>;
	m_CreateTable["LadderBattleWnd"]		= new WindowCreator<LadderBattleWnd>;
	m_CreateTable["LadderBattleHeroWnd"]	= new WindowCreator<LadderBattleHeroWnd>;
	m_CreateTable["LadderBattleHeroRankBtn"]= new WindowCreator<LadderBattleHeroRankBtn>;
	m_CreateTable["LadderModeNMapBtn"]      = new WindowCreator<LadderModeNMapBtn>;
	m_CreateTable["LadderBattleMemberList"] = new WindowCreator<LadderBattleMemberList>;
	m_CreateTable["LadderUserOptionWnd"]    = new WindowCreator<LadderUserOptionWnd>;
	m_CreateTable["LadderChatWnd"]			= new WindowCreator<LadderChatWnd>;
	m_CreateTable["LadderTeamInviteList"]   = new WindowCreator<LadderTeamInviteList>;

	m_CreateTable["RankBattleWnd"]			= new WindowCreator<RankBattleWnd>;
	m_CreateTable["ioRankBattleUpWnd"]		= new WindowCreator<ioRankBattleUpWnd>;

	//UJ 140120, 보안강화: 의미는 알수없으나 방지 #3
	_ENCSTR( "LadderTeamInviteListWnd", szEncLadderTeamInviteListWnd );
	m_CreateTable[szEncLadderTeamInviteListWnd]= new WindowCreator<LadderTeamInviteListWnd>;

	m_CreateTable["LadderTeamInvitedBtn"]   = new WindowCreator<LadderTeamInvitedBtn>;
	m_CreateTable["LadderTeamInvitedWnd"]   = new WindowCreator<LadderTeamInvitedWnd>;
	m_CreateTable["LadderRankingListWnd"]   = new WindowCreator<LadderRankingListWnd>;
	m_CreateTable["LadderRankListBtn"]  = new WindowCreator<LadderRankListBtn>;
	m_CreateTable["LadderTeamRankListWnd"]  = new WindowCreator<LadderTeamRankListWnd>;
	m_CreateTable["LadderHeroRankListWnd"]  = new WindowCreator<LadderHeroRankListWnd>;

	m_CreateTable["FileBtn"]                = new WindowCreator<FileBtn>;
	m_CreateTable["FileListWnd"]            = new WindowCreator<FileListWnd>;
	m_CreateTable["SettingCustomSoundItemWnd"]       = new WindowCreator<SettingCustomSoundItemWnd>;
	m_CreateTable["GameOptionSettingCustomSoundTab"] = new WindowCreator<GameOptionSettingCustomSoundTab>;
	m_CreateTable["CustomSoundBtn"]  = new WindowCreator<CustomSoundBtn>;
	m_CreateTable["FileListItem"]    = new WindowCreator<FileListItem>;
	m_CreateTable["FileDriveListWnd"]= new WindowCreator<FileDriveListWnd>;
	m_CreateTable["PresentAlarmWnd"]  = new WindowCreator<PresentAlarmWnd>;

	m_CreateTable["PresentDeleteWnd"] = new WindowCreator<PresentDeleteWnd>;
	m_CreateTable["PresentTimeCashAlarmWnd"]  = new WindowCreator<PresentTimeCashAlarmWnd>;
	m_CreateTable["PresentTimeCashReleaseWnd"] = new WindowCreator<PresentTimeCashReleaseWnd>;

	m_CreateTable["PCRoomAuthorityWnd"] = new WindowCreator<PCRoomAuthorityWnd>;
	m_CreateTable["PCRoomSoldierBonusWnd"] = new WindowCreator<PCRoomSoldierBonusWnd>;
	m_CreateTable["PCRoomSoldierBonusBtn"] = new WindowCreator<PCRoomSoldierBonusBtn>;
	m_CreateTable["BillingWantWnd"] = new WindowCreator<BillingWantWnd>;
	m_CreateTable["IDChangeWnd"]    = new WindowCreator<IDChangeWnd>;
	m_CreateTable["ChangeNameWnd"]    = new WindowCreator<ChangeNameWnd>;
	m_CreateTable["ChangeNameResultWnd"]    = new WindowCreator<ChangeNameResultWnd>;
	m_CreateTable["RecordInitWnd"]			= new WindowCreator<RecordInitWnd>;
	m_CreateTable["RecordInitResultWnd"]    = new WindowCreator<RecordInitResultWnd>;

	//UJ 140120, 보안강화: 의미는 알수없으나 방지 #2
	_ENCSTR( "GradeUpBonusEtcItemWnd", szEncGradeUpBonusEtcItemWnd );
	m_CreateTable[szEncGradeUpBonusEtcItemWnd] = new WindowCreator<GradeUpBonusEtcItemWnd>;

	m_CreateTable["BankruptcyCharWnd"]  = new WindowCreator<BankruptcyCharWnd>;

	m_CreateTable["GrowthDownSelectWnd"] = new WindowCreator<GrowthDownSelectWnd>;
	m_CreateTable["GrowthAllDownSelectWnd"] = new WindowCreator<GrowthAllDownSelectWnd>;
	m_CreateTable["SelectListBtn"] = new WindowCreator<SelectListBtn>;

	m_CreateTable["ItemCompoundWnd"] = new WindowCreator<ItemCompoundWnd>;
	m_CreateTable["SelectExtraItemBtn"] = new WindowCreator<SelectExtraItemBtn>;
	m_CreateTable["ItemCompoundResultWnd"] = new WindowCreator<ItemCompoundResultWnd>;
	
	m_CreateTable["ItemGrowthCatalystWnd"] = new WindowCreator<ItemGrowthCatalystWnd>;
	m_CreateTable["ItemGrowthCatalystResultWnd"] = new WindowCreator<ItemGrowthCatalystResultWnd>;

	m_CreateTable["ItemCompoundExWnd"] = new WindowCreator<ItemCompoundExWnd>;
	m_CreateTable["ItemCompoundExResultWnd"] = new WindowCreator<ItemCompoundExResultWnd>;

	m_CreateTable["ItemLuckyCoinWnd"] = new WindowCreator<ItemLuckyCoinWnd>;
	m_CreateTable["ItemLuckyCoinResultWnd"] = new WindowCreator<ItemLuckyCoinResultWnd>;

	m_CreateTable["ItemRainbowMixerWnd"] = new WindowCreator<ItemRainbowMixerWnd>;
	m_CreateTable["ItemRainbowMixerResultWnd"] = new WindowCreator<ItemRainbowMixerResultWnd>;

	m_CreateTable["ItemLostSagaMixerWnd"] = new WindowCreator<ItemLostSagaMixerWnd>;
	m_CreateTable["ItemLostSagaMixerResultWnd"] = new WindowCreator<ItemLostSagaMixerResultWnd>;

	m_CreateTable["ItemGoldBoxWnd"] = new WindowCreator<ItemGoldBoxWnd>;

	m_CreateTable["AccessoryCompoundWnd"] = new WindowCreator<AccessoryCompoundWnd>;
	m_CreateTable["AccessoryCompoundResultWnd"] = new WindowCreator<AccessoryCompoundResultWnd>;

	m_CreateTable["GrowthDownDoneWnd"] = new WindowCreator<GrowthDownDoneWnd>;
	m_CreateTable["GrowthAllDownDoneWnd"] = new WindowCreator<GrowthAllDownDoneWnd>;
	m_CreateTable["ioExtraItemRandomBox"] = new WindowCreator<ioExtraItemRandomBox>;

	m_CreateTable["ioExtraItemListWnd"] = new WindowCreator<ioExtraItemListWnd>;
	m_CreateTable["ItemListIconBtn"]	= new WindowCreator<ItemListIconBtn>;

	m_CreateTable["ioExtraItemReleaseWnd"] = new WindowCreator<ioExtraItemReleaseWnd>;
	m_CreateTable["ioTimeGrowthEndWnd"] = new WindowCreator<ioTimeGrowthEndWnd>;

	m_CreateTable["ioDecoItemListWnd"] = new WindowCreator<ioDecoItemListWnd>;

	m_CreateTable["ExpandMedalSlotWnd"] = new WindowCreator<ExpandMedalSlotWnd>;
	m_CreateTable["ExpandMedalSlotResultWnd"] = new WindowCreator<ExpandMedalSlotResultWnd>;
	m_CreateTable["ioExMedalSlotReleaseWnd"] = new WindowCreator<ioExMedalSlotReleaseWnd>;
	m_CreateTable["MedalSlotResultBtn"] = new WindowCreator<MedalSlotResultBtn>;

	m_CreateTable["SelectSoldierListBtn"] = new WindowCreator<SelectSoldierListBtn>;
	m_CreateTable["MedalSlotBtn"] = new WindowCreator<MedalSlotBtn>;


	m_CreateTable["FishingExpWnd"] = new WindowCreator<FishingExpWnd>;
	m_CreateTable["FishingScreenBtn"] = new WindowCreator<FishingScreenBtn>;
	m_CreateTable["FishingGearEquipBtn"] = new WindowCreator<FishingGearEquipBtn>;
	m_CreateTable["FishingItemPageBtn"] = new WindowCreator<FishingItemPageBtn>;
	m_CreateTable["FishingItemInfoWnd"] = new WindowCreator<FishingItemInfoWnd>;
	m_CreateTable["FishingInventoryWnd"] = new WindowCreator<FishingInventoryWnd>;
	m_CreateTable["FishingBaitListWnd"] = new WindowCreator<FishingBaitListWnd>;
	m_CreateTable["FishingBuyWnd"] = new WindowCreator<FishingBuyWnd>;
	m_CreateTable["FishingIventoryCategoryTab"]		= new WindowCreator<FishingIventoryCategoryTab>;

	m_CreateTable["CenterHelpWnd"] = new WindowCreator<CenterHelpWnd>;
	m_CreateTable["MonsterSurvivalHelWnd"] = new WindowCreator<MonsterSurvivalHelWnd>;

	m_CreateTable["PackageItemBuyWnd"] = new WindowCreator<PackageItemBuyWnd>;
	m_CreateTable["PackageItemBtn"] = new WindowCreator<PackageItemBtn>;
	m_CreateTable["PackageItemBuyAgreeWnd"] = new WindowCreator<PackageItemBuyAgreeWnd>;
	m_CreateTable["PackageItemBuyResultWnd"] = new WindowCreator<PackageItemBuyResultWnd>;

	m_CreateTable["PreSetSoldierPackageWnd"] = new WindowCreator<PreSetSoldierPackageWnd>;
	m_CreateTable["PreSetSoldierPackageResultWnd"] = new WindowCreator<PreSetSoldierPackageResultWnd>;

	m_CreateTable["FirstIDChangeWnd"] = new WindowCreator<FirstIDChangeWnd>;

	_ENCSTR( "MonsterCoinUseWnd", szEncMonsterCoinUseWnd );
	m_CreateTable[szEncMonsterCoinUseWnd] = new WindowCreator<MonsterCoinUseWnd>;

	m_CreateTable["GashaponWnd"] = new WindowCreator<GashaponWnd>;
	m_CreateTable["GashaponPresentListWnd"] = new WindowCreator<GashaponPresentListWnd>;
	m_CreateTable["GashaponPresentListBtn"] = new WindowCreator<GashaponPresentListBtn>;

	m_CreateTable["RandomDecoWnd"] = new WindowCreator<RandomDecoWnd>;
	m_CreateTable["RandomDecoSelectWnd"] = new WindowCreator<RandomDecoSelectWnd>;
	m_CreateTable["ioNewMedalRandomBox"] = new WindowCreator<ioNewMedalRandomBox>;
	m_CreateTable["ioNewMedalBtn"] = new WindowCreator<ioNewMedalBtn>;

	m_CreateTable["ItemMultipleCompoundWnd"] = new WindowCreator<ItemMultipleCompoundWnd>;
	m_CreateTable["ItemMultipleCompoundResultWnd"] = new WindowCreator<ItemMultipleCompoundResultWnd>;

	m_CreateTable["UniqueItemMultipleCompoundWnd"] = new WindowCreator<UniqueItemMultipleCompoundWnd>;
	m_CreateTable["UniqueItemMultipleCompoundResultWnd"] = new WindowCreator<UniqueItemMultipleCompoundResultWnd>;

	m_CreateTable["GuildMarkChangeWnd"] = new WindowCreator<GuildMarkChangeWnd>;
	m_CreateTable["ExerciseSoldierEventWnd"] = new WindowCreator<ExerciseSoldierEventWnd>;

	m_CreateTable["SendPresentIdWnd"]          = new WindowCreator<SendPresentIdWnd>;
	m_CreateTable["SendPresentFriendListWnd"]  = new WindowCreator<SendPresentFriendListWnd>;
	m_CreateTable["SendPresentConfirmWnd"]     = new WindowCreator<SendPresentConfirmWnd>;

	m_CreateTable["ExcavationWnd"]		= new WindowCreator<ExcavationWnd>;
	m_CreateTable["ExcavationResultWnd"]	= new WindowCreator<ExcavationResultWnd>;
	m_CreateTable["ExcavationShovelWnd"]	= new WindowCreator<ExcavationShovelWnd>;
	m_CreateTable["ExcavationLevelUpWnd"]	= new WindowCreator<ExcavationLevelUpWnd>;

	m_CreateTable["InviteProgressWnd"] = new WindowCreator<InviteProgressWnd>;
	m_CreateTable["JoinRequestProgressWnd"] = new WindowCreator<JoinRequestProgressWnd>;
	m_CreateTable["QuestOccurStateWnd"] = new WindowCreator<QuestOccurStateWnd>;
	m_CreateTable["QuestOccurStateMainWnd"] = new WindowCreator<QuestOccurStateMainWnd>;
	m_CreateTable["QuestAttainStateWnd"] = new WindowCreator<QuestAttainStateWnd>;
	m_CreateTable["QuestAttainSelectStateBrownWnd"] = new WindowCreator<QuestAttainSelectStateWnd>;
	m_CreateTable["QuestAttainSelectStatePinkWnd"] = new WindowCreator<QuestAttainSelectStateWnd>;
	m_CreateTable["QuestProgressInfoWnd"] = new WindowCreator<QuestProgressInfoWnd>;
	m_CreateTable["QuestProgressInfoMainWnd"] = new WindowCreator<QuestProgressInfoMainWnd>;
	m_CreateTable["QuestCompleteInfoWnd"] = new WindowCreator<QuestCompleteInfoWnd>;
	m_CreateTable["QuestDeleteAndFailWnd"]= new WindowCreator<QuestDeleteAndFailWnd>;
	m_CreateTable["QuestToolTip"]		  = new WindowCreator<QuestToolTip>;
	m_CreateTable["QuestGuideWnd"]        = new WindowCreator<QuestGuideWnd>;
	m_CreateTable["UppderAlphaWnd"]       = new WindowCreator<UppderAlphaWnd>;
	m_CreateTable["GradeUPWnd"] = new WindowCreator<GradeUPWnd>;
	m_CreateTable["SoldierLevelUPWnd"] = new WindowCreator<SoldierLevelUPWnd>;
	m_CreateTable["GlobalTextTooltip"] = new WindowCreator<GlobalTextTooltip>;
	m_CreateTable["ModeHelpTooltip"]   = new WindowCreator<ModeHelpTooltip>;
	m_CreateTable["QuickModeToolTip"]   = new WindowCreator<CQuickModeToolTip>;

	m_CreateTable["UserBlockTypeAlarm"]= new WindowCreator<UserBlockTypeAlarm>;
	m_CreateTable["ChatBubbleWnd"]     = new WindowCreator<ChatBubbleWnd>;
	m_CreateTable["MovieModeHelpWnd"]  = new WindowCreator<MovieModeHelpWnd>;
	m_CreateTable["ClauseViewWnd"]     = new WindowCreator<ClauseViewWnd>;

	m_CreateTable["NewShopIconBtn"]        = new WindowCreator<NewShopIconBtn>;

	//UJ 140120, 보안강화: 상점 치트 방지
	_ENCSTR( "NewShopWnd", szEncNewShopWnd );
	_ENCSTR( "NewShopItemInfoWnd", szEncNewShopItemInfoWnd );
	_ENCSTR( "NewShopPullDownWnd", szEncNewShopPullDownWnd );
	m_CreateTable[szEncNewShopItemInfoWnd]    = new WindowCreator<NewShopItemInfoWnd>;
	m_CreateTable[szEncNewShopWnd]            = new WindowCreator<NewShopWnd>;
	m_CreateTable[szEncNewShopPullDownWnd]    = new WindowCreator<NewShopPullDownWnd>;

	m_CreateTable["NewShopHelpControlWnd"] = new WindowCreator<NewShopHelpControlWnd>;
	m_CreateTable["NewShopHelpWnd"]        = new WindowCreator<NewShopHelpWnd>;
	m_CreateTable["NewShopBoughtWnd"]      = new WindowCreator<NewShopBoughtWnd>;
	m_CreateTable["NewShopBuyWnd"]         = new WindowCreator<NewShopBuyWnd>;
	m_CreateTable["NewShopBuyConfirmWnd"]  = new WindowCreator<NewShopBuyConfirmWnd>;
	m_CreateTable["NewShopToolTip"]        = new WindowCreator<NewShopToolTip>;
	m_CreateTable["NewShopHelpControlBtn"] = new WindowCreator<NewShopHelpControlBtn>;
	m_CreateTable["NewShopNeedLevelWnd"]   = new WindowCreator<NewShopNeedLevelWnd>;
	m_CreateTable["NewShopNeedPesoWnd"]    = new WindowCreator<NewShopNeedPesoWnd>;

	m_CreateTable["GlobalChatWnd"] = new WindowCreator<GlobalChatWnd>;
	m_CreateTable["ChatOptionWnd"] = new WindowCreator<ChatOptionWnd>;

	m_CreateTable["TotalResultMainWnd"] = new WindowCreator<TotalResultMainWnd>;
	m_CreateTable["TotalResultAwardWnd"]= new WindowCreator<TotalResultAwardWnd>;
	m_CreateTable["AwardChatBubbleWnd"] = new WindowCreator<AwardChatBubbleWnd>;
	m_CreateTable["AwardSlotWnd"]       = new WindowCreator<AwardSlotWnd>;
	m_CreateTable["AwardSlotNameBtn"]   = new WindowCreator<AwardSlotNameBtn>;
	m_CreateTable["TotalResultScoreWnd"]= new WindowCreator<TotalResultScoreWnd>;
	m_CreateTable["ResultScoreUserWnd"] = new WindowCreator<ResultScoreUserWnd>;
	m_CreateTable["ResultRaidScoreUserWnd"] = new WindowCreator<ResultRaidScoreUserWnd>;
	m_CreateTable["ResultRaidScoreUserBtn"] = new WindowCreator<ResultRaidScoreUserBtn>;
	m_CreateTable["TotalResultTreasureCardWnd"] = new WindowCreator<TotalResultTreasureCardWnd>;
	m_CreateTable["TotalResultTreasureCardBtn"] = new WindowCreator<TotalResultTreasureCardBtn>;
	m_CreateTable["TotalResultExpWnd"]  = new WindowCreator<TotalResultExpWnd>;
	m_CreateTable["TotalResultExpCharSlotWnd"] = new WindowCreator<TotalResultExpCharSlotWnd>;
	m_CreateTable["TotalResultExpGradeWnd"] = new WindowCreator<TotalResultExpGradeWnd>;
	m_CreateTable["TotalResultRawardWnd"] = new WindowCreator<TotalResultRawardWnd>;
	m_CreateTable["TotalResultChatWnd"] = new WindowCreator<TotalResultChatWnd>;
	m_CreateTable["RoundStateUserWnd"]  = new WindowCreator<RoundStateUserWnd>;
	m_CreateTable["RoundStateMainWnd"]  = new WindowCreator<RoundStateMainWnd>;
	m_CreateTable["RoundResultMainWnd"] = new WindowCreator<RoundResultMainWnd>;

	m_CreateTable["UISoldierBtn"]       = new WindowCreator<UISoldierBtn>;
	m_CreateTable["UISoldierSelectWnd"] = new WindowCreator<UISoldierSelectWnd>;

	//UJ 140120, 보안강화: 의미는 알수없으나 방지
	_ENCSTR( "HeroMainWnd", szEncHeroMainWnd );
	m_CreateTable[szEncHeroMainWnd] = new WindowCreator<HeroMainWnd>;

	m_CreateTable["HeroMatchWnd"]= new WindowCreator<HeroMatchWnd>;
	m_CreateTable["HeroMatchRankBtn"] = new WindowCreator<HeroMatchRankBtn>;
	m_CreateTable["CampJoinNeedPopup"]  = new WindowCreator<CampJoinNeedPopup>;
	m_CreateTable["HeroRankingListWnd"] = new WindowCreator<HeroRankingListWnd>;
	m_CreateTable["HeroRankingListBtn"] = new WindowCreator<HeroRankingListBtn>;
	m_CreateTable["HeroMatchHistoryWnd"]= new WindowCreator<HeroMatchHistoryWnd>;
	m_CreateTable["HeroMatchHistoryBtn"]= new WindowCreator<HeroMatchHistoryBtn>;
	m_CreateTable["HeroMatchTop100Wnd"] = new WindowCreator<HeroMatchTop100Wnd>;
	m_CreateTable["HeroMatchTop100Btn"] = new WindowCreator<HeroMatchTop100Btn>;
	m_CreateTable["HeroMatchRankGraphWnd"] = new WindowCreator<HeroMatchRankGraphWnd>;
	m_CreateTable["HeroMatchVersusSceneWnd"] = new WindowCreator<HeroMatchVersusSceneWnd>;

	m_CreateTable["TradeIconBtn"] = new WindowCreator<TradeIconBtn>;
	m_CreateTable["TradeItemInfoWnd"] = new WindowCreator<TradeItemInfoWnd>;

	m_CreateTable["ChangeTradeStateWnd"] = new WindowCreator<ChangeTradeStateWnd>;
	m_CreateTable["ChangeTradeStateConfirmWnd"] = new WindowCreator<ChangeTradeStateConfirmWnd>;
	m_CreateTable["ChangeTradeStateResultWnd"] = new WindowCreator<ChangeTradeStateResultWnd>;

	m_CreateTable["TradeMainWnd"] = new WindowCreator<TradeMainWnd>;
	m_CreateTable["TradeItemBtn"] = new WindowCreator<TradeItemBtn>;
	m_CreateTable["LobbyTradeItemBtn"] = new WindowCreator<LobbyTradeItemBtn>;
	m_CreateTable["LobbyTradeItemCustomBtn"] = new WindowCreator<LobbyTradeItemCustomBtn>;
	
	m_CreateTable["LobbyTradeInfoWnd"] = new WindowCreator<LobbyTradeInfoWnd>;
	m_CreateTable["LobbyOwnerTradeListWnd"] = new WindowCreator<LobbyOwnerTradeListWnd>;

	m_CreateTable["TradeItemBuyWnd"] = new WindowCreator<TradeItemBuyWnd>;
	m_CreateTable["TradeItemBuyResultWnd"] = new WindowCreator<TradeItemBuyResultWnd>;

	m_CreateTable["TradeItemCancelWnd"] = new WindowCreator<TradeItemCancelWnd>;
	m_CreateTable["TradeItemCancelResultWnd"] = new WindowCreator<TradeItemCancelResultWnd>;

	m_CreateTable["TradeItemRegisterWnd"] = new WindowCreator<TradeItemRegisterWnd>;
	m_CreateTable["TradeItemRegisterConfirmWnd"] = new WindowCreator<TradeItemRegisterConfirmWnd>;
	m_CreateTable["TradeItemRegisterResultWnd"] = new WindowCreator<TradeItemRegisterResultWnd>;

	m_CreateTable["ExtraItemEquipConfirmWnd"] = new WindowCreator<ExtraItemEquipConfirmWnd>;

	m_CreateTable["GameOptionSettingKeyInput"] = new WindowCreator<GameOptionSettingKeyInput>;

	m_CreateTable["ItemCustomIconBtn"]	 = new WindowCreator<ItemCustomIconBtn>;
	m_CreateTable["ItemCustomInfoWnd"]	 = new WindowCreator<ItemCustomInfoWnd>;
	m_CreateTable["ItemCustomSelectWnd"] = new WindowCreator<ItemCustomSelectWnd>;
	m_CreateTable["ItemCustomMakeWnd"]   = new WindowCreator<ItemCustomMakeWnd>;
	m_CreateTable["ItemCustomMake3DViewWnd"] = new WindowCreator<ItemCustomMake3DViewWnd>;
	m_CreateTable["ItemCustomMakeDetailViewWnd"] = new WindowCreator<ItemCustomMakeDetailViewWnd>;
	m_CreateTable["ItemCustomGlobalDetailViewWnd"] = new WindowCreator<ItemCustomGlobalDetailViewWnd>;
	m_CreateTable["ItemCustomMakeDownloadWnd"] = new WindowCreator<ItemCustomMakeDownloadWnd>;
	m_CreateTable["ItemCustomMakeUploadWnd"]   = new WindowCreator<ItemCustomMakeUploadWnd>;
	m_CreateTable["ItemCustomMakeConfirmWnd"]  = new WindowCreator<ItemCustomMakeConfirmWnd>;
	m_CreateTable["ItemCustomChangeConfirmWnd"]= new WindowCreator<ItemCustomChangeConfirmWnd>;
	m_CreateTable["FileWriteUploadWnd"]        = new WindowCreator<FileWriteUploadWnd>;
	m_CreateTable["ItemCustomMakeCompleteWnd"] = new WindowCreator<ItemCustomMakeCompleteWnd>;
	m_CreateTable["ItemCustomDeleteWnd"]       = new WindowCreator<ItemCustomDeleteWnd>;
	m_CreateTable["ItemCustomThumbnailTooltip"]= new WindowCreator<ItemCustomThumbnailTooltip>;
	m_CreateTable["ItemCustomThumbnailWnd"]    = new WindowCreator<ItemCustomThumbnailWnd>;

	m_CreateTable["ItemCustomTrainIconBtn"] = new WindowCreator<ItemCustomTrainIconBtn>;
	m_CreateTable["ItemCustomTrainInfoWnd"] = new WindowCreator<ItemCustomTrainInfoWnd>;
	m_CreateTable["ItemCustomTrainSelectWnd"] = new WindowCreator<ItemCustomTrainSelectWnd>;
	m_CreateTable["ItemCustomTrainMakeWnd"] = new WindowCreator<ItemCustomTrainMakeWnd>;
	m_CreateTable["ItemCustomWantEtcItemWnd"] = new WindowCreator<ItemCustomWantEtcItemWnd>;

	m_CreateTable["EtcMotionSelectWnd"]       = new WindowCreator<EtcMotionSelectWnd>;
	m_CreateTable["EtcMotionSelectListWnd"]   = new WindowCreator<EtcMotionSelectListWnd>;

	m_CreateTable["CharRentalRequestWnd"] = new WindowCreator<CharRentalRequestWnd>;
	m_CreateTable["CharRentalReturnWnd"]  = new WindowCreator<CharRentalReturnWnd>;
	m_CreateTable["CharRentalAcquireWnd"] = new WindowCreator<CharRentalAcquireWnd>;

	m_CreateTable["HeadquartersInfoWnd"] = new WindowCreator<HeadquartersInfoWnd>;
	m_CreateTable["HeadquartersJoinRequestWnd"] = new WindowCreator<HeadquartersJoinRequestWnd>;
	m_CreateTable["HeadquartersMainOptionWnd"] = new WindowCreator<HeadquartersMainOptionWnd>;
	m_CreateTable["HeadquartersMainWnd"] = new WindowCreator<HeadquartersMainWnd>;
	m_CreateTable["HeadquartersOwnerWnd"] = new WindowCreator<HeadquartersOwnerWnd>;
	m_CreateTable["HeadquartersInviteListWnd"] = new WindowCreator<HeadquartersInviteListWnd>;
	m_CreateTable["HeadquartersInviteList"]    = new WindowCreator<HeadquartersInviteList>;
	m_CreateTable["HeadquartersInvitedBtn"]    = new WindowCreator<HeadquartersInvitedBtn>;
	m_CreateTable["HeadquartersInvitedWnd"]	   = new WindowCreator<HeadquartersInvitedWnd>;
	m_CreateTable["SoldierExperienceWnd"]      = new WindowCreator<SoldierExperienceWnd>;
	m_CreateTable["FirstSoldierSelectBtn"] = new WindowCreator<FirstSoldierSelectBtn>;
	m_CreateTable["FirstSoldierSelectWnd"] = new WindowCreator<FirstSoldierSelectWnd>;
	m_CreateTable["InvenSoldierSelectBuyWnd"]       = new WindowCreator<InvenSoldierSelectBuyWnd>;
	m_CreateTable["InvenSoldierSelectBuyAgreeWnd"]  = new WindowCreator<InvenSoldierSelectBuyAgreeWnd>;
	m_CreateTable["InvenSoldierSelectBuyResultWnd"] = new WindowCreator<InvenSoldierSelectBuyResultWnd>;
	m_CreateTable["InvenSoldierExpBonusWnd"]        = new WindowCreator<InvenSoldierExpBonusWnd>;
	m_CreateTable["InvenSoldierExpBonusResultWnd"]  = new WindowCreator<InvenSoldierExpBonusResultWnd>;

	m_CreateTable["InvenFourExtraCompoundBuyWnd"]       = new WindowCreator<InvenFourExtraCompoundBuyWnd>;
	m_CreateTable["InvenFourExtraCompoundBuyResultWnd"] = new WindowCreator<InvenFourExtraCompoundBuyResultWnd>;

	m_CreateTable["RunningManNameWnd"] = new WindowCreator<RunningManNameWnd>;

	m_CreateTable["FightClubTimeGaugeWnd"] = new WindowCreator<FightClubTimeGaugeWnd>;
	m_CreateTable["FightClubReadyWnd"]     = new WindowCreator<FightClubReadyWnd>;
	m_CreateTable["FightClubRoundEndWnd"]  = new WindowCreator<FightClubRoundEndWnd>;
	m_CreateTable["FightClubChallengerWnd"]  = new WindowCreator<FightClubChallengerWnd>;
	m_CreateTable["FightClubFightNPCContinueWnd"]  = new WindowCreator<FightClubFightNPCContinueWnd>;

	m_CreateTable["PCRoomLocalBenefitWnd"] = new WindowCreator<PCRoomLocalBenefitWnd>;

	m_CreateTable["BattleModeTimeGaugeWnd"] = new WindowCreator<BattleModeTimeGaugeWnd>;
	m_CreateTable["BattleModeBlueSideUserInfoWnd"]  = new WindowCreator<BattleModeBlueSideUserInfoWnd>;
	m_CreateTable["BattleModeRedSideUserInfoWnd"]  = new WindowCreator<BattleModeRedSideUserInfoWnd>;
	m_CreateTable["BattleModeFunctionWnd"]  = new WindowCreator<BattleModeFunctionWnd>;

	m_CreateTable["TowerDefWnd"] = new WindowCreator<TowerDefWnd>;
	m_CreateTable["TowerDefUserGaugeWnd"] = new WindowCreator<TowerDefUserGaugeWnd>;
	m_CreateTable["ConsumptionWnd"] = new WindowCreator<ConsumptionWnd>;
	m_CreateTable["ConsumptionSlotBtn"] = new WindowCreator<ConsumptionSlotBtn>;

	m_CreateTable["SelectExtraIconBtn"] = new WindowCreator<SelectExtraIconBtn>;
	m_CreateTable["SelectExtraInfoWnd"] = new WindowCreator<SelectExtraInfoWnd>;
	m_CreateTable["SelectExtraGashaponSubWnd"] = new WindowCreator<SelectExtraGashaponSubWnd>;
	m_CreateTable["SelectExtraGashaponBtn"] = new WindowCreator<SelectExtraGashaponBtn>;
	m_CreateTable["SelectExtraGashaponWnd"] = new WindowCreator<SelectExtraGashaponWnd>;

	m_CreateTable["EtcItemSlotWnd"] = new WindowCreator<EtcItemSlotWnd>;
	m_CreateTable["ItemSlotBtn"] = new WindowCreator<ItemSlotBtn>;
	m_CreateTable["SlotWnd"] = new WindowCreator<SlotWnd>;
	
	m_CreateTable["TournamentMainWnd"] = new WindowCreator< TournamentMainWnd >;
	m_CreateTable["TournamentListBtn"] = new WindowCreator< TournamentListBtn >;
	m_CreateTable["LobbyTournamentInfoWnd"] = new WindowCreator< LobbyTournamentInfoWnd >;
	m_CreateTable["LobbyTournamentTeamBtn"] = new WindowCreator< LobbyTournamentTeamBtn >;
	m_CreateTable["TournamentTeamCreateWnd"]	= new WindowCreator< TournamentTeamCreateWnd >;
	m_CreateTable["TournamentTeamCreateComplete"]	= new WindowCreator< TournamentTeamCreateComplete >;
	m_CreateTable["TournamentTeamInfoBtn"]	= new WindowCreator< TournamentTeamInfoBtn >;
	m_CreateTable["TournamentTeamInfoWnd"]	= new WindowCreator< TournamentTeamInfoWnd >;
	m_CreateTable["TournamentTeamUserOptionWnd"] = new WindowCreator< TournamentTeamUserOptionWnd >;
	m_CreateTable["TournamentTeamInvitationWnd"] = new WindowCreator< TournamentTeamInvitationWnd >;
	m_CreateTable["TournamentTeamInvitationFriendWnd"] = new WindowCreator< TournamentTeamInvitationFriendWnd >;
	m_CreateTable["TournamentTeamInvitedWnd"] = new WindowCreator< TournamentTeamInvitedWnd >;
	m_CreateTable["TournamentTeamEntryDelayUserWnd"] = new WindowCreator< TournamentTeamEntryDelayUserWnd >;
	m_CreateTable["TournamentTeamEntryDelayUserBtn"] = new WindowCreator< TournamentTeamEntryDelayUserBtn >;
	m_CreateTable["TournamentTeamEventWnd"] = new WindowCreator< TournamentTeamEventWnd >;
	m_CreateTable["TournamentTeamAlarmWnd"] = new WindowCreator< TournamentTeamAlarmWnd >;

	m_CreateTable["TournamentDetailMainWnd"] = new WindowCreator< TournamentDetailMainWnd >;
	m_CreateTable["TournamentDetailMainTabBtn"] = new WindowCreator< TournamentDetailMainTabBtn >;
	m_CreateTable["TournamentDetailFrameWnd"] = new WindowCreator< TournamentDetailFrameWnd >;
	m_CreateTable["TournamentDetailFrameInfoWnd"] = new WindowCreator< TournamentDetailFrameInfoWnd >;
	m_CreateTable["TournamentDetailFrameCheerWnd"] = new WindowCreator< TournamentDetailFrameCheerWnd >;
	m_CreateTable["TournamentDetailScheduleFrameWnd"] = new WindowCreator< TournamentDetailScheduleFrameWnd >;
	m_CreateTable["TournamentDetailFrameAwardWnd"] = new WindowCreator< TournamentDetailFrameAwardWnd >;
	m_CreateTable["MatchBtn"] = new WindowCreator< MatchBtn >;
	m_CreateTable["TournamentDetailInnerRewardWnd"] = new WindowCreator< TournamentDetailInnerRewardWnd >;
	m_CreateTable["TournamentDetailInnerCampRewardWnd"] = new WindowCreator< TournamentDetailInnerCampRewardWnd >;	
	m_CreateTable["TournamentDetailInnerCheerWnd"] = new WindowCreator< TournamentDetailInnerCheerWnd >;
	m_CreateTable["TournamentDetailInnerCustomWnd"] =  new WindowCreator< TournamentDetailInnerCustomWnd >;
	m_CreateTable["CheerListRadioBtn"] = new WindowCreator< CheerListRadioBtn >;
	m_CreateTable["TournamentDetailInnerCheerListWnd"] = new WindowCreator< TournamentDetailInnerCheerListWnd >;
	m_CreateTable["TournamentDetailInnerPageNoticeWnd"] = new WindowCreator< TournamentDetailInnerPageNoticeWnd >;
	m_CreateTable["TournamentDetailInnerPageScheduleWnd"] = new WindowCreator< TournamentDetailInnerPageScheduleWnd >;
	m_CreateTable["TournamentDetailInnerPageRewardWnd"] = new WindowCreator< TournamentDetailInnerPageRewardWnd >;

	m_CreateTable["TournamentDetailFrameAllocateWnd"] = new WindowCreator< TournamentDetailFrameAllocateWnd >;
	m_CreateTable["TournamentDetailInnerAllocateWnd"] = new WindowCreator< TournamentDetailInnerAllocateWnd >;
	
	m_CreateTable["TournamentTeamCharWnd"] = new WindowCreator< TournamentTeamCharWnd >;
	m_CreateTable["TournamentBattleInviteWnd"] = new WindowCreator< TournamentBattleInviteWnd >;
	m_CreateTable["TournamentRuleAgreeConfirmWnd"] = new WindowCreator< TournamentRuleAgreeConfirmWnd >;
	m_CreateTable["TournamentMatchWinAlarmWnd"] = new WindowCreator< TournamentMatchWinAlarmWnd >;
	m_CreateTable["TournamentAlarmMainRewardWnd"] = new WindowCreator< TournamentAlarmMainRewardWnd >;
	m_CreateTable["TournamentAlarmRewardWnd"] = new WindowCreator< TournamentAlarmRewardWnd >; 
	m_CreateTable["TournamentAlarmCampRewardWnd"] = new WindowCreator< TournamentAlarmCampRewardWnd >; 
	m_CreateTable["TournamentAlarmCheerRewardWnd"] = new WindowCreator< TournamentAlarmCheerRewardWnd >; 
	m_CreateTable["TournamentAlarmCustomRewardWnd"] = new WindowCreator< TournamentAlarmCustomRewardWnd >; 
	m_CreateTable["TournamentUnearnedWinAlarmWnd"] = new WindowCreator< TournamentUnearnedWinAlarmWnd >;
	m_CreateTable["TotalResultTournamentWnd"] = new WindowCreator< TotalResultTournamentWnd >;
	
	m_CreateTable["StringScrollListWnd"] =  new WindowCreator< StringScrollListWnd >;

	m_CreateTable["CustomTournamentCreateWnd"] = new WindowCreator< CustomTournamentCreateWnd >;
	m_CreateTable["CustomTournamentViewWnd"] = new WindowCreator< CustomTournamentViewWnd >;
	m_CreateTable["CustomTournamentScheduleWnd"] = new WindowCreator< CustomTournamentScheduleWnd >;
	m_CreateTable["CustomTournamentListBtn"] = new WindowCreator< CustomTournamentListBtn >; 	
	m_CreateTable["CustomTournamentScheduleBtn"] = new WindowCreator< CustomTournamentScheduleBtn >;
	m_CreateTable["CalendarWnd"] = new WindowCreator< CalendarWnd >;
	m_CreateTable["CalendarButton"] = new WindowCreator< CalendarButton >;

	m_CreateTable["TournamentEtcItemNeedPopup"] = new WindowCreator< TournamentEtcItemNeedPopup >;
	m_CreateTable["TournamentEtcItemUsePopup"]  = new WindowCreator< TournamentEtcItemUsePopup >;
	m_CreateTable["TournamentRoomListWnd"]		= new WindowCreator< TournamentRoomListWnd >;
	m_CreateTable["TournamentRoomListBtn"]      = new WindowCreator< TournamentRoomListBtn >;

	m_CreateTable["CustomTournamentCreateResultWnd"] = new WindowCreator< CustomTournamentCreateResultWnd >;
	m_CreateTable["CustomTournamentNoticeWnd"] = new WindowCreator< CustomTournamentNoticeWnd >;
	m_CreateTable["CustomTournamentAllocateWnd"] = new WindowCreator< CustomTournamentAllocateWnd >;
	m_CreateTable["UnAllocateButton"] = new WindowCreator< UnAllocateButton >;
	m_CreateTable["AllocatedButton"] = new WindowCreator< AllocatedButton >;
	m_CreateTable["UnAllocateWnd"] = new WindowCreator< UnAllocateWnd >;
	m_CreateTable["AllocatedWnd"] = new WindowCreator< AllocatedWnd >;
	m_CreateTable["CustomTournamentAllocateResultWnd"] = new WindowCreator< CustomTournamentAllocateResultWnd >;
	m_CreateTable["CustomTournamentRewardSelectWnd"] = new WindowCreator < CustomTournamentRewardSelectWnd >;
	m_CreateTable["CustomTournamentRewardButton"] = new WindowCreator< CustomTournamentRewardButton >;
	m_CreateTable["CustomTournamentRewardLabel"] = new WindowCreator< CustomTournamentRewardLabel >;
	m_CreateTable["TournamentCoinItemNeedPopup"] = new WindowCreator< TournamentCoinItemNeedPopup >;
	m_CreateTable["CustomTournamentRewardToolTip"] = new WindowCreator< CustomTournamentRewardToolTip >;
	m_CreateTable["CusotmTournamentTeamListBtn"] = new WindowCreator< CusotmTournamentTeamListBtn >;
	m_CreateTable["CustomTournamentEntryListWnd"] = new WindowCreator< CustomTournamentEntryListWnd >;
	m_CreateTable["CustomTournamentDelayCreateTeamListWnd"] = new WindowCreator< CustomTournamentDelayCreateTeamListWnd >;
	m_CreateTable["CustomTournamentDelayCreateTeamListBtn"] = new WindowCreator< CustomTournamentDelayCreateTeamListBtn >;
	m_CreateTable["TournamentJoinConfirmAgreeWnd"] = new WindowCreator< TournamentJoinConfirmAgreeWnd >;

	m_CreateTable["TournamentInvitationWnd"] = new WindowCreator< TournamentInvitationWnd >;

	m_CreateTable["NormalModeBtn"]  = new WindowCreator< NormalModeBtn >;
	m_CreateTable["PVEModeBtn"]  = new WindowCreator< PVEModeBtn >;	
	m_CreateTable["PVPModeBtn"]  = new WindowCreator< PVPModeBtn >;	

	m_CreateTable["AniTextWnd"] = new WindowCreator <AniTextWnd>;
	m_CreateTable["AniLetterSubWnd"] = new WindowCreator <AniLetterSubWnd>;

	m_CreateTable["RouletteWnd"] = new WindowCreator< RouletteWnd >;
	m_CreateTable["RouletteHelpWnd"] = new WindowCreator< RouletteHelpWnd >;
	m_CreateTable["RouletteCenterHelpWnd"] = new WindowCreator< RouletteCenterHelpWnd >;

	m_CreateTable["BingoWnd"] = new WindowCreator< BingoWnd >;
	m_CreateTable["RewardButton"] = new WindowCreator< RewardButton >;
	m_CreateTable["NumberButton"] = new WindowCreator< NumberButton >;
	m_CreateTable["BingoToolTip"] = new WindowCreator< BingoToolTip >;
	m_CreateTable["BingoRightWnd"] = new WindowCreator< BingoRightWnd >;
	m_CreateTable["BingoMsgBox"] = new WindowCreator< BingoMsgBox >;
	m_CreateTable["BingoHelpButton"] = new WindowCreator< BingoHelpButton >;
	m_CreateTable["BingoHelpToolTip"] = new WindowCreator< BingoHelpToolTip >;

	m_CreateTable["ioBingoIconButton"] = new WindowCreator< ioBingoIconButton >;
	m_CreateTable["ioBingoRewardWnd"] = new WindowCreator< ioBingoRewardWnd >;

	m_CreateTable["RelativeGradeBonusWnd"] = new WindowCreator< RelativeGradeBonusWnd >;
	m_CreateTable["SuperGashaponWnd"] = new WindowCreator< SuperGashaponWnd >;
	m_CreateTable["MultiSuperGashaponWnd"] = new WindowCreator< MultiSuperGashaponWnd >;
	m_CreateTable["MultiSuperGashaponToastPopupWnd"] = new WindowCreator< MultiSuperGashaponToastPopupWnd >;

	m_CreateTable["UsePresentEtcItemWnd"] = new WindowCreator< UsePresentEtcItemWnd >;
	m_CreateTable["FriendListWnd"]        = new WindowCreator< FriendListWnd >;
	m_CreateTable["PresentListWnd"]       = new WindowCreator< PresentListWnd >;
	m_CreateTable["SendPresentEtcItemConfirmWnd"] = new WindowCreator< SendPresentEtcItemConfirmWnd >;

	m_CreateTable["ItemRechargeWnd"]        = new WindowCreator< ItemRechargeWnd >;
	m_CreateTable["AccessoryRechargeWnd"]        = new WindowCreator< AccessoryRechargeWnd >;
	m_CreateTable["AccesorryRechargeProcessWnd"] = new WindowCreator< AccessoryRechargeProcessWnd >;
	m_CreateTable["AccessoryInfoButton"]         = new WindowCreator< AccessoryInfoButton >;
	m_CreateTable["ItemInfoButton"]         = new WindowCreator< ItemInfoButton >;
	m_CreateTable["ItemIconButton"]         = new WindowCreator< ItemIconButton >;
	m_CreateTable["ItemRechargeProcessWnd"] = new WindowCreator< ItemRechargeProcessWnd >;
	m_CreateTable["RechargeItemTooltip"] = new WindowCreator< RechargeItemTooltip >;

	m_CreateTable["TournamentCheerPopUpWnd"] = new WindowCreator< TournamentCheerPopUpWnd >;
	m_CreateTable["TournamentPredictPopUpWnd"] = new WindowCreator< TournamentPredictPopUpWnd >;

	m_CreateTable["TimeGateSearchShuffle"]       = new WindowCreator<TimeGateSearchShuffle>;
	m_CreateTable["TimeGateSearchShuffleResult"] = new WindowCreator<TimeGateSearchShuffleResult>;
	m_CreateTable["ShuffleBonusResultWnd"] = new WindowCreator<ShuffleBonusResultWnd>;

	m_CreateTable["ExPCRoomAuthorityWnd"]    = new WindowCreator<ExPCRoomAuthorityWnd>;
	m_CreateTable["ExPCRoomSoldierBonusBtn"] = new WindowCreator<ExPCRoomSoldierBonusBtn>;
	m_CreateTable["ExPCRoomSoldierBonusWnd"] = new WindowCreator<ExPCRoomSoldierBonusWnd>;

	m_CreateTable["SelectCharViewWnd"]       = new WindowCreator<SelectCharViewWnd>;
	m_CreateTable["SelectCharControlWnd"]    = new WindowCreator<SelectCharControlWnd>;
	m_CreateTable["SelectCharControlBtn"]    = new WindowCreator<SelectCharControlBtn>;

	m_CreateTable["AttendanceWnd"] = new WindowCreator<AttendanceWnd>;
	m_CreateTable["CalendarToolTip"] = new WindowCreator<CalendarToolTip>;
	m_CreateTable["ioAttendanceCheckButton"] = new WindowCreator<ioAttendanceCheckButton>;
	m_CreateTable["ioAttendanceCheckedButton"] = new WindowCreator<ioAttendanceCheckedButton>;

	m_CreateTable["SelectGashaponBtn"] = new WindowCreator<SelectGashaponBtn>;
	m_CreateTable["SelectGashaponWnd"] = new WindowCreator<SelectGashaponWnd>;

	m_CreateTable["SelectGashaponSubWnd"] = new WindowCreator<SelectGashaponSubWnd>;
	m_CreateTable["GashaponCategoryTab"] = new WindowCreator<GashaponCategoryTab>;

	m_CreateTable["SelectGashaponItemInfoWnd"] = new WindowCreator<SelectGashaponItemInfoWnd>;
	m_CreateTable["SelectGashaponItemIconBtn"] = new WindowCreator<SelectGashaponItemIconBtn>;

	m_CreateTable["ShuffleCenterHelpWnd"] = new WindowCreator<ShuffleCenterHelpWnd>;
	m_CreateTable["ShuffleRankWnd"] = new WindowCreator<ShuffleRankWnd>;

	m_CreateTable["ItemMaterialCompoundWnd"] = new WindowCreator<ItemMaterialCompoundWnd>;
	m_CreateTable["SelectMaterialItemBtn"]	= new WindowCreator<SelectMaterialItemBtn>;
	m_CreateTable["ItemMaterialCompoundResultWnd"] = new WindowCreator<ItemMaterialCompoundResultWnd>;
	
	m_CreateTable["PetWnd"]	= new WindowCreator<PetWnd>;
	m_CreateTable["PetToolTip"] = new WindowCreator<PetToolTip>;
	m_CreateTable["PetViewWnd"] = new WindowCreator<PetViewWnd>;
	m_CreateTable["PetIconBtn"]	= new WindowCreator<PetIconBtn>;
	m_CreateTable["PetResellWnd"] = new WindowCreator<PetResellWnd>;
	m_CreateTable["PetEggResultWnd"] = new WindowCreator<PetEggResultWnd>;
	m_CreateTable["PetEatWnd"] = new WindowCreator<PetEatWnd>;
	m_CreateTable["SelectPetTargetBtn"] = new WindowCreator<SelectPetTargetBtn>;
	m_CreateTable["PetCompoundWnd"] = new WindowCreator<PetCompoundWnd>;
	m_CreateTable["PetResellResultWnd"] = new WindowCreator<PetResellResultWnd>;
	m_CreateTable["PetEmptyWnd"] = new WindowCreator<PetEmptyWnd>;
		
	m_CreateTable["ioBasicToolTip"] = new WindowCreator<ioBasicToolTip>;
	m_CreateTable["ioPullDownList"] = new WindowCreator<ioPullDownList>;
	m_CreateTable["ioAwakeReleaseWnd"] = new WindowCreator<ioAwakeReleaseWnd>;

	m_CreateTable["ShopItemMovieWnd"] = new WindowCreator<ShopItemMovieWnd>;
	m_CreateTable["ShopItemDescWnd"] = new WindowCreator<ShopItemDescWnd>;

	//용병 강화
	m_CreateTable["ReinforceSelectWnd"]		= new WindowCreator<ReinforceSelectWnd>;
	m_CreateTable["ReinforceSelectSubWnd"]	= new WindowCreator<ReinforceSelectSubWnd>;
	m_CreateTable["SoulStoneBuyWnd"]		= new WindowCreator<SoulStoneBuyWnd>;

	m_CreateTable["HeroReinforceDownWnd"]	= new WindowCreator<HeroReinforceDownWnd>;
	m_CreateTable["HeroReinforceDoneWnd"]	= new WindowCreator<HeroReinforceDoneWnd>;

	m_CreateTable["ItemReinforceDownWnd"]	= new WindowCreator<ItemReinforceDownWnd>;
	m_CreateTable["ItemReinforceDoneWnd"]	= new WindowCreator<ItemReinforceDoneWnd>;

	m_CreateTable["PowerUpSelectWnd"]		= new WindowCreator<PowerUpSelectWnd>;
	m_CreateTable["PowerUpSelectInfoWnd"]	= new WindowCreator<PowerUpSelectInfoWnd>;
	m_CreateTable["PowerUpSelectIconBtn"]	= new WindowCreator<PowerUpSelectIconBtn>;
	m_CreateTable["PowerUpCategoryTab"]		= new WindowCreator<PowerUpCategoryTab>;
	// 진화&퇴화UI
	m_CreateTable["GradeAdjustmentWnd"]				= new WindowCreator<GradeAdjustmentWnd>;
	m_CreateTable["GradeAdjustmentInfoWnd"]			= new WindowCreator<GradeAdjustmentInfoWnd>;
	m_CreateTable["GradeAdjustmentSelectIconBtn"]	= new WindowCreator<GradeAdjustmentSelectIconBtn>;
	m_CreateTable["GradeAdjustmentCategoryTab"]		= new WindowCreator<GradeAdjustmentCategoryTab>;

	m_CreateTable["EnablePowerupListWnd"]			= new WindowCreator<EnablePowerupListWnd>;
	m_CreateTable["EnablePowerupListIconBtn"]		= new WindowCreator<EnablePowerupListIconBtn>;
	m_CreateTable["PowerUpToolTip"]					= new WindowCreator<PowerUpToolTip>;

	m_CreateTable["MyItemSelecteWnd"]				= new WindowCreator<MyItemSelecteWnd>;
	m_CreateTable["MyItemSelectRadioButton"]		= new WindowCreator<MyItemSelectRadioButton>;
	m_CreateTable["ItemSelectInfoWnd"]				= new WindowCreator<ItemSelectInfoWnd>;
	m_CreateTable["AccessorySelectWnd"] = new WindowCreator<AccessorySelectWnd>;
	m_CreateTable["MySpiritSelectWnd"]				= new WindowCreator<MySpiritSelectWnd>;

	// UI코스튬 관련 (윈도우 생성)
	m_CreateTable["CostumeCustomInfoWnd"]				= new WindowCreator<CostumeCustomInfoWnd>;
	m_CreateTable["CostumeCustomSelectWnd"]				= new WindowCreator<CostumeCustomSelectWnd>;
	m_CreateTable["CostumeCustomMakeWnd"]				= new WindowCreator<CostumeCustomMakeWnd>;
	m_CreateTable["CostumeCustomMake3DViewWnd"]			= new WindowCreator<CostumeCustomMake3DViewWnd>;
	m_CreateTable["CostumeCustomMakeDetailViewWnd"]		= new WindowCreator<CostumeCustomMakeDetailViewWnd>;
	m_CreateTable["CostumeCustomMakeConfirmWnd"]		= new WindowCreator<CostumeCustomMakeConfirmWnd>;
	m_CreateTable["CostumeCustomMakeCompleteWnd"]		= new WindowCreator<CostumeCustomMakeCompleteWnd>;
	m_CreateTable["CostumeCustomChangeConfirmWnd"]		= new WindowCreator<CostumeCustomChangeConfirmWnd>;
	m_CreateTable["CostumeCustomDeleteWnd"]				= new WindowCreator<CostumeCustomDeleteWnd>;
	m_CreateTable["CostumeCustomGlobalDetailViewWnd"]	= new WindowCreator<CostumeCustomGlobalDetailViewWnd>;
	m_CreateTable["CostumeCustomThumbnailWnd"]			= new WindowCreator<CostumeCustomThumbnailWnd>;
	m_CreateTable["CostumeCustomThumbnailTooltip"]		= new WindowCreator<CostumeCustomThumbnailTooltip>;
	m_CreateTable["ioCostumeRandomBox"]					= new WindowCreator<ioCostumeRandomBox>;
	m_CreateTable["ioCostumeListWnd"]					= new WindowCreator<ioCostumeListWnd>;
	m_CreateTable["ioCostumeReleaseWnd"]				= new WindowCreator<ioCostumeReleaseWnd>;

	//UI Mission 관련
	m_CreateTable["MissionQuestMainWnd"]	= new WindowCreator<MissionQuestMainWnd>;
	m_CreateTable["QuestListWnd"]	= new WindowCreator<QuestListWnd>;
	m_CreateTable["LobbyQuestNewAlarmWnd"] = new WindowCreator<LobbyQuestNewAlarmWnd>;
	m_CreateTable["LobbyQuestNewAlarmBtn"] = new WindowCreator<LobbyQuestNewAlarmBtn>;
	m_CreateTable["NewQuestListBtn"] = new WindowCreator<NewQuestListBtn>;

	m_CreateTable["LobbyMainMissionWnd"]	= new WindowCreator<LobbyMainMissionWnd>;
	m_CreateTable["LobbyMissionInfoBtn"]	= new WindowCreator<LobbyMissionInfoBtn>;
	m_CreateTable["MainMissionWnd"]			= new WindowCreator<MainMissionWnd>;
	m_CreateTable["MissionInfoBtn"]			= new WindowCreator<MissionInfoBtn>;
	m_CreateTable["MissionAttainStateWnd"]	= new WindowCreator<MissionAttainStateWnd>;
	m_CreateTable["MissionToolTip"]	= new WindowCreator<MissionToolTip>;
	m_CreateTable["MissionInfoRewardBtn"]	= new WindowCreator<MissionInfoRewardBtn>;
	m_CreateTable["MissionRadioBtn"]	= new WindowCreator<MissionRadioBtn>;
	
	// 누적 출석부
	m_CreateTable["RollBookWnd"] = new WindowCreator<RollBookWnd>;
	m_CreateTable["RollBookCloseBtn"] = new WindowCreator<RollBookCloseBtn>;
	m_CreateTable["RollBookToolTip"] = new WindowCreator<RollBookToolTip>;

	//길드 리뉴얼
	m_CreateTable["GuildPossibleRewardListWnd"] = new WindowCreator<GuildPossibleRewardListWnd>;
	m_CreateTable["RewardItemListIconBtn"] = new WindowCreator<RewardItemListIconBtn>;
	m_CreateTable["GuildAttendUserBtn"]   = new WindowCreator<GuildAttendUserBtn>;
	m_CreateTable["GuildAttendInfoListWnd"]   = new WindowCreator<GuildAttendInfoListWnd>;
	m_CreateTable["GuildAttendRewardWnd"]   = new WindowCreator<GuildAttendRewardWnd>;
	m_CreateTable["GuildRankRewardWnd"]   = new WindowCreator<GuildRankRewardWnd>;
	m_CreateTable["GuildEnableRewardToolTip"]   = new WindowCreator<GuildEnableRewardToolTip>;
	m_CreateTable["PopupStoreWnd"] = new WindowCreator<PopupStoreWnd>;
	m_CreateTable["PopupStoreSubWnd"] = new WindowCreator<PopupStoreSubWnd>;	

	//상점 개편
	m_CreateTable["ADTextPanelWnd"] = new WindowCreator<ADTextPanelWnd>;
	m_CreateTable["ADImagePanelWnd"] = new WindowCreator<ADImagePanelWnd>;

	m_CreateTable["NewShopStartWnd"] = new WindowCreator<NewShopStartWnd>;
	m_CreateTable["ioProductLinkButton"] = new WindowCreator<ioProductLinkButton>;
	m_CreateTable["ioProductIconButton"] = new WindowCreator<ioProductIconButton>;
	m_CreateTable["NewShopSearchWnd"] = new WindowCreator<NewShopSearchWnd>;
	m_CreateTable["SmallTabButton"] = new WindowCreator<SmallTabButton>;

	m_CreateTable["CenterKillInfoWnd"] = new WindowCreator<CenterKillInfoWnd>;

	m_CreateTable["NoviceGradeBuffBtn"] = new WindowCreator<NoviceGradeBuffBtn>;

	m_CreateTable["InventorySelectItemRadioButton"] = new WindowCreator<InventorySelectItemRadioButton>;
	m_CreateTable["InventorySelectItemBtn"] = new WindowCreator<InventorySelectItemBtn>;
	m_CreateTable["InventorySelectItemWnd"] = new WindowCreator<InventorySelectItemWnd>;
	m_CreateTable["InventorySelectItemToolTip"] = new WindowCreator<InventorySelectItemToolTip>;

	m_CreateTable["ItemNewMultipleCompoundWnd"] = new WindowCreator<ItemNewMultipleCompoundWnd>;

	m_CreateTable["NewShopItemPurchaseLeedWnd"] = new WindowCreator<NewShopItemPurchaseLeedWnd>;
	
	m_CreateTable["TrainingFuncMenuWnd"] = new WindowCreator<TrainingFuncMenuWnd>;
	m_CreateTable["TrainingFuncWnd"] = new WindowCreator<TrainingFuncWnd>;
	m_CreateTable["ioHousingBlockSetupWnd"] = new WindowCreator<ioHousingBlockSetupWnd>;
	m_CreateTable["ioHousingIconButton"] = new WindowCreator<ioHousingIconButton>;
	m_CreateTable["ioHousingBlockInvenWnd"] = new WindowCreator<ioHousingBlockInvenWnd>;
	m_CreateTable["ioHousingBlockInvenItemButton"] = new WindowCreator<ioHousingBlockInvenItemButton>;
	m_CreateTable["ioHousingBlockInvenIcon"] = new WindowCreator<ioHousingBlockInvenIcon>;
	m_CreateTable["ioHousingBlockInvenToolTip"] = new WindowCreator<ioHousingBlockInvenToolTip>;

	m_CreateTable["GuildHomeAdminInfoListWnd"] = new WindowCreator<GuildHomeAdminInfoListWnd>;
	m_CreateTable["GuildHomeAdminListBtn"] = new WindowCreator<GuildHomeAdminListBtn>;
	m_CreateTable["ioHousingShortCutKeyButton"] = new WindowCreator<ioHousingShortCutKeyButton>;
	m_CreateTable["GuildHQEtcItemNeedPopup"] = new WindowCreator<GuildHQEtcItemNeedPopup>;

	m_CreateTable["HouseInfoWnd"] = new WindowCreator<HouseInfoWnd>;
	m_CreateTable["HouseJoinRequestWnd"] = new WindowCreator<HouseJoinRequestWnd>;
	m_CreateTable["HouseMainOptionWnd"] = new WindowCreator<HouseMainOptionWnd>;
	m_CreateTable["HouseMainWnd"] = new WindowCreator<HouseMainWnd>;
	m_CreateTable["HouseInviteList"] = new WindowCreator<HouseInviteList>;
	m_CreateTable["HouseInviteListWnd"] = new WindowCreator<HouseInviteListWnd>;
	m_CreateTable["HouseInvitedBtn"] = new WindowCreator<HouseInvitedBtn>;
	m_CreateTable["HouseInvitedWnd"] = new WindowCreator<HouseInvitedWnd>;
	m_CreateTable["HomeEtcItemNeedPopup"] = new WindowCreator<HomeEtcItemNeedPopup>;	
	m_CreateTable["SingleSettingWnd"] = new WindowCreator<SingleSettingWnd>;

	m_CreateTable["NamedTitleBtn"]	= new WindowCreator<NamedTitleBtn>;
	m_CreateTable["NamedTitleResultWnd"] = new WindowCreator<NamedTitleResultWnd>;
	m_CreateTable["NamedTitlePremiumResultWnd"] = new WindowCreator<NamedTitlePremiumResultWnd>;
	m_CreateTable["NamedTitlePremiumSelectWnd"] = new WindowCreator<NamedTitlePremiumSelectWnd>;
	m_CreateTable["NamedTitleSelectWnd"] = new WindowCreator<NamedTitleSelectWnd>;
	m_CreateTable["FilterNamedTitleInfoWnd"] = new WindowCreator<FilterNamedTitleInfoWnd>;

	m_CreateTable["PriateNewStartButton"] = new WindowCreator<PriateNewStartButton>;
	m_CreateTable["OakHoleButton"] = new WindowCreator<OakHoleButton>;
	m_CreateTable["PriateRulletWnd"] = new WindowCreator<PriateRulletWnd>;
	m_CreateTable["PrivateRulletRewardButton"] = new WindowCreator<PrivateRulletRewardButton>;
	m_CreateTable["SwordFrameButton"] = new WindowCreator<SwordFrameButton>;
	m_CreateTable["PrivateRulletBonusRewardTooltip"] = new WindowCreator<PrivateRulletBonusRewardTooltip>;
	m_CreateTable["DevelopKBubble"] = new WindowCreator<DevelopKBubble>;
	m_CreateTable["PrivateRulletResetPopup"] = new WindowCreator<PrivateRulletResetPopup>;
	
	m_CreateTable["BonusCashWnd"] = new WindowCreator<BonusCashWnd>;
	m_CreateTable["BonusCashBtn"] = new WindowCreator<BonusCashBtn>;
	m_CreateTable["BonusCashProgressWnd"] = new WindowCreator<BonusCashProgressWnd>;
	m_CreateTable["BonusCashExpireWnd"] = new WindowCreator<BonusCashExpireWnd>;

	m_CreateTable["RaidReadyCountWnd"] = new WindowCreator<RaidReadyCountWnd>;
	m_CreateTable["TotalResultRaidCardBtn"] = new WindowCreator<TotalResultRaidCardBtn>;
	m_CreateTable["TotalResultRaidCardWnd"] = new WindowCreator<TotalResultRaidCardWnd>;
	m_CreateTable["TotalResultSpiritRewardWnd"] = new WindowCreator<TotalResultSpiritRewardWnd>;

	//Accessory
	m_CreateTable["AccessorySkillListWnd"]	 = new WindowCreator<AccessorySkillListWnd>;
	m_CreateTable["AccessorySkillListBtn"]	 = new WindowCreator<AccessorySkillListBtn>;
	m_CreateTable["ioAccessoryReleseWnd"] = new WindowCreator<ioAccessoryReleseWnd>;

	//오크통 리뉴얼
	m_CreateTable["RenewalPriateNewStartButton"] = new WindowCreator<RenewalPriateNewStartButton>;
	m_CreateTable["RenewalOakHoleButton"] = new WindowCreator<RenewalOakHoleButton>;
	m_CreateTable["PriateRulletRenewalWnd"] = new WindowCreator<PriateRulletRenewalWnd>;
	m_CreateTable["RenewalPrivateRulletRewardButton"] = new WindowCreator<RenewalPrivateRulletRewardButton>;
	m_CreateTable["RenewalDevelopKBubble"] = new WindowCreator<RenewalDevelopKBubble>;
	m_CreateTable["PriateTextTooltip"] = new WindowCreator<PriateTextTooltip>;

	// 매칭 게임
	m_CreateTable["MatchingGameWnd"] = new WindowCreator<MatchingGameWnd>;
	m_CreateTable["MatchingGameCardButton"] = new WindowCreator<MatchingGameCardButton>;

	// 뱀주사위 게임
	m_CreateTable["DiceGameWnd"] = new WindowCreator<DiceGameWnd>;
	m_CreateTable["DiceRewardBtn"] = new WindowCreator<DiceRewardBtn>;
	m_CreateTable["DiceItemEffect"] = new WindowCreator<DiceItemEffect>;
	m_CreateTable["DiceToolTip"] = new WindowCreator<DiceToolTip>;
	m_CreateTable["DiceDevK"] = new WindowCreator<DiceDevK>;
	m_CreateTable["DiceDisabledBack"] = new WindowCreator<DiceDisabledBack>;

	m_CreateTable["SuccessionBattleWnd"] = new WindowCreator<SuccessionBattleWnd>;
	m_CreateTable["SuccessionBattleRequestWnd"] = new WindowCreator<SuccessionBattleRequestWnd>;
	m_CreateTable["SuccessionBattleReplyWnd"] = new WindowCreator<SuccessionBattleReplyWnd>;
	m_CreateTable["SuccessionMatchVersusSceneWnd"] = new WindowCreator<SuccessionMatchVersusSceneWnd>;
	m_CreateTable["WaitEffectWnd"] = new WindowCreator<WaitEffectWnd>;
	
	m_CreateTable["AlchemicChangeAlarmWnd"] = new WindowCreator<AlchemicChangeAlarmWnd>;

	//수련장
	m_CreateTable["CountUpWnd"] = new WindowCreator<CountUpWnd>;
	m_CreateTable["StartingSignalWnd"] = new WindowCreator<StartingSignalWnd>;
	m_CreateTable["PracticeInfoWnd"]	= new WindowCreator<PracticeInfoWnd>;
	m_CreateTable["PracticeResultWnd"]	= new WindowCreator<PracticeResultWnd>;
	m_CreateTable["PracticeUpdateRank"]	= new WindowCreator<PracticeUpdateRank>;
	m_CreateTable["PracticeMainWnd"] = new WindowCreator<PracticeMainWnd>;
	m_CreateTable["PracticeLobbyInfoWnd"]	= new WindowCreator<PracticeLobbyInfoWnd>;
	m_CreateTable["PracticeBtn"] = new WindowCreator<PracticeBtn>;
	m_CreateTable["PracticeToolTip"] = new WindowCreator<PracticeToolTip>;
	m_CreateTable["PracticeRankingWnd"] = new WindowCreator<PracticeRankingWnd>;
	m_CreateTable["RetryModeWnd"] = new WindowCreator<RetryModeWnd>;

	// 검색 2019-05-21 
	m_CreateTable["NewSearchWnd"] = new WindowCreator<NewSearchWnd>;

	ioGUIRegister::Register( this );
}

ioUIRenderFrame *ioSP2GUIManager::CreateFrame( const ioHashString &rkFrameName )
{
	if( m_pUIFrameMgr )
		return m_pUIFrameMgr->CloneFrame( rkFrameName );
	return NULL;
}

bool ioSP2GUIManager::IsEscKeyCheck()
{
	if( IsShowSettingKeyWnd() )
		return false;
	if( IsNoneUIMode() )
		return false;
	if( m_pPlayStage )
	{
		if( m_pPlayStage->HasCameraFixedStyle( CAMERA_STYLE_MOVIE ) )
			return false;
		else if( m_pPlayStage->HasCameraFixedStyle( CAMERA_STYLE_HOUSING ) )
			return false;
	}
	return true;
}

bool ioSP2GUIManager::IsSpaceBarKeyCheck()
{
	if( IsShowSettingKeyWnd() )
		return false;
	if( IsNoneUIMode() )
		return false;
	return true;
}

void ioSP2GUIManager::ProcessGUI( ioMouse &mouse )
{
	ioGUIManager::ProcessGUI( mouse );

	bool bShowSettingKeyWnd = IsShowSettingKeyWnd();

	// ESC 
	if( IsEscKeyCheck() )
	{
		if( g_Input.IsKeyUp( KEY_ESCAPE ) )
		{
			if( g_BrowserMgr.CloseBrowserByType( BROWSE_SINGLE ) )
			{
				// do nothing...
			}
			else if( g_BrowserMgr.CloseBrowserByType( BROWSE_GUILD_MARK ) )
			{
				// do nothins..
			}			
			else if( m_pDeskTopWnd )
			{
				ioWnd *pReturnWnd = m_pDeskTopWnd->HideChildByEsc();
				if( pReturnWnd == NULL )
				{
					// 하단바 작동
					HelpWnd *pHelpWnd = dynamic_cast<HelpWnd*>(FindWnd( HELP_WND ));
					if( pHelpWnd )
					{
						pHelpWnd->EscKeyProcess();
					}
				}
				else
				{
					//LOG.PrintTimeAndLog( 0, "Close WND %d", pReturnWnd->GetID() );
				}
			}
		}
	}

	// SPACE BAR
	if( IsSpaceBarKeyCheck() )
	{
		if( g_Input.IsKeyUp( KEY_SPACE ) )
		{
			if( m_pDeskTopWnd )
			{
				m_pDeskTopWnd->CommandChildBySpaceBar();
			}
		}
	}

	if( !bShowSettingKeyWnd )
	{
		if(	g_Input.IsKeyPress(KEY_LWIN) || g_Input.IsKeyPress(KEY_RWIN) )
		{
			
		}
	}

	SetSelectedWnd();
}

void ioSP2GUIManager::SetUIPostionToDefault()
{
	if( m_pDeskTopWnd )
	{
		m_pDeskTopWnd->SetPositionToDefault( Setting::Width(), Setting::Height(), true );
	}

	// 윈도우가 없는 경우 bool 변수에 체크만 해놓았다가 실재 생성시에 설정한다.
	if( FindWnd( TIME_SCORE_WND ) == NULL )
		TimeScoreWnd::SetPosToDefualt( true );
	if( FindWnd( OBSERVER_RED_INFO_WND) == NULL )
		ioObserverRedWnd::SetPosToDefualt( true );
	if( FindWnd( ROULETTE_WND ) == NULL )
		RouletteWnd::SetPosToDefualt( true );
}

ioWnd* ioSP2GUIManager::CreateNewWindow( const ioHashString &szClassName )
{
	ioWnd *pWnd = ioGUIManager::CreateNewWindow( szClassName );
	if( pWnd )
		return pWnd;

	WindowCreateTable::iterator iter = m_CreateTable.find( szClassName );
	if( iter != m_CreateTable.end() )
	{
		return iter->second->CreateWnd();
	}

	return NULL;
}

void ioSP2GUIManager::RenderBackMostWindows()
{
	if( !m_pDeskTopWnd )	return;

	bool bNoRenderGuide = IsShow( PRACTICE_HELP_WND );
	int iNumChild = m_pDeskTopWnd->GetNumChildren();
	for( int i=0 ; i<iNumChild ; i++ )
	{
		ioWnd *pWnd = m_pDeskTopWnd->GetChild( i );
		if( pWnd && pWnd->HasWndStyle( IWS_BACKMOST ) )
		{
			pWnd->RenderWnd();
			
			if( !bNoRenderGuide )
				g_QuestGuide.RenderGuide( pWnd->GetID() );
		}
	}
}

void ioSP2GUIManager::RenderNotBackMostWindows()
{
	if( !m_pDeskTopWnd )	return;

	bool bRender = false;
	bool bNoRenderGuide = IsShow( PRACTICE_HELP_WND );
	int iNumChild = m_pDeskTopWnd->GetNumChildren();
	for( int i=0 ; i<iNumChild ; i++ )
	{
		ioWnd *pWnd = m_pDeskTopWnd->GetChild( i );
		if( pWnd && !pWnd->HasWndStyle( IWS_BACKMOST ) )
		{
			ErrorReport::SetPosition( 1130, 1 );

			bRender = CheckAndRenderModalBackGround(bRender, pWnd);

			ErrorReport::SetPosition( 1130, 2 );
			ErrorReport::SetAndSoOnInfo( pWnd->GetID());
			pWnd->RenderWnd();

			ErrorReport::SetPosition( 1130, 3 );

			if( !bNoRenderGuide )
				g_QuestGuide.RenderGuide( pWnd->GetID() );

			ErrorReport::SetPosition( 1130, 4 );
		}
	}
	g_UIGuide.Render();
}

ioWnd* ioSP2GUIManager::AddWndWithID( const char *szXMLFileName, DWORD dwID )
{
	ioWnd *pWnd = AddWnd( szXMLFileName );
	if( pWnd )
		pWnd->SetID( dwID );

	return pWnd;
}

ioWnd* ioSP2GUIManager::AddWndWithID( const char *szXMLFileName, ioWnd *pParent, DWORD dwID )
{
	ioWnd *pWnd = AddWnd( szXMLFileName, pParent );
	if( pWnd )
		pWnd->SetID( dwID );

	return pWnd;
}

void ioSP2GUIManager::AddAndSetMsgBox( int iType, ioWnd *pPrevWnd, ioWnd *pWnd )
{
	ioMessageBox *pBox = dynamic_cast<ioMessageBox*>( FindWnd( MESSAGE_BOX ) );
	if( !pBox )
	{
		AddWnd( "XML/MessageBox.xml" );
	}

	pBox = dynamic_cast<ioMessageBox*>( FindWnd( MESSAGE_BOX ) );
	if( pBox )
	{
		if( !pPrevWnd )
		{
			if( pBox->IsShow() )
				pBox->HideWnd();
		}

		pBox->SetMessage( iType, m_szMsgBuf, pPrevWnd, pWnd );
		m_pMsgReturn = pWnd;
	}
}

#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
void ioSP2GUIManager::SetMsgBox( int iType, ioWnd *pWnd, const char *szSource, ... )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		va_list args;
		va_start( args, szSource );
		StringCbVPrintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, args );
		va_end( args );
		AddAndSetMsgBox( iType, NULL, pWnd );
	}
	else
	{
		SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource );
		AddAndSetMsgBox( iType, NULL, pWnd );
	}
}
#else
void ioSP2GUIManager::SetMsgBox(int iType, ioWnd *pWnd, const char *szSource )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource );
	AddAndSetMsgBox( iType, NULL, pWnd );
}
#endif

void ioSP2GUIManager::SetMsgBox(int iType, ioWnd *pWnd, IoString* pSource)
{
	ioMessageBox *pBox = (ioMessageBox*)FindWnd( MESSAGE_BOX );
	if( !pBox )
	{
		AddWnd( "XML/MessageBox.xml" );
	}

	pBox = (ioMessageBox*)FindWnd( MESSAGE_BOX );
	if( pBox )
	{
 		pBox->SetIoStringMessage(iType, pSource, NULL, pWnd);
 		m_pMsgReturn = pWnd;
	}
}

void ioSP2GUIManager::SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, int iArg1 )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, iArg1 );
	AddAndSetMsgBox( iType, NULL, pWnd );
}

void ioSP2GUIManager::SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, DWORD dwArg1 )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, dwArg1 );
	AddAndSetMsgBox( iType, NULL, pWnd );
}

void ioSP2GUIManager::SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, __int64 i64Arg1 )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, i64Arg1 );
	AddAndSetMsgBox( iType, NULL, pWnd );
}

void ioSP2GUIManager::SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, const char *szArg1 )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, szArg1 );
	AddAndSetMsgBox( iType, NULL, pWnd );
}

void ioSP2GUIManager::SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, int iArg1, int iArg2 )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, iArg1, iArg2 );
	AddAndSetMsgBox( iType, NULL, pWnd );
}

void ioSP2GUIManager::SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, int iArg1, const char *szArg2 )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, iArg1, szArg2 );
	AddAndSetMsgBox( iType, NULL, pWnd );
}

void ioSP2GUIManager::SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, const char *szArg1, int iArg2 )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, szArg1, iArg2 );
	AddAndSetMsgBox( iType, NULL, pWnd );
}

void ioSP2GUIManager::SetMsgBox( int iType, ioWnd *pWnd, const char *szSource, const char *szArg1, __int64 i64Arg2 )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, szArg1, i64Arg2 );
	AddAndSetMsgBox( iType, NULL, pWnd );
}

void ioSP2GUIManager::SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, const char *szArg1, const char *szArg2 )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, szArg1, szArg2 );
	AddAndSetMsgBox( iType, NULL, pWnd );
}

void ioSP2GUIManager::SetMsgBox( int iType, ioWnd *pWnd, const char *szSource, int iArg1, int iArg2, int iArg3 )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, iArg1, iArg2, iArg3 );
	AddAndSetMsgBox( iType, NULL, pWnd );
}

void ioSP2GUIManager::SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, int iArg1, const char *szArg2, int iArg3 )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, iArg1, szArg2, iArg3 );
	AddAndSetMsgBox( iType, NULL, pWnd );
}

void ioSP2GUIManager::SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, const char *szArg1, int iArg2, const char *szArg3 )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, szArg1, iArg2, szArg3 );
	AddAndSetMsgBox( iType, NULL, pWnd );
}

void ioSP2GUIManager::SetMsgBox( int iType, ioWnd *pWnd, const char *szSource, int iArg1, int iArg2, int iArg3, int iArg4 )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, iArg1, iArg2, iArg3, iArg4 );
	AddAndSetMsgBox( iType, NULL, pWnd );
}

void ioSP2GUIManager::SetMsgBox(int iType, ioWnd *pWnd, const char *szSource, const char *szArg1, const char *szArg2, int iArg3, const char *szArg4 )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, szArg1, szArg2, iArg3, szArg4 );
	AddAndSetMsgBox( iType, NULL, pWnd );
}

#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
void ioSP2GUIManager::SetPrevMsgBox( ioWnd *pPrevWnd, int iType, ioWnd *pWnd, const char *szSource, ... )
{
	if( ioLocalManagerParent::IsVPrintf() )
	{
		va_list args;
		va_start( args, szSource );
		StringCbVPrintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, args );
		va_end( args );
		AddAndSetMsgBox( iType, pPrevWnd, pWnd );
	}
	else
	{
		SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource );
		AddAndSetMsgBox( iType, pPrevWnd, pWnd );
	}
}
#else
void ioSP2GUIManager::SetPrevMsgBox( ioWnd *pPrevWnd, int iType, ioWnd *pWnd, const char *szSource )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource );
	AddAndSetMsgBox( iType, pPrevWnd, pWnd );
}
#endif

void ioSP2GUIManager::SetPrevMsgBox( ioWnd *pPrevWnd, int iType, ioWnd *pWnd, const char *szSource, const char *szArg1 )
{
	SafeSprintf( m_szMsgBuf, MSG_BUF_SIZE, szSource, szArg1 );
	AddAndSetMsgBox( iType, pPrevWnd, pWnd );
}


void ioSP2GUIManager::SetPrevMsgListBox( ioWnd *pPrevWnd,
										 int iType,
										 ioWnd *pWnd,
										 ioComplexStringPrinter *MsgPrinter )
{
	ioMessageBox3 *pBox = dynamic_cast<ioMessageBox3*>( FindWnd( MESSAGE_BOX3 ) );
	if( !pBox )
	{
		AddWnd( "XML/MessageBox3.xml" );
	}
	
	pBox = dynamic_cast<ioMessageBox3*>( FindWnd( MESSAGE_BOX3 ) );
	if( pBox )
	{
		pBox->SetTitleText( STR(1) );
		pBox->SetBtnTitle( STR(2), STR(3), STR(4) );
		pBox->SetAlignType( TAT_CENTER );
		pBox->SetPrintMessage( iType, MsgPrinter, pPrevWnd, pWnd );
		m_pMsgReturn = pWnd;
	}
}


void ioSP2GUIManager::SetPrevMsgListBoxWithTitle( ioWnd *pPrevWnd, 
												    int iType,
													ioWnd *pWnd,
													ioComplexStringPrinter *MsgPrinter,
													const char *szTitle,
													const char *szOKTitle,
													const char *szYesTitle,
													const char *szNoTitle, 
													TextAlignType eAlignType /*= TAT_CENTER*/ )
{
	ioMessageBox3 *pBox = dynamic_cast<ioMessageBox3*>( FindWnd( MESSAGE_BOX3 ) );
	if( !pBox )
	{
		AddWnd( "XML/MessageBox3.xml" );
	}

	pBox = dynamic_cast<ioMessageBox3*>( FindWnd( MESSAGE_BOX3 ) );
	if( pBox )
	{
		pBox->SetTitleText( szTitle );
		pBox->SetBtnTitle( szOKTitle, szYesTitle, szNoTitle );
		pBox->SetAlignType( eAlignType );
		pBox->SetPrintMessage( iType, MsgPrinter, pPrevWnd, pWnd );
		m_pMsgReturn = pWnd;
	}
}

void ioSP2GUIManager::SetExpandMedalSlotMsgBox( ioWnd *pPrevWnd, 
												ioWnd *pWnd,
												int iSlotNumber,
												TextAlignType eAlignType /*= TAT_CENTER*/ )
{
	ioMessageBox4 *pBox = dynamic_cast< ioMessageBox4* >( FindWnd( MESSAGE_BOX4 ) );
	if( !pBox )
	{
		AddWnd( "XML/MessageBox4.xml" );
	}

	pBox = dynamic_cast< ioMessageBox4* >( FindWnd( MESSAGE_BOX4 ) );
	if( pBox )
	{
		pBox->SetInfo( iSlotNumber );
		pBox->SetAlignType( eAlignType );
		pBox->SetPrintMessage( pPrevWnd, pWnd );
	}
}

void ioSP2GUIManager::SetFishingMsgListBox( ioWnd *pWnd, int iStartArray )
{
	ioFishingMessageBox *pBox = dynamic_cast<ioFishingMessageBox*>( FindWnd( FISHING_MESSAGE_BOX ) );
	if( !pBox )
	{
		AddWnd( "xml/fishingmessagebox.xml" );
	}

	pBox = dynamic_cast<ioFishingMessageBox*>( FindWnd( FISHING_MESSAGE_BOX ) );
	if( pBox )
	{
		pBox->SetPrintMessage( iStartArray, pWnd );
	}
}


void ioSP2GUIManager::SetFishingLevelUpBox( ioWnd *pWnd, int iCurLevel, bool bAddSlot )
{
	ioFishingLevelUpWnd *pBox = dynamic_cast<ioFishingLevelUpWnd*>( FindWnd( FISHING_LEVEL_UP_BOX ) );
	if( !pBox )
	{
		AddWnd( "xml/fishinglevelupbox.xml" );
	}

	pBox = dynamic_cast<ioFishingLevelUpWnd*>( FindWnd( FISHING_LEVEL_UP_BOX ) );
	if( pBox )
	{
		pBox->SetPrintMessage( iCurLevel, bAddSlot, pWnd );
	}
}

void ioSP2GUIManager::SetPrevMsgListPinkBoxWithTitle( ioWnd *pPrevWnd, 
													  int iType,
													  ioWnd *pWnd,
													  ioComplexStringPrinter *MsgPrinter,
												      const char *szTitle,
												      const char *szOKTitle,
												      const char *szYesTitle,
												      const char *szNoTitle, 
												      TextAlignType eAlignType /*= TAT_CENTER*/ )
{
	ioMessageBox3 *pBox = dynamic_cast<ioMessageBox3*>( FindWnd( PINK_MESSAGE_BOX ) );
	if( !pBox )
	{
		AddWnd( "XML/PinkMessageBox.xml" );
	}

	pBox = dynamic_cast<ioMessageBox3*>( FindWnd( PINK_MESSAGE_BOX ) );
	if( pBox )
	{
		pBox->SetTitleText( szTitle );
		pBox->SetBtnTitle( szOKTitle, szYesTitle, szNoTitle );
		pBox->SetAlignType( eAlignType );
		pBox->SetPrintMessage( iType, MsgPrinter, pPrevWnd, pWnd );
		m_pMsgReturn = pWnd;
	}
}

void ioSP2GUIManager::SetReserveMsgBox( int iMsgType )
{
	for( IntVec::iterator iter = m_vReserveMsgBox.begin(); iter != m_vReserveMsgBox.end(); ++iter )
	{
		if( *iter == iMsgType || *iter == MB_UDP_FAIL_EXIT_ROOM )
		{
			//이미 예약되있는 메시지나 MB_UDP_FAIL_EXIT_ROOM가 예약되어있다면 무시
			return;
		}
	}

	m_vReserveMsgBox.push_back( iMsgType );
}

void ioSP2GUIManager::AddReserverShowUI( DWORD dwAddUI )
{
	m_dwReserveShowUI |= dwAddUI;
}

void ioSP2GUIManager::NotifyMsgBoxClose( ioWnd *pMsgBox, int iPushed )
{
	if( m_pMsgReturn )
	{
		m_pMsgReturn->iwm_command( pMsgBox, IOBN_BTNUP, iPushed );
		m_pMsgReturn = NULL;
	}
}

void ioSP2GUIManager::NotifyMsgBoxCommand( ioWnd *pMsgBox, int iPushed )
{
	if( m_pMsgReturn )
	{
		m_pMsgReturn->iwm_command( pMsgBox, IOBN_BTNUP, iPushed );
	}
}

void ioSP2GUIManager::ShowReserveMsgBox( int iReserveMsgBox /* = -1  */ )
{
	if( m_vReserveMsgBox.empty() )
		return;

	int iCurrMsgBox = iReserveMsgBox;
	for( IntVec::iterator iter = m_vReserveMsgBox.begin(); iter != m_vReserveMsgBox.end(); )
	{
		if( iReserveMsgBox == -1 )
		{
			iCurrMsgBox = *iter;
		}
		else
		{
			iCurrMsgBox = iReserveMsgBox;
		}

		if( *iter == iCurrMsgBox )
		{
			ioComplexStringPrinter kPrinter[MAX_MSG_LIST_SIZE];

			switch( iCurrMsgBox )
			{
			case MB_RESERVE_NO_ROOM:
				SetMsgBox( MB_OK, NULL, STR(1) );
				break;
			case MB_RESERVE_FULL_ROOM:
				SetMsgBox( MB_OK, NULL, STR(2) );
				break;
			case MB_RESERVE_EXCEPTION_BATTLE_ROOM:
				SetMsgBox( MB_OK, NULL, "룸에 입장할 수 없습니다." );
				break;
			case MB_RESERVE_SPEEDHACK:
				kPrinter[0].SetTextStyle( TS_NORMAL );
				kPrinter[0].SetBkColor( 0, 0, 0 );	
				kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
				kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(3) );
				kPrinter[1].AddTextPiece( FONT_SIZE_13,STR(27), g_MyInfo.GetKickSubInfo().c_str() );

				SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
				g_MyInfo.ClearKickedInfo();
				break;
			case MB_RESERVE_ABUSE:
			case MB_RESERVE_SPEEDHACK_PENALTY:
				{
					kPrinter[0].SetTextStyle( TS_NORMAL );
					kPrinter[0].SetBkColor( 0, 0, 0 );	
					kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
					kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(4) );

					kPrinter[1].SetTextStyle( TS_NORMAL );
					kPrinter[1].SetBkColor( 0, 0, 0 );	
					kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
					kPrinter[1].AddTextPiece( FONT_SIZE_13,STR(28), g_MyInfo.GetKickSubInfo().c_str() );

					kPrinter[2].SetTextStyle( TS_NORMAL );
					kPrinter[2].SetBkColor( 0, 0, 0 );	
					kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
					if( m_iPenaltyLadderPoint > 0 )
						kPrinter[2].AddTextPiece( FONT_SIZE_13,STR(5), m_iPenaltyPeso, m_iPenaltyLadderPoint );
					else
						kPrinter[2].AddTextPiece( FONT_SIZE_13,STR(6), m_iPenaltyPeso );

					SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
					g_MyInfo.ClearKickedInfo();

#if defined( USE_GA )
					char chLabel[32] = {0,};

					if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
						sprintf_e( chLabel, "%d", m_iPenaltyPeso );
					else
						SafeSprintf( chLabel, sizeof(chLabel), "%1", m_iPenaltyPeso );

					// PESO_USE_PENALTY
					g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
						, "Peso"
						, "Penalty"
						, chLabel
						, 1
						, "%2FPESO%2FUSE%2FPENALTY" );
#endif
				}
				break;
			case MB_RESERVE_OPEN_SPACE_ABUSE:
				kPrinter[0].SetTextStyle( TS_NORMAL );
				kPrinter[0].SetBkColor( 0, 0, 0 );	
				kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
				kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(7) );

				kPrinter[1].SetTextStyle( TS_NORMAL );
				kPrinter[1].SetBkColor( 0, 0, 0 );	
				kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
				kPrinter[1].AddTextPiece( FONT_SIZE_13,STR(8) );

				kPrinter[2].SetTextStyle( TS_NORMAL );
				kPrinter[2].SetBkColor( 0, 0, 0 );	
				kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
				kPrinter[2].AddTextPiece( FONT_SIZE_13,STR(29), g_MyInfo.GetKickSubInfo().c_str() );

				kPrinter[3].SetTextStyle( TS_NORMAL );
				kPrinter[3].SetBkColor( 0, 0, 0 );	
				kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
				kPrinter[3].AddTextPiece( FONT_SIZE_13,STR(9) );

				SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
				g_MyInfo.ClearKickedInfo();
				break;
			case MB_INFOMATION_ALTTAB:
				SetMsgBox( MB_OK, NULL, STR(10) );
				break;
			case MB_INFOMATION_PENALTY:
				{
					kPrinter[0].SetTextStyle( TS_NORMAL );
					kPrinter[0].SetBkColor( 0, 0, 0 );	
					kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
					kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(11) );

					kPrinter[1].SetTextStyle( TS_NORMAL );
					kPrinter[1].SetBkColor( 0, 0, 0 );	
					kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
					if( m_iPenaltyLadderPoint > 0 )
						kPrinter[1].AddTextPiece( FONT_SIZE_13,STR(12), m_iPenaltyPeso, m_iPenaltyLadderPoint );
					else
						kPrinter[1].AddTextPiece( FONT_SIZE_13,STR(13), m_iPenaltyPeso );

					SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );

#if defined( USE_GA )
					char chLabel[32] = {0,};

					if ( ioLocalManager::GetLocalType() == ioLocalManager::LCT_KOREA )
						sprintf_e( chLabel, "%d", m_iPenaltyPeso );
					else
						SafeSprintf( chLabel, sizeof(chLabel), "%1", m_iPenaltyPeso );

					// PESO_USE_PENALTY
					g_HttpMng.GA_EventTracking( g_MyInfo.GetUserIndex()
						, "Peso"
						, "Penalty"
						, chLabel
						, 1
						, "%2FPESO_USE_PENALTY" );
#endif

				}
				break;
			case MB_LEAVE_SAFE_ROOM:
				kPrinter[0].SetTextStyle( TS_NORMAL );
				kPrinter[0].SetBkColor( 0, 0, 0 );	
				kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
				kPrinter[0].AddTextPiece( FONT_SIZE_13,STR(14), g_MyInfo.GetPublicID().c_str() );

				kPrinter[1].SetTextStyle( TS_NORMAL );
				kPrinter[1].SetBkColor( 0, 0, 0 );	
				kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
				kPrinter[1].AddTextPiece( FONT_SIZE_13,STR(15) );

				kPrinter[2].SetTextStyle( TS_NORMAL );
				kPrinter[2].SetBkColor( 0, 0, 0 );	
				kPrinter[2].SetTextColor( TCT_DEFAULT_GRAY );
				kPrinter[2].AddTextPiece( FONT_SIZE_13,STR(16) );

				kPrinter[3].SetTextStyle( TS_NORMAL );
				kPrinter[3].SetBkColor( 0, 0, 0 );	
				kPrinter[3].SetTextColor( TCT_DEFAULT_GRAY );
				kPrinter[3].AddTextPiece( FONT_SIZE_13,STR(17) );
				SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
				break;
			case MB_PLAZA_KICK_OUT:
				SetMsgBox( MB_OK, NULL, STR(18) );
				break;
			case MB_SOLDIER_LIMIT:
				SetMsgBox( MB_OK, NULL, STR(19) );
				break;
			case MB_ENTRY_NOT_FORMALITY:
				SetMsgBox( MB_OK, NULL, STR(20) );
				break;
			case MB_BATTLE_ROOM_KICK_OUT:
				SetMsgBox( MB_OK, NULL, STR(21) );
				break;
			case MB_LADDER_TEAM_KICK_OUT:
				SetMsgBox( MB_OK, NULL, STR(22) );
				break;
			case MB_UDP_FAIL_EXIT_ROOM:
				SetMsgBox( MB_OK, NULL, "룸에서 핑이 좋지않아 #광장으로 소환되었습니다." );
				break;
			case MB_RESERVED_CHANGE_ID:
				SetMsgBox( MB_OK, NULL, STR(24));
				break;
			case MB_VOTE_KICK_OUT:
				{
					kPrinter[0].SetTextStyle( TS_NORMAL );
					kPrinter[0].SetBkColor( 0, 0, 0 );	
					kPrinter[0].SetTextColor( TCT_DEFAULT_GRAY );
					kPrinter[0].AddTextPiece( FONT_SIZE_13, g_MyInfo.GetVoteKickReason().c_str() );

					kPrinter[1].SetTextStyle( TS_NORMAL );
					kPrinter[1].SetBkColor( 0, 0, 0 );	
					kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
					kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(30) );

					SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
					//SetMsgBox( MB_OK, NULL, STR(25) );
				}
				break;
			case MB_CAMP_BATTLE_END:
				SetMsgBox( MB_OK, NULL, STR(26), g_CampMgr.GetPlayPreceedMent().c_str() );
				break;
			case MB_MONSTER_COIN_LACK:
				SetMsgBox( MB_OK, NULL, STR(31) );
				break;
			case MB_RAID_COIN_LACK:
				SetMsgBox( MB_OK, NULL, "레이드 티켓이 부족하여 방에서 이탈되었습니다.#(이탈 페널티는 적용되지 않았습니다.)" );
				break;
			case MB_RESERVE_BAD_NETWORK_KICK:
				{
					kPrinter[0].SetTextStyle( TS_NORMAL );
					kPrinter[0].SetBkColor( 0, 0, 0 );	
					kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
					kPrinter[0].AddTextPiece( FONT_SIZE_13, STR(26) );

					kPrinter[1].SetTextStyle( TS_NORMAL );
					kPrinter[1].SetBkColor( 0, 0, 0 );	
					kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
					kPrinter[1].AddTextPiece( FONT_SIZE_13, STR(32) );

					SetPrevMsgListBox( NULL, MB_OK, NULL, kPrinter );
				}
				break;
			case MB_HEADQUARTERS_KICK_OUT:
				SetMsgBox( MB_OK, NULL, STR(33) );
				break;
			case MB_HEADQUARTERS_OUT_MASTER_LOGOUT:
				SetMsgBox( MB_OK, NULL, STR(34) );
				break;
			case MB_HEADQUARTERS_OUT_FRIEND_DELETE:
				SetMsgBox( MB_OK, NULL, STR(35) );
				break;
			case MB_SERVER_MOVE_DELAY:
				SetMsgBox( MB_OK, NULL, STR(36) );
				break;
			case MB_SERVER_MOVE_NO_ROOM:
				SetMsgBox( MB_OK, NULL, STR(37) );
				break;
			case MB_RESERVE_MACRO_OUT:
				SetMsgBox( MB_OK, NULL, STR(38) );
				break;
			case MB_RELAYSVR_DISCONNECTED: // for relay		
				SetMsgBox( MB_OK, NULL, STR(26) );		
				break;
			case MB_SHUFFLE_ROOM_KICK_OUT:
				SetMsgBox( MB_OK, NULL, "실력이 너무 뛰어나 유저보호를 위해 전투방에서#이탈되셨습니다." );		
				break;
			case MB_BATTLEROOM_JOIN_SEARCH_FAILED:
				SetMsgBox( MB_OK, NULL, "참여 가능한 전투방이 없습니다." );
				break;
			case MB_BATTLEROOM_JOIN_NOT_NODE:
				SetMsgBox( MB_OK, NULL, "존재하지 않는 전투방입니다." );
				break;
			case MB_BATTLEROOM_JOIN_FULL_USER:
				SetMsgBox( MB_OK, NULL, "인원이 초과 되었습니다." );
				break;
			case MB_BATTLEROOM_JOIN_NOT_PASSWORD:
				SetMsgBox( MB_OK, NULL, "비밀번호가 정확하지 않습니다" );
				break;
			case MB_BATTLEROOM_JOIN_EXCEPTION:
				SetMsgBox( MB_OK, NULL, "예외 오류가 발생되었습니다." );
				break;
			case MB_BATTLEROOM_JOIN_TIME_CLOSE:
				SetMsgBox( MB_OK, NULL, "플레이가 많이 진행되어서#참가할 수 없습니다." );
				break;
			case MB_BATTLEROOM_JOIN_START_ROOM_ENTER_X:
				SetMsgBox( MB_OK, NULL, "도중참가 불가로 설정되어 #참가할 수 없습니다." );
				break;
			case MB_BATTLEROOM_JOIN_SAFETY_ROOM:
				SetMsgBox( MB_OK, NULL, "데스매치 - 초보 모드에 참가할 수 없습니다." );
				break;
			case MB_BATTLEROOM_JOIN_MAP_LIMIT_PLAYER:
				SetMsgBox( MB_OK, NULL, "맵 최대 인원이 초과 되었습니다." );
				break;
			case MB_BATTLEROOM_JOIN_MAP_LIMIT_GRADE:
				SetMsgBox( MB_OK, NULL, "계급 제한에 걸려 입장 할 수 없습니다" );
				break;
			case MB_BATTLEROOM_JOIN_NO_CHALLENGER:
				SetMsgBox( MB_OK, NULL, "도전자를 받지 않습니다." );
				break;
			case MB_HOUSE_KICK_OUT:
				SetMsgBox( MB_OK, NULL, "개인본부에서 강제퇴장#되었습니다." );
				break;
			case MB_HOUSE_MASTER_OUT:
				SetMsgBox( MB_OK, NULL, "주인이 개인본부에서 퇴장하여#개인본부에서 이탈되었습니다." );
				break;
			case MB_ROOM_EXCEPTION:
				SetMsgBox( MB_OK, NULL, "예외 오류가 발생되었습니다." );
				break;
			case MB_FLAG_MODE_ERROR:
				SetMsgBox( MB_OK, NULL, STR(39) );
				break;
			}

			iter = m_vReserveMsgBox.erase( iter );
			m_iPenaltyPeso = m_iPenaltyLadderPoint = 0;
		}
		else
		{
			++iter;
		}
	}	
}

void ioSP2GUIManager::SetNoneUIMode( bool bNoneUI )
{
	m_bNoneUIMode = bNoneUI;
	
	if( m_pPlayStage )
	{
		if( bNoneUI && Setting::MovieModeUIHide() )
			m_pPlayStage->SetScreenShotMode( bNoneUI );
		else if( !bNoneUI )
			m_pPlayStage->SetScreenShotMode( bNoneUI );
	}
}

void ioSP2GUIManager::ShowReserveUI()
{
	if( m_dwReserveShowUI & RESERVE_LADDER_TEAM_UI )
		ShowWnd( LADDER_BATTLE_WND );
	m_dwReserveShowUI = 0;
}

void ioSP2GUIManager::CheckGUIPosSave()
{
	char fName[MAX_PATH];
	sprintf( fName, "Save\\%s\\window.ini", g_MyInfo.GetPublicID().c_str() );
	
	FILE *fp = fopen( fName, "r" );
	if( fp == NULL )
	{
		sprintf( fName, "Save\\%s" , g_MyInfo.GetPublicID().c_str() );
		CreateDirectory("Save",NULL);
		CreateDirectory(fName,NULL);
		sprintf(fName,"Save\\%s\\window.ini",g_MyInfo.GetPublicID().c_str() );

		fp = fopen( fName, "w" );
		if( fp == NULL )
		{
			LOG.PrintTimeAndLog( 0, "GUI File Create Failed." );
			return;
		}
	}

	fclose( fp );
}

int ioSP2GUIManager::GUIPosLoadInt( const char *szTitle, const char *szKeyName, int iValue ,int iVersion /*= 0*/ )
{
	char fName[MAX_PATH];
	sprintf(fName,"Save\\%s\\window.ini",g_MyInfo.GetPublicID().c_str() );

	ioINILoader kLoader( fName , false );
	kLoader.SetTitle( szTitle );
	int value       = kLoader.LoadInt( szKeyName, SCREEN_OUT_POS );
	int iINIVersion = kLoader.LoadInt_e( "Version", 0 );
	int iINIWidth   = kLoader.LoadInt_e( "ScreenWidth", 0 );
	int iINIHeight  = kLoader.LoadInt_e( "ScreenHeight", 0 );

	if( value       == SCREEN_OUT_POS   ||
		iVersion   !=  iINIVersion      ||
		iINIWidth  !=  Setting::Width() || 
		iINIHeight !=  Setting::Height() )
	{
		value = iValue;
	}

	return value;
}

int ioSP2GUIManager::GUIPosSaveInt( const char *szTitle, const char *szKeyName, int iValue ,int iVersion /*= 0*/ )
{
	char fName[MAX_PATH];
	sprintf(fName,"Save\\%s\\window.ini",g_MyInfo.GetPublicID().c_str() );
	
	ioINILoader kLoader( fName , false );
	kLoader.SetTitle( szTitle );
	kLoader.SaveInt( "Version", iVersion );
	kLoader.SaveInt( szKeyName, iValue );
	kLoader.SaveInt( "ScreenWidth", Setting::Width() );
	kLoader.SaveInt( "ScreenHeight", Setting::Height() );

	return iValue;
}

int ioSP2GUIManager::EtcValueLoadInt( const char *szTitle, const char *szKeyName )
{
	char fName[MAX_PATH];
	sprintf(fName,"Save\\%s\\window.ini",g_MyInfo.GetPublicID().c_str() );

	ioINILoader kLoader( fName , false );
	kLoader.SetTitle( szTitle );
	int value       = kLoader.LoadInt( szKeyName, 0 );

	return value;
}

int ioSP2GUIManager::EtcValueSaveInt( const char *szTitle, const char *szKeyName, int iValue )
{
	char fName[MAX_PATH];
	sprintf(fName,"Save\\%s\\window.ini",g_MyInfo.GetPublicID().c_str() );

	ioINILoader kLoader( fName , false );
	kLoader.SetTitle( szTitle );
	kLoader.SaveInt( szKeyName, iValue );

	return iValue;
}

void ioSP2GUIManager::MoveZorderLayer( DWORD dwWndID, DWORD dwMoveID )
{
	if( m_pDeskTopWnd )
		m_pDeskTopWnd->MoveZorderLayer( dwWndID, dwMoveID );
}

void ioSP2GUIManager::MoveZorderTop( DWORD id )
{
	if ( m_pDeskTopWnd )
		m_pDeskTopWnd->MoveZorderTop( id );
}

void ioSP2GUIManager::SetModal( ioWnd* wnd )
{
	if ( !wnd ) return;

	g_GUIMgr.GetDeskTop()->MoveZorderTop( wnd->GetID() );
	
	wnd->AddWndStyle( IWS_MODAL );
}

bool ioSP2GUIManager::IsUIKeyBlock()
{
	if( !ioEdit::m_pKeyFocusEdit )
		return false;

	ioWnd *pParent = ioEdit::m_pKeyFocusEdit->GetParent();
	while( pParent )
	{
		if( !pParent->IsShow() )
			return false;

		pParent = pParent->GetParent();
	}

	return true;
}

bool ioSP2GUIManager::IsDeskTopOver() const
{
	if( m_pDeskTopWnd == m_pPreOverWnd )
		return true;

	return false;
}

bool ioSP2GUIManager::IsCurrentOver( DWORD dwID ) const
{
	if( !m_pPreOverWnd )
		return false;

	if( m_pPreOverWnd->GetID() != dwID )
		return false;

	return true;
}

bool ioSP2GUIManager::IsDragMove( DWORD dwID ) const
{
	if( !m_pCurDragMoveWnd )
		return false;

	if( m_pCurDragMoveWnd->GetID() != dwID )
		return false;

	return true;
}

void ioSP2GUIManager::ShowPartyUICmd()
{
}

void ioSP2GUIManager::HidePartyUICmd()
{
}

bool ioSP2GUIManager::IsShowSettingKeyWnd()
{
	bool bShowSettingKeyWnd = false;
	GameOptionWnd *pOptionWnd = dynamic_cast<GameOptionWnd*> ( FindWnd( GAME_OPTION_WND ) );
	if(pOptionWnd)
	{
		GameOptionSettingKeyTab *pKeyTab = dynamic_cast<GameOptionSettingKeyTab*>(pOptionWnd->FindChildWnd( GameOptionWnd::ID_KEYBOARD_TAB_WND ) );			
		if( pKeyTab )
		{
			// 최상위 윈도우일 경우에만 해당
			if( pKeyTab->IsShow() && ioWnd::GetSelectedID() == GAME_OPTION_WND )
				bShowSettingKeyWnd = true;
		}

		if( !bShowSettingKeyWnd )
		{
			GameOptionSettingJoyKeyTab *pJoyKeyTab = dynamic_cast<GameOptionSettingJoyKeyTab*>(pOptionWnd->FindChildWnd( GameOptionWnd::ID_JOYPAD_TAB_WND ) );
			if( pJoyKeyTab )
			{
				// 최상위 윈도우일 경우에만 해당
				if( pJoyKeyTab->IsShow() && ioWnd::GetSelectedID() == GAME_OPTION_WND )
					bShowSettingKeyWnd = true;
			}
		}

		if( !bShowSettingKeyWnd )
		{
			GameOptionSettingJoyKeyTab *pJoyKeyTab = dynamic_cast<GameOptionSettingJoyKeyTab*>(pOptionWnd->FindChildWnd( GameOptionWnd::ID_JOYPAD_TAB_WND ) );

			if( pJoyKeyTab )
			{
				// 최상위 윈도우일 경우에만 해당
				if( pJoyKeyTab->IsShow() && ioWnd::GetSelectedID() == GAME_OPTION_WND )
					bShowSettingKeyWnd = true;
			}
		}
	}

	return bShowSettingKeyWnd;
}

void ioSP2GUIManager::ShowLoginPopup( ioHashString szName )
{
	if( g_App.IsAdminID( szName.c_str() ) ) return;
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_BattleRoomMgr.IsTournamentModeType() || g_BattleRoomMgr.IsBroadcastModeType() || g_RankBattleMgr.IsRankBattlePlaying() ) 
			return;
	}

	MessengerPopup *pPopup = dynamic_cast<MessengerPopup*>(FindWnd( MESSENGER_POPUP ));
	if( pPopup )
	{
		pPopup->ShowPopup( MessengerPopup::FRIEND_POPUP, szName, "" );
	}
	g_SoundMgr.PlaySound( "interface\\team_invite.wav" );
}

void ioSP2GUIManager::ShowBestFriendPopup( ioHashString szName, bool bLogin )
{
	if( g_App.IsAdminID( szName.c_str() ) ) return;
	if( g_BattleRoomMgr.IsBattleRoom() )
	{
		if( g_BattleRoomMgr.IsTournamentModeType() || g_BattleRoomMgr.IsBroadcastModeType() || g_RankBattleMgr.IsRankBattlePlaying()) 
			return;
	}

	MessengerPopup *pPopup = dynamic_cast<MessengerPopup*>(FindWnd( MESSENGER_POPUP ));
	if( pPopup )
	{
		pPopup->ShowPopup( MessengerPopup::BEST_FRIEND_POPUP, szName, "" );
	}
	g_SoundMgr.PlaySound( "interface\\team_invite.wav" );
}

ioSP2GUIManager& ioSP2GUIManager::GetSingleton()
{
	return Singleton< ioSP2GUIManager >::GetSingleton();
}

bool ioSP2GUIManager::CheckAndRenderModalBackGround( bool bRender, ioWnd *pWnd )
{
	if( !pWnd )
		return false;

	bool bReturn = false;
	if( !bRender && pWnd->HasWndStyle( IWS_MODAL ) && pWnd->IsShow() )
	{
		bReturn = true;
		if( m_pModalBackGround )
		{
			m_pModalBackGround->SetScale( (float)Setting::Width(), (float)Setting::Height() );
			m_pModalBackGround->Render( (float)Setting::Width() * FLOAT05, (float)Setting::Height() * FLOAT05 );
		}
	}	

	if( pWnd->HasWndStyle( IWS_MODAL_EXTEND ) && pWnd->IsShow() )
	{
		bReturn = true;
		if( m_pModalBackGround )
		{	
			m_pModalBackGround->SetScale( (float)Setting::Width(), (float)Setting::Height() );
			for( int i = 0; i < 3; ++i )
				m_pModalBackGround->Render( (float)Setting::Width() * FLOAT05, (float)Setting::Height() * FLOAT05 );
		}
	}
	
	return bReturn;
}

 
void ioSP2GUIManager::SetSelectedWnd()
{
	if( !m_pDeskTopWnd )	return;

	int iNumChild = m_pDeskTopWnd->GetNumChildren();
	for (int i = iNumChild - 1; i >= 0 ; i--)
	{
		ioWnd *pWnd = m_pDeskTopWnd->GetChild( i );

		if( !pWnd ) continue;
		if( !pWnd->IsShow() ) continue;
		if(  pWnd->HasWndStyle( IWS_BACKMOST ) ) continue;
		if( !pWnd->IsHaveSelectedFrm() ) continue;
		if(  pWnd->GetID() == USERINFO_TOOLTIP ) continue;
		if(  pWnd->GetID() == NEW_SHOP_TOOLTIP ) continue;
		if(  pWnd->GetID() == PRACTICE_TOOLTIP_WND ) continue;
		if(  pWnd->GetID() == MY_INVENTORY_TOOLTIP ) continue;
		if(  pWnd->GetID() == ALCHEMIC_ITEM_TOOLTIP ) continue;
		if(  pWnd->GetID() == PET_TOOLTIP ) continue;
  
		ioWnd::SetSelectedID( pWnd->GetID() );
		return;
	}
}

void ioSP2GUIManager::CheckShopSound( bool bEnable )
{
	m_bEnableShopSound = bEnable;
}

bool ioSP2GUIManager::IsCanShopSoundForField()
{
	if( IsShow( TOTAL_RESULT_MAIN_WND ) ||
		IsShow( ROUND_RESULT_MAIN_WND ) )
	{
		return false;
	}

	return m_bEnableShopSound;
}

void ioSP2GUIManager::ChildWndShowIDList()
{
	if( !g_App.IsMeDeveloper() ) return;

	if( m_pDeskTopWnd )
		m_pDeskTopWnd->ChildWndShowIDList();
}
void ioSP2GUIManager::NotifyLoadWnd( ioWnd* pWnd )
{
#ifndef SHIPPING
	if( pWnd )
	{	
		WndBoundaryContainer* pContainer = GetBoundaryContainer( pWnd->GetInnerUniqueIndex() );
		if( pContainer )
		{
			pContainer->AddBoundary( pWnd->GetID(), ioWndBoundary::BT_WND, pWnd, pWnd->GetParent(), "" );
		}
	}
#endif
}

void ioSP2GUIManager::NotifyLoadFrame( ioUIRenderFrame* pFrame, const ioHashString& szType, ioWnd* pWnd )
{
#ifndef SHIPPING
	if( pFrame && !pFrame->GetName().empty() )
	{	
		WndBoundaryContainer* pContainer = GetBoundaryContainer( pWnd->GetInnerUniqueIndex() );
		if( pContainer )
		{
			pContainer->AddBoundary( pWnd->GetID(), ioWndBoundary::BT_UIFRAME, pFrame, pWnd, szType );
		}
	}
#endif
}
void ioSP2GUIManager::NotifyUIRenderImage( ioUIRenderImage* pImage, const ioHashString& szType, ioWnd* pWnd )
{
#ifndef SHIPPING
	if( pImage && !pImage->GetImageName().empty() )
	{	
		WndBoundaryContainer* pContainer = GetBoundaryContainer( pWnd->GetInnerUniqueIndex() );
		if( pContainer )
		{
			pContainer->AddBoundary( pWnd->GetID(), ioWndBoundary::BT_UIIMAGE, pImage, pWnd, szType );
		}
	}
#endif
}

void ioSP2GUIManager::DestroyBoundary()
{
#ifndef SHIPPING
	for( auto iter = m_WndBoundaryContainerMap.begin(); iter != m_WndBoundaryContainerMap.end(); ++iter )
	{
		WndBoundaryContainer* pContainer = iter->second;
		if( pContainer )
		{
			SAFEDELETE( pContainer );
		}
	}
	m_WndBoundaryContainerMap.clear();
#endif
}

WndBoundaryContainer* ioSP2GUIManager::GetBoundaryContainer( DWORD dwIndex )
{
#ifndef SHIPPING
	auto iter = m_WndBoundaryContainerMap.find( dwIndex );
	if( iter != m_WndBoundaryContainerMap.end() )
	{
		return iter->second;
	}
	else
	{
		WndBoundaryContainer* pContainer = new WndBoundaryContainer;
		m_WndBoundaryContainerMap.insert( WndBoundaryContainerMap::value_type( dwIndex, pContainer ) );
		return pContainer;
	}
#else
	return NULL;
#endif
}

void ioSP2GUIManager::RenderBoundary()
{
#ifndef SHIPPING

	if( !Setting::ShowWndBoundary() )
		return;

	if( g_App.IsFocus() == FALSE )
		return;

	int nContextMaxWidth = 0;
	ioHashStringVec ContextList;

	for( auto iter = m_WndBoundaryContainerMap.begin(); iter != m_WndBoundaryContainerMap.end(); ++iter )
	{
		WndBoundaryContainer* pContainer = iter->second;
		if( pContainer )
		{
			pContainer->CollectBoundaryContext( nContextMaxWidth, ContextList );
			pContainer->RenderBoundary();
		}
	}
	
	if( g_Input.IsKeyDown( KEY_LEFTCTRL ) && g_Input.IsKeyDown( KEY_C ) && !ContextList.empty() )
	{
		BoundaryUtil::ExportContextClipBoard( ContextList );
	}

	BoundaryUtil::OnBoundaryContextRender( &RenderSystem(), ContextList, nContextMaxWidth );

#endif
}