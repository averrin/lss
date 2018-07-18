#ifndef __REPORT_H_
#define __REPORT_H_
#include <string>
#include <map>

struct Report {
    int damageTaken = 0;
    int damageInflicted = 0;
    int damageDeflected = 0;

    int apCommited = 0;

    std::map<std::string, int> kills;
    
};


#endif // __REPORT_H_
