#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define N 10  // Tamanho da tabela Hash

// Estrutura para armazenar os dados do aluno
typedef struct aluno {
    int mat;
    char nome[100];
    char curso[50];
    int disponibilidade;   // 0: Ocupado, 1: Disponível
} Aluno;

// Função hash
int hash(int key) {
    return (key % N + N) % N;  // Garante valores positivos
}

// Inicializa a tabela no arquivo
void inicializar(char *nomeArq) {
    FILE *arq = fopen(nomeArq, "wb");
    if (!arq) {
        printf("Erro: Nao foi possivel criar o arquivo.\n");
        exit(1);
    }

    Aluno a = {0, "", "", 1};  // Estrutura inicial
    for (int i = 0; i < N; i++) {
        fwrite(&a, sizeof(Aluno), 1, arq);
    }

    fclose(arq);
}

// Calcula a posição disponível para inserção
int acharPosicao(char *nomeArq, int mat) {
    int pos = hash(mat);
    Aluno a;
    FILE *arq = fopen(nomeArq, "rb");
    if (!arq) {
        printf("Erro: Arquivo nao encontrado.\n");
        exit(1);
    }

    fseek(arq, pos * sizeof(Aluno), SEEK_SET);
    fread(&a, sizeof(Aluno), 1, arq);
    int inicio = pos;  // Marca a posição inicial para evitar loop infinito
    while (a.disponibilidade == 0) {  // Procura próxima posição livre
        pos = (pos + 1) % N;
        if (pos == inicio) {
            fclose(arq);
            return -1;
        }

        fseek(arq, pos * sizeof(Aluno), SEEK_SET);
        fread(&a, sizeof(Aluno), 1, arq);
    }

    fclose(arq);
    return pos;
}

// Função para validar se a entrada é um número (feita após diversos erros em testes)
int lerInteiro(char *mensagem) {
    int numero;
    char buffer[100];
    while (1) {
        printf("%s", mensagem);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%d", &numero) == 1) {
                return numero;
            }
            else {
                printf("Erro: Entrada invalida! Digite um numero.\n");
            }
        }
    }
}

// Insere um novo aluno
void inserirAluno(char *nomeArq) {
    Aluno a;
    a.mat = lerInteiro("Digite a matricula: ");
    printf("Digite o nome: ");
    fgets(a.nome, sizeof(a.nome), stdin);
    strtok(a.nome, "\n");
    printf("Digite o curso: ");
    fgets(a.curso, sizeof(a.curso), stdin);
    strtok(a.curso, "\n");

    a.disponibilidade = 0;
    int pos = acharPosicao(nomeArq, a.mat);
    if (pos == -1) {
        printf("Erro: Tabela cheia. Nao e possivel inserir novos alunos.\n");
        return;
    }

    FILE *arq = fopen(nomeArq, "r+b");
    if (!arq) {
        printf("Erro: Nao foi possivel abrir o arquivo.\n");
        return;
    }

    fseek(arq, pos * sizeof(Aluno), SEEK_SET);
    fwrite(&a, sizeof(Aluno), 1, arq);
    fclose(arq);
    printf("Aluno inserido com sucesso na posicao %d!\n", pos);
}

// Busca informações de um aluno pela matrícula
void buscarAluno(char *nomeArq) {
    int mat, pos;
    Aluno a;
    mat = lerInteiro("Digite a matricula do aluno: ");
    pos = hash(mat);
    FILE *arq = fopen(nomeArq, "rb");
    if (!arq) {
        printf("Erro: Arquivo nao encontrado.\n");
        return;
    }

    fseek(arq, pos * sizeof(Aluno), SEEK_SET);
    fread(&a, sizeof(Aluno), 1, arq);
    int inicio = pos;
    while (a.disponibilidade == 0 && a.mat != mat) {
        pos = (pos + 1) % N;
        if (pos == inicio) {
            fclose(arq);
            printf("Erro: Aluno nao encontrado.\n");
            return;
        }

        fseek(arq, pos * sizeof(Aluno), SEEK_SET);
        fread(&a, sizeof(Aluno), 1, arq);
    }
    fclose(arq);
    if (a.disponibilidade == 1) {
        printf("Erro: Aluno nao encontrado.\n");
    }
    else {
        printf("Matricula: %d\n", a.mat);
        printf("Nome: %s\n", a.nome);
        printf("Curso: %s\n", a.curso);
    }
}

// Imprime a tabela de dispersão
void imprimirTabela(char *nomeArq) {
    Aluno a;
    FILE *arq = fopen(nomeArq, "rb");
    if (!arq) {
        printf("Erro: Arquivo nao encontrado.\n");
        return;
    }

    printf("Tabela de Dispersao:\n");
    for (int i = 0; i < N; i++) {
        fseek(arq, i * sizeof(Aluno), SEEK_SET);
        fread(&a, sizeof(Aluno), 1, arq);
        printf("[%d]: ", i);
        if (a.disponibilidade == 1) {
            printf("Vazio\n");
        }
        else {
            printf("Matricula: %d, Nome: %s, Curso: %s\n", a.mat, a.nome, a.curso);
        }
    }

    fclose(arq);
}

// Menu
void menu(char *nomeArq) {
    int opcao;
    do {
        printf("\nMenu:\n");
        printf("1. Inserir um novo aluno\n");
        printf("2. Buscar informacoes de um aluno\n");
        printf("3. Imprimir a tabela de dispersao\n");
        printf("4. Sair\n");
        printf("Escolha uma opcao: ");
        opcao = lerInteiro("");

        switch (opcao) {
            case 1:
                inserirAluno(nomeArq);
                break;

            case 2:
                buscarAluno(nomeArq);
                break;

            case 3:
                imprimirTabela(nomeArq);
                break;

            case 4:
                printf("Saindo...\n");
                break;

            default:
                printf("Opcao invalida!\n");
        }
    }
    while (opcao != 4);
}


int main() {
    char nomeArq[] = "tabela_hash.dat";
    inicializar(nomeArq);
    menu(nomeArq);
    return 0;
}
