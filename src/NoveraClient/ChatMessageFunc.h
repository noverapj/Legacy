#pragma once

#include "ioComplexStringPrinter.h"
#include "GUI/MannerTrialChat.h"

enum ChatType
{
	CT_NONE = -1,
	CT_TEAM,
	CT_ALL,
	CT_PRIVATE,
	CT_ANNOUNCE,
	CT_PARTY,
	CT_LADDER,
	CT_SERVER_LOBBY,
	CT_WHOLE_SERVER,
	CT_WHOLE_SERVER_RAINBOW,
};

/*
	로비와 게임의 채팅 내용이 동일해도 채팅 머릿말 부분의 이미지와 채팅 스타일/컬러가 다르므로 스타일이 추가되었다.
	불편한 시스템이지만 로비와 게임의 채팅 메니저를 각각 두지 않으므로해서 패킷이 분리 될 필요가 없어짐.   2010.02.24 LJH
*/
enum MsgStyleType
{
	MSG_NONEHEAD_CHAT = 1,      //헤더가 없는 채팅
	MSG_INFO_CHAT     = 2,      //정보 (기존 시스템 알림으로 쓴 참여, 킬, 입장, 채널알림 같은것) / 낚시 (내 낚시 경험치와 해당방 낚시 정보)
	MSG_USER_CHAT     = 3,      //로비 / 훈련소 / 진영
	MSG_WHOLE_CHAT    = 4,      //외침
	MSG_ANNOUNCE_CHAT = 5,      //알림 (낚시대박, 영구당첨, 고강화달성, 고레벨달성, 이벤트당청 등등)
	MSG_BLUE_ALL_CHAT = 6,      //전체 (블루)
	MSG_RED_ALL_CHAT  = 7,      //전체 (레드)
	MSG_BLUE_TEAM_CHAT= 8,      //팀 (블루)
	MSG_RED_TEAM_CHAT = 9,      //팀 (레드)
	MSG_OBSERVER_CHAT = 10,     //관전자
	MSG_WHOLE_CHANGE_COLOR_CHAT = 11,	//외침 (색이 지속적으로 변하는)
};

enum MsgSortFeature
{
	MSF_NORMAL_CHAT		= 0x1,
	MSF_WHOLE_CHAT		= 0x2,
	MSF_SYSTEM_CHAT		= 0x4,

	MSF_NO_CHAT			= 0x8,
};

void SetChatMsg( TeamType eTeam, const char *szID, const char *szChat, bool bNoChatSound, bool bCustomSound = false );
void SetServerLobbyChatMsg( TeamType eTeam, const char *szID, const char *szChat, bool bNoChatSound );
void SetTeamChatMsg( TeamType eTeam, const char *szID, const char *szChat, bool bNoChatSound, bool bCustomSound = false );
void SetWholeServerChatMsg( const char *szID, const char *szChat, bool bNoChatSound );
void SetWholeServerRainbowChatMsg( const char *szID, const char *szChat, bool bNoChatSound );
void SetKickUserChatMsg( const char *szID, TeamType eTeam );

void SetPartyChatMsg( TeamType eTeam, const char *szID, const char *szChat, bool bNoChatSound );

void SetLadderChatMsg( const char *szID, const char *szChat, bool bNoChatSound );

void SetMemberBalanceMsg( TeamType eTeam );

void SendChatMsg( TeamType eTeam, const char *szID, const char *szChat, ModeType eModeType = MT_NONE , const int iRoomIndex = -1,
				  bool bCustomSound = false );
void SendServerLobbyChatMsg( const char *szID, const char *szChat );
void SendTeamChatMsg( TeamType eTeam, const char *szID, const char *szChat, ModeType eModeType = MT_NONE , const int iRoomIndex = -1,
					  bool bCustomSound = false );
void SendAnnChatMsg( const char *szID, const char *szChat );
void SendPartyChatMsg( TeamType eTeam, const char *szID, const char *szChat );
void SendLadderChatMsg( const char *szID, const char *szChat );
void SendWholeServerChatMsg( const char *szID, const char *szChat );
void SendWholeServerRainbowChatMsg( const char *szID, const char *szChat );

// Chat Msg Class...
class ioUIRenderImage;

typedef struct tagRainbowStyle
{
	int   m_iRedColorMin;
	int   m_iRedColorMax;
	DWORD m_dwRedColorTicTime;
	DWORD m_dwRedColorSpeed;
	bool  m_bRedReverse;

	int   m_iGreenColorMin;
	int   m_iGreenColorMax;
	DWORD m_dwGreenColorTicTime;
	DWORD m_dwGreenColorSpeed;
	bool  m_bGreenReverse;

	int   m_iBlueColorMin;
	int   m_iBlueColorMax;
	DWORD m_dwBlueColorTicTime;
	DWORD m_dwBlueColorSpeed;
	bool  m_bBlueReverse;

	DWORD m_dwRainbowDuration;

	//숫자가 min->max인지 max->min인지


	tagRainbowStyle()
	{
		m_iRedColorMin = m_iRedColorMax = m_dwRedColorTicTime = 0;
		m_iGreenColorMin = m_iGreenColorMax = m_dwGreenColorTicTime = 0;
		m_iBlueColorMin = m_iBlueColorMax = m_dwBlueColorTicTime = 0;
		m_dwRainbowDuration = 0;
		m_bRedReverse = m_bGreenReverse = m_bBlueReverse = false;
		m_dwRedColorSpeed = m_dwBlueColorSpeed = m_dwGreenColorSpeed = 0;
	}

}RainbowStyle;
typedef std::vector< tagRainbowStyle > RainbowStyleVec;
typedef std::vector< RainbowStyleVec > RainbowTypeVec;

typedef struct tagMsgStyle
{
	ioUIRenderImage *m_pNormalHeadImage;
	ioUIRenderImage *m_pOutlineHeadImage;

	TextStyle m_eNormalHeadStyle;
	TextStyle m_eCustomHeadStyle;

	TextStyle m_eNormalChatStyle;
	TextStyle m_eCustomChatStyle;

	DWORD m_dwNormalChatOverColor;
	DWORD m_dwCustomChatOverColor;

	// 로비 기준의 채팅 컬러 - 게임중에는 아래 컬러에 매칭되는 값으로 교체 
	DWORD m_dwHeadColor;
	DWORD m_dwChatColor;
	DWORD m_dwBlueIDColor;
	DWORD m_dwRedIDColor;


	RainbowTypeVec m_RainbowTypeList;
	
	tagMsgStyle()
	{
		m_pNormalHeadImage = m_pOutlineHeadImage = NULL;
		m_eNormalHeadStyle = m_eNormalChatStyle = TS_NORMAL;
		m_eCustomHeadStyle = m_eCustomChatStyle = TS_OUTLINE_FULL_3X;
		m_dwNormalChatOverColor = m_dwCustomChatOverColor = 0;
		m_dwHeadColor = m_dwChatColor = m_dwBlueIDColor = m_dwRedIDColor = 0;
	}

	TextStyle GetHeadStyle();
	TextStyle GetChatStyle();
	DWORD GetChatOverColor();
}MsgStyle;

class IMsgItem
{
protected:
	MsgStyle    *m_pMsgStyle;

	BYTE		m_Alpha;
	DWORD		m_dwCurTime;
	DWORD		m_dwStartTime;
	DWORD		m_dwSentenceIndex; // index가 같으면 개행으로 분리된 한 문장.
	bool		m_bFirstLine;      // 개행으로 분리된 문장의 가장 첫 문장.
	bool        m_bMultiLine;      // 개행으로 분리된 문장이다.
	bool		m_bCustomSound;
	ChatType	m_eChatType;
	ioHashString m_szHead;

public:
	inline BYTE  GetAlpha() const { return m_Alpha; }
	inline DWORD GetSentenceIndex() const { return m_dwSentenceIndex; }
	inline bool  IsFirstLine() const { return m_bFirstLine; }
	inline bool  IsMultiLine() const { return m_bMultiLine; }
	inline ChatType GetChatType() const { return m_eChatType; }
	inline bool  IsCustomSound() const { return m_bCustomSound; }
	
public:
	virtual bool DrawItem( int iXPos, int iYPos, bool bOver ) = 0;
	virtual void ProcessItem( DWORD dwStartTime, DWORD dwAlphaTime ) = 0;
	virtual bool CheckPtInItemRect( const POINT &ptPos, int iHeight, ioHashString &szReturn, bool bAlphaNoCheck = false ) { return false; }
	virtual ioHashString *GetChatID() {return NULL;}
	virtual void SetStartTime( DWORD dwStartTime ) = 0;
	virtual float GetChatWidth() { return 0.0f; }
	virtual void AlphaTimeInit(){};
	virtual void ChangeProcess(){};
	virtual void ChangeTextColor( int iColorType ) {};
public:
	IMsgItem(){};
	virtual ~IMsgItem(){};
};

class ChatMsgItem : public IMsgItem
{
protected:
	DWORD m_dwHeadColor;
	ioComplexStringPrinter m_HeadPrinter;
	ioComplexStringPrinter m_HeadPrinterOver;
	ioComplexStringPrinter m_ChatPrinter;
	ioComplexStringPrinter m_ChatPrinterOver;
	ioHashString m_szChatID;

	int			m_iCurChatColorRed;
	int			m_iCurChatColorGreen;
	int			m_iCurChatColorBlue;
	DWORD		m_dwRainbowStartTime;
	DWORD		m_dwRainbowRedStartTime;
	DWORD		m_dwRainbowBlueStartTime;
	DWORD		m_dwRainbowGreenStartTime;
	int			m_iCurStyleNum;
	
public:
	virtual bool DrawItem( int iXPos, int iYPos, bool bOver );
	virtual void ProcessItem( DWORD dwStartTime, DWORD dwAlphaTime );
	virtual bool CheckPtInItemRect( const POINT &ptPos, int iHeight, ioHashString &szReturn, bool bAlphaNoCheck = false);
	virtual ioHashString *GetChatID() {return &m_szChatID;}
	virtual void SetStartTime( DWORD dwStartTime );
	virtual float GetChatWidth();
	virtual void AlphaTimeInit();
	virtual void ChangeProcess();
	virtual void ChangeTextColor( int iColorType );

public:
	ChatMsgItem( MsgStyle *pMsgStyle, const ioHashString &szHead, ioComplexStringPrinter &ChatPrinter, const char *szID, DWORD dwSentenceIndex,
				 bool bFirstLine, bool bMultiLine, ChatType eChatType, bool bCustomSound );
	virtual ~ChatMsgItem();
};
//-------------------------------------------------------------------------------
class ioPlayStage;
class ioSP2ChatManager : public Singleton< ioSP2ChatManager >
{
public:
	enum
	{
		NORMAL_COLOR_TYPE = 0,
		CUSTOM_COLOR_TYPE = 1,
		COLOR_TYPE_MAX	  = 2,
	};

protected:
	struct ColorFlipTable
	{
		DWORD m_dwNolmalColor;
		DWORD m_dwCustomColor;
	};
	typedef std::vector< ColorFlipTable > vColorFlipTable;
	vColorFlipTable m_HeadColorTable;
	vColorFlipTable m_ChatColorTable;
	DWORD m_dwColorTableType;

protected:
	typedef std::map< DWORD, MsgStyle* > MsgStyleMap;
	MsgStyleMap m_MsgStyleMap;
	MsgStyle    *m_pCurMsgStyle;

protected:
	enum 
	{
		MAX_SPLIT_LINE = 10,
		MSG_BUFF_SIZE  = 1024,
	};
	char m_szMsgBuf[MSG_BUFF_SIZE];

	typedef std::deque< IMsgItem* > ChatMsgList;
	ChatMsgList  m_ChatList;
	ChatMsgList  m_ChatListByFeature;
	ChatMsgList	 m_TextHelp;

	DWORD		m_dwMsgSortFeature;

	int          m_iMaxViewMsgCnt;
	int          m_iMaxHelpMsgCnt;
	float        m_fChatFontScale;
	int			 m_iMaxLimitMsgCnt;
	int          m_iMaxLimitAlphaMsg;
		
	DWORD		 m_dwLastMsgAddTime;
	DWORD		 m_dwDeleteMsgDuration;

	DWORD        m_dwStartAlphaTime;
	DWORD        m_dwCurAlphaTime;
		
	ioHashString m_CurChatSound;
	ioHashString m_SystemMsgSound;
	ioHashString m_EventMsgSound;
	ioHashStringVec m_DefaultChatSoundList;
	
	bool		 m_bNoChatSound;

	float        m_fSplitWidth;
	DWORD        m_dwSentenceIndexer;

	ioPlayStage  *m_pPlayStage;
	
protected:
	void ClearChatStyleMap();

protected:
	bool CheckHanByte( char *szText );
	void SplitText( const char *szText, float fFirstSplitWidth , OUT char szSplitText[MAX_SPLIT_LINE][MAX_PATH]);
	void AddSentenceIndex( bool bFirsLine );
	void AddChatSplitItem( const char *a_head, ioComplexStringPrinter &a_rkPrinter, const char *a_szID ,const char *a_szChat , ChatType a_eChatType = CT_NONE , bool a_bCustomSound = false );
	void AddSystemMsg(); // for SetSystemMsg()
	
public:
	void InitChatManager();
	void InitRainbowChat( ioINILoader& kLoader, MsgStyle* pMsgStyle );
	void DeleteAllMsg();

	void AddChatItem( const char *a_head, ioComplexStringPrinter &rkChatPrinter, const char *szID, bool bFirstLine, bool bMultiLIne, ChatType eChatType = CT_NONE, bool bCustmSound = false );
	void AddHelpItem( const char *a_head, ioComplexStringPrinter &rkChatPrinter, const char *szID, bool bFirstLine, bool bMultiLIne, ChatType eChatType = CT_NONE, bool bCustmSound = false );
	void AddHelpSplitItem( const char *a_head, ioComplexStringPrinter &a_rkPrinter, const char *a_szID ,const char *a_szChat , ChatType a_eChatType = CT_NONE , bool a_bCustomSound = false );
	void ReArrangeByMaxLimit();
	void DestroyHeadMessage();
	void AllAlphaTimeInit();

	// Load
	void LoadChatInfo( const ioHashString &rkLoadTitle );
		
	// Chat Setting..
	void SetCurChatSound( ioHashString szSound );
	void SetMaxLimitMsg( int iLimit );
	void SetLimitAlphaMsg( int iLimit );
	void SetDelMsgDuration( DWORD dwDelDuration );
	void SetSplitWidth( float fWidth );
	void SetAlphaFade( DWORD dwStartTime, DWORD dwCurTime );
	void SetMsgStyle( MsgStyleType eStyle );
	void SetMaxViewMsgCnt( int iCount ){ m_iMaxViewMsgCnt = iCount; }

	inline void SetNoChatSound( bool bNoChatSound ) { m_bNoChatSound = bNoChatSound; }
	inline bool IsNoChatSound() { return m_bNoChatSound; }
	inline float GetChatFontScale(){ return m_fChatFontScale; }

	// Chat Msg Process
	void ProcessChat();

	// Cha Msg List Sort By Feature
	void SetMsgWndFeature( DWORD dwFeature );
	void AddChatItemByFeature( IMsgItem *pItem );
	bool CheckEnableAddByFeature( IMsgItem *pItem );

	int GetMaxChatMsgByFeature();
	IMsgItem *GetReverseChatMsgByFeature( int list_count );
	IMsgItem *GetChatMsgByFeature( int list_count );
		
	// Chat Msg List Get
	IMsgItem *GetReverseChatMsg( int list_count );
	IMsgItem *GetChatMsg( int list_count );
	inline int GetMaxChatMsg() { return m_ChatList.size(); }
	inline int GetMaxChatViewMsg(){ return m_iMaxViewMsgCnt; }
	inline int GetMaxHelpViewMsg(){ return m_iMaxHelpMsgCnt; }

	//
	void SetPlayStage( ioPlayStage *pPlayStage );

	//
	DWORD GetColorTableType();
	DWORD ChangeColorTableHead( DWORD dwColor );
	DWORD ChangeColorTableChat( DWORD dwColor );
	TextStyle GetTextStyle();
	DWORD GetChatColor();
	DWORD GetChatBlueIDColor();
	DWORD GetChatRedIDColor();
	DWORD GetChatBkColor();

public:
	void SetMsg( const char *szSource );          //[알림], [전체] 이런거 없이 그냥 채팅창에 출력
	void SetMsg( const char *szSource, ioComplexStringPrinter& kPrinter );          //수련장 로비 설명 스크롤용 
	void SetRecvChatMsg( DWORD dwColor, const char *szID, const char *szChat );
	void SetChatMsg( TeamType eTeam, const char *szID, const char *szChat, bool bNoChatSound, bool bCustomSound = false );
	void SetServerLobbyChatMsg( TeamType eTeam, const char *szID, const char *szChat, bool bNoChatSound );
	void SetTeamChatMsg( TeamType eTeam, const char *szID, const char *szChat, bool bNoChatSound, bool bCustomSound = false );
	void SetPartyChatMsg( TeamType eTeam, const char *szID, const char *szChat, bool bNoChatSound );
	void SetLadderChatMsg( const char *szID, const char *szChat, bool bNoChatSound );
	
public:    // 용병, 경험치, 페소, 장비, 치장, 특별 아이템 획득
	void SetSoldierObtainMsg( int iClassType, int iLimitDate, int iPeriodType, int iExerciseStyle );
	void SetGradeExpObtainMsg( int iGradeExp );
	void SetPesoObtainMsg( int iPeso );
	void SetDecoObtainMsg( int iClassType, int iSexType, int iDecoType, int iDecoCode );
	void SetEtcItemObtainMsg( int iItemType, int iValue );
	void SetExtraItemObtainMsg( int iItemType, int iReinforce, int iPeriodType, int iDate );
	void SetMedalItemObtainMsg( int iItemType, int iLimitTime );
	void SetCostumeObtainMsg( int nCostumeCode, int nPeriodType, int nDate );

#ifdef USE_LIST_ARQUMENTS_TEXT_FUNCTION
	void SetSystemMsg( const char *szSource, ... );
#else
	void SetSystemMsg( const char *szSource );
#endif
	void SetSystemMsg( const char *szSource, int iArg1 );
	void SetSystemMsg( const char *szSource, const char *szArg1 );
	void SetSystemMsg( const char *szSource, int iArg1, int iArg2 );
	void SetSystemMsg( const char *szSource, const char *szArg1, int iArg2 );
	void SetSystemMsg( const char *szSource, const char *szArg1, int iArg2, int iArg3 );
	void SetSystemMsg( const char *szSource, int iArg1, const char *szArg2, int iArg3, int iArg4 );
	void SetSystemMsg( const char *szSource, const char *szArg1, int iArg2, int iArg3, int iArg4 );
	void SetSystemMsg( const char *szSource, int iArg1, int iArg2, int iArg3, int iArg4, int iArg5, int iArg6 );

	void SetInfomationMsg( const char *szMsg );
	void SetKOMsgTeamColor( TeamType eTeam, const ioHashString &szID, ioComplexStringPrinter &rkPrinter );	

	void SetScoreMsg( TeamType eTeam, ioHashString szName, bool bParty );
	void SetLastScoreMsg( TeamType eTeam, ioHashString szName );
	void SetSymbolDieMsg( TeamType eTeam, const ioHashString &szName );
	void SetGetCrownMsg( TeamType eTeam, const ioHashString &szName );
	void SetDropCrownMsg( TeamType eAttacker, const ioHashString &szAttacker, TeamType eKing, const ioHashString &szKing );
	void SetBadPingCrownDropMsg( TeamType eKing, const ioHashString &szKing );
	void SetDropFlagMsg( TeamType eTeam, const ioHashString &szName );
	void SetPickupFlagMsg( TeamType eTeam, const ioHashString &szName );
	void SetBadPingFlagDropMsg( TeamType eTeam, const ioHashString &szName );
	void SetGoalMsg( TeamType eGoalTeam, TeamType eAttackerTeam, const ioHashString &szAttacker,
					 TeamType eAssistTeam, const ioHashString &szAssist );
	
	void SetJoinUserChatMsg( const char *szID, TeamType eTeam, int nGrade );
	void SetOutUserChatMsg( const char *szID, TeamType eTeam );
	void SetKickUserChatMsg( const char *szID, TeamType eTeam );
	void SetBadNetworkKickUserChatMsg( const char *szID, TeamType eTeam );
	void SetKickHighLevelUserChatMsg( const char *szID, TeamType eTeam );
	void SetKickLowLevelUserChatMsg( const char *szID, TeamType eTeam );
	void SetKickSysError( const char *szID, TeamType eTeam );

	void SetHostChange( const char *szTitle, const char *szHostID, TeamType eTeam );

	void PresetChatComplexString( MsgStyleType eStyle, ioComplexStringPrinter &rkChatPrinter );
	void SetChatComplexString( const ioHashString &szHead, ioComplexStringPrinter &rkChatPrinter, bool bPlaySound = true );

	void SetMemberBalanceMsg( TeamType eTeam );

	void SetEventMsg( const char *szSource, DWORD dwColor = 0xFFE5E5E5 ); // TCT_DEFAULT_IVORY

	void SetWholeServerChatMsg( const char *szID, const char *szChat, bool bNoChatSound );

	void SetWholeServerRainbowChatMsg( const char *szID, const char *szChat, bool bNoChatSound );

public:
	void SendChatLog( ioMannerTrialChatManager::TrialType eTrialType, const char *szID, const char *szChat, ModeType eModeType = MT_NONE , const int iRoomIndex = -1);

protected:
	ioHashString GetDefaultChatSound();

public:
	static ioSP2ChatManager& GetSingleton();
		
public:
	ioSP2ChatManager();
	virtual ~ioSP2ChatManager();
};

#define g_ChatMgr ioSP2ChatManager::GetSingleton()
