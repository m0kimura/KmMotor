//*******1*********2*********3*********4*********4*********5*********6*********7*********8
#include "Arduino.h"
#define DATA_NUM 50
/* ir.h */
#ifndef KMMOTOR_H
#define KMMOTOR_H

class KmMotor{
  public:
    KmMotor(int pdirect, int pspeed);
    void begin(int base, int itvl);
    void fore(int n);
    void back(int n);
    void stop();
    void wait(int n);
    void pattern(String txt);
    void loop(String txt, int n);
    void halt();
    void drive();
    int Now;
    int Dir;
    int Next;
    int Itvl;
    int Que[3];
    byte Cmd[DATA_NUM];
    int Vol[DATA_NUM];
    int Mx;
    int Nx;
    int Loop;
    String Pttn;
    long Cont;
    long Save;
    int PinA;
    int PinB;
    int Base;

  private:

};

#endif
