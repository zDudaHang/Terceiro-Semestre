/**
*    @file main.cpp
*    @brief Primeiro trabalho da disciplina de Estrutura de Dados(INE5408).

*	 Propósito: Verificação do fechamento de "tags" no arquivo xml mediante uma pilha encadeada de strings
*    e rotulação das imagens binárias, presentes no segmento "<data>", com o uso de vizinhança 4, utilizando
*    duas filas encadeadas contendo: matrizes inteiras e ElementToSee (struct definida).

*    @author Maria Eduarda de Melo Hang
*    @date 06 October 2018
*/

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include "linked_stack.h"
#include "linked_queue.h"

/**
*	@brief Classe para armazenar a linha e a coluna de um elemento da matriz.
*
*	Utilizada para o processo de rotulação de imagens binárias do trabalho,
* 	guardará a linha e a coluna da respectiva matriz que está sendo rotulada.	
*/
namespace structures {
	/** Classe para guardar linha e coluna*/
	class ElementToSee {
	 private:
	 	int line;  /**< inteiro para guardar a linha do elemento da matriz */ 
	 	int column;  /**< inteiro para guardar a coluna do elemento da matriz */ 
	 public:
	 	/** Construtor */
	 	ElementToSee();
	 	/** Getter da linha */
	 	int getLine() const;
	 	/** Setter da linha */
	 	void setLine(int line_);
	 	/** Getter da coluna */
	 	int getColumn() const;
	 	/** Setter da coluna */
	 	void setColumn(int column_);
	};
}  // namespace structures //

structures::ElementToSee::ElementToSee() {
	line = 0;
	column = 0;
}

int structures::ElementToSee::getLine() const {
	return line;
}

void structures::ElementToSee::setLine(int line_) {
	line = line_;
}

int structures::ElementToSee::getColumn() const {
	return column;
}

void structures::ElementToSee::setColumn(int column_) {
	column = column_;
}

std::string getXMLText(char xmlfilename[]);

int countingWidthTags(std::string XMLtext);

int countingHeightTags(std::string XMLtext);

int countingNameTags(std::string XMLtext);

bool findTheBeginOfXML(std::string XMLtext);

bool findTheEndOfXML(std::string XMLtext);

int lookUp(int** matrix, int line, int column);

int** createMatrixBase(int lines, int columns);

int lookLeft(int** matrix, int line, int column);

std::string cleaningMatrixString(std::string stringMatrix);

int* getMatrixWidths(std::string XMLtext, int countWidths);

std::string* getXMLMatrix(std::string XMLtext, int counter);

int* getMatrixHeights(std::string XMLtext, int countHeights);

int lookDown(int** matrix, int line, int column, int lines);

int lookRight(int** matrix, int line, int column, int columns);

std::string* gettingImageNames(std::string XMLtext, int countNames);

int** transformToIntMatrix(std::string stringMatrix, int lines, int columns);

void parsingXML(std::string XMLtext, structures::LinkedStack<std::string> stackToTags);

int searchMatrix(int** matrixXML, 
	int** matrixAuxiliar, 
	int lines, int columns, 
	structures::LinkedQueue<structures::ElementToSee> elementsQueue
	);

/**
*	@brief Recebe o nome do arquivo como entrada e chama todos os metodos necessarios.
*/
int main() {

    char xmlfilename[100];

	std::cin >> xmlfilename;

    std::string XMLtext = getXMLText(xmlfilename);

    if (!findTheBeginOfXML(XMLtext) || !findTheEndOfXML(XMLtext)) {
    	std:: cout << "error" << std:: endl;
    	exit(1);
    }

    structures::LinkedStack<std::string> stackToTags{};

    parsingXML(XMLtext, stackToTags);

    int counterHeights = countingHeightTags(XMLtext);

    int counterWidths = countingWidthTags(XMLtext);

    int counterNames = countingNameTags(XMLtext);

    if (!stackToTags.empty() || counterHeights != counterWidths || counterNames != counterWidths ) {
    	std:: cout << "error" << std:: endl;
    	exit(1);
    } 

    int* heights = getMatrixHeights(XMLtext, counterHeights);

    int* widths = getMatrixWidths(XMLtext, counterHeights);

    std::string* names = new std::string[counterNames];

    structures::LinkedQueue<int**> matricesQueue{};

    std:: string* s = new std::string[counterHeights];

    s = getXMLMatrix(XMLtext, counterHeights);

    names = gettingImageNames(XMLtext, counterNames);

    for (int i = 0; i < counterHeights; i++) {
    	s[i] = cleaningMatrixString(s[i]);
    	matricesQueue.enqueue(transformToIntMatrix(s[i],heights[i],widths[i]));
    }


    for (int i = 0; i < counterHeights; i++) {
    	structures::LinkedQueue<structures::ElementToSee> elementsQueue{};
    	int** matrixXML = matricesQueue.dequeue();
    	int** auxiliar = createMatrixBase(heights[i], widths[i]);
    	int biggerLabel = searchMatrix(matrixXML, auxiliar, heights[i], widths[i], elementsQueue);
    	std:: cout << names[i] << " " << biggerLabel << std:: endl; 
    }

}

/**
*	@brief Leitura do arquivo xml e armazenamento do respectivo texto.
*
*	Primeiramente, tenta-se abrir o arquivo xml recebido no terminal, caso
* 	não seja possível, o programa sairá retorna 1 (erro). Após essa verificação,
*	o ifstream source lerá caractere por caractere e concatenará com a variável
*	local XMLtext e terminará quando encontrar o EOF.
*
*	@param xmlfilename Nome do arquivo xml que será aberto e lido.
*	@return O texto do arquivo xml em formate de uma string.
*/
std::string getXMLText(char xmlfilename[]) {
	std::ifstream source(xmlfilename);
	if (!source) {
    	std:: cout << "error" << std:: endl;
    	exit(1);
    }
    char input;
    std::string XMLtext;
    while (source.get(input)) {
    	XMLtext += input;
    }
    return XMLtext;
}

/**
*	@brief Verificação dos fechamentos das tags no arquivo xml.
*
*	Usando o método find da biblioteca de strings, pegará o índice do primeiro caractere
*	que for igual a "<", depois o outro índice que for igual a ">" e por fim pegará uma substring,
* 	presente no intervalo [begin, end - begin + 1] do texto do XML e passará para a string
*	auxiliar. Caso o caractere "/" seja encontrado, ele será retirado com o método erase.
*	Posteriormente, haverá uma verificação no caso da pilha ser vazia, se a condição for
*	verdadeira, essa string auxiliar será automaticamente empilhada. Caso contrário, 
*	o método verificará se o topo coincide com a string auxiliar, se for verdade, 
*	a pilha realizará um pop, caso contrário um push, colocando a auxiliar como parâmetro.
*	Ao fim, o begin será incrementado para pegar o próximo índice do caractere que conter
* 	o "<". Caso uma tag de fechamento foi encontrada e não for igual ao topo, o programa
*	será finalizado e colocará "error" na tela.
*
*	@param XMLtext A string que contém o texto arquivo xml.
*	@param stackToTags Uma pilha para guardar as tags.
*/
void parsingXML(std::string XMLtext, structures::LinkedStack<std::string> stackToTags) {
	size_t begin = 0, end;
	int count = 0;
	while (begin < XMLtext.size()) {
		begin = XMLtext.find("<", begin);
		if (begin == std::string::npos) {
			break;
		}
		end = XMLtext.find(">", begin);
		if (end == std::string::npos) {
			break;
		}
		std::string auxiliar = XMLtext.substr(begin, end - begin + 1);
		size_t findBackSlash = 0;
		findBackSlash = auxiliar.find("/", 0);
		if (findBackSlash != std::string::npos) {
			auxiliar.erase(findBackSlash, 1);
			int comp_result = auxiliar.compare(stackToTags.top());
			if (comp_result != 0) {
				std:: cout << "error" << std:: endl;
				exit(1);
			}
		}
		if (stackToTags.empty()) {
			stackToTags.push(auxiliar);
		} else {
			int comp_result = auxiliar.compare(stackToTags.top());
			if (comp_result != 0) {
				stackToTags.push(auxiliar);
			} else {
				stackToTags.pop();
			}
		}
		begin++;
	}
}

/**
*	@brief Contagem das tags "<width>"
*
*	Ao encontrar a tag "<width>", o contador será incrementado.
*
*	@param XMLtext A string que contém o texto arquivo xml.
*	@return Um inteiro representando a quantidade de tags "<width>" encontradas.
*/
int countingWidthTags(std::string XMLtext) {
	size_t begin = 0, end;
	int count = 0;
	while (begin < XMLtext.size()) {
		begin = XMLtext.find("<width>", begin);
		if (begin == std::string::npos) {
			break;
		}
		end = XMLtext.find(">", begin);
		if (end == std::string::npos) {
			break;
		}
		begin++;
		count++;
	}
	return count;
}

/**
*	@brief Contagem das tags "<height>"
*
*	Ao encontrar a tag "<height>", o contador será incrementado.
*
*	@param XMLtext A string que contém o texto arquivo xml.
*	@return Um inteiro representando a quantidade de tags "<height>" encontradas.
*/
int countingHeightTags(std::string XMLtext) {
	size_t begin = 0, end;
	int count = 0;
	while (begin < XMLtext.size()) {
		begin = XMLtext.find("<height>", begin);
		if (begin == std::string::npos) {
			break;
		}
		end = XMLtext.find(">", begin);
		if (end == std::string::npos) {
			break;
		}
		begin++;
		count++;
	}
	return count;
}

/**
*	@brief Contagem das tags "<name>"
*
*	Ao encontrar a tag "<name>", o contador será incrementado.
*
*	@param XMLtext A string que contém o texto arquivo xml.
*	@return Um inteiro representando a quantidade de tags "<name>" encontradas.
*/
int countingNameTags(std::string XMLtext) {
	size_t begin = 0, end;
	int count = 0;
	while (begin < XMLtext.size()) {
		begin = XMLtext.find("<name>", begin);
		if (begin == std::string::npos) {
			break;
		}
		end = XMLtext.find(">", begin);
		if (end == std::string::npos) {
			break;
		}
		begin++;
		count++;
	}
	return count;
}

/**
*	@brief Verificação da existência da tag "<dataset>"
*
*	Esse método, com a utilização do método find, procurará pela tag
*	"<dataset>", caso seja encontrada, será atribuído o índice onde se encontra
*	e retornará verdadeiro. Caso contrário, retornará falso.
*
*	@param XMLtext A string que contém o texto arquivo xml.
*	@return Retornará verdadeiro se a tag "<dataset>" foi encontrada no arquivo.
*/
bool findTheBeginOfXML(std::string XMLtext) {
	size_t begin = 0, end;
	while (begin < XMLtext.size()) {
		begin = XMLtext.find("<dataset>", begin);
		if (begin == std::string::npos) {
			break;
		}
		end = XMLtext.find(">", begin);
		if (end == std::string::npos) {
			break;
		}
		return true;
	}
	return false;
}

/**
*	@brief Verificação da existência da tag "</dataset>"
*
*	Esse método, com a utilização do método find, procurará pela tag
*	"</dataset>", caso seja encontrada, será atribuído o índice onde se encontra
*	e retornará verdadeiro. Caso contrário, retornará falso.
*
*	@param XMLtext A string que contém o texto arquivo xml.
*	@return Retornará verdadeiro se a tag "</dataset>" foi encontrada no arquivo.
*/
bool findTheEndOfXML(std::string XMLtext) {
	size_t begin = 0, end;
	while (begin < XMLtext.size()) {
		begin = XMLtext.find("</dataset>", begin);
		if (begin == std::string::npos) {
			break;
		}
		end = XMLtext.find(">", begin);
		if (end == std::string::npos) {
			break;
		}
		return true;
	}
	return false;
}
/**
*	@brief Extração dos nomes das imagens.
*
*	Da mesma forma que o método parsingXML, utiliza-se a função find da biblioteca de strings,
*	nesse caso as strings padrão serão "<name>", para obter o índice do primeiro caractere, e
* 	"</name>", para encontrar o índice do último caractere dessa tag. Posteriormente, será pego a
*	substring presente no intervalo [begin, position - begin], sendo extraído apenas o número entre
* 	as duas tags. Por fim, essa substring será  guardada no array de strings
*	names. O begin será incrementado para buscar os novos índices.
*
*	@param XMLtext A string que contém o texto arquivo xml.
*	@param countNames Quantidade de nomes que devem ser buscados.
*	@return Um array de strings contendo todos os nomes das imagens.
*/
std::string* gettingImageNames(std::string XMLtext, int countNames) {
	std::string* names = new std::string[countNames];
	int i = 0;
	size_t begin, position = 0;
	while (i < countNames) {
		begin = XMLtext.find("<name>",position);
		if (begin == std::string::npos) {
			break;
		}
		begin = XMLtext.find(">", begin);
		begin++;
		position = XMLtext.find("</name>", begin);
		if (position == std::string::npos) {
			break;
		}
		names[i] = XMLtext.substr(begin, position - begin);
		i++;
	}
	return names;
}

/**
*	@brief Extração das linhas das matrizes.
*
*	Da mesma forma que o método parsingXML, utiliza-se a função find da biblioteca de strings,
*	nesse caso as strings padrão serão "<height>", para obter o índice do primeiro caractere, e
* 	"</height>", para encontrar o índice do último caractere dessa tag. Posteriormente, será pego a
*	substring presente no intervalo [begin, position - begin], sendo extraído apenas o número entre
* 	as duas tags. Por fim, essa substring será convertida em inteiro e guardada no array de inteiros
*	heights. O begin será incrementado para buscar os novos índices.
*
*	@param XMLtext A string que contém o texto arquivo xml.
*	@param countHeights Quantidade de valores que devem ser coletados da string(número de matrizes presentes no arquivo).
*	@return Um array de inteiros contendo todos os valores de linhas de cada matriz contida no arquivo xml.
*/
int* getMatrixHeights(std::string XMLtext, int countHeights) {
	int* heights = new int[countHeights];
	int i = 0;
	size_t begin, position = 0;
	while(i < countHeights) {
		begin = XMLtext.find("<height>", position);
		if (begin == std::string::npos) {
			break;
		}
		begin = XMLtext.find(">", begin);
		begin++;
		position = XMLtext.find("</height>", begin);
		if (position == std::string::npos) {
			break;
		}
		std::string auxiliar = XMLtext.substr(begin, position - begin);
		heights[i] = stoi(auxiliar);
		i++;
	}
	return heights;
}

/**
*	@brief Extração das colunas das matrizes.
*
*	Da mesma forma que o método parsingXML, utiliza-se a função find da biblioteca de strings,
*	nesse caso as strings padrão serão "<width>", para obter o índice do primeiro caractere, e
* 	"</width>", para encontrar o índice do último caractere dessa tag. Posteriormente, será pego a
*	substring presente no intervalo [begin, position - begin], sendo extraído apenas o número entre
* 	as duas tags. Por fim, essa substring será convertida em inteiro e guardada no array de inteiros
*	widths. O begin será incrementado para buscar os novos índices.
*
*	@param XMLtext A string que contém o texto arquivo xml.
*	@param countWidths Quantidade de valores que devem ser coletados da string(número de matrizes presentes no arquivo).
*	@return Um array de inteiros contendo todos os valores de colunas de cada matriz contida no arquivo xml.
*/
int* getMatrixWidths(std::string XMLtext,int countWidths) {
	int* widths = new int[countWidths];
	int i = 0;
	size_t begin, position = 0;
	while(i < countWidths) {
		begin = XMLtext.find("<width>", position);
		if (begin == std::string::npos) {
			break;
		}
		begin = XMLtext.find(">", begin);
		begin++;
		position = XMLtext.find("</width>", begin);
		if (position == std::string::npos) {
			break;
		}
		std::string auxiliar = XMLtext.substr(begin, position - begin);
		widths[i] = stoi(auxiliar);
		i++;
	}
	return widths;
}

/**
*	@brief Extração das matrizes.
*
*	Da mesma forma que nos métodos de extração de linhas e colunas das matrizes,
*	esse método realizará a busca das matrizes contidas entre as tags com o padrão
*	"<data>" e "</data>", pegando os respectivos índices e extraindo a substring no
*	intervalo [begin, position - begin] (a matriz). O begin será incrementado para
*	buscar os novos índices.
*
*	@param XMLtext A string que contém o texto arquivo xml.
*	@param counter Quantidade de matrizes presentes no arquivo.
*	@return Um array de strings contendo cada matrize do arquivo xml.
*/
std::string* getXMLMatrix(std::string XMLtext, int counter) {
	std::string* strings = new std::string[counter];
	for (int i = 0; i < counter; i++) {
		strings[i] = " ";
	}
	size_t begin = 0, position = 0;
	int i = 0;
	while (begin < XMLtext.size()) {
		begin = XMLtext.find("<data>", position);
		if (begin == std::string::npos) {
			break;
		}
		begin = XMLtext.find(">", begin);
		begin++;
		position = XMLtext.find("</data>", begin);
		if (position == std::string::npos) {
			break;
		}
		std::string auxiliar = XMLtext.substr(begin, position - begin);
		strings[i] = auxiliar;
		i++;
	}
	return strings;
}

/**
*	@brief Retirada do caractere de quebra de linha das strings das matrizes.
*
*	Caso o caractere de quebra de linha seja encontrado nessa string, será pego o índice e
*	removido com o uso do método erase até que não reste mais nenhum.
*
*	@param stringMatrix A string que contém uma das matrizes do arquivo xml.
*	@return Uma string sem o caractere de quebra de linha presente.
*/
std::string cleaningMatrixString(std::string stringMatrix) {
	size_t index = 0;
	size_t size = stringMatrix.size();
	while(index < size) {
		index = stringMatrix.find("\n", 0);
		if (index == std::string::npos) {
			break;
		} else {
			stringMatrix.erase(index, 1);
		}
	}
	return stringMatrix;
}

/**
*	@brief Transformação das matrizes do arquivo xml em matrizes inteiras.
*
*	Para realizar essa transformação, o método pegará caractere por caractere
*	e inserir na matriz de inteiros em cada elemento da respectiva posição.
*
*	@param stringMatrix A string que contém uma matriz do arquivo xml.
*	@param lines Quantidade de linhas da matriz.
*	@param columns Quantidade de colunas da matriz.
*	@return Uma matriz de inteiros contendo os valores (0 ou 1) da matrize do arquivo xml.
*/
int** transformToIntMatrix(std::string stringMatrix, int lines, int columns) {
	int** matrixTransformed = new int*[lines];
	for (int i = 0; i < lines; i++) {
		matrixTransformed[i] = new int[columns];
	}
	size_t counter = 0;
	for (int i = 0; i < lines; i++) {
		for (int j = 0; j < columns; j++) {
			std::string auxiliar = stringMatrix.substr(counter, 1);
			matrixTransformed[i][j] = std::stoi(auxiliar.c_str());
			counter++;
		}
	}
	return matrixTransformed;
}

/**
*	@brief Criação de uma matriz nula.
*
*	Cria uma matriz nula para ser usada na rotulação, tendo
*	a mesma quantidade de linhas e colunas que matriz do xml.
*
*	@param lines Quantiade de linhas.
*	@param columns Quantidade de colunas.
*	@return Uma matriz de inteiros nula.
*/
int** createMatrixBase(int lines, int columns) {
	int** baseMatrix = new int*[lines];
	for (int i = 0; i < lines; i++) {
		baseMatrix[i] = new int[columns];
	}
	for (int i = 0; i < lines; i++) {
		for (int j = 0; j < columns; j++) {
			baseMatrix[i][j] = 0;
		}
	}
	return baseMatrix;
}


/**
*	@brief Rotulamento da Matriz (vizinhança 4).
*
*	Primeiramente, o método percorrerá a matriz do xml e ao encontrar o valor 1 em um dos elementos,	
*	ocorrerá uma verificação para ver se já não foi visitado na matriz de rotulamento (se não foi visitado
*	o valor será 0), caso não tenha sido visitado, será instanciado um elemento da classe ElementToSee para
*	guardar a linha e coluna desse elemento, sendo inserido na fila de rotulamento, e o rótulo será aplicado
*	aplicado a matriz de rotulamento na mesma linha e coluna. Posteriormente, os 4 vizinhos desse elemento
*	serão verificados com os métodos lookUp, lookDown, lookRight e lookLeft e depois se a matriz de rotulamento
*	já não visitou esse vizinho outro elemento da classe ElementToSee será instanciado e inserido na fila
*	de rotulamento e o rótulo atual será atribuído ao vizinho que atendeu às condições mostradas anteriormente.
*	Por fim, o rótulo será incrementado se nenhum outro elemento conexo for encontrado.
*
*	@param matrixXML A matriz de inteiros do arquivo xml.
*	@param matrixAuxiliar A matriz auxiliar de rotulamento.
*	@param lines Quantidade de linhas da matriz.
* 	@param columns Quantidade de colunas da matriz.
*	@param elementsQueue Fila para guardar os elementos a serem verificados e seus vizinhos (vizinhança 4).
*	@return O maior rótulo atribuído.
*/
int searchMatrix(int** matrixXML, int** matrixAuxiliar, int lines, int columns, structures::LinkedQueue<structures::ElementToSee> elementsQueue) {
	int label = 1;
	for (int i = 0; i < lines; i++) {
		for (int j = 0; j < columns; j++) {
			if( matrixXML[i][j] == 1) {
				if (matrixAuxiliar[i][j] == 0) {
					structures::ElementToSee element;
					element.setLine(i);
					element.setColumn(j);
					elementsQueue.enqueue(element);
					while (!elementsQueue.empty()) {
						structures::ElementToSee auxiliar;
						auxiliar = elementsQueue.dequeue();
						int line = auxiliar.getLine();
						int column = auxiliar.getColumn();
						matrixAuxiliar[line][column] = label;
						if (lookUp(matrixXML, line, column)) {
							if (matrixAuxiliar[line-1][column] == 0) {
								structures::ElementToSee newElement;
								newElement.setLine(line-1);
								newElement.setColumn(column);
								elementsQueue.enqueue(newElement);
								matrixAuxiliar[line-1][column] = label;
							}
						}
						if (lookDown(matrixXML, line, column, lines)) {
							if (matrixAuxiliar[line+1][column] == 0) {
								structures::ElementToSee newElement;
								newElement.setLine(line+1);
								newElement.setColumn(column);
								elementsQueue.enqueue(newElement);
								matrixAuxiliar[line+1][column] = label;
							}
						}
						if (lookLeft(matrixXML, line, column)) {
							if (matrixAuxiliar[line][column-1] == 0) {
								structures::ElementToSee newElement;
								newElement.setLine(line);
								newElement.setColumn(column-1);
								elementsQueue.enqueue(newElement);
								matrixAuxiliar[line][column-1] = label;
							}
						}
						if (lookRight(matrixXML, line, column, columns)) {
							if (matrixAuxiliar[line][column+1] == 0) {
								structures::ElementToSee newElement;
								newElement.setLine(line);
								newElement.setColumn(column+1);
								elementsQueue.enqueue(newElement);
								matrixAuxiliar[line][column+1] = label;
							}
						}
					}	
					label++;
				}
			}
		}
	}
	return label - 1;
}

/**
*	@brief Verificando o elemento abaixo do elemento referência.
* 	@param matrix A matriz.
*	@param line A linha do elemento de referência que deseja verificar.
*	@param column A coluna do elemento de referência que deseja verificar.
*	@param lines Quantidade de linhas da matriz para testar o limite do índice.
* 	@return Retornará 1 caso o elemento debaixo contenha o valor 1, caso contrário, 0.
*/
int lookDown(int** matrix, int line, int column, int lines) {
	if (line == lines -1) {
		return 0;
	} else {
		return (matrix[line+1][column] == 1);
	}
}

/**
*	@brief Verificando o elemento à direita do elemento referência.
* 	@param matrix A matriz.
*	@param line A linha do elemento de referência que deseja verificar.
*	@param column A coluna do elemento de referência que deseja verificar.
*	@param columns Quantidade de colunas da matriz para testar o limite do índice.
* 	@return Retornará 1 caso o elemento à direita contenha o valor 1, caso contrário, 0.
*/
int lookRight(int** matrix, int line, int column, int columns) {
	if (column == columns - 1) {
		return 0;
	} else {
		return (matrix[line][column+1] == 1);
	}
}

/**
*	@brief Verificando o elemento à esquerda do elemento referência.
* 	@param matrix A matriz.
*	@param line A linha do elemento de referência que deseja verificar.
*	@param column A coluna do elemento de referência que deseja verificar.
* 	@return Retornará 1 caso o elemento à esquerda contenha o valor 1, caso contrário, 0.
*/
int lookLeft(int** matrix, int line, int column) {
	if (column == 0) {
		return 0;
	} else {
		return (matrix[line][column-1] == 1);
	}
}

/**
*	@brief Verificando o elemento acima do elemento referência.
* 	@param matrix A matriz.
*	@param line A linha do elemento de referência que deseja verificar.
*	@param column A coluna do elemento de referência que deseja verificar.
* 	@return Retornará 1 caso o elemento acima contenha o valor 1, caso contrário, 0.
*/
int lookUp(int** matrix, int line, int column) {
	if (line == 0) {
		return 0;
	} else {
		return (matrix[line-1][column] == 1);
	}
}
