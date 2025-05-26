
#include "../../gcode.h"
#include "../../../feature/dac_ad5663r.h"

#if ENABLED(HAS_XY_DAC)

/**
 * @brief Set Laser X Parameters
 * <Begin, Mid, End, Scaling>
 */
void GcodeSuite::M5001() {
    if (!parser.seen_any())
    {
        SERIAL_ECHOPGM("X Axis DAC:");
        SERIAL_ECHOPGM("Begin", ad5663r::settings[0].min_val);
        SERIAL_ECHOPGM("Middle", ad5663r::settings[0].mid_val);
        SERIAL_ECHOPGM("End", ad5663r::settings[0].max_val);
        SERIAL_ECHOPGM("Scaling", ad5663r::settings[0].scaling);
        return;
    }

    if (parser.seen("B")) {}
    if (parser.seen("M")) {}
    if (parser.seen("E")) {}
    if (parser.seen("S")) {}
}

/**
 * @brief Set Laser Y Parameters
 * <Start, Mid, End, Scaling>
 */
void GcodeSuite::M5002() {
    if (!parser.seen_any())
    {
        SERIAL_ECHOPGM("X Axis DAC:");
        SERIAL_ECHOPGM("Begin", ad5663r::settings[1].min_val);
        SERIAL_ECHOPGM("Middle", ad5663r::settings[1].mid_val);
        SERIAL_ECHOPGM("End", ad5663r::settings[1].max_val);
        SERIAL_ECHOPGM("Scaling", ad5663r::settings[1].scaling);
        return;
    }
    if (parser.seen("B")) {}
    if (parser.seen("M")) {}
    if (parser.seen("E")) {}
    if (parser.seen("S")) {}
}

/**
 * @brief Set Laser Speed Scaling
 * S
 */
void GcodeSuite::M5003() {
    if (parser.seen("S")) {}
}

#endif