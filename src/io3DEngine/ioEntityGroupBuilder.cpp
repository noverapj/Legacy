

#include "stdafx.h"

#include "HelpFunc.h"
#include "ioEntityGroup.h"
#include "ioSkeleton.h"
#include "ioOpcodeShape.h"

#include "ioEntity.h"
#include "ioEntityGroupBuilder.h"

#include "ioNode.h"
#include "ioStringInterface.h"
#include "ioEffect.h"

#include "ioMesh.h"
#include "ioMeshManager.h"
#include "ioEdgeRender.h"

#include "ioStream.h"
#include "ioResourceLoader.h"
#include "ioOpcodeManager.h"

#include "ioMath.h"

char ioEntityGroupBuilder::m_szStartPath[MAX_PATH];

ioEntityGroupBuilder::ioEntityGroupBuilder()
{
	InitDefaultMeshColor();
}

void ioEntityGroupBuilder::InitDefaultMeshColor()
{
	m_iRed = m_iGreen = m_iBlue = 255;
}

void ioEntityGroupBuilder::SetStartDir( const char *szPath )
{
	strcpy( m_szStartPath, szPath );
}

bool ioEntityGroupBuilder::IsModel( ioTextStream &rkStream )
{
	char szText[MAX_PATH];
	memset( szText, 0, MAX_PATH );

	if( TT_MODEL_TOKEN != LineParsing( rkStream, szText, MAX_PATH ) )
		return false;
	
	return true;
}

ioEntityGroupBuilder::TOKEN_TYPE ioEntityGroupBuilder::LineParsing( ioTextStream &rkStream, char *szBuf, int iBufLen )
{
	if( rkStream.IsEOF() )	// EOF
		return TT_MODEL_END;

	char szTemp[MAX_PATH];
	memset(szTemp, 0, MAX_PATH);

	//UJ 140203, 보안강화: 인니 핵 대응하면서 성능 저하가 우려돼 방식을 변경함(아래 숫자 데이터가 문자열 대신 노출됨)
	//#AABOX = 23 41 41 42 4F 58 (6 letters)
	//#SKELETON = 23 53 4B 45 4C 45 54 4F 4E (9 letters)
	//#COLLISION_MESH = 23 43 4f 4c 4c 49 53 49 4f 4e 5f 4d 45 53 48 (15 letters)
	static BYTE szEncAABoxSrc[] = {0x13, 0x31, 0x31, 0x32, 0x3F, 0x48, 0xFE, 0xAA, 0xBB}; //각 글자에서 -0x10하고 뒤에 +3bytes dummy
	static BYTE szEncSkeletonSrc[] = {0x03, 0x33, 0x2B, 0x25, 0x2C, 0x25, 0x34, 0x2F, 0x2E, 0x01, 0x00, 0x02, 0x10}; //각 글자에서 -0x20하고 뒤에 +4bytes dummy
	static BYTE szEncCollisionMeshSrc[] = {0x13, 0x33, 0x3f, 0x3c, 0x3c, 0x39, 0x43, 0x39, 0x3f, 0x3e, 0x4f, 0x3d, 0x35, 0x43, 0x38, 0x50}; //각 글자에서 -0x10하고 뒤에 +1byte dummy
	static bool bEncComplete = false;

	//UJ 140203, 보안강화: 인니 핵 대응하면서 성능 저하가 우려돼 첫 1회만 +0x10, +0x20 코드 실행하게 변경
	static char szEncCollisionMesh[16];
	static char szEncSkeleton[12];
	static char szEncAABox[8];
	if( !bEncComplete )
	{
		bEncComplete = true;

		*((DWORD*)szEncAABox) = *((DWORD*)szEncAABoxSrc) + 0x10101010;
		*((DWORD*)(szEncAABox+4)) = *((DWORD*)(szEncAABoxSrc+4)) + 0x10101010;
		szEncAABox[6] = 0;

		*((DWORD*)szEncSkeleton) = *((DWORD*)szEncSkeletonSrc) + 0x20202020;
		*((DWORD*)(szEncSkeleton+4)) = *((DWORD*)(szEncSkeletonSrc+4)) + 0x20202020;
		*((DWORD*)(szEncSkeleton+8)) = *((DWORD*)(szEncSkeletonSrc+8)) + 0x20202020;
		szEncSkeleton[9] = 0;

		*((DWORD*)szEncCollisionMesh) = *((DWORD*)szEncCollisionMeshSrc) + 0x10101010;
		*((DWORD*)(szEncCollisionMesh+4)) = *((DWORD*)(szEncCollisionMeshSrc+4)) + 0x10101010;
		*((DWORD*)(szEncCollisionMesh+8)) = *((DWORD*)(szEncCollisionMeshSrc+8)) + 0x10101010;
		*((DWORD*)(szEncCollisionMesh+12)) = *((DWORD*)(szEncCollisionMeshSrc+12)) + 0x10101010;
		szEncCollisionMesh[15] = 0;
	}

	std::string szChunktemp;
	rkStream.GetLine(szChunktemp);

	int iSize = szChunktemp.size();
	if(MAX_PATH > iSize)
		memcpy(szTemp, szChunktemp.c_str(), iSize);
	else
		memcpy(szTemp, szChunktemp.c_str(), MAX_PATH-1);

	StringHeadBlankClear( szTemp );

	TOKEN_TYPE tType = TT_UNKNOWN;

	if( szTemp[0] == '#' )
		tType = TT_TITLE;
	else if( szTemp[0] == ';' || !strcmp( szTemp, "" ) )
		tType = TT_COMMENT;
	else
		tType = TT_DATA;

	if( TT_COMMENT == tType )	// 주석은 무시
		tType = LineParsing( rkStream, szBuf, iBufLen );
	else
	{
		if( TT_TITLE == tType )
		{
			if( !strcmp( szTemp, MODEL_TOKEN ) )
				tType = TT_MODEL_TOKEN;
			else if( !strcmp( szTemp, MESH_LIST ) )
				tType = TT_MESH_LIST;
			else if( !strcmp( szTemp, MESH_LIST_END ) )
				tType = TT_MESH_LIST_END;
			//UJ 140113, 보안강화: 인니 핵 대응
			//else if( !strcmp( szTemp, SKELETON ) )
			else if( !strcmp( szTemp, szEncSkeleton ) )
				tType = TT_SKELETON;
			//UJ 140206, 보안강화: 인니 핵 대응
			//else if( !strcmp( szTemp, COLLISION_MESH ) )
			else if( !strcmp( szTemp, szEncCollisionMesh ) )
				tType = TT_COLLISION_MESH;
			//UJ 140120, 보안강화: 인니 핵 대응
			//else if( !strcmp( szTemp, AABOX ) )
			else if( !strcmp( szTemp, szEncAABox ) )
				tType = TT_AABOX;
			else if( !strcmp( szTemp, SPHERE ) )
				tType = TT_SPHERE;
			else if( !strcmp( szTemp, OUTLINE_EDGE ) )
				tType = TT_OUTLINE;
			else if( !strcmp( szTemp, VIEW_CHECK_ONLY) )
				tType = TT_VIEW_CHECK_ONLY;
			else if( !strcmp( szTemp, MODEL_COLOR ) )
				tType = TT_MODEL_COLOR;
			else if( !strcmp( szTemp, OWN_ALPHA_DIST ) )
				tType = TT_OWN_ALPHA_DIST;
			else if( !strcmp( szTemp, EFFECT_LIST ) )
				tType = TT_EFFECT_LIST;
			else if( !strcmp( szTemp, EFFECT_LIST_END ) )
				tType = TT_EFFECT_LIST_END;
			else if( !strcmp( szTemp, ANIMATION_LIST ) )
				tType = TT_ANIMATION_LIST;
			else if( !strcmp( szTemp, ANIMATION_LIST_END ) )
				tType = TT_ANIMATION_LIST_END;
			else if( !strcmp( szTemp, MODEL_END ) )
				tType = TT_MODEL_END;			
			else
				tType = TT_UNKNOWN;
		}

		if( NULL != szBuf )
		{
			memset(szBuf, 0, iBufLen);
			strncpy(szBuf, szTemp, iBufLen-1);
			szBuf[iBufLen-1] = NULL;
		}
	}

	return tType;
}

bool ioEntityGroupBuilder::BuildGroup( const char *szFileName, ioEntityGroup *pGrp, bool bThread )
{
	InitDefaultMeshColor();

	char szFilePath[MAX_PATH];
	if( !strcmp( m_szStartPath, "" ) )
	{
		strcpy( szFilePath, szFileName );
	}
	else
	{
		wsprintf( szFilePath, "%s\\%s", m_szStartPath, szFileName );
	}

	ioTextStream kStream;
	if( !g_ResourceLoader.LoadStream( szFilePath, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroupBuilder::BuildGroup - %s Load Failed", szFilePath );
		return false;
	}

	if( !IsModel( kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroupBuilder::BuildGroup - %s Is Not Model File", szFilePath );
		return false;
	}

	Parsing( kStream, pGrp, bThread );

	if( pGrp )
	{
		pGrp->SetFileName( szFileName );
	}

	return true;
}

bool ioEntityGroupBuilder::SaveGroup( const char *szFileName, ioEntityGroup *pGrp )
{
	char szPath[MAX_PATH];
	if( !strcmp( m_szStartPath, "" ) )
	{
		strcpy( szPath, szFileName );
	}
	else
	{
		wsprintf( szPath, "%s/%s", m_szStartPath, szFileName );
	}

	FILE *fp = fopen( szPath, "wt" );
	if( !fp )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroupBuilder::SaveGroup - %s Save Fail", szFileName );
		return false;
	}

	fprintf( fp, "%s\n", MODEL_TOKEN );

	if( pGrp->HasSkeleton() )
	{
		fprintf( fp, "%s\n", SKELETON );
		ioSkeleton *pSkeleton = pGrp->GetSkeleton();
		fprintf( fp, "%s\n", pSkeleton->GetName().c_str() );

		fprintf( fp, "%s\n", ANIMATION_LIST );

		for( int i=0 ;i<pGrp->GetAnimationCnt() ; i++ )
		{
			fprintf(fp, "%s\n", pGrp->GetAnimationName(i).c_str() );
		}

		fprintf( fp, "%s\n", ANIMATION_LIST_END );
	}

	ioOrientBox kOBB = pGrp->GetCollisionBox();
	D3DXVECTOR3 vTemp;

	fprintf( fp, "%s\n", AABOX );
	
	vTemp = kOBB.GetCenter();
	vTemp.x -= kOBB.GetExtents(0);
	vTemp.y -= kOBB.GetExtents(1);
	vTemp.z -= kOBB.GetExtents(2);
	fprintf( fp, "%.2f %.2f %.2f\n", vTemp.x, vTemp.y, vTemp.z );

	vTemp = kOBB.GetCenter();
	vTemp.x += kOBB.GetExtents(0);
	vTemp.y += kOBB.GetExtents(1);
	vTemp.z += kOBB.GetExtents(2);
	fprintf( fp, "%.2f %.2f %.2f\n", vTemp.x, vTemp.y, vTemp.z );

	fprintf( fp, "%s\n", SPHERE );
	ioSphere kSphere = pGrp->GetCollisionSphere();
	vTemp = kSphere.GetCenter();
	fprintf( fp, "%.2f %.2f %.2f\n", vTemp.x, vTemp.y, vTemp.z );
	fprintf( fp, "%.2f\n", kSphere.GetRadius() );

	ioOpcodeShape *pShape = pGrp->GetOpcodeShape();
	if( pShape )
	{
		fprintf( fp, "%s\n", COLLISION_MESH );
		fprintf( fp, "%s\n", pShape->GetName().c_str() );
	}

	D3DCOLORVALUE kMeshColor = pGrp->GetMeshColor();
	fprintf( fp, "%s\n", MODEL_COLOR );

	int iRed, iGreen, iBlue;
	iRed   = (int)( kMeshColor.r * 255.0f );
	iGreen = (int)( kMeshColor.g * 255.0f );
	iBlue  = (int)( kMeshColor.b * 255.0f );
	fprintf( fp, "%d %d %d\n", iRed, iGreen, iBlue );

	int iEffectCnt = pGrp->GetAttachedEffectCnt();
	if( iEffectCnt > 0 )
	{
		fprintf( fp, "%s\n", EFFECT_LIST );

		for( int i=0 ; i<iEffectCnt ; i++ )
		{
			ioEffect *pEff = pGrp->GetAttachedEffect( i );
			if( pEff )
			{
				D3DXVECTOR3 vOffset( 0.0f, 0.0f, 0.0f );

				ioNode *pNode = pEff->GetParentNode();
				if( pNode )
				{
					vOffset = pNode->GetPosition();
				}

				fprintf( fp, "%s %f %f %f\n", pEff->GetFileName().c_str(),
											  vOffset.x,
											  vOffset.y,
											  vOffset.z );
			}
		}

		fprintf( fp, "%s\n", EFFECT_LIST_END );
	}

	fprintf( fp, "%s\n", MESH_LIST );
	int iEntityCnt = pGrp->GetNumEntities();
	for( int i=0 ; i<iEntityCnt ; i++ )
	{
		ioEntity *pEntity = pGrp->GetEntity(i);

		fprintf( fp, "%s\n", pEntity->GetMeshName().c_str() );
	}
	fprintf( fp, "%s\n", MESH_LIST_END );

	ioEdgeRender *pEdge = pGrp->GetEdgeRender();
	if( pEdge )
	{
		fprintf( fp, "%s\n", OUTLINE_EDGE );
		fprintf( fp, "%s\n", pEdge->GetFileName().c_str() );
		
		fprintf( fp, "%d\n", pEdge->GetEdgeThickness() );

		D3DCOLORVALUE kColor = pEdge->GetEdgeColor();
		fprintf( fp, "%d %d %d\n", (int)( kColor.r * 255.0f ),
								   (int)( kColor.g * 255.0f ),
								   (int)( kColor.b * 255.0f ) );

		fprintf( fp, "%.1f\n", pEdge->GetCreaseAngle() );
	}

	fprintf( fp, "%s\n", MODEL_END );
	fclose(fp);
	
	return true;
}

bool ioEntityGroupBuilder::LoadMeshNameList( const char *szFileName, ioHashStringVec &rkNameList )
{
	char szFilePath[MAX_PATH];
	if( !strcmp( m_szStartPath, "" ) )
	{
		strcpy( szFilePath, szFileName );
	}
	else
	{
		wsprintf( szFilePath, "%s\\%s", m_szStartPath, szFileName );
	}

	ioTextStream kStream;
	if( !g_ResourceLoader.LoadStream( szFilePath, &kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroupBuilder::LoadMeshNameList - %s Load Failed", szFilePath );
		return false;
	}

	if( !IsModel( kStream ) )
	{
		LOG.PrintTimeAndLog( 0, "ioEntityGroupBuilder::LoadMeshNameList - %s Is Not Model File", szFilePath );
		return false;
	}

	char szBuffer[MAX_PATH];
	TOKEN_TYPE tType = TT_UNKNOWN;
	while( TT_MODEL_END != tType && TT_ERROR != tType )
	{
		tType = LineParsing( kStream, szBuffer, MAX_PATH );

		if( tType == TT_MESH_LIST )
		{
			ParsingMeshNameList( kStream, rkNameList );
			break;
		}
	}

	return true;
}

void ioEntityGroupBuilder::ParsingMeshNameList( ioTextStream &rkStream, ioHashStringVec &rkNameList )
{
	char szBuf[MAX_PATH];
	TOKEN_TYPE tType = LineParsing( rkStream, szBuf, MAX_PATH );

	while( TT_MESH_LIST_END != tType && TT_MODEL_END != tType )
	{
		// Parsing Mesh Name
		if( TT_DATA != tType )
		{
			LOG.PrintTimeAndLog( 0, "%s Line is NOT Mesh", szBuf );
			return;
		}

		rkNameList.push_back( ioHashString(szBuf) );

		tType = LineParsing( rkStream, szBuf, MAX_PATH );		
	}
}

void ioEntityGroupBuilder::Parsing( ioTextStream &rkStream, ioEntityGroup *pGrp, bool bThread )
{
	char szBuffer[MAX_PATH];
	TOKEN_TYPE tType = TT_UNKNOWN;

	while( TT_MODEL_END != tType && TT_ERROR != tType )
	{
		tType = LineParsing( rkStream, szBuffer, MAX_PATH );

		switch( tType )
		{
		case TT_MESH_LIST:
			tType = ParsingMesh( rkStream, pGrp, bThread );
			break;
		case TT_AABOX:
			tType = ParsingAABox( rkStream, pGrp );
			break;
		case TT_SPHERE:
			tType = ParsingSphere( rkStream, pGrp );
			break;
		case TT_OUTLINE:
			tType = ParsingOutLine( rkStream, pGrp );
			break;
		case TT_VIEW_CHECK_ONLY:
			tType = ParsingViewCheckOnly( rkStream, pGrp );
			break;
		case TT_MODEL_COLOR:
			tType = ParsingModelColor( rkStream );
			break;
		case TT_OWN_ALPHA_DIST:
			tType = ParsingOwnDistAlpha( rkStream, pGrp );
			break;
		case TT_SKELETON:
			tType = ParsingSkeleton( rkStream, pGrp );
			break;
		case TT_COLLISION_MESH:
			tType = ParsingCollisionMesh( rkStream, pGrp );
			break;
		case TT_EFFECT_LIST:
			tType = ParsingEffect( rkStream, pGrp );
			break;
		case TT_ANIMATION_LIST:
			tType = ParsingAnimation( rkStream, pGrp, bThread );
			break;
		}
	}

	float fInvMaxColor = 1.0f / 255.0f;
	float fRed, fGreen, fBlue;
	fRed   = (float)m_iRed * fInvMaxColor;
	fGreen = (float)m_iGreen * fInvMaxColor;
	fBlue  = (float)m_iBlue * fInvMaxColor;

	if( pGrp )
	{
		pGrp->SetMeshColor( fRed, fGreen, fBlue );
	}
}

ioEntityGroupBuilder::TOKEN_TYPE ioEntityGroupBuilder::ParsingMesh( ioTextStream &rkStream,
																    ioEntityGroup *pGrp,
																	bool bThread )
{
	char szBuf[MAX_PATH];
	TOKEN_TYPE tType = LineParsing( rkStream, szBuf, MAX_PATH );
	
	while( TT_MESH_LIST_END != tType && TT_MODEL_END != tType )
	{
		// Parsing Mesh Name
		if( TT_DATA != tType )
		{
			LOG.PrintTimeAndLog( 0, "%s Line is NOT Mesh", szBuf );
			return TT_ERROR;
		}

		if( pGrp )
		{
			if( !pGrp->AddEntity( ioHashString(szBuf), bThread ) )
				return TT_ERROR;
		}
		else
		{
			ioMesh *pMesh = ioMeshManager::GetSingleton().CreateMesh( ioHashString(szBuf), bThread );
			if( pMesh )
			{
				ioMeshManager::GetSingleton().Destroy( pMesh );
			}
		}

		tType = LineParsing( rkStream, szBuf, MAX_PATH );		
	}

	return tType;
}

ioEntityGroupBuilder::TOKEN_TYPE ioEntityGroupBuilder::ParsingAABox( ioTextStream &rkStream,
																	 ioEntityGroup *pGrp )
{
	char szBuf[MAX_PATH];
	TOKEN_TYPE tType = LineParsing( rkStream, szBuf, MAX_PATH );

	if( tType != TT_DATA )
	{
		LOG.PrintTimeAndLog( 0, "%s Line is Not Data", szBuf );
		return TT_ERROR;
	}

	D3DXVECTOR3 vMin;
	sscanf( szBuf, "%f %f %f", &vMin.x, &vMin.y, &vMin.z );

	tType = LineParsing( rkStream, szBuf, MAX_PATH );

	if( tType != TT_DATA )
	{
		LOG.PrintTimeAndLog( 0, "%s Line is Not Data", szBuf );
		return TT_ERROR;
	}

	D3DXVECTOR3 vMax;
	sscanf( szBuf, "%f %f %f", &vMax.x, &vMax.y, &vMax.z );

	if( pGrp )
	{
		ioOrientBox kOBB;
		kOBB.SetBoxByMinMax( vMin, vMax );
		pGrp->SetCollisionBox( kOBB );
	}

	return tType;
}

ioEntityGroupBuilder::TOKEN_TYPE ioEntityGroupBuilder::ParsingSphere( ioTextStream &rkStream,
																	  ioEntityGroup *pGrp )
{
	char szBuf[MAX_PATH];
	TOKEN_TYPE tType = LineParsing( rkStream, szBuf, MAX_PATH );

	if( tType != TT_DATA )
	{
		LOG.PrintTimeAndLog( 0, "%s Line is Not Data", szBuf );
		return TT_ERROR;
	}

	D3DXVECTOR3 vCenter;
	sscanf( szBuf, "%f %f %f", &vCenter.x, &vCenter.y, &vCenter.z );

	tType = LineParsing( rkStream, szBuf, MAX_PATH );

	if( tType != TT_DATA )
	{
		LOG.PrintTimeAndLog( 0, "%s Line is Not Data", szBuf );
		return TT_ERROR;
	}

	float fRadius;
	sscanf( szBuf, "%f", &fRadius );

	if( pGrp )
	{
		ioSphere kSphere;
		kSphere.SetCenter( vCenter );
		kSphere.SetRadius( fRadius );
		pGrp->SetCollisionSphere( kSphere );
	}

	return tType;
}

ioEntityGroupBuilder::TOKEN_TYPE ioEntityGroupBuilder::ParsingOutLine( ioTextStream &rkStream,
																	   ioEntityGroup *pGrp )
{
	char szBuf[MAX_PATH];

	// File Name
	char szFileName[MAX_PATH];
	TOKEN_TYPE tType = LineParsing( rkStream, szFileName, MAX_PATH );
	if( tType != TT_DATA )
	{
		LOG.PrintTimeAndLog( 0, "%s Line is Not Data", szFileName );
		return TT_ERROR;
	}

	// Edge Thickness
	tType = LineParsing( rkStream, szBuf, MAX_PATH );
	if( tType != TT_DATA )
	{
		LOG.PrintTimeAndLog( 0, "%s Line is Not Data", szBuf );
		return TT_ERROR;
	}

	int iThickness = 1;
	sscanf( szBuf, "%d", &iThickness );

	// Edge Color
	tType = LineParsing( rkStream, szBuf, MAX_PATH );
	if( tType != TT_DATA )
	{
		LOG.PrintTimeAndLog( 0, "%s Line is Not Data", szBuf );
		return TT_ERROR;
	}

	int iRed, iGreen, iBlue;
	sscanf( szBuf, "%d %d %d", &iRed, &iGreen, &iBlue );

	// Crease Angle
	tType = LineParsing( rkStream, szBuf, MAX_PATH );
	if( tType != TT_DATA )
	{
		LOG.PrintTimeAndLog( 0, "%s Line is Not Data", szBuf );
		return TT_ERROR;
	}

	float fCreaseAngle = 0.0f;
	sscanf( szBuf, "%f", &fCreaseAngle );

	if( pGrp )
	{
		ioEdgeRender *pEdge = new ioEdgeRender;
		if( pEdge )
		{
			pEdge->SetFileName( szFileName );
			pEdge->SetEdgeThickness( iThickness );
			pEdge->SetEdgeColor( (float)iRed/255.0f, (float)iGreen/255.0f, (float)iBlue/255.0f );
			pEdge->SetCreaseAngle( fCreaseAngle );

			char szFullName[MAX_PATH];
			wsprintf( szFullName, "%s/%s", ioMeshManager::GetSingleton().GetStartDir(), szFileName );

			if( pEdge->LoadEdgeFile( szFullName ) )
			{
				pGrp->AddEdgeRender( pEdge );
			}
			else
			{
				delete pEdge;
			}
		}
	}

	return tType;
}

ioEntityGroupBuilder::TOKEN_TYPE ioEntityGroupBuilder::ParsingOwnDistAlpha( ioTextStream &rkStream,
																		    ioEntityGroup *pGrp )
{
	char szBuf[MAX_PATH];
	TOKEN_TYPE tType = LineParsing( rkStream, szBuf, MAX_PATH );

	if( tType != TT_DATA )
	{
		LOG.PrintTimeAndLog( 0, "%s Line is Not Data", szBuf );
		return TT_ERROR;
	}

	// 안쓰이는 기능 ( 파일 호환성을 위해 남겨둠 )
/*
	float fAlphaDist;
	sscanf( szBuf, "%f", &fAlphaDist );

	if( pGrp )
	{
		pGrp->SetOwnerDistAlpha( fAlphaDist );
	}
*/
	return tType;
}

ioEntityGroupBuilder::TOKEN_TYPE ioEntityGroupBuilder::ParsingViewCheckOnly( ioTextStream &rkStream,
																			 ioEntityGroup *pGrp )
{
	// 안쓰이는 기능 ( 파일 호환성을 위해 남겨둠 )
	return TT_DATA;
}

ioEntityGroupBuilder::TOKEN_TYPE ioEntityGroupBuilder::ParsingModelColor( ioTextStream &rkStream )
{
	char szBuf[MAX_PATH];
	TOKEN_TYPE tType = LineParsing( rkStream, szBuf, MAX_PATH );

	if( tType != TT_DATA )
	{
		LOG.PrintTimeAndLog( 0, "%s Line is Not Data", szBuf );
		return TT_ERROR;
	}

	sscanf( szBuf, "%d %d %d", &m_iRed, &m_iGreen, &m_iBlue );

	return tType;
}

ioEntityGroupBuilder::TOKEN_TYPE ioEntityGroupBuilder::ParsingEffect( ioTextStream &rkStream, ioEntityGroup *pGrp )
{
	char szBuf[MAX_PATH];
	TOKEN_TYPE tType = LineParsing( rkStream, szBuf, MAX_PATH );

	while( TT_DATA == tType )
	{
		char szEffect[MAX_PATH];
		memset( szEffect, 0, MAX_PATH );

		D3DXVECTOR3 vOffset( 0.0f, 0.0f, 0.0f );
		sscanf( szBuf, "%s %f %f %f", szEffect, &vOffset.x, &vOffset.y, &vOffset.z );

		if( pGrp )
		{
			pGrp->AttachEffect( szEffect, NULL, ioMath::QUAT_IDENTITY, vOffset );
		}

		tType = LineParsing( rkStream, szBuf, MAX_PATH );
	}

	return tType;
}

ioEntityGroupBuilder::TOKEN_TYPE ioEntityGroupBuilder::ParsingSkeleton( ioTextStream &rkStream,
																	    ioEntityGroup *pGrp )
{
	char szBuf[MAX_PATH];
	TOKEN_TYPE tType = LineParsing( rkStream, szBuf, MAX_PATH );

	if( TT_DATA != tType )
	{
		LOG.PrintTimeAndLog( 0, "%s Line is Not Skeleton Name", szBuf );
		return TT_ERROR;
	}

	if( pGrp )
	{
		pGrp->SetSkeletonName( ioHashString( szBuf ) );
	}

	return tType;
}

ioEntityGroupBuilder::TOKEN_TYPE ioEntityGroupBuilder::ParsingAnimation( ioTextStream &rkStream,
																		 ioEntityGroup *pGrp,
																		 bool bThread )
{
	char szBuf[MAX_PATH];
	TOKEN_TYPE tType = LineParsing( rkStream, szBuf, MAX_PATH );

	while( TT_DATA == tType )
	{
		if( pGrp )
		{
			pGrp->AddAnimation( ioHashString(szBuf) );
		}

		tType = LineParsing( rkStream, szBuf, MAX_PATH );
	}

	return tType;
}

ioEntityGroupBuilder::TOKEN_TYPE ioEntityGroupBuilder::ParsingCollisionMesh( ioTextStream &rkStream,
																			 ioEntityGroup *pGrp )
{
	char szBuf[MAX_PATH];
	TOKEN_TYPE tType = LineParsing( rkStream, szBuf, MAX_PATH );

	if( TT_DATA != tType )
	{
		LOG.PrintTimeAndLog( 0, "%s Line is Not CollisionMesh Name", szBuf );
		return TT_ERROR;
	}

	if( pGrp )
	{
		pGrp->SetCollisionShape( ioHashString(szBuf) );
	}
	else
	{
		ioOpcodeShape *pShape = g_OpcodeMgr.CreateShape( ioHashString(szBuf) );
		if( pShape )
		{
			g_OpcodeMgr.Destroy( pShape );
		}
	}

	return tType;
}


