#pragma once

class ioWnd;
class ioINILoader;
class ioUIRenderImage;

class ioItemRight
{
public:
	enum RightType
	{
		RT_UNKOWN,
		RT_HIRE_TROOPS,
		RT_TIME_GATE,
		RT_ITEM_SHOP,
		RT_TRAINING,
		RT_COMMUNITY,
		RT_SET_ITEM_BUY,
		RT_HIDE_BLOOD,
	};

public:
	enum RightState
	{
		RS_RELEASED,		// 해제됨
		RS_RELEASE_ENABLE,	// 해제 가능
		RS_LOCKED,			// 잠금
		RS_SEALED,			// 봉인
		RS_HIDED,
	};

protected:
	int m_iLevel;
	DWORD m_dwRightCode;
	DWORD m_dwPreConditionCode;
	RightState m_RightState;
	int m_iSpendPeso;

	ioHashString m_Name;
	ioHashString m_TypeDesc;
	ioHashString m_Description;

	ioHashString m_FirstFunc;
	ioHashString m_SecondFunc;
	
	ioHashString m_DescImageSet;
	ioHashString m_DescImageName;

public:
	virtual bool IsRightValid() const;
	virtual void LoadProperty( ioINILoader &rkLoader );

	virtual bool CheckPreCondition( ioWnd *pWnd ) const;
	virtual void ExecuteFirstFunc( ioWnd *pWnd ) const = 0;
	virtual void ExecuteSecondFunc( ioWnd *pWnd ) const = 0;
	virtual RightType GetType() const = 0;

public:
	void SetRightState( RightState eState );

public:
	inline int GetRightLevel() const { return m_iLevel; }
	inline DWORD GetRightCode() const { return m_dwRightCode; }
	inline DWORD GetPreConditionCode() const { return m_dwPreConditionCode; }
	inline RightState GetState() const { return m_RightState; }
	inline int GetSpendPeso() const { return m_iSpendPeso; }

	inline const ioHashString& GetName() const { return m_Name; }
	inline const ioHashString& GetTypeDesc() const { return m_TypeDesc; }
	inline const ioHashString& GetDescription() const { return m_Description; }

public:
	inline bool HasFirstFunc() const { return !m_FirstFunc.IsEmpty(); }
	inline bool HasSecondFunc() const { return !m_SecondFunc.IsEmpty(); }

	inline const ioHashString& GetFirstFunc() const { return m_FirstFunc; }
	inline const ioHashString& GetSecondFunc() const { return m_SecondFunc; }

	bool HasDescImage() const;
	inline const ioHashString& GetDescImageSet() const { return m_DescImageSet; }
	inline const ioHashString& GetDescImageName() const { return m_DescImageName; }

	ioUIRenderImage* CreateDescImage() const;

public:
	ioItemRight();
	virtual ~ioItemRight();
};

