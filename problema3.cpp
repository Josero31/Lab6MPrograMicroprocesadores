#include <iostream>
#include <vector>
#include <pthread.h>
#include <cstdlib>
#include <ctime>
#include <cstring>

const int N = 10; // Tamaño de la cuadrícula
const int NUM_THREADS = 4; // Número de hilos
int grid[N][N]; // Cuadrícula
long total_live_cells = 0; // Variable compartida para el conteo total de celdas vivas
pthread_mutex_t mutex_live_cells; // Mutex para proteger la variable compartida

struct ThreadData {
    int start_row;
    int end_row;
    long live_cells;
};

void* count_live_cells(void* arg) {
    ThreadData* data = (ThreadData*) arg;
    data->live_cells = 0;

    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < N; j++) {
            if (grid[i][j] == 1) {
                data->live_cells++;
            }
        }
    }

    pthread_mutex_lock(&mutex_live_cells);
    total_live_cells += data->live_cells;
    pthread_mutex_unlock(&mutex_live_cells);

    return nullptr;
}

void initialize_grid() {
    std::srand(std::time(0));
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            grid[i][j] = std::rand() % 2; // Inicializa la cuadrícula con 0s y 1s aleatorios
        }
    }
}

void print_grid() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            std::cout << grid[i][j] << " ";
        }
        std::cout << std::endl;
    }
}

int main() {
    initialize_grid();
    std::cout << "Cuadrícula inicial:" << std::endl;
    print_grid();

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    pthread_mutex_init(&mutex_live_cells, nullptr);

    int rows_per_thread = N / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].start_row = i * rows_per_thread;
        thread_data[i].end_row = (i == NUM_THREADS - 1) ? N : (i + 1) * rows_per_thread;
        thread_data[i].live_cells = 0;

        int rc = pthread_create(&threads[i], nullptr, count_live_cells, &thread_data[i]);
        if (rc) {
            std::cerr << "Error al crear el hilo " << i << ": " << strerror(rc) << std::endl;
            return 1;
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        int rc = pthread_join(threads[i], nullptr);
        if (rc) {
            std::cerr << "Error al unir el hilo " << i << ": " << strerror(rc) << std::endl;
            return 1;
        }
    }

    pthread_mutex_destroy(&mutex_live_cells);

    std::cout << "Número total de celdas vivas: " << total_live_cells << std::endl;

    return 0;
}
