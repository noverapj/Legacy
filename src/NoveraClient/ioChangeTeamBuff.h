#pragma once

#include "ioBuff.h"

class ioINILoader;
class ioBaseChar;

class ioChangeTeamBuff : public ioBuff
{
private:
	enum EndType 
	{
		ET_DEFAULT,
		ET_NODASH,
		ET_NOCHANGE,
	};
protected:
	bool m_bEnableTargetAim;
	DWORD m_BuffEffectID;
	EndType m_iEndType;

	// 게이지 표시할때 쓰임
	CEncrypt<float> m_fExtraRate;

	// 미니맵에서도 팀 색 변경할것인지 확인
	bool m_bChangeTeamInMinimap;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );

public:
	inline bool IsEnableTargetAim() const { return m_bEnableTargetAim; }
	inline bool EnableChangeTeamInMinimap() const { return m_bChangeTeamInMinimap; }
	const float GetExtraRate() const { return m_fExtraRate; }

protected:
	virtual void SetAction();
	virtual void ProcessDelay( float fTimePerSec );

protected:
	void CheckingOwnerChar( ioBaseChar *pOwner );

public:
	ioChangeTeamBuff();
	ioChangeTeamBuff( const ioChangeTeamBuff &rhs );
	virtual ~ioChangeTeamBuff();
};

inline ioChangeTeamBuff* ToChangeTeamBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CHANGE_TEAM )
		return NULL;

	return dynamic_cast< ioChangeTeamBuff* >( pBuff );
}
