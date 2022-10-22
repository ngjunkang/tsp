#include <algorithm>
#include <chrono>
#include <cmath>
#include <ctime>
#include <random>
#include <stdio.h>
#include <vector>

using namespace std;
using namespace chrono;

typedef vector<int> vi;

// obtained from codeforces blog entry:
// https://codeforces.com/blog/entry/79148
class timer : high_resolution_clock {
    public:
        time_point start_time = now();
        rep elapsed_time() const {
            return duration_cast<milliseconds>(now() - start_time).count();
        }
};

size_t pointsCount;
double points[1000][2];
double distMatrix[1000][1000];
vi bestTour;
double bestDist;
double minDist = 0;

double getDistance(int index1, int index2) {
    double yDist = (points[index1][1] - points[index2][1]) *
        (points[index1][1] - points[index2][1]);
    double xDist = (points[index1][0] - points[index2][0]) *
        (points[index1][0] - points[index2][0]);
    // remove sqrt for efficiency
    return sqrt(yDist + xDist);
}

// implementation: https://en.wikipedia.org/wiki/2-opt
void twoExchangeSwap(vi &tour, int i, int j) {
    reverse(tour.begin() + i + 1, tour.begin() + j + 1);
}

bool twoExchange(vi &tour) {
    bool improved = false;
    for (size_t i = 0; i < pointsCount - 2; ++i) {
        for (size_t j = i + 1; j < pointsCount - 1; ++j) {
            int distDelta =
                -distMatrix[tour[i]][tour[(i + 1) % pointsCount]] -
                distMatrix[tour[j]][tour[(j + 1) % pointsCount]] +
                distMatrix[tour[i]][tour[j]] +
                distMatrix[tour[(i + 1) % pointsCount]][tour[(j + 1) % pointsCount]];
            if (distDelta < 0) {
                twoExchangeSwap(tour, i, j);
                minDist += distDelta;
                improved = true;
            }
        }
    }
    return improved;
}

vi doubleBridge(vi &tour, mt19937& mt) {
    vi newTour;
    newTour.reserve(pointsCount);
    uniform_int_distribution<size_t> randomOffset(1, pointsCount / 4);
    size_t firstOffset = randomOffset(mt);
    size_t secondOffset = firstOffset + randomOffset(mt);
    size_t thirdOffset = secondOffset + randomOffset(mt);
    // start to first
    copy(tour.begin(), tour.begin() + firstOffset, back_inserter(newTour));
    // first to third, third to end
    copy(tour.begin() + thirdOffset, tour.end(), back_inserter(newTour));
    // end to second, second to third
    copy(tour.begin() + secondOffset, tour.begin() + thirdOffset, back_inserter(newTour));
    // third to first, first to second
    copy(tour.begin() + firstOffset, tour.begin() + secondOffset, back_inserter(newTour));
    minDist += distMatrix[tour[firstOffset - 1]][tour[thirdOffset]]
        + distMatrix[tour[pointsCount - 1]][tour[secondOffset]]
        + distMatrix[tour[thirdOffset - 1]][tour[firstOffset]]
        + distMatrix[tour[secondOffset - 1]][tour[0]];
    minDist -= (distMatrix[tour[firstOffset - 1]][tour[firstOffset]]
            + distMatrix[tour[pointsCount - 2]][tour[0]]
            + distMatrix[tour[thirdOffset - 1]][tour[thirdOffset]]
            + distMatrix[tour[secondOffset - 1]][tour[secondOffset]]);
    return newTour;
}

int main() {
    // uncomment when submitting
    // timer s;
    
    mt19937 mt(time(nullptr));
    size_t n;
    scanf("%ld", &n);
    pointsCount = n;

    // comment out when submitting
    timer s;

    for (int i = 0; i < n; ++i) {
        scanf("%lf %lf", &points[i][0], &points[i][1]);
    }

    if (n <= 3) {
        for (int i = 0; i < n; ++i) {
            printf("%d\n", i);
        }
        return 0;
    }

    // preprocess distance matrix, distance[1000][1000]
    // where distance[i][j] is the distance between point i and j
    // Did not implement lazy evaluation as every cell will likely
    // be used, and there will be used checks
    for (size_t i = 0; i < pointsCount; ++i) {
        for (size_t j = i + 1; j < pointsCount; ++j) {
            distMatrix[i][j] = distMatrix[j][i] = getDistance(i, j);
        }
    }

    // start init
    // random or greedy

    // random
    vi tour(n, -1);
    bestTour.reserve(n);

    for (int i = 0; i < n; ++i) {
        tour[i] = i;
    }

    shuffle(tour.begin(), tour.end(), mt);

    // calculate initial tour distance
    for (size_t i = 0; i < pointsCount - 1; ++i) {
        minDist += distMatrix[tour[i]][tour[i + 1]];
    }
    minDist += distMatrix[tour[pointsCount - 1]][tour[0]];
    bestDist = minDist;
    copy(tour.begin(), tour.end(), back_inserter(bestTour));

    // start step
    while (s.elapsed_time() < 1980) {
        if (!twoExchange(tour)) {
            if (minDist < bestDist) {
                bestTour.assign(tour.begin(), tour.end()); 
                bestDist = minDist;
            }
            tour = doubleBridge(bestTour, mt);
        }
    }
    for (int i = 0; i < tour.size(); i++) {
        printf("%d\n", bestTour[i]);
    }
}
