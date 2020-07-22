#include "flowMeasure.h"
#include <Wire.h>


float addFlowMeasure(void) {
    byte msbByte;
    byte lsbByte;
    unsigned int flowUInt;
    float instantFlow;
    
    Wire.requestFrom(FLOW_SENSOR_ADDRESS, FLOW_SENSOR_DATA_LENGTH);
    msbByte = Wire.read(); // Read the first byte this is the MSB
    lsbByte = Wire.read(); // Now Read the second byte this is the LSB 43
    flowUInt = (((msbByte << 8) + lsbByte));                    //formulas OK???

    instantFlow = (200 * ((flowUInt / 16384.0) - .1) / .8);                    //me salen litros/min

    return instantFlow;
}

void inspirationVolume(t_hopeSystem *hopeSystem) {
    static float previousInspitarionVolume = 0;
    static unsigned long previousGetInspirationFlowData = millis();
    float inspirationVolume;
    unsigned long currentMillis, dataFlowAcquisitionTimeInterval;
    
    currentMillis = millis();
    dataFlowAcquisitionTimeInterval = currentMillis - previousGetInspirationFlowData;

    if ((dataFlowAcquisitionTimeInterval) > ADD_FLOW_MEASURE_INTERVAL) {

        previousGetInspirationFlowData = currentMillis;
        inspirationVolume = (addFlowMeasure()/60000)* dataFlowAcquisitionTimeInterval;
    
        hopeSystem->sensorsMeasure.inspirationVolume = hopeSystem->sensorsMeasure.inspirationVolume+inspirationVolume;
    }
    inspirationVolumeLimit(hopeSystem);

    if (hopeSystem->sensorsMeasure.inspiratoryVolumeLimit == HIGH) {
        hopeSystem->sensorsMeasure.inspirationVolume = 0;
    }
}

void inspirationVolumeLimit(t_hopeSystem* hopeSystem) {  
    if (hopeSystem->sensorsMeasure.inspirationVolume > hopeSystem->displayParameters.inspiratoryFlow) {
        hopeSystem->sensorsMeasure.inspiratoryVolumeLimit = HIGH;
    }
    else {
        hopeSystem->sensorsMeasure.inspiratoryVolumeLimit = LOW;
    }
}
