#include <chrono>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <vector>

#include "core/chip8.h"

constexpr double FRAME_RATE = 60.0;
constexpr unsigned int STEPS_PER_FRAME = 12;

int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cerr << "Usage: ./chip8 [rom file path]" << std::endl;
		return 1;
	}

	const char* sRomFilePath = argv[1];
	std::ifstream romFile(sRomFilePath, std::ios::binary);
	if (!romFile)
	{
		std::cerr << "Failed to open ROM file: " << sRomFilePath << std::endl;
		return 1;
	}

	romFile.seekg(0, std::ios::end);
	std::streamsize romSize = romFile.tellg();
	romFile.seekg(0, std::ios::beg);

	if (romSize <= 0)
	{
		std::cerr << "ROM file is empty." << std::endl;
		return 1;
	}
	// ROM starts in 0x200 and RAM has 4096 bytes, so the maximum size of a ROM is 4096 - 512 = 3584 bytes (0xE00)
	else if (romSize > 3584)
	{
		std::cerr << "ROM file is too large. Maximum size is 3584 bytes." << std::endl;
		return 1;
	}

	std::vector<uint8_t> vRomData(static_cast<size_t>(romSize));
	romFile.read(reinterpret_cast<char*>(vRomData.data()), romSize);
	if (!romFile)
	{
		std::cerr << "Failed to read ROM file: " << sRomFilePath << std::endl;
		return 1;
	}

	CChip8 chip8;
	if (!chip8.loadROMOnMemory(vRomData))
	{
		std::cerr << "Failed to load ROM on memory." << std::endl;
		return 1;
	}

	std::cout << "Loaded ROM: " << sRomFilePath << " " << romSize << std::endl;

	auto start = std::chrono::steady_clock::now();

	while (true)
	{
		auto end = std::chrono::steady_clock::now();
		if (std::chrono::duration<double>(end - start).count() >= 1.0 / FRAME_RATE)
		{
			start = end;
			// Iterate through the number of steps per frame and will draw the state of the display after all steps have been executed
			for (unsigned int i = 0; i < STEPS_PER_FRAME; ++i)
			{
				chip8.step();
				if (chip8.getIsHalted())
				{
					break;
				}
			}
			if (chip8.getDrawFlag())
			{
				int pixelCount = 0;
				const std::array<bool, 64 * 32>& display = chip8.getDisplay();
				for (const bool& pixel : display)
				{
					if (pixelCount % 64 == 0)
					{
						std::cout << std::endl;
					}
					std::cout << (pixel ? "#" : ".");
					pixelCount++;

				}
				chip8.setDrawFlag(false);
			}
			if (chip8.getIsHalted())
			{
				std::cerr << "Program halted." << std::endl;
				break;
			}
		}
	}


	//romFile.close();
	return 0;
}