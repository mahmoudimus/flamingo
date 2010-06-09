/*
  $Id: simmetric.h 5149 2010-03-24 23:37:18Z abehm $

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

#ifndef _simmetric_h_
#define _simmetric_h_

#include "typedef.h"
#include "gramgen.h"
#include "compressionargs.h"
#include "filtertypes.h"

enum DistanceMeasure
{
  DM_EDIT_DISTANCE,
  DM_JACCARD,
  DM_COSINE
};

class SimMetric                 // abstract class
{
public:
  const GramGen& gramGen;
  const std::string name;

  SimMetric(const GramGen &gramGen, const std::string name): 
    gramGen(gramGen), name(name) 
  {}

  virtual ~SimMetric() 
  {};

  virtual float operator()(const std::string &s1, const std::string &s2)
    const = 0;

  virtual bool operator()(
    const std::string &s1, 
    const std::string &s2, 
    float threshold)
    const;

  // compute merging threshold
  virtual uint getMergeThreshold(
    const std::string& query, 
    const std::vector<uint>& queryGramCodes,
    const float simThreshold,
    const CompressionArgs* compressArgs = NULL) 
    const = 0;

  // functions used within holes compression (sampling techniques)
  // used for quickly getting threshold if number of holes is known
  // ATTENTION: part of compression techniques
  virtual uint getMergeThreshold(
    const std::string& query,
    const float simThreshold,
    const uint numberHoles)
    const = 0;
  
  // get filter bounds  
  virtual void getFilterBounds(
    const std::string& query,
    const float simThreshold,
    const AbstractFilter* filter,
    uint& lbound,
    uint& ubound)
    const = 0;  

  // bounds on similarity
  virtual float getSimMin(
    uint noGramsQuery, 
    uint noGramsData, 
    uint noGramsCommon) 
    const = 0;
 
  virtual float getSimMax(
    uint lenQuery, 
    uint noGramsQuery, 
    uint noGramsData, 
    uint noGramsCommon) 
    const = 0;
  
  // bound on common grams
  virtual uint getNoGramsMin(
    uint lenQuery, 
    uint noGramsMin, 
    uint noGramsQuery, 
    float sim)
    const = 0;

  // bound on common grams for multiple keyword search 
  virtual uint getNoGramsMax(
    uint lenQuery, 
    uint noGramsMin, 
    uint noGramsQuery, 
    float sim)
    const = 0;
};

class SimMetricEd: public SimMetric 
{
public:
  SimMetricEd(const GramGen &gramGen, const std::string name = "ed"): 
    SimMetric(gramGen, name) 
    {}

  virtual float operator()(const std::string &s1, const std::string &s2)
    const;

  virtual bool operator()(
    const std::string &s1, 
    const std::string &s2, 
    float threshold)
    const;

  // ATTENTION: part of compression techniques
  virtual uint getMergeThreshold(
    const std::string& query, 
    const std::vector<uint>& queryGramCodes,
    const float simThreshold,
    const CompressionArgs* compressArgs = NULL) 
    const;

  // ATTENTION: part of compression techniques
  virtual uint getMergeThreshold(
    const std::string& query,
    const float simThreshold,
    const uint numberHoles)
    const;

  virtual void getFilterBounds(
    const std::string& query,
    const float simThreshold,
    const AbstractFilter* filter,
    uint& lbound,
    uint& ubound)
    const;  
  
  virtual float getSimMin(
    uint noGramsQuery, 
    uint noGramsData, 
    uint noGramsCommon) 
    const;

  virtual float getSimMax(
    uint lenQuery, 
    uint noGramsQuery, 
    uint noGramsData, 
    uint noGramsCommon) 
    const;

  virtual uint getNoGramsMin(
    uint lenQuery, 
    uint noGramsMin, 
    uint noGramsQuery, 
    float sim)
    const;

  virtual uint getNoGramsMax(
    uint lenQuery, 
    uint noGramsMin, 
    uint noGramsQuery, 
    float sim)
    const;
};

class SimMetricEdNorm: public SimMetricEd
{
public:
  SimMetricEdNorm(const GramGen &gramGen, const std::string name = "ednorm"): 
    SimMetricEd(gramGen, name) 
  {}

  virtual float operator()(const std::string &s1, const std::string &s2)
    const;

  virtual bool operator()(
    const std::string &s1, 
    const std::string &s2, 
    float threshold)
    const;

  // ATTENTION: part of compression techniques
  virtual uint getMergeThreshold(
    const std::string& query, 
    const std::vector<uint>& queryGramCodes,
    const float simThreshold,
    const CompressionArgs* compressArgs = NULL) 
    const;

  // ATTENTION: part of compression techniques
  virtual uint getMergeThreshold(
    const std::string& query,
    const float simThreshold,
    const uint numberHoles)
    const;

  virtual void getFilterBounds(
    const std::string& query,
    const float simThreshold,
    const AbstractFilter* filter,
    uint& lbound,
    uint& ubound)
    const;  
  
  virtual float getSimMin(
    uint noGramsQuery, 
    uint noGramsData, 
    uint noGramsCommon) 
    const;

  virtual float getSimMax(
    uint lenQuery, 
    uint noGramsQuery, 
    uint noGramsData, 
    uint noGramsCommon) 
    const;

  virtual uint getNoGramsMin(
    uint lenQuery, 
    uint noGramsMin, 
    uint noGramsQuery, 
    float sim)
    const;

  virtual uint getNoGramsMax(
    uint lenQuery, 
    uint noGramsMin, 
    uint noGramsQuery, 
    float sim)
    const;
};

class SimMetricEdSwap: public SimMetric 
{
public:
  SimMetricEdSwap(const GramGen &gramGen, const std::string name = "edswap"): 
    SimMetric(gramGen, name) 
  {}

  virtual float operator()(const std::string &s1, const std::string &s2)
    const;

  virtual uint getMergeThreshold(
    const std::string& query, 
    const std::vector<uint>& queryGramCodes,
    const float simThreshold,
    const CompressionArgs* compressArgs = NULL) 
    const;

  // ATTENTION: part of compression techniques
  virtual uint getMergeThreshold(
    const std::string& query,
    const float simThreshold,
    const uint numberHoles)
    const;

  virtual void getFilterBounds(
    const std::string& query,
    const float simThreshold,
    const AbstractFilter* filter,
    uint& lbound,
    uint& ubound)
    const;  
  
  virtual float getSimMin(
    uint noGramsQuery, 
    uint noGramsData, 
    uint noGramsCommon) 
    const;

  virtual float getSimMax(
    uint lenQuery, 
    uint noGramsQuery, 
    uint noGramsData, 
    uint noGramsCommon) 
    const;

  virtual uint getNoGramsMin(
    uint lenQuery, 
    uint noGramsMin, 
    uint noGramsQuery, 
    float sim)
    const;

  virtual uint getNoGramsMax(
    uint lenQuery, 
    uint noGramsMin, 
    uint noGramsQuery, 
    float sim)
    const;
};

class SimMetricGram: public SimMetric // abstract class
{
public:
  SimMetricGram(const GramGen &gramGen, const std::string name): 
    SimMetric(gramGen, name) 
  {}

  virtual float operator()(
    uint noGramsData, 
    uint noGramsQuery, 
    uint noGramsCommon) 
    const = 0;

  virtual bool operator()(
    uint noGramsData, 
    uint noGramsQuery, 
    uint noGramsCommon, 
    float threshold)
    const 
  { return operator()(noGramsData, noGramsQuery, noGramsCommon) >= threshold; }

  virtual float getSimMin(
    uint noGramsQuery, 
    uint noGramsData, 
    uint noGramsCommon) 
    const 
  { return operator()(noGramsData, noGramsQuery, noGramsCommon); }

  virtual float getSimMax(
    uint lenQuery, 
    uint noGramsQuery, 
    uint noGramsData, 
    uint noGramsCommon) 
    const
  { return operator()(noGramsData, noGramsQuery, noGramsCommon); }
};

class SimMetricJacc: public SimMetricGram // set semantics !
{
public:
  SimMetricJacc(const GramGen &gramGen, const std::string name = "jc"): 
    SimMetricGram(gramGen, name) 
  {}

  virtual float operator()(const std::string &s1, const std::string &s2)
    const;

  virtual float operator()(
    uint noGramsData, 
    uint noGramsQuery, 
    uint noGramsCommon) 
    const;
  
  // compute merging threshold
  virtual uint getMergeThreshold(
    const std::string& query, 
    const std::vector<uint>& queryGramCodes,
    const float simThreshold,
    const CompressionArgs* compressArgs = NULL) 
    const; 

  // functions used within holes compression (sampling techniques)
  // used for quickly getting threshold if number of holes is known
  // ATTENTION: part of compression techniques
  virtual uint getMergeThreshold(
    const std::string& query,
    const float simThreshold,
    const uint numberHoles)
    const;

  // get filter bounds  
  virtual void getFilterBounds(
    const std::string& query,
    const float simThreshold,
    const AbstractFilter* filter,
    uint& lbound,
    uint& ubound)
    const;

  virtual uint getNoGramsMin(
    uint lenQuery, 
    uint noGramsMin, 
    uint noGramsQuery, 
    float sim)
    const;

  virtual uint getNoGramsMax(
    uint lenQuery, 
    uint noGramsMin, 
    uint noGramsQuery, 
    float sim)
    const;
};

class SimMetricCos: public SimMetricGram // set semantics !
{
public:
  SimMetricCos(const GramGen &gramGen, const std::string name = "cos"): 
    SimMetricGram(gramGen, name) 
  {}

  virtual float operator()(const std::string &s1, const std::string &s2)
    const;

  virtual float operator()(
    uint noGramsData, 
    uint noGramsQuery, 
    uint noGramsCommon) 
    const;

  virtual uint getMergeThreshold(
    const std::string& query, 
    const std::vector<uint>& queryGramCodes,
    const float simThreshold,
    const CompressionArgs* compressArgs = NULL) 
    const;
  
  // ATTENTION: part of compression techniques
  virtual uint getMergeThreshold(
    const std::string& query,
    const float simThreshold,
    const uint numberHoles)
    const;

  virtual void getFilterBounds(
    const std::string& query,
    const float simThreshold,
    const AbstractFilter* filter,
    uint& lbound,
    uint& ubound)
    const;

  virtual uint getNoGramsMin(
    uint lenQuery, 
    uint noGramsMin, 
    uint noGramsQuery, 
    float sim)
    const;

  virtual uint getNoGramsMax(
    uint lenQuery, 
    uint noGramsMin, 
    uint noGramsQuery, 
    float sim)
    const;
};

class SimMetricDice: public SimMetricGram // set semantics !
{
public:
  SimMetricDice(const GramGen &gramGen, const std::string name = "dice"): 
    SimMetricGram(gramGen, name) 
  {}

  virtual float operator()(const std::string &s1, const std::string &s2)
    const;

  virtual float operator()(
    uint noGramsData, 
    uint noGramsQuery, 
    uint noGramsCommon) 
    const;

  virtual uint getMergeThreshold(
    const std::string& query, 
    const std::vector<uint>& queryGramCodes,
    const float simThreshold,
    const CompressionArgs* compressArgs = NULL)
    const;

  // ATTENTION: part of compression techniques
  virtual uint getMergeThreshold(
    const std::string& query,
    const float simThreshold,
    const uint numberHoles)
    const;

  virtual void getFilterBounds(
    const std::string& query,
    const float simThreshold,
    const AbstractFilter* filter,
    uint& lbound,
    uint& ubound)
    const;

  virtual uint getNoGramsMin(
    uint lenQuery, 
    uint noGramsMin, 
    uint noGramsQuery, 
    float sim)
    const;

  virtual uint getNoGramsMax(
    uint lenQuery, 
    uint noGramsMin, 
    uint noGramsQuery, 
    float sim)
    const;
};

class SimMetricGramCount: public SimMetricGram 
{
public:
  SimMetricGramCount(const GramGen &gramGen, const std::string name = "cnt"): 
    SimMetricGram(gramGen, name) 
  {}
 
  virtual float operator()(const std::string &s1, const std::string &s2)
    const;

  virtual float operator()(
    uint noGramsData, 
    uint noGramsQuery, 
    uint noGramsCommon) 
    const;

  virtual uint getMergeThreshold(
    const std::string& query, 
    const std::vector<uint>& queryGramCodes,
    const float simThreshold,
    const CompressionArgs* compressArgs = NULL)
    const;

  // ATTENTION: part of compression techniques
  virtual uint getMergeThreshold(
    const std::string& query,
    const float simThreshold,
    const uint numberHoles)
    const;

  virtual void getFilterBounds(
    const std::string& query,
    const float simThreshold,
    const AbstractFilter* filter,
    uint& lbound,
    uint& ubound)
    const;

  virtual uint getNoGramsMin(
    uint lenQuery, 
    uint noGramsMin, 
    uint noGramsQuery, 
    float sim)
    const;

  virtual uint getNoGramsMax(
    uint lenQuery, 
    uint noGramsMin, 
    uint noGramsQuery, 
    float sim)
    const;
};

#endif
