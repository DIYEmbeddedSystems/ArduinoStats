/*
 * Copyright DIYEmbeddedSystems 2020
 */

#ifndef STATS_HPP
#define STATS_HPP

class Stats
{
public:
  Stats(const char *id, int N_bins, bool adaptive, float binsMin, float binsMax);
  void push(float x);
  
  unsigned int getSampleCount();
  float getMin();
  float getMax();
  float getAverage();
  float getVariance();
  float getStdDev();
  float getCount(unsigned int);
  void print();
  void histogram();

private:
  void extendRange(float);
  
  unsigned int _N_bins;
  unsigned int *_bin;
  unsigned int _outOfRange;
  unsigned int _sampleCount;
  bool _adaptive;

  float _binsMin, _binsMax;
  float _sampleMin, _sampleMax;
  float _sum, _m, _m2;

  const char *_id;
};


#endif // STATS_HPP
