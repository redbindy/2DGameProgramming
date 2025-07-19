#include <xaudio2.h>
#include <cassert>
#include <fstream>

#pragma comment(lib, "xaudio2.lib")

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
	int32_t size;
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

int main()
{
	HRESULT hr = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	assert(SUCCEEDED(hr));
	{
		IXAudio2* pXAudio = nullptr;
		hr = XAudio2Create(
			&pXAudio,
			0, // 반드시 0
			XAUDIO2_USE_DEFAULT_PROCESSOR // 추천값. 라이브러리에서 적절한 프로세서 선택
		);
		assert(SUCCEEDED(hr));

		IXAudio2MasteringVoice* pMasteringVoice = nullptr;
		hr = pXAudio->CreateMasteringVoice(&pMasteringVoice);

		IXAudio2SourceVoice* pSourceVoice = nullptr;

		// 실시간 오디오 처리를 목적으로 하는 라이브러리
		// 그냥 날로 읽어서 처리가 가능한 원시 데이터류만 지원
		std::ifstream fin;
		fin.open("sound.wav", std::ios::binary);
		{
			WavHeader header;
			ZeroMemory(&header, sizeof(header));

			fin.read(reinterpret_cast<char*>(&header.riffChunk), sizeof(header.riffChunk));
			assert(!fin.fail());

			while (!fin.eof())
			{
				constexpr DWORD FMT = ' tmf';
				constexpr DWORD DATA = 'atad';
				constexpr DWORD LIST = 'TSIL';

				DWORD id;
				fin.read(reinterpret_cast<char*>(&id), sizeof(id));
				assert(!fin.fail());

				switch (id)
				{
				case FMT:
					header.formatChunk.id = id;

					fin.read(reinterpret_cast<char*>(&header.formatChunk.size), sizeof(header.formatChunk.size));
					assert(!fin.fail());

					fin.read(reinterpret_cast<char*>(&header.formatChunk.format), header.formatChunk.size);
					assert(!fin.fail());
					break;

				case LIST:
					{
						ListHeader listHeader;

						fin.read(reinterpret_cast<char*>(&listHeader.size), sizeof(listHeader.size));
						assert(!fin.fail());

						char* pTrash = new char[listHeader.size];
						fin.read(pTrash, listHeader.size);

						assert(!fin.fail());

						delete[] pTrash;
					}
					break;

				case DATA:
					header.dataChunk.id = id;
					fin.read(reinterpret_cast<char*>(&header.dataChunk) + sizeof(DWORD), sizeof(header.dataChunk) - sizeof(header.dataChunk.id));
					assert(!fin.fail());
					break;

				default:
					continue;
				}

				if (id == DATA)
				{
					break;
				}
			}

			BYTE* pData = new BYTE[header.dataChunk.size];
			{
				fin.read(reinterpret_cast<char*>(pData), header.dataChunk.size);

				hr = pXAudio->CreateSourceVoice(&pSourceVoice, &header.formatChunk.format);
				assert(SUCCEEDED(hr));

				XAUDIO2_BUFFER audioBuffer;
				ZeroMemory(&audioBuffer, sizeof(audioBuffer));

				audioBuffer.AudioBytes = header.dataChunk.size;
				audioBuffer.pAudioData = pData;
				audioBuffer.Flags = XAUDIO2_END_OF_STREAM;

				pSourceVoice->SubmitSourceBuffer(&audioBuffer);

				pSourceVoice->Start(0);
				Sleep(3000);
				pSourceVoice->Stop();
			}
			delete[] pData;
		}
		fin.close();

		pSourceVoice->DestroyVoice();
		pMasteringVoice->DestroyVoice();
		pXAudio->Release();
	}
	CoUninitialize();

	return 0;
}