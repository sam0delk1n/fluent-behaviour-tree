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
#include "parentnode_i.hpp"

namespace smd::fbt {

//==============================================================================================================================
// Runs childs nodes in parallel.
//==============================================================================================================================
template< typename timeData_t >
class parallelNode_c final : public parentNode_i< timeData_t > {
    using parentNode_t = parentNode_i< timeData_t >;
    using nodePtr_t    = std::shared_ptr< node_i< timeData_t > >;

public:
    inline status_t mTick( timeData_t time )      override;
    inline void     mAddChild( nodePtr_t pChild ) override;

    parallelNode_c& operator=( const parallelNode_c& ) = delete;
    parallelNode_c& operator=( parallelNode_c&& )      noexcept = delete;
    inline          parallelNode_c( const std::string& cName, uint32_t numToFail, uint32_t numToSucceed );
    inline          parallelNode_c( const parallelNode_c& cNode );
                    parallelNode_c( parallelNode_c&& ) noexcept = delete;
                    ~parallelNode_c( void )            = default;

private:
    const uint32_t mcNumToFail;    // Number of child failures required to terminate with failure.
    const uint32_t mcNumToSucceed; // Number of child successess require to terminate with success.

    std::vector< nodePtr_t > mChildren;
};

//==============================================================================================================================
template< typename timeData_t >
inline status_t parallelNode_c< timeData_t >::mTick( timeData_t time ) {
    uint32_t numChildrenSuceeded = 0;
    uint32_t numChildrenFailed = 0;

    for ( auto ipChild = mChildren.cbegin(); ipChild != mChildren.cend(); ++ipChild ) {
        switch ( ( *ipChild )->mTick( time ) ) {
            case status_t::SUCCESS:
                ++numChildrenSuceeded;
                break;
            case status_t::FAILURE:
                ++numChildrenFailed;
                break;
            default:
                break;
        }
    }

    if ( mcNumToSucceed > 0 && numChildrenSuceeded >= mcNumToSucceed ) {
        return status_t::SUCCESS;
    }
    if ( mcNumToFail > 0 && numChildrenFailed >= mcNumToFail ) {
        return status_t::FAILURE;
    }

    return status_t::RUNNING;
}

//==============================================================================================================================
template< typename timeData_t >
inline void parallelNode_c< timeData_t >::mAddChild( nodePtr_t pChild ) {
    mChildren.emplace_back( move( pChild ) );
}

//==============================================================================================================================
template< typename timeData_t >
inline parallelNode_c< timeData_t >::parallelNode_c( const std::string& cName, uint32_t numToFail, uint32_t numToSucceed ) :
    parentNode_t( cName ),
    mcNumToFail( numToFail ),
    mcNumToSucceed( numToSucceed ) {}

//==============================================================================================================================
template< typename timeData_t >
inline parallelNode_c< timeData_t >::parallelNode_c( const parallelNode_c& cNode ) :
    parentNode_t( cNode ),
    mcNumToFail( cNode.mcNumToFail ),
    mcNumToSucceed( cNode.mcNumToSucceed ),
    mChildren( cNode.mChildren ) {}

} // namespace smd::fbt
