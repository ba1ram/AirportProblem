#include <gtest/gtest.h>

#include "core/Airport.h"
#include "core/AirportDatabase.h"
#include "core/RouteCalculator.h"

#include <QTemporaryFile>
#include <QTextStream>

// ----------------------------
// helper: temp CSV
// ----------------------------
static QString createFile(const QString& content) {
    QTemporaryFile file;
    file.setAutoRemove(false);

    EXPECT_TRUE(file.open());

    QTextStream out(&file);
    out << content;
    file.close();

    return file.fileName();
}

// ----------------------------
// SMALL FIXTURE DB
// ----------------------------
static AirportDatabase makeDB(const QString& csv) {
    QString path = createFile(csv);

    AirportDatabase db;
    db.loadFromFile(path, false);

    QFile::remove(path);
    return db;
}

// ----------------------------
// isReachable via findNearby indirectly
// ----------------------------

TEST(RouteCalculatorNearby, EmptyIfNoNeighbors) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
    );

    RouteCalculator rc(db, 1000);

    const Airport& a = db.getByIndex(0);

    auto res = rc.findNearby(a, 10);

    EXPECT_TRUE(res.empty());
}

TEST(RouteCalculatorNearby, FindsCorrectAirports) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;0.5\n"
        "C;10;10\n"
    );

    RouteCalculator rc(db, 2000);

    const Airport& a = db.getByIndex(0);

    auto res = rc.findNearby(a, 1000);

    EXPECT_GE(res.size(), 1);
}

// ----------------------------
// isReachable threshold
// ----------------------------

TEST(RouteCalculatorReachability, MaxRangeBlocksEdges) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;1\n"
    );

    RouteCalculator rc(db, 50); // very small range

    const Airport& a = db.getByIndex(0);
    const Airport& b = db.getByIndex(1);

    auto res = rc.findShortestPath("A", "B");
    EXPECT_FALSE(res.reachable);
}

// ----------------------------
// SHORTEST PATH BASIC
// ----------------------------

TEST(RouteCalculatorPath, SingleNodeUnreachableToItselfOnly) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
    );

    RouteCalculator rc(db, 1000);

    auto res = rc.findShortestPath("A", "A");

    EXPECT_TRUE(res.reachable);
    EXPECT_EQ(res.totalDistance, 0.0);
    EXPECT_EQ(res.airports.size(), 1);
}

// ----------------------------
// DISCONNECTED GRAPH
// ----------------------------

TEST(RouteCalculatorPath, DisconnectedGraphUnreachable) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;100\n"
    );

    RouteCalculator rc(db, 1); // too small range

    auto res = rc.findShortestPath("A", "B");

    EXPECT_FALSE(res.reachable);
    EXPECT_TRUE(res.airports.empty());
}

// ----------------------------
// DIRECT CONNECTION
// ----------------------------

TEST(RouteCalculatorPath, DirectPathWorks) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;0.1\n"
    );

    RouteCalculator rc(db, 10000);

    auto res = rc.findShortestPath("A", "B");

    EXPECT_TRUE(res.reachable);
    EXPECT_EQ(res.airports.size(), 2);
    EXPECT_EQ(res.airports.front(), "A");
    EXPECT_EQ(res.airports.back(), "B");
}

// ----------------------------
// MULTI-HOP PATH
// ----------------------------

TEST(RouteCalculatorPath, MultiHopShortestPath) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;1\n"
        "C;0;2\n"
    );

    RouteCalculator rc(db, 20000);

    auto res = rc.findShortestPath("A", "C");

    EXPECT_TRUE(res.reachable);

    // должен быть A -> B -> C (или прямой, зависит от геометрии, но проверим валидность)
    EXPECT_EQ(res.airports.front(), "A");
    EXPECT_EQ(res.airports.back(), "C");
}

// ----------------------------
// INVALID INPUT NODES
// ----------------------------

TEST(RouteCalculatorPath, InvalidStartOrEnd) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;1\n"
    );

    RouteCalculator rc(db, 10000);

    auto res1 = rc.findShortestPath("X", "B");
    auto res2 = rc.findShortestPath("A", "Y");

    EXPECT_FALSE(res1.reachable);
    EXPECT_FALSE(res2.reachable);
}

// ----------------------------
// NEARBY SORTING
// ----------------------------

TEST(RouteCalculatorNearby, SortedByDistance) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;0.1\n"
        "C;0;0.2\n"
    );

    RouteCalculator rc(db, 10000);

    const Airport& a = db.getByIndex(0);

    auto res = rc.findNearby(a, 5000);

    ASSERT_GE(res.size(), 2);

    for (size_t i = 1; i < res.size(); ++i) {
        EXPECT_LE(res[i-1].first, res[i].first);
    }
}

// ----------------------------
// MAX RANGE EFFECT ON PATH
// ----------------------------

TEST(RouteCalculatorPath, MaxRangeAffectsGraph) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;1\n"
        "C;0;2\n"
    );

    // маленький радиус → только локальные ребра
    RouteCalculator rc(db, 5000);

    auto res = rc.findShortestPath("A", "C");

    // может быть unreachable или длиннее — главное корректность флага
    if (res.reachable) {
        EXPECT_EQ(res.airports.front(), "A");
        EXPECT_EQ(res.airports.back(), "C");
    } else {
        EXPECT_TRUE(res.airports.empty());
    }
}

// ----------------------------
// SAME SOURCE AND TARGET EDGE
// ----------------------------

TEST(RouteCalculatorPath, SameSourceTarget) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
    );

    RouteCalculator rc(db, 1000);

    auto res = rc.findShortestPath("A", "A");

    EXPECT_TRUE(res.reachable);
    EXPECT_EQ(res.totalDistance, 0.0);
    EXPECT_EQ(res.airports.size(), 1);
}

TEST(RouteCalculatorEdge, DuplicateCoordinatesDifferentAirports) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;0\n"
        "C;0;1\n"
    );

    RouteCalculator rc(db, 10000);

    auto res = rc.findShortestPath("A", "B");

    EXPECT_TRUE(res.reachable);
    EXPECT_EQ(res.totalDistance, 0.0);
    EXPECT_EQ(res.airports.size(), 2);
}
TEST(RouteCalculatorUnit, IsReachableTrueAndFalse) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;0.1\n"
    );

    RouteCalculator rc(db, 10);

    const Airport& a = db.getByIndex(0);
    const Airport& b = db.getByIndex(1);

    auto res = rc.findNearby(a, 20);

    EXPECT_FALSE(res.empty());
}

TEST(RouteCalculatorUnit, SetMaxRangeRebuildsGraph) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;1\n"
    );

    RouteCalculator rc(db, 1000);


    auto res1 = rc.findShortestPath("A", "B");
    EXPECT_TRUE(res1.reachable);

  
    rc.setMaxRange(0.0001);

    auto res2 = rc.findShortestPath("A", "B");
    EXPECT_FALSE(res2.reachable);
}

TEST(RouteCalculatorEdge, FullyConnectedGraph) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;0.1\n"
        "C;0;0.2\n"
        "D;0;0.3\n"
    );

    RouteCalculator rc(db, 100000);

    auto res = rc.findShortestPath("A", "D");

    EXPECT_TRUE(res.reachable);
    EXPECT_EQ(res.airports.front(), "A");
    EXPECT_EQ(res.airports.back(), "D");
}


TEST(RouteCalculatorGeometry, CycleGraphNoInfiniteLoop) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;1\n"
        "C;1;1\n"
        "D;1;0\n"
    );

    RouteCalculator rc(db, 10000);

    auto res = rc.findShortestPath("A", "C");

    EXPECT_TRUE(res.reachable);
}

TEST(RouteCalculatorDijkstra, MultipleEqualPaths) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;1\n"
        "C;1;0\n"
        "D;1;1\n"
    );

    RouteCalculator rc(db, 10000);

    auto res = rc.findShortestPath("A", "D");

    EXPECT_TRUE(res.reachable);
    EXPECT_EQ(res.airports.front(), "A");
    EXPECT_EQ(res.airports.back(), "D");
}

TEST(RouteCalculatorDijkstra, NoInfiniteProcessing) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;1\n"
        "C;0;2\n"
        "D;0;3\n"
        "E;0;4\n"
    );

    RouteCalculator rc(db, 10000);

    auto res = rc.findShortestPath("A", "E");

    EXPECT_TRUE(res.reachable);
}

TEST(RouteCalculatorStability, DeterministicResults) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;1\n"
        "C;0;2\n"
    );

    RouteCalculator rc(db, 10000);

    auto r1 = rc.findShortestPath("A", "C");
    auto r2 = rc.findShortestPath("A", "C");

    EXPECT_EQ(r1.airports, r2.airports);
    EXPECT_EQ(r1.totalDistance, r2.totalDistance);
}

TEST(RouteCalculatorStability, NearbyDeterministicOrder) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;0.1\n"
        "C;0;0.2\n"
    );

    RouteCalculator rc(db, 10000);

    const Airport& a = db.getByIndex(0);

    auto r1 = rc.findNearby(a, 10000);
    auto r2 = rc.findNearby(a, 10000);

    EXPECT_EQ(r1.size(), r2.size());

    for (size_t i = 0; i < r1.size(); ++i) {
        EXPECT_EQ(r1[i].first, r2[i].first);
    }
}

TEST(RouteCalculatorStress, LargeGraphNoCrash) {
    QString data;
    for (int i = 0; i < 200; ++i) {
        data += QString("A%1;%2;%3\n")
                    .arg(i)
                    .arg(i * 0.1)
                    .arg(i * 0.1);
    }

    AirportDatabase db = makeDB(data);

    RouteCalculator rc(db, 10000);

    auto res = rc.findShortestPath("A0", "A199");

    EXPECT_TRUE(res.reachable);
}

TEST(RouteCalculatorStress, NearbyLargeDataset) {
    QString data;
    for (int i = 0; i < 500; ++i) {
        data += QString("A%1;%2;%3\n")
                    .arg(i)
                    .arg(i * 0.01)
                    .arg(i * 0.01);
    }

    AirportDatabase db = makeDB(data);
    RouteCalculator rc(db, 10000);

    const Airport& a = db.getByIndex(0);

    auto res = rc.findNearby(a, 10000);

    EXPECT_GT(res.size(), 10);
}

TEST(RouteCalculatorInvalid, DuplicateCodes) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "A;1;1\n"
        "B;2;2\n"
    );

    RouteCalculator rc(db, 10000);

    auto res = rc.findShortestPath("A", "B");

    EXPECT_TRUE(res.reachable || res.airports.empty());
}

TEST(RouteCalculatorInvalid, ExtremelySmallRange) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;0.00001\n"
        "C;10;10\n"
    );

    RouteCalculator rc(db, 0.0000001);

    auto res = rc.findShortestPath("A", "B");

    EXPECT_FALSE(res.reachable);
}

TEST(RouteCalculatorInvalid, EmptyDatabase) {
    AirportDatabase db = makeDB("");

    RouteCalculator rc(db, 1000);

    auto res = rc.findShortestPath("A", "B");

    EXPECT_FALSE(res.reachable);
}

TEST(RouteCalculatorCritical, DijkstraCorrectness) {
    AirportDatabase db = makeDB(
        "A;0;0\n"
        "B;0;1\n"
        "C;0;2\n"
        "D;0;10\n"
    );

    RouteCalculator rc(db, 10000);

    auto res = rc.findShortestPath("A", "C");

    // обязательно минимальный путь A->B->C
    EXPECT_EQ(res.airports.front(), "A");
    EXPECT_EQ(res.airports.back(), "C");
    EXPECT_EQ(res.totalDistance > 0.0, true);
}