#include <iostream>
#include <pthread.h>
#include <cmath>

const int NUM_THREADS = 4;
const double a = 0.0; // Inicio del intervalo
const double b = 1.0; // Fin del intervalo
const int n = 1000000; // Número de trapecios

double total_sum = 0.0;
pthread_mutex_t mutex_sum;

double f(double x) {
    // Ejemplo de función: f(x) = x^2
    return x * x;
}

void* trapezoidal_rule(void* rank) {
    long my_rank = (long) rank;
    double h = (b - a) / n;
    int local_n = n / NUM_THREADS;
    double local_a = a + my_rank * local_n * h;
    double local_b = local_a + local_n * h;
    double local_sum = (f(local_a) + f(local_b)) / 2.0;

    for (int i = 1; i < local_n; i++) {
        double x = local_a + i * h;
        local_sum += f(x);
    }
    local_sum *= h;

    pthread_mutex_lock(&mutex_sum);
    total_sum += local_sum;
    pthread_mutex_unlock(&mutex_sum);

    return nullptr;
}

int main() {
    pthread_t threads[NUM_THREADS];
    pthread_mutex_init(&mutex_sum, nullptr);

    for (long thread = 0; thread < NUM_THREADS; thread++) {
        pthread_create(&threads[thread], nullptr, trapezoidal_rule, (void*) thread);
    }

    for (long thread = 0; thread < NUM_THREADS; thread++) {
        pthread_join(threads[thread], nullptr);
    }

    pthread_mutex_destroy(&mutex_sum);

    std::cout << "Resultado de la integral: " << total_sum << std::endl;

    return 0;
}
