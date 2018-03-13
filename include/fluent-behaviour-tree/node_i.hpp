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

namespace smd::fbt {

//==============================================================================================================================
template< typename timeData_t >
class node_i {
public:
    const std::string mcName; // Name of the node.

    virtual status_t mTick( timeData_t timeData ) = 0;

    node_i&         operator=( const node_i& ) = delete;
    node_i&         operator=( node_i&& )      noexcept = delete;
    explicit inline node_i( const std::string& cName );
    inline          node_i( const node_i& cNode );
                    node_i( node_i&& )         noexcept = delete;
    virtual         ~node_i( void )            = default;
};

//==============================================================================================================================
template< typename timeData_t >
inline node_i< timeData_t >::node_i( const std::string& cName ) :
    mcName( cName ) {}

//==============================================================================================================================
template< typename timeData_t >
inline node_i< timeData_t >::node_i( const node_i& cNode ) :
    mcName( cNode.mcName ) {}

} // namespace smd::fbt
