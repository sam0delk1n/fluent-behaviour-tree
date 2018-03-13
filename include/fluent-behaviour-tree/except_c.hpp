//==============================================================================================================================
//
// This file is part of the sam0delk1n's Conventional Tools (smdCT).
//
// Copyright © 2016-2018 sam0delk1n <skipper05ster@gmail.com>.
// All rights reserved.
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
//    claim that you wrote the original software. If you use this software
//    in a product, an acknowledgment in the product documentation would be
//    appreciated but is not required.
// 2. Altered source versions must be plainly marked as such, and must not be
//    misrepresented as being the original software.
// 3. This notice may not be removed or altered from any source distribution.
//
//==============================================================================================================================

#pragma once

#include <exception>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>

namespace smd {

//==============================================================================================================================
struct callStack_t final {
    std::vector< void* > frames;
};

//==============================================================================================================================
class except_c final : public std::exception {
public:
    inline const char* what( void ) const noexcept override;

    inline const callStack_t& mcGetCallStack( void )    const noexcept;
    inline std::string        mcGetCallStackStr( void ) const noexcept;

    except_c&       operator=( const except_c& )                                        noexcept = delete;
    except_c&       operator=( except_c&& )                                             noexcept = delete;
    inline explicit except_c( const std::string& cWhat )                                noexcept;
    inline explicit except_c( const std::string& cWhat, const callStack_t& cCallStack ) noexcept;
    inline          except_c( const except_c& cEx )                                     noexcept;
    inline          except_c( except_c&& ex )                                           noexcept;
                    ~except_c( void )                                                   noexcept = default;

private:
    std::string mWhat;
    callStack_t mCallStack;
};

//==============================================================================================================================
inline const char* except_c::what( void ) const noexcept {
    return mWhat.c_str();
}

//==============================================================================================================================
inline const callStack_t& except_c::mcGetCallStack( void ) const noexcept {
    return mCallStack;
}

//==============================================================================================================================
inline std::string except_c::mcGetCallStackStr( void ) const noexcept {
    try {
        const size_t cNumOfFrames { mCallStack.frames.size() };

        std::stringstream out;

        for ( size_t iFrame { 0 }; iFrame < cNumOfFrames; ++iFrame ) {
            std::stringstream temp;
            temp << "[" << std::dec << iFrame << "]";

            out << std::setw( 4 ) << std::setfill( ' ' ) << temp.str() << " 0x" << std::hex << std::setw( 16 )
                << std::setfill( '0' ) << reinterpret_cast< uint64_t >( mCallStack.frames.at( iFrame ) );

            if ( iFrame + 1 < cNumOfFrames ) {
                out << "\n";
            }
        }

        return out.str();

    } catch ( ... ) {
        return "'except_c::mcGetCallStackStr()' internal error!";
    }
}

//==============================================================================================================================
inline except_c::except_c( const std::string& cWhat ) noexcept :
mWhat( cWhat ) {}

//==============================================================================================================================
inline except_c::except_c( const std::string& cWhat, const callStack_t& cCallStack ) noexcept :
mWhat( cWhat ),
mCallStack( cCallStack ) {}

//==============================================================================================================================
inline except_c::except_c( const except_c& cEx ) noexcept :
exception( cEx ),
mWhat( cEx.mWhat ),
mCallStack( cEx.mCallStack ) {}

//==============================================================================================================================
inline except_c::except_c( except_c&& ex ) noexcept :
exception( std::move( ex ) ),
mWhat( std::move( ex.mWhat ) ),
mCallStack( std::move( ex.mCallStack ) ) {}

} // namespace smd

//==============================================================================================================================
#ifdef SMD_DEBUG
#define SMD_ASSERT( condition )                \
    if ( !( condition ) ) {                    \
        std::stringstream out;                 \
        out << "ASSERT: file=\"" << __FILE__   \
            << "\" line=\""      << __LINE__   \
            << "\" function=\""  << __func__   \
            << "\""              << std::endl; \
        throw smd::except_c( out.str() );      \
    }
#else
#define SMD_ASSERT( condition )
#endif

//==============================================================================================================================
#ifdef SMD_DEBUG
#define PYR_ASSERT( condition, comment )       \
    if ( !( condition ) ) {                    \
        std::stringstream out;                 \
        out << "ASSERT: file=\"" << __FILE__   \
            << "\" line=\""      << __LINE__   \
            << "\" function=\""  << __func__   \
            << "\" comment=\""   << comment    \
            << "\""              << std::endl; \
        throw smd::except_c( out.str() );      \
    }
#else
#define PYR_ASSERT( condition, comment )
#endif

//==============================================================================================================================
#define SMD_HASSERT( condition )               \
    if ( !( condition ) ) {                    \
        std::stringstream out;                 \
        out << "ASSERT: file=\"" << __FILE__   \
            << "\" line=\""      << __LINE__   \
            << "\" function=\""  << __func__   \
            << "\""              << std::endl; \
        throw smd::except_c( out.str() );      \
    }

//==============================================================================================================================
#define PYR_HASSERT( condition, comment )      \
    if ( !( condition ) ) {                    \
        std::stringstream out;                 \
        out << "ASSERT: file=\"" << __FILE__   \
            << "\" line=\""      << __LINE__   \
            << "\" function=\""  << __func__   \
            << "\" comment=\""   << comment    \
            << "\""              << std::endl; \
        throw smd::except_c( out.str() );      \
    }
