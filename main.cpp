#include <iostream>
#include <string>
#include <vector>
#include "Scene.h"
#include "mat4.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        std::cout << "Please run the rasterizer as:" << std::endl
             << "\t./rasterizer <input_file_name>" << std::endl;
        return EXIT_FAILURE;
    }
    else
    {
        Scene scene(argv[1]);



        return EXIT_SUCCESS;
    }
}