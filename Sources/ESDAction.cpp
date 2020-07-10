#include "ESDAction.h"

#include <StreamDeckSDK/EPLJSONUtils.h>
#include <StreamDeckSDK/ESDLogger.h>

ESDAction::ESDAction(ESDConnectionManager* esd_connection, const std::string& context)
  : mESDConnection(esd_connection), mContext(context) {
}

ESDAction::~ESDAction() {
}

std::string ESDAction::GetContext() const {
  return mContext;
}

void ESDAction::DidReceiveSettings(const nlohmann::json& json) {
}

ESDConnectionManager* ESDAction::GetESD() const {
  return mESDConnection;
}

void ESDAction::WillAppear(const nlohmann::json& settings) {
}

void ESDAction::KeyUp(const nlohmann::json& settings) {
}
