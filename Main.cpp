#include "IncludeFiles.h"
#include "GasStationComputer.h"

// BONUS FEATURES
// 1) Use MOVE_CURSOR to display everything in really nice way
// 2) Display real time list of customers at pump
// 3) Use of proper 3++. No global variables, Injection dependency, C++ 11 features (unique_ptr, lock_guard)
//    Proper use of object oriented design, Shared files for code reduction, Only c++ style cast, ect 

// Combine the two safeprint classes into 1 class which lives in shared, pass in ref to mtx

int main(void) {
    // Seed the random timer
    srand(time(NULL));

    // Create instance of safePrint class
    std::mutex mtx;
    SafePrint safePrint(mtx, 11);
    std::string headerStr = "GAS STATION PROCESS";
    safePrint.sPrint(headerStr, safePrint.getColumnSize() / 2 - headerStr.length() / 2, 0, Color::CYAN);
    
    // Build the grid
    safePrint.drawHorizontalLine(1, Color::MAGENTA);
    safePrint.drawHorizontalLine(4, Color::MAGENTA);
    safePrint.drawHorizontalLine(11, Color::MAGENTA);

    for (int i = 0; i < 4; ++i) {
        // Create the title for each pump, center within the 
        std::string pumpName ("Pump" + std::to_string(i + 1));
        safePrint.sPrint(pumpName, safePrint.getColumnSize() / 8 - pumpName.length() / 2 + safePrint.getColumnSize() / 4 * (i), 2, Color::DARK_GREEN);
        safePrint.drawVerticalLine(safePrint.getColumnSize() / 4 * (i), 5, 11, Color::MAGENTA);

    }

    GasStationComputer gasStationComputer(safePrint);
    gasStationComputer.Resume();
    while (true);
    return 0;
}