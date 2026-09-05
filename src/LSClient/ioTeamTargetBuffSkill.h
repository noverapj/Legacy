#pragma once

class ioINILoader;
class ioBaseChar;

#include "ioBuffSkill.h"
#include "ioUserKeyInput.h"


class ioTeamTargetBuffSkill : public ioBuffSkill
{
public:
	enum SkillState
	{
		SS_NONE,
		SS_LOOP,
		SS_FIRE,
	};

protected:
	SkillState m_SkillState;

	ioHashString m_SkillLoopMotion;
	CEncrypt<float> m_fSkillLoopAniRate;

	DWORD m_dwLoopStartTime;
	DWORD m_dwLoopDuration;

	// Find Target
	CEncrypt<float> m_fTargetAngle;
	CEncrypt<float>	m_fTargetRange;
	TargetWoundType m_TargetWoundType;

	// Aim Mark
	ioHashString	m_szAimMark;
	UniqueObjID		m_dwAimMarkID;

	// TargetList
	ioHashString	m_szTargetName;
	bool			m_bJumpKey;
	bool			m_bDefenseKey;
	
	DWORD m_dwFireStartTime;

protected:
	void ClearData();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:
	virtual bool OnSkillStart( ioBaseChar *pChar );
	virtual void OnProcessState( ioBaseChar *pChar, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pChar );

	virtual bool IsEnableReserve() const;
	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;

	virtual void ApplyExtraSkillInfo( ioBaseChar *pChar, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool CheckUseItemMesh();

	virtual bool IsProtected( int iDefenseBreakType ) const;

public:
	virtual bool CheckExceptionCamera();
	virtual D3DXVECTOR3 GetExceptionCameraPos( ioBaseChar *pChar );

protected:
	void SetLoopState( ioBaseChar *pOwner, ioPlayStage *pStage );
	void SetFireState( ioBaseChar *pOwner );

	void CheckKeyInput( ioBaseChar *pOwner );
	void CheckTarget( ioBaseChar *pChar, ioPlayStage *pStage );	

	void UpdateAimMark( ioBaseChar *pChr, ioPlayStage *pStage );
	void DestroyAimMark( ioBaseChar *pChar );

	void AddTargetBuff( ioBaseChar *pChar );

public:
	ioTeamTargetBuffSkill();
	ioTeamTargetBuffSkill( const ioTeamTargetBuffSkill &rhs );
	virtual ~ioTeamTargetBuffSkill();
};

inline ioTeamTargetBuffSkill* ToTeamTargetBuffSkill( ioSkill *pSkill )
{
	ioBuffSkill *pBuff = ToBuffSkill( pSkill );
	if( !pBuff || pBuff->GetSubSkillType() != BST_TEAM_TARGET )
		return NULL;

	return dynamic_cast< ioTeamTargetBuffSkill* >( pSkill );
}