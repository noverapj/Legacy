#pragma once


class ioBlackListManager  : public Singleton< ioBlackListManager >
{
public:
	enum
	{
		MAX_BLACK_LIST = 100,
	};
protected:
	ioHashStringVec m_vBlackList;
	bool m_bSave;

public:
	void LoadBlackList();
	void SaveBlackList();
	bool InsertBlackList(const ioHashString &szID);
	void RemoveBlackList(const ioHashString &szID);

public:
	bool IsBlackList(const ioHashString &szID);
	int GetNodeSize(){ return m_vBlackList.size(); }
	ioHashString *GetNode( int iArray );

public:
	static ioBlackListManager &GetSingleton();

public:
	ioBlackListManager(void);
	virtual ~ioBlackListManager(void);
};
#define g_BlackListManager ioBlackListManager::GetSingleton()
