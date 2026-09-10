#pragma once

#include "ioAttackSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;

class ioControlFireSkill4 : public ioAttackSkill
{
public:
	enum SkillState
	{
		SS_PRE,		
		SS_ATTACK,
		SS_LOOP,
		SS_END,
	};

	enum UpDownState
	{
		UDS_UP,
		UDS_DOWN,
	};

	enum SkillStateSync
	{
		SSS_DIR,
	};

	typedef ioUserKeyInput::DirKeyInput DirKey;

protected:
	SkillState		m_SkillState;
	DWORD			m_dwSkillProtectTime;

protected:
	ioHashString	m_LoopUpAnimation;
	ioHashString	m_LoopCenterAnimation;
	ioHashString	m_LoopDownAnimation;

	float			m_fVertUpLimitAngle;
	float			m_fVertDownLimitAngle;
	float			m_fCurrMotionRate;

	DWORD			m_dwHorzRotateWeight;
	DWORD			m_dwVertRotateWeight;
	DWORD			m_dwLoopDuration;
	DWORD			m_dwLoopStartTime;	

	D3DXVECTOR3		m_vAttackDir;
	DirKey			m_CurDirKey;

protected:
	IntVec			m_WeaponIndexList;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	void Init();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

public:
	virtual bool CheckUseItemMesh();
	virtual void ApplyAttributeToWeapon( ioWeapon *pWeapon, ioPlayStage *pStage );
	virtual void ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket );

protected:
	void CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckActionState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void CheckLoopState( ioBaseChar *pOwner, ioPlayStage *pStage );

protected:
	bool CheckDirectionKey( ioBaseChar* pOwner );

	void ProcessWeapon( ioBaseChar *pOwner );
	void RemoveWeapon( ioBaseChar *pOwner );

protected:
	void SetSkillActionState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetLoopState( ioBaseChar* pOwner, ioPlayStage *pStage );

protected:
	bool IsLeftRotate( ioBaseChar* pOwner, DirKey eNewDirKey );
	bool IsLimitAngle( float fLimitAngle, float fCurrAngle );

protected:	
	void ProcessRotateHorz( ioBaseChar *pOwner, float fRotateTime, DirKey eDir, bool bLeft );
	bool ProcessRotateVert( ioBaseChar *pOwner, float fRotateTime, UpDownState eState );
	void ProcessRotateAnimation( ioBaseChar *pOwner );

public:
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;	
	virtual bool IsProtected( int iDefenseBreakType ) const;

public:
	ioControlFireSkill4();
	ioControlFireSkill4( const ioControlFireSkill4 &rhs );
	virtual ~ioControlFireSkill4();
};

inline ioControlFireSkill4* ToControlFireSkill4( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CONTROL_FIRE4 )
		return NULL;

	return dynamic_cast< ioControlFireSkill4* >( pAttack );
}

