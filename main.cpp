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

    CourseData() = default;
    CourseData(const std::string& cname, Teacher teach, std::uint8_t crn)
        : course_name{cname}, teacher{teach}, crn{crn} {};
    CourseData(const CourseData& other)
        : course_name{other.course_name}, teacher{other.teacher}, crn{other.crn} {};
    CourseData(CourseData&& other) : course_name{std::move(other.course_name)}, teacher{std::move(other.teacher)}, crn{other.crn} {};
    CourseData& operator=(const CourseData& o) {
        this->course_name = o.course_name;
        this->teacher = o.teacher;
        this->crn = o.crn;
        return *this;
    }
    ~CourseData() {

    }
};



void print_queries(const std::vector<CourseData>& vec) {
    std::cout << "{\n";
    for(const auto& c : vec) {
        std::cout << c.course_name << " " << c.teacher.first_name << " " << c.teacher.last_name << std::endl;
    }
    std::cout << "}";
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
                                    vis[filtered_pool[i].course_name] = true;
                                    curr.push_back(filtered_pool[i]);
                                    permute_filtered_courses(filtered_pool, vis, curr, ret, query_size);
                                    curr.pop_back();
                                    vis[filtered_pool[i].course_name] = false;
                                }

};


int main(int argc, char** argv)
{
    std::vector<std::string> courses_to_query = {"COT", "COP"};
    std::vector<CourseData> courses = {{.course_name = "COP",
                               .teacher = {
                                            .first_name = "Tim",
                                            .last_name = "Bob" },
                               .crn = 1},
                               {.course_name = "COT",
                               .teacher = {
                                            .first_name = "Jim",
                                            .last_name = "Som" },
                               .crn = 2},{.course_name = "COP",
                               .teacher = {
                                            .first_name = "Lem",
                                            .last_name = "Turner" },
                               .crn = 3}};

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
