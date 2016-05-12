#ifndef _MEC_HUGE_H_
#define _MEC_HUGE_H_
#include <stdio.h>
#include <malloc.h>

#define TRUE	1
#define FALSE	0

#define POSITIVE   0
#define NEGATIVE   1
#define SIGN_NOT_IN_FILE		0x00
#define SIGN_IN_FILE			0x80
#define NOT_FOUND_INDEX	  -1

#define isPositive(sign)    (((sign) & 0x01) == POSITIVE)
#define isSignInFile(sign)  (((sign) & 0x80) == SIGN_IN_FILE)     
#define setSign(sign, value)		(((value) == POSITIVE) ? (sign &= ~NEGATIVE) : (sign |= NEGATIVE))
#define setSignInFile(sign, value)	(((value) == SIGN_IN_FILE) ? (sign |= SIGN_IN_FILE) : (sign &= ~SIGN_IN_FILE))

typedef unsigned char boolean;
typedef unsigned int u32;

typedef struct MEC_HUGE{
	boolean sign;
	u32 *intValue;
	u32 *decValue;
	u32 intLength;
	u32 decLength;
}MEC_HUGE;

boolean initHuge(MEC_HUGE **hugeNumber);
void destoryHuge(MEC_HUGE **hugeNumber);
boolean getHugeNumberFromFile(MEC_HUGE *hugeNumber, const char *fileName);
boolean resetHugeNumber(MEC_HUGE *hugeNumber);
int findDot(FILE *fp);
void getIntValue(MEC_HUGE *hugeNumber, FILE *fp);
void showHugeNumber(MEC_HUGE hugeNumber);
void showHugeInt(MEC_HUGE hugeNumber);
void showHugeDec(MEC_HUGE hugeNumber);
void getDecValue(MEC_HUGE *hugeNumber, FILE *fpIn);
void getHugeLength(MEC_HUGE *hugeNumber, FILE *fpIn);

void getHugeLength(MEC_HUGE *hugeNumber, FILE *fpIn){
	int ch;
	int decLength = 0;
	int intLength;

	fseek(fpIn, 0L, SEEK_SET);
	ch = fgetc(fpIn);
	if(ch == '+' || ch == '-'){
		setSignInFile(hugeNumber->sign, SIGN_IN_FILE);
	}else{
		intLength++;
		setSignInFile(hugeNumber->sign, SIGN_NOT_IN_FILE);
	}
	ch = fgetc(fpIn);
	while(!feof(fpIn) && ch != '.'){
		intLength++;
		ch = fgetc(fpIn);
	}

	if(!feof(fpIn)){
		ch = fgetc(fpIn);
		while(!feof(fpIn)){
			if(isdigit(ch)){
				decLength++;
			}
		ch = fgetc(fpIn);
		}
	}
	rewind(fpIn);
	hugeNumber->intLength = intLength;
	hugeNumber->decLength = decLength;
}

void getDecValue(MEC_HUGE *hugeNumber, FILE *fpIn){
	int count = 0;
	int ch;
	u32 value = 0;
	int i = 0;

	hugeNumber->decValue = (u32 *)calloc(sizeof(u32), (hugeNumber->decLength+3)/4);

	ch = fgetc(fpIn);
	printf("ch = %c\n", ch);
	while(!feof(fpIn)){
		if(isdigit(ch)){
			value = value*10 + ch - '0';
			if(++count >= 4){
				hugeNumber->decValue[i++] = value;
				value = 0;
				count = 0;
			}
		}
		ch = fgetc(fpIn);
	}

	if(count < 4){
		hugeNumber->decValue[i] = value;
	}

}

void showHugeDec(MEC_HUGE hugeNumber){
	int i = 0;
	int length = (hugeNumber.decLength + 3) / 4;
	char *decFormat[] = {"%04d", "%d", "%02d", "%03d"};
	for(; i < length - 1; i++){
		printf("%04d", hugeNumber.decValue[i]);
	}
	printf(decFormat[(hugeNumber.decLength - 1) % 4], hugeNumber.decValue[i]);

}

void showHugeInt(MEC_HUGE hugeNumber){
	int i;
	i = (hugeNumber.intLength + 3) / 4 - 1;
	printf("%d", hugeNumber.intValue[i]);
	for(i--; i >= 0; i--){
		printf("%04d", hugeNumber.intValue[i]);
	}
}

void showHugeNumber(MEC_HUGE hugeNumber){
	printf(hugeNumber.sign == NEGATIVE ? "-" : "");
	showHugeInt(hugeNumber);
	if(hugeNumber.decLength > 0){
		putchar('.');
		showHugeDec(hugeNumber);
	}
}

void getIntValue(MEC_HUGE *hugeNumber, FILE *fp){
	int intIndex;
	int value = 0;
	int ch;
	int count;

	intIndex = (hugeNumber->intLength+3) / 4;
	count = hugeNumber->intLength % 4;
	hugeNumber->intValue = (u32 *)calloc(sizeof(u32), intIndex+1);

	if(hugeNumber->sign == NEGATIVE){
		hugeNumber->intValue[intIndex] = 9999;
	}
	--intIndex;

	while(intIndex >= 0){
		ch = fgetc(fp);
		value = value*10 + ch - '0';
		count--;
		if(count <= 0){
			hugeNumber->intValue[intIndex] = value;
			value = 0;
			count = 4;
			intIndex--;
		}
	}
}

int findDot(FILE *fp){
	int position = 0;
	int ch;

	rewind(fp);
	ch = fgetc(fp);
	while(!feof(fp)){
		if(ch == '.'){
			rewind(fp);

			return position;
		}
		position++;
		ch = fgetc(fp);
	}
	rewind(fp);
	return position;
}

boolean resetHugeNumber(MEC_HUGE *hugeNumber){
	if(hugeNumber == NULL){

		return FALSE;
	}
	if(hugeNumber->intValue != NULL){
		free(hugeNumber->intValue);
		hugeNumber->intValue = NULL;
	}

	if(hugeNumber->decValue != NULL){
		free(hugeNumber->decValue);
		hugeNumber->decValue = NULL;
	}

	hugeNumber ->decLength = hugeNumber ->intLength = 0;
	hugeNumber ->sign = POSITIVE;
	
	return TRUE;
}

boolean getHugeNumberFromFile(MEC_HUGE *hugeNumber, const char *fileName){
	FILE *fpIn;
	int dotPosition;  //
	int ch;

	fpIn = fopen(fileName, "rt");
	if(fpIn == NULL){

		return FALSE;
	}

	resetHugeNumber(hugeNumber);
	getHugeLength(hugeNumber, fpIn);

	ch = fgetc(fpIn);
	if(isdigit(ch)){
		hugeNumber->sign = POSITIVE;
		ungetc(ch, fpIn);     //把字符退回到输入流，执行成功，返回非零值
	}else{
		hugeNumber->sign =(ch == '+') ? POSITIVE : NEGATIVE;
	}

	getIntValue(hugeNumber, fpIn);

	return TRUE;
}

void destoryHuge(MEC_HUGE **hugeNumber){
	if((*hugeNumber) == NULL){

		return;
	}

	if((*hugeNumber) -> intValue != NULL){
		free((*hugeNumber) -> intValue);
	}

	if((*hugeNumber) -> decValue != NULL){
		free((*hugeNumber) -> decValue);
	}

	free(hugeNumber);
	*hugeNumber = NULL;
}

boolean initHuge(MEC_HUGE **hugeNumber){
	if(*hugeNumber){

		return FALSE;
	}

	if(((*hugeNumber) = (MEC_HUGE *)calloc(sizeof(MEC_HUGE), 1)) == NULL){
	
	}

	return TRUE;
}
#endif


