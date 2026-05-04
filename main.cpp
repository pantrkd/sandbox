#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @brief Forward declaration for TimeStamp structure.
 */
struct TimeStamp;

/**
 * @brief Concept that ensures a type contains time boundaries.
 *
 * A valid TimeStampable type must expose:
 * - start_time
 * - end_time
 *
 * Both must be convertible to TimeStamp.
 *
 * @tparam Ty Type to validate.
 */
template <typename Ty>
concept TimeStampable = requires(Ty v) {
    { v.start_time } -> std::convertible_to<TimeStamp>;
    { v.end_time } -> std::convertible_to<TimeStamp>;
};

/**
 * @brief Represents a time in 24-hour format.
 */
struct TimeStamp {
    int hour;    ///< Hour component [0–23]
    int minute;  ///< Minute component [0–59]

    /**
     * @brief Namespace-like container for time-related algorithms.
     */
    struct Algorithms {
        /**
         * @brief Validates that a time interval does not overlap with existing ones.
         *
         * Checks whether the interval in `looking_at` conflicts with any
         * intervals in `already_added`.
         *
         * @tparam T A TimeStampable type.
         * @param looking_at The candidate element to validate.
         * @param already_added Collection of already selected elements.
         * @return true if no overlap is detected.
         * @return false if any overlap exists.
         */
        template <typename T>
            requires TimeStampable<T>
        constexpr static bool validate_times(
            const T& looking_at,
            const std::vector<T>& already_added) {
            return std::all_of(
                already_added.cbegin(),
                already_added.cend(),
                [&](const T& elem) -> bool {
                    return [](const T& a, const T& b) -> bool {
                        // NOTE: This logic only compares hours (not minutes)
                        // and may produce incorrect overlap detection.

                        if (a.start_time.hour >= b.start_time.hour &&
                            a.start_time.hour <= b.start_time.hour) {
                            return false;
                        }

                        if (a.end_time.hour >= b.start_time.hour &&
                            a.end_time.hour <= b.end_time.hour) {
                            return false;
                        }

                        return true;
                    }(elem, looking_at);
                });
        }
    };
};

/**
 * @brief Represents an instructor.
 */
struct Teacher {
    std::string first_name;  ///< Instructor first name
    std::string last_name;   ///< Instructor last name
};

/**
 * @brief Represents a course with scheduling information.
 */
struct CourseData {
    std::string course_name;  ///< Course identifier (e.g., "COP")
    Teacher teacher;          ///< Assigned instructor
    std::uint8_t crn;         ///< Course reference number
    TimeStamp start_time;     ///< Start time
    TimeStamp end_time;       ///< End time

    /// Default constructor
    CourseData() = default;

    /**
     * @brief Construct a CourseData object.
     */
    CourseData(const std::string& cname,
               Teacher teach,
               std::uint8_t crn,
               TimeStamp start_time,
               TimeStamp end_time)
        : course_name{cname},
          teacher{teach},
          crn{crn},
          start_time{start_time},
          end_time{end_time} {}

    /// Copy constructor
    CourseData(const CourseData& other) = default;

    /// Move constructor
    CourseData(CourseData&& other) = default;

    /// Copy assignment
    CourseData& operator=(const CourseData& o) = default;

    /// Destructor
    ~CourseData() = default;
};

/**
 * @brief Stream operator for TimeStamp.
 *
 * Outputs time in HH:MM format.
 *
 * @param os Output stream.
 * @param t TimeStamp instance.
 * @return std::ostream&
 */
std::ostream& operator<<(std::ostream& os, const TimeStamp& t) {
    if (t.hour < 10)
        os << "0";
    os << t.hour << ":";

    if (t.minute < 10)
        os << "0";
    os << t.minute;

    return os;
}

/**
 * @brief Prints a list of course schedules.
 *
 * @param vec Vector of CourseData representing a schedule.
 */
void print_queries(const std::vector<CourseData>& vec) {
    std::cout << "{\n";
    for (const auto& c : vec) {
        std::cout << c.course_name << " "
                  << c.teacher.first_name << " "
                  << c.teacher.last_name << " "
                  << c.start_time << "-"
                  << c.end_time
                  << std::endl;
    }
    std::cout << "}";
}

/**
 * @brief Generates valid course schedules without time conflicts.
 *
 * The Scheduler operates on immutable course data and produces
 * combinations of courses that satisfy time constraints.
 */
class Scheduler {
    const std::vector<CourseData> ro_courses;  ///< Immutable course pool

    /**
     * @brief Recursive helper to generate valid permutations.
     *
     * @param filtered_pool Filtered course list.
     * @param vis Tracks visited course names.
     * @param curr Current partial schedule.
     * @param ret Output collection of valid schedules.
     * @param query_size Desired number of courses.
     * @param index Current recursion depth.
     */
    void permute_filtered_courses(
        const std::vector<CourseData>& filtered_pool,
        std::unordered_map<std::string, bool>& vis,
        std::vector<CourseData> curr,
        std::vector<std::vector<CourseData>>& ret,
        const int query_size,
        int index = 0) const {
        if (index > query_size)
            return;

        if (curr.size() >= query_size) {
            ret.push_back(curr);
            return;
        }

        for (int i = 0; i < filtered_pool.size(); i++) {
            if (vis[filtered_pool[i].course_name])
                continue;

            if (!TimeStamp::Algorithms::validate_times(filtered_pool[i], curr))
                continue;

            vis[filtered_pool[i].course_name] = true;
            curr.push_back(filtered_pool[i]);

            permute_filtered_courses(
                filtered_pool, vis, curr, ret, query_size, index + 1);

            curr.pop_back();
            vis[filtered_pool[i].course_name] = false;
        }
    }

   public:
    /**
     * @brief Constructs a Scheduler with a set of courses.
     *
     * @tparam Args Types convertible to CourseData.
     * @param data Course entries.
     */
    template <typename... Args>
        requires(std::convertible_to<Args, CourseData> && ...)
    Scheduler(Args&&... data)
        : ro_courses{std::forward<Args>(data)...} {}

    /**
     * @brief Generates all valid schedules for given course names.
     *
     * Filters the course pool and returns all non-overlapping
     * combinations matching the requested course names.
     *
     * @tparam T String-like types for course names.
     * @param courses_to_query Names of courses to include.
     * @return Vector of valid course combinations.
     */
    template <typename... T>
        requires(std::convertible_to<T, const char*> && ...)
    [[nodiscard]] std::vector<std::vector<CourseData>>
    permute_courses(T&&... courses_to_query) const noexcept {
        /**
         * !TO-DO
         * GENERALIZE "courses_to_query" TO QUERY TYPE INSTEAD OF JUST STRINGS FOR THE COURSE NAME
         *
         */
        std::vector<CourseData> filtered;
        std::copy_if(
            ro_courses.cbegin(),
            ro_courses.cend(),
            std::back_inserter(filtered),
            [&](auto c) {
                return ((c.course_name == courses_to_query) || ...);
            });

        std::unordered_map<std::string, bool> vis;
        std::vector<std::vector<CourseData>> possibilities;

        this->permute_filtered_courses(
            filtered,
            vis,
            {},
            possibilities,
            sizeof...(courses_to_query),
            0);

        return possibilities;
    }
};

int main(int argc, char** argv) {
    CourseData cop_dummy1{"COP", Teacher{"Tim", "Bob"}, 1, TimeStamp{12, 30},
                          TimeStamp{13, 45}};
    CourseData cop_dummy2{"COP", Teacher{"Jim", "Sob"}, 1, TimeStamp{14, 0},
                          TimeStamp{15, 40}};
    CourseData cot_dummy1{"COT", Teacher{"Lim", "Nom"}, 1, TimeStamp{14, 5},
                          TimeStamp{15, 45}};
    CourseData cot_dummy2{"COT", Teacher{"Stim", "Murmer"}, 1, TimeStamp{2, 5},
                          TimeStamp{3, 45}};

    std::vector<std::string> courses_to_query = {"COT", "COP"};
    Scheduler s{cop_dummy1, cop_dummy2, cot_dummy1,
                cot_dummy2};
    auto possibilities = s.permute_courses("COT", "COP");
    for (const auto& p : possibilities) {
        print_queries(p);
    }

    return 0;
}