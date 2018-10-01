/*--------------------------------------------------------------
 * UNIFAL – Universidade Federal de Alfenas.
 * Trabalho..: Gerencia de projetos usando lista
 * Disciplina: Estrutura de Dados I
 * Professor.: Luiz Eduardo da Silva
 * Aluno.....: José dos Santos Fernandes
 * Data......: 15/07/2019
 *--------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PORCENTO 100

/*
 * ponteiros
 */
typedef struct nomeNo * PTnome; // pt estrutura nome
typedef struct projNo * PTproj; // pt estrutura projeto
typedef struct no * PTno; // pt no

/*
 * representacao da estrutura Nomes
 */
typedef struct nomeNo {
    char nome[10]; // nome do funcionario

    union {
        int tmp; // tempo OU
        PTno lProj; // lista de projetos
    };
} nomeNo;

/*
 * representacao da estrutura Projetos
 */
typedef struct projNo {
    int id; // identificacao do projeto

    union {
        int tmp; // tempo OU
        PTno lNome; // lista de nomes
    };
} projNo;

/*
 * representacao da lista encadeada
 */
typedef struct no {
    void * info;
    PTno prox;
} no;

/* Funcao geral de insercao numa lista encadeada ordenada geral
 * Os parâmetro são:
 * 1. L = a lista onde inserir
 * 2. N = o novo no a inserir
 * 3. cmp = funcao de comparacao (ID ou NOME)
 * A funcao retorna a lista modificada pela insercao
 */
PTno insere(PTno L, PTno N, int (*cmp)(const void *, const void *)) {
    PTno P = NULL, Q = L;
    while (Q && cmp(Q, N) < 0) {
        P = Q;
        Q = Q->prox;
    }
    if (P) {
        N->prox = P->prox;
        P->prox = N;
    } else {
        N->prox = L;
        L = N;
    }
    return L;
}

/* Funcao que retorna a comparacao de ID de nos
 * Retorna:
 * negativo - se P->info->id MENOR QUE Q->info->id
 * zero - se P->info->id IGUAL Q->info->id
 * positivo - se P->info->id MAIOR QUE Q->info->id
 */
int compId(const void * P, const void * Q) {
    return ((PTproj) ((PTno) P)->info)->id -
            ((PTproj) ((PTno) Q)->info)->id;
}

/* Funcao que retorna a comparacao de NOMES de nos
 */
int compNome(const void * P, const void * Q) {
    return strcmp(((PTnome) ((PTno) P)->info)->nome,
            ((PTnome) ((PTno) Q)->info)->nome);
}

/* Funcao que carrega os nomes e os projetos onde esta alocado
 * Le o numero de pessoas.
 * Para cada pessoa:
 * le o nome e o numero de projetos onde esta alocado
 * Para cada projeto:
 * le a identificacao e o tempo alocado no projeto
Exemplo de entrada de dados:
----------------------------
3
Bruno 3 4 8 3 10 5 5
Daniel 1 5 25
Alan 2 3 15 1 5
 */
PTno carregaDados(void) {
    int id, tmp, nNome, nProj;
    char nome[10];
    PTno lNome = NULL, lProj, aux;
    PTnome N;
    PTproj P;
    scanf("%d", &nNome);
    while (nNome) {
        scanf("%s", nome);
        scanf("%d", &nProj);
        lProj = NULL;
        while (nProj) {
            scanf("%d %d", &id, &tmp);
            P = malloc(sizeof (projNo)); //alocação estrutura projeto
            P->id = id;
            P->tmp = tmp;
            aux = malloc(sizeof (no)); //alocação nó
            aux->info = P;
            lProj = insere(lProj, aux, compId);
            nProj--;
        }
        N = malloc(sizeof (nomeNo)); //alocação nomes
        strncpy(N->nome, nome, 10); //para funcionar no Linux, mudei para strncpy
        N->lProj = lProj;
        aux = malloc(sizeof (no)); //alocação nó
        aux->info = N;
        lNome = insere(lNome, aux, compNome);
        nNome--;
    }
    return lNome;
}

/* Funcao que mostra a lista de Pessoas e os projetos em
 que esta alocado
 Para os dados anteriores, apresenta:
 ------------------------------------
 Alan: [(1,5),(3,15)]
 Bruno: [(3,10),(4,8),(5,5)]
 Daniel: [(5,25)]
 */
void mostraPorNome(PTno Ln) {
    PTnome N;
    PTproj P;
    PTno Lp;
    while (Ln) {
        N = Ln->info;
        printf("%10s: [", N->nome);
        Lp = N->lProj;
        while (Lp) {
            P = Lp->info;
            printf("(%d,%d)", P->id, P->tmp);
            Lp = Lp->prox;
            if (Lp) printf(",");
        }
        printf("]\n");
        Ln = Ln->prox;
    }
    printf("\n");
}

/* Funcao que converte a Lista de Nomes por Projetos numa
 * lista de Projetos por Nomes. */
PTno converte(PTno Ln) {
    PTno Novo = NULL, Lp;
    PTno ptNoProjeto, ptNoNome;
    PTnome N, noNome;
    PTproj P, noProjeto;
    // lista de nomes
    while (Ln) {
        Lp = ((PTnome) Ln->info)->lProj; // lista de projetos
        while (Lp) {
            noNome = malloc(sizeof (nomeNo)); //alocação estrutura nome
            strcpy(noNome->nome, ((PTnome) Ln->info)->nome); //seta nome
            noNome->tmp = ((PTproj) Lp->info)->tmp; //seta tempo
            ptNoNome = malloc(sizeof (no)); //alocação nó de nome
            ptNoNome->info = noNome; //seta o ponteiro info para estrutura nome alocada
            P = malloc(sizeof (projNo)); // alocação da estrutura de projeto
            P->id = ((PTproj) Lp->info)->id; // setando id do projeto
            P->lNome = insere(P->lNome, ptNoNome, compNome); // inserindo o no nome na lista de nomes
            ptNoProjeto = malloc(sizeof (no)); // alocando o no projeto
            ptNoProjeto->info = P; // seta o ponteiro de projeto ao nó de projeto
            Novo = insere(Novo, ptNoProjeto, compId); // inserindo o no de projeto na lista de projetos criada
            Lp = Lp->prox; // proximo da lista de projetos
        }
        Ln = Ln->prox; // proximo da lista de nomes
    }
    // percorre a lista novamente acertando a lista de nomes aos projetos
    Lp = Novo; //projeto anterior
    ptNoProjeto = Novo->prox; //projeto atual
    while (ptNoProjeto) {// enquanto não é o fim da lista de projetos
        P = ptNoProjeto->info; // projeto atual
        if (ptNoProjeto && compId(Lp, ptNoProjeto) == 0) {// se existe projeto atual e se os id's são iguais
            ((PTproj) Lp->info)->lNome = insere(((PTproj) Lp->info)->lNome, P->lNome, compNome); // insere na lista de nomes do projeto anterior, a lista de nomes do projeto atual
            Lp->prox = ptNoProjeto->prox; // proximo do no do projeto anterior aponta para o proximo no do projeto atual
            free(P); // desaloca o no do projeto
            free(ptNoProjeto); // desaloca no da lista de projeto
            ptNoProjeto = Lp->prox; // projeto atual recebe o proximo do anterior
        } else {// se os id's não são iguais
            Lp = ptNoProjeto;
            ptNoProjeto = ptNoProjeto->prox;
        }
    }
    return Novo;
}

/* Funcao que mostra a Lista de Projetos e as pessoas alocadas
 Para os dados anteriores, deve apresentar:
 ------------------------------------------
      1: [(Alan,5)]
      3: [(Alan,15),(Bruno,10)]
      4: [(Bruno,8)]
      5: [(Bruno,5),(Daniel,25)]
 */
void mostraPorProj(PTno Lp) {
    PTno Ln;
    while (Lp) {
        printf("%2d: [", ((PTproj) Lp->info)->id);
        Ln = ((PTproj) Lp->info)->lNome;
        while (Ln) {
            printf("(%s,%d)", ((PTnome) Ln->info)->nome, ((PTnome) Ln->info)->tmp);
            Ln = Ln->prox;
            if (Ln) printf(", ");
        }
        printf("]\n");
        Lp = Lp->prox;
    }
    printf("\n");
}

/* Funcao que devolve o ID do projeto que consome mais tempo
 */
int projMaisTempo(PTno Lp) {
    int id, maior = 0;
    int menor;
    PTno Ln;
    while (Lp) {
        Ln = ((PTproj) Lp->info)->lNome;
        menor = 0;
        while (Ln) {
            menor += ((PTnome) Ln->info)->tmp;
            Ln = Ln->prox;
        }
        if (menor > maior) {
            id = ((PTproj) Lp->info)->id;
            maior = menor;
        }
        Lp = Lp->prox;
    }
    return id;
}

/* Funcao que retorna o nome do funcionario que tem mais tempo alocado
 */
void nomeMaisTempo(PTno Ln, char *nome) {
    int maior = 0;
    int menor;
    PTno Lp;
    while (Ln) {
        Lp = (((PTnome) Ln->info)->lProj);
        menor = 0;
        while (Lp) {
            menor += ((PTproj) Lp->info)->tmp;
            Lp = Lp->prox;
        }
        if (menor > maior) {
            strncpy(nome, ((PTnome) Ln->info)->nome, 10);
            maior = menor;
        }
        Ln = Ln->prox;
    }
}

/* Funcao que retorna o tempo total dos projetos
 */
int tempoTotal(PTno Ln) {
    int total = 0;
    PTno Lp;
    while (Ln) {
        Lp = (((PTnome) Ln->info)->lProj);
        while (Lp) {
            total += ((PTproj) Lp->info)->tmp;
            Lp = Lp->prox;
        }
        Ln = Ln->prox;
    }
    return total;
}

/* Funcao que mostra o percentual de tempo alocado em projetos
 * Para os dados anteriores, deve apresentar:
 Alan - 29.41%
 Bruno - 33.82%
 Daniel - 36.76%
 */
void mostraPercAlocado(PTno Ln) {
    PTno Lp;
    double hrsTotalProjetos = tempoTotal(Ln);
    double horasPorPessoa = 0;
    printf("\n");
    while (Ln) {
        Lp = ((PTnome) Ln->info)->lProj;
        horasPorPessoa = 0;
        while (Lp) {
            horasPorPessoa += ((PTproj) Lp->info)->tmp;
            Lp = Lp->prox;
        }
        printf("%s - %.2f%%\n", ((PTnome) Ln->info)->nome, (horasPorPessoa * PORCENTO) / hrsTotalProjetos);
        Ln = Ln->prox;
    }
}

int main(int argc, char** argv) {
    PTno listaNome = carregaDados();
    PTno listaProj = converte(listaNome);
    char nome[10];
    mostraPorNome(listaNome);
    mostraPorProj(listaProj);
    printf("Projeto mais longo = %d\n", projMaisTempo(listaProj));
    nomeMaisTempo(listaNome, nome);
    printf("Nome com mais tempo alocado = %s\n", nome);
    printf("Tempo Total dos projetos = %d\n", tempoTotal(listaNome));
    mostraPercAlocado(listaNome);
    return (EXIT_SUCCESS);
}
