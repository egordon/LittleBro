#ifndef PID_H
#define PID_H

typedef struct PID *PID_T;

// angleOrNot is true if this PID loop is controlling angle measurements
// ranging from 0 to TWO_PI, and angleOrNot is false otherwise
PID_T PID_init(int angleOrNot);

void PID_setpoint(PID_T pid, double setPoint);

void PID_gains(PID_T pid, double kP, double kI, double kD);

void PID_clamp(PID_T pid, double min, double max);

/* Clear stored D and I state. */
void PID_clear(PID_T pid);

/* Each iteration, get output from measurement. */
double PID_update(PID_T pid, double measurement, double dt);

// return the setpoint
double PID_getSetpoint(PID_T pid);

void PID_free(PID_T pid);

#endif