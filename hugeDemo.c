#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <ctype.h>

#include "huge.h"

void main(int argc, char **argv){
	MEC_HUGE *hugeNumber = NULL;
	char fileName[80];

	if(argc < 2){
		puts("�÷�: hugeDemo <�޴��������ļ���>");

		return;
	}

	strcpy(fileName, argv[1]);
	initHuge(&hugeNumber);

	getHugeNumberFromFile(hugeNumber, fileName);
	puts("\n�޴���Ϊ:");
	showHugeNumber(*hugeNumber);

	destoryHuge(&hugeNumber);
}