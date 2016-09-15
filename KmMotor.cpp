//*******1*********2*********3*********4*********4*********5*********6*********7*********8
// Nゲージ運転モジュール
#define LIMITTER 120000  // 120s Run Limitter
#define NOISIE 3 // Noise Cancel under times
#define IGNORE 2000 // Ignore under interval ms

#include "Arduino.h"
#include "KmMotor.h"
KmMotor::KmMotor(int paa, int pab, int pba, int pbb, int psense1, int psense2, int md, int debug){
  PinAA=paa; PinAB=pab; PinBA=pba; PinBB=pbb;
  PinS1=psense1; PinS2=psense2; Md=md; Debug=debug;
}

void KmMotor::begin(int base, int itvl){ // base arduino: 25, ESP8266 100, itvl 1

  if(PinAA){pinMode(PinAA, OUTPUT); pinMode(PinAB, OUTPUT);}

  if(PinBA){pinMode(PinBA, OUTPUT); pinMode(PinBB, OUTPUT);}

  if(PinS1){pinMode(PinS1, INPUT);} if(PinS2){pinMode(PinS2, INPUT);}

  KmMotor::init(base, itvl);

  if(Debug){Serial.println("Motor Start");}
}

void KmMotor::init(int base, int itvl){

  if(PinAA){analogWrite(PinAA, 0); analogWrite(PinAB, 0);}

  if(PinBA){digitalWrite(PinBA, LOW); digitalWrite(PinBB, LOW);}

  Now=0; Dir=1; Next=0; if(base>0){Base=base; Itvl=itvl;} Save=millis();
Itvl=1;
  Que[0]=0; Que[1]=0;
  Mx=0; Nx=0; Loop=0;
  if(PinS1){
    Cnt1=-1; Sv1=millis(); Sf1=digitalRead(PinS1);// 位置検出センサー
  }
  if(PinS2){
    Cnt2=-1; Sv2=millis(); Sf2=digitalRead(PinS2);// 位置検出センサー
  }
  Tc1=0; Tc2=0;
  Cont=0; Turn=0;
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
   KmMotor::init(0, 0);
}

void KmMotor::wait(int n){
  if(Mx<DATA_NUM){Cmd[Mx]=3; Vol[Mx]=n; Mx++;}
}

void KmMotor::turn(int n){
  if(Mx<DATA_NUM){Cmd[Mx]=4; Vol[Mx]=n; Mx++;}
}

void KmMotor::pattern(String txt){
  String x; int c[3]; c[0]=0; c[1]=0; c[2]=0; String sp=""; String dr="";
  txt.concat(",");
  for(int i=0; i<txt.length(); i++){
    x=(String)txt.charAt(i);
    if(x==","){
      if(dr){if(Mx<DATA_NUM){
        if(dr=="f"){Cmd[Mx]=1;}else if(dr=="b"){Cmd[Mx]=2;}
        else if(dr=="w"){Cmd[Mx]=3;}else if(dr=="t"){Cmd[Mx]=4;}
        else if(dr=="m"){Cmd[Mx]=7;}else if(dr=="s"){Cmd[Mx]=8;}
        else if(dr=="g"){Cmd[Mx]=9;}else if(dr=="h"){Cmd[Mx]=10;}
        else if(dr=="i"){Cmd[Mx]=5;}else if(dr=="r"){Cmd[Mx]=6;}else{Cmd[Mx]=0;}
        Vol[Mx]=sp.toInt();
        sp=""; dr=""; Mx++;
      }}
    }else{
      if(x>="0"&&x<="9"){sp.concat(x);}else{dr=x;}
    }
  }
}

void KmMotor::loop(String txt, int n){
  Loop=n; Pttn=txt; KmMotor::pattern(Pttn);
}

void KmMotor::point(int cmd){

  if(cmd==5){
    if(Md){
      digitalWrite(PinBA, HIGH); digitalWrite(PinBB, LOW);
      delay(300); digitalWrite(PinBA, LOW);
    }else{
      digitalWrite(PinBA, LOW); digitalWrite(PinBB, HIGH); 
      delay(300); digitalWrite(PinBB, LOW); 
    }
    if(Debug){Serial.println("Point 5i");}
  }

  if(cmd==6){
    if(Md){
      digitalWrite(PinBB, HIGH); digitalWrite(PinBA, LOW);
      delay(300); digitalWrite(PinBB, LOW); 
    }else{
      digitalWrite(PinBA, HIGH); digitalWrite(PinBB, LOW); 
      delay(300); digitalWrite(PinBB, HIGH); 
    }
    if(Debug){Serial.println("Point 6r");}
  }  

}
//
// analog
//
void KmMotor::analog(){

    int s;
    if(Md){
      s=Now*Base/10;
      if(Dir==1){digitalWrite(PinAB, LOW); analogWrite(PinAA, s);}
      else if(Dir==2){digitalWrite(PinAA, LOW); analogWrite(PinAB, s);}
      else{return;}
    }else{
      if(Dir==1){s=Now*Base/10; digitalWrite(PinAA, LOW);}
      else if(Dir==2){s=(100-Now)*Base/10; digitalWrite(PinAA, HIGH);}
      else{return;}
      analogWrite(PinAB, s);
    }
    if(Debug){Serial.println("s:"+String(s));}

}
//
// drive
//
String KmMotor::drive(){
  long span, laps, nowm; if(Next<0){Next=0;} if(Next>100){Next=100;}
  nowm=millis(); laps=nowm-Save; span=Itvl; String rt="";
//
  bool sw1=false;
  if(PinS1){
    bool f1=digitalRead(PinS1);
    long p1=millis();
    if(!f1){
      if(Sf1){
        Tc1++; if(false){Serial.println(Tc1);} Cnt1=0;
      }else{
        if(Cnt1>-1){Cnt1++;}
        if(Cnt1>NOISIE){
          long d1=p1-Sv1;
          if(d1>IGNORE){
            sw1=true; Sv1=p1;
            if(Debug){Serial.println("Sense1 cnt="+String(Cnt1));}
          }
          Cnt1=-1;
        }
      }
    }else{
      Cnt1=-1;
    }
    Sf1=f1;
  }
//
  bool sw2=false;
  if(PinS2){
    bool f2=digitalRead(PinS2);
    long p2=millis();
    if(!f2){
      if(Sf2){
        Tc2++; if(false){Serial.println(Tc2);} Cnt2=0;
      }else{
        if(Cnt2>-1){Cnt2++;}
        if(Cnt2>NOISIE){
          long d2=p2-Sv2;
          if(d2>IGNORE){sw2=true; Sv2=p2; if(Debug){Serial.println("Sense2");}}
          Cnt2=-1;
        }
      }
    }else{
      Cnt2=-1;
    }
    Sf2=f2;
  }
//
  if(sw2){
    int c2=millis()-RapG;
    if(c2>2000){
      if(Debug){Serial.println("sw2:"+String(sw2));}
      KmMotor::halt();
      return "";
    }
  }
//
  if(Next>Now){
    if(span<laps){Now++; KmMotor::analog(); Save=nowm;}
    return "";
  }
//
  if(Next<Now){
    if(span<laps){Now--; KmMotor::analog(); Save=nowm;}
    return "";
  }
//
  if(Que[0]>0){
    if(Que[2]>0){Que[2]=Que[2]-(nowm-Save);}
    else{Dir=Que[0]; Next=Que[1]; Que[0]=0; Que[1]=0; Que[2]=0;}
    Save=nowm;
    return "";
  }
//
  if(Cont>0){
    Cont=Cont-(nowm-Save); Save=nowm;
    return "";
  }
//
  if(Turn>0){
    int c1=millis()-RapT;
    if(sw1){
      if(c1>1000){
        Turn=Turn-1;
        if(Debug){Serial.println("Turn:"+String(Turn)+" sw1:"+String(sw1));}
      }
    }else{
      if(c1>LIMITTER){Turn=Turn-1;} // 暴走対策
    }
    return "";
  }
//
  if(Mx>0&&Nx<Mx){
    if(Debug){Serial.println("Nx:"+String(Nx)+" Mx:"+String(Mx)+" Cmd:"+Cmd[Nx]);}
    if(Cmd[Nx]==0){Next=0;}
    else if(Cmd[Nx]==3){Cont=Vol[Nx]*1000;}
    else if(Cmd[Nx]==4){Turn=Vol[Nx]; RapT=millis();}
    else if(Cmd[Nx]==5){KmMotor::point(Cmd[Nx]);}
    else if(Cmd[Nx]==6){KmMotor::point(Cmd[Nx]);}
    else if(Cmd[Nx]==7){rt="m"+String(Vol[Nx]);}
    else if(Cmd[Nx]==8){rt="s"+String(Vol[Nx]);}
    else if(Cmd[Nx]==9){RapG=millis();}
    else if(Cmd[Nx]==9){KmMotor::init(0, 0);}
    else{
      if(Debug){Serial.println("Dir:"+String(Cmd[Nx])+" Next:"+String(Vol[Nx]));}
      if(Dir==Cmd[Nx]){
        Dir=Cmd[Nx]; Next=Vol[Nx];
      }else{
        Next=0; Que[0]=Cmd[Nx]; Que[1]=Vol[Nx]; Que[2]=500;
      }
    }
    Save=nowm;
    Nx++;
  }else{
    Mx=0; Nx=0; if(Loop>0){KmMotor::pattern(Pttn); if(Loop<10000){Loop--;}}
  }
  return rt;
//
}
