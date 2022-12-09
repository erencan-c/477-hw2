#include "mat4.hpp"

#include <cmath>

int main() {
    auto rot = mat4f::rotation_z(M_PI_4);
    std::cout << rot*vec4f{1,1,1,1};
}
