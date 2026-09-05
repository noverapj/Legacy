

#ifndef _ioBiped_h_
#define _ioBiped_h_

/*
애니메이션이 Action과 Loop로 나뉘어 져있는데
이런 큰 구분 사이에 저장장소로 m_qtRotation과 m_vTranslation이 사용된다.
각 애니메이션 종류안에서의 데이터는 Absolute를 이용해서 연산된다.

애니메이션 간의 가중치는

  1. Action > Loop
  2. 나중에추가 > 기존에 Play중
*/

#include "ioNode.h"

class ioRcBiped;
class ioSkeleton;

class __EX ioBiped : public ioNode
{
protected:
	ioHashString m_Name;
	ioSkeleton	*m_pCreator;

	D3DXQUATERNION	m_qtInitialOrientation;
	D3DXVECTOR3		m_vInitialPosition;
	D3DXMATRIX		m_BipedSpaceTM;

	mutable D3DXMATRIX m_CachedBipedTransform;
	mutable D3DXMATRIX m_CachedTransposeTransform;

	mutable bool m_bCachedBipedXformOutOfDate;
	mutable bool m_bCachedTransposeXformOutOfDate;

	D3DXMATRIX m_ObjectOffsetTM;
	float	m_fLinkOffsetRate;

	D3DXQUATERNION	m_qtRotationAbsolute;	// 한종류의 에니메이션 블렌딩 결과( Action or Loop )
	D3DXVECTOR3		m_vTranslationAbsolute;
	float	m_accumulatedWeightAbsolute;	// 세부 애니메이션들간의 블렌딩 조절
	float	m_accumulatedWeight;			// 애니메이션 종류간의 블렌딩 조절

	D3DXVECTOR3 m_vInitDerivedPosition;
	D3DXQUATERNION m_qtInitDerivedOrientation;

	D3DXQUATERNION	m_qtExtraRotation;
	bool	m_bUseExtraRotation;
	bool	m_bLocalExtraRotation;

	bool	m_bManualBlending;

public:
	void Blend( float fWeight, const D3DXVECTOR3 &vTrans, const D3DXQUATERNION &qtRot );
	void ClearBlend();
	void LockBlend();

	inline void ApplyObjectOffsetTM()
	{
		if( m_accumulatedWeight > 0.0f )
		{
			m_vPosition *= m_fLinkOffsetRate;
		}
	}

public:
	void InitializeByRcBiped( ioRcBiped *pRcBiped );
	void SaveDefDerivedXform();

public:
	void SetManualBlending( bool bManual );
	void SetExtraRotation( const D3DXQUATERNION &qtExtra, bool bLocal );
	void ReleaseExtraRotation();

protected:
	virtual ioNode* CreateChildImpl();

public:
	virtual D3DXMATRIX* GetFullTransform() const;
	virtual void UpdateFromParent() const;

	D3DXMATRIX* GetBipedSpaceFullXform() const;
	D3DXMATRIX* GetTransposeBipedSpaceFullXform() const;
	ioBiped* GetParentBiped() const;

public:
	inline void SetName( const ioHashString &rkName ) { m_Name = rkName; }
	inline const ioHashString& GetName() const { return m_Name; }

	inline const D3DXVECTOR3& GetInitialPosition() const { return m_vInitialPosition; }
	inline const D3DXQUATERNION& GetInitialOrientation() const { return m_qtInitialOrientation; }
	inline const D3DXMATRIX& GetBipedSpaceTM() const { return m_BipedSpaceTM; }
	inline const D3DXMATRIX& GetObjectOffsetTM() const { return m_ObjectOffsetTM; }
	inline float GetLinkOffsetRate() const { return m_fLinkOffsetRate; }

public:
	inline const D3DXVECTOR3& GetInitDerivedPosition() const { return m_vInitDerivedPosition; }
	inline const D3DXQUATERNION& GetInitDerivedOrientation() const { return m_qtInitDerivedOrientation; }

	inline const D3DXQUATERNION& GetExtraRotation() const { return m_qtExtraRotation; }
	inline bool HasExtraRotation() const { return m_bUseExtraRotation; }

public:
	ioBiped( ioSkeleton *pCreator );
	virtual ~ioBiped();
};

#endif
