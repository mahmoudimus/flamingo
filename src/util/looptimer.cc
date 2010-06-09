/*  
    $Id: looptimer.cc 5149 2010-03-24 23:37:18Z abehm $ 

    Copyright (C) 2010 by The Regents of the University of California
 	
    Redistribution of this file is permitted under the terms of the BSD license.

    Author: Shengyue Ji <shengyuj (at) ics.uci.edu>
    Date: 05/11/2007

*/


#include "looptimer.h"

LoopTimer::LoopTimer()
{
    file = stderr;
    running = false;
}

void LoopTimer::print()
{
    if(desc.empty() || count == 0) return;
    
    fprintf(file, "\r%u%% %s: %u/%u; ", 
        step * 100 / count, desc.c_str(), step, count);
    
    unsigned t = totalTime();
    t /= 1000;
    fprintf(file, "%u'%u\"/", t / 60, t % 60);
    
    if(step)
    {
        t = t * count / step;
        
        fprintf(file, "%u'%u\"   ", t / 60, t % 60);
    }
    else fprintf(file, "inf   ");

}

void LoopTimer::begin(const string &desc, unsigned count)
{
    this->desc = desc;
    this->count = count;
    this->step = 0;
    sumTime.tv_sec = 0;
    sumTime.tv_nsec = 0;

    running = true;
    clock_gettime(CLOCK_REALTIME, &startTime);
    currTime = startTime;
    prevTime = currTime;
    print();
}

void LoopTimer::next(unsigned step, unsigned count)
{
    this->step += step;
    if(count)this->count = count + this->step;
    
    if(!running)return;
    clock_gettime(CLOCK_REALTIME, &currTime);
    if((currTime.tv_sec - prevTime.tv_sec) * 1000
        + (currTime.tv_nsec - prevTime.tv_nsec) / 1000000 >= 200)
    {
        prevTime = currTime;
        print();
    }
}

void LoopTimer::end()
{
    step = count;
    pause();
    print();
    if(!desc.empty() && !count == 0)
        fprintf(file, "\n");
}

void LoopTimer::pause()
{
    if(!running)return;
    running = false;
    clock_gettime(CLOCK_REALTIME, &currTime);
    
    sumTime.tv_sec += currTime.tv_sec - startTime.tv_sec;
    sumTime.tv_nsec += currTime.tv_nsec - startTime.tv_nsec;
    if(sumTime.tv_nsec < 0)
    {
        sumTime.tv_nsec += 1000000000;
        sumTime.tv_sec--;
    }
    else if(sumTime.tv_nsec >= 1000000000)
    {
        sumTime.tv_nsec -= 1000000000;
        sumTime.tv_sec++;
    }
    startTime = currTime;
}

void LoopTimer::resume()
{
    if(running)return;
    running = true;
    clock_gettime(CLOCK_REALTIME, &startTime);
    currTime = startTime;
}

unsigned LoopTimer::totalTime()
{
    return (currTime.tv_sec - startTime.tv_sec + sumTime.tv_sec) * 1000
        + (currTime.tv_nsec - startTime.tv_nsec + sumTime.tv_nsec) / 1000000;
}

float LoopTimer::avgTime()
{
    return (float)totalTime() / step;
}

