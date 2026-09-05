#pragma once

class ioINILoader;

#define MAX_SET_LEVEL 3
#define ORDER_DEFAULT 999999999


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

	enum CharAttackType
	{
		CAT_NONE        = -1,
		CAT_SHORT_RANGE =  1,
		CAT_LONG_RANGE  =  2,
		CAT_MAGIC       =  3,
		CAT_SPECIAL     =  4,
		CAT_PREMIUM     =  5, // 공격타입은 아니지만 상점에서 용병을 구분하기 위해서
	};

	enum CharSexType
	{
		CST_ALL,		// 남/여 모두
		CST_MALE,		// 남자만
		CST_FEMALE,		// 여자만
	};

	enum PackageType
	{
		PT_PREMIUM = 0,
		PT_NORMAL  = 1,
		PT_RARE    = 2,
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

private:
	friend class ioSetItemInfoManager;

protected:
	bool			m_bIsDummyInfo;

	ioHashString	m_SetName;
	int				m_iVoiceType;

	DWORD			m_dwSetCode;	
	DWORD			m_dwRequireRightCode;

	PackageType		m_ePackageType;

	CEncrypt<int>	m_CharSexType;				// CharSexType

	int				m_iItemManual;				// sp2_item_manual.ini 의 [Manual숫자] 값
	int				m_iShopOrder;				// 상점에서 정렬 순서
	int				m_iRecommendedShopOrder;	// 상점의 금주의 상품에서의 정렬 순서
	int				m_iShopMarkType;			// SHOP_MARK_TYPE_NONE = 0, SHOP_MARK_TYPE_SALE = 1, SHOP_MARK_TYPE_NEW = 2, SHOP_MARK_TYPE_HIT = 3, SHOP_MARK_TYPE_EVENT = 4,
		
	
	DWORDVec		m_vSetItemCodeList;
	ioHashString	m_SetItemSkillName;

	DWORD			m_dwSetFocusColor;
	vNeedLevelInfo	m_vNeedLevelInfoList;

	CharAttackType	m_eCharAttackType;
	ioHashString	m_szSubIconName;
		
	DWORD			m_dwLimitDate;				// m_iShopMarkType = SHOP_MARK_TYPE_DATE 일때 판매 시간, 판매 기간이 종료되면 상품 표시를 하지 않음

protected:
	// Package Item Default
	struct PackageDeco
	{
		int m_iFaceDeco;
		int m_iHairDeco;
		int m_iHairColorDeco;
		int m_iSkinColorDeco;
		int m_iUnderwearDeco;
		PackageDeco()
		{
			m_iFaceDeco = m_iHairDeco = m_iHairColorDeco = 1;
			m_iSkinColorDeco = m_iUnderwearDeco = 1;
		}
	};
	PackageDeco m_PackageManDef;
	PackageDeco m_PackageWomanDef;

public:
	void LoadInfo( ioINILoader &rkLoader );

public:
	inline DWORD GetSetCode() const { return m_dwSetCode; }
	inline const ioHashString& GetName() const { return m_SetName; }
	inline int GetVoiceType() const { return m_iVoiceType; }
	inline DWORD GetRequireRightCode() const { return m_dwRequireRightCode; }
	inline PackageType  GetPackageType() const { return m_ePackageType; }
	inline int GetItemManual() const { return m_iItemManual; }
	inline const ioHashString& GetSetItemSkillName() const { return m_SetItemSkillName; }
	inline DWORD GetSetFocusColor() const { return m_dwSetFocusColor; }
	inline const ioHashString& GetSubIconName() const { return m_szSubIconName; }
	inline int GetShopOrder() const { return m_iShopOrder; }
	inline int GetRecommendedShopOrder() const { return m_iRecommendedShopOrder; }
	inline int GetShopMarkType() const { return m_iShopMarkType; }
	inline int GetCharSexType() const { return m_CharSexType; }
	inline bool IsDummyInfo() const {return m_bIsDummyInfo; }

	ConstItemVector GetSetItemList() const;

	inline void SetShopOrder( int iShopOrder ) { m_iShopOrder = iShopOrder; }
	inline void SetRecommendedShopOrder( int iRecommendedShopOrder ) { m_iRecommendedShopOrder = iRecommendedShopOrder; }
	inline void SetShopMarkType( int iShopMarkType ) { m_iShopMarkType = iShopMarkType; }

public:
	int GetNeedLevelInfoListCnt() const;
	NeedLevelType GetNeedLevelType( int iIndex ) const;
	int GetNeedLevel( int iIndex ) const;
	int GetPackageDefaultDecoCode( int iDecoType, int iSex ) const;

public:
	int GetClassType() const;
	int GetSetItemCnt() const;
	DWORD GetSetItemCode( int iIndex ) const;

	CharAttackType GetCharAttackType() const { return m_eCharAttackType; }

public:
	DWORD GetLimitDate() const;

protected:
	ioSetItemInfo();
	virtual ~ioSetItemInfo();
};

