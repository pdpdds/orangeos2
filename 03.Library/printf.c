/*
 * printf.c
 *
 *  Created on: 2012. 2. 26.
 *      Author: darkx
 */

#include "printf.h"
#include "util.h"
#include "Console.h"

extern Console OrangeConsole;


char sz[512] = {0,};

int printf(const char *format, ...)
{
		va_list ap;
		int i = 0;

		memset(sz, 0, 512);

		va_start(ap, format);
		vsprintf(sz, format, ap);
		va_end(ap);

		while (sz[i] != '\0') {
			if (sz[i] == '\n') {
				OrangeConsole.WriteChar('\n');
				i++;
				continue;
			} else if (sz[i] == '\b') {
				OrangeConsole.WriteChar('\b');
				i++;
				continue;
			}

			OrangeConsole.WriteChar(sz[i++]);
		}

		return i;
}


int print_hex(char *buf, int a, char flag)
{
	unsigned char *p;
	unsigned char v[10];
	int i=0, j=0;
	int count=0;

	if (a == 0) {
		*buf = 0x30;
		return 1;
	}

	p = (unsigned char *)&a;

	for (i = sizeof(int); i > 0; i--) {
		if ( (p[i-1] >> 4) <= 9)
			v[count] = (p[i-1] >> 4) + 0x30;
		else
			v[count] = (p[i-1] >> 4) + 0x37;
		count++;

		if ( (p[i-1] & 0x0f) <= 9)
			v[count] = (p[i-1] & 0x0f) + 0x30;
		else
			v[count] = (p[i-1] & 0x0f) + 0x37;
		count++;
	}


	i=0;
	while (v[i] == '0')	// skip initial zeros
		i++;

	count = 8 - i;	// how many digits
	for (j=0; j<count; j++)
		buf[j] = v[i++];

	return count;

}

void vsprintf(char *A_szString, const char *A_szFormat, va_list A_pAp)
{
		int temp;


		while (*A_szFormat != '\0') {
			// print decimal number
			if (*A_szFormat == '%' && *(A_szFormat+1) == 'd') {
				temp = va_arg(A_pAp, int);
				temp = print_decimal(A_szString, temp, *(A_szFormat+1));
				A_szString += temp;
				A_szFormat += 2;
			// print hexa-decimal number
			} else if (*A_szFormat == '%' && (*(A_szFormat+1) == 'x' || *(A_szFormat+1) == 'X')) {
				temp = va_arg(A_pAp, int);
				temp = print_hex(A_szString, temp, *(A_szFormat+1));
				A_szString += temp;
				A_szFormat += 2;
			// print scring
			} else if (*A_szFormat == '%' && *(A_szFormat+1) == 's') {

				char* szArg = va_arg(A_pAp, char *);
				temp = strlen(szArg);
				A_szString = strcat(A_szString, szArg);
				A_szString += temp;
				A_szFormat += 2;
			// print one character
			} else if (*A_szFormat == '%' && *(A_szFormat+1) == 'c') {
				*A_szString++ = va_arg(A_pAp, int);
				A_szFormat += 2;
			// normal character is copied as itself
			} else {
				*A_szString++ = *A_szFormat++;
			}
		}
		*A_szString = '\0';

}


int print_decimal(char *A_szBuffer, int a, char flag)
{
	unsigned char v[10];
	int i = 0;
	int count = 0;

	if (a == 0) {
		*A_szBuffer = 0x30;
		return 1;
	}

	/* print signed integer */
	if (a & 0x80000000) {
		/* print minus */
		*A_szBuffer = '-';
		count++;
		a = -a;
	}

	while (a > 0) {
		v[i++] = a % 10;
		a /= 10;
	}

	for (--i; i >= 0; i--) {
		*A_szBuffer++ = v[i]+0x30;
		count++;
	}


	return count;
}
