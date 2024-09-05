#include <iostream>
#include <vector>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <cstring>

struct ThreadData {
    int start_row;
    int end_row;
    const std::vector<std::vector<int>>* matrixA;
    const std::vector<std::vector<int>>* matrixB;
    std::vector<std::vector<int>>* matrixC;
};

void* sum_matrices(void* arg) {
    ThreadData* data = (ThreadData*) arg;
    for (int i = data->start_row; i < data->end_row; ++i) {
        for (int j = 0; j < data->matrixA->at(0).size(); ++j) {
            data->matrixC->at(i).at(j) = data->matrixA->at(i).at(j) + data->matrixB->at(i).at(j);
        }
    }
    return nullptr;
}

void initialize_matrix(std::vector<std::vector<int>>& matrix, int rows, int cols) {
    std::srand(std::time(0));
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            matrix[i][j] = std::rand() % 100; // Valores aleatorios entre 0 y 99
        }
    }
}

void print_matrix(const std::vector<std::vector<int>>& matrix) {
    for (const auto& row : matrix) {
        for (const auto& elem : row) {
            std::cout << elem << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
    int rows, cols, num_threads;

    std::cout << "Ingrese el número de filas: ";
    std::cin >> rows;
    std::cout << "Ingrese el número de columnas: ";
    std::cin >> cols;
    std::cout << "Ingrese el número de hilos: ";
    std::cin >> num_threads;

    std::vector<std::vector<int>> matrixA(rows, std::vector<int>(cols));
    std::vector<std::vector<int>> matrixB(rows, std::vector<int>(cols));
    std::vector<std::vector<int>> matrixC(rows, std::vector<int>(cols));

    initialize_matrix(matrixA, rows, cols);
    initialize_matrix(matrixB, rows, cols);

    std::cout << "Matriz A:" << std::endl;
    print_matrix(matrixA);
    std::cout << "Matriz B:" << std::endl;
    print_matrix(matrixB);

    pthread_t threads[num_threads];
    ThreadData thread_data[num_threads];

    int rows_per_thread = rows / num_threads;

    for (int i = 0; i < num_threads; ++i) {
        thread_data[i].start_row = i * rows_per_thread;
        thread_data[i].end_row = (i == num_threads - 1) ? rows : (i + 1) * rows_per_thread;
        thread_data[i].matrixA = &matrixA;
        thread_data[i].matrixB = &matrixB;
        thread_data[i].matrixC = &matrixC;

        int rc = pthread_create(&threads[i], nullptr, sum_matrices, &thread_data[i]);
        if (rc) {
            std::cerr << "Error al crear el hilo " << i << ": " << strerror(rc) << std::endl;
            return 1;
        }
    }

    for (int i = 0; i < num_threads; ++i) {
        int rc = pthread_join(threads[i], nullptr);
        if (rc) {
            std::cerr << "Error al unir el hilo " << i << ": " << strerror(rc) << std::endl;
            return 1;
        }
    }

    std::cout << "Matriz C (Resultado de la suma):" << std::endl;
    print_matrix(matrixC);

    return 0;
}
