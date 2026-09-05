#pragma once

#include "CharacterInfo.h"
#include "GUI/ioPullDownEventListener.h"
#include "GUI/ioPullDownItem.h"

class ioPullDownList;
class CharAwakePullDownItem;
class ioPlayStage;

enum NeedMaterialType
{
	NMT_NONE	 = 0,
	NMT_PIECE	 = 1,	// 용병 조각
	NMT_ADDITIVE = 2,	// 첨가제
	NMT_PESO	 = 3,
};

//-----------------------------------------------------------------------------------------------------------------------

//각성재료 정보
struct NeedMaterial
{
	NeedMaterialType	eType;
	int					nNeedCount;		
	DWORD				dwNeedCode;

	NeedMaterial()
	{
		Clear();
	}

	void Clear()
	{
		eType		= NMT_NONE;
		nNeedCount	= 0;
		dwNeedCode	= 0;
	}
};
typedef std::map< int, NeedMaterial > NeedMaterialMap;

//-----------------------------------------------------------------------------------------------------------------------

//각성 정보
struct AwakeInfo
{
	AwakeType		eAwakeType;
	int				nCharGrowth[MAX_CHAR_GROWTH];
	int				nItemGrowth[MAX_ITEM_GROWTH];

	DWORD			dwNameColor;
	std::string		szToopTipFile;
	std::string		szName;
	std::string		szEffect;

	AwakeInfo()
	{
		Clear();
	}

	void Clear()
	{
		eAwakeType		= AWAKE_NONE;			
		dwNameColor		= 0xFF3A3A3A;

		memset( nCharGrowth, 0x00, sizeof(nCharGrowth) );
		memset( nItemGrowth, 0x00, sizeof(nItemGrowth) );

		szToopTipFile.clear();
		szName.clear();
	}
};
typedef std::map<int, AwakeInfo> AwakeInfoMap;

//-----------------------------------------------------------------------------------------------------------------------

//각성 풀 다운 메뉴 텍스트 정보
struct MenuElement
{
	std::string		szTitle;
	DWORD			dwTitleColor;
	int				iTitleSize;

	MenuElement()
	{
		szTitle.clear();
		dwTitleColor	= 0xFF1646B2;
		iTitleSize		= 12;
	}
};

//-----------------------------------------------------------------------------------------------------------------------

//각성 풀다운 메뉴 정보
struct AwakeProduct
{
	int nNeedDay;
	int nNeedMtrlIdx;
	int nNeedAwakeIdx;

	MenuElement sMainTitle;
	MenuElement sSubTitle;

	AwakeProduct()
	{
		nNeedDay	  = 0;
		nNeedMtrlIdx  = 0;
		nNeedAwakeIdx = 0;
	}
};
typedef std::map<int, AwakeProduct> AwakeProductMap;

//-----------------------------------------------------------------------------------------------------------------------

enum AWAKERESULT
{
	AR_ERROR	= 0,	//예외처리에 의한 에러
	AR_OK		= 1,	//성공
	AR_FAIL		= 2,	//실패
};

//-----------------------------------------------------------------------------------------------------------------------

//각성해제 정보
struct AwakeEndInfo
{
	int				iClassType;
	std::string		szClassName;
	std::string		szClassIconName;
	std::string		szClassSubIconName;
	std::string		szAwakeName;
	DWORD			dwAwakeColor;

	AwakeEndInfo()
	{
		iClassType		= 0;
		dwAwakeColor	= 0;
	}
};
typedef std::vector<AwakeEndInfo> AwakeEndInfoVector;

//-----------------------------------------------------------------------------------------------------------------------

//각성 풀다운 메뉴 아이템(GUI)
class CharAwakePullDownItem : public DoublePullDownItem
{
public:
	int	m_iProductIdx;
	int m_iTooltipIdx;

	CharAwakePullDownItem()
	{
		m_iProductIdx = 0;
		m_iTooltipIdx = 0;
	}

	virtual ~CharAwakePullDownItem(){}
};

//-----------------------------------------------------------------------------------------------------------------------

class ioSoldierAwakeManager : public Singleton< ioSoldierAwakeManager >
{
protected:
	int					m_nMaxAwakeLimit;

	AwakeInfoMap		m_AwakeInfoMap;
	NeedMaterialMap		m_NeedMaterialMap;
	AwakeProductMap		m_AwakeProductMap;
	AwakeEndInfoVector	m_AwakeEndVector;

	ioPlayStage*	m_pPlayStage;

public:
	virtual void Initialize();
	virtual void SetPlayStage( ioPlayStage* pStage ){ m_pPlayStage = pStage; }

protected:
	void LoadGrowth( ioINILoader& rkLoader, AwakeInfo& rkInfo );
	void LoadUI( ioINILoader& rkLoader, AwakeInfo& rkInfo );
	void LoadEffect( ioINILoader& rkLoader, AwakeInfo& rkInfo );
	void LoadMtrl( ioINILoader& rkLoader );
	void LoadAwakeProduct( ioINILoader& rkLoader );

protected:
	const NeedMaterial* GetMtrl( int nIdx ) const;
	const AwakeInfo* GetAwake( int nIdx ) const;
	const AwakeInfo* GetAwakeByType( AwakeType eType ) const;
	const AwakeProduct* GetAwakeProduct( int nIdx ) const;

public:
	const std::string& GetAwakeEffect( AwakeType eType ) const;

	int GetAwakeCharGrowth( AwakeType eType, int iArray );
	int GetAwakeItemGrowth( AwakeType eType, int iArray );

protected:
	void GetMtrlTitle( std::string& szTtitle, int nIdx ) const;
	void GetMtrlTitle( IN ioPullDownList* pList, int iClassType );
	void GetAwakeTtitle( std::string& szTtitle, int nIdx, int nPeriod ) const;

public:
	const std::string& GetAwakeName( AwakeType eType ) const;
	void GetRemainTime( int iLimitTime, std::string& szText );

public:	
	void GetMenuListName( std::string& szName, int iAwakeType, int iLimitTime );
	void GetMenuList( IN ioPullDownList* pList, int iClassType );
	
	bool AwakeTryMessageBox( const ClickPullDownEvent& Event, int iCharArray, ioWnd* pCalledWnd );
	void AwakeChangeFailMessageBox( const std::string& szOrgName, const std::string& szChangeName );
	void AwakeResultMessageBox( const std::string& szName, int iLimitTime );	
	void AwakeCharPeriodTypeError( const std::string& szName ) const;
	void AwakeCharExerciseTypeError( const std::string& szName ) const;

	byte GetAwakeType( const CharAwakePullDownItem* pItem ) const;
	AWAKERESULT IsEnableAwakeMtrlUse( const CharAwakePullDownItem* pItem, int iClassType ) const;

public:
	void OnAwakeMenuEvent( const ClickPullDownEvent& Event, int iCharArray ) const;
	void OnAwakeMenuToolTipShow( const OnOverPullDownEvent& Event, int iCharArray ) const;
	void OnAwakeMenuToolTipHide( const OnLeavePullDownEvent& Event, int iCharArray ) const;

	void SetAwake( const CharAwakePullDownItem* pItem, const CHARACTERDATA* pChar ) const;
	void ExtendAwake( const CharAwakePullDownItem* pItem, const CHARACTERDATA* pChar ) const;

public:
	void ApplyAwake( SP2Packet &rkPacket );
	void ApplyAwakeExtend( SP2Packet &rkPacket );
	void ApplyAwakeEnd( SP2Packet &rkPacket );

public:
	bool IsReserveDeleteAwake();
	void GetFirstDeleteAwake( std::string& szClassName, std::string& szClassIconName, std::string& szClassSubIconName, std::string& szAwakeName, DWORD& dwAwakeColor, int& nClassType );
	void EraseDeleteExtraAwake( bool bAllErase );

public:
	static ioSoldierAwakeManager& GetSingleton();

public:
	ioSoldierAwakeManager(void);
	virtual ~ioSoldierAwakeManager(void);
};

#define g_AwakeMgr ioSoldierAwakeManager::GetSingleton()

