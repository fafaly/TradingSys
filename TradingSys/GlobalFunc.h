#pragma once
#include <stdio.h>
#include <Windows.h>

namespace GlobalFunc
{

	extern HINSTANCE hdll;//ȫ��dll���.

	FILE *F_OPEN(char *fname, char *mode, int log_no, char *fdate);

	float LoadGetLevel2PxDll(char *tk, int direction, int index);

	void LoadDll(char *dllname);

	void LoadSubscribeData();

}