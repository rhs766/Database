#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define DBPOS 11	// 10 records in database (1-10)

static int gwcount;	// number of requests that waited due to its group
static int uwcount;	// number of requests that waited due to used position
static int startgroup;	// starting group
static int startgroupnum;	// number of requests in starting group
static int nextgroup;	// next group
static int donecount;	// number of requests that finished
static int totalcount;	// total number of requests
static int dbuser[DBPOS];	// array that indicates the current user of data records
static pthread_mutex_t bsem;	// database mutex
static pthread_cond_t grp = PTHREAD_COND_INITIALIZER;	// condition variable to block due to its group
static pthread_cond_t used[DBPOS] = {PTHREAD_COND_INITIALIZER, 	// array of condition variables to block due to used position
	PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER,
	PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER,
	PTHREAD_COND_INITIALIZER, PTHREAD_COND_INITIALIZER};

// each request has the following info
struct Request {
	int group;	// user group
	int pos;	// position requested
	int time;	// arrival time
	int dur;	// duration time
	int tnum;	// user number
};

// simulate db requests to access data records
void *access_db(void *request)
{
	pthread_mutex_lock(&bsem);
	struct Request *req = (struct Request *) request;
	printf("User %d from Group %d arrives to the DBMS\n", req->tnum, req->group);

	// condition for group
	if(req->group != startgroup)
	{	
		gwcount++;
		printf("User %d is waiting due to its group\n", req->tnum);
		
		// edge case of no requests in start group
		if(gwcount == totalcount)
		{
			printf("\n");
			printf("All users from Group %d finished their execution\n", startgroup);
			printf("The users from Group %d start their execution\n", nextgroup);
			printf("\n");
			startgroup = nextgroup;
			pthread_cond_broadcast(&grp);
		}
		else
		{
			pthread_cond_wait(&grp, &bsem);
		}
	}

	// condition for position
	if(dbuser[req->pos] != 0)
	{
		uwcount++;
		printf("User %d is waiting: position %d of the database is being used by user %d\n", req->tnum, req->pos, dbuser[req->pos]);
		pthread_cond_wait(&used[req->pos], &bsem);
	}

	// access db record
	dbuser[req->pos] = req->tnum;
	printf("User %d is accessing the position %d of the database for %d second(s)\n", req->tnum, req->pos, req->dur);
	pthread_mutex_unlock(&bsem);
	
	// simulate db record access for duration
	sleep(req->dur);

	pthread_mutex_lock(&bsem);
	printf("User %d finished its execution\n", req->tnum);
	donecount++;
	dbuser[req->pos] = 0;

	// signal request waiting for same position
	pthread_cond_signal(&used[req->pos]);

	// broadcast requests waiting for next group
	if(donecount == startgroupnum)
	{
		printf("\n");
		printf("All users from Group %d finished their execution\n", startgroup);
		printf("The users from Group %d start their execution\n", nextgroup);
		printf("\n");
		startgroup = nextgroup;
		pthread_cond_broadcast(&grp);
	}
	pthread_mutex_unlock(&bsem);
	return NULL;
}

int main()
{
	int g1count = 0;	// number of requests in group 1
	int g2count = 0;	// number of requests in group 2
	int tcount = 0;		// count number of requests
	int buffer[256];	// input buffer
	int buffpos = 0;	// iterates through input buffer

	// set startgroup
	scanf("%d", &startgroup);

	// set nextgroup
	if(startgroup == 1)
	{
		nextgroup = 2;
	}
	else
	{
		nextgroup = 1;
	}

	// read input from input file
	while(scanf("%d", &buffer[buffpos]) == 1)
	{
		scanf("%d", &buffer[buffpos+1]);
		scanf("%d", &buffer[buffpos+2]);
		scanf("%d", &buffer[buffpos+3]);
		buffpos += 4;
		tcount++;
	}
	buffpos = 0;
	
	// make array of requests
	struct Request req[tcount];
	
	// create requests with corresponding info
	for(int i = 0; i < tcount; i++)
	{
		req[i].group = buffer[buffpos];
		req[i].pos = buffer[buffpos+1];
		req[i].time = buffer[buffpos+2];
		req[i].dur = buffer[buffpos+3];
		req[i].tnum = i + 1;
		buffpos += 4;

		// track number of requests per group
		if(req[i].group == 1)
		{
			g1count++;
		}
		else
		{
			g2count++;
		}		
	}
	
	// set number of requests for starting group
	if(startgroup == 1)
	{
		startgroupnum = g1count;
	}
	else
	{
		startgroupnum = g2count;
	}

	// calculate total amount of requests
	totalcount = g1count + g2count;
	
	// array of tid's
	pthread_t tid[tcount];
	
	// initialize bsem access to 1
	pthread_mutex_init(&bsem, NULL);
	
	// simulate arrival of requests
	for(int i = 0; i < tcount; i++)
	{
		sleep(req[i].time);
		if(pthread_create(&tid[i], NULL, access_db,(void *)&req[i])) 
		{
			fprintf(stderr, "Error creating thread\n");
			return 1;
		}
	}

	// Wait for threads to finish.
	for (int i = 0; i < tcount; i++)
        	pthread_join(tid[i], NULL);

	// print summary
	printf("\n");
	printf("Total Requests:\n");
	printf("\tGroup 1: %d\n", g1count);
	printf("\tGroup 2: %d\n", g2count);
	printf("\n");
	printf("Requests that waited:\n");
	printf("\tDue to its group: %d\n", gwcount);
	printf("\tDue to a locked position: %d\n", uwcount);
	return 0;
}
