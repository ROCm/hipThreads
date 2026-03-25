//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// UNSUPPORTED: no-threads
// UNSUPPORTED: windows

// This is a death test: it relies on the host std::terminate / set_terminate
// mechanism to turn a misuse (move-assigning onto a joinable thread) into a
// clean _Exit(0). That mechanism does not exist in GPU device code: there is no
// device-side terminate handler, and a device abort surfaces as an HSA hardware
// exception whose host exit code is not stable across runtimes (observed exit 0
// on RDNA/Linux but nonzero on CDNA/Linux and Windows). So this contract cannot
// be verified through an exit code on the GPU. Skip unconditionally.
// UNSUPPORTED: true

// <thread>

// class thread

// thread& operator=(thread&& t);

#include <hip/thread>
#include <cassert>
#include <cstdlib>
#include <exception>
#include <utility>

#include "make_test_thread.h"
#include "test_macros.h"

#include "force_include_hip.h"

struct G
{
    __device__ void operator()() { }
};

void f1()
{
    ::std::_Exit(0);
}

int main(int, char**)
{
#ifndef __HIP_DEVICE_COMPILE__
    ::std::set_terminate(f1);
#else
    {
        G g;
        hip::thread t0 = support::make_test_thread(g);
        hip::thread t1;
        t0 = ::std::move(t1);
        assert(false);
    }
#endif

    return 0;
}
