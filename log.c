#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "arff.h"
#include "log.h"

#define MAX 1024
#define MAX2 2048

// Funcao para gerar o relatorio de ataques
// Recebe como parametro um ponteiro para arquivo, um vetor de atributos,
// e as informacoes de em qual linahs entao localizadas os Rotulos necessarios
void gera_ataques(FILE *arff, atributo *elemento, dados info){
    // verifica se ha o rotulo PKT_CLASS que informa o tipo do ataque
    if(info.PKT_CLASS == -1)
    {
        printf("Sem o rotulo PKT_CLASS!\n");
        free(elemento);
        exit(14);
    }

    // variavel do tipo atk
    // serve para marcar as ocorrencias de cara ataque
    ataque atk;
    // buffer para ler a linha de dados
    char buffer[MAX2];
    int i;
    // variavel para salvar em qual linha esta o atributo referente ao tipo de ataque
    int pkt = info.PKT_CLASS;

    // zera todas as ocorrencias de ataque
    for(i=0; i<elemento[pkt].tam_cat; i++)
        atk.ocorrencias[i] = 0;

    // joga o ponteiro do arquivo para depois do @data
    while (fgets(buffer, sizeof(buffer), arff)) 
    {
        if (strcmp(buffer, "\n") == 0)
            continue;
        if(strncmp(buffer, "@data", 5) == 0)
            break;
    }
    // comeca a leitura das linhas
    while (fgets(buffer, sizeof(buffer), arff))
    {
        int j = 0;
        int k;
        // verifica se a linha está em branco
        if (strcmp(buffer, "\n") == 0)
            continue;
        // divide a linha em tokens
        char *token = strtok(buffer, ",\n");
        // enquanto for um token diferente de NULL vai realizando as operacoes
        while(token)
        {   
            // se estiver na coluna referente a PKT_CLASS
            // vai incrementando o numero de ocorrencias de cada tipo de ataque
            if (j == pkt)
            {  
                // verifica se qual das categorias o ataque pertence
               for(k=0; k<elemento[pkt].tam_cat; k++)
               {
                    if(strcmp(elemento[pkt].categorias[k], token) == 0)
                    {
                        atk.ocorrencias[k] = atk.ocorrencias[k] + 1;
                    }
               } 
            }
            // le a proxima coluna
            token = strtok(NULL, ",\n");
            j++;
        }
    }

    // gera o arquivo de relatorio
    FILE *arq = fopen("R_ATAQUES.txt", "w+");
    if(!arq)
    {
        printf("Erro ao gerar o arquivo!\n");
        exit(17);
    }
    // faz a impressao no arquivo
    // se for "Normal", nao imprime
    for(i=0; i<elemento[pkt].tam_cat; i++)
    {
        if(strcmp(elemento[pkt].categorias[i], "Normal") != 0)
        {
            fprintf(arq, "%s;%d\n", elemento[pkt].categorias[i], atk.ocorrencias[i]);
        }
    }

    // retorna o ponteiro do arquivo gerado para o inicio
    rewind(arq);
    // fecha o arquivo
    fclose(arq);
}

// Funcao para gerar o relatorio dos tipo de enderessos
// Recebe como parametro um ponteiro para arquivo, um vetor de atributos,
// e as informacoes de em qual linhas entao localizadas os Rotulos necessarios
void gera_classificacao(FILE *arff, atributo *elemento, dados info){
    // Verifica se ha no arquivo os rotulos necessarios
    if(info.PKT_CLASS == -1)
    {
        printf("Sem o rotulo PKT_CLASS!\n");
        free(elemento);
        exit(14);
    }
    if(info.SRC_ADD == -1)
    {
        printf("Sem o rotulo SRC_ADD\n");
        free(elemento);
        exit(15);
    }

    // var para salvar a linha qual o atributo SRC_ADD esta
    int add = info.SRC_ADD;
    // var para salvar a linha qual o atributo PKT_CLASS esta
    int pkt = info.PKT_CLASS;
    // buffer para ler a linha de dados
    char buffer[MAX2];
    // var para incrementar fazer o realloc do vetor de enderessos
    int ini = 1;
    // var do tamanho do vetor de enderessos
    int tamvet = 0;
    // var para guardar o token de enderesso
    char *tokenadd;
    // var apra guardar o token de tipo de ataque
    char *tokenpkt;
    // var para verificar se achou o enderesso no vetor
    int achou = 0;

    // vetor de enderessor
    enderesso *vetoradd = (enderesso *) malloc(sizeof(enderesso) * ini);

    // joga o ponteiro do arquivo para depois do @data
    while (fgets(buffer, sizeof(buffer), arff)) 
    {
        if (strcmp(buffer, "\n") == 0)
            continue;
        if(strncmp(buffer, "@data", 5) == 0)
            break;
    }

    // le as linhas de dados
    while (fgets(buffer, sizeof(buffer), arff))
    {
        if (strcmp(buffer, "\n") == 0)
            continue;
        int i = 0;
        // divide a linha em tokens
        char *token = strtok(buffer, ",\n");
        while (token)
        {   
            // verifica se o token eh referente a linha de SRC_ADD
            if (i == add)
            {
                tokenadd = strdup(token);
            }
            // verifica se o token eh referente a linha e PKT_CLASS
            else if(i == pkt)
            {
                tokenpkt = strdup(token);
            }
            // retira outro token
            token = strtok(NULL, ",\n");
            i++;
        }


        // faz a verificacao se o enderesso lido ja paraceu antes
        achou = 0;
        for(i=0; i<tamvet; i++)
        {
            // se sim, aumenta o numero de ocorrencias do mesmo enderreso
            if(strcmp(vetoradd[i].ip, tokenadd) == 0)
            {
                achou = 1;
                if(strcmp(tokenpkt, "Normal") != 0)
                    vetoradd[i].qnt++;
                break;
            }
        }
        // se nao, adiciona no vetor
        if(achou == 0)
        {
            // faz o realloc para aumentar o tamanho do vetor
            ini++;
            vetoradd = (enderesso *)realloc(vetoradd, ini * sizeof(enderesso));
            strcpy(vetoradd[tamvet].ip, tokenadd);
            // verifica se o tipo de ataque eh "Normal"
            if(strcmp(tokenpkt, "Normal") != 0)
                vetoradd[tamvet].qnt = 1;
            else
                vetoradd[tamvet].qnt = 0;
            tamvet++;
        }

        // libera a memoria alocada pelo strdup
        free(tokenadd);
        free(tokenpkt);
    }

    // gera o arquivo de relatorio dos tipo de cada enderesso
    FILE *arq = fopen("R_ENTIDADES.txt", "w+");
    if(!arq)
    {
        printf("Erro ao gerar o arquivo!\n");
        exit(17);
    }

    // faz a verificacao de quantos ataques cada enderresso fez e esqueve no arquivo
    int i;
    for(i=0;i<tamvet;i++)
    {   
        if(vetoradd[i].qnt == 0)
            fprintf(arq, "%s;benigna\n", vetoradd[i].ip);
        else if(vetoradd[i].qnt <= 5)
            fprintf(arq, "%s;potencialmente maliciosa\n", vetoradd[i].ip);
        else
            fprintf(arq, "%s;maliciosa\n", vetoradd[i].ip);
    }

    // libera o vetor de enderessos
    free(vetoradd);
    // retorna o ponteiro do arquivo gerado para o inicio
    rewind(arq);
    // fecha o arquivo
    fclose(arq);
}

// Funcao para gerar o arquivo de enderresos considerados maliciosos
// Recebe como parametro um ponteiro para arquivo, um vetor de atributos,
// e as informacoes de em qual linhas entao localizadas os Rotulos necessarios
void gera_blacklist(FILE *arff, atributo *elemento, dados info){
    // verifica se o arquivo tem os rotulos necessarios
    if(info.PKT_CLASS == -1)
    {
        printf("Sem o rotulo PKT_CLASS!\n");
        free(elemento);
        exit(14);
    }
    if(info.SRC_ADD == -1)
    {
        printf("Sem o rotulo SRC_ADD\n");
        free(elemento);
        exit(15);
    }

    // var para ver em qual linha esta o rotulo SRC_ADD
    int add = info.SRC_ADD;
    // var para ver em qual linha esta o rotulo PKT_CLASS
    int pkt = info.PKT_CLASS;
    // buffer para ler as linhas de dados
    char buffer[MAX2];
    // var para realizar o realloc
    int ini = 1;
    // var para o tamanho do vetor de enderessos
    int tamvet = 0;
    // token para pegar o dado referente a linha de SRC_ADD
    char *tokenadd;
    // token para pegar o dado referente a linha de PKT_CLASS
    char *tokenpkt;
    // var para verificar se o enderesso ja esta presente no vetor de enedressos
    int achou = 0;

    // vetor de enderessos
    enderesso *vetoradd = (enderesso *) malloc(sizeof(enderesso) * ini);
    // joga o ponteiro do arquivo para depois do @data
    while (fgets(buffer, sizeof(buffer), arff)) 
    {
        if (strcmp(buffer, "\n") == 0)
            continue;
        if(strncmp(buffer, "@data", 5) == 0)
            break;
    }

    // le as linahs de dados
    while (fgets(buffer, sizeof(buffer), arff))
    {
        if (strcmp(buffer, "\n") == 0)
            continue;
        int i = 0;
        char *token = strtok(buffer, ",\n");
        while (token)
        {
            // verifica se a coluna de dados eh referente ao atributo SRC_ADD
            if (i == add)
            {
                tokenadd = strdup(token);
            }
            // verifica se a coluna de dados eh referente ao atributo PKT_CLASS
            else if(i == pkt)
            {
                tokenpkt = strdup(token);
            }
            // pega o proximo token
            token = strtok(NULL, ",\n");
            i++;
        }

        achou = 0;
        // verifica se a ocorrencia nao eh do tipo normal
        if(strcmp(tokenpkt, "Normal") != 0)
        {
            // se nao for, salva o enderesso no vetor de enderessos
            for(i=0; i<tamvet; i++)
            {
                if(strcmp(vetoradd[i].ip, tokenadd) == 0)
                {
                    achou = 1;
                    vetoradd[i].qnt++;
                    break;
                }
            }
            if(achou == 0)
            {
                ini++;
                vetoradd = (enderesso *)realloc(vetoradd, ini * sizeof(enderesso));
                strcpy(vetoradd[tamvet].ip, tokenadd);
                vetoradd[tamvet].qnt = 1;
                tamvet++;
            }
        }

        //libera a memoria dos tokens
        free(tokenadd);
        free(tokenpkt);
    }

    // gera o arquivo  de blacklist
    FILE *arq = fopen("BLACKLIST.bl", "w+");
    if(!arq)
    {
        printf("Erro ao gerar o arquivo!\n");
        exit(17);
    }

    //escreve no arquivo apenas os enderessos que tem mais que 5 ocorrencias
    int i;
    for(i=0;i<tamvet;i++)
    {
        if(vetoradd[i].qnt > 5)
            fprintf(arq, "%s\n", vetoradd[i].ip);
    }

    // libera a momoria do vetor de enderessos
    free(vetoradd);
    // retorna o ponteiro do arquivo para o inico
    rewind(arq);
    // fecha o arquivo
    fclose(arq);
}

// Funcao para gerar a media da media do tamanho dos pacotes
// Recebe como parametro um ponteiro para arquivo, um vetor de atributos,
// e as informacoes de em qual linhas entao localizadas os Rotulos necessarios
void gera_media(FILE *arff, atributo *elemento, dados info){
    // faz a verificacaos se os existem os rotulos necessarios no arquivo
    if(info.PKT_AVG_SIZE == -1)
    {
        printf("Sem o rotulo PKT_AVG_SIZE!\n");
        free(elemento);
        exit(13);
    }
    if(info.PKT_CLASS == -1)
    {
        printf("Sem o rotulo PKT_CLASS!\n");
        free(elemento);
        exit(14);
    }

    // buffer para ler as linhas de dados
    char buffer[MAX2];
    // var para salvar em qual linha o atributo esta PKT_CLASS 
    int pkt = info.PKT_CLASS;
    // var para salvar em qual linha o atributo esta PKT_AVG_SIZE
    int avg = info.PKT_AVG_SIZE;
    // token para salvar o dado referente a PKT_CLASS
    char *tokenpkt;
    // token para salvar o dado referente a PKT_AVG_SIZE
    char *tokenavg;
    // var para salvar as ocorrencias do ataque e a media de cada um
    ataque atk;
    int i;

    // zera a media e a ocorrencia de cada tipo de ataque
    for(i=0; i<elemento[pkt].tam_cat; i++)
    {
        atk.media[i] = 0;
        atk.ocorrencias[i] = 0;
    }

    // joga o ponteiro do arquivo para depois do @data
    while (fgets(buffer, sizeof(buffer), arff)) 
    {
        if (strcmp(buffer, "\n") == 0)
            continue;
        if(strncmp(buffer, "@data", 5) == 0)
            break;
    }
    // le as linhas de dados
    while (fgets(buffer, sizeof(buffer), arff))
    {
        int j = 0;
        int k;
        // verifica se a linha está em branco
        if (strcmp(buffer, "\n") == 0)
            continue;

        // divide a linha em tokens
        char *token = strtok(buffer, ",\n");
        while(token)
        {   
            // verifica se a coluna de dados eh referente ao atributo PKT_CLASS
            if (j == pkt)
            {
               tokenpkt = strdup(token);
            }
            // verifica se a coluna de dados eh referente ao atrobuto PKT_AVG_SIZE
            else if(j == avg)
            {
                tokenavg = strdup(token);
            }
            // retira mais um token
            token = strtok(NULL, ",\n");
            j++;
        }
        
        // incrementa o numero de ocorrencias do tipo de ataque
        // soma o tamanho total da media dos tamanhos dos pacote do tipo de taque
        for(k=0; k<elemento[pkt].tam_cat; k++)
        {
            if(strcmp(elemento[pkt].categorias[k], tokenpkt) == 0)
            {
                atk.ocorrencias[k] = atk.ocorrencias[k] + 1;
                atk.media[k] = atk.media[k] + atof(tokenavg);
            }
        }

        // libera os tokens
        free(tokenavg);
        free(tokenpkt);
    }

    // gera o arquivo de tamanho
    FILE *arq = fopen("R_TAMANHO.txt", "w+");
    if(!arq)
    {
        printf("Erro ao gerar o arquivo!\n");
        exit(17);
    }
    // faz a media de cada tipo de ataque, dividindo o tamanho total
    // pela quantidade de ocorrencias
    // imprime no arquivo de for diferente de "Normal"
    for(i=0; i<elemento[pkt].tam_cat; i++)
    {
        if(strcmp(elemento[pkt].categorias[i], "Normal") != 0)
        {
            if(atk.ocorrencias[i] != 0)
                fprintf(arq, "%s;%.5f\n", elemento[pkt].categorias[i], atk.media[i] / atk.ocorrencias[i]);
        }
    }

    // retorna o ponteiro do arquivo para o incio
    rewind(arq);
    // fecha o arquivo
    fclose(arq);

}

