/**
 ******************************************************************************
 * @addtogroup OpenPilotModules OpenPilot Modules
 * @{
 * @addtogroup StabilizationModule Stabilization Module
 * @brief Virtual flybar mode
 * @note This file implements the logic for a virtual flybar
 * @{
 *
 * @file       virtualflybar.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2012.
 * @brief      Attitude stabilization module.
 *
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include "openpilot.h"
#include <pios_math.h>
#include "stabilization.h"
#include "stabilizationsettings.h"

// ! Private variables
static float vbar_integral[3];
static float vbar_decay = 0.991f;

int stabilization_virtual_flybar(float gyro, float command, float *output, float dT, bool reinit, uint32_t axis, StabilizationSettingsData *settings)
{
    float gyro_gain = 1.0f;
    float kp = 0, ki = 0;

    if (reinit) {
        vbar_integral[axis] = 0;
    }

    // Track the angle of the virtual flybar which includes a slow decay
    vbar_integral[axis] = vbar_integral[axis] * vbar_decay + gyro * dT;
    vbar_integral[axis] = boundf(vbar_integral[axis], -settings->VbarMaxAngle, settings->VbarMaxAngle);

    // Command signal can indicate how much to disregard the gyro feedback (fast flips)
    if (settings->VbarGyroSuppress > 0) {
        gyro_gain = (1.0f - fabsf(command) * settings->VbarGyroSuppress / 100.0f);
        gyro_gain = (gyro_gain < 0) ? 0 : gyro_gain;
    }

    // Get the settings for the correct axis
    switch (axis) {
    case 0:
        kp = settings->VbarRollPI.Kp;
        ki = settings->VbarRollPI.Ki;
        break;
    case 1:
        kp = settings->VbarPitchPI.Kp;
        ki = settings->VbarPitchPI.Ki;;
        break;
    case 2:
        kp = settings->VbarYawPI.Kp;
        ki = settings->VbarYawPI.Ki;
        break;
    default:
        PIOS_DEBUG_Assert(0);
    }

    // Command signal is composed of stick input added to the gyro and virtual flybar
    *output = command * StabilizationSettingsVbarSensitivityToArray(settings->VbarSensitivity)[axis] -
              gyro_gain * (vbar_integral[axis] * ki + gyro * kp);

    return 0;
}

/**
 * Want to keep the virtual flybar fixed in world coordinates as we pirouette
 * @param[in] z_gyro The deg/s of rotation along the z axis
 * @param[in] dT The time since last sample
 */
int stabilization_virtual_flybar_pirocomp(float z_gyro, float dT)
{
    float cy = cosf(DEG2RAD(z_gyro) * dT);
    float sy = sinf(DEG2RAD(z_gyro) * dT);

    float vbar_pitch = cy * vbar_integral[1] - sy * vbar_integral[0];
    float vbar_roll  = sy * vbar_integral[1] + cy * vbar_integral[0];

    vbar_integral[1] = vbar_pitch;
    vbar_integral[0] = vbar_roll;

    return 0;
}
