ZipArc is a sample application demonstrating features of the ZipArchive library.

Copyright (C) 2000 - 2007 Artpol Software - Tadeusz Dracz

This code may be used in compiled form in any way you desire PROVIDING 
it is not sold for profit as a stand-alone application.

This code may be redistributed unmodified by any means providing it is
not sold for profit without the authors written consent, and
providing that this notice and the authors name and all copyright 
notices remains intact. 

E-Mail: support@artpol-software.com
Web Site: http://www.artpol-software.com/


;jal

BOOL CZipArcDoc::OpenZipFile(LPCTSTR lpszZipFileName, long iMode, long iVolumeSize)


함수를 보시면 아래와 같이 정의되어 있어요

enum LocalType
				{
					LCT_KOREA     = 119483910, // 타입값은 보안을 위해서 순차적으로 증가하지 않고 복잡하게 만듬.
					LCT_US        = 365849202,
					LCT_CHINA     = 917382048,
					LCT_TAIWAN    = 521837584,
					LCT_JAPAN     = 213527182,
					LCT_MALAYSIA  = 493820302,
					LCT_INDONESIA = 938400398,
					LCT_VIETNAM   = 783920174,
					LCT_THAILAND  = 800125411,
				};

				LocalType eLocalType = LCT_CHINA;
				
				 
원하는 나라를 정의한 후 Release 로 빌드하면 끝. (현재는 LCT_CHINA 로 되어 있음 )