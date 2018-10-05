// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#pragma once

#include "carla/Debug.h"
#include "carla/Iterator.h"
#include "carla/NonCopyable.h"
#include "carla/client/ActorBlueprint.h"
#include "carla/client/Memory.h"

#include <type_traits>
#include <unordered_map>
#include <vector>

namespace carla {
namespace client {

  /// @todo Works as a list but its actually a map. We should assess the use
  /// cases and reconsider this implementation.
  class BlueprintLibrary
    : public EnableSharedFromThis<BlueprintLibrary>,
      private NonCopyable {
    using map_type = std::unordered_map<std::string, ActorBlueprint>;
  public:

    // Here we force a bit the typedefs to make this class look like a list.
    using key_type = map_type::key_type;
    using value_type = map_type::mapped_type;
    using size_type = map_type::size_type;
    using const_iterator = decltype(carla::iterator::make_map_values_iterator<map_type::const_iterator>(map_type::const_iterator{}));
    using const_reference = const value_type &;
    using const_pointer = const value_type *;

    explicit BlueprintLibrary(const std::vector<rpc::ActorDefinition> &blueprints);

    BlueprintLibrary(BlueprintLibrary &&) = default;
    BlueprintLibrary &operator=(BlueprintLibrary &&) = default;

    /// Filters a list of ActorBlueprint with tags matching @a wildcard_pattern.
    SharedPtr<BlueprintLibrary> Filter(const std::string &wildcard_pattern) const;

    const_pointer Find(const std::string &key) const {
      auto it = _blueprints.find(key);
      return it != _blueprints.end() ? &it->second : nullptr;
    }

    const_reference at(const std::string &key) const {
      return _blueprints.at(key);
    }

    /// @warning Linear complexity.
    const_reference operator[](size_type pos) const {
      return std::next(_blueprints.begin(), pos)->second;
    }

    /// @warning Linear complexity.
    const_reference at(size_type pos) const;

    const_iterator begin() const /*noexcept*/ {
      return iterator::make_map_values_iterator(_blueprints.begin());
    }

    const_iterator end() const /*noexcept*/ {
      return iterator::make_map_values_iterator(_blueprints.end());
    }

    bool empty() const /*noexcept*/ {
      return _blueprints.empty();
    }

    size_type size() const /*noexcept*/ {
      return _blueprints.size();
    }

  private:

    BlueprintLibrary(map_type blueprints)
      : _blueprints(std::move(blueprints)) {}

    map_type _blueprints;
  };

} // namespace client
} // namespace carla
