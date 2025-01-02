#ifndef __LOG__
#define __LOG__

#define MAX 1024
#define MAX2 2048


void gera_ataques(FILE *arff, atributo *elemento, dados info);

void gera_classificacao(FILE *arff, atributo *elemento, dados info);

void gera_blacklist(FILE *arff, atributo *elemento, dados info);

void gera_media(FILE *arff, atributo *elemento, dados info);




#endif
