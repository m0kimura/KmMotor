//*******1*********2*********3*********4*********4*********5*********6*********7*********8
#include "Arduino.h"
#define DATA_NUM 50
/* ir.h */
#ifndef KMMOTOR_H
#define KMMOTOR_H

class KmMotor{
  public:
    KmMotor(int paa, int pab, int pba, int pbb, int psense1, int psense2, int md, int debug);
    void begin(int base, int itvl);
    void fore(int n);
    void back(int n);
    void stop();
    void wait(int n);
    void turn(int n);
    void pattern(String txt);
    void loop(String txt, int n);
    void halt();
    void point(int cmd);
    String drive();
    int Now;
    int Dir;
    int Next;
    long Itvl;
    int Que[3];
    byte Cmd[DATA_NUM];
    int Vol[DATA_NUM];
    int Mx;
    int Nx;
    int Loop;
    String Pttn;
    long Cont;
    int Turn;
    long Save;
    int PinAA;
    int PinAB;
    int PinBA;
    int PinBB;
    int PinS1;
    int PinS2;
    int Base;
    long Tc1;
    long Tc2;

  private:
    void analog();
    void init(int base, int itvl);
    int Cnt1;
    int Cnt2;
    long RapT;
    long RapG;
    long Sv1;
    long Sv2;
    int Ix;
    bool Sf1;
    bool Sf2;
    int Debug;
    int Md;
};

#endif
