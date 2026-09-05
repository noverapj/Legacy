

#ifndef _ioNode_h_
#define _ioNode_h_

#include "io3DCommon.h"
#include "ioPrerequisites.h"

class ioCamera;

class __EX ioNode
{
public:
	enum TransformSpace
	{
        /// Transform is relative to the local space
        TS_LOCAL,
		/// Transform is relative to the space of the parent node
		TS_PARENT,
		/// Transform is relative to world space
		TS_WORLD
	};

protected:
	typedef std::map< UniqueNodeID, ioNode* > ChildNodeMap;
	ChildNodeMap m_ChildMap;

	typedef std::set< ioNode* > ChildUpdateSet;
	mutable ChildUpdateSet m_ChildrenToUpdate;

	ioNode	*m_pParent;
	UniqueNodeID m_UniqueID;

	// Parent에 상대적인 Node 자체의 Transform
	D3DXQUATERNION	m_qtOrientation;
	D3DXVECTOR3		m_vPosition;
	D3DXVECTOR3		m_vScale;

	// Parent의 Transform과 자체Transform이 결합된 결과
	mutable D3DXQUATERNION	m_qtDerivedOrientation;
	mutable D3DXVECTOR3		m_vDerivedPosition;
	mutable D3DXVECTOR3		m_vDerivedScale;

	mutable D3DXMATRIX		m_CachedTransform;

	mutable float m_fSquaredViewDepth;
	mutable DWORD m_dwLastViewDepthUpdateFrame;

	mutable bool	m_bNeedParentUpdate;
	mutable bool	m_bNeedChildUpdate;
	mutable bool	m_bParentNotified;
	mutable bool	m_bCachedTransformOutOfDate;

protected:
	static UniqueNodeID m_sNextNodeID;
	static UniqueNodeID GetNextUniqueID();

protected:
	void SetParent( ioNode *pParent );

	virtual ioNode* CreateChildImpl() = 0;

public:
	inline UniqueNodeID GetUniqueID() const { return m_UniqueID; }
	virtual ioNode* GetParent() const;

	virtual void SetOrientation( const D3DXQUATERNION &qt );
	virtual const D3DXQUATERNION& GetOrientation() const;

	virtual void SetPosition( const D3DXVECTOR3 &vPos );
	virtual void SetPosition( float fX, float fY, float fZ );
	virtual const D3DXVECTOR3& GetPosition() const;

	virtual void SetScale( const D3DXVECTOR3 &vScale );
	virtual void SetScale( float fX, float fY, float fZ );
	virtual const D3DXVECTOR3& GetScale() const;

	virtual void Scale( const D3DXVECTOR3 &vScale );
	virtual void Scale( float fX, float fY, float fZ );

	virtual void Translate( const D3DXVECTOR3 &vMove, TransformSpace eRelativeTo = TS_PARENT );
	virtual void Translate( float fX, float fY, float fZ, TransformSpace eRelativeTo = TS_PARENT );

	virtual void Translate( const D3DXMATRIX &matAxis,
							const D3DXVECTOR3 &vMove,
							TransformSpace eRelativeTo = TS_PARENT );
	virtual void Translate( const D3DXMATRIX &matAxis,
							float fX, float fY, float fZ,
							TransformSpace eRelativeTo = TS_PARENT );

	virtual void RollZ( float fDegrees, TransformSpace eRelativeTo = TS_PARENT );
	virtual void PitchX( float fDegrees, TransformSpace eRelativeTo = TS_PARENT );
	virtual void YawY( float fDegrees, TransformSpace eRelativeTo = TS_PARENT );

	virtual void Rotate( const D3DXVECTOR3 &vAxis,
						 float fDegrees,
						 TransformSpace eRelativeTo = TS_PARENT );
	virtual void Rotate( const D3DXQUATERNION &qt, TransformSpace eRelativeTo = TS_PARENT );

	virtual D3DXMATRIX GetLocalAxes() const;

	virtual ioNode* CreateChild();

	virtual void AddChild( ioNode *pChild );

	virtual int GetNumChildren() const;
	virtual ioNode* GetChildByIndex( int iIndex ) const;
	virtual ioNode* GetChildByUniqueID( UniqueNodeID eNodeID ) const;

	//Do NOT DELETE child
	virtual ioNode* RemoveChild( UniqueNodeID eNodeID );
	//Do NOT DELETE child
	virtual ioNode* RemoveChild( ioNode *pChild );
	//Do NOT DELETE child
	virtual void RemoveAllChildren();

	virtual const D3DXQUATERNION& GetDerivedOrientation() const;
	virtual const D3DXVECTOR3& GetDerivedPosition() const;
	virtual const D3DXVECTOR3& GetDerivedScale() const;

	virtual D3DXMATRIX* GetFullTransform() const;

	virtual void Update( bool bUpdateChildren, bool bParentHasChanged, bool bResetVolume=false );
	virtual void UpdateFromParent() const;

	virtual void RequestUpdate( ioNode *pChild );
	virtual void CancelUpdate( ioNode *pChild );
	virtual void NeedUpdate();

public:
	float GetSquaredViewDepth( const ioCamera *pCam ) const;

public:
	ioNode();
	virtual ~ioNode();
};

#endif
