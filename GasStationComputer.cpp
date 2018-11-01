#include "GasStationComputer.h"

Transaction::Transaction() {
    // Sets the timestamp when the transaction object is initialized
    m_currentTime = std::chrono::system_clock::now();
}

int GasStationComputer::main(void) {
    // Create 4 gas pump Data pools
    CDataPool pump1DataPool("Pump1DataPool", sizeof(PumpStatus));
    CDataPool pump2DataPool("Pump2DataPool", sizeof(PumpStatus));
    CDataPool pump3DataPool("Pump3DataPool", sizeof(PumpStatus));
    CDataPool pump4DataPool("Pump4DataPool", sizeof(PumpStatus));

    CDataPool fuelTankDataPool("FuelTankDataPool", sizeof(FuelTankStatus));
    m_fuelTankStatusPtr = static_cast<FuelTankStatus*>(fuelTankDataPool.LinkDataPool());

    // Set the initial quantity of gas for each of the 4 pumps
    for (int i = 0; i < 4; ++i) {
        m_fuelTankStatusPtr->m_gasVec[i] = MAX_FUELTANK_CAPACITY;
    }

    // Set the initial gas prices
    m_fuelTankStatusPtr->m_prices.m_g87Price = 1.52f;
    m_fuelTankStatusPtr->m_prices.m_g89Price = 1.58f;
    m_fuelTankStatusPtr->m_prices.m_g91Price = 1.72f;
    m_fuelTankStatusPtr->m_prices.m_g92Price = 1.97f;

    m_pump1.m_pumpStatus = static_cast<PumpStatus*>(pump1DataPool.LinkDataPool());
    m_pump2.m_pumpStatus = static_cast<PumpStatus*>(pump2DataPool.LinkDataPool());
    m_pump3.m_pumpStatus = static_cast<PumpStatus*>(pump3DataPool.LinkDataPool());
    m_pump4.m_pumpStatus = static_cast<PumpStatus*>(pump4DataPool.LinkDataPool());

    // Create Producer Consumer Semaphores
    m_pump1.m_pumpProducerLock = new CSemaphore(PUMP1_P_STR, 0, 1);
    m_pump1.m_pumpConsumerLock = new CSemaphore(PUMP1_C_STR, 1, 1);
    m_pump2.m_pumpProducerLock = new CSemaphore(PUMP2_P_STR, 0, 1);
    m_pump2.m_pumpConsumerLock = new CSemaphore(PUMP2_C_STR, 1, 1);
    m_pump3.m_pumpProducerLock = new CSemaphore(PUMP3_P_STR, 0, 1);
    m_pump3.m_pumpConsumerLock = new CSemaphore(PUMP3_C_STR, 1, 1);
    m_pump4.m_pumpProducerLock = new CSemaphore(PUMP4_P_STR, 0, 1);
    m_pump4.m_pumpConsumerLock = new CSemaphore(PUMP4_C_STR, 1, 1);
    m_pump1.m_signal = new CSemaphore("Pump1Signal", 0, 1);
    m_pump2.m_signal = new CSemaphore("Pump2Signal", 0, 1);
    m_pump3.m_signal = new CSemaphore("Pump3Signal", 0, 1);
    m_pump4.m_signal = new CSemaphore("Pump4Signal", 0, 1);
    m_pump1.m_clearSignal = new CSemaphore("Pump1ClearSignal", 0, 1);
    m_pump2.m_clearSignal = new CSemaphore("Pump2ClearSignal", 0, 1);
    m_pump3.m_clearSignal = new CSemaphore("Pump3ClearSignal", 0, 1);
    m_pump4.m_clearSignal = new CSemaphore("Pump4ClearSignal", 0, 1);
    m_pump1.m_pumpNum = 1;
    m_pump2.m_pumpNum = 2;
    m_pump3.m_pumpNum = 3;
    m_pump4.m_pumpNum = 4;

    // Create child thread to display Fuel Tank Data Pool data
    ClassThread<GasStationComputer> fuelTankStatusThread(this, &GasStationComputer::checkFuelTankStatus, ACTIVE, nullptr);
    ClassThread<GasStationComputer> pump1StatusThread(this, &GasStationComputer::checkPumpStatus, ACTIVE, &m_pump1);
    ClassThread<GasStationComputer> pump2StatusThread(this, &GasStationComputer::checkPumpStatus, ACTIVE, &m_pump2);
    ClassThread<GasStationComputer> pump3StatusThread(this, &GasStationComputer::checkPumpStatus, ACTIVE, &m_pump3);
    ClassThread<GasStationComputer> pump4StatusThread(this, &GasStationComputer::checkPumpStatus, ACTIVE, &m_pump4);
    
    // Create child thread to constantly read from keyboard
    ClassThread<GasStationComputer> keyboardInputThread(this, &GasStationComputer::readFromKeyboard, ACTIVE, nullptr);

    // Create child threads to wait on clear signal
    ClassThread<GasStationComputer> pump1ClearThread(this, &GasStationComputer::waitForClearSignal, ACTIVE, &m_pump1);
    ClassThread<GasStationComputer> pump2ClearThread(this, &GasStationComputer::waitForClearSignal, ACTIVE, &m_pump2);
    ClassThread<GasStationComputer> pump3ClearThread(this, &GasStationComputer::waitForClearSignal, ACTIVE, &m_pump3);
    ClassThread<GasStationComputer> pump4ClearThread(this, &GasStationComputer::waitForClearSignal, ACTIVE, &m_pump4);

    // Launch the child process
    CProcess p1("C:\\Users\\cyrus\\source\\repos\\Mech4\\CPEN_333\\Assignment1\\Debug\\Process1.exe",
        NORMAL_PRIORITY_CLASS,
        OWN_WINDOW,
        ACTIVE
    );

    fuelTankStatusThread.WaitForThread();
    pump1StatusThread.WaitForThread();
    pump2StatusThread.WaitForThread();
    pump3StatusThread.WaitForThread();
    pump4StatusThread.WaitForThread();
    keyboardInputThread.WaitForThread();

    return 0;
}

int GasStationComputer::waitForClearSignal(void* args) {
    while (true) {
        // Wait for the clear signal
        // When it is received, clear the display
        auto status = static_cast<PumpStatusPtrLock*>(args);
        status->m_clearSignal->Wait();
        m_safePrint.clearSection(status->m_pumpNum);
    }
    return 0;
}

GasStationComputer::GasStationComputer(SafePrint& safePrint)
    :m_fuelTankSemaphore(FUEL_TANK_SEMAPHORE_STR, 1, 1)
    , m_safePrint(safePrint)
{}

GasStationComputer::~GasStationComputer() {
    delete m_pump1.m_pumpStatus;
    delete m_pump1.m_pumpConsumerLock;
    delete m_pump1.m_pumpProducerLock;
    delete m_pump1.m_signal;
    delete m_pump1.m_clearSignal;

    delete m_pump2.m_pumpStatus;
    delete m_pump2.m_pumpConsumerLock;
    delete m_pump2.m_pumpProducerLock;
    delete m_pump2.m_signal;
    delete m_pump2.m_clearSignal;

    delete m_pump3.m_pumpStatus;
    delete m_pump3.m_pumpConsumerLock;
    delete m_pump3.m_pumpProducerLock;
    delete m_pump3.m_signal;
    delete m_pump3.m_clearSignal;

    delete m_pump4.m_pumpStatus;
    delete m_pump4.m_pumpConsumerLock;
    delete m_pump4.m_pumpProducerLock;
    delete m_pump4.m_signal;
    delete m_pump4.m_clearSignal;


}

int GasStationComputer::checkFuelTankStatus(void* args) {
    // Thread Function
    // Create the Fuel Tank data pool
    while (true) {
        // If any of the gas tanks have less than 200 liters, they should flash red
        // m_hasGas used to determine if there is enough gas to begin a gas up procedure
        // Block scope b/c colliding var name, easier than changing var names...
        m_fuelTankSemaphore.Wait();
        {
            if (m_fuelTankStatusPtr->m_gasVec[0] <= 200) {
                TEXT_COLOUR(12, 0);
                m_hasGas1 = false;
            }
            else {
                m_hasGas1 = true;
            }
            std::string gasString = "Liters: " + std::to_string(m_fuelTankStatusPtr->m_gasVec[0]);
            m_safePrint.sPrint(gasString, m_safePrint.getColumnSize() / 8 - gasString.length() / 2 + m_safePrint.getColumnSize() / 4 * 0, 3);
            TEXT_COLOUR();
        }
        {
            if (m_fuelTankStatusPtr->m_gasVec[1] <= 200) {
                TEXT_COLOUR(12, 0);
                m_hasGas2 = false;
            }
            else {
                m_hasGas2 = true;
            }
            std::string gasString = "Liters: " + std::to_string(m_fuelTankStatusPtr->m_gasVec[1]);
            m_safePrint.sPrint(gasString, m_safePrint.getColumnSize() / 8 - gasString.length() / 2 + m_safePrint.getColumnSize() / 4 * 1, 3);
            TEXT_COLOUR();
        }
        {
            if (m_fuelTankStatusPtr->m_gasVec[2] <= 200) {
                TEXT_COLOUR(12, 0);
                m_hasGas3 = false;
            }
            else {
                m_hasGas3 = true;
            }
            std::string gasString = "Liters: " + std::to_string(m_fuelTankStatusPtr->m_gasVec[2]);
            m_safePrint.sPrint(gasString, m_safePrint.getColumnSize() / 8 - gasString.length() / 2 + m_safePrint.getColumnSize() / 4 * 2, 3);
            TEXT_COLOUR();
        }
        {
            if (m_fuelTankStatusPtr->m_gasVec[3] <= 200) {
                TEXT_COLOUR(12, 0);
                m_hasGas4 = false;
            }
            else {
                m_hasGas4 = true;
            }
            std::string gasString = "Liters: " + std::to_string(m_fuelTankStatusPtr->m_gasVec[3]);
            m_safePrint.sPrint(gasString, m_safePrint.getColumnSize() / 8 - gasString.length() / 2 + m_safePrint.getColumnSize() / 4 * 3, 3);
            TEXT_COLOUR();
        }

        m_fuelTankSemaphore.Signal();
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    return 0;
}

int GasStationComputer::readFromKeyboard(void* args) {
    std::string msg = "Enter a command [1/2/3/4 Dispense gas to pump 1/2/3/4, 5/6/7/8 refill tank 5/6/7/8, print to display all transactions, d1/d2/d3/d4 to disable pumps, set87/set89/set91/set92 to set new price for grades]";
    m_safePrint.sPrint(msg, 0, 13);
    while (true) {
        std::string cmd;
        std::cin >> cmd;
        if (cmd == "1") {
            // Check that we actually have enough gas in tank
            if (m_hasGas1 && m_isEnabledpump1) {
                m_pump1.m_signal->Signal();
                m_safePrint.sPrint("Status: Fueling...           ", m_safePrint.getColumnSize() / 4 * (0) + 1, 10);
            }
        }
        else if (cmd == "2") {
            // Check that we actually have enough gas in tank
            if (m_hasGas2  && m_isEnabledpump2) {
                m_pump2.m_signal->Signal();
                m_safePrint.sPrint("Status: Fueling...           ", m_safePrint.getColumnSize() / 4 * (1) + 1, 10);
            }
        }
        else if (cmd == "3") {
            // Check that we actually have enough gas in tank
            if (m_hasGas3  && m_isEnabledpump3) {
                m_pump3.m_signal->Signal();
                m_safePrint.sPrint("Status: Fueling...           ", m_safePrint.getColumnSize() / 4 * (2) + 1, 10);
            }
        }
        else if (cmd == "4") {
            // Check that we actually have enough gas in tank
            if (m_hasGas4  && m_isEnabledpump4) {
            m_pump4.m_signal->Signal();
            m_safePrint.sPrint("Status: Fueling...           ", m_safePrint.getColumnSize() / 4 * (3) + 1, 10);
            }
        }
         else if (cmd == "print") {
            int i = 0;
            int j = 0;
            for (const auto& trans : m_transactionVec) {
                std::time_t t = std::chrono::system_clock::to_time_t(trans.m_currentTime);
                char timeStr[MAX_NAME_LENGTH];
                ctime_s(timeStr, sizeof(timeStr), &t);
                m_safePrint.sPrint(std::to_string(++j) + ") Name:" + trans.m_customerName, 1, 15 + i++);
                m_safePrint.sPrint("CC: " + std::to_string(trans.m_cardNum), 4, 15 + i++);
                m_safePrint.sPrint("Time: " + std::string(timeStr, MAX_NAME_LENGTH), 4, 15 + i++);
                m_safePrint.sPrint("Grade: " + m_safePrint.gradeToString(trans.m_grade), 4, 15 + i++);
                m_safePrint.sPrint("Liters: " + std::to_string(trans.m_liters), 4, 15 + i++);
                m_safePrint.sPrint("Price: " + std::to_string(trans.m_price), 4, 15 + i++);
                m_safePrint.sPrint(" ", 4, 15 + i++);
            }
        }
         else if (cmd == "5") {
            m_fuelTankSemaphore.Wait();
            m_fuelTankStatusPtr->m_gasVec[0] = MAX_FUELTANK_CAPACITY;
            m_fuelTankSemaphore.Signal();
        }
         else if (cmd == "6") {
            m_fuelTankSemaphore.Wait();
            m_fuelTankStatusPtr->m_gasVec[1] = MAX_FUELTANK_CAPACITY;
            m_fuelTankSemaphore.Signal();
        }
         else if (cmd == "7") {
            m_fuelTankSemaphore.Wait();
            m_fuelTankStatusPtr->m_gasVec[2] = MAX_FUELTANK_CAPACITY;
            m_fuelTankSemaphore.Signal();
        }
         else if (cmd == "8") {
            m_fuelTankSemaphore.Wait();
            m_fuelTankStatusPtr->m_gasVec[3] = MAX_FUELTANK_CAPACITY;
            m_fuelTankSemaphore.Signal();
        }
         else if (cmd == "d1") {
            m_isEnabledpump1 = !m_isEnabledpump1;
        }
         else if (cmd == "d2") {
            m_isEnabledpump2 = !m_isEnabledpump2;
        }
         else if (cmd == "d3") {
            m_isEnabledpump3 = !m_isEnabledpump3;
        }
         else if (cmd == "d4") {
            m_isEnabledpump4 = !m_isEnabledpump4;
        }
         else if (cmd == "set87") {
            float newPrice;
            std::cin >> newPrice;
            m_fuelTankSemaphore.Wait();
            m_fuelTankStatusPtr->m_prices.m_g87Price = newPrice;
            m_fuelTankSemaphore.Signal();
        }
         else if (cmd == "set89") {
            float newPrice;
            std::cin >> newPrice;
            m_fuelTankSemaphore.Wait();
            m_fuelTankStatusPtr->m_prices.m_g89Price = newPrice;
            m_fuelTankSemaphore.Signal();
        }
         else if (cmd == "set91") {
            float newPrice;
            std::cin >> newPrice;
            m_fuelTankSemaphore.Wait();
            m_fuelTankStatusPtr->m_prices.m_g91Price = newPrice;
            m_fuelTankSemaphore.Signal();
        }
         else if (cmd == "set92") {
            float newPrice;
            std::cin >> newPrice;
            m_fuelTankSemaphore.Wait();
            m_fuelTankStatusPtr->m_prices.m_g92Price = newPrice;
            m_fuelTankSemaphore.Signal();
        }
    }
    return 0;
}

int GasStationComputer::checkPumpStatus(void* args) {
    // Thread Function
    PumpStatusPtrLock* status = static_cast<PumpStatusPtrLock*>(args);
    auto& pStat = status->m_pumpStatus;
    while (true) {
        Transaction newTransaction;
        status->m_pumpProducerLock->Wait();

        // Get new transaction information and add it to a record of all transactions
        newTransaction.m_cardNum = pStat->m_creditCardNum;
        newTransaction.m_customerName = std::string(pStat->m_customerNameC, MAX_NAME_LENGTH);
        newTransaction.m_grade = pStat->m_grade;
        newTransaction.m_liters = pStat->m_liters;
        newTransaction.m_price = pStat->m_price;
        // Store transaction in overall transaction tracker, and tracker for each pump
        status->m_transactionVec.push_back(newTransaction);
        m_transactionVec.push_back(newTransaction);
        status->m_pumpConsumerLock->Signal();

        m_safePrint.sPrint("Name:   " + newTransaction.m_customerName, m_safePrint.getColumnSize() / 4 * (status->m_pumpNum - 1) + 1, 5);
        m_safePrint.sPrint("Liters: " + std::to_string(newTransaction.m_liters), m_safePrint.getColumnSize() / 4 * (status->m_pumpNum - 1) + 1, 6);
        m_safePrint.sPrint("Price:  " + std::to_string(newTransaction.m_price), m_safePrint.getColumnSize() / 4 * (status->m_pumpNum - 1) + 1, 7);
        m_safePrint.sPrint("Grade:  " + m_safePrint.gradeToString(newTransaction.m_grade), m_safePrint.getColumnSize() / 4 * (status->m_pumpNum - 1) + 1, 8);
        m_safePrint.sPrint("CC Num: " + std::to_string(newTransaction.m_cardNum), m_safePrint.getColumnSize() / 4 * (status->m_pumpNum - 1) + 1, 9);
        m_safePrint.sPrint("Status: Waiting for approval", m_safePrint.getColumnSize() / 4 * (status->m_pumpNum - 1) + 1, 10);
    }
    return 0;
}
