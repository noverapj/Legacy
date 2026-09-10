#pragma once



class ioBaseChar;
class ioAxisAlignBox;
class ioUIRenderImage;
class io3DCharEmoticon;
class ioComplexStringPrinter;

struct CHARACTER;

struct EmoticonInfo
{
	ioHashString m_EmoticonName;
	EmoticonType m_Type;
	
	bool m_bOwner;
	int m_iPriority;

	ioComplexStringPrinter m_DescString;
	
	DWORD m_dwStartTime;
	DWORD m_dwDuration;

	EmoticonInfo()
	{
		m_bOwner = false;
		m_Type = EMT_DEFAULT;
		m_iPriority = 0;
		m_dwStartTime = 0;
		m_dwDuration = 0;
		m_DescString.ClearList();
	}
};
typedef std::vector< EmoticonInfo* > EmoticonInfoList;

class EmoticonInfoSort : public std::binary_function< const EmoticonInfo*, const EmoticonInfo*, bool >
{
public:
	bool operator()( const EmoticonInfo *lhs , const EmoticonInfo *rhs ) const
	{
		if( lhs->m_iPriority < rhs->m_iPriority )
		{
			return true;
		}
		else if( lhs->m_iPriority == rhs->m_iPriority )
		{
			if( lhs->m_dwStartTime > rhs->m_dwStartTime )
			{
				return true;
			}
			else if( lhs->m_dwStartTime == rhs->m_dwStartTime )
			{
				int iCmpValue = stricmp( lhs->m_EmoticonName.c_str(), rhs->m_EmoticonName.c_str() );
				if( iCmpValue < 0 )
					return true;
			}
		}

		return false;
	}
};


class ioCharEmoticon
{
protected:
	ioUIRenderImage *m_pEmoticon;
	ioUIRenderImage *m_pEmoticonBack;
	ioUIRenderImage *m_pEmoticonNum;

	io3DCharEmoticon *m_p3DChatEmoticon;

	DWORD m_dwEmoticonStartTime;
	DWORD m_dwCurEmoticonDuration;
	DWORD m_dwCurScaleDuration;
	float m_fCurStartScale;
	int   m_iHitComboCount; 
	int   m_iCurHitComboCount;
	int	  m_iKillPoint;

	ioHashString m_LongTimeLiveEmoticon;
	bool  m_bLongTimeLiveEmoticon;

	ioHashString m_UnLimitedEmoticon;
	bool m_bUnLimitedEmoticon;

	ioHashStringVec m_vHitComboNum;

	int m_iCharRank;

	enum EmoticonState
	{
		ES_NONE,
		ES_SCALE,
		ES_DURATION,
		ES_END_WHITE,
		ES_END_ALPHA,
		ES_UNLIMITED,
		ES_3D_CHAT,
		ES_DURATION_SCALE,
	};
	EmoticonState m_State;
	EmoticonType  m_Type;
	bool  m_bOwnerChar;

	UIRenderType m_RenderType;
	int m_iRenderXPos;
	int m_iRenderYPos;

	int m_iNumberOffset;
	float m_fNumberScale;

	bool m_bNeedRender;
	bool m_bInScreen;
	bool m_bVisible;

	DWORD m_dwEmoticonNameColor;
	ioHashString m_szEmoticonName;

	DWORD m_dwUnLimitedEmoticonNameColor;
	ioHashString m_szUnLimitedEmoticonName;

	float m_fNamedAlpha;
	float m_fNamedScale;
	int m_iNamedBkColor;

	// For KillPointEmoticon
	ioUIRenderImage *m_pKillPointNum;
	ioUIRenderImage *m_pKillPointPer;

	// For Chat Bubble
	ioUIRenderImage *m_pChatBubbleCenter;
	ioUIRenderImage *m_pChatBubbleRight;
	ioHashStringVec  m_vChatTextVec;

	// For Fishing & LongTimeEmoticonAndScale
	static float m_fDurationScaleRate;
	static float m_fDurationScaleFastRate;

	static DWORD m_dwDurationScaleTime;
	static DWORD m_dwDurationScaleFastTime;
	
	bool m_bFastDuration;
	float m_fCurDurationScaleRate;
	DWORD m_dwDurationScaleStartTime;

	ioUIRenderImage *m_pFishingGrade;
	ioUIRenderImage *m_pFishingLevelUp;

	static float m_fFishingGradeEndScale;

	float m_fCurFishingGradeScale;


protected:
	static ioHashString m_EmoticonBack;

	static int m_iEmoticonXPos;
	static int m_iEmoticonYPos;

	static int m_iEmoticonNameOffsetX;
	static int m_iEmoticonNameOffsetY;
	static int m_iEmoticonNameAlignType;


	static DWORD m_dwDefaultDuration;
	static DWORD m_dwChatIconDuration;
	static DWORD m_dw3DChatIconDuration;
	static DWORD m_dwLongDuration;

	static float m_fEmoticonEndScale;

	static float m_fOwnerGameEventScale;
	static float m_fOwnerSkillBuffScale;
	static float m_fOwnerBadStateScale;
	static float m_fOwnerMovieEventScale;

	static float m_fOtherGameEventScale;
	static float m_fOtherSkillBuffScale;
	static float m_fOtherBadStateScale;
	static float m_fOtherMovieEventScale;

	static DWORD m_dwOwnerGameEventTime;
	static DWORD m_dwOwnerSkillBuffTime;
	static DWORD m_dwOwnerBadStateTime;

	static DWORD m_dwOtherGameEventTime;
	static DWORD m_dwOtherSkillBuffTime;
	static DWORD m_dwOtherBadStateTime;

	static DWORD m_dwDisapearWhiteTime;
	static DWORD m_dwDisapearAlphaTime;

// NewEmoticon
protected:
	EmoticonInfo* m_pCurEmoticonInfo;
	EmoticonInfoList m_ReserveEmoticonList;

public:
	static void LoadStaticValues();

public:
	void SetEmoticon( const ioHashString &szEmoticon,
					  EmoticonType eType,
					  bool bOwnerChar,
					  DWORD dwDuration = 0 );

	void SetNamedEmoticon( const ioHashString &szEmoticon, const ioHashString &szDescName, DWORD dwColor, bool bOwnerChar, DWORD dwDuration = 0 );
	void Set3DChatEmoticon( const CHARACTER &rkInfo, int iChatLen, bool bOwnerChar, bool bNoSound );
	
	void SetHitCombo( const ioHashString &szEmoticon,
					  const ioHashString &szIconComboNum,
					  int iHitCount,
					  bool bOwnerChar );

	void SetKillPoint( const ioHashString &szEmoticon, const ioHashString &szIconNum, const ioHashString &szPer, int iPoint, bool bOwnerChar );
	void SetFishingGrade( const ioHashString &szEmoticon, const ioHashString &szGrade, bool bOwnerChar, DWORD dwDuration = 0 );
	void SetFishingLevelUp( const ioHashString &szEmoticon, const ioHashString &szLevelUp, bool bOwnerChar, DWORD dwDuration = 0 );

	void SetUserRankEmoticon( const ioHashString &szEmoticon, const ioHashString &szRankNumber, int iRank, bool bOwnerChar );

	void SetLongTimeLiveEmoticon( const ioHashString &szEmoticon );
	void SetLongTimeLiveEmoticonAndScale( const ioHashString &szEmoticon, bool bFast );
	void EndLongTimeLiveEmoticon();

	void SetNamedUnLimitedEmoticon( ioBaseChar *pChar, const ioHashString &szEmoticon, const ioHashString &szDescName, DWORD dwColor );
	void EndUnLimitedEmoticon();

	inline DWORD GetDefaultDuration() const { return m_dwDefaultDuration; }

	void EndEmoticonByName( const ioHashString &rszDescName );

	void SetChatTextBubble( const ioHashString &rszChatText ); 

public:
	void UpdateEmoticon( const D3DXVECTOR3 &vPos,
						 const D3DXVECTOR3 &vCamTargetPos,
						 bool bVisible );

	void RenderEmoticon();
	RECT GetEmoticonRenderRect() const;

	inline void NoNeedRender() { m_bNeedRender = false; }
	inline bool IsNeedRender() const { return m_bNeedRender; }
	bool IsEmoticonEvent();

protected:
	void CheckNamedEmoticon( float fAlphaRate, float fScale );
	void CheckNamedUnLimitedEmoticon( float fAlphaRate, float fScale );

	void CheckEmoticonBackImage();
	void EndEmoticon();
	void ReSetHitCombo();

	void CheckOutScreen( const D3DXVECTOR3 &vPos,
						 const D3DXVECTOR3 &vCamTargetPos,
						 int &iXPos,
						 int &iYPos,
						 float fScale,
						 bool bVisible );

	bool RenderChatBubble();
	void RenderChatText();

	//
	void ProcessDurationScale();
	//


// NewEmoticon
public:
	void SetNewEmoticon( const ioHashString &szName,
						 ioComplexStringPrinter DescString,
						 EmoticonType eType,
						 int iPriority,
						 bool bOwnerChar,
						 DWORD dwDuration = 0 );

	void UpdateNewEmoticon( const D3DXVECTOR3 &vPos, const D3DXVECTOR3 &vCamTargetPos, bool bVisible );

protected:
	void SetReserveEmoticon();
	void AddReserveEmoticon( EmoticonInfo* pInfo );	// 리스트에 추가
	EmoticonInfo* GetEmoticonInfoByList();			// 리스트중 가장 우선순위 높은 값 리턴, 해당값은 리스트에서 제거

	void UpdateInfoList();
	void EndNewEmoticon();
	void CheckEmoticonDesc( float fAlphaRate, float fScale );

public:
	ioCharEmoticon();
	~ioCharEmoticon();
};

