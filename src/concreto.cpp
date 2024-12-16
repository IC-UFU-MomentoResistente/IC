#include <iostream> // Para std::cerr
#include <cfloat>   // Para FLT_MA
#include "poligono.h"
#include "reforco.h"
#include "concreto.h"

Concreto::Concreto (float fck, float gama_c, float eps1, float eps2, float x_por_d, 
                    float yMaxSecao, float yMinSecao, float yMinArmadura, float yCG) {
    calculaParametros(fck, gama_c);
    calculaAlturaDeformacao(eps1, eps2, x_por_d, yMaxSecao, yMinSecao, yMinArmadura, yCG);
}

Concreto::ParametrosConcreto Concreto::getParametros() const {
    return { 
        fator_mult_tensao_comp_concreto, 
        epsilon_concreto_ultimo, 
        expoente_tensao_concreto, 
        epsilon_concreto_2, 
        fcd 
    };
}

Concreto::AlturasConcreto Concreto::getAlturas() const {
    return { 
        altura_deformacao_2,
        altura_deformacao_ultima,
        altura_LN,
        d
    };
}

void Concreto::calculaParametros(float fck, float gama_c) {
    if (fck <= 50) {
        fator_mult_tensao_comp_concreto = 0.85;
        epsilon_concreto_ultimo = 3.5;
        expoente_tensao_concreto = 2;
        epsilon_concreto_2 = 2;
    } else {
        fator_mult_tensao_comp_concreto = 0.85 * (1 - (fck - 50) / 200);
        epsilon_concreto_ultimo = 2.6 + 35 * pow((90 - fck) / 100, 4);
        expoente_tensao_concreto = 1.4 + 23.4 * pow((90 - fck) / 100, 4);
        epsilon_concreto_2 = 2 + 0.085 * pow(fck - 50, 0.53);

        if (epsilon_concreto_2 > epsilon_concreto_ultimo) {
            epsilon_concreto_2 = epsilon_concreto_ultimo;
        }
    }
    fcd = fck / gama_c;
}

void Concreto::calculaAlturaDeformacao (float eps1, float eps2, float x_por_d, 
                                        float yMaxSecao, float yMinSecao, float yMinArmadura, float yCG)
{
    // Calcula 'd' e outras alturas
    d = yMaxSecao - yMinArmadura;
    h_secao = yMaxSecao - yMinSecao;

    altura_LN = yMaxSecao - (((-eps1/1000) / ((eps2/1000) - (eps1/1000))) * h_secao);
    altura_deformacao_2 = yMaxSecao - ((((-epsilon_concreto_2/1000) - (eps1/1000)) / ((eps2/1000) - (eps1/1000))) * h_secao);
    altura_deformacao_ultima = yMaxSecao - ((((-epsilon_concreto_ultimo/1000) - (eps1/1000)) / ((eps2/1000) - (eps1/1000))) * h_secao);
}