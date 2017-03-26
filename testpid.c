/* testpid.c */

/* format of input file should be:
"
	kp ki kd
	setpoint
	kMin kMax
	error0 error1 error2 ...
"
*/

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <pid.h>

int main() {
	double error;
	double kpIn, kiIn, kdIn, setPointIn, kMinIn, kMaxIn;
	printf("Hi Ethan\n");
	printf("Input as follows: \'kp ki kd\'\n");
	scanf("%lf %lf %lf", &kpIn, &kiIn, &kdIn);

	printf("Input as follows: \'setPoint kMinForClamp kMaxForClamp\'\n");
	scanf("%lf %lf %lf", &setPointIn, &kMinIn, &kMaxIn);

	PID_T pid = PID_init();
	PID_setpoint(pid, setPointIn);
	PID_gains(pid, kpIn, kiIn, kdIn);
	PID_clamp(pid, kMinIn, kMaxIn);
	printf("PID successfully initialized. kp is %f, ki is %f, kd is %f\n", kpIn, kiIn, kdIn);
	printf("Clamp min is: %f   max is: %f\n", kMinIn, kMaxIn);
	while (scanf(" %lf", &error) == 1 && !feof(stdin)) {
		printf("Output is: %f \n", PID_update(pid, error));
	}

	return 0;
}