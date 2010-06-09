/*
  $Id: gramgen.h Tue Apr 05 10:20:24 PDT 2008 abehm$

  Copyright (C) 2010 by The Regents of the University of California
	
  Redistribution of this file is permitted under
  the terms of the BSD license
    
  Date: 04/05/2008
  Author: Rares Vernica <rares (at) ics.uci.edu> 
  Alexander Behm <abehm (at) ics.uci.edu>
*/

#include "gramgen.h"
#include <iostream>

using namespace std;
using namespace tr1;

#if _WIN32
const hash_win32 GramGen::hashString = hash_win32();
#else
const hash<string> GramGen::hashString = hash<string>();
#endif

unsigned GramGenFixedLen::getPrePostStrLen(unsigned origStrLen) const {
  if(prePost) return origStrLen;
  else return origStrLen + 2 * q - 2;
}

GramGen* GramGen::loadGramGenInstance(ifstream& fpIn)
{
  GramGenType ggt = GGT_FIXED;
  fpIn.read((char*)&ggt, sizeof(GramGenType));

  switch(ggt) {
  case GGT_FIXED: return new GramGenFixedLen(fpIn); break;
    // TODO: implement others!
  default:
    cout << "ERROR: unknown gramgentype loaded from file!" << endl;
    return NULL; break;
  }
}

GramGenFixedLen::GramGenFixedLen(ifstream& fpIn)
{
  fpIn.read((char*)&q, sizeof(uint));
  fpIn.read((char*)&prePost, sizeof(bool));  
}

bool GramGenFixedLen::containsSpace(string& s) const 
{
  string::size_type loc = s.find(' ', 0);
  return !(loc == string::npos);
}

void GramGenFixedLen::decompose(
  const string &s, 
  vector<string> &res, 
  uchar st, 
  uchar en) 
  const 
{
  if(prePost) {
    const string sPad = string(q - 1, st) + s + string(q - 1, en);  
    for (uint i = 0; i < s.length() + q - 1; i++) {
      string substring = sPad.substr(i, q);
      if(!(noSpace && containsSpace(substring)))
	res.push_back(sPad.substr(i, q));        
    }
  }
  else {
    for (uint i = 0; i < s.length() - q + 1; i++) {
      string substring = s.substr(i, q);
      if(!(noSpace && containsSpace(substring)))
        res.push_back(substring);
    }
  }
}

void GramGenFixedLen::decompose(
  const string &s, 
  vector<uint> &res, 
  uchar st, 
  uchar en) 
  const
{
  if(prePost) {
    string sPad = string(q - 1, st) + s + string(q - 1, en);  
    for(uint i = 0; i < s.length() + q - 1; i++) {
      string substring = sPad.substr(i, q);
      if(!(noSpace && containsSpace(substring)))
	res.push_back(hashString(substring));
    }
  }
  else {
    for (uint i = 0; i < s.length() - q + 1; i++) {
      string substring = s.substr(i, q);
      if(!(noSpace && containsSpace(substring)))
	res.push_back(hashString(substring));
    }      
  }
}

void GramGenFixedLen::decompose(
  const string &s,
  multiset<string> &res,  
  uchar st, 
  uchar en) 
  const 
{
  if(prePost) {
    const string sPad = string(q - 1, st) + s + string(q - 1, en);
    for (uint i = 0; i < s.length() + q - 1; i++) {
      string substring = sPad.substr(i, q);
      if(!(noSpace && containsSpace(substring)))      
	res.insert(substring);  
    }
  }
  else {      
    for (uint i = 0; i < s.length() - q + 1; i++) {
      string substring = s.substr(i, q);      
      if(!(noSpace && containsSpace(substring)))      
	res.insert(substring);  
    }
  }
}

void GramGenFixedLen::decompose(
  const string &s, 
  multiset<uint> &res, 
  uchar st, 
  uchar en) 
  const 
{
  if(prePost) {
    const string sPad = string(q - 1, st) + s + string(q - 1, en);  
    for (uint i = 0; i < s.length() + q - 1; i++) {
      string substring = sPad.substr(i, q);
      if(!(noSpace && containsSpace(substring)))     
	res.insert(hashString(substring));      
    }
  }
  else {
    for (uint i = 0; i < s.length() - q + 1; i++) {
      string substring = s.substr(i, q);
      if(!(noSpace && containsSpace(substring)))           
	res.insert(hashString(substring));  
    }
  }
}

void GramGenFixedLen::decompose(
  const string &s, 
  set<string> &res, 
  uchar st, 
  uchar en) 
  const 
{
  if(prePost) {
    const string sPad = string(q - 1, st) + s + string(q - 1, en);
    for (uint i = 0; i < s.length() + q - 1; i++) {
      string substring = sPad.substr(i, q);
      if(!(noSpace && containsSpace(substring)))     
	res.insert(substring);  
    }
  }
  else {      
    for (uint i = 0; i < s.length() - q + 1; i++) {
      string substring = s.substr(i, q);
      if(!(noSpace && containsSpace(substring)))           
	res.insert(substring);  
    }
  }
}

void GramGenFixedLen::decompose(
  const string &s, 
  set<uint> &res, 
  uchar st, 
  uchar en) 
  const 
{
 if(prePost) {
    const string sPad = string(q - 1, st) + s + string(q - 1, en);  
    for (uint i = 0; i < s.length() + q - 1; i++) {
      string substring = sPad.substr(i, q); 
      if(!(noSpace && containsSpace(substring)))        
	res.insert(hashString(substring));
    }
  }
  else {
    for (uint i = 0; i < s.length() - q + 1; i++) {
      string substring = s.substr(i, q);
      if(!(noSpace && containsSpace(substring)))    
	res.insert(hashString(substring));  
    }
  }
}

void GramGenFixedLen::decompose(
  const string &s, 
  map<uint, uint> &res, 
  uchar st, 
  uchar en) 
  const 
{
  if(prePost) {
    const string sPad = string(q - 1, st) + s + string(q - 1, en);
    for (uint i = 0; i < s.length() + q - 1; i++) {
      string substring = sPad.substr(i, q); 
      if(!(noSpace && containsSpace(substring)))    
	res[hashString(substring)]++;  
    }
  }
  else {
    for (uint i = 0; i < s.length() - q + 1; i++) {
      string substring = s.substr(i, q);
      if(!(noSpace && containsSpace(substring)))    
	res[hashString(substring)]++;  
    }
  }
}

uint GramGenFixedLen::getNumGrams(const string& s) const 
{
  if(prePost) return s.length() + q - 1;
  else return s.length() - q + 1;
}

void 
GramGenFixedLen::
getPrePostString(const std::string& src, std::string& dest, uchar st, uchar en) const {
  if(prePost) dest = string(q - 1, st) + src + string(q - 1, en);  
  else dest = src;
}

void GramGenFixedLen::saveGramGenInstance(ofstream& fpOut) 
{
  fpOut.write((const char*)&gramGenType, sizeof(gramGenType));
  fpOut.write((const char*)&q, sizeof(uint));
  fpOut.write((const char*)&prePost, sizeof(bool));
}
