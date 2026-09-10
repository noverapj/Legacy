#pragma once

#include "ioNormalSkill.h"

class ioINILoader;
class ioBaseChar;
class ioItem;

class ioComboJumpSkill2 : public ioNormalSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_ACTION,
		SS_END,
	};

	enum SkillSync
	{
		SSC_ACTION,
		SSC_END,
	};

	struct ComboInfo
	{
		AttackAttribute m_AttackAttribute;
		CEncrypt<float> m_fEnableMinHeight;
		CEncrypt<float> m_fEnableMaxHeight;
		CEncrypt<float> m_fEnableReserveHeight;
		CEncrypt<float> m_fJumpPower;
		ComboInfo()
		{
			m_AttackAttribute.Init();
			m_fEnableMinHeight = 0.0f;
			m_fEnableMaxHeight = 0.0f;
			m_fEnableReserveHeight = 0.0f;
			m_fJumpPower = 0.0f;
		}
	};
	typedef std::vector<ComboInfo> vComboInfoList;

	SkillState     m_SkillState;
	vComboInfoList m_ComboList;
	CEncrypt<int>  m_iMaxCombo;
	int            m_iCurCombo;

	ioHashString    m_szEndAnimation;
	CEncrypt<float> m_fEndAniRate;

	D3DXVECTOR3 m_vPrePosition;
	D3DXVECTOR3 m_vMoveDirection;
	float m_fLastHeight;

	bool m_bJump;

	DWORD m_dwAttackEndTime;

	bool m_bReserveJump;

	BuffInfoList	m_OwnerBuffList;

	ioHashString m_SkillMapEffect;
	DWORD		 m_dwMapUniqId;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadBuffList( ioINILoader &rkLoader );
	void AddOwnerBuff( ioBaseChar *pChar );

	virtual ioSkill* Clone();

public:
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsAttackEndState() const;

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	void ProcessActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetAction( ioBaseChar *pChar, bool bSend );
	void SetEndState( ioBaseChar *pChar, bool bSetEndMotion );

	bool CheckLanding( ioBaseChar *pChar );
	bool CheckEnableKeyInput( ioBaseChar *pChar, ioPlayStage *pStage );
	bool CheckReserveKeyInput( ioBaseChar *pChar, ioPlayStage *pStage );
	void CheckRotateByKeyInput( ioBaseChar *pChar );

protected:
	virtual void SetNormalSkill( ioBaseChar *pChar );

public:
	ioComboJumpSkill2();
	ioComboJumpSkill2( const ioComboJumpSkill2 &rhs );
	virtual ~ioComboJumpSkill2();
};

inline ioComboJumpSkill2* ToComboJumpSkill2( ioSkill *pSkill )
{
	if( !pSkill || pSkill->GetTypeID() != NST_COMBO_JUMP2 )
		return NULL;

	return dynamic_cast< ioComboJumpSkill2* >( pSkill );
}
