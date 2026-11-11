// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

// DLSS Frame Generation pacing policy, independent of Streamline's Vulkan integration.

#pragma once

#include <chrono>
#include <optional>

#include "common/common_types.h"

namespace Vulkan::Streamline {

enum class FrameGenMode : u32 {
    Fixed2x,
    Auto,
};

/// Reads LICHII_DLSS_FG_MODE. Auto is the default.
[[nodiscard]] FrameGenMode ConfiguredFrameGenMode();

/// Reads LICHII_DLSS_FG_TARGET. Defaults to 60 FPS.
[[nodiscard]] f32 FrameGenTargetRate();

/// Chooses how many generated frames accompany each rendered frame.
/// DLSS-G remains the generator; this class only controls the requested output cadence.
class FrameGenPacer {
public:
    [[nodiscard]] u32 Plan(u32 capacity, f32 target_rate);
    void Reset();

private:
    using Clock = std::chrono::steady_clock;

    static constexpr f32 INTERVAL_SMOOTHING = 0.25f;
    static constexpr f32 MINIMUM_BASE_RATE = 10.0f;
    static constexpr f32 BURST_CADENCE_RATIO = 3.0f;
    static constexpr f32 BURST_TARGET_RATIO = 2.0f;
    static constexpr f32 PROBE_THROUGHPUT_TOLERANCE = 0.95f;
    static constexpr f32 PROBE_BASE_COLLAPSE_RATIO = 0.70f;
    static constexpr f32 PROBE_MARGINAL_GAIN = 1.15f;
    static constexpr f32 TARGET_SATISFIED_RATIO = 0.95f;
    static constexpr f32 UNLOADED_BASE_RETENTION = 0.75f;
    static constexpr f32 CREDIT_EPSILON = 1.0e-4f;
    static constexpr u32 MAX_PROBE_FAILURES = 4;

    static constexpr auto STABILIZATION_DURATION = std::chrono::seconds(1);
    static constexpr auto PROBE_DURATION = std::chrono::seconds(1);
    static constexpr auto DEFICIT_DURATION = std::chrono::seconds(1);
    static constexpr auto PROBE_STEP_DELAY = std::chrono::milliseconds(250);

    [[nodiscard]] static Clock::duration ProbeBackoff(u32 failures);
    void UpdateLimit(Clock::time_point now, f32 base_rate, f32 target_rate, u32 capacity);
    void DeferEvaluations(Clock::duration amount);
    void Stabilize(Clock::time_point now);

    std::optional<Clock::time_point> last_frame;
    std::optional<Clock::time_point> stable_until;
    std::optional<Clock::time_point> probe_until;
    std::optional<Clock::time_point> next_probe;
    std::optional<Clock::time_point> deficit_since;
    f32 smoothed_interval{};
    f32 output_credit{};
    f32 probe_base_rate{};
    f32 unloaded_base_rate{};
    u32 issued_generations{};
    u32 probe_previous_limit{};
    u32 limit{};
    u32 probe_failures{};
};

} // namespace Vulkan::Streamline
