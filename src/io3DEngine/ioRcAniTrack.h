

#ifndef _ioRcAniTrack_h_
#define _ioRcAniTrack_h_

#include "io3DCommon.h"

class __EX ioRcAniTrack
{
protected:
	typedef std::vector< float > KeyTimeList;

	struct KeyFrame
	{
		D3DXQUATERNION m_qtRot;
		D3DXVECTOR3 m_vTrans;

		KeyFrame( const D3DXQUATERNION &qtRot, const D3DXVECTOR3 &vPos )
			: m_qtRot( qtRot ), m_vTrans( vPos )
		{
		}
	};
	typedef std::vector< KeyFrame > KeyFrameList;

	KeyTimeList  m_vKeyTimeList;
	KeyFrameList m_vKeyFrameList;

	ioHashString m_BipedName;
	float	m_fWeight;

public:
	void ReserveFrameList( int iFrameCount );
	void AddKeyFrame( float fTime,
					  const D3DXVECTOR3 &vTrans,
					  const D3DXQUATERNION &qtRot );

	void SetBipedName( const ioHashString &rkName ) { m_BipedName = rkName; }
	void SetWeight( float fWeight ) { m_fWeight = fWeight; }

public:
	int  GetInterpolatedFrame( float fTime,
		 				       int iHintFrame,	// 이전에 검색한 프레임, 모르면 그냥 0 삽입
							   D3DXVECTOR3 &vTrans,
							   D3DXQUATERNION &qtRot ) const;

	int  GetNoInterpolatedFrame( float fTime,
		 						 int iHintFrame,	// 이전에 검색한 프레임, 모르면 그냥 0 삽입
								 D3DXVECTOR3 &vTrans,
								 D3DXQUATERNION &qtRot ) const;

protected:
	int GetFrameIndex( float fTime, int iHintFrame ) const;

public:
	float GetKeyTime( int iIndex ) const;
	D3DXQUATERNION GetKeyOrientation( int iIndex ) const;
	D3DXVECTOR3 GetKeyPosition( int iIndex ) const;

public:
	inline int GetFrameCount() const { return m_vKeyTimeList.size(); }
	inline const ioHashString& GetRcBipedName() const { return m_BipedName; }
	inline float GetWeight() const { return m_fWeight; }

public:
	ioRcAniTrack();
	ioRcAniTrack( const ioHashString &szBipedName );
	virtual ~ioRcAniTrack();
};

#endif