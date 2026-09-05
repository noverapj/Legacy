#pragma once

class BonusCashManager : public Singleton< BonusCashManager >
{
public:
	BonusCashManager();
	virtual ~BonusCashManager();

	void Init();
	void Destroy();

public:
	static BonusCashManager& GetSingleton();

public:
	void LoadIni();

protected:

};

#define g_BonusCashMgr BonusCashManager::GetSingleton()