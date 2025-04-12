#ifndef FA_H
#define FA_H

#include <vector>
#include <string>
#include <cstdint>
#include <array>


// DataEntry struct to hold individual signal data
struct DataEntry {
    long double value;        // Store value as long double to handle very large floating-point numbers
    long double cycleTime;    // Time between cycles for this data
    long double deltaTime;    // Time difference since last update (could be useful for rate calculations)
    std::string signal;       // Signal name (e.g., signal from CAN message)
    std::string message;      // Message name (e.g., the actual name of the message)

    // Constructor to initialize the fields
    DataEntry(long double val, long double ct, long double dt, const std::string& sig, const std::string& msg)
        : value(val), cycleTime(ct), deltaTime(dt), signal(sig), message(msg) {}
};

// Message struct to hold a set of SHM and SDM entries
struct Message {
    std::vector<DataEntry> SHM;               // List of SHM (Shared Memory) entries
    std::vector<DataEntry> SDM;               // List of SDM (Signed Data Memory) entries
    std::array<uint8_t, 8> shmSignedData{};   // 8-byte unsigned array for SHM data
    std::array<uint8_t, 8> sdmSignedData{};   // 8-byte unsigned array for SDM data
    int32_t messageId29Bit = 0;               // 29-bit message ID stored in a 32-bit integer
    std::vector<long double> actual_data;     // Vector for storing actual data values
    uint8_t shmCounter = 0;                   // Counter for SHM entries
    std::string sheetName;                    // Name of the sheet (could be related to DBC file)
    std::string nodeName;

    // Increment the counter and reset if it reaches 32
    void incrementCounter() {
        shmCounter = (shmCounter + 1) % 32; // Increment and wrap around at 32
    }
};

#endif // FA_H
