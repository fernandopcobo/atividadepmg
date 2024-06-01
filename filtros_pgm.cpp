#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <cstdio>
#include <cstdlib>
#include <cstring>

using namespace std;

struct Imagem {
    int largura, altura, max_cor;
    vector<vector<int>> dados;
};

Imagem lerPGM(const string &caminho) {
    FILE *arquivo = fopen(caminho.c_str(), "rt");
    if (arquivo == NULL) {
        throw runtime_error("Erro ao abrir arquivo '" + caminho + "'");
    }

    char formato[3];
    char comentario[200];
    fscanf(arquivo, " %s", formato);
    if (strcmp(formato, "P2") != 0) {
        fclose(arquivo);
        throw runtime_error("Formato invalido!");
    }
    fgetc(arquivo);
    fgets(comentario, 200, arquivo);

    Imagem img;
    fscanf(arquivo, "%d %d", &img.largura, &img.altura);
    fscanf(arquivo, "%d", &img.max_cor);

    img.dados.resize(img.altura, vector<int>(img.largura));
    for (int i = 0; i < img.altura; i++) {
        for (int j = 0; j < img.largura; j++) {
            fscanf(arquivo, "%d", &img.dados[i][j]);
        }
    }

    fclose(arquivo);
    return img;
}

void escreverPGM(const string &caminho, const Imagem &img) {
    FILE *arquivo = fopen(caminho.c_str(), "wt");
    if (arquivo == NULL) {
        throw runtime_error("Erro ao abrir arquivo '" + caminho + "'");
    }

    fprintf(arquivo, "P2\n");
    fprintf(arquivo, "# teste\n");
    fprintf(arquivo, "%d %d\n", img.largura, img.altura);
    fprintf(arquivo, "%d\n", img.max_cor);

    for (int i = 0; i < img.altura; i++) {
        for (int j = 0; j < img.largura; j++) {
            fprintf(arquivo, "%d ", img.dados[i][j]);
        }
        fprintf(arquivo, "\n");
    }

    fclose(arquivo);
}

Imagem aplicarNegativo(const Imagem &img) {
    Imagem resultado = img;
    for (auto &linha : resultado.dados) {
        for (auto &pixel : linha) {
            pixel = img.max_cor - pixel;
        }
    }
    return resultado;
}

Imagem aplicarDesfoque(const Imagem &img) {
    Imagem resultado = img;
    for (int i = 1; i < img.altura - 1; ++i) {
        for (int j = 1; j < img.largura - 1; ++j) {
            int soma = 0;
            for (int di = -1; di <= 1; ++di) {
                for (int dj = -1; dj <= 1; ++dj) {
                    soma += img.dados[i + di][j + dj];
                }
            }
            resultado.dados[i][j] = soma / 9;
        }
    }
    return resultado;
}

Imagem aplicarContraste(const Imagem &img) {
    Imagem resultado = img;
    int total = 0;
    for (const auto &linha : img.dados) {
        for (const auto &pixel : linha) {
            total += pixel;
        }
    }
    int media = total / (img.largura * img.altura);
    for (auto &linha : resultado.dados) {
        for (auto &pixel : linha) {
            if (pixel > media) {
                pixel = min(img.max_cor, pixel + (pixel - media));
            } else {
                pixel = max(0, pixel - (media - pixel));
            }
        }
    }
    return resultado;
}

Imagem ajustarBrilho(const Imagem &img, int ajuste) {
    Imagem resultado = img;
    for (auto &linha : resultado.dados) {
        for (auto &pixel : linha) {
            pixel = min(img.max_cor, max(0, pixel + ajuste));
        }
    }
    return resultado;
}

Imagem aplicarLimiarizacao(const Imagem &img, int limiar) {
    Imagem resultado = img;
    for (auto &linha : resultado.dados) {
        for (auto &pixel : linha) {
            pixel = (pixel > limiar) ? img.max_cor : 0;
        }
    }
    return resultado;
}

int main() {
    char caminhoEntrada[200];
    char caminhoSaida[200];

    cout << "Caminho da imagem PGM: ";
    cin.getline(caminhoEntrada, 200);

    try {
        Imagem img = lerPGM(caminhoEntrada);

        cout << "Resolucao: " << img.largura << "x" << img.altura << endl;
        cout << "Brilho maximo: " << img.max_cor << endl;

        cout << "Caminho para salvar a imagem PGM com filtro aplicado: ";
        cin.getline(caminhoSaida, 200);

        cout << "Escolha o filtro a ser aplicado:\n";
        cout << "1. Negativo\n";
        cout << "2. Desfoque\n";
        cout << "3. Realce de Contraste\n";
        cout << "4. Ajuste de Brilho\n";
        cout << "5. Limiarizacao\n";

        int escolha;
        cin >> escolha;
        cin.ignore();

        Imagem resultado;
        switch (escolha) {
            case 1:
                resultado = aplicarNegativo(img);
                break;
            case 2:
                resultado = aplicarDesfoque(img);
                break;
            case 3:
                resultado = aplicarContraste(img);
                break;
            case 4: {
                int brilho;
                cout << "Digite o valor do brilho (-255 a 255): ";
                cin >> brilho;
                resultado = ajustarBrilho(img, brilho);
                break;
            }
            case 5: {
                int limiar;
                cout << "Digite o valor do limiar (0 a 255): ";
                cin >> limiar;
                resultado = aplicarLimiarizacao(img, limiar);
                break;
            }
            default:
                cout << "Escolha invalida!\n";
                return -3;
        }

        escreverPGM(caminhoSaida, resultado);

    } catch (const runtime_error &erro) {
        cerr << "Erro: " << erro.what() << endl;
        return -1;
    }

    return 0;
}
