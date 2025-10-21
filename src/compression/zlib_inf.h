#pragma once

//#include "../memr/memr.hpp"
#include "../util/umem.h"
#include <assert.h>
#include <stdio.h>
#include <zlib.h>

#ifndef ZLIB_INF__
#define ZLIB_INF__

//TODO: is this needed?
#ifdef _WIN32
//#include "io.h"
#endif

#ifndef CHUNK
#	define CHUNK 16384
#endif

i32 zlib_inf(UMEM* src, UMEM* dst);

#endif