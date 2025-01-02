#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "arff.h"
#include "log.h"

#define HELP "Forma de uso: ./arff -i <arq_in> [-p -v -a -b -c -d]\n"
#define MAX 1024
#define MAX2 2048

int main(int argc, char **argv){
  int opt;
  char exibicao = 0;
  char validacao = 0;
  char ataques = 0;
  char entidades = 0;
  char tamanho = 0;
  char firewall = 0;
  char *entrada = 0;

  while ((opt = getopt(argc, argv, "pvabcdi:")) != -1) {
    switch (opt) {
    case 'i':
      entrada = strdup(optarg);
      break;
    case 'p':
      exibicao = 1;
      break;
    case 'v':
      validacao = 1;
      break;
    case 'a': //Relatório de todos os ataques ocorridos e o número de ocorrências no dataset (nome do arquivo de saída: R_ATAQUES.txt)
      ataques = 1;
      break;
    case 'b': //Relatório de endereços de origem maliciosos, pot. maliciosos e benignos (nome do arquivo de saída: R_ENTIDADES.txt)
      entidades = 1;
      break;
    case 'c': //A média da média do tamanho dos pacotes por cada tipo de ataque, considerando cada origem (nome do arquivo de saída: R_TAMANHO.txt)
      tamanho = 1; 
      break;
    case 'd': //Geração de um arquivo de configuração de iptable para bloquear as origens maliciosas (nome do arquivo de saída: C_IPTABLES.txt)
      firewall = 1;
      break;

    default:
      fprintf(stderr, HELP);
      exit(1);
    }
  }

  if (!entrada){
    fprintf(stderr, HELP);
    exit(2);
  }

  FILE *arquivo = fopen(entrada, "r");
  if (!arquivo){
    fprintf(stderr, "Não foi possível abrir o arquivo informado!\n");
    exit(3);
  }

  int qntd_atributos = conta_atributos(arquivo);
  if (!qntd_atributos){
    fprintf(stderr, "Existem erros no arquivo fornecido!\n");
    exit(4);
  }

  // Variaveis para saber em qual linhas estao casa atributo
  dados infos;
  infos.PKT_AVG_SIZE = 0;
  infos.PKT_CLASS = 0;
  infos.SRC_ADD = 0;

  rewind(arquivo);
  atributo *dados_atributos = processa_atributos(arquivo, &qntd_atributos, &infos);

  if (exibicao){
    exibe_atributos(dados_atributos, qntd_atributos);
  }
  if (validacao){
    rewind(arquivo);
    valida_arff(arquivo, dados_atributos, qntd_atributos);
  }
  if (ataques){
    //Chamar a função de relatórios de ataque;
    rewind(arquivo);
    gera_ataques(arquivo, dados_atributos, infos);
  }
  if (entidades){
    //Chamar a função de relatórios de entidade;
    rewind(arquivo);
    gera_classificacao(arquivo, dados_atributos, infos);
    
  }
  if (tamanho){
    //Chamar a função de relatórios de tamanho;
    rewind(arquivo);
    gera_media(arquivo, dados_atributos, infos);
  }
  if (firewall){
    //Chamar a função de geração de arquivo de firewall.
    rewind(arquivo);
    gera_blacklist(arquivo, dados_atributos, infos);
  }
  
  // Libera a memoria que foi alocada no vetor
  for(int i = 0; i < qntd_atributos; i++){
    free(dados_atributos[i].rotulo);
    free(dados_atributos[i].tipo);
    if (dados_atributos[i].categorias[0])
    {
      for(int j=0; j<dados_atributos[i].tam_cat; j++)
      free(dados_atributos[i].categorias[j]);
    }
  }

  free(dados_atributos);
  fclose(arquivo);


  return 0 ;
}



/*
  RETORNOS:
  0 - arquivo invalido
  1 - erro na forma de execucao
  2 - sem aqruivo de entrada
  3 - erro ao abrir o arquivo
  4 - erro nos atributos fornecidos
  5 - excesso de argumentos na linha de atributos
  6 - falta tipo na linha de atributos
  7 - falta rotulo na linha de atributos
  8 - @attribute escrito errado
  9 - sem @data no final do arquivo
  10 - categoria nula
  11 - erro ao alocar memoria para vetor
  12 - erro ao alocar memoria no realloc
  13 - sem o rotulo PKT_AVG_SIZE
  14 - sem o rotulo PKT_CLASS
  15 - sem o rotulo SRC_ADD
  16 - erro nos dados, mais ou menos colunas
  17 - erro ao gerar o arquivo
  18 - categoria nao definida na linha de attributos
  19 - erro, dado nao confere com o tipo
  20 - erro na formatacao de categorias
  21 - erro, atributos repetidos
*/




