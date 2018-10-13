#include <cstdio>
#include <vector>
#include <pthread.h>


class MySemaphore 
{
private:
    int val;
    std::vector<int> flags;
    std::vector<int> turn;
    bool amIPlayingTheHighestRound(int me, int myRound); 
    void playTheTournament(int me);
    void donePlaying(int me);
public:
    explicit MySemaphore(size_t numThreads, int initVal);
    void init(int initVal);
    void signal(int me);
    void wait(int me);
};

bool MySemaphore::amIPlayingTheHighestRound(int me, int myRound) 
{
    for (int i = 0; i < flags.size(); ++i) 
    {
        if (i == me)
            continue;
        if (flags[i] >= myRound )
		return false; 
    }
    return true;
}

void MySemaphore::playTheTournament(int me) 
{
    for (int round = 0; round < turn.size(); round++) 
    {
        flags[me] = round;
        turn[round] = me;

        while (!amIPlayingTheHighestRound(me, round) && turn[round] == me) {}
    }
}

void MySemaphore::donePlaying(int me)
{
	flags[me] = -1;
}

void MySemaphore::init(int initVal) 
{
	val = initVal;
}

MySemaphore::MySemaphore(size_t numThreads, int initVal) 
{
    flags.resize(numThreads, -1);
    turn.resize(numThreads-1, 0);
    init(initVal);
}

void MySemaphore::signal(int me) 
{
	playTheTournament(me);
	val++;
	donePlaying(me);
}

void MySemaphore::wait(int me) 
{
    bool done = false;
    while (!done) 
    {
	playTheTournament(me);
	if( val > 0 )
	{
		val--;
		done = true;
	}
	donePlaying(me);
    }
}

static int shared_num = 0;
static MySemaphore sem = MySemaphore(5,1);

void* my_thread(void* args) {
    int me = *(int*)args;
    sem.wait(me);
    shared_num++;
    printf("after Thread: %d, shared: %d\n", me, shared_num);
    sem.signal(me);
    return NULL;
}

int main() {
    pthread_t threads[5];
    int id[5];
    int i;
    for (i = 0; i < 5; ++i) {
        id[i] = i;
    }
    for (i = 0; i < 5; ++i) {
        pthread_create(&threads[i], NULL, my_thread, &id[i]);
    }
    for (i = 0; i < 5; ++i) {
        pthread_join(threads[i], NULL);
    }
    printf("Completed Shared: %d\n", shared_num);
    return 0;
}

