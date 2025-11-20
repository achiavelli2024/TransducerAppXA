#ifndef PEAKFINDER_H
#define	PEAKFINDER_H

#ifdef	__cplusplus
extern "C" {
#endif

#define SMOOTH_COEF_SIZE        8       //(-N ... 0 ... +N) N must be an even number. The final size is N * 2 + 1
#define MIN_DATABASE_SIZE       100     //Número mínimo de registros para iniciar a análise
#define MAX_PEAK_COUNT          160
#define PEAK_PHASE_SIZE         2

void PeakFinderInit(void);
//void PeakFinder(long *DataArray, unsigned int DataLenght);
void PeakFinder(unsigned int DataLenght);
void GetMinMax(unsigned int DataLenght);
void GetMinMaxPeak(unsigned int *IndexArray, unsigned int DataLenght);
void CopyRomArray2Registry(unsigned int DataLenght);
void PeakAverage(long ValueThreshold);
float AdCount2Torque(long AdValue);
void ClickFind(void);

extern const long pulseTestArray[];
#define TEST_ARRAY_LENGHT 4062//4062

typedef struct
{
    long SmoothCoef[SMOOTH_COEF_SIZE * 2 + 1];
    long ValueThreshold;
    long DerivativeTheshold;
    long DerivativePrevious;
    long DerivativeCurrent;
    long DerivativeDelta;
    long PeakCounter;
    long long PeakAverage;
    long PeakThreshold;
    long MaxValue;
    long MinValue;
    long MaxPeakValue;
    long MinPeakValue;
    unsigned int DataLenght;
    unsigned int MaxValueIndex;
    unsigned int MinValueIndex;
    unsigned int MaxPeakValueIndex;
    unsigned int MinPeakValueIndex;
    unsigned char DerivativePrevReady;
    unsigned int PeakIndexList[MAX_PEAK_COUNT];
    unsigned int DerivativeThesholdCount;
    unsigned int ValueThresholdCount;
    unsigned int ZeroCrossCount;
    unsigned int ValidPeakCounter;
    unsigned int ValidPeakPercentage;
    float fValueThreshold;
    float fPeakValue;
    float fDerivativeTheshold;
    float fPeakThreshold;
    float fPeakAverage;
    float fMaxValue;
    float fMinValue;
    float fMaxPeakValue;
    float fMinPeakValue;
    long ResultClickTorque;
}PeakFinderStruct;

extern PeakFinderStruct sPeakFinder;

#ifdef	__cplusplus
}
#endif

#endif	/* PEAKFINDER_H */

