/*
Copyright (c) RELEX Oy

All rights reserved.

Source made public only to facilitate research and bug reproduction in WASM, Esmcripten, C# Runtimes such as Blazor, Angular and WebGL.
*/

#ifndef __CALLBACKS_H_INCLUDED__
#define __CALLBACKS_H_INCLUDED__

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#endif

EM_BOOL key_callback(int eventType, const EmscriptenKeyboardEvent* e, void* userData);
EM_BOOL mouse_callback(int eventType, const EmscriptenMouseEvent* e, void* userData);

#endif // !__CALLBACKS_H_INCLUDED__