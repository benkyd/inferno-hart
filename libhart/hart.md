# inferno HART modules

Modules are registered at load time - instantiated when selected
_GET, _DESTROY & _CREDIT must be defined and return valid context's
 
Inferno will first initialise the module and then wait for the Ready state.
 
The possible states a module can be in are:
 - Bad (Not ready)
 - Idle (Ready for rays)
 - Build (Scene is submitted and being processed)
 - Trace (Tracing!)
 
Once the HHM dispatches a new scene to the module, it will wait until
the state is Done to dispatch work during scene building the modules
state must be Build.
 
Once the scene is ready and so is the trace, the HHM will start the tracing
state by calling the start function of the module, the module must go
through the rays added to it before start was called and then
for each ray, call Hit and pass the current context, this may result
in Inferno to push another ray to the queue, the module will go until
empty or signaled to stop.
 
Once empty the module will switch to the Ready state again, so Inferno
can get the next frame ready and repeat.
 
The HART Module also has the option to
 
