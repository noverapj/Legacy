#pragma once

struct SpiritInvenInfo
{
	int m_iCode;
	int m_iCount;

	bool m_bNewAdd;

	SpiritInvenInfo()
	{
		Init();
	}

	void Init()
	{
		m_iCode = 0;
		m_iCount = 0;

		m_bNewAdd = false;
	}
};
typedef std::map< int, SpiritInvenInfo > SpiritInvenInfoMap;

class ioUserSpiritInven
{
protected:
	SpiritInvenInfoMap m_vSpiritList;

public:
	void Initialize();
	void ApplySpiritData( SP2Packet &rkPacket );
	void ApplySpiritSync( SP2Packet &rkPacket );

	void FindSpiritInfo( int iCode, SpiritInvenInfo &rkItem );
	int GetSpiritCount( int iCode );

	int SetSpiritItem( int iCode, int iCount );

public:
	ioUserSpiritInven(void);
	virtual ~ioUserSpiritInven(void);
};


