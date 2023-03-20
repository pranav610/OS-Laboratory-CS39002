
#include <bits/stdc++.h>
#include "data_structures.hpp"
#include <semaphore.h>

#ifndef THREADS_HPP
#define THREADS_HPP

using namespace std;
void *guest(void *arg);
void *cleaner(void *arg);

#endif