/* Copyright (c) 2019-present, Fred Emmott
 *
 * This source code is licensed under the MIT-style license found in the
 * LICENSE file.
 */

#include "DefaultAudioDevices.h"

#include <AudioDevices/AudioDevices.h>

const std::string DefaultAudioDevices::DEFAULT_INPUT_ID(
  "de.codeix.sdmute.deviceIds.defaultInput");
const std::string DefaultAudioDevices::DEFAULT_OUTPUT_ID(
  "de.codeix.sdmute.deviceIds.defaultOutput");
const std::string DefaultAudioDevices::COMMUNICATIONS_INPUT_ID(
  "de.codeix.sdmute.deviceIds.communicationsInput");
const std::string DefaultAudioDevices::COMMUNICATIONS_OUTPUT_ID(
  "de.codeix.sdmute.deviceIds.communicationsOutput");

std::string DefaultAudioDevices::GetRealDeviceID(const std::string& dev) {
  if (dev == DEFAULT_INPUT_ID) {
    return GetDefaultAudioDeviceID(
      AudioDeviceDirection::INPUT, AudioDeviceRole::DEFAULT);
  }
  if (dev == DEFAULT_OUTPUT_ID) {
    return GetDefaultAudioDeviceID(
      AudioDeviceDirection::OUTPUT, AudioDeviceRole::DEFAULT);
  }
  if (dev == COMMUNICATIONS_INPUT_ID) {
    return GetDefaultAudioDeviceID(
      AudioDeviceDirection::INPUT, AudioDeviceRole::COMMUNICATION);
  }
  if (dev == COMMUNICATIONS_OUTPUT_ID) {
    return GetDefaultAudioDeviceID(
      AudioDeviceDirection::OUTPUT, AudioDeviceRole::COMMUNICATION);
  }
  return dev;
}

std::string DefaultAudioDevices::GetSpecialDeviceID(
  AudioDeviceDirection direction,
  AudioDeviceRole role) {
  if (
    direction == AudioDeviceDirection::INPUT
    && role == AudioDeviceRole::DEFAULT) {
    return DEFAULT_INPUT_ID;
  }
  if (
    direction == AudioDeviceDirection::OUTPUT
    && role == AudioDeviceRole::DEFAULT) {
    return DEFAULT_OUTPUT_ID;
  }
  if (
    direction == AudioDeviceDirection::INPUT
    && role == AudioDeviceRole::COMMUNICATION) {
    return COMMUNICATIONS_INPUT_ID;
  }
  if (
    direction == AudioDeviceDirection::OUTPUT
    && role == AudioDeviceRole::COMMUNICATION) {
    return COMMUNICATIONS_OUTPUT_ID;
  }
  return std::string();
}
