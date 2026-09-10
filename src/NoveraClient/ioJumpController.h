#pragma once


class ioJumpController
{
protected:
	struct JumpData
	{
		WORD  m_wKeyData;
		DWORD m_dwDelayTime;
		JumpData()
		{
			m_wKeyData = 0;
			m_dwDelayTime = 0;
		}
	};
	typedef std::vector< JumpData > vJumpData;
	struct JumpList
	{
		vJumpData m_Jump;
		float     m_fJumpRange;
		JumpList()
		{
			m_fJumpRange = 0.0f;
		}
	};
	typedef std::map< DWORD, JumpList > JumpMap;
	JumpMap  m_JumpMap;

protected:
	JumpList m_CurJump;
	DWORD    m_dwCurrentTime;

protected:
	void AttachJumpData( JumpList &rkList );

public:
	void LoadController( ioINILoader &rkLoader );
	void ClearCurrentData();

public:
	bool Attach( ioNpcChar *pOwner, ioBaseChar *pTarget, float fDistSq );
	
public:
	bool OnJumpAction( ioNpcChar *pOwner, ioBaseChar *pTarget );

public:
	bool IsJump();

public:
	ioJumpController();
	virtual ~ioJumpController();
};
