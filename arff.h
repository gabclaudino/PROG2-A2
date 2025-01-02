#ifndef __ARFF__
#define __ARFF__

#define MAX 1024
#define MAX2 2048

// struct para salvar cada linha de atributo
typedef struct {
  char *rotulo;
  char *tipo;
  char *categorias[MAX];
  int tam_cat;
} atributo;

// struct para salvar em qual linha estao cada rotulo no arquivo .arff
typedef struct {
  int PKT_CLASS;
  int SRC_ADD;
  int PKT_AVG_SIZE;
} dados;

// struct para salvar os enderessos e quantas vezes eles aparecem
typedef struct {
  char ip[30];
  int qnt;
} enderesso;

// struct para salvar as quantidades de ocorrencias de cada tipo de ataque
// e a media do tamanho dos pacotes
typedef struct{
  int ocorrencias[MAX];
  double media[MAX];
} ataque;

int eh_numero(char *str);

int esta_presente(char *str, char *vetor[], int tam);

void exibe_atributos(atributo *infos, int quantidade);

int conta_atributos(FILE *arff);

void processa_categorias(atributo *elemento, char *categorias, int j);

atributo* processa_atributos(FILE *arff, int *quantidade, dados *info);

void valida_arff(FILE *arff, atributo *atributos, int quantidade);

#endif