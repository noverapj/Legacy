#ifndef __ioFirstSoldierManager_h__
#define __ioFirstSoldierManager_h__

#include "../Util/Singleton.h"

class ioFirstSoldierManager : public Singleton< ioFirstSoldierManager >
{
protected:
	struct DecoVectorInfo
	{
		int       m_iClassType; 
		int       m_iSexType;   // RDT_HUMAN_MAN, RDT_WOHUMAN_MAN
		int       m_iDecoType;  // UID_HAIR..
		IntVec    m_vDecoCode; 

		DecoVectorInfo()
		{
			m_iSexType    = 0; //RDT_HUMAN_MAN;
			m_iDecoType   = 2; //UID_HAIR;
			m_iClassType  = -1;
			m_vDecoCode.clear();
		}
	};
	typedef std::vector<DecoVectorInfo*> vDecoVectorInfo;

protected:
	vDecoVectorInfo m_vDecoVectorInfo;

protected:
	void Clear();
	int  GetDecoKindredCode( const CHARACTER &rkCharInfo );
	bool IsExistDeco( int iClassType, int iSexType, int iDecoType, int iDecoCode );

public:
	static ioFirstSoldierManager& GetSingleton();

public:
	void LoadINI();
	bool IsExistDecoAll( const CHARACTER &rkCharInfo );

public:
	ioFirstSoldierManager(void);
	virtual ~ioFirstSoldierManager(void);
};


#define g_FirstSoldierMgr ioFirstSoldierManager::GetSingleton()

#endif // __ioFirstSoldierManager_h__