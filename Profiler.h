/*
 * Profiler.h
 *
 *  Created on: Sep 5, 2019
 *      Author: st1016431
 */

#ifndef PROFILER_H_
#define PROFILER_H_

#include <fstream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <vector>


class Profiler
{
public:
    Profiler(const std::string& logfile);
    virtual ~Profiler();

    void Parse();
    void Map();

private:
    time_t GetLogTime(const std::string &line);

private:

    struct MapData
    {
        unsigned long dataSize;
        time_t        time;
        time_t        log;

        MapData() { Reset(); };
        void Reset() {
            dataSize = 0;
            time = 0;
            log = 0;
        };
    };

//    std::map<struct tm, MapData> m_profileMap;
    std::vector<MapData> m_profile;
    std::fstream m_file;
    std::vector<std::string> m_months;
    const struct tm *m_timeInfo;
};

#endif /* PROFILER_H_ */
