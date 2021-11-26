#include <array>
#include <chrono>
#include <thread>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

namespace {

using Clock = std::chrono::steady_clock;
using Timestamp = Clock::time_point;
using Duration = Timestamp::duration;

inline Timestamp Now() { return Clock::now(); }

inline Duration PeriodFromFrequency(int32_t frequency) {
    return std::chrono::duration_cast<Duration>(
        std::chrono::nanoseconds(1'000'000'000 / frequency));
}

const sf::VideoMode kVideoMode(640, 480);
const unsigned int kFrameLimit = 60;

const auto kUpdatePeriod = PeriodFromFrequency(100);

const float kMusicVolumePerTick = 0.5F;

}  // namespace

class MusicController {
public:
    MusicController() : active_idx_(0), inactive_idx(1) {}

    void Play(const std::string& filename) {
        if (filename_ == filename) {
            return;
        }

        filename_ = filename;

        std::swap(active_idx_, inactive_idx);

        if (filename.empty()) {
            music_[active_idx_].stop();
        }
        else {
            music_[active_idx_].openFromFile(filename);
            music_[active_idx_].setVolume(0.0F);
            music_[active_idx_].play();
        }
    }

    void Update() {
        if (music_[inactive_idx].getStatus() == sf::SoundSource::Status::Playing) {
            float inactive_volume = music_[inactive_idx].getVolume();
            if (inactive_volume > 0.0F) {
                inactive_volume = std::max(inactive_volume - kMusicVolumePerTick, 0.0F);
                music_[inactive_idx].setVolume(inactive_volume);
            }
        }

        if (music_[active_idx_].getStatus() == sf::SoundSource::Status::Playing) {
            float active_volume = music_[active_idx_].getVolume();
            if (active_volume < 100.0F) {
                active_volume = std::min(active_volume + kMusicVolumePerTick, 100.0F);
                music_[active_idx_].setVolume(active_volume);
            }
        }
    }

private:
    std::array<sf::Music, 2> music_;

    size_t active_idx_;
    size_t inactive_idx;

    std::string filename_;
};

int main() {
    sf::RenderWindow window(kVideoMode, "demo");
    window.setFramerateLimit(kFrameLimit);

    MusicController music_controller;
    music_controller.Play("Greatmatis_-_Crossroads.flac");

    auto update_ts = Now();
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == event.KeyReleased) {
                switch (event.key.code) {
                    case sf::Keyboard::Escape:
                        window.close();
                        break;

                    case sf::Keyboard::Num1:
                        music_controller.Play("Greatmatis_-_Crossroads.flac");
                        break;

                    case sf::Keyboard::Num2:
                        music_controller.Play("Greatmatis_-_Drama_Initiative.flac");
                        break;

                    case sf::Keyboard::Num0:
                        music_controller.Play("");
                        break;

                    default:
                        break;
                }
            }
        }

        if (Now() < update_ts)
            std::this_thread::sleep_until(update_ts);
        const auto current_time = Now();

        while (current_time >= update_ts) {
            update_ts += kUpdatePeriod;
            music_controller.Update();
        }

        window.clear();
        window.display();
    }

    return 0;
}
