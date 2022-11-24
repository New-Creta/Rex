// ============================================ 
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: streambuf.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/string/char_traits.h"

namespace rsl
{
    class locale;

//    template <typename CharType, typename Traits = char_traits<CharType>>
//    class basic_streambuf
//    {
//    public:
//        using char_type = CharType;
//        using traits_type = Traits;
//        using int_type = typename traits_type::int_type;
//        using pos_type = typename traits_type::pos_type;
//        using off_type = typename traits_type::off_type;
//
//        virtual ~basic_streambuf();
//
//        rsl::locale pubimbue(const rsl::locale& loc);
//        rsl::locale getloc() const;
//
//        basic_streambuf<CharType, Traits>* pubsetbuf(char_type* s, streamsize n);
//        pos_type pubseekoff(off_type off, rsl::ios_base::Seekdir dir, rsl::ios_base::Openmode which = rsl::ios_base::in | rsl::ios_base::out);
//        pos_type pubseekpos(pos_type pos, rsl::ios_base::Openmode which = rsl::ios_base::in | rsl::ios_base::out);
//        int32 pubsync();
//
//        streamsize in_avail();
//        int_type snextc();
//        int_type sbumpc();
//        int_type sgetc();
//        streamsize sgetn(char_type* s, streamsize count);
//        int_type sputc(char_type ch);
//        streamsize sputn(const char_type* s, streamsize count);
//        int_type sputbackc(char_type c);
//        int_type sungetc();
//
//    protected:
//        basic_streambuf();
//        basic_streambuf(const basic_streambuf& rhs);
//    
//        basic_streambuf& operator=(const basic_streambuf& other);
//    
//        void swap(basic_streambuf& other);
//    
//        virtual void imbue(const rsl::Locale& loc);
//        virtual basic_streambuf<CharType, Traits>* setbuf(char_type* s, streamsize n);
//        virtual pos_type seekoff(off_type off, rsl::ios_base::Seekdir dir, rsl::ios_base::Openmode which = ios_base::in | ios_base::out);
//        virtual pos_type seekpos(pos_type pos, rsl::ios_base::Openmode which = rsl::ios_base::in | rsl::ios_base::out);
//        virtual int32 sync();
//        virtual streamsize xsgetn(char_type* str, streamsize count);
//        virtual streamsize xsputn(const char_type* s, streamsize count);
//        virtual streamsize showmanyc();
//        virtual int_type underflow();
//        virtual int_type overflow(int_type ch = traits_type::eof());
//        virtual int_type uflow();
//        virtual int_type pbackfail(int_type c = traits_type::eof());
//
//        char_type* eback() const;
//        char_type* gptr() const;
//        char_type* egptr() const;
//
//        void gbump(count_t count);
//
//        void setg(char_type* gbeg, char_type* gcurr, char_type* gend);
//
//        char_type* pbase() const;
//        char_type* pptr() const;
//        char_type* epptr() const;
//
//        void pbump(int_type count);
//        void setp(char_type* pbeg, char_type* pend);
//    };
//
//    using streambuf = basic_streambuf<char8>;
//    using wstreambuf = basic_streambuf<tchar>;
//
//#ifdef REX_USE_REX_CODING_GUIDELINES_FOR_RSL
//    template <typename CharType, typename Traits = char_traits<CharType>>
//    using BasicStreambuf = basic_streambuf<CharType, Traits>;
//
//    using Streambuf = BasicStreambuf<char8>;
//    using WStreambuf = BasicStreambuf<tchar>;
//#endif
}