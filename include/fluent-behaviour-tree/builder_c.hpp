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
#include "actionnode_c.hpp"
#include "inverternode_c.hpp"
#include "parallelnode_c.hpp"
#include "selectornode_c.hpp"
#include "sequencenode_c.hpp"

namespace smd::fbt {

//==============================================================================================================================
// Fluent API for building a behaviour tree.
//==============================================================================================================================
template< typename timeData_t >
class builder_c final {
    using nodePtr_t       = std::shared_ptr< node_i< timeData_t > >;
    using parentNodePtr_t = std::shared_ptr< parentNode_i< timeData_t > >;
    using function_t      = std::function< status_t( timeData_t ) >;
    using funcBool_t      = std::function< bool( timeData_t ) >;

public:
    inline builder_c& mDo( const std::string& cName, const function_t& cFunction );
    inline builder_c& mCondition( const std::string& cName, const funcBool_t& cFunction );
    inline builder_c& mInverter( const std::string& cName );
    inline builder_c& mSequence( const std::string& cName );
    inline builder_c& mParallel( const std::string& cName, uint32_t numToFail, uint32_t numToSucceed );
    inline builder_c& mSelector( const std::string& cName );

    inline builder_c& mSplice( nodePtr_t pSubTree );
    inline builder_c& mEnd( void );
    inline nodePtr_t  mBuild( void );

    builder_c& operator=( const builder_c& ) = delete;
    builder_c& operator=( builder_c&& )      noexcept = delete;
               builder_c( void )             = default;
               builder_c( const builder_c& ) = delete;
               builder_c( builder_c&& )      noexcept = delete;
               ~builder_c( void )            = default;

private:
    nodePtr_t                     mpCurrent; // Last node created.
    std::stack< parentNodePtr_t > mStack;    // Stack node nodes that we are build via the fluent API.
};

//==============================================================================================================================
// Create an action node.
//==============================================================================================================================
template< typename timeData_t >
inline auto builder_c< timeData_t >::mDo( const std::string& cName, const function_t& cFunction ) ->builder_c& {
    if ( mStack.empty() ) {
        throw except_c( "Can't create an unnested 'actionNode_c', it must be a leaf node." );
    }

    mStack.top()->mAddChild( std::make_shared< actionNode_c< timeData_t > >( cName, cFunction ) );

    return *this;
}

//==============================================================================================================================
// Like an action node... but the function can return true/false and is mapped to success/failure.
//==============================================================================================================================
template< typename timeData_t >
inline auto builder_c< timeData_t >::mCondition( const std::string& cName, const funcBool_t& cFunction ) ->builder_c& {
    return mDo( cName, [ &cFunction ]( timeData_t timeDelta ) ->status_t {
        return cFunction( timeDelta ) ? status_t::SUCCESS : status_t::FAILURE;
    } );
}

//==============================================================================================================================
// Create an inverter node that inverts the success/failure of its children.
//==============================================================================================================================
template< typename timeData_t >
inline auto builder_c< timeData_t >::mInverter( const std::string& cName ) ->builder_c& {
    auto pNode = std::make_shared< inverterNode_c< timeData_t > >( cName );

    if ( mStack.size() ) {
        mStack.top()->mAddChild( pNode );
    }

    mStack.emplace( pNode );

    return *this;
}

//==============================================================================================================================
// Create a sequence node.
//==============================================================================================================================
template< typename timeData_t >
inline auto builder_c< timeData_t >::mSequence( const std::string& cName ) ->builder_c& {
    auto pNode = std::make_shared< sequenceNode_c< timeData_t > >( cName );

    if ( mStack.size() ) {
        mStack.top()->mAddChild( pNode );
    }

    mStack.emplace( pNode );

    return *this;
}

//==============================================================================================================================
// Create a parallel node.
//==============================================================================================================================
template< typename timeData_t >
inline auto builder_c< timeData_t >::mParallel( const std::string& cName,
                                               uint32_t numToFail, uint32_t numToSucceed ) ->builder_c& {
    auto pNode = std::make_shared< parallelNode_c< timeData_t > >( cName, numToFail, numToSucceed );

    if ( mStack.size() ) {
        mStack.top()->mAddChild( pNode );
    }

    mStack.emplace( pNode );

    return *this;
}

//==============================================================================================================================
// Create a selector node.
//==============================================================================================================================
template< typename timeData_t >
inline auto builder_c< timeData_t >::mSelector( const std::string& cName ) ->builder_c& {
    auto pNode = std::make_shared< selectorNode_c< timeData_t > >( cName );

    if ( mStack.size() ) {
        mStack.top()->mAddChild( pNode );
    }

    mStack.emplace( pNode );

    return *this;
}

//==============================================================================================================================
// Splice a sub tree into the parent tree.
//==============================================================================================================================
template< typename timeData_t >
inline auto builder_c< timeData_t >::mSplice( nodePtr_t pSubTree ) ->builder_c& {
    if ( mStack.empty() ) {
        throw except_c( "Can't splice an unnested sub-tree, there must be a parent-tree." );
    }

    mStack.top()->mAddChild( move( pSubTree ) );

    return *this;
}

//==============================================================================================================================
// Ends a sequence of children.
//==============================================================================================================================
template< typename timeData_t >
inline auto builder_c< timeData_t >::mEnd( void ) ->builder_c& {
    mpCurrent = mStack.top();
    mStack.pop();

    return *this;
}

//==============================================================================================================================
// Build the actual tree.
//==============================================================================================================================
template< typename timeData_t >
inline auto builder_c< timeData_t >::mBuild( void ) ->nodePtr_t {
    if ( !mpCurrent ) {
        throw except_c( "Can't create a behaviour tree with zero nodes" );
    }

    return mpCurrent;
}

} // namespace smd::fbt
