#pragma once
#include "stdafx.h"

class ioBasicToolTip;

class ioBasicTooltipManager : public Singleton< ioBasicTooltipManager >
{
public:
	struct TooltipAlloc
	{
		std::string		szTooltipXmlName;		
		DWORD			dwUniqueIdx;		

		TooltipAlloc()
		{
			dwUniqueIdx = 0;
		}
	};
	typedef std::map<DWORD,TooltipAlloc> TooltipAllocMap;
	typedef std::vector<TooltipAlloc> TooltipAllocVector;


protected:
	ioWnd*				m_pToolTipWnd;
	TooltipAllocMap		m_TooltipAllocMap;
	TooltipAllocVector	m_ReserveAlloc;


public:
	virtual void Initialize();

public:
	void ClearAllGUI();
	void InitDeskTop( int iWidth, int iHeight );

public:
	DWORD NewToolTip( const std::string& szTooltipXmlName );
	void NewReserveToolTip( const std::string& szTooltipXmlName );
	void ExcuteReserveAlloc();
	void ExcuteReAlloc();

public:
	ioWnd* FindToolTip( const std::string& szTooltipXmlName );
	ioWnd* FindToolTip( DWORD dwID );

public:
	int GetToolTipID( const std::string& szTooltipXmlName );	

public:
	void ShowToolTip( DWORD dwID );
	void ShowToolTip( const std::string& szTooltipXmlName );
		
	void HideToolTip( DWORD dwID );
	void HideToolTip( const std::string& szTooltipXmlName );

public:
	static ioBasicTooltipManager& GetSingleton();

public:
	ioBasicTooltipManager(void);
	virtual ~ioBasicTooltipManager(void);
};

#define g_BasicTooltipMgr ioBasicTooltipManager::GetSingleton()