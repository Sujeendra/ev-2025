#ifndef CRC32CALCULATOR_H
#define CRC32CALCULATOR_H

#include <array>
#include <vector>
#include <cstdint>

class Crc32Calculator
{
private:
    std::vector<uint32_t> crcTable_32;
    std::vector<uint32_t> crcTable_8;

    // Reflects (reverses) the bits of an 8-bit value
    uint8_t Reflect8(uint8_t value);

    // Reflects (reverses) the bits of a 32-bit value
    uint32_t Reflect32(uint32_t value);

public:
    // Constructor
    Crc32Calculator();

    // Calculate the CRC table for a given polynomial (CRC32)
    void CalculateCrcTable_CRC32();

    // Compute the CRC32 for an array of bytes
    uint32_t Compute_CRC32(const std::array<uint8_t, 8>& bytes);

    // Calculate the CRC table for CRC8
    void CalculateCrcTable_CRC8();

    // Compute the CRC8 for a vector of bytes
    uint8_t Compute_CRC8(const std::vector<uint8_t>& bytes);
};

#endif // CRC32CALCULATOR_H
