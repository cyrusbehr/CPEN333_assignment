#pragma once
#include "IncludeFiles.h"
#ifndef __Shared__
#define __Shared__

constexpr int PIPE_SIZE = 1024;
constexpr int MAX_FUELTANK_CAPACITY = 500;
constexpr int MAX_NAME_LENGTH = 512;
constexpr int CUSTOMER_SPAWN_MAX_TIME = 8; // 10
constexpr int GAS_FILL_SLEEP = 100; // Milliseconds to sleep per 0.5 liters, default 1000

const std::string PUMP1_P_STR = "Pump1ProducerSemaphore";
const std::string PUMP1_C_STR = "Pump1ConsumerSemaphore";
const std::string PUMP2_P_STR = "Pump22ProducerSemaphore";
const std::string PUMP2_C_STR = "Pump22ConsumerSemaphore";
const std::string PUMP3_P_STR = "Pump3ProducerSemaphore";
const std::string PUMP3_C_STR = "Pump3ConsumerSemaphore";
const std::string PUMP4_P_STR = "Pump4ProducerSemaphore";
const std::string PUMP4_C_STR = "Pump4ConsumerSemaphore";

const std::string FUEL_TANK_SEMAPHORE_STR = "FuelTankSemaphore";

enum class Color {
    BLACK = 0,
    DARK_BLUE,
    DARK_GREEN,
    DARK_CYAN,
    DARK_RED,
    DARK_MAGENTA,
    DARK_YELLOW,
    GREY,
    BLACK_TWO,
    BLUE,
    GREEN,
    CYAN,
    RED, 
    MAGENTA,
    YELLOW,
    WHITE
};

enum class GasGrade {
    G87,
    G89,
    G91,
    G92
};

struct GasPrice {
    float m_g87Price = 0.f;
    float m_g89Price = 0.f;
    float m_g91Price = 0.f;
    float m_g92Price = 0.f;
};

struct CustomerPipelineData {
    GasGrade m_grade = GasGrade::G87;
    char m_name[MAX_NAME_LENGTH];
    int m_ccNum = 0;
    float m_liters = 0;
    float m_price = 0;
};

struct FuelTankStatus {
    float m_gasVec[4];
    GasPrice m_prices;
};

struct PumpStatus {
    char m_customerNameC[MAX_NAME_LENGTH];
    int m_creditCardNum = 0;
    float m_liters = 0.f;
    float m_price = 0.f;
    GasGrade m_grade = GasGrade::G87;
};



// Return random number generated between min and max, inclusive 
inline int getRandNum(int min, int max) {
    return rand() % (max - min + 1) + min;
}

// Wrapper class for print resource
class SafePrint {
public:
    SafePrint(std::mutex& mtx, int secSize);
    void sPrint(std::string str, int x, int y, Color color = Color::BLACK);

    void clearColumn(int x, int yStart);
    void drawHorizontalLine(int y, Color color = Color::BLACK);
    void drawVerticalLine(int x, int yStart, Color color = Color::BLACK);
    void drawVerticalLine(int x, int yStart, int yEnd, Color color = Color::BLACK);
    int getRowSize();
    int getColumnSize();
    void clearSection(int pumpNumber);

    std::string gradeToString(GasGrade grade);

private:
    std::mutex& m_mutex;
    int m_sectionSize = 0;
    int m_consoleRowSize = 100;
    int m_consoleCoslumSize = 100;
};

#endif
