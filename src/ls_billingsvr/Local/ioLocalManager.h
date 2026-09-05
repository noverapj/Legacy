#ifndef __ioLocalManager_h__
#define __ioLocalManager_h__

#include "../Util/Singleton.h"

class ioLocalParent;

class ioLocalManager : public Singleton< ioLocalManager >
{
public:
	enum LocalType
	{
		LCT_KOREA     = 119483910, // 타입값은 보안을 위해서 순차적으로 증가하지 않고 복잡하게 만듬.
		LCT_US        = 365849202,
		LCT_CHINA     = 917382048,
		LCT_TAIWAN    = 521837584,
		LCT_JAPAN     = 213527182,
		LCT_MALAYSIA  = 493820302,
		LCT_INDONESIA = 938400398,
		LCT_VIETNAM   = 783920174,
		LCT_THAILAND  = 800125411,
		LCT_LATIN	  = 458278951,
		LCT_EU		  = 647291028,
		LCT_BRAZIL	  = 329859423,
		LCT_TURKEY    = 129590349,

	};

protected:
	static LocalType m_eLocalType;
	
public:
	static LocalType GetLocalType() { return m_eLocalType; }
	static void SetLocalType( LocalType eLocalType) { m_eLocalType = eLocalType; }

protected:
	typedef std::vector< ioLocalParent* > vLocalVector;
	vLocalVector m_vLocalVector;

protected:
	ioLocalParent *CreateLocal( LocalType eLocalType );
	void AddLocal( ioLocalParent *pLocal );
	const char *GetLocalName( LocalType eLocalType );

public:
	void Init();
	ioLocalParent *GetLocal( LocalType eLocalType );

public:
	static ioLocalManager &GetSingleton();


public:
	ioLocalManager(void);
	virtual ~ioLocalManager(void);
};


#define g_LocalMgr ioLocalManager::GetSingleton()

#endif // __ioLocalManager_h__