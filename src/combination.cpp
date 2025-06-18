#include "Combination.h"

Combination::Combination() : Normal(0.0), MsdX(0.0), MsdY(0.0), MsolverXX(0.0), MsolverYY(0.0), isMomentValid(true)
{
}

Combination::Combination(double normal, double msdX, double msdY, double msolverxx, double msolveryy, bool isValid)
    : Normal(normal), MsdX(msdX), MsdY(msdY), MsolverXX(msolverxx), MsolverYY(msolveryy), isMomentValid(isValid)
{
}