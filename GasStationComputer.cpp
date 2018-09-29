#include "GasStationComputer.h"

GasStationComputer::GasStationComputer() {
    // Create the Fuel Tank data pool
    CDataPool fuelTankDataPool("FuelTankDataPool", sizeof(FuelTankStatus));
    m_fuelTankStatusPtr = static_cast<FuelTankStatus*>(fuelTankDataPool.LinkDataPool());

    // Create child thread to display Fuel Tank Data Pool data
    m_fuelTankStatusThreadPtr = std::make_unique<ClassThread<GasStationComputer>>(ClassThread<GasStationComputer>(this, &GasStationComputer::displayFuelTankStatus, ACTIVE, nullptr));

    // Create 4 gas pump Data pools
    CDataPool pump1DataPool("Pump1DataPool", sizeof(PumpStatus));
    CDataPool pump2DataPool("Pump2DataPool", sizeof(PumpStatus));
    CDataPool pump3DataPool("Pump3DataPool", sizeof(PumpStatus));
    CDataPool pump4DataPool("Pump4DataPool", sizeof(PumpStatus));

    m_pump1StatusPtr = static_cast<PumpStatus*>(pump1DataPool.LinkDataPool());
    m_pump2StatusPtr = static_cast<PumpStatus*>(pump2DataPool.LinkDataPool());
    m_pump3StatusPtr = static_cast<PumpStatus*>(pump3DataPool.LinkDataPool());
    m_pump4StatusPtr = static_cast<PumpStatus*>(pump4DataPool.LinkDataPool());
    
    // TODO: Create 4 child threads to read from Gas Pump Data Pool
    m_pump1StatusThreadPtr = std::make_unique<ClassThread<GasStationComputer>>(ClassThread<GasStationComputer>(this, &GasStationComputer::readPumpStatus, ACTIVE, m_pump1StatusPtr));
    m_pump2StatusThreadPtr = std::make_unique<ClassThread<GasStationComputer>>(ClassThread<GasStationComputer>(this, &GasStationComputer::readPumpStatus, ACTIVE, m_pump2StatusPtr));
    m_pump3StatusThreadPtr = std::make_unique<ClassThread<GasStationComputer>>(ClassThread<GasStationComputer>(this, &GasStationComputer::readPumpStatus, ACTIVE, m_pump3StatusPtr));
    m_pump4StatusThreadPtr = std::make_unique<ClassThread<GasStationComputer>>(ClassThread<GasStationComputer>(this, &GasStationComputer::readPumpStatus, ACTIVE, m_pump4StatusPtr));
}

int GasStationComputer::displayFuelTankStatus(void* args) {
    // TODO: Periodically print out the pump status
    return 0;
}

int GasStationComputer::readPumpStatus(void* args) {
    PumpStatus* status = static_cast<PumpStatus*>(args);
    // TODO: Do something with data pool data, such as print it out to the console
    return 0;
}