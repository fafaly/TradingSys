#pragma once
#include <stdio.h>
#include <Windows.h>

namespace GlobalFunc
{
	FILE *F_OPEN(char *fname, char *mode, int log_no, char *fdate);

	float LoadGetLevel2PxDll(char *tk, int direction, int index);
}