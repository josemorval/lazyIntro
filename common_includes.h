// common_includes.h
#ifndef COMMON_INCLUDES_H
#define COMMON_INCLUDES_H

#ifdef _DEBUG
#include <stdio.h>
#include <windows.h>

//Some definitions for console
#define RESET   "\033[0m"
#define BLACK   "\033[1;30m"
#define RED     "\033[1;31m"
#define GREEN   "\033[1;32m"
#define YELLOW  "\033[1;33m"
#define BLUE    "\033[1;34m"
#define MAGENTA "\033[1;35m"
#define CYAN    "\033[1;36m"
#define WHITE   "\033[1;37m"
#define BOLD    "\033[1m"

// Colores de fondo básicos (ANSI 16 colores)
#define BG_BLACK   "\033[40m"
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN    "\033[46m"
#define BG_WHITE   "\033[47m"
#endif

#include <d3d11.h>
#include <d3dcompiler.h>
#include "shader_helper.h"

#ifndef _DEBUG
#include "generated_shader.h"
#endif

#include "libmem.h"
#include "libdevice.h"
#include "lib3d.h"
#include "logic.h"

#endif // COMMON_INCLUDES_H