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
#include <ae/audio.h>
#include <alc.h>
#include <stdexcept>
#include <vector>
#include <glm/gtx/norm.hpp>

namespace ae {

_Audio Audio;

// Function to run the audio thread
static void RunThread(void *Arguments) {

	// Loop until audio system is closed
	while(!Audio.Done) {

		// Update streams
		Audio.UpdateMusic();

		// Sleep thread
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

// Custom file seek
int AudioFileSeek(void *Source, ogg_int64_t Offset, int Origin) {
	_AudioFile *AudioFile = (_AudioFile *)Source;
	switch(Origin) {
		case SEEK_SET:
			fseek(AudioFile->FileHandle, Offset + AudioFile->Start, SEEK_SET);
		break;
		case SEEK_CUR:
			fseek(AudioFile->FileHandle, Offset, SEEK_CUR);
		break;
		case SEEK_END:
			fseek(AudioFile->FileHandle, Offset + AudioFile->Start + AudioFile->Size, SEEK_SET);
		break;
		default:
			return -1;
		break;
	}

	return 0;
}

// Custom file tell
long AudioFileTell(void *Source) {
	_AudioFile *AudioFile = (_AudioFile *)Source;

	return ftell(AudioFile->FileHandle) - AudioFile->Start;
}

// Custom file read
size_t AudioFileRead(void *Destination, size_t Size, size_t Count, void *Source) {
	_AudioFile *AudioFile = (_AudioFile *)Source;

	// Check for reading past end of file
	long Current = AudioFileTell(AudioFile);
	if(Current + (int)(Size * Count) > AudioFile->Size) {
		Size = 1;
		Count = AudioFile->Size - Current;
	}

	return fread(Destination, Size, Count, AudioFile->FileHandle);
}

// Constructor
_AudioSource::_AudioSource(const _Sound *Sound) : SoundID(Sound->ID) {

	// Create source
	alGenSources(1, &ID);

	// Assign buffer to source
	alSourcei(ID, AL_BUFFER, (ALint)Sound->ID);
	alSourcef(ID, AL_GAIN, Sound->Volume);
}

// Destructor
_AudioSource::~_AudioSource() {
	alDeleteSources(1, &ID);
}

// Set various settings for a source
void _AudioSource::SetSettings(const _SoundSettings &SoundSettings) const {
	alSourcei(ID, AL_SOURCE_RELATIVE, SoundSettings.Relative);
	alSourcei(ID, AL_LOOPING, SoundSettings.Loop);
	alSourcef(ID, AL_MIN_GAIN, SoundSettings.MinGain);
	alSourcef(ID, AL_MAX_GAIN, SoundSettings.MaxGain);
	if(SoundSettings.Relative) {
		alSource3f(ID, AL_POSITION, 0.0f, 0.0f, 0.0f);
	}
	else {
		alSourcef(ID, AL_REFERENCE_DISTANCE, SoundSettings.ReferenceDistance);
		alSourcef(ID, AL_MAX_DISTANCE, SoundSettings.MaxDistance);
		alSourcef(ID, AL_ROLLOFF_FACTOR, SoundSettings.RollOff);
		alSource3f(ID, AL_POSITION, SoundSettings.Position.x, SoundSettings.Position.y, SoundSettings.Position.z);
	}
}

// Determine if source is actively playing
bool _AudioSource::IsPlaying() const {
	ALenum State;
	alGetSourcei(ID, AL_SOURCE_STATE, &State);

	return State == AL_PLAYING;
}

// Play
void _AudioSource::Play() const {
	if(ID)
		alSourcePlay(ID);
}

// Stop
void _AudioSource::Stop() const {
	if(ID)
		alSourceStop(ID);
}

// Returns true if the source is relative
bool _AudioSource::IsRelative() const {
	ALenum State;

	alGetSourcei(ID, AL_SOURCE_RELATIVE, &State);

	return State == AL_TRUE;
}

// Set relative
void _AudioSource::SetRelative(bool Value) const {
	alSourcei(ID, AL_SOURCE_RELATIVE, Value);
}

// Set looping
void _AudioSource::SetLooping(bool Value) const {
	alSourcei(ID, AL_LOOPING, Value);
}

// Set pitch
void _AudioSource::SetPitch(float Value) const {
	alSourcef(ID, AL_PITCH, Value);
}

// Set gain
void _AudioSource::SetGain(float Value) const {
	alSourcef(ID, AL_GAIN, Value);
}

// Set position
void _AudioSource::SetPosition(const glm::vec3 &Position) const {
	alSource3f(ID, AL_POSITION, Position.x, Position.y, Position.z);
}

// Get source position
glm::vec3 _AudioSource::GetPosition() const {
	float Position[3];
	alGetSource3f(ID, AL_POSITION, &Position[0], &Position[1], &Position[2]);

	return glm::vec3(Position[0], Position[1], Position[2]);
}

// Destructor
_Sound::~_Sound() {
	alDeleteBuffers(1, &ID);
}

// Destructor
_Music::~_Music() {
	if(Loaded)
		ov_clear(&Stream);
}

// Initialize
void _Audio::Init(bool Enabled, bool StartMusicThread) {
	if(!Enabled)
		return;

	// Open device
	ALCdevice *Device = alcOpenDevice(nullptr);
	if(Device == nullptr)
		throw std::runtime_error(std::string(__func__) + " alcOpenDevice failed");

	// Create context
	ALCcontext *Context = alcCreateContext(Device, nullptr);
	alcMakeContextCurrent(Context);

	// Create music buffers
	alGenBuffers(BUFFER_COUNT, MusicBuffers);
	alGenSources(1, &MusicSource);

	// Clear code
	alGetError();

	// Initialize
	CurrentSong = nullptr;
	NewSong = nullptr;
	Done = false;
	this->Enabled = Enabled;

	// Start thread
	if(StartMusicThread)
		Thread = new std::thread(RunThread, this);
}

// Close
void _Audio::Close() {
	if(!Enabled)
		return;

	// Close thread
	Done = true;
	if(Thread)
		Thread->join();

	delete Thread;
	Thread = nullptr;

	// Delete sources
	for(auto &Iterator : Sources)
		delete Iterator;

	// Delete channels
	for(auto &Iterator : Channels) {
		for(const auto &AudioSource : Iterator.second.AudioSources) {
			delete AudioSource;
		}
	}
	Channels.clear();

	Sources.clear();

	// Get active context
	ALCcontext *Context = alcGetCurrentContext();

	// Get device for active context
	ALCdevice *Device = alcGetContextsDevice(Context);

	// Disable context
	alcMakeContextCurrent(nullptr);

	// Free context
	alcDestroyContext(Context);

	// Close device
	alcCloseDevice(Device);

	Enabled = false;
}

// Update audio
void _Audio::Update(double FrameTime) {
	if(!Enabled)
		return;

	// Update sources
	for(size_t i = 0; i < Sources.size(); i++) {
		const _AudioSource *Source = Sources[i];

		// Delete source
		if(!Source->IsPlaying()) {
			delete Source;
			if(i < Sources.size() - 1)
				Sources[i] = Sources.back();
			Sources.pop_back();
		}
	}
}

// Update music stream
void _Audio::UpdateMusic() {

	// Check for stopped song
	if(CurrentSong && CurrentSong->Stop)
		CurrentSong = nullptr;

	// Check for track change
	if(NewSong) {
		alSourceStop(MusicSource);

		// Clear queued buffers
		ALint Queued;
		alGetSourcei(MusicSource, AL_BUFFERS_QUEUED, &Queued);
		while(Queued--) {
			ALuint CurrentBuffer;
			alSourceUnqueueBuffers(MusicSource, 1, &CurrentBuffer);
		}

		// Seek to beginning
		ov_time_seek(&NewSong->Stream, 0);

		// Load initial buffers
		for(int i = 0; i < BUFFER_COUNT; i++) {

			// Queue buffers
			bool End = QueueBuffers(NewSong, MusicBuffers[i]);
			if(End) {
				NewSong = nullptr;
				break;
			}
		}

		// Play music
		alSourcef(MusicSource, AL_GAIN, MusicVolume);
		alSourcePlay(MusicSource);

		CurrentSong = NewSong;
		NewSong = nullptr;
	}

	// Get number of buffers processed
	ALint Processed;
	alGetSourcei(MusicSource, AL_BUFFERS_PROCESSED, &Processed);

	// Handle processed buffers
	while(Processed-- > 0) {

		// Pop buffer
		ALuint CurrentBuffer;
		alSourceUnqueueBuffers(MusicSource, 1, &CurrentBuffer);

		// Check for song
		if(CurrentSong) {

			// Queue buffers
			bool End = QueueBuffers(CurrentSong, CurrentBuffer);
			if(End)
				CurrentSong = nullptr;
		}
	}
}

// Initialize a channel with a sound and sound limit
void _Audio::LoadChannel(const _Sound *Sound) {
	if(Sound->Limit <= 0)
		throw std::runtime_error(std::string(__func__) + " bad sound limit for sound_id " + std::to_string(Sound->ID));

	// Create audio sources
	_Channel Channel;
	for(int i = 0; i < Sound->Limit; i++)
		Channel.AudioSources.push_back(new _AudioSource(Sound));

	Channels[Sound] = Channel;
}

// Load sound
_Sound *_Audio::LoadSound(const std::string &Path) {
	if(!Enabled)
		return nullptr;

	// Open file
	OggVorbis_File VorbisStream;
	OpenVorbis(Path, &VorbisStream);

	return LoadSoundData(&VorbisStream);
}

// Load sound from handle
_Sound *_Audio::LoadSound(const _AudioFile &AudioFile) {
	if(!Enabled)
		return nullptr;

	// Open file
	OggVorbis_File VorbisStream;
	OpenVorbis(AudioFile, &VorbisStream);

	return LoadSoundData(&VorbisStream);
}

// Load music
_Music *_Audio::LoadMusic(const std::string &Path) {
	if(!Enabled)
		return nullptr;

	_Music *Music = new _Music();

	// Get vorbis file info
	OpenVorbis(Path, &Music->Stream);
	GetVorbisInfo(&Music->Stream, Music->Frequency, Music->Format);
	Music->Loaded = true;

	return Music;
}

// Play a sound
const _AudioSource *_Audio::PlaySound(const _Sound *Sound, const _SoundSettings &SoundSettings) {
	if(!Enabled || !Sound)
		return nullptr;

	// Check max distance
	if(!SoundSettings.Relative) {
		float DistanceSquared = glm::distance2(SoundSettings.Position, GetPosition());
		if(DistanceSquared > MaxDistanceSquared)
			return nullptr;
	}

	// Get audio source
	const _AudioSource *AudioSource;
	if(Sound->Limit) {

		// Get channel
		const auto &Iterator = Channels.find(Sound);
		if(Iterator == Channels.end())
			return nullptr;

		_Channel &Channel = Iterator->second;

		// Get next audio source
		AudioSource = Channel.AudioSources[Channel.Index];

		// Update next channel index
		Channel.Index++;
		if(Channel.Index >= Channel.AudioSources.size())
			Channel.Index = 0;
	}
	else {

		// Create new source
		AudioSource = new _AudioSource(Sound);

		// Add to list
		Sources.push_back(AudioSource);
	}

	// Create audio source
	AudioSource->SetSettings(SoundSettings);
	AudioSource->SetGain(SoundVolume * Sound->Volume * SoundSettings.Volume);
	AudioSource->Play();

	return AudioSource;
}

// Play music
void _Audio::PlayMusic(_Music *Music, bool Loop) {
	if(!Enabled)
		return;

	// Stop music if playing nothing
	if(!Music) {
		StopMusic();
		return;
	}

	// Change songs only
	if(CurrentSong != Music) {
		NewSong = Music;
		NewSong->Stop = false;
		NewSong->Loop = Loop;
	}
}

// Stop an audio source
void _Audio::StopSource(const _AudioSource *AudioSource) {

	// Find source
	for(size_t i = 0; i < Sources.size(); i++) {
		if(AudioSource == Sources[i])
			AudioSource->Stop();
	}
}

// Stop all sound and music
void _Audio::Stop() {
	StopSounds();
	StopMusic();
}

// Stop all sounds
void _Audio::StopSounds() {
	for(auto &Source : Sources) {
		Source->Stop();
		delete Source;
	}

	Sources.clear();
}

// Stop all music
void _Audio::StopMusic() {
	alSourceStop(MusicSource);

	// Request song stop
	if(CurrentSong)
		CurrentSong->Stop = true;

	NewSong = nullptr;

	// Wait for thread to kill current song
	while(CurrentSong)
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

// Read data from a vorbis stream
long _Audio::ReadStream(OggVorbis_File *VorbisFile, char *Buffer, int Size) {

	// Decode vorbis file
	int BitStream;
	long BytesRead = ov_read(VorbisFile, Buffer, Size, 0, 2, 1, &BitStream);

	return BytesRead;
}

// Open vorbis file and return format/rate
void _Audio::OpenVorbis(const std::string &Path, OggVorbis_File *VorbisFile) {

	// Open file
	int ReturnCode = ov_fopen(Path.c_str(), VorbisFile);
	if(ReturnCode != 0)
		throw std::runtime_error(std::string(__func__) + " ov_fopen failed: ReturnCode=" + std::to_string(ReturnCode));
}

// Open vorbis file from a handle and return format/rate
void _Audio::OpenVorbis(const _AudioFile &AudioFile, OggVorbis_File *VorbisFile) {

	// Set up custom file functions
	ov_callbacks Callbacks = {
		(size_t (*)(void *, size_t, size_t, void *)) AudioFileRead,
		(int (*)(void *, ogg_int64_t, int)) AudioFileSeek,
		nullptr,
		(long (*)(void *)) AudioFileTell,
	};

	// Open file
	int ReturnCode = ov_open_callbacks((void *)&AudioFile, VorbisFile, nullptr, 0, Callbacks);
	if(ReturnCode != 0)
		throw std::runtime_error(std::string(__func__) + " ov_fopen failed: ReturnCode=" + std::to_string(ReturnCode));
}

// Get stream info
void _Audio::GetVorbisInfo(OggVorbis_File *VorbisFile, long &Rate, int &Format) {

	// Get vorbis file info
	vorbis_info *Info = ov_info(VorbisFile, -1);
	Rate = Info->rate;

	// Get openal format
	switch(Info->channels) {
		case 1:
			Format = AL_FORMAT_MONO16;
		break;
		case 2:
			Format = AL_FORMAT_STEREO16;
		break;
		default:
			throw std::runtime_error(std::string(__func__) + " unsupported number of channels: " + std::to_string(Info->channels));
	}
}

// Queue openal buffers with vorbis stream data, return true on stream end or error
bool _Audio::QueueBuffers(_Music *Music, ALuint Buffer) {

	// Decode vorbis stream
	char Data[BUFFER_SIZE];
	long BytesNeeded = BUFFER_SIZE;
	while(BytesNeeded > 0) {

		// Read some bytes
		int BitStream;
		long BytesRead = ov_read(&Music->Stream, Data + (BUFFER_SIZE - BytesNeeded), (int)BytesNeeded, 0, 2, 1, &BitStream);
		if(BytesRead < 0)
			return true;

		// Handle track end
		if(BytesRead == 0) {
			if(Music->Loop)
				ov_time_seek(&Music->Stream, 0);
			else
				return true;
		}
		else
			BytesNeeded -= BytesRead;
	}

	if(Music) {
		alBufferData(Buffer, Music->Format, Data, (ALsizei)BUFFER_SIZE, (ALsizei)Music->Frequency);
		alSourceQueueBuffers(MusicSource, 1, &Buffer);
	}

	return false;
}

// Set sound volume
void _Audio::SetSoundVolume(float Volume) {
	if(!Enabled)
		return;

	SoundVolume = std::min(std::max(Volume, 0.0f), 1.0f);
}

// Set music volume
void _Audio::SetMusicVolume(float Volume) {
	if(!Enabled)
		return;

	MusicVolume = std::min(std::max(Volume, 0.0f), 1.0f);
	alSourcef(MusicSource, AL_GAIN, MusicVolume);
}

// Set listener position
void _Audio::SetPosition(const glm::vec3 &Position) {
	if(!Enabled)
		return;

	alListener3f(AL_POSITION, Position.x, Position.y, Position.z);
}

// Set listener direction
void _Audio::SetDirection(const glm::vec3 &Look, const glm::vec3 &Up) {
	if(!Enabled)
		return;

	float Orientation[6] = { Look.x, Look.y, Look.z, Up.x, Up.y, Up.z };
	alListenerfv(AL_ORIENTATION, Orientation);
}

// Get listener position
glm::vec3 _Audio::GetPosition() {
	float Position[3];
	alGetListener3f(AL_POSITION, &Position[0], &Position[1], &Position[2]);

	return glm::vec3(Position[0], Position[1], Position[2]);
}

// Load sound data from a vorbis stream
_Sound *_Audio::LoadSoundData(OggVorbis_File *VorbisFile) {

	// Decode vorbis file
	std::vector<char> Data;
	long BytesRead;
	char Buffer[BUFFER_SIZE];
	int BitStream;
	do {
		BytesRead = ov_read(VorbisFile, Buffer, BUFFER_SIZE, 0, 2, 1, &BitStream);
		Data.insert(Data.end(), Buffer, Buffer + BytesRead);
	} while(BytesRead > 0);

	// Get info
	long Rate;
	int Format;
	GetVorbisInfo(VorbisFile, Rate, Format);

	// Create buffer
	_Sound *Sound = new _Sound();
	alGenBuffers(1, &Sound->ID);
	alBufferData(Sound->ID, Format, &Data[0], (ALsizei)Data.size(), (ALsizei)Rate);

	// Close vorbis file
	ov_clear(VorbisFile);

	return Sound;
}

}
