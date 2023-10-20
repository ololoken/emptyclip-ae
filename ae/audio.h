/******************************************************************************
* Copyright (c) 2023 Alan Witkowski
*
* This software is provided 'as-is', without any express or implied
* warranty. In no event will the authors be held liable for any damages
* arising from the use of this software.
*
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
*
* 1. The origin of this software must not be misrepresented; you must not
*    claim that you wrote the original software. If you use this software
*    in a product, an acknowledgment in the product documentation would be
*    appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
*    misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*******************************************************************************/
#pragma once

// Libraries
#include <al.h>
#include <vorbis/vorbisfile.h>
#include <glm/vec3.hpp>
#include <thread>
#include <unordered_map>
#include <vector>
#include <string>

namespace ae {

// Settings for audio sources
struct _SoundSettings {

	_SoundSettings(float Volume=1.0f) : Volume(Volume) {}
	_SoundSettings(const glm::vec3 &Position, float Volume=1.0f) : Position(Position), Volume(Volume), Relative(false) {}
	_SoundSettings(const glm::vec3 &Position, float Volume, float ReferenceDistance, float MaxDistance, float RollOff) : Position(Position), Volume(Volume), ReferenceDistance(ReferenceDistance), MaxDistance(MaxDistance), RollOff(RollOff), Relative(false) {}

	glm::vec3 Position{0.0f};
	float Volume{1.0f};
	float MinGain{0.0f};
	float MaxGain{1.0f};
	float ReferenceDistance{10.0f};
	float MaxDistance{100.0f};
	float RollOff{2.5f};
	bool Loop{false};
	bool Relative{true};
};

// Sound class
class _Sound {

	public:

		~_Sound();

		ALuint ID{0};
		float Volume{1.0f};
		int Limit{0};
};

// Music class
class _Music {

	public:

		~_Music();

		bool Loaded{false};
		bool Loop{false};
		bool Stop{false};
		ALenum Format{0};
		long Frequency{0};
		OggVorbis_File Stream;
};

// Audio source class
class _AudioSource {

	public:

		_AudioSource(const _Sound *Sound);
		~_AudioSource();

		void SetSettings(const _SoundSettings &SoundSettings) const;
		void Play() const;
		void Stop() const;

		bool IsPlaying() const;
		bool IsRelative() const;

		void SetRelative(bool Value) const;
		void SetLooping(bool Value) const;
		void SetPitch(float Value) const;
		void SetGain(float Value) const;
		void SetPosition(const glm::vec3 &Position) const;
		glm::vec3 GetPosition() const;

		ALuint ID;
		ALuint SoundID;
};

// Holds audio sources
struct _Channel {
	std::vector<const _AudioSource *> AudioSources;
	size_t Index{0};
};

// Wrapper around file handle
struct _AudioFile {

	_AudioFile(FILE *FileHandle, int Start, int Size) : FileHandle(FileHandle), Start(Start), Size(Size) {}

	FILE *FileHandle{nullptr};
	int Start{0};
	int Size{0};
};

// Audio class
class _Audio {

	public:

		static const int BUFFER_COUNT{3};
		static const int BUFFER_SIZE{4096};

		void Init(bool Enabled, bool StartMusicThread=true, const char *DeviceName=nullptr);
		void Close();

		void Update(double FrameTime);
		void UpdateMusic();

		void LoadChannel(const _Sound *Sound);
		_Sound *LoadSound(const std::string &Path);
		_Sound *LoadSound(const _AudioFile &AudioFile);
		_Music *LoadMusic(const std::string &Path);

		const _AudioSource *PlaySound(const _Sound *Sound, const _SoundSettings &SoundSettings=_SoundSettings(1.0f));
		void PlayMusic(_Music *Music, bool Loop=true);
		void StopSource(const _AudioSource *AudioSource);
		void Stop();
		void StopSounds();
		void StopMusic();

		void SetMaxDistance(float Distance) { MaxDistanceSquared = Distance * Distance; }
		void SetSoundVolume(float Volume);
		void SetMusicVolume(float Volume);
		void SetPosition(const glm::vec3 &Position);
		void SetDirection(const glm::vec3 &Look, const glm::vec3 &Up);

		float GetSoundVolume() const { return SoundVolume; }
		float GetMusicVolume() const { return MusicVolume; }
		glm::vec3 GetPosition();

		bool IsEnabled() const { return Enabled; }

		bool Done{false};

	private:

		_Sound *LoadSoundData(OggVorbis_File *VorbisFile);

		long ReadStream(OggVorbis_File *VorbisFile, char *Buffer, int Size);
		void OpenVorbis(const std::string &Path, OggVorbis_File *VorbisFile);
		void OpenVorbis(const _AudioFile &AudioFile, OggVorbis_File *VorbisFile);
		void GetVorbisInfo(OggVorbis_File *VorbisFile, long &Rate, int &Format);
		bool QueueBuffers(_Music *Music, ALuint Buffer);

		bool Enabled{false};
		float SoundVolume{1.0f};
		float MusicVolume{1.0f};
		float MaxDistanceSquared{10000.0f};

		ALuint MusicSource{0};
		ALuint MusicBuffers[BUFFER_COUNT]{0};

		_Music *CurrentSong{nullptr};
		_Music *NewSong{nullptr};

		std::vector<const _AudioSource *> Sources;
		std::unordered_map<const _Sound *, _Channel> Channels;

		std::thread *Thread{nullptr};
};

extern _Audio Audio;

}
