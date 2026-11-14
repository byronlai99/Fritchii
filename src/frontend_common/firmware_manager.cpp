// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include "firmware_manager.h"
#include "common/logging.h"


#ifdef __ANDROID__
#include <jni.h>
#include <common/android/id_cache.h>
#include <common/android/android_common.h>
#endif

FirmwareManager::FirmwareCheckResult FirmwareManager::VerifyFirmware(Core::System &system) {
    if (!CheckFirmwarePresence(system)) {
        return ErrorFirmwareMissing;
    } else {
        const auto pair = GetFirmwareVersion(system);
        const auto result = pair.second;

        if (result.IsError()) {
            LOG_INFO(Frontend, "Unable to read firmware");
            return ErrorFirmwareCorrupted;
        }
    }

    return FirmwareGood;
}
