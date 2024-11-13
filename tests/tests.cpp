#include <exe_path/exe_path.h>
#include <Audio/Audio.hpp>
#include <algorithm>
#include <complex>
#include <iterator>
#include <quick_imgui/quick_imgui.hpp>
#include "imgui.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest/doctest.h>

// Learn how to use Dear ImGui: https://coollibs.github.io/contribute/Programming/dear-imgui

static auto window(int64_t idx, int64_t size) -> float
{
    float const t = static_cast<float>(idx) / static_cast<float>(size - 1);
    return 1.f - std::abs(2.f * t - 1.f); // Triangular window
}

static auto get_device_name_impl(Audio::InputStream const& input_stream, Audio::UseDefaultDevice) -> std::string
{
    return "Use default device: " + input_stream.device_info(input_stream.default_device_id()).name;
}
static auto get_device_name_impl(Audio::InputStream const&, Audio::UseGivenDevice const& device) -> std::string
{
    return device.name;
}

static auto get_device_name(Audio::InputStream const& input_stream, Audio::SelectedDevice const& selected_device) -> std::string
{
    return std::visit([&](auto&& selected_device) { return get_device_name_impl(input_stream, selected_device); }, selected_device);
}

auto main(int argc, char* argv[]) -> int
{
    int const  exit_code              = doctest::Context{}.run();                   // Run all unit tests
    bool const should_run_imgui_tests = argc < 2 || strcmp(argv[1], "-nogpu") != 0; // NOLINT(*pro-bounds-pointer-arithmetic)
    if (!should_run_imgui_tests
        || exit_code != 0 // Only open the window if the tests passed; this makes it easier to notice when some tests fail
    )
    {
        return exit_code;
    }

    // Input stream
    Audio::InputStream input_stream{
        [](RtAudioErrorType /* type */, std::string const& error_message) {
            std::cerr << error_message << '\n';
        }
    };
    static constexpr size_t nb_samples_in_input_stream{512};
    input_stream.set_nb_of_retained_samples(nb_samples_in_input_stream);
    // Load the audio file
    Audio::load_audio_file(Audio::player(), exe_path::dir() / "../../tests/res/Monteverdi - L'Orfeo, Toccata.mp3");
    Audio::player().play();

    static constexpr int64_t fft_size{8000};
    float                    max_spectrum_frequency_in_hz{15000.f};

    quick_imgui::loop("Audio tests", [&]() { // Open a window and run all the ImGui-related code
        auto const spectrum = Audio::fourier_transform(
            fft_size,
            [&](std::function<void(float)> const& callback) {
                for (int64_t i = 0; i < fft_size; i++)
                    callback(
                        window(i, fft_size)
                        * Audio::player().sample_unaltered_volume(i + Audio::player().current_frame_index())
                    );
            },
            static_cast<float>(Audio::player().audio_data().sample_rate),
            max_spectrum_frequency_in_hz
        );

        ImGui::Begin("Audio tests");
        // Player
        ImGui::SeparatorText("Player");
        if (ImGui::Button(Audio::player().properties().is_muted ? "Unmute" : "Mute"))
            Audio::player().properties().is_muted = !Audio::player().properties().is_muted;
        ImGui::PlotLines(
            "Spectrum",
            spectrum.data.data(),
            static_cast<int>(spectrum.data.size()),
            0, nullptr,
            0.f, 1.f,
            {0.f, 100.f}
        );
        ImGui::SliderFloat("Max spectrum frequency", &max_spectrum_frequency_in_hz, 0.f, 22000.f, "%.0f Hertz");

        // Input stream
        ImGui::NewLine();
        ImGui::SeparatorText("Input stream");
        auto const input_device_ids = input_stream.device_ids();
        auto const combo_text       = get_device_name(input_stream, input_stream.current_device());
        if (ImGui::BeginCombo("Input device", combo_text.c_str()))
        {
            bool const is_selected = std::holds_alternative<Audio::UseDefaultDevice>(input_stream.current_device());
            if (ImGui::Selectable(get_device_name_impl(input_stream, Audio::UseDefaultDevice{}).c_str(), is_selected))
                input_stream.use_default_device();
            if (is_selected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                ImGui::SetItemDefaultFocus();

            for (auto const id : input_device_ids)
            {
                auto const info         = input_stream.device_info(id);
                bool const is_selected2 = info.name == combo_text;
                if (ImGui::Selectable(info.name.c_str(), is_selected2))
                {
                    input_stream.use_given_device(info);
                }

                if (is_selected2) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        auto data_from_input_stream = std::vector<float>{};
        input_stream.for_each_sample(nb_samples_in_input_stream, [&](float const sample) {
            data_from_input_stream.push_back(sample);
        });
        ImGui::PlotLines(
            "Waveform",
            data_from_input_stream.data(),
            static_cast<int>(data_from_input_stream.size()),
            0, nullptr,
            -0.1f, 0.1f,
            {0.f, 100.f}
        );
        //
        ImGui::End();
        ImGui::ShowDemoWindow();
    });
    Audio::shut_down();
    return 0;
}

TEST_CASE("At least one API has been compiled")
{
    Audio::Player player{}; // This will assert if no API is available, which is something we want to detect.
}

TEST_CASE("Loading a .wav file")
{
    Audio::load_audio_file(Audio::player(), exe_path::dir() / "../../tests/res/10-1000-10000-20000.wav");

    CHECK(Audio::player().audio_data().channels_count == 1);
    CHECK(Audio::player().audio_data().sample_rate == 41000);
    CHECK(Audio::player().audio_data().samples.size() == 164000);
}

TEST_CASE("Loading a .mp3 file")
{
    Audio::load_audio_file(Audio::player(), exe_path::dir() / "../../tests/res/Monteverdi - L'Orfeo, Toccata.mp3");

    CHECK(Audio::player().audio_data().channels_count == 2);
    CHECK(Audio::player().audio_data().sample_rate == 44100);
    CHECK(Audio::player().audio_data().samples.size() == 9819648);
}

static auto is_big(float x) -> bool
{
    return x > 5.f;
}
static auto is_small(float x) -> bool
{
    return x < 0.1f;
}

static constexpr float TAU = 6.2831853071f;

TEST_CASE("Fourier transform")
{
    auto fft_input = std::vector<std::complex<float>>{};

    static constexpr int64_t sample_rate = 44000; // Allows us to detect frequencies up to sample_rate / 2 = 22000Hz
    static constexpr int64_t fft_size    = 8000;  // Will give us a good enough resolution (fft_size / 2 = 4000 values, spread between 0Hz and 22000Hz)

    auto const spectrum = Audio::fourier_transform(
        fft_size, [&](std::function<void(float)> const& callback) {
            for (int64_t i = 0; i < fft_size; i++)
            {
                float time = static_cast<float>(i) / static_cast<float>(sample_rate);
                callback(
                    window(i, fft_size)
                    * (std::sin(10.f * time * TAU)      // 10Hz frequency
                       + std::sin(1000.f * time * TAU)  // 1000Hz frequency
                       + std::sin(10000.f * time * TAU) // 10000Hz frequency
                       + std::sin(20000.f * time * TAU) // 20000Hz frequency
                    )
                );
            }
        },
        static_cast<float>(sample_rate)
    );

    CHECK(is_big(spectrum.at_frequency(10.f)));
    CHECK(is_big(spectrum.at_frequency(1000.f)));
    CHECK(is_big(spectrum.at_frequency(10000.f)));
    CHECK(is_big(spectrum.at_frequency(20000.f)));
    CHECK(is_small(spectrum.at_frequency(500.f)));
    CHECK(is_small(spectrum.at_frequency(5000.f)));
    CHECK(is_small(spectrum.at_frequency(15000.f)));
}