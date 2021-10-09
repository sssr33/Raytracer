#include "StdAfx.h"
#include "AlphaBlendStrategy.h"


void MyAlphaBlend10(unsigned int *dstColor, unsigned int *srcColor);

AlphaBlendSSE32BitStrategy::AlphaBlendSSE32BitStrategy()
{
	/*for(int i = 0; i < 4; i++){
		ADD255[i] = 255;
		MASKALPHA[i] = 0xFF000000;
		MASKRED[i] = 0x00FF0000;
		MASKGREEN[i] = 0x0000FF00;
		MASKBLUE[i] = 0x000000FF;
	}*/
}

void AlphaBlendSSE32BitStrategy::AlphaBlend(void *destination, void *source, unsigned int numPixels){
	unsigned int *dest = (unsigned int *)destination;
	unsigned int *sour = (unsigned int *)source;

	if(numPixels & 4){
		Blend4Pixels(dest, sour);
		//MyAlphaBlend8(dest, sour);
		//Blend1Pixel(dest, sour);
		//Blend1Pixel(dest + 1, sour + 1);
		//Blend1Pixel(dest + 2, sour + 2);
		//Blend1Pixel(dest + 3, sour + 3);
	}
	if(numPixels & 2){
		Blend2Pixels(dest, sour);
		dest += 2;
		sour += 2;
	}
	if(numPixels & 1){
		Blend1Pixel(dest, sour);
	}
}

void AlphaBlendSSE32BitStrategy::Blend4Pixels(unsigned int *destination, unsigned int *source){
	__declspec(align(16))unsigned int ADD256[4] = {256, 256, 256, 256};
	__declspec(align(16))unsigned int MASKALPHA[4] = {  0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000};
	__declspec(align(16))unsigned int MASKRED[4] = {  0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000};
	__declspec(align(16))unsigned int MASKGREEN[4] = {0x0000FF00, 0x0000FF00, 0x0000FF00, 0x0000FF00};
	__declspec(align(16))unsigned int MASKBLUE[4] = { 0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF};

	// TODO rewrite with intrinsics and plain C/C++
	//__asm{
	//	mov esi, source
	//	//movaps xmm7, MASKALPHA

	//	mov edi, destination
	//	movups xmm2, [esi]
	//	
	//	movups xmm4, [edi]
	//	pand xmm2, MASKALPHA
	//	
	//	movups xmm6, [esi]
	//	psrld xmm2, 24

	//	//movd xmm2, alpha
	//	//pshufd xmm2, xmm2, 0

	//	//movups xmm4, [edi]
	//	//movups xmm6, [esi]

	//	//psrldq xmm7, 1
	//	movaps xmm0, xmm4

	//	pand xmm0, MASKRED
	//	movaps xmm1, xmm6

	//	pand xmm1, MASKRED
	//	psrld xmm0, 16  //pure red channel of 4 pixels

	//	movaps xmm5, xmm4 // load green
	//	psrld xmm1, 16  //pure red channel of 4 pixels

	//	paddd xmm1, ADD256

	//	movaps xmm3, xmm6 // load green
	//	psubd xmm1, xmm0

	//	pand xmm5, MASKGREEN
	//	pmaddwd xmm1, xmm2

	//	pand xmm3, MASKGREEN
	//	psrld xmm5, 8  //green

	//	psrld xmm3, 8 //green
	//	nop

	//	paddd xmm3, ADD256

	//	psrld xmm1, 8
	//	psubd xmm3, xmm5

	//	pmaddwd xmm3, xmm2
	//	paddd xmm1, xmm0

	//	psubd xmm1, xmm2//red

	//	pslld xmm1, 16
	//	movaps xmm0, xmm4

	//	pand xmm0, MASKBLUE
	//	psrld xmm3, 8

	//	paddd xmm3, xmm5
	//	movaps xmm4, xmm6

	//	psubd xmm3, xmm2

	//	pand xmm4, MASKBLUE
	//	pslld xmm3, 8

	//	paddd xmm4, ADD256

	//	psubd xmm4, xmm0
	//	por xmm1, xmm3

	//	pmaddwd xmm4, xmm2

	//	psrld xmm4, 8

	//	paddd xmm4, xmm0

	//	psubd xmm4, xmm2

	//	por xmm1, xmm4

	//	movups [edi], xmm1
	//}
}

void AlphaBlendSSE32BitStrategy::Blend1Pixel(unsigned int *destination, unsigned int *source){
	// TODO rewrite with intrinsics and plain C/C++
	/*__asm{
		mov esi, source
		mov edi, destination

		mov eax, [esi]
		shr eax, 24
		mov edx, 256
		inc eax
		sub edx, eax
		movd xmm2, edx
		movd xmm3, eax
		movd xmm0, [edi]
		movd xmm1, [esi]
		pshufd xmm2, xmm2, 0
		pshufd xmm3, xmm3, 0
		pxor xmm4, xmm4

		punpcklbw xmm0, xmm4
		punpcklbw xmm1, xmm4
		punpcklwd xmm0, xmm4
		punpcklwd xmm1, xmm4

		pmaddwd xmm0, xmm2
		pmaddwd xmm1, xmm3

		psrld xmm0, 8
		psrld xmm1, 8

		paddd xmm0, xmm1


		packssdw xmm0, xmm4
		packuswb xmm0, xmm4

		movd [edi], xmm0

	}*/
}

void MyAlphaBlend10(unsigned int *dstColor, unsigned int *srcColor){
	__declspec(align(16))unsigned int ADD255[4] = {255, 255, 255, 255};
	__declspec(align(16))unsigned int MASKALPHA[4] = {  0xFF000000, 0xFF000000, 0xFF000000, 0xFF000000};
	__declspec(align(16))unsigned int MASKRED[4] = {  0x00FF0000, 0x00FF0000, 0x00FF0000, 0x00FF0000};
	__declspec(align(16))unsigned int MASKGREEN[4] = {0x0000FF00, 0x0000FF00, 0x0000FF00, 0x0000FF00};
	__declspec(align(16))unsigned int MASKBLUE[4] = { 0x000000FF, 0x000000FF, 0x000000FF, 0x000000FF};

	// TODO rewrite with intrinsics and plain C/C++
	//__asm{
	//	mov esi, srcColor
	//	//movaps xmm7, MASKALPHA

	//	mov edi, dstColor
	//	movups xmm2, [esi]
	//	
	//	movups xmm4, [edi]
	//	pand xmm2, MASKALPHA
	//	
	//	movups xmm6, [esi]
	//	psrld xmm2, 24

	//	//movd xmm2, alpha
	//	//pshufd xmm2, xmm2, 0

	//	//movups xmm4, [edi]
	//	//movups xmm6, [esi]

	//	//psrldq xmm7, 1
	//	movaps xmm0, xmm4

	//	pand xmm0, MASKRED
	//	movaps xmm1, xmm6

	//	pand xmm1, MASKRED
	//	psrld xmm0, 16  //pure red channel of 4 pixels

	//	movaps xmm5, xmm4 // load green
	//	psrld xmm1, 16  //pure red channel of 4 pixels

	//	paddd xmm1, ADD255

	//	movaps xmm3, xmm6 // load green
	//	psubd xmm1, xmm0

	//	pand xmm5, MASKGREEN
	//	pmaddwd xmm1, xmm2

	//	pand xmm3, MASKGREEN
	//	psrld xmm5, 8  //green

	//	psrld xmm3, 8 //green
	//	nop

	//	paddd xmm3, ADD255

	//	psrld xmm1, 8
	//	psubd xmm3, xmm5

	//	pmaddwd xmm3, xmm2
	//	paddd xmm1, xmm0

	//	psubd xmm1, xmm2//red

	//	pslld xmm1, 16
	//	movaps xmm0, xmm4

	//	pand xmm0, MASKBLUE
	//	psrld xmm3, 8

	//	paddd xmm3, xmm5
	//	movaps xmm4, xmm6

	//	psubd xmm3, xmm2

	//	pand xmm4, MASKBLUE
	//	pslld xmm3, 8

	//	paddd xmm4, ADD255

	//	psubd xmm4, xmm0
	//	por xmm1, xmm3

	//	pmaddwd xmm4, xmm2

	//	psrld xmm4, 8

	//	paddd xmm4, xmm0

	//	psubd xmm4, xmm2

	//	por xmm1, xmm4

	//	movups [edi], xmm1
	//}
}
