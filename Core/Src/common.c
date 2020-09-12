/*
 * common.c
 *
 *  Created on: Sep 8, 2020
 *      Author: Matt
 */

#include "common.h"

///////////////// PEDAL STATE /////////////////////////



//////////////// QUEUE IMPLEMENTATION //////////////////
//TODO QUEUE IS BORK
queue_t* init_queue(void){
	queue_t *queue = NULL;

	queue = (queue_t*)malloc(sizeof(queue_t));

	if(!queue) return NULL;

	queue->items = (press_t*)malloc(QUEUE_MAX * sizeof(press_t));
	if(!queue->items) return NULL;

	queue->front = 0;
	queue->rear = -1;
	queue->n_items = 0;

	return queue;
}

int size(queue_t *queue){
	return queue->n_items;
}

bool_t isEmpty(queue_t *queue){
	return size(queue) ? TRUE : FALSE;
}

int peak(queue_t *queue){
	if(isEmpty(queue))
		return -1;

	return queue->items[queue->front];
}

int enqueue(queue_t *queue, press_t press){

	if(size(queue) == QUEUE_MAX)
		return -1;

	queue->rear  = (queue->rear + 1) % QUEUE_MAX;
	queue->items[queue->rear] = press;
	queue->n_items++;
	return 1;
}

press_t dequeue(queue_t *queue){
	if(isEmpty(queue))
		return None;
	press_t ret = peak(queue);
	queue->front = (queue->front + 1) % QUEUE_MAX;
	queue->n_items--;
	return ret;
}
////////////////// END QUEUE IMPLEMENTATION /////////////////
