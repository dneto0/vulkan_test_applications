/* Copyright 2017 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SUPPORT_CONTAINERS_DEQUE_H_
#define SUPPORT_CONTAINERS_DEQUE_H_

#include <deque>

#include "support/containers/stl_compatible_allocator.h"

namespace containers {
template <typename T>
using deque = std::deque<T, StlCompatibleAllocator<T>>;
}  // namespace containers

#endif  // SUPPORT_CONTAINERS_DEQUE_H_
