#pragma once

#include "ioRangeSkill.h"
#include "WeaponAttribute.h"

class ioBaseChar;
class ioINILoader;
class ioBuff;

class ioRangeBuffAttackSkill : public ioRangeSkill
{
public:
	enum SyncState
	{
		SS_STATE	= 1,
		SS_KEY_ROT	= 2,
	};

protected:
	// 1차 루프
	bool m_bLoopMoveState;
	ioHashString m_szLoopDelayAni;
	ioHashString m_szLoopMoveAni;

	// 2차 액션
	ioHashString m_szPullActionAni;
	float m_fPullActionAniRate;

	// 2차 루프
	ioHashString m_szPullLoopAni;
	CEncrypt<DWORD> m_dwPullLoopDuration;

	// 2차 End
	ioHashString m_szPullEndAni;
	float m_fPullEndAniRate;

	// areaweapon
	ioHashString m_AreaWeaponName;
	DWORD m_dwAreaWeaponIndex;

	ioUserKeyInput::DirKeyInput m_CurDirKey;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

protected:
	// 1차 액션, 루프, End
	virtual void SetActionState( ioBaseChar *pChar );
	virtual void SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void SetEndState( ioBaseChar *pChar );

	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	
	virtual bool OnSkillStart( ioBaseChar *pChar );	
	virtual void OnSkillEnd( ioBaseChar *pChar );

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	// 2차 액션, 루프, End
	void SetActionState2( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetLoopState2( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState2( ioBaseChar *pChar, bool bSendNet );

	// 2차 액션 발생시 버프 적용
	void SetBuffList( ioBaseChar *pOwner, ioPlayStage *pStage );

	void RemoveOwnerBuffList( ioBaseChar *pOwner );

	void CheckLoopMoveAniState( ioBaseChar *pOwner );

	// areaweapon
	void CreateAreaWeapon( ioBaseChar *pChar );

	void CheckKeyRot( ioBaseChar *pChar );

public:
	ioRangeBuffAttackSkill();
	ioRangeBuffAttackSkill( const ioRangeBuffAttackSkill &rhs );
	virtual ~ioRangeBuffAttackSkill();
};

inline ioRangeBuffAttackSkill* ToRangeBuffAttackSkill( ioSkill *pSkill )
{
	ioRangeSkill *pRange = ToRangeSkill( pSkill );
	if( !pRange || pRange->GetSubSkillType() != RST_BUFF_ATTACK )
		return NULL;

	return dynamic_cast< ioRangeBuffAttackSkill* >(pRange);
}

