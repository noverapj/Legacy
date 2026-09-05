

#ifndef _ioTextureUnitState_h_
#define _ioTextureUnitState_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioPass;
class ioTexture;
class ioTextureEffect;

struct TextureEffect
{
	TextureEffectType m_Type;
	float m_fArg1;
	float m_fArg2;
	DWORD m_dwOnce;

	ColorTransformType m_ColorXformType;
	TextureTransformType m_XformType;
	WaveformType m_WaveType;
	float m_fBase;
	float m_fFrequency;
	float m_fPhase;
	float m_fAmplitude;

	ioTextureEffect *m_pController;
};

class __EX ioTextureUnitState
{
public:
	enum
	{
		MAX_ANI_TEX_FRAMES = 16
	};


private:
	ioPass *m_pParent;

	ioHashString m_Frames[MAX_ANI_TEX_FRAMES];
	ioTexture* m_FrameTex[MAX_ANI_TEX_FRAMES];

	int m_iNumFrames;
	int m_iCurFrame;
	int m_iTextureCoordSetIdx;
	int m_iTextureCustomValue;

	TextureAddressMode m_AddressMode;
	StageBlendOperation m_ColorOp;
	StageBlendOperation m_AlphaOp;

	float m_fAnimDuration;
	float m_fUMod, m_fVMod;
	float m_fUScale, m_fVScale;
	float m_fRotate;

	typedef std::vector< TextureEffect > TexEffectList;
	TexEffectList m_TexEffectList;

	TextureFilterOption m_TexFilterOption;
	float	m_fTextureMipMapBias;
	int	m_iMaxAniso;

	D3DXMATRIX m_TexModMatrix;
	bool m_bRecalcTexMatrix;
	bool m_bIdentityTexMatrix;

	bool m_bUseLightTexture;
	bool m_bDefaultFiltering;
	bool m_bRenderTarget;
	bool m_bScaleUVOnCenter;

public:
	void Load( bool bLoadThread = true );
	bool IsLoaded() const;	// 이전에 로딩함수가 호출된적이 있는가..
	bool IsLoadDone();		// 스레드로딩이 모두 완료되었는가.

	ErasePolicy GetTextureErasePolicy() const;

private:
	void RecalcTextureMatrix();
	void ClearTextureModifyValues();
	void UnLoad();

public:
	void SetTextureName( const ioHashString &kName );
	void SetTextureCoordSet( int iCoordSet );
	void SetTextureCustomValue( int iCustomValue );
	
	void SetCurFrame( int iFrameNum );
	void SetCurFrameByRate( float fRate );

	void SetTextureTransform( const D3DXMATRIX &kMat );

	// 이름이 순차적으로 나감 tex.jpg, tex1.jpg, tex2.jpg...
	void SetTextureAnimation( int iNumFrames, float fDuration = 0.0f, bool bOnce = false );

	void SetTextureScroll( float fU, float fV );
	void SetTextureUScroll( float fValue );
	void SetTextureVScroll( float fValue );
	void SetTextureScaleOnCenter( bool bCenter );

	void SetTextureScale( float fU, float fV );
	void SetTextureUScale( float fValue );
	void SetTextureVScale( float fValue );

	void SetTextureRotate( float fDegrees );

	void SetScrollAnimation( float fUSpeed, float fVSpeed, bool bOnce = false );

	void SetRotateAnimation( float fSpeed, bool bOnce = false );
	void SetTransformAnimation( TextureTransformType ttType,
								WaveformType waveType,
								float fBase = 0.0f,
								float fFrequence = 1.0f,
								float fPhase = 0.0f,
								float fAmplitude = 1.0f,
								bool bOnce = false );

	void SetColorTransformRate( ColorTransformType ctType, float fRate );
	void SetColorTransform( ColorTransformType ctType,
							WaveformType eWaveType,
							float fBase = 0.0f,
							float fFrequence = 1.0f,
							float fPhase = 0.0f,
							float fAmplitude = 1.0f,
							bool bOnce = false );

	void StopAllTextureEffect( bool bStop );

public:
	void AddTexEffect( TextureEffect &rkEffect );
	void RemoveAllEffects();
	void RemoveEffect( const TextureEffectType tType );
	void RemoveEffectByIndex( int iIndex );

public:
	void SetUseLightTexture();

	void SetTextureAddressMode( TextureAddressMode eMode );
	void SetTextureFiltering( TextureFilterOption eOption );
	void SetTextureMipMapBias( float fBias );
	void SetTextureAnisotropy( int iMaxAniso );

	void SetColorStageBlendOp( D3DTEXTUREOP eOp, DWORD dwSrc, DWORD dwDest );
	void SetColorStageBlendOp( D3DTEXTUREOP eOp );
	void SetColorStageBlendSource( DWORD dwSrc );
	void SetColorStageBlendDest( DWORD dwDest );

	void SetAlphaStageBlendOp( D3DTEXTUREOP eOp, DWORD dwSrc, DWORD dwDest );
	void SetAlphaStageBlendOp( D3DTEXTUREOP eOp );
	void SetAlphaStageBlendSource( DWORD dwSrc );
	void SetAlphaStageBlendDest( DWORD dwDest );

public:
	D3DXVECTOR2 GetTextureDimensions( int iFrame = 0 ) const;

	inline DWORD GetTextureHashCode() const { return m_Frames[m_iCurFrame].GetHashCode(); }
	inline const ioHashString& GetTextureName() const { return m_Frames[m_iCurFrame]; }
	inline const ioHashString& GetFrameTextureName( int iFrame ) const { return m_Frames[iFrame]; }

	inline ioTexture* GetTexture() const { return m_FrameTex[m_iCurFrame]; }
	inline ioTexture* GetFrameTexture( int iFrame ) const { return m_FrameTex[iFrame]; }

	inline int GetCurFrame() const { return m_iCurFrame; }
	inline int GetNumFrames() const { return m_iNumFrames; }

	inline int GetTextureCoordSet() const { return m_iTextureCoordSetIdx; }
	inline int GetTextureCustomValue() const { return m_iTextureCustomValue; }

	inline float GetTextureUScroll() const { return m_fUMod; }
	inline float GetTextureVScroll() const { return m_fVMod; }

	inline float GetTextureUScale() const { return m_fUScale; }
	inline float GetTextureVScale() const { return m_fVScale; }
	inline bool  IsTextureScaleOnCenter() const { return m_bScaleUVOnCenter; }

	inline float GetTextureRotate() const { return m_fRotate; }
	inline float GetAnimationDuration() const { return m_fAnimDuration; }
	inline bool  IsUseLightTexture() const { return m_bUseLightTexture; }

	inline TextureAddressMode GetTextureAddressMode() const { return m_AddressMode; }
	inline float GetTextureMipMapBias() const { return m_fTextureMipMapBias; }

	inline TextureFilterOption GetTextureFiltering() const
	{
		if( m_bDefaultFiltering )
			return TFO_BILINEAR;

		return m_TexFilterOption;
	}

	inline int GetTextureAnisotropy() const { return m_iMaxAniso; }

	inline bool IsIdentityTextureTransform() const { return m_bIdentityTexMatrix; }
	inline const D3DXMATRIX& GetTextureTransform()
	{
		if( m_bRecalcTexMatrix )
			RecalcTextureMatrix();

		return m_TexModMatrix;
	}
	
	inline const StageBlendOperation& GetColorStageBlendOp() const { return m_ColorOp; }
	inline const StageBlendOperation& GetAlphaStageBlendOp() const { return m_AlphaOp; }

	inline int GetTextureEffectCnt() const { return m_TexEffectList.size(); }
	TextureEffect* GetTextureEffect( int iIndex );

public:
	inline void SetRenderTarget( bool bTarget ) { m_bRenderTarget = bTarget; }
	inline bool IsRenderTarget() const { return m_bRenderTarget; }

public:
	ioTextureUnitState& operator = ( const ioTextureUnitState &rhs );
	
	bool operator==( const ioTextureUnitState &rhs ) const;
	bool operator!=( const ioTextureUnitState &rhs ) const;

public:
	ioTextureUnitState( ioPass *pParent );
	ioTextureUnitState( ioPass *pParent, const ioTextureUnitState &rhs );
	ioTextureUnitState( ioPass *pParent, const ioHashString &szTexName, int iTexCoordSet = 0 );
	~ioTextureUnitState();
};

#endif
