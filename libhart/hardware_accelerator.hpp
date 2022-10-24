#pragma once

// THIS IS WHAT THE LIBRARY IMPLEMENTS AND PASSES BACK - EVERYTHING ELSE IS JUST DATA
namespace inferno {

enum ACCEL_TYPE
{
    RAY_TRI_ONLY,
};

class Accelerator 
{
public:
    Accelerator();
    virtual void Init();
};

}
