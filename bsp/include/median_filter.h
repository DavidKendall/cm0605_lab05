#ifndef __MEDIAN_FILTER_H
#define __MEDIAN_FILTER_H

#include <stdint.h>

#define FILTER_NODE_NULL (((filterNode_t *)0))

enum {
  FILTER_SIZE = 11,
  FILTER_MAX_INDEX = FILTER_SIZE - 1
};

typedef struct filterNode {
  uint32_t value;
  struct filterNode *next;
} filterNode_t;

typedef struct {
  uint32_t data[FILTER_SIZE];
  uint32_t dataIn;
  filterNode_t list;
  filterNode_t node[FILTER_SIZE];
} medianFilter_t;


void medianFilterInit(medianFilter_t *f, uint32_t value);
uint32_t medianFilterAdd(medianFilter_t *f, uint32_t value);

#endif