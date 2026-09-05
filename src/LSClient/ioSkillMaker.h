#pragma once


#include "ioSkill.h"

class ioINILoader;
class ioPlayStage;

class ioSkillMaker : public Singleton< ioSkillMaker >
{
protected:
	ioPlayStage* m_pPlayStage;

	typedef std::map< ioHashString, ioSkill* > SkillMap;
	SkillMap m_SkillMap;

public:
	void ClearSkillList();
	void LoadSkillList( const char *szFileName );

#ifndef SHIPPING //UJ
	#define LoadSkillList_e	LoadSkillList
#else
	//UJ 130812, 스트링 상수 암호화 및 숨기기 위한 코드
	template <int N>
	__forceinline void LoadSkillList_e( const char (&szFileName)[N] )
	{
		_ENCSTR(szFileName, szEncFileName);
		LoadSkillList(szEncFileName);
	}
#endif // _DEBUG

	void SetPlayStage( ioPlayStage *pStage );
	ioPlayStage* GetPlayStage();

public:
	ioSkill* CreateSkill( const ioHashString &rkName );
	const ioSkill* GetSkillConst( const ioHashString &rkName ) const;

	bool HasSkill( const ioHashString &rkName );

protected:
	ioSkill* CreateSkillTemplete( ioINILoader &rkLoader );
	ioSkill* CreateAttackSkillTemplete( ioINILoader &rkLoader );
	ioSkill* CreateBuffSkillTemplete( ioINILoader &rkLoader );
	ioSkill* CreateNormalSkillTemplete( ioINILoader &rkLoader );
	ioSkill* CreateRangeSkillTemplete( ioINILoader &rkLoader );
	ioSkill* CreatePassiveSkillTemplete( ioINILoader &rkLoader );
	ioSkill* CreateCountSkillTemplete( ioINILoader &rkLoader );

public:
	static ioSkillMaker& GetSingleton();

public:
	ioSkillMaker();
	virtual ~ioSkillMaker();
};

#define g_SkillMaker ioSkillMaker::GetSingleton()

