#include "chip8.h"

#include <iostream>

CChip8::CChip8()
{
	reset();
}

void CChip8::reset()
{
	m_aMemory.fill(0);
	m_aRegisters.fill(0);
	m_aDisplay.fill(false);
	m_u16ProgramCounter = 0x200;
	m_u16IndexRegister = 0x0000;
	m_bDrawFlag = false;
	m_bIsHalted = false;
}

bool CChip8::loadROMOnMemory(const std::vector<uint8_t>& _romData)
{
	// 3584 = 4096 - 512 (0x200)
	if (_romData.size() == 0 || _romData.size() > 3584)
	{
		std::cerr << "Invalid ROM size. ROM must be between 1 and 3584 bytes." << std::endl;
		return false;
	}

	// Clear memory before loading ROM
	m_aMemory.fill(0);
	// Load ROM data into memory starting at address 0x200
	std::copy(_romData.begin(), _romData.end(), m_aMemory.begin() + 0x200);

	return true;
}

void CChip8::step()
{
	if (m_bIsHalted)
	{
		return;
	}
	if (m_u16ProgramCounter >= 4095)
	{
		// m_programCounter + 1 will be out of bounds
		std::cerr << "Program counter out of bounds: " << std::hex << m_u16ProgramCounter << std::endl;
		m_bIsHalted = true;
		return;
	}
	uint16_t opcode = static_cast<uint16_t>((m_aMemory[m_u16ProgramCounter] << 8) | m_aMemory[static_cast<uint16_t>(m_u16ProgramCounter + 1)]); //  Big-endian
	m_u16ProgramCounter += 2;
	decodeAndExecuteOpcode(opcode);
}

void CChip8::decodeAndExecuteOpcode(uint16_t opcode)
{
	// TODO: Fill with the rest of opcodes
	if (opcode == 0x00E0)
	{
		// Clear the display
		m_aDisplay.fill(false);
		m_bDrawFlag = true;
	}
	else if ((opcode & 0xF000) == 0x1000)
	{
		// 1NNN: PC = NNN
		m_u16ProgramCounter = static_cast<uint16_t>(opcode & 0x0FFF);
	}
	else if ((opcode & 0xF000) == 0x6000)
	{
		// 6XNN: VX = NN
		uint8_t x = static_cast<uint8_t>((opcode & 0x0F00) >> 8);
		uint8_t nn = static_cast<uint8_t>(opcode & 0x00FF);
		m_aRegisters[x] = nn;
	}
	else if ((opcode & 0xF000) == 0x7000)
	{
		// 7XNN: VX += NN
		uint8_t x = static_cast<uint8_t>((opcode & 0x0F00) >> 8);
		uint8_t nn = static_cast<uint8_t>(opcode & 0x00FF);
		m_aRegisters[x] += nn;
	}
	else if ((opcode & 0xF000) == 0xA000)
	{
		// ANNN: I = NNN
		m_u16IndexRegister = static_cast<uint16_t>(opcode & 0x0FFF);
	}
	else if ((opcode & 0xF000) == 0xD000)
	{
		// DXYN: Draw sprite at (VX, VY) with height N
		uint8_t X = static_cast<uint8_t>((opcode & 0x0F00) >> 8);
		uint8_t x = static_cast<uint8_t>(m_aRegisters[X] & 0x3F);
		uint8_t Y = static_cast<uint8_t>((opcode & 0x00F0) >> 4);
		uint8_t y = static_cast<uint8_t>(m_aRegisters[Y] & 0x1F);

		uint8_t vf = 0;
		uint8_t rows = static_cast<uint8_t>(opcode & 0x000F);
		for (uint8_t row = 0; row < rows; ++row)
		{
			uint16_t u16MemoryIndex = static_cast<uint16_t>(m_u16IndexRegister + row);
			if (u16MemoryIndex >= 4096)
			{
				std::cerr << "Index register out of bounds: " << std::hex << u16MemoryIndex << std::endl;
				break;
			}
			uint8_t spriteRow = m_aMemory[u16MemoryIndex];
			for (uint8_t col = 0; col < 8; ++col)
			{
				if (spriteRow & (0x80 >> col))
				{
					if (x + col >= 64 || y + row >= 32)
					{
						continue; // Skip if out of bounds
					}
					uint16_t index = static_cast<uint16_t>((y + row) * 64 + (x + col));

					if (m_aDisplay[index])
					{
						vf = 1; // Collision detected
					}
					m_aDisplay[index] ^= true; // XOR the pixel
				}
			}
		}
		m_aRegisters[0xF] = vf;
		m_bDrawFlag = true;
	}	
	else
	{
		std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
		m_bIsHalted = true;
	}
}