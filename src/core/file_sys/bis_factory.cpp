// SPDX-FileCopyrightText: Copyright 2018 yuzu Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <array>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include <fmt/ranges.h>
#include "common/fs/path_util.h"
#include "common/string_util.h"
#include "core/file_sys/bis_factory.h"
#include "core/file_sys/registered_cache.h"
#include "core/file_sys/vfs/vfs.h"

namespace FileSys {

constexpr u64 NAND_USER_SIZE = 0x680000000;  // 26624 MiB
constexpr u64 NAND_SYSTEM_SIZE = 0xA0000000; // 2560 MiB
constexpr u64 NAND_TOTAL_SIZE = 0x747C00000; // 29820 MiB

BISFactory::BISFactory(VirtualDir nand_root_, VirtualDir load_root_, VirtualDir dump_root_)
    : nand_root(std::move(nand_root_)), load_root(std::move(load_root_)),
      dump_root(std::move(dump_root_)),
      sysnand_cache(std::make_unique<RegisteredCache>(
          GetOrCreateDirectoryRelative(nand_root, "/system/Contents/registered"))),
      usrnand_cache(std::make_unique<RegisteredCache>(
          GetOrCreateDirectoryRelative(nand_root, "/user/Contents/registered"))),
      sysnand_placeholder(std::make_unique<PlaceholderCache>(
          GetOrCreateDirectoryRelative(nand_root, "/system/Contents/placehld"))),
      usrnand_placeholder(std::make_unique<PlaceholderCache>(
          GetOrCreateDirectoryRelative(nand_root, "/user/Contents/placehld"))) {}

BISFactory::~BISFactory() = default;

VirtualDir BISFactory::GetSystemNANDContentDirectory() const {
    return GetOrCreateDirectoryRelative(nand_root, "/system/Contents");
}

VirtualDir BISFactory::GetUserNANDContentDirectory() const {
    return GetOrCreateDirectoryRelative(nand_root, "/user/Contents");
}

RegisteredCache* BISFactory::GetSystemNANDContents() const {
    return sysnand_cache.get();
}

RegisteredCache* BISFactory::GetUserNANDContents() const {
    return usrnand_cache.get();
}

PlaceholderCache* BISFactory::GetSystemNANDPlaceholder() const {
    return sysnand_placeholder.get();
}

PlaceholderCache* BISFactory::GetUserNANDPlaceholder() const {
    return usrnand_placeholder.get();
}

VirtualDir BISFactory::GetModificationLoadRoot(u64 title_id) const {
    // LayeredFS doesn't work on updates and title id-less homebrew
    if (title_id == 0 || (title_id & 0xFFF) == 0x800)
        return nullptr;
    if (load_root == nullptr)
        return nullptr;
    return GetOrCreateDirectoryRelative(load_root, fmt::format("/{:016X}", title_id));
}

VirtualDir BISFactory::GetModificationDumpRoot(u64 title_id) const {
    if (title_id == 0)
        return nullptr;
    return GetOrCreateDirectoryRelative(dump_root, fmt::format("/{:016X}", title_id));
}

VirtualDir BISFactory::OpenPartition(BisPartitionId id) const {
    switch (id) {
    case BisPartitionId::CalibrationFile:
        return GetOrCreateDirectoryRelative(nand_root, "/prodinfof");
    case BisPartitionId::SafeMode:
        return GetOrCreateDirectoryRelative(nand_root, "/safe");
    case BisPartitionId::System:
        return GetOrCreateDirectoryRelative(nand_root, "/system");
    case BisPartitionId::User:
        return GetOrCreateDirectoryRelative(nand_root, "/user");
    default:
        return nullptr;
    }
}

namespace {

VirtualFile FindNANDDataFile(const VirtualDir& directory, std::string_view name) {
    if (directory == nullptr) {
        return nullptr;
    }

    const std::string file_name{name};
    const auto upper_name = Common::ToUpper(file_name);
    const std::array candidates{
        file_name,
        file_name + ".bin",
        upper_name,
        upper_name + ".BIN",
    };

    for (const auto& candidate : candidates) {
        if (auto file = directory->GetFile(candidate); file != nullptr) {
            return file;
        }
    }
    return nullptr;
}

} // namespace

VirtualFile BISFactory::OpenPartitionStorage(BisPartitionId id,
                                             VirtualFilesystem file_system) const {
    if (file_system == nullptr) {
        return nullptr;
    }

    // Reading raw BIS storage is independent from external key derivation.
    const auto nand_directory = file_system->OpenDirectory(
        Common::FS::GetEdenPathString(Common::FS::EdenPath::NANDDir), OpenMode::Read);
    if (nand_directory == nullptr) {
        return nullptr;
    }

    switch (id) {
    case BisPartitionId::CalibrationBinary:
        // CalibrationBinary is a decrypted PRODINFO view. Without an externally supplied
        // BIS key, returning the raw encrypted PRODINFO file as plaintext would be incorrect.
        return nullptr;
    case BisPartitionId::BootConfigAndPackage2Part1:
    case BisPartitionId::BootConfigAndPackage2Part2:
    case BisPartitionId::BootConfigAndPackage2Part3:
    case BisPartitionId::BootConfigAndPackage2Part4:
    case BisPartitionId::BootConfigAndPackage2Part5:
    case BisPartitionId::BootConfigAndPackage2Part6: {
        static constexpr std::array<std::string_view, 6> package2_names{
            "BCPKG2-1-Normal-Main",
            "BCPKG2-2-Normal-Sub",
            "BCPKG2-3-SafeMode-Main",
            "BCPKG2-4-SafeMode-Sub",
            "BCPKG2-5-Repair-Main",
            "BCPKG2-6-Repair-Sub",
        };
        const auto index = static_cast<std::size_t>(
            static_cast<u8>(id) - static_cast<u8>(BisPartitionId::BootConfigAndPackage2Part1));
        return FindNANDDataFile(nand_directory, package2_names[index]);
    }
    default:
        return nullptr;
    }
}

VirtualDir BISFactory::GetImageDirectory() const {
    return GetOrCreateDirectoryRelative(nand_root, "/user/Album");
}

u64 BISFactory::GetSystemNANDFreeSpace() const {
    const auto sys_dir = GetOrCreateDirectoryRelative(nand_root, "/system");
    if (sys_dir == nullptr) {
        return GetSystemNANDTotalSpace();
    }

    return GetSystemNANDTotalSpace() - sys_dir->GetSize();
}

u64 BISFactory::GetSystemNANDTotalSpace() const {
    return NAND_SYSTEM_SIZE;
}

u64 BISFactory::GetUserNANDFreeSpace() const {
    // For some reason games such as BioShock 1 checks whether this is exactly 0x680000000 bytes.
    // Set the free space to be 1 MiB less than the total as a workaround to this issue.
    return GetUserNANDTotalSpace() - 0x100000;
}

u64 BISFactory::GetUserNANDTotalSpace() const {
    return NAND_USER_SIZE;
}

u64 BISFactory::GetFullNANDTotalSpace() const {
    return NAND_TOTAL_SIZE;
}

VirtualDir BISFactory::GetBCATDirectory(u64 title_id) const {
    return GetOrCreateDirectoryRelative(nand_root,
                                        fmt::format("/system/save/bcat/{:016X}", title_id));
}

} // namespace FileSys
