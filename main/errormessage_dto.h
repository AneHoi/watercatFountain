#ifndef ERRORMESSAGEDTO_H
#define ERRORMESSAGEDTO_H

#include <string>
#include <vector>
#include <sstream>

// Device readings data structure
struct ErrorReadingsDto {
    bool waterTooHot;
    bool temperatureSensorError;
    bool unacceptableWaterheight;
    std::string TimeStamp;
    // Constructor with default values
    
    ErrorReadingsDto()
        : waterTooHot(false), 
          temperatureSensorError(false), 
          unacceptableWaterheight(false), 
          TimeStamp("1970-01-01T00:00:00Z") {}
};

// Device data class
class ErrorData {
public:
    ErrorData(int deviceId, const ErrorReadingsDto& error);
    std::string toJsonString() const;

private:
    int deviceId;
    ErrorReadingsDto error;
};

#endif
