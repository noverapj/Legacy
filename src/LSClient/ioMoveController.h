#pragma once


class ioMoveController
{
protected:
	struct MoveData
	{
		WORD  m_wKeyData;
		DWORD m_dwDelayTime;
		MoveData()
		{
			m_wKeyData = 0;
			m_dwDelayTime = 0;
		}
	};
	typedef std::vector< MoveData > vMoveData;
	struct MoveList
	{
		vMoveData m_Move;
		float     m_fKeepRange;
		float     m_fMoveRange;
		float     m_fLimitHPRateMin;
		float     m_fLimitHPRateMax;
		MoveList()
		{
			m_fMoveRange = m_fLimitHPRateMin = m_fLimitHPRateMax = 0.0f;
		}
	};
	typedef std::map< DWORD, MoveList > MoveMap;
	MoveMap  m_MoveMap;

protected:
	MoveList m_CurMove;
	DWORD    m_dwCurrentTime;

protected:
	void AttachMoveData( MoveList &rkList );

public:
	void LoadController( ioINILoader &rkLoader );
	void ClearCurrentData();

public:
	bool Attach( ioNpcChar *pOwner, ioBaseChar *pTarget, float fDistSq );
	
public:
	bool IsMoveRangeCheck( float fRange );
	bool OnMoveAction( ioNpcChar *pOwner, ioBaseChar *pTarget, D3DXVECTOR3 &vMovePos, float fAngle, float fRange );
	bool OnMoveAIAction( ioNpcChar *pOwner, ioBaseChar *pTarget, D3DXVECTOR3 &vMovePos, float fAngle, float fRange );

public:
	bool IsMove();

public:
	ioMoveController();
	virtual ~ioMoveController();
};
