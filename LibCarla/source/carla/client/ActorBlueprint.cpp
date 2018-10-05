// Copyright (c) 2017 Computer Vision Center (CVC) at the Universitat Autonoma
// de Barcelona (UAB).
//
// This work is licensed under the terms of the MIT license.
// For a copy, see <https://opensource.org/licenses/MIT>.

#include "carla/client/ActorBlueprint.h"

#include "carla/StringUtil.h"

#include <algorithm>

namespace carla {
namespace client {

  template <typename Map, typename Container>
  static void FillMap(Map &destination, Container &source) {
    destination.reserve(source.size());
    for (auto &item : source) {
      auto id = item.id;
      destination.emplace(id, std::move(item));
    }
  }

  ActorBlueprint::ActorBlueprint(rpc::ActorDefinition definition)
    : _uid(definition.uid),
      _id(std::move(definition.id)) {
    StringUtil::Split(_tags, definition.tags, ",");
    FillMap(_attributes, definition.attributes);
  }

  bool ActorBlueprint::MatchTags(const std::string &wildcard_pattern) const {
    return std::any_of(_tags.begin(), _tags.end(), [&](const auto &tag) {
      return StringUtil::Match(tag, wildcard_pattern);
    });
  }

  rpc::ActorDescription ActorBlueprint::MakeActorDescription() const {
    rpc::ActorDescription description;
    description.uid = _uid;
    description.id = _id;
    description.attributes.reserve(_attributes.size());
    for (const auto &attribute : *this) {
      if (attribute.IsModifiable()) {
        description.attributes.push_back(attribute);
      }
    }
    return description;
  }

} // namespace client
} // namespace carla
