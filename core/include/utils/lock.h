#pragma once

#pragma once
#include <stdint.h>

extern "C" bool atomic_lock(uint64_t* mutex, uint64_t bit);
extern "C" bool atomic_unlock(uint64_t* mutex, uint64_t bit);
extern "C" void atomic_spinlock(uint64_t* mutex, uint64_t bit);

#define define_spinlock(name) static uint64_t name = 0;

#define atomic_acquire_spinlock(name) atomic_spinlock(&name, 0); atomic_lock(&name, 0);
#define atomic_release_spinlock(name) atomic_unlock(&name, 0);