#pragma once
class ioUIRenderImage;

class ioModeResourceManager : public Singleton< ioModeResourceManager >
{
protected:
	struct ModeIcon
	{
		int					eType;
		int					eKindValue1;	//모드 구별 인자(광장, 룸 등)
		int					eKindValue2;	//위와 동

		ioUIRenderImage*	pModeIcon;
		ioUIRenderImage*	pUserModeIcon;

		ModeIcon()
		{
			eType			= 0;
			eKindValue1		= 0;
			eKindValue2		= 0;
			pModeIcon		= NULL;
			pUserModeIcon	= NULL;
		}
	};
	typedef std::vector< ModeIcon > vModeIconList;

	vModeIconList m_vModeIconList;

public:
	static ioModeResourceManager &GetSingleton();
	static ioModeResourceManager *GetSingletonPtr();

public:
	void GetModeTitle( OUT ioHashString& szOut, int eModeType );
	void GetPlazaTitle( OUT ioHashString& szOut, int ePlazaType );
	void GetModeRoomTitle( OUT ioHashString& szOut, int eMyRoomType, bool bCampHeroBattle );

	void InitCommonModeIcon();
	void AddModeIcon( ioINILoader& rkLoader, const ioHashString& szTitle, int eType, int eKindValue1, int eKindValue2 );

	ioUIRenderImage* GetModeIcon( int eType );
	ioUIRenderImage* GetModeIconByUserMode( int eType );

	ioUIRenderImage* GetPlazaIcon( int ePlazaType );
	ioUIRenderImage* GetModeRoomIcon( int eRoomType, bool bCampHeroBattle );

public:
	ioModeResourceManager();
	virtual ~ioModeResourceManager();
};

#define g_ModeResourceMgr ioModeResourceManager::GetSingleton()

