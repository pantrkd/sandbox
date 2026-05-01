#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>

struct Teacher {
    std::string first_name;
    std::string last_name;
};

struct CourseData {
    std::string course_name;
    Teacher teacher;
    std::uint8_t crn;
    std::string start_time;
    std::string end_time;

    CourseData() = default;
    CourseData(const std::string& cname, Teacher teach, std::uint8_t crn, std::string start_time, std::string end_time)
        : course_name{cname}, teacher{teach}, crn{crn}, start_time{start_time}, end_time{end_time} {};
    CourseData(const CourseData& other)
        : course_name{other.course_name}, teacher{other.teacher}, crn{other.crn}, start_time{other.start_time}, end_time{other.end_time} {};
    CourseData(CourseData&& other) : course_name{std::move(other.course_name)}, teacher{std::move(other.teacher)}, crn{other.crn}, start_time{std::move(other.start_time)}, end_time{std::move(other.end_time)} {};
    CourseData& operator=(const CourseData& o) {
        this->course_name = o.course_name;
        this->teacher = o.teacher;
        this->crn = o.crn;
        this->start_time = o.start_time;
        this->end_time = o.end_time;
        return *this;
    }
    ~CourseData() {

    }
};


struct Time {
    int hour;
    int minute;
    std::string time_of_day;

};

Time string_to_time(const std::string& time_string) {
    std::string hour = "";
    int i = 0;
    for(;time_string[i] != ':'; i++) {
        hour += time_string[i];
    }
    i++;
    std::string minute = "";

    for(;time_string[i] != ' '; i++) {
        minute += time_string[i];
    }

    std::string time_of_day = "";
    for(;i < time_string.size(); i++) {
        time_of_day += time_string[i];
    }

    return { .hour = std::stoi(hour), .minute = std::stoi(minute), .time_of_day = time_of_day };


}



void print_queries(const std::vector<CourseData>& vec) {
    std::cout << "{\n";
    for(const auto& c : vec) {
        std::cout << c.course_name << " " << c.teacher.first_name << " " << c.teacher.last_name << " " << c.start_time << "-" << c.end_time << std::endl;
    }
    std::cout << "}";
}

bool valid_times_helper(const CourseData& a, const CourseData& b) {
    Time as = string_to_time(a.start_time);
    Time ae = string_to_time(a.end_time);
    Time bs = string_to_time(b.start_time);
    Time be = string_to_time(b.end_time);
    //check if a start time is within b start and end time
    if(as.hour >= bs.hour && as.hour <= be.hour) {
        //check time of day
        if(as.time_of_day == bs.time_of_day || as.time_of_day == be.time_of_day)
            return false;
    }
    if(ae.hour >= bs.hour && ae.hour <= be.hour) {
          if(ae.time_of_day == bs.time_of_day || ae.time_of_day == be.time_of_day)
            return false;

    }

    return true;

}

bool validate_times(const CourseData& looking_at, const std::vector<CourseData>& already_added) {
    for(int i = 0; i < already_added.size(); i++) {
        if(!valid_times_helper(looking_at, already_added[i])) return false;
    }
    return true;

}

void permute_filtered_courses(const std::vector<CourseData>& filtered_pool,
                              std::unordered_map<std::string, bool>& vis,
                              std::vector<CourseData> curr,
                              std::vector<std::vector<CourseData>>& ret,
                              const int query_size,
                              int index = 0) {
                                if(index > query_size) return;
                                if(curr.size() >= query_size) {
                                    ret.push_back(curr);
                                    return;
                                }

                               for(int i = 0; i < filtered_pool.size(); i++) {
                                    if(vis[filtered_pool[i].course_name]) continue;
                                    if(!validate_times(filtered_pool[i], curr)) continue;
                                    vis[filtered_pool[i].course_name] = true;
                                    curr.push_back(filtered_pool[i]);
                                    permute_filtered_courses(filtered_pool, vis, curr, ret, query_size, index + 1);
                                    curr.pop_back();
                                    vis[filtered_pool[i].course_name] = false;
                                }

};


int main(int argc, char** argv)
{   CourseData cop_dummy1{"COP", .teacher = {.first_name = "Tim", .last_name = "Bob"}, 1, "12:30 PM", "01:45 PM"};
    CourseData cop_dummy2{"COP", .teacher = {.first_name = "Jim", .last_name = "Sob"}, 1, "02:00 PM", "03:40 PM"};
    CourseData cot_dummy1{"COT", .teacher = {.first_name = "Lim", .last_name = "Nom"}, 1, "02:05 PM", "03:45 PM"};
    CourseData cot_dummy2{"COT", .teacher = {.first_name = "Stim", .last_name = "Murmer"}, 1, "02:05 AM", "03:45 AM"};

    std::vector<std::string> courses_to_query = {"COT", "COP"};
    std::vector<CourseData> courses = {cop_dummy1, cop_dummy2, cot_dummy1, cot_dummy2};

    std::vector<CourseData> filtered;
    std::copy_if(courses.begin(), courses.end(), std::back_inserter(filtered), [&](auto c) {
                        for(const auto& s : courses_to_query) {
                            if(c.course_name == s) return true;
                        }
                        return false;
            });

    std::unordered_map<std::string, bool> vis;
    std::vector<std::vector<CourseData>> possibilities;

    permute_filtered_courses(filtered, vis, {}, possibilities, courses_to_query.size(), 0);

    for(const auto& p : possibilities) {
        print_queries(p);
    }









    return 0;
}
