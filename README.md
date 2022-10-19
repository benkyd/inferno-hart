![](img/inferno-hart-high-resolution-logo-color-on-transparent-background.png)

Inferno HART (Hardware Accelerated RayTracing) will revolutionise the way we write application-specific raytracing applications with a clear and distinct abstraction between the core renderer, Inferno and the HART layer.

Seperating Inferno and HART allows us to create first an ART (Accelerated RayTracing) layer which handles both the tracing and the BRDF, as well as the HART which can implement an accelerator for the tracing. Allowing for custom RayTracing silicon to be utilised WITHOUT changing the code code, nor the Global Illumination code.

This combined with it's full functionality through Julia makes Inferno HART a very nice pathtracing engine.

## Features

## Technologies

- OpenGL, utilised with GLAD and GLFW
- ImGui for lovely GUI
- SPDLog for lovely logging

## Acknowledgements
