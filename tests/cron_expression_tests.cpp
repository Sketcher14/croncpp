#include <gtest/gtest.h>
#include "croncpp.h"

#include <cstdlib>
#include <ctime>

// UTC time_t from calendar fields.
static std::time_t make_tm(int year, int month, int day, int hour = 0, int min = 0, int sec = 0) {
    std::tm t{};
    t.tm_year = year - 1900;
    t.tm_mon  = month - 1;
    t.tm_mday = day;
    t.tm_hour = hour;
    t.tm_min  = min;
    t.tm_sec  = sec;
    t.tm_isdst = 0;
    return timegm(&t);
}

static void expect_time(std::time_t t, int year, int month, int day, int hour, int min, int sec,
                        const char* ctx = "") {
    std::tm result{};
    gmtime_r(&t, &result);
    EXPECT_EQ(result.tm_year + 1900, year) << ctx;
    EXPECT_EQ(result.tm_mon + 1, month) << ctx;
    EXPECT_EQ(result.tm_mday, day) << ctx;
    EXPECT_EQ(result.tm_hour, hour) << ctx;
    EXPECT_EQ(result.tm_min, min) << ctx;
    EXPECT_EQ(result.tm_sec, sec) << ctx;
}

static void expect_wday(std::time_t t, int wday, const char* ctx = "") {
    std::tm result{};
    gmtime_r(&t, &result);
    EXPECT_EQ(result.tm_wday, wday) << ctx;
}


class CronExpressionTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite() {
        setenv("TZ", "UTC", 1);
        tzset();
    }
};

// ===== cron_next: seconds =====

TEST_F(CronExpressionTest, test_second) {
    auto cron = cron::make_cron("*/1 * * * * *");
    auto base = make_tm(2012, 4, 6, 13, 26, 10);
    auto n1 = cron::cron_next(cron, base);
    expect_time(n1, 2012, 4, 6, 13, 26, 11);
}

TEST_F(CronExpressionTest, test_second_sec) {
    auto cron = cron::make_cron("15,25 * * * * *");
    auto base = make_tm(2012, 4, 6, 13, 26, 10);
    auto n1 = cron::cron_next(cron, base);
    expect_time(n1, 2012, 4, 6, 13, 26, 15);
    auto n2 = cron::cron_next(cron, n1);
    expect_time(n2, 2012, 4, 6, 13, 26, 25);
    auto n3 = cron::cron_next(cron, n2);
    expect_time(n3, 2012, 4, 6, 13, 27, 15);
}

TEST_F(CronExpressionTest, test_second_repeat) {
    auto cron = cron::make_cron("*/15 * * * * *");
    auto base = make_tm(2012, 4, 6, 13, 26, 36);
    auto n1 = cron::cron_next(cron, base);
    expect_time(n1, 2012, 4, 6, 13, 26, 45);
    auto n2 = cron::cron_next(cron, n1);
    expect_time(n2, 2012, 4, 6, 13, 27, 0);
    auto n3 = cron::cron_next(cron, n2);
    expect_time(n3, 2012, 4, 6, 13, 27, 15);
}

// ===== cron_next: minutes =====

TEST_F(CronExpressionTest, test_minute) {
    // */1 minute
    {
        auto cron = cron::make_cron("0 */1 * * * *");
        auto base = make_tm(2010, 1, 23, 12, 18);
        auto n = cron::cron_next(cron, base);
        expect_time(n, 2010, 1, 23, 12, 19, 0);
        // Advance to minute 59
        for (int i = 0; i < 40; ++i) n = cron::cron_next(cron, n);
        expect_time(n, 2010, 1, 23, 12, 59, 0);
        // Next should roll to hour 13
        n = cron::cron_next(cron, n);
        expect_time(n, 2010, 1, 23, 13, 0, 0);
    }
    // */5 minute
    {
        auto cron = cron::make_cron("0 */5 * * * *");
        auto base = make_tm(2010, 1, 23, 12, 18);
        auto n = cron::cron_next(cron, base);
        expect_time(n, 2010, 1, 23, 12, 20, 0);
        for (int i = 0; i < 7; ++i) n = cron::cron_next(cron, n);
        expect_time(n, 2010, 1, 23, 12, 55, 0);
        n = cron::cron_next(cron, n);
        expect_time(n, 2010, 1, 23, 13, 0, 0);
    }
}

TEST_F(CronExpressionTest, test_range_generator) {
    auto cron = cron::make_cron("0 1-9/2 0 1 * *");
    auto base = make_tm(2013, 3, 4, 0, 0);
    auto n1 = cron::cron_next(cron, base);
    expect_time(n1, 2013, 4, 1, 0, 1, 0);
    auto n2 = cron::cron_next(cron, n1);
    expect_time(n2, 2013, 4, 1, 0, 3, 0);
    auto n3 = cron::cron_next(cron, n2);
    expect_time(n3, 2013, 4, 1, 0, 5, 0);
    auto n4 = cron::cron_next(cron, n3);
    expect_time(n4, 2013, 4, 1, 0, 7, 0);
    auto n5 = cron::cron_next(cron, n4);
    expect_time(n5, 2013, 4, 1, 0, 9, 0);
}

// ===== cron_next: hours =====

TEST_F(CronExpressionTest, test_hour) {
    auto cron = cron::make_cron("0 0 */3 * * *");
    auto base = make_tm(2010, 1, 24, 12, 2);
    auto n = cron::cron_next(cron, base);
    expect_time(n, 2010, 1, 24, 15, 0, 0);
    n = cron::cron_next(cron, n);
    expect_time(n, 2010, 1, 24, 18, 0, 0);
    n = cron::cron_next(cron, n);
    expect_time(n, 2010, 1, 24, 21, 0, 0);
    n = cron::cron_next(cron, n);
    expect_time(n, 2010, 1, 25, 0, 0, 0);
}

TEST_F(CronExpressionTest, test_bug57) {
    {
        auto cron = cron::make_cron("0 0 4/6 * * *");
        auto base = make_tm(2012, 2, 24, 0, 0, 0);
        auto n = cron::cron_next(cron, base);
        expect_time(n, 2012, 2, 24, 4, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0/6 * * *");
        auto base = make_tm(2012, 2, 24, 0, 0, 0);
        auto n = cron::cron_next(cron, base);
        expect_time(n, 2012, 2, 24, 6, 0, 0);
    }
}

// ===== cron_next: days =====

TEST_F(CronExpressionTest, test_day) {
    {
        auto cron = cron::make_cron("0 0 0 */3 * *");
        auto base = make_tm(2010, 2, 24, 12, 9);
        auto n = cron::cron_next(cron, base);
        expect_time(n, 2010, 2, 25, 0, 0, 0);
        n = cron::cron_next(cron, n);
        expect_time(n, 2010, 2, 28, 0, 0, 0);
        n = cron::cron_next(cron, n);
        expect_time(n, 2010, 3, 1, 0, 0, 0);
    }
    // Leap year 1996
    {
        auto cron = cron::make_cron("0 0 0 * * *");
        auto base = make_tm(1996, 2, 27);
        auto n = cron::cron_next(cron, base);
        expect_time(n, 1996, 2, 28, 0, 0, 0);
        n = cron::cron_next(cron, n);
        expect_time(n, 1996, 2, 29, 0, 0, 0);
    }
    // Leap year 2000
    {
        auto cron = cron::make_cron("0 0 0 * * *");
        auto base = make_tm(2000, 2, 27);
        auto n = cron::cron_next(cron, base);
        expect_time(n, 2000, 2, 28, 0, 0, 0);
        n = cron::cron_next(cron, n);
        expect_time(n, 2000, 2, 29, 0, 0, 0);
    }
}

TEST_F(CronExpressionTest, test_bug3) {
    auto cron = cron::make_cron("0 0 3 16,30 * *");
    auto base = make_tm(2013, 3, 1, 12, 17, 34);
    auto n1 = cron::cron_next(cron, base);
    expect_time(n1, 2013, 3, 16, 3, 0, 0);
    auto n2 = cron::cron_next(cron, n1);
    expect_time(n2, 2013, 3, 30, 3, 0, 0);
    auto n3 = cron::cron_next(cron, n2);
    expect_time(n3, 2013, 4, 16, 3, 0, 0);
}

TEST_F(CronExpressionTest, test_first_of_march) {
    auto cron = cron::make_cron("0 0 0 */10 * *");
    auto base = make_tm(2025, 2, 22);
    auto n = cron::cron_next(cron, base);
    expect_time(n, 2025, 3, 1, 0, 0, 0);
}

// ===== cron_next: weekdays =====

TEST_F(CronExpressionTest, test_weekday) {
    auto cron = cron::make_cron("0 0 0 * * 6");
    auto base = make_tm(2010, 2, 25);
    auto n1 = cron::cron_next(cron, base);
    expect_time(n1, 2010, 2, 27, 0, 0, 0);
    expect_wday(n1, 6); // Saturday
    auto n2 = cron::cron_next(cron, n1);
    expect_time(n2, 2010, 3, 6, 0, 0, 0);
    expect_wday(n2, 6);
}

TEST_F(CronExpressionTest, test_sunday_to_thursday) {
    auto cron = cron::make_cron("0 30 22 * * 0-4");
    auto base = make_tm(2010, 8, 25, 15, 56);
    auto n = cron::cron_next(cron, base);
    expect_time(n, 2010, 8, 25, 22, 30, 0);
}

TEST_F(CronExpressionTest, test_iso_weekday) {
    auto cron = cron::make_cron("0 0 0 * * 6");
    auto base = make_tm(2010, 2, 25);
    auto n1 = cron::cron_next(cron, base);
    expect_time(n1, 2010, 2, 27, 0, 0, 0);
    expect_wday(n1, 6); // Saturday
    auto n2 = cron::cron_next(cron, n1);
    expect_time(n2, 2010, 3, 6, 0, 0, 0);
    expect_wday(n2, 6);
}

TEST_F(CronExpressionTest, test_weekday_range) {
    auto cron = cron::make_cron("0 0 0 * * 2-4");
    auto base = make_tm(2019, 1, 14, 0, 0, 0); // Monday
    auto n = cron::cron_next(cron, base);
    expect_time(n, 2019, 1, 15, 0, 0, 0); // Tue
    expect_wday(n, 2);
    n = cron::cron_next(cron, n);
    expect_time(n, 2019, 1, 16, 0, 0, 0); // Wed
    expect_wday(n, 3);
    n = cron::cron_next(cron, n);
    expect_time(n, 2019, 1, 17, 0, 0, 0); // Thu
    expect_wday(n, 4);
    n = cron::cron_next(cron, n);
    expect_time(n, 2019, 1, 22, 0, 0, 0); // Next Tue
    expect_wday(n, 2);
}

// ===== cron_next: months =====

TEST_F(CronExpressionTest, test_month) {
    auto cron = cron::make_cron("0 0 0 1 * *");
    auto base = make_tm(2010, 1, 25);
    auto n = cron::cron_next(cron, base);
    expect_time(n, 2010, 2, 1, 0, 0, 0);
    // Advance through all months
    for (int m = 3; m <= 12; ++m) {
        n = cron::cron_next(cron, n);
        expect_time(n, 2010, m, 1, 0, 0, 0);
    }
    // Roll over to next year
    n = cron::cron_next(cron, n);
    expect_time(n, 2011, 1, 1, 0, 0, 0);
}

TEST_F(CronExpressionTest, test_multiple_months) {
    auto cron = cron::make_cron("0 0 0 1 3,6,9,12 *");
    {
        auto base = make_tm(2016, 3, 1, 0, 0, 0);
        auto n = cron::cron_next(cron, base);
        expect_time(n, 2016, 6, 1, 0, 0, 0);
    }
    {
        auto base = make_tm(2016, 2, 15, 0, 0, 0);
        auto n = cron::cron_next(cron, base);
        expect_time(n, 2016, 3, 1, 0, 0, 0);
    }
    {
        auto base = make_tm(2016, 12, 3, 10, 0, 0);
        auto n = cron::cron_next(cron, base);
        expect_time(n, 2017, 3, 1, 0, 0, 0);
    }
}

TEST_F(CronExpressionTest, test_bug2) {
    auto cron = cron::make_cron("0 0 * * 3 *");
    auto base = make_tm(2012, 1, 1, 0, 0);
    auto n = cron::cron_next(cron, base);
    expect_time(n, 2012, 3, 1, 0, 0, 0);
    n = cron::cron_next(cron, n);
    expect_time(n, 2012, 3, 1, 1, 0, 0);
    n = cron::cron_next(cron, n);
    expect_time(n, 2012, 3, 1, 2, 0, 0);
}

// ===== cron_next: bulk regression set (ex-StandardTest.Next) =====

TEST_F(CronExpressionTest, test_next) {
    {
        auto cron = cron::make_cron("*/15 * 1-4 * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 7, 1, 9, 53, 50));
        expect_time(t, 2012, 7, 2, 1, 0, 0);
    }
    {
        auto cron = cron::make_cron("*/15 * 1-4 * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 7, 1, 9, 53, 0));
        expect_time(t, 2012, 7, 2, 1, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 */2 1-4 * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 7, 1, 9, 0, 0));
        expect_time(t, 2012, 7, 2, 1, 0, 0);
    }
    {
        auto cron = cron::make_cron("* * * * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 7, 1, 9, 0, 0));
        expect_time(t, 2012, 7, 1, 9, 0, 1);
    }
    {
        auto cron = cron::make_cron("* * * * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 12, 1, 9, 0, 58));
        expect_time(t, 2012, 12, 1, 9, 0, 59);
    }
    {
        auto cron = cron::make_cron("10 * * * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 12, 1, 9, 42, 9));
        expect_time(t, 2012, 12, 1, 9, 42, 10);
    }
    {
        auto cron = cron::make_cron("11 * * * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 12, 1, 9, 42, 10));
        expect_time(t, 2012, 12, 1, 9, 42, 11);
    }
    {
        auto cron = cron::make_cron("10 * * * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 12, 1, 9, 42, 10));
        expect_time(t, 2012, 12, 1, 9, 43, 10);
    }
    {
        auto cron = cron::make_cron("10-15 * * * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 12, 1, 9, 42, 9));
        expect_time(t, 2012, 12, 1, 9, 42, 10);
    }
    {
        auto cron = cron::make_cron("10-15 * * * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 12, 1, 21, 42, 14));
        expect_time(t, 2012, 12, 1, 21, 42, 15);
    }
    {
        auto cron = cron::make_cron("0 * * * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 12, 1, 21, 10, 42));
        expect_time(t, 2012, 12, 1, 21, 11, 0);
    }
    {
        auto cron = cron::make_cron("0 * * * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 12, 1, 21, 11, 0));
        expect_time(t, 2012, 12, 1, 21, 12, 0);
    }
    {
        auto cron = cron::make_cron("0 11 * * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 12, 1, 21, 10, 42));
        expect_time(t, 2012, 12, 1, 21, 11, 0);
    }
    {
        auto cron = cron::make_cron("0 10 * * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 12, 1, 21, 11, 0));
        expect_time(t, 2012, 12, 1, 22, 10, 0);
    }
    {
        auto cron = cron::make_cron("0 0 * * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 9, 30, 11, 1, 0));
        expect_time(t, 2012, 9, 30, 12, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 * * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 9, 30, 12, 0, 0));
        expect_time(t, 2012, 9, 30, 13, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 * * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 9, 10, 23, 1, 0));
        expect_time(t, 2012, 9, 11, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 * * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 9, 11, 0, 0, 0));
        expect_time(t, 2012, 9, 11, 1, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 9, 1, 14, 42, 43));
        expect_time(t, 2012, 9, 2, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 9, 2, 0, 0, 0));
        expect_time(t, 2012, 9, 3, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("* * * 10 * *");
        auto t = cron::cron_next(cron, make_tm(2012, 10, 9, 15, 12, 42));
        expect_time(t, 2012, 10, 10, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("* * * 10 * *");
        auto t = cron::cron_next(cron, make_tm(2012, 10, 11, 15, 12, 42));
        expect_time(t, 2012, 11, 10, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 9, 30, 15, 12, 42));
        expect_time(t, 2012, 10, 1, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 10, 1, 0, 0, 0));
        expect_time(t, 2012, 10, 2, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 8, 30, 15, 12, 42));
        expect_time(t, 2012, 8, 31, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 8, 31, 0, 0, 0));
        expect_time(t, 2012, 9, 1, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 10, 30, 15, 12, 42));
        expect_time(t, 2012, 10, 31, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 * * *");
        auto t = cron::cron_next(cron, make_tm(2012, 10, 31, 0, 0, 0));
        expect_time(t, 2012, 11, 1, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 1 * *");
        auto t = cron::cron_next(cron, make_tm(2012, 10, 30, 15, 12, 42));
        expect_time(t, 2012, 11, 1, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 1 * *");
        auto t = cron::cron_next(cron, make_tm(2012, 11, 1, 0, 0, 0));
        expect_time(t, 2012, 12, 1, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 1 * *");
        auto t = cron::cron_next(cron, make_tm(2010, 12, 31, 15, 12, 42));
        expect_time(t, 2011, 1, 1, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 1 * *");
        auto t = cron::cron_next(cron, make_tm(2011, 1, 1, 0, 0, 0));
        expect_time(t, 2011, 2, 1, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 31 * *");
        auto t = cron::cron_next(cron, make_tm(2011, 10, 30, 15, 12, 42));
        expect_time(t, 2011, 10, 31, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 1 * *");
        auto t = cron::cron_next(cron, make_tm(2011, 10, 30, 15, 12, 42));
        expect_time(t, 2011, 11, 1, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("* * * * * 2");
        auto t = cron::cron_next(cron, make_tm(2010, 10, 25, 15, 12, 42));
        expect_time(t, 2010, 10, 26, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("* * * * * 2");
        auto t = cron::cron_next(cron, make_tm(2010, 10, 20, 15, 12, 42));
        expect_time(t, 2010, 10, 26, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("* * * * * 2");
        auto t = cron::cron_next(cron, make_tm(2010, 10, 27, 15, 12, 42));
        expect_time(t, 2010, 11, 2, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("55 5 * * * *");
        auto t = cron::cron_next(cron, make_tm(2010, 10, 27, 15, 4, 54));
        expect_time(t, 2010, 10, 27, 15, 5, 55);
    }
    {
        auto cron = cron::make_cron("55 5 * * * *");
        auto t = cron::cron_next(cron, make_tm(2010, 10, 27, 15, 5, 55));
        expect_time(t, 2010, 10, 27, 16, 5, 55);
    }
    {
        auto cron = cron::make_cron("55 * 10 * * *");
        auto t = cron::cron_next(cron, make_tm(2010, 10, 27, 9, 4, 54));
        expect_time(t, 2010, 10, 27, 10, 0, 55);
    }
    {
        auto cron = cron::make_cron("55 * 10 * * *");
        auto t = cron::cron_next(cron, make_tm(2010, 10, 27, 10, 0, 55));
        expect_time(t, 2010, 10, 27, 10, 1, 55);
    }
    {
        auto cron = cron::make_cron("* 5 10 * * *");
        auto t = cron::cron_next(cron, make_tm(2010, 10, 27, 9, 4, 55));
        expect_time(t, 2010, 10, 27, 10, 5, 0);
    }
    {
        auto cron = cron::make_cron("* 5 10 * * *");
        auto t = cron::cron_next(cron, make_tm(2010, 10, 27, 10, 5, 0));
        expect_time(t, 2010, 10, 27, 10, 5, 1);
    }
    {
        auto cron = cron::make_cron("55 * * 3 * *");
        auto t = cron::cron_next(cron, make_tm(2010, 10, 2, 10, 5, 54));
        expect_time(t, 2010, 10, 3, 0, 0, 55);
    }
    {
        auto cron = cron::make_cron("55 * * 3 * *");
        auto t = cron::cron_next(cron, make_tm(2010, 10, 3, 0, 0, 55));
        expect_time(t, 2010, 10, 3, 0, 1, 55);
    }
    {
        auto cron = cron::make_cron("* * * 3 11 *");
        auto t = cron::cron_next(cron, make_tm(2010, 10, 2, 14, 42, 55));
        expect_time(t, 2010, 11, 3, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("* * * 3 11 *");
        auto t = cron::cron_next(cron, make_tm(2010, 11, 3, 0, 0, 0));
        expect_time(t, 2010, 11, 3, 0, 0, 1);
    }
    {
        auto cron = cron::make_cron("0 0 0 29 2 *");
        auto t = cron::cron_next(cron, make_tm(2007, 2, 10, 14, 42, 55));
        expect_time(t, 2008, 2, 29, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 29 2 *");
        auto t = cron::cron_next(cron, make_tm(2008, 2, 29, 0, 0, 0));
        expect_time(t, 2012, 2, 29, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 7 ? * MON-FRI");
        auto t = cron::cron_next(cron, make_tm(2009, 9, 26, 0, 42, 55));
        expect_time(t, 2009, 9, 28, 7, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 7 ? * MON-FRI");
        auto t = cron::cron_next(cron, make_tm(2009, 9, 28, 7, 0, 0));
        expect_time(t, 2009, 9, 29, 7, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 30 23 30 1/3 ?");
        auto t = cron::cron_next(cron, make_tm(2010, 12, 30, 0, 0, 0));
        expect_time(t, 2011, 1, 30, 23, 30, 0);
    }
    {
        auto cron = cron::make_cron("0 30 23 30 1/3 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 1, 30, 23, 30, 0));
        expect_time(t, 2011, 4, 30, 23, 30, 0);
    }
    {
        auto cron = cron::make_cron("0 30 23 30 1/3 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 4, 30, 23, 30, 0));
        expect_time(t, 2011, 7, 30, 23, 30, 0);
    }

    // Last day of month in and out of DST (DST is irrelevant under TZ=UTC)
    {
        auto cron = cron::make_cron("0 25 23 31 12 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 9, 22, 14, 20, 0));
        expect_time(t, 2011, 12, 31, 23, 25, 0);
    }
    {
        auto cron = cron::make_cron("0 25 23 31 12 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 12, 31, 23, 30, 0));
        expect_time(t, 2012, 12, 31, 23, 25, 0);
    }
    {
        auto cron = cron::make_cron("0 25 23 30 11 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 9, 22, 14, 20, 0));
        expect_time(t, 2011, 11, 30, 23, 25, 0);
    }
    {
        auto cron = cron::make_cron("0 25 23 30 11 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 10, 22, 14, 20, 0));
        expect_time(t, 2011, 11, 30, 23, 25, 0);
    }
    {
        auto cron = cron::make_cron("0 25 23 30 11 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 11, 22, 14, 20, 0));
        expect_time(t, 2011, 11, 30, 23, 25, 0);
    }
    {
        auto cron = cron::make_cron("0 25 23 30 11 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 12, 22, 14, 20, 0));
        expect_time(t, 2012, 11, 30, 23, 25, 0);
    }
    {
        auto cron = cron::make_cron("0 25 23 31 10 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 9, 22, 14, 20, 0));
        expect_time(t, 2011, 10, 31, 23, 25, 0);
    }
    {
        auto cron = cron::make_cron("0 25 23 31 10 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 10, 22, 14, 20, 0));
        expect_time(t, 2011, 10, 31, 23, 25, 0);
    }
    {
        auto cron = cron::make_cron("0 25 23 31 10 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 10, 31, 23, 30, 0));
        expect_time(t, 2012, 10, 31, 23, 25, 0);
    }
    {
        auto cron = cron::make_cron("0 25 23 31 10 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 11, 22, 14, 20, 0));
        expect_time(t, 2012, 10, 31, 23, 25, 0);
    }
    {
        auto cron = cron::make_cron("0 25 23 31 8 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 9, 22, 14, 20, 0));
        expect_time(t, 2012, 8, 31, 23, 25, 0);
    }
    {
        auto cron = cron::make_cron("0 25 23 31 7 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 9, 22, 14, 20, 0));
        expect_time(t, 2012, 7, 31, 23, 25, 0);
    }
    {
        auto cron = cron::make_cron("0 25 23 30 6 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 9, 22, 14, 20, 0));
        expect_time(t, 2012, 6, 30, 23, 25, 0);
    }
    {
        auto cron = cron::make_cron("0 25 23 31 5 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 9, 22, 14, 20, 0));
        expect_time(t, 2012, 5, 31, 23, 25, 0);
    }
    {
        auto cron = cron::make_cron("0 25 23 30 4 ?");
        auto t = cron::cron_next(cron, make_tm(2011, 9, 22, 14, 20, 0));
        expect_time(t, 2012, 4, 30, 23, 25, 0);
    }
}

// ===== Error handling =====

TEST_F(CronExpressionTest, test_error) {
    EXPECT_THROW(cron::make_cron("* * * *"), cron::bad_cronexpr);
    EXPECT_THROW(cron::make_cron("0 -90 * * * *"), cron::bad_cronexpr);
    EXPECT_THROW(cron::make_cron("0 a * * * *"), cron::bad_cronexpr);
    EXPECT_THROW(cron::make_cron("0 0-10/error * * * *"), cron::bad_cronexpr);
    EXPECT_THROW(cron::make_cron("0 0-10/ * * * *"), cron::bad_cronexpr);
    EXPECT_THROW(cron::make_cron("0 * * 5-100 * *"), cron::bad_cronexpr);
}

TEST_F(CronExpressionTest, test_overflow) {
    EXPECT_THROW(cron::make_cron("0 0-10000000 * * * *"), cron::bad_cronexpr);
}

TEST_F(CronExpressionTest, test_invalid_zerorepeat) {
    EXPECT_THROW(cron::make_cron("0 */0 * * * *"), cron::bad_cronexpr);
}

// ===== DISABLED: Nth weekday (#) =====

TEST_F(CronExpressionTest, DISABLED_test_nth_weekday) {
    {
        auto cron = cron::make_cron("0 0 0 * * 6#1"); // 1st Saturday
        auto base = make_tm(2010, 2, 25);
        auto n1 = cron::cron_next(cron, base);
        expect_time(n1, 2010, 3, 6, 0, 0, 0);
        expect_wday(n1, 6);
        auto n2 = cron::cron_next(cron, n1);
        expect_time(n2, 2010, 4, 3, 0, 0, 0);
        expect_wday(n2, 6);
    }
    {
        auto cron = cron::make_cron("0 0 0 * * 3#5"); // 5th Wednesday
        auto base = make_tm(2010, 1, 25);
        auto n1 = cron::cron_next(cron, base);
        expect_time(n1, 2010, 3, 31, 0, 0, 0);
        auto n2 = cron::cron_next(cron, n1);
        expect_time(n2, 2010, 6, 30, 0, 0, 0);
        auto n3 = cron::cron_next(cron, n2);
        expect_time(n3, 2010, 9, 29, 0, 0, 0);
    }
}

TEST_F(CronExpressionTest, DISABLED_test_hash_mixup_all_fri_3rd_sat) {
    auto cron = cron::make_cron("0 0 0 * * 6#3"); // 3rd Saturday
    auto base = make_tm(2021, 3, 1);
    auto n = cron::cron_next(cron, base);
    expect_time(n, 2021, 3, 20, 0, 0, 0);
}

TEST_F(CronExpressionTest, DISABLED_test_lwom_mixup_firstlast_sat) {
    {
        auto cron = cron::make_cron("0 0 0 * * 6#1"); // 1st Saturday
        auto base = make_tm(2021, 3, 1);
        auto n = cron::cron_next(cron, base);
        expect_time(n, 2021, 3, 6, 0, 0, 0);
        n = cron::cron_next(cron, n);
        expect_time(n, 2021, 4, 3, 0, 0, 0);
        n = cron::cron_next(cron, n);
        expect_time(n, 2021, 5, 1, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 * * L6"); // Last Saturday
        auto base = make_tm(2021, 3, 1);
        auto n = cron::cron_next(cron, base);
        expect_time(n, 2021, 3, 27, 0, 0, 0);
        n = cron::cron_next(cron, n);
        expect_time(n, 2021, 4, 24, 0, 0, 0);
        n = cron::cron_next(cron, n);
        expect_time(n, 2021, 5, 29, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 * * L6,6#1"); // Last + 1st Saturday
        auto base = make_tm(2021, 3, 1);
        std::time_t results[6];
        auto n = base;
        for (auto& r : results) { n = cron::cron_next(cron, n); r = n; }
        expect_time(results[0], 2021, 3, 6, 0, 0, 0);
        expect_time(results[1], 2021, 3, 27, 0, 0, 0);
        expect_time(results[2], 2021, 4, 3, 0, 0, 0);
        expect_time(results[3], 2021, 4, 24, 0, 0, 0);
        expect_time(results[4], 2021, 5, 1, 0, 0, 0);
        expect_time(results[5], 2021, 5, 29, 0, 0, 0);
    }
}

TEST_F(CronExpressionTest, DISABLED_test_lwom_mixup_4th_and_last) {
    {
        auto cron = cron::make_cron("0 0 0 * * 1#4"); // 4th Monday
        auto base = make_tm(2021, 11, 1);
        auto n = cron::cron_next(cron, base);
        expect_time(n, 2021, 11, 22, 0, 0, 0);
        n = cron::cron_next(cron, n);
        expect_time(n, 2021, 12, 27, 0, 0, 0);
        n = cron::cron_next(cron, n);
        expect_time(n, 2022, 1, 24, 0, 0, 0);
    }
    {
        auto cron = cron::make_cron("0 0 0 * * 1#4,L1"); // 4th + last Monday
        auto base = make_tm(2021, 11, 1);
        std::time_t results[5];
        auto n = base;
        for (auto& r : results) { n = cron::cron_next(cron, n); r = n; }
        expect_time(results[0], 2021, 11, 22, 0, 0, 0);
        expect_time(results[1], 2021, 11, 29, 0, 0, 0);
        expect_time(results[2], 2021, 12, 27, 0, 0, 0);
        expect_time(results[3], 2022, 1, 24, 0, 0, 0);
        expect_time(results[4], 2022, 1, 31, 0, 0, 0);
    }
}

// ===== DISABLED: Last day of month (L in dom) =====

TEST_F(CronExpressionTest, DISABLED_test_last_day_of_month) {
    auto cron = cron::make_cron("0 0 0 L * *");
    auto base = make_tm(2015, 9, 4);
    auto n = cron::cron_next(cron, base);
    expect_time(n, 2015, 9, 30, 0, 0, 0);
    n = cron::cron_next(cron, n);
    expect_time(n, 2015, 10, 31, 0, 0, 0);
    n = cron::cron_next(cron, n);
    expect_time(n, 2015, 11, 30, 0, 0, 0);
    n = cron::cron_next(cron, n);
    expect_time(n, 2015, 12, 31, 0, 0, 0);
}

TEST_F(CronExpressionTest, DISABLED_test_last_day_of_month_with_leap_year) {
    auto cron = cron::make_cron("0 0 0 L * *");
    auto base = make_tm(2020, 2, 1);
    auto n = cron::cron_next(cron, base);
    expect_time(n, 2020, 2, 29, 0, 0, 0);
    n = cron::cron_next(cron, n);
    expect_time(n, 2020, 3, 31, 0, 0, 0);
}

TEST_F(CronExpressionTest, DISABLED_test_last_day_of_month_with_leap_year_century_exception) {
    auto cron = cron::make_cron("0 0 0 L * *");
    // 2100 is NOT a leap year
    {
        auto base = make_tm(2100, 2, 1);
        auto n = cron::cron_next(cron, base);
        expect_time(n, 2100, 2, 28, 0, 0, 0);
        n = cron::cron_next(cron, n);
        expect_time(n, 2100, 3, 31, 0, 0, 0);
    }
    // 2400 IS a leap year (divisible by 400)
    {
        auto base = make_tm(2400, 2, 1);
        auto n = cron::cron_next(cron, base);
        expect_time(n, 2400, 2, 29, 0, 0, 0);
        n = cron::cron_next(cron, n);
        expect_time(n, 2400, 3, 31, 0, 0, 0);
    }
}

TEST_F(CronExpressionTest, DISABLED_test_range_with_uppercase_last_day_of_month) {
    auto cron = cron::make_cron("0 0 0 29-L * *");
    auto base = make_tm(2015, 9, 4);
    auto n = cron::cron_next(cron, base);
    expect_time(n, 2015, 9, 29, 0, 0, 0);
    n = cron::cron_next(cron, n);
    expect_time(n, 2015, 9, 30, 0, 0, 0);
}

// ===== DISABLED: Last weekday of month (L in dow) =====

TEST_F(CronExpressionTest, DISABLED_test_lwom_friday) {
    auto cron = cron::make_cron("0 0 0 * * L5"); // Last Friday
    auto base = make_tm(1987, 1, 15);
    int expected[][3] = {
        {1987,1,30}, {1987,2,27}, {1987,3,27}, {1987,4,24},
        {1987,5,29}, {1987,6,26}, {1987,7,31}, {1987,8,28},
        {1987,9,25}, {1987,10,30}, {1987,11,27}, {1987,12,25}
    };
    auto n = base;
    for (auto& e : expected) {
        n = cron::cron_next(cron, n);
        expect_time(n, e[0], e[1], e[2], 0, 0, 0);
    }
}

TEST_F(CronExpressionTest, DISABLED_test_lwom_friday_2hours) {
    auto cron = cron::make_cron("0 0 1,5 * * L5");
    auto base = make_tm(1987, 1, 15);
    int expected[][4] = {
        {1987,1,30,1}, {1987,1,30,5}, {1987,2,27,1}, {1987,2,27,5},
        {1987,3,27,1}, {1987,3,27,5}, {1987,4,24,1}, {1987,4,24,5},
        {1987,5,29,1}, {1987,5,29,5}, {1987,6,26,1}, {1987,6,26,5}
    };
    auto n = base;
    for (auto& e : expected) {
        n = cron::cron_next(cron, n);
        expect_time(n, e[0], e[1], e[2], e[3], 0, 0);
    }
}

TEST_F(CronExpressionTest, DISABLED_test_lwom_tue_thu) {
    auto cron = cron::make_cron("0 0 0 * * L2,L4");
    auto base = make_tm(2016, 6, 1);
    int expected[][3] = {
        {2016,6,28}, {2016,6,30}, {2016,7,26}, {2016,7,28},
        {2016,8,25}, {2016,8,30}, {2016,9,27}, {2016,9,29},
        {2016,10,25}, {2016,10,27}
    };
    auto n = base;
    for (auto& e : expected) {
        n = cron::cron_next(cron, n);
        expect_time(n, e[0], e[1], e[2], 0, 0, 0);
    }
}

// ===== Impossible date =====

TEST_F(CronExpressionTest, test_bug34) {
    // Feb 31 never exists — cron_next should not hang
    // croncpp returns (time_t)-1 for impossible dates (unlike croniter which throws)
    auto cron = cron::make_cron("0 * * 31 2 *");
    auto base = make_tm(2012, 2, 24, 0, 0, 0);
    auto n = cron::cron_next(cron, base);
    EXPECT_EQ(n, static_cast<std::time_t>(-1));
}

// ===== DISABLED: L in dom mixed with numbers =====

TEST_F(CronExpressionTest, DISABLED_test_mixdow) {
    // dom field: 7,14,21,L — L not supported in croncpp
    auto cron = cron::make_cron("0 1 1 7,14,21,L * *");
    auto base = make_tm(2018, 10, 1, 0, 0);
    auto n = cron::cron_next(cron, base);
    // Just check it returns something valid (doesn't crash)
    EXPECT_GT(n, base);
}

// ===== Year 2038 problem =====

TEST_F(CronExpressionTest, test_issue_2038y) {
    auto cron = cron::make_cron("0 */1 * * * *");
    auto base = make_tm(2040, 1, 1, 0, 0);
    EXPECT_NO_THROW({
        auto n = cron::cron_next(cron, base);
        expect_time(n, 2040, 1, 1, 0, 1, 0);
    });
}

// ===== Question mark syntax =====

TEST_F(CronExpressionTest, test_question_mark) {
    // ? in dow field should act as * (any)
    auto cron = cron::make_cron("0 0 0 1 * ?");
    auto base = make_tm(2010, 8, 25, 15, 56);
    auto n = cron::cron_next(cron, base);
    expect_time(n, 2010, 9, 1, 0, 0, 0);
}

TEST_F(CronExpressionTest, test_invalid_question_mark) {
    // ? is only valid in dom and dow fields
    EXPECT_THROW(cron::make_cron("0 ? * * * *"), cron::bad_cronexpr);   // min
    EXPECT_THROW(cron::make_cron("0 * ? * * *"), cron::bad_cronexpr);   // hour
    EXPECT_THROW(cron::make_cron("0 * * ?,* * *"), cron::bad_cronexpr); // dom with comma
}

// ===== DISABLED: # and L out-of-range validation =====

TEST_F(CronExpressionTest, DISABLED_test_nth_out_of_range) {
    EXPECT_THROW(cron::make_cron("0 0 0 * * 1#7"), cron::bad_cronexpr); // nth > 5
    EXPECT_THROW(cron::make_cron("0 0 0 * * 1#0"), cron::bad_cronexpr); // nth = 0
}

TEST_F(CronExpressionTest, DISABLED_test_last_out_of_range) {
    EXPECT_THROW(cron::make_cron("0 0 0 * * L-1"), cron::bad_cronexpr); // negative dow
    EXPECT_THROW(cron::make_cron("0 0 0 * * L8"), cron::bad_cronexpr);  // dow > 7
}
