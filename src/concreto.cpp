#include "concreto.h"

Concreto::Concreto (float fck, float gama_c, float eps1, float eps2, float x_por_d, float d) {
    calculaParametros(fck, gama_c);
    calculaAlturaDeformacao(eps1, eps2, x_por_d, d);
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
        altura_LN
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

void Concreto::calculaAlturaDeformacao (float eps1, float eps2, float x_por_d, float d)
{
    altura_LN = x_por_d * d;

    float diferenca_epsilon = eps1 - eps2;
    if (diferenca_epsilon != 0.0f) {
        altura_deformacao_2 = (d / diferenca_epsilon) * epsilon_concreto_2;
        altura_deformacao_ultima = (d / diferenca_epsilon) * epsilon_concreto_ultimo;
    } else {
        altura_deformacao_2 = 0.0f;  
        altura_deformacao_ultima = 0.0f; 
    }
}
