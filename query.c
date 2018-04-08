/* COMP20007 Design of Algorithms Assignment 1
 * Written by Jia Hao Liu
 * Codes from heap.h and heap.c is sourced from LMS written by Andrew Turpin
 * and Matt Farrugia. */

#include <stdio.h>
#include <stdlib.h>
#include "query.h"
#include "list.h"
#include "heap.h"

/* Task 1 - Array-based accumulation approach:
 * Total document scores for a document ID in an array and find top k elements
 * from this using min-heap based priority queue. */
void print_array_results(Index *index, int n_results, int n_documents) {

	/* Terminate immediately if no outputs requested */
	if (n_results <= 0) {
		return;
	}

	int i = 0;
	float *array_doc = malloc(n_documents * sizeof(float));
	/* Set all scores in array_doc to 0.0 ready for summing */
	while (i < n_documents) {
		array_doc[i] = 0.0;
		i++;
	}

	/* Iterate through linked list of documents keeping a running total of scores
	 * for each document ID */
	for(i=0; i < index->num_terms; i++) {
		Node *cur = index->doclists[i]->head;
		/* Iterate through all docs in linked list */
		while(cur != NULL) {
			Document *doc = cur->data;
			array_doc[doc->id] += doc->score; // total scores
			cur = cur->next; // traverse to next node
		}
	}

	/* Create min-heap priority queue */
	Heap *h = new_heap(n_results);
	/* Iterate through array of document scores and pick out top k scores */
	for (i = 0; i < n_documents; i++) {
		/* Swap out the smallest item from the queue if queue is full and a higher
		 * score is encountered */
		if (h->cur_size >= n_results && array_doc[i] > heap_peek_key(h)) {
			heap_remove_min(h);
			heap_insert(h, array_doc[i], i);
		}
		/* Do nothing if smallest score in queue is larger than one in array_doc */
		else if (h->cur_size >= n_results && array_doc[i] <= heap_peek_key(h)) {
		}
		else if (array_doc[i] > 0.0){
			heap_insert(h, array_doc[i], i);
		}
	}

	print_heap(h);
	free(array_doc);
	free_heap(h);
}

/* Task 2 - Priority queue-based multi-way merge approach:
 * Set up two priority queues, one for doclists and one for scores. Step
 * through all lists simultaneously to add scores of the same doc ID at once.
 * Then use the min-heap priority queue to find top k elements */
void print_merge_results(Index *index, int n_results) {

	/* Terminate immediately if no outputs requested */
	if (n_results <= 0) {
		return;
	}

	int i = 0, null_count = 0, i_front, cur_id;
	float cur_sc = 0.0;
	/* Create array storing the current node for a list of docs */
	Node *array_nodes = malloc(index->num_terms * sizeof(Node));
	/* Create 2 min-heap priority queue storing ID/index and score/ID */
	Heap *h_id = new_heap(index->num_terms);
	Heap *h_score = new_heap(n_results);

	/* Add node of linked list to a priority queue. Keep track of where you are
	 * in linked list by storing the node in array_nodes */
	while (i < index->num_terms) {
		array_nodes[i] = *index->doclists[i]->head;
	 	Document *doc = index->doclists[i]->head->data;
		heap_insert(h_id, doc->id, i);
		i++;
	}

	Document *doc = array_nodes[heap_peek_min(h_id)].data;
	cur_id = doc->id; // initialise first doc ID

	/* Keep iterating until all doclists have reached NULL */
	while (null_count <= index->num_terms) {
	  i_front = heap_peek_min(h_id); // find which doclist is at front of queue
		/* If next doc has the same ID as current then add score to total */
	  if (heap_peek_key(h_id) == cur_id) {

	    cur_sc += doc->score;
	  }

		/* If not insert score/ID pair to heap and update cur_sc and cur_id */
		else {
			/* Swap out the smallest item from the queue if queue is full and a higher
			 * score is encountered */
			if (h_score->cur_size >= n_results && cur_sc > heap_peek_key(h_score)) {
				heap_remove_min(h_score);
				heap_insert(h_score, cur_sc, cur_id);
			}
			/* Do nothing if smallest score in queue is larger than one in array_doc
			 */
			else if (h_score->cur_size >= n_results && cur_sc <= heap_peek_key(h_score)) {
			}
			else {
				heap_insert(h_score, cur_sc, cur_id);
			}
			cur_id = doc->id;
			cur_sc = doc->score;
		}

		/* If next node is NULL: remove corresponding HeapItem from priority queue
		 * and continue iteration */
		if (array_nodes[i_front].next == NULL) {
			if (h_id->cur_size == 1) {
				null_count++;
				continue;
			}
			heap_remove_min(h_id);
			null_count++; // increment NULL counter
			continue;
		}

		else {
			/* Update node to next */
			array_nodes[i_front] = *array_nodes[i_front].next;
			doc = array_nodes[i_front].data;
			/* Update queue */
			heap_remove_min(h_id); // dequeue front of queue
			heap_insert(h_id, doc->id, i_front); // enqueue based on new ID
			doc = array_nodes[heap_peek_min(h_id)].data;
		}
	}

	print_heap(h_score);
	free(array_nodes);
	free_heap(h_id);
	free_heap(h_score);
}
