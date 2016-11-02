#include <median_filter.h>


void medianFilterInit(medianFilter_t *f, uint32_t value) {
  int32_t i;

  f->data[FILTER_MAX_INDEX] = value;
  f->node[FILTER_MAX_INDEX].value = FILTER_MAX_INDEX; 
  f->node[FILTER_MAX_INDEX].next = FILTER_NODE_NULL; 
  for (i=FILTER_MAX_INDEX-1; i>=0; i--) {
    f->data[i] = value; 
    f->node[i].value = i;
    f->node[i].next = &f->node[i+1];
  }
  f->dataIn = 0U;
  f->list.value = 0U;
  f->list.next = &f->node[0];
}

uint32_t medianFilterAdd(medianFilter_t *f, uint32_t value) {
  filterNode_t *predp = FILTER_NODE_NULL;
  filterNode_t *dp = FILTER_NODE_NULL;
  filterNode_t *ep = FILTER_NODE_NULL;
  filterNode_t *tp = &f->list;
  int32_t i;

  while (tp->next != FILTER_NODE_NULL) {
    if (tp->next->value == f->dataIn) {
      predp = tp;
      dp = tp->next;
    }
    if ((ep == FILTER_NODE_NULL) && (value < f->data[tp->next->value])) {
      ep = tp;
    }
    tp = tp->next;
  }
  f->data[f->dataIn] = value;
  f->dataIn = f->dataIn < FILTER_MAX_INDEX ? f->dataIn + 1 : 0;
  if (ep == FILTER_NODE_NULL) {
    ep = tp;
  }
  if (ep != dp) {
    predp->next = dp->next;
    dp->next = ep->next;
    ep->next = dp;
  }
  tp = f->list.next;
  for (i = FILTER_MAX_INDEX / 2; i > 0; i--) {
    tp = tp->next;
  }
  return f->data[tp->value];
}

#if 0
#include <stdlib.h>

int main(void) {
  medianFilter_t f;
  int32_t i;
  uint32_t value;

  medianFilterInit(&f, 50);
  for (i = 0; i < 1000000; i++) {
    value = (uint32_t)rand();
    medianFilterAdd(&f, value);
  }  
  return 0;
}

#endif
