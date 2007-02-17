#include <windows.h>
#include <stdio.h>
#include "Ucs2Conv.h"

int main( int argc, char* argv[] )
{
	CUcs2Conv conv;
	conv.InitBig52Ucs2Tab();
	conv.InitUcs22Big5Tab();

	printf("This program is used to generate condensed B2U table\n");

	FILE *fo = fopen( "B2U", "wb" );
	if( !fo )
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
	for( ch1 = 0x81; ch1 <= 0xA0; ++ch1 )	// 0x8140 - 0xA0FE
	{
		for( ch2 = 0x40; ch2 <= 0x7E; ++ch2 )
		{
			char big5[3] = { ch1, ch2, '\0' };
			wchar_t ucs2 = 0;
			conv.Big52Ucs2( big5, &ucs2, 2 );
			printf( "big5 = 0x%02X%02X ==> ucs2(UAO) = 0x%04X\n", ch1, ch2, (int)ucs2 );
			fwrite( &ucs2, 1, 2, fo );
		}
		for( ch2 = 0xA1; ch2 <= 0xFE; ++ch2 )
		{
			char big5[3] = { ch1, ch2, '\0' };
			wchar_t ucs2 = 0;
			conv.Big52Ucs2( big5, &ucs2, 2 );
			printf( "big5 = 0x%02X%02X ==> ucs2(UAO) = 0x%04X\n", ch1, ch2, (int)ucs2 );
			fwrite( &ucs2, 1, 2, fo );
		}
	}

	for( ch1 = 0xC6; ch1 <= 0xC8; ++ch1 )	// 0xC640 - 0xC8FE
	{
		for( ch2 = 0x40; ch2 <= 0x7E; ++ch2 )
		{
			char big5[3] = { ch1, ch2, '\0' };
			wchar_t ucs2 = 0;
			conv.Big52Ucs2( big5, &ucs2, 2 );
			printf( "big5 = 0x%02X%02X ==> ucs2(UAO) = 0x%04X\n", ch1, ch2, (int)ucs2 );
			fwrite( &ucs2, 1, 2, fo );
		}
		for( ch2 = 0xA1; ch2 <= 0xFE; ++ch2 )
		{
			char big5[3] = { ch1, ch2, '\0' };
			wchar_t ucs2 = 0;
			conv.Big52Ucs2( big5, &ucs2, 2 );
			printf( "big5 = 0x%02X%02X ==> ucs2(UAO) = 0x%04X\n", ch1, ch2, (int)ucs2 );
			fwrite( &ucs2, 1, 2, fo );
		}
	}

	for( ch1 = 0xFA; ch1 <= 0xFE; ++ch1 )	// 0xFA40 - 0xFEFE
	{
		for( ch2 = 0x40; ch2 <= 0x7E; ++ch2 )
		{
			char big5[3] = { ch1, ch2, '\0' };
			wchar_t ucs2 = 0;
			conv.Big52Ucs2( big5, &ucs2, 2 );
			printf( "big5 = 0x%02X%02X ==> ucs2(UAO) = 0x%04X\n", ch1, ch2, (int)ucs2 );
			fwrite( &ucs2, 1, 2, fo );
		}
		for( ch2 = 0xA1; ch2 <= 0xFE; ++ch2 )
		{
			char big5[3] = { ch1, ch2, '\0' };
			wchar_t ucs2 = 0;
			conv.Big52Ucs2( big5, &ucs2, 2 );
			printf( "big5 = 0x%02X%02X ==> ucs2(UAO) = 0x%04X\n", ch1, ch2, (int)ucs2 );
			fwrite( &ucs2, 1, 2, fo );
		}
	}

	fclose( fo );
	printf( "Press \"Enter\" to continue" );
	getchar();
	return 0;
}
