# Novus Engine 2

Novus Engine 2 is an engine used for graphics experimentation that takes advantage of D3D12 features

The primary purpose of **NE2** is to rebuild the base architecture to be more functional and robust using lessons learned from NE1 and create a new rendering system made to take advantage of D3D12's multithreading support.

## Building

Novus Engine 2 is built using the Visual Studio 2015 RC at the moment and will require a Windows 10 system to execute since it uses D3D12 it has last been tested on build 10162. If it is not too time consuming, once I implement the D3D12 rendering context I will put in support for a D3D11 rendering context and Windows 7/8.1.

## Current State

At the moment I'm fairly comfortable with the changes to the basic API stuff on D3D12 from D3D11 and have built the TestSample application to do some basic things that take advantage of asynchronous command buffer generation and bundles. The application can currently do 120,000 plain draw calls in 16 ms (purposely avoiding draw instanced). 
TestSample is pretty simple right now, and attempts to do all of the work in a single frame and then blocks for the frame to finish. It does not really saturate the CPU or GPU with about 30% CPU utilization across all cores and 60% GPU load.
Currently working on improving the abstraction of resources and scheduling.

## Planned Features

#### Core
* Overall more consistent and usable architecture
* More functional component-entity system
* More robust and comprehensive metadata system (hopefully) leveraging constexpr on actors and components
* Less coupled and more consistent inter-system communication
* Multithreaded rendering system for asynchronous command list generation

#### Graphics
* Overall improvements for authoring and managing shaders with different behaviour for different rendering passes (forward, GBuffer, depth pass, voxelization, etc.)
* Voxel Traced Global Illumination using new volumetric tiled resources and hardware conservative rasterization
* Improvements to VXGI to allow more efficient distinction of static and dynamic scene geometry for voxelization
* Tile map distance field accelerated voxel cone tracing to allow for larger scenes, higher resolution voxelization, lower memory footprint, higher performance, and higher quality voxel tracing overall
* Physically Based Shading with Forward+ renderer (possibly using clustered shading instead)
* Measurable High Dynamic Range rendering
* Physically based post-processing effects

#### Resource Management
* More unified and consistent resource management system that handles textures, shaders, fonts, models, data files, etc.
* Distance field calculation for fonts
* Mesh optimization operations
* Custom resource formats for meshes and general scene description
* Asynchronous loading, processing, and staging to the GPU of resources like textures and meshes

#### VR Device Support
* Support for the Rift and Vive
* Support for tracking devices like Lighthouse's wands and Oculus Touch
* Support for full body tracking devices like the Perception Neuron
* Design rendering performance to be capable of 90Hz refresh rate consistently
* Async Timewarp using D3D12

#### GUI
* Capable of operating in 2D screen space or in 3D world space interchangeably
* Easy to interact with using 3D control peripherals (Lighthouse wand, Touch, Perception Neuron, LEAP Motion, etc.)
* Dynamic and fluid transitions
* Material Design-based simplistic layout