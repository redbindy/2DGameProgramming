#include "Audio.h"

Audio::Audio(const int resourceId, IXAudio2* const pXAudio)
	: mpSourceVoice(nullptr)
	, mpRawAudio(nullptr)
	, mAudioLength(0)
	, mXAudioBuffer{ 0, }
{
	std::cout << "Initialize Audio" << std::endl;

	const HRSRC hWavInfo = FindResource(nullptr, MAKEINTRESOURCE(resourceId), TEXT("WAVE"));
	if (hWavInfo == nullptr)
	{
		std::cerr << __LINE__ << ' ' << "FindResource" << std::endl;

		DebugBreak();
	}
	assert(hWavInfo != nullptr);

	const HGLOBAL hWav = LoadResource(nullptr, hWavInfo);
	{
		if (hWav == nullptr)
		{
			std::cerr << __LINE__ << ' ' << "LoadResource" << std::endl;

			DebugBreak();
		}
		assert(hWav != nullptr);

		const void* pRawBytes = LockResource(hWav);
		if (pRawBytes == nullptr)
		{
			std::cerr << __LINE__ << ' ' << "LockResource" << std::endl;

			DebugBreak();
		}
		assert(pRawBytes);

		const int bytesLength = SizeofResource(nullptr, hWavInfo);

		const uint8_t* pRawWav = reinterpret_cast<const uint8_t*>(pRawBytes);
		const void* pEnd = pRawWav + bytesLength;

		WavHeader header;
		while (pRawWav < pEnd)
		{
			DWORD id;
			pRawWav = getDataFromRawMemory(pRawWav, sizeof(DWORD), &id);

			switch (id)
			{
			case RIFF:
				{
					header.riffChunk.id = id;
					pRawWav = getDataFromRawMemory(
						pRawWav,
						sizeof(header.riffChunk) - sizeof(header.riffChunk.id),
						&header.riffChunk.chunkSize
					);

					if (header.riffChunk.format != 'EVAW')
					{
						std::cerr << "Invalid file format" << std::endl;

						DebugBreak();
					}
				}
				break;

			case FMT:
				{
					header.formatChunk.id = id;
					pRawWav = getDataFromRawMemory(
						pRawWav,
						sizeof(header.formatChunk.size),
						&header.formatChunk.size
					);

					pRawWav = getDataFromRawMemory(
						pRawWav,
						header.formatChunk.size,
						&header.formatChunk.format
					);
				}
				break;

			case LIST:
				{
					DWORD listSize;

					pRawWav = getDataFromRawMemory(
						pRawWav,
						sizeof(listSize),
						&listSize
					);

					uint8_t* pTrash = new uint8_t[listSize];
					{
						pRawWav = getDataFromRawMemory(
							pRawWav,
							listSize,
							pTrash
						);
					}
					delete[] pTrash;
				}
				break;

			case DATA:
				{
					header.dataChunk.id = id;
					pRawWav = getDataFromRawMemory(
						pRawWav,
						sizeof(header.dataChunk.size),
						&header.dataChunk.size
					);

					mpRawAudio = new BYTE[header.dataChunk.size];
					mAudioLength = header.dataChunk.size;

					pRawWav = getDataFromRawMemory(pRawWav, mAudioLength, mpRawAudio);
				}
				break;

			default:
				assert(false);
				break;
			}
		}

		HRESULT hr = pXAudio->CreateSourceVoice(&mpSourceVoice, &header.formatChunk.format);
	}
	FreeResource(hWav);

	mXAudioBuffer.AudioBytes = mAudioLength;
	mXAudioBuffer.pAudioData = mpRawAudio;
	mXAudioBuffer.Flags = XAUDIO2_END_OF_STREAM;
}

Audio::~Audio()
{
	delete[] mpRawAudio;
	mpSourceVoice->DestroyVoice();
}

bool Audio::TryPlay() const
{
	XAUDIO2_VOICE_STATE state;
	mpSourceVoice->GetState(&state);

	if (state.BuffersQueued > 0)
	{
		return false;
	}

	mpSourceVoice->SubmitSourceBuffer(&mXAudioBuffer);

	mpSourceVoice->Start();

	return true;
}

void Audio::Stop() const
{
	mpSourceVoice->Stop();
}

const uint8_t* Audio::getDataFromRawMemory(const uint8_t* pRaw, const int byteSize, void* pOut) const
{
	memcpy(pOut, pRaw, byteSize);

	return pRaw + byteSize;
}
