/*
  $Id: perftest.cc 5229 2010-05-11 05:44:03Z abehm $

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD License.
    
  Date: 09/19/2007
  Author: Alexander Behm <abehm (at) ics.uci.edu>
*/

#include "statsgen.h"

void statsGenMemSimple();
void statsGenMemDiscardLists();
void statsGenMemCombineLists();
void statsGenDiskSimple();

void onDiskTest();

int main() {
  statsGenMemSimple();
  statsGenMemDiscardLists();
  statsGenMemCombineLists();
  statsGenDiskSimple();  


  //onDiskTest();

  return 0;
}

void onDiskTest() {
  StatsGenConfig config;
  
  typedef FtIndexerOnDisk<> indexer;
  typedef FtSearcherOnDisk<indexer, OnDiskMergerAdapt<> > ignoreSearcher;
  typedef FtSearcherOnDisk<indexer> simpleSearcher;  
  
  typedef FtIndexerOnDisk<StringContainerRM, Array<PosID> > posIndexer;
  typedef FtSearcherOnDisk<posIndexer, OnDiskMergerAdapt<Array<PosID> > > posSearcher;
  
  StatsGen<indexer, ignoreSearcher> statsgen(&config);
  StatsGen<indexer, simpleSearcher> simpleStatsgen(&config); 
  StatsGen<FtIndexerMem<>, FtSearcherMem<> > memStatsgen(&config); 
  StatsGen<posIndexer, posSearcher> posStatsgen(&config);
  OnDiskMergerSimple<> simpleReg(false);
  OnDiskMergerSimple<> simpleOpt(true);
  OnDiskMergerAdapt<> ignoreReg(false);
  OnDiskMergerAdapt<> ignoreOpt(true);
  DivideSkipMerger<> dsMerger(false);
  OnDiskMergerAdapt<Array<PosID> > posMergerReg(false);
  OnDiskMergerAdapt<Array<PosID> > posMergerOptNothing(true);
  OnDiskMergerAdapt<Array<PosID> > posMergerOptDP(true, POSFILTER_LENGTH | POSFILTER_DP);
  OnDiskMergerAdapt<Array<PosID> > posMergerOptEndDP(true, POSFILTER_LENGTH | POSFILTER_ENDDP);
  OnDiskMergerAdapt<Array<PosID> > posMergerOptSubstr(true, POSFILTER_LENGTH | POSFILTER_SUBSTR);
  OnDiskMergerAdapt<Array<PosID> > posMergerOptSubstrEndDP(true, POSFILTER_LENGTH | POSFILTER_SUBSTR | POSFILTER_ENDDP);
  
  GramGenFixedLen gramGen(4);
  float ed = 0.0f;
  SimMetricEd simMetric(gramGen);

  // indexing params
  config.setReduction(0, 0, 0.1);
  config.setGramGen(&gramGen);
  config.setRunBuffer(419430400);
  config.setScatteredOrg(false);
  config.setDictSize(5000000, 5000000, 5000000);
  
  config.setAutoPartFilter(false);

  // dataset params
  //config.setDictDataFile("data/googlebig250.txt");
  //config.setMaxStrLen(200);
  //config.setFanout(300);
  //config.addPartFilter(new CharsumFilter(200));
  //config.setAvgStrLen(22);
  
  config.setDictDataFile("data/pubmedtitles250.txt");
  config.setMaxStrLen(300);
  config.setFanout(100);
  config.addPartFilter(new LengthFilter(300));
  config.setAvgStrLen(84);

  // query params
  config.setDropCachesBeforeBuildingIndex(false);
  config.setDropCachesBeforeEachQuery(false);
  config.setDisableStreamBuffer(false);
  
  // workload params
  config.setNumberQueries(2000);
  config.setDistinctQueries(2000);
  config.setQueriesDistribution(QD_UNIFORM);
  config.setNumberRuns(3);
  config.useExistingWorkload("data/pubmedtitles_2kqueries.txt");
  //config.useExistingWorkload("data/googlebig_2kqueries.txt");
  
  config.setCharsumFilterOptions(CSF_OPT);
  config.setPhysOrd(PHO_LENGTH_CHARSUM);
  config.setOutputFlags(OF_WORKLOADSTATS);
  
  statsgen.setMerger(&ignoreOpt);
  
  ed = 2.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/pubmed_adaptptcsord_cache_ed2");
  statsgen.generate();
  
  ed = 4.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/pubmed_adaptptcsord_cache_ed4");
  statsgen.generate();

  ed = 6.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/pubmed_adaptptcsord_cache_ed6");
  statsgen.generate();

  ed = 8.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/pubmed_adaptptcsord_cache_ed8");
  statsgen.generate();

  ed = 10.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/pubmed_adaptptcsord_cache_ed10");
  statsgen.generate();


  
  simpleStatsgen.setMerger(&simpleOpt);
  //config.clearFilters();
  config.setCharsumFilterOptions(CSF_OPT);
  config.setPhysOrd(PHO_LENGTH_CHARSUM);
  config.setOutputFlags(OF_WORKLOADSTATS);
  
  ed = 2.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/pubmed_simpleptcsord_cache_ed2");
  simpleStatsgen.generate();
  
  ed = 4.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/pubmed_simpleptcsord_cache_ed4");
  simpleStatsgen.generate();

  ed = 6.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/pubmed_simpleptcsord_cache_ed6");
  simpleStatsgen.generate();

  ed = 8.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/pubmed_simpleptcsord_cache_ed8");
  simpleStatsgen.generate();

  ed = 10.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/pubmed_simpleptcsord_cache_ed10");
  simpleStatsgen.generate();


  
  simpleStatsgen.setMerger(&simpleOpt);
  config.clearFilters();
  config.setCharsumFilterOptions(CSF_NONE);
  config.setPhysOrd(PHO_NONE);
  config.setOutputFlags(OF_WORKLOADSTATS);
  
  ed = 2.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/pubmed_simple_cache_ed2");
  simpleStatsgen.generate();
  
  ed = 4.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/pubmed_simple_cache_ed4");
  simpleStatsgen.generate();

  ed = 6.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/pubmed_simple_cache_ed6");
  simpleStatsgen.generate();

  ed = 8.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/pubmed_simple_cache_ed8");
  simpleStatsgen.generate();

  ed = 10.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/pubmed_simple_cache_ed10");
  simpleStatsgen.generate();


  
  /*
  simpleStatsgen.setMerger(&simpleOpt);
  simpleStatsgen.setMerger(&simpleOpt);
  
  ed = 1.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/improve/google_simpleopt_cache_ed1");
  //simpleStatsgen.generate();
  
  ed = 2.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/improve/google_simpleopt_cache_ed2");
  //simpleStatsgen.generate();  
  
  statsgen.setMerger(&ignoreOpt);
  
  ed = 1.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/improve/google_ignoreopt_cache_ed1");
  statsgen.generate();
  
  ed = 2.0f;
  config.setSimMetric(&simMetric, ed);
  config.setOutputFilePrefix("experiments/improve/google_ignoreopt_cache_ed2");
  statsgen.generate();
  */
}


void statsGenMemSimple() {
  GramGenFixedLen gramGen(2);
  SimMetricEd simMetric(gramGen); // using the edit distance
  //SimMetricJacc simMetric(gramGen); // using jaccard similarity
  //SimMetricCos simMetric(gramGen); // using cosine similarity
  //SimMetricDice simMetric(gramGen); // using dice similarity
  float ed = 1.0f; // this represents the similarity threshold, in this case edit distance 1

  DivideSkipMerger<> merger;
  typedef FtIndexerMem<> indexer;
  typedef FtSearcherMem<indexer> searcher;

  StatsGenConfig config;
  config.setGramGen(&gramGen);
  config.setSimMetric(&simMetric, ed); // similarity metric, similarity threshold
  config.setDictSize(4000, 4000, 4000); // size of string dictionary, start, stop, step
  config.setFanout(10); // fanout of filtertree
  config.setMaxStrLen(20);
  config.clearFilters();
  config.addPartFilter(new LengthFilter(20)); // add partitioning filter
  config.setNumberQueries(2000); // set total number of queries for the workload to run
  config.setDistinctQueries(2000); // set distinct number of queries generated by randomly picking strings from the dictionary
  config.setQueriesDistribution(QD_UNIFORM); // can be QD_UNIFORM or QD_ZIPF
  //config.setZipfSkew(1); // zipf skew parameter if QD_ZIPF specified
  config.setNumberRuns(3); // number times to repeat running workload to stabilize numbers
      
  StatsGen<indexer, searcher> statsgen(&config); // create an instance of the stats generator
  statsgen.setMerger(&merger);
  
  config.setDictDataFile("data/female_names.txt"); // set the data file
  config.setOutputFlags(OF_WORKLOADSTATS | OF_QUERYRESULTS | OF_QUERYSTATS); // output workload stats, query stats and query results
  config.setOutputFilePrefix("memsimple_names_ed1"); // prefix of output files
  statsgen.generate(); // run the experiment

  cout << "SUCCESS!" << endl;
}

void statsGenMemDiscardLists() {
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen); // using the edit distance
  float ed = 2.0f; // this represents the similarity threshold, in this case edit distance 1
  
  DivideSkipMerger<> merger(true); // true == optimization for combining lists activated
  //typedef FtIndexerDiscardListsLLF<> indexer; // discard long lists first
  //typedef FtIndexerDiscardListsSLF<> indexer; // discard short lists first
  //typedef FtIndexerDiscardListsRandom<> indexer; // randomly discard lists first
  //typedef FtIndexerDiscardListsPanicCost<> indexer; // minimize number of panics
  typedef FtIndexerDiscardListsTimeCost<> indexer; // minimize total running time
  //typedef FtIndexerCombineListsBasic<> indexer; // combine lists based on correlation
  //typedef FtIndexerCombineListsCost<> indexer; // combine lists based on total running time
  typedef FtSearcherMem<indexer> searcher;
  
  StatsGenConfig config;
  config.setGramGen(&gramGen);
  config.setSimMetric(&simMetric, ed); // similarity metric, similarity threshold
  config.setSimMetric(&simMetric, ed); // similarity metric, similarity threshold
  config.setDictSize(4000, 4000, 4000); // size of string dictionary, start, stop, step
  config.setFanout(10); // fanout of filtertree
  config.setMaxStrLen(20);
  config.clearFilters();
  config.addPartFilter(new LengthFilter(20)); // add partitioning filter
  config.setNumberQueries(2000); // set total number of queries for the workload to run
  config.setDistinctQueries(2000); // set distinct number of queries generated by randomly picking strings from the dictionary
  config.setQueriesDistribution(QD_UNIFORM); // can be QD_UNIFORM or QD_ZIPF
  //config.setZipfSkew(1); // zipf skew parameter if QD_ZIPF specified
  config.setNumberRuns(3); // number times to repeat running workload to stabilize numbers
  
  // parameters for compression experiments
  config.setReduction(0.0, 0.51, 0.1); // reduction ratio start, stop, step
  config.setSampleRatioCost(false); // minimize cost or minimize benefit/cost? false means minimize cost (only for discarding lists)
  config.setQueriesSamplingFrac(0.25f); // use this sample of queries for compression
  config.setDictSamplingFrac(0.01f); // use this sample of data strings for compression

  StatsGen<indexer, searcher> statsgen(&config); // create an instance of the stats generator
  statsgen.setMerger(&merger);
  
  config.setDictDataFile("data/female_names.txt"); // set the data file
  config.setOutputFlags(OF_WORKLOADSTATS | OF_QUERYRESULTS | OF_QUERYSTATS); // output workload stats, query stats and query results
  config.setOutputFilePrefix("memdl_names_ed1"); // prefix of output files
  statsgen.generate(); // run the experiment

  cout << "SUCCESS!" << endl;
}

void statsGenMemCombineLists() {
  GramGenFixedLen gramGen(3);
  SimMetricEd simMetric(gramGen); // using the edit distance
  float ed = 2.0f; // this represents the similarity threshold, in this case edit distance 1
  
  DivideSkipMerger<> merger(true); // true == optimization for combining lists activated
  //typedef FtIndexerDiscardListsLLF<> indexer; // discard long lists first
  //typedef FtIndexerDiscardListsSLF<> indexer; // discard short lists first
  //typedef FtIndexerDiscardListsRandom<> indexer; // randomly discard lists first
  //typedef FtIndexerDiscardListsPanicCost<> indexer; // minimize number of panics
  //typedef FtIndexerDiscardListsTimeCost<> indexer; // minimize total running time
  //typedef FtIndexerCombineListsBasic<> indexer; // combine lists based on correlation
  typedef FtIndexerCombineListsCost<> indexer; // combine lists based on total running time
  typedef FtSearcherMem<indexer> searcher;
  
  StatsGenConfig config;
  config.setGramGen(&gramGen);
  config.setSimMetric(&simMetric, ed); // similarity metric, similarity threshold
  config.setDictSize(4000, 4000, 4000); // size of string dictionary, start, stop, step
  config.setFanout(10); // fanout of filtertree
  config.setMaxStrLen(20);
  config.clearFilters();
  config.addPartFilter(new LengthFilter(20)); // add partitioning filter
  config.setNumberQueries(2000); // set total number of queries for the workload to run
  config.setDistinctQueries(2000); // set distinct number of queries generated by randomly picking strings from the dictionary
  config.setQueriesDistribution(QD_UNIFORM); // can be QD_UNIFORM or QD_ZIPF
  //config.setZipfSkew(1); // zipf skew parameter if QD_ZIPF specified
  config.setNumberRuns(3); // number times to repeat running workload to stabilize numbers
  
  // parameters for compression experiments
  config.setReduction(0.0, 0.51, 0.1); // reduction ratio start, stop, step
  config.setSampleRatioCost(false); // minimize cost or minimize benefit/cost? false means minimize cost (only for discarding lists)
  config.setQueriesSamplingFrac(1.0f); // use this sample of queries for compression
  config.setDictSamplingFrac(0.01f); // use this sample of data strings for compression

  StatsGen<indexer, searcher> statsgen(&config); // create an instance of the stats generator
  statsgen.setMerger(&merger);
  
  config.setDictDataFile("data/female_names.txt"); // set the data file
  config.setOutputFlags(OF_WORKLOADSTATS | OF_QUERYRESULTS | OF_QUERYSTATS); // output workload stats, query stats and query results
  config.setOutputFilePrefix("memcl_names_ed1"); // prefix of output files
  statsgen.generate(); // run the experiment

  cout << "SUCCESS!" << endl;
}

void statsGenDiskSimple() {
  GramGenFixedLen gramGen(2);
  SimMetricEd simMetric(gramGen); // using the edit distance
  //SimMetricJacc simMetric(gramGen); // using jaccard similarity
  //SimMetricCos simMetric(gramGen); // using cosine similarity
  //SimMetricDice simMetric(gramGen); // using dice similarity
  float ed = 1.0f; // this represents the similarity threshold, in this case edit distance 1

  OnDiskMergerSimple<> merger;
  typedef FtIndexerOnDisk<> indexer;
  typedef FtSearcherOnDisk<indexer> searcher;
  
  StatsGenConfig config;
  config.setGramGen(&gramGen);
  config.setSimMetric(&simMetric, ed); // similarity metric, similarity threshold
  config.setDictSize(4000, 4000, 4000); // size of string dictionary, start, stop, step
  config.setFanout(10); // fanout of filtertree
  config.setMaxStrLen(20);
  config.clearFilters();
  config.addPartFilter(new LengthFilter(20)); // add partitioning filter
  config.setNumberQueries(2000); // set total number of queries for the workload to run
  config.setDistinctQueries(2000); // set distinct number of queries generated by randomly picking strings from the dictionary
  config.setQueriesDistribution(QD_UNIFORM); // can be QD_UNIFORM or QD_ZIPF
  //config.setZipfSkew(1); // zipf skew parameter if QD_ZIPF specified
  config.setNumberRuns(3); // number times to repeat running workload to stabilize numbers
  
  config.setRunBuffer(50000); // size of buffer for index construction (in bytes)
  config.setAvgStrLen(8); // for optimizing disk-based storage of strings
  config.setPhysOrd(PHO_AUTO); // automatically choose physical ordering of strings in container  
  config.setCharsumFilterOptions(CSF_OPT); // use optimized charsum filter  

  // for experiments on raw disk IOs, clear file-system caches and disable c++ filestream buffering?
  // if set to true, perftest MUST be run as root user, sudo is NOT good enough
  // it is recommended to run perftest as root anyway, in order to clear caches after index construction, 
  // otherwise queries may seem too fast due to a hot cache
  config.setDropCachesBeforeBuildingIndex(false);
  config.setDropCachesBeforeEachQuery(false);
  config.setDisableStreamBuffer(false);
  
  StatsGen<indexer, searcher> statsgen(&config); // create an instance of the stats generator
  statsgen.setMerger(&merger);
  
  config.setDictDataFile("data/female_names.txt"); // set the data file
  config.setOutputFlags(OF_WORKLOADSTATS | OF_QUERYRESULTS | OF_QUERYSTATS); // output workload stats, query stats and query results
  config.setOutputFilePrefix("disk_names_ed1"); // prefix of output files
  statsgen.generate(); // run the experiment
  
  cout << "SUCCESS!" << endl;
}

