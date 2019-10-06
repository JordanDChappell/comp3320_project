/* audio.hpp
 * Contains functions to help load audio into an audio buffer
 * Currently supports .wav audio files
 * Has a source class which holds a source and can play audio from a buffer
 */

#ifndef ASSIGNMENT_AUDIO_HPP
#define ASSIGNMENT_AUDIO_HPP

namespace audio {
	// Precondition:	"file" is a .wav audio file
	// Postcondition:	returns buffer with audio data for "file"
	GLuint loadAudio(const char* file) {
		// Open the file
		FILE* fp = NULL;
		fp = fopen(file, "rb");

		// Declare variables for dealing with .wav data
		char type[4];
		DWORD size, chunkSize;
		short formatType, channels;
		DWORD sampleRate, avgBytesPerSec;
		short bytesPerSample, bitsPerSample;
		DWORD dataSize;

		// Reading header "RIFF"
		fread(type, sizeof(char), 4, fp);
		if (type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F')
			std::cout << "OpenAL Error: No RIFF" << std::endl;

		// Read header "WAVE"
		fread(&size, sizeof(DWORD), 1, fp);
		fread(type, sizeof(char), 4, fp);
		if (type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E')
			std::cout << "OpenAL Error: Not a WAVE file" << std::endl;

		// Read "fmt "
		fread(type, sizeof(char), 4, fp);
		if (type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ')
			std::cout << "OpenAL Error: Not a fmt" << std::endl;

		// Set variables based on info in the .wav file
		fread(&chunkSize, sizeof(DWORD), 1, fp);
		fread(&formatType, sizeof(short), 1, fp);
		fread(&channels, sizeof(short), 1, fp);
		fread(&sampleRate, sizeof(DWORD), 1, fp);
		fread(&avgBytesPerSec, sizeof(DWORD), 1, fp);
		fread(&bytesPerSample, sizeof(short), 1, fp);
		fread(&bitsPerSample, sizeof(short), 1, fp);

		// Find the data header
		bool looking = true;
		fread(type, sizeof(char), 1, fp);
		while (looking) {
			if (type[0] == 'd') {
				fread(type, sizeof(char), 1, fp);
				if (type[0] == 'a') {
					fread(type, sizeof(char), 1, fp);
					if (type[0] == 't') {
						fread(type, sizeof(char), 1, fp);
						if (type[0] == 'a') {
							looking = false;
						}
					}
				}
			}
			else {
				fread(type, sizeof(char), 1, fp);
			}
		}
		//fread(type, sizeof(char), 3, fp);
		//if (type[0] != 'a' || type[1] != 't' || type[2] != 'a')
		//	std::cout << "OpenAL Error: Missing DATA" << std::endl;

		// Read data
		fread(&dataSize, sizeof(DWORD), 1, fp);
		unsigned char* buf = new unsigned char[dataSize];
		fread(buf, sizeof(BYTE), dataSize, fp);

		// Determine format of the audio file
		ALuint frequency = sampleRate;
		ALenum format = 0;

		if (bitsPerSample == 8)
		{
			if (channels == 1)
				format = AL_FORMAT_MONO8;
			else if (channels == 2)
				format = AL_FORMAT_STEREO8;
		}
		else if (bitsPerSample == 16)
		{
			if (channels == 1)
				format = AL_FORMAT_MONO16;
			else if (channels == 2)
				format = AL_FORMAT_STEREO16;
		}
		std::cout << bitsPerSample << " " << channels << std::endl;
		
		// Create the buffer and return it
		GLuint buffer;
		alGenBuffers(1, &buffer);
		alBufferData(buffer, format, buf, dataSize, frequency);

		return buffer;
	}

	// Precondition:	cameraPos is the camera's position
	// Postcondition:	listener is set to camera's position
	void setListener(glm::vec3 cameraPos) {
		alListener3f(AL_POSITION, cameraPos.x, cameraPos.y, cameraPos.z);
		alListener3f(AL_VELOCITY, 0, 0, 0);
	}

	// Source class holds a source and can play audio from a buffer
	class Source {
	public:
		// Constructor for Source, initialises source
		Source() {
			alGenSources(1, &sourceid);
			alSourcef(sourceid, AL_GAIN, 1);
			alSourcef(sourceid, AL_PITCH, 1);
			alSource3f(sourceid, AL_POSITION, 0, 0, 0);
		}

		// Cleans up buffers
		void cleanup() {
			stop();
			alDeleteSources(1, &sourceid);
		}

		// Plays audio from buffer at source
		void play(GLuint buffer) {
			stop();
			alSourcei(sourceid, AL_BUFFER, buffer);
			continuePlaying();
		}

		void setVolume(float volume) {
			alSourcef(sourceid, AL_GAIN, volume);
		}

		void setPitch(float pitch) {
			alSourcef(sourceid, AL_PITCH, 1);		
		}

		void setPosition(glm::vec3 position) {
			alSource3f(sourceid, AL_POSITION, position.x, position.y, position.z);
		}

		void setVelocity(glm::vec3 velocity) {
			alSource3f(sourceid, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
		}

		void setLooping(bool loop) {
			alSourcei(sourceid, AL_LOOPING, loop ? AL_TRUE : AL_FALSE);
		}

		bool isPlaying() {
			int isPlaying;
			alGetSourcei(sourceid, AL_SOURCE_STATE, &isPlaying);
			return isPlaying == AL_PLAYING;
		}

		void setReferenceDistance(float distance) {
			alSourcef(sourceid, AL_REFERENCE_DISTANCE, distance);
		}
			
		void pause() {
			alSourcePause(sourceid);
		}

		void continuePlaying() {
			alSourcePlay(sourceid);
		}

		void stop() {
			alSourceStop(sourceid);
		}

	private:
		GLuint sourceid;	// id of the source
	};
}

#endif