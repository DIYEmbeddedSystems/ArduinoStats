/* 
 * Copyright DIYEmbeddedSystems 2020
 */

#include <ArduinoStats.h>

#include <Arduino.h>  // Serial.print(...)
#include <math.h>     // sqrt, floor
#include <float.h>    // FLT_sampleMax, FLT_sampleMin


Stats::Stats(const char *id, int N_bins, bool adaptive, float binsMin, float binsMax) 
    : _N_bins(N_bins)
    , _sampleMin(FLT_MAX), _sampleMax(-FLT_MAX)
    , _sampleCount(0)
    , _outOfRange(0)
    , _sum(0.0), _m(0.0), _m2(0.0)
    , _binsMin(binsMin), _binsMax(binsMax)
    , _adaptive(adaptive)
    , _id(id)
{
  _bin = reinterpret_cast<unsigned int*> (calloc(_N_bins, sizeof(unsigned int)));
  for (int i = 0; i < _N_bins; ++i)
  {
    _bin[i] = 0;
  }
}

void Stats::extendRange(float x)
{
  while (x >= _binsMax) 
  {
//    Serial.printf("%s; extending up\n", _id); 
    _binsMax = _binsMin + 2 * (_binsMax - _binsMin);
    // Special case for 1st bin: do not clear _bin[2*i]
    _bin[0] += _bin[1];
    for (int i = 1; i < _N_bins/2; ++i)
    {
      _bin[i] = _bin[2 * i] + _bin[2 * i + 1];
      _bin[2 * i] = 0;
      _bin[2 * i + 1] = 0;
    }
  }
  while (x < _binsMin) 
  {
//    Serial.printf("%s; extending down\n", _id);
    _binsMin = _binsMax - 2 * (_binsMax - _binsMin);
    _bin[_N_bins - 1] += _bin[_N_bins - 2];
    for (int i = 1; i < _N_bins/2; ++i)
    {
      _bin[_N_bins - 1 - i] = _bin[_N_bins - 1 - 2 * i] + _bin[_N_bins - 2 - 2 * i];
      _bin[_N_bins - 1 - 2 * i] = 0;
      _bin[_N_bins - 2 - 2 * i] = 0;
    }
  }
}


void Stats::push(float x)
{
  // Update statistical moments
  /* Update average and variance -- cf. https://en.wikipedia.org/wiki/Algorithms_for_calculating_variance#Online_algorithm */
  ++_sampleCount;
  _sum += x;
  float delta = x - _m;
  _m += delta / _sampleCount;
  float delta2 = x - _m;
  _m2 += delta * delta2;

  _sampleMin = (x < _sampleMin) ? x : _sampleMin;
  _sampleMax = (x > _sampleMax) ? x : _sampleMax;

  // Add sample to appropriate counting bin
  if (_adaptive)
  {
    extendRange(x);
  }

  if (x < _binsMin || x >= _binsMax)
  {
    Serial.printf("%s : sample %3.3f out of bins range (%3.3f - %3.3f)\n", _id, x, _binsMin, _binsMax);
    ++_outOfRange;
  }
  else
  {
    int idx = (x - _binsMin) * _N_bins / (_binsMax - _binsMin);

    float binLow, binHigh;
    binLow = _binsMin + idx * (_binsMax - _binsMin) / _N_bins;
    binHigh = _binsMin + (idx + 1) * (_binsMax - _binsMin) / _N_bins;

    //Serial.printf("%s ; sample %3.3f falls into bin %d (%3.3f - %3.3f)\n", _id, x, idx, binLow, binHigh);
    if (0 <= idx && idx < _N_bins)
    {
      ++_bin[idx];
    }
    else
    {
      Serial.print("Error ! Index out of range "); Serial.print(idx);      
    }            
  }
}

unsigned int Stats::getSampleCount()
{
  return _sampleCount; 
}

float Stats::getMin()
{
  return _sampleMin;
}


float Stats::getMax()
{
  return _sampleMax;
}


float Stats::getAverage()
{
  return (_sampleCount == 0) ? NAN : (_sum / _sampleCount);
}


float Stats::getVariance()
{
  return (_sampleCount < 2) ? NAN : (_m2 / _sampleCount);
}


float Stats::getStdDev()
{
  float var = getVariance();
  if (isnan(var))
  {
    return NAN;
  }
  else if (var >= 0.)
  {
    return std::sqrt(var);
  }
}

float Stats::getCount(unsigned int i)
{
  if (0 <= i && i < _N_bins) 
  {
    return _bin[i];
  }
  else
  {
    return _outOfRange;
  }
}

void Stats::print()
{
  Serial.printf("Stats %s has %d samples, min %3.3f to max %3.3f, ", 
      _id, _sampleCount, _sampleMin, _sampleMax);
  Serial.printf("average %3.3f, stdDev %3.3f \n",
      getAverage(), getStdDev());
  
/*  if (_sampleCount > 0) 
  {
    for (int i = 0; i < _N_bins; ++i)
    {
      float binLow = _binsMin + i * (_binsMax - _binsMin) / _N_bins;
      float binHigh = _binsMin + (i + 1) * (_binsMax - _binsMin) / _N_bins;
      Serial.printf("[%3.3f; %3.3f[ : \t%d samples \t(%3.3f%%) \n",
          binLow, binHigh, 
          _bin[i], 
          100. * _bin[i] / _sampleCount);
    }
    Serial.printf("out of range : %d\n\n", _outOfRange);
  }
*/
}

void Stats::histogram()
{
  Serial.printf("Stats %s has %d samples, min %3.3f to max %3.3f, ", 
      _id, _sampleCount, _sampleMin, _sampleMax);
  Serial.printf("average %3.3f, stdDev %3.3f \n",
      getAverage(), getStdDev());
  
  if (_sampleCount > 0) 
  {
    int max_count = 0;
    for (int i = 0; i < _N_bins; ++i) {
      if (_bin[i] > max_count) {
        max_count = _bin[i];
      }
    }
  
    for (int i = 0; i < _N_bins; ++i)
    {
      float binLow = _binsMin + i * (_binsMax - _binsMin) / _N_bins;
      float binHigh = _binsMin + (i + 1) * (_binsMax - _binsMin) / _N_bins;
      int barLength = 40 * _bin[i] / max_count;
      Serial.printf("[%3.3f; %3.3f[ : ", binLow, binHigh);
      for (int j = 0; j < 40; ++j) {
        Serial.print((j <= barLength) ? "#" : " ");
      }
      Serial.printf("  %d samples \t(%3.3f%%) \n",
          _bin[i], 100. * _bin[i] / _sampleCount);
    }
    Serial.printf("out of range : %d\n\n", _outOfRange);
  }
  else 
  {
    Serial.printf("No sample recorded yet.\n");
  }
}

