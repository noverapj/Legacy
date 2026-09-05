#pragma once

#include "../ioTargetMarker.h"

class ioBaseChar;
class ioPlayStage;
class ioTutorialManager : public Singleton< ioTutorialManager >
{
public:
	enum TutorialSkillResult
	{
		TSR_OK		= 1,
		TSR_ERROR1	= 2,
		TSR_ERROR2	= 3,
		TSR_ERROR3	= 4,
		TSR_ERROR4	= 5,
	};

protected:
	struct StepList 
	{
		ioHashString m_szTitle;
		ioHashString m_szHelp1;
		ioHashString m_szHelp2;
		ioHashString m_szComplete;
		ioHashString m_szCombo1;
		ioHashString m_szCombo2;
		int          m_iComboCount;
		int          m_iMaxValue;

		StepList();
		~StepList();
	};

	struct Tutorial
	{
		int m_iMaxStep;
		int m_iSetCode;
		typedef std::vector< StepList * > vStepList;
		vStepList m_vStepList;

		Tutorial();
		~Tutorial();
	};

	typedef std::vector< Tutorial * > vTutorialList;
	vTutorialList m_vTutorialList;
	void ClearList();

protected:
	enum
	{
		IRON_SET_CODE		= 1,
		HOOK_SET_CODE,
		WESTERN_SET_CODE,
		FIRE_SET_CODE,
		MEDIC_SET_CODE,
		SAVAGE_SET_CODE,
		ROBIN_SET_CODE,
		CRAZY_SET_CODE,
		ICE_SET_CODE,
		SOLDIER_SET_CODE,
		SPACE_SET_CODE,
		SHADOW_SET_CODE,
		DEFENDER_SET_CODE,
		BLUEDRAGON_SET_CODE,
	};

	enum
	{
		STEP_1 = 0,
		STEP_2,
		STEP_3,
		STEP_4,
		STEP_5,
	};

	Tutorial *m_pTutorial;
	int       m_iCurStep;
	int       m_iCurValue;

	int       m_iTutorialState;

	ioHashString m_szCountUpSnd;
	ioHashString m_szStartSnd;
	ioHashString m_szNextSnd;
	ioHashString m_szClearSnd;

	bool      m_bRandomItemDrop;
	bool      m_bCharInitPos;
	bool      m_bCharDropStepPos;
	bool      m_bEnableMagicCircleMoving;
	
	void SetDataGUI();
	void CountUpTutorial();

public:
	void LoadTutorialList();
	void InitTutorial( int iState );
	
	void OnStart();
	void OnNext();
	void OnClear( bool bSound = true );
	void OnAllSkip();

	void SetSound( const ioHashString &szCountUpSnd,
				   const ioHashString &szStartSnd,
				   const ioHashString &szNextSnd,
				   const ioHashString &szClearSnd );

	bool IsTutorial();
	bool IsShowSkillStep();
	bool IsShowExerciseStep();

	bool StartRandomItemDrop();
	bool StartCharInitPos();
	bool StartCharDropStepPos();

public:
	void ActionCharacter( int iState );

	void ActionJumpAttack();
	void ActionChargeAttack();
		
	void ActionDoubleJump();
	void ActionHighJump();

	void ActionChangeToAiming();
	void ActionAimed();
	void ActionZeroAimed();
	void ActionAimingCancel();
	void ActionAimingMove();

	void ActionRollingDash();
	void ActionSuperDash();
	void ActionDashSpear();
	void ActionAssaultDash();

	void ActionWeaponCharge();

	void ActionDefenseMove();
	
	void ActionAimedFireDeveloperK( const ioHashString &rszPublicID );
	void ActionNormalFire();
	void ActionBombFire();
	void ActionMineBurying();
	
	int ActionUseSkill( DWORD dwSetItemCode, const ioHashString &szSkillName 
		                ,const ioTargetMarker::MarkerState eMarkerState = ioTargetMarker::MS_NONE );
	void ActionMagicCircleMoving();

	void ActionEnterPlaza();

	void ActionTextTickEnd();

	void ActionPickItem();
	void ActionDropDevoloperK( DWORD dwCharIndex );
	void ActionBuyExercise();

	void SetEnableMagicCircleMoving( bool bEnableMagicCircleMoving ) { m_bEnableMagicCircleMoving = bEnableMagicCircleMoving; }
public:
	static ioTutorialManager& GetSingleton();

public:
	ioTutorialManager();
	virtual ~ioTutorialManager();
};

#define g_TutorialMgr ioTutorialManager::GetSingleton()

