#pragma once
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <unordered_map>
#include <unordered_set>
#include <memory>

using u8  = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;

using i8  = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;

template <typename K, typename V>
using umap = std::unordered_map<K,V>;

template <typename T>
using uset = std::unordered_set<T>;

template <typename T>
using sp = std::shared_ptr<T>;