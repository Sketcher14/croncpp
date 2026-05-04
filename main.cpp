#include "croncpp.h"

#include <iostream>

int main()
{
    try
    {
        // Standard 6-field cron: sec min hour day-of-month month day-of-week
        // "Every weekday (Mon-Fri) at 09:30:00"
        auto cex = cron::make_cron("0 30 9 * * MON-FRI");

        std::cout << "Parsed cron expression: " << cron::to_cronstr(cex) << '\n';
        std::cout << "Bitset form: "             << cron::to_string(cex)  << '\n';

        std::time_t now  = std::time(nullptr);
        std::time_t next = cron::cron_next(cex, now);

        std::tm now_tm{};
        std::tm next_tm{};
        cron::utils::time_to_tm(&now,  &now_tm);
        cron::utils::time_to_tm(&next, &next_tm);

        std::cout << "Now:  " << cron::utils::to_string(now_tm)  << '\n';
        std::cout << "Next: " << cron::utils::to_string(next_tm) << '\n';
    }
    catch (cron::bad_cronexpr const& ex)
    {
        std::cerr << "Invalid cron expression: " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
