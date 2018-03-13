//==============================================================================================================================
//
// This file is part of the sam0delk1n's Conventional Tools (smdCT).
//
// Copyright © 2018 sam0delk1n <skipper05ster@gmail.com>.
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

#include "common.hpp"
#include "node_i.hpp"

namespace smd::fbt {

//==============================================================================================================================
template< typename timeData_t >
class parentNode_i : public node_i< timeData_t > {
    using node_t = node_i< timeData_t >;

public:
    virtual void mAddChild( std::shared_ptr< node_t > pChild ) = 0;

    parentNode_i&   operator=( const parentNode_i& ) = delete;
    parentNode_i&   operator=( parentNode_i&& )      noexcept = delete;
    explicit inline parentNode_i( const std::string& cName );
    inline          parentNode_i( const parentNode_i& cNode );
                    parentNode_i( parentNode_i&& )   noexcept = delete;
    virtual         ~parentNode_i( void )            = default;
};

//==============================================================================================================================
template< typename timeData_t >
inline parentNode_i< timeData_t >::parentNode_i( const std::string& cName ) :
    node_t( cName ) {}

//==============================================================================================================================
template< typename timeData_t >
inline parentNode_i< timeData_t >::parentNode_i( const parentNode_i& cNode ) :
    node_t( cNode ) {}

} // namespace smd::fbt
