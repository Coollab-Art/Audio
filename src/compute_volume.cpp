#include "compute_volume.hpp"
#include <cmath>

namespace Audio {

auto compute_volume(std::span<float const> data) -> float
{
    if (data.empty())
        return 0.f;
    // TODO(Audio-Philippe) Implement a much smarter loudness computation like https://github.com/klangfreund/LUFSMeter/tree/master
    float sum_of_squares{0.f};
    for (float const sample : data)
        sum_of_squares += sample * sample;
    return std::sqrt(sum_of_squares / static_cast<float>(data.size())); // TODO(Audio) Return the result in decibels ?
}

} // namespace Audio