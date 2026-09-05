// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// RichTextEditor.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"
#include "stdio.h"

// TODO: 필요한 추가 헤더는
// 이 파일이 아닌 STDAFX.H에서 참조합니다.
#include "ioWnd.h"
#include "ioIndexBufferManager.h"
#include "ioVertexBufferManager.h"
#include "ioSingleton.h"
#include "ioVertexFormat.h"
#include "ioUIRenderer.h"
#include "ioTextureManager.h"
#include "ioShaderManager.h"
#include "ioFontManager.h"
#include "ioFrameTimer.h"
#include "InputBox.h"
#include "ProjectManager/ProjectManager.h"

//DWORD ioWnd::m_dwSelectedID;
//BufferUsage ioIndexBufferManager::m_AutoBufferUsage;
//ioIndexBufferManager* Singleton<ioIndexBufferManager>::ms_Singleton;
//BufferUsage ioVertexBufferManager::m_AutoBufferUsage;
//ioVertexBufferManager* Singleton<ioVertexBufferManager>::ms_Singleton;
//ioUIRenderer* Singleton<ioUIRenderer>::ms_Singleton;
//ioTextureManager* Singleton<ioTextureManager>::ms_Singleton;
//ioShaderManager* Singleton<ioShaderManager>::ms_Singleton;
//ioFontManager* Singleton<ioFontManager>::ms_Singleton;
//ioFrameTimer* Singleton<ioFrameTimer>::ms_Singleton;
//ioInputBox* Singleton<ioInputBox>::ms_Singleton;
ProjectManager g_projmgr;