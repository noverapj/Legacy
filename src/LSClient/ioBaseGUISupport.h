#pragma once

#include "ioTargetMarker.h"
#include "ioFireItemMarker.h"
#include "ioHealItemMarker.h"
#include "ioBowItemMarker.h"
#include "ioCrossBowMarker.h"
#include "ioOtherMarker.h"

#include "ioOwnerMark.h"
#include "ioTeamCircle.h"
#include "ioGaugeHelper.h"
#include "ioFlagPointGaugeHelper.h"

class ioPlayState;
class ioEntityGroup;
class ioPlayMode;
class ioBaseChar;

typedef std::vector<ioTargetMarker*> ioMultipleTargetMarker;

enum TargetedType
{
	TT_SINGLE,
	TT_OTHER,	
};

enum TargetConstant
{
	NONE_TARGET = INT_MAX,
};

class ioShuffleCharStateUI;
class ioBaseGUISupport
{
protected:
	ioEntityGroup*	m_pGrp;
	ioPlayStage*	m_pCreator;	
	ioPlayMode*		m_pMode;
	ioBaseChar*		m_pOwner;
	
protected:
	ioTargetMarker*			m_pTargetMarker;
	ioTargetMarker*			m_pTargetMarkerOther;
	ioTargetMarker*			m_pBackUpMarker;

	ioMultipleTargetMarker	m_MultipleTargetMarker;
	int						m_MultipleTargetMax;

protected:
	ioMagicCircle *m_pMagicCircle;
	ioHashString   m_szMagicCircleTarget;

protected:
	static ioOwnerMark *m_pOwnerMark;

	ioTeamCircle  *m_pTeamCircle;
	ioGaugeHelper *m_pGauge;	
	bool           m_bDisableGauge;

	bool m_bVisibleBottomCircle;

	static bool m_bTeamCircleCheck;	// For TeamCircle Check
	
	// For Gauge Alpha
	bool m_bGaugeAlpha;
	DWORD m_dwGaugeCheckTime;
	static DWORD m_dwGaugeTic;
	static DWORD m_dwAlphaTic;

	int          m_iHeroTitle;

protected:
	ioFlagPointGaugeHelper * m_pFlagPointGauge;

	bool m_bEnableFlagPointGauge;
	DWORD m_dwFlagPointStartTime;

protected:
	int		m_iMonsterRewardExp;
	int		m_iMonsterRewardPeso;
	DWORD   m_dwMonsterRewardTime;

	//for RunningMan
protected:
	ioHashString m_szRunningManName;
	ioUIRenderImage *m_pRunningManTitleLeft;
	ioUIRenderImage *m_pRunningManTitleCenter;
	ioUIRenderImage *m_pRunningManTitleRight;

	// For Emoticon
protected:
	ioCharEmoticon *m_pCharEmoticon;

	static ioHashString m_StartEmoticon;
	static ioHashString m_ProtectEmoticon;
	static ioHashString m_ExitRoomEmoticon;
	static ioHashString m_KillEmoticon;
	static ioHashStringVec m_KillComboEmoticonList;
	static ioHashString m_KillComboMaxEmoticon;
	static ioHashString m_DeathEmoticon;
	static ioHashString m_EnablePickEmoticon;
	static ioHashString m_HitBlueIcon;
	static ioHashString m_HitRedIcon;
	//static ioHashString m_ComboBlueIcon;
	//static ioHashString m_ComboRedIcon;
	static ioHashStringVec m_IconComboNumberList;
	static ioHashStringVec m_ComboEmotionRedList;
	static ioHashStringVec m_ComboEmotionBlueList;
	static ioHashString m_MaxComboEmotionRed;
	static ioHashString m_MaxComboEmotionBlue;
	static ioHashString m_IconComboMax;
	static ioHashString m_ResistanceEmoticon;
	static ioHashString m_DisableAttackEmoticon;
	static ioHashString m_NoneGaugeEmoticon;
	static ioHashString m_ScreenShotEmoticon;
	static ioHashString m_HomePageEmoticon;
	static ioHashString m_ProtectWound;
	static ioHashStringVec m_OwnerRankEmoticon;
	static ioHashStringVec m_OtherRankEmoticon;
	static ioHashString m_CoinEventEmoticon;
	static ioHashString m_DefenseBreakEmoticon;
	static ioHashString m_BossEmoticon;

	// PvE 이모티콘
	static ioHashString m_TargetCommandEmoticon;
	static ioHashString m_TargetingEmoticon;

	DWORD m_dwMovieCapStartCurTime;
	DWORD m_dwMovieCapStartTime;
	int   m_iMovieStartSec;
	DWORD m_dwMovieCapturingCurTime;

//For Bubble
protected:
	ioCharChatBubble *m_pCharChatBubble;

protected:
	ioShuffleCharStateUI* m_pShuffleCharUI;

protected:
	DWORD m_dwNamedTitleCode;
	int m_nNamedTitleLevel;
	BYTE m_byNamedTitlePremium;
	ioUIRenderImage* m_pNamedTitleMark;

public:
	ioTargetMarker* CreateMarkerTemplete( ioTargetMarker::MarkerType eMarkType );

	void BackupCurrMarker();
	void RestoreBackupMarker();

	void SetTargetMarker( ioTargetMarker::MarkerType eMarkType, 
						const ioHashString &szReadyEffect,
						const ioHashString &szAimingEffect,
						const ioHashString &szAimedEffect,
						const ioHashString &szZeroAimedEffect,
						const ioHashString &szHeadAimedEffect,
						TargetedType eType = TT_SINGLE );
	void SetSpecialMarker( const ioHashString &szSpecialAimedEffect );

	void SetMultipleTargetMarker( ioTargetMarker::MarkerType eMarkType, 
								const ioHashString&	szReadyEffect,
								const ioHashString&	szAimingEffect,
								const ioHashString&	szAimedEffect,
								const ioHashString&	szZeroAimedEffect,
								const ioHashString&	szHeadAimedEffect,
								int					iMultipleTargetMax );

	void SetTargetMarkerEnable( bool bEnable, TargetedType eType = TT_SINGLE );
	void SetMultipleTargetMarkerEnable( bool bEnable, int iMarkerIndex );

	void SetTargetMarkerState( ioTargetMarker::MarkerState eMarkType, TargetedType eType = TT_SINGLE );
	void SetMultipleTargetMarkerState( ioTargetMarker::MarkerState eMarkType, int iMarkerIndex );

	void SetTargetMarkerTarget( const ioHashString &szTarget, bool bBall, TargetedType eType = TT_SINGLE );
	int SetMultipleTargetMarkerTarget( const ioHashString &szTarget, bool bBall );

	void SetTargetMarkerRange( float fRange, TargetedType eType = TT_SINGLE );
	void SetTargetMarkerYOffset( float fYOffset, TargetedType eType = TT_SINGLE );

	void SetMultipleTargetMarkerRange( float fRange );
	void SetMultipleTargetMarkerRange( float fRange, int iMarkerIndex );	

	void SetTargetMarkerJumpAngle( float fAngle, TargetedType eType = TT_SINGLE );
	void SetMultipleTargetMarkerJumpAngle( float fAngle, int iMarkerIndex );

	bool IsMultipleTargetMarkerTargeted( const ioHashString &szTarget );
	int GetMultipleTargetMarkerIndex( const ioHashString &szTarget );

	void UpdateMarker();
	void ClearTargetMarker();
	void ClearMultipleTargetMarker();

	ioTargetMarker::MarkerState GetMarkerState( TargetedType eType = TT_SINGLE );
	ioTargetMarker::MarkerState GetMultipleMarkerState( int iMarkerIndex );
	
public:
	ioTargetMarker* GetTargetMarker()		{ return m_pTargetMarker; }
	ioTargetMarker* GetTargetMarkerOther()	{ return m_pTargetMarkerOther; }

	// MagicCircle
public:
	virtual void CreateMagicCircle( const ioHashString &ReadyCircle, const ioHashString &EnableCircle, const ioHashString &GuidCircle, float fOffSet );
	virtual void CreateMagicCircle( const ioHashString &ReadyCircle, const ioHashString &EnableCircle, const ioHashString &GuidCircle, D3DXVECTOR3 vPos, float fOffSet );
	virtual void CreateMagicCircleForBlock( const ioHashString &ReadyCircle, const ioHashString &EnableCircle, const ioHashString &GuidCircle, float fOffSet, bool bHeightCheck = true );
	void DestroyMagicCircle();

public:
	ioMagicCircle* GetMagicCircle() { return m_pMagicCircle; }

	D3DXVECTOR3 GetMagicCircleStartPos();
	D3DXVECTOR3 GetMagicCirclePos( float fOffSet = 0.0f, bool bNoChangePos = false ) const;
	D3DXVECTOR3 GetMagicCirclePos( float fOffSet, float fYOffSet ) const;
	D3DXVECTOR3 GetMagicCirclePrePos();
	D3DXQUATERNION GetMagicCircleRot();

	void UpdateMagicCirclePos( float fMoveSpeed, float fRange, float fHeightGap, bool bEnableSkill, bool bNetwork = false );
	void UpdateMagicCirclePos( float fMoveSpeed, float fMinRange, float fMaxRange, float fHeightGap, bool bEnableSkill, D3DXVECTOR3 vStartPos, bool bNetwork = false );
	void UpdateMagicCirclePos( float fMoveSpeed, float fMinRange, float fMaxRange, float fMinAngle, float fMaxAngle, float fHeightGap, bool bEnableSkill, D3DXVECTOR3 vStartPos, bool bNetwork = false );
	void UpdateMagicCirclePosAll( float fMoveSpeed, float fRange, float fHeightGap, bool bEnableSkill );	// Owner, 고스트 모두 갱신
	void UpdateMagicCirclePosAll( float fMoveSpeed, float fMaxRange, float fMinRange, float fHeightGap, bool bEnableSkill );	// Owner, 고스트 모두 갱신
	void UpdateMagicCircleToCirclePosAll( float fMoveSpeed, 
		float fMaxRange,
		float fMinRange,
		float fHeightGap,
		float fHeightMin,
		float fHeightMax,
		D3DXVECTOR3 vCharPos,
		bool bEnableSkill,
		bool bEnableHeightGap = true );
	void UpdateMagicCirclePosForBlock( float fMoveSpeed, float fRange, float fHeightGap, bool bEnableSkill, bool bCheckHeight = true );
	void UpdateMagicCirclePosAll2( float fMoveSpeed, float fRange, float fLimitHeight, bool bEnableSkill );	// Owner, 고스트 모두 갱신
	void UpdateMagicCirclePosByStartPos( float fMoveSpeed, float fRange, float fLimitHeight, bool bEnableSkill );

	void SetMagicCirclePos( bool bEnableSkill, const D3DXVECTOR3& vPos );
	void SyncMagicCirclePos( bool bVisibleTime, bool bCharInfo );

	void ChangeMagicCircle( bool bVisible=true );
	void CheckMagicCircleVisible( bool bEnableRender );

	// 사신 머리 스킬 테스트
	void SetMagicCircleTarget( const ioHashString &szCircleTarget );
	const ioHashString &GetMagicCircleTarget(){ return m_szMagicCircleTarget; }
	D3DXVECTOR3 GetMagicCircleTargetPos();
	D3DXQUATERNION GetMagicCircleTargetRot();

	bool CheckMagicCircle() const;
	bool ApplyMagicCircle( SP2Packet &rkPacket );
	bool CheckChangeDirectionForMagicCircle();		// 방향키 바뀔때마다 마법진 동기화 위한 함수

public:
	static void _InitStaticINI_GUIValue( ioINILoader &rkLoader );

	static void InitOwnerMark();
	static void ClearOwnerMark();

	void InitINI_GUIValue( ioINILoader &rkLoader );
	void ReSetOwnerMarkTeam();

	bool IsDisableGauge(){ return m_bDisableGauge; }
	void SetDisableGauge( bool bDisable ){ m_bDisableGauge = bDisable; }

	void SetEnableFlagPointGauge( bool bEnable );

	void SetTeamUI( TeamType eType );
	void UpdateBottomCircle();
	void DestroyTeamCircle();
	bool IsVisibleBottomCircle();
	bool IsVisibleBottomShadow();
	void SetBottomCircleVisible( bool bVisible );

	void CheckGaugeTeam( bool bChange );

	void SetIsOwnerCharUI( TeamType eType );

	inline void SetHeroTitle( const int iHeroTitle ) { m_iHeroTitle = iHeroTitle; }
	inline const int& GetHeroTitle() const { return m_iHeroTitle; }
	void SetMonsterReward( int iMonsterRewardExp, int iMonsterRewardPeso );

	void UpdateGaugeGUI();

	void RenderGaugeBar( int iGaugeXPos, int iGaugeYPos, int iOwnerIconOffsetX, int iOwnerIconOffsetY, int iOwnerIconOffsetExY );
	void NormalRenderGauge( int iOwnerIconOffsetX, int iOwnerIconOffsetY, int iOwnerIconOffsetExY );
	void ObserverRenderGauge( int iOwnerIconOffsetX, int iOwnerIconOffsetY, int iOwnerIconOffsetExY );
	void RenderMonsterReward();

	bool GetRenderGaugeRect( RECT *pRect ) const;

	inline void SetRunningManName( const ioHashString &rkName ) { m_szRunningManName = rkName; }
	void RunningManName( int iXPos, int iYPos );
	void InitRaceInfoForRunningMan( const int& dwRunningManDecoIndex, const CHARACTER &rkCharInfo );

protected:
	bool IsEasterEgg();

public:
	bool IsVisibleID();

//For Emotion
public:
	void EndEmoticonByName( const ioHashString &rkDescName );
	void SetLongTimeLiveEmoticon( const ioHashString &szEmoticon );
	void SetLongTimeLiveEmoticonAndScale( const ioHashString &szEmoticon, bool bFast );
	void EndLongTimeLiveEmoticon();

	void SetEmoticon( const ioHashString &szEmoticonName, EmoticonType eType = EMT_DEFAULT, DWORD dwDuration = 0 );
	void SetNamedEmoticon( const ioHashString &szIconName, const ioHashString &szDescName, DWORD dwColor , DWORD dwDuration = 0);
	void SetNamedUnLimitedEmoticon( const ioHashString &szIconName, const ioHashString &szDescName, DWORD dwColor );
	void Set3DChatEmoticon( int iChatLen, bool bNoSound );

	void SetHitEmoticon( const ioHashString &szEmoticonName, const ioHashString &szIconComboNum, int iHitComboCnt );
	void SetKillPointEmoticon( const ioHashString &szEmoticonName, const ioHashString &szIconNum, const ioHashString &szPer, int iNum );
	void SetFishingGradeEmoticon( const ioHashString &szEmoticonName, const ioHashString &szGrade, DWORD dwDuration = 0 );
	void SetFishingLevelUpEmoticon( const ioHashString &szEmoticonName, const ioHashString &szLevelUp, DWORD dwDuration = 0 );
	void SetUserRankEmoticon();
	void SetBossLevelEmoticon( int iLevel );
	void SetTargetCommandEmoticon();
	void SetTargetingEmoticon();
	void SetDisableAttackEmoticon();
	void SetScreenShotEmoticon();
	void SetHomePageEmoticon();
	void SetPraiseEmoticon( int iType, int iValue );
	void SetMovieStartEmoticon( DWORD dwStartTime );
	void SetMovieCutEmoticon();
	void SetNetworkBadEmoticon();
	void SetBlockInterPlayEmoticon( const ioHashString& szInterPlayDesc, DWORD dwInterPlayDsecColor );
	void EndUnLimitedEmoticon();

	void UpdateEmoticon( bool bSystemVisible, ioBaseChar *pCameraTarget );

	void ProcessBossModeChar();
	void ProcessBattleRoomTeamEmoticon();
	void ProcessMovieMakerEmoticon();

	void RenderEmoticon( const RECT *pTop, const RECT *pBottom, bool bCollisionRender );

	bool IsEmoticonEvent();

	bool CheckTeamEmoticon() const;

//For Bubble
public:
	ioCharChatBubble *GetCharChatBubble(){ return m_pCharChatBubble; }
	bool IsVisibleBubbleChat();

	void SetChatBubble( const char *szID, const char *szChat, bool bWholeChat, DWORD dwCurrentTime, DWORD dwDuration=0 );
	void SetChatBubble( const char *szID, const char *szChat, bool bWholeChat );
	void SetChatBubbleEmoticon( const ioHashString &rkEmoticonName );
	void Set3DChatBubbleEmoticon( int iChatLen, bool bNoSound );

	void UpdateChatBubble();

	void RenderChatBubble( ioBaseChar *pCameraTarget );

//For ShuffleMode
public:
	void UpdateShuffleModeUI( bool bSystemVisible, ioBaseChar *pCameraTarget );
	void RenderShuffleModeUI( const RECT *pTop, const RECT *pBottom, bool bCollisionRender );
	ioShuffleCharStateUI* GetShuffleCharUI(){ return m_pShuffleCharUI; }

// 칭호 관련
public:
	void SetNamedTitle( DWORD dwCode, int nLevel, BYTE byPremium );

public:
	ioBaseGUISupport( ioEntityGroup *pGrp, ioPlayStage *pCreator, ioPlayMode *pMode, ioBaseChar* pOwner );
	virtual ~ioBaseGUISupport();
};