#ifndef RESPONSEDTO_H
#define RESPONSEDTO_H

#include <string>
#include <vector>
#include <sstream>

// Sensor data structure
struct SensorDto {
    double MotorValue;
    double TemperatureValue;
    std::string TimeStamp;
};

// Device readings data structure
struct DeviceReadingsDto {
    std::vector<SensorDto> DeviceData;
};

// Device data class
class DeviceData {
public:
    DeviceData(int deviceId, const DeviceReadingsDto& data);

    std::string toJsonString() const;

private:
    int deviceId;
    DeviceReadingsDto data;

    void serializeSensorArray(std::stringstream& ss, const std::vector<SensorDto>& sensors) const;
};

#endif
