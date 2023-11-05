#pragma once
#include <span>

namespace Audio {

/// Returns the Root Mean Square (RMS) of the samples in the buffer.
/// The data MUST NOT be multi-channel. If it is, please merge them into a single channel by averaging the values of the various channels.
auto compute_volume(std::span<float const> data) -> float;

} // namespace Audio