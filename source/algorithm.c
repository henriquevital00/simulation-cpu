#include "../include/input.h"
#include "../include/process.h"
#include "../include/queue.h"

/* UTILS */
Process* (*currentProcess)() = first;

void dequeueProcess(){
    Process current = *currentProcess();
    current.quantumCount = 0;

    /* Move to the end of the queue*/
    pop();
    push(current);
}

void incrementTime(int time){
    if (currentProcess()->duration == currentProcess()->startDuration)
        currentProcess()->startTime = time;
    currentProcess()->duration--;
}

int calculateTotalDuration(Process *processes){
    int duration = 0;
    for (int i = 0; i < numberProcess; i++)
        duration += processes[i].duration;
    return duration + 1;
}

void calculateProcessWaitTime(int *totalWaitTime){
    int waitTime = (currentProcess()->finalTime - currentProcess()->startTime);
    (*totalWaitTime) += waitTime;
    printf("Tempo de espera: %d ms\n", waitTime);
}

float calculateWaitTimeAverage(int *totalWaitTime){
    return *totalWaitTime / numberProcess;
}

void onExitProcess(int time, int *totalWaitTime){
    if (currentProcess()->duration == 0){
        printf("fim de processo: P%d\n", currentProcess()->number);
        currentProcess()->finalTime = time;
        calculateProcessWaitTime(totalWaitTime);

        pop();
    }
}

/* INSERTION */
bool isInserting(Process *processes, int time){
    for (int n = 0; n < numberProcess; n++){
        Process process = processes[n];
        if (process.arrival == time)
            return true;
    }
    return false;
}

void onInsertAtTime(Process *processes, int time){
    for (int n = 0; n < numberProcess; n++){
        Process process = processes[n];
        if (process.arrival == time){
            push(process);
            printf("Chegada do processo: P%d\n", process.number);
        }
    }
}

/* QUANTUM */
bool isQuantumOverflow(int quantum){
    if (currentProcess()){
        if (currentProcess()->quantumCount == quantum)
            return true;
    }
    return false;
}

void onOverflowQuantum(){
    printf("fim de quantum: P%d\n", currentProcess()->number);
    dequeueProcess();
}

void incrementQuantum(){
    if (currentProcess())
        currentProcess()->quantumCount++;
}

/* INTERRUPTION */
bool isInterrupting(){
    if (currentProcess()){
        for (int i = 0; i < currentProcess()->numberIO; i++){
            int remainingTime = currentProcess()->startDuration - currentProcess()->duration;

            if (currentProcess()->interruptions[i] == remainingTime)
                return true;
        }
    }
    return false;
}

void onInterruptProcess(){
    printf("operação de I/O: P%d\n", currentProcess()->number);
    dequeueProcess();
}

/* ALGORITHM */
void roundRobbin(Process *processes){
    int totalDuration = calculateTotalDuration(processes);
    int quantum = 4;
    int totalWaitTime = 0;

    for (int t = 0; t < totalDuration; t++){
        printf("\n\nTempo: %d - ", t);

        bool hasInsertion = isInserting(processes, t);
        bool hasInterruption = isInterrupting();
        bool hasQuantumOverflow = isQuantumOverflow(quantum);

        if (hasInsertion && hasInterruption){
            onInterruptProcess();
            onInsertAtTime(processes, t);
        }
        else if (hasInterruption || (hasQuantumOverflow && hasInterruption))
            onInterruptProcess();
        else if (hasInsertion)
            onInsertAtTime(processes, t);
        else if (hasQuantumOverflow)
            onOverflowQuantum();

        showQueue();
        onExitProcess(t, &totalWaitTime);

        if (currentProcess()){
            printf("Na CPU: P%d \n", currentProcess()->number);
            incrementQuantum();
            incrementTime(t);
        }
    }

    printf("Tempo médio de espera = %.2f", calculateWaitTimeAverage(&totalWaitTime));
}

void calculate(Process *processes){
    createQueue();
    roundRobbin(processes);
}