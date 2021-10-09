// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>


// TODO: reference additional headers your program requires here
#define ARGB32BIT(a,r,g,b) (b + (g << 8) + (r << 16) + (a << 24))
#define RGBFROM32BIT(color, r, g, b) { r = static_cast<decltype(r)>((color >> 16) & 0x000000FF); g = static_cast<decltype(g)>((color >> 8) & 0x000000FF); b = static_cast<decltype(b)>(color & 0x000000FF); }
#define SWAP(a,b,t) {t=static_cast<decltype(t)>(a); a=static_cast<decltype(a)>(b); b=static_cast<decltype(b)>(t);}