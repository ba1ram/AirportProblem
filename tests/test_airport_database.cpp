#include <gtest/gtest.h>
#include "core/AirportDatabase.h"

#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QFile>
#include <QTextStream>

// ----------------------------
// helper: create temp CSV file
// ----------------------------
static QString createTempFile(const QString& content) {
    QTemporaryFile file;
    file.setAutoRemove(false);

    EXPECT_TRUE(file.open());

    QTextStream out(&file);
    out << content;
    file.close();

    return file.fileName();
}

// ----------------------------
// BASIC LOAD TESTS
// ----------------------------

TEST(AirportDatabaseLoad, EmptyFile) {
    QString path = createTempFile("");

    AirportDatabase db;
    EXPECT_FALSE(db.loadFromFile(path));

    QFile::remove(path);
}

TEST(AirportDatabaseLoad, SingleValidLineNoHeader) {
    QString content =
        "JFK;40.6413;-73.7781\n";

    QString path = createTempFile(content);

    AirportDatabase db;
    EXPECT_TRUE(db.loadFromFile(path, false));

    EXPECT_EQ(db.size(), 1);
    EXPECT_FALSE(db.isEmpty());

    QFile::remove(path);
}

TEST(AirportDatabaseLoad, HeaderSkipped) {
    QString content =
        "code;lat;lon\n"
        "JFK;40.0;-70.0\n";

    QString path = createTempFile(content);

    AirportDatabase db;
    EXPECT_TRUE(db.loadFromFile(path, true));

    EXPECT_EQ(db.size(), 1);

    QFile::remove(path);
}

// ----------------------------
// PARSING EDGE CASES
// ----------------------------

TEST(AirportDatabaseLoad, EmptyLinesIgnored) {
    QString content =
        "\n"
        "\n"
        "JFK;40;-70\n"
        "\n";

    QString path = createTempFile(content);

    AirportDatabase db;
    EXPECT_TRUE(db.loadFromFile(path, false));

    EXPECT_EQ(db.size(), 1);

    QFile::remove(path);
}

TEST(AirportDatabaseLoad, InvalidColumnCountSkipped) {
    QString content =
        "JFK;40\n"        // only 2 columns
        "LAX;34;-118\n";  // valid

    QString path = createTempFile(content);

    AirportDatabase db;
    EXPECT_TRUE(db.loadFromFile(path, false));

    EXPECT_EQ(db.size(), 1);

    QFile::remove(path);
}

TEST(AirportDatabaseLoad, InvalidCoordinatesSkipped) {
    QString content =
        "JFK;abc;-70\n"
        "LAX;34;xyz\n"
        "SFO;37;-122\n";

    QString path = createTempFile(content);

    AirportDatabase db;
    EXPECT_TRUE(db.loadFromFile(path, false));

    EXPECT_EQ(db.size(), 1);

    QFile::remove(path);
}

TEST(AirportDatabaseLoad, OutOfRangeCoordinatesSkipped) {
    QString content =
        "BAD1;100;0\n"     // invalid lat
        "BAD2;0;200\n"     // invalid lon
        "OK1;45;90\n";

    QString path = createTempFile(content);

    AirportDatabase db;
    EXPECT_TRUE(db.loadFromFile(path, false));

    EXPECT_EQ(db.size(), 1);

    QFile::remove(path);
}

// ----------------------------
// DUPLICATES & NORMALIZATION
// ----------------------------

TEST(AirportDatabaseLoad, DuplicateCodesSkipped) {
    QString content =
        "jfk;40;-70\n"
        "JFK;41;-71\n"  // duplicate after upper()
        "LAX;34;-118\n";

    QString path = createTempFile(content);

    AirportDatabase db;
    EXPECT_TRUE(db.loadFromFile(path, false));

    EXPECT_EQ(db.size(), 2);

    QFile::remove(path);
}

TEST(AirportDatabaseLoad, CodeIsUppercased) {
    QString content =
        "jfk;40;-70\n";

    QString path = createTempFile(content);

    AirportDatabase db;
    EXPECT_TRUE(db.loadFromFile(path, false));

    const Airport* a = db.findAirport("JFK");
    const Airport* b = db.findAirport("jfk");

    EXPECT_NE(a, nullptr);
    EXPECT_NE(b, nullptr);
    EXPECT_EQ(a, b);

    QFile::remove(path);
}

// ----------------------------
// FIND / INDEX TESTS
// ----------------------------

TEST(AirportDatabaseLookup, FindAirportExists) {
    QString content =
        "JFK;40;-70\n";

    QString path = createTempFile(content);

    AirportDatabase db;
    db.loadFromFile(path, false);

    const Airport* a = db.findAirport("JFK");

    ASSERT_NE(a, nullptr);
    EXPECT_TRUE(a->isValid());

    QFile::remove(path);
}

TEST(AirportDatabaseLookup, FindAirportNotFound) {
    QString content =
        "JFK;40;-70\n";

    QString path = createTempFile(content);

    AirportDatabase db;
    db.loadFromFile(path, false);

    EXPECT_EQ(db.findAirport("LAX"), nullptr);

    QFile::remove(path);
}

TEST(AirportDatabaseLookup, GetIndexValid) {
    QString content =
        "JFK;40;-70\n"
        "LAX;34;-118\n";

    QString path = createTempFile(content);

    AirportDatabase db;
    db.loadFromFile(path, false);

    EXPECT_EQ(db.getIndex("JFK"), 0);
    EXPECT_EQ(db.getIndex("LAX"), 1);

    QFile::remove(path);
}

TEST(AirportDatabaseLookup, GetIndexInvalid) {
    QString content =
        "JFK;40;-70\n";

    QString path = createTempFile(content);

    AirportDatabase db;
    db.loadFromFile(path, false);

    EXPECT_EQ(db.getIndex("XXX"), -1);

    QFile::remove(path);
}

// ----------------------------
// GET BY INDEX
// ----------------------------

TEST(AirportDatabaseAccess, GetByIndexValid) {
    QString content =
        "JFK;40;-70\n";

    QString path = createTempFile(content);

    AirportDatabase db;
    db.loadFromFile(path, false);

    const Airport& a = db.getByIndex(0);
    EXPECT_TRUE(a.isValid());

    QFile::remove(path);
}

TEST(AirportDatabaseAccess, GetByIndexOutOfBounds) {
    QString content =
        "JFK;40;-70\n";

    QString path = createTempFile(content);

    AirportDatabase db;
    db.loadFromFile(path, false);

    EXPECT_NO_THROW({
        db.getByIndex(999);
    });

    QFile::remove(path);
}

TEST(AirportDatabaseLoad, FileDoesNotExist) {
    AirportDatabase db;

    QString fakePath = "/this/file/does/not/exist.csv";

    EXPECT_FALSE(db.loadFromFile(fakePath, false));
}

TEST(AirportDatabaseLoad, CannotOpenFile) {
    AirportDatabase db;

    QTemporaryDir dir;
    QString path = dir.path() + "/file.csv";

    QFile file(path);
    EXPECT_FALSE(db.loadFromFile(path, false));
}

TEST(AirportDatabaseLoad, CannotOpenFile_BrokenPath) {
    AirportDatabase db;

    QString path = "/root/forbidden_file.csv";

    EXPECT_FALSE(db.loadFromFile(path, false));
}

TEST(AirportDatabaseLoad, EmptyCodeSkipped) {
    QString content =
        ";40;-70\n"      // empty code
        "   ;40;-70\n"   // whitespace code
        "JFK;40;-70\n";

    QString path = createTempFile(content);

    AirportDatabase db;
    EXPECT_TRUE(db.loadFromFile(path, false));

    EXPECT_EQ(db.size(), 1);

    QFile::remove(path);
}

TEST(AirportDatabaseLoad, AllLinesInvalidReturnsFalse) {
    QString content =
        ";abc;def\n"
        ";;\n"
        "BAD;999;999\n";

    QString path = createTempFile(content);

    AirportDatabase db;
    EXPECT_FALSE(db.loadFromFile(path, false));

    QFile::remove(path);
}