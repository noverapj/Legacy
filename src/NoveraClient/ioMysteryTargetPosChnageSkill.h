#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioMysteryTargetPosChnageSkill : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_DELAY,
		SS_ACTION,
		SS_END,
	};

	enum KeyState
	{
		KS_NONE,
		KS_PRESS,
		KS_RELEASE,
	};

	enum Sync
	{
		S_ACTION,
		S_END,
	};

	//추가 yong jun
	//skill 끝낼 시 motino이 종료하고 끝나느냐 바로 끝나느냐 체크
	enum SkillEndState
	{
		SES_NONE,
		SES_DIRECT,
	};

	struct ChangePos
	{
		ioHashString	szBanName;
		D3DXVECTOR3		vPos;
	};
	typedef std::vector<ChangePos> ChangePosList;	
	typedef std::vector<UniqueObjID> UniqueObjIDList;

protected:	
	SkillState			m_State;
	SkillEndState		m_SkillEndState;
	AttributeList		m_AttributeList;
	AttackAttribute		m_EndAttribute;
	int					m_iCurrAttribute;

	ioHashString		m_szEndAnimation;
	ioHashString		m_szDummyBuffName;
	ioHashString		m_szTeleportRedEffect;
	ioHashString		m_szTeleportBlueEffect;

	ioHashStringVec		m_TargetCharList;
	ChangePosList		m_ChangePosList;
	DWORD				m_dwStartChangePosTime;
	DWORD				m_dwSkillProtectTime;	
	DWORD				m_dwTeleportTime;
	DWORD				m_dwSkillStartTime;
	
	KeyState			m_KeyState;

	UniqueObjIDList		m_UniqueObjIDList;

	int					m_iTeleportHeightOffset;

protected:
	void Init();
	void CreateMapEffect( ioBaseChar* pChar, ioPlayStage* pStage );
	void DestoryMapEffect( ioBaseChar* pChar );
	
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	void LoadAttribute( ioINILoader &rkLoader );

	virtual ioSkill* Clone();

public:
	virtual bool HasSkillMotion() const;
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );

	void CheckAttackKey( ioBaseChar *pChar );
	void SetActionState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndState( ioBaseChar *pChar, ioPlayStage *pStage );
	void SetEndAnimation( ioBaseChar *pChar, ioPlayStage *pStage );

	void OnDelayState( ioBaseChar *pChar, ioPlayStage *pStage );
	void OnActionState( ioBaseChar *pChar, ioPlayStage *pStage );	
	void OnEndState( ioBaseChar *pChar, ioPlayStage *pStage );

	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool CheckUseItemMesh();

protected:
	virtual AttackAttribute& GetAttribute();
	virtual const AttackAttribute& GetAttributeConst() const;

	D3DXVECTOR3 GetRandPos( ioHashString szBanName );
	
	virtual void AttackSkillFire( ioBaseChar *pChar );
	virtual bool IsAttackEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;
	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );

public:
	ioMysteryTargetPosChnageSkill();
	ioMysteryTargetPosChnageSkill( const ioMysteryTargetPosChnageSkill &rhs );
	virtual ~ioMysteryTargetPosChnageSkill();
};

inline ioMysteryTargetPosChnageSkill* ToMysteryTargetPosChnageSkill( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_MYSTERY_TARGET_POS_CHANGE )
		return NULL;

	return dynamic_cast< ioMysteryTargetPosChnageSkill* >(pAttack);
}

