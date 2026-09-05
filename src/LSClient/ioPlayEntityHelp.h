#pragma once

class ioEntity;
class ioEntityGroup;
class ioEffect;
class ioTexture;
class ioHashString;
class ioINILoader;

#include "ioMath.h"

namespace ioPlayEntityHelp
{
	void CheckOrientation( const D3DXVECTOR3 &vDir, D3DXQUATERNION& qtRot );

	void RotateMoveDir( IN float fAngleH, IN float fAngleV, OUT D3DXVECTOR3& vDir, OUT D3DXQUATERNION& qtRot );
	void MoveOffset( IN const D3DXVECTOR3& vOffset, IN const D3DXQUATERNION& qtTargetDir, OUT D3DXVECTOR3& vOutOffset );
}