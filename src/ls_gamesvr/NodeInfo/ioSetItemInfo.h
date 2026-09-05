

#ifndef _ioSetItemInfo_h_
#define _ioSetItemInfo_h_

class ioINILoader;

class ioSetItemInfo
{
public:
	enum NeedLevelType
	{
		NLT_NONE     = -1,
		NLT_GRADE    =  1, // 계급
		NLT_BATTLE   =  2, // 전투
		NLT_TRAINING =  3, // 훈련
		NLT_AWARD    =  4, // 수상
	};

	enum PackageType
	{
		PT_PREMIUM = 0,
		PT_NORMAL  = 1,
		PT_RARE    = 2,
	};

	enum InvenDecoType
	{
		IDT_FACE,
		IDT_SKIN,
		IDT_HAIR,
		IDT_HAIR_COLOR,
		IDT_UNDERWEAR,
		IDT_MAX,
	};

private:
	typedef struct tagNeedLevelInfo
	{
		NeedLevelType m_eNeedLevelType;
		int           m_iNeedLevel;

		tagNeedLevelInfo()
		{
			m_eNeedLevelType = NLT_NONE;
			m_iNeedLevel     = -1;
		}
	}NeedLevelInfo;
	typedef std::vector< NeedLevelInfo > vNeedLevelInfo;

	struct PresetDeco
	{
		int m_iFace;
		int m_iHair;
		int m_iSkinColor;
		int m_iHairColor;
		int m_iUnderwear;

		PresetDeco()
		{
			m_iFace = 0;
			m_iHair = 0;
			m_iSkinColor = 0;
			m_iHairColor = 0;
			m_iUnderwear = 0;
		}
	};

private:
	friend class ioSetItemInfoManager;

protected:
	ioHashString m_SetName;
	DWORD m_dwSetCode;
	DWORD m_dwRequireRightCode;
	PackageType  m_ePackageType;

	DWORDVec m_vSetItemCodeList;
	vNeedLevelInfo m_vNeedLevelInfoList;

	int m_iMortmainSellValue;
	int m_iPresetSex;
	PresetDeco m_ManPresetDeco;
	PresetDeco m_WomanPresetDeco;

public:
	void LoadInfo( int iIndex );
	void LoadInfo( ioINILoader &rkLoader );
	void LoadPreset( const char *szDeco, PresetDeco &stPreset, ioINILoader &rkLoader );

public:
	inline DWORD GetSetCode() const { return m_dwSetCode; }
	inline const ioHashString& GetName() const { return m_SetName; }
	inline DWORD GetRequireRightCode() const { return m_dwRequireRightCode; }
	inline const DWORDVec& GetSetItemList() const { return m_vSetItemCodeList; }
	inline PackageType  GetPackageType() const { return m_ePackageType; }
	inline int GetMortainSellValue() const	{ return m_iMortmainSellValue; }

public:
	int GetSetItemCnt() const;
	DWORD GetSetItemCode( int iIndex ) const;

	int GetNeedLevelInfoListCnt() const;
	NeedLevelType GetNeedLevelType( int iIndex ) const;
	int GetNeedLevel( int iIndex ) const;

	int GetPresetSex() const { return m_iPresetSex; }
	int GetPresetDecoCode( int iGendedrType, int iDecoParts ) const;
	int GetCheckValue( int iValue );

	void SetItemCodeList( DWORDVec vSetItemCodeList );

public:
	ioSetItemInfo();
	virtual ~ioSetItemInfo();
};

#endif
