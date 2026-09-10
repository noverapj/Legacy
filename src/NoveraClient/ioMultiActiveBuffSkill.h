#pragma once

#include "ioBuffSkill.h"

class ioBaseChar;
class ioINILoader;
class ioBuff;

class ioMultiActiveBuffSkill : public ioBuffSkill
{
public:
	enum CreateState
	{
		RS_NONE,
		RS_ACTION,
		RS_LOOP,
		RS_END,
	};

	enum ThisSkillState
	{
		TSS_NONE,
		TSS_CHECK_TEL_LIST,
		TSS_HAS_BUFF,
		TSS_NO_BUFF,
		TSS_CON_ACTION,
		TSS_TEL_ACTION,
		TSS_LOOP,			
		TSS_END,
	};

	enum CharAttackType
	{
		CCAT_LAND,
		CCAT_AIR,
		CCAT_DOWN,
		CCAT_FAIL,
	};

protected:
	CreateState m_CreateState;
	ThisSkillState m_State;

	ioHashString m_LoopAnimation;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopTime;

	DWORD m_dwLoopProtectDuration;
	DWORD m_dwProtectTime;

	ioHashString m_EndAnimation;
	float m_fEndAniRate;
	DWORD m_dwEndAniStartTime;
	DWORD m_dwEndAniEndTime;

	CharAttackType m_AttackType;

protected:
	ioHashString m_StartHandMesh;

	ioHashString m_CreateEffect;
	DWORDVec     m_vObjectItemList;

	DWORD m_dwCurObjectItem;

	CEncrypt<bool>	m_bProtectOnUsingSkill;

	//////////////////////////////////////////////////////////////////////////
	int m_iCurLinearCombo;
	AttributeList m_vComboListLand;
	AttributeList m_vComboListAir;
	AttributeList m_vComboListDown;

	ioHashString	m_stMyCheckBuffName;
	ioHashString	m_stTargetCheckBuffName;

	AttackAttribute	m_ContactAttack;

	//////////////////////////////////////////////////////////////////////////
	TargetColType m_TargetColType;
	TargetWoundType m_TargetWoundType;

	DWORD m_dwDuration;

	bool m_bEnableAttacker;
	bool m_bEnableAttackCnt;
	bool m_bCheckCollisionLine;

	float m_fSkillRange;
	float m_fSkillAngle;
	float m_fOffSet;

	float m_fUpHeight;
	float m_fUnderHeight;
	float m_fWidth;

	ioHashString m_RangeEffect;
	D3DXVECTOR3 m_vEffectDir;

	float m_fHeightRange;
	bool  m_bSelectTeleportDirection;
	float m_fRangeEmptyTarget;
	float m_fBackRange;

	BuffInfoList m_OwnerBuffList;
	BuffInfoList m_TeamBuffList;
	BuffInfoList m_EnemyBuffList;

	int m_iEnemyRandomBuffCount;
	std::vector<BuffInfoList> m_EnemyRandomBuffList;

	ioHashStringVec m_vCollisionedList;
	ioHashStringVec m_vBlockedList;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

	CEncrypt<bool> m_bNoGravityCombo;
	CEncrypt<bool> m_bUsedComboList;
	CEncrypt<bool> m_bSetAttackFire;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void BuffSkillSet( ioBaseChar *pChar );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual void RemoveSkillBuff( ioBaseChar *pChar );
	virtual void RemoveSkillBuff( ioBaseChar *pChar, const ioHashString &szExceptName );

	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsEndState() const;

	virtual DWORD GetCreateItemCode( ioBaseChar *pChar );

	virtual bool IsEnableMultiActive( ioBaseChar *pChar );

	virtual void SetActionState( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	void ChangeContactFireState( ioBaseChar *pOwner );
	void ChangTelListState( ioBaseChar *pOwner );
	void ChangeEndState( ioBaseChar *pOwner );

	void SetTeleportList( ioBaseChar *pOwner, ioPlayStage *pStage );

	void SetSkillActionState( ioBaseChar *pChar );

	void SetNextLinearCombo( ioBaseChar *pChar, bool bSend );

	bool IsCollisioned( ioBaseChar *pChar );

	CharAttackType GetAttackType( ioBaseChar *pChar );

public:
	ioMultiActiveBuffSkill();
	ioMultiActiveBuffSkill( const ioMultiActiveBuffSkill &rhs );
	virtual ~ioMultiActiveBuffSkill();
};

inline ioMultiActiveBuffSkill* ToMultiActiveBuffSkill( ioSkill *pSkill )
{
	ioBuffSkill *pBuff = ToBuffSkill( pSkill );
	if( !pBuff || pBuff->GetSubSkillType() != BST_MULTI_ACTIVE )
		return NULL;

	return dynamic_cast< ioMultiActiveBuffSkill* >( pSkill );
}

