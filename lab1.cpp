#include <iostream>
#include <ctime>
#include <cstdlib>
#include <thread>
#include <chrono>

using namespace std;
using std::chrono::nanoseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

const int MATRIX_SIZE = 20000;  // Matrix size
int matrix[MATRIX_SIZE][MATRIX_SIZE];  // 2D matrix of integers

// Function to fill the matrix with random values between 1 and 10
void fill_matrix() {
    srand(time(0));  // Seed the random number generator with the current time
    for (int i = 0; i < MATRIX_SIZE; ++i) {
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            if (j == MATRIX_SIZE - 1 - i) {
                matrix[i][j] = 1;  // Set 1 at the position of the secondary diagonal
            }
            else {
                matrix[i][j] = rand() % 10 + 1;  // Random value between 1 and 10
            }
        }
    }
}

// Function to compute the product of each row and store it in a secondary diagonal position
void compute_product(int start, int end) {
    for (int i = start; i < end; ++i) {
        int product = 1;
        for (int j = 0; j < MATRIX_SIZE; ++j) {
            product *= matrix[i][j];  // Compute the product of elements in row i
        }
        matrix[i][MATRIX_SIZE - 1 - i] = product;  // Store the product in the secondary diagonal position
    }
}

// Function to measure the execution time of the matrix computation function
void measure_execution_time(void (*func)(int, int), int threads = 1) {
    auto start_time = high_resolution_clock::now();  // Get start time

    // If using a single thread, just call the function directly
    if (threads == 1) {
        func(0, MATRIX_SIZE);
    }
    else {
        thread* thread_pool = new thread[threads];  // Create a thread pool
        int chunk_size = MATRIX_SIZE / threads;  // Divide the work into chunks for each thread

        // Create and launch threads
        for (int i = 0; i < threads; ++i) {
            int start = i * chunk_size;
            int end = (i == threads - 1) ? MATRIX_SIZE : (i + 1) * chunk_size;
            thread_pool[i] = thread(func, start, end);  // Assign function to thread
        }

        // Wait for all threads to finish
        for (int i = 0; i < threads; ++i) {
            thread_pool[i].join();
        }

        delete[] thread_pool;  // Clean up thread pool
    }

    auto end_time = high_resolution_clock::now();  // Get end time
    std::chrono::duration<double> elapsed = end_time - start_time;  // Calculate elapsed time
    cout << "Execution time with " << threads << " thread(s): " << elapsed.count() << " seconds" << endl;  // Print the result
}

int main() {
    fill_matrix();  // Fill the matrix with random values

    cout << "Without parallelization:" << endl;
    measure_execution_time(compute_product);  // Measure execution time without threads

    int thread_counts[] = { 4, 8, 16, 32, 64, 128, 256 };  // Different thread counts for parallel execution
    for (int threads : thread_counts) {
        cout << "With " << threads << " threads:" << endl;
        measure_execution_time(compute_product, threads);  // Measure execution time with threads
    }

    return 0;
}
