#include <cstdio>

#include "../ShadowPeople/rendering/PatchGenerator.hpp"

int main(int argc, char** argv)
{
    rendering::PatchCache cache;
    rendering::PatchGenerator generator(cache);
}
