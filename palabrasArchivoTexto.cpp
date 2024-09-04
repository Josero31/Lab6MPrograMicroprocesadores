#include <iostream>
#include <fstream>
#include <vector>
#include <pthread.h>
#include <cstring>
#include <cctype>

const int NUM_THREADS = 4;
long total_word_count = 0;
pthread_mutex_t mutex_word_count;

struct ThreadData {
    std::string text;
    long word_count;
};

void* count_words(void* arg) {
    ThreadData* data = (ThreadData*) arg;
    std::string& text = data->text;
    long& word_count = data->word_count;

    word_count = 0;
    bool in_word = false;
    for (char c : text) {
        if (std::isspace(c)) {
            if (in_word) {
                word_count++;
                in_word = false;
            }
        } else {
            in_word = true;
        }
    }
    if (in_word) {
        word_count++;
    }

    pthread_mutex_lock(&mutex_word_count);
    total_word_count += word_count;
    pthread_mutex_unlock(&mutex_word_count);

    return nullptr;
}

int main() {
    std::ifstream file("input.txt");
    if (!file.is_open()) {
        std::cerr << "Error al abrir el archivo." << std::endl;
        return 1;
    }

    file.seekg(0, std::ios::end);
    long file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    long chunk_size = file_size / NUM_THREADS;
    std::vector<ThreadData> thread_data(NUM_THREADS);
    std::vector<pthread_t> threads(NUM_THREADS);

    pthread_mutex_init(&mutex_word_count, nullptr);

    for (int i = 0; i < NUM_THREADS; i++) {
        long start_pos = i * chunk_size;
        long end_pos = (i == NUM_THREADS - 1) ? file_size : (i + 1) * chunk_size;

        file.seekg(start_pos);
        std::string chunk(end_pos - start_pos, '\0');
        file.read(&chunk[0], end_pos - start_pos);

        // Verificar si la lectura fue exitosa
        if (!file) {
            std::cerr << "Error al leer el archivo en la sección " << i << std::endl;
            return 1;
        }

        thread_data[i].text = chunk;
        thread_data[i].word_count = 0;

        int rc = pthread_create(&threads[i], nullptr, count_words, &thread_data[i]);
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

    pthread_mutex_destroy(&mutex_word_count);

    std::cout << "Número total de palabras: " << total_word_count << std::endl;

    return 0;
}
