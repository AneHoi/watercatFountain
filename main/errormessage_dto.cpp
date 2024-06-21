
#include "errormessage_dto.h"

// Constructor definition
ErrorData::ErrorData(int deviceId, const ErrorReadingsDto& error)
  : deviceId(deviceId), error(error) {}

// Method to convert device data to JSON string
std::string ErrorData::toJsonString() const {
  std::stringstream ss;
  ss << "{\n";
  ss << "  \"DeviceId\": " << deviceId << ",\n";
  ss << "  \"ErrorData\": {\n";
  ss << "    \"WaterisTooHot\": " << error.waterTooHot << ",\n";
  ss << "    \"TemperaturSensorError\": " << error.temperatureSensorError << ",\n";
  ss << "    \"NotEnoughWater\": " << error.unacceptableWaterheight << ",\n";
  ss << "    \"TimeStamp\": \"" << error.TimeStamp << "\"\n";
  ss << "  }\n";
  ss << "}\n";
  return ss.str();
}
