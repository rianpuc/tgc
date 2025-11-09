#ifndef __ALGORITHMS_H__
#define __ALGORITHMS_H__
#include "graph_generator.h"
#include <stdbool.h>
#include <time.h>

int algoritmoAproximado(Graph *graph);
int algoritmoExato(Graph *graph, clock_t start_time, double limit, bool *timeout_flag);

#endif