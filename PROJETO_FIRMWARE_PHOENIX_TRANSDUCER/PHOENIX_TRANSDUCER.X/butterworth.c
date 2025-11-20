#include <stdio.h>
#include <stdlib.h>
#include <libq.h>
#include "hardware.h"
#include "timer.h"
#include "butterworth.h"
#include "math.h"

ButterStruct sButter;
double xData[5] = {0};
//
double yData[5] = {0};

void ButterInit(void)
{
    sButter.fc = BUTTER_DEFAULT_FC;
    sButter.fs = BUTTER_DEFAULT_FS;
    ButterComputeCoef(sButter.fc);
}
//
void ButterComputeCoef(double fc)
{
    double dTemp;
    double wc2, wc3, T2;
    if(fc >= BUTTER_MIN_FC && fc <= BUTTER_MAX_FC)
    {
        //Compute T
        sButter.T = 1.0/sButter.fs;
        T2 = sButter.T * sButter.T;
        //Compute Omega
        dTemp = 3.14159265358979323846 * sButter.T * fc;
        sButter.wc = (2 * (tan(dTemp))) / sButter.T;
        wc2 = sButter.wc * sButter.wc;
        wc3 = wc2 * sButter.wc;
        //Compute B's coeffs
        sButter.B0 = T2 * sButter.T;
        sButter.B1 = sButter.B0 * 3;
        sButter.B2 = sButter.B1;
        sButter.B3 = sButter.B0;
        //Compute A0
        dTemp = 8.0 / wc3;
        dTemp += (8.0 * sButter.T) / wc2;
        dTemp += (4.0 * T2) / sButter.wc;
        sButter.A0 = dTemp + sButter.B0;
        //Compute A1
        dTemp = -24.0 / wc3;
        dTemp -= (8.0 * sButter.T) /wc2;
        dTemp += (4.0 * T2) / sButter.wc;
        sButter.A1 = dTemp + sButter.B1;
        //Compute A2
        dTemp = 24.0 / wc3;
        dTemp -= (8.0 * sButter.T) / wc2;
        dTemp -= (4.0 * T2) / sButter.wc;
        sButter.A2 = dTemp + sButter.B1;
        //Compute A3
        dTemp = -8.0 / wc3;
        dTemp += (8.0 * sButter.T) / wc2;
        dTemp -= (4.0 * T2) / sButter.wc;
        sButter.A3 = dTemp + sButter.B0;
        //
        sButter.B0A0 = sButter.B0 / sButter.A0;
        sButter.B1A0 = sButter.B1 / sButter.A0;
        sButter.B2A0 = sButter.B2 / sButter.A0;
        sButter.B3A0 = sButter.B3 / sButter.A0;
        sButter.A1A0 = sButter.A1 / sButter.A0;
        sButter.A2A0 = sButter.A2 / sButter.A0;
        sButter.A3A0 = sButter.A3 / sButter.A0;
    }
}
//
void ButterComputeYn(void)
{
    unsigned int n;
    double dTemp, dTemp2;
    for(n = 3; n < 10; n++)
    {
        //TEST_PIN = ON;
        dTemp = sButter.B0A0 * xData[n];
        dTemp += sButter.B1A0 * xData[n - 1];
        dTemp += sButter.B2A0 * xData[n - 2];
        dTemp += sButter.B3A0 * xData[n - 3];
        dTemp2 = sButter.A1A0 * yData[n - 1];
        dTemp2 += sButter.A2A0 * yData[n - 2];
        dTemp2 += sButter.A3A0 * yData[n - 3];
        yData[n] = dTemp - dTemp2;
        //TEST_PIN = OFF;
    }
}

