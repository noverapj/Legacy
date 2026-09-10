#ifndef __TOOLTIP_MANAGER_H__
#define __TOOLTIP_MANAGER_H__

#include "ToolTip_Base.h"

//----------------------------------------------------------------------
// TooltipInfo
//----------------------------------------------------------------------
struct TooltipInfo
{
	int m_iTooltipItemType;
	DWORD m_dwCode;
	int m_iValue;
    int m_iReinforce;

	TooltipInfo()
	{
		Clear();
	}

	void Clear()
	{
		m_iTooltipItemType = 0;
		m_dwCode = 0;
		m_iValue = 0;
        m_iReinforce = 0;
	}
};
typedef std::vector< TooltipInfo > TooltipInfoList;

//----------------------------------------------------------------------
// ToolTipData
//----------------------------------------------------------------------
class ToolTipData
{
public:
    ToolTipData()
    {
        Init();
    }

    void Init()
    {
        m_szIconName.Clear();
        m_szString[TTCT_TITLE   ].Clear();
        m_szString[TTCT_SUBTITLE].Clear();
        m_szString[TTCT_COMMENT ].Clear();
        m_fSize[TTCT_TITLE   ] = FONT_SIZE_15;
        m_fSize[TTCT_SUBTITLE] = FONT_SIZE_15;
        m_fSize[TTCT_COMMENT ] = FONT_SIZE_12;
        m_bFixedPos = false;
        m_iFixedPosX = 0;
        m_iFixedPosY = 0;
    }

    void        SetIconName(const char* szIconName) { m_szIconName = szIconName; }
    const char* GetIconName() { return m_szIconName.c_str(); }

    void        SetString(TOOLTIP_CONTENTS_TYPE eType, const char* szString) { m_szString[eType] = szString; }
    const char* GetString(TOOLTIP_CONTENTS_TYPE eType) { return m_szString[eType].c_str(); }

    void        SetSize(TOOLTIP_CONTENTS_TYPE eType, float fSize) { m_fSize[eType] = fSize; }
    float       GetSize(TOOLTIP_CONTENTS_TYPE eType) { return m_fSize[eType]; }

    void        SetFixedPos(bool bFixedPos) { m_bFixedPos = bFixedPos; }
    bool        GetFixedPos() { return m_bFixedPos; }

    void        SetFixedPosX(int iFixedPosX) { m_iFixedPosX = iFixedPosX; }
    int         GetFixedPosX() { return m_iFixedPosX; }

    void        SetFixedPosY(int iFixedPosY) { m_iFixedPosY = iFixedPosY; }
    int         GetFixedPosY() { return m_iFixedPosY; }

protected:
    ioHashString    m_szIconName;
    ioHashString    m_szString[TTCT_COUNT];
    float           m_fSize[TTCT_COUNT];
    bool            m_bFixedPos;
    int             m_iFixedPosX;
    int             m_iFixedPosY;
};


//----------------------------------------------------------------------
// IoToolTipManager
//----------------------------------------------------------------------
class IoToolTipManager : public Singleton< IoToolTipManager >
{
public:
	enum eToolTip_Type
	{
		eToolTip_Base,
		eToolTip_Size,
	};

public:
	IoToolTipManager();
	virtual ~IoToolTipManager();

	void			InitToolTipMgr();

	bool			OpenToolTip(eToolTip_Type eType, const char* szIconName, const char* szTitle, const char* szSubTitle, const char* szComment, bool bFixedPos = false, int iFixedPosX = 0, int iFixedPosY = 0, bool bReverse = false );
    bool            OpenToolTip(eToolTip_Type eType, ToolTipData& kData);

	void			SetTitleColor( eToolTip_Type eType, DWORD dwTextColor, DWORD dwBkColor );
	void			SetSubTitleColor( eToolTip_Type eType, DWORD dwTextColor, DWORD dwBkColor );
	void			SetCommentColor( eToolTip_Type eType, DWORD dwTextColor, DWORD dwBkColor );

	ToolTip_Base*	GetToolTip(eToolTip_Type eType);
	void			CloseToolTip();

	void			SetCurrentToolTip(ioWnd* pWnd);

	void			Update();
	void			Release();

protected:
	ioWnd*									m_pCurrentTip;
	std::map<eToolTip_Type, ToolTip_Base*>	m_mapTooltip;
};

#define g_pToolTipMgr IoToolTipManager::GetSingletonPtr()

#endif