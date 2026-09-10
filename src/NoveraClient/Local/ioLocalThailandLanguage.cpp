#include "stdafx.h"
#include "ioLocalThailandLanguage.h"

namespace ThailandLanguage
{

const char *GetMemTextList()
{
	return "|EXE_WinMain_1|เกิด error จา?Security Program|" //Except Extracting Hangeul
           "|EXE_WinMain_2|lostsaga Run อยู่ก่อนแล้ว|" //Except Extracting Hangeul
           "|EXE_WinMain_3|    ไม่สามารถค้นหา DirectX ได?r\nกรุณ?install DirectX เวอร์ชั่นล่าสุ?r\n(Error Num:301)|" //Except Extracting Hangeul
           "|EXE_WinMain_4|DirectX ที?install เป็น version เก่า(%1)\nกรุณ?install DirectX เวอร์ชั่นล่าสุ?n driver ขอ?Graphics card เป็นเวอร์ชั่นล่าสุดหรือไม่นั้น กรุณาตรวจสอบด้วย\n(Error Num:302)|" //Except Extracting Hangeul
		   "|EXE_WinMain_6|เกิดปั?าที่ฟังชั่นการป้องกันการแฮ?(%1)|" //Except Extracting Hangeul
           "|EXE_ErrorDlgProc_1|--- system specification ---\r\n%1\r\n\r\n--- ?อมูล error ---\r\n%2|"; //Except Extracting Hangeul
}

}