#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "arff.h"

#define MAX 1024
#define MAX2 2048

// Funcao que verifica se a uma string eh um numero
// Recebe uma string como parametro
// Retorna 1 se seh um numero, 0 caso contrario
int eh_numero(char *str){
    // Verifica se nao eh uma string vazia
    if(str == NULL || *str == '\0')
        return 0;
    
    // Verifica digito por digito se ha cacteres diferente dos numericos
    int i;
    for(i=0;i<strlen(str);i++)
    {
        if((isdigit(str[i]) == 0 ) && (str[i] != '.') && (str[i] != '-') && (str[i] != '+'))
            return 0;
    }
    // Se nao encontar esses caracteres, entao eh um numero
    return 1;
}

// Funcao para verificar se uma string esta presente num vetor de strings
// Recebe como parametro, a string, o vetor que deseja verificar, e o tamanho do vetor
// Retorna 1 se esta presente, e 0 caso contrario
int esta_presente(char *str, char *vetor[], int tam){
    // Vai comparanto a string com todos os elementos do vetor
    for(int i=0; i<tam; i++)
    {
        if(strcmp(str, vetor[i]) == 0)
            return 1;
    }
    return 0;
    
}

// Fun��o do A1 (com modifica��es para o atributo de categorias)
void exibe_atributos(atributo *infos, int quantidade){
    if (infos == 0){
        printf("O arquivo ARFF fornecido � inv�lido!\n");
        exit(0);
    }
    int j;
    printf("==== ATRIBUTOS DO ARQUIVO ====\n");
    for(int i = 0; i < quantidade; i++){
        printf("-> Atributo #%d\n", i+1);
        printf("R�tulo: %s\n", infos[i].rotulo);
        printf("Tipo: %s\n", infos[i].tipo);
        // Se a o vetor categorias[0] for != de NULL
        // entao eh do tipo categoric
        if (infos[i].categorias[0])
        {
            printf("Categorias: |");
            // Percorre todo o vetor de categorias e vai imprimindo
            for(j=0; j<infos[i].tam_cat; j++)
                printf("%s|", infos[i].categorias[j]);
            printf("\n");
        }
        if (i < quantidade-1) 
            printf("------------------------------\n");
    }
    printf("===============================\n");

}


// Funcao para contar quantos atributos tem no arquivo
// Recebe como parametro um ponteiro para arquivo
// Retorna quantas linhas de atributos VALIDOS tem no arquivo
// Se ha ma linha de attributos invalida, ira acusar ERRO
int conta_atributos(FILE *arff){
    //Fun��o do A1
    // var para contar quantos atributos
    int qnt = 0;
    // vetor para pegar as linhas do arquivo
    char buffer[MAX];
    // variável para verificar se encontrou "@data"
    int encontrouData = 0;

    // enquanto conseguir ler uma linha realiza as operacoes
    while (fgets(buffer, sizeof(buffer), arff)) 
    {
        // verifica se a linha está em branco
        if (strcmp(buffer, "\n") == 0)
            continue;

        // divide a linha em tokens
        // retira o @attribute
        char *token = strtok(buffer, " \n");
        // verifica se a linha começa com @attribute
        if (strcmp(token, "@attribute") == 0) 
        {
            // retira o rotulo
            token = strtok(NULL, " ");
            // verifica se nao falta o rotulo
            if (token != NULL) 
            {   
                // retira o tipo
                token = strtok(NULL, " ");
                // verifica se nao falta o tipo
                if (token != NULL) 
                {
                    qnt++;
                    token = strtok(NULL, " ");
                    if(token != NULL)
                    {
                        printf("Exesso de argumentos na linha: %s!\n", buffer);
                        exit(5);
                    }

                } 
                // se falta o tipo, da exit e retorna 6
                else 
                {
                    printf("Tipo faltando na linha: %s!\n", buffer);
                    exit(6);
                }
            } 
            // se falta o rotulo, da exit e retorna 7
            else 
            {
                printf("Rotulo faltando na linha: %s!\n", buffer);
                exit(7);
            }
        }
        // se nao for @attribute, verifica se eh @data
        // entao encerra o while
        else if(strcmp(token, "@data") == 0)
        {
            encontrouData = 1;
            break;
        }
        // se nao eh @attribute nem @data, algo esta escrito errado
        // entao da exit e retorna 8 
        else 
        {
            printf("@attribute escrito de forma errada na linha: %s!\n", buffer);
            exit(8);
        }
    }


    // faz a verificacao se ha @data no fim do arquivo
    // se nao, da exit e retorna 9 
    if (!encontrouData) {
        printf("Nao foi encontrado @data no final do arquivo!\n");
        exit(9);
    }
    return qnt;
}

// Funcao para processar as categorias em um vetor
// Recebe como parametros o vetor de atributos, a string de categorias, e a linha a qual o atributo pertence
void processa_categorias(atributo *elemento, char *categorias, int j){
    // Se a string foir menor que 2, significa que eh nula ou invalida
    if(strlen(categorias) <= 2)
    {
        printf("Erro, categoria nula!\n");
        exit(10);
    }

    // Verifica se ha "{}" no comeco e no final da string
    if((categorias[0] != '{') || (categorias[strlen(categorias) - 1] != '}'))
    {
        printf("Erro na formatacao das categorias\n");
        exit(20);
    }

    // retira as chaves
    categorias[strlen(categorias) - 1] = '\0';
    categorias[0] = ',';
    char *token;
    int i = 0;
    token = strtok(categorias, ",");
    if(token == NULL)
    {
        printf("Erro, categoria nula!\n");
        exit(10);
    }
    // enquanto o token for diferente de NULL
    while(token)
    {   
        // salva o token no vetor
        elemento[j].categorias[i] = strdup(token);
        // aumenta o tamanho do vetor
        elemento[j].tam_cat++;
        // passa pra proxima posicao do vetor
        i++;
        // divide novamente a string
        token = strtok(NULL, ",");
    }
    // libera a string passada no parametro
    free(categorias);
}

atributo* processa_atributos(FILE *arff, int *quantidade, dados *info){
    //Fun��o do A1 (com modifica��es para o atributo de categorias)
    // vetor para pegar as linhas do arquivo
    char buffer[MAX];
    // vetor de atributos
    atributo *vetoratributo = NULL;
    // variavel para realizar os reallocs
    int ini = 1;
    // variavel para percorrer o vetor
    int i = 0;

    // aloca o vetor de atributos
    vetoratributo = (atributo*)malloc(ini * sizeof(atributo));
    if (vetoratributo == NULL) 
    {
        printf("Erro ao alocar memória!\n");
        exit(11);
    }

    // variável para verificar se encontrou "@data"
    int encontrouData = 0;
    vetoratributo[i].tam_cat = 0;

    // enquanto conseguir ler uma linha realiza as operacoes
    while (fgets(buffer, sizeof(buffer), arff)) 
    {
        // verifica se a linha está em branco
        if (strcmp(buffer, "\n") == 0)
            continue;

        // divide a linha em tokens
        // retira o @attribute
        char *token = strtok(buffer, " \n");
        // verifica se a linha começa com @attribute
        if (strcmp(token, "@attribute") == 0) 
        {
            // faz um realloc para aumentar o vetor
            ini++;
            atributo *temp = (atributo *)realloc(vetoratributo, ini * sizeof(atributo));
            if (temp == NULL) 
            {
                printf("Erro ao realocar memória\n");
                free(vetoratributo);
                exit(12);
            }
            vetoratributo = temp;

            // retira o rotulo
            token = strtok(NULL, " ");
            // verifica se nao falta o rotulo
            if (token != NULL) 
            {
                // pega o rotulo
                // guarda na struct de informacoes em qual linha esta cada atributo
                // necessario para a geracao de arquivos
                if(strncmp(token, "PKT_CLASS", 9) == 0)
                    info->PKT_CLASS = i;
                else if(strncmp(token, "SRC_ADD", 7) == 0)
                    info->SRC_ADD = i;
                else if(strncmp(token, "PKT_AVG_SIZE", 11) == 0)
                    info->PKT_AVG_SIZE = i;
                for(int j=0; j<i; j++)
                {
                    // verifica se nao ha rotulos repetidos no arquivo
                    if(strcmp(token, vetoratributo[j].rotulo) == 0)
                    {
                        printf("Erro, atributos repetidos\n");
                        exit(21);
                    }
                }

                // atribuiu o rotulo
                vetoratributo[i].rotulo = strdup(token);

                // pega o tipo
                token = strtok(NULL, " \n");
                // verifica se nao falta o tipo
                if (token != NULL) 
                {
                    // verifica se eh tipo string ou numeric
                    if (strncmp(token, "string", 6) == 0 || strncmp(token, "numeric", 7) == 0) 
                    {
                        vetoratributo[i].tipo = strdup(token);
                        vetoratributo[i].categorias[0] = NULL;
                    }
                    // se nao, eh do tipo categoric 
                    else 
                    {
                        vetoratributo[i].tipo = strdup("categoric");
                        processa_categorias(vetoratributo, strdup(token), i);
                    }
                    // aumenta o tamanho do vetor
                    i++;
                    // verifica se ha tokens extras, se sim da exit retorna 10
                    token = strtok(NULL, " ");
                    if(token != NULL)
                    {
                        printf("Exesso de argumentos na linha: %s!\n", buffer);
                        free(vetoratributo);
                        exit(5);
                    }

                } 
                // se falta o tipo, da exit e retorna 6
                else 
                {
                    printf("Tipo faltando na linha: %s!\n", buffer);
                    free(vetoratributo);
                    exit(6);
                }
            } 
            // se falta o rotulo, da exit e retorna 7
            else 
            {
                printf("Rotulo faltando na linha: %s!\n", buffer);
                free(vetoratributo);
                exit(7);
            }
        }
        // se nao for @attribute, verifica se eh @data
        // entao encerra o while
        else if(strcmp(token, "@data") == 0)
        {
            encontrouData = 1;
            break;
        }
        // se nao eh @attribute nem @data, algo esta escrito errado
        // entao da exit e retorna 8 
        else 
        {
            printf("@attribute escrito de forma errada na linha: %s!\n", buffer);
            free(vetoratributo);
            exit(8);
        }
    }


    // faz a verificacao se ha @data no fim do arquivo
    // se nao, da exit e retorna 9 
    if (!encontrouData) {
        printf("Nao foi encontrado @data no final do arquivo!\n");
        free(vetoratributo);
        exit(9);
    }

    // a var passada por referencia recebe i, que eh o tamanho do vetor
    *quantidade = i;
    // retorna o vetor
    return vetoratributo;
}

// Funcao para validar um arquivo do fipo .arff
// Recebe como parametros um ponteiro par arquivo, um vetor de atributos e quantidade de atributos
void valida_arff(FILE *arff, atributo *atributos, int quantidade){
    // buffer para ler a linha de dados
    char buffer[MAX2];
    int i = 0;
    // pula o ponteiro do arquivo para depois do @data
    while (fgets(buffer, sizeof(buffer), arff)) 
    {
        if (strcmp(buffer, "\n") == 0)
            continue;
        if(strncmp(buffer, "@data", 5) == 0)
            break;
    }
    // comeca aleitura dos dados
    while (fgets(buffer, sizeof(buffer), arff))
    {
        if (strcmp(buffer, "\n") == 0)
            continue;
        // var para contar quantas colunas
        int qnt = 0;
        i = 0;
        // divide a linha em tokens
        char *token = strtok(buffer, ",\n");
        // enquanto os tokens lidos forem != de NULL
        while (token)
        {   
            // verifica se ja nao tem mais colunas de dados que linahs de atributos
            if(i < quantidade)
            {    
                // se a coluna for referente ao tipo categoric
                // verifica se o dado esta presente no vetor de categorias
                // se nao retorna erro
                if(strncmp(atributos[i].tipo, "categoric", 9) == 0)
                {
                    // chama a funcao esta_presente para a verificacao
                    if(esta_presente(token, atributos[i].categorias, atributos[i].tam_cat) == 0)
                    {
                        printf("Erro, categoria nao definida na linha atributos");
                        exit(18);
                    }
                }
                // se a coluna for referente ao tipo numeric
                // verifica se o dado eh um numero
                // se nao retorna erro
                else if(strncmp(atributos[i].tipo, "numeric", 7) == 0)
                {
                    // chama a funcao eh_numero
                    if(eh_numero(token) == 0)
                    {
                        printf("Erro, dado: '%s' nao confere com o tipo: '%s'\n", token, atributos[i].tipo);
                        exit(19);
                    }
                }
            }
            // incrementa a var para quantidade de dados
            qnt++;
            // incremeta a var para percorrer o vetor de atributos
            i++;
            // retira uma um token
            token = strtok(NULL, ",\n");
        }
        // se a quantidade de colunas de dados for diferente da quantidade de linhas de atributos
        // entao ha algo de errado
        if(qnt != quantidade)
        {
            printf("arquivo com erro nos dados!\n");
            exit(16);
        }
    }
}