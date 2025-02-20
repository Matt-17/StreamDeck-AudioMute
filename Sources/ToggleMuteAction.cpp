/* Copyright (c) 2019-present, Fred Emmott
 *
 * This source code is licensed under the MIT-style license found in the
 * LICENSE file.
 */

#include "ToggleMuteAction.h"

#include <AudioDevices/AudioDevices.h>
#include <StreamDeckSDK/ESDConnectionManager.h>
#include <StreamDeckSDK/ESDLogger.h>

#include "MuteAction.h"
#include "UnmuteAction.h"

const std::string ToggleMuteAction::ACTION_ID(
  "de.codeix.audiotoggle.toggle");

void ToggleMuteAction::MuteStateDidChange(bool isMuted) {
  ESDDebug("MuteStateDidChange to {} for context {}", isMuted, GetContext());
  SetState(isMuted ? 0 : 1);
}

void ToggleMuteAction::WillAppear() {
  auto muteState = IsAudioDeviceMuted(GetRealDeviceID());
  if (!muteState) {
    ShowAlert();
    return;
  }

  MuteStateDidChange(*muteState);
}

void ToggleMuteAction::DoAction() {
  const auto device(GetRealDeviceID());

  const auto muteState = IsAudioDeviceMuted(device);
  if (!muteState) {
    ShowAlert();
    return;
  }

  const auto muted = *muteState;

  if (muted) {
    if (!UnmuteAudioDevice(device)) {
      ShowAlert();
      return;
    }
    if (FeedbackSoundsEnabled()) {
      UnmuteAction::PlayFeedbackSound();
    }
  } else {
    if (!MuteAudioDevice(device)) {
      ShowAlert();
      return;
    }
    if (FeedbackSoundsEnabled()) {
      MuteAction::PlayFeedbackSound();
    }
  }
}

void ToggleMuteAction::KeyUp() {
  if (mPushAndHoldToTalk) {
    const auto muteState = IsAudioDeviceMuted(GetRealDeviceID());
    if (!muteState) {
      ShowAlert();
      return;
    }
    const auto muted = *muteState;

    if (muted) {
      // PTM-mode, unmute immediately...
      DoAction();
      return;
    }

    // PTT: people tend to let go of the button just a little too soon, so delay
    auto ctx = GetESD()->GetAsioContext();
    mPttReleaseTimer = std::make_unique<asio::steady_timer>(*ctx, std::chrono::milliseconds(250));
    mPttReleaseTimer->async_wait([this](const asio::error_code& ec) {
      if (ec) {
        return;
      }
      DoAction();
    });
    return;
  }
  BaseMuteAction::KeyUp();
}

void ToggleMuteAction::KeyDown() {
  if (mPushAndHoldToTalk) {
    DoAction();
    return;
  }
  BaseMuteAction::KeyDown();
}

void ToggleMuteAction::SettingsDidChange(
  const MuteActionSettings& oldSettings,
  const MuteActionSettings& newSettings) {
  mPushAndHoldToTalk = newSettings.ptt;
  BaseMuteAction::SettingsDidChange(oldSettings, newSettings);
}
