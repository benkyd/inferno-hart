#include "pipeline.hpp"

#include "device.hpp"
#include "swapchain.hpp"

namespace inferno::graphics {

Pipeline* pipeline_create(GraphicsDevice* device)
{
    Pipeline* pipeline = new Pipeline();

    pipeline->Device = device;
    pipeline->Swap = swapchain_create(device, device->SurfaceSize);

    return pipeline;
}

void pipeline_cleanup(Pipeline* pipeline)
{
    delete pipeline;
}

} // namespace inferno::graphics
