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
// Runs child nodes in sequence, until one fails.
//==============================================================================================================================
template< typename timeData_t >
class sequenceNode_c final : public parentNode_i< timeData_t > {
    using parentNode_t = parentNode_i< timeData_t >;
    using nodePtr_t    = std::shared_ptr< node_i< timeData_t > >;

public:
    inline status_t mTick( timeData_t time )      override;
    inline void     mAddChild( nodePtr_t pChild ) override;

    sequenceNode_c& operator=( const sequenceNode_c& ) = delete;
    sequenceNode_c& operator=( sequenceNode_c&& )      noexcept = delete;
    explicit inline sequenceNode_c( const std::string& cName );
    inline          sequenceNode_c( const sequenceNode_c& cNode );
                    sequenceNode_c( sequenceNode_c&& ) noexcept = delete;
                    ~sequenceNode_c( void )            = default;

private:
    std::vector< nodePtr_t > mChildren;
};

//==============================================================================================================================
template< typename timeData_t >
inline status_t sequenceNode_c< timeData_t >::mTick( timeData_t time ) {
    for ( auto ipChild = mChildren.cbegin(); ipChild != mChildren.cend(); ++ipChild ) {
        const status_t cStatus { ( *ipChild )->mTick( time ) };

        if ( cStatus != status_t::SUCCESS ) {
            return cStatus;
        }
    }

    return status_t::SUCCESS;
}

//==============================================================================================================================
template< typename timeData_t >
inline void sequenceNode_c< timeData_t >::mAddChild( nodePtr_t pChild ) {
    mChildren.emplace_back( move( pChild ) );
}

//==============================================================================================================================
template< typename timeData_t >
inline sequenceNode_c< timeData_t >::sequenceNode_c( const std::string& cName ) :
    parentNode_t( cName ) {}

//==============================================================================================================================
template< typename timeData_t >
inline sequenceNode_c< timeData_t >::sequenceNode_c( const sequenceNode_c& cNode ) :
    parentNode_t( cNode ),
    mChildren( cNode.mChildren ) {}

} // namespace smd::fbt
