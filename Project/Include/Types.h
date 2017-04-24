#pragma once

#include <stdint.h>
#include <functional>
#include <string>
#include <vector>
#include <map>

namespace XO
{
typedef int8_t   int8;
typedef int16_t  int16;
typedef int32_t  int32;
typedef int64_t  int64;
typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef float float32;
typedef double float64;

#ifdef GAME_WIN64
typedef int64 intptr;
#else
typedef int32 intptr;
#endif

typedef std::string String;
typedef std::wstring WString;

template<typename T>
using Array = std::vector<T>;

template<typename S, typename T>
using Map = std::map<S, T>;

template<typename T>
using Lambda = std::function<T>;

typedef Lambda<void()> Lambda0;
}