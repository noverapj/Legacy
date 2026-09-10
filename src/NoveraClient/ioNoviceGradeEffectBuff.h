#pragma once

#include "ioBuff.h"

class ioBaseChar;
class ioINILoader;

class ioNoviceGradeEffectBuff : public ioBuff
{
protected:
	ioHashString m_szNoviceGradeEmotion;
	ioHashString m_szNoviceGradeEffect;
	DWORD m_dwNoviceGradeEmotionTime;
	bool m_bCreateEffect;

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );
	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );
	virtual void ProcessBuff( float fTimePerSec );
	virtual void EndBuff();

public:
	ioNoviceGradeEffectBuff();
	ioNoviceGradeEffectBuff( const ioNoviceGradeEffectBuff &rhs );
	virtual ~ioNoviceGradeEffectBuff();
};

inline ioNoviceGradeEffectBuff* ToNoviceGradeEffectBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_NOVICE_GRADE_EFFECT )
		return NULL;

	return dynamic_cast< ioNoviceGradeEffectBuff* >( pBuff );
}