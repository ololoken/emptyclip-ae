/******************************************************************************
* Copyright (c) 2022 Alan Witkowski
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
#include <list>
#include <string>

namespace ae {

// Sound class
class _Sound {

	public:

		_Sound() : ID(0), Volume(1.0f), Limit(0) { }
		~_Sound();

		ALuint ID;
		float Volume;
		int Limit;
};

// Music class
class _Music {

	public:

		_Music() : Loaded(false), Loop(false), Stop(false), Format(0), Frequency(0) { }
		~_Music();

		bool Loaded;
		bool Loop;
		bool Stop;
		ALenum Format;
		long Frequency;
		OggVorbis_File Stream;
};

// Audio source class
class _AudioSource {

	public:

		_AudioSource(const _Sound *Sound, float Volume=1.0f);
		~_AudioSource();

		void Play() const;
		void Stop() const;

		bool IsPlaying() const;

		ALuint ID;
};

// Wrapper around file handle
struct _AudioFile {

	_AudioFile() : FileHandle(nullptr), Start(0), Size(0) { }
	_AudioFile(FILE *FileHandle, int Start, int Size) : FileHandle(FileHandle), Start(Start), Size(Size) { }

	FILE *FileHandle;
	int Start;
	int Size;
};

// Audio class
class _Audio {

	public:

		static const int BUFFER_COUNT = 3;
		static const int BUFFER_SIZE = 4096;

		_Audio();

		void Init(bool Enabled);
		void Close();

		void Update(double FrameTime);
		void UpdateMusic();

		_Sound *LoadSound(const std::string &Path);
		_Sound *LoadSound(const _AudioFile &AudioFile);
		_Music *LoadMusic(const std::string &Path);

		void Stop();
		void StopSounds();
		void StopMusic();
		const _AudioSource *PlaySound(_Sound *Sound, float Volume=1.0f);
		const _AudioSource *PlaySound(_Sound *Sound, const glm::vec3 &Position, float Volume=1.0f, bool Loop=false, float MinGain=0.0f, float MaxGain=1.0f, float ReferenceDistance=10.0f, float MaxDistance=100.0f, float RollOff=2.5f);
		void PlayMusic(_Music *Music, bool Loop=true);

		void SetSoundVolume(float Volume);
		void SetMusicVolume(float Volume);
		void SetPosition(const glm::vec3 &Position);
		void SetDirection(const glm::vec3 &Look, const glm::vec3 &Up);
		glm::vec3 GetPosition();

		bool Done;

	private:

		_Sound *LoadSoundData(OggVorbis_File *VorbisFile);

		long ReadStream(OggVorbis_File *VorbisFile, char *Buffer, int Size);
		void OpenVorbis(const std::string &Path, OggVorbis_File *VorbisFile);
		void OpenVorbis(const _AudioFile &AudioFile, OggVorbis_File *VorbisFile);
		void GetVorbisInfo(OggVorbis_File *VorbisFile, long &Rate, int &Format);
		bool QueueBuffers(_Music *Music, ALuint Buffer);

		bool Enabled;

		float SoundVolume;
		float MusicVolume;

		ALuint MusicSource;
		ALuint MusicBuffers[BUFFER_COUNT];

		_Music *CurrentSong;
		_Music *NewSong;

		std::list<const _AudioSource *> Sources;

		std::thread *Thread;
};

extern _Audio Audio;

}
