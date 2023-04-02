#include "pic_engine_directx12_pch.h"

#include "dx_helper.h"

#include <comdef.h>

namespace rex
{
     
    //-------------------------------------------------------------------------
    DirectXException::DirectXException()
    {

    }

    //-------------------------------------------------------------------------
    DirectXException::DirectXException(HRESULT hr, const std::string& functionName, const std::string& fileName, int lineNumber)
        :m_error_code(hr)
        ,m_function_name(functionName)
        ,m_file_name(fileName)
        ,m_line_number(lineNumber)
    {

    }

    //-------------------------------------------------------------------------
    std::string DirectXException::to_string() const
    {
        // Get the string description of the error code.
        _com_error err(m_error_code);
        std::string msg = err.ErrorMessage();

        return m_function_name + " failed in " + m_file_name + "; line " + std::to_string(m_line_number) + "; error: " + msg;
    }

}