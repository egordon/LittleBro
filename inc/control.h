/* control.h */

#ifndef CONTROL_H
#define CONTROL_H

typedef struct Control *Control_T;

Control_T Control_init(int pifd);

// turn for secs seconds
// these are absolute turns, not relative turns
// I can write relative turns fairly easily, I think. IDK if they are useful, though.
void Control_turnNorth(Control_T oControl, double secs);
void Control_turnEast(Control_T oControl, double secs);
void Control_turnSouth(Control_T oControl, double secs);
void Control_turnWest(Control_T oControl, double secs);

// advance one square forward in the maze
// return 0 if success
int Control_advance(Control_T oControl);

// eventually add things like Control_changeHomeGrid

void Control_free(Control_T oControl);

#endif