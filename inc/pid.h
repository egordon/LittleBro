#ifndef PID_H
#define PID_H

typedef struct PID PID_T;

PID_T PID_init();

void PID_gains(PID_T pid, double kP, double kI, double kD);

void PID_clamp(PID_T pid, double min, double max);

/* Clear stored D and I state. */
void PID_clear(PID_T pid);

/* Each iteration, get output from error. */
double PID_update(PID_T pid, double error);

void PID_free(PID_T pid);

#endif