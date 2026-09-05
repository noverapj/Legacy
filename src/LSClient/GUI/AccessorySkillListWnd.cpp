

#include "StdAfx.h"

#include "../ioVoiceChat/Fourier.h"
#include "../ioMineSkill.h"
#include "../ioEngineerMachineGunAttack.h"
#include "../ioFlyJumpExtraAttackBuff.h"
#include "../ioConsecution3Skill.h"
#include "../ioBlindlyShotSkill.h"

#include "GlobalWnd.h"
#include "AccessorySkillListWnd.h"

bool AccessorySkillListBtn::SlotInfo::IsNeedKeyRender() const
{
	if( !pSkillImg || !pItemImg )
		return false;
	if( !bOnOffEnable && IsNeedBuffGaugeProtect() )
		return false;
	if(	iSkillType == ST_PASSIVE )
		return false;

	return true;
}

bool AccessorySkillListBtn::SlotInfo::IsNeedBuffGaugeProtect() const
{
	if( pSkillImg && iSkillType == ST_BUFF && m_iCurGauge == 0 && bUseBuff )
		return true;

	return false;
}

void AccessorySkillListBtn::SlotInfo::ProcessScale()
{
	if( !bSlotGray && bUseScale )
	{
		if( m_dwScaleStartTime == 0 )
			m_dwScaleStartTime = FRAMEGETTIME();

		DWORD dwGapTime = FRAMEGETTIME() - m_dwScaleStartTime;
		if( dwGapTime > m_dwScaleTime )
		{
			dwGapTime -= m_dwScaleTime;
			m_dwScaleStartTime = FRAMEGETTIME() - ( dwGapTime % m_dwScaleTime );
		}

		float fTimeRate = (float)dwGapTime / (float)m_dwScaleTime;
		m_fCurScaleRate = FLOAT1 + m_fScaleRate * sinf( D3DX_PI * fTimeRate );
	}
	else
	{
		m_fCurScaleRate = FLOAT1;
		m_dwScaleStartTime = 0;
	}

	if( pSkillImg )
	{
		if( pItemImg )
			pItemImg->SetScale( m_fCurScaleRate );

		pSkillImg->SetScale( m_fCurScaleRate );

		if( pBackImg )
			pBackImg->SetScale( m_fCurScaleRate );
	}
	else
	{
		if( pBackImg )
			pBackImg->SetScale( FLOAT1 );
	}
}

AccessorySkillListBtn::AccessorySkillListBtn()
{
	m_pGrowthNum = NULL;
	m_pGrowthPlus = NULL;

	m_pActiveCountBack	= NULL;
	m_pActiveCountOver	= NULL;
	m_pActiveCountOn	= NULL;

	m_pActiveCountOnYellow = NULL;
	m_pActiveCountOnRed = NULL;

	m_pKeyBackNor = NULL;
	m_pKeyBackOn  = NULL;
	m_pKeyBack    = NULL;

	m_pEnableEffect1 = NULL;
	m_pEnableEffect2 = NULL;

	m_iCurActiveCount = 0;
	m_iMaxActiveCount = 0;

	m_bAllGrayState = false;

	m_SlotInfo.m_fCurScaleRate = FLOAT1;
	m_SlotInfo.m_dwScaleStartTime = 0;	
	m_pAccessorySkillListWnd = NULL;

	m_wSkillKeyLeftValue  = 0;
	m_wSkillKeyCenterValue= 0;
	m_wSkillKeyRightValue = 0;

	InitSkillState();
	InitKeyState();

	m_pExcaliburRed = NULL;
	m_pExcaliburRedS = NULL;
	m_pExcaliburYellow = NULL;
	m_pExcaliburYellowS = NULL;

	m_pExtraGaugeBack = NULL;
	m_pExtraGaugeRed = NULL;
	m_pExtraGaugeYellow = NULL;

	m_SkillKeyInfoNor.m_SkillKey.Clear();
	m_SkillKeyInfoNor.m_TextColor = 0x000000;
	m_SkillKeyInfoNor.m_BkColor = 0x000000;
	m_SkillKeyInfoOn.m_SkillKey.Clear();
	m_SkillKeyInfoOn.m_TextColor = 0x000000;
	m_SkillKeyInfoOn.m_BkColor = 0x000000;

	//T50 메가 개틀링건(2012. 06. 20) : 스킬에 따라 텍스트 색 및 크기 등 분류
	m_ActiveCountType = DACT_ONE_TEXT;
	m_bRedUI = false;
}

AccessorySkillListBtn::~AccessorySkillListBtn()
{
	SAFEDELETE( m_SlotInfo.pItemImg );
	SAFEDELETE( m_SlotInfo.pSkillImg );
	SAFEDELETE( m_SlotInfo.pBackImg );
	SAFEDELETE( m_SlotInfo.pTextImg );

	SAFEDELETE( m_pGrowthNum );
	SAFEDELETE( m_pGrowthPlus );

	SAFEDELETE( m_pActiveCountBack );
	SAFEDELETE( m_pActiveCountOver );
	SAFEDELETE( m_pActiveCountOn );

	SAFEDELETE( m_pActiveCountOnYellow );
	SAFEDELETE( m_pActiveCountOnRed );

	SAFEDELETE( m_pKeyBackNor );
	SAFEDELETE( m_pKeyBackOn );
	SAFEDELETE( m_pKeyBack );

	SAFEDELETE( m_pEnableEffect1 );
	SAFEDELETE( m_pEnableEffect2 );

	//
	SAFEDELETE( m_pExcaliburRed );
	SAFEDELETE( m_pExcaliburRedS );
	SAFEDELETE( m_pExcaliburYellow );
	SAFEDELETE( m_pExcaliburYellowS );

	SAFEDELETE( m_pExtraGaugeBack );
	SAFEDELETE( m_pExtraGaugeRed );
	SAFEDELETE( m_pExtraGaugeYellow );
}

void AccessorySkillListBtn::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "BackImage")
	{
		m_SlotInfo.pBackImg = pImage;
		m_SlotInfo.pBackImg->SetScale( FLOAT1 );
	}
	else if( szType == "ActiveCountBack" )
		m_pActiveCountBack = pImage;
	else if( szType == "ActiveCountOver" )
		m_pActiveCountOver = pImage;
	else if( szType == "ActiveCountOn" )
		m_pActiveCountOn = pImage;
	else if( szType == "ActiveCountOn_Yellow" )
		m_pActiveCountOnYellow = pImage;
	else if( szType == "ActiveCountOn_Red" )
		m_pActiveCountOnRed = pImage;

	else if( szType == "GrowthNumText" )
	{
		SAFEDELETE( m_pGrowthNum );
		m_pGrowthNum  = pImage;
	}
	else if( szType == "GrowthPlusText" )
	{
		SAFEDELETE( m_pGrowthPlus );
		m_pGrowthPlus  = pImage;
	}
	else if( szType == "EffectImg1" )
	{
		SAFEDELETE( m_pEnableEffect1 );
		m_pEnableEffect1  = pImage;
	}
	else if( szType == "EffectImg2" )
	{
		SAFEDELETE( m_pEnableEffect2 );
		m_pEnableEffect2  = pImage;
	}
	else if( szType == "ExcaliburRed" )
		m_pExcaliburRed = pImage;
	else if( szType == "ExcaliburYellow" )
		m_pExcaliburYellow = pImage;
	else if( szType == "ExcaliburRed_S" )
		m_pExcaliburRedS = pImage;
	else if( szType == "ExcaliburYellow_S" )
		m_pExcaliburYellowS = pImage;
	else if( szType == "ExtraGaugeBack" )
		m_pExtraGaugeBack = pImage;
	else if( szType == "ExtraGaugeRed" )
		m_pExtraGaugeRed = pImage;
	else if( szType == "ExtraGaugeYellow" )
		m_pExtraGaugeYellow = pImage;
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}

// 용병교체UI 관련 작업. ( 2012.02.27 )
void AccessorySkillListBtn::AddRenderFrame( const ioHashString &szType, ioUIRenderFrame *pFrame )
{
	if( szType == "KeyBackNor" )
		m_pKeyBackNor = pFrame;
	else if( szType == "KeyBackOn" )
		m_pKeyBackOn = pFrame;
	else if( szType == "KeyBack" )
		m_pKeyBack = pFrame;
	else
	{
		ioWnd::AddRenderFrame( szType, pFrame );
	}
}

void AccessorySkillListBtn::ParseExtraInfo( ioXMLElement &xElement )
{
	m_dwEffectTime1 = xElement.GetIntAttribute_e( "EffectTime1" );
	m_dwEffectTime2 = xElement.GetIntAttribute_e( "EffectTime2" );

	m_fEffectScale = xElement.GetFloatAttribute_e( "EffectScale" );

	m_szEffectSound = xElement.GetStringAttribute_e( "EffectSound" );
}

void AccessorySkillListBtn::SkillKeySet( AccessorySkillListBtn::SkillKeyInfo &rkSkillKeyInfo, WORD wKeySetLeftValue, WORD wKeySetCenterValue, WORD wKeySetRightValue, DWORD dwTextColor, DWORD dwBkColor )
{
	if( m_pAccessorySkillListWnd == NULL ) return;

	char szSkillKey[MAX_PATH] = "";

	if( wKeySetLeftValue > 0)
	{
		char szToken[MAX_PATH] = "";
		sprintf( szToken, "%s", g_KeyManager.GetKeyText( wKeySetLeftValue ) );
		szToken[1] = NULL;
		strcat( szSkillKey, szToken );
	}

	if( wKeySetCenterValue > 0)
	{
		char szToken[MAX_PATH] = "";
		sprintf( szToken, "%s", g_KeyManager.GetKeyText( wKeySetCenterValue ) );
		szToken[1] = NULL;
		strcat( szSkillKey, szToken );
	}

	if( wKeySetRightValue > 0)
	{
		char szToken[MAX_PATH] = "";
		sprintf( szToken, "%s", g_KeyManager.GetKeyText( wKeySetRightValue ) );
		szToken[1] = NULL;
		strcat( szSkillKey, szToken );
	}

	rkSkillKeyInfo.m_SkillKey = szSkillKey;
	rkSkillKeyInfo.m_TextColor = dwTextColor;
	rkSkillKeyInfo.m_BkColor = dwBkColor;
}

void AccessorySkillListBtn::UpdateActiveCount( int iCur, int iMax, int iSlotType )
{
	m_iCurActiveCount = iCur;
	m_iMaxActiveCount = iMax;
}

void AccessorySkillListBtn::UpdateActiveCountType( DrawActiveCountType iType, bool bRedUI )
{
	m_ActiveCountType = iType;
	m_bRedUI = bRedUI;
}

void AccessorySkillListBtn::ResetActiveCount( int iSlotType )
{
	m_iCurActiveCount = 0;
	m_iMaxActiveCount = 0;
}

void AccessorySkillListBtn::UpdateGaugeInfo( int iMaxValue, int iCurValue, int iExtendMax, bool bUse, bool bUsePassiveGauge /*= false*/ )
{
	if( bUse )
	{
		m_SlotInfo.m_iMaxGauge = iMaxValue;
		m_SlotInfo.m_iCurGauge = iCurValue;
		m_SlotInfo.m_iExtendMaxGauge = iExtendMax;
		m_SlotInfo.m_bUseGauge = true;

		if( m_SlotInfo.iSkillType == ST_PASSIVE && !bUsePassiveGauge )       //패시브 스킬은 즉시 발동.
		{
			m_SlotInfo.m_iCurGauge = iMaxValue;
		}
		else if( m_SlotInfo.iSkillType == ST_COUNT )
		{
			m_SlotInfo.m_iCurGauge = iMaxValue;
		}
	}
	else
	{
		m_SlotInfo.m_bUseGauge = false;
	}
}

void AccessorySkillListBtn::UpdateItemSlotInfo( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	m_bAllGrayState = false;

	CharState eState = pOwner->GetState();
	if( eState == CS_DIE ||
		eState == CS_READY ||
		eState == CS_ENDMOTION ||
		pOwner->GetChatModeState() != CMS_NONE )
	{
		m_bAllGrayState = true;
		if( m_SkillState.GetCurState() != SDT_NONE )
			m_SkillState.ChangeState(SDT_NONE);
	}

	ioItem *pItem = pOwner->GetEquipedItem( GetID() );
	if( !pItem )	return;

	ioHashString szIconName = pItem->GetItemLargeIconName();

	bool bExtendSkill = false;
	bool bItemHwIcon = false;
	char szNewName[MAX_PATH] = "";
	ioUIRenderImage *pItemImgHw = NULL;

	int iMax = pItem->GetMaxSkillGauge( pOwner->GetCharName() );
	int iExtendMax = pItem->GetMaxExtendSkillGauge( pOwner->GetCharName() );
	int iCur = pItem->GetCurSkillGuage();

	if( iExtendMax > 0 && COMPARE(iCur, iMax, iExtendMax) )
		bExtendSkill = true;

	if( bExtendSkill )
	{
		ioSkill *pSkill = pOwner->GetEquipedSkill( GetID() );
		if( pSkill )
		{
			szIconName = pSkill->GetIconName();
		}
	}
	else if( !szIconName.IsEmpty() && !pOwner->IsMale() )
	{
		StringCbPrintf( szNewName, sizeof(szNewName), "%s_hw", szIconName.c_str() );
		pItemImgHw = g_UIImageSetMgr.CreateImageByFullName( szNewName );

		if( pItemImgHw )
		{
			bItemHwIcon = true;
			szIconName = szNewName;
		}
	}

	if( m_SlotInfo.pItemImg )
	{
		if( m_SlotInfo.m_ItemIconName != szIconName )
		{
			SAFEDELETE( m_SlotInfo.pItemImg );

			m_SlotInfo.m_ItemIconName = szIconName;

			if( bItemHwIcon )
			{
				m_SlotInfo.pItemImg = pItemImgHw;
				m_SlotInfo.pItemImg->SetScale( 0.38f );
			}
			else
			{
				m_SlotInfo.pItemImg = g_UIImageSetMgr.CreateImageByFullName( m_SlotInfo.m_ItemIconName );
				m_SlotInfo.pItemImg->SetScale( 0.38f );
			}
		}
		else if( pItemImgHw )
		{
			SAFEDELETE( m_SlotInfo.pItemImg );
		}
	}
	else
	{
		m_SlotInfo.m_ItemIconName = szIconName;

		if( bItemHwIcon )
		{
			m_SlotInfo.pItemImg = pItemImgHw;
			m_SlotInfo.pItemImg->SetScale( 0.38f );
		}
		else
		{
			m_SlotInfo.pItemImg = g_UIImageSetMgr.CreateImageByFullName( m_SlotInfo.m_ItemIconName );
			m_SlotInfo.pItemImg->SetScale( 0.38f );
		}
	}

	m_SlotInfo.bEnableUpdateGauge = pItem->IsEnableUpdateSkillGauge(pOwner->GetCharName());
}

void AccessorySkillListBtn::UpdateSkillSlotInfo( ioBaseChar *pOwner )
{
	bool bPreUseScale = m_SlotInfo.bUseScale;

	m_SlotInfo.bUseScale = false;
	m_SlotInfo.bDisable = false;
	m_SlotInfo.bUseBuffButStopped = false;

	int iSlot = GetID();
	ioSkill *pSkill = pOwner->GetEquipedSkill( iSlot );
	if( !pSkill )
	{
		SAFEDELETE( m_SlotInfo.pSkillImg );

		m_SlotInfo.bSlotGray = false;
		m_SlotInfo.m_fCurScaleRate = FLOAT1;
		m_SlotInfo.m_dwScaleStartTime = 0;

		if( m_SlotInfo.pBackImg )
		{
			m_SlotInfo.pBackImg->SetScale( FLOAT1 );
		}
		return;
	}

	ioHashString szIconName = pSkill->GetIconName();
	bool bExtendSkill = false;
	ioItem *pItem = pOwner->GetEquipedItem( GetID() );
	if( pItem )
	{
		int iMax = pItem->GetMaxSkillGauge( pOwner->GetCharName() );
		int iExtendMax = pItem->GetMaxExtendSkillGauge( pOwner->GetCharName() );
		int iCur = pItem->GetCurSkillGuage();

		if( iExtendMax > 0 && COMPARE(iCur, iMax, iExtendMax) )
		{
			bExtendSkill = true;
			ioHashString szExtendSkill;
			pItem->GetExtendSkillName( szExtendSkill );
			const ioSkill *pExtendSkill = g_SkillMaker.GetSkillConst( szExtendSkill );
			if( pExtendSkill )
			{
				szIconName = pExtendSkill->GetIconName();
			}
		}
	}

	m_SlotInfo.iSkillType = pSkill->GetTypeID();

	if( m_SlotInfo.iSkillType == ST_BUFF )
	{
		m_SlotInfo.bOnOffEnable = pSkill->IsOnOffEnable();
	}
	else
	{
		m_SlotInfo.bOnOffEnable = false;
	}

	// 버프.
	m_SlotInfo.bUseBuff = pSkill->CheckUseBuff( pOwner );

	bool bDownUsing = false;
	bool bJumppingUsing = false;
	CharState eState = (CharState)pOwner->GetState();
	SkillUseType eUseType = SUT_NORMAL;

	// down check
	if( eState == CS_BLOW_WOUNDED || eState == CS_BOUND_BLOW_WOUNDED )
	{
		if( pOwner->GetBlowWoundedState() == BW_BLOW_ENDED )
			eUseType = SUT_DOWN;
	}
	else if( eState == CS_BLOW_DASH_ATTACK && pOwner->GetBlowDashState() == BDS_DOWN )
	{
		eUseType = SUT_DOWN;
	}
	else if( eState == CS_JUMP && pOwner->GetJumpState() == JS_JUMPPING )
	{
		eUseType = SUT_JUMP;
	}

	if( !pOwner->GetUseEnableSkill(iSlot, eUseType) || m_bAllGrayState )
	{
		if( pOwner->CheckDisableSkillByMode( pSkill->GetName() ) )
		{
			m_SlotInfo.bDisable = true;
			m_SlotInfo.bSlotGray = true;
		}
		else if( !pSkill->IsCanUseSkillWithCrown( pOwner ) )
		{
			m_SlotInfo.bDisable = true;
			m_SlotInfo.bSlotGray = true;
		}
		else if( !pSkill->IsCanUseByWeaponType( pOwner ) )
		{
			m_SlotInfo.bDisable = true;
			m_SlotInfo.bSlotGray = true;
		}
		else if( !pSkill->IsCanUseBoss( pOwner ) )
		{
			m_SlotInfo.bDisable = true;
			m_SlotInfo.bSlotGray = true;
		}
		else if( !pSkill->IsCanUseFootball() )
		{
			m_SlotInfo.bDisable = true;
			m_SlotInfo.bSlotGray = true;
		}
		else if( !pSkill->IsCanUseGangSiMode() )
		{
			m_SlotInfo.bDisable = true;
			m_SlotInfo.bSlotGray = true;
		}
		else if( !pSkill->IsCanUseZeroHP( pOwner ) )
		{
			if( m_SlotInfo.bUseBuff )
			{
				m_SlotInfo.bSlotGray = false;
				m_SlotInfo.bUseScale = false;
			}
			else
			{
				m_SlotInfo.bDisable = true;
				m_SlotInfo.bSlotGray = true;
			}
		}
		else if( pSkill->IsCanSwitchSkill() && pOwner->IsCurUsingSkill( pSkill->GetName() ) )
		{
			m_SlotInfo.bSlotGray = false;
		}
		else
		{
			m_SlotInfo.bSlotGray = true;
		}
	}
	else
	{
		m_SlotInfo.bSlotGray = false;
		m_SlotInfo.bUseScale = !m_SlotInfo.bUseBuff;
		m_SlotInfo.bUseBuffButStopped = pOwner->IsSkillBuffLiveButStopped( pSkill->GetName() );

		if( !bPreUseScale && m_SlotInfo.bUseScale && m_SlotInfo.iSkillType != ST_PASSIVE )
		{
			SetEnableEffect( true );
		}
	}

	char szBuf[MAX_PATH] = "";

	if( m_SlotInfo.pSkillImg )
	{
		if( m_SlotInfo.m_SkillIconName != szIconName )
		{
			SAFEDELETE( m_SlotInfo.pSkillImg );
			SAFEDELETE( m_SlotInfo.pTextImg );

			m_SlotInfo.m_SkillIconName = szIconName;
			m_SlotInfo.m_SkillTextImage = pSkill->GetSkillTextImage();

			m_SlotInfo.pSkillImg = g_UIImageSetMgr.CreateImageByFullName( m_SlotInfo.m_SkillIconName );
			if ( m_SlotInfo.pSkillImg )
				m_SlotInfo.pSkillImg->SetScale( 0.38f );
			//m_SlotInfo.pTextImg = g_UIImageSetMgr.CreateImageByFullName( m_SlotInfo.m_SkillTextImage );
		}
	}
	else
	{
		m_SlotInfo.m_SkillIconName = szIconName;
		m_SlotInfo.m_SkillTextImage = pSkill->GetSkillTextImage();

		m_SlotInfo.pSkillImg = g_UIImageSetMgr.CreateImageByFullName( m_SlotInfo.m_SkillIconName );
		if ( m_SlotInfo.pSkillImg )
			m_SlotInfo.pSkillImg->SetScale( 0.38f );

		if( m_SlotInfo.pTextImg )
			SAFEDELETE( m_SlotInfo.pTextImg );
	}

	m_SlotInfo.m_SkillDesc = pSkill->GetDescName();
	m_SlotInfo.m_iGrowthLevel = pSkill->GetGrowthLevel(pOwner->GetCharName());
	m_SlotInfo.m_iReinforce = pSkill->GetGrowthExtraLevel();

	m_SlotInfo.m_iMedalLevel = 0;
	m_SlotInfo.m_nPetLevel = 0;
	m_SlotInfo.m_nAwakeLevel = 0;
	m_SlotInfo.m_iRCharLevel = 0;
	m_SlotInfo.m_iSoldierSetLevel = 0;
	if( m_SlotInfo.iSkillType != ST_PASSIVE && m_SlotInfo.iSkillType != ST_COUNT )
	{
		m_SlotInfo.ProcessScale();
	}
}

void AccessorySkillListBtn::UpdateSkillKeySet()
{
	int  iSlot = GetID();
	bool bChangeKey = false;
	ioUserKeyInput::UseKeyValue kKeys;
	kKeys.SetDefaultFirstKey();
	g_KeyManager.GetGameKey( kKeys );

	switch( iSlot )
	{
	case ES_WEAPON:        // -SD
		{
			if ( kKeys.m_wWeaponSkillKey != KEY_NULL )
			{
				if ( m_wSkillKeyCenterValue != kKeys.m_wWeaponSkillKey )
				{
					m_wSkillKeyLeftValue   = 0;
					m_wSkillKeyCenterValue = kKeys.m_wWeaponSkillKey;
					m_wSkillKeyRightValue  = 0;
					bChangeKey = true;
				}
			}
			else
			{
				if( m_wSkillKeyCenterValue != kKeys.m_wDefenseKey ||
					m_wSkillKeyRightValue != kKeys.m_wAttackKey )
				{				
					m_wSkillKeyLeftValue   = 0;
					m_wSkillKeyCenterValue = kKeys.m_wDefenseKey;
					m_wSkillKeyRightValue  = kKeys.m_wAttackKey;
					bChangeKey = true;
				}
			}
		}
		break;
	case ES_ARMOR:         // ASD
		{
			if ( kKeys.m_wArmorSkillKey != KEY_NULL )
			{
				if ( m_wSkillKeyCenterValue != kKeys.m_wArmorSkillKey )
				{
					m_wSkillKeyLeftValue   = 0;
					m_wSkillKeyCenterValue = kKeys.m_wArmorSkillKey;
					m_wSkillKeyRightValue  = 0;
					bChangeKey = true;
				}
			}
			else
			{
				if( m_wSkillKeyLeftValue  != kKeys.m_wJumpKey ||
					m_wSkillKeyCenterValue != kKeys.m_wDefenseKey ||
					m_wSkillKeyRightValue != kKeys.m_wAttackKey )
				{				
					m_wSkillKeyLeftValue   = kKeys.m_wJumpKey;
					m_wSkillKeyCenterValue = kKeys.m_wDefenseKey;
					m_wSkillKeyRightValue  = kKeys.m_wAttackKey;
					bChangeKey = true;
				}
			}
		}
		break;
	case ES_HELMET:        // A-D
		{
			if ( kKeys.m_wArmorSkillKey != KEY_NULL )
			{
				if ( m_wSkillKeyCenterValue != kKeys.m_wHelmetSkillKey )
				{
					m_wSkillKeyLeftValue   = 0;
					m_wSkillKeyCenterValue = kKeys.m_wHelmetSkillKey;
					m_wSkillKeyRightValue  = 0;
					bChangeKey = true;
				}
			}
			else
			{
				if( m_wSkillKeyLeftValue  != kKeys.m_wJumpKey ||				
					m_wSkillKeyRightValue != kKeys.m_wAttackKey )
				{				
					m_wSkillKeyLeftValue   = kKeys.m_wJumpKey;
					m_wSkillKeyCenterValue = 0;
					m_wSkillKeyRightValue  = kKeys.m_wAttackKey;
					bChangeKey = true;
				}
			}
		}
		break;
	case ES_CLOAK:         // AS-
		{
			if ( kKeys.m_wArmorSkillKey != KEY_NULL )
			{
				if ( m_wSkillKeyCenterValue != kKeys.m_wCloakSkillKey )
				{
					m_wSkillKeyLeftValue   = 0;
					m_wSkillKeyCenterValue = kKeys.m_wCloakSkillKey;
					m_wSkillKeyRightValue  = 0;
					bChangeKey = true;
				}
			}
			else
			{
				if( m_wSkillKeyLeftValue  != kKeys.m_wJumpKey ||
					m_wSkillKeyCenterValue != kKeys.m_wDefenseKey )
				{				
					m_wSkillKeyLeftValue   = kKeys.m_wJumpKey;
					m_wSkillKeyCenterValue = kKeys.m_wDefenseKey;
					m_wSkillKeyRightValue  = 0;
					bChangeKey = true;
				}
			}
		}
		break;
	}

	if( bChangeKey )
	{ 
		SkillKeySet( m_SkillKeyInfoNor, m_wSkillKeyLeftValue, m_wSkillKeyCenterValue, m_wSkillKeyRightValue, TCT_DEFAULT_WHITE, 0xFF133587 );
		SkillKeySet( m_SkillKeyInfoOn, m_wSkillKeyLeftValue, m_wSkillKeyCenterValue, m_wSkillKeyRightValue, TCT_DEFAULT_WHITE, 0xFF133587 );
	}
}

int AccessorySkillListBtn::GetRenderImageStartHeight( ioUIRenderImage *pImage, int iSrcHeight, int iCur, int iMax )
{
	if( iMax == iCur ) return 0;
	if( !pImage )	return 0;

	float fGap = (float)iCur / (float)iMax;
	float fGapHeight = (float)iSrcHeight * fGap;
	int iOffYPos = pImage->GetOffsetY();
	int iHeight  = pImage->GetHeight();

	return iOffYPos + ( (iSrcHeight / 2) - fGapHeight );							
}

void AccessorySkillListBtn::OnRender()
{
	if( m_pAccessorySkillListWnd )
	{
		if( m_pAccessorySkillListWnd->SkipRender() ) return;
	}

	ioWnd::OnRender();

	int iXPos, iYPos;
	iXPos = GetDerivedPosX();
	iYPos = GetDerivedPosY();

	enum { SLOT_XOFFSET = 28, SLOT_YOFFSET = 27, 
		INPUT_XOFFSET = 13, INPUT_YOFFSET = 54,
		INPUTBACK_XOFFSET = 12, INPUTBACK_YOFFSET = 53,
		BULLETBACK_XOFFSET = 46 , BULLETBACK_YOFFSET = -18,	
		SECOUND_BULLET_XOFFSET = 25,
	};

	m_KeyState.UpdateTime();
	m_SkillState.UpdateTime();

	bool bDraw = RenderUseSkillEffect( iXPos + SLOT_XOFFSET, iYPos  + SLOT_YOFFSET);
	if( !bDraw )
		RenderSkillSlot( iXPos + SLOT_XOFFSET, iYPos + SLOT_YOFFSET );

	RenderInputKeyBack( iXPos, iYPos );
	RenderInputKey( iXPos, iYPos );
	RenderActiveCount( iXPos, iYPos );
	RenderEnableEffect( iXPos+SLOT_XOFFSET, iYPos+SLOT_YOFFSET );
}

void AccessorySkillListBtn::RenderSkillSlot( int xPos, int yPos )
{
	if( m_SlotInfo.bSlotGray )
	{
		if( m_SlotInfo.pBackImg )
			m_SlotInfo.pBackImg->Render( xPos, yPos );

		m_KeyState.ChangeState( KET_NONE );

		if( m_SlotInfo.iSkillType == ST_COUNT )
		{
			m_SlotInfo.pItemImg->Render( xPos, yPos, UI_RENDER_GRAY );
		}
		else if( !m_SlotInfo.bOnOffEnable && m_SlotInfo.IsNeedBuffGaugeProtect() )
		{
			if( m_SkillState.ChangeState( SDT_USING ) == SDT_EFFECT_WHITE )
				return;

			m_SlotInfo.pSkillImg->Render( xPos + 2, yPos + 1 );
		}
		else if( m_SlotInfo.pItemImg )
		{
			if( m_SkillState.ChangeState( SDT_WAIT ) == SDT_EFFECT_WHITE_ITEM )
				return;

			m_SlotInfo.pItemImg->Render( xPos + 2, yPos + 1, UI_RENDER_GRAY );
			if( !m_bAllGrayState && !m_SlotInfo.bDisable )
			{
				if( m_SlotInfo.m_bUseGauge && (m_SlotInfo.bEnableUpdateGauge || m_SlotInfo.bUseBuff) )
				{
					if( m_SlotInfo.m_iCurGauge < m_SlotInfo.m_iMaxGauge )
					{
						RenderGaugeIcon( xPos, yPos, m_SlotInfo.pItemImg, ICON_HEIGHT, m_SlotInfo.m_iCurGauge, m_SlotInfo.m_iMaxGauge );
					}
					else
					{
						m_SlotInfo.pItemImg->SetScale( 0.38f );
						m_SlotInfo.pItemImg->Render( xPos, yPos );
					}
				}
				else
				{
					RenderGaugeIcon( xPos, yPos, m_SlotInfo.pItemImg, ICON_HEIGHT, m_SlotInfo.m_iCurGauge, m_SlotInfo.m_iMaxGauge, false );
				}
			}
		}
	}
	else
	{
		if( m_SlotInfo.pSkillImg )
		{
			if( m_SlotInfo.pBackImg )
				m_SlotInfo.pBackImg->Render( xPos, yPos );
			if( !m_SlotInfo.bOnOffEnable && m_SlotInfo.IsNeedBuffGaugeProtect() )
			{
				m_KeyState.ChangeState( KET_NONE );
				if( m_SkillState.ChangeState( SDT_USING ) == SDT_EFFECT_WHITE )
					return ;

				m_SlotInfo.pSkillImg->Render( xPos + 2, yPos + 1, UI_RENDER_NORMAL, TFO_BILINEAR );
			}
			else if( m_SlotInfo.m_iCurGauge < m_SlotInfo.m_iMaxGauge )
			{
				m_KeyState.ChangeState( KET_NONE );
				if( m_SkillState.ChangeState( SDT_USING ) == SDT_EFFECT_WHITE )
					return ;

				RenderGaugeIcon( xPos, yPos, m_SlotInfo.pSkillImg, ICON_HEIGHT, m_SlotInfo.m_iCurGauge, m_SlotInfo.m_iMaxGauge );
			}
			else if( m_SlotInfo.m_iExtendMaxGauge > 0 && COMPARE(m_SlotInfo.m_iCurGauge, m_SlotInfo.m_iMaxGauge, m_SlotInfo.m_iExtendMaxGauge) )
			{
				m_KeyState.ChangeState( KET_NONE );
				if( m_SkillState.ChangeState( SDT_USING ) == SDT_EFFECT_WHITE )
					return ;

				int iMax = m_SlotInfo.m_iExtendMaxGauge - m_SlotInfo.m_iMaxGauge;
				int iCur = m_SlotInfo.m_iCurGauge - m_SlotInfo.m_iMaxGauge;

				RenderGaugeIcon2( xPos, yPos, m_SlotInfo.pItemImg, m_SlotInfo.pSkillImg, ICON_HEIGHT, iCur, iMax );
			}
			else
			{
				if( m_SlotInfo.bUseBuffButStopped )
				{
					m_KeyState.ChangeState( KET_NONE );
					m_SlotInfo.pSkillImg->Render( xPos + 2, yPos + 1, UI_RENDER_NORMAL, TFO_BILINEAR );
				}
				else
				{
					m_SkillState.ChangeState( SDT_CAN );
					m_KeyState.ChangeState( KET_CAN_SKILL );
					m_SlotInfo.pSkillImg->Render( xPos + 2, yPos + 1, UI_RENDER_NORMAL, TFO_BILINEAR );
				}
			}
		}
		else if( m_SkillState.GetCurState() == SDT_NO_SKILL)
		{
			m_KeyState.ChangeState( KET_NONE );
			m_SlotInfo.pItemImg->Render( xPos, yPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
		else
		{
			m_KeyState.ChangeState( KET_NONE );
		}
	}
}

void AccessorySkillListBtn::RenderGrowthLevel( int xPos, int yPos, int iGrowthLevel )
{
	if( !m_pGrowthPlus || !m_pGrowthNum )
		return;

	enum
	{
		PLUS_SIZE	= 7,

		FIRST_SIZE	= 8,
		SECOND_SIZE	= 15,
		THIRD_SIZE	= 22,
	};

	// Level Num
	int iLevelXPos = xPos;
	int iLevelYPos = yPos - 50.0f;

	float fNewXPos = 0.0f;
	if( iGrowthLevel < 10 )
	{
		fNewXPos = iLevelXPos - (PLUS_SIZE+FIRST_SIZE)/2;
		m_pGrowthPlus->Render( fNewXPos, iLevelYPos );

		fNewXPos += PLUS_SIZE;
		m_pGrowthNum->RenderNum( fNewXPos, iLevelYPos, iGrowthLevel );
	}
	else if( iGrowthLevel < 100 )
	{
		fNewXPos = iLevelXPos - (PLUS_SIZE+SECOND_SIZE)/2;
		m_pGrowthPlus->Render( fNewXPos, iLevelYPos );

		fNewXPos += PLUS_SIZE;
		m_pGrowthNum->RenderNum( fNewXPos, iLevelYPos, iGrowthLevel, -FLOAT1 );
	}
	else
	{
		fNewXPos = iLevelXPos - (PLUS_SIZE+THIRD_SIZE)/2;
		m_pGrowthPlus->Render( fNewXPos, iLevelYPos );

		fNewXPos += PLUS_SIZE;
		m_pGrowthNum->RenderNum( fNewXPos, iLevelYPos, iGrowthLevel, -FLOAT1 );
	}
}

void AccessorySkillListBtn::RenderGaugeIcon( int xPos, int yPos, ioUIRenderImage *pImage, int iSrcHeight, int iCur, int iMax, bool bEnableUpdate )
{
	int iRealHeight = GetRenderImageStartHeight( pImage, iSrcHeight, iCur, iMax );

	if( bEnableUpdate )
	{
		pImage->Render( xPos, yPos, UI_RENDER_GRAY );
		pImage->RenderHeightCut( xPos, yPos, iRealHeight, pImage->GetHeight() );
	}
	else
	{
		pImage->SetColor( 150, 150, 150 );
		pImage->RenderHeightCut( xPos, yPos, iRealHeight, pImage->GetHeight(), UI_RENDER_GRAY );
		pImage->SetColor( 255, 255, 255 );
	}

	if( iRealHeight + 2 <= pImage->GetHeight() )
	{
		pImage->SetColor( 0, 0, 0 );
		pImage->RenderHeightCut( xPos, yPos, iRealHeight, iRealHeight + 2 );
		pImage->SetColor( 255, 255, 255 );
	}
}

void AccessorySkillListBtn::RenderGaugeIcon2( int xPos, int yPos,
	ioUIRenderImage *pBaseImage, ioUIRenderImage *pOverImage,
	int iSrcHeight, int iCur, int iMax, bool bEnableUpdate )
{
	int iRealHeight = GetRenderImageStartHeight( pBaseImage, iSrcHeight, iCur, iMax );

	if( bEnableUpdate )
	{
		pBaseImage->Render( xPos, yPos );
		pOverImage->RenderHeightCut( xPos, yPos, iRealHeight, pOverImage->GetHeight() );
	}
	else
	{
		pBaseImage->SetColor( 150, 150, 150 );
		pOverImage->RenderHeightCut( xPos, yPos, iRealHeight, pOverImage->GetHeight(), UI_RENDER_GRAY );
		pOverImage->SetColor( 255, 255, 255 );
	}

	if( iRealHeight + 2 <= pBaseImage->GetHeight() )
	{
		pBaseImage->SetColor( 0, 0, 0 );
		pBaseImage->RenderHeightCut( xPos, yPos, iRealHeight, iRealHeight + 2 );
		pBaseImage->SetColor( 255, 255, 255 );
	}
}

void AccessorySkillListBtn::RenderInputKeyBack( int xPos, int yPos )
{
	if( !m_pKeyBack ) return;
	if( !m_SlotInfo.IsNeedKeyRender() ) return;

	m_pKeyBack->Render( xPos, yPos);
}

void AccessorySkillListBtn::RenderInputKey( int xPos, int yPos )
{
	if( !m_pKeyBackNor || !m_pKeyBackOn ) return;
	if( !m_SlotInfo.IsNeedKeyRender() ) return;

	if( m_KeyState.GetCurState() == KET_NONE )
	{
		m_pKeyBackNor->Render(xPos, yPos);
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetBkColor( m_SkillKeyInfoNor.m_BkColor );
		g_FontMgr.SetTextColor( m_SkillKeyInfoNor.m_TextColor );
		g_FontMgr.PrintText( xPos+28, yPos+53, FONT_SIZE_11, m_SkillKeyInfoNor.m_SkillKey.c_str() );
	}
	else
	{
		m_pKeyBackOn->Render(xPos, yPos);
		g_FontMgr.SetAlignType( TAT_CENTER );
		g_FontMgr.SetTextStyle( TS_OUTLINE_FULL_2X );
		g_FontMgr.SetBkColor( m_SkillKeyInfoOn.m_BkColor );
		g_FontMgr.SetTextColor( m_SkillKeyInfoOn.m_TextColor );
		g_FontMgr.PrintText( xPos+28, yPos+53, FONT_SIZE_11, m_SkillKeyInfoOn.m_SkillKey.c_str() );
	}
}

void AccessorySkillListBtn::RenderActiveCount( int xPos, int yPos )
{
	if( m_iMaxActiveCount <= 0 )
		return;

	if( m_pActiveCountBack )
		m_pActiveCountBack->Render( xPos + 46, yPos - 18 );

	if( !m_pActiveCountOn && !m_pActiveCountOver && !m_pActiveCountOnRed && !m_pActiveCountOnYellow )
		return;

	enum 
	{
		OVER1_XOFFSET = 52, OVER1_YOFFSET = -17,
		NUMBER1_XOFFSET = 64, NUMBER1_YOFFSET = -15,

		OVER2_XOFFSET = 52, OVER2_YOFFSET = -17,
		NUMBER2_XOFFSET = 61, NUMBER2_YOFFSET = -14,

		OVER3_XOFFSET = -22, OVER3_YOFFSET = -19,
		NUMBER3_XOFFSET = -13, NUMBER3_YOFFSET = -16,
	};

	switch( m_ActiveCountType )
	{
	case DACT_ONE_TEXT:
		{
			m_pActiveCountOver->Render( xPos + OVER1_XOFFSET, yPos + OVER1_YOFFSET );

			m_pActiveCountOn->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
			m_pActiveCountOn->RenderNum( xPos + NUMBER1_XOFFSET, yPos + NUMBER1_YOFFSET, m_iCurActiveCount, FLOAT1 );
		}
		break;
	case DACT_TWO_TEXT:
		{
			m_pActiveCountOver->Render( xPos + OVER2_XOFFSET, yPos + OVER2_YOFFSET );

			if( m_bRedUI )
			{
				m_pActiveCountOnRed->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
				m_pActiveCountOnRed->RenderNum( xPos + NUMBER2_XOFFSET, yPos + NUMBER2_YOFFSET, m_iCurActiveCount, -4.0f );
			}
			else
			{
				m_pActiveCountOnYellow->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
				m_pActiveCountOnYellow->RenderNum( xPos + NUMBER2_XOFFSET, yPos + NUMBER2_YOFFSET, m_iCurActiveCount, -4.0f );
			}
		}
		break;
	case DACT_CONSECUTION3:
		{
			m_pActiveCountOver->Render( xPos + OVER3_XOFFSET, yPos + OVER3_YOFFSET );

			if( m_bRedUI )
			{
				m_pActiveCountOnRed->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
				m_pActiveCountOnRed->RenderNum( xPos + NUMBER3_XOFFSET, yPos + NUMBER3_YOFFSET, m_iCurActiveCount, -4.0f );
			}
			else
			{
				m_pActiveCountOnYellow->SetHorzFormatting( ioUIRenderImage::HF_CENTER );
				m_pActiveCountOnYellow->RenderNum( xPos + NUMBER3_XOFFSET, yPos + NUMBER3_YOFFSET, m_iCurActiveCount, -4.0f );
			}
		}
		break;
	}
}

bool AccessorySkillListBtn::RenderUseSkillEffect( int xPos, int yPos )
{
	ioUIRenderImage *pImg = NULL;
	int iColor = 0;
	if( m_SkillState.GetCurState() == SDT_EFFECT_WHITE )
	{
		pImg =m_SlotInfo.pSkillImg;
		iColor = 255.0f * ( (float)m_SkillState.GetCurTime() / (float)m_SkillState.GetDelay() );
	}
	else if( m_SkillState.GetCurState() == SDT_EFFECT_NORMAL )
	{
		pImg =m_SlotInfo.pSkillImg;
		float fRate = FLOAT1 - ( (float)m_SkillState.GetCurTime() / (float)m_SkillState.GetDelay() );
		iColor = 255.0f * fRate;
	}
	else if( m_SkillState.GetCurState() == SDT_EFFECT_WHITE_ITEM )
	{
		pImg =m_SlotInfo.pItemImg;
		iColor = 255.0f * ( (float)m_SkillState.GetCurTime() / (float)m_SkillState.GetDelay() );
	}
	else if( m_SkillState.GetCurState() == SDT_EFFECT_NORMAL_ITEM )
	{
		pImg =m_SlotInfo.pItemImg;
		float fRate = FLOAT1 - ( (float)m_SkillState.GetCurTime() / (float)m_SkillState.GetDelay() );
		iColor = 255.0f * fRate;
	}
	else if( m_SkillState.GetCurState() == SDT_EFFECT_END )
	{
		if( m_SlotInfo.pItemImg )
			m_SlotInfo.pItemImg->SetColor( 255, 255, 255 );

		if( m_SlotInfo.pSkillImg)
		{
			m_SlotInfo.pSkillImg->SetColor(  255, 255, 255 );
		}
	}

	if( pImg )
	{
		pImg->SetColor( iColor, iColor, iColor );
		pImg->Render( xPos, yPos, UI_RENDER_NORMAL_ADD_COLOR );
		return true;
	}

	return false;
}


void AccessorySkillListBtn::InitSkillState()
{
	enum { DELAY_BEING_WHITE = 66, MAX_BEING_NORMAL = 66, };

	m_SkillState.SetCurState( SDT_NONE );

	FSMState *pState = new FSMState(SDT_NONE);
	if( pState )
	{
		pState->Add( SDT_CAN,  SDT_CAN );
		pState->Add( SDT_NO_SKILL,  SDT_NO_SKILL );
	}

	m_SkillState.Add( pState );

	pState = new FSMState(SDT_CAN);
	if( pState )
	{
		pState->Add( SDT_WAIT,  SDT_EFFECT_WHITE_ITEM );
		pState->Add( SDT_USING, SDT_EFFECT_WHITE );
		pState->Add( SDT_NONE, SDT_NONE );
	}
	m_SkillState.Add( pState );

	pState = new FSMState(SDT_EFFECT_WHITE);
	if( pState )
		pState->Add( SDT_EFFECT_WHITE,  SDT_EFFECT_NORMAL, DELAY_BEING_WHITE );
	m_SkillState.Add( pState );

	pState = new FSMState(SDT_EFFECT_WHITE_ITEM);
	if( pState )
		pState->Add( SDT_EFFECT_WHITE_ITEM,  SDT_EFFECT_NORMAL_ITEM, DELAY_BEING_WHITE );
	m_SkillState.Add( pState );

	pState = new FSMState(SDT_EFFECT_NORMAL);
	if( pState )
		pState->Add( SDT_EFFECT_NORMAL,  SDT_EFFECT_END, MAX_BEING_NORMAL );
	m_SkillState.Add( pState );

	pState = new FSMState(SDT_EFFECT_NORMAL_ITEM);
	if( pState )
		pState->Add( SDT_EFFECT_NORMAL_ITEM,  SDT_EFFECT_END,  MAX_BEING_NORMAL );
	m_SkillState.Add( pState );

	pState = new FSMState(SDT_EFFECT_END);
	if( pState )
		pState->Add( SDT_EFFECT_END,  SDT_NONE );
	m_SkillState.Add( pState );

	pState = new FSMState(SDT_NO_SKILL);
	if( pState )
		pState->Add( SDT_NO_SKILL,  SDT_NO_SKILL );
	m_SkillState.Add( pState );
}

void AccessorySkillListBtn::InitKeyState()
{
	enum { DELAY_BEING_ALPHA = 165, DELAY_DONT_RENDER = 168 };

	m_KeyState.SetCurState( KET_NONE );

	FSMState *pState = new FSMState(KET_NONE);
	if( pState )
		pState->Add( KET_CAN_SKILL,  KET_EFFECT_ALPHA );
	m_KeyState.Add( pState );

	pState = new FSMState(KET_EFFECT_ALPHA);
	if( pState )
	{
		pState->Add( KET_EFFECT_ALPHA,  KET_EFFECT_NONE,  DELAY_BEING_ALPHA );
		pState->Add( KET_NONE,  KET_NONE );
	}
	m_KeyState.Add( pState );

	pState = new FSMState(KET_EFFECT_NONE);
	if( pState )
	{
		pState->Add( KET_EFFECT_NONE,  KET_EFFECT_ALPHA,  DELAY_DONT_RENDER );
		pState->Add( KET_NONE,  KET_NONE );
	}
	m_KeyState.Add( pState );
}

void AccessorySkillListBtn::iwm_create()
{
	AccessorySkillListWnd *pAccessorySkillListWnd = dynamic_cast<AccessorySkillListWnd *>( m_pParent );
	if(pAccessorySkillListWnd)
	{
		m_pAccessorySkillListWnd = pAccessorySkillListWnd;
	}
}

void AccessorySkillListBtn::SetEnableEffect( bool bEnable )
{
	if( bEnable )
	{
		m_bEnableEffect = true;
		m_dwEffectStartTime = FRAMEGETTIME();
		m_fCurEffectRate = 0.0f;

		if( !m_szEffectSound.IsEmpty() )
			g_SoundMgr.PlaySound( m_szEffectSound );
	}
	else
	{
		m_bEnableEffect = false;
	}
}

void AccessorySkillListBtn::UpdateEnableEffect()
{
	if( !m_bEnableEffect ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwEffectStartTime;

	if( COMPARE( dwGapTime, m_dwEffectTime1, m_dwEffectTime1+m_dwEffectTime2 ) )
	{
		DWORD dwNewGap = dwGapTime - m_dwEffectTime1;
		float fCurRate = (float)dwNewGap/m_dwEffectTime2;
		m_fCurEffectRate = (float)sin( D3DX_PI * FLOAT05 * fCurRate);
	}
	else
	{
		if( !COMPARE( dwGapTime, 0, m_dwEffectTime1 ) )
		{
			SetEnableEffect( false );
		}
	}
}

void AccessorySkillListBtn::RenderEnableEffect( int iXPos, int iYPos )
{
	if( !m_bEnableEffect ) return;

	DWORD dwCurTime = FRAMEGETTIME();
	DWORD dwGapTime = dwCurTime - m_dwEffectStartTime;

	if( dwGapTime < m_dwEffectTime1 )
	{
		if( m_pEnableEffect1 )
		{
			m_pEnableEffect1->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
	else if( dwGapTime < m_dwEffectTime1+m_dwEffectTime2 )
	{
		float fCurScale = (float)((m_fEffectScale - FLOAT1) * m_fCurEffectRate + FLOAT1);
		float fCurAlpha = (float)(255.0f * (1.0f - m_fCurEffectRate));

		if( m_pEnableEffect2 )
		{
			m_pEnableEffect2->SetScale( fCurScale );
			m_pEnableEffect2->SetAlpha( (BYTE)fCurAlpha );
			m_pEnableEffect2->Render( iXPos, iYPos, UI_RENDER_NORMAL, TFO_BILINEAR );
		}
	}
}
//////////////////////////////////////////////////////////////////////////
AccessorySkillListWnd::AccessorySkillListWnd()
{
	for (int i = 0; i < MAX_SLOT ; i++)
	{
		m_iClassTypes[i] = 0;	
	}
	m_iDefaultX = 0;
	m_iDefaultY = 0;
	m_iCurOverWnd = -1;
	m_eOwnerState = CS_DELAY;

	m_pToolTipCen = NULL;
	m_pToolTipLMid = NULL;
	m_pToolTipRMid = NULL;
	m_pToolTipLeft = NULL;
	m_pToolTipRight = NULL;

	m_pPointImage = NULL;

	InitShowAniState();
}

AccessorySkillListWnd::~AccessorySkillListWnd()
{
	SAFEDELETE( m_pToolTipCen );
	SAFEDELETE( m_pToolTipLMid );
	SAFEDELETE( m_pToolTipRMid );
	SAFEDELETE( m_pToolTipLeft );
	SAFEDELETE( m_pToolTipRight );

	SAFEDELETE( m_pPointImage );

	KeyImageSetMap::iterator iCreate;
	for( iCreate = m_KeyImageSetMap.begin() ; iCreate != m_KeyImageSetMap.end() ; ++iCreate )
	{
		SAFEDELETE( iCreate->second );
	}
	m_KeyImageSetMap.clear();

	if( !COMPARE( m_iDefaultX, 0, Setting::Width() ) || 
		!COMPARE( m_iDefaultY, 0, Setting::Height() ) )
		return;

	g_GUIMgr.GUIPosSaveInt( "AccessorySkillListWnd", "XPos", m_iDefaultX ,POS_SAVE_VERSION );
	g_GUIMgr.GUIPosSaveInt( "AccessorySkillListWnd", "YPos", m_iDefaultY ,POS_SAVE_VERSION );
}

void AccessorySkillListWnd::UpdateByOwner( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	m_eOwnerState = pOwner->GetState();
	UpdateSkillList( pOwner );

	int iIndex = 0;
	for( int i=ID_RING; i<ID_RING+MAX_SLOT; ++i, ++iIndex )
	{
		int iClassType = 0;
		bool bGaugeMax = false;
		ioItem *pCurItem = pOwner->GetEquipedItem( i );
		if( pCurItem )
		{
			int iMax = pCurItem->GetMaxSkillGauge(pOwner->GetCharName());
			int iCur = pCurItem->GetCurSkillGuage();
			int iExtendMax = 0;
			bool bUsePassiveGauge = false;

			ioSkill *pSkill = pOwner->GetEquipedSkill( i );
			ioPassiveSkill *pPassiveSkill = ToPassiveSkill( pSkill );
			if( pPassiveSkill && pPassiveSkill->UsePassiveGauge() )
			{
				iMax = pPassiveSkill->GetMaxPassiveGauge();
				iCur = pPassiveSkill->GetCurPassiveGauge();
				bUsePassiveGauge = true;
			}

			ioHashString szExtendSkill;
			if( pCurItem->GetExtendSkillName( szExtendSkill ) )
			{
				iExtendMax = pCurItem->GetMaxExtendSkillGauge( pOwner->GetCharName() );
			}

			UpdateGaugeInfo( i, iMax, iCur, iExtendMax, pCurItem->HasSkillGauge(), bUsePassiveGauge );
			iClassType = pOwner->GetCharacterInfo().m_class_type;

			if( iCur >= iMax )
				bGaugeMax = true;
		}
		else
		{
			UpdateGaugeInfo( i, 0, 0, 0, false );
			iClassType = 0;
		}

		bool bChange = false;
		if( m_iClassTypes[iIndex] != iClassType )
		{
			m_iClassTypes[iIndex] = iClassType;
			bChange = true;
		}

		// skill desc
		ioSkill *pSkill = pOwner->GetEquipedSkill( i );
		if( pSkill )
		{
			m_kSkillDesc[iIndex].m_szDescName = pSkill->GetDescName();
			m_kSkillDesc[iIndex].m_szDescFullName = pSkill->GetDescFullName();
			m_kSkillDesc[iIndex].m_szDesc     = pSkill->GetDesc();
			m_kSkillDesc[iIndex].m_szIconName = pSkill->GetIconName();
		}
		else
		{
			m_kSkillDesc[iIndex].m_szDescName.Clear();
			m_kSkillDesc[iIndex].m_szDescFullName.Clear();
			m_kSkillDesc[iIndex].m_szDesc.Clear();
			m_kSkillDesc[iIndex].m_szIconName.Clear();
		}

		AccessorySkillListBtn *pSkillBtn = (AccessorySkillListBtn*)FindChildWnd( i );
		if( pSkillBtn && bChange && bGaugeMax )
		{
			if( pSkill && pSkill->GetTypeID() != ST_PASSIVE )
			{
				pSkillBtn->SetEnableEffect( true );
			}
		}

		UpdateActiveCount( pOwner, i );
	}
}

void AccessorySkillListWnd::UpdateSkillList( ioBaseChar *pOwner )
{
	if( !pOwner )	return;

	for( int i=ID_RING; i<=ID_BRACELET + 1; i++ )
	{
		AccessorySkillListBtn *pSkillBtn = (AccessorySkillListBtn*)FindChildWnd( i );
		if( pSkillBtn )
		{
			pSkillBtn->UpdateItemSlotInfo( pOwner );
			pSkillBtn->UpdateSkillSlotInfo( pOwner );
			pSkillBtn->UpdateSkillKeySet();
			pSkillBtn->UpdateEnableEffect();
		}
	}
}

void AccessorySkillListWnd::UpdateGaugeInfo( int iSlot, int iMaxValue, int iCurValue, int iExtendMax, bool bUse, bool bUsePassiveGauge /*= false*/ )
{
	AccessorySkillListBtn *pSkillBtn = (AccessorySkillListBtn*)FindChildWnd( iSlot );
	if( pSkillBtn )
	{
		pSkillBtn->UpdateGaugeInfo( iMaxValue, iCurValue, iExtendMax, bUse, bUsePassiveGauge );
	}
}

void AccessorySkillListWnd::UpdateActiveCount( ioBaseChar *pOwner, int iSlotType )
{
	AccessorySkillListBtn *pSkillBtn = (AccessorySkillListBtn*)FindChildWnd( iSlotType );
	if( !pSkillBtn )
	{
		ResetActiveCount( iSlotType );
		return;
	}

	bool bDisable = false;

	// 오브젝트 아이템 처리
	ioObjectItem *pObject = pOwner->GetObject();
	ioItem *pCurItem = pOwner->GetEquipedItem( iSlotType );

	if( pObject && pCurItem )
	{
		ioBuff *pBuff = pOwner->GetUseActiveCountInBuff();
		ioHashString szBuffCreateItem;
		if ( pBuff && pBuff->GetOwnerItem() )
			szBuffCreateItem = pBuff->GetOwnerItem()->GetName();
		if( pBuff && szBuffCreateItem == pCurItem->GetName() && pObject->GetObjectCreateItem() == pCurItem->GetName() )
		{
			int iCurCount = pBuff->GetCurActiveCount();
			int iMaxCount = pBuff->GetMaxActiveCount();

			pSkillBtn->UpdateActiveCount( iCurCount, iMaxCount, iSlotType );
			UpdateActiveCountType(iSlotType, AccessorySkillListBtn::DACT_ONE_TEXT, bDisable );
			return;
		}
	}

	// 버프 / 버프 스킬 처리
	ioSkill *pEquipSkill = pOwner->GetEquipedSkill( iSlotType );
	if( pEquipSkill )
	{
		if( pEquipSkill->CheckUseBuff( pOwner ) || pEquipSkill->CheckUseActiveCount() )
		{
			ioBuff *pBuff = pOwner->GetUseActiveCountInBuff( pEquipSkill->GetName() );
			if( pBuff && pBuff->UseActiveCount() )
			{
				int iCurCount = pBuff->GetCurActiveCount();
				int iMaxCount = pBuff->GetMaxActiveCount();

				UpdateActiveCountType(iSlotType, AccessorySkillListBtn::DACT_ONE_TEXT, bDisable );
				pSkillBtn->UpdateActiveCount( iCurCount, iMaxCount, iSlotType );
				return;
			}

			ioFlyJumpExtraAttackBuff *pFlyJumpExtraAttackBuff = ToFlyJumpExtraAttackBuff( pOwner->GetBuff( BT_FLY_JUMP_EXTRA_ATTACK ) );
			if ( pFlyJumpExtraAttackBuff && pFlyJumpExtraAttackBuff->GetSkillName() == pEquipSkill->GetName() )
			{
				int iCurCount = pFlyJumpExtraAttackBuff->GetCurCnt();
				int iMaxCount = pFlyJumpExtraAttackBuff->GetMaxCnt();

				UpdateActiveCountType(iSlotType, AccessorySkillListBtn::DACT_ONE_TEXT, bDisable );
				pSkillBtn->UpdateActiveCount( iCurCount, iMaxCount, iSlotType );
				return;
			}
		}

		if ( pEquipSkill->IsUseActiveCnt() )
		{
			int nMaxActiveCnt = pEquipSkill->GetMaxActiveCnt( pOwner );			
			int nCurCnt = nMaxActiveCnt - pEquipSkill->GetCurActiveCnt( pOwner );

			UpdateActiveCountType(iSlotType, AccessorySkillListBtn::DACT_ONE_TEXT, bDisable );
			pSkillBtn->UpdateActiveCount( nCurCnt, nMaxActiveCnt, iSlotType );
			return;
		}

		ioMineSkill *pMineSkill = ToMine( pEquipSkill );
		if( pMineSkill )
		{
			int iMineType = pMineSkill->GetMineType();
			int iMaxMineCnt = pMineSkill->GetMaxMineCnt();
			int iCurMineCnt = g_WeaponMgr.GetCurMineWeaponCnt( pOwner, (MineType)iMineType );

			if( iMineType > 0 )
			{
				int iNewCurCnt = iCurMineCnt;
				if( iMineType == MIT_TRACE )
					iNewCurCnt = iMaxMineCnt - iCurMineCnt;
				else if( iMineType == MIT_TRAP2 )
					iNewCurCnt = iMaxMineCnt - iCurMineCnt;
				else if( iMineType == MIT_AIR )
					iNewCurCnt = iMaxMineCnt - iCurMineCnt;

				UpdateActiveCountType(iSlotType, AccessorySkillListBtn::DACT_ONE_TEXT, bDisable );
				pSkillBtn->UpdateActiveCount( iNewCurCnt, iMaxMineCnt, iSlotType );
				return;
			}
		}

		ioCountSkill *pCountSkill = ToCountSkill( pEquipSkill );
		if( pCountSkill )
		{
			int iCurCount = pCountSkill->GetCurUseCount();
			int iMaxCount = pCountSkill->GetMaxUseCount();
			int iNeedCount = pCountSkill->GetNeedCount();

			if( iCurCount < iNeedCount )
				bDisable = true;
			else
				bDisable = false;

			UpdateActiveCountType(iSlotType, AccessorySkillListBtn::DACT_TWO_TEXT, bDisable );
			pSkillBtn->UpdateActiveCount( iCurCount, iMaxCount, iSlotType );
			return;
		}
	}

	// 사용중인 스킬 처리
	ioSkill *pActiveSkill = pOwner->GetCurActiveSkill();
	if( pActiveSkill && pEquipSkill && pActiveSkill == pEquipSkill )
	{
		ioConsecution3Skill* pCon3 = ToConsecution3Skill( pActiveSkill );
		if( pCon3 )
		{
			int iCurEquip = (int)pCon3->GetOwerItemEquipSlot();			

			if( iSlotType == iCurEquip )
			{
				int iDisableCnt = pCon3->GetChangeUIOverHeatCnt();
				int iCurCount = (int)pCon3->GetCurOverHeatCnt();
				int iMaxCount = (int)pCon3->GetMaxOverHeatCnt();

				if( iCurCount < iDisableCnt )
					bDisable = true;
				else
					bDisable = false;

				UpdateActiveCountType( iSlotType, AccessorySkillListBtn::DACT_CONSECUTION3, bDisable );
				pSkillBtn->UpdateActiveCount( iCurCount, iMaxCount, iSlotType );
				return;
			}
		}

		ioBlindlyShotSkill* pBlindly = ToBlindlyShotSkill( pActiveSkill );
		if( pBlindly )
		{
			int iCurEquip = (int)pBlindly->GetOwerItemEquipSlot();

			if( iSlotType == iCurEquip )
			{
				int iCurCount = pBlindly->GetCurBulletCnt();
				int iMaxCount = pBlindly->GetMaxBulletCnt();
				UpdateActiveCountType( iSlotType, AccessorySkillListBtn::DACT_TWO_TEXT, bDisable );
				pSkillBtn->UpdateActiveCount( iCurCount, iMaxCount, iSlotType );
				return;
			}
		}

		ioEngineerMachineGunAttack* pMachine = ToEngineerMachineGunAttack( pActiveSkill );
		if( pMachine )
		{
			int iCurEquip = (int)pMachine->GetOwerItemEquipSlot();
			if( iSlotType == iCurEquip )
			{
				UpdateActiveCountType( iSlotType, AccessorySkillListBtn::DACT_TWO_TEXT, bDisable );
				pSkillBtn->UpdateActiveCount( pMachine->GetCurBullet(), pMachine->GetMaxBullet(), iSlotType );
				return;
			}
		}

		if( pActiveSkill->UseActiveCount( pOwner ) )
		{
			int iCurCount = pActiveSkill->GetCurActiveCount( pOwner );
			int iMaxCount = pActiveSkill->GetMaxActiveCount( pOwner );
			UpdateActiveCountType( iSlotType, AccessorySkillListBtn::DACT_ONE_TEXT, bDisable );				
			pSkillBtn->UpdateActiveCount( iCurCount, iMaxCount, iSlotType );
			return;
		}
	}

	ResetActiveCount( iSlotType );
}

void AccessorySkillListWnd::UpdateActiveCountType( int iSlotType, AccessorySkillListBtn::DrawActiveCountType iType, bool bRedUI )
{
	AccessorySkillListBtn *pSkillBtn = (AccessorySkillListBtn*)FindChildWnd( iSlotType );
	if( pSkillBtn )
	{
		pSkillBtn->UpdateActiveCountType( iType, bRedUI );
	}	
}

void AccessorySkillListWnd::ResetActiveCount( int iSlotType )
{
	AccessorySkillListBtn *pSkillBtn = (AccessorySkillListBtn*)FindChildWnd( iSlotType );
	if( pSkillBtn )
	{
		pSkillBtn->ResetActiveCount( iSlotType );
	}
}

bool AccessorySkillListWnd::SkipRender()
{
	if( g_GUIMgr.IsSkipUIByBlind() ) return true;

	if( m_ShowAniState.GetCurState() == SAT_DELAY || 
		m_ShowAniState.GetCurState() == SAT_START_ROUND )
		return true;
	if( m_eOwnerState == CS_DIE || 
		m_eOwnerState == CS_READY ||
		m_eOwnerState == CS_VIEW ||
		m_eOwnerState == CS_OBSERVER )
		return true;
	return false;
}

ioUIRenderImage *AccessorySkillListWnd::GetKeyImageSet( const ioHashString &szImageFullName )
{
	KeyImageSetMap::iterator iter = m_KeyImageSetMap.find( szImageFullName );
	if( iter != m_KeyImageSetMap.end() )
		return iter->second;
	else 
	{
		ioUIRenderImage *pKeySetImage = g_UIImageSetMgr.CreateImageByFullName( szImageFullName );
		if( pKeySetImage )
			m_KeyImageSetMap.insert( KeyImageSetMap::value_type( szImageFullName, pKeySetImage ) );

		return pKeySetImage;
	}
	return NULL;
}

void AccessorySkillListWnd::RenderWnd()
{
	if( !IsShow() )	return;

	OnRender();

	if( HasChild() )
	{
		m_bNowGoToTopReserving = true;

		ioWnd *pWnd = NULL;
		// 스킬 아이콘
		for ( int i=ID_RING; i < ID_RING+MAX_SLOT; ++i )
		{
			pWnd = FindChildWnd( i );
			if( pWnd )
			{
				pWnd->RenderWnd();
			}
		}

		// HARDCODE : ID_TOOL_TIP는 RenderAfterChild 시점에 한다
		m_bNowGoToTopReserving = false;
		ExecuteReservedChildGoToTop();
	}

	OnRenderAfterChild();
}

void AccessorySkillListWnd::OnRender()
{
	if( SkipRender() ) return;

	ioWnd::OnRender();
}

void AccessorySkillListWnd::OnRenderAfterChild()
{
	if( !IsShow() ) return;
	if( SkipRender() ) return;

	if( HasChild() )
	{
		m_bNowGoToTopReserving = true;

		ioWnd *pWnd = NULL;
		pWnd = FindChildWnd( ID_TOOL_TIP );
		if( pWnd )
			pWnd->RenderWnd();

		m_bNowGoToTopReserving = false;
		ExecuteReservedChildGoToTop();
	}
}

void AccessorySkillListWnd::OnProcess( float fTimePerSec )
{
	if( !IsShow() ) return;

	if( !g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_NONE )
		m_ShowAniState.ChangeState( SAT_END  );

	if( !g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_START_ROUND )
	{
		if( g_TutorialMgr.IsShowSkillStep() )
		{
			m_ShowAniState.ChangeState( SAT_DELAY );
			g_SoundMgr.PlaySound( m_szAniSound );
		}
	}

	if( g_GUIMgr.IsSkipUIByBlind() && m_ShowAniState.GetCurState() == SAT_END )
		m_ShowAniState.ChangeState( SAT_NONE );

	m_ShowAniState.UpdateTime();

	if( m_ShowAniState.GetCurState() == SAT_UP )
	{
		enum { PLUS_Y_POS = 75 };
		int iStartYPos = PLUS_Y_POS + ( Setting::Height() - ( m_iDefaultY + GetHeight() ) );
		int iCurYpos = m_iDefaultY + ( cos( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * iStartYPos );	
		SetWndPos( m_iDefaultX, iCurYpos );
	}
	else if( m_ShowAniState.GetCurState() == SAT_DOWN )
	{
		enum { MAX_OFFSET_YPOS = 11 };
		int iCurYpos = m_iDefaultY + ( sin( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
		SetWndPos( m_iDefaultX, iCurYpos );
	}
	else if( m_ShowAniState.GetCurState() == SAT_UP_STOP )
	{
		enum { MAX_OFFSET_YPOS = 11 };
		int iCurYpos = m_iDefaultY + ( cos( (PI/2 * m_ShowAniState.GetCurTime()  ) / m_ShowAniState.GetDelay() ) * MAX_OFFSET_YPOS );	
		SetWndPos( m_iDefaultX, iCurYpos );
	}
	else if( m_ShowAniState.GetCurState() == SAT_STOP )
	{
		SetWndPos( m_iDefaultX, m_iDefaultY );
		m_ShowAniState.ChangeState( SAT_END );
	}

	UpdateDefaultPos();

	ProcessMouseOver();
	ioMovingWnd::OnProcess( fTimePerSec );
}

void AccessorySkillListWnd::ProcessMouseOver()
{
	ioLSMouse *pMouse = g_App.GetMouse();

	POINT pt;
	pt.x = pMouse->GetMousePos().x;
	pt.y = pMouse->GetMousePos().y;

	ioWnd *pWnd = NULL;
	bool bSkillOver = false;
	if( pMouse && pMouse->IsMouseShowing() )
	{
		for ( int i=ID_RING; i < ID_RING+MAX_SLOT; ++i )
		{
			pWnd = FindChildWnd( i );
			if( pWnd )
			{
				if( pWnd->IsInWndRect( pt ) && m_iClassTypes[i] != 0 )
				{
					if( m_iCurOverWnd != pWnd->GetID() )
					{
						m_iCurOverWnd = pWnd->GetID();
					}

					bSkillOver = true;
					break;
				}
			}
		}
	}

	if( !bSkillOver )
		m_iCurOverWnd = -1;

	// Skill Over
	ioComplexStringPrinter kPrinter[GLOBAL_TEXT_TOOLTIP_LINE];
	if( COMPARE( m_iCurOverWnd, ID_RING, MAX_SLOT ) )
	{
		AccessorySkillListBtn *pSkillBtn = (AccessorySkillListBtn*)FindChildWnd( m_iCurOverWnd );
		if( pSkillBtn )
		{
			kPrinter[0].SetTextStyle( TS_NORMAL );
			kPrinter[0].SetBkColor( 0, 0, 0 );
			kPrinter[0].SetTextColor( TCT_DEFAULT_DARKGRAY );
			if( m_kSkillDesc[m_iCurOverWnd].m_szDescFullName.IsEmpty() )
				kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(1), m_kSkillDesc[m_iCurOverWnd].m_szDescName.c_str() );
			else
				kPrinter[0].AddTextPiece( FONT_SIZE_12, STR(1), m_kSkillDesc[m_iCurOverWnd].m_szDescFullName.c_str() );

			kPrinter[1].SetTextStyle( TS_NORMAL );
			kPrinter[1].SetBkColor( 0, 0, 0 );
			kPrinter[1].SetTextColor( TCT_DEFAULT_GRAY );
			kPrinter[1].AddTextPiece( FONT_SIZE_12, STR(2), m_kSkillDesc[m_iCurOverWnd].m_szDesc.c_str() );
		}
	}

	GlobalTextTooltip *pTooltip = dynamic_cast<GlobalTextTooltip*>(FindChildWnd( ID_TOOL_TIP ));
	if( pTooltip )
	{
		pTooltip->SetTooltipText( kPrinter );
	}
}

int AccessorySkillListWnd::GetCurOverID()
{
	return m_iCurOverWnd;
}

void AccessorySkillListWnd::InitShowAniState()
{
	enum { DELAY_DELAY = 165, DELAY_UP = 198, DELAY_DOWN = 66, DELAY_UP_STOP = 66, };

	m_ShowAniState.SetCurState( SAT_NONE );

	FSMState *pState = new FSMState(SAT_NONE);
	if( pState )
	{
		pState->Add( SAT_DELAY,  SAT_DELAY );
		pState->Add( SAT_START_ROUND, SAT_START_ROUND );
		pState->Add( SAT_END,  SAT_END );
	}
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_START_ROUND);
	if( pState )
		pState->Add( SAT_DELAY,  SAT_DELAY);
	m_ShowAniState.Add( pState );	

	pState = new FSMState(SAT_DELAY);
	if( pState )
		pState->Add( SAT_DELAY,  SAT_UP , DELAY_DELAY);
	m_ShowAniState.Add( pState );	

	pState = new FSMState(SAT_UP);
	if( pState )
		pState->Add( SAT_UP,  SAT_DOWN , DELAY_UP);
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_DOWN);
	if( pState )
		pState->Add( SAT_DOWN,  SAT_UP_STOP , DELAY_DOWN );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_UP_STOP);
	if( pState )
		pState->Add( SAT_UP_STOP,  SAT_STOP , DELAY_UP_STOP );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_STOP);
	if( pState )
		pState->Add( SAT_END,  SAT_END );
	m_ShowAniState.Add( pState );

	pState = new FSMState(SAT_END);
	if( pState )
	{
		pState->Add( SAT_NONE,  SAT_NONE );
		pState->Add( SAT_START_ROUND, SAT_START_ROUND );
	}
	m_ShowAniState.Add( pState );
}

void AccessorySkillListWnd::iwm_create()
{
	int iXPos = g_GUIMgr.GUIPosLoadInt( "AccessorySkillListWnd", "XPos", GetXPos() ,POS_SAVE_VERSION );
	int iYPos = g_GUIMgr.GUIPosLoadInt( "AccessorySkillListWnd", "YPos", GetYPos() ,POS_SAVE_VERSION );

	if( COMPARE( iXPos, 0, Setting::Width() ) && 
		COMPARE( iYPos, 0, Setting::Height() ) )
	{
		SetWndPos( iXPos, iYPos );
	}

	m_iDefaultX = GetXPos();
	m_iDefaultY = GetYPos();	
}

int AccessorySkillListWnd::GetCurState() const
{
	return m_ShowAniState.GetCurState();
}

void AccessorySkillListWnd::ChangeShowAniState( ShowAniType eShowAniType )
{
	m_ShowAniState.ChangeState( (int) eShowAniType );
}

void AccessorySkillListWnd::UpdateDefaultPos()
{
	if(  g_GUIMgr.IsDragMove( m_dwID )  || IsMoving() )
	{
		if( m_iDefaultX != GetXPos() )
		{
			if( COMPARE( GetXPos(), 0, (Setting::Width() - GetWidth() ) + 1 ))
				m_iDefaultX = GetXPos();
		}

		if( m_iDefaultY != GetYPos() )
		{
			if( COMPARE( GetYPos(), 0, (Setting::Height() - GetHeight() ) + 1 ))
				m_iDefaultY = GetYPos();
		}
	}
}

void AccessorySkillListWnd::SetPositionToDefault( int iParentWidth, int iParentHeight, bool bChildMove, bool bCreate /*= false */ )
{
	float fPosGap = ( Setting::Height() - 600.0f ) / 10;

	int iTempDefYPos = m_iDefYPos;

	m_iDefYPos = iTempDefYPos - fPosGap;

	ioMovingWnd::SetPositionToDefault( iParentWidth, iParentHeight, bChildMove, bCreate );

	m_iDefYPos = iTempDefYPos;

	if( !bCreate )
	{
		m_iDefaultX = GetXPos();
		m_iDefaultY = GetYPos();
		SetWndPos( m_iDefaultX, m_iDefaultY );
	}
}

void AccessorySkillListWnd::ParseExtraInfo( ioXMLElement &xElement )
{
	m_szAniSound = xElement.GetStringAttribute_e( "AniSound" );
}

void AccessorySkillListWnd::AddRenderImage( const ioHashString &szType, ioUIRenderImage *pImage )
{
	if( szType == "ToolTipCenter")
	{
		SAFEDELETE( m_pToolTipCen );
		m_pToolTipCen = pImage;
	}
	else if( szType == "ToolTipLMiddle" )
	{
		SAFEDELETE( m_pToolTipLMid );
		m_pToolTipLMid = pImage;
	}
	else if( szType == "ToolTipRMiddle" )
	{
		SAFEDELETE( m_pToolTipRMid );
		m_pToolTipRMid = pImage;
	}
	else if( szType == "ToolTipLeft" )
	{
		SAFEDELETE( m_pToolTipLeft );
		m_pToolTipLeft = pImage;
	}
	else if( szType == "ToolTipRight" )
	{
		SAFEDELETE( m_pToolTipRight );
		m_pToolTipRight = pImage;
	}
	else if( szType == "PointImage" )
	{
		SAFEDELETE( m_pPointImage );
		m_pPointImage = pImage;
	}
	else
	{
		ioWnd::AddRenderImage( szType, pImage );
	}
}