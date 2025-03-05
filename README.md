# UFU-EngCivil-IC-MomentoResistente
 Repositório UFU-EngCivil-IC-MomentoResistente

---
# Regras de negócio

## Validação de Entrada de Dados

### Seção Transversal
 - A seção transversal deve formar um polígono fechado e convexo
 - Não deve haver vértices duplicados ou colineares
 - Área mínima da seção deve ser maior que zero
 - Ordem dos vértices deve ser consistente (horária ou anti-horária)
 - Validar coordenadas dentro de limites razoáveis (prevenir erros de entrada)

### Materiais
 - Valores de resistência do concreto (fck) devem estar dentro de intervalos normalizados (ex: 20 MPa a 90 MPa)
 - Módulo de elasticidade do concreto deve ser calculado conforme norma técnica
 - Resistência característica do aço (fyk) deve ter valores padrão (ex: 500 MPa, 600 MPa)
 - Diagrama tensão-deformação deve seguir modelos normatizados (parábola-retângulo, etc.)
 - Coeficientes de segurança para materiais devem seguir normas técnicas (γc, γs)

### Armaduras
 - Diâmetros das barras devem corresponder a valores comerciais disponíveis
 - Armaduras devem estar posicionadas dentro da seção de concreto
 - Espaçamento mínimo entre armaduras deve ser respeitado
 - Cobrimento mínimo de concreto deve ser verificado
 - Área de armadura total deve respeitar limites mínimos e máximos (taxas de armadura)

### Esforços
 - Valores de esforço normal devem estar dentro de limites compatíveis com a capacidade da seção
 - Combinações de esforços devem ser coerentes fisicamente

---
## Cálculos e Algoritmos

### Estado Limite Último
 - Seções planas permanecem planas (Hipótese de Bernoulli)
 - Deformação máxima do concreto limitada (3,5‰ ou valor normativo)
 - Deformação limite do aço (10‰ ou valor normativo)
 - Resistência do concreto à tração desprezada
 - Perfeita aderência entre concreto e aço

### Algoritmo de Cálculo
 - Implementar método iterativo para encontrar a posição da linha neutra
 - Critério de convergência bem definido (tolerância para equilíbrio de forças)
 - Número máximo de iterações para evitar loop infinito
 - Tratamento de casos especiais (seções totalmente comprimidas/tracionadas)
 - Verificação de domínios de deformação conforme normas técnicas

### Equilíbrio
 - Forças internas devem equilibrar forças externas
 - Somatório de forças normais internas deve igualar o esforço normal externo aplicado
 - Momento resistente calculado deve garantir o equilíbrio da seção

---
## Verificações e Alertas

### Limitações Normativas
 - Verificar taxas de armadura (mínima e máxima)
 - Alertar sobre dimensões não usuais da seção
 - Verificar esbeltez da seção (quando aplicável)
 - Alertar sobre posicionamento inadequado das armaduras

### Verificações de Processamento
 - Alertar quando não houver convergência do algoritmo
 - Notificar quando a capacidade resistente da seção for excedida
 - Indicar quando as deformações ultrapassarem limites normativos
 - Avisar sobre casos de instabilidade numérica

---
## Saída e Relatórios

### Resultados
 - Fornecer momento resistente calculado para o esforço normal dado
 - Apresentar distribuição de tensões na seção
 - Mostrar deformações no concreto e nas armaduras
 - Indicar o domínio de deformação em que a seção se encontra

### Relatórios
 - Gerar relatório detalhado com dados de entrada e resultados
 - Incluir diagramas de distribuição de tensões e deformações
 - Fornecer resumo dos cálculos e iterações realizadas
 - Apresentar verificações normativas realizadas

---
## Persistência de Dados

### Salvamento de Projetos
 - Verificar integridade dos dados antes de salvar
 - Validar formato e extensão do arquivo
 - Implementar controle de versões básico (data e hora)
 - Garantir que todos os dados necessários sejam salvos

### Carregamento de Projetos
 - Verificar compatibilidade da versão do arquivo
 - Validar dados carregados antes de processá-los
 - Implementar recuperação de falhas no carregamento

---
## Interface e Usabilidade

### Edição de Dados
 - Bloquear cálculos se dados obrigatórios estiverem faltando
 - Validar dados em tempo real durante a entrada
 - Permitir desfazer/refazer operações de edição

### Visualização
 - Exibir escala automática para a seção transversal
 - Destacar visualmente posição da linha neutra
 - Representar graficamente as tensões e deformações

--- 
# Estrutura desejada de pastas do projeto
 ```
 /core
 |-- Geometria.h/.cpp
  - Cálculos geométricos
  - Operações com pontos/vetores
  - Verificações de polígonos

 |-- SecaoTransversal.h/.cpp
  - Representação geométrica da seção
  - Cálculos de propriedades geométricas
  - Verificação de validade da seção

 |-- Armadura.h/.cpp
  - Modelagem das armaduras
  - Propriedades individuais de cada barra de aço
  - Cálculo de tensões e deformações

 |-- Material.h/.cpp
  - Propriedades do concreto
  - Propriedades do aço
  - Relações constitutivas
  - Cálculo de resistências de cálculo

 |-- CalculadorMomentoResistente.h/.cpp
  - Algoritmo da falsa posição
  - Métodos de equilíbrio de forças
  - Cálculo iterativo da linha neutra
  - Verificação de convergência
  - Cálculo do momento resistente

 |-- EstadoLimiteUltimo.h/.cpp
  - Hipóteses normativas
  - Verificações de domínios de deformação
  - Critérios de ruína

 /ui
 |-- MainWindow.h/.cpp
  - Janela principal
  - Layout geral da aplicação

 |-- SecaoTransversalWidget.h/.cpp
  - Desenho da seção transversal
  - Edição interativa de vértices
  - Feedback visual de propriedades

 |-- ArmaduraWidget.h/.cpp
  - Configuração de armaduras
  - Visualização do posicionamento
  - Edição de diâmetros e coordenadas

 |-- MaterialWidget.h/.cpp
  - Entrada de propriedades de materiais
  - Seleção de tipos de concreto/aço
  - Visualização de parâmetros

 |-- ResultadosWidget.h/.cpp
  - Exibição de resultados de cálculo
  - Gráficos de deformações
  - Relatórios detalhados

 /io
 |-- GerenciadorProjeto.h/.cpp
  - Salvar/Carregar configurações
  - Serialização de dados
  - Controle de versões de projeto

 |-- ExportadorRelatorio.h/.cpp
  - Geração de PDFs
  - Exportação para planilhas
  - Criação de relatórios técnicos

 |-- ImportadorDados.h/.cpp
  - Leitura de arquivos de configuração
  - Importação de dados de outras fontes
 ```

