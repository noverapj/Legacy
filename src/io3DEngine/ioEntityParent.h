

#ifndef _ioEntityParent_h_
#define _ioEntityParent_h_

#include "io3DCommon.h"

class ioCamera;

class __EX ioEntityParent
{
public:
	virtual void  GetEntityTransform( D3DXMATRIX *pMatWorld ) const = 0;
	virtual float GetSquaredViewDepth( const ioCamera *pCamera ) = 0;

public:
	virtual bool IsShadowRecvRender() const = 0;
	virtual bool HasSkeleton() const = 0;

public:
	virtual ~ioEntityParent(){}
};

#endif