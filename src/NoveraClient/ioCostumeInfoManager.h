#pragma once

struct RopeSpringParam;
class ioPlayStage;

class ioCostumeInfoManager : public Singleton< ioCostumeInfoManager >
{
protected:
	CostumeInfoList m_vInfoList;
	IntVec m_vEnableCostumeMode;
	IntVec m_vDisableCostumeMode;
	int m_nCurModeType;

public:
	static ioCostumeInfoManager& GetSingleton();

public:
	void ClearData();
	void LoadProperty();

protected:
	void LoadBase( ioINILoader &rkLoader );
	void LoadArmor( ioINILoader &rkLoader, CostumeInfo &sCostumeInfo );
	void LoadHelmet( ioINILoader &rkLoader, CostumeInfo &sCostumeInfo );
	void LoadCloak( ioINILoader &rkLoader, CostumeInfo &sCostumeInfo );
	
public:
	bool GetCostumeInfo( int nCostumeCode, CostumeInfo &sCostumeInfo );
	ioHashString GetCostumeName( int nCostumeCode );
	ioHashString GetCostumeIcon( int nCostumeCode );
	int GetCostumeManual( int nCostumeCode );
	int GetGradeType( int nCostumeCode );
	
public:
	bool IsEnableCostumeMode();
	bool IsDisableCostumeMode();
	void SetCurPlayMode( int nCurModeType );

public:
	void OnCostumeBuyResult( SP2Packet &rkPacket );
	void OnCostumeSellResult( SP2Packet &rkPacket );
	void OnCostumePresentResult( SP2Packet &rkPacket );
	void OnCostumeAdd( SP2Packet &rkPacket, int &nCode );
	void OnCostumeChange( SP2Packet &rkPacket, ioPlayStage* pPlayStage );

public:
	ioCostumeInfoManager();
	virtual ~ioCostumeInfoManager();
};

#define g_CostumeInfoMgr ioCostumeInfoManager::GetSingleton()