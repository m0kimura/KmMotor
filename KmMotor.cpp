//*******1*********2*********3*********4*********4*********5*********6*********7*********8
#include "Arduino.h"
#include "KmMotor.h"

KmMotor::KmMotor(int pdirect, int pspeed){
  PinA=pdirect; PinB=pspeed;
}

void KmMotor::begin(int base, int itvl){ // base arduino: 25, ESP8266 100, itvl 50
  pinMode(PinA, OUTPUT); //direction
  pinMode(PinB, OUTPUT); //speed
  analogWrite(PinA, 0); analogWrite(PinB, 0);
  Now=0; Dir=1; Next=0; Base=base; Itvl=itvl; Save=micros();
  Que[0]=0; Que[1]=0;
  Mx=0; Nx=0; Loop=0;
}

void KmMotor::fore(int n){
  if(Mx<DATA_NUM){Cmd[Mx]=1; Vol[Mx]=n; Mx++;}
}

void KmMotor::back(int n){
  if(Mx<DATA_NUM){Cmd[Mx]=2; Vol[Mx]=n; Mx++;}
}

void KmMotor::stop(){
  if(Mx<DATA_NUM){Cmd[Mx]=0; Vol[Mx]=0; Mx++;}
}

void KmMotor::halt(){
  Mx=0; Nx=0; Next=0; Loop=0;
}

void KmMotor::wait(int n){
  if(Mx<DATA_NUM){Cmd[Mx]=3; Vol[Mx]=n; Mx++;}
}

void KmMotor::pattern(String txt){
  String x; int c[3]; c[0]=0; c[1]=0; c[2]=0; String sp=""; String dr="";

  txt.concat(",");
  for(int i=0; i<txt.length(); i++){
    x=(String)txt.charAt(i);
    if(x==","){
      if(dr){if(Mx<DATA_NUM){
        if(dr=="f"){Cmd[Mx]=1;}else if(dr=="b"){Cmd[Mx]=2;}
        else if(dr=="w"){Cmd[Mx]=3;}else{Cmd[Mx]=0;}
        Vol[Mx]=sp.toInt();
        sp=""; dr=""; Mx++;
      }}
    }else{
      if(x>="0"&&x<"9"){sp.concat(x);}
      else if(x=="f" || x=="b" || x=="w"){dr=x;}
      else{dr="p";}
    }
  }

}

void KmMotor::loop(String txt, int n){
  Loop=n; Pttn=txt; KmMotor::pattern(Pttn);
}

void KmMotor::drive(){
  int s, l; long t, n; if(Next<0){Next=0;} if(Next>100){Next=100;}
  n=micros(); t=n-Save; l=Itvl*1000;
//
  if(Next>Now){
    if(t>l){
      Now++;
      if(Dir==1){s=Now*Base/10; digitalWrite(PinA, LOW);}
      else if(Dir==2){s=(100-Now)*Base/10; digitalWrite(PinA, HIGH);}
      else{return;}
      analogWrite(PinB, s);
    }
    Save=n;
    return;
  }
//
  if(Next<Now){
    if(t>l){
      Now--;
      if(Dir==1){s=Now*Base/10; digitalWrite(PinA, LOW);}
      else if(Dir==2){s=(100-Now)*Base/10; digitalWrite(PinA, HIGH);}
      else{return;}
      analogWrite(PinB, s);
    }
    Save=n;
    return;
  }
//
  if(Que[0]>0){
    if(Que[2]>0){Que[2]=Que[2]-(n-Save);}
    else{Dir=Que[0]; Next=Que[1]; Que[0]=0; Que[1]=0; Que[2]=0;}
    Save=n;
    return;
  }
//
  if(Cont>0){
    Cont=Cont-(n-Save); Save=n;
    return;
  }
//
  if(Mx>0&&Nx<Mx){
    if(Cmd[Nx]==0){Next=0;}
    else if(Cmd[Nx]==3){Cont=Vol[Nx]*1000000;}
    else{
      if(Dir==Cmd[Nx]){
        Dir=Cmd[Nx]; Next=Vol[Nx];
      }else{
        Next=0; Que[0]=Cmd[Nx]; Que[1]=Vol[Nx]; Que[2]=500000;
      }
    }
    Save=n;
    Nx++;
  }else{
    Mx=0; Nx=0; if(Loop){KmMotor::pattern(Pttn); if(Loop<10000){Loop--;}}
  }
//
}
