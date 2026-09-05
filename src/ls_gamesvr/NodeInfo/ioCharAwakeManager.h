#ifndef __ioCharAwakeManger_h__
#define __ioCharAwakeManger_h__

#include "../Util/Singleton.h"

class ioCharAwakeManager : public Singleton< ioCharAwakeManager > 
{
protected:
	enum
	{
		AWAKE_NONE      = 0,
		AWAKE_NORMAL	= 1,
		AWAKE_RARE      = 2
	};
	
	struct AwakeMaterialInfo
	{
		int iMaterialType;
		int iMaterialCode;
		int iNeedCount;

		AwakeMaterialInfo()
		{
			iMaterialType = 0;
			iMaterialCode = 0;
			iNeedCount = 0;
		}
	};

	struct AwakeInfo
	{
		int iAwakeType;			// 각성 종류
		int iMaterialInfoNum;	// AwakeMaterialInfo 구조체에 저장된 번호
		int iActDay;			// 각성 시간

		AwakeInfo()
		{
			iAwakeType = 0;
			iMaterialInfoNum = 0;
			iActDay = 0;
		}
	};

protected:
	void Clear();

protected:
	int m_iMaxAwakePeriod;
	int m_iMaxAwakeKind;
	int m_iMaxMaterial;

	std::vector< int > m_vAwakeAddDateInfo; //각성 가능한 각성 일자 저장
	std::vector< AwakeMaterialInfo > m_vAwakeMaterialInfo; // 각성에 따른 재료 정보 저장 
	std::vector< AwakeInfo > m_vAwakeInfo; //각성 정보

public:
	static ioCharAwakeManager& GetSingleton();

public:
	void LoadINI();

	bool IsLoadedAwakeDay( int iAwakeDay );	//각성 가능한 일자 저장.

	void GetMaterialInfo( const int iAwakeType, const int iAddDay, const int iSoldierCode, int &iMaterialCode, int &iMaterialCount );

	AwakeInfo* GetAwakeInfo( const int iAwakeType, const int iAddDay );

	bool CheckRightDate( const int& iAddDay );
	bool CheckMaxAwakePeriod( const int& iAddDay, const int& iCurEndDate );
	
	bool CheckRightAwakeChange( const int iPrevAwake, const int iNextAwake );

public:
	ioCharAwakeManager();
	virtual ~ioCharAwakeManager();
};

#define g_CharAwakeManager ioCharAwakeManager::GetSingleton()

#endif