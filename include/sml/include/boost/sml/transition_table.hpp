//
// Copyright (c) 2016-2018 Kris Jusiak (kris at jusiak dot net)
//
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef BOOST_SML_TRANSITION_TABLE_HPP
#define BOOST_SML_TRANSITION_TABLE_HPP

#include "boost/sml/aux_/type_traits.hpp"
#include "boost/sml/back/internals.hpp"
#include "boost/sml/concepts/transitional.hpp"
#include "boost/sml/front/actions/defer.hpp"
#include "boost/sml/front/actions/process.hpp"
#include "boost/sml/front/event.hpp"
#include "boost/sml/front/operators.hpp"
#include "boost/sml/front/state.hpp"
#include "boost/sml/front/transition.hpp"

using _ = back::_;

/// events

template <class TEvent>
front::event<TEvent> event __BOOST_SML_VT_INIT;

template <class TEvent>
__BOOST_SML_UNUSED front::event<back::on_entry<_, TEvent>> on_entry __BOOST_SML_VT_INIT;

template <class TEvent>
__BOOST_SML_UNUSED front::event<back::on_exit<_, TEvent>> on_exit __BOOST_SML_VT_INIT;

template <class TEvent>
front::event<back::unexpected_event<TEvent>> unexpected_event __BOOST_SML_VT_INIT;

template <class T>
front::event<back::exception<T>> exception __BOOST_SML_VT_INIT;

using anonymous = back::anonymous;
using initial = back::initial;

/// states

template <class T>
typename front::state_sm<T>::type state __BOOST_SML_VT_INIT;

#if !defined(_MSC_VER)  // __pph__
template <class T, T... Chrs>
auto operator""_s() {
  return front::state<aux::string<T, Chrs...>>{};
}
template <class T, T... Chrs>
auto operator""_e() {
  return event<aux::string<T, Chrs...>>;
}
#endif  // __pph__

__BOOST_SML_UNUSED static front::state<back::terminate_state> X;
__BOOST_SML_UNUSED static front::history_state H;

/// actions

__BOOST_SML_UNUSED static front::actions::defer defer;
__BOOST_SML_UNUSED static front::actions::process process;

/// transition table

template <class... Ts, __BOOST_SML_REQUIRES(aux::is_same<aux::bool_list<aux::always<Ts>::value...>,
                                                         aux::bool_list<concepts::transitional<Ts>::value...>>::value)>
auto make_transition_table(Ts... ts) {
  return aux::pool<Ts...>{ts...};
}

#endif
