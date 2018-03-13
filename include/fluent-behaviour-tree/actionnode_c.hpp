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
// A behaviour tree leaf node for running an action.
//==============================================================================================================================
template< typename timeData_t >
class actionNode_c final : public node_i< timeData_t > {
    using node_t     = node_i< timeData_t >;
    using function_t = std::function< status_t( timeData_t ) >;

public:
    inline status_t mTick( timeData_t timeData ) override;

    actionNode_c& operator=( const actionNode_c& ) = delete;
    actionNode_c& operator=( actionNode_c&& )      noexcept = delete;
    inline        actionNode_c( const std::string& cName, const function_t& cFunction );
    inline        actionNode_c( const actionNode_c& cNode );
                  actionNode_c( actionNode_c&& )   noexcept = delete;
                  ~actionNode_c( void )            = default;

private:
    const function_t mcFunction; // Function to invoke for the action.
};

//==============================================================================================================================
template< typename timeData_t >
inline status_t actionNode_c< timeData_t >::mTick( timeData_t timeData ) {
    return mcFunction( timeData );
}

//==============================================================================================================================
template< typename timeData_t >
inline actionNode_c< timeData_t >::actionNode_c( const std::string& cName, const function_t& cFunction ) :
    node_t( cName ),
    mcFunction( cFunction ) {}

//==============================================================================================================================
template< typename timeData_t >
inline actionNode_c< timeData_t >::actionNode_c( const actionNode_c& cNode ) :
    node_t( cNode ),
    mcFunction( cNode.mcFunction ) {}

} // namespace smd::fbt
