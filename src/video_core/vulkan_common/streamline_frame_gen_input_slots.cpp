// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include "video_core/vulkan_common/streamline_frame_gen_input_slots.h"

namespace Vulkan::Streamline {

void FrameGenInputSlots::MarkPending(u32 slot) {
    if (slot >= FRAME_GEN_INPUT_SLOT_COUNT) {
        return;
    }
    std::lock_guard lock{mutex};
    pending[slot] = true;
}

void FrameGenInputSlots::Complete(u32 slot, VkSemaphore fence, u64 value) {
    if (slot >= FRAME_GEN_INPUT_SLOT_COUNT) {
        return;
    }
    {
        std::lock_guard lock{mutex};
        fences[slot] = value != 0 ? fence : VK_NULL_HANDLE;
        values[slot] = value;
        pending[slot] = false;
    }
    cv.notify_all();
}

bool FrameGenInputSlots::WaitForCompletion(u32 slot, VkSemaphore& fence, u64& value) {
    if (slot >= FRAME_GEN_INPUT_SLOT_COUNT) {
        return false;
    }
    std::unique_lock lock{mutex};
    cv.wait(lock, [this, slot] { return !pending[slot]; });
    fence = fences[slot];
    value = values[slot];
    return true;
}

void FrameGenInputSlots::ClearCompletionIfMatches(u32 slot, u64 value) {
    if (slot >= FRAME_GEN_INPUT_SLOT_COUNT) {
        return;
    }
    std::lock_guard lock{mutex};
    if (values[slot] == value) {
        values[slot] = 0;
        fences[slot] = VK_NULL_HANDLE;
    }
}

bool FrameGenInputSlots::IsPending(u32 slot) {
    if (slot >= FRAME_GEN_INPUT_SLOT_COUNT) {
        return false;
    }
    std::lock_guard lock{mutex};
    return pending[slot];
}

void FrameGenInputSlots::AbandonPending(u32 slot) {
    if (slot >= FRAME_GEN_INPUT_SLOT_COUNT) {
        return;
    }
    {
        std::lock_guard lock{mutex};
        if (!pending[slot]) {
            return;
        }
        pending[slot] = false;
        values[slot] = 0;
        fences[slot] = VK_NULL_HANDLE;
    }
    cv.notify_all();
}

void FrameGenInputSlots::Reset() {
    {
        std::lock_guard lock{mutex};
        pending.fill(false);
        fences.fill(VK_NULL_HANDLE);
        values.fill(0);
    }
    cv.notify_all();
}

} // namespace Vulkan::Streamline
