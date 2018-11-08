#include "FuelTankMonitor.h"


// MARK: Initializer

FuelTankMonitor::FuelTankMonitor(const std::string fuelTankDataPoolId) {
	m_fuelTankSemaphore = new CSemaphore(FUEL_TANK_SEMAPHORE_STR, 1, 1);
	m_fuelTankDataPool = new CDataPool(fuelTankDataPoolId, sizeof(FuelTankStatus));
	m_fuelTankStatusPtr = static_cast<FuelTankStatus*>(m_fuelTankDataPool->LinkDataPool());
}

FuelTankMonitor::~FuelTankMonitor() {
	delete m_fuelTankDataPool;
	delete m_fuelTankSemaphore;
}


// MARK: API

// Set initial gas grade price, etc.
void FuelTankMonitor::initializeFuelTank() {
	m_fuelTankSemaphore->Wait();

	// Set initial fuel tank quantity to maximum
	for (int i = 0; i < 4; ++i) {
		m_fuelTankStatusPtr->m_gasVec[i] = MAX_FUELTANK_CAPACITY;
	}

	// Set initial gas prices for the different grades
	m_fuelTankStatusPtr->m_prices.m_g87Price = 1.52f;
	m_fuelTankStatusPtr->m_prices.m_g89Price = 1.58f;
	m_fuelTankStatusPtr->m_prices.m_g91Price = 1.72f;
	m_fuelTankStatusPtr->m_prices.m_g92Price = 1.97f;

	m_fuelTankSemaphore->Signal();
}

// Gets the current gas prices for the new customer to see
GasPrice FuelTankMonitor::getCurrentGasPrices() {
	GasPrice currentPrices;
	m_fuelTankSemaphore->Wait();
	currentPrices = m_fuelTankStatusPtr->m_prices;
	m_fuelTankSemaphore->Signal();
	return currentPrices;
}

// Gets the current quantity left in the specified fuel tank
float FuelTankMonitor::getFuelQuantityForTank(int tankNum) {
	return m_fuelTankStatusPtr->m_gasVec[tankNum];
}

// Refills the fuel tank specified by the gas station attendant
void FuelTankMonitor::refillFuelTank(int tankNum) {
	m_fuelTankSemaphore->Wait();
	m_fuelTankStatusPtr->m_gasVec[tankNum] = MAX_FUELTANK_CAPACITY;
	m_fuelTankSemaphore->Signal();
}

// Sets a new price for the specified gas grade
void FuelTankMonitor::setPriceForGasGrade(GasGrade grade, float newPrice) {
	m_fuelTankSemaphore->Wait();
	switch (grade)
	{
	case GasGrade::G87:
		m_fuelTankStatusPtr->m_prices.m_g87Price = newPrice;
	case GasGrade::G89:
		m_fuelTankStatusPtr->m_prices.m_g89Price = newPrice;
	case GasGrade::G91:
		m_fuelTankStatusPtr->m_prices.m_g91Price = newPrice;
	case GasGrade::G92:
		m_fuelTankStatusPtr->m_prices.m_g92Price = newPrice;
	default:
		break;
	}
	m_fuelTankSemaphore->Signal();
}

// Decrements fuel tank quantity as gas is dispensed
float FuelTankMonitor::dispenseFuelFromTank(int tankNum) {
	m_fuelTankSemaphore->Wait();
	m_fuelTankStatusPtr->m_gasVec[tankNum] -= 0.5;
	float gas = m_fuelTankStatusPtr->m_gasVec[tankNum];
	m_fuelTankSemaphore->Signal();
	return gas;
}

// Waits for semaphore
void FuelTankMonitor::wait() {
	m_fuelTankSemaphore->Wait();
}

// Signals the semaphore
void FuelTankMonitor::signal() {
	m_fuelTankSemaphore->Signal();
}