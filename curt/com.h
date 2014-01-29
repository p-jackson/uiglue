//===-- Use boost::intrusive_ptr with COM ---------------------------------===//
//
// Copyright (c) 2013 Philip Jackson
// This file may be freely distributed under the MIT license.
//
//===----------------------------------------------------------------------===//

#ifndef CURT_COM_H
#define CURT_COM_H

#include <boost/intrusive_ptr.hpp>
#include <Unknwn.h>

// boost::intrusive_ptr uses ADL to find these methods.

inline void intrusive_ptr_add_ref(IUnknown* p) {
  p->AddRef();
}

inline void intrusive_ptr_release(IUnknown* p) {
  p->Release();
}

namespace curt {

namespace detail {

  // Wrapper used by the getOut() function.
  template <class I>
  class ComPtrWrapper {
    boost::intrusive_ptr<I>& m_ref;
    I* m_ptr;

  public:
    explicit ComPtrWrapper(boost::intrusive_ptr<I>& ref)
        : m_ref(ref), m_ptr{ nullptr } {
    }

    ~ComPtrWrapper() {
      m_ref = boost::intrusive_ptr<I>{ m_ptr, false };
    }

    ComPtrWrapper& operator=(const ComPtrWrapper&) = delete;

    operator I**() {
      return &m_ptr;
    }

    operator void**() {
      return reinterpret_cast<void**>(&m_ptr);
    }
  };

} // end namespace

// getOut() implements the COM semantics for returning new objects using out
// parameters i.e. the interface has already been ref'd
// usage:
//   boost::intrusive_ptr<ISomeInterface> obj;
//   SomeCOMFactory(curt::getOut(obj));
template <class I>
detail::ComPtrWrapper<I> getOut(boost::intrusive_ptr<I>& ptr) {
  return detail::ComPtrWrapper<I>{ ptr };
}

} // end namespace curt

#endif
