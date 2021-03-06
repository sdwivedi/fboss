/*
 *  Copyright (c) 2004-present, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 *
 */

#include "fboss/lib/phy/ExternalPhy.h"
#include "fboss/mdio/MdioError.h"
#include "folly/json.h"

#include <thrift/lib/cpp/util/EnumUtils.h>
#include <thrift/lib/cpp2/protocol/Serializer.h>

namespace {
template <typename T>
folly::dynamic thriftToDynamic(const T& val) {
  return folly::parseJson(
      apache::thrift::SimpleJSONSerializer::serialize<std::string>(val));
}

template <typename T>
folly::dynamic thriftOptToDynamic(const std::optional<T>& opt) {
  return opt.has_value() ? thriftToDynamic(opt.value()) : "null";
}
} // namespace

namespace facebook::fboss::phy {

bool LaneConfig::operator==(const LaneConfig& rhs) const {
  return (polaritySwap == rhs.polaritySwap) && (tx == rhs.tx);
}

LaneSettings LaneConfig::toLaneSettings() const {
  LaneSettings settings;
  if (!polaritySwap.has_value()) {
    *settings.polaritySwap_ref() = {};
  } else {
    *settings.polaritySwap_ref() = polaritySwap.value();
  }
  if (tx.has_value()) {
    settings.tx_ref() = tx.value();
  }
  return settings;
}

LaneConfig LaneConfig::fromLaneSettings(const LaneSettings& settings) {
  LaneConfig config;
  if (auto tx = settings.tx_ref()) {
    config.tx = *tx;
  }
  config.polaritySwap = *settings.polaritySwap_ref();
  return config;
}

bool PhySideConfig::operator==(const PhySideConfig& rhs) const {
  return std::equal(
      lanes.begin(), lanes.end(), rhs.lanes.begin(), rhs.lanes.end());
}

PhySideConfig PhySideConfig::fromPhyPortSideSettings(
    const PhyPortSideSettings& settings) {
  PhySideConfig phySideConfig;

  for (auto in : *settings.lanes_ref()) {
    phySideConfig.lanes.insert(std::pair<int32_t, LaneConfig>(
        in.first, LaneConfig::fromLaneSettings(in.second)));
  }
  return phySideConfig;
}

bool ExternalPhyConfig::operator==(const ExternalPhyConfig& rhs) const {
  return (system == rhs.system) && (line == rhs.line);
}

bool ExternalPhyProfileConfig::operator==(
    const ExternalPhyProfileConfig& rhs) const {
  return (speed == rhs.speed) && (system == rhs.system) && (line == rhs.line);
}

ExternalPhyConfig ExternalPhyConfig::fromConfigeratorTypes(
    PortPinConfig portPinConfig,
    const std::map<int32_t, PolaritySwap>& linePolaritySwapMap) {
  ExternalPhyConfig xphyCfg;

  if (!portPinConfig.xphySys_ref()) {
    throw MdioError("Port pin config is missing xphySys");
  }
  if (!portPinConfig.xphyLine_ref()) {
    throw MdioError("Port pin config is missing xphyLine");
  }

  auto fillLaneConfigs =
      [](const std::vector<PinConfig>& pinConfigs,
         const std::map<int32_t, PolaritySwap>& polaritySwapMap,
         std::map<int32_t, LaneConfig>& laneConfigs) {
        std::map<int32_t, TxSettings> txMap;
        for (auto pinCfg : pinConfigs) {
          LaneConfig laneCfg;
          if (pinCfg.tx_ref()) {
            laneCfg.tx = *pinCfg.tx_ref();
          }
          if (auto it = polaritySwapMap.find(*pinCfg.id_ref()->lane_ref());
              it != polaritySwapMap.end()) {
            laneCfg.polaritySwap = it->second;
          }
          laneConfigs.emplace(pinCfg.id_ref()->lane, laneCfg);
        }
      };

  fillLaneConfigs(*portPinConfig.xphySys_ref(), {}, xphyCfg.system.lanes);
  fillLaneConfigs(
      *portPinConfig.xphyLine_ref(), linePolaritySwapMap, xphyCfg.line.lanes);

  return xphyCfg;
}

bool PhyPortConfig::operator==(const PhyPortConfig& rhs) const {
  return config == rhs.config && profile == rhs.profile;
}

bool PhyPortConfig::operator!=(const PhyPortConfig& rhs) const {
  return !(*this == rhs);
}

ExternalPhyProfileConfig ExternalPhyProfileConfig::fromPortProfileConfig(
    const PortProfileConfig& portCfg) {
  if (!portCfg.xphyLine_ref()) {
    throw MdioError(
        "Attempted to create xphy config without xphy line settings");
  }
  ExternalPhyProfileConfig xphyCfg;
  xphyCfg.speed = *portCfg.speed_ref();
  xphyCfg.system = *portCfg.iphy_ref();
  xphyCfg.line = *portCfg.xphyLine_ref();
  return xphyCfg;
}

PhyPortSettings PhyPortConfig::toPhyPortSettings(int16_t phyID) const {
  PhyPortSettings settings;

  for (auto in : config.system.lanes) {
    settings.system_ref()->lanes_ref()->insert(
        std::pair<int16_t, LaneSettings>(in.first, in.second.toLaneSettings()));
  }
  for (auto in : config.line.lanes) {
    settings.line_ref()->lanes_ref()->insert(
        std::pair<int16_t, LaneSettings>(in.first, in.second.toLaneSettings()));
  }

  *settings.phyID_ref() = phyID;
  *settings.speed_ref() = profile.speed;
  *settings.line_ref()->modulation_ref() = *profile.line.modulation_ref();
  *settings.system_ref()->modulation_ref() = *profile.system.modulation_ref();
  *settings.line_ref()->fec_ref() = *profile.line.fec_ref();
  *settings.system_ref()->fec_ref() = *profile.system.fec_ref();

  return settings;
}

PhyPortConfig PhyPortConfig::fromPhyPortSettings(
    const PhyPortSettings& settings) {
  PhyPortConfig result;

  for (auto in : *settings.system_ref()->lanes_ref()) {
    result.config.system.lanes.insert(std::pair<int32_t, LaneConfig>(
        in.first, LaneConfig::fromLaneSettings(in.second)));
  }

  for (auto in : *settings.line_ref()->lanes_ref()) {
    result.config.line.lanes.insert(std::pair<int32_t, LaneConfig>(
        in.first, LaneConfig::fromLaneSettings(in.second)));
  }

  result.config.system =
      PhySideConfig::fromPhyPortSideSettings(*settings.system_ref());
  result.config.line =
      PhySideConfig::fromPhyPortSideSettings(*settings.line_ref());

  *result.profile.line.numLanes_ref() =
      settings.line_ref()->lanes_ref()->size();
  *result.profile.line.modulation_ref() =
      *settings.line_ref()->modulation_ref();
  *result.profile.line.fec_ref() = *settings.line_ref()->fec_ref();

  *result.profile.system.numLanes_ref() =
      settings.system_ref()->lanes_ref()->size();
  *result.profile.system.modulation_ref() =
      *settings.system_ref()->modulation_ref();
  *result.profile.system.fec_ref() = *settings.system_ref()->fec_ref();

  result.profile.speed = *settings.speed_ref();

  return result;
}

folly::dynamic LaneConfig::toDynamic() const {
  folly::dynamic obj = folly::dynamic::object;
  obj["polaritySwap"] = thriftOptToDynamic(polaritySwap);
  obj["tx"] = thriftOptToDynamic(tx);

  return obj;
}

folly::dynamic PhySideConfig::toDynamic() const {
  folly::dynamic elements = folly::dynamic::array;
  for (auto pair : lanes) {
    elements.push_back(folly::dynamic::object(
        std::to_string(pair.first), pair.second.toDynamic()));
  }

  return elements;
}

folly::dynamic ExternalPhyConfig::toDynamic() const {
  folly::dynamic obj = folly::dynamic::object;
  obj["system"] = system.toDynamic();
  obj["line"] = line.toDynamic();

  return obj;
}

folly::dynamic ExternalPhyProfileConfig::toDynamic() const {
  folly::dynamic obj = folly::dynamic::object;
  obj["speed"] = apache::thrift::util::enumNameSafe(speed);
  obj["system"] = thriftToDynamic(system);
  obj["line"] = thriftToDynamic(line);

  return obj;
}

folly::dynamic PhyPortConfig::toDynamic() const {
  folly::dynamic obj = folly::dynamic::object;
  obj["config"] = config.toDynamic();
  obj["profile"] = profile.toDynamic();
  return obj;
}

} // namespace facebook::fboss::phy
