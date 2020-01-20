#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/time.h>


typedef struct node{
	int state[16];
	int g;
	int f;
	int last;
} node;


/**
 * Global Variables
 */

// used to track the position of the blank in a state,
// so it doesn't have to be searched every time we check if an operator is applicable
// When we apply an operator, blank_pos is updated
int blank_pos;

// Initial node of the problem
node initial_node;

// Statistics about the number of generated and expendad nodes
unsigned long generated;
unsigned long expanded;


/**
 * The id of the four available actions for moving the blank (empty slot). e.x.
 * Left: moves the blank to the left, etc.
 */

#define LEFT 0
#define RIGHT 1
#define UP 2
#define DOWN 3
#define PUZZLE_LEN 16
#define NO_MOVE -2
#define MAX_MOVES 4
#define SIDE_LEN 4
/*
 * Helper arrays for the applicable function
 * applicability of operators: 0 = left, 1 = right, 2 = up, 3 = down
 */
int ap_opLeft[]  = { 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1 };
int ap_opRight[]  = { 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0, 1, 1, 1, 0 };
int ap_opUp[]  = { 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };
int ap_opDown[]  = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0 };
int *ap_ops[] = { ap_opLeft, ap_opRight, ap_opUp, ap_opDown };


/* print state */
void print_state( int* s )
{
	int i;

	for( i = 0; i < PUZZLE_LEN; i++ )
		printf( "%2d%c", s[i], ((i+1) % 4 == 0 ? '\n' : ' ') );
}

void printf_comma (long unsigned int n) {
    if (n < 0) {
        printf ("-");
        printf_comma (-n);
        return;
    }
    if (n < 1000) {
        printf ("%lu", n);
        return;
    }
    printf_comma (n/1000);
    printf (",%03lu", n%1000);
}

void copy_state(struct node* new, struct node* old){
	int i = 0;

	for(i = 0; i < PUZZLE_LEN; i++){
		new->state[i] = old->state[i];
	}
}

/* return the sum of manhattan distances from state to goal */
int manhattan( int* state )
{
	int i, sum = 0;
	for(i = 0; i < PUZZLE_LEN ; i++){
		if(state[i]){
			sum += abs((state[i] / SIDE_LEN) - (i / SIDE_LEN)) +
				   abs((state[i]%4) - (i % SIDE_LEN));
		}
	}
	return( sum );
}




/* return 1 if op is applicable in state, otherwise return 0 */
int applicable( int op)
{
       	return( ap_ops[op][blank_pos] );
}


/* apply operator */
void apply( node* n, int op)
{
	int t;

	//find tile that has to be moved given the op and blank_pos
	t = blank_pos + (op == 0 ? -1 : (op == 1 ? 1 : (op == 2 ? -4 : 4)));
	//printf("op: %d\nt: %d\n",op, t);
	//apply op
	n->state[blank_pos] = n->state[t];
	n->state[t] = 0;

}

/* Function returns the opposite operator value of an input operator. */
int last_move(int op){
	return (op == LEFT ? RIGHT : (op == RIGHT ? LEFT :
		(op == UP ? DOWN : (op == DOWN ? UP: NO_MOVE))));
}

int find_blank(node* node){
	int j;
	for(j = 0; j < PUZZLE_LEN; j++){
	   if(node->state[j] == 0){
		   return j;
	   }
   	}
	return -1;
}

/* Recursive IDA */
node* ida( node* node, int threshold, int* newThreshold){

	int i = 0;
	struct node test_node;
	struct node* r = NULL;

	/* Update blank_pos to reflect current actual blank position. */
	blank_pos = find_blank(node);
	int call_blank = blank_pos;

	/* Find all applicable moves for current state. */
	int app[MAX_MOVES];
	for(i = 0; i < MAX_MOVES; i++){
		app[i] = applicable(i);
	}

	/* For all applicable moves. */
	 for(i = 0; i < MAX_MOVES; i++){
		 /* Does not perform opposite of last move. */
		 if(app[i] && i != node->last){

			 /* Increase number of generated nodes. */
			 generated++;

			 /* Set blank_pos to represent blank position at call. */
			 blank_pos = call_blank;

			 /* Copy state of called node to new node. */
			 copy_state(&test_node, node);

			 /* Apply operator. */
			 apply(&test_node, i);

			 /* Update the last move flag made. */
			 test_node.last = last_move(i);

			 /* Update cost. */
			 test_node.g = node->g + 1;

			 /* Update evaluation function. */
			 test_node.f = test_node.g + manhattan(test_node.state);

			 /* Assess whether to prune node. */
			 if(test_node.f > threshold){
				 if(test_node.f < *newThreshold){
					 *newThreshold = test_node.f;
				 }
			 }
			 else{
				 /* Solution found. */
				 if(manhattan(test_node.state) ==  0){
					 r = &test_node;
					 return r;
				 }

				 /* Update number of expanded nodes. */
				 expanded++;

				 r = ida(&test_node, threshold, newThreshold);

				 if(r != NULL){
					 return r;
				 }
			 }
		 }

	 }


	return(NULL);
}


/* main IDA control loop */
int IDA_control_loop(  ){

	struct node* r = NULL;
	struct node exp_node;

	int threshold, newThreshold;

	/* initialize statistics */
	generated = expanded = 0;

	/* compute initial threshold B */
	initial_node.f = threshold = manhattan( initial_node.state );
	printf( "Initial Estimate = %d\nThreshold = ", threshold );


	while (r == NULL) {

		// Experimental node.
		copy_state(&exp_node, &initial_node);
		exp_node.g = 0;
		exp_node.last = -1;
		newThreshold = INT_MAX;
		r = ida(&exp_node, threshold, &newThreshold);
		if(r==NULL){
			threshold = newThreshold;
			printf("%d ", threshold);
		}
	}

	if(r){
		return r->g;
	}
	else
		return -1;
}


static inline float compute_current_time()
{
	struct rusage r_usage;

	getrusage( RUSAGE_SELF, &r_usage );
	float diff_time = (float) r_usage.ru_utime.tv_sec;
	diff_time += (float) r_usage.ru_stime.tv_sec;
	diff_time += (float) r_usage.ru_utime.tv_usec / (float)1000000;
	diff_time += (float) r_usage.ru_stime.tv_usec / (float)1000000;
	return diff_time;
}

int main( int argc, char **argv )
{
	int i, solution_length;

	/* check we have a initial state as parameter */
	if( argc != 2 )
	{
		fprintf( stderr, "usage: %s \"<initial-state-file>\"\n", argv[0] );
		return( -1 );
	}


	/* read initial state */
	FILE* initFile = fopen( argv[1], "r" );
	char buffer[256];

	if( fgets(buffer, sizeof(buffer), initFile) != NULL ){
		char* tile = strtok( buffer, " " );
		for( i = 0; tile != NULL; ++i )
			{
				initial_node.state[i] = atoi( tile );
				blank_pos = (initial_node.state[i] == 0 ? i : blank_pos);
				tile = strtok( NULL, " " );
			}
	}
	else{
		fprintf( stderr, "Filename empty\"\n" );
		return( -2 );

	}

	if( i != 16 )
	{
		fprintf( stderr, "invalid initial state\n" );
		return( -1 );
	}

	/* initialize the initial node */
	initial_node.g=0;
	initial_node.f=0;

	print_state( initial_node.state );


	/* solve */
	float t0 = compute_current_time();

	solution_length = IDA_control_loop();

	float tf = compute_current_time();

	/* report results */
	printf( "\nSolution = %d\n", solution_length);
	printf( "Generated = ");
	printf_comma(generated);
	printf("\nExpanded = ");
	printf_comma(expanded);
	printf( "\nTime (seconds) = %.2f\nExpanded/Second = ", tf-t0 );
	printf_comma((unsigned long int) expanded/(tf+0.00000001-t0));
	printf("\n\n");

	/* aggregate all executions in a file named report.dat, for marking purposes */
	FILE* report = fopen( "report.dat", "a" );

	fprintf( report, "%s", argv[1] );
	fprintf( report, "\n\tSoulution = %d, Generated = %lu, Expanded = %lu", solution_length, generated, expanded);
	fprintf( report, ", Time = %f, Expanded/Second = %f\n\n", tf-t0, (float)expanded/(tf-t0));
	fclose(report);

	return( 0 );
}
