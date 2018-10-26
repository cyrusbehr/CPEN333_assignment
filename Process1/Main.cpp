#include "../IncludeFiles.h"
#include "Pump.h"
#include "Customer.h"

int main(void) {

    std::vector<std::unique_ptr<Pump>> pumpVec;

    // Create 4 pump objects, establish fuel tank and pump data pool connections
    pumpVec.push_back(std::make_unique<Pump>("Pump1", "FuelTankDataPool", "Pump1DataPool"));
    pumpVec.push_back(std::make_unique<Pump>("Pump2", "FuelTankDataPool", "Pump2DataPool"));
    pumpVec.push_back(std::make_unique<Pump>("Pump3", "FuelTankDataPool", "Pump3DataPool"));
    pumpVec.push_back(std::make_unique<Pump>("Pump4", "FuelTankDataPool", "Pump4DataPool"));

    int sleepTime = 0;

    // Main loop which periodically creates 
    while (true) {

        // Wait 1 to 10 seconds before creating a new customer
        sleepTime = getRandNum(1, 10);
        
        sleepTime = 1;
        std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
        
        // Create a new customer and randomly add them to a pump queue
        auto newCustomer = new Customer;
        const auto pumpIdx = getRandNum(0, pumpVec.size() - 1);
        pumpVec.at(pumpIdx)->addCustomer(newCustomer);
    }

    return 0;
}