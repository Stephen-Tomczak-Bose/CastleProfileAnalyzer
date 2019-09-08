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
    int GetLogTime(const std::string &line, struct tm &time);

private:

    struct MapData
    {
        unsigned long dataSize;
        time_t        time;
        struct tm     log;

        MapData() { Reset(); };
        void Reset() {
            dataSize = 0;
            time = 0;
            memset((char *)&log, 0, sizeof(struct tm));
        };
    };

//    std::map<struct tm, MapData> m_profileMap;
    std::vector<MapData> m_profile;
    std::fstream m_file;
    std::vector<std::string> m_months;
    unsigned int m_year;
};

#endif /* PROFILER_H_ */
