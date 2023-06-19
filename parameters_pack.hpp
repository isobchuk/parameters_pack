/**
 * @file parameters_pack.hpp
 * @author Ivan Sobchuk (i.a.sobchuk.1994@gmail.com)
 * @brief Simple class to operate with template class variadic
 * parameter pack
 *
 * @date 2023-06-19
 *
 * @copyright Ivan Sobchuk (c) 2023
 *
 * License Apache 2.0
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */
#pragma once

// Suppoted since C++17 as a last fully-supported standard for gcc and clang
static_assert((__cplusplus >= 201703L), "Supported only with C++11 and newer!");

// The meta framework for embedded, chosen to not cross the existing ones
namespace iso {

/**
 * @brief Class to to operate with template class variadic parameter pack.
 * Provides three public method:
 * - value::extract - get value from the pack according to the type
 * - types::check - check the pack parameters according to type list
 * - no_duplicates - check that pack has no duplicated types
 *
 * @example iso::parameters_pack::types<Input, Pull, Drive, Sense, McuSel>::check(params...):
 *          Check that all parameters are one of Input, Pull, Drive, Sense, McuSel.
 * @example iso::parameters_pack::no_duplicates(params...): Check that all parameters have uniq types.
 * @example iso::parameters_pack::value<Pull>::extract(params...): Get value of type Pull.
 * @example iso::parameters_pack::value<Input, Input::Disconnect>::extract(params...): Get value of type Input with user defined default.
 */
class parameters_pack {
  template <typename TypeFirst, typename... TypesRest> struct check_v {
    inline static constexpr bool list_contains_param_v(const TypeFirst) { return true; }
    template <typename Value> inline static constexpr bool list_contains_param_v(const Value value) {
      return check_v<TypesRest...>::list_contains_param_v(value);
    }

    inline static constexpr bool params_types_v() { return true; }
    template <typename First, typename... Rest> inline static constexpr bool params_types_v(const First first, const Rest... rest) {
      return list_contains_param_v(first) ? params_types_v(rest...) : false;
    }
  };

  template <typename TypeFirst> struct check_v<TypeFirst> {
    inline static constexpr bool list_contains_param_v(const TypeFirst) { return true; }
    template <typename Value> inline static constexpr bool list_contains_param_v(const Value) { return false; }

    inline static constexpr bool params_types_v() { return true; }
    template <typename First, typename... Rest> inline static constexpr bool params_types_v(const First first, const Rest... rest) {
      return list_contains_param_v(first) ? params_types_v(rest...) : false;
    }
  };

  struct duplicate {
    template <typename T> inline static constexpr bool is_same_v(const T, const T) { return true; }
    template <typename T, typename U> inline static constexpr bool is_same_v(const T, const U) { return false; }

    inline static constexpr bool is_duplicates_v() { return true; }
    template <typename First> inline static constexpr bool is_duplicates_v(const First) { return true; }
    template <typename First, typename... Rest> inline static constexpr bool is_duplicates_v(const First first, const Rest... rest) {
      return (!is_same_v(first, rest) && ...) && is_duplicates_v(rest...);
    }
  };

public:
  /**
   * @brief Get value from the pack according to the type.
   *
   * @tparam Type: Type that have to be found in the pack.
   * @tparam defaultValue (Additional): Default type value (if different from the Type{}).
   *
   * @return value - Type was found.
   * @return defaultValue - Type was not found.
   *
   */
  template <typename Type, const Type defaultValue = Type{}> struct value {
    inline static constexpr Type extract() { return defaultValue; }
    inline static constexpr Type extract(const Type first) { return first; }
    template <typename First> inline static constexpr Type extract(const First) { return defaultValue; }
    template <typename... Rest> inline static constexpr Type extract(const Type first, const Rest...) { return first; }
    template <typename First, typename... Rest> inline static constexpr Type extract(const First, const Rest... rest) { return extract(rest...); }
  };

  /**
   * @brief Check the pack parameters according to type list (all parameters have the one of type list type).
   *
   * @tparam TypeFirst: First type in the list.
   * @tparam TypesRest: Rest types in the list.
   *
   * @return true: All parameters in the pack has types from the types list.
   * @return false: Otherwise.
   */
  template <typename TypeFirst, typename... TypesRest> struct types {
    inline static constexpr bool check() { return true; }
    template <typename First, typename... Rest> inline static constexpr bool check(const First first, const Rest... rest) {
      return check_v<TypeFirst, TypesRest...>::params_types_v(first, rest...);
    }
  };

  /**
   * @brief Check that pack has no duplicated types.
   *
   * @return true: Each parameter in the pack has uniq type.
   * @return false: Otherwise.
   */
  inline static constexpr bool no_duplicates() { return true; }
  template <typename First> inline static constexpr bool no_duplicates(const First first) { return true; }
  template <typename First, typename... Rest> inline static constexpr bool no_duplicates(const First first, const Rest... rest) {
    return duplicate::is_duplicates_v(first, rest...);
  }
};
} // namespace iso