#pragma once

#include "ioNormalSkill.h"


class ioWeaponControlSkill : public ioNormalSkill
{
public:
	enum ControlState
	{
		RS_NONE,
		RS_ACTION,
		RS_LOOP,
		RS_END,
	};
	enum SyncState
	{
		SS_SET,
		SS_MOVE,
		SS_WEAPON,
		SS_END,
	};
	
protected:
	ControlState m_ControlState;

	DWORD m_dwLoopProtectDuration;
	DWORD m_dwProtectTime;

	AttackAttribute m_AttackAttribute;

	D3DXVECTOR3 m_vCreatePos;
	DWORD m_dwFireStartTime;

	// Animation
	ioHashString m_LoopAnimation;
	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopTime;

	ioHashString m_EndAnimation;
	float m_fEndAniRate;
	DWORD m_dwEndAniEndTime;

	// HandMesh
	ioHashString m_StartHandMesh;
	ioHashString m_LoopHandMesh;
	ioHashString m_EndHandMesh;

	ioHashString m_szTargetName;
	ioHashString m_szControlBuffName;
	ioUserKeyInput::DirKeyInput m_PrevKey;
	DWORD m_dwTargetOffset;
	DWORD m_dwCreateOffset;
	bool m_bExplosion;
	int m_iStopSkillKey;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsCanUseSkill( ioBaseChar *pChar, bool bHelpMsg );
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual void StartPreDelayEffect( ioBaseChar *pChar );
	virtual void EndPreDelayEffect( ioBaseChar *pChar );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

	virtual bool UseActiveCount( ioBaseChar *pOwner );

	virtual int GetMaxActiveCount( ioBaseChar *pOwner );
	virtual int GetCurActiveCount( ioBaseChar *pOwner );

protected:
	void SetActionState( ioBaseChar *pChar );
	void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	bool CheckControlWeapon( ioBaseChar *pChar );

public:
	bool IsWeaponControl( ioBaseChar *pChar );
	const D3DXVECTOR3& GetWeaponPos( ioBaseChar *pChar );

	void SetExplosion();
	bool SetControlBuffInfo( ioBaseChar *pOwner, ioHashString szTargetName, ioHashString szControlBuffName );
	void ClearControlBuffInfo( ioBaseChar *pOwner );
	void SetControlMoveInfo( ioBaseChar *pOwner, ioUserKeyInput::DirKeyInput eKey, bool bJump );
	void CreateControlWeaponAgain( ioBaseChar *pOwner );

public:
	ioWeaponControlSkill();
	ioWeaponControlSkill( const ioWeaponControlSkill &rhs );
	virtual ~ioWeaponControlSkill();
};

inline ioWeaponControlSkill* ToWeaponControlSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_WEAPON_CONTROL )
		return NULL;

	return dynamic_cast< ioWeaponControlSkill* >( pSkill );
}

