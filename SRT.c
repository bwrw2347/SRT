#include <stdio.h>
#include <stdlib.h>
#define TIME_QUANTUM 20

double turnaround_time;            
double normalized_time;            	 // Tr/Ts = Turnaround time / Computing Time 
double total_turnaroundtime = 0.0;  
double total_normalizedtime = 0.0;  

int process_cnt = 0;                     // Number of process  
int remain_timequantum = TIME_QUANTUM;   
int present_time = 0;                     

// Queue 
typedef struct Node {              
	int process_id;                  // Pid
	int priority;
	int computing_time;              // (Real time) Computing time (changeable)
	int first_computing_time;        // (First) Computing time (unchangeable)
	int enter_time;                  
	struct Node *llink, *rlink;
}Node;

// Initializing Queue
void init(Node *head) {
	head->rlink = head;
	head->llink = head;
}

// Creating Node 
Node *create_node(int process_id, int priority, int computing_time) {
	Node *new_node;
	new_node = (Node *)malloc(sizeof(Node));
	new_node->process_id = process_id;
	new_node->priority = priority;
	new_node->computing_time = computing_time;
	new_node->first_computing_time = computing_time;
	new_node->enter_time = present_time;

	return new_node;
}

// Inserting Node
void insert_node(Node *before, Node *new_node) {
	new_node->llink = before;
	new_node->rlink = before->rlink;
	before->rlink->llink = new_node;
	before->rlink = new_node;
}

// Removing Node
void remove_node(Node *node, Node *removed) {

	if (removed == node) return;
	removed->llink->rlink = removed->rlink;
	removed->rlink->llink = removed->llink;
	free(removed);
}

// Finding location of Node
Node *find_loc(Node *head, int computing_time) {
	Node *q = NULL;
	q = head;

	// Node is Located according to computing_time. Use FCFS when computing_time is same.
	while (q->rlink != head) {
		if (computing_time >= q->rlink->computing_time)
			q = q->rlink;
		else
			break;
	}
	return q;
}

// Printing Node
void print_node(Node *q) {
	present_time += q->rlink->computing_time;              // Renew present_time
	remain_timequantum -= q->rlink->computing_time;        // Renew remain_timequantum 
	turnaround_time = present_time - q->rlink->enter_time;                
	total_turnaroundtime += turnaround_time; process_cnt++;               
	normalized_time = (turnaround_time / q->rlink->first_computing_time); 
	total_normalizedtime += normalized_time;                              

	printf("\t%d\t\t%d\t\t%d\t\t%.lf\t\t\t%.4lf\n", q->rlink->process_id, q->rlink->priority,
		       	q->rlink->first_computing_time, turnaround_time, normalized_time);
}

// Queue Calculating 1 (When there's a new incoming process)
void cal_queue(Node *head, int type) {
	Node *q = NULL;
	q = head;
	
	 
 	// Print when computing_time is less than TIME_QUANTUM  
 	// Repeat until the remain_timequantum is 0  
	if (q->rlink->computing_time <= TIME_QUANTUM && q->rlink != head) {
		print_node(q);
		remove_node(head, q->rlink);

		while (remain_timequantum > 0) { 
			if (q->rlink->computing_time <= remain_timequantum && q->rlink != head) {
					print_node(q);
					remove_node(head, q->rlink);
			}
      			else { // Bigger than TIME_QUANTUM or No more inserting node for a moment
				present_time += remain_timequantum; 
				q->rlink->computing_time -= remain_timequantum; 
				break;
			}
		}
	} 
 	// Just renew present_time and  computing_time when computing time is greater than TIME_QUANTUM
	else if (q->rlink->computing_time > TIME_QUANTUM) { 
		present_time += TIME_QUANTUM; // 
		q->rlink->computing_time -= TIME_QUANTUM;
	}
}

// Queue Calculating 2 (When there's no more incoming process)
void cal_last_queue(Node *head) {
	Node *q = NULL;
	double turnaround_time, normalized_time;

	q = head;
	while (q->rlink != head) {
		print_node(q);
		q = q->rlink;
	}
	printf("Average_Turnaround_Time: %.4lf\nAverage_Normalized_Turnaround_Time: %.4lf\n",
		       	total_turnaroundtime / process_cnt, total_normalizedtime / process_cnt);
}
	
int main(int argc, char *argv[]){

        FILE* stream;
        Node head; 

        int type, process_id[TIME_QUANTUM], priority[TIME_QUANTUM], computing_time[TIME_QUANTUM]; // type 1 == TIME_QUANTUM
        int cnt = 0;
        int zero, i;

	if(argc != 2){
		fputs("./srt <input.txt>\n", stderr);
		exit(0);
	}

        init(&head);
	stream = fopen(argv[1], "r");
        printf("    Process_id\t     Priority     Computing_time   Turnaround_time     Normalized_turnaround_time\n");
        while (1) {

                fscanf(stream, "%d", &type);
                if (type == 0 ) {
                        fscanf(stream,"%d %d %d", &process_id[cnt], &priority[cnt], &computing_time[cnt]);
			cnt++;
                }else if (type == 1 || type == -1){
                        for (i = 0; i < cnt; i++)
				insert_node(find_loc(&head, computing_time[i]), create_node(process_id[i], priority[i], computing_time[i]));
                        cnt = 0; 
			remain_timequantum = TIME_QUANTUM;

                        cal_queue(&head, type);
                        if (type == 1)
                                fscanf(stream, "%d %d %d", &zero, &zero, &zero);
                        else if (type == -1)
                                break;
                }
        }

        cal_last_queue(&head);
	free(stream);

        return 0;
}
