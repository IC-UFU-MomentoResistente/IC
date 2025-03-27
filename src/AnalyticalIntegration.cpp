#include "AnalyticalIntegration.h"

AnalyticalIntegration::AnalyticalIntegration()
{
    integrationVersion = NormativeIntegrationVersion::ABNT_NBR6118_2014;
}

void AnalyticalIntegration::setVersion(NormativeIntegrationVersion modelVersion)
{
    integrationVersion = modelVersion;
}

double AnalyticalIntegration::computeNormalConcreteRectangular(double multFcd, double fcd, double coef1, double coef2, double coordY)
{
    switch (integrationVersion)
    {
        case NormativeIntegrationVersion::ABNT_NBR6118_2014:
        {
            double nctr = multFcd * fcd * ((coef1 * coordY) + (coef2 * coordY * coordY / 2));

            return nctr * 1000; // kN
            break;
        }

        case NormativeIntegrationVersion::ABNT_NBR6118_2023:
        {
            return 0;
            break;
        }

        default:
        {
            return 0;
        }
    }
}

double AnalyticalIntegration::computeNormalConcreteParabolic(double coordLN, double coordEpc2, double nConc, double fcd,
                                                             double multFcd, double coef1, double coef2, double coordY)
{
    switch (integrationVersion)
    {
        case NormativeIntegrationVersion::ABNT_NBR6118_2014:
        {
            double g = coordLN / 100;                   // m
            double xEpc2 = (coordEpc2 - coordLN) / 100; // m
            double n1 = nConc + 1;
            double n2 = nConc + 2;
            double n3 = nConc + 3;
            double eexp = pow(((g + xEpc2 - coordY) / xEpc2), n1);

            double nctp = -multFcd * fcd *
                        (-((xEpc2 * eexp * (coef1 * n2 + coef2 * (g + xEpc2 + nConc * coordY + coordY))) / (n1 * n2)) -
                        coef1 * coordY - (coef2 * coordY * coordY) / 2);

            return nctp * 1000; // kN
            break;
        }
        case NormativeIntegrationVersion::ABNT_NBR6118_2023:
        {
            return 0;
            break;
        }

        default:
        {
            return 0;
        }
    }
}

double AnalyticalIntegration::computeMomentConcreteRectangular(double multFcd, double fcd, double coef1, double coef2, double coordY)
{
    switch (integrationVersion)
    {
        case NormativeIntegrationVersion::ABNT_NBR6118_2014:
        {
            double mctr = multFcd * fcd * ((coef1 * coordY * coordY / 2) + (coef2 * coordY * coordY * coordY / 3));

            return mctr * 1000; // kN.m
            break;
        }
        case NormativeIntegrationVersion::ABNT_NBR6118_2023:
        {
            return 0;
            break;
        }
        default:
        {
            return 0;
        }
    }
}

double AnalyticalIntegration::computeMomentConcreteParabolic(double coordLN, double coordEpc2, double nConc, double fcd,
                                                             double multFcd, double coef1, double coef2, double coordY)
{
    switch (integrationVersion)
    {
        case NormativeIntegrationVersion::ABNT_NBR6118_2014:
        {
            double g = coordLN / 100;                   // m
            double xEpc2 = (coordEpc2 - coordLN) / 100; // m
            double n1 = nConc + 1;
            double n2 = nConc + 2;
            double n3 = nConc + 3;
            double eexp = pow(((g + xEpc2 - coordY) / xEpc2), n1);

            double mctp = (multFcd * fcd * (3 * coef1 * ((n1) * (n2) * (n3)*coordY * coordY + 2 * xEpc2 * eexp * (((g) + xEpc2) * (n3) + (3 + 4 * nConc + nConc * nConc) * coordY)) + 2 * coef2 * ((n1) * (n2) * (n3)*coordY * coordY * coordY + 3 * xEpc2 * eexp * (2 * (g) * (g) + 2 * xEpc2 * xEpc2 + 2 * xEpc2 * (n1)*coordY + (2 + 3 * nConc + nConc * nConc) * coordY * coordY + 2 * (g) * (2 * xEpc2 + (n1)*coordY))))) / (6 * (n1) * (n2) * (n3));

            return mctp * 1000;

            break;
        }
        case NormativeIntegrationVersion::ABNT_NBR6118_2023:
        {
            return 0;
            break;
        }
        default:
        {
            return 0;
        }
    }
}