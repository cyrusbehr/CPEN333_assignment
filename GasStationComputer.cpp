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

    // TODO we need rendevous in all of our child threads!
    // TODO attendant can change gas price
    // TODO attendant can disable pumps
    // Command so that attendant can display list of all transactions

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
    std::string msg = "Enter a command [1/2/3/4 Dispense gas to pump 1/2/3/4, 5/6/7/8 refill tank 5/6/7/8]";
    m_safePrint.sPrint(msg, 0, 13);
    while (true) {
        int cmd;
        std::cin >> cmd;
        switch (cmd)
        {
        case 1:
            // Check that we actually have enough gas in tank
            if (!m_hasGas1) 
                break;
            m_pump1.m_signal->Signal();
            m_safePrint.sPrint("Status: Fueling...           ", m_safePrint.getColumnSize() / 4 * (0) + 1, 10);
            break;
        case 2:
            // Check that we actually have enough gas in tank
            if (!m_hasGas2)
                break;
            m_pump2.m_signal->Signal();
            m_safePrint.sPrint("Status: Fueling...           ", m_safePrint.getColumnSize() / 4 * (1) + 1, 10);
            break;
        case 3:
            // Check that we actually have enough gas in tank
            if (!m_hasGas3)
                break;
            m_pump3.m_signal->Signal();
            m_safePrint.sPrint("Status: Fueling...           ", m_safePrint.getColumnSize() / 4 * (2) + 1, 10);
            break;
        case 4:
            // Check that we actually have enough gas in tank
            if (!m_hasGas4)
                break;
            m_pump4.m_signal->Signal();
            m_safePrint.sPrint("Status: Fueling...           ", m_safePrint.getColumnSize() / 4 * (3) + 1, 10);
            break;

        default:
            break;
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
        status->m_transactionVec.push_back(newTransaction);
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
// TODO have a function whose only job is to read the input of the keyboard. if the user specifies the fill command, it sets a bool. This then sends a messaage back to the pump class which checks if there is a customer there. If so, then it fills the tanks. It should be within the listing function in the pump where we check if there is enough gas in the tank, deduct the price from the customer, and finally delete the customer
// should check if there is enough gas in the function which listens to the keyboard inputs

// TODO need to call our clear function when a new customer moves in