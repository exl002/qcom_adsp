/***************************************************************************
* Copyright (c) Date: Mon Nov 24 16:25:59 CST 2008 Qualcomm Technologies INCORPORATED 
* All Rights Reserved 
* Modified by Qualcomm Technologies INCORPORATED on Mon Nov 24 16:25:59 CST 2008 
****************************************************************************/ 


int32 bitrev_s32( int32 x, int32 BITS );

void ButterflyRadix2_c64 ( cint2x32 *x );
void ButterflyRadix4_c64 ( cint2x32 *x );

void ifftButterflyRadix2_c64 ( cint2x32 *x );
void ifftButterflyRadix4_c64 ( cint2x32 *x );



void ButterflyRadix2_c32( cint2x16 *x );
void ButterflyRadix4qv3_c32( cint2x16 *x );
void ButterflyRadix4_c32( cint2x16 *x );

// scaled version 
void sButterflyRadix2_c32( cint2x16 *x );
void sButterflyRadix4_c32( cint2x16 *x );
void sButterflyRadix4qv3_c32( cint2x16 *x );

void ifftButterflyRadix2_c32( cint2x16 *x );
void ifftButterflyRadix4_c32( cint2x16 *x );
void ifftButterflyRadix4qv3_c32( cint2x16 *x );
