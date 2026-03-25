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
// mechanism to turn a misuse (destroying a joinable thread) into a clean
// _Exit(0). That mechanism does not exist in GPU device code: there is no
// device-side terminate handler, and a device abort surfaces as an HSA hardware
// exception whose host exit code is not stable across runtimes (observed exit 0
// on RDNA/Linux but nonzero on CDNA/Linux and Windows). So this contract cannot
// be verified through an exit code on the GPU. Skip unconditionally.
// UNSUPPORTED: true


// <thread>

// class thread

// ~thread();

#include <cassert>
#include <cstdlib>
#include <exception>
#include <new>
#include <hip/thread>

#include "make_test_thread.h"
#include "test_macros.h"

#include "force_include_hip.h"

class G
{
    int alive_;
public:
    static __device__ int n_alive;
    static __device__ bool op_run;

    __device__ G() : alive_(1) {++n_alive;}
    __device__ G(const G& g) : alive_(g.alive_) {++n_alive;}
    __device__ ~G() {alive_ = 0; --n_alive;}

    __device__ void operator()()
    {
        assert(alive_ == 1);
        assert(n_alive >= 1);
        op_run = true;
    }
};

__device__ int G::n_alive = 0;
__device__ bool G::op_run = false;

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
        assert(G::n_alive == 0);
        assert(!G::op_run);
        G g;
        {
          hip::thread t = support::make_test_thread(g);
          hip::this_thread::sleep_for(cuda::std::chrono::milliseconds(250));
        }
    }
    assert(false);
#endif

  return 0;
}
