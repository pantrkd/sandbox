#include <algorithm>
#include <cstdint>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>


struct Teacher {
  std::string first_name;
  std::string last_name;
};

struct CourseData {
  std::string course_name;
  Teacher teacher;
  std::uint8_t crn;
  Time24Hour start_time;
  Time24Hour end_time;

  CourseData() = default;
  CourseData(const std::string &cname, Teacher teach, std::uint8_t crn,
             Time24Hour start_time, Time24Hour end_time)
      : course_name{cname}, teacher{teach}, crn{crn}, start_time{start_time},
        end_time{end_time} {};
  CourseData(const CourseData &other)
      : course_name{other.course_name}, teacher{other.teacher}, crn{other.crn},
        start_time{other.start_time}, end_time{other.end_time} {};
  CourseData(CourseData &&other)
      : course_name{std::move(other.course_name)},
        teacher{std::move(other.teacher)}, crn{other.crn},
        start_time{std::move(other.start_time)},
        end_time{std::move(other.end_time)} {};
  CourseData &operator=(const CourseData &o) {
    this->course_name = o.course_name;
    this->teacher = o.teacher;
    this->crn = o.crn;
    this->start_time = o.start_time;
    this->end_time = o.end_time;
    return *this;
  }
  ~CourseData() {}
};

/** Stores the time in 24 hour standard */
struct Time24Hour {
  int hour;
  int minute;
};

/** Support << streaming for the Time24Hour struct */
std::ostream &operator<<(std::ostream &os, const Time24Hour &t) {
  if (t.hour < 10)
    os << "0";
  os << t.hour << ":";

  if (t.minute < 10)
    os << "0";
  os << t.minute;

  return os;
}

void print_queries(const std::vector<CourseData> &vec) {
  std::cout << "{\n";
  for (const auto &c : vec) {
    std::cout << c.course_name << " " << c.teacher.first_name << " "
              << c.teacher.last_name << " " << c.start_time << "-" << c.end_time
              << std::endl;
  }
  std::cout << "}";
}

/** 
 * Compares a and b to determine whether the times overlap
 * @returns true if no conflict, else false
 */
bool valid_times_helper(const CourseData &a, const CourseData &b) {
  // check if a start time is within b start and end time
  if (a.start_time.hour >= b.start_time.hour &&
      a.start_time.hour <= b.start_time.hour) {
    return false;
  }
  if (a.end_time.hour >= b.start_time.hour &&
      a.end_time.hour <= b.end_time.hour) {
    return false;
  }

  return true;
}

bool validate_times(const CourseData &looking_at,
                    const std::vector<CourseData> &already_added) {
  for (int i = 0; i < already_added.size(); i++) {
    if (!valid_times_helper(looking_at, already_added[i]))
      return false;
  }
  return true;
}

void permute_filtered_courses(const std::vector<CourseData> &filtered_pool,
                              std::unordered_map<std::string, bool> &vis,
                              std::vector<CourseData> curr,
                              std::vector<std::vector<CourseData>> &ret,
                              const int query_size, int index = 0) {
  if (index > query_size)
    return;
  if (curr.size() >= query_size) {
    ret.push_back(curr);
    return;
  }

  for (int i = 0; i < filtered_pool.size(); i++) {
    if (vis[filtered_pool[i].course_name])
      continue;
    if (!validate_times(filtered_pool[i], curr))
      continue;
    vis[filtered_pool[i].course_name] = true;
    curr.push_back(filtered_pool[i]);
    permute_filtered_courses(filtered_pool, vis, curr, ret, query_size,
                             index + 1);
    curr.pop_back();
    vis[filtered_pool[i].course_name] = false;
  }
};

int main(int argc, char **argv) {
  CourseData cop_dummy1{"COP", Teacher{"Tim", "Bob"}, 1, Time24Hour{12, 30},
                        Time24Hour{13, 45}};
  CourseData cop_dummy2{"COP", Teacher{"Jim", "Sob"}, 1, Time24Hour{14, 0},
                        Time24Hour{15, 40}};
  CourseData cot_dummy1{"COT", Teacher{"Lim", "Nom"}, 1, Time24Hour{14, 5},
                        Time24Hour{15, 45}};
  CourseData cot_dummy2{"COT", Teacher{"Stim", "Murmer"}, 1, Time24Hour{2, 5},
                        Time24Hour{3, 45}};

  std::vector<std::string> courses_to_query = {"COT", "COP"};
  std::vector<CourseData> courses = {cop_dummy1, cop_dummy2, cot_dummy1,
                                     cot_dummy2};

  std::vector<CourseData> filtered;
  std::copy_if(courses.begin(), courses.end(), std::back_inserter(filtered),
               [&](auto c) {
                 for (const auto &s : courses_to_query) {
                   if (c.course_name == s)
                     return true;
                 }
                 return false;
               });

  std::unordered_map<std::string, bool> vis;
  std::vector<std::vector<CourseData>> possibilities;

  permute_filtered_courses(filtered, vis, {}, possibilities,
                           courses_to_query.size(), 0);

  for (const auto &p : possibilities) {
    print_queries(p);
  }

  return 0;
}
