#ifndef COMBINATION_H
#define COMBINATION_H

#include <vector>
#include <string>

class Combination
{
public:
    double Normal;
    double MsdX;
    double MsdY;
    double Msolver;
    bool isMomentValid;
    Combination(double normal, double msdX, double msdY, double Msolver, bool isValid = true);
};

#endif
