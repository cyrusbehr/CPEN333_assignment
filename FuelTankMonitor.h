#include "\RTExamples\rt.h"
#include "IncludeFiles.h"
#include "Shared.h"

#ifndef __FuelTankMonitor__
#define __FuelTankMonitor__

class FuelTankMonitor {

private:
	CDataPool* m_fuelTankDataPool;
	FuelTankStatus* m_fuelTankStatusPtr;
	CSemaphore* m_fuelTankSemaphore;

public:
	FuelTankMonitor(const std::string fuelTankDataPoolId);
	~FuelTankMonitor();

	void initializeFuelTank();
	void wait();
	void signal();
	GasPrice getCurrentGasPrices();
	float getFuelQuantityForTank(int tankNum);
	void refillFuelTank(int tankNum);
	void setPriceForGasGrade(GasGrade grade, float newPrice);
	float dispenseFuelFromTank(int tankNum);
};

#endif // !__FuelTankMonitor__

