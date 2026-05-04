#include <gtest/gtest.h>
#include "croncpp.h"

#define CRON_STD_EQUAL(x, y)     EXPECT_EQ(make_cron(x), make_cron(y))
#define CRON_EXPECT_EXCEPT(x)    EXPECT_THROW(make_cron(x), bad_cronexpr)

using namespace cron;

class CronTest : public ::testing::Test
{
protected:
    static void SetUpTestSuite() {
        setenv("TZ", "UTC", 1);
        tzset();
    }
};


// ===== Parsing equivalence =====

TEST_F(CronTest, test_simple) {
    auto cex = make_cron("* * * * * *");
    EXPECT_EQ(to_string(cex), "111111111111111111111111111111111111111111111111111111111111 111111111111111111111111111111111111111111111111111111111111 111111111111111111111111 1111111111111111111111111111111 111111111111 1111111");
}

TEST_F(CronTest, test_check_seconds) {
    CRON_STD_EQUAL("*/5 * * * * *", "0,5,10,15,20,25,30,35,40,45,50,55 * * * * *");
    CRON_STD_EQUAL("1/6 * * * * *", "1,7,13,19,25,31,37,43,49,55 * * * * *");
    CRON_STD_EQUAL("0/30 * * * * *", "0,30 * * * * *");
    CRON_STD_EQUAL("0-5 * * * * *", "0,1,2,3,4,5 * * * * *");
    CRON_STD_EQUAL("55/1 * * * * *", "55,56,57,58,59 * * * * *");
    CRON_STD_EQUAL("57,59 * * * * *", "57/2 * * * * *");
    CRON_STD_EQUAL("1,3,5 * * * * *", "1-6/2 * * * * *");
    CRON_STD_EQUAL("0,5,10,15 * * * * *", "0-15/5 * * * * *");
}

TEST_F(CronTest, test_check_minutes) {
    CRON_STD_EQUAL("* */5 * * * *", "* 0,5,10,15,20,25,30,35,40,45,50,55 * * * *");
    CRON_STD_EQUAL("* 1/6 * * * *", "* 1,7,13,19,25,31,37,43,49,55 * * * *");
    CRON_STD_EQUAL("* 0/30 * * * *", "* 0,30 * * * *");
    CRON_STD_EQUAL("* 0-5 * * * *", "* 0,1,2,3,4,5 * * * *");
    CRON_STD_EQUAL("* 55/1 * * * *", "* 55,56,57,58,59 * * * *");
    CRON_STD_EQUAL("* 57,59 * * * *", "* 57/2 * * * *");
    CRON_STD_EQUAL("* 1,3,5 * * * *", "* 1-6/2 * * * *");
    CRON_STD_EQUAL("* 0,5,10,15 * * * *", "* 0-15/5 * * * *");
}

TEST_F(CronTest, test_check_hours) {
    CRON_STD_EQUAL("* * */5 * * *", "* * 0,5,10,15,20 * * *");
    CRON_STD_EQUAL("* * 1/6 * * *", "* * 1,7,13,19 * * *");
    CRON_STD_EQUAL("* * 0/12 * * *", "* * 0,12 * * *");
    CRON_STD_EQUAL("* * 0-5 * * *", "* * 0,1,2,3,4,5 * * *");
    CRON_STD_EQUAL("* * 15/1 * * *", "* * 15,16,17,18,19,20,21,22,23 * * *");
    CRON_STD_EQUAL("* * 17,19,21,23 * * *", "* * 17/2 * * *");
    CRON_STD_EQUAL("* * 1,3,5 * * *", "* * 1-6/2 * * *");
    CRON_STD_EQUAL("* * 0,5,10,15 * * *", "* * 0-15/5 * * *");
}

TEST_F(CronTest, test_check_days_of_month) {
    CRON_STD_EQUAL("* * * 1-31 * *", "* * * 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31 * *");
    CRON_STD_EQUAL("* * * 1/5 * *", "* * * 1,6,11,16,21,26,31 * *");
    CRON_STD_EQUAL("* * * 1,11,21,31 * *", "* * * 1-31/10 * *");
    CRON_STD_EQUAL("* * * */5 * *", "* * * 1,6,11,16,21,26,31 * *");
}

TEST_F(CronTest, test_check_months) {
    CRON_STD_EQUAL("* * * * 1,6,11 *", "* * * * 1/5 *");
    CRON_STD_EQUAL("* * * * 1-12 *", "* * * * 1,2,3,4,5,6,7,8,9,10,11,12 *");
    CRON_STD_EQUAL("* * * * 1-12/3 *", "* * * * 1,4,7,10 *");
    CRON_STD_EQUAL("* * * * */2 *", "* * * * 1,3,5,7,9,11 *");
    CRON_STD_EQUAL("* * * * 2/2 *", "* * * * 2,4,6,8,10,12 *");
    CRON_STD_EQUAL("* * * * 1 *", "* * * * JAN *");
    CRON_STD_EQUAL("* * * * 2 *", "* * * * FEB *");
    CRON_STD_EQUAL("* * * * 3 *", "* * * * mar *");
    CRON_STD_EQUAL("* * * * 4 *", "* * * * apr *");
    CRON_STD_EQUAL("* * * * 5 *", "* * * * may *");
    CRON_STD_EQUAL("* * * * 6 *", "* * * * Jun *");
    CRON_STD_EQUAL("* * * * 7 *", "* * * * Jul *");
    CRON_STD_EQUAL("* * * * 8 *", "* * * * auG *");
    CRON_STD_EQUAL("* * * * 9 *", "* * * * sEp *");
    CRON_STD_EQUAL("* * * * 10 *", "* * * * oCT *");
    CRON_STD_EQUAL("* * * * 11 *", "* * * * nOV *");
    CRON_STD_EQUAL("* * * * 12 *", "* * * * DEC *");
    CRON_STD_EQUAL("* * * * 1,FEB *", "* * * * JAN,2 *");
    CRON_STD_EQUAL("* * * * 1,6,11 *", "* * * * NOV,JUN,jan *");
    CRON_STD_EQUAL("* * * * 1,6,11 *", "* * * * jan,jun,nov *");
    CRON_STD_EQUAL("* * * * 1,6,11 *", "* * * * jun,jan,nov *");
    CRON_STD_EQUAL("* * * * JAN,FEB,MAR,APR,MAY,JUN,JUL,AUG,SEP,OCT,NOV,DEC *", "* * * * 1,2,3,4,5,6,7,8,9,10,11,12 *");
    CRON_STD_EQUAL("* * * * JUL,AUG,SEP,OCT,NOV,DEC,JAN,FEB,MAR,APR,MAY,JUN *", "* * * * 1,2,3,4,5,6,7,8,9,10,11,12 *");
}

TEST_F(CronTest, test_check_days_of_week) {
    CRON_STD_EQUAL("* * * * * 0-6", "* * * * * 0,1,2,3,4,5,6");
    CRON_STD_EQUAL("* * * * * 0-6/2", "* * * * * 0,2,4,6");
    CRON_STD_EQUAL("* * * * * 0-6/3", "* * * * * 0,3,6");
    CRON_STD_EQUAL("* * * * * */3", "* * * * * 0,3,6");
    CRON_STD_EQUAL("* * * * * 1/3", "* * * * * 1,4");
    CRON_STD_EQUAL("* * * * * 0", "* * * * * SUN");
    CRON_STD_EQUAL("* * * * * 1", "* * * * * MON");
    CRON_STD_EQUAL("* * * * * 2", "* * * * * TUE");
    CRON_STD_EQUAL("* * * * * 3", "* * * * * WED");
    CRON_STD_EQUAL("* * * * * 4", "* * * * * THU");
    CRON_STD_EQUAL("* * * * * 5", "* * * * * FRI");
    CRON_STD_EQUAL("* * * * * 6", "* * * * * SAT");
    CRON_STD_EQUAL("* * * * * 0-6", "* * * * * SUN,MON,TUE,WED,THU,FRI,SAT");
    CRON_STD_EQUAL("* * * * * 0-6/2", "* * * * * SUN,TUE,THU,SAT");
    CRON_STD_EQUAL("* * * * * 0-6/3", "* * * * * SUN,WED,SAT");
    CRON_STD_EQUAL("* * * * * */3", "* * * * * SUN,WED,SAT");
    CRON_STD_EQUAL("* * * * * 1/3", "* * * * * MON,THU");
}

// ===== Parsing errors =====

TEST_F(CronTest, test_invalid_seconds) {
    CRON_EXPECT_EXCEPT("TEN * * * * *");
    CRON_EXPECT_EXCEPT("60 * * * * *");
    CRON_EXPECT_EXCEPT("-1 * * * * *");
    CRON_EXPECT_EXCEPT("0-60 * * * * *");
    CRON_EXPECT_EXCEPT("0-10-20 * * * * *");
    CRON_EXPECT_EXCEPT(", * * * * *");
    CRON_EXPECT_EXCEPT("0,,1 * * * * *");
    CRON_EXPECT_EXCEPT("0,1, * * * * *");
    CRON_EXPECT_EXCEPT(",0,1 * * * * *");
    CRON_EXPECT_EXCEPT("0/10/2 * * * * *");
    CRON_EXPECT_EXCEPT("/ * * * * *");
    CRON_EXPECT_EXCEPT("/2 * * * * *");
    CRON_EXPECT_EXCEPT("2/ * * * * *");
    CRON_EXPECT_EXCEPT("*/ * * * * *");
    CRON_EXPECT_EXCEPT("/* * * * * *");
    CRON_EXPECT_EXCEPT("-/ * * * * *");
    CRON_EXPECT_EXCEPT("/- * * * * *");
    CRON_EXPECT_EXCEPT("*-/ * * * * *");
    CRON_EXPECT_EXCEPT("-*/ * * * * *");
    CRON_EXPECT_EXCEPT("/-* * * * * *");
    CRON_EXPECT_EXCEPT("/*- * * * * *");
    CRON_EXPECT_EXCEPT("*2/ * * * * *");
    CRON_EXPECT_EXCEPT("/2* * * * * *");
    CRON_EXPECT_EXCEPT("-2/ * * * * *");
    CRON_EXPECT_EXCEPT("/2- * * * * *");
    CRON_EXPECT_EXCEPT("*2-/ * * * * *");
    CRON_EXPECT_EXCEPT("-2*/ * * * * *");
    CRON_EXPECT_EXCEPT("/2-* * * * * *");
    CRON_EXPECT_EXCEPT("/2*- * * * * *");
}

TEST_F(CronTest, test_invalid_minutes) {
    CRON_EXPECT_EXCEPT("* TEN * * * *");
    CRON_EXPECT_EXCEPT("* 60 * * * *");
    CRON_EXPECT_EXCEPT("* -1 * * * *");
    CRON_EXPECT_EXCEPT("* 0-60 * * * *");
    CRON_EXPECT_EXCEPT("* 0-10-20 * * * *");
    CRON_EXPECT_EXCEPT("* , * * * *");
    CRON_EXPECT_EXCEPT("* 0,,1 * * * *");
    CRON_EXPECT_EXCEPT("* 0,1, * * * *");
    CRON_EXPECT_EXCEPT("* ,0,1 * * * *");
    CRON_EXPECT_EXCEPT("* 0/10/2 * * * *");
    CRON_EXPECT_EXCEPT("* / * * * *");
    CRON_EXPECT_EXCEPT("* /2 * * * *");
    CRON_EXPECT_EXCEPT("* 2/ * * * *");
    CRON_EXPECT_EXCEPT("* */ * * * *");
    CRON_EXPECT_EXCEPT("* /* * * * *");
    CRON_EXPECT_EXCEPT("* -/ * * * *");
    CRON_EXPECT_EXCEPT("* /- * * * *");
    CRON_EXPECT_EXCEPT("* *-/ * * * *");
    CRON_EXPECT_EXCEPT("* -*/ * * * *");
    CRON_EXPECT_EXCEPT("* /-* * * * *");
    CRON_EXPECT_EXCEPT("* /*- * * * *");
    CRON_EXPECT_EXCEPT("* *2/ * * * *");
    CRON_EXPECT_EXCEPT("* /2* * * * *");
    CRON_EXPECT_EXCEPT("* -2/ * * * *");
    CRON_EXPECT_EXCEPT("* /2- * * * *");
    CRON_EXPECT_EXCEPT("* *2-/ * * * *");
    CRON_EXPECT_EXCEPT("* -2*/ * * * *");
    CRON_EXPECT_EXCEPT("* /2-* * * * *");
    CRON_EXPECT_EXCEPT("* /2*- * * * *");
}

TEST_F(CronTest, test_invalid_hours) {
    CRON_EXPECT_EXCEPT("* * TEN * * *");
    CRON_EXPECT_EXCEPT("* * 24 * * *");
    CRON_EXPECT_EXCEPT("* * -1 * * *");
    CRON_EXPECT_EXCEPT("* * 0-24 * * *");
    CRON_EXPECT_EXCEPT("* * 0-10-20 * * *");
    CRON_EXPECT_EXCEPT("* * , * * *");
    CRON_EXPECT_EXCEPT("* * 0,,1 * * *");
    CRON_EXPECT_EXCEPT("* * 0,1, * * *");
    CRON_EXPECT_EXCEPT("* * ,0,1 * * *");
    CRON_EXPECT_EXCEPT("* * 0/10/2 * * *");
    CRON_EXPECT_EXCEPT("* * / * * *");
    CRON_EXPECT_EXCEPT("* * /2 * * *");
    CRON_EXPECT_EXCEPT("* * 2/ * * *");
    CRON_EXPECT_EXCEPT("* * */ * * *");
    CRON_EXPECT_EXCEPT("* * /* * * *");
    CRON_EXPECT_EXCEPT("* * -/ * * *");
    CRON_EXPECT_EXCEPT("* * /- * * *");
    CRON_EXPECT_EXCEPT("* * *-/ * * *");
    CRON_EXPECT_EXCEPT("* * -*/ * * *");
    CRON_EXPECT_EXCEPT("* * /-* * * *");
    CRON_EXPECT_EXCEPT("* * /*- * * *");
    CRON_EXPECT_EXCEPT("* * *2/ * * *");
    CRON_EXPECT_EXCEPT("* * /2* * * *");
    CRON_EXPECT_EXCEPT("* * -2/ * * *");
    CRON_EXPECT_EXCEPT("* * /2- * * *");
    CRON_EXPECT_EXCEPT("* * *2-/ * * *");
    CRON_EXPECT_EXCEPT("* * -2*/ * * *");
    CRON_EXPECT_EXCEPT("* * /2-* * * *");
    CRON_EXPECT_EXCEPT("* * /2*- * * *");
}

TEST_F(CronTest, test_invalid_days_of_month) {
    CRON_EXPECT_EXCEPT("* * * TEN * *");
    CRON_EXPECT_EXCEPT("* * * 32 * *");
    CRON_EXPECT_EXCEPT("* * * 0 * *");
    CRON_EXPECT_EXCEPT("* * * 0-32 * *");
    CRON_EXPECT_EXCEPT("* * * 0-10-20 * *");
    CRON_EXPECT_EXCEPT("* * * , * *");
    CRON_EXPECT_EXCEPT("* * * 0,,1 * *");
    CRON_EXPECT_EXCEPT("* * * 0,1, * *");
    CRON_EXPECT_EXCEPT("* * * ,0,1 * *");
    CRON_EXPECT_EXCEPT("* * * 0/10/2 * * *");
    CRON_EXPECT_EXCEPT("* * * / * *");
    CRON_EXPECT_EXCEPT("* * * /2 * *");
    CRON_EXPECT_EXCEPT("* * * 2/ * *");
    CRON_EXPECT_EXCEPT("* * * */ * *");
    CRON_EXPECT_EXCEPT("* * * /* * *");
    CRON_EXPECT_EXCEPT("* * * -/ * *");
    CRON_EXPECT_EXCEPT("* * * /- * *");
    CRON_EXPECT_EXCEPT("* * * *-/ * *");
    CRON_EXPECT_EXCEPT("* * * -*/ * *");
    CRON_EXPECT_EXCEPT("* * * /-* * *");
    CRON_EXPECT_EXCEPT("* * * /*- * *");
    CRON_EXPECT_EXCEPT("* * * *2/ * *");
    CRON_EXPECT_EXCEPT("* * * /2* * *");
    CRON_EXPECT_EXCEPT("* * * -2/ * *");
    CRON_EXPECT_EXCEPT("* * * /2- * *");
    CRON_EXPECT_EXCEPT("* * * *2-/ * *");
    CRON_EXPECT_EXCEPT("* * * -2*/ * *");
    CRON_EXPECT_EXCEPT("* * * /2-* * *");
    CRON_EXPECT_EXCEPT("* * * /2*- * *");
}

TEST_F(CronTest, test_invalid_months) {
    CRON_EXPECT_EXCEPT("* * * * TEN *");
    CRON_EXPECT_EXCEPT("* * * * 13 *");
    CRON_EXPECT_EXCEPT("* * * * 0 *");
    CRON_EXPECT_EXCEPT("* * * * 0-13 *");
    CRON_EXPECT_EXCEPT("* * * * 0-10-11 *");
    CRON_EXPECT_EXCEPT("* * * * , *");
    CRON_EXPECT_EXCEPT("* * * * 0,,1 *");
    CRON_EXPECT_EXCEPT("* * * * 0,1, *");
    CRON_EXPECT_EXCEPT("* * * * ,0,1 *");
    CRON_EXPECT_EXCEPT("* * * * 0/10/2 *");
    CRON_EXPECT_EXCEPT("* * * * / *");
    CRON_EXPECT_EXCEPT("* * * * /2 *");
    CRON_EXPECT_EXCEPT("* * * * 2/ *");
    CRON_EXPECT_EXCEPT("* * * * */ *");
    CRON_EXPECT_EXCEPT("* * * * /* *");
    CRON_EXPECT_EXCEPT("* * * * -/ *");
    CRON_EXPECT_EXCEPT("* * * * /- *");
    CRON_EXPECT_EXCEPT("* * * * *-/ *");
    CRON_EXPECT_EXCEPT("* * * * -*/ *");
    CRON_EXPECT_EXCEPT("* * * * /-* *");
    CRON_EXPECT_EXCEPT("* * * * /*- *");
    CRON_EXPECT_EXCEPT("* * * * *2/ *");
    CRON_EXPECT_EXCEPT("* * * * /2* *");
    CRON_EXPECT_EXCEPT("* * * * -2/ *");
    CRON_EXPECT_EXCEPT("* * * * /2- *");
    CRON_EXPECT_EXCEPT("* * * * *2-/ *");
    CRON_EXPECT_EXCEPT("* * * * -2*/ *");
    CRON_EXPECT_EXCEPT("* * * * /2-* *");
    CRON_EXPECT_EXCEPT("* * * * /2*- *");
}

TEST_F(CronTest, test_invalid_days_of_week) {
    CRON_EXPECT_EXCEPT("* * * * * TEN");
    CRON_EXPECT_EXCEPT("* * * * * 7");
    CRON_EXPECT_EXCEPT("* * * * * -1");
    CRON_EXPECT_EXCEPT("* * * * * -1-7");
    CRON_EXPECT_EXCEPT("* * * * * 0-5-6");
    CRON_EXPECT_EXCEPT("* * * * * ,");
    CRON_EXPECT_EXCEPT("* * * * * 0,,1");
    CRON_EXPECT_EXCEPT("* * * * * 0,1,");
    CRON_EXPECT_EXCEPT("* * * * * ,0,1");
    CRON_EXPECT_EXCEPT("* * * * * 0/2/2 *");
    CRON_EXPECT_EXCEPT("* * * * * /");
    CRON_EXPECT_EXCEPT("* * * * * /2");
    CRON_EXPECT_EXCEPT("* * * * * 2/");
    CRON_EXPECT_EXCEPT("* * * * * */");
    CRON_EXPECT_EXCEPT("* * * * * /*");
    CRON_EXPECT_EXCEPT("* * * * * -/");
    CRON_EXPECT_EXCEPT("* * * * * /-");
    CRON_EXPECT_EXCEPT("* * * * * *-/");
    CRON_EXPECT_EXCEPT("* * * * * -*/");
    CRON_EXPECT_EXCEPT("* * * * * /-*");
    CRON_EXPECT_EXCEPT("* * * * * /*-");
    CRON_EXPECT_EXCEPT("* * * * * *2/");
    CRON_EXPECT_EXCEPT("* * * * * /2*");
    CRON_EXPECT_EXCEPT("* * * * * -2/");
    CRON_EXPECT_EXCEPT("* * * * * /2-");
    CRON_EXPECT_EXCEPT("* * * * * *2-/");
    CRON_EXPECT_EXCEPT("* * * * * -2*/");
    CRON_EXPECT_EXCEPT("* * * * * /2-*");
    CRON_EXPECT_EXCEPT("* * * * * /2*-");
}

// ===== Round-trip preservation =====

TEST_F(CronTest, test_cron_expr) {
    EXPECT_EQ(to_cronstr(make_cron("*/15 * 1-4 * * *")),  std::string("*/15 * 1-4 * * *"));
    EXPECT_EQ(to_cronstr(make_cron("0 */2 1-4 * * *")),   std::string("0 */2 1-4 * * *"));
    EXPECT_EQ(to_cronstr(make_cron("* * * * * *")),       std::string("* * * * * *"));
    EXPECT_EQ(to_cronstr(make_cron("10 * * * * *")),      std::string("10 * * * * *"));
    EXPECT_EQ(to_cronstr(make_cron("11 * * * * *")),      std::string("11 * * * * *"));
    EXPECT_EQ(to_cronstr(make_cron("10-15 * * * * *")),   std::string("10-15 * * * * *"));
    EXPECT_EQ(to_cronstr(make_cron("0 * * * * *")),       std::string("0 * * * * *"));
    EXPECT_EQ(to_cronstr(make_cron("0 11 * * * *")),      std::string("0 11 * * * *"));
    EXPECT_EQ(to_cronstr(make_cron("0 10 * * * *")),      std::string("0 10 * * * *"));
    EXPECT_EQ(to_cronstr(make_cron("0 0 * * * *")),       std::string("0 0 * * * *"));
    EXPECT_EQ(to_cronstr(make_cron("0 0 0 * * *")),       std::string("0 0 0 * * *"));
    EXPECT_EQ(to_cronstr(make_cron("* * * 10 * *")),      std::string("* * * 10 * *"));
    EXPECT_EQ(to_cronstr(make_cron("0 0 0 1 * *")),       std::string("0 0 0 1 * *"));
    EXPECT_EQ(to_cronstr(make_cron("0 0 0 31 * *")),      std::string("0 0 0 31 * *"));
    EXPECT_EQ(to_cronstr(make_cron("* * * * * 2")),       std::string("* * * * * 2"));
    EXPECT_EQ(to_cronstr(make_cron("55 5 * * * *")),      std::string("55 5 * * * *"));
    EXPECT_EQ(to_cronstr(make_cron("55 * 10 * * *")),     std::string("55 * 10 * * *"));
    EXPECT_EQ(to_cronstr(make_cron("* 5 10 * * *")),      std::string("* 5 10 * * *"));
    EXPECT_EQ(to_cronstr(make_cron("55 * * 3 * *")),      std::string("55 * * 3 * *"));
    EXPECT_EQ(to_cronstr(make_cron("* * * 3 11 *")),      std::string("* * * 3 11 *"));
    EXPECT_EQ(to_cronstr(make_cron("0 0 0 29 2 *")),      std::string("0 0 0 29 2 *"));
    EXPECT_EQ(to_cronstr(make_cron("0 0 7 ? * MON-FRI")), std::string("0 0 7 ? * MON-FRI"));
    EXPECT_EQ(to_cronstr(make_cron("0 30 23 30 1/3 ?")),  std::string("0 30 23 30 1/3 ?"));
}

