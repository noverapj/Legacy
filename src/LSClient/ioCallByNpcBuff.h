#pragma once

class ioCallByNpcBuff :	public ioBuff
{
protected:
	struct BuffByNpc
	{
		bool			bLive;
		DWORD			dwNpcCode;
		ioHashString	szBuff;
	};
	
	std::vector<BuffByNpc> m_vecRedBuff;
	std::vector<BuffByNpc> m_vecBlueBuff;


public:
	void SetBlueBuff(const DWORD dwNpcCode, bool bAlive);
	void SetRedBuff(const DWORD dwNpcCode, bool bAlive);
	void RefreshBuff();

public:
	virtual void LoadProperty( ioINILoader &rkLoader );
	virtual ioBuff* Clone();

public:
	virtual void StartBuff( ioBaseChar *pOwner );

	virtual bool ApplyStartBuff( ioBaseChar *pOwner, DWORD dwGlobalTime, DWORD dwGapTime );


public:
	ioCallByNpcBuff(void);
	ioCallByNpcBuff(const ioCallByNpcBuff &rhs);
	virtual ~ioCallByNpcBuff(void);

};

inline ioCallByNpcBuff* ToCallByNpcBuff( ioBuff *pBuff )
{
	if( !pBuff || pBuff->GetTypeID() != BT_CALL_BY_NPC )
		return NULL;

	return dynamic_cast< ioCallByNpcBuff* >( pBuff );
}
