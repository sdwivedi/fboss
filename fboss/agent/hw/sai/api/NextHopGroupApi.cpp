/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "fboss/agent/hw/sai/api/NextHopGroupApi.h"

#include <boost/functional/hash.hpp>

#include <functional>

namespace std {
size_t hash<facebook::fboss::SaiNextHopGroupTraits::AdapterHostKey>::operator()(
    const facebook::fboss::SaiNextHopGroupTraits::AdapterHostKey& k) const {
  size_t seed = 0;
  for (const auto& p : k) {
    boost::hash_combine(seed, std::get<0>(p).value());
    boost::hash_combine(seed, std::get<1>(p).value());
    // TODO(borisb): one of these should work but requires hashing sai attribute
    // boost::hash_combine(seed, std::hash<std::decay_t<decltype(p)>>{}(p));
    // boost::hash_combine(seed, p);
  }
  return seed;
}
} // namespace std