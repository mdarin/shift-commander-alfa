#ifndef APPXX_TCU_PARAMS_HPP
#define APPXX_TCU_PARAMS_HPP

/* Includes ------------------------------------------------------------------*/
#include <xx_types.hpp>

/* TUC parameters */
namespace tcu
{
    // Это пространство или слой программы, в котором уже можно и нужно использовать возможности С++

    /* Exported variables -------------------------------------------------------*/

    // INPUTS (контакты указаны на разъёме TCU)

    // [Position switch] Park Neutral Position Switch (PNPS) - Adjustment
    // X(L2) 8
    // Y(L3) 37
    // Z(L4, GS8.87.0) 9
    // Only 3 - the X, Y, Z switches - are connected to the TCM.
    int pnps; // todo class

    // Primary power supply 26
    // Ignition power supply 54
    int ign_pwr; // todo class

    // High/Low range 13
    // Есть два режима(на картинке MES в мануле по 4hp22 для land rover понятно)
    // In high range the choice is between Economy and Sport.
    // In low range, the driver can by using the MES switch select a Manual mode.
    int mode_range; // todo class

    // Output Shaft Speed Sensor 14 (42,15 экран?)
    int rpm;
    int n3;
    app::TachoN2 n2; // todo class

    // [Engine management]
    // CAN_H  16
    // CAN_L  44
    int eng; // todo class

    // OUTPUTS
    // [MES] H-Gate Shifter Module
    // The H-gate module contains a series of lights to indicate gear and range (high or low)
    // selected as well as a mode switch - Manual, Economy, Sport (MES)
    // MES1(Mode output) 25
    // MES2(Mode output) 51
    int mes; // todo class

    // Mode(Sport/Manual) 45
    int mode_s_m; // todo class

    // Solenoid valves 30,32,33
    int mv1, mv2, mv3;

    // Preasure regulator 6
    int mv5;

} // end of namespace tcu

#endif /* APPXX_TCU_PARAMS_HPP */
