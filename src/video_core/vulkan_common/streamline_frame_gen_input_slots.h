// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

// Synchronization bookkeeping for DLSS Frame Generation's rotating input-copy ring.

#pragma once

#include <array>
#include <condition_variable>
#include <mutex>

#include "common/common_types.h"
#include "video_core/vulkan_common/streamline.h"

namespace Vulkan::Streamline {

/// Tracks Streamline's outstanding reads from the four depth/motion input-copy slots.
class FrameGenInputSlots {
public:
    /// Marks a slot as being tagged for a frame that may be presented.
    void MarkPending(u32 slot);

    /// Publishes the completion fence associated with a presented slot and wakes waiters.
    void Complete(u32 slot, VkSemaphore fence, u64 value);

    /// Waits until tagging/present bookkeeping is complete, then returns the fence for the slot.
    [[nodiscard]] bool WaitForCompletion(u32 slot, VkSemaphore& fence, u64& value);

    /// Clears a fence only when it is still the completion recorded by the caller.
    void ClearCompletionIfMatches(u32 slot, u64 value);

    /// Whether the slot is awaiting a present or discard decision.
    [[nodiscard]] bool IsPending(u32 slot);

    /// Releases an abandoned pending slot after the caller has made device use safe.
    void AbandonPending(u32 slot);

    /// Clears all slot state and wakes any blocked renderer threads.
    void Reset();

private:
    std::mutex mutex;
    std::condition_variable cv;
    std::array<bool, FRAME_GEN_INPUT_SLOT_COUNT> pending{};
    std::array<VkSemaphore, FRAME_GEN_INPUT_SLOT_COUNT> fences{};
    std::array<u64, FRAME_GEN_INPUT_SLOT_COUNT> values{};
};

} // namespace Vulkan::Streamline
