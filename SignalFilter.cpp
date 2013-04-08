#include "SignalFilter.h"

/// Constructor
BaseFilter::BaseFilter()
{
  _v[0]=0;
  _v[1]=0;
  _v[2]=0;
  _helper=0;
  _counter=0;
//  _filter=0;
//  _order=0;
  _average=0;
  _median=0;
}

FilterChain::FilterChain() {
  filterCount = 0;
}

/**
 * @param num number of filters to chain
 */
FilterChain::FilterChain(int num) {
  filterCount = num;
  filters = new BaseFilter*[num];
}

/**
 * Add filter to chain
 *
 * @param pos position of the filter in the chain
 * @param ftr filter object
 */
void FilterChain::addFilter(int pos, BaseFilter *ftr) {
  if( (pos < 0) || (pos > filterCount-1) ) {
    return;
  } else {
    filters[pos] = ftr;
  }
}

/**
 * Pass value through the filter chain in order
 */
int FilterChain::filter(int data) {
  int retval = data;

  for(int i = 0; i < filterCount; i++) {
    retval = filters[i]->filter(retval);
  }

  return retval;
}


// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////
// FILTER IMPLEMENTATIONS
// ///////////////////////////////////////////////////////////////////////////
// ///////////////////////////////////////////////////////////////////////////

/// runChebyshev: Runs the actual filter: input=rawdata, output=filtered data
int ChebyshevFilter::filter(int data)
{
  //if(_order==1) {                                 //ripple -3dB
    _v[0] = _v[1];
    long tmp = ((((data * 3269048L) >>  2)        //= (3.897009118e-1 * data)
      + ((_v[0] * 3701023L) >> 3)                 //+(  0.2205981765*v[0])
      )+1048576) >> 21;                           // round and downshift fixed point /2097152
    _v[1]= (int)tmp;
    return (int)(_v[0] + _v[1]);                  // 2^
  //}
  /*
  if(_order==2) {                                 //ripple -1dB
    _v[0] = _v[1];
    _v[1] = _v[2];
    long tmp = ((((data * 662828L) >>  4)         //= (    7.901529699e-2 * x)
      + ((_v[0] * -540791L) >> 1)                 //+( -0.5157387562*v[0])
      + (_v[1] * 628977L)                         //+(  1.1996775682*v[1])
      )+262144) >> 19;                            // round and downshift fixed point /524288

    _v[2]= (int)tmp;
    return (int)((
      (_v[0] + _v[2])
      +2 * _v[1]));                               // 2^
  }
  else {
    return 0;
  }
  */
}

/// runBessel: Runs the actual filter: input=rawdata, output=filtered data
int BesselFilter::filter(int data)
{
  //if(_order==1) {                                 //Alpha Low 0.1
    _v[0] = _v[1];
    long tmp = ((((data * 2057199L) >>  3)        //= (    2.452372753e-1 * data)
      + ((_v[0] * 1068552L) >> 1)                 //+(  0.5095254495*v[0])
      )+524288) >> 20;                            // round and downshift fixed point /1048576
    _v[1]= (int)tmp;
    return (int)(((_v[0] + _v[1])));              // 2^
  //}
  /*
  if(_order==2) {                                 //Alpha Low 0.1
    _v[0] = _v[1];
    _v[1] = _v[2];
    long tmp = ((((data * 759505L) >>  4)         //= (    9.053999670e-2 * data)
      + ((_v[0] * -1011418L) >> 3)                //+( -0.2411407388*v[0])
      + ((_v[1] * 921678L) >> 1)                  //+(  0.8789807520*v[1])
      )+262144) >> 19;                            // round and downshift fixed point /524288

    _v[2]= (int)tmp;
    return (int)(((_v[0] + _v[2])+2 * _v[1]));    // 2^
  }
  else {
    return 0;
  }
  */
}

// Median filter (78 bytes, 12 microseconds)
int MedianFilter::filter(int data)
{
  // Note:
  //  quick & dirty dumb implementation that only keeps 3 samples: probably better to do insertion sort when more samples are needed in the calculation
  //   or Partial sort: http://en.cppreference.com/w/cpp/algorithm/nth_element
  // On better inspection of this code... performance seem quite good
  // TODO: compare with: http://embeddedgurus.com/stack-overflow/tag/median-filter/
  _v[0] = _v[1];
  _v[1] = _v[2];
  _v[2]= data;

  // printSamples();

  if (_v[2] < _v[1]) {
    if (_v[2] < _v[0]) {
      if (_v[1] < _v[0]) {
        _median = _v[1];
      }
      else {
        _median = _v[0];
      }
    }
    else {
      _median = _v[2];
    }
  }
  else {
    if (_v[2] < _v[0]) {
      _median = _v[2];
    }
    else {
      if (_v[1] < _v[0]) {
        _median = _v[0];
      }
      else {
        _median = _v[1];
      }
    }
  }
  return (_median);
}

// Growing-shrinking filter (fast)
int GrowFastFilter::filter(int data)
{
  if (data > _helper) {
    if (data > _helper+512)
      _helper=_helper+512;
    if (data > _helper+128)
      _helper=_helper+128;
    if (data > _helper+32)
      _helper=_helper+32;
    if (data > _helper+8)
      _helper=_helper+8;
    _helper++;
  }
  else if (data < _helper) {
    if (data < _helper-512)
      _helper=_helper-512;
    if (data < _helper-128)
      _helper=_helper-128;
    if (data < _helper-32)
      _helper=_helper-32;
    if (data < _helper-8)
      _helper=_helper-8;
    _helper--;
  }
  return _helper;
}

/// Growing-shrinking filter (smoother)
int GrowSmoothFilter::filter(int data)
{
  if (data > _helper) {
    if (data > _helper+8) {
      _counter++;
      _helper=_helper + 8 * _counter;
    }
    _helper++;
  }
  else if (data < _helper) {
    if (data < _helper-8) {
      _counter++;
      _helper=_helper- 8 * _counter;
    }
    _helper--;
  }

  if (_counter > 10) {
    _counter=0;
  }
  Serial.print(" counter: ");
  Serial.print(_counter);
  Serial.print("  ");
  return _helper;
}

