#pragma once

namespace core::HART {

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

