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
    if(fck <= 40) 
    {
       nc = 1,0;
    }
    
    else 
    {
        nc = (pow(40/fck,1/3));
    }

    if (fck <= 50) 
    // ATÉ CLASSE C50
    {
        fator_mult_tensao_comp_concreto = 0.85;
        epsilon_concreto_ultimo = 3.5; // c50 ecu
        expoente_tensao_concreto = 2; // n = 2
        epsilon_concreto_2 = 2; // c50 ec2
        

    } else {
        // fck > C50
        fator_mult_tensao_comp_concreto = 0.85 * (1 - (fck - 50) / 200); 
        epsilon_concreto_ultimo = 2.6 + 35 * pow((90 - fck) / 100, 4); // ecu classes c55 até c90 
        expoente_tensao_concreto = 1.4 + 23.4 * pow((90 - fck) / 100, 4); // EXPOENTE PARA FCK > 50 MPa 
        epsilon_concreto_2 = 2 + 0.085 * pow(fck - 50, 0.53); // EC2 para concretos de classe C55 ATÉ C90

        if (epsilon_concreto_2 > epsilon_concreto_ultimo) {
            epsilon_concreto_2 = epsilon_concreto_ultimo;
        }
    }
    fcd = fck / gama_c;
    parametros.tensaoCompressao = calculaCompressao(nc, fcd, epsilon_c, epsilon_concreto_2, expoente_tensao_concreto);
    
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

float Concreto::calculaCompressao(float nc, float fcd, float epsilon_c, float epsilon_concreto_2, float expoente_tensao_concreto) 
{
    parametros.tensaoCompressao = nc * fcd * (1 - std::pow((1 - (epsilon_c / epsilon_concreto_2)), expoente_tensao_concreto));
    return parametros.tensaoCompressao;
}