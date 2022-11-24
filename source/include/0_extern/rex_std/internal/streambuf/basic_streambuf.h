// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: basic_streambuf.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/io/basic_streambuf
//
// the class basic_streambuf controls input and output to a character sequence.
// the associated character sequence is described by 3 pointers, very similar to
// how rsl::vector is implemented. It is defined by 3 pointers,
// beginning pointer, always pointing to the lowest element of the buffer
// next pointer, points at the element that is the next candidate for reading or writing
// end pointer, points to one past the end of the buffer.
// if the stream is for both reading and writing, then the stream holds 6 pointer
// all of them pointing into the same character array.
//
// Unlike in C++ Standard Library, Rex Standard Library doesn't store or use locales
// so stream buffers don't store locales, making them less complex.
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/internal/algorithm/min.h"
#include "rex_std/internal/assert/assert_fwd.h"
#include "rex_std/internal/ios/ios_base.h"
#include "rex_std/internal/memory/addressof.h"
#include "rex_std/internal/string/char_traits.h"
#include "rex_std/internal/type_traits/is_same.h"
#include "rex_std/internal/utility/swap.h"

namespace rsl
{
  inline namespace v1
  {

    template <typename CharT, typename Traits = char_traits<CharT>>
    class basic_streambuf
    {
    public:
      using char_type   = CharT;
      using traits_type = Traits;
      using int_type    = typename Traits::int_type;
      using pos_type    = typename Traits::pos_type;
      using off_type    = typename Traits::off_type;

      static_assert(rsl::is_same_v<typename Traits::char_type, CharT>, "char type of Traits has to be the same type as the stream's underlying type");

      // destroys the object.
      // this DOESN'T delete any of the member pointers.
      // that should be managed by the derived classes,
      // as they manage setting them.
      virtual ~basic_streambuf() = default;

      /// [09/Sep/2022] RSL Comment: Rex Standard Library doesn't support locales
      // so defining the member functions pubimbue and getloc makes no sense.

      //
      // positioning
      //

      // calls setbuf(s, n) of the most derived class
      basic_streambuf<CharT, Traits>* pubsetbuf(char_type* s, streamsize n)
      {
        setbuf(s, n);
      }

      // calls seekoff(off, dir, which) of the most derived class
      pos_type pubseekoff(off_type off, io::seekdir dir, io::openmode which = io::openmode::in | io::openmode::out)
      {
        seekoff(off, dir, which);
      }

      // calls seekpos(pos, which) of the most derived class
      pos_type pubseekpos(pos_type pos, io::openmode which = io::openmode::in | io::openmode::out)
      {
        seekpos(pos, which);
      }

      // the return value of sync
      int32 pubsync()
      {
        return sync();
      }

      //
      // get area
      //

      // returns the number of characters available in the get area.
      streamsize in_avail()
      {
        streamsize num_available_in_get_area = available_in_get_area();
        if(num_available_in_get_area != 0)
        {
          return num_available_in_get_area;
        }

        return showmanyc();
      }

      // first increases the current read pointer
      // then gets a character and returns it
      int_type snextc()
      {
        int_type ch = sbumpc();
        if(Traits::eq_int_type(ch, Traits::eof())) // no more characters in get area
        {
          return Traits::eof();
        }
        else
        {
          return sgetc();
        }
      }

      // gets a character, then increases the read pointer
      int_type sbumpc()
      {
        if(in_avail() == 0)
        {
          return uflow();
        }

        return Traits::to_int_type(inc_gptr());
      }

      // reads one character from the input sequence.
      // does NOT increase the read pointer
      int_type sgetc()
      {
        if(in_avail() == 0)
        {
          return underflow();
        }

        return Traits::to_int_type(*gptr());
      }

      // calls xsgetn(s, count) of the most derived class
      streamsize sgetn(char_type* s, streamsize count)
      {
        return xsgetn(s, count);
      }

      //
      // put area
      //
      // write one character to the output stream
      int_type sputc(char_type ch)
      {
        if(m_write_buf_curr == nullptr)
        {
          return overflow(ch);
        }

        return Traits::to_int_type(inc_pptr() = ch);
      }

      // calls xsputn(s, count) of the most derived class
      streamsize sputn(const char_type* s, streamsize count)
      {
        return xsputn(s, count);
      }

      //
      // putback
      //

      // puts back a character back to the get area
      int_type sputbackc(char_type c)
      {
        if(gptr() && eback() < gptr() && Traits::eq(c, gptr()[-1]))
        {
          return Traits::to_int_type(dec_gptr());
        }

        return pbackfail(Traits::to_int_type(c));
      }

      // if a putback position is available in the get area, then decrements the next pointer
      // and returns the character it now point to.
      int_type sungetc()
      {
        if(gptr() && eback() < gptr())
        {
          return Traits::to_int_type(dec_gptr());
        }

        return pbackfail();
      }

    protected:
      // constructs the basic_streambuf object
      basic_streambuf()
          : m_read_buf_begin(nullptr)
          , m_read_buf_curr(nullptr)
          , m_read_buf_end(nullptr)
          , m_write_buf_begin(nullptr)
          , m_write_buf_curr(nullptr)
          , m_write_buf_end(nullptr)
      {
      }

      // constructs a copy of rhs
      basic_streambuf(const basic_streambuf& rhs)
          : m_read_buf_begin(rhs.m_read_buf_begin)
          , m_read_buf_curr(rhs.m_read_buf_curr)
          , m_read_buf_end(rhs.m_read_buf_end)
          , m_write_buf_begin(rhs.m_write_buf_begin)
          , m_write_buf_curr(rhs.m_write_buf_curr)
          , m_write_buf_end(rhs.m_write_buf_end)
      {
      }

      basic_streambuf& operator=(const basic_streambuf& other) // NOLINT(bugprone-unhandled-self-assignment)
      {
        REX_ASSERT_X(this != addressof(other), "Can't assign an streambuffer to itself");

        m_read_buf_begin = other.m_read_buf_begin;
        m_read_buf_curr  = other.m_read_buf_curr;
        m_read_buf_end   = other.m_read_buf_end;

        m_write_buf_begin = other.m_write_buf_begin;
        m_write_buf_curr  = other.m_write_buf_curr;
        m_write_buf_end   = other.m_write_buf_end;

        return *this;
      }

      // exchanges the contents of the stream buffer with those of other
      void swap(basic_streambuf& other)
      {
        rsl::swap(m_read_buf_begin, other.m_read_buf_begin);
        rsl::swap(m_read_buf_curr, other.m_read_buf_curr);
        rsl::swap(m_read_buf_end, other.m_read_buf_end);

        rsl::swap(m_write_buf_begin, other.m_write_buf_begin);
        rsl::swap(m_write_buf_curr, other.m_write_buf_curr);
        rsl::swap(m_write_buf_end, other.m_write_buf_end);
      }

      //
      // positioning
      //

      // the base class version of this function has no effect, Derived classes may override this function
      // to allow removal or replacement of the controlled character sequence
      virtual basic_streambuf<CharT, Traits>* setbuf(char_type* /*unused*/, streamsize /*unused*/)
      {
        return this;
      }

      // the base class version of this function has no effect. Derived classes may override this function
      // to allow relative positioning of the position indicator
      virtual pos_type seekoff(off_type /*unused*/, io::seekdir /*unused*/, io::openmode /*unused*/)
      {
        return pos_type(-1);
      }

      // the base class version of this function has no effect. Derived classes may override this function
      // to allow absolute positioning of the position indicator
      virtual pos_type seekpos(pos_type /*unused*/, io::openmode /*unused*/)
      {
        return pos_type(-1);
      }

      // the base class version of this function has no effect. Derived classes may override this function
      // to allow synchronizing the underlying device with the buffers
      virtual int32 sync()
      {
        return 0;
      }

      //
      // get area
      //

      // obtains the number of characters available for input in the associated input sequence
      // returns 0 in here as we don't know how many characters are left available in the current stream.
      virtual streamsize showmanyc()
      {
        return 0;
      }

      // reads characters from the associated input sequence to the get area
      virtual int_type underflow()
      {
        return Traits::eof();
      }

      // reads characters from the associated input sequence to the get area and advances the next pointer
      virtual int_type uflow()
      {
        const int_type ch = underflow();
        if(Traits::eq_int_type(ch, Traits::eof()))
        {
          return Traits::eof();
        }

        inc_gptr();
        return ch;
      }

      /// RSL Comment: Not in ISO C++ Standard at time of writing (10/Sep/2022)
      // this is different version of uflow where multiple chars can be read in 1 go.
      // this is to avoid multiple sequential virtual calls and have all characters up to count
      // read in 1 go.
      // the function is pure virtual as the derived classes have the responsibility
      // to define what needs to happen when this function is called.
      // reads characters from the associated input sequence to the get area and advance the next pointer
      virtual streamsize uflown(char_type* s, streamsize count) = 0;

      // reads count characters from the input sequence and stores them into a character array pointed to by s.
      // works as if by repeated calls to sbumpc.
      virtual streamsize xsgetn(char_type* s, streamsize count)
      {
        streamsize num_chars_read              = 0;
        const streamsize to_read_from_get_area = (rsl::min)(count, available_in_get_area());
        if(to_read_from_get_area > 0)
        {
          Traits::copy(s, gptr(), to_read_from_get_area);
          inc_gptr(to_read_from_get_area);
          num_chars_read += to_read_from_get_area;
          s += to_read_from_get_area;
          count -= to_read_from_get_area;
        }

        if(count > 0)
        {
          num_chars_read += uflown(s, count);
        }

        return num_chars_read;
      }

      // returns the pointer to the beginning of the get area
      char_type* eback() const
      {
        return m_read_buf_begin ? *m_read_buf_begin : nullptr;
      }
      // returns the pointer to the current character in the get area
      char_type* gptr() const
      {
        return m_read_buf_curr ? *m_read_buf_curr : nullptr;
      }
      // returns the pointer one past the end of the get area
      char_type* egptr() const
      {
        return m_read_buf_end ? *m_read_buf_end : nullptr;
      }

      // skips count characters in the get area
      void gbump(card32 count)
      {
        inc_gptr(count);
      }

      /// RSL Comment: Different from ISO C++ Standard at time of writing (13/Sep/2022)
      // this takes pointer to pointers instead
      // sets the values of the pointers defining the get area
      void setg(char_type** beg, char_type** curr, char_type** end)
      {
        m_read_buf_begin = beg;
        m_read_buf_curr  = curr;
        m_read_buf_end   = end;
      }

      //
      // putback
      //

      // write count characters to the output sequence from the character array whose element is pointed to by s.
      virtual streamsize xsputn(const char_type* s, streamsize count)
      {
        streamsize num_chars_written          = 0;
        const streamsize to_write_to_put_area = (rsl::min)(count, available_in_put_area());
        if(to_write_to_put_area > 0)
        {
          // this cannot fail, so no point in setting the iostate
          Traits::copy(pptr(), s, to_write_to_put_area);
          inc_pptr(to_write_to_put_area);
          num_chars_written += to_write_to_put_area;
          s += to_write_to_put_area;
          count -= to_write_to_put_area;
        }

        if(count > 0)
        {
          num_chars_written += overflown(s, count);
        }

        return num_chars_written;
      }

      // writes characters to the associated output sequence from the put area
      virtual int_type overflow(int_type ch)
      {
        return ch;
      }
      int_type overflow()
      {
        return overflow(Traits::eof());
      }

      /// RSL Comment: Not in ISO C++ Standard at time of writing (10/Sep/2022)
      // this is different version of overflow where multiple chars can be written in 1 go.
      // this is to avoid multiple sequential virtual calls and have all characters up to count
      // written in 1 go.
      // the function is pure virtual as the derived classes have the responsibility
      // to define what needs to happen when this function is called.
      // writes characters to the associated output sequence and advances the next pointer
      virtual streamsize overflown(const char_type* s, streamsize count) = 0;

      // returns the pointer to the beginning of the put area
      char_type* pbase() const
      {
        return m_write_buf_begin ? *m_write_buf_begin : nullptr;
      }
      // returns the pointer to the current character in the put area
      char_type* pptr() const
      {
        return m_write_buf_curr ? *m_write_buf_curr : nullptr;
      }
      // returns the pointer one past the end of the put area
      char_type* epptr() const
      {
        return m_write_buf_end ? *m_write_buf_end : nullptr;
      }

      // advances the next pointer of the output sequence
      void pbump(card32 count)
      {
        inc_pptr(count);
      }

      void setp(char_type** beg, char_type** current, char_type** end)
      {
        m_write_buf_begin = beg;
        m_write_buf_curr  = beg;
        m_read_buf_end    = end;
      }

      // repositions the beginning, next and end pointers of the output sequence
      void setp(char_type** beg, char_type** end) {}

      // puts a character back into the input sequence, possibly modifying the input sequence
      virtual int_type pbackfail(int_type c)
      {
        return c;
      }
      int_type pbackfail()
      {
        return pbackfail(Traits::eof());
      }

      // increases current read pointer
      // returns previously pointed to element
      CharT& inc_gptr(card32 amount = 1)
      {
        REX_ASSERT_X(m_read_buf_curr != nullptr, "Trying to increase read buf pointer, which is nullptr");
        CharT& ch = **m_read_buf_curr;
        *m_read_buf_curr += amount;
        return ch;
      }

      virtual streamsize available_in_get_area() const
      {
        return 0;
      }

    private:
      CharT& dec_gptr()
      {
        REX_ASSERT_X(m_read_buf_curr != nullptr, "Trying to decrease read buf pointer, which is nullptr");
        --*m_read_buf_curr;
        return **m_read_buf_curr;
      }

      // increases current write pointer
      // returns previously pointed to element
      CharT& inc_pptr(card32 amount = 1)
      {
        REX_ASSERT_X(m_write_buf_curr != nullptr, "Trying to increase read buf pointer, which is nullptr");
        CharT& ch = **m_write_buf_curr;
        *m_write_buf_curr += amount;
        return ch;
      }

      streamsize available_in_put_area() const
      {
        return static_cast<streamsize>(epptr() - pptr());
      }

    private:
      CharT** m_read_buf_begin; // pointer to begin of read buffer
      CharT** m_read_buf_curr;  // pointer to current element in the read buffer
      CharT** m_read_buf_end;   // size of read buffer

      CharT** m_write_buf_begin; // pointer to begin of write buffer
      CharT** m_write_buf_curr;  // pointer to current element in the write buffer
      CharT** m_write_buf_end;   // size of the write buffer
    };

  } // namespace v1
} // namespace rsl
