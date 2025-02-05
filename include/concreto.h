#ifndef CONCRETO_H
#define CONCRETO_H

#include <cmath>

class Concreto {
private:
    float fator_mult_tensao_comp_concreto;
    float epsilon_concreto_ultimo;
    float expoente_tensao_concreto;
    float epsilon_concreto_2;
    float epsilon_c;
    float fcd;
    float altura_deformacao_2;
    float altura_deformacao_ultima;
    float altura_LN;
    float nc; 
    float beta; 

public:
    struct ParametrosConcreto {
        float fatorMultTensaoCompConcreto;
        float epsilonConcretoUltimo;
        float expoenteTensaoConcreto;
        float epsilonConcreto2;
        float fcd;
        float tensaoCompressao; 
    };

    ParametrosConcreto parametros; 

    struct AlturasConcreto{
        float altura_deformacao_2;
        float altura_deformacao_ultima;
        float altura_LN;
    };

    Concreto(float fck, float gama_c, float eps1, float eps2, float x_por_d, float d);

    ParametrosConcreto getParametros() const;
    AlturasConcreto getAlturas() const;
    
private:
    void calculaParametros(float fck, float gama_c);
    void calculaAlturaDeformacao (float eps1, float eps2, float x_por_d, float d);
    float calculaCompressao(float beta, float nc, float fcd, float epsilon_c, float epsilon_concreto_2, float expoente_tensao_concreto);
    float deformacaoABNT2023(float fck, float gama_c, float beta, float epsilon_concreto_ultimo, float epsilon_concreto_2);
    float deformacaoABNT2014(float fck, float gama_c, float beta);
    
};

#endif 
