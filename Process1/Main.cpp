#include "../IncludeFiles.h"
#include "Pump.h"
#include "Customer.h"

int main(void) {

    std::vector<std::unique_ptr<Pump>> pumpVec;

    // Create 4 pump objects, establish fuel tank and pump data pool connections
    pumpVec.push_back(std::make_unique<Pump>("Pump1", 1, "FuelTankDataPool", "Pump1DataPool", PUMP1_P_STR, PUMP1_C_STR));
    pumpVec.push_back(std::make_unique<Pump>("Pump2", 2, "FuelTankDataPool", "Pump2DataPool", PUMP2_P_STR, PUMP2_C_STR));
    pumpVec.push_back(std::make_unique<Pump>("Pump3", 3, "FuelTankDataPool", "Pump3DataPool", PUMP3_P_STR, PUMP3_C_STR));
    pumpVec.push_back(std::make_unique<Pump>("Pump4", 4, "FuelTankDataPool", "Pump4DataPool", PUMP4_P_STR, PUMP4_C_STR));

    int sleepTime = 0;

    // Main loop which periodically creates 
    while (true) {

        // Wait 1 to 10 seconds before creating a new customer
        sleepTime = getRandNum(1, 10);
        std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
        
        // Create a new customer and randomly add them to a pump queue
        auto newCustomer = new Customer;
        const auto pumpIdx = getRandNum(0, pumpVec.size() - 1);
        pumpVec.at(pumpIdx)->addCustomer(newCustomer);
    }

    return 0;
}