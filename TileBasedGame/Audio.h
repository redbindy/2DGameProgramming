#pragma once

#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")

#include <iostream>
#include <cassert>

#include "Constants.h"

enum EHeaderConstant
{
	RIFF = 'FFIR',
	FMT = ' tmf',
	LIST = 'TSIL',
	DATA = 'atad'
};

struct RIFFChunk
{
	DWORD id;
	DWORD chunkSize;
	DWORD format;
};

struct FormatChunk
{
	DWORD id;
	DWORD size;
	WAVEFORMATEX format;
};

struct DataChunk
{
	DWORD id;
	DWORD size;
};

struct WavHeader
{
	RIFFChunk riffChunk;
	FormatChunk formatChunk;
	DataChunk dataChunk;
};

struct ListHeader
{
	DWORD id;
	DWORD size;
	DWORD listTypeId;
};

class Audio final
{
public:
	Audio(const int resourceId, IXAudio2* const pXAudio);
	virtual ~Audio();

	void UpdateState();

	bool TryPlay() const;
	void Stop() const;

private:
	IXAudio2SourceVoice* mpSourceVoice;

	BYTE* mpRawAudio;
	uint32_t mAudioLength;

	XAUDIO2_BUFFER mXAudioBuffer;

private:
	const uint8_t* getDataFromRawMemory(const uint8_t* pRaw, const int byteSize, void* pOut) const;
};