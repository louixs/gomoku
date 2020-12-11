#include "SoundPlayer.hpp"

#include <SFML/Audio/Listener.hpp>

#include <cmath>

namespace
{
  // Sound coordinate system, point of view of a player in front of the screen:
  // X = left; Y = up; Z = back (out of the screen)
  const float ListenerZ = 300.f;
  const float Attenuation = 8.f;
  const float MinDistance2D = 200.f;
  const float MinDistance3D = std::sqrt(MinDistance2D*MinDistance2D + ListenerZ*ListenerZ);
}

SoundPlayer::SoundPlayer()
: mSoundBuffers()
, mSounds() {
  mSoundBuffers.load(SoundEffect::Greetings, "assets/greetings.wav");
  mSoundBuffers.load(SoundEffect::HavingLaungh, "assets/having_laugh.wav");
  mSoundBuffers.load(SoundEffect::MissionCompleted, "assets/mission_completed.wav");
  mSoundBuffers.load(SoundEffect::SomePeople, "assets/some_people.wav");
  mSoundBuffers.load(SoundEffect::Stone1, "assets/stone_click_1.wav");
  mSoundBuffers.load(SoundEffect::Stone2, "assets/stone_click_2.wav");

  sf::Listener::setDirection(0.f, 0.f, -1.f);
}

void SoundPlayer::play(SoundEffect::ID effect) {
  play(effect, getListenerPosition());
}

void SoundPlayer::play(SoundEffect::ID effect, sf::Vector2f position) {
  mSounds.push_back(sf::Sound());
  sf::Sound& sound = mSounds.back();

  sound.setBuffer(mSoundBuffers.get(effect));
  sound.setPosition(position.x, -position.y, 0.f);
  sound.setAttenuation(Attenuation);
  sound.setMinDistance(MinDistance3D);

  sound.play();
}

void SoundPlayer::removeStoppedSounds() {
  mSounds.remove_if([] (const sf::Sound& s) {
    return s.getStatus() == sf::Sound::Stopped;
    }
  );
}

sf::Vector2f SoundPlayer::getListenerPosition() const {
  sf::Vector3f position = sf::Listener::getPosition();
  return sf::Vector2f(position.x, -position.y);
}
