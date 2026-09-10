#pragma once


class ioINILoader;

class ioItemExpert  
{
protected:
	struct ExpertInfo
	{
		int          iReachExpert;
		ioHashString szTargetKeyName;
		int          iValue;
		float        fValue;
		ioHashString szValue;
		ioHashString szDesc;
		int          iPeso;
		
		ExpertInfo()
		{
			iReachExpert = 0;
			szTargetKeyName.Clear();
			iValue = 0;
			fValue = 0.0f;
			szValue.Clear();
			szDesc.Clear();
			iPeso  = 0;
		}
	};
	
protected:
	typedef std::vector<ExpertInfo> ExpertInfoList;

protected:
	ExpertInfoList       m_ExpertInfoList;

protected:
	const ExpertInfo* GetExpertInfo(int iLevel);

public:
	void LoadExpertInfo( ioINILoader &rkLoader );
	
public:
	const ioHashString* GetExpertKeyName(int iLevel);
	void AddExpertValue(int iLevel, int &iDestValue);
	void AddExpertValue(int iLevel, DWORD &dwDestValue);
	void AddExpertValue(int iLevel, float &fDestValue);
	void AddExpertValue(int iLevel, ioHashString &szDestValue);

	const ioHashString* GetExpertDesc(int iLevel);
	int GetReachExpert(int iLevel);
	int GetPeso(int iLevel);
	
public:
	ioItemExpert();
	virtual ~ioItemExpert();
};

