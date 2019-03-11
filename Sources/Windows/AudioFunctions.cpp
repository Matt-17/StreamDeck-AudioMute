#include "windows.h"
#include "mmdeviceapi.h"
#include "mmsystem.h"
#include "endpointvolume.h"
#include "Functiondiscoverykeys_devpkey.h"
#include "PolicyConfig.h"

#include "../AudioFunctions.h"

#include <cassert>
#include <locale>
#include <codecvt>

namespace {
	std::string WCharPtrToString(LPWSTR in) {
		return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.to_bytes(in);
	}

	std::wstring Utf8StrToWString(const std::string& in) {
		return std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>, wchar_t>{}.from_bytes(in);
	}

	EDataFlow DirectionToEDataFlow(const Direction dir) {
		switch (dir) {
		case Direction::INPUT:
			return eCapture;
		case Direction::OUTPUT:
			return eRender;
		}
		__assume(0);
	}

	ERole RoleToERole(const Role role) {
		switch (role) {
		case Role::COMMUNICATION:
			return eCommunications;
		case Role::DEFAULT:
			return eConsole;
		}
		__assume(0);
	}

	IMMDevice* DeviceIDToDevice(const std::string& in) {
		IMMDeviceEnumerator* de;
		CoCreateInstance(
			__uuidof(MMDeviceEnumerator), NULL,
			CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
			(void**)&de
		);
		IMMDevice* device = nullptr;
		de->GetDevice(Utf8StrToWString(in).data(), &device);
		de->Release();
		return device;
	}

	IAudioEndpointVolume* DeviceIDToAudioEndpointVolume(const std::string& deviceID) {
		auto device = DeviceIDToDevice(deviceID);
		if (!device) {
			return nullptr;
		}
		IAudioEndpointVolume* volume = nullptr;
		device->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, nullptr, (void**) &volume);
		device->Release();
		return volume;
	}
}

std::map<std::string, std::string> GetAudioDeviceList(Direction direction)
{
	IMMDeviceEnumerator* de;
	CoCreateInstance(
		__uuidof(MMDeviceEnumerator), NULL,
		CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
		(void**)&de
	);

	IMMDeviceCollection* devices;
	de->EnumAudioEndpoints(DirectionToEDataFlow(direction), DEVICE_STATE_ACTIVE, &devices);

	UINT deviceCount;
	devices->GetCount(&deviceCount);
	std::map<std::string, std::string> out;

	for (UINT i = 0; i < deviceCount; ++i) {
		IMMDevice* device;
		devices->Item(i, &device);
		LPWSTR deviceID;
		device->GetId(&deviceID);
		IPropertyStore* properties;
		device->OpenPropertyStore(STGM_READ, &properties);
		PROPVARIANT name;
		properties->GetValue(PKEY_Device_FriendlyName, &name);
		out[WCharPtrToString(deviceID)] = WCharPtrToString(name.pwszVal);
		properties->Release();
		device->Release();
	}
	devices->Release();
	de->Release();
	return out;
}

std::string GetDefaultAudioDeviceID(Direction direction, Role role) {
	IMMDeviceEnumerator* de;
	CoCreateInstance(
		__uuidof(MMDeviceEnumerator), NULL,
		CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
		(void**)&de
	);
	IMMDevice* device;
	de->GetDefaultAudioEndpoint(DirectionToEDataFlow(direction), RoleToERole(role), &device);
	LPWSTR deviceID;
	device->GetId(&deviceID);
	const auto ret = WCharPtrToString(deviceID);
	device->Release();
	de->Release();
	return ret;
}

void SetDefaultAudioDeviceID(Direction direction, Role role, const std::string& desiredID) {
	if (desiredID == GetDefaultAudioDeviceID(direction, role)) {
		return;
	}

	IPolicyConfigVista *pPolicyConfig;

	CoCreateInstance(__uuidof(CPolicyConfigVistaClient),
		NULL, CLSCTX_ALL, __uuidof(IPolicyConfigVista), (LPVOID *)&pPolicyConfig);
	pPolicyConfig->SetDefaultEndpoint(Utf8StrToWString(desiredID).c_str(), RoleToERole(role));
	pPolicyConfig->Release();
}

bool IsAudioDeviceMuted(const std::string& deviceID) {
	auto volume = DeviceIDToAudioEndpointVolume(deviceID);
	if (!volume) {
		return false;
	}
	BOOL ret;
	volume->GetMute(&ret);
	volume->Release();
	return ret;
}

void SetIsAudioDeviceMuted(const std::string & deviceID, MuteAction action)
{
	auto volume = DeviceIDToAudioEndpointVolume(deviceID);
	if (!volume) {
		return;
	}
	if (action == MuteAction::MUTE) {
		volume->SetMute(true, nullptr);
	} else if (action == MuteAction::UNMUTE) {
		volume->SetMute(false, nullptr);
	}
	else {
		assert(action == MuteAction::TOGGLE);
		BOOL muted;
		volume->GetMute(&muted);
		volume->SetMute(!muted, nullptr);
	}
	volume->Release();
}
