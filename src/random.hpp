#pragma once

#include "singleton.hpp"

#include <glm/glm.hpp>

#include <random>

// TODO: compile time precomputed random numbers

namespace inferno::helpers {

class Random : public Singleton<Random>
{
public:
    inline Random()
        : mGenerator()
    {

    }

    inline ~Random()
    {

    }

    inline float rand(float l, float u)
    {
        static std::uniform_real_distribution<float> distribution(l, u);
        return distribution(mGenerator);
    }

    inline float rand01() {
        static std::uniform_real_distribution<float> distribution(0, 1);
        return distribution(mGenerator);
    }

    inline float rand11() {
        static std::uniform_real_distribution<float> distribution(-1, 1);
        return distribution(mGenerator);
    }

    inline glm::vec3 randomUnitVector() 
    {
        static std::uniform_real_distribution<float> distribution(-1, 1);

        float x = distribution(mGenerator);
        float y = distribution(mGenerator);
        float z = distribution(mGenerator);

        return glm::normalize(glm::vec3({x,y,z}));
    }

private:
    std::default_random_engine mGenerator;
};

}
