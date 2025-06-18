#ifndef COMBINATION_H
#define COMBINATION_H

#include <vector>
#include <string>

#include <cereal/cereal.hpp>
#include <cereal/archives/json.hpp>

class Combination
{
public:
    double Normal;
    double MsdX;
    double MsdY;
    double MsolverXX;
    double MsolverYY;
    bool isMomentValid;

    Combination();
    Combination(double normal, double msdX, double msdY, double MsolverXX, double MsolverYY, bool isValid = true);

    friend class cereal::access;
    template <class Archive>
    void serialize(Archive& archive)
    {
        archive(
            CEREAL_NVP(Normal),
            CEREAL_NVP(MsdX),
            CEREAL_NVP(MsdY),
            CEREAL_NVP(MsolverXX),
            CEREAL_NVP(MsolverYY),
            CEREAL_NVP(isMomentValid)
        );
    }
};

#endif
