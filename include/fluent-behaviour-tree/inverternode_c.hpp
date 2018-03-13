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
// Decorator node that inverts the success/failure of its child.
//==============================================================================================================================
template< typename timeData_t >
class inverterNode_c final : public parentNode_i< timeData_t > {
    using parentNode_t = parentNode_i< timeData_t >;
    using nodePtr_t    = std::shared_ptr< node_i< timeData_t > >;

public:
    inline status_t mTick( timeData_t time )      override;
    inline void     mAddChild( nodePtr_t pChild ) override;

    inverterNode_c& operator=( const inverterNode_c& ) = delete;
    inverterNode_c& operator=( inverterNode_c&& )      noexcept = delete;
    explicit inline inverterNode_c( const std::string& cName );
    inline          inverterNode_c( const inverterNode_c& cNode );
                    inverterNode_c( inverterNode_c&& ) noexcept = delete;
                    ~inverterNode_c( void )            = default;

private:
    nodePtr_t mpChild;
};

//==============================================================================================================================
template< typename timeData_t >
inline status_t inverterNode_c< timeData_t >::mTick( timeData_t time ) {
    if ( !mpChild ) {
        throw except_c( "'inverterNode_c' must have a child node!" );
    }

    const status_t cResult { mpChild->mTick( time ) };

    if ( cResult == status_t::FAILURE ) {
        return status_t::SUCCESS;
    } else if ( cResult == status_t::SUCCESS ) {
        return status_t::FAILURE;
    } else {
        return cResult;
    }
}

//==============================================================================================================================
template< typename timeData_t >
inline void inverterNode_c< timeData_t >::mAddChild( nodePtr_t pChild ) {
    if ( mpChild ) {
        throw except_c( "Can't add more than a single child to 'inverterNode_c'!" );
    }

    mpChild = move( pChild );
}

//==============================================================================================================================
template< typename timeData_t >
inline inverterNode_c< timeData_t >::inverterNode_c( const std::string& cName ) :
    parentNode_t( cName ) {}

//==============================================================================================================================
template< typename timeData_t >
inline inverterNode_c< timeData_t >::inverterNode_c( const inverterNode_c& cNode ) :
    parentNode_t( cNode ),
    mpChild( cNode.mpChild ) {}

} // namespace smd::fbt
