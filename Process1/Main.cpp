#include "../IncludeFiles.h"
#include "Pump.h"
#include "Customer.h"
#include "SafePrint.h"

int main(void) {
    // Seed the random function
    srand(time(NULL));

    // TODO make gas flash red when below threshold

    SafePrint safePrint;
    std::string headerStr = "PUMP PROCESS";
    safePrint.sPrint(headerStr, safePrint.getColumnSize() / 2 - headerStr.length()/2, 0);
    safePrint.drawHorizontalLine(1);

    std::vector<std::unique_ptr<Pump>> pumpVec;
    // Create 4 pump objects, establish fuel tank and pump data pool connections
    pumpVec.push_back(std::make_unique<Pump>(safePrint, "Pump1", 1, "FuelTankDataPool", "Pump1DataPool", PUMP1_P_STR, PUMP1_C_STR));
    pumpVec.push_back(std::make_unique<Pump>(safePrint, "Pump2", 2, "FuelTankDataPool", "Pump2DataPool", PUMP2_P_STR, PUMP2_C_STR));
    pumpVec.push_back(std::make_unique<Pump>(safePrint, "Pump3", 3, "FuelTankDataPool", "Pump3DataPool", PUMP3_P_STR, PUMP3_C_STR));
    pumpVec.push_back(std::make_unique<Pump>(safePrint, "Pump4", 4, "FuelTankDataPool", "Pump4DataPool", PUMP4_P_STR, PUMP4_C_STR));

    for (auto& pump : pumpVec) {
        pump->Resume();
    }

    int sleepTime = 0;

    // Main loop which periodically creates 
    while (true) {
        // Create a new customer and randomly add them to a pump queue
        auto newCustomer = new Customer;
        const auto pumpIdx = rand() % (pumpVec.size());
        pumpVec.at(pumpIdx)->addCustomer(newCustomer);

        // Wait 1 to 10 seconds before creating the next customer
        sleepTime = rand() % (10 - 1 + 1) + 1;
        std::this_thread::sleep_for(std::chrono::seconds(sleepTime));
    }

    return 0;
}