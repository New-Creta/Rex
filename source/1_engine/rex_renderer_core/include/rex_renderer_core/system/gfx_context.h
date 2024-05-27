#pragma once

// The graphics context acts as the interface for renderers to speak with the graphics api
// eg. for DirectX it holds the following objects
// The DirectX Device
//   This is the objects needed to create gpu resources
// The Graphics Engine
//   This is reponsible for creating various types of sub engines (eg. copy engine, render engine, compute engine)
//   
// The Compute Engine
//  
// The Commandlist Manager 
//   This object is responsible of creating command lists
//   Pooling them when they're no longer used
//   It holds an internal command allocator pool to make sure
//   Command lists get assigned an allocator that's not currently used
//  
