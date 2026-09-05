#pragma once

class ioINILoader;
class ioUIRenderImage;
class ioUIRenderFrame;

class ioUIImageRenderManager : public Singleton< ioUIImageRenderManager >
{
protected:
	enum UIGradeType
	{
		UGT_NONE,
		UGT_NORMAL,
		UGT_PREMIUM,
		UGT_RARE,
		UGT_POWERUP,
		UGT_UNIQUE,
	};

	struct UIGradeInfo
	{
		ioUIRenderImage *m_pGradeIconBack;
		DWORD m_dwGradeTextColor;
	};

	typedef std::map<int, UIGradeInfo> UIGradeInfoList;
	
protected:
	UIGradeInfoList m_UIGradeInfoList;

public:
	void LoadProperty();

protected:
	void LoadUIGradeInfo( ioINILoader& kLoader );
	void ReleaseUIGradeInfo();

public:
	int GetPowerUpGradeType( int nClassType );
	int GetPowerUpGradeType( int nClassType, byte eCharPowerUpType );

public:
	DWORD GetGradeTextColor( int nGradeType );

public:
	void RenderGradeIconBack( int nGradeType,
							  int nXpos, int nYpos, float fScale = FLOAT1,
							  UIRenderType eRenderType = UI_RENDER_NORMAL, TextureFilterOption eOption = TFO_NEAREST,
							  int nRed = 255, int nGreen = 255, int nBlue = 255,
							  bool bNoMagicAlign = false, UIRenderType eGrayRenderType = UI_RENDER_NORMAL );

public:
	static ioUIImageRenderManager& GetSingleton();

public:
	ioUIImageRenderManager();
	virtual ~ioUIImageRenderManager();
};

#define g_UIImageRenderMgr ioUIImageRenderManager::GetSingleton()