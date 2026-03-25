//===----------------------------------------------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// UNSUPPORTED: no-threads

// <thread>

// class thread

// thread();

#include <hip/thread>
#include <cassert>
#include <cstdlib>

#include "test_macros.h"

#include "force_include_hip.h"

int main(int, char**)
{
    hip::thread t;
    assert(t.get_id() == hip::thread::id());

#if defined(_WIN32) && !defined(__HIP_DEVICE_COMPILE__)
    ::std::_Exit(0);
#endif
  return 0;
}
