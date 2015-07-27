#if !defined STATIC_CONTROL_WRAP_HPP_INCLUDED_
#define      STATIC_CONTROL_WRAP_HPP_INCLUDED_

namespace desalt { namespace static_control {
namespace here = static_control;

template<typename T>
struct wrap {
    using type = T;
};

}}

#endif
