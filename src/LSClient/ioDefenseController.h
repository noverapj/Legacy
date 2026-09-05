#pragma once


class ioDefenseController
{	
protected:
	struct DefenseData
	{
		WORD  m_wKeyData;
		DWORD m_dwDelayTime;
		DefenseData()
		{
			m_wKeyData = 0;
			m_dwDelayTime = 0;
		}
	};
	typedef std::vector< DefenseData > vDefenseData;
	struct DefenseList
	{
		vDefenseData m_Defense;
		DWORDVec     m_WeaponSkipList;
		float        m_fDefenseRate;
		float		 m_fDefenseRange;
		float        m_fDefenseDirection;
		DefenseList()
		{
			m_fDefenseRate = m_fDefenseRange = m_fDefenseDirection = 0.0f;
		}
	};
	typedef std::map< DWORD, DefenseList > DefenseMap;
	DefenseMap  m_DefenseMap;

protected:
	DefenseList m_CurDefense;
	DWORD    m_dwCurrentTime;

protected:
	bool IsSkipType( DWORDVec &rkType, DWORD dwType );
	void AttachDefenseData( DefenseList &rkList, DWORD dwPredicTime );

protected:
	float GetAngle( D3DXVECTOR3 vPos1, D3DXVECTOR3 vPos2 );

public:
	void LoadController( ioINILoader &rkLoader );
	void ClearCurrentData();

public:
	bool Attach( ioNpcChar *pOwner, ioBaseChar *pTarget, float fTargetRange );
	
public:
	bool OnDefenseAction( ioNpcChar *pOwner, ioBaseChar *pTarget );

public:
	bool IsDefense();

public:
	ioDefenseController();
	virtual ~ioDefenseController();
};
