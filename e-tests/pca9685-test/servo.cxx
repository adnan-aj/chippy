/*
 * Author: Jon Trulson <jtrulson@ics.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <unistd.h>
#include <signal.h>
#include <iostream>
#include "pca9685.h"

using namespace std;

#define map(X, A, B, C, D) (((float)(X)-(float)(A))/((float)(B)-(float)(A)) * ((float)(D)-(float)(C)) + (float)(C))
// s = map(s, 0, 60, 0, 360);  // map the 0-60, to "360 degrees"

  upm::PCA9685 *leds = new upm::PCA9685(PCA9685_I2C_BUS, 
                                        PCA9685_DEFAULT_I2C_ADDR);

void setServo(int servo, int usec)
{
    int pwm;
    if ((servo < 0) || (servo > 15))
	return;
    pwm = map(usec, 1000, 2000, 205, 410);
    leds->ledOffTime(servo, pwm);
}



int main(int argc, char **argv)
{
//! [Interesting]
  // Instantiate an PCA9685 on I2C bus 0


  // put device to sleep
  leds->setModeSleep(true);

  // setup a period of 50Hz
  leds->setPrescaleFromHz(46);
  
  // wake device up
  leds->setModeSleep(false);

  // Setup a 50% duty cycle -- on time at 0, off time at 2048 (4096 / 2)
  // Set for all channels

  leds->ledOnTime(PCA9685_ALL_LED, 0);
  leds->ledOffTime(PCA9685_ALL_LED, 307); // 1500usec

 
  leds->ledOffTime(1, 205);
  leds->ledOffTime(2, 307);
  leds->ledOffTime(3, 410);

setServo(0, 800);
setServo(1, 1000);
setServo(2, 1500);
setServo(3, 2000);
setServo(4, 2400);

  // now, just sleep for 5 seconds, reset channels 3 and 4, and exit.
  cout << "Ctrl-C to exit..." << endl;

  int v = 0, dir = 1;
  while (1) {
      if (dir) {
	  v++;
	  if (v > 2200) {
	      dir = 0;
	      v = 2200;
	  }
      } else {
	  v--;
	  if (v <= 800) {
	      dir = 1;
	      v = 800;
	  }
      }
//      leds->ledOffTime(5, v);
	setServo(0, v);
	setServo(1, v);
	setServo(2, v);
	setServo(3, v);
	setServo(4, v);
	setServo(5, v);
	setServo(6, v);
	setServo(7, v);
      usleep(2000);
  }

  sleep(5);

  cout << "Exiting..." << endl;

  // clear the bits we set earlier
  leds->ledFullOff(2, false);
  leds->ledFullOn(3, false);

//! [Interesting]

  delete leds;
  return 0;
}
