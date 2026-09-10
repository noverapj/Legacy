#pragma once

class ioPlayStage;
class ioDummyChar;

struct GroupDummyInfo
{
	ioHashString		szDummyName;
	float				fNeedGauge;

	GroupDummyInfo()
	{
		fNeedGauge	= 0.0f;
	}
};
typedef std::vector<GroupDummyInfo> GroupDummyInfoList;
typedef std::vector<ioDummyChar*> DummyCharList;

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class ioDummyGroup
{
friend class ioDummyCharManager;
protected:
	ioPlayStage*	m_pStage;

	ioHashString	m_GroupOwner;
	ioHashString	m_IdentityName;	//스킬 및 아이템 명(여러개의 그룹을 구분하기 위한 인자)

	IntVec			m_DummyList;

public:
	const ioHashString& GetOnwerName()const { return m_GroupOwner; }
	const ioHashString& GetIdentityName() const { return m_IdentityName; }

public:
	void CheckDieDummy();
	ioDummyChar* FirstDummyDie( ioDummyChar* pExceptionDummy );

public:
	bool IsLiveDummy( int iDummyIndex );
	bool HasDummy( int iDummyIndex );

public:
	void AddDummy( int iDummyIndex );
	ioDummyChar* GetDummy( int iDummyIndex );
	void GetDummyByArea( IN float fMaxRange, OUT DummyCharList& List );
	void GetDummyAll( OUT DummyCharList& List );
	int GetDummyCount( ioHashString szName );

public:
	ioDummyGroup();
	ioDummyGroup( ioPlayStage* pStage, const ioHashString& szGroupOwner, const ioHashString& szIdentityName );
	~ioDummyGroup();
};

typedef std::vector<ioDummyGroup> ioDummyGroupList;