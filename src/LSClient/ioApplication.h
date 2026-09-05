#pragma once

#include "ioGameStage.h"
#include "ioLSMouse.h"
#include "WellKnownURL.h"
#include "SP2Packet.h"
#include "ChatMessageFunc.h"

class CLogin;
class ioCamera;
class ioCameraController;
class ioRenderSystem;

class ioTextureManager;
class ioMaterialManager;
class ioAnimationManager;
class ioSkeletonManager;
class ioMeshManager;
class ioShaderManager;
class ioFontManager;
class ioUIRenderer;
class ioInputBox;
class CDirectInput;
class ioFrameTimer;
class ioGlobalTimer;
class ioSoundManager;
class ioExtendSoundManager;
class ioTextureEffectManager;
class ioEffectFactory;
class ioVertexBufferManager;
class ioIndexBufferManager;
class ioOpcodeManager;
class ioAnimateFXManager;
class ioMeshTrailDataManager;
class ioSP2GUIManager;
class ioSP2ChatManager;

class ioCollisionBoxGrpManager;
class ioWeaponManager;
class ioItemMaker;
class ioSkillMaker;
class ioBuffMaker;
class ioChatMacro;
class ChatMacroMgr;
class ioStatAttribute;
class ioSetItemInfoManager;
class ioEtcItemManager;
class MathManager;
class DropManager;
class SkillManager;
class ioGrowthInfoManager;
class ioFishingManager;
class ioExtraItemInfoManager;
class ioShopItemExtraInfoManager;
class ioItemCompoundManager;
class ioItemCompoundMaterialManager;
class ioRoomOptionManager;
class TradeInfoManager;
class ioUserSpiritManager;
class ioCostumeShopInfoManager;

class ioTutorialManager;
class ioMemoManager;
class ioFriendManager;
class ioBlackListManager;
class ioChannelManager;
class ioSoldierManualMgr;
class ioAnnouncementManualMgr;
class ioItemManualMgr;
class ioDecoManualMgr;
class ioEtcManualMgr;
class ioEventGoodsManualMgr;
class ioEtcInvenManualMgr;
class ioEtcInvenSubManualMgr;
class ioPresentInvenManualMgr;
class ioExtraInvenManualMgr;
class ioMedalInvenManualMgr;
class ioDecoInvenManualMgr;
class ioPetInvenManualMgr;
class ioCostumeInvenManualMgr;
class ioCostumeShopManualMgr;
class ioSpecialShopMgr;
class ioShopDataCache;
class ioPopupStorePresentItemManualMgr;
class ioAccessoryInvenManualMgr;

class ioUserInfoMgr;
class ioGlobalSoldierInfo;

class ioGameSceneManager;
class ioClientUDPSocket;
class ioTCPSocketManager;
class ioUserNetworkTable;
class ioMyInfo;
class ioP2PChecker;
class ioTCPChecker;
class ioMyLevelMgr;
class ioSlotMgr;
class ioBattleRoomMgr;
class ioLadderTeamMgr;
class ioShuffleRoomMgr;
class ioModeSelectInfo;
class ioAbuseManager;
class ioAwardManager;
class ioModeUIController;
class ioVictoriesMgr;
class ioPlayBonusMgr;
class ioTreasureCardMgr;
class ioGuildInfo;
class ioGuildMarkMgr;
class ioGuildChatMgr;
class ioCustomTextureMgr;
class ioCampManager;
class ioQuestManager;
class ioQuestGuide;
class ioFieldRewardItemMgr;
class ioInterfaceBackground;
class ioNpcMgr;
class ioAISymbolTable;
class ioMapEventSceneMgr;
class ioFieldItemMgr;
class ioTournamentManager;
class ioTournamentTeamManager;
class TournamentRewardManager;
class ioFlashDownLoadMgr;
class ioRankBattleManager;

class ioTickTimer;
class ioClassPrice;
class ioEventGoodsMgr;
class ioDecorationPrice;
class ioLoadingRender;

class CSlangUtil;
class CBlockKeyBoard;

struct SettingOption;
class ioBrowserManager;
class ioModeHelpManager;
class ioAlarmManager;
class ioKeyManager;

class ioVoiceDisableListManager;
class ioVoiceOffListManager;
class ioEventManager;
class ioCustomSoundMgr;
class ioPresentMgr;
class ioSubscriptionMgr;
class ioMannerTrialChatManager;

class ioScreenShotTask;
class ioMovieMaker;
class ioGameAnnMgr;
class ioChannelingNodeManager;
class ioSaleManager;
class ioFileWriteClient;
class ioExcavationManager;
class ioMedalItemInfoManager;
class ioExpandMedalSlotManager;
class ioReservedTask;

class ioRouletteManager;

class ioAISymbolTable;
class ioAIDatabase;
class ioSuperGashaponMgr;
class ioMultiSuperGashaponMgr;

class ioItemRechargeManager;
class ioAccRechargeManager;
class ioModeItemManager;
class ioGashaponMgr;
struct ReservedTask;

class ioPetInfoManager;
class ioSoldierAwakeManager;
class ioBasicTooltipManager;
class IoToolTipManager;
class ioModeResourceManager;
class ioRequestCharManager;
class ioRequestLagManager;

class ioPowerUpManager;

class ioCostumeInfoManager;

class ioMissionManager;
class ioUIImageRenderManager;

#if defined( USE_GA )
class ioHttpMng;
#endif
class ioShopManager;
class ioRenderTargetManager;
class ioRSoldierInfoManager;

class ioUserBlockStorage;
class ioBlockEventAgency;

class ioUserNamedTitle;

class ioBonusCashManager;

class ioAccessoryInfoManager;
class ioUIGuide;

class ioApplication : public Singleton< ioApplication >
{
protected:
	HINSTANCE		m_hInstance;
	HWND			m_hWnd;
	BOOL			m_bAppActive;
	BOOL			m_bFocus;
	DWORD m_dwResetSoundTime;

	HBITMAP			m_hWindowTitleLosa;
	HBITMAP			m_hWindowOutLine;

	typedef std::vector< SP2Packet > SP2PacketList;
	SP2PacketList m_TCPBeforeSetUpList;
	SP2PacketList m_UDPBeforeSetUpList;
	bool			m_bSetUpDone;

	cLua			m_lua;
	MathManager		*m_pMathManager;
	DropManager		*m_pDropManager;

	int				m_iWidth;
	int				m_iHeight;

	int				m_iDeskTopWidth;
	int				m_iDeskTopHeight;
	
	CLogin		    *m_pLogin;
	ioTickTimer		*m_pTickTimer;

	DWORD			m_dwLastFaceCountTime;
	int				m_iFramePerFaceCount;
	int				m_iTotalFrameFaceCount;
	int				m_iPastedFrameCount;

	POINT			m_ptPreMouse;
	POINT			m_ptMouseMoveAmt;
	ioLSMouse		m_Mouse;
	DWORD			m_dwMousePreCheckTime;
	DWORD			m_dwMouseUnvisiblTime;
	DWORD			m_dwMouseUnvisiblLobbyTime;

	POINT			m_ptPreDragPos;
	bool			m_bWindowTitleDragging;

	ioGameStage		*m_pGameStage;
	ioGameStage::GameStage	m_CurStage;
	ioGameStage::GameStage	m_NextStage;

	int				m_iModeType;
	int				m_iModeSubNum;
	int             m_iModeMapNum;
	bool            m_bCampBattle;
	bool            m_bCampHeroBattle;

	PlazaType       m_ePlazaType;
	bool			m_bReloadINILists;

	int				m_iFrameSkipCount;

	char			m_szResourcePath[MAX_PATH];
	int				m_iTextureGrayRate;
	int             m_iTextureGrayLightRate;

	ioRenderSystem  *m_pRenderSystem;

	ioMeshManager			*m_pMeshMgr;
	ioMaterialManager		*m_pMaterialMgr;
	ioAnimationManager		*m_pAnimationMgr;
	ioAnimationEventManager *m_pAnimationEventManager;
	ioSkeletonManager		*m_pSkeletonMgr;
	ioTextureManager		*m_pTextureMgr;
	ioShaderManager			*m_pShaderMgr;
	ioUIRenderer			*m_pUIRenderer;
	ioFontManager			*m_pFontMgr;
	ioSP2GUIManager			*m_pGUIMgr;
	ioSP2ChatManager        *m_pChatMgr;
	ioInputBox				*m_pInputBox;
	ioSoundManager			*m_pSoundMgr;
	ioExtendSoundManager	*m_pExSoundMgr;
	CDirectInput			*m_pDxInput;
	ioFrameTimer			*m_pFrameTimer;
	ioGlobalTimer			*m_pGlobalTimer;
	ioTextureEffectManager	*m_pTexEffMgr;
	ioEffectFactory			*m_pEffectFac;
	ioVertexBufferManager   *m_pVtxBufMgr;
	ioIndexBufferManager    *m_pIdxBufMgr;
	ioOpcodeManager			*m_pOpcodeMgr;
	ioAnimateFXManager		*m_pAnimateFXMgr;
	ioMeshTrailDataManager  *m_pTrailDataMgr;
	ioModeHelpManager		*m_pModeHelpMgr;
	ioAlarmManager          *m_pAlarmMgr;
	ioUserInfoMgr			*m_pUserInfoMgr;
	ioGlobalSoldierInfo     *m_pGlobalSoldierInfo;

	ioGameSceneManager		*m_pSceneMgr;	// for default value : kaseton
	ioCamera				*m_pCamera;

	ioTCPSocketManager		*m_pTCPSocket;
	ioClientUDPSocket		*m_pUDPSocket;
	ioUserNetworkTable		*m_pNetworkTable;
	
	ioMyInfo	            *m_pMyInfo;
	ioGuildInfo             *m_pGuildInfo;
	ioGuildMarkMgr			*m_pGuildMarkMgr;
	ioGuildChatMgr          *m_pGuildChatMgr;
	ioCustomTextureMgr      *m_pCustomTextureMgr;
	ioP2PChecker            *m_pP2PChecker;
	ioTCPChecker			*m_pTCPChecker;
	ioMyLevelMgr			*m_pMyLevelMgr;
	ioSlotMgr				*m_pSlotMgr;
	ioBattleRoomMgr			*m_pBattleRoomMgr;
	ioShuffleRoomMgr		*m_pShuffleRoomMgr;
	ioLadderTeamMgr			*m_pLadderTeamMgr;
	ioModeSelectInfo        *m_pModeSelectInfo;
	ioClassPrice			*m_pClassPrice;
	ioEventGoodsMgr         *m_pEventGoodsMgr;
	ioDecorationPrice		*m_pDecorationPrice;
	ioAbuseManager			*m_pAbuseMgr;
	ioCampManager           *m_pCampManager;
	ioQuestManager          *m_pQuestManager;
	ioQuestGuide            *m_pQuestGuide;
	ioFieldRewardItemMgr    *m_pFieldRewardItemMgr;
	ioInterfaceBackground   *m_pBackground;
	ioNpcMgr                *m_pNpcMgr;
	ioAISymbolTable         *m_pSymbolTable;
	ioAIDatabase            *m_pAIDB;
	ioMapEventSceneMgr      *m_pMapEventSceneMgr;
	ioFieldItemMgr			*m_pFieldItemMgr;
	ioTournamentManager     *m_pTournamentManager;
	ioTournamentTeamManager *m_pTournamentTeamManager;
	TournamentRewardManager *m_pTournamentRewardManager;
	ioFlashDownLoadMgr		*m_pFlashDownLoadMgr;
	ioRankBattleManager		*m_pRankBattleMgr;
	ioCollisionBoxGrpManager *m_pCollisionGrpMgr;

	ioWeaponManager	*m_pWeaponMgr;
	ioItemMaker		*m_pItemMaker;
	ioSkillMaker	*m_pSkillMaker;
	ioBuffMaker		*m_pBuffMaker;

	ioChatMacro				 *m_pChatMacro;
	ChatMacroMgr             *m_pChatMacroMgr;

	ioStatAttribute			 *m_pStatAttribute;
	ioSetItemInfoManager	 *m_pSetItemInfoMgr;
	ioEtcItemManager	     *m_pEtcItemMgr;
	ioTutorialManager        *m_pTutorialMgr;
	ioMemoManager            *m_pMemoMgr;
	ioFriendManager          *m_pFriendMgr;
	ioBlackListManager       *m_pBlackListMgr; 
	ioChannelManager         *m_pChannelMgr;
	ioKeyManager             *m_pKeyMgr;
	ioSoldierManualMgr       *m_pSoldierManualMgr;
	ioAnnouncementManualMgr  *m_pAnnouncementManualMgr;
	ioItemManualMgr          *m_pItemManualMgr;
	ioDecoManualMgr          *m_pDecoManualMgr;
	ioEtcManualMgr           *m_pEtcManualMgr;
	ioEventGoodsManualMgr    *m_pEventGoodsManualMgr;
	ioEtcInvenManualMgr      *m_pEtcInvenManualMgr;
	ioEtcInvenSubManualMgr   *m_pEtcInvenSubManualMgr;
	ioPresentInvenManualMgr  *m_pPresentInvenManualMgr;
	ioPopupStorePresentItemManualMgr* m_pPopupStoreManualMgr;
	ioExtraInvenManualMgr    *m_pExtraInvenManualMgr;
	ioMedalInvenManualMgr    *m_pMedalInvenManualMgr;
	ioDecoInvenManualMgr     *m_pDecoInvenManualMgr;
	ioPetInvenManualMgr		 *m_pPetInvenManualMgr;
	ioCostumeInvenManualMgr	 *m_pCostumeInvenManualMgr;
	ioCostumeShopManualMgr	 *m_pCostumeShopManualMgr;
	ioBonusCashManager	     *m_pBonusCashMgr;
	ioAccessoryInvenManualMgr *m_pAccessoryInvenManualMgr;

	ioAwardManager           *m_pAwardMgr;
	ioModeUIController       *m_pModeUIController;
	ioVictoriesMgr           *m_pVictoriesMgr;
	ioPlayBonusMgr           *m_pPlayBonusMgr;
	ioTreasureCardMgr        *m_pTreasureCardMgr;
	ioMovieMaker             *m_pMovieMaker;
	ioGameAnnMgr             *m_pGameAnnMgr;
	ioBrowserManager         *m_pBrowserMgr;
	ioGrowthInfoManager		 *m_pGrowthInfoMgr;
	ioFishingManager		 *m_pFishingMgr;
	ioChannelingNodeManager  *m_pChannelingMgr;
	ioExtraItemInfoManager	 *m_pExtraItemInfoMgr;
	ioShopItemExtraInfoManager *m_pShopItemExtraInfoManager;
	ioItemCompoundManager	 *m_pItemCompoundMgr;
	ioItemCompoundMaterialManager	 *m_pItemCompoundMaterialMgr;
	ioSaleManager            *m_pSaleMgr;
	ioRoomOptionManager		 *m_pRoomOptionMgr;
	ioFileWriteClient        *m_pFileWriteClient;
	ioExcavationManager      *m_pExcavationMgr;
	ioMedalItemInfoManager   *m_pMedalItemInfoMgr;
	ioExpandMedalSlotManager *m_pExpandMedalSlotManager;
	TradeInfoManager		 *m_pTradeInfoMgr;
	ioUserSpiritManager		 *m_pSpiritMgr;
	ioReservedTask			 *m_pReservedTask;
	ioRouletteManager		 *m_pRouletteMgr;
	ioSuperGashaponMgr		 *m_pSuperGashponMgr;
	ioMultiSuperGashaponMgr	 *m_pMultiSuperGashaponMgr;
	ioItemRechargeManager    *m_pItemRechargeMgr;
	ioAccRechargeManager    *m_pAccRechargeMgr;
	ioGashaponMgr			 *m_pGashponMgr;

	ioModeItemManager        *m_pModeItemMgr;

	ioPetInfoManager		 *m_pPetInfoMgr;
	ioSoldierAwakeManager	 *m_pSoldierAwakeMgr;
	ioBasicTooltipManager	 *m_pBasicTooltipManager;
	IoToolTipManager		 *m_pTooltipManager;
	ioModeResourceManager	 *m_pModeResourceMgr;

	ioRequestCharManager	 *m_pRequestCharMgr;
	ioRequestLagManager		 *m_pRequestLagMgr;

	ioPowerUpManager		 *m_pPowerUpMgr;

	ioCostumeInfoManager	 *m_pCostumeInfoMgr;
	ioCostumeShopInfoManager *m_pCostumeShopInfoMgr;
	ioSpecialShopMgr		 *m_pSpecialShopMgr;
	ioShopManager			 *m_pShopMgr;
	ioShopDataCache			 *m_pShopDataCache;
	ioRenderTargetManager	 *m_pRenderTargetMgr;
	ioMissionManager		 *m_pMissionManager;
	ioUIImageRenderManager	 *m_pUIImageRenderManager;

	ioRSoldierInfoManager	 *m_pRSoldierInfoManager;
	ioUserBlockStorage		 *m_pGuildBlockStorage;
	ioUserBlockStorage		 *m_pHomeBlockStorage;
	ioBlockEventAgency		 *m_pBlockEventAgency;
	ioNamedTitleInfoManager	 *m_pNamedTitleInfoManager;

	ioAccessoryInfoManager	 *m_pAccessoryInfoMgr;
	ioUIGuide				 *m_pUIGuide;

#if defined( USE_GA )
	ioHttpMng				 *m_pHttpMng;;
#endif

	ioHashString m_ReserveOpenHeadQuater;

	CSlangUtil     *m_pSlangUtil;
	CBlockKeyBoard *m_pBlockKeyBoard;
	ioLoadingRender *m_pLoadingRender;

	ioHashString m_LogServerIP;
	int          m_iLogPort;
	bool         m_bP2PDataDBLog;
	bool         m_bControlSyncExpand;

	bool	m_bIsMeDeveloperFirst;
	bool	m_bIsMeDeveloperCache;

	float	m_fDefaultFov;
	float   m_fDefaultNear;
	float   m_fDefaultFar;

	ioHashString m_LoadingStartSound;
	ioHashString m_LoadingEndSound;
	DWORD m_dwLoadingDelay;
	DWORD m_dwLoadingEndTime;
	DWORD m_dwLoadingLogoTime;
	DWORD m_dwLoadingRegulation;
	DWORD m_dwLoadingLogoColor;
	DWORD m_dwLoadingIssueColor;

	CEncrypt<bool>	m_bProgramExit;

	int		m_iNextUDPSendIndex;	// 전체적인 UDP인덱스 체크
	int		m_iNextSendIndex;		// Control메세지 한정의 인덱스

	bool	m_bFirstLoading;
	bool	m_bFirstMyRoom;
	bool	m_bFirstPlaza;	

	DWORD   m_dwReserveLogOutTime;
	DWORD   m_dwReserveLogOutCnt;
	DWORD   m_dwReserveLogOutRandTime;
	DWORD   m_dwReserveLogOutType;

	int     m_iRoomNum;

	ioVoiceDisableListManager *m_pVoiceDisableListMgr;
	ioVoiceOffListManager *m_pVoiceOffListMgr;
	ioEventManager        *m_pEventMgr;
	ioCustomSoundMgr	  *m_pCustomSoundMgr;
	ioPresentMgr          *m_pPresentMgr;
	ioSubscriptionMgr	*m_pSubscriptionMgr;
	ioMannerTrialChatManager *m_pMannerTrialChatMgr;

	DWORD   m_dwEntryRefreshTime;

	DWORD m_dwNextLanCheckTime;
	DWORD m_dwPreLanCheckState;

	ioHashString m_ReserveCheckExistID;

	bool m_bMouseControlModeCancel;

	DWORD m_all_udp_recv_time;
protected:
	DWORD m_dwMannerTrialTime;

protected:	// screenshot
	ioScreenShotTask *m_pScreenShotSaveTask;
	DWORD m_dwScreenShotStartTime;
	bool m_bReservedScreenShot;

	DWORD m_dwScreenShotMinGap;
	ioHashString m_ScreenShotStartSound;

	bool m_bWebLogOutForLuncher;
	bool m_bProcessCandidate;
	ioHashStringVec m_vNotMakeIDVector;

	DWORD m_dwShowWindowCursorStyle;

	ioHashString m_szAutoupgradeInfoININame;

	bool m_bResourceLoadFailed;

protected:
	DWORD m_dwGrapplingCheckDelay;

protected:  // 채팅 안내
	DWORD m_dwGamePlayAlarmTime;
	int   m_iGamePlayAlarmHour;

protected:
	DWORD m_dwProcessTimeCheck;         // 전체 루프에 걸린 시간
	DWORD m_dwProcessTimeLagCount;      // 2초에 렉이 걸린 횟수
	DWORD m_dwProcessTimeLagTime;       // 횟수 초기화용

// Send Recive Packet Test
protected:
	DWORD m_dwTestValue;
	bool m_bTestChangeStage;

	DWORD m_dwTestReciveStartTime;
	int m_iTestReciveCnt;
	DWORD m_dwTestReciveTime;
	DWORD m_dwTestReciveSize;

	DWORD m_dwTestSendStartTime;
	int m_iTestSendCnt;
	DWORD m_dwTestSendSize;

	DWORD m_dwTestFrameStartTime;
	DWORD m_iTestFrameCnt;

	DWORD m_dwLastBusyPacketID;
	DWORD m_dwLastBusyStartTime;

protected: // for relay 
	bool m_bRelayUse;
	char m_realyIpaddr[64];
	int  m_relayPort;

#ifdef _DEBUG
protected:
	DWORD m_dwCurPacketPrintCount;
	DWORD m_dwMaxPacketPrintCount;
#endif

public:
	enum ConnectedMoveMode
	{		
		CMM_PLAZA,
		CMM_HEADQUARTER,
		CMM_LOBBY,
	};	
	
	enum ExLoadingType
	{
		LOADING_NONE,
		LOADING_FIRST,
		LOADING_SEARCH_MODE,
		LOADING_HEADQUARTER,
		LOADING_HOUSE,
	};

protected:
	ExLoadingType	m_eLoadingType;
	int				m_eConnectedMoveMode;
	int				m_iConnectTryMax;
	int				m_iConnectTryCount;
	bool			m_bStepByConnectTry;
	bool			m_bEnableConnectedMoveMode;
	//수련장 로비로 돌아오는 불값
	bool			m_bPractice;
protected:
	typedef std::vector< std::pair < ioHashString, int > > vPackFileSize;
	vPackFileSize m_vPackFileSize;

protected:
	bool m_bFPSRender;
	bool m_bFirstSound;

protected:
	IntVec m_HotKeyIDList;

	// 일대일모드 오픈시간.
private:
	DWORD m_dwSuccessionStartTime[7];
	DWORD m_dwSuccessionEndTime[7];

	//수련장 로비용
public:
	void SetPracticeState(bool bPractice){m_bPractice = bPractice;}
	bool GetPracticeState(){return m_bPractice;}

public:
	std::vector<int> m_vecOpenPopupIndex;
	void OnPopupOpenIndex( SP2Packet& rkPacket );
	void OnPopupItemBuyResult( SP2Packet& rkPacket );	
	void CheckTodayPopupTime();
	int GetActivePopupIndex( SP2Packet& rkPacket );
	void SaveOpenPopupIndex( int iIndex );
	void CheckFolder();
	void OnPenalty( SP2Packet& rkPacket );

public:
	bool InitWindow( HINSTANCE hInstance, const ioHashString &szKeyOnCmd );
	bool SetupBeforeLogin( const ioHashString &szGameServerIP, int iGameServerPort );
	bool InitUDPSocket();
	bool ResetInitUDPSocket();     // 테스트를 위해 UDP 소켓을 다시 초기화한다.

	// CLogin
	void DebugWebLogin(OUT LPSTR &szCmdLine);
	bool ParseCmd( IN const char *szCmdLine,
				   OUT ioHashString &szKeyOnCmd,
				   OUT ioHashString &szOptinosOnCmd,
#ifdef NEXON_IP
				   OUT ioHashString &szIPKeyOnCmd,
#endif
		           OUT ioHashString &szIPOnCmd,
				   OUT int &iPortOnCmd );
#ifdef NEXON_IP
	bool LoginLoop (const ioHashString &szKey, const ioHashString &szIPKey);
#else
	bool LoginLoop (const ioHashString &szKey);
#endif
	bool ProcessOptions( const ioHashString &szOptions);
	//

	cLua *GetLua() { return &m_lua; }

public:
	bool IsWebShow() const;
	void OpenMannerWnd( const char *szNickName, bool bOnline ,ChatType eChatType = CT_NONE );
	void OpenHeadQuaterPage( const char *szNickName, bool bNormalUser = true );
	void OpenHeadQuaterPage( DWORD dwUserIndex );
	void OpenHeadquartersMini( const ioHashString kNickName );
	void OpenGuildHomePage( DWORD dwGuildIndex );
	bool IsGuildMarkChangeOK();
	void OpenGuildMarkWnd();
	void CloseGuildMarkPage();
	void OpenFormalityEntryPage();
	void OpenFillCashPage();
	void OpenCashEntryPage();
	void NotifyDeleteFriendByWeb( const char *szFriendNick );
	void NotifyUserEntryRefresh( const char *szNick );
	void NotifyGuildMarkChangeComplete();
	void NotifyHideProcess( BrowsePageType eType );
	bool IsCanMannerTrialTime();
	void SetMannerTrialTime();
	bool IsP2PDataDBLog(){ return m_bP2PDataDBLog; }
	inline BOOL IsFocus(){ return m_bFocus; }

public:
	inline bool IsControlSyncExpand(){ return m_bControlSyncExpand; }
	inline void SetControlSyncExpand( bool bControlSyncExpand ){ m_bControlSyncExpand = bControlSyncExpand; }

	inline int GetCurModeSubNum() const {	return m_iModeSubNum;	}

public:
	void InitGlobalTimer();
	bool IsRightID( const char *id );
	bool IsNotMakeID( const char *szNewID, OUT ioHashString &rszNotMakeString );
public://for relay
	bool IsRelayServer();
	void SetRelayServerState(bool bState);
	void SetRelayServerIpaddr(const char* ipaddr, const int port);
	char* GetRelayServerIp() { return m_realyIpaddr;}
	int  GetRelayServerPort(){ return m_relayPort;}
	bool SendUDPMessageToServer(SP2Packet& pk);
public:
	int  Run();

	void ReSize( int iWidth, int iHeight );
	void ReleaseAll();
	void SetExitProgram();
	bool IsExitProgram() const { return m_bProgramExit; }
	bool IsFirstLoading() const { return m_bFirstLoading; }

	inline int GetCurWidth() const { return m_iWidth; }
	inline int GetCurHeight() const { return m_iHeight; }

	bool ApplyOptionChange( const SettingOption &rkNewOption );

	HWND GetHWnd() const { return m_hWnd; }
	HINSTANCE GetInstance() const { return m_hInstance; }

protected:
	bool Setup();

	void SetupForSound();
	void ProcessBeforeSetUpPacket();
	void ProcessBeforeCharLoading();

	void ReloadINILists();

	void ModifyMainWindow();
	void ApplyEffectQuality( int iQuality );
	void SendINILog();
	void SendNexonAuthLog( int iErrorType );

public:
	void Reload();

protected:
	void MainLoop();
	void P2PNetworkProcess();
	void RenderLoop();
	void ResourceFailedCheck();

	void UpdateInput();
	void UpdateStage( DWORD dwTotalTime );

	void RenderDebugInfo();
	void CheckServerToClientPing();
	void CheckLanConnected();
	void CheckRecvUDPUnviable();
	void UpdateAllUDPRecvTime();

	void CheckForegroundForSound();

	void ProcessPacketBusy();

public:
	void CreateScreenShotPath();
	void ScreenShotCapture();
	void CheckScreenShotSaveTask();
	void ScreenShotDone( const char *szPath, bool bSuccess );

protected:
	void ClearInputState();
	void VoiceChatLoop();
	
	void SendVoiceChat( VOICEDATA &rVD );
	void UpdateReserveLogOut();

	void WindowMouseCursorProcess();
	void WindowTitleDragStart();
	void WindowTitleDragEnd();

	void QueryEndSessionProcess();

	void ExecuteLuncher();
	#ifdef _DEBUG
	void PrintPacketID( DWORD dwPacketID , bool bSend = true , const char *szRcvIP ="", int iRcvPort = 0);
	#endif

	void SetAfterCandidateWnd( UINT uMsg, WPARAM wParam );
	void ProcessCandidateWnd();

	void ProcessTickCheck();
	void ProcessTestPacketCnt();

	void LoadNotMakeID();
	void ChangeDisplaySettingsBySetting();

public:
	void AltF4ExitProcess( bool bShowMenu = true );
	void SetNextStage( ioGameStage::GameStage eStage, const char* szCall );
	void SetModeType( int iModeType, int iModeSubNum, int iModeMapNum, PlazaType ePlazaType );

	void SetRoomNum( int iRoomNum );
	PlazaType  GetPlazaModeType(){ return m_ePlazaType; }

	void ChangeStage();
	void ChangeStageLoadingCheck( DWORDVec &rLoadingVec, DWORD dwTotalTime );
	bool ChangeServer();

	void RenderFirstLoading( int iCurRate = 10, int iTotalRate = 10 );
	void RenderLoading( int iModeType, int iModeSubNum, int iModeMapNum, bool bCampBattle, bool bCampHeroBattle, PlazaType ePlazaType, int iCurRate = 0, int iTotalRate = 0, ExLoadingType eLoadingType = LOADING_NONE );
	

	DWORD GetLoadingLogoBackColor(){ return m_dwLoadingLogoColor; }
	DWORD GetLoadingIssueBackColor(){ return m_dwLoadingIssueColor; }

	bool CheckFirstMyRoom();
	bool CheckFirstPlaza();

	void UpdateLoadedResource();
	void EraseManualResource();
	void EraseManualResourceByUserRemove();

	void KickToHeadQuater( int iKickType, bool bOpenSpace = false );

	int  CreateP2PPort( char szLoaclIP[LENGTH_IP] );

	void ShowWindowCursor( DWORD dwShowWindowCursorStyle, bool bShow );
	
	//
	void SetTestSendData( SP2Packet &rkPacket );
	void SetTestReciveData( SP2Packet &rkPacket );
	void CheckTestSendRecive();

public:
	inline ioCamera* GetCamera() const { return m_pCamera; }
	inline const POINT& GetMouseAmount() const { return m_ptMouseMoveAmt; }
	inline ioLSMouse* GetMouse(){ return &m_Mouse;	}
	inline ioLSMouse& GetMouseRef(){ return m_Mouse; }
	inline bool IsPlaying() const { return (m_CurStage == ioGameStage::GS_PLAY); }
	inline bool IsNextStageLobby() const { return (m_NextStage == ioGameStage::GS_LOBBY); }
	inline ioGameStage::GameStage GetCurrentStage() const { return m_CurStage; }
	
    bool CheckMouseControlMode( bool bMoveMode );

public:
	void SetMouseBusy( bool bBusy, DWORD dwPacketID = CTPK_INVALID_PACKET );
	bool IsMouseBusy();
	bool IsMouseRender();
	
	void EnableNetworkPlay( bool bEnable );
	void SetDialogBoxMode( bool bDialog );
	void SetNagleAlgorithm( bool bOn );

public:
	void StartChatBubble( const char *szID, const char *szChat, bool bWholeChat );

public:
	void SendP2PPacket( const char *szIP, int iPort, SP2Packet &rkPacket );
	void SendP2PDirectPacket( char *szDirectIP, int iDirectPort, SP2Packet &rkPacket );
	void SendP2PRelayPacket( const ioHashString &rkName, SP2Packet &rkPacket );
	void RecvUDPPacket( SP2Packet &rkPacket, const char *szRcvIP, int iRcvPort );
	bool FirstUDPPacketParse( SP2Packet &rkPacket, const char *szRcvIP, int iRcvPort, bool bHeaderParsing = false );

public:
	void SendToAllPlayingUser( SP2Packet &rkPacket );
	void SendToAllUser( SP2Packet &rkPacket );
	void SendToTeam( SP2Packet &rkPacket, TeamType eSendTeam);
	void SendToUser( const ioHashString &rkName, SP2Packet &rkPacket );
	void SendToUserByIndex( const DWORD dwUserIndex, SP2Packet &rkPacket );
	void SendToHost( SP2Packet &rkPacket );
	void SendP2PUserInfoSync( const ioHashString &rkName, SP2Packet &rkPacket );

public:
	void SendRoomStealthEnter( const ioHashString &rkSyncName );

public:
	void SendToLogServer( SP2Packet &rkPacket );
	void SendUDPToServer( SP2Packet &rkPacket );

public:
	bool IsNetworkTableUser( const ioHashString &rkName );

	void AddNetworkTable( const DWORD dwUserIndex,
						  const ioHashString &rkName,
						  const ioHashString &szPublicIP,
						  const ioHashString &szPrivateIP,
						  const ioHashString &szTransferIP,
						  int iPort,
						  int iTransferPort,
						  DWORD dwStyle, int nDebugValue );

	void AddNetworkTableChangeStyle(const ioHashString &rkName, DWORD dwStyle );
	void RemoveNetworkTable( const ioHashString &rkName , DWORD dwStyle );

	void ServerToClientPortChange( const ioHashString &rkName, 
		                           const ioHashString &szPublicIP,		
								   const ioHashString &szPrivateIP,
								   const ioHashString &szTransferIP,		
								   int iPort,	
								   int iTransferPort );

	const char* GetUserGlobalIP( const ioHashString &rkName );
	void ClearAllNetworkTable( DWORD dwStyle );
	void AllNetworkTableServerRelay();

	void GetNetworkTablePublicID( IN DWORD dwUserIndex, OUT ioHashString &rszPublicID );
	void CheckValidateNetTable( ioPlayStage *pStage );
public:
	int  GetNextSendIndex();
	void SetLastControlIndex( const ioHashString &rkName, int iIndex );
	int  GetLastControlIndex( const ioHashString &rkName ) const;
	
	void SetLastRecvTime( const ioHashString &rkName, DWORD dwPingTime );
	int  GetP2pPingTime( const ioHashString &rkName );
	bool IsServerRelay( const ioHashString &rkName );
	bool IsNetworkTimeOut( const ioHashString &rkName );
	void SetHoleSendComplete( const ioHashString &rkName, bool bComplete );
	bool IsHoleSendComplete( const ioHashString &rkName );
	void SetHoleRecvComplete( const ioHashString &rkName, bool bComplete, const ioHashString &rkIP, const int &rkPort );
	bool IsHoleRecvComplete( const ioHashString &rkName );
	
	void GetMyLocalIP(char *iip);
	void SendAllP2PPing();
	void ClientToClientPortChange( const ioHashString &rkName, const char *szRcvIP, int iRcvPort );

public:
	void ResetVoiceChatWithGUI();
	int GetRoomNum();

public:
	void SendTCPPacket( SP2Packet &rkPacket );
	bool CheckSendTCPPacket( SP2Packet &rkPacket );
	void RecvTCPPacket( SP2Packet &rkPacket );
	
	bool IsTCPConnected() const;
	bool IsVisibleMouse();

	bool IsMeDeveloper( bool bMacro = false );
	bool IsDeveloperID( const char *szID );

	bool IsMeGameMaster();
	bool IsGameMasterID( const char *szID );

	bool IsAdminID( const char *szID );       // 개발자 + 마스터 + 매니저

	void SetReserveCheckExistID( const ioHashString &ReserveCheckExistID );
	void SetWebLogOutForLuncher(bool bWebLogOutForLuncher) { m_bWebLogOutForLuncher = bWebLogOutForLuncher; }

	void EnableAltCtrl();
	void DisableAltCtrl();

	const ioHashString &GetAutoupgradeInfoININame() const { return m_szAutoupgradeInfoININame; }
	void SetAutoupgradeInfoININame( const ioHashString &rszAutoupgradeInfoININame) { m_szAutoupgradeInfoININame = rszAutoupgradeInfoININame; }

	void ClearLoadingChangeHelp();

	void GetClientSize( POINT & ptOutClientSize, bool bNonClientArea = true );

	int GetDeskTopWidth() const { return m_iDeskTopWidth; }
	int GetDeskTopHeight() const { return m_iDeskTopHeight; }

	void OnMacroPracticeCompleted( DWORD dwPracticeResult );

// 개발자 아이디에만 30일 전부터 라이센스에 대한 정보를 팝업창으로 띄움		JCLEE 140718
#if defined( SRC_OVERSEAS )
	void CheckLicenseForDev();
#endif

public:
	inline DWORD GetGrapplingCheckDelay() const { return m_dwGrapplingCheckDelay; }
	inline ioRenderSystem* GetRenderSystem(){ return m_pRenderSystem; }

public:
	bool IsWemadeChannelingGetCash();

protected:
	bool ProcessUDPPacketRecvTCP( SP2Packet &rkPacket );
	bool ProcessTCPPacket( SP2Packet &rkPacket );
	bool ProcessUDPPacket( SP2Packet &rkPacket );
	void OnClassPrice( SP2Packet &rkPacket );
	void OnCharCreate( SP2Packet &rkPacket );
	void OnCharLoad( SP2Packet &rkPacket );
	void OnCharAllLoad( SP2Packet &rkPacket );
	void OnArenaCharAllLoad( SP2Packet &rkPacket );
	void OnClassExpert( SP2Packet &rkPacket );
	void OnInventorySlot( SP2Packet &rkPacket );
	void OnAwardData( SP2Packet &rkPacket );
	void OnUserRecordData( SP2Packet &rkPacket );
	void OnGrowthLevelData( SP2Packet &rkPacket );
	void OnBuyItem( SP2Packet &rkPacket );
	void OnIncreseStat( SP2Packet &rkPacket );
	void OnInitStat( SP2Packet &rkPacket );
	void OnEquipSlotItem( SP2Packet &rkPacket );
	void OnMoveSlotItem( SP2Packet &rkPacket );
	void OnSellSlotItem( SP2Packet &rkPacket );
	void OnSellDecoItem( SP2Packet &rkPacket );

	void OnExtraItemData( SP2Packet &rkPacket );
	void OnExtraItemDeleteByDate( SP2Packet &rkPacket );

	void OnPetData( SP2Packet &rkPacket );

	void OnCostumeData( SP2Packet &rkPacket );
	void OnCostumeDeleteByDate( SP2Packet &rkPacket );

	void OnTimeGrowthCheck( SP2Packet &rkPacket );
	
	void OnLevelUpItem( SP2Packet &rkPacket );
	
	void OnCharDelete( SP2Packet &rkPacket );
	void OnChangeLeaderChar( SP2Packet &rkPacket );
	void OnSetMyRentalChar( SP2Packet &rkPacket );
	void OnCharLimitCheck( SP2Packet &rkPacket );
	void OnCharDecorationBuy( SP2Packet &rkPacket );
	void OnCharExtend( SP2Packet &rkPacket );
	void OnExitRoomPenalty( SP2Packet &rkPacket );

	void OnAnnounce( SP2Packet &rkPacket );

	void OnLogOut( SP2Packet &rkPacket );
	void OnReserveLogOut( SP2Packet &rkPacket );

	void OnHackAnnounce( SP2Packet &rkPacket );
	void OnHackQuiz( SP2Packet &rkPacket );
	void OnAbuseAnnounce( SP2Packet &rkPacket );
	void OnAbuseQuiz( SP2Packet &rkPacket );
	void OnTutorialStepBonus( SP2Packet &rkPacket );
	void OnDeveloperMacro( SP2Packet &rkPacket );
	void OnGradeNClassUpBonus( SP2Packet &rkPacket );
	void OnMovingServer( SP2Packet &rkPacket );
	void OnReserveServerMoving( SP2Packet &rkPacket );
    void OnMovingServerComplete( SP2Packet &rkPacket );
	void OnReserverRoomJoin( SP2Packet &rkPacket );
	void OnExerciseCharCreate( SP2Packet &rkPacket );
	void OnExercisePcRoomCharCreate( SP2Packet &rkPacket );
	void OnDeleteExpiredCharByPCRoom( SP2Packet &rkPacket );

	void OnExerciseCharDelete( SP2Packet &rkPacket );

	void OnMemoSendMsg( SP2Packet &rkPacket );
	void OnOfflineMemo( SP2Packet &rkPacket );
	void OnFriendList( SP2Packet &rkPacket );
	void OnFriendRequestList( SP2Packet &rkPacket );
	void OnFriendApplication( SP2Packet &rkPacket );
	void OnFriendCommand( SP2Packet &rkPacket );
	void OnFriendDelete( SP2Packet &rkPacket );
	void OnBestFriendList( SP2Packet &rkPacket );
	void OnBestFriendInsert( SP2Packet &rkPacket );
	void OnBestFriendDeleteList( SP2Packet &rkPacket );
	void OnBestFriendDismiss( SP2Packet &rkPacket );
	void OnBestFriendExceptionList( SP2Packet &rkPacket );
	void OnUserLogin( SP2Packet &rkPacket );
	void OnChannelCreate( SP2Packet &rkPacket );
	void OnChannelInvite( SP2Packet &rkPacket );
	void OnChannelJoin( SP2Packet &rkPacket );
	void OnChannelLeave( SP2Packet &rkPacket );
	void OnChannelChat( SP2Packet &rkPacket );
	void OnChannelAllUser( SP2Packet &rkPacket );
	void OnRegisteredUser( SP2Packet &rkPacket );
	void OnUserPosRefresh( SP2Packet &rkPacket );
	void OnUserInfoRefresh( SP2Packet &rkPacket );
	void OnSimpleUserInfoRefresh( SP2Packet &rkPacket );
	void OnUserCharInfoRefresh( SP2Packet &rkPacket );
	void OnUserCharSubInfoRefresh( SP2Packet &rkPacket );
	void OnUserCharRentalRequest( SP2Packet &rkPacket );
	void OnUserCharRentalAgree( SP2Packet &rkPacket );
	void OnUserCharRentalReturn( SP2Packet &rkPacket );
	void OnBankruptcyPeso( SP2Packet &rkPacket );
	void OnTotalRegUserCount( SP2Packet &rkPacket );
	void OnStartCharLimitDate( SP2Packet &rkPacket );
	void OnUpdatecharLimitDate( SP2Packet &rkPacket );
	void OnCharCharge( SP2Packet &rkPacket );
	void OnEtcItemBuyResult( SP2Packet &rkPacket );
	void OnEventData( SP2Packet &rkPacket );
	void OnUserEntryRefresh( SP2Packet &rkPacket );
	void OnEventAlive( SP2Packet &rkPacket );

	void OnGuildCreate( SP2Packet &rkPacket );
	void OnMyGuildInfo( SP2Packet &rkPacket );
	void OnEntryDelayGuildLIst( SP2Packet &rkPacket );
	void OnGuildEntryDelayMember( SP2Packet &rkPacket );
	void OnGuildInfo( SP2Packet &rkPacket );
	void OnGuildMemberList( SP2Packet &rkPacket );
	void OnMyGuildMemberList( SP2Packet &rkPacket );
	void OnMyGuildLeaveList( SP2Packet &rkPacket );
	void OnGuildJoinerChange( SP2Packet &rkPacket );
	void OnGuildEntryApp( SP2Packet &rkPacket );
	void OnGuildEntryAgree( SP2Packet &rkPacket );
	void OnGuildEntryGuildDelete( SP2Packet &rkPacket );
	void OnGuildInvitation( SP2Packet &rkPacket );
	void OnGuildInvited( SP2Packet &rkPacket );
	void OnGuildLeave( SP2Packet &rkPacket );
	void OnGuildMasterChange( SP2Packet &rkPacket );
	void OnGuildPositionChange( SP2Packet &rkPacket );
	void OnGuildKickOut( SP2Packet &rkPacket );
	void OnGuildSimpleData( SP2Packet &rkPacket );
	void OnGuildMarkChange( SP2Packet &rkPacket );
	void OnGuildMarkChangeKeyValue( SP2Packet &rkPacket );
	void OnGuildChat( SP2Packet &rkPacket );
	void OnGuildTitleSync( SP2Packet &rkPacket );
	void OnGuildExist( SP2Packet &rkPacket );
	void OnGuildRoomActive( SP2Packet &rkPacket );

	void OnUserInfoExist( SP2Packet &rkPacket );
	void OnUserEtcItem( SP2Packet &rkPacket );
	void OnServiceChar( SP2Packet &rkPacket );
	void OnPresentData( SP2Packet &rkPacket );
	void OnPresentRecv( SP2Packet &rkPacket );
	void OnPresentSell( SP2Packet &rkPacket );

	// 청약
	void OnSubscriptionData( SP2Packet &rkPacket );
	void OnSubscriptionRecv( SP2Packet &rkPacket );
	void OnSubscriptionRetr( SP2Packet &rkPacket );

	void OnDefaultDecoItem( SP2Packet &rkPacket );
	void OnEventDataUpdate( SP2Packet &rkPacket );

	void OnProtectCheck( SP2Packet &rkPacket );
	void OnGetCash( SP2Packet &rkPacket );
	void OnHoleSendComplete( SP2Packet &rkPacket );
	void OnUDPRecvTimeOut( SP2Packet &rkPacket );
	void OnJoinHeadquarterInfo( SP2Packet &rkPacket );
	void OnWholeServerChat( SP2Packet &rkPacket );
	void OnWholeServerRainbowChat( SP2Packet &rkPacket );
	void OnStartEtcItemTime( SP2Packet &rkPacket );
	void OnUpdateEtcItemTime( SP2Packet &rkPacket );
	void OnDeleteEtcItemDate( SP2Packet &rkPacket );
	void OnUDPRecvTimeOutReCheck( SP2Packet &rkPacket );
	void OnCampDataSync( SP2Packet &rkPacket );
	void OnCampChangePos( SP2Packet &rkPacket );
	void OnRefreshLadderPointNRecord( SP2Packet &rkPacket );
	void OnCampSeasonBonus( SP2Packet &rkPacket );
	void OnGuildNameChange( SP2Packet &rkPacket );

	void OnCharChangePeriod( SP2Packet &rkPacket );
	void OnChangeUserName( SP2Packet &rkPacket );

	void OnBlockType( SP2Packet &rkPacket );
	void OnFirstChangeID( SP2Packet &rkPacket );
	void OnCheckSalePrice( SP2Packet &rkPacket );
	void OnGashaponList( SP2Packet &rkPacket );
	void OnControlKeys( SP2Packet &rkPacket );

	void OnQuestData( SP2Packet &rkPacket );
	void OnQuestCompleteData( SP2Packet &rkPacket );
	void OnQuestOccur( SP2Packet &rkPacket );
	void OnQuestAttain( SP2Packet &rkPacket );
	void OnQuestReward( SP2Packet &rkPacket );
	void OnQuestRepeatReward( SP2Packet &rkPacket );
	void OnQuestDelete( SP2Packet &rkPacket );
	void OnQuestEventAlive( SP2Packet &rkPacket );
	void OnQuestOneDayDelete( SP2Packet &rkPacket );
	void OnQuestAbuseAlarm( SP2Packet &rkPacket );

	void OnUserHeroData( SP2Packet &rkPacket );
	void OnHeroTop100Data( SP2Packet &rkPacket );

	void OnMedalItemData( SP2Packet &rkPacket );
	void OnMedalItemDeleteByDate( SP2Packet &rkPacket );
	void OnCustomMedalItemData( SP2Packet &rkPacket );
	void OnCustomMedalItemInsert( SP2Packet &rkPacket );

	void OnExpandMedalSlotData( SP2Packet &rkPacket );
	void OnExpandMedalSlotDeleteByDate( SP2Packet &rkPacket );

	void OnEventShopGoodsList( SP2Packet &rkPacket );
	void OnEventShopGoodsBuy( SP2Packet &rkPacket );

	void OnNProtectCheck( SP2Packet &rkPacket );

	void OnProtect( SP2Packet &rkPacket );
	void OnRoomStealthEnter( SP2Packet &rkPacket );
	void OnCustomItemSkinUniqueIndex( SP2Packet &rkPacket );
	void OnCustomItemSkinDelete( SP2Packet &rkPacket );	
	
	void OnHeadquartersOption( SP2Packet &rkPacket );
	void OnPCRoomAuthority( SP2Packet &rkPacket );

	void OnGetMileage( SP2Packet &rkPacket );
	void OnAddMileage( SP2Packet &rkPacket );

	void OnShutDownUser( SP2Packet &rkPacket );
	void OnSelectShutDownUser( SP2Packet &rkPacket );

	void OnFriendRecommendData( SP2Packet &rkPacket );
	void OnMacroNotify( SP2Packet &rkPacket );

	void OnDisconnectHDDSerialBlock( SP2Packet &rkPacket );

	void OnLoginMoveServer( SP2Packet &rkPacket );

	void OnSyncGiftCloverInfo( SP2Packet &rkPacket );
	void OnSyncFriendCloverInfo( SP2Packet &rkPacket );
	void OnEtcItemCloverUpdate( SP2Packet &rkPacket );
	void OnAddCash( SP2Packet &rkPacket );
	void OnFillCashUrl( SP2Packet &rkPacket );
	void OnRelativeGradeBonus( SP2Packet &rkPacket );
	void OnUpdateRelativeGradeExp( SP2Packet &rkPacket );
	void OnNexonSessionMsg( SP2Packet &rkPacket );
	void OnAttendanceCheck( SP2Packet &rkPacket );
	void OnAttendanceRecord( SP2Packet &rkPacket );
	void OnSearchPlazaRoom( SP2Packet &rkPacket );
	void OnJoinHeadQuarters( SP2Packet &rkPacket );
	void OnCreatePlaza( SP2Packet &rkPacket );
	void OnJoinHouse( SP2Packet &rkPacket );
	void OnCreateTutorialStage( SP2Packet &rkPacket );

	//Mission
	void OnMissionData( SP2Packet &rkPacket );
	
	//RollBook
	void OnRollBookReward( SP2Packet &rkPacket );

	//for Guild Reward
	void OnGuildAttend( SP2Packet &rkPacket );
	void OnGuildReceiveAttendReward( SP2Packet &rkPacket );
	void OnGuildRankReward( SP2Packet &rkPacket );
	void OnGuildMemberAttendInfo( SP2Packet &rkPacket );

	//Gender Change(for girl friend)
	void OnGenderChange( SP2Packet &rkPacket );

	void OnAccessoryData( SP2Packet &rkPacket );
	void OnAccessoryDeleteByDate( SP2Packet &rkPacket );

	void OnSpiritData( SP2Packet &rkPacket );
	void OnSpiritCompose( SP2Packet &rkPacket );
	void OnSpiritDecompose( SP2Packet &rkPacket );
	void OnSpiritConversion( SP2Packet &rkPacket );
	void OnSpiritSync( SP2Packet &rkPacket );
	void OnSpiritDelete( SP2Packet &rkPacket );

	void OnTimeGateResponse( SP2Packet &rkPacket );
	void OnTimeGateInfo( SP2Packet &rkPacket );

	void OnPracticeList( SP2Packet &rkPacket );
	void OnPracticeEnter( SP2Packet &rkPacket );

public:
	void SendSearchModeJoin( ExLoadingType eLoadingType = LOADING_SEARCH_MODE );

public:
	inline void SetExLoadingType( ExLoadingType eExLoadingType ){ m_eLoadingType = eExLoadingType; }
	
	inline ConnectedMoveMode GetConnectedMoveMode(){ return static_cast<ConnectedMoveMode>( m_eConnectedMoveMode ); }
	inline void SetConnectedMoveMode( ConnectedMoveMode eType ){ m_eConnectedMoveMode = eType; }
	inline void ResetConnectedMoveMode(){ m_eConnectedMoveMode = CMM_LOBBY; }
	inline bool IsStepByConnectTry(){ return m_bStepByConnectTry; }
	inline void SetStepByConnectTry( bool bStepByConnectTry ){ m_bStepByConnectTry = bStepByConnectTry ; }

public:
	bool IsSingleMode();

	enum REFRESHTYPE
	{
		RT_PROCESS,
		RT_ADD,
		RT_DELETE,
	};
	void ProcessMemoryUsageLOG( int iRefreshType = RT_PROCESS );
	void ChangeSwitch();

#if defined( USE_GA )
	// GA On, Off
	bool m_bGAStart;
	void SetGAStart( bool bOn ){ m_bGAStart = bOn; }
	bool GetGAStart(){ return m_bGAStart; }

	// GA 전송할 유저 ( Sampling )
	bool m_bGAUser;
	bool GetGAUser(){ return m_bGAUser; }

	DWORD m_dwGAAliveTime;

	// GA 에서 처음 내려주는 케시 정보는 스킵
	bool m_bGAFirstCash;
	void SetGAFirstCash( bool bFirst ){ m_bGAFirstCash = bFirst; }
	bool GetGAFirstCash(){ return m_bGAFirstCash; }

	// ETC 아이템 index, cash 정보
	struct stGAEtcInfo
	{
		int iSubCode;
		int iCash;

		stGAEtcInfo()
		{
			iSubCode	= 0;
			iCash		= 0;
		}
	};
	typedef std::vector< stGAEtcInfo > GAEtcInfoVec;
	GAEtcInfoVec m_vecGAEtcInfo;

	typedef std::map< int, GAEtcInfoVec > GAEtcInfoMap;
	GAEtcInfoMap m_mapGAEtcInfo;

	// Item code, Quantity 로 데이터 세팅
	void SetGAEtcSubInfo( int iSubCode, int iCash );
	void SetGAEtcMainInfo( int iMainCode );

	// Item code, Quantity 로 Cash 값 받아온다.
	int GetGAEtcCash( int iMainCode, int iSubCode );

	// Item code, 상점 세팅 순서로 Cash 값 받아온다.
	int GetGAEtcCash2( int iMainCode, int iNum );

	// Compulsion Mouse UnBusy First check
	bool m_bFirstUnMouseBusy;
#endif


public:
	bool IsFPSRender(){ return m_bFPSRender; }
	void SetFPSRender( bool bFPSRender ){ m_bFPSRender = bFPSRender; }

public:
	ioUserBlockStorage* GetGuildStorage(){ return m_pGuildBlockStorage; }
	ioUserBlockStorage* GetHomeStorage(){ return m_pHomeBlockStorage; }

public:
	LRESULT MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	BOOL LoginProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	bool OnNCPaint( HDC hDC );

	static void CALLBACK OnIMETimer(UINT uID, UINT uMsg, DWORD dwUser, DWORD dw1, DWORD dw2);

public:
	static ioApplication& GetSingleton();

protected:
	void SetRegHotKey( HWND hWnd );
	void SetUnRegHotKey( HWND hWnd );
	bool ApplyHotKey( WPARAM wParam );

protected:
	void OnRSoldierInfo( SP2Packet& rkPacket );

protected:
	void OnNamedTitleData( SP2Packet &rkPacket );
	void OnPriateRulletInfo( SP2Packet &rkPacket );
	void OnPriateRulletUse( SP2Packet &rkPacket );
	void OnPriateRulletRset( SP2Packet &rkPacket );

protected:
	void OnRenewalOakPacket( SP2Packet &rkPacket );

protected:
	void OnSoldierSetInfo( SP2Packet& rkPacket );

protected:
	void OnMatchingGamePacket( SP2Packet rkPacket );

protected:
	void OnDiceGamePacket( SP2Packet rkPacket );

private:
	void DeleteLSCFile();

private:
	void OnSucessionModeTime( SP2Packet &rkPacket );

public:
	bool IsSuccessionModeOpen();
	// 임시 렉권 by Minyeong
	bool IsOpenWebPageState();
	//

public:
	ioApplication();
	virtual ~ioApplication();
};

#define g_App ioApplication::GetSingleton()

//kyg 여기에 아이피포트 추가할것 