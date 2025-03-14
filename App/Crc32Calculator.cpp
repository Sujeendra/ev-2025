#include "Crc32Calculator.h"

// CrcModel class used for CRC initialization and configuration
class CrcModel
{
public:
    uint32_t Initial;
    bool ResultReflected;
    bool InputReflected;
    uint32_t FinalXor;

    CrcModel(uint32_t initial, bool resultReflected, bool inputReflected, uint32_t finalXor)
        : Initial(initial), ResultReflected(resultReflected), InputReflected(inputReflected), FinalXor(finalXor) {}
};

// Constructor
Crc32Calculator::Crc32Calculator()
{
    // Optionally initialize tables or members if needed
}

// Calculate the CRC table for CRC32
void Crc32Calculator::CalculateCrcTable_CRC32()
{
    const uint32_t polynomial = 0x6938392D;
    crcTable_32.resize(256);
    for (int dividend = 0; dividend < 256; ++dividend)
    {
        uint32_t curByte = static_cast<uint32_t>(dividend << 24); // Move dividend byte into MSB of 32-bit CRC
        for (uint8_t bit = 0; bit < 8; ++bit)
        {
            if ((curByte & 0x80000000) != 0)
            {
                curByte = (curByte << 1) ^ polynomial;
            }
            else
            {
                curByte <<= 1;
            }
        }
        crcTable_32[dividend] = curByte;
    }
}

// Compute the CRC32 for an array of bytes
uint32_t Crc32Calculator::Compute_CRC32(const std::array<uint8_t, 8>& bytes)
{
    CrcModel crcModel(0xFFFFFFFF, false, false, 0x00000000); // Set up the CRC model

    uint32_t crc = crcModel.Initial;
    for (auto& b : bytes)
    {
        uint8_t pos = static_cast<uint8_t>((crc ^ (b << 24)) >> 24);
        crc = (crc << 8) ^ crcTable_32[pos];
    }
    crc = (crcModel.ResultReflected ? Reflect32(crc) : crc);
    return crc ^ crcModel.FinalXor;
}

// Calculate the CRC table for CRC8
void Crc32Calculator::CalculateCrcTable_CRC8()
{
    const uint8_t generator = 0x2F;
    crcTable_8.resize(256);
    for (int dividend = 0; dividend < 256; dividend++)
    {
        uint8_t currByte = static_cast<uint8_t>(dividend);

        for (uint8_t bit = 0; bit < 8; bit++)
        {
            if ((currByte & 0x80) != 0)
            {
                currByte <<= 1;
                currByte ^= generator;
            }
            else
            {
                currByte <<= 1;
            }
        }

        crcTable_8[dividend] = currByte;
    }
}

// Compute the CRC8 for a vector of bytes
uint8_t Crc32Calculator::Compute_CRC8(const std::vector<uint8_t>& bytes)
{
    CrcModel crcModel(0xFF, false, false, 0xFF); // Set up the CRC model
    uint8_t crc = crcModel.Initial;

    for (uint8_t b : bytes)
    {
        uint8_t data = b ^ crc;
        crc = crcTable_8[data];
    }
    crc = (crcModel.ResultReflected ? Reflect8(crc) : crc);
    return crc ^ crcModel.FinalXor;
}

// Reflects (reverses) the bits of an 8-bit value
uint8_t Crc32Calculator::Reflect8(uint8_t value)
{
    uint8_t reflected = 0;
    for (int i = 0; i < 8; i++)
    {
        if (value & (1 << i))
        {
            reflected |= (1 << (7 - i));
        }
    }
    return reflected;
}

// Reflects (reverses) the bits of a 32-bit value
uint32_t Crc32Calculator::Reflect32(uint32_t value)
{
    uint32_t reflected = 0;
    for (int i = 0; i < 32; i++)
    {
        if (value & (1 << i))
        {
            reflected |= (1 << (31 - i));
        }
    }
    return reflected;
}
