#pragma once



class ioINILoader;
class ioPlayStage;
class ioBaseChar;

class ioTalisman
{
protected:
	int m_iTalismanNum;
	ioHashString m_OwnerName;
	
	// 유지시간
	DWORD m_dwDuration;
	DWORD m_dwEndTime;

	// Effect
	ioHashString m_TalismanEffect;
	UniqueObjID m_dwTalismanEffect;

	// 버프
	ioHashStringVec m_vTargetBuffList;

public:
	ioTalisman* Clone();
	void LoadProperty( ioINILoader &rkLoader );
	void InitTalisman( ioBaseChar *pOwner );
	bool DestroyTalisman( ioBaseChar *pOwner );

	DWORD GetCurLiveTime();
	void SetCurLiveTime( DWORD dwCurGapTime );

	bool AddTalismanBuff( ioBaseChar *pOwner );
	bool RemoveTalismanBuff( ioBaseChar *pOwner );

	inline DWORD GetEndTime() const { return m_dwEndTime; }
	inline int GetTalismanNum() const { return m_iTalismanNum; }
	inline const ioHashString& GetOwnerName() const { return m_OwnerName; }

public:
	ioTalisman();
	ioTalisman( const ioTalisman &rhs );
	virtual ~ioTalisman();
};

