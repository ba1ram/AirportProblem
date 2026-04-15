#include <gtest/gtest.h>
#include "core/Airport.h"

// --------------------
// isValid tests
// --------------------

TEST(AirportValidity, ValidAirport) {
    Airport a("JFK", 40.6413, -73.7781);
    EXPECT_TRUE(a.isValid());
}

TEST(AirportValidity, EmptyCodeInvalid) {
    Airport a("", 40.0, 50.0);
    EXPECT_FALSE(a.isValid());
}

TEST(AirportValidity, LatitudeLowerBound) {
    Airport a("AAA", -90.0, 0.0);
    EXPECT_TRUE(a.isValid());
}

TEST(AirportValidity, LatitudeBelowLowerBound) {
    Airport a("AAA", -90.0001, 0.0);
    EXPECT_FALSE(a.isValid());
}

TEST(AirportValidity, LatitudeUpperBound) {
    Airport a("AAA", 90.0, 0.0);
    EXPECT_TRUE(a.isValid());
}

TEST(AirportValidity, LatitudeAboveUpperBound) {
    Airport a("AAA", 90.0001, 0.0);
    EXPECT_FALSE(a.isValid());
}

TEST(AirportValidity, LongitudeLowerBound) {
    Airport a("AAA", 0.0, -180.0);
    EXPECT_TRUE(a.isValid());
}

TEST(AirportValidity, LongitudeBelowLowerBound) {
    Airport a("AAA", 0.0, -180.0001);
    EXPECT_FALSE(a.isValid());
}

TEST(AirportValidity, LongitudeUpperBound) {
    Airport a("AAA", 0.0, 180.0);
    EXPECT_TRUE(a.isValid());
}

TEST(AirportValidity, LongitudeAboveUpperBound) {
    Airport a("AAA", 0.0, 180.0001);
    EXPECT_FALSE(a.isValid());
}

// --------------------
// operator tests
// --------------------

TEST(AirportOperators, EqualitySameCode) {
    Airport a1("ABC", 0.0, 0.0);
    Airport a2("ABC", 10.0, 20.0);
    EXPECT_TRUE(a1 == a2);
}

TEST(AirportOperators, InequalityDifferentCode) {
    Airport a1("ABC", 0.0, 0.0);
    Airport a2("DEF", 0.0, 0.0);
    EXPECT_TRUE(a1 != a2);
}

TEST(AirportOperators, LessOperatorLexicographic) {
    Airport a1("AAA", 0.0, 0.0);
    Airport a2("AAB", 0.0, 0.0);
    EXPECT_TRUE(a1 < a2);
}

// --------------------
// distanceTo tests
// --------------------

TEST(AirportDistance, SameAirportZeroDistance) {
    Airport a("AAA", 10.0, 20.0);
    EXPECT_NEAR(a.distanceTo(a), 0.0, 1e-6);
}

TEST(AirportDistance, Symmetry) {
    Airport a1("A1", 55.7558, 37.6173); // Moscow
    Airport a2("A2", 48.8566, 2.3522);  // Paris

    double d1 = a1.distanceTo(a2);
    double d2 = a2.distanceTo(a1);

    EXPECT_NEAR(d1, d2, 1e-6);
}

TEST(AirportDistance, KnownCitiesReasonableRange) {
    Airport nyc("NYC", 40.7128, -74.0060);
    Airport la("LAX", 34.0522, -118.2437);

    double dist = nyc.distanceTo(la);

    // примерно 3900 km
    EXPECT_GT(dist, 3500);
    EXPECT_LT(dist, 4500);
}

TEST(AirportDistance, AntipodesMaxDistanceApprox) {
    Airport a("A", 0.0, 0.0);
    Airport b("B", 0.0, 180.0);

    double dist = a.distanceTo(b);

    // половина окружности Земли ~ 20015 km
    EXPECT_NEAR(dist, 20015, 1000);
}

// --------------------
// Edge coordinate stress
// --------------------

TEST(AirportDistance, PoleToPole) {
    Airport north("N", 90.0, 0.0);
    Airport south("S", -90.0, 0.0);

    double dist = north.distanceTo(south);

    EXPECT_GT(dist, 20000);
}