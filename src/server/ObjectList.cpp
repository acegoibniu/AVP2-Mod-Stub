// ----------------------------------------------------------------------- //
//
// MODULE  : ObjectList.cpp
//
// PURPOSE : Registers BaseClass, the one engine class object.lto must
//           provide, and exports ObjectDLLSetup, which hands every class
//           registered with LT_CLASS to the engine.
//
//           A world would also need the engine's other built-in classes
//           (VisContainer, etc.); this demo never starts one.
//
// ----------------------------------------------------------------------- //

#include <windows.h>
#include <new>	// placement new, used by LT_ROOT_CLASS

#include "lt_server.h"

LT_DEFINE_CLASSES()

LT_ROOT_CLASS(BaseClass)
