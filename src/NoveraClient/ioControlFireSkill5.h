#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioAttackSkill.h"
#include "ioUserKeyInput.h"

class ioControlFireSkill5 :	public ioAttackSkill
{
protected:
	enum SkillState
	{
		SS_NONE,
		SS_FLY,
		SS_LOOP,
		SS_FIRE,
		SS_END,
	};

	enum UpDownKeyType
	{
		UDKT_NONE,
		UDKT_AUTO_UP_KEY,
	};

	enum SkillSyncType
	{
		SST_CREAT_DUMMY,
		SST_DIRCHANGE,
		SST_FIRE,
	};
protected:
	SkillState					m_SkillState;
	UpDownKeyType				m_UpDownKeyType;

	//Fly
	float						m_fStartAngle;
	float						m_fFlyCurRange;
	float						m_fFlyMaxRange;
	float						m_fCurPositionOffset;
	DWORD						m_dwFlyDuration;
	ioHashString				m_szFlyMotion;
	float						m_fFlyMotionRate;

	//Loop
	DWORD						m_dwLoopStartTime;
	DWORD						m_dwLoopDuration;
	ioHashString				m_SkillLoopMotion;
	ioHashString				m_SkillUpLoopMotion;
	ioHashString				m_SkillDownLoopMotion;

	//Dummy
	ioHashString				m_DummyCharName;
	int							m_iDummyIndex;
	bool						m_bCheckDummyDie;
	bool						m_bDieDummy;

	//Fire
	int							m_iMaxAttackCount;
	int							m_iCurAttackCount;

	/////////////////Key & Rotate//////////////////
	//Rotate Data
	D3DXVECTOR3					m_vSkillStartPosition;
	float						m_fTargetAttachAngleY;
	float						m_fTargetAttachAngleX;
	float						m_fCurMotionRate;
	//Dir
	ioUserKeyInput::DirKeyInput m_CurDirKey;
	DWORD						m_dwDirKeyRotateSpeed;
	bool						m_bTargetRot;
	bool						m_bLeftRot;
	bool						m_bMotionSetted;
	//Up & Down
	DWORD						m_dwUpDownRotateSpeed;
	float						m_fMaxLimitAngle;
	float						m_fMinLimitAngle;
	////////////////////////////////////////////////

	// Sync
	bool						m_bDir;
	bool						m_bUp;
	bool						m_bDown;

	//Protect Time
	DWORD						m_dwProtectTime;
	DWORD						m_dwFlyProtectTime;
	

protected:
	void	ClearData();

	void	SetFlyState( ioBaseChar *pChar );
	void	SetLoopState( ioBaseChar *pChar, ioPlayStage *pStage );
	void	SetFireState( ioBaseChar *pChar );
	void	SetEndState( ioBaseChar *pChar );

	bool	ProcessFlyStart( ioBaseChar *pChar );

	void	CreateDummy( ioBaseChar *pChar, ioPlayStage *pStage );

	void	CheckKeyInput( ioBaseChar *pChar );
	bool	CheckUpDownKey( ioBaseChar *pChar );
	bool	CheckDirKey( ioBaseChar *pChar );
	void	CheckRotate( ioBaseChar *pChar, ioPlayStage *pStage );
	bool	CheckHDir( ioBaseChar *pChar );
	bool	CheckVDir( ioBaseChar *pChar );
	void	CheckDummy( ioBaseChar *pChar, ioPlayStage *pStage );
public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsNoDropState() const;
	virtual bool IsAttackEndState() const;
	virtual bool IsEndState() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );
public:
	ioControlFireSkill5(void);
	ioControlFireSkill5( const ioControlFireSkill5 &rhs );
	~ioControlFireSkill5(void);
};

inline ioControlFireSkill5* ToControlFireSkill5( ioSkill *pSkill )
{
	ioAttackSkill *pAttack = ToAttackSkill( pSkill );
	if( !pAttack || pAttack->GetSubSkillType() != AST_CONTROL_FIRE5 )
		return NULL;

	return dynamic_cast< ioControlFireSkill5* >( pAttack );
}