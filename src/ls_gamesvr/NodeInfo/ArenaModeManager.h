#pragma once

#include "../Util/Singleton.h"
#include "ioDecorationPrice.h"

// ArenaModeManager

class ArenaModeManager : public Singleton< ArenaModeManager >
{
public:
	struct stArenaSoldierInfo
	{
		int iSoldier_Number;
		int iSoldier_PowerUp_On;
		int iSoldier_PowerUp_Code;
		int iface;
		int ihair;
		int iskin_color;
		int ihair_color;
		int iunderwear;

		stArenaSoldierInfo()
		{
			iSoldier_Number = 0;
			iSoldier_PowerUp_On = 0;
			iSoldier_PowerUp_Code = 0;
			iface = 0;
			ihair = 0;
			iskin_color = 0;
			ihair_color = 0;
			iunderwear = 0;
		}
	};

	typedef std::vector<stArenaSoldierInfo> vSoldierInfo;

private:
	vSoldierInfo m_vecvSoldierInfo; 
	
public:
	ArenaModeManager();
	virtual ~ArenaModeManager();

	void Init();
	void Destroy();

public:
	BOOL LoadINIData( const ioHashString &rkFileName );
	vSoldierInfo GetSoldierInfo() {return m_vecvSoldierInfo;}

public:
	static ArenaModeManager& GetSingleton();
};

#define g_ArenaModeManager ArenaModeManager::GetSingleton()
