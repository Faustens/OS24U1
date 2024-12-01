#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <time.h>
#include <sys/wait.h>

#define ITERATIONS 1e6

// Hilfsfunktion zur Zeitmessung (nanosekunden)
static inline long long get_time_ns() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1e9 + ts.tv_nsec;
}

int main() {
    // Erstelle zwei Pipes zur Kommunikation und gegenseitigen Blockade zweier Prozesse
    int pipe1[2], pipe2[2];
    if (pipe(pipe1) < 0 || pipe(pipe2) < 0) {
        perror("Fehler beim Erstellen der Pipes");
        exit(EXIT_FAILURE);
    }
    // Erstelle einen Child-Prozess mittels fork
    pid_t pid = fork();
    if (pid < 0) {
        perror("Fehler beim Fork");
        exit(EXIT_FAILURE);
    }
    // Zeitmessung der 2 * ITERATIONS Kontexwechsel
    if (pid == 0) { // Kindprozess
        char msg;
        for (int i = 0; i < ITERATIONS; i++) {
            read(pipe1[0], &msg, 1); // Blockiert, bis Parent schreibt
            write(pipe2[1], "x", 1);   // Antwort an Parent
        }
        exit(EXIT_SUCCESS);
    } else { // Elternprozess
        char msg;
        long long start_time = get_time_ns();
	// 2 Kontexwechsel pro Durchlauf, wegen parent -> child -> parent
        for (int i = 0; i < ITERATIONS; i++) {
            write(pipe1[1], "x", 1);   // Nachricht an Child
            read(pipe2[0], &msg, 1); // Warten auf Antwort
        }

        long long end_time = get_time_ns();
        long long total_time = end_time - start_time;

        printf("Gesamtlaufzeit: %.3f s\n", (double)total_time / 1e9 );
        printf("Durchschnittliche Zeit pro Kontextwechsel: %.5f us\n", (double)total_time / (ITERATIONS * 2 * 1000));

        wait(NULL); // Warten auf Kindprozess
    }

    return 0;
}
