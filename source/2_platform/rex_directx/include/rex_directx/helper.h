#pragma once

#include <string>

#include <d3d12.h>

namespace rex
{
    class DirectXException
    {
    public:
        DirectXException();
        DirectXException(HRESULT hr, const std::string& functionName, const std::string& filename, int lineNumber);

        std::string     to_string()const;

        HRESULT         m_error_code = S_OK;
        std::string     m_function_name;
        std::string     m_file_name;
        int             m_line_number = -1;
    };
}

//------------------------------------------------------------------------------------------------
struct CD3DX12_DEFAULT {};

extern const DECLSPEC_SELECTANY CD3DX12_DEFAULT D3D12_DEFAULT;

//------------------------------------------------------------------------------------------------
inline bool operator==(const D3D12_VIEWPORT& l, const D3D12_VIEWPORT& r)
{
    return l.TopLeftX == r.TopLeftX 
        && l.TopLeftY == r.TopLeftY 
        && l.Width == r.Width 
        && l.Height == r.Height 
        && l.MinDepth == r.MinDepth 
        && l.MaxDepth == r.MaxDepth;
}
//------------------------------------------------------------------------------------------------
inline bool operator!=(const D3D12_VIEWPORT& l, const D3D12_VIEWPORT& r)
{
    return !(l == r);
}

//------------------------------------------------------------------------------------------------
struct CD3DX12_HEAP_PROPERTIES : public D3D12_HEAP_PROPERTIES
{
    //------------------------------------------------------------------------------------------------
    CD3DX12_HEAP_PROPERTIES()
    {}
    //------------------------------------------------------------------------------------------------
    explicit CD3DX12_HEAP_PROPERTIES(const D3D12_HEAP_PROPERTIES& o) :
        D3D12_HEAP_PROPERTIES(o)
    {}

    //------------------------------------------------------------------------------------------------
    CD3DX12_HEAP_PROPERTIES(
        D3D12_CPU_PAGE_PROPERTY cpuPageProperty,
        D3D12_MEMORY_POOL memoryPoolPreference,
        UINT creationNodeMask = 1,
        UINT nodeMask = 1)
    {
        Type = D3D12_HEAP_TYPE_CUSTOM;
        CPUPageProperty = cpuPageProperty;
        MemoryPoolPreference = memoryPoolPreference;
        CreationNodeMask = creationNodeMask;
        VisibleNodeMask = nodeMask;
    }
    //------------------------------------------------------------------------------------------------
    explicit CD3DX12_HEAP_PROPERTIES(
        D3D12_HEAP_TYPE type,
        UINT creationNodeMask = 1,
        UINT nodeMask = 1)
    {
        Type = type;
        CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        CreationNodeMask = creationNodeMask;
        VisibleNodeMask = nodeMask;
    }
    //------------------------------------------------------------------------------------------------
    operator const D3D12_HEAP_PROPERTIES& () const
    {
        return *this; 
    }

    //------------------------------------------------------------------------------------------------
    bool IsCPUAccessible() const
    {
        return Type == D3D12_HEAP_TYPE_UPLOAD || Type == D3D12_HEAP_TYPE_READBACK || (Type == D3D12_HEAP_TYPE_CUSTOM &&
            (CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_COMBINE || CPUPageProperty == D3D12_CPU_PAGE_PROPERTY_WRITE_BACK));
    }
};

//------------------------------------------------------------------------------------------------
inline bool operator==(const D3D12_HEAP_PROPERTIES& l, const D3D12_HEAP_PROPERTIES& r)
{
    return l.Type == r.Type && l.CPUPageProperty == r.CPUPageProperty &&
        l.MemoryPoolPreference == r.MemoryPoolPreference &&
        l.CreationNodeMask == r.CreationNodeMask &&
        l.VisibleNodeMask == r.VisibleNodeMask;
}
//------------------------------------------------------------------------------------------------
inline bool operator!=(const D3D12_HEAP_PROPERTIES& l, const D3D12_HEAP_PROPERTIES& r)
{
    return !(l == r);
}

//------------------------------------------------------------------------------------------------
struct CD3DX12_CPU_DESCRIPTOR_HANDLE : public D3D12_CPU_DESCRIPTOR_HANDLE
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE() 
    {}

    explicit CD3DX12_CPU_DESCRIPTOR_HANDLE(const D3D12_CPU_DESCRIPTOR_HANDLE& o) 
        :D3D12_CPU_DESCRIPTOR_HANDLE(o)
    {}

    CD3DX12_CPU_DESCRIPTOR_HANDLE(CD3DX12_DEFAULT) 
    {
        ptr = 0; 
    }

    CD3DX12_CPU_DESCRIPTOR_HANDLE(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& other, INT offsetScaledByIncrementSize)
    {
        InitOffsetted(other, offsetScaledByIncrementSize);
    }
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& other, INT offsetInDescriptors, UINT descriptorIncrementSize)
    {
        InitOffsetted(other, offsetInDescriptors, descriptorIncrementSize);
    }
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE& Offset(INT offsetInDescriptors, UINT descriptorIncrementSize)
    {
        ptr += offsetInDescriptors * descriptorIncrementSize;
        return *this;
    }
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE& Offset(INT offsetScaledByIncrementSize)
    {
        ptr += offsetScaledByIncrementSize;
        return *this;
    }
    
    bool operator==(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& other)
    {
        return (ptr == other.ptr);
    }
    
    bool operator!=(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& other)
    {
        return (ptr != other.ptr);
    }
    
    CD3DX12_CPU_DESCRIPTOR_HANDLE& operator=(const D3D12_CPU_DESCRIPTOR_HANDLE& other)
    {
        ptr = other.ptr;
        return *this;
    }

    inline void InitOffsetted(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& base, INT offsetScaledByIncrementSize)
    {
        InitOffsetted(*this, base, offsetScaledByIncrementSize);
    }

    inline void InitOffsetted(_In_ const D3D12_CPU_DESCRIPTOR_HANDLE& base, INT offsetInDescriptors, UINT descriptorIncrementSize)
    {
        InitOffsetted(*this, base, offsetInDescriptors, descriptorIncrementSize);
    }

    static inline void InitOffsetted(_Out_ D3D12_CPU_DESCRIPTOR_HANDLE& handle, _In_ const D3D12_CPU_DESCRIPTOR_HANDLE& base, INT offsetScaledByIncrementSize)
    {
        handle.ptr = base.ptr + offsetScaledByIncrementSize;
    }

    static inline void InitOffsetted(_Out_ D3D12_CPU_DESCRIPTOR_HANDLE& handle, _In_ const D3D12_CPU_DESCRIPTOR_HANDLE& base, INT offsetInDescriptors, UINT descriptorIncrementSize)
    {
        handle.ptr = base.ptr + offsetInDescriptors * descriptorIncrementSize;
    }
};
//------------------------------------------------------------------------------------------------
struct CD3DX12_GPU_DESCRIPTOR_HANDLE : public D3D12_GPU_DESCRIPTOR_HANDLE
{
    CD3DX12_GPU_DESCRIPTOR_HANDLE()
    {}

    explicit CD3DX12_GPU_DESCRIPTOR_HANDLE(const D3D12_GPU_DESCRIPTOR_HANDLE& o) 
        :D3D12_GPU_DESCRIPTOR_HANDLE(o)
    {}

    CD3DX12_GPU_DESCRIPTOR_HANDLE(CD3DX12_DEFAULT) 
    {
        ptr = 0; 
    }

    CD3DX12_GPU_DESCRIPTOR_HANDLE(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE& other, INT offsetScaledByIncrementSize)
    {
        InitOffsetted(other, offsetScaledByIncrementSize);
    }
    
    CD3DX12_GPU_DESCRIPTOR_HANDLE(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE& other, INT offsetInDescriptors, UINT descriptorIncrementSize)
    {
        InitOffsetted(other, offsetInDescriptors, descriptorIncrementSize);
    }
    
    CD3DX12_GPU_DESCRIPTOR_HANDLE& Offset(INT offsetInDescriptors, UINT descriptorIncrementSize)
    {
        ptr += offsetInDescriptors * descriptorIncrementSize;
        return *this;
    }
    
    CD3DX12_GPU_DESCRIPTOR_HANDLE& Offset(INT offsetScaledByIncrementSize)
    {
        ptr += offsetScaledByIncrementSize;
        return *this;
    }
    
    inline bool operator==(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE& other)
    {
        return (ptr == other.ptr);
    }
    
    inline bool operator!=(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE& other)
    {
        return (ptr != other.ptr);
    }
    
    CD3DX12_GPU_DESCRIPTOR_HANDLE& operator=(const D3D12_GPU_DESCRIPTOR_HANDLE& other)
    {
        ptr = other.ptr;
        return *this;
    }

    inline void InitOffsetted(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE& base, INT offsetScaledByIncrementSize)
    {
        InitOffsetted(*this, base, offsetScaledByIncrementSize);
    }

    inline void InitOffsetted(_In_ const D3D12_GPU_DESCRIPTOR_HANDLE& base, INT offsetInDescriptors, UINT descriptorIncrementSize)
    {
        InitOffsetted(*this, base, offsetInDescriptors, descriptorIncrementSize);
    }

    static inline void InitOffsetted(_Out_ D3D12_GPU_DESCRIPTOR_HANDLE& handle, _In_ const D3D12_GPU_DESCRIPTOR_HANDLE& base, INT offsetScaledByIncrementSize)
    {
        handle.ptr = base.ptr + offsetScaledByIncrementSize;
    }

    static inline void InitOffsetted(_Out_ D3D12_GPU_DESCRIPTOR_HANDLE& handle, _In_ const D3D12_GPU_DESCRIPTOR_HANDLE& base, INT offsetInDescriptors, UINT descriptorIncrementSize)
    {
        handle.ptr = base.ptr + offsetInDescriptors * descriptorIncrementSize;
    }
};

//------------------------------------------------------------------------------------------------
struct CD3DX12_RESOURCE_BARRIER : public D3D12_RESOURCE_BARRIER
{
    //------------------------------------------------------------------------------------------------
    CD3DX12_RESOURCE_BARRIER()
    {}
    //------------------------------------------------------------------------------------------------
    explicit CD3DX12_RESOURCE_BARRIER(const D3D12_RESOURCE_BARRIER& o) 
        :D3D12_RESOURCE_BARRIER(o)
    {}

    //------------------------------------------------------------------------------------------------
    static inline CD3DX12_RESOURCE_BARRIER Transition(
        _In_ ID3D12Resource* pResource,
        D3D12_RESOURCE_STATES stateBefore,
        D3D12_RESOURCE_STATES stateAfter,
        UINT subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES,
        D3D12_RESOURCE_BARRIER_FLAGS flags = D3D12_RESOURCE_BARRIER_FLAG_NONE)
    {
        CD3DX12_RESOURCE_BARRIER result;
        ZeroMemory(&result, sizeof(result));
        D3D12_RESOURCE_BARRIER& barrier = result;
        result.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        result.Flags = flags;
        barrier.Transition.pResource = pResource;
        barrier.Transition.StateBefore = stateBefore;
        barrier.Transition.StateAfter = stateAfter;
        barrier.Transition.Subresource = subresource;
        return result;
    }
    //------------------------------------------------------------------------------------------------
    static inline CD3DX12_RESOURCE_BARRIER Aliasing(
        _In_ ID3D12Resource* pResourceBefore,
        _In_ ID3D12Resource* pResourceAfter)
    {
        CD3DX12_RESOURCE_BARRIER result;
        ZeroMemory(&result, sizeof(result));
        D3D12_RESOURCE_BARRIER& barrier = result;
        result.Type = D3D12_RESOURCE_BARRIER_TYPE_ALIASING;
        barrier.Aliasing.pResourceBefore = pResourceBefore;
        barrier.Aliasing.pResourceAfter = pResourceAfter;
        return result;
    }
    //------------------------------------------------------------------------------------------------
    static inline CD3DX12_RESOURCE_BARRIER UAV(
        _In_ ID3D12Resource* pResource)
    {
        CD3DX12_RESOURCE_BARRIER result;
        ZeroMemory(&result, sizeof(result));
        D3D12_RESOURCE_BARRIER& barrier = result;
        result.Type = D3D12_RESOURCE_BARRIER_TYPE_UAV;
        barrier.UAV.pResource = pResource;
        return result;
    }

    //------------------------------------------------------------------------------------------------
    operator const D3D12_RESOURCE_BARRIER& () const 
    {
        return *this; 
    }
};

#ifndef DX_CALL
#define DX_CALL(x)                                                                  \
{                                                                                   \
    HRESULT hr__ = (x);                                                             \
    std::string fn = __FILE__;                                                      \
    if(FAILED(hr__)) { throw rex::DirectXException(hr__, #x, fn, __LINE__); }       \
}
#endif

#ifndef RELEASE_COM
#define RELEASE_COM(x) { if(x){ x->Release(); x = 0; } }
#endif