#pragma once

class DamageTable
{
private:
	struct DamageData
	{
		int m_iPrevDamage;
		int m_iCurDamage;
		DamageData()
		{
			m_iPrevDamage = m_iCurDamage = 0;
		}
	};
	typedef std::map< ioHashString, DamageData > DamageTableMap;
	DamageTableMap m_DamageTable;

public:
	void AddDamage( const ioHashString &rkAttacker, int iDamage );
	void ClearTable();

public:
	int GetDamageTableCnt() const;
	bool GetCurDamage( int iIdx, ioHashString &rkName, int &iDamage );
	bool GetAllDamage( int iIdx, ioHashString &rkName, int &iDamage );
	int  GetAllDamage( const ioHashString &rkAttacker );

public:
	void AttachAllDamage();
	void AttachAllDamage( const ioHashString &rkName );

public:
	DamageTable();
	~DamageTable();
};
