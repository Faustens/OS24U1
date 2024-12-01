#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

#define ITERATIONS 10000000 // Anzahl der Wiederholungen

int main() {
    struct timespec start, end;
    long long total_time_loss;
    long long elapsed_ns;
    long long elapsed_ns_corrected;
    long long single_elapsed_ns;
    long long elapsed_ns_min = 1LL << 60;
    // Ermittle den Zeitaufwand des durchlaufens eines for-Loops
    clock_gettime(CLOCK_MONOTONIC,&start);
    for (long i = 0; i<ITERATIONS; i++) {
    }
    clock_gettime(CLOCK_MONOTONIC,&end);
    total_time_loss = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);

    // Ermittle den Zeitaufwand eines for-Loops zusammen mit getpid.
    clock_gettime(CLOCK_MONOTONIC,&start);
    for (long i = 0; i < ITERATIONS; i++) {
	getpid();
    }
    clock_gettime(CLOCK_MONOTONIC,&end);
    elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    elapsed_ns_corrected = elapsed_ns - total_time_loss;

    // Ermittle geringste benötigte Zeit für einen getpid() Aufruf
    for (long i = 0; i < ITERATIONS; i++) {
        clock_gettime(CLOCK_MONOTONIC,&start);
	getpid();
	clock_gettime(CLOCK_MONOTONIC,&end);
	single_elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
	if (single_elapsed_ns < elapsed_ns_min) elapsed_ns_min = single_elapsed_ns;
    }
    elapsed_ns_corrected = elapsed_ns - total_time_loss;
    clock_gettime(CLOCK_MONOTONIC,&start);
    getpid();
    clock_gettime(CLOCK_MONOTONIC,&end);
    single_elapsed_ns = (end.tv_sec - start.tv_sec) * 1e9 + (end.tv_nsec - start.tv_nsec);
    printf("Latenz Einzelaufruf: %lld\n ns",  single_elapsed_ns);
    // Zeitdifferenz in Nanosekunden berechnen
    double avg_ns = elapsed_ns_corrected / (double) ITERATIONS;
    // Ergebnis ausgeben
    printf("Gesamtlaufzeit für %d Iterationen: %lld ns, Durchschnitt: %.6f ns\n", ITERATIONS, elapsed_ns_corrected, avg_ns);
    printf("Total Time Taken: %lld ns\nTotal Time Loss: %lld ns\n", elapsed_ns, total_time_loss);
    printf("\nGeringste Zeit: %lld", elapsed_ns_min);
    return 0;
}
