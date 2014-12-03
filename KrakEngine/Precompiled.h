#pragma once

#include <string>
#include <sstream>
#include <map>
#include <set>
#include <vector>
#include <list>
#include <algorithm>
#include <io.h>
#include <hash_map>
#include <locale>
#include <codecvt>
#include <wrl\client.h> // Include for ComPtr.

#include "tinyXML2\tinyxml2.h"

#include "TweakBar\include\AntTweakBar.h"

#include "DirectXHeaders.h"
#include "DebugDiagnostic.h"


#include "alglib\src\alglibinternal.h"
#include "alglib\src\alglibmisc.h"
#include "alglib\src\linalg.h"
#include "alglib\src\statistics.h"
#include "alglib\src\dataanalysis.h"
#include "alglib\src\specialfunctions.h"
#include "alglib\src\solvers.h"
#include "alglib\src\optimization.h"
#include "alglib\src\diffequations.h"
#include "alglib\src\fasttransforms.h"
#include "alglib\src\integration.h"
#include "alglib\src\interpolation.h"

#define ComPtr Microsoft::WRL::ComPtr

//Collision flags
#define	COLLISION_LEFT		0x00000001	//0001
#define	COLLISION_RIGHT		0x00000002	//0010
#define	COLLISION_TOP		0x00000004	//0100
#define	COLLISION_BOTTOM	0x00000008	//1000

template< typename T >
void SafeRelease(T& ptr)
{
    if (ptr) ptr->Release();
    ptr = nullptr;
}

template< typename T >
void SafeDelete(T& ptr)
{
    if (ptr) delete ptr;
    ptr = NULL;
}
