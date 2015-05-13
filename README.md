# Novus Engine 2

Novus Engine 2 is the successor of Novus Engine rebuilt from the ground up for DX12 support

The primary purpose of **NE2** is to rebuild the base architecture to be more functional and robust using lessons learned from NE1 and create a new rendering system made to take advantage of DX12's multithreading support.

## Building

Novus Engine 2 is built using the Visual Studio 2015 RC at the moment and will require a Windows 10 system to execute since it uses DX12. If it is not too difficult, once I implement the DX12 rendering context I will put in support for a DX11 rendering context and Windows 7/8.1.

## Planned Features

#### Core
* Overall more consistent and usable architecture
* More functional component-entity system
* More robust and comprehensive metadata system leveraging constexpr on actors and components
* Less coupled and more consistent intersystem communication
* Multithreaded rendering system for asynchronous command list generation

#### Graphics
* Overall improvements for authoring and managing shaders with different behavior for different rendering passes (forward, GBuffer, depth pass, voxelization, etc.)
* Voxel Traced Global Illumination leveraging new volumetric tiled resources and hardware conservative rasterization
* Improvements to VXGI to allow more efficient distinction of static and dynamic scene geometry for voxelization
* Physically Based Shading with Forward+ renderer (possibly using clustered shading instead)
* High Dynamic Range rendering

#### Resource Management
* More unified and consistent resource management system that handles textures, shaders, fonts, models, data files, etc.
* Distance field calculation for fonts
* Vertex smoothing and merging for 3D models



