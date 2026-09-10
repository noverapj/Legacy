#include "stdafx.h"
#include "ioLocalJapanLanguage.h"

namespace JapanLanguage
{

const char *GetMemTextList()
{
	return "|EXE_WinMain_1|セキュリティプログラ?にエラ?が発生しました。|" //Except Extracting Hangeul
		   "|EXE_WinMain_2|LostSagaがすでに実行中です。|" //Except Extracting Hangeul
           "|EXE_WinMain_3|    DirectXを見つける事が出来ません。\r\n最新バ?ジョンの DirectXを設置してください。\r\n(エラ?番号:301)|" //Except Extracting Hangeul
           "|EXE_WinMain_4|設置された DirectXのバ?ジョンが低いです。(%1)\n最新バ?ジョンの DirectXを設置してください。\nグラフィックドライバ?も最新バ?ジョンであるか、ご確認ください。\n(エラ?番号:302)|" //Except Extracting Hangeul
           "|EXE_ErrorDlgProc_1|--- システ?仕様 ---\r\n%1\r\n\r\n--- エラ?情報 ---\r\n%2|"; //Except Extracting Hangeul
   		 
}

}