#ifndef UTILITY_H
#define UTILITY_H

#include <QString>
#include <QDebug>
#include <QSysInfo>
#include <QtCore>
#include <gpiod.h>
#include <array>
#include <QCanDbcFileParser>

struct gpiod_line_request;
struct gpiod_chip;
struct gpiod_line_request;

uint32_t combineBytesTo32Bit(uint8_t byte1, uint8_t byte2, uint8_t byte3, uint8_t byte4);
uint64_t CombineTo64Bit(uint32_t high, uint32_t low);
uint32_t ReverseByteOrder(uint32_t value);

void encodeSignal(double value, double offset, double scale, int startBit, int length, std::array<uint8_t, 8>& data, QSysInfo::Endian byteOrder, QtCanBus::DataFormat dataformat);
double decodeSignal(const uint8_t data[8], double offset, double scale, int startBit, int length, QSysInfo::Endian byteOrder, QtCanBus::DataFormat dataformat);

bool setGpioPin(int pinNumber, bool active, const QString &consumer = "gpio-controller");
struct gpiod_line_request *request_output_line(const char *chip_path,
                                               unsigned int offset,
                                               enum gpiod_line_value value,
                                               const char *consumer);

#endif // UTILITY_H
