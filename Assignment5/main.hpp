#include <bits/stdc++.h>
#include <semaphore.h>
#include <pthread.h>

#define SLEEP_MIN 10
#define SLEEP_MAX 20
#define STAY_MIN 10
#define STAY_MAX 30

using namespace std;

void *guest(void *arg);
void *cleaner(void *arg);

