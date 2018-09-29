#include <string>

struct FuelTankStatus {
    float m_fuelTank1 = 0;
    float m_fuelTank2 = 0;
    float m_fuelTank3 = 0;
    float m_fuelTank4 = 0;
};

struct PumpStatus {
    std::string m_customerName = "";
    UINT m_creditCardNum = 0;
    float m_liters = 0.f;
    float m_price = 0.f;
};