#include "IncludeFiles.h"

enum class GasGrade {
    G87,
    G89,
    G91,
    G92
};

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
    GasGrade m_grade = GasGrade::G87;
};

