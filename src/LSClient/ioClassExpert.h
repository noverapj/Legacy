#pragma once

class SP2Packet;
class ioClassExpert
{
public:
	enum
	{
		MAX_SLOT   = 10,	
	};

	struct CLASSSLOT
	{
		int m_iType;				// 용병 타입 
		int m_iLevel;				// 용병 레벨
		int m_iExpert;				// 용병 경험치
		byte m_ePowerUpCharGrade;	// 용병 강화 등급
	};

	struct ClassInfo
	{
		CLASSSLOT	m_Data[MAX_SLOT];
		ClassInfo()
		{
			memset( m_Data, 0, sizeof( m_Data ) );
		}
	};
	typedef std::vector< ClassInfo > vClassInfo;

protected:
	vClassInfo m_vClassInfo;

protected:
	void CreateClass( int iClassType );  

public:
	void Initialize();
	
public:
	void ApplyClassInfo( SP2Packet &rkPacket );
	void AddClassInfo( CLASSSLOT &kSlot );

public:
	bool AddClassExp( int iClassType, int iExp, int &rRemainExp );
	bool AddPowerUpGrade( int iClassType, byte ePowerUpCharGrade );
	bool SetClassExp( int iClassType, int iExp );
	bool SetClassLevel( int iClassType, int iLevel );

public:
	int  GetClassLevel( int iClassType );	
	int  GetClassExpert( int iClassType );
	int  GetTopLevelClassType();

public:
	const ioClassExpert::CLASSSLOT* GetClassExpertSlot( int iClassType );

public:
	ioClassExpert();
	virtual ~ioClassExpert();
};

