
// BillingInfoListDlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "BillingInfoList.h"
#include "BillingInfoListDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBillingInfoListDlg 대화 상자





CBillingInfoListDlg::CBillingInfoListDlg(const TCHAR* strParameter, CWnd* pParent /*=NULL*/)
	: CDialogEx(CBillingInfoListDlg::IDD, pParent)
{
	m_strParameter = strParameter;
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBillingInfoListDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TEXT_TOTAL, text_total);
}

BEGIN_MESSAGE_MAP(CBillingInfoListDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ETCITEM, &CBillingInfoListDlg::OnBnClickedEtcitem)
	ON_BN_CLICKED(IDC_DECOITEM, &CBillingInfoListDlg::OnBnClickedDecoitem)
	ON_BN_CLICKED(ID_SOLDIER, &CBillingInfoListDlg::OnBnClickedSoldier)
	ON_BN_CLICKED(IDC_EXTRAITEM, &CBillingInfoListDlg::OnBnClickedExtraitem)
	ON_BN_CLICKED(IDC_POPUP, &CBillingInfoListDlg::OnBnClickedPopup)
	ON_BN_CLICKED(IDC_TOTAL, &CBillingInfoListDlg::OnBnClickedTotal)
	ON_BN_CLICKED(IDC_ITEMTOTAL, &CBillingInfoListDlg::OnBnClickedItemtotal)
	ON_BN_CLICKED(IDC_ITEMETC, &CBillingInfoListDlg::OnBnClickedItemetc)
	ON_BN_CLICKED(IDC_ITEMDECO, &CBillingInfoListDlg::OnBnClickedItemdeco)
	ON_BN_CLICKED(IDC_ITEMSOLDiER, &CBillingInfoListDlg::OnBnClickedItemsoldier)
	ON_BN_CLICKED(IDC_ITEMEXTRA, &CBillingInfoListDlg::OnBnClickedItemextra)
	ON_BN_CLICKED(IDC_ITEM_POPUP, &CBillingInfoListDlg::OnBnClickedItemPopup)
END_MESSAGE_MAP()


// CBillingInfoListDlg 메시지 처리기

BOOL CBillingInfoListDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다. 응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	// TODO: Add extra initialization here

	LoadChangeItemInfo();

	LoadBillingGoods();

	LoadItemNameINI(_T("config/sp2_item.ini"));

	text_total.SetWindowText("Ready");

	GetDlgItem(IDC_ETCITEM)->EnableWindow(FALSE);
	GetDlgItem(IDC_DECOITEM)->EnableWindow(FALSE);
	GetDlgItem(ID_SOLDIER)->EnableWindow(FALSE);
	GetDlgItem(IDC_EXTRAITEM)->EnableWindow(FALSE);
	GetDlgItem(IDC_POPUP)->EnableWindow(FALSE);
	GetDlgItem(IDC_TOTAL)->EnableWindow(FALSE);

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CBillingInfoListDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다. 문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CBillingInfoListDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CBillingInfoListDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int CBillingInfoListDlg::LoadIntSetting(CString lpAppName, CString lpKeyName, int iDefailt, CString strfullPath)
{
	int iValue;
	TCHAR buffer[512];
	CString defaultStr= "default";

	// name
	DWORD result= GetPrivateProfileString( lpAppName, lpKeyName, defaultStr, buffer, _countof(buffer), strfullPath);
	buffer[result] = 0;
	if(defaultStr == buffer)
	{
		iValue = iDefailt;
	}
	else
	{
		iValue = _ttoi(buffer);
	}
	return iValue;
}

float CBillingInfoListDlg::LoadFloatSetting(CString lpAppName, CString lpKeyName, float fDefailt, CString strfullPath)
{
	float fValue;
	TCHAR buffer[512];
	CString defaultStr= "default";

	// name
	DWORD result= GetPrivateProfileString( lpAppName, lpKeyName, defaultStr, buffer, _countof(buffer), strfullPath);
	buffer[result] = 0;
	if(defaultStr == buffer)
	{
		fValue = fDefailt;
	}
	else
	{
		fValue = _ttof(buffer);
	}

	return fValue;
}

CString CBillingInfoListDlg::LoadStringSetting(CString lpAppName, CString lpKeyName, CString strDefault, CString strfullPath)
{
	CString strValue;
	TCHAR buffer[512];
	CString defaultStr= "default";

	// name
	DWORD result= GetPrivateProfileString( lpAppName, lpKeyName, defaultStr, buffer, _countof(buffer), strfullPath);
	buffer[result] = 0;
	if(defaultStr == buffer)
	{
		strValue = strDefault;
	}
	else
	{
		strValue = buffer;
	}

	strValue.Replace(_T(','), _T('_'));

	return strValue;
}


void CBillingInfoListDlg::ChangeIntSetting(CString lpAppName, CString lpKeyName, int ChangeValue, CString strfullPath)
{
	int iValue;
	TCHAR buffer[512];
	CString strChangeValue= "";
	strChangeValue.Format(_T("%d"), ChangeValue);

	// name
	bool bResult = WritePrivateProfileString( lpAppName, lpKeyName, strChangeValue, strfullPath);
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


void CBillingInfoListDlg::LoadChangeItemInfo()
{
	TCHAR buffer[512];
	TCHAR path[512];
	GetCurrentDirectory(sizeof(path), path);

	CString fullPath;
	fullPath.Format(_T("%s\\%s"), path, _T("config/ChangeItemInfo.ini"));

	CString section =_T("ChangeInfo");
	// name

	m_fChangeCash   = LoadFloatSetting(section, _T("ChangeCash"), 0.0f, fullPath);
	m_iEtcItemValue	= LoadIntSetting(section, _T("etcitem"), 0, fullPath);
	m_iDecoItem		= LoadIntSetting(section, _T("decoItem"), 0, fullPath);

	CString strValue;
	int Class_Max	= LoadIntSetting(section, _T("Class_Max"), 0, fullPath);
	for (int k = 0; k < Class_Max ; k++)
	{
		strValue.Format(_T("Class_%dStart"), k+1 );
		int iClassStart = LoadIntSetting(section, strValue, 0, fullPath);
		strValue.Format(_T("Class_%dEnd"), k+1 );
		int iClassEnd = LoadIntSetting(section, strValue, 0, fullPath);
		strValue.Format(_T("Class_%dRate"), k+1 );
		int iClassRate = LoadIntSetting(section, strValue, 0, fullPath);

		HeroClassRate* pkHeroClassRate = new HeroClassRate;

		pkHeroClassRate->m_iStart = iClassStart;
		pkHeroClassRate->m_iEnd = iClassEnd;
		pkHeroClassRate->m_iRate = iClassRate;
		m_vecHeroClassRate.push_back(pkHeroClassRate);
	}

	m_iHero_Normal	= LoadIntSetting(section, _T("normal_char"), 0, fullPath);
	m_iHero_Rare	= LoadIntSetting(section, _T("rare_char"), 0, fullPath);
	m_iHero_Premium	= LoadIntSetting(section, _T("premium_char"), 0, fullPath);

	m_iChange		= LoadIntSetting(section, _T("Change"), 0, fullPath);
}



void CBillingInfoListDlg::LoadBillingGoods()
{
	MapItemEtc.clear();
	
	TCHAR buffer[512];
	TCHAR path[512];
	GetCurrentDirectory(sizeof(path), path);

	CString fullPath;
	fullPath.Format(_T("%s\\%s"), path, "config/GoodsList.ini");

	CString section =_T("SoldierInfo");
	// name
	int iMaxCount = LoadIntSetting(section, _T("Max"), 0, fullPath);

	CString strValue, strPeso, strCash;
	int iLimitSeconds, iGoodsNoValue;
	for (int k = 0; k < iMaxCount ; k++)
	{
		strValue.Format(_T("LimitSeconds_%d"), k+1 );
		strPeso.Format(_T("GoodsNoValue_%d"), k+1 );

		iLimitSeconds = LoadIntSetting(section, strValue, 0, fullPath);
		iGoodsNoValue = LoadIntSetting(section, strPeso, 0, fullPath);

		SoldierInfo kInfo;
		kInfo.m_iLimitSeconds = iLimitSeconds;
		kInfo.m_iGoodsNoValue = iGoodsNoValue;
		m_vSoldierInfo.push_back( kInfo );
	}

	m_iMortmainCharGoodsNoValue = LoadIntSetting(section, _T("MortmainCharGoodsNoValue"), 0, fullPath);

	CString GoodsList =_T("GoodsList");
	// name
	int iMaxCountL = LoadIntSetting(GoodsList, _T("Max"), 0, fullPath);
	for (int k = 0; k < iMaxCountL ; k++)
	{
		strValue.Format(_T("GoodsNo_%d"), k+1 );
		strPeso.Format(_T("GoodsName_%d"), k+1 );

		int iGoodsNo = LoadIntSetting(GoodsList, strValue, 0, fullPath);
		CString strGoodsName = LoadStringSetting(GoodsList, strPeso, "", fullPath);

		m_GoodsInfoMap.insert(std::map<int, CString>::value_type(iGoodsNo, strGoodsName));
	}

	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////////////////
	LoadBillingEtcItem(fullPath);

	LoadBillingExtraItem(fullPath);

}


void CBillingInfoListDlg::LoadBillingEtcItem(CString fullPath)
{
	TCHAR buffer[512];

	CString section =_T("PackageShortTypeList");
	// name

	int iMaxCount = LoadIntSetting(section, _T("Max"), 0, fullPath);

	CString strValue, strPeso, strCash;
	int iShortType;
	for (int k = 0; k < iMaxCount ; k++)
	{
		strValue.Format(_T("ShortType_%d"), k+1 );

		iShortType = LoadIntSetting(section, strValue, 0, fullPath);

		m_vPackageEtcItemShortTypeList.push_back(iShortType);
	}
}

void CBillingInfoListDlg::LoadBillingExtraItem(CString fullPath)
{
	TCHAR buffer[512];

	CString name, main, login, billing;

	CString section =_T("PackageExtraItemMachineCodeList");
	// name
	int iMaxCount = LoadIntSetting(section, _T("Max"), 0, fullPath);

	CString strValue, strPeso, strCash;
	int iMachineCode;
	for (int k = 0; k < iMaxCount ; k++)
	{
		strValue.Format(_T("MachineCode_%d"), k+1 );

		iMachineCode = LoadIntSetting(section, _T("Max"), 0, fullPath);

		m_vPackageExtraItemMachineCodeList.push_back(iMachineCode);
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void CBillingInfoListDlg::LoadItemNameINI(const TCHAR* file)
{
	m_mapItemName.clear();
	
	TCHAR buffer[512];
	TCHAR path[512];
	GetCurrentDirectory(sizeof(path), path);

	CString fullPath;
	fullPath.Format(_T("%s\\%s"), path, file);

	CString section =_T("common_info");
	
	int iMaxCount = LoadIntSetting(section, _T("item_count"), 0, fullPath);

	CString strItem;
	for( int i=0 ; i<iMaxCount ; i++ )
	{
		strItem.Format(_T("item%d"), i+1 );
		
		int iCode = LoadIntSetting(strItem, _T("code"), 0, fullPath);
		CString strName = LoadStringSetting(strItem, _T("name"), "", fullPath);

		m_mapItemName.insert( std::map<int, CString>::value_type( iCode, strName ) );
	}
}

void CBillingInfoListDlg::LoadEtcItemINI(const TCHAR* file)
{
	MapItemEtc.clear();
	
	TCHAR buffer[512];
	TCHAR path[512];
	GetCurrentDirectory(sizeof(path), path);

	CString fullPath;
	fullPath.Format(_T("%s\\%s"), path, file);

	CString section =_T("common");
	
	int iMaxCount = LoadIntSetting(section, _T("Max"), 0, fullPath);

	CString strEtcItem;
	for( int i=0 ; i<iMaxCount ; i++ )
	{
		strEtcItem.Format(_T("etcitem%d"), i+1 );
		
		CString strName = LoadStringSetting(strEtcItem, _T("name"), "", fullPath);
		int iType = LoadIntSetting(strEtcItem, _T("type"), 0, fullPath);

		CItemEtc* pkItemEtc = new CItemEtc;
		pkItemEtc->strName = strName;
		pkItemEtc->iType = iType;

		if(iType == 0)
		{
			continue;
		}

		CString strValue, strPeso, strCash, strActive;
		int iValue, iPeso, iCash, iActive;
		for (int k = 0; k < 15 ; k++)
		{
			strValue.Format(_T("value%d"), k+1 );
			strPeso.Format(_T("peso%d"), k+1 );
			strCash.Format(_T("cash%d"), k+1 );
			strActive.Format(_T("active%d"), k+1 );

			iValue = LoadIntSetting(strEtcItem, strValue, -1, fullPath);
			if(iValue == -1)
			{
				continue;
			}

			iPeso = LoadIntSetting(strEtcItem, strPeso, 0, fullPath);
			iCash = LoadIntSetting(strEtcItem, strCash, 0, fullPath);
			iActive = LoadIntSetting(strEtcItem, strActive, 0, fullPath);

			if(0 >= iCash)
			{
				continue;
			}

			if(1 == m_iChange)
			{
				iCash = iCash * m_iEtcItemValue / 100;
				ChangeIntSetting(strEtcItem, strCash, iCash, fullPath);
			}

			CEtcValue* pkEtcValue = new CEtcValue;
			pkEtcValue->iActive = iActive;
			pkEtcValue->iValue = iValue;
			pkEtcValue->iPeso = iPeso;
			pkEtcValue->iCash = iCash;
			pkItemEtc->EtcValueList.push_back(pkEtcValue);

			//SALE g_SaleMgr.LoadINI( fullPath, strEtcItem, ioSaleManager::IT_ETC, iType, k, k );
		}

		MapItemEtc.insert( std::map<int, CItemEtc*>::value_type( iType, pkItemEtc ) );
	}
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void CBillingInfoListDlg::LoadDecoItemINI(const TCHAR* file)
{
	m_vSexList.clear();
	
	TCHAR buffer[512];
	TCHAR path[512];
	GetCurrentDirectory(sizeof(path), path);

	CString fullPath;
	fullPath.Format(_T("%s\\%s"), path, file);

	CString section =_T("Info");

	int iMaxCount = LoadIntSetting(section, _T("max_sex"), 0, fullPath);

	CString strSex;
	int iSex, result;
	CString strName, strDeco_ini;
	for( int i=0 ; i<iMaxCount ; i++ )
	{
		strSex.Format(_T("Sex%d"), i+1 );

		iSex = LoadIntSetting(strSex, _T("Sex"), 0, fullPath);
		strName = LoadStringSetting(strSex, _T("Name"), "", fullPath);
		strDeco_ini = strName = LoadStringSetting(strSex, _T("Deco_ini"), "", fullPath);;

		SexDecoList kSexList;
		kSexList.m_iSex = iSex;
		kSexList.m_szName = strName;
		kSexList.m_szINI = strDeco_ini;

		m_vSexList.push_back(kSexList);
	} 

	LoadDecoInfoItemINI(path);
}

void CBillingInfoListDlg::LoadDecoInfoItemINI(const TCHAR* path)
{
	int iSexSize = m_vSexList.size();
	TCHAR buffer[512];
	CString fullPath;

	for(int i = 0;i < iSexSize;i++)
	{
		fullPath.Format(_T("%s\\%s"), path, m_vSexList[i].m_szINI);

		CString section =_T("Info");

		int iMaxDeco = LoadIntSetting(section, _T("max_Deco"), 0, fullPath);

		CString strDeco;
		int iType, iPackageKeepPeso, iMax;
		CString strTitle;
		for(int j = 0;j < iMaxDeco;j++)
		{
			strDeco.Format(_T("Deco%d"), j+1 );

			strTitle = LoadStringSetting(strDeco, _T("Title"), "", fullPath);
			iType = LoadIntSetting(strDeco, _T("Type"), 0, fullPath);
			iPackageKeepPeso = LoadIntSetting(strDeco, _T("PackageKeepPeso"), 3000, fullPath);
			iMax = LoadIntSetting(strDeco, _T("Max"), 0, fullPath);

			DecoList kDL;
			kDL.m_strTitle = strTitle;
			kDL.m_iDecoType = iType;
			kDL.m_iPackageKeepPeso = iPackageKeepPeso;
			m_vSexList[i].m_vList.push_back( kDL );

			int iDecoArray = j;
			CString strKey, strCash;
			for(int k = 0;k < iMax;k++)
			{
				DecoData kDD;

				strKey.Format(_T("Name_%d"), k + 1 );
				kDD.m_szName = LoadStringSetting(strDeco, strKey, "", fullPath);

				strKey.Format(_T("Code_%d"), k + 1 );
				kDD.m_iDecoCode = LoadIntSetting(strDeco, strKey, 0, fullPath);

				strKey.Format(_T("Peso_%d"), k + 1 );
				kDD.m_iPeso = LoadIntSetting(strDeco, strKey, 0, fullPath);

				strCash.Format(_T("Cash_%d"), k + 1 );
				kDD.m_iCash = LoadIntSetting(strDeco, strCash, 0, fullPath);

				strKey.Format(_T("BonusPeso_%d"), k + 1 );
				kDD.m_iBonusPeso = LoadIntSetting(strDeco, strKey, 0, fullPath);

				strKey.Format(_T("SellPeso_%d"), k + 1 );
				kDD.m_iSellPeso = LoadIntSetting(strDeco, strKey, 0, fullPath);

				strKey.Format(_T("LimitLevel_%d"), k + 1 );
				kDD.m_iLimitLevel = LoadIntSetting(strDeco, strKey, 0, fullPath);

				strKey.Format(_T("SubscriptionType_%d"), k + 1 );
				kDD.m_iSubscriptionType = LoadIntSetting(strDeco, strKey, 0, fullPath);

				strKey.Format(_T("Active_%d"), k + 1 );
				kDD.m_bActive = LoadIntSetting(strDeco, strKey, 0, fullPath);

				if(0 >= kDD.m_iCash)
				{
					continue;
				}

				if(1 == m_iChange)
				{
					kDD.m_iCash = kDD.m_iCash * m_iDecoItem / 100;
					ChangeIntSetting(strDeco, strCash, kDD.m_iCash, fullPath);
				}

				m_vSexList[i].m_vList[iDecoArray].m_vList.push_back( kDD );

				//SALE g_SaleMgr.LoadINI( fullPath, strDeco, ioSaleManager::IT_DECO, kDL.m_iDecoType+(i*1000), kDD.m_iDecoCode, k ); // i*1000은 성별 타입을 넣는다.
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void CBillingInfoListDlg::LoadSoldierItemINI(const TCHAR* file)
{
	m_MapItemClass.clear();
	
	TCHAR buffer[512];
	TCHAR path[512];
	GetCurrentDirectory(sizeof(path), path);

	CString fullPath;
	fullPath.Format(_T("%s\\%s"), path, file);

	CString section =_T("INFO");

	//무제한 용병의 캐쉬 판매가격 곱하기 
	//;무제한 용병의 캐쉬 판매가격 곱하기 >> cash x mortmain_char_multiply_cash 
	fMortmainCharMultiplyCash = LoadFloatSetting(section, _T("mortmain_char_multiply_cash"), 1.0f, fullPath);
	//;무제한 프리미엄 용병의 캐쉬 판매가격 곱하기 >> cash x mortmain_premium_char_multiply_cash 
	fMortmainPremiumCharMultiplyCash = LoadFloatSetting(section, _T("mortmain_premium_char_multiply_cash"), 1.0f, fullPath);
	//;무제한 레어 용병의 캐쉬 판매가격 곱하기 >> cash x mortmain_rare_char_multiply_cash
	fMortmainRareCharMultiplyCash = LoadFloatSetting(section, _T("mortmain_rare_char_multiply_cash"), 1.0f, fullPath);

	CString strCashLimit;
	int iMaxCashLimit = LoadIntSetting(section, _T("MAX_CASH_LIMIT"), 0, fullPath);
	for(int w = 0;w < iMaxCashLimit;w++)
	{
		strCashLimit.Format(_T("CASH_LIMIT_%d"), w+1 );

		LimitData *pLimit = new LimitData;
		pLimit->m_iLimitDate = LoadIntSetting(strCashLimit, _T("LIMIT_DATE"), 24, fullPath);
		pLimit->m_fLimitPricePer  = LoadFloatSetting(strCashLimit, _T("LIMIT_PER"), 1.0f, fullPath);

		m_vCashLimitDataList.push_back( pLimit );
	}

	int iMaxPremiumCashLimit = LoadIntSetting(section, _T("MAX_PREMIUM_CASH_LIMIT"), 0, fullPath);
	for(int w = 0;w < iMaxPremiumCashLimit;w++)
	{
		strCashLimit.Format(_T("CASH_PREMIUM_LIMIT_%d"), w+1 );

		LimitData *pLimit = new LimitData;
		pLimit->m_iLimitDate = LoadIntSetting(strCashLimit, _T("LIMIT_DATE"), 24, fullPath);
		pLimit->m_fLimitPricePer  = LoadFloatSetting(strCashLimit, _T("LIMIT_PER"), 1.0f, fullPath);

		m_vPremiumCashLimitDataList.push_back( pLimit );
	}

	int iMaxRareCashLimit = LoadIntSetting(section, _T("MAX_RARE_CASH_LIMIT"), 0, fullPath);
	for(int w = 0;w < iMaxPremiumCashLimit;w++)
	{
		strCashLimit.Format(_T("CASH_RARE_LIMIT_%d"), w+1 );

		LimitData *pLimit = new LimitData;
		pLimit->m_iLimitDate = LoadIntSetting(strCashLimit, _T("LIMIT_DATE"), 24, fullPath);
		pLimit->m_fLimitPricePer  = LoadFloatSetting(strCashLimit, _T("LIMIT_PER"), 1.0f, fullPath);

		m_vRareCashLimitDataList.push_back( pLimit );
	}

	int iMaxClass = LoadIntSetting(section, _T("MAX_CLASS"), 0, fullPath);

	CString strClass;
	CString strName, strDeco_ini;
	for( int i=0 ; i<iMaxClass; i++ )
	{
		strClass.Format(_T("CLASS_%d"), i+1 );

		PriceData* pkPriceData = new PriceData;
		pkPriceData->m_iSetCode = i+1;
		pkPriceData->m_bActive = LoadIntSetting(strClass, _T("ACTIVE"), 1, fullPath);
		pkPriceData->m_iBuyCash = LoadIntSetting(strClass, _T("CASH"), 0, fullPath);
		pkPriceData->m_eType = (PriceType)LoadIntSetting(strClass, _T("TYPE"), 0, fullPath);


		if(0 >= pkPriceData->m_iBuyCash)
		{
			continue;
		}

		/*
		if(1 == m_iChange)
		{
			if(pkPriceData->m_eType == PT_PREMIUM)
			{
				pkPriceData->m_iBuyCash = pkPriceData->m_iBuyCash * m_iHero_Premium / 100;
			}
			else if(pkPriceData->m_eType == PT_RARE)
			{
				pkPriceData->m_iBuyCash = pkPriceData->m_iBuyCash * m_iHero_Rare / 100;
			}
			else
			{
				pkPriceData->m_iBuyCash = pkPriceData->m_iBuyCash * m_iHero_Normal / 100;
			}

//			ChangeIntSetting(strClass, _T("CASH"), pkPriceData->m_iBuyCash, fullPath);
		}

		int iRate = GetClassRate(pkPriceData->m_iSetCode);
		pkPriceData->m_iBuyCash = pkPriceData->m_iBuyCash * iRate / 100;
		if( iRate != 100 )
		{
//			ChangeIntSetting(strClass, _T("CASH"), pkPriceData->m_iBuyCash, fullPath);
		}
		*/

		m_MapItemClass.insert( std::map<int, PriceData*>::value_type( pkPriceData->m_iSetCode, pkPriceData) );

		CString strSaleValue; 
		enum { MAX_LOOP = 10, };
		for (int j = 0; j < MAX_LOOP ; j++)
		{
			strSaleValue.Format(_T("sale_start_date_%d"), j+1 );
			int iStartDate = LoadIntSetting(strClass, strSaleValue, 0, fullPath);
			if( iStartDate == 0 )
			{
				continue;
			}
	
			SaleInfo *pSaleInfo = new SaleInfo;

			pSaleInfo->Clear();

			pSaleInfo->m_eItemType = IT_CLASS;
			pSaleInfo->m_iType1   = pkPriceData->m_iSetCode;
			pSaleInfo->m_iType2   = j;

			pSaleInfo->m_iStartDate = iStartDate;

			strSaleValue.Format(_T("sale_end_date_%d"), j+1 );
			pSaleInfo->m_iEndDate = LoadIntSetting(strClass, strSaleValue, 0, fullPath);

			strSaleValue.Format(_T("sale_cash_%d"), j+1 );
			pSaleInfo->m_iCash = LoadIntSetting(strClass, strSaleValue, 0, fullPath);
			m_vSaleInfoVector.push_back( pSaleInfo );
		} 
	}
}


void CBillingInfoListDlg::LoadExtraItemINI(const TCHAR* file)
{
	m_mapExtraItem.clear();

	TCHAR buffer[512];
	TCHAR path[512];
	GetCurrentDirectory(sizeof(path), path);

	CString fullPath;
	fullPath.Format(_T("%s\\%s"), path, file);

	CString section =_T("common_info");

	int iMaxClass = LoadIntSetting(section, _T("random_machine_cnt"), 0, fullPath);

	CString strMachine;
	CString strName, strDeco_ini;
	for( int i=0 ; i<iMaxClass; i++ )
	{
		strMachine.Format(_T("random_machine%d"), i+1 );

		int iType = LoadIntSetting(strMachine, _T("type"), -1, fullPath);
		int iMachine_code = LoadIntSetting(strMachine, _T("machine_code"), -1, fullPath);
		int iPackage = LoadIntSetting(strMachine, _T("package"), -1, fullPath);
		int iNeed_peso1 = LoadIntSetting(strMachine, _T("need_peso1"), -1, fullPath);
		int iNeed_cash = LoadIntSetting(strMachine, _T("need_cash"), -1, fullPath);
		int iActive = LoadIntSetting(strMachine, _T("active"), -1, fullPath);
		strName = LoadStringSetting(strMachine, _T("name"), "", fullPath);

		if(0 >= iNeed_cash)
		{
			continue;
		}

		int iItemCnt = LoadIntSetting(strMachine, _T("item_cnt"), -1, fullPath);

		if(0>= iItemCnt)
		{
			continue;
		}

		ExtraItem* pkExItem = new ExtraItem;

		pkExItem->m_iType = iType;
		pkExItem->m_iMachine_code = iMachine_code;
		pkExItem->m_iPackage = iPackage;
		pkExItem->m_iBuyCash = iNeed_cash;
		pkExItem->m_iActive = iActive;
		pkExItem->m_strMachineName = strName;

		for( int i=0; i < iItemCnt; ++i )
		{
			CString strCode, strRate, strTrade;
			strCode.Format(_T("item%d_code"), i+1 );
			int iItemCode = LoadIntSetting(strMachine, strCode, -1, fullPath);

			strRate.Format(_T("item%d_rate"), i+1 );
			int iItemRate = LoadIntSetting(strMachine, strRate, -1, fullPath);

			RandomItem kRanItem;
			kRanItem.m_iItemCode = iItemCode;
			kRanItem.m_iRandomRate = iItemRate;
			pkExItem->m_vRandomItemList.push_back(kRanItem);
		}


		m_mapExtraItem.insert( mExtraItem::value_type( iMachine_code, pkExItem) );
/*//SALE 
		enum { MAX_LOOP = 10, };
		for (int j = 0; j < MAX_LOOP , j++)
		{
			g_SaleMgr.LoadINI( fullPath, strClass, ioSaleManager::IT_CLASS, pkPriceData->m_iSetCode, j, j );
		}
*/
	} 
}


void CBillingInfoListDlg::LoadPopupItemINI(const TCHAR* file)
{
	m_vPopupItemInfo.clear();

	TCHAR buffer[512];
	TCHAR path[512];
	GetCurrentDirectory(sizeof(path), path);

	CString fullPath;
	fullPath.Format(_T("%s\\%s"), path, file);

	CString section =_T("Common");

	int iMaxCount = LoadIntSetting(section, _T("Max"), 0, fullPath);

	CString strPopup;
	CString strName, strDeco_ini;
	for( int i=0 ; i<iMaxCount; i++ )
	{
		strPopup.Format(_T("Popup%d"), i+1 );

		int iActive = LoadIntSetting(strPopup, _T("Active"), -1, fullPath);
		int iIndex = LoadIntSetting(strPopup, _T("Index"), -1, fullPath);
		int iPresentType = LoadIntSetting(strPopup, _T("PresentType"), -1, fullPath);
		int iPresentValue1 = LoadIntSetting(strPopup, _T("PresentValue1"), -1, fullPath);
		int iPresentValue2 = LoadIntSetting(strPopup, _T("PresentValue2"), -1, fullPath);
		int iCash = LoadIntSetting(strPopup, _T("Cash"), -1, fullPath);
		int iPrevCash = LoadIntSetting(strPopup, _T("PrevCash"), -1, fullPath);

		int iDisCount = LoadIntSetting(strPopup, _T("Discount"), -1, fullPath);
		strName = LoadStringSetting(strPopup, _T("name"), "", fullPath);

		if(0 >= iCash)
		{
			continue;
		}

		sPopupItemInfo* pkPopupItem = new sPopupItemInfo;

		pkPopupItem->m_iActive = iActive;
		pkPopupItem->m_iIndex = iIndex;
		pkPopupItem->m_iPresentType = iPresentType;
		pkPopupItem->m_iPresentValue1 = iPresentValue1;
		pkPopupItem->m_iPresentValue2 = iPresentValue2;
		pkPopupItem->m_iCash = iCash;
		pkPopupItem->m_iPrevCash = iPrevCash;
		pkPopupItem->m_iDisCount = iDisCount;

		m_vPopupItemInfo.push_back(pkPopupItem);

/*//SALE 
		enum { MAX_LOOP = 10, };
		for (int j = 0; j < MAX_LOOP , j++)
		{
			g_SaleMgr.LoadINI( fullPath, strClass, ioSaleManager::IT_CLASS, pkPriceData->m_iSetCode, j, j );
		}
*/
	} 
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

int CBillingInfoListDlg::GetGoodsNoValue( int iLimitSeconds )
{
	for(vSoldierInfo::iterator iter = m_vSoldierInfo.begin(); iter != m_vSoldierInfo.end(); ++iter)
	{
		SoldierInfo &rkInfo = (*iter);
		if( rkInfo.m_iLimitSeconds == iLimitSeconds )
			return rkInfo.m_iGoodsNoValue;
	}

	return -1;
}

bool CBillingInfoListDlg::IsPackageItem( int iEtcItemShortType )
{
	for(DWORDVec::iterator iter = m_vPackageEtcItemShortTypeList.begin(); iter != m_vPackageEtcItemShortTypeList.end(); ++iter)
	{
	    if( iEtcItemShortType == ( *iter) )
			return true;
	}

	return false;
}

bool CBillingInfoListDlg::IsPackageItemExtra( int iExtraItemMachineCode )
{
	for(DWORDVec::iterator iter = m_vPackageExtraItemMachineCodeList.begin(); iter != m_vPackageExtraItemMachineCodeList.end(); ++iter)
	{
	    if( iExtraItemMachineCode == ( *iter) )
			return true;
	}

	return false;
}

int CBillingInfoListDlg::GetClassRate(int iClass)
{
	int iRate = 100;
	for(vHeroClassRate::iterator iter = m_vecHeroClassRate.begin(); iter != m_vecHeroClassRate.end(); ++iter)
	{
		HeroClassRate* pkHeroClassRate = (*iter);

		if(pkHeroClassRate->m_iStart <= iClass && pkHeroClassRate->m_iEnd >= iClass)
		{
			return pkHeroClassRate->m_iRate;
		}
	}

	return iRate;
}

 
CString CBillingInfoListDlg::GetGoodsName( int iGoodsNo)
{
	CString strValue;
	strValue.Empty();

	std::map<int, CString>::const_iterator iter = m_GoodsInfoMap.find( iGoodsNo );
	if( iter != m_GoodsInfoMap.end() )
	{
		strValue = iter->second;
	}

	return strValue;
}

DWORD CBillingInfoListDlg::ConvertYYMMDDHHMMToDate( WORD wYear, WORD wMonth, WORD wDay, WORD wHour, WORD wMinute )
{
	// 년도에서 2010년을 빼고 4바이트 날짜를 리턴
	enum { DEFAULT_YEAR = 2010,	DATE_YEAR_VALUE = 100000000, DATE_MONTH_VALUE= 1000000, DATE_DAY_VALUE =  10000, DATE_HOUR_VALUE = 100, };

	DWORD dwReturnDate = ((wYear - DEFAULT_YEAR) * DATE_YEAR_VALUE) +
						  (wMonth * DATE_MONTH_VALUE) + (wDay * DATE_DAY_VALUE) +
						  (wHour * DATE_HOUR_VALUE) + wMinute;
	return dwReturnDate;
}
bool CBillingInfoListDlg::IsCheckAlive( SYSTEMTIME st , DWORD dwStartDate, DWORD dwEndDate )
{
int iStartYear  = (dwStartDate/1000000);
	int iStartMonth = ((dwStartDate/10000)%100 );
	int iStartDay   = ((dwStartDate/100)%100);
	int iStartHour  = dwStartDate%100;

	int iEndYear  = (dwEndDate/1000000);
	int iEndMonth = ((dwEndDate/10000)%100 );
	int iEndDay   = ((dwEndDate/100)%100);
	int iEndHour  = dwEndDate%100;
	
	if( COMPARE( ConvertYYMMDDHHMMToDate( st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute ), 
				 ConvertYYMMDDHHMMToDate( iStartYear, iStartMonth, iStartDay, iStartHour, 0 ),
				 ConvertYYMMDDHHMMToDate( iEndYear, iEndMonth, iEndDay, iEndHour, 0 ) ) )
	{
		return true;
	}

	return false;
}

int CBillingInfoListDlg::GetCash( ItemType eItemType, int iType1, int iType2 )
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	for(vSaleInfoVector::iterator iter = m_vSaleInfoVector.begin(); iter != m_vSaleInfoVector.end(); ++iter)
	{
		SaleInfo *pInfo = *iter;
		if( !pInfo )
			continue;
		if( false == IsCheckAlive(st, pInfo->m_iStartDate, pInfo->m_iEndDate))
		{
			continue;
		}
		if( pInfo->m_eItemType != eItemType )
			continue;
		if( pInfo->m_iType1 != iType1 )
			continue;
		if( pInfo->m_iType2 != iType2 )
			continue;

		return pInfo->m_iCash;
	}

	return -1;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
void CBillingInfoListDlg::SaveEtcItemTotal()
{
	CString strfile = _T("Billing\\Item_EtcTotal.csv");
	
	// 폴더 생성
	TCHAR temp[512], folder[512];
	GetCurrentDirectory(_countof(temp), temp);
	sprintf_s(folder, _countof(folder), _T("%s\\Billing"), temp); 
	CreateDirectory(folder, NULL);
	
	cFileWriter* pkfile = new cFileWriter;
	if(!pkfile->Open(strfile))
	{
		delete pkfile;
		pkfile = NULL;
		return;
	}
	pkfile->Move(FILE_END, 0);

	std::map<int, CItemEtc*>::iterator iter;
	for( iter=MapItemEtc.begin() ; iter!=MapItemEtc.end() ; ++iter )
	{
		CItemEtc *pkItemEtc = iter->second;
		if(NULL == pkItemEtc)
		{
			continue;
		}

		for (int i = 0; i < pkItemEtc->EtcValueList.size(); i++)
		{
			CEtcValue* pkEtcValue = pkItemEtc->EtcValueList[i];
			if(NULL == pkEtcValue)
			{
				continue;
			}

			enum { 	USE_TYPE_CUT_VALUE = 1000000, ARRAY_POS = 1000000,};

			int iType = pkItemEtc->iType%USE_TYPE_CUT_VALUE;       // 사용타입을 제거한다. 

			int rdwGoodsNo = 300000000;                      // 3억은 권한
			rdwGoodsNo += iType;                         // AABBBBBB --> AA: 갯수나 시간 나타내는 array sp2_etcitem_info.ini value값에 대음됨 예) 확성기 10개, 20개 / BBBBBB: 종류 1~99999 : 권한iMaxCount 100000 ~ 199999 : 클래스별 권한 아이템
			rdwGoodsNo += ( i * ARRAY_POS );
		if( IsPackageItem( iType ) )
			rdwGoodsNo += 100000000;                 // 패키지 아이템은 4억대가 됨 ( 삼성측 요청 )
			int iChangeCash = (int)(pkEtcValue->iCash * m_fChangeCash);
			pkfile->WriteFormat(_T("Billing_ItemETC ,GoodsName,%s(%d), GoodsNo,%d, ItemCash,%d, ChangeCash,%d, ItemType,%d\r\n"), pkItemEtc->strName, pkEtcValue->iValue, rdwGoodsNo, pkEtcValue->iCash, iChangeCash, pkItemEtc->iType);
			rdwGoodsNo += 1000000000;
			pkfile->WriteFormat(_T("Billing_ItemETC Present ,GoodsName, [Present] %s(%d), GoodsNo,%d, ItemCash,%d, ChangeCash,%d, ItemType,%d\r\n"), pkItemEtc->strName, pkEtcValue->iValue, rdwGoodsNo, pkEtcValue->iCash, iChangeCash, pkItemEtc->iType);
		}
	}
	pkfile->Close();
}

void CBillingInfoListDlg::SaveEtcItemActive()
{
	CString strfile = _T("Billing\\Item_EtcTotalActive.csv");
	
	// 폴더 생성
	TCHAR temp[512], folder[512];
	GetCurrentDirectory(_countof(temp), temp);
	sprintf_s(folder, _countof(folder), _T("%s\\Billing"), temp); 
	CreateDirectory(folder, NULL);
	
	cFileWriter* pkfile = new cFileWriter;
	if(!pkfile->Open(strfile))
	{
		delete pkfile;
		pkfile = NULL;
		return;
	}
	pkfile->Move(FILE_END, 0);

	std::map<int, CItemEtc*>::iterator iter;
	for( iter=MapItemEtc.begin() ; iter!=MapItemEtc.end() ; ++iter )
	{
		CItemEtc *pkItemEtc = iter->second;
		if(NULL == pkItemEtc)
		{
			continue;
		}

		for (int i = 0; i < pkItemEtc->EtcValueList.size(); i++)
		{
			CEtcValue* pkEtcValue = pkItemEtc->EtcValueList[i];
			if(NULL == pkEtcValue)
			{
				continue;
			}

			if(0 == pkEtcValue->iActive)
			{
				continue;
			}

			enum { 	USE_TYPE_CUT_VALUE = 1000000, ARRAY_POS = 1000000,};

			int iType = pkItemEtc->iType%USE_TYPE_CUT_VALUE;       // 사용타입을 제거한다. 

			int rdwGoodsNo = 300000000;                      // 3억은 권한
			rdwGoodsNo += iType;                         // AABBBBBB --> AA: 갯수나 시간 나타내는 array sp2_etcitem_info.ini value값에 대음됨 예) 확성기 10개, 20개 / BBBBBB: 종류 1~99999 : 권한iMaxCount 100000 ~ 199999 : 클래스별 권한 아이템
			rdwGoodsNo += ( i * ARRAY_POS );
		if( IsPackageItem( iType ) )
			rdwGoodsNo += 100000000;                 // 패키지 아이템은 4억대가 됨 ( 삼성측 요청 )
			int iChangeCash = (int)(pkEtcValue->iCash * m_fChangeCash);
			pkfile->WriteFormat(_T("Billing_ItemETC ,GoodsName,%s(%d), GoodsNo,%d, ItemCash,%d, ChangeCash,%d, ItemType,%d\r\n"), pkItemEtc->strName, pkEtcValue->iValue, rdwGoodsNo, pkEtcValue->iCash, iChangeCash, pkItemEtc->iType);
			rdwGoodsNo += 1000000000;
			pkfile->WriteFormat(_T("Billing_ItemETC Present ,GoodsName, [Present] %s(%d), GoodsNo,%d, ItemCash,%d, ChangeCash,%d, ItemType,%d\r\n"), pkItemEtc->strName, pkEtcValue->iValue, rdwGoodsNo, pkEtcValue->iCash, iChangeCash, pkItemEtc->iType);
		}
	}
	pkfile->Close();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

void CBillingInfoListDlg::SaveDecoItemTotal()
{
	CString strfile = _T("Billing\\Item_DecoTotal.csv");
	
	// 폴더 생성
	TCHAR temp[512], folder[512];
	GetCurrentDirectory(_countof(temp), temp);
	sprintf_s(folder, _countof(folder), _T("%s\\Billing"), temp); 
	CreateDirectory(folder, NULL);
	
	cFileWriter* pkfile = new cFileWriter;
	if(!pkfile->Open(strfile))
	{
		delete pkfile;
		pkfile = NULL;
		return;
	}
	pkfile->Move(FILE_END, 0);

	int iSexSize = m_vSexList.size();
	for (int i = 0; i < iSexSize; i++)
	{
		SexDecoList kSexDecoList = m_vSexList[i];

		int iListSize = kSexDecoList.m_vList.size();
		for (int k = 0; k < iListSize; k++)
		{
			DecoList &kDecoList = kSexDecoList.m_vList[k];

			kDecoList.m_iPackageKeepPeso;

			int iSize = kDecoList.m_vList.size();
			for(int z =0; z < iSize; z++)
			{
				kDecoList.m_vList[z].m_iCash;


				int iDecoType  = kDecoList.m_iDecoType;
				int iKindred   = i;

				int rdwGoodsNo = 200000000;								// 2억은 치장
				rdwGoodsNo += kDecoList.m_vList[z].m_iDecoCode;         // 1~999 치장코드 : 얼굴이라면 웃는 얼굴, 화난 얼굴, 멍한 얼굴등
				rdwGoodsNo += ( iDecoType * 1000 );						// 1000~999000 치장타입 : 얼굴,머리,머리색,속옷등
				rdwGoodsNo += ( iKindred  * 1000000 );					// 1000000~99000000 종족:  / 0 : 남자 / 1 : 여자 / 2 : 엘프남자 /
				int iCash = kDecoList.m_vList[z].m_iCash;
				int iChangeCash = (int)(iCash * m_fChangeCash);

				pkfile->WriteFormat(_T("Billing_ItemDeco,GoodsName,%s, GoodsNo,%d, ItemCash,%d, ChangeCash,%d, ItemType,%d, ItemCode,%d\r\n"), kDecoList.m_vList[z].m_szName, rdwGoodsNo, iCash, iChangeCash, iDecoType, kDecoList.m_vList[z].m_iDecoCode);
				rdwGoodsNo += 1000000000;
				pkfile->WriteFormat(_T("Billing_ItemDeco Present ,GoodsName,[Present] %s, GoodsNo,%d, ItemCash,%d, ChangeCash,%d, ItemType,%d, ItemCode,%d\r\n"), kDecoList.m_vList[z].m_szName, rdwGoodsNo, iCash, iChangeCash, iDecoType, kDecoList.m_vList[z].m_iDecoCode);
			}
		}
	}
	pkfile->Close();
}


void CBillingInfoListDlg::SaveDecoItemActive()
{
	CString strfile = _T("Billing\\Item_DecoTotalActive.csv");
	
	// 폴더 생성
	TCHAR temp[512], folder[512];
	GetCurrentDirectory(_countof(temp), temp);
	sprintf_s(folder, _countof(folder), _T("%s\\Billing"), temp); 
	CreateDirectory(folder, NULL);
	
	cFileWriter* pkfile = new cFileWriter;
	if(!pkfile->Open(strfile))
	{
		delete pkfile;
		pkfile = NULL;
		return;
	}
	pkfile->Move(FILE_END, 0);

	int iSexSize = m_vSexList.size();
	for (int i = 0; i < iSexSize; i++)
	{
		SexDecoList kSexDecoList = m_vSexList[i];

		int iListSize = kSexDecoList.m_vList.size();
		for (int k = 0; k < iListSize; k++)
		{
			DecoList &kDecoList = kSexDecoList.m_vList[k];

			kDecoList.m_iPackageKeepPeso;

			int iSize = kDecoList.m_vList.size();
			for(int z =0; z < iSize; z++)
			{
				if( 0 == kDecoList.m_vList[z].m_bActive)
				{
					continue;
				}

				int iDecoType  = kDecoList.m_iDecoType;
				int iKindred   = i;

				int rdwGoodsNo = 200000000;								// 2억은 치장
				rdwGoodsNo += kDecoList.m_vList[z].m_iDecoCode;         // 1~999 치장코드 : 얼굴이라면 웃는 얼굴, 화난 얼굴, 멍한 얼굴등
				rdwGoodsNo += ( iDecoType * 1000 );						// 1000~999000 치장타입 : 얼굴,머리,머리색,속옷등
				rdwGoodsNo += ( iKindred  * 1000000 );					// 1000000~99000000 종족:  / 0 : 남자 / 1 : 여자 / 2 : 엘프남자 /
				int iCash = kDecoList.m_vList[z].m_iCash;
				int iChangeCash = (int)(iCash * m_fChangeCash);

				pkfile->WriteFormat(_T("Billing_ItemDeco,GoodsName,%s, GoodsNo,%d, ItemCash,%d, ChangeCash,%d, ItemType,%d, ItemCode,%d\r\n"), kDecoList.m_vList[z].m_szName, rdwGoodsNo, iCash, iChangeCash, iDecoType, kDecoList.m_vList[z].m_iDecoCode);
				rdwGoodsNo += 1000000000;
				pkfile->WriteFormat(_T("Billing_ItemDeco Present ,GoodsName,[Present] %s, GoodsNo,%d, ItemCash,%d, ChangeCash,%d, ItemType,%d, ItemCode,%d\r\n"), kDecoList.m_vList[z].m_szName, rdwGoodsNo, iCash, iChangeCash, iDecoType, kDecoList.m_vList[z].m_iDecoCode);
			}
		}
	}
	pkfile->Close();
}


void CBillingInfoListDlg::SaveSoldierItemTotal()
{
	CString strfile = _T("Billing\\Item_SoldierTotalActive.csv");
	
	// 폴더 생성
	TCHAR temp[512], folder[512];
	GetCurrentDirectory(_countof(temp), temp);
	sprintf_s(folder, _countof(folder), _T("%s\\Billing"), temp); 
	CreateDirectory(folder, NULL);
	
	cFileWriter* pkfile = new cFileWriter;
	if(!pkfile->Open(strfile))
	{
		delete pkfile;
		pkfile = NULL;
		return;
	}
	pkfile->Move(FILE_END, 0);

	std::map<int, PriceData*>::iterator iter;
	for( iter=m_MapItemClass.begin() ; iter!=m_MapItemClass.end() ; ++iter )
	{
		PriceData *pkItemHero = iter->second;
		if(NULL == pkItemHero)
		{
			continue;
		}
		
		if(0 == pkItemHero->m_bActive)
		{
			continue;
		}

		int iMaxLimit = 0;	
		if(pkItemHero->m_eType == PT_PREMIUM)
		{
			iMaxLimit = m_vPremiumCashLimitDataList.size();
		}
		else if(pkItemHero->m_eType == PT_RARE)
		{
			iMaxLimit = m_vRareCashLimitDataList.size();
		}
		else
		{
			iMaxLimit = m_vCashLimitDataList.size();
		}

		// CharPeriodType 참조
		for(int z = 0; z < CPT_MAX; z++)
		{
			for(int j = 0;j < iMaxLimit;j++)
			{
				int iPeriodType = (int)CPT_TIME;
				int iLimitDate = m_vCashLimitDataList[j]->m_iLimitDate;
				int iBuyCash = pkItemHero->m_iBuyCash;
				if( z == 1)	// 시간제
				{
					iLimitDate = 0;
				}

				LimitData *pLimit = NULL;
				if( pkItemHero->m_eType == PT_PREMIUM )
				{
					pLimit = m_vPremiumCashLimitDataList[j];
				}
				else if( pkItemHero->m_eType == PT_RARE )
				{
					pLimit = m_vRareCashLimitDataList[j];
				}
				else
				{
					pLimit = m_vCashLimitDataList[j];
				}

				if( pLimit->m_iLimitDate == iLimitDate )
				{
					int iReturnCash = -1;
					iReturnCash = GetCash( IT_CLASS, pkItemHero->m_iSetCode, j );
					if( iReturnCash != -1 )
					{
						iBuyCash = iReturnCash;
					}
					else
					{
						iBuyCash = iBuyCash * pLimit->m_fLimitPricePer;
					}
				}
				else
				{
					if(z == 0 )
					{
						continue;
					}
					iPeriodType = (int)CPT_MORTMAIN;

					int iReturnCash = -1;
					iReturnCash = GetCash( IT_CLASS, pkItemHero->m_iSetCode, 9 );
					if( iReturnCash != -1 )
					{
						iBuyCash = iReturnCash;
					}
					else
					{
						if( pkItemHero->m_eType == PT_PREMIUM )
						{
							iBuyCash = iBuyCash * fMortmainPremiumCharMultiplyCash;
						}
						else if( pkItemHero->m_eType == PT_RARE )
						{
							iBuyCash = iBuyCash * fMortmainRareCharMultiplyCash;
						}
						else
						{
							iBuyCash = iBuyCash * fMortmainCharMultiplyCash;
						}
					}
				}

				int iGoodsNoValue = 0;
				float fCashMultiply = 0.0f;
				if( iPeriodType == CPT_TIME )
				{
					iGoodsNoValue = GetGoodsNoValue( iLimitDate );
				}
				else if( iPeriodType == CPT_MORTMAIN )
				{
					iGoodsNoValue = m_iMortmainCharGoodsNoValue;
				}

				if( iGoodsNoValue <= 0 )
				{
					continue;
				}

				int rdwGoodsNo = 100000000;                    // 1억은 용병
				rdwGoodsNo +=   pkItemHero->m_iSetCode;                // 1~9999            :클래스자리
				rdwGoodsNo += ( iGoodsNoValue * 10000 );   // 10000 ~ 99990000  :구매시간자리

				CString strName = GetGoodsName( rdwGoodsNo);
				if(0 == strName.GetLength())
				{
					continue;
				}

				int iChangeCash = (int)(iBuyCash * m_fChangeCash);
				pkfile->WriteFormat(_T("Billing_ItemSoldier ,GoodsName,%s, GoodsNo,%d, ItemCash,%d, ChangeCash,%d, Class,%d, LimitSecond,%d, PriceType,%d\r\n"), strName, rdwGoodsNo, iBuyCash, iChangeCash, pkItemHero->m_iSetCode, iLimitDate, pkItemHero->m_eType);
				rdwGoodsNo += 1000000000;
				pkfile->WriteFormat(_T("Billing_ItemSoldier Present ,GoodsName,[Present] %s, GoodsNo,%d, ItemCash,%d, ChangeCash,%d, Class,%d, LimitSecond,%d, PriceType,%d\r\n"), strName, rdwGoodsNo, iBuyCash, iChangeCash, pkItemHero->m_iSetCode, iLimitDate, pkItemHero->m_eType);
			}
		}
	}

	pkfile->Close();
}

void CBillingInfoListDlg::SaveSoldierItemActive()
{
	CString strfile = _T("Billing\\Item_SoldierTotalActive.csv");
	
	// 폴더 생성
	TCHAR temp[512], folder[512];
	GetCurrentDirectory(_countof(temp), temp);
	sprintf_s(folder, _countof(folder), _T("%s\\Billing"), temp); 
	CreateDirectory(folder, NULL);
	
	cFileWriter* pkfile = new cFileWriter;
	if(!pkfile->Open(strfile))
	{
		delete pkfile;
		pkfile = NULL;
		return;
	}
	pkfile->Move(FILE_END, 0);

	std::map<int, PriceData*>::iterator iter;
	for( iter=m_MapItemClass.begin() ; iter!=m_MapItemClass.end() ; ++iter )
	{
		PriceData *pkItemHero = iter->second;
		if(NULL == pkItemHero)
		{
			continue;
		}
		
		if(0 == pkItemHero->m_bActive)
		{
			continue;
		}

		int iMaxLimit = 0;	
		if(pkItemHero->m_eType == PT_PREMIUM)
		{
			iMaxLimit = m_vPremiumCashLimitDataList.size();
		}
		else if(pkItemHero->m_eType == PT_RARE)
		{
			iMaxLimit = m_vRareCashLimitDataList.size();
		}
		else
		{
			iMaxLimit = m_vCashLimitDataList.size();
		}

		// CharPeriodType 참조
		for(int z = 0; z < CPT_MAX; z++)
		{
			for(int j = 0;j < iMaxLimit;j++)
			{
				int iPeriodType = (int)CPT_TIME;
				int iLimitDate = m_vCashLimitDataList[j]->m_iLimitDate;
				int iBuyCash = pkItemHero->m_iBuyCash;
				if( z == 1)	// 시간제
				{
					iLimitDate = 0;
				}

				LimitData *pLimit = NULL;
				if( pkItemHero->m_eType == PT_PREMIUM )
				{
					pLimit = m_vPremiumCashLimitDataList[j];
				}
				else if( pkItemHero->m_eType == PT_RARE )
				{
					pLimit = m_vRareCashLimitDataList[j];
				}
				else
				{
					pLimit = m_vCashLimitDataList[j];
				}

				if( pLimit->m_iLimitDate == iLimitDate )
				{
					int iReturnCash = -1;
					iReturnCash = GetCash( IT_CLASS, pkItemHero->m_iSetCode, j );
					if( iReturnCash != -1 )
					{
						iBuyCash = iReturnCash;
					}
					else
					{
						iBuyCash = iBuyCash * pLimit->m_fLimitPricePer;
					}
				}
				else
				{
					if(z == 0 )
					{
						continue;
					}
					iPeriodType = (int)CPT_MORTMAIN;

					int iReturnCash = -1;
					iReturnCash = GetCash( IT_CLASS, pkItemHero->m_iSetCode, 9 );
					if( iReturnCash != -1 )
					{
						iBuyCash = iReturnCash;
					}
					else
					{
						if( pkItemHero->m_eType == PT_PREMIUM )
						{
							iBuyCash = iBuyCash * fMortmainPremiumCharMultiplyCash;
						}
						else if( pkItemHero->m_eType == PT_RARE )
						{
							iBuyCash = iBuyCash * fMortmainRareCharMultiplyCash;
						}
						else
						{
							iBuyCash = iBuyCash * fMortmainCharMultiplyCash;
						}
					}
				}

				int iGoodsNoValue = 0;
				float fCashMultiply = 0.0f;
				if( iPeriodType == CPT_TIME )
				{
					iGoodsNoValue = GetGoodsNoValue( iLimitDate );
				}
				else if( iPeriodType == CPT_MORTMAIN )
				{
					iGoodsNoValue = m_iMortmainCharGoodsNoValue;
				}

				if( iGoodsNoValue <= 0 )
				{
					continue;
				}

				int rdwGoodsNo = 100000000;                    // 1억은 용병
				rdwGoodsNo +=   pkItemHero->m_iSetCode;                // 1~9999            :클래스자리
				rdwGoodsNo += ( iGoodsNoValue * 10000 );   // 10000 ~ 99990000  :구매시간자리

				CString strName = GetGoodsName( rdwGoodsNo);
				if(0 == strName.GetLength())
				{
					continue;
				}

				int iChangeCash = (int)(iBuyCash * m_fChangeCash);
				pkfile->WriteFormat(_T("Billing_ItemSoldier ,GoodsName,%s, GoodsNo,%d, ItemCash,%d, ChangeCash,%d, Class,%d, LimitSecond,%d, PriceType,%d\r\n"), strName, rdwGoodsNo, iBuyCash, iChangeCash, pkItemHero->m_iSetCode, iLimitDate, pkItemHero->m_eType);
				rdwGoodsNo += 1000000000;
				pkfile->WriteFormat(_T("Billing_ItemSoldier Present ,GoodsName,[Present] %s, GoodsNo,%d, ItemCash,%d, ChangeCash,%d, Class,%d, LimitSecond,%d, PriceType,%d\r\n"), strName, rdwGoodsNo, iBuyCash, iChangeCash, pkItemHero->m_iSetCode, iLimitDate, pkItemHero->m_eType);
			}
		}
	}

	pkfile->Close();
}

void CBillingInfoListDlg::SaveExtraItemTotal()
{
	CString strfile = _T("Billing\\Item_ExtraTotal.csv");
	
	// 폴더 생성
	TCHAR temp[512], folder[512];
	GetCurrentDirectory(_countof(temp), temp);
	sprintf_s(folder, _countof(folder), _T("%s\\Billing"), temp); 
	CreateDirectory(folder, NULL);
	
	cFileWriter* pkfile = new cFileWriter;
	if(!pkfile->Open(strfile))
	{
		delete pkfile;
		pkfile = NULL;
		return;
	}
	pkfile->Move(FILE_END, 0);

	mExtraItem::iterator iter;
	for( iter=m_mapExtraItem.begin() ; iter!=m_mapExtraItem.end() ; ++iter )
	{
		ExtraItem *pkItemExtra = iter->second;
		if(NULL == pkItemExtra)
		{
			continue;
		}

		int rdwGoodsNo = 500000000;                     // 5억은 장비 보급
		rdwGoodsNo +=   pkItemExtra->m_iMachine_code;  
		if( IsPackageItemExtra( pkItemExtra->m_iMachine_code) )
		{
			rdwGoodsNo -= 90000000;                 // 패키지 아이템은 4억1천만대 됨 
		}

		CString strName = GetGoodsName( rdwGoodsNo);
		if(0 == strName.GetLength())
		{
			continue;
		}

		int iBuyCash = pkItemExtra->m_iBuyCash;
		int iChangeCash = (int)(iBuyCash * m_fChangeCash);
		pkfile->WriteFormat(_T("Billing_ItemExtra ,GoodsName,%s, GoodsNo,%d, ItemCash,%d, ChangeCash,%d, Machine_code,%d \r\n"), strName, rdwGoodsNo, iBuyCash, iChangeCash, pkItemExtra->m_iMachine_code);
		rdwGoodsNo += 1000000000;
		pkfile->WriteFormat(_T("Billing_ItemExtra Present ,GoodsName,[Present] %s, GoodsNo,%d, ItemCash,%d, ChangeCash,%d, Machine_code,%d \r\n"), strName, rdwGoodsNo, iBuyCash, iChangeCash, pkItemExtra->m_iMachine_code);
	}
	pkfile->Close();
}

void CBillingInfoListDlg::SaveExtraItemActive()
{
	CString strfile = _T("Billing\\Item_ExtraTotalActive.csv");
	
	// 폴더 생성
	TCHAR temp[512], folder[512];
	GetCurrentDirectory(_countof(temp), temp);
	sprintf_s(folder, _countof(folder), _T("%s\\Billing"), temp); 
	CreateDirectory(folder, NULL);
	
	cFileWriter* pkfile = new cFileWriter;
	if(!pkfile->Open(strfile))
	{
		delete pkfile;
		pkfile = NULL;
		return;
	}
	pkfile->Move(FILE_END, 0);

	mExtraItem::iterator iter;
	for( iter=m_mapExtraItem.begin() ; iter!=m_mapExtraItem.end() ; ++iter )
	{
		ExtraItem *pkItemExtra = iter->second;
		if(NULL == pkItemExtra)
		{
			continue;
		}

		if(0 == pkItemExtra->m_iActive)
		{
			continue;
		}

		int rdwGoodsNo = 500000000;                     // 5억은 장비 보급
		rdwGoodsNo +=   pkItemExtra->m_iMachine_code;  
		if( IsPackageItemExtra( pkItemExtra->m_iMachine_code) )
		{
			rdwGoodsNo -= 90000000;                 // 패키지 아이템은 4억1천만대 됨 
		}

		CString strName = GetGoodsName( rdwGoodsNo);
		if(0 == strName.GetLength())
		{
			continue;
		}

		int iBuyCash = pkItemExtra->m_iBuyCash;
		int iChangeCash = (int)(iBuyCash * m_fChangeCash);
		pkfile->WriteFormat(_T("Billing_ItemExtra ,GoodsName,%s, GoodsNo,%d, ItemCash,%d, ChangeCash,%d, Machine_code,%d \r\n"), strName, rdwGoodsNo, iBuyCash, iChangeCash, pkItemExtra->m_iMachine_code);
		rdwGoodsNo += 1000000000;
		pkfile->WriteFormat(_T("Billing_ItemExtra Present ,GoodsName,[Present] %s, GoodsNo,%d, ItemCash,%d, ChangeCash,%d, Machine_code,%d \r\n"), strName, rdwGoodsNo, iBuyCash, iChangeCash, pkItemExtra->m_iMachine_code);
	}
	pkfile->Close();
}

void CBillingInfoListDlg::SavePopupItemTotal()
{
	CString strfile = _T("Billing\\Item_PopupTotal.csv");
	
	// 폴더 생성
	TCHAR temp[512], folder[512];
	GetCurrentDirectory(_countof(temp), temp);
	sprintf_s(folder, _countof(folder), _T("%s\\Billing"), temp); 
	CreateDirectory(folder, NULL);
	
	cFileWriter* pkfile = new cFileWriter;
	if(!pkfile->Open(strfile))
	{
		delete pkfile;
		pkfile = NULL;
		return;
	}
	pkfile->Move(FILE_END, 0);

	int iPopupSize = m_vPopupItemInfo.size();
	for (int i = 0; i < iPopupSize; i++)
	{
		sPopupItemInfo *pkItemPopup = m_vPopupItemInfo[i];
		if(NULL == pkItemPopup)
		{
			continue;
		}

		int rdwGoodsNo = 700000000;                     // 5억은 장비 보급
		rdwGoodsNo +=   pkItemPopup->m_iIndex;  

		CString strName = GetGoodsName( rdwGoodsNo);
		if(0 == strName.GetLength())
		{
			continue;
		}

		int iBuyCash = pkItemPopup->m_iCash;
		int iChangeCash = (int)(iBuyCash * m_fChangeCash);
		pkfile->WriteFormat(_T("Billing_ItemPopup ,GoodsName,%s, GoodsNo,%d, ItemCash,%d, ChangeCash,%d, Machine_code,%d \r\n"), strName, rdwGoodsNo, iBuyCash, iChangeCash, pkItemPopup->m_iIndex);
	}
	pkfile->Close();
}

void CBillingInfoListDlg::SavePopupItemActive()
{
	CString strfile = _T("Billing\\Item_PopupTotalActive.csv");
	
	// 폴더 생성
	TCHAR temp[512], folder[512];
	GetCurrentDirectory(_countof(temp), temp);
	sprintf_s(folder, _countof(folder), _T("%s\\Billing"), temp); 
	CreateDirectory(folder, NULL);
	
	cFileWriter* pkfile = new cFileWriter;
	if(!pkfile->Open(strfile))
	{
		delete pkfile;
		pkfile = NULL;
		return;
	}
	pkfile->Move(FILE_END, 0);

	int iPopupSize = m_vPopupItemInfo.size();
	for (int i = 0; i < iPopupSize; i++)
	{
		sPopupItemInfo *pkItemPopup = m_vPopupItemInfo[i];
		if(NULL == pkItemPopup)
		{
			continue;
		}

		if(0 == pkItemPopup->m_iActive)
		{
			continue;
		}

		int rdwGoodsNo = 700000000;                     // 5억은 장비 보급
		rdwGoodsNo +=   pkItemPopup->m_iIndex;  

		CString strName = GetGoodsName( rdwGoodsNo);
		if(0 == strName.GetLength())
		{
			continue;
		}

		int iBuyCash = pkItemPopup->m_iCash;
		int iChangeCash = (int)(iBuyCash * m_fChangeCash);
		pkfile->WriteFormat(_T("Billing_ItemPopup ,GoodsName,%s, GoodsNo,%d, ItemCash,%d, ChangeCash,%d, Machine_code,%d \r\n"), strName, rdwGoodsNo, iBuyCash, iChangeCash, pkItemPopup->m_iIndex);
	}
	pkfile->Close();
}


void CBillingInfoListDlg::OnBnClickedEtcitem()
{
	text_total.SetWindowText("Billing Etc Save");
	SaveEtcItemTotal();
	SaveEtcItemActive();
	text_total.SetWindowText("Billing Etc  Save End");
}


void CBillingInfoListDlg::OnBnClickedDecoitem()
{
	text_total.SetWindowText("Billing Deco Save");
	SaveDecoItemTotal();
	SaveDecoItemActive();
	text_total.SetWindowText("Billing Deco Save End");
}


void CBillingInfoListDlg::OnBnClickedSoldier()
{
	text_total.SetWindowText("Billing Soldier Save");
	SaveSoldierItemTotal();
	SaveSoldierItemActive();
	text_total.SetWindowText("Billing Soldier Save End");
}


void CBillingInfoListDlg::OnBnClickedExtraitem()
{
	text_total.SetWindowText("Billing Extra Save");
	SaveExtraItemTotal();
	SaveExtraItemActive();
	text_total.SetWindowText("Billing Extra Save End");
}


void CBillingInfoListDlg::OnBnClickedPopup()
{
	text_total.SetWindowText("Billing Popup Save");
	SavePopupItemTotal();
	SavePopupItemActive();
	text_total.SetWindowText("Billing Popup Save End");
}


void CBillingInfoListDlg::OnBnClickedTotal()
{
	text_total.SetWindowText("Billing Total Etc Save");
	SaveEtcItemTotal();
	SaveEtcItemActive();
	text_total.SetWindowText("Billing Total Deco Save");
	SaveDecoItemTotal();
	SaveDecoItemActive();
	text_total.SetWindowText("Billing Total Soldier Save");
	SaveSoldierItemTotal();
	SaveSoldierItemActive();
	text_total.SetWindowText("Billing Total Extra Save");
	SaveExtraItemTotal();
	SaveExtraItemActive();
	text_total.SetWindowText("Billing Total Popup Save");
	SavePopupItemTotal();
	SavePopupItemActive();
	text_total.SetWindowText("Billing Total Save End");
}


void CBillingInfoListDlg::OnBnClickedItemtotal()
{
	GetDlgItem(IDC_ITEMTOTAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_ITEMETC)->EnableWindow(FALSE);
	GetDlgItem(IDC_ITEMDECO)->EnableWindow(FALSE);
	GetDlgItem(IDC_ITEMSOLDiER)->EnableWindow(FALSE);
	GetDlgItem(IDC_ITEMEXTRA)->EnableWindow(FALSE);
	GetDlgItem(IDC_ITEM_POPUP)->EnableWindow(FALSE);

	text_total.SetWindowText("Item Total Etc Loading");
	LoadEtcItemINI("config/sp2_etcitem_info.ini");
	text_total.SetWindowText("Item Total Deco Loading");
	LoadDecoItemINI(_T("config/sp2_deco_sex.ini"));
	text_total.SetWindowText("Item Total Soldier Loading");
	LoadSoldierItemINI(_T("config/sp2_item_price.ini"));
	text_total.SetWindowText("Item Total Extra Loading");
	LoadExtraItemINI(_T("config/sp2_extraitem_info.ini"));
	text_total.SetWindowText("Item Total Popup Loading");
	LoadPopupItemINI(_T("config/sp2_popupstore.ini"));
	text_total.SetWindowText("Item Total Loading End");

	GetDlgItem(IDC_ETCITEM)->EnableWindow(TRUE);
	GetDlgItem(IDC_DECOITEM)->EnableWindow(TRUE);
	GetDlgItem(ID_SOLDIER)->EnableWindow(TRUE);
	GetDlgItem(IDC_EXTRAITEM)->EnableWindow(TRUE);
	GetDlgItem(IDC_POPUP)->EnableWindow(TRUE);
	GetDlgItem(IDC_TOTAL)->EnableWindow(TRUE);
}


void CBillingInfoListDlg::OnBnClickedItemetc()
{
	GetDlgItem(IDC_ITEMETC)->EnableWindow(FALSE);
	text_total.SetWindowText("Item Etc Loading");
	LoadEtcItemINI("config/sp2_etcitem_info.ini");
	text_total.SetWindowText("Item Etc Loading End");
	GetDlgItem(IDC_ETCITEM)->EnableWindow(TRUE);
}


void CBillingInfoListDlg::OnBnClickedItemdeco()
{
	GetDlgItem(IDC_ITEMTOTAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_ITEMDECO)->EnableWindow(FALSE);
	text_total.SetWindowText("Item Deco Loading");
	LoadDecoItemINI(_T("config/sp2_deco_sex.ini"));
	text_total.SetWindowText("Item Deco Loading End");
	GetDlgItem(IDC_DECOITEM)->EnableWindow(TRUE);
}


void CBillingInfoListDlg::OnBnClickedItemsoldier()
{
	GetDlgItem(IDC_ITEMTOTAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_ITEMSOLDiER)->EnableWindow(FALSE);
	text_total.SetWindowText("Item Soldier Loading");
	LoadSoldierItemINI(_T("config/sp2_item_price.ini"));
	text_total.SetWindowText("Item Soldier Loading End");
	GetDlgItem(ID_SOLDIER)->EnableWindow(TRUE);
}


void CBillingInfoListDlg::OnBnClickedItemextra()
{
	GetDlgItem(IDC_ITEMTOTAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_ITEMEXTRA)->EnableWindow(FALSE);
	text_total.SetWindowText("Item Extra Loading");
	LoadExtraItemINI(_T("config/sp2_extraitem_info.ini"));
	text_total.SetWindowText("Item Extra Loading End");
	GetDlgItem(IDC_EXTRAITEM)->EnableWindow(TRUE);
}


void CBillingInfoListDlg::OnBnClickedItemPopup()
{
	GetDlgItem(IDC_ITEMTOTAL)->EnableWindow(FALSE);
	GetDlgItem(IDC_ITEM_POPUP)->EnableWindow(FALSE);
	text_total.SetWindowText("Item Popup Loading");
	LoadPopupItemINI(_T("config/sp2_popupstore.ini"));
	text_total.SetWindowText("Item Popup Loading End");
	GetDlgItem(IDC_POPUP)->EnableWindow(TRUE);
}
