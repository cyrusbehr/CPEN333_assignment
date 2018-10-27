#pragma once
#include "IncludeFiles.h"
#ifndef __Shared__
#define __Shared__

constexpr int PIPE_SIZE = 1024;
constexpr int MAX_FUELTANK_CAPACITY = 500;

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
