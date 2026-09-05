#pragma once

#include "../Define.h"
class User;

struct CompensationInfo
{
	CompensationType m_eCompensationType;

	__int64 m_iStartDate;
	__int64	m_iEndDate;

	int		m_iType;
	int		m_iCode;
	int		m_iValue;

	CompensationInfo()
	{
		m_iStartDate	= 0;
		m_iEndDate		= 0;
		m_iType			= 0;
		m_iCode			= 0;
		m_iValue		= 0;
	}
};

class CompensationMgr : public Singleton< CompensationMgr >
{
public:
	CompensationMgr();
	virtual ~CompensationMgr();

	void Init();
	void Destroy();

public:
	static CompensationMgr& GetSingleton();

public:
	void RegistCompensation(CompensationType eType, int iItemType, int iCode, int iValue, __int64 iEndDate);

	void SendCompensation(User* pUser);


protected:
	typedef std::vector<CompensationInfo> COMPENSATIONINFO;

protected:
	COMPENSATIONINFO	m_vCompensationInfoVec;
};

#define g_CompensationMgr CompensationMgr::GetSingleton()