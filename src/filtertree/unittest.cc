/*
  $Id: unittest.cc 5146 2010-03-24 23:05:57Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license.
    
  Date: 04/04/2008
  Author: Alexander Behm
*/

#include "ftindexerholesglobal.h"
#include "ftindexerunionglobal.h"
#include "ftsearchermem.h"
#include "common/query.h"
#include "common/simmetric.h"
#include "listmerger/divideskipmerger.h"
#include "listmerger/scancountmerger.h"
#include <fstream>
#include <stdio.h>

using namespace std;
using namespace tr1;

// ASSUMPTIONS: uncompressed indexer with no filters delivers correct results
// it will be used to fill expectedResults

// global vars for performing unittests
StringContainerVector strContainer;
vector<Query*> queries;
vector<string> queryStrings;
vector<unsigned> expectedResults;
GramGenFixedLen gramGen(3);
SimMetricEd simMetric(gramGen);

void init();
void deinit();
bool compareResults(vector<unsigned>& results, const string& identifier);

bool testFtIndexerMem();
bool testFtIndexerHolesGlobalLLF();
bool testFtIndexerHolesGlobalSLF();
bool testFtIndexerHolesGlobalRandom();
bool testFtIndexerHolesGlobalTimeCost();
bool testFtIndexerHolesGlobalTimeCostPlus();
bool testFtIndexerHolesGlobalPanicCost();
bool testFtIndexerHolesGlobalPanicCostPlus();
bool testFtIndexerUnionGlobalBasic();
bool testFtIndexerUnionGlobalCost();

int main() {
  init();

  bool passed = false;

  cout << "NOTE: THESE TESTS MAY TAKE SOME MINUTES, PLEASE BE PATIENT" << endl << endl;

  cout << "TEST FtIndexerMem:" << endl;
  passed = testFtIndexerMem();  
  if(passed) cout << "--- PASSED ---" << endl;
  else cout << "--- FAILED ---" << endl;
  cout << endl;

  cout << "TEST FtIndexerHolesGlobalLLF:" << endl;  
  passed = testFtIndexerHolesGlobalLLF();
  if(passed) cout << "--- PASSED ---" << endl;
  else cout << "--- FAILED ---" << endl;  
  cout << endl;
  
  cout << "TEST FtIndexerHolesGlobalSLF:" << endl;  
  passed = testFtIndexerHolesGlobalSLF();
  if(passed) cout << "--- PASSED ---" << endl;
  else cout << "--- FAILED ---" << endl;  
  cout << endl;

  cout << "TEST FtIndexerHolesGlobalRandom:" << endl;  
  passed = testFtIndexerHolesGlobalRandom();
  if(passed) cout << "--- PASSED ---" << endl;
  else cout << "--- FAILED ---" << endl;  
  cout << endl;

  cout << "TEST FtIndexerHolesGlobalTimeCost:" << endl;  
  passed = testFtIndexerHolesGlobalTimeCost();
  if(passed) cout << "--- PASSED ---" << endl;
  else cout << "--- FAILED ---" << endl;  
  cout << endl;

  cout << "TEST FtIndexerHolesGlobalTimeCostPlus:" << endl;  
  passed = testFtIndexerHolesGlobalTimeCostPlus();
  if(passed) cout << "--- PASSED ---" << endl;
  else cout << "--- FAILED ---" << endl;  
  cout << endl;

  cout << "TEST FtIndexerHolesGlobalPanicCost:" << endl;  
  passed = testFtIndexerHolesGlobalPanicCost();
  if(passed) cout << "--- PASSED ---" << endl;
  else cout << "--- FAILED ---" << endl;  
  cout << endl;

  cout << "TEST FtIndexerHolesGlobalPanicCostPlus:" << endl;  
  passed = testFtIndexerHolesGlobalPanicCostPlus();
  if(passed) cout << "--- PASSED ---" << endl;
  else cout << "--- FAILED ---" << endl;  
  cout << endl;

  cout << "TEST FtIndexerUnionGlobalBasic:" << endl;  
  passed = testFtIndexerUnionGlobalBasic();
  if(passed) cout << "--- PASSED ---" << endl;
  else cout << "--- FAILED ---" << endl;  
  cout << endl;

  cout << "TEST FtIndexerUnionGlobalCost:" << endl;  
  passed = testFtIndexerUnionGlobalCost();
  if(passed) cout << "--- PASSED ---" << endl;
  else cout << "--- FAILED ---" << endl;  
  cout << endl;

  deinit();
  return 0;
}

void init() {
  cout << "INITIALIZING UNITTEST" << endl;

  vector<string> prefixes;
  prefixes.push_back("string");
  prefixes.push_back("example");  
  prefixes.push_back("test");
  prefixes.push_back("hello");
  prefixes.push_back("world");
  prefixes.push_back("foo");
  prefixes.push_back("bar");
  
  vector<string> suffixes;
  suffixes.push_back("1");
  suffixes.push_back("10");
  suffixes.push_back("100");
  suffixes.push_back("2");
  suffixes.push_back("20");
  suffixes.push_back("200");
  suffixes.push_back("3");
  suffixes.push_back("30");
  suffixes.push_back("300");
  
  for(unsigned j = 0; j < prefixes.size(); j++)
    for(unsigned i = 0; i < suffixes.size(); i++)
      strContainer.insertString(prefixes.at(j) + suffixes.at(i));
  
  // create queries
  queries.push_back(new Query("xample", simMetric, 2.0f));
  queries.push_back(new Query("ring1", simMetric, 2.0f));
  queries.push_back(new Query("wrld", simMetric, 2.0f));
  queries.push_back(new Query("fooa", simMetric, 2.0f));
  queries.push_back(new Query("br", simMetric, 2.0f));  

  for(unsigned i = 0; i < 10; i++) {
    queryStrings.push_back("xample");
    queryStrings.push_back("ring1");
    queryStrings.push_back("wrld");
    queryStrings.push_back("fooa");
    queryStrings.push_back("br");
  }

  // execute queries on uncompressed index without filters to get expected results
  FtIndexerMem<> indexer(&strContainer, &gramGen);
  indexer.buildIndex(false);      
  DivideSkipMerger<> merger;
  FtSearcherMem<> searcher(&merger, &indexer);

  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
    searcher.search(**iter, expectedResults);
    
  // sort expected results
  sort(expectedResults.begin(), expectedResults.end());
  
  cout << "UNITTEST INITIALIZED" << endl << endl;
}

void deinit() {
  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
    delete *iter;  
}

bool compareResults(vector<unsigned>& results, const string& identifier) {
  // compare results
  sort(results.begin(), results.end());
  if(results.size() != expectedResults.size()) {
    cout << "FAILED IN " << identifier << endl;
    return false;
  }

  for(unsigned i = 0; i < results.size(); i++)
    if(results.at(i) != expectedResults.at(i)) {
      cout << "FAILED IN " << identifier << endl;
      return false;
    }

  return true;
}

bool testFtIndexerMem() {  
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen);
  DivideSkipMerger<> merger;
  
  FtSearcherMem<> searcher(&merger);

  bool success = true;

  // try different filters with different fanouts and values for max string length
  for(unsigned maxStrLength = 10; maxStrLength <= 200; maxStrLength += 10) {
    for(unsigned fanout = 1; fanout <= 10; fanout++) {
      vector<unsigned> results;      

      // begin block for indexer with lengthfilter
      {
	FtIndexerMem<> indexer(&strContainer, &gramGen, maxStrLength, fanout);
	indexer.addPartFilter(new LengthFilter(maxStrLength));
	indexer.buildIndex(false);      

	// execute queries and compute results
	results.clear();
	searcher.setFtIndexer(&indexer);	
	for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	  searcher.search(**iter, results);
	
	success = success && compareResults(results, "FtIndexerMem, LENGTH FILTER BUILT");

	// save index, load it into differrent indexer and repeat
	indexer.saveIndex("UnittestIndex.ix");
	FtIndexerMem<> loadedIndexer(&strContainer);
	loadedIndexer.loadIndex("UnittestIndex.ix");

	if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH) {
	  cout << "FtIndexerMem, LENGTH FILTER LOADED INCORRECTLY" << endl;
	  success = false;
	}

	results.clear();
	searcher.setFtIndexer(&loadedIndexer);	
	for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	  searcher.search(**iter, results);
	
	success = success && compareResults(results, "FtIndexerMem, LENGTH FILTER LOADED");
      }

      // begin block for indexer with checksum filter
      {
	FtIndexerMem<> indexer(&strContainer, &gramGen, maxStrLength, fanout);
	indexer.addPartFilter(new CharsumFilter(maxStrLength));
	indexer.buildIndex(false);      

	// execute queries and compute results
	searcher.setFtIndexer(&indexer);	
	results.clear();
	for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	  searcher.search(**iter, results);
	
	success = success && compareResults(results, "FtIndexerMem, CHECKSUM FILTER BUILT");

	// save index, load it into differrent indexer and repeat
	indexer.saveIndex("UnittestIndex.ix");
	FtIndexerMem<> loadedIndexer(&strContainer);
	loadedIndexer.loadIndex("UnittestIndex.ix");

	if(loadedIndexer.filterTypes.at(0)->getType() != FT_CHARSUM) {
	  cout << "FtIndexerMem, CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	  success = false;
	}

	results.clear();
	searcher.setFtIndexer(&loadedIndexer);	
	for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	  searcher.search(**iter, results);
	
	success = success && compareResults(results, "FtIndexerMem, CHECKSUM FILTER LOADED");
      }

      // begin block for indexer with both length and checksum filters
      {
	FtIndexerMem<> indexer(&strContainer, &gramGen, maxStrLength, fanout);
	indexer.addPartFilter(new LengthFilter(maxStrLength));
	indexer.addPartFilter(new CharsumFilter(maxStrLength));
	indexer.buildIndex(false);      

	// execute queries and compute results
	searcher.setFtIndexer(&indexer);      
	results.clear();
	for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	  searcher.search(**iter, results);

	success = success && compareResults(results, "FtIndexerMem, LENGTH+CHECKSUM FILTER BUILT");

	// save index, load it into differrent indexer and repeat
	indexer.saveIndex("UnittestIndex.ix");
	FtIndexerMem<> loadedIndexer(&strContainer);
	loadedIndexer.loadIndex("UnittestIndex.ix");

	if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH
	   && loadedIndexer.filterTypes.at(1)->getType() != FT_CHARSUM) {
	  cout << "FtIndexerMem, LENGTH+CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	  success = false;
	}

	results.clear();
	searcher.setFtIndexer(&loadedIndexer);	
	for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	  searcher.search(**iter, results);
	
	success = success && compareResults(results, "FtIndexerMem, LENGTH FILTER LOADED");
      }

    }
  }

  return success;
}

bool testFtIndexerHolesGlobalLLF() {  
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen);
  DivideSkipMerger<> merger;
  
  FtSearcherMem<FtIndexerHolesGlobalLLF<> > searcher(&merger);

  bool success = true;

  // try different filters with different fanouts and values for max string length
  for(unsigned maxStrLength = 10; maxStrLength <= 200; maxStrLength += 10) {
    for(unsigned fanout = 1; fanout <= 10; fanout++) {
      for(float compressionRatio = 0.1f; compressionRatio <= 0.7f; compressionRatio += 0.1f) {
	vector<unsigned> results;
	
	// begin block for indexer with lengthfilter
	{
	  FtIndexerHolesGlobalLLF<> indexer(&strContainer, &gramGen, compressionRatio, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  results.clear();
	  searcher.setFtIndexer(&indexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalLLF, LENGTH FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalLLF<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH) {
	    cout << "FtIndexerHolesGlobalLLF, LENGTH FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalLLF, LENGTH FILTER LOADED");
	}
	
	// begin block for indexer with checksum filter
	{
	  FtIndexerHolesGlobalLLF<> indexer(&strContainer, &gramGen, compressionRatio, maxStrLength, fanout);
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);	
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalLLF, CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalLLF<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerHolesGlobalLLF, CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalLLF, CHECKSUM FILTER LOADED");
	}
	
	// begin block for indexer with both length and checksum filters
	{
	  FtIndexerHolesGlobalLLF<> indexer(&strContainer, &gramGen, compressionRatio, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);      
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalLLF, LENGTH+CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalLLF<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH
	     && loadedIndexer.filterTypes.at(1)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerHolesGlobalLLF, LENGTH+CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalLLF, LENGTH FILTER LOADED");
	}
	
      }
    }
  }

  return success;
}

bool testFtIndexerHolesGlobalSLF() {  
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen);
  DivideSkipMerger<> merger;
  
  FtSearcherMem<FtIndexerHolesGlobalSLF<> > searcher(&merger);

  bool success = true;

  // try different filters with different fanouts and values for max string length
  for(unsigned maxStrLength = 10; maxStrLength <= 200; maxStrLength += 10) {
    for(unsigned fanout = 1; fanout <= 10; fanout++) {
      for(float compressionRatio = 0.1f; compressionRatio <= 0.7f; compressionRatio += 0.1f) {
	vector<unsigned> results;
	
	// begin block for indexer with lengthfilter
	{
	  FtIndexerHolesGlobalSLF<> indexer(&strContainer, &gramGen, compressionRatio, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  results.clear();
	  searcher.setFtIndexer(&indexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalSLF, LENGTH FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalSLF<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH) {
	    cout << "FtIndexerHolesGlobalSLF, LENGTH FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalSLF, LENGTH FILTER LOADED");
	}
	
	// begin block for indexer with checksum filter
	{
	  FtIndexerHolesGlobalSLF<> indexer(&strContainer, &gramGen, compressionRatio, maxStrLength, fanout);
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);	
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalSLF, CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalSLF<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerHolesGlobalSLF, CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalSLF, CHECKSUM FILTER LOADED");
	}
	
	// begin block for indexer with both length and checksum filters
	{
	  FtIndexerHolesGlobalSLF<> indexer(&strContainer, &gramGen, compressionRatio, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalSLF, LENGTH+CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalSLF<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH
	     && loadedIndexer.filterTypes.at(1)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerHolesGlobalSLF, LENGTH+CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalSLF, LENGTH FILTER LOADED");
	}
	
      }
    }
  }

  return success;
}

bool testFtIndexerHolesGlobalRandom() {  
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen);
  DivideSkipMerger<> merger;
  
  FtSearcherMem<FtIndexerHolesGlobalRandom<> > searcher(&merger);

  bool success = true;

  // try different filters with different fanouts and values for max string length
  for(unsigned maxStrLength = 10; maxStrLength <= 200; maxStrLength += 10) {
    for(unsigned fanout = 1; fanout <= 10; fanout++) {
      for(float compressionRatio = 0.1f; compressionRatio <= 0.7f; compressionRatio += 0.1f) {
	vector<unsigned> results;
	
	// begin block for indexer with lengthfilter
	{
	  FtIndexerHolesGlobalRandom<> indexer(&strContainer, &gramGen, compressionRatio, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  results.clear();
	  searcher.setFtIndexer(&indexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalRandom, LENGTH FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalRandom<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH) {
	    cout << "FtIndexerHolesGlobalRandom, LENGTH FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalRandom, LENGTH FILTER LOADED");
	}
	
	// begin block for indexer with checksum filter
	{
	  FtIndexerHolesGlobalRandom<> indexer(&strContainer, &gramGen, compressionRatio, maxStrLength, fanout);
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);	
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalRandom, CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalRandom<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerHolesGlobalRandom, CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalRandom, CHECKSUM FILTER LOADED");
	}
	
	// begin block for indexer with both length and checksum filters
	{
	  FtIndexerHolesGlobalRandom<> indexer(&strContainer, &gramGen, compressionRatio, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);      
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalRandom, LENGTH+CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalRandom<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH
	     && loadedIndexer.filterTypes.at(1)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerHolesGlobalRandom, LENGTH+CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalRandom, LENGTH FILTER LOADED");
	}
	
      }
    }
  }

  return success;
}

bool testFtIndexerHolesGlobalTimeCost() {  
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen);
  DivideSkipMerger<> merger;
  
  FtSearcherMem<FtIndexerHolesGlobalTimeCost<> > searcher(&merger);

  bool success = true;

  // try different filters with different fanouts and values for max string length
  for(unsigned maxStrLength = 10; maxStrLength <= 200; maxStrLength += 10) {
    for(unsigned fanout = 1; fanout <= 10; fanout++) {
      for(float compressionRatio = 0.1f; compressionRatio <= 0.7f; compressionRatio += 0.1f) {
	vector<unsigned> results;
	
	// begin block for indexer with lengthfilter
	{
	  FtIndexerHolesGlobalTimeCost<> indexer(&strContainer, &gramGen, compressionRatio, &queryStrings, &simMetric, 2.0f, true, 0.1f, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  results.clear();
	  searcher.setFtIndexer(&indexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalTimeCost, LENGTH FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalTimeCost<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH) {
	    cout << "FtIndexerHolesGlobalTimeCost, LENGTH FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalTimeCost, LENGTH FILTER LOADED");
	}
	
	// begin block for indexer with checksum filter
	{
	  FtIndexerHolesGlobalTimeCost<> indexer(&strContainer, &gramGen, compressionRatio, &queryStrings, &simMetric, 2.0f, true, 0.1f, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);	
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalTimeCost, CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalTimeCost<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerHolesGlobalTimeCost, CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalTimeCost, CHECKSUM FILTER LOADED");
	}
	
	// begin block for indexer with both length and checksum filters
	{
	  FtIndexerHolesGlobalTimeCost<> indexer(&strContainer, &gramGen, compressionRatio, &queryStrings, &simMetric, 2.0f, true, 0.1f, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);      
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalTimeCost, LENGTH+CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalTimeCost<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH
	     && loadedIndexer.filterTypes.at(1)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerHolesGlobalTimeCost, LENGTH+CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalTimeCost, LENGTH FILTER LOADED");
	}
	
      }
    }
  }

  return success;
}


bool testFtIndexerHolesGlobalTimeCostPlus() {  
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen);
  DivideSkipMerger<> merger;
  
  FtSearcherMem<FtIndexerHolesGlobalTimeCost<> > searcher(&merger);

  bool success = true;

  // try different filters with different fanouts and values for max string length
  for(unsigned maxStrLength = 10; maxStrLength <= 200; maxStrLength += 10) {
    for(unsigned fanout = 1; fanout <= 10; fanout++) {
      for(float compressionRatio = 0.1f; compressionRatio <= 0.7f; compressionRatio += 0.1f) {
	vector<unsigned> results;
	
	// begin block for indexer with lengthfilter
	{
	  FtIndexerHolesGlobalTimeCost<> indexer(&strContainer, &gramGen, compressionRatio, &queryStrings, &simMetric, 2.0f, false, 0.1f, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  results.clear();
	  searcher.setFtIndexer(&indexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalTimeCostPlus, LENGTH FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalTimeCost<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH) {
	    cout << "FtIndexerHolesGlobalTimeCostPlus, LENGTH FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalTimeCostPlus, LENGTH FILTER LOADED");
	}
	
	// begin block for indexer with checksum filter
	{
	  FtIndexerHolesGlobalTimeCost<> indexer(&strContainer, &gramGen, compressionRatio, &queryStrings, &simMetric, 2.0f, false, 0.1f, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);	
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalTimeCostPlus, CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalTimeCost<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerHolesGlobalTimeCostPlus, CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalTimeCostPlus, CHECKSUM FILTER LOADED");
	}
	
	// begin block for indexer with both length and checksum filters
	{
	  FtIndexerHolesGlobalTimeCost<> indexer(&strContainer, &gramGen, compressionRatio, &queryStrings, &simMetric, 2.0f, false, 0.1f, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);      
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalTimeCostPlus, LENGTH+CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalTimeCost<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH
	     && loadedIndexer.filterTypes.at(1)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerHolesGlobalTimeCostPlus, LENGTH+CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalTimeCostPlus, LENGTH FILTER LOADED");
	}
	
      }
    }
  }

  return success;
}

bool testFtIndexerHolesGlobalPanicCost() {  
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen);
  DivideSkipMerger<> merger;
  
  FtSearcherMem<FtIndexerHolesGlobalPanicCost<> > searcher(&merger);

  bool success = true;

  // try different filters with different fanouts and values for max string length
  for(unsigned maxStrLength = 10; maxStrLength <= 200; maxStrLength += 10) {
    for(unsigned fanout = 1; fanout <= 10; fanout++) {
      for(float compressionRatio = 0.1f; compressionRatio <= 0.7f; compressionRatio += 0.1f) {
	vector<unsigned> results;
	
	// begin block for indexer with lengthfilter
	{
	  FtIndexerHolesGlobalPanicCost<> indexer(&strContainer, &gramGen, compressionRatio, &queryStrings, &simMetric, 2.0f, true, 0.1f, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  results.clear();
	  searcher.setFtIndexer(&indexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalPanicCost, LENGTH FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalPanicCost<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH) {
	    cout << "FtIndexerHolesGlobalPanicCost, LENGTH FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalPanicCost, LENGTH FILTER LOADED");
	}
	
	// begin block for indexer with checksum filter
	{
	  FtIndexerHolesGlobalPanicCost<> indexer(&strContainer, &gramGen, compressionRatio, &queryStrings, &simMetric, 2.0f, true, 0.1f, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);	
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalPanicCost, CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalPanicCost<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerHolesGlobalPanicCost, CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalPanicCost, CHECKSUM FILTER LOADED");
	}
	
	// begin block for indexer with both length and checksum filters
	{
	  FtIndexerHolesGlobalPanicCost<> indexer(&strContainer, &gramGen, compressionRatio, &queryStrings, &simMetric, 2.0f, true, 0.1f, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);      
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalPanicCost, LENGTH+CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalPanicCost<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH
	     && loadedIndexer.filterTypes.at(1)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerHolesGlobalPanicCost, LENGTH+CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalPanicCost, LENGTH FILTER LOADED");
	}
	
      }
    }
  }

  return success;
}

bool testFtIndexerHolesGlobalPanicCostPlus() {  
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen);
  DivideSkipMerger<> merger;
  
  FtSearcherMem<FtIndexerHolesGlobalPanicCost<> > searcher(&merger);

  bool success = true;

  // try different filters with different fanouts and values for max string length
  for(unsigned maxStrLength = 10; maxStrLength <= 200; maxStrLength += 10) {
    for(unsigned fanout = 1; fanout <= 10; fanout++) {
      for(float compressionRatio = 0.1f; compressionRatio <= 0.7f; compressionRatio += 0.1f) {
	vector<unsigned> results;
	
	// begin block for indexer with lengthfilter
	{
	  FtIndexerHolesGlobalPanicCost<> indexer(&strContainer, &gramGen, compressionRatio, &queryStrings, &simMetric, 2.0f, false, 0.1f, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  results.clear();
	  searcher.setFtIndexer(&indexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalPanicCostPlus, LENGTH FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalPanicCost<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH) {
	    cout << "FtIndexerHolesGlobalPanicCostPlus, LENGTH FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalPanicCostPlus, LENGTH FILTER LOADED");
	}
	
	// begin block for indexer with checksum filter
	{
	  FtIndexerHolesGlobalPanicCost<> indexer(&strContainer, &gramGen, compressionRatio, &queryStrings, &simMetric, 2.0f, false, 0.1f, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);	
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalPanicCostPlus, CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalPanicCost<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerHolesGlobalPanicCostPlus, CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalPanicCostPlus, CHECKSUM FILTER LOADED");
	}
	
	// begin block for indexer with both length and checksum filters
	{
	  FtIndexerHolesGlobalPanicCost<> indexer(&strContainer, &gramGen, compressionRatio, &queryStrings, &simMetric, 2.0f, false, 0.1f, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);      
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalPanicCostPlus, LENGTH+CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerHolesGlobalPanicCost<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH
	     && loadedIndexer.filterTypes.at(1)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerHolesGlobalPanicCostPlus, LENGTH+CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerHolesGlobalPanicCostPlus, LENGTH FILTER LOADED");
	}
	
      }
    }
  }

  return success;
}

bool testFtIndexerUnionGlobalBasic() {  
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen);
  DivideSkipMerger<> merger(true);
  
  FtSearcherMem<FtIndexerUnionGlobalBasic<> > searcher(&merger);
  
  bool success = true;

  // try different filters with different fanouts and values for max string length
  for(unsigned maxStrLength = 10; maxStrLength <= 200; maxStrLength += 10) {
    for(unsigned fanout = 1; fanout <= 10; fanout++) {
      for(float compressionRatio = 0.1f; compressionRatio <= 0.7f; compressionRatio += 0.1f) {
	vector<unsigned> results;
	
	// begin block for indexer with lengthfilter
	{
	  FtIndexerUnionGlobalBasic<> indexer(&strContainer, &gramGen, compressionRatio, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  results.clear();
	  searcher.setFtIndexer(&indexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerUnionGlobalBasic, LENGTH FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerUnionGlobalBasic<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH) {
	    cout << "FtIndexerUnionGlobalBasic, LENGTH FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);	  

	  success = success && compareResults(results, "FtIndexerUnionGlobalBasic, LENGTH FILTER LOADED");
	}
	
	// begin block for indexer with checksum filter
	{
	  FtIndexerUnionGlobalBasic<> indexer(&strContainer, &gramGen, compressionRatio, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);	
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerUnionGlobalBasic, CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerUnionGlobalBasic<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerUnionGlobalBasic, CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerUnionGlobalBasic, CHECKSUM FILTER LOADED");
	}
	
	// begin block for indexer with both length and checksum filters
	{
	  FtIndexerUnionGlobalBasic<> indexer(&strContainer, &gramGen, compressionRatio, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);      
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerUnionGlobalBasic, LENGTH+CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerUnionGlobalBasic<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH
	     && loadedIndexer.filterTypes.at(1)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerUnionGlobalBasic, LENGTH+CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerUnionGlobalBasic, LENGTH FILTER LOADED");
	}
	
      }
    }
  }

  return success;
}

bool testFtIndexerUnionGlobalCost() {  
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen);
  DivideSkipMerger<> merger;
  
  FtSearcherMem<FtIndexerUnionGlobalCost<> > searcher(&merger);

  bool success = true;

  // try different filters with different fanouts and values for max string length
  for(unsigned maxStrLength = 10; maxStrLength <= 200; maxStrLength += 10) {
    for(unsigned fanout = 1; fanout <= 10; fanout++) {
      for(float compressionRatio = 0.1f; compressionRatio <= 0.7f; compressionRatio += 0.1f) {
	vector<unsigned> results;
	
	// begin block for indexer with lengthfilter
	{
	  FtIndexerUnionGlobalCost<> indexer(&strContainer, &gramGen, compressionRatio, &queryStrings, &simMetric, 2.0f, 0.1f, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  results.clear();
	  searcher.setFtIndexer(&indexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerUnionGlobalCost, LENGTH FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerUnionGlobalCost<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH) {
	    cout << "FtIndexerUnionGlobalCost, LENGTH FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerUnionGlobalCost, LENGTH FILTER LOADED");
	}
	
	// begin block for indexer with checksum filter
	{
	  FtIndexerUnionGlobalCost<> indexer(&strContainer, &gramGen, compressionRatio, &queryStrings, &simMetric, 2.0f, 0.1f, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);	
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerUnionGlobalCost, CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerUnionGlobalCost<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerUnionGlobalCost, CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerUnionGlobalCost, CHECKSUM FILTER LOADED");
	}
	
	// begin block for indexer with both length and checksum filters
	{
	  FtIndexerUnionGlobalCost<> indexer(&strContainer, &gramGen, compressionRatio, &queryStrings, &simMetric, 2.0f, 0.1f, 0.1f, maxStrLength, fanout);
	  indexer.addPartFilter(new LengthFilter(maxStrLength));
	  indexer.addPartFilter(new CharsumFilter(maxStrLength));
	  indexer.buildIndex(false);      
	  
	  // execute queries and compute results
	  searcher.setFtIndexer(&indexer);      
	  results.clear();
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerUnionGlobalCost, LENGTH+CHECKSUM FILTER BUILT");
	  
	  // save index, load it into differrent indexer and repeat
	  indexer.saveIndex("UnittestIndex.ix");
	  FtIndexerUnionGlobalCost<> loadedIndexer(&strContainer);
	  loadedIndexer.loadIndex("UnittestIndex.ix");
	  
	  if(loadedIndexer.filterTypes.at(0)->getType() != FT_LENGTH
	     && loadedIndexer.filterTypes.at(1)->getType() != FT_CHARSUM) {
	    cout << "FtIndexerUnionGlobalCost, LENGTH+CHECKSUM FILTER LOADED INCORRECTLY" << endl;
	    success = false;
	  }
	  
	  results.clear();
	  searcher.setFtIndexer(&loadedIndexer);	
	  for(vector<Query*>::iterator iter = queries.begin(); iter != queries.end(); iter++)
	    searcher.search(**iter, results);
	  
	  success = success && compareResults(results, "FtIndexerUnionGlobalCost, LENGTH FILTER LOADED");
	}	
      }
    }
  }

  return success;
}
