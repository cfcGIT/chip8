#pragma once

#include <array>
#include <vector>
#include <cstdint>

class CChip8
{
public:
	CChip8();

	/**
	 * @brief Resets the Chip8 emulator to its initial state.
	 */
	void reset();

	/**
	 * @brief Loads a ROM into the Chip8 memory.
	 * @param _romData A vector containing the ROM data to be loaded.
	 * @return True if the ROM was successfully loaded, false otherwise.
	 */
	bool loadROMOnMemory(const std::vector<uint8_t>& _romData);

	/**
	 * @brief Executes a single step of the Chip8 emulator, processing one opcode.
	 */
	void step();

	/**
	 * @brief Retrieves the current state of the display.
	 * @return A constant reference to the display array.
	 */
	const std::array<bool, 64 * 32>& getDisplay() const { return m_aDisplay; }

	/**
	 * @brief Checks if the display needs to be redrawn.
	 * @return True if the display needs to be redrawn, false otherwise.
	 */
	bool getDrawFlag() const { return m_bDrawFlag; }

	/**
	 * @brief Sets the draw flag to indicate whether the display needs to be redrawn.
	 * @param _flag The new value for the draw flag.
	 */
	void setDrawFlag(bool _flag) { m_bDrawFlag = _flag; }

	/**
	 * @brief Checks if the Chip8 emulator is halted.
	 * @return True if the emulator is halted, false otherwise.
	 */
	bool getIsHalted() const { return m_bIsHalted; }

private:
	/**
	 * @brief Decodes and executes a given opcode.
	 * @param opcode The opcode to be decoded and executed.
	 */
	void decodeAndExecuteOpcode(uint16_t opcode);

	std::array<uint8_t, 4096> m_aMemory = {}; // RAM memory (4K)
	std::array<uint8_t, 16> m_aRegisters = {}; // V registers (V0 to VF)
	std::array <bool, 64 * 32> m_aDisplay = {}; // True means that the pixel is on, false means that the pixel is off.

	uint16_t m_u16ProgramCounter = 0x200; // PC
	uint16_t m_u16IndexRegister = 0x0000; // I

	bool m_bDrawFlag = false;
	bool m_bIsHalted = false;
};
