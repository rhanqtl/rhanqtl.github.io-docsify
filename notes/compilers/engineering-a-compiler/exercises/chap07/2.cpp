#include <algorithm>
#include <vector>
#include <string>
#include <iostream>
#include <cstring>

struct Field {
    std::string name;
    unsigned int size;
    unsigned int align;

    Field(const std::string &name, unsigned int size, unsigned int align)
        : name(name), size(size), align(align) {}

    bool operator<(const Field &rhs) {
        const Field &lhs = *this;
        if (lhs.align > rhs.align) {
            return true;
        }
        if (lhs.align < rhs.align) {
            return false;
        }
        if (lhs.size > rhs.size) {
            return true;
        }
        if (lhs.size < rhs.size) {
            return false;
        }
        return lhs.name < rhs.name;
    }
};

void arrange(std::vector<Field> &fields) {
    std::sort(fields.begin(), fields.end());
}

int main() {
    auto fields = std::vector<Field> {
        {"a", 4, 4},
        {"b", 1, 3},
        {"c", 8, 8},
        {"d", 4, 4},
        {"e", 1, 4},
        {"f", 8, 16},
        {"g", 1, 1},
    };
    unsigned int raw_bytes = 0;
    for (auto &f : fields) {
        raw_bytes += f.size;
    }
    std::cout << "raw size in bytes: " << raw_bytes << '\n';
    arrange(fields);
    unsigned int total_bytes = 0;
    for (auto &f : fields) {
        if (total_bytes % f.align != 0) {
            total_bytes += (f.align - total_bytes % f.align);
        }
        total_bytes += f.size;
        std::cout << f.name << ' ';
    }
    std::cout << '\n';
    std::cout << "total size in bytes: " << total_bytes << '\n';
}
