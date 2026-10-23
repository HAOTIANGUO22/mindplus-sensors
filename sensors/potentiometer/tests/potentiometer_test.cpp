#include <assert.h>
#include <math.h>
#include <stdio.h>
#include "MindPlusPotentiometer.h"
int fakeValue=0,readCalls=0,setBits=0,attenuationPin=-1,lastInputPin=-1;
int values[40],valuesCount=0,valuesIndex=0;
static void near(double value,double expected) { assert(fabs(value-expected)<0.00001); }
int main() {
    MindPlusPotentiometer p;
#ifdef ESP32
    const int valid[]={33,32,35,34}; const int invalid[]={-1,0,2,14,25,36,39,256};
    assert(p.maximum()==4095);
#else
    const int valid[]={14,15,16,17,18,19}; const int invalid[]={-1,0,2,13,20,33,256};
    assert(p.maximum()==1023);
#endif
    assert(!p.ready()); assert(p.raw()==0 && p.percent()==0 && readCalls==0); near(p.scaled(-10,10),-10);
    for(unsigned i=0;i<sizeof(invalid)/sizeof(invalid[0]);++i) assert(!p.begin(invalid[i]));
    for(unsigned i=0;i<sizeof(valid)/sizeof(valid[0]);++i) { assert(p.begin(valid[i]) && p.ready()); assert(lastInputPin==valid[i]); }
    assert(p.begin(valid[0]));
    fakeValue=0; assert(p.percent()==0); near(p.scaled(-20,80),-20);
    fakeValue=p.maximum(); assert(p.percent()==100); near(p.scaled(-20,80),80);
    assert(p.setSamples(32)); readCalls=0; assert(p.raw()==p.maximum() && readCalls==33);
    assert(p.setSamples(4));
    valuesCount=5; valuesIndex=0; values[0]=999; values[1]=0; values[2]=10; values[3]=20; values[4]=30;
    assert(p.raw()==15); assert(valuesIndex==5); valuesCount=0;
    assert(!p.setSamples(0)); assert(!p.setSamples(3)); assert(!p.setSamples(256));
    readCalls=0; p.raw(); assert(readCalls==5);
    assert(p.calibrate(100,900)); fakeValue=500; assert(p.percent()==50);
    near(p.scaled(0,255),127.5); near(p.scaled(1.5,-2.5),-0.5);
    near(p.scaled(7,7),7);
    fakeValue=0; assert(p.percent()==0); fakeValue=p.maximum(); assert(p.percent()==100);
    assert(!p.calibrate(-1,900)); assert(!p.calibrate(200,200));
    assert(!p.calibrate(900,100)); assert(!p.calibrate(0,p.maximum()+1));
    fakeValue=500; assert(p.percent()==50); // Invalid calibration kept previous range.
    p.setReverse(true); fakeValue=100; assert(p.percent()==100 && p.raw()==100);
    fakeValue=900; assert(p.percent()==0); near(p.scaled(20,100),20);
    p.resetCalibration(); fakeValue=p.maximum(); assert(p.percent()==0);
    assert(p.begin(valid[0])); fakeValue=p.maximum(); assert(p.percent()==100);
    readCalls=0; p.raw(); assert(readCalls==9); // Re-init restores eight samples.
    fakeValue=-5; assert(p.raw()==0); fakeValue=p.maximum()+20; assert(p.raw()==p.maximum());
#ifdef ESP32
    setBits=8; attenuationPin=-1; p.raw(); assert(setBits==12 && attenuationPin==valid[0]);
#endif
    assert(!p.begin(invalid[0])); assert(!p.ready()); readCalls=0; assert(p.raw()==0 && readCalls==0);
    puts("PASS: board pins/range, averaging/discard, endpoints, mapping, reverse, invalid calibration, clamping, re-init, ADC setup");
}
