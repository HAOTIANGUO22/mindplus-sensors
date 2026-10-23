#include <assert.h>
#include <stdio.h>
#include <limits.h>
#include "MindPlusRotaryEncoder.h"
uint8_t fakePins[256];
uint32_t fakeMillis = 0;
void (*fakeInterrupts[2])() = {0,0};
static void state(int bits) {
    fakePins[2] = (bits >> 1) & 1; fakePins[3] = bits & 1;
    if (fakeInterrupts[0]) fakeInterrupts[0]();
    if (fakeInterrupts[1]) fakeInterrupts[1]();
}
static void forward() { state(1); state(0); state(2); state(3); }
static void backward() { state(2); state(0); state(1); state(3); }
int main() {
    for (int i=0;i<256;++i) fakePins[i] = HIGH;
    MindPlusRotaryEncoder e;
    assert(!e.ready()); assert(!e.pressed()); assert(!e.clicked());
    assert(!e.begin(2,2,4)); assert(!e.begin(4,5,6)); assert(!e.begin(2,3,2));
    assert(e.begin(2,3,4)); assert(e.ready());
    forward(); assert(e.read()==1 && e.direction()==1);
    assert(e.takeDelta()==1 && e.takeDelta()==0);
    backward(); assert(e.read()==0 && e.direction()==-1);
    // Contact chatter and reversal within a partial step must cancel.
    e.write(0); state(1); state(3); state(1); state(0); state(1); state(0); state(2); state(3);
    assert(e.read()==1);
    e.write(0); state(1); state(0); state(1); state(3); assert(e.read()==0);
    // Illegal two-bit jumps resynchronize without manufacturing a count.
    state(0); state(3); assert(e.read()==0); forward(); assert(e.read()==1);
    e.setReverse(true); forward(); assert(e.read()==0 && e.direction()==-1);
    e.setReverse(false); e.write(0); e.setTransitions(2); forward(); assert(e.read()==2);
    e.write(0); e.setTransitions(1); forward(); assert(e.read()==4);
    e.setTransitions(3); forward(); assert(e.read()==8); // Invalid setting ignored.
    e.setTransitions(4); e.write(INT32_MAX); forward(); assert(e.read()==INT32_MAX);
    e.write(INT32_MIN); backward(); assert(e.read()==INT32_MIN);
    e.write(-123); assert(e.read()==-123 && e.takeDelta()==0 && e.direction()==0);
    // Button bounce, hold, release, and one-shot consumption.
    fakePins[4]=LOW; e.update(); fakeMillis=10; assert(!e.pressed());
    fakePins[4]=HIGH; e.update(); fakeMillis=11; fakePins[4]=LOW; e.update();
    fakeMillis=30; assert(!e.clicked()); fakeMillis=31; assert(e.clicked());
    assert(e.pressed() && !e.clicked()); fakeMillis=1000; assert(!e.clicked());
    fakePins[4]=HIGH; e.update(); fakeMillis=1020; assert(!e.pressed());
    // millis wraparound still allows debounce.
    fakeMillis=UINT32_MAX-10; fakePins[4]=LOW; e.update();
    fakeMillis=10; assert(e.clicked());
    MindPlusRotaryEncoder other;
    assert(!other.begin(2,3,5)); forward(); assert(e.read()==-122);
    e.end(); assert(!e.ready() && !e.pressed());
    assert(e.begin(2,3,-1)); assert(!e.pressed() && !e.clicked());
    forward(); assert(e.read()==1);
    assert(!e.begin(2,3,99)); assert(!e.ready());
    assert(!fakeInterrupts[0] && !fakeInterrupts[1]);
    puts("PASS: quadrature, bounce, reverse, invalid states, scaling, saturation, reset, button debounce/wrap, ownership and reinit");
}
