#pragma once

#include "ioCountSkill.h"
#include "ioAttackAttribute.h"

class ioINILoader;
class ioBaseChar;
class ioItem;
class SP2Packet;


class ioBuffCountSkill : public ioCountSkill
{
protected:
	enum SkillState
	{
		SS_PRE,
		SS_ACTION,
		SS_END,
	};
	
	struct BuffInfo
	{
		ioHashStringVec	BuffNameList;
		int				nNeedCount;		

		BuffInfo()
		{
			nNeedCount = 0;
		}
	};
	typedef std::vector<BuffInfo> BuffInfoList;

	class BuffInfoSort : public std::binary_function< const BuffInfo& , const BuffInfo&, bool >
	{
	public:
		bool operator() ( const BuffInfo &lhs, const BuffInfo &rhs ) const
		{
			if( lhs.nNeedCount < rhs.nNeedCount )
				return true;

			return false;
		}
	};

protected:
	SkillState		m_SkillState;		
	BuffInfoList	m_BuffInfoList;
	ioHashStringVec m_CurrBuffList;
	DWORD			m_dwSkillProtectTime;
	bool			m_bNoResetCount;
	int				m_nCurNeedCount;
	int				m_iPrevCount;

protected:
	void Init();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioSkill* Clone();

public:	
	virtual int ReduceNeedCount( int iNeedCount );

public:
	virtual bool OnSkillStart( ioBaseChar *pOwner );
	virtual void OnProcessState( ioBaseChar *pOwner, ioPlayStage *pStage );
	virtual void OnSkillEnd( ioBaseChar *pOwner );

protected:
	void SetSkillActionState( ioBaseChar* pOwner, ioPlayStage* pStage );
	void SetOwnerBuff( ioBaseChar* pOwner, ioPlayStage* pStage );

protected:
	void CheckBuff( ioBaseChar* pOwner );
	void CheckPreState( ioBaseChar *pOwner, ioPlayStage *pStage );

public:
	virtual void ApplyExtraSkillInfo( ioBaseChar *pOwner, ioPlayStage *pStage, SP2Packet &rkPacket );

	virtual bool IsSkillMotionEnd( DWORD dwActionStop, ioBaseChar* pOwner ) const;
	virtual bool IsProtected( int iDefenseBreakType ) const;

public:
	ioBuffCountSkill();
	ioBuffCountSkill( const ioBuffCountSkill &rhs );
	virtual ~ioBuffCountSkill();
};

inline ioBuffCountSkill* ToBuffCountSkill( ioSkill *pSkill )
{
	ioCountSkill *pCount = ToCountSkill( pSkill );
	if( !pCount || pCount->GetSubSkillType() != CST_BUFF )
		return NULL;

	return dynamic_cast< ioBuffCountSkill* >( pCount );
}