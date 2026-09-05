#pragma once

class SP2Packet;
class ioTalisman;
class ioBaseChar;
class ioINILoader;
class ioPlayStage;

class ioTalismanMaker : public Singleton< ioTalismanMaker >
{
protected:
	typedef std::vector< ioTalisman* > TalismanList;

	TalismanList m_vTalismanList;
	ioPlayStage *m_pPlayStage;

public:
	void LoadTalismanList();

	ioTalisman* CreateTalisman( ioBaseChar *pOwner, int iTalismanNum );

	void ClearTalismanList();

protected:
	ioTalisman* FindTalisman( int iTalismanNum );

public:
	static ioTalismanMaker& GetSingleton();

public:
	ioTalismanMaker( ioPlayStage *pStage );
	virtual ~ioTalismanMaker();
};

#define g_TalismanMaker ioTalismanMaker::GetSingleton()

