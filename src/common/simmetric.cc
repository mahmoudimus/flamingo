/*
  $Id: simmetric.cc 5149 2010-03-24 23:37:18Z abehm $

  Copyright (C) 2010 by The Regents of the University of California

  Redistribution of this file is permitted under the terms of the
  BSD license.
  
  Some of the code in this file is part of compression techniques.
  Such parts are appropriately marked.
  Redistribution of the compression techniques is permitted under 
  the terms of the Academic BSD License.
  
  Date: 04/15/2008
  Author: Rares Vernica <rares (at) ics.uci.edu>, 
  Alexander Behm <abehm (at) ics.uci.edu>
*/

#include "simmetric.h"
#include "filtertypes.h"
#include "util/misc.h"

#include <cmath>
#include <cstring>
#include <iostream>

using namespace std;

// ------------------------------ SimMetric       ------------------------------

bool SimMetric::operator()(const string &s1, const string &s2, float threshold) 
  const 
{
  return operator()(s1, s2) >= threshold;
}

// ------------------------------ SimMetricEd     ------------------------------

float SimMetricEd::operator()(const string &s1, const string &s2) 
  const 
{  
  uint i, iCrt, iPre, j;
  uint
    n = s1.length(), 
    m = s2.length();

  if (n == 0)
    return m;
  if (m == 0)
    return n;

  uint* d[2];
  for (uint i = 0; i < 2; ++i)
    d[i] = new uint[m + 1];

  for (j = 0; j <= m; j++)
    d[0][j] = j;

  iCrt = 1;
  iPre = 0;
  for (i = 1; i <= n; i++) {
    d[iCrt][0] = i;
    for (j = 1; j <= m; j++)
      d[iCrt][j] = min(min(d[iPre][j] + 1, 
                           d[iCrt][j - 1] + 1), 
                       d[iPre][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1));
    iPre = !iPre;
    iCrt = !iCrt;
  }
  
  uint res = d[iPre][m];
  
  for (uint i = 0; i < 2; ++i)
    delete [] d[i];
  
  return res;
}

bool SimMetricEd::operator()(const string &s1, const string &s2, float threshold) 
  const 
{
  uint T = static_cast<uint>(threshold);

  uint i, j, ii, jj;
  uint
    n = s1.length(), 
    m = s2.length();

  if (n == 0)
    return m <= T;
  if (m == 0)
    return n <= T;
  if ((n > m && n - m > T) ||  
      (m > n &&  m - n > T))
    return false;

  uint **d;
  d = new uint*[n + 1];
  for (uint i = 0; i < n + 1; ++i)
    d[i] = new uint[m + 1];
  
  uint dmin, dmax = T + 1;

  for (i = 0; i <= n; i++)
    d[i][0] = i;
  for (j = 1; j <= m; j++)
    d[0][j] = j;

  for (ii = 1; ii <= n; ii++) {
    dmin = dmax;
    for (j = 1; j <= min(ii, m); j++) {
      i = ii - j + 1;
      d[i][j] = min(min(d[i - 1][j] + 1,
                        d[i][j - 1] + 1),
                    d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1));
      dmin = min(dmin, min(d[i][j], d[i - 1][j]));
    }
    if (dmin > T) {
      for (uint i = 0; i < n + 1; ++i)
	delete [] d[i];
      delete [] d;
      return false;
    }
  }
  
  for (jj = 2; jj <= m; jj++) {
    dmin = dmax;
    for (j = jj; j <= min(n + jj - 1, m); j++) {
      i = n - (j - jj);
      d[i][j] = min(min(d[i - 1][j] + 1,
                        d[i][j - 1] + 1),
                    d[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1));
      dmin = min(dmin, min(d[i][j], d[i - 1][j]));
    }

    if (dmin > T) {
      for (uint i = 0; i < n + 1; ++i)
	delete [] d[i];
      delete [] d;
      return false;
    }
  }

  bool res = d[n][m] <= T;
  
  for (uint i = 0; i < n + 1; ++i)
    delete [] d[i];
  delete [] d;

  return res;
}

// ATTENTION: part of compression techniques
uint SimMetricEd::getMergeThreshold(
  const string& query, 
  const vector<uint>& queryGramCodes, 
  const float simThreshold, 
  const CompressionArgs* compressArgs) 
  const 
{
  uint edThreshold = (uint)simThreshold;
  uint q = gramGen.getGramLength();
  uint numGrams = queryGramCodes.size();

  if(!compressArgs) return numGrams - (q * edThreshold); 
  else {
    // compression arguments set, analyze them
    // check to see if we have a blacklist
    if(compressArgs->blackList) {
      set<uint>* blackList = compressArgs->blackList;

      if(!compressArgs->holesOptimized) {
        // using holes compression but optimization is disabled
        // count the number of holes
        uint numberHoles = 0;
        for(uint i = 0; i < numGrams; i++)
          if( blackList->find( queryGramCodes.at(i )) != blackList->end() ) 
            numberHoles++;
	
        return numGrams - (edThreshold * q) - numberHoles;
      }
      else {
        // we have holes compression with optimization turned on
        // start dynamic programming algorithm
	
        // initialize dynamic programming matrix
        uint numberHoles = 0;
        uint imax = edThreshold + 1;
        uint jmax = numGrams + q;
        uint dpMatrix[imax][jmax];
        memset(*dpMatrix, 0, sizeof(uint)*imax*jmax);
	
        // initialize singlePosVector and count number of holes
        uint singlePosVector[numGrams];
        memset(singlePosVector, 0, sizeof(uint)*numGrams);
        uint killCounter = 0;
        for(uint i = 0; i < numGrams; i++) {
          if(i >= q) {      
            if( blackList->find( queryGramCodes.at(i - q) ) == blackList->end() ) 
              killCounter--;    
          }
	  
          uint gramCode = queryGramCodes.at(i);        
          if( blackList->find(gramCode) == blackList->end() ) killCounter++;
          else numberHoles++;
          singlePosVector[i] = killCounter;
        }
	
        // start dynamic programming algorithm
        for(uint i = 1; i < imax; i++) {
          for(uint j = q; j < jmax; j++) {
            uint a = dpMatrix[i][j-1];
            uint b = singlePosVector[j-q] + dpMatrix[i-1][j-q];     
            dpMatrix[i][j] = (a>b) ? a : b;
          }
        }
	
        return numGrams - numberHoles - dpMatrix[imax-1][jmax-1];
      }
    }
    else {
      // blackList is NULL, return default
      return numGrams - (q * edThreshold);
    }
  }
}

// ATTENTION: part of compression techniques
uint SimMetricEd::getMergeThreshold(
  const string& query, 
  const float simThreshold, 
  const uint numberHoles) 
  const 
{
  uint edThreshold = (uint)simThreshold;
  uint originalT = gramGen.getNumGrams(query) - 
    (gramGen.getGramLength() * edThreshold);
  return originalT - numberHoles;
}

void SimMetricEd::getFilterBounds(
  const string& query,
  const float simThreshold,
  const AbstractFilter* filter,
  uint& lbound,
  uint& ubound) 
  const 
{
  uint edThreshold = (uint)simThreshold;
  switch(filter->getType()) {
    
  case FT_LENGTH: {
    lbound = query.length() - edThreshold;
    if((signed)lbound <= 0) lbound = 0;
    ubound = query.length() + edThreshold;
  } break;
    
  case FT_CHARSUM: { 
    const CharsumFilter* csFilter = dynamic_cast<const CharsumFilter*>(filter);
    uint sum = csFilter->getCharsum(query);
    lbound = sum - (edThreshold * csFilter->getMaxChar());
    if((signed)lbound < 0) lbound = 0;
    ubound = sum + (edThreshold * csFilter->getMaxChar());
  } break;

  default: {
    cout << "UNKNOWN FILTER TYPE IN getFilterBounds" << endl;
    lbound = 0;
    ubound = 0;
  } break;

  }
}
  
float SimMetricEd::getSimMin(
  uint noGramsQuery, 
  uint noGramsData, 
  uint noGramsCommon) 
  const 
{
  cerr << "SimMetricEd::getSimMin Not Implemented" << endl; 
  exit(1); 
}

float SimMetricEd::getSimMax(
  uint lenQuery, 
  uint noGramsQuery, 
  uint noGramsData, 
  uint noGramsCommon) 
  const 
{
  return static_cast<float>(noGramsQuery - noGramsCommon) / 
    gramGen.getGramLength();
}

uint SimMetricEd::getNoGramsMin(
  uint lenQuery, 
  uint noGramsMin, 
  uint noGramsQuery, 
  float sim)
  const 
{
  float th = noGramsQuery - sim * gramGen.getGramLength();
  return th > 1 ? static_cast<uint>(floor(th)) : 1;
}

uint SimMetricEd::getNoGramsMax(
  uint lenQuery, 
  uint noGramsMin, 
  uint noGramsQuery, 
  float sim)
  const 
{
  return noGramsQuery + sim;
}

// ------------------------------ SimMetricEdNorm ------------------------------

float SimMetricEdNorm::operator()(const string &s1, const string &s2) 
  const 
{
  return 1.0f - static_cast<float>(SimMetricEd::operator()(s1, s2)) / 
    max(s1.length(), s2.length());
}

bool SimMetricEdNorm::operator()(const string &s1, const string &s2, float threshold) 
  const 
{  
  return SimMetricEd::operator()(s1, s2, ceil((1.0f - threshold) * max(s1.length(), s2.length())));
}

// ATTENTION: part of compression techniques
uint SimMetricEdNorm::getMergeThreshold(
  const string& query, 
  const vector<uint>& queryGramCodes,
  const float simThreshold,
  const CompressionArgs* compressArgs) 
  const 
{  
  float ed = ceil((1.0f - simThreshold) * query.length());
  return this->SimMetricEd::getMergeThreshold(query, queryGramCodes, ed, compressArgs);
}

// ATTENTION: part of compression techniques
uint SimMetricEdNorm::getMergeThreshold(
  const string& query,
  const float simThreshold,
  const uint numberHoles) 
  const 
{
  float ed = ceil((1.0f - simThreshold) * query.length());
  return this->SimMetricEd::getMergeThreshold(query, ed, numberHoles);  
}

void SimMetricEdNorm::getFilterBounds(
  const string& query,
  const float simThreshold,
  const AbstractFilter* filter,
  uint& lbound,
  uint& ubound) 
  const 
{
  float ed = ceil((1.0f - simThreshold) * query.length());
  this->SimMetricEd::getFilterBounds(query, ed, filter, lbound, ubound);
} 

float SimMetricEdNorm::getSimMin(
  uint noGramsQuery, 
  uint noGramsData, 
  uint noGramsCommon) 
  const 
{
  return 0;
}

float SimMetricEdNorm::getSimMax(
  uint lenQuery, 
  uint noGramsQuery, 
  uint noGramsData, 
  uint noGramsCommon) 
  const 
{
  return 1 - static_cast<float>(noGramsQuery - noGramsCommon) / 
    (gramGen.getGramLength() * lenQuery);
}

uint SimMetricEdNorm::getNoGramsMin(
  uint lenQuery, 
  uint noGramsMin, 
  uint noGramsQuery, 
  float sim)
  const 
{
  float th = noGramsQuery - (1 - sim) * lenQuery * gramGen.getGramLength(); 
  return th > 1 ? static_cast<uint>(floor(th)) : 1;
}

uint SimMetricEdNorm::getNoGramsMax(
  uint lenQuery, 
  uint noGramsMin, 
  uint noGramsQuery, 
  float sim)
  const 
{
  float th = 
    (noGramsQuery +  (1 - sim) * (1 -  gramGen.getGramLength())) / sim; 
  return th > 1 ? static_cast<uint>(ceil(th)) : 1;
}

// ------------------------------ SimMetricEdSwap ------------------------------

float SimMetricEdSwap::operator()(const string &s1, const string &s2) 
  const 
{
  uint i, iCrt, iPre, j;
  uint
    n = s1.length(), 
    m = s2.length();

  uint* d[2];
  for (uint i = 0; i < 2; ++i)
    d[i] = new uint[m + 1];
  
  for (j = 0; j <= m; j++)
    d[0][j] = j;

  iCrt = 1;
  iPre = 0;
  for (i = 1; i <= n; i++) {
    d[iCrt][0] = i;
    for (j = 1; j <= m; j++)
      d[iCrt][j] = min(min(d[iPre][j] + 1,
                           d[iCrt][j - 1] + 1),
                       d[iPre][j - 1] + ((s1[i - 1] == s2[j - 1] ||
                                          (i > 1 &&
                                           j > 1 &&
                                           s1[i - 1] == s2[j - 2] &&
                                           s1[i - 2] == s2[j - 1])) ? 0 : 1));
    iPre = !iPre;
    iCrt = !iCrt;
  }
  
  uint res = d[iPre][m];
  
  for (uint i = 0; i < 2; ++i)
    delete [] d[i];
  
  return res;
}

uint SimMetricEdSwap::getMergeThreshold(
  const string& query, 
  const vector<uint>& queryGramCodes,
  const float simThreshold,
  const CompressionArgs* compressArgs)
  const 
{
  cerr << "SimMetricEdSwap::getMergeThreshold Not Implemented" << endl;
  exit(1);
}

// ATTENTION: part of compression techniques
uint SimMetricEdSwap::getMergeThreshold(
  const string& query,
  const float simThreshold,
  const uint numberHoles) 
  const 
{
  cerr << "SimMetricEdSwap::getMergeThreshold Not Implemented" << endl;
  exit(1);
}

void SimMetricEdSwap::getFilterBounds(
  const string& query,
  const float simThreshold,
  const AbstractFilter* filter,
  uint& lbound,
  uint& ubound) 
  const 
{
  cerr << "SimMetricEdSwap::getFilterBounds Not Implemented" << endl;
  exit(1);
} 

float SimMetricEdSwap::getSimMin(
  uint noGramsQuery, 
  uint noGramsData, 
  uint noGramsCommon) 
  const 
{
  cerr << "SimMetricEdSwap::getSimMin Not Implemented" << endl;
  exit(1);
}

float SimMetricEdSwap::getSimMax(
  uint lenQuery, 
  uint noGramsQuery, 
  uint noGramsData, 
  uint noGramsCommon) 
  const 
{
  cerr << "SimMetricEdSwap::getSimMax Not Implemented" << endl;
  exit(1);
}

uint SimMetricEdSwap::getNoGramsMin(
  uint lenQuery, 
  uint noGramsMin, 
  uint noGramsQuery, 
  float sim)
  const 
{
  cerr << "SimMetricEdSwap::noGramsMin Not Implemented" << endl;
  exit(1);
}

uint SimMetricEdSwap::getNoGramsMax(
  uint lenQuery, 
  uint noGramsMin, 
  uint noGramsQuery, 
  float sim)
  const 
{
  cerr << "SimMetricEdSwap::getNoGramsMax Not Implemented" << endl; 
  exit(1); 
}

// ------------------------------ SimMetircGram   ------------------------------

// ------------------------------ SimMetricJacc   ------------------------------

float SimMetricJacc::operator()(const string &s1, const string &s2) 
  const 
{
  uint
    n = s1.length(), 
    m = s2.length();

  if (n == 0 || m == 0)
    return 0;

  set<uint> s1Gram, s2Gram, sUni;
  gramGen.decompose(s1, s1Gram);
  gramGen.decompose(s2, s2Gram);

  set_union(s1Gram.begin(), s1Gram.end(),
            s2Gram.begin(), s2Gram.end(), 
            inserter(sUni, sUni.begin()));
  
  uint interSize  = s1Gram.size() + s2Gram.size() - sUni.size();
  
  float d = static_cast<float>(interSize) / sUni.size();
  
  return d;
}
 
float SimMetricJacc::operator()(
  uint noGramsData, 
  uint noGramsQuery, 
  uint noGramsCommon) 
  const 
{
  return static_cast<float>(noGramsCommon) / 
    (noGramsQuery + noGramsData - noGramsCommon);
}
     
void SimMetricJacc::getFilterBounds(
  const string& query,
  const float simThreshold,
  const AbstractFilter* filter,
  uint& lbound,
  uint& ubound) 
  const 
{
  uint numGrams = gramGen.getNumGrams(query);
  uint gramLength = gramGen.getGramLength();
  switch(filter->getType()) {
    
  case FT_LENGTH: {
    lbound = (uint)floor((float)numGrams*simThreshold);
    ubound = (uint)ceil(((float)numGrams/simThreshold)); 
    if(gramGen.prePost) {
      lbound = lbound - gramLength + 1;
      ubound = ubound - gramLength + 1;
    }
    else {
      lbound = lbound + gramLength - 1;
      ubound = ubound + gramLength - 1;
    }
    if((signed)lbound < 0) lbound = 0;
  } break;
      
  case FT_CHARSUM: {
    const CharsumFilter* csFilter = dynamic_cast<const CharsumFilter*>(filter);
    uint queryChecksum = csFilter->getCharsum(query);
    uint minGrams = (uint)floor((float)numGrams*simThreshold);
    uint maxGrams = (uint)ceil(((float)numGrams/simThreshold)); 
    lbound = queryChecksum - ((numGrams - minGrams)*csFilter->getMaxChar());
    ubound = queryChecksum + ((maxGrams - numGrams)*csFilter->getMaxChar());
  } break;
      
  default: {
    lbound = 0;
    ubound = 0;
    cout << "WARNING: unknown filter passed to distancemeasure." << endl;
  } break;
      
  }   
}

uint SimMetricJacc::getMergeThreshold(
  const string& query, 
  const vector<uint>& queryGramCodes,
  const float simThreshold,
  const CompressionArgs* compressArgs) 
  const 
{
  uint numGrams = queryGramCodes.size();
  if(!compressArgs) return (uint)floor(simThreshold*(numGrams));
  else {
    // compression args are set
    if(compressArgs->blackList) {
      set<uint>* blackList = compressArgs->blackList;      
      for(uint i = 0; i < numGrams; i++) { 
        uint gramCode = queryGramCodes.at(i);
        if(blackList->find(gramCode) != blackList->end())
          numGrams--;    
      } 
	
      return (uint)floor(simThreshold*numGrams); 
    }
    else {
      // compression but no holes compression, so return default
      return (uint)floor(simThreshold*(numGrams));
    }
  }
}
 
// ATTENTION: part of compression techniques
uint SimMetricJacc::getMergeThreshold(
  const string& query,
  const float simThreshold,
  const uint numberHoles) 
  const 
{
  return (uint)floor(simThreshold * gramGen.getNumGrams(query) -  numberHoles);
}

uint SimMetricJacc::getNoGramsMin(
  uint lenQuery, 
  uint noGramsMin, 
  uint noGramsQuery, 
  float sim)
  const 
{
  float th = max(sim * noGramsQuery,
                 (noGramsQuery + noGramsMin) / (1 + 1 / sim));
  return th > 1 ? static_cast<uint>(ceil(th)) : 1;
}

uint SimMetricJacc::getNoGramsMax(
  uint lenQuery, 
  uint noGramsMin, 
  uint noGramsQuery, 
  float sim)
  const 
{
  return noGramsQuery / sim;
}

// ------------------------------ SimMetricCos    ------------------------------

float SimMetricCos::operator()(const string &s1, const string &s2) 
  const 
{
  uint
    n = s1.length(), 
    m = s2.length();

  if (n == 0 || m == 0)
    return 0;

  set<uint> s1Gram, s2Gram, sInt;
  gramGen.decompose(s1, s1Gram);
  gramGen.decompose(s2, s2Gram);

  set_intersection(s1Gram.begin(), s1Gram.end(),
                   s2Gram.begin(), s2Gram.end(), 
                   inserter(sInt, sInt.begin()));
  
  float d =  static_cast<float>(sInt.size()) / 
    sqrt(static_cast<float>(s1Gram.size() * s2Gram.size()));
  
  return d;
}
      
float SimMetricCos::operator()(
  uint noGramsData, 
  uint noGramsQuery, 
  uint noGramsCommon) 
  const 
{
  return noGramsCommon / sqrt(static_cast<float>(noGramsQuery * noGramsData));
}
     
uint SimMetricCos::getMergeThreshold(
  const string& query, 
  const vector<uint>& queryGramCodes,
  const float simThreshold,
  const CompressionArgs* compressArgs)
  const 
{

  // uint gramLength = gramGen.getGramLength();
  uint numGrams = queryGramCodes.size();
  if(!compressArgs) return (uint)floor(simThreshold*simThreshold*numGrams); 
  else {
    // compression args are set
    if(compressArgs->blackList) {
      set<uint>* blackList = compressArgs->blackList;      
      for(uint i = 0; i < numGrams; i++) {     
        uint gramCode = queryGramCodes.at(i);	  
        if(blackList->find(gramCode) != blackList->end())
          numGrams--;    	  
      } 	
      return (uint)floor(simThreshold*simThreshold*numGrams); 
    }
    else {
      // compression but no holes compression, so return default
      return (uint)floor(simThreshold*simThreshold*numGrams); 
    }
  }    
}
  
// ATTENTION: part of compression techniques
uint SimMetricCos::getMergeThreshold(
  const string& query,
  const float simThreshold,
  const uint numberHoles) 
  const 
{
  return (uint)floor(simThreshold*simThreshold*gramGen.getNumGrams(query) - 
                         numberHoles);   
}

void SimMetricCos::getFilterBounds(
  const string& query,
  const float simThreshold,
  const AbstractFilter* filter,
  uint& lbound,
  uint& ubound) 
  const 
{
    
  uint numGrams = gramGen.getNumGrams(query);
  uint gramLength = gramGen.getGramLength();
    
  switch(filter->getType()) {
      
  case FT_LENGTH: {
    lbound = (uint)floor((float)numGrams*simThreshold*simThreshold - gramLength + 1);
    ubound = (uint)ceil(((float)numGrams/(simThreshold*simThreshold)) - gramLength + 1);
    if ((signed)lbound < 0) lbound = 0;
  } break;
      
  case FT_CHARSUM: {
    const CharsumFilter* csFilter = dynamic_cast<const CharsumFilter*>(filter);
    uint queryChecksum = csFilter->getCharsum(query);
    uint minGrams = (uint)floor((float)numGrams*simThreshold*simThreshold);
    uint maxGrams = (uint)ceil((float)numGrams/(simThreshold*simThreshold));
    lbound = queryChecksum - ((numGrams - minGrams)*csFilter->getMaxChar()*gramLength);
    ubound = queryChecksum + ((maxGrams - numGrams)*csFilter->getMaxChar()*gramLength);
  } break;
      
  default: {
    lbound = 0;
    ubound = 0;
    cout << "WARNING: unknown filter passed to distancemeasure." << endl;
  } break;
      
  }
}

uint SimMetricCos::getNoGramsMin(
  uint lenQuery, 
  uint noGramsMin, 
  uint noGramsQuery, 
  float sim)
  const 
{
  float th = sim * sqrt(static_cast<float>(noGramsQuery * noGramsMin));
  return th > 1 ? static_cast<uint>(ceil(th)) : 1;
}

uint SimMetricCos::getNoGramsMax(
  uint lenQuery, 
  uint noGramsMin, 
  uint noGramsQuery, 
  float sim)
  const 
{
  cerr << "SimMetricCos::getNoGramsMax Not Implemented" << endl; 
  exit(1); 
}

// ------------------------------ SimMetricDice   ------------------------------

float SimMetricDice::operator()(const string &s1, const string &s2) 
  const 
{
  uint
    n = s1.length(), 
    m = s2.length();

  if (n == 0 || m == 0)
    return 0;

  set<uint> s1Gram, s2Gram, sInt;
  gramGen.decompose(s1, s1Gram);
  gramGen.decompose(s2, s2Gram);

  set_intersection(s1Gram.begin(), s1Gram.end(),
                   s2Gram.begin(), s2Gram.end(), 
                   inserter(sInt, sInt.begin()));
  
  float d =  2. * sInt.size() / (s1Gram.size() + s2Gram.size());
  
  return d;
}
      
float SimMetricDice::operator()(
  uint noGramsData, 
  uint noGramsQuery, 
  uint noGramsCommon) 
  const 
{
  cerr << "SimMetricDice::operator(noGramsData, noGramQuery, noGramsCommon) "
       << "Not Implemented" << endl;
  exit(1);
}
     
uint SimMetricDice::getMergeThreshold(
  const string& query, 
  const vector<uint>& queryGramCodes,
  const float simThreshold,
  const CompressionArgs* compressArgs)
  const 
{
  cerr << "SimMetricEdNorm::getMergeThreshold Not Implemented" << endl;
  exit(1);
}
 
// ATTENTION: part of compression techniques
uint SimMetricDice::getMergeThreshold(
	const string& query,
  const float simThreshold,
  const uint numberHoles) 
  const 
{
  cerr << "SimMetricEdNorm::getMergeThreshold Not Implemented" << endl;
  exit(1);
}

void SimMetricDice::getFilterBounds(
  const string& query,
  const float simThreshold,
  const AbstractFilter* filter,
  uint& lbound,
  uint& ubound) 
  const 
{
  uint numGrams = gramGen.getNumGrams(query);
  uint gramLength = gramGen.getGramLength();
  switch(filter->getType()) {
    
  case FT_LENGTH: {
    lbound = (uint)floor( ((float)numGrams*simThreshold) / (2.0f - simThreshold) );
    ubound = (uint)ceil( ((2.0f - simThreshold) * numGrams) / simThreshold);
    if(gramGen.prePost) {
      lbound = lbound - gramLength + 1;
      ubound = ubound - gramLength + 1;
    }
    else {
      lbound = lbound + gramLength - 1;
      ubound = ubound + gramLength - 1;
    }
    if((signed)lbound < 0) lbound = 0;
  } break;
    
  case FT_CHARSUM: {
    const CharsumFilter* csFilter = dynamic_cast<const CharsumFilter*>(filter);
    uint queryChecksum = csFilter->getCharsum(query);
    uint minGrams = (uint)floor( ((float)numGrams*simThreshold) / (2.0f - simThreshold) );
    uint maxGrams = (uint)ceil( ((2.0f - simThreshold) * numGrams) / simThreshold);
    lbound = queryChecksum - ((numGrams - minGrams)*csFilter->getMaxChar()*gramLength);
    ubound = queryChecksum + ((maxGrams - numGrams)*csFilter->getMaxChar()*gramLength);
  } break;
      
  default: {
    lbound = 0;
    ubound = 0;
    cout << "WARNING: unknown filter passed to distancemeasure." << endl;
  } break;
      
  }   
}

uint SimMetricDice::getNoGramsMin(
  uint lenQuery, 
  uint noGramsMin, 
  uint noGramsQuery, 
  float sim)
  const 
{
  cerr << "SimMetricDice::noGramsMin Not Implemented" << endl;
  exit(1);
}

uint SimMetricDice::getNoGramsMax(
  uint lenQuery, 
  uint noGramsMin, 
  uint noGramsQuery, 
  float sim)
  const 
{
  cerr << "SimMetricDice::getNoGramsMax Not Implemented" << endl; 
  exit(1); 
}

// ------------------------------ SimGramCount    ------------------------------

float SimMetricGramCount::operator()(
  const string &s1, 
  const string &s2) 
  const 
{
  uint
    n = s1.length(), 
    m = s2.length();

  if (n == 0 || m == 0)
    return 0;
  
  set<uint> s1Gram, s2Gram, sInt;
  gramGen.decompose(s1, s1Gram);
  gramGen.decompose(s2, s2Gram);

  set_intersection(s1Gram.begin(), s1Gram.end(),
                   s2Gram.begin(), s2Gram.end(), 
                   inserter(sInt, sInt.begin()));
  
  return sInt.size();
}
      
float SimMetricGramCount::operator()(
  uint noGramsData, 
  uint noGramsQuery, 
  uint noGramsCommon) 
  const 
{
  return noGramsCommon;
}
     
uint SimMetricGramCount::getMergeThreshold(
  const string& query, 
  const vector<uint>& queryGramCodes,
  const float simThreshold,
  const CompressionArgs* compressArgs) 
  const 
{
  cerr << "SimMetricGramCount::getMergeThreshold Not Implemented" << endl;
  exit(1);
}

// ATTENTION: part of compression techniques
uint SimMetricGramCount::getMergeThreshold(
  const string& query,
  const float simThreshold,
  const uint numberHoles) 
  const 
{
  cerr << "SimMetricGramCount::getMergeThreshold Not Implemented" << endl;
  exit(1);
}

void SimMetricGramCount::getFilterBounds(
  const string& query,
  const float simThreshold,
  const AbstractFilter* filter,
  uint& lbound,
  uint& ubound) 
  const 
{
  cerr << "SimMetricGramCount::getFilterBounds Not Implemented" << endl;
  exit(1);
}

uint SimMetricGramCount::getNoGramsMin(
  uint lenQuery, 
  uint noGramsMin, 
  uint noGramsQuery, 
  float sim)
  const 
{
  return static_cast<uint>(sim);
}

uint SimMetricGramCount::getNoGramsMax(
  uint lenQuery, 
  uint noGramsMin, 
  uint noGramsQuery, 
  float sim)
  const 
{
  cerr << "SimMetricGramCount::getNoGramsMax Not Implemented" << endl; 
  exit(1); 
}
