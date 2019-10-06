//http://www.youtube.com/user/thecplusplusguy
//Playing 3D sound with OpenAL, and loading a wav file manually

GLuint loadAudio(const char* file) {
	FILE* fp = NULL;

	fp = fopen(file, "rb");

	char type[4];
	DWORD size, chunkSize;
	short formatType, channels;
	DWORD sampleRate, avgBytesPerSec;
	short bytesPerSample, bitsPerSample;
	DWORD dataSize;

	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'R' || type[1] != 'I' || type[2] != 'F' || type[3] != 'F')
		std::cout << "OpenAL Error: No RIFF" << std::endl;

	fread(&size, sizeof(DWORD), 1, fp);
	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'W' || type[1] != 'A' || type[2] != 'V' || type[3] != 'E')
		std::cout << "OpenAL Error: Not a WAVE file" << std::endl;

	fread(type, sizeof(char), 4, fp);
	if (type[0] != 'f' || type[1] != 'm' || type[2] != 't' || type[3] != ' ')
		std::cout << "OpenAL Error: Not a fmt" << std::endl;

	fread(&chunkSize, sizeof(DWORD), 1, fp);
	fread(&formatType, sizeof(short), 1, fp);
	fread(&channels, sizeof(short), 1, fp);
	fread(&sampleRate, sizeof(DWORD), 1, fp);
	fread(&avgBytesPerSec, sizeof(DWORD), 1, fp);
	fread(&bytesPerSample, sizeof(short), 1, fp);
	fread(&bitsPerSample, sizeof(short), 1, fp);

	fread(type, sizeof(char), 1, fp);
	while (type[0] != 'd') {
		fread(type, sizeof(char), 1, fp);
	}
	fread(type, sizeof(char), 3, fp);
	if (type[0] != 'a' || type[1] != 't' || type[2] != 'a')
		std::cout << "OpenAL Error: Missing DATA" << std::endl;

	fread(&dataSize, sizeof(DWORD), 1, fp);

	unsigned char* buf = new unsigned char[dataSize];
	fread(buf, sizeof(BYTE), dataSize, fp);

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
	
	GLuint buffer;
	alGenBuffers(1, &buffer);
	std::cout << alGetError() << std::endl;

	alBufferData(buffer, format, buf, dataSize, frequency);
	std::cout << alGetError() << std::endl;

	return buffer;
}

void setListener(glm::vec3 cameraPos) {
	//alListener3f(AL_POSITION, cameraPos.x, cameraPos.y, cameraPos.z);
	alListener3f(AL_POSITION, 0, 0, 0);
	alListener3f(AL_VELOCITY, 0, 0, 0);
}

namespace util {

	class Source {
	public:
		Source() {
			alGenSources(1, &sourceid);
			alSourcef(sourceid, AL_GAIN, 1);
			alSourcef(sourceid, AL_PITCH, 1);
			alSource3f(sourceid, AL_POSITION, 0, 0, 0);
			std::cout << "Source id: " << sourceid << std::endl;
		}

		void cleanup() {
			alDeleteSources(1, &sourceid);
		}

		void play(GLuint buffer) {
			alSourcei(sourceid, AL_BUFFER, buffer);
			alSourcePlay(sourceid);
			std::cout << "Source id: " << sourceid << std::endl;
			std::cout << "buffer: " << buffer << std::endl;
		}


	private:
		GLuint sourceid;
	};
}



