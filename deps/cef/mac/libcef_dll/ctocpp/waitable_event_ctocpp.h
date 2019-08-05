// Copyright (c) 2019 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
//
// ---------------------------------------------------------------------------
//
// This file was generated by the CEF translator tool. If making changes by
// hand only do so within the body of existing method and function
// implementations. See the translator.README.txt file in the tools directory
// for more information.
//
// $hash=4cd46b2c619b3f89830c4703169521c6f4ad6ee9$
//

#ifndef CEF_LIBCEF_DLL_CTOCPP_WAITABLE_EVENT_CTOCPP_H_
#define CEF_LIBCEF_DLL_CTOCPP_WAITABLE_EVENT_CTOCPP_H_
#pragma once

#if !defined(WRAPPING_CEF_SHARED)
#error This file can be included wrapper-side only
#endif

#include "include/capi/cef_waitable_event_capi.h"
#include "include/cef_waitable_event.h"
#include "libcef_dll/ctocpp/ctocpp_ref_counted.h"

// Wrap a C structure with a C++ class.
// This class may be instantiated and accessed wrapper-side only.
class CefWaitableEventCToCpp
    : public CefCToCppRefCounted<CefWaitableEventCToCpp,
                                 CefWaitableEvent,
                                 cef_waitable_event_t> {
 public:
  CefWaitableEventCToCpp();
  virtual ~CefWaitableEventCToCpp();

  // CefWaitableEvent methods.
  void Reset() OVERRIDE;
  void Signal() OVERRIDE;
  bool IsSignaled() OVERRIDE;
  void Wait() OVERRIDE;
  bool TimedWait(int64 max_ms) OVERRIDE;
};

#endif  // CEF_LIBCEF_DLL_CTOCPP_WAITABLE_EVENT_CTOCPP_H_
