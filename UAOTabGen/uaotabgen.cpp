#include <windows.h>
#include <stdio.h>
#include "Ucs2Conv.h"

int main(int argc, char* argv[])
{
	CUcs2Conv conv;
	conv.InitBig52Ucs2Tab();
	conv.InitUcs22Big5Tab();

	printf("This program is used to generate condensed B2U table\n");

	FILE *fo = fopen("B2U", "wb");
	if (!fo)
		return 1;
	int ch1, ch2;
	/*
		for( ch1 = 0x81; ch1 <= 0xFE; ++ch1 )
		{
			for( ch2 = 0x40; ch2 <= 0xFE; ++ch2 )
			{
				char big5[3] = { ch1, ch2, '\0' };
				wchar_t ucs2 = 0, sys = 0;
				conv.Big52Ucs2( big5, &ucs2, 2 );
				if( ucs2 < 128 || ucs2 == 0x003F )
					continue;
				MultiByteToWideChar( 950, 0, big5, 2, &sys, 1 );
				if( ucs2 && ucs2 == sys || sys==0x003f || sys==0xf8f8 )
					continue;
				printf( "big5 = 0x%02X%02X ==> ucs2(UAO) = 0x%04X, windows=0x%04X\n", ch1, ch2, (int)ucs2, (int)sys );
				fwrite( &ucs2, 1, 2, fo );
			}
		}
	*/
	// Generate condensed Big5 to UCS2 table
	printf("Generating condensed B2U...\n");
	for (ch1 = 0x81; ch1 <= 0xA0; ++ch1)	// 0x8140 - 0xA0FE
	{
		for (ch2 = 0x40; ch2 <= 0x7E; ++ch2)
		{
			char big5[3] = { ch1, ch2, '\0' };
			wchar_t ucs2 = 0;
			conv.Big52Ucs2(big5, &ucs2, 2);
			printf("big5 = 0x%02X%02X ==> ucs2(UAO) = 0x%04X\n", ch1, ch2, (int)ucs2);
			fwrite(&ucs2, 1, 2, fo);
		}
		for (ch2 = 0xA1; ch2 <= 0xFE; ++ch2)
		{
			char big5[3] = { ch1, ch2, '\0' };
			wchar_t ucs2 = 0;
			conv.Big52Ucs2(big5, &ucs2, 2);
			printf("big5 = 0x%02X%02X ==> ucs2(UAO) = 0x%04X\n", ch1, ch2, (int)ucs2);
			fwrite(&ucs2, 1, 2, fo);
		}
	}

	for (ch1 = 0xC6; ch1 <= 0xC8; ++ch1)	// 0xC640 - 0xC8FE
	{
		for (ch2 = 0x40; ch2 <= 0x7E; ++ch2)
		{
			char big5[3] = { ch1, ch2, '\0' };
			wchar_t ucs2 = 0;
			conv.Big52Ucs2(big5, &ucs2, 2);
			printf("big5 = 0x%02X%02X ==> ucs2(UAO) = 0x%04X\n", ch1, ch2, (int)ucs2);
			fwrite(&ucs2, 1, 2, fo);
		}
		for (ch2 = 0xA1; ch2 <= 0xFE; ++ch2)
		{
			char big5[3] = { ch1, ch2, '\0' };
			wchar_t ucs2 = 0;
			conv.Big52Ucs2(big5, &ucs2, 2);
			printf("big5 = 0x%02X%02X ==> ucs2(UAO) = 0x%04X\n", ch1, ch2, (int)ucs2);
			fwrite(&ucs2, 1, 2, fo);
		}
	}

	for (ch1 = 0xFA; ch1 <= 0xFE; ++ch1)	// 0xFA40 - 0xFEFE
	{
		for (ch2 = 0x40; ch2 <= 0x7E; ++ch2)
		{
			char big5[3] = { ch1, ch2, '\0' };
			wchar_t ucs2 = 0;
			conv.Big52Ucs2(big5, &ucs2, 2);
			printf("big5 = 0x%02X%02X ==> ucs2(UAO) = 0x%04X\n", ch1, ch2, (int)ucs2);
			fwrite(&ucs2, 1, 2, fo);
		}
		for (ch2 = 0xA1; ch2 <= 0xFE; ++ch2)
		{
			char big5[3] = { ch1, ch2, '\0' };
			wchar_t ucs2 = 0;
			conv.Big52Ucs2(big5, &ucs2, 2);
			printf("big5 = 0x%02X%02X ==> ucs2(UAO) = 0x%04X\n", ch1, ch2, (int)ucs2);
			fwrite(&ucs2, 1, 2, fo);
		}
	}

	fclose(fo);


	// Generate condensed UCS2 to Big5 table
	printf("Generating condensed U2B...\n");

	fo = fopen("U2B", "wb");
	if (!fo)
		return 1;

	wchar_t ucs2[2] = {0};
	for (ucs2[0] = 0x0081; ucs2[0] <= 0xFFFE; ++ucs2[0])
	{
		char big5[3] = {0}, sys[3] = {0};
		conv.Ucs22Big5(ucs2, big5);
		big5[2] = '\0';
		WideCharToMultiByte(950, 0, ucs2, 1, sys, 3, "\xff\xfd", NULL);
		if (0 == strcmp(big5, sys) || 0 == strcmp(big5, "\xff\xfd"))
			continue;
		printf("ucs2(UAO) = 0x%04X ==> big5 = 0x%02X%02X, windows=0x%02X%02X\n", (int)ucs2[0], BYTE(big5[0]), BYTE(big5[1]), BYTE(sys[0]), BYTE(sys[1]));
		fwrite(ucs2, 1, 2, fo);
		fwrite(big5, 1, 2, fo);
	}

	fclose(fo);

	printf("Press \"Enter\" to continue");
	getchar();
	return 0;
}
