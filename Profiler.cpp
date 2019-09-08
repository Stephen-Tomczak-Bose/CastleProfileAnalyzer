/*
 * Profiler.cpp
 *
 *  Created on: Sep 5, 2019
 *      Author: st1016431
 */

#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include "Profiler.h"

Profiler::Profiler(const std::string &logfile)
{
    m_file.open(logfile.c_str(), std::fstream::in);

    m_months.push_back("Jan");
    m_months.push_back("Feb");
    m_months.push_back("Mar");
    m_months.push_back("Apr");
    m_months.push_back("May");
    m_months.push_back("Jun");
    m_months.push_back("Jul");
    m_months.push_back("Aug");
    m_months.push_back("Sep");
    m_months.push_back("Nov");
    m_months.push_back("Dec");
    m_year = 2019;
}

Profiler::~Profiler()
{
    m_file.close();
}

void Profiler::Parse()
{
    // 638:Sep  6 07:36:54 apq8017 local0.info APServer[2405]:
    //   [(005835):APSrcSelector_stream_1:INFO]Profiling:
    //      Profiler[OpenHttpStream]: 163858s50us -> 165564s717us = 1706s667us

    MapData avg;
    std::string line;
    while(std::getline(m_file, line))
    {
        unsigned int epos, pos = 0;

 //       std::cout << "Read Line: " << line << std::endl;

        if((std::string::npos != line.find("Profiler")) &&
           (std::string::npos != (pos = line.find("ProvideData") ) ) &&
           (pos < line.size()))
        {
//            std::cout << "Found 'ProvideData' at " << pos << std::endl;

            struct tm logTime;
            if(GetLogTime(line, logTime) != 0)
            {
                continue;
            }

            // Find the data size.
            if(std::string::npos == (pos = line.find("(", pos)))
            {
                break;
            }
            ++pos;
            if(std::string::npos == (epos = line.find(")", pos)))
            {
                break;
            }
            unsigned int dataSize = strtoul(line.substr(pos, epos - pos).c_str(), NULL, 10);

            if(std::string::npos == (pos = line.find("=", epos)))
            {
                break;
            }
            pos += 2;
            if(std::string::npos == (epos = line.find("s", pos)))
            {
                break;
            }

            time_t uSec = 1000000 * strtoul(line.substr(pos, epos - pos).c_str(), NULL, 10);
            pos = epos + 1;
            if(std::string::npos == (epos = line.find("us", pos)))
            {
                break;
            }
            uSec += strtoul(line.substr(pos, epos - pos).c_str(), NULL, 10);

            std::cout << "Log " << logTime.tm_hour << ":" << logTime.tm_min << ":" << logTime.tm_sec
                      << " DataSize: " << dataSize << " Time(uSec): " << uSec << std::endl;

            if(avg.log.tm_hour == logTime.tm_hour ||
               avg.log.tm_min == logTime.tm_min ||
               avg.log.tm_sec == logTime.tm_sec )
            {
                avg.dataSize = (avg.dataSize + dataSize) / 2;
                avg.time = (avg.time + uSec) / 2;
            }
            else
            {
                m_profile.push_back(avg);
                avg.dataSize = dataSize;
                avg.time = uSec;
                avg.log.tm_hour = logTime.tm_hour;
                avg.log.tm_min  = logTime.tm_min;
                avg.log.tm_sec  = logTime.tm_sec;
            }
        }
        else
        {
            std::cout << "Skipping..." << std::endl;
        }

        if(avg.dataSize > 0 && avg.time > 0)
        {
            m_profile.push_back(avg);
        }
    }
}

int Profiler::GetLogTime(const std::string &line, struct tm &time)
{
    // 638:Sep  6 07:36:54 ......

    int rc = -1;
    unsigned int pos = line.find(":");        // Skip Log id

    if(std::string::npos != pos && pos < line.size())
    {
//       std::cout << "Found ':' at " << pos << std::endl;
        std::string month = line.substr(pos+1, 3);
//        std::cout << "Month read as '" << month << "'" << std::endl;
        for(unsigned int m = 0; m < m_months.size(); ++m)
        {
            if(month == m_months[m])
            {
                time.tm_mon = m; // tm_mon is 0 based.
//                std::cout << "Month # set to " << time.tm_mon << std::endl;
                break;
            }
        }
        
        std::string day = line.substr(pos+4, 4);
        day.erase(day.begin(),
                  std::find_if(day.begin(), day.end(),
                               std::bind1st(std::not_equal_to<char>(), ' ')));
        day.erase(std::find_if(day.rbegin(), day.rend(),
                               std::bind1st(std::not_equal_to<char>(), ' ')).base(),
                  day.end());
        time.tm_mday = strtoul(day.c_str(), NULL, 10);
//        std::cout << "Day # set to " << time.tm_mday << std::endl;
    }

    // Hardcode the year for now.
    time.tm_year = m_year;
    
    if(std::string::npos != pos)
    {
 //       std::cout << "Found ':' at " << pos << std::endl;
        pos = line.find(":", pos + 1);   // Find hour
        if(std::string::npos != pos)
        {
//            std::cout << "Found next ':' at " << pos << std::endl;

            time.tm_hour = strtoul(line.substr(pos - 2, 2).c_str(), NULL, 10);
//            std::cout << "Hour is " << time.tm_hour << std::endl;
            pos = line.find(":", pos + 1);
//            std::cout << "Found next ':' at " << pos << std::endl;
            time.tm_min = strtoul(line.substr(pos - 2, 2).c_str(), NULL, 10);
//            std::cout << "Minute is " << time.tm_min << std::endl;
            time.tm_sec = strtoul(line.substr(pos + 1, 2).c_str(), NULL, 10);
//            std::cout << "Second is " << time.tm_sec << std::endl;
            rc = 0;
        }
    }
    return rc;
}


void Profiler::Map()
{
    std::stringstream ss;
    int    y = 0;
    time_t endTime = mktime(&m_profile[m_profile.size()-1].log);
    time_t startTime = mktime(&m_profile[0].log);

    std::cout << "startTime: " << startTime << " endTime: " << endTime << std::endl;

    ss << "ClockTime |-------------------------------> Processing Time" << std::endl;

    for(int t = startTime; t <= endTime; ++t)
    {
        if(t != mktime(&m_profile[y].log))
        {
            ss << "          |";
        }
        else
        {
            ss << std::setw(2) << std::left << m_profile[y].log.tm_hour << ":"
                << std::setw(2)  << std::left << m_profile[y].log.tm_min << ":"
                << std::setw(2)  << std::left << m_profile[y].log.tm_sec << "  |";

            ss << std::setfill('-') << std::setw(m_profile[y].time / 70) << ">" << std::endl;
            ++y;
        }
    }

    ss << "          |" << std::setfill('_') << std::setw(70) << "_" << std::endl;
    std::cout << ss.str();
}


int main(int argc, char *argv[])
{
    // 1. Parse CLI arguments, pass in log file.
    std::string filename = "";
    if(argc == 2)
    {
        filename = argv[1];
    }
    else
    {
        std::cout << "Usage: profiler [log_filename]" << std::endl;
    }

    Profiler profiler(filename);

    profiler.Parse();
    profiler.Map();

    return 0;
}
