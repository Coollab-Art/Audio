#pragma once
#include "RtAudioWrapper/RtAudioWrapper.hpp"

namespace Cool {

/// Returns a number between 0 and 1.
auto compute_volume(RtAudioW::Player const& player, float average_duration_in_seconds) -> float;

} // namespace Cool