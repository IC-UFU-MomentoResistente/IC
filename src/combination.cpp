#include "Combination.h"

Combination::Combination() : Normal(0.0), MsdX(0.0), MsdY(0.0), Msolver(0.0), isMomentValid(true)
{
}

Combination::Combination(double normal, double msdX, double msdY, double msolver, bool isValid)
    : Normal(normal), MsdX(msdX), MsdY(msdY), Msolver(msolver), isMomentValid(isValid)
{
}