// SPDX-FileCopyrightText: Copyright 2026 Eden Emulator Project
// SPDX-License-Identifier: GPL-3.0-or-later

#include "video_core/vulkan_common/streamline_frame_gen_pacer.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string_view>
#include <utility>

namespace Vulkan::Streamline {

FrameGenMode ConfiguredFrameGenMode() {
    if (const char* const value = std::getenv("LICHII_DLSS_FG_MODE")) {
        const std::string_view mode{value};
        if (mode == "auto") {
            return FrameGenMode::Auto;
        }
        if (mode == "2x" || mode == "double") {
            return FrameGenMode::Fixed2x;
        }
    }
    // Auto is the default; unrecognized or legacy modes safely fall back to Auto.
    return FrameGenMode::Auto;
}

f32 FrameGenTargetRate() {
    // Custom Auto pacing target for Vulkan; 60 FPS is the initial default.
    const char* const value = std::getenv("LICHII_DLSS_FG_TARGET");
    if (!value || !*value) {
        return 60.0f;
    }
    char* end{};
    const f32 parsed = std::strtof(value, &end);
    return end != value && std::isfinite(parsed) && parsed >= 20.0f && parsed <= 360.0f
               ? parsed
               : 60.0f;
}

u32 FrameGenPacer::Plan(u32 capacity, f32 target_rate) {
    if (capacity == 0 || target_rate <= 0.0f) {
        Reset();
        return 0;
    }

    const Clock::time_point now = Clock::now();
    const u32 previous_generations = std::exchange(issued_generations, 0u);
    if (!last_frame) {
        last_frame = now;
        return 0;
    }

    const Clock::duration interval = now - *last_frame;
    const f32 interval_seconds = std::chrono::duration<f32>(interval).count();
    last_frame = now;
    if (interval_seconds <= 0.0f) {
        Stabilize(now);
        return 0;
    }

    if (smoothed_interval > 0.0f) {
        f32 burst_threshold = BURST_CADENCE_RATIO / smoothed_interval;
        burst_threshold = std::max(burst_threshold, target_rate * BURST_TARGET_RATIO);
        if (1.0f / interval_seconds > burst_threshold) {
            DeferEvaluations(interval);
            output_credit = 0.0f;
            return 0;
        }
    }

    if (interval_seconds > 1.0f / MINIMUM_BASE_RATE) {
        Stabilize(now);
        return 0;
    }

    smoothed_interval = smoothed_interval > 0.0f
                            ? smoothed_interval +
                                  (interval_seconds - smoothed_interval) * INTERVAL_SMOOTHING
                            : interval_seconds;

    if (previous_generations == 0) {
        const f32 measured = 1.0f / smoothed_interval;
        unloaded_base_rate =
            unloaded_base_rate > 0.0f
                ? unloaded_base_rate + (measured - unloaded_base_rate) * INTERVAL_SMOOTHING
                : measured;
    }

    if (stable_until) {
        if (now < *stable_until) {
            return 0;
        }
        stable_until.reset();
    }

    UpdateLimit(now, 1.0f / smoothed_interval, target_rate, capacity);

    const u32 allowed = std::min(limit, capacity);
    const f32 desired_outputs = smoothed_interval * target_rate;
    if (allowed == 0 || desired_outputs <= 1.0f) {
        output_credit = 0.0f;
        return 0;
    }

    output_credit += desired_outputs;
    const u32 outputs =
        std::max<u32>(1u, static_cast<u32>(std::floor(output_credit + CREDIT_EPSILON)));
    const u32 generations = std::min(outputs - 1, allowed);

    output_credit -= static_cast<f32>(generations + 1);
    if (output_credit < 0.0f) {
        output_credit = 0.0f;
    } else if (generations == allowed && output_credit >= 1.0f) {
        output_credit = std::fmod(output_credit, 1.0f);
    }

    issued_generations = generations;
    return generations;
}

void FrameGenPacer::Reset() {
    last_frame.reset();
    stable_until.reset();
    probe_until.reset();
    next_probe.reset();
    deficit_since.reset();
    smoothed_interval = 0.0f;
    output_credit = 0.0f;
    probe_base_rate = 0.0f;
    unloaded_base_rate = 0.0f;
    issued_generations = 0;
    probe_previous_limit = 0;
    limit = 0;
    probe_failures = 0;
}

FrameGenPacer::Clock::duration FrameGenPacer::ProbeBackoff(u32 failures) {
    switch (failures) {
    case 1:
        return std::chrono::seconds(5);
    case 2:
        return std::chrono::seconds(15);
    case 3:
        return std::chrono::seconds(30);
    default:
        return std::chrono::seconds(60);
    }
}

void FrameGenPacer::UpdateLimit(Clock::time_point now, f32 base_rate, f32 target_rate,
                                u32 capacity) {
    limit = std::min(limit, capacity);

    if (probe_until) {
        if (now < *probe_until) {
            return;
        }
        probe_until.reset();
        output_credit = 0.0f;

        const f32 previous_output =
            std::min(target_rate, probe_base_rate * static_cast<f32>(probe_previous_limit + 1));
        const f32 current_output =
            std::min(target_rate, base_rate * static_cast<f32>(limit + 1));

        const bool throughput_regressed =
            current_output < previous_output * PROBE_THROUGHPUT_TOLERANCE;
        const bool collapsed_for_marginal_gain =
            base_rate < probe_base_rate * PROBE_BASE_COLLAPSE_RATIO &&
            current_output < previous_output * PROBE_MARGINAL_GAIN;
        const bool emulation_slowed = unloaded_base_rate > 0.0f &&
                                      base_rate < unloaded_base_rate * UNLOADED_BASE_RETENTION;

        if (throughput_regressed || collapsed_for_marginal_gain || emulation_slowed) {
            limit = probe_previous_limit;
            probe_failures = std::min(probe_failures + 1, MAX_PROBE_FAILURES);
            next_probe = now + ProbeBackoff(probe_failures);
            deficit_since.reset();
            return;
        }

        probe_failures = 0;
        next_probe = now + PROBE_STEP_DELAY;
    }

    if (base_rate * static_cast<f32>(limit + 1) >= target_rate * TARGET_SATISFIED_RATIO ||
        limit >= capacity) {
        deficit_since.reset();
        return;
    }
    if (!deficit_since) {
        deficit_since = now;
        return;
    }
    if (now - *deficit_since < DEFICIT_DURATION || (next_probe && now < *next_probe)) {
        return;
    }

    probe_previous_limit = limit;
    probe_base_rate = base_rate;
    ++limit;
    probe_until = now + PROBE_DURATION;
    deficit_since.reset();
    output_credit = 0.0f;
}

void FrameGenPacer::DeferEvaluations(Clock::duration amount) {
    const auto defer = [amount](std::optional<Clock::time_point>& deadline) {
        if (deadline) {
            *deadline += amount;
        }
    };
    defer(stable_until);
    defer(probe_until);
    defer(next_probe);
    deficit_since.reset();
}

void FrameGenPacer::Stabilize(Clock::time_point now) {
    stable_until = now + STABILIZATION_DURATION;
    probe_until.reset();
    deficit_since.reset();
    smoothed_interval = 0.0f;
    output_credit = 0.0f;
}

} // namespace Vulkan::Streamline
