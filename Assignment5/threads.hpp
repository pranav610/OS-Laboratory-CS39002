#ifndef THREADS_HPP
#define THREADS_HPP

#include <bits/stdc++.h>
#include "data_structures.hpp"
#include <semaphore.h>
#include <random>
#include <unistd.h>
#include <semaphore.h>

#include "data_structures.hpp"

using namespace std;

void *guest(void *arg);
void *cleaner(void *arg);

#endif