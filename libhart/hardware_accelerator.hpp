#pragma once

// THIS IS WHAT THE LIBRARY IMPLEMENTS AND PASSES BACK - EVERYTHING ELSE IS JUST DATA
namespace inferno {

enum ACCEL_TYPE
{
    ACCEL_TYPE_RAY_TRI_ONLY,
};

class Accelerator 
{
public:
    Accelerator();
    virtual void Init();
};

}
