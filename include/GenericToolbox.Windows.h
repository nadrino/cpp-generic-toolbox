#pragma once

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
// The toolbox uses console/filesystem APIs, not GDI (which defines ERROR).
#ifndef NOGDI
#define NOGDI
#endif
#include <Windows.h>
#endif
