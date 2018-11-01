#pragma once
#include "..\rt.h"
#include "IncludeFiles.h"
#include "Shared.h"
#include "SafePrint.h"

#ifndef __GasStationComputer__
#define __GasStationComputer__

struct Transaction {
    float m_price = 0;
    float m_liters = 0;
    GasGrade m_grade;
    int m_cardNum;
    std::string m_customerName = "";
    std::chrono::system_clock::time_point m_currentTime;
    Transaction();
};

class GasStationComputer 
    : public ActiveClass
{
public:
    GasStationComputer(SafePrint& safePrint);
    ~GasStationComputer();
    int main(void);
private:
    int checkFuelTankStatus(void* args);
    int checkPumpStatus(void* args);
    int readFromKeyboard(void* args);
    int waitForClearSignal(void* args);

    // Data pool vars
    FuelTankStatus* m_fuelTankStatusPtr    = nullptr;

    std::vector<Transaction> m_transactionVec;

    struct PumpStatusPtrLock {
        PumpStatus* m_pumpStatus = nullptr;
        CSemaphore* m_pumpProducerLock = nullptr;
        CSemaphore* m_pumpConsumerLock = nullptr;
        CSemaphore* m_signal = nullptr;
        CSemaphore* m_clearSignal = nullptr;
        std::vector<Transaction> m_transactionVec;
        int m_pumpNum;
    };

    // Status of the pumps
    bool m_isEnabledpump1 = true;
    bool m_isEnabledpump2 = true;
    bool m_isEnabledpump3 = true;
    bool m_isEnabledpump4 = true;

    // Used to track if gas tank has sufficient gas to fuel
    bool m_hasGas1 = true;
    bool m_hasGas2 = true;
    bool m_hasGas3 = true;
    bool m_hasGas4 = true;

    // Pump Status Objects
    PumpStatusPtrLock m_pump1;
    PumpStatusPtrLock m_pump2;
    PumpStatusPtrLock m_pump3;
    PumpStatusPtrLock m_pump4;

    // FuelTank Semaphore
    CSemaphore m_fuelTankSemaphore;

    // Reference to safePrint
    SafePrint& m_safePrint;
};

#endif