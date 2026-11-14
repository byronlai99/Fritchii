// SPDX-FileCopyrightText: Copyright 2025 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

// SPDX-FileCopyrightText: Copyright 2023 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <cstring>

#include "core/file_sys/fssystem/fssystem_crypto_configuration.h"

namespace FileSys {

namespace {

// Deliberately refuse to derive external console keys. Plaintext NCA files do not
// need this callback; encrypted headers/sections therefore fail closed.
void RejectEncryptedKey(void* dst_key, size_t dst_key_size, const void*, size_t, s32) {
    std::memset(dst_key, 0xFF, dst_key_size);
}

} // namespace

const NcaCryptoConfiguration& GetCryptoConfiguration() {
    static const NcaCryptoConfiguration configuration = {
        .header_1_sign_key_moduli{},
        .header_1_sign_key_public_exponent{},
        .key_area_encryption_key_source{},
        .header_encryption_key_source{},
        .header_encrypted_encryption_keys{},
        .generate_key = RejectEncryptedKey,
        .verify_sign1{},
        .is_plaintext_header_available{},
        .is_available_sw_key{},
    };

    return configuration;
}

} // namespace FileSys
