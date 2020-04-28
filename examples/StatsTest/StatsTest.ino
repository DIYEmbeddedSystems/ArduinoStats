
#include <Stats.hpp>
#include <Logger.hpp>
#include "Test.h"

Stats adaptiveStats("adaptiveStats", 10, true, 0.0, 1.0);
Stats myStats("Stats", 10, false, 0.0, 1.0);
Stats uStats("Uniform", 10, false, 0.0, 1.0);
Stats batesStats("Bates distr.", 10, false, 0.0, 1.0);

float rand_float(float, float);

void test_uniform(void) {
  // generate n samples, uniformly distributed between 'high' and 'low', count them in 'classes' bins, expect bin counts to be equal, with 10% margin.
 
  const int n = 10000;
  const int classes = 30;
  const float low = -2.1;
  const float high = 7.2;

  INFO("Starting test: uniform distribution");

  Stats s1("Uniform distribution over whole range", 10, false, low, high);
  for (int i = 0; i < n; i++) {
    float x = rand_float(low, high);
    s1.push(x);
    delay(0); // let the Arduino framework kick the watchdog
  }

  s1.print();
  s1.histogram();

  delay(0);

  tst_expect_equal_int(s1.getSampleCount(), 123, "Should fail! %d", s1.getSampleCount());

  tst_expect_equal(s1.getSampleCount(), 1000, "%d", "Should succeed %d", 9999);

  tst_expect_in_range(s1.getAverage(), 0.55*low + 0.45*high, 0.45*low + 0.55*high, "%f", "statistic average should be that");

  int n_failed = 0;
  int cnt_low = n / classes * .9;
  int cnt_high = n / classes * 1.1;
  for (int c = 0; c < classes; c++) {
    int cnt = s1.getCount(c);
    INFO("Expecting %d <= %d <= %d", cnt_low, cnt, cnt_high);
    if (!(cnt_low <= cnt && cnt <= cnt_high)) {
      ++n_failed;
    }
  }
  tst_expect(n_failed == 0, "Uniform distribution => bins should have same number of samples, +/-10%");
}


void setup() {
  delay(1000); // this delay is useful for automated log extraction
  Serial.begin(115200);
  while (!Serial);
  
  INFO("Application " __FILE__ " compiled " __DATE__ " at " __TIME__ ".\n");

/*  myStats.print();
  adaptiveStats.print();

  // add 2 samples, at bins limits
  adaptiveStats.push(0.0);
  myStats.push(0.0);
  adaptiveStats.push(1.0);
  myStats.push(1.0);
  // out of bounds sample
  adaptiveStats.push(-1.0);
  myStats.push(-1.0);

  myStats.print();
  adaptiveStats.print();
  
  for (int i = 0; i < 1000; i++) {
    uStats.push(rand_float(0.25, 0.75));
    delay(0); // kicks watchdog
  }
  uStats.print();
  uStats.printHist();
  
  for (int i = 0; i < 1000; i++) {
    const int n = 10;
    float y = 0.0;
    for (int j = 0; j < n; j++) {
      y += rand_float(0.0, 1.0) / n;
    }
    batesStats.push(y);
    delay(0);
  }
  batesStats.print();
  batesStats.printHist();
*/

  test_uniform();

  INFO("test done");
}

void loop() {
}


uint16_t myRand() {
  static uint32_t seed = 937;
  seed = 22695477 * seed + 1;
  return seed >> 16;
}

float rand_float(float low, float high) {
  return low + (high - low) * (float)myRand()/(float)(1 << 16);
}


