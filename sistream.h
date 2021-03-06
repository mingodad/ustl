// This file is part of the uSTL library, an STL implementation.
//
// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#pragma once
#include "mistream.h"
#include "ustring.h"
#ifndef EOF
    #define EOF (-1)
#endif

namespace ustl {

/// \class istringstream sistream.h ustl.h
/// \ingroup TextStreams
///
/// \brief A stream that reads textual data from a memory block.
///
class istringstream : public istream {
public:
    static const size_type	c_MaxDelimiters = 16;	///< Maximum number of word delimiters.
public:
				istringstream (void) noexcept;
				istringstream (const void* p, size_type n) noexcept;
    explicit			istringstream (const cmemlink& source) noexcept;
    inline void			iread (char& v)			{ v = skip_delimiters(); }
    inline void			iread (unsigned char& v)	{ char c; iread(c); v = c; }
    void			iread (int& v);
    inline void			iread (unsigned int& v)		{ int c; iread(c); v = c; }
    inline void			iread (short& v)		{ int c; iread(c); v = c; }
    inline void			iread (unsigned short& v)	{ int c; iread(c); v = c; }
    void			iread (long& v);
    inline void			iread (unsigned long& v)	{ long c; iread(c); v = c; }
#if HAVE_THREE_CHAR_TYPES
    void			iread (signed char& v)		{ char c; iread(c); v = c; }
#endif
#if HAVE_LONG_LONG
    void			iread (long long& v);
    inline void			iread (unsigned long long& v)	{ long long c; iread(c); v = c; }
#endif
    void			iread (double& v);
    inline void			iread (float& v)		{ double c; iread(c); v = c; }
    inline void			iread (long double& v)		{ double c; iread(c); v = c; }
    void			iread (bool& v);
    void			iread (wchar_t& v);
    void			iread (string& v);
    inline string		str (void) const	{ string s; s.link (*this); return s; }
    inline istringstream&	str (const string& s)	{ link (s); return *this; }
    inline istringstream&	get (char& c)	{ return read (&c, sizeof(c)); }
    inline int			get (void)	{ char c = EOF; get(c); return c; }
    istringstream&		get (char* p, size_type n, char delim = '\n');
    istringstream&		get (string& s, char delim = '\n');
    istringstream&		getline (char* p, size_type n, char delim = '\n');
    istringstream&		getline (string& s, char delim = '\n');
    istringstream&		ignore (size_type n, char delim = '\0');
    inline char			peek (void)	{ char v = get(); ungetc(); return v; }
    inline istringstream&	putback (char)	{ ungetc(); return *this; }
    inline istringstream&	unget (void)	{ ungetc(); return *this; }
    inline void			set_delimiters (const char* delimiters);
    inline void			set_base (short base);
    inline void			set_decimal_separator (char)	{ }
    inline void			set_thousand_separator (char)	{ }
    istringstream&		read (void* buffer, size_type size);
    inline istringstream&	read (memlink& buf)		{ return read (buf.begin(), buf.size()); }
    inline istringstream&	seekg (off_t p, seekdir d =beg)	{ istream::seekg(p,d); return *this; }
    inline int			sync (void)			{ skip (remaining()); return 0; }
protected:
    char			skip_delimiters (void);
private:
    inline void			read_strz (string&)	{ assert (!"Reading nul characters is not allowed from text streams"); }
    inline bool			is_delimiter (char c) const noexcept;
    template <typename T> void	read_number (T& v);
private:
    char			_delimiters [c_MaxDelimiters];
    uint8_t			_base;
};

//----------------------------------------------------------------------

/// Sets the numeric base used to read numbers.
inline void istringstream::set_base (short base)
{
    _base = base;
}

/// Sets delimiters to the contents of \p delimiters.
inline void istringstream::set_delimiters (const char* delimiters)
{
#if (__i386__ || __x86_64__) && CPU_HAS_SSE && HAVE_VECTOR_EXTENSIONS
    typedef uint32_t v16ud_t __attribute__((vector_size(16)));
    asm("xorps\t%%xmm0, %%xmm0\n\tmovups\t%%xmm0, %0":"=m"(*noalias_cast<v16ud_t*>(_delimiters))::"xmm0");
#else
    memset (_delimiters, 0, sizeof(_delimiters));
#endif
    memcpy (_delimiters, delimiters, min (strlen(delimiters),sizeof(_delimiters)-1));
}

/// Reads a line of text from \p is into \p s
inline istringstream& getline (istringstream& is, string& s, char delim = '\n')
    { return is.getline (s, delim); }

//----------------------------------------------------------------------

template <typename T> struct object_text_reader {
    inline void operator()(istringstream& is, T& v) const { v.text_read (is); }
};
template <typename T> struct integral_text_object_reader {
    inline void operator()(istringstream& is, T& v) const { is.iread (v); }
};
template <typename T>
inline istringstream& operator>> (istringstream& is, T& v) {
    typedef typename tm::Select <numeric_limits<T>::is_integral,
	integral_text_object_reader<T>, object_text_reader<T> >::Result object_reader_t;
    object_reader_t()(is, v);
    return is;
}

//----------------------------------------------------------------------

template <> struct object_text_reader<string> {
    inline void operator()(istringstream& is, string& v) const { is.iread (v); }
};
} // namespace ustl
