

#ifndef _ioAniTrack_h_
#define _ioAniTrack_h_

class ioRcAniTrack;
class ioAnimation;

#include "ioMemoryPool.h"

class __EX ioAniTrack : public ioPoolObject< ioAniTrack >
{
private:
	friend class ioAnimation;

private:
	const ioRcAniTrack *m_pRcTrack;
	int		m_iPreFrameIndex;
	int		m_iBipedID;
	float   m_fWeight;

public:
	void SetBipedID( int iBipedID ) { m_iBipedID = iBipedID; }

public:
	void GetInterpolatedFrame( float fCurTime,
							   D3DXVECTOR3 &vTrans,
							   D3DXQUATERNION &qtRot );

	void GetNoInterpolatedFrame( float fCurTime,
							     D3DXVECTOR3 &vTrans,
							     D3DXQUATERNION &qtRot );

public:
	inline int GetBipedID() const { return m_iBipedID; }
	inline float GetWeight() const { return m_fWeight; }

	const ioHashString& GetBipedName() const;

private:
	ioAniTrack( const ioRcAniTrack *pRcTrack );
	virtual ~ioAniTrack();
};

#endif