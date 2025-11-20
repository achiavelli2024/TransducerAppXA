#ifndef BUTTERWORTH_H
#define	BUTTERWORTH_H

#ifdef	__cplusplus
extern "C" {
#endif

#define  BUTTER_DEFAULT_FC      100.0f
#define  BUTTER_DEFAULT_FS      21000.0f
#define  BUTTER_MIN_FC          100.0f
#define  BUTTER_MAX_FC          5000.0f

typedef struct
{
    double fc;
    double fs;
    double T;
    double wc;
    double A0;
    double A1;
    double A2;
    double A3;
    double B0;
    double B1;
    double B2;
    double B3;
    //
    double B0A0;
    double B1A0;
    double B2A0;
    double B3A0;
    double A1A0;
    double A2A0;
    double A3A0;                
    //
    double Xn;
    double Xn1;
    double Xn2;
    double Xn3;
    double Yn;
    double Yn1;
    double Yn2;
    double Yn3;    

}ButterStruct;

void ButterInit(void);    
void ButterComputeCoef(double fc);
void ButterComputeYn(void);

extern double xData[];
extern double yData[];

#ifdef	__cplusplus
}
#endif
#endif


