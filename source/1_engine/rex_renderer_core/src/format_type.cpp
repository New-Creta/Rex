#include "rex_renderer_core/format_type.h"

namespace rex
{
    //-------------------------------------------------------------------------
    Format::Format()
        :m_type(FormatType::UNKNOWN)
    {}
    //-------------------------------------------------------------------------
    Format::Format(FormatType type)
        :m_type(type)
    {}

    //-------------------------------------------------------------------------
    FormatType Format::as_format_type() const 
    { 
        return m_type; 
    }
}

//-------------------------------------------------------------------------
rsl::ostream& operator<<(rsl::ostream& os, const rex::FormatType type)
{
    switch (type)
    {
    case rex::FormatType::UNKNOWN:									os << "UNKNOWN"; break;						
    case rex::FormatType::R32G32B32A32_TYPLESS:						os << "R32G32B32A32_TYPLESS"; break;			
    case rex::FormatType::R32G32B32A32_FLOAT:						os << "R32G32B32A32_FLOAT"; break;			
    case rex::FormatType::R32G32B32A32_UINT:						os << "R32G32B32A32_UINT"; break;			
    case rex::FormatType::R32G32B32A32_SINT:						os << "R32G32B32A32_SINT"; break;			
    case rex::FormatType::R32G32B32_TYPELESS:						os << "R32G32B32_TYPELESS"; break;			
    case rex::FormatType::R32G32B32_FLOAT:							os << "R32G32B32_FLOAT"; break;				
    case rex::FormatType::R32G32B32_UINT:							os << "R32G32B32_UINT"; break;				
    case rex::FormatType::R32G32B32_SINT:							os << "R32G32B32_SINT"; break;				
    case rex::FormatType::R16G16B16A16_TYPELESS:					os << "R16G16B16A16_TYPELESS"; break;		
    case rex::FormatType::R16G16B16A16_FLOAT:						os << "R16G16B16A16_FLOAT"; break;			
    case rex::FormatType::R16G16B16A16_UNORM:						os << "R16G16B16A16_UNORM"; break;			
    case rex::FormatType::R16G16B16A16_UINT:						os << "R16G16B16A16_UINT"; break;			
    case rex::FormatType::R16G16B16A16_SNORM:						os << "R16G16B16A16_SNORM"; break;			
    case rex::FormatType::R16G16B16A16_SINT:						os << "R16G16B16A16_SINT"; break;			
    case rex::FormatType::R32G32_TYPELESS:							os << "R32G32_TYPELESS"; break;				
    case rex::FormatType::R32G32_FLOAT:								os << "R32G32_FLOAT"; break;					
    case rex::FormatType::R32G32_UINT:								os << "R32G32_UINT"; break;					
    case rex::FormatType::R32G32_SINT:								os << "R32G32_SINT"; break;					
    case rex::FormatType::R32G8X24_TYPELESS:						os << "R32G8X24_TYPELESS"; break;			
    case rex::FormatType::D32_FLOAT_S8X24_UINT:						os << "D32_FLOAT_S8X24_UINT"; break;			
    case rex::FormatType::R32_FLOAT_X8X24_TYPELESS:					os << "R32_FLOAT_X8X24_TYPELESS"; break;		
    case rex::FormatType::X32_TYPELESS_G8X24_UINT:					os << "X32_TYPELESS_G8X24_UINT"; break;		
    case rex::FormatType::R10G10B10A2_TYPELESS:						os << "R10G10B10A2_TYPELESS"; break;			
    case rex::FormatType::R10G10B10A2_UNORM:						os << "R10G10B10A2_UNORM"; break;			
    case rex::FormatType::R10G10B10A2_UINT:							os << "R10G10B10A2_UINT"; break;				
    case rex::FormatType::R11G11B10_FLOAT:							os << "R11G11B10_FLOAT"; break;				
    case rex::FormatType::R8G8B8A8_TYPELESS:						os << "R8G8B8A8_TYPELESS"; break;			
    case rex::FormatType::R8G8B8A8_UNORM:							os << "R8G8B8A8_UNORM"; break;				
    case rex::FormatType::R8G8B8A8_UNORM_SRGB:						os << "R8G8B8A8_UNORM_SRGB"; break;			
    case rex::FormatType::R8G8B8A8_UINT:							os << "R8G8B8A8_UINT"; break;				
    case rex::FormatType::R8G8B8A8_SNORM:							os << "R8G8B8A8_SNORM"; break;				
    case rex::FormatType::R8G8B8A8_SINT:							os << "R8G8B8A8_SINT"; break;				
    case rex::FormatType::R16G16_TYPELESS:							os << "R16G16_TYPELESS"; break;				
    case rex::FormatType::R16G16_FLOAT:								os << "R16G16_FLOAT"; break;					
    case rex::FormatType::R16G16_UNORM:								os << "R16G16_UNORM"; break;					
    case rex::FormatType::R16G16_UINT:								os << "R16G16_UINT"; break;					
    case rex::FormatType::R16G16_SNORM:								os << "R16G16_SNORM"; break;					
    case rex::FormatType::R16G16_SINT:								os << "R16G16_SINT"; break;					
    case rex::FormatType::R32_TYPELESS:								os << "R32_TYPELESS"; break;					
    case rex::FormatType::D32_FLOAT:								os << "D32_FLOAT"; break;					
    case rex::FormatType::R32_FLOAT:								os << "R32_FLOAT"; break;					
    case rex::FormatType::R32_UINT:									os << "R32_UINT"; break;						
    case rex::FormatType::R32_SINT:									os << "R32_SINT"; break;						
    case rex::FormatType::R24G8_TYPELESS:							os << "R24G8_TYPELESS"; break;				
    case rex::FormatType::D24_UNORM_S8_UINT:						os << "D24_UNORM_S8_UINT"; break;			
    case rex::FormatType::R24_UNORM_X8_TYPELESS:					os << "R24_UNORM_X8_TYPELESS"; break;		
    case rex::FormatType::X24_TYPELESS_G8_UINT:						os << "X24_TYPELESS_G8_UINT"; break;			
    case rex::FormatType::R8G8_TYPELESS:							os << "R8G8_TYPELESS"; break;				
    case rex::FormatType::R8G8_UNORM:								os << "R8G8_UNORM"; break;					
    case rex::FormatType::R8G8_UINT:								os << "R8G8_UINT"; break;					
    case rex::FormatType::R8G8_SNORM:								os << "R8G8_SNORM"; break;					
    case rex::FormatType::R8G8_SINT:								os << "R8G8_SINT"; break;					
    case rex::FormatType::R16_TYPELESS:								os << "R16_TYPELESS"; break;					
    case rex::FormatType::R16_FLOAT:								os << "R16_FLOAT"; break;					
    case rex::FormatType::D16_UNORM:								os << "D16_UNORM"; break;					
    case rex::FormatType::R16_UNORM:								os << "R16_UNORM"; break;					
    case rex::FormatType::R16_UINT:									os << "R16_UINT"; break;						
    case rex::FormatType::R16_SNORM:								os << "R16_SNORM"; break;					
    case rex::FormatType::R16_SINT:									os << "R16_SINT"; break;						
    case rex::FormatType::R8_TYPELESS:								os << "R8_TYPELESS"; break;					
    case rex::FormatType::R8_UNORM:									os << "R8_UNORM"; break;						
    case rex::FormatType::R8_UINT:									os << "R8_UINT"; break;						
    case rex::FormatType::R8_SNORM:									os << "R8_SNORM"; break;						
    case rex::FormatType::R8_SINT:									os << "R8_SINT"; break;						
    case rex::FormatType::A8_UNORM:									os << "A8_UNORM"; break;						
    case rex::FormatType::R1_UNORM:									os << "R1_UNORM"; break;						
    case rex::FormatType::R9G9B9E5_SHAREDEXP:						os << "R9G9B9E5_SHAREDEXP"; break;			
    case rex::FormatType::R8G8_B8G8_UNORM:							os << "R8G8_B8G8_UNORM"; break;				
    case rex::FormatType::G8R8_G8B8_UNORM:							os << "G8R8_G8B8_UNORM"; break;				
    case rex::FormatType::BC1_TYPELESS:								os << "BC1_TYPELESS"; break;					
    case rex::FormatType::BC1_UNORM:								os << "BC1_UNORM"; break;					
    case rex::FormatType::BC1_UNORM_SRGB:							os << "BC1_UNORM_SRGB"; break;				
    case rex::FormatType::BC2_TYPELESS:								os << "BC2_TYPELESS"; break;					
    case rex::FormatType::BC2_UNORM:								os << "BC2_UNORM"; break;					
    case rex::FormatType::BC2_UNORM_SRGB:							os << "BC2_UNORM_SRGB"; break;				
    case rex::FormatType::BC3_TYPELESS:								os << "BC3_TYPELESS"; break;					
    case rex::FormatType::BC3_UNORM:								os << "BC3_UNORM"; break;					
    case rex::FormatType::BC3_UNORM_SRGB:							os << "BC3_UNORM_SRGB"; break;				
    case rex::FormatType::BC4_TYPELESS:								os << "BC4_TYPELESS"; break;					
    case rex::FormatType::BC4_UNORM:								os << "BC4_UNORM"; break;					
    case rex::FormatType::BC4_SNORM:								os << "BC4_SNORM"; break;					
    case rex::FormatType::BC5_TYPELESS:								os << "BC5_TYPELESS"; break;					
    case rex::FormatType::BC5_UNORM:								os << "BC5_UNORM"; break;					
    case rex::FormatType::BC5_SNORM:								os << "BC5_SNORM"; break;					
    case rex::FormatType::B5G6R5_UNORM:								os << "B5G6R5_UNORM"; break;					
    case rex::FormatType::B5G5R5A1_UNORM:							os << "B5G5R5A1_UNORM"; break;				
    case rex::FormatType::B8G8R8A8_UNORM:							os << "B8G8R8A8_UNORM"; break;				
    case rex::FormatType::B8G8R8X8_UNORM:							os << "B8G8R8X8_UNORM"; break;				
    case rex::FormatType::R10G10B10_XR_BIAS_A2_UNORM:				os << "R10G10B10_XR_BIAS_A2_UNORM"; break;	
    case rex::FormatType::B8G8R8A8_TYPELESS:						os << "B8G8R8A8_TYPELESS"; break;			
    case rex::FormatType::B8G8R8A8_UNORM_SRGB:						os << "B8G8R8A8_UNORM_SRGB"; break;			
    case rex::FormatType::B8G8R8X8_TYPELESS:						os << "B8G8R8X8_TYPELESS"; break;			
    case rex::FormatType::B8G8R8X8_UNORM_SRGB:						os << "B8G8R8X8_UNORM_SRGB"; break;			
    case rex::FormatType::B4G4R4A4_UNORM:							os << "B4G4R4A4_UNORM"; break;				
    }

    return os;
}