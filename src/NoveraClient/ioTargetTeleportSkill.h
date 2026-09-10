#pragma once

#include "ioNormalSkill.h"

class ioBaseChar;
class ioINILoader;
class SP2Packet;

class ioTargetTeleportSkill : public ioNormalSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_WAIT,
		SS_TELEPORT,
		SS_END,
	};

protected:
	SkillState m_SkillState;

	CEncrypt<DWORD> m_dwWaitStartTime;
	CEncrypt<DWORD> m_dwWaitDuration;

	DWORD m_dwAttackEndTime;

	ioHashString m_SkillEndMotion;
	float m_fEndMotionRate;

	D3DXVECTOR3 m_vBasePos;
	D3DXVECTOR3 m_vTargetPos;
	
	ioHashString m_CurTargetName;
	ioHashStringVec m_TargetCharList;
	CEncrypt<int> m_iCurTargetIndex;
	CEncrypt<float> m_fTargetOffSetHeight;

	ioHashString m_TeleportCameraBuff;

	ioHashString m_TargetMapEffect;
	UniqueObjID m_dwTargetMapEffect;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnSkillEnd( ioBaseChar *pChar );	
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

public:
	virtual bool IsAttackEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

protected:
	virtual bool StartPreDelayAnimation( ioBaseChar *pChar );
	virtual void SetNormalSkill( ioBaseChar *pChar );

protected:
	void SetWaitState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetTeleportState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar );

	void SetTargetList( ioBaseChar *pChar, ioPlayStage *pStage );
	bool CheckValidateTarget( ioBaseChar *pChar, ioPlayStage *pStage );
	void SelectNextTarget( ioBaseChar *pChar, ioPlayStage *pStage );

	void SetTargetMapEffect( ioBaseChar *pChar );
	void UpdateTargetMapEffect( ioPlayStage *pStage );
	void DestroyTargetMapEffect( ioBaseChar *pChar );

	void CheckTargetPos( ioBaseChar *pChar, ioPlayStage *pStage );

public:
	inline int GetSkillState() const { return m_SkillState; }

	bool IsViewPositionState();
	const D3DXVECTOR3& GetViewPosition();

public:
	ioTargetTeleportSkill();
	ioTargetTeleportSkill( const ioTargetTeleportSkill &rhs );
	virtual ~ioTargetTeleportSkill();
};

inline ioTargetTeleportSkill* ToTargetTeleportSkill( ioSkill *pSkill )
{
	ioNormalSkill *pNormal = ToNormalSkill( pSkill );
	if( !pNormal || pNormal->GetSubSkillType() != NST_TARGET_TELEPORT )
		return NULL;

	return dynamic_cast< ioTargetTeleportSkill* >( pSkill );
}

