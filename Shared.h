#pragma once
#include "IncludeFiles.h"
#ifndef __Shared__
#define __Shared__

constexpr int PIPE_SIZE = 1024;
constexpr int MAX_FUELTANK_CAPACITY = 500;

const std::string PUMP1_P_STR = "Pump1ProducerSemaphore";
const std::string PUMP1_C_STR = "Pump1ConsumerSemaphore";
const std::string PUMP2_P_STR = "Pump22ProducerSemaphore";
const std::string PUMP2_C_STR = "Pump22ConsumerSemaphore";
const std::string PUMP3_P_STR = "Pump3ProducerSemaphore";
const std::string PUMP3_C_STR = "Pump3ConsumerSemaphore";
const std::string PUMP4_P_STR = "Pump4ProducerSemaphore";
const std::string PUMP4_C_STR = "Pump4ConsumerSemaphore";

const std::string FUEL_TANK_SEMAPHORE_STR = "FuelTankSemaphore";

enum class GasGrade {
    G87,
    G89,
    G91,
    G92
};

typedef std::map<GasGrade, float> PriceMap;

struct CustomerPipelineData {
    GasGrade m_grade = GasGrade::G87;
    int m_liters = 0;
    float m_price = 0;
};

struct FuelTankStatus {
    std::vector<float> m_gasVec;
    PriceMap m_priceMap;
};

struct PumpStatus {
    std::string m_customerName = "";
    int m_creditCardNum = 0;
    float m_liters = 0.f;
    float m_price = 0.f;
    GasGrade m_grade = GasGrade::G87;
};

// Return random number generated between min and max, inclusive 
inline int getRandNum(int min, int max) {
    return rand() % (max - min + 1) + min;
}

#endif
