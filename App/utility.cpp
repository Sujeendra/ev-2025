#include "utility.h"
#include <QSysInfo>
#include <QCanBusFrame>
#include <QCanBus>
#include <QtCore>
#include <gpiod.h>
#include <cstring>
#include <errno.h>

uint32_t combineBytesTo32Bit(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4) {
    // Shift each byte to its correct position and combine with bitwise OR
    uint32_t result = (static_cast<uint32_t>(byte1) << 24) |
                      (static_cast<uint32_t>(byte2) << 16) |
                      (static_cast<uint32_t>(byte3) << 8)  |
                      (static_cast<uint32_t>(byte4));
    return result;
}

uint64_t CombineTo64Bit(uint32_t high, uint32_t low) {
    // Shift the high 32-bit integer to the upper half (left by 32 bits)
    // and OR with the low 32-bit integer.
    return (static_cast<uint64_t>(high) << 32) | low;
}

uint32_t ReverseByteOrder(uint32_t value) {
    // Extract each byte and rearrange
    return ((value & 0xFF000000) >> 24) | // Move byte 0 to byte 3
           ((value & 0x00FF0000) >> 8)  | // Move byte 1 to byte 2
           ((value & 0x0000FF00) << 8)  | // Move byte 2 to byte 1
           ((value & 0x000000FF) << 24);   // Move byte 3 to byte 0
}

void encodeSignal(double value, double offset, double scale, int startBit, int length, std::array<uint8_t, 8>& data, QSysInfo::Endian byteOrder
                  ,QtCanBus::DataFormat dataformat) {
    // Calculate adjusted binary value
    double adjustedValue = (value - offset) / scale;
    int binaryValue = static_cast<int>(adjustedValue);
    // If the signal is signed, convert negative values to two’s complement

    if (dataformat == QtCanBus::DataFormat::SignedInteger && binaryValue < 0) {
        binaryValue = (1 << length) + binaryValue; // Convert to two's complement
    }
    
    int i, bitPosition, rev_bitPosition, rev_startBit = (startBit/8)*8 +7- (startBit % 8);
   
    for (int ii = 0; ii < length; ++ii) {
        // Handle bit position based on endianness
        if (byteOrder == QSysInfo::Endian::LittleEndian) {
            bitPosition = startBit + ii;
	        i=ii;
        } else { // BigEndian
           rev_bitPosition = rev_startBit + ii;
           bitPosition =(rev_bitPosition/8)*8 +7- (rev_bitPosition%8);
	        i=length-1-ii;
        }

        int byteIndex = bitPosition / 8;
        int bitIndex = bitPosition % 8;

        if (binaryValue & (1 << i)) {
            data[byteIndex] |= (1 << bitIndex);  // Set the bit
        } else {
            data[byteIndex] &= ~(1 << bitIndex); // Clear the bit
        }
    }
}

double decodeSignal(const uint8_t data[8], double offset, double scale, int startBit, int length, QSysInfo::Endian byteOrder, QtCanBus::DataFormat dataformat) {
   
    int binaryValue = 0;
   int i, bitPosition, rev_bitPosition, rev_startBit = (startBit/8)*8 +7- (startBit % 8);

    for (int ii = 0; ii < length; ++ii) {
        // Handle bit position based on endianness
        if (byteOrder == QSysInfo::Endian::LittleEndian) {
            bitPosition = startBit + ii;
            i=ii;
        } else { // BigEndian
           rev_bitPosition = rev_startBit + ii;
           bitPosition =(rev_bitPosition/8)*8 +7- (rev_bitPosition%8);
           i=length-1-ii;
        }

        int byteIndex = bitPosition / 8;
        int bitIndex = bitPosition % 8;

        if (data[byteIndex] & (1 << bitIndex)) {
            binaryValue |= (1 << i);
        }
        // If the signal is signed, interpret as two's complement : to check the working
        if (dataformat == QtCanBus::DataFormat::SignedInteger && (binaryValue & (1 << (length - 1)))) {
            // Extend the sign bit
            binaryValue -= (1 << length);
        }
    }
    return (binaryValue * scale) + offset;
}

bool setGpioPin(int pinNumber, bool active, const QString &consumer) {
    static const char *const chipPath = "/dev/gpiochip0";

    struct gpiod_line_request *request = nullptr;
    enum gpiod_line_value value = active ? GPIOD_LINE_VALUE_ACTIVE : GPIOD_LINE_VALUE_INACTIVE;

    // Request output line
    request = request_output_line(chipPath, pinNumber, value, consumer.toStdString().c_str());
    if (!request) {
        qCritical() << "Failed to request line for pin" << pinNumber << ":" << strerror(errno);
        return false;
    }

    // Set the value
    qDebug() << "Value is: " << value;
    if (gpiod_line_request_set_value(request, pinNumber, value) < 0) {
        qCritical() << "Failed to set value for pin" << pinNumber << ":" << strerror(errno);
        gpiod_line_request_release(request);
        return false;
    }

    // Release the line after setting
    gpiod_line_request_release(request);
    return true;
}

struct gpiod_line_request *request_output_line(const char *chip_path,
                                               unsigned int offset,
                                               enum gpiod_line_value value,
                                               const char *consumer) {
    struct gpiod_request_config *req_cfg = nullptr;
    struct gpiod_line_request *request = nullptr;
    struct gpiod_line_settings *settings = nullptr;
    struct gpiod_line_config *line_cfg = nullptr;
    struct gpiod_chip *chip = nullptr;

    // Open GPIO chip
    chip = gpiod_chip_open(chip_path);
    if (!chip) {
        qCritical() << "Failed to open GPIO chip" << chip_path << ":" << strerror(errno);
        return nullptr;
    }

    // Create line settings and configure direction and initial value
    settings = gpiod_line_settings_new();
    if (!settings) {
        qCritical() << "Failed to create line settings";
        goto close_chip;
    }
    gpiod_line_settings_set_direction(settings, GPIOD_LINE_DIRECTION_OUTPUT);
    gpiod_line_settings_set_output_value(settings, value);

    // Create line configuration
    line_cfg = gpiod_line_config_new();
    if (!line_cfg) {
        qCritical() << "Failed to create line config";
        goto free_settings;
    }

    // Add line settings to configuration
    if (gpiod_line_config_add_line_settings(line_cfg, &offset, 1, settings) < 0) {
        qCritical() << "Failed to add line settings:" << strerror(errno);
        goto free_line_config;
    }

    // Create request configuration if a consumer name is provided
    if (consumer) {
        req_cfg = gpiod_request_config_new();
        if (!req_cfg) {
            qCritical() << "Failed to create request config";
            goto free_line_config;
        }
        gpiod_request_config_set_consumer(req_cfg, consumer);
    }

    // Request the line
    request = gpiod_chip_request_lines(chip, req_cfg, line_cfg);
    if (!request) {
        qCritical() << "Failed to request lines:" << strerror(errno);
    }

    // Clean up
    gpiod_request_config_free(req_cfg);
free_line_config:
    gpiod_line_config_free(line_cfg);
free_settings:
    gpiod_line_settings_free(settings);
close_chip:
    gpiod_chip_close(chip);

    return request;
}
