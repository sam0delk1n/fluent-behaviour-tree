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

#include "../include/fluent-behaviour-tree.hpp"

#include <iomanip>
#include <ios>
#include <iostream>
#include <string>
#include <exception>

#define SMD_CHK_EXCEPT_CT( statement ) { \
    bool caught { false };               \
    try {                                \
        statement;                       \
    } catch ( const except_c& cEx ) {    \
        caught = true;                   \
    }                                    \
    SMD_HASSERT( caught );               \
}

#define SMD_CHK_EXCEPT_ALL( statement ) { \
    bool caught { false };                \
    try {                                 \
        statement;                        \
    } catch ( ... ) {                     \
        caught = true;                    \
    }                                     \
    SMD_HASSERT( caught );                \
}

using namespace std;
using namespace smd;
using namespace smd::fbt;

using timeData_t     = float;
using actionNode_t   = actionNode_c< timeData_t >;
using inverterNode_t = inverterNode_c< timeData_t >;
using parallelNode_t = parallelNode_c< timeData_t >;
using selectorNode_t = selectorNode_c< timeData_t >;
using sequenceNode_t = sequenceNode_c< timeData_t >;
using builder_t      = builder_c< timeData_t >;
using nodePtr_t      = shared_ptr< node_i< timeData_t > >;

//==============================================================================================================================
void check_fbtActionNode( void ) {
    constexpr timeData_t cTimeDelta { 0.1 };

    unsigned int invokeCount { 0 };

    actionNode_t testObject( "some-action", [ & ]( timeData_t timeDelta ) ->status_t {
        SMD_HASSERT( cTimeDelta == timeDelta );

        ++invokeCount;
        return status_t::RUNNING;
    } );

    SMD_HASSERT( status_t::RUNNING == testObject.mTick( cTimeDelta ) );
    SMD_HASSERT( 1 == invokeCount );

    cout << setw( 100 ) << "check_fbtActionNode: " << " PASSED" << endl;
}

//==============================================================================================================================
void check_fbtInverterNode( void ) {
    string testName;

    auto testPassed = [ &testName ]{
        cout << setw( 100 ) << ( "check_fbtInverterNode: " + testName + ": " ) << " PASSED" << endl;
    };

    testName = "Ticking with no child node throws exception";
    {
        inverterNode_t testObject( "some-action" );

        SMD_CHK_EXCEPT_CT( testObject.mTick( 0.1f ) );
    }
    testPassed();

    testName = "Inverts success of child node";
    {
        unsigned int mock1Times { 0 };

        inverterNode_t testObject( "some-action" );

        constexpr timeData_t cTimeDelta { 0.1 };

        nodePtr_t pMock1( new actionNode_t( "mock1", [ & ]( timeData_t ) ->status_t {
            ++mock1Times;
            return status_t::SUCCESS;
        } ) );

        testObject.mAddChild( pMock1 );

        SMD_HASSERT( status_t::FAILURE == testObject.mTick( cTimeDelta ) );

        SMD_HASSERT( 1 == mock1Times );
    }
    testPassed();

    testName = "Inverts failure of child node";
    {
        unsigned int mock1Times { 0 };

        inverterNode_t testObject( "some-action" );

        constexpr timeData_t cTimeDelta { 0.1 };

        nodePtr_t pMock1( new actionNode_t( "mock1", [ & ]( timeData_t ) ->status_t {
            ++mock1Times;
            return status_t::FAILURE;
        } ) );

        testObject.mAddChild( pMock1 );

        SMD_HASSERT( status_t::SUCCESS == testObject.mTick( cTimeDelta ) );

        SMD_HASSERT( 1 == mock1Times );
    }
    testPassed();

    testName = "Pass through running of child node";
    {
        unsigned int mock1Times { 0 };

        inverterNode_t testObject( "some-action" );

        constexpr timeData_t cTimeDelta { 0.1 };

        nodePtr_t pMock1( new actionNode_t( "mock1", [ & ]( timeData_t ) ->status_t {
            ++mock1Times;
            return status_t::RUNNING;
        } ) );

        testObject.mAddChild( pMock1 );

        SMD_HASSERT( status_t::RUNNING == testObject.mTick( cTimeDelta ) );

        SMD_HASSERT( 1 == mock1Times );
    }
    testPassed();

    testName = "Adding more than a single child throws exception";
    {
        inverterNode_t testObject( "some-action" );

        nodePtr_t pMock1( new actionNode_t( "mock1", [ & ]( timeData_t ) ->status_t {
            return status_t::RUNNING;
        } ) );

        testObject.mAddChild( pMock1 );

        nodePtr_t pMock2( new actionNode_t( "mock2", [ & ]( timeData_t ) ->status_t {
            return status_t::RUNNING;
        } ) );

        SMD_CHK_EXCEPT_CT( testObject.mAddChild( pMock2 ) );
    }
    testPassed();
}

//==============================================================================================================================
void check_fbtParallelNode( void ) {
    string testName;

    auto testPassed = [ &testName ]{
        cout << setw( 100 ) << ( "check_fbtParallelNode: " + testName + ": " ) << " PASSED" << endl;
    };

    testName = "Runs all nodes in order";
    {
        unsigned int mock1Times { 0 };
        unsigned int mock2Times { 0 };

        parallelNode_t testObject( "some-parallel", 0, 0 );

        constexpr timeData_t cTimeDelta { 0.1 };
        unsigned int    callOrder  { 0 };

        nodePtr_t pMock1( new actionNode_t( "mock1", [ & ]( timeData_t ) ->status_t {
            SMD_HASSERT( 1 == ++callOrder );
            ++mock1Times;
            return status_t::RUNNING;
        } ) );

        nodePtr_t pMock2( new actionNode_t( "mock2", [ & ]( timeData_t ) ->status_t {
            SMD_HASSERT( 2 == ++callOrder );
            ++mock2Times;
            return status_t::RUNNING;
        } ) );

        testObject.mAddChild( pMock1 );
        testObject.mAddChild( pMock2 );

        SMD_HASSERT( status_t::RUNNING == testObject.mTick( cTimeDelta ) );
        SMD_HASSERT( 2 == callOrder );

        SMD_HASSERT( 1 == mock1Times );
        SMD_HASSERT( 1 == mock2Times );
    }
    testPassed();

    testName = "Fails when required number of children fail";
    {
        unsigned int mock1Times { 0 };
        unsigned int mock2Times { 0 };
        unsigned int mock3Times { 0 };

        parallelNode_t testObject( "some-parallel", 2, 2 );

        constexpr timeData_t cTimeDelta { 0.1 };

        nodePtr_t pMock1( new actionNode_t( "mock1", [ & ]( timeData_t ) ->status_t {
            ++mock1Times;
            return status_t::FAILURE;
        } ) );

        nodePtr_t pMock2( new actionNode_t( "mock2", [ & ]( timeData_t ) ->status_t {
            ++mock2Times;
            return status_t::FAILURE;
        } ) );

        nodePtr_t pMock3( new actionNode_t( "mock3", [ & ]( timeData_t ) ->status_t {
            ++mock3Times;
            return status_t::RUNNING;
        } ) );

        testObject.mAddChild( pMock1 );
        testObject.mAddChild( pMock2 );
        testObject.mAddChild( pMock3 );

        SMD_HASSERT( status_t::FAILURE == testObject.mTick( cTimeDelta ) );

        SMD_HASSERT( 1 == mock1Times );
        SMD_HASSERT( 1 == mock2Times );
        SMD_HASSERT( 1 == mock3Times );
    }
    testPassed();

    testName = "Succeeeds when required number of children succeed";
    {
        unsigned int mock1Times { 0 };
        unsigned int mock2Times { 0 };
        unsigned int mock3Times { 0 };

        parallelNode_t testObject( "some-parallel", 2, 2 );

        constexpr timeData_t cTimeDelta { 0.1 };

        nodePtr_t pMock1( new actionNode_t( "mock1", [ & ]( timeData_t ) ->status_t {
            ++mock1Times;
            return status_t::SUCCESS;
        } ) );

        nodePtr_t pMock2( new actionNode_t( "mock2", [ & ]( timeData_t ) ->status_t {
            ++mock2Times;
            return status_t::SUCCESS;
        } ) );

        nodePtr_t pMock3( new actionNode_t( "mock3", [ & ]( timeData_t ) ->status_t {
            ++mock3Times;
            return status_t::RUNNING;
        } ) );

        testObject.mAddChild( pMock1 );
        testObject.mAddChild( pMock2 );
        testObject.mAddChild( pMock3 );

        SMD_HASSERT( status_t::SUCCESS == testObject.mTick( cTimeDelta ) );

        SMD_HASSERT( 1 == mock1Times );
        SMD_HASSERT( 1 == mock2Times );
        SMD_HASSERT( 1 == mock3Times );
    }
    testPassed();

    testName = "Continues to run if required number children neither succeed or fail";
    {
        unsigned int mock1Times { 0 };
        unsigned int mock2Times { 0 };

        parallelNode_t testObject( "some-parallel", 2, 2 );

        constexpr timeData_t cTimeDelta { 0.1 };

        nodePtr_t pMock1( new actionNode_t( "mock1", [ & ]( timeData_t ) ->status_t {
            ++mock1Times;
            return status_t::SUCCESS;
        } ) );

        nodePtr_t pMock2( new actionNode_t( "mock2", [ & ]( timeData_t ) ->status_t {
            ++mock2Times;
            return status_t::FAILURE;
        } ) );

        testObject.mAddChild( pMock1 );
        testObject.mAddChild( pMock2 );

        SMD_HASSERT( status_t::RUNNING == testObject.mTick( cTimeDelta ) );

        SMD_HASSERT( 1 == mock1Times );
        SMD_HASSERT( 1 == mock2Times );
    }
    testPassed();
}

//==============================================================================================================================
void check_fbtSelectorNode( void ) {
    string testName;

    auto testPassed = [ &testName ]{
        cout << setw( 100 ) << ( "check_fbtSelectorNode: " + testName + ": " ) << " PASSED" << endl;
    };

    testName = "Runs the first node if it succeeds";
    {
        unsigned int mock1Times { 0 };
        unsigned int mock2Times { 0 };

        selectorNode_t testObject( "some-selector" );

        constexpr timeData_t cTimeDelta { 0.1 };

        nodePtr_t pMock1( new actionNode_t( "mock1", [ & ]( timeData_t ) ->status_t {
            ++mock1Times;
            return status_t::SUCCESS;
        } ) );

        nodePtr_t pMock2( new actionNode_t( "mock2", [ & ]( timeData_t ) ->status_t {
            ++mock2Times;
            return status_t::SUCCESS;
        } ) );

        testObject.mAddChild( pMock1 );
        testObject.mAddChild( pMock2 );

        SMD_HASSERT( status_t::SUCCESS == testObject.mTick( cTimeDelta ) );

        SMD_HASSERT( 1 == mock1Times );
        SMD_HASSERT( 0 == mock2Times );
    }
    testPassed();

    testName = "Stops on the first node when it is running";
    {
        unsigned int mock1Times { 0 };
        unsigned int mock2Times { 0 };

        selectorNode_t testObject( "some-selector" );

        constexpr timeData_t cTimeDelta { 0.1 };

        nodePtr_t pMock1( new actionNode_t( "mock1", [ & ]( timeData_t ) ->status_t {
            ++mock1Times;
            return status_t::RUNNING;
        } ) );

        nodePtr_t pMock2( new actionNode_t( "mock2", [ & ]( timeData_t ) ->status_t {
            ++mock2Times;
            return status_t::RUNNING;
        } ) );

        testObject.mAddChild( pMock1 );
        testObject.mAddChild( pMock2 );

        SMD_HASSERT( status_t::RUNNING == testObject.mTick( cTimeDelta ) );

        SMD_HASSERT( 1 == mock1Times );
        SMD_HASSERT( 0 == mock2Times );
    }
    testPassed();

    testName = "Runs the second node if the first fails";
    {
        unsigned int mock1Times { 0 };
        unsigned int mock2Times { 0 };

        selectorNode_t testObject( "some-selector" );

        constexpr timeData_t cTimeDelta { 0.1 };

        nodePtr_t pMock1( new actionNode_t( "mock1", [ & ]( timeData_t ) ->status_t {
            ++mock1Times;
            return status_t::FAILURE;
        } ) );

        nodePtr_t pMock2( new actionNode_t( "mock2", [ & ]( timeData_t ) ->status_t {
            ++mock2Times;
            return status_t::SUCCESS;
        } ) );

        testObject.mAddChild( pMock1 );
        testObject.mAddChild( pMock2 );

        SMD_HASSERT( status_t::SUCCESS == testObject.mTick( cTimeDelta ) );

        SMD_HASSERT( 1 == mock1Times );
        SMD_HASSERT( 1 == mock2Times );
    }
    testPassed();

    testName = "Fails when all children fail";
    {
        unsigned int mock1Times { 0 };
        unsigned int mock2Times { 0 };

        selectorNode_t testObject( "some-selector" );

        constexpr timeData_t cTimeDelta { 0.1 };

        nodePtr_t pMock1( new actionNode_t( "mock1", [ & ]( timeData_t ) ->status_t {
            ++mock1Times;
            return status_t::FAILURE;
        } ) );

        nodePtr_t pMock2( new actionNode_t( "mock2", [ & ]( timeData_t ) ->status_t {
            ++mock2Times;
            return status_t::FAILURE;
        } ) );

        testObject.mAddChild( pMock1 );
        testObject.mAddChild( pMock2 );

        SMD_HASSERT( status_t::FAILURE == testObject.mTick( cTimeDelta ) );

        SMD_HASSERT( 1 == mock1Times );
        SMD_HASSERT( 1 == mock2Times );
    }
    testPassed();
}

//==============================================================================================================================
void check_fbtSequenceNode( void ) {
    string testName;

    auto testPassed = [ &testName ]{
        cout << setw( 100 ) << ( "check_fbtSequenceNode: " + testName + ": " ) << " PASSED" << endl;
    };

    testName = "Can run all children in order";
    {
        unsigned int mock1Times { 0 };
        unsigned int mock2Times { 0 };

        sequenceNode_t testObject( "some-selector" );

        constexpr timeData_t cTimeDelta { 0.1 };
        unsigned int    callOrder  { 0 };

        nodePtr_t pMock1( new actionNode_t( "mock1", [ & ]( timeData_t ) ->status_t {
            ++mock1Times;
            SMD_HASSERT( 1 == ++callOrder );
            return status_t::SUCCESS;
        } ) );

        nodePtr_t pMock2( new actionNode_t( "mock2", [ & ]( timeData_t ) ->status_t {
            ++mock2Times;
            SMD_HASSERT( 2 == ++callOrder );
            return status_t::SUCCESS;
        } ) );

        testObject.mAddChild( pMock1 );
        testObject.mAddChild( pMock2 );

        SMD_HASSERT( status_t::SUCCESS == testObject.mTick( cTimeDelta ) );
        SMD_HASSERT( 2 == callOrder );

        SMD_HASSERT( 1 == mock1Times );
        SMD_HASSERT( 1 == mock2Times );
    }
    testPassed();

    testName = "When first child is running second child is supressed";
    {
        unsigned int mock1Times { 0 };
        unsigned int mock2Times { 0 };

        sequenceNode_t testObject( "some-selector" );

        constexpr timeData_t cTimeDelta { 0.1 };

        nodePtr_t pMock1( new actionNode_t( "mock1", [ & ]( timeData_t ) ->status_t {
            ++mock1Times;
            return status_t::RUNNING;
        } ) );

        nodePtr_t pMock2( new actionNode_t( "mock2", [ & ]( timeData_t ) ->status_t {
            ++mock2Times;
            return status_t::RUNNING;
        } ) );

        testObject.mAddChild( pMock1 );
        testObject.mAddChild( pMock2 );

        SMD_HASSERT( status_t::RUNNING == testObject.mTick( cTimeDelta ) );

        SMD_HASSERT( 1 == mock1Times );
        SMD_HASSERT( 0 == mock2Times );
    }
    testPassed();

    testName = "When first child fails then entire sequence fails";
    {
        unsigned int mock1Times { 0 };
        unsigned int mock2Times { 0 };

        sequenceNode_t testObject( "some-selector" );

        constexpr timeData_t cTimeDelta { 0.1 };

        nodePtr_t pMock1( new actionNode_t( "mock1", [ & ]( timeData_t ) ->status_t {
            ++mock1Times;
            return status_t::FAILURE;
        } ) );

        nodePtr_t pMock2( new actionNode_t( "mock2", [ & ]( timeData_t ) ->status_t {
            ++mock2Times;
            return status_t::FAILURE;
        } ) );

        testObject.mAddChild( pMock1 );
        testObject.mAddChild( pMock2 );

        SMD_HASSERT( status_t::FAILURE == testObject.mTick( cTimeDelta ) );

        SMD_HASSERT( 1 == mock1Times );
        SMD_HASSERT( 0 == mock2Times );
    }
    testPassed();

    testName = "When second child fails then entire sequence fails";
    {
        unsigned int mock1Times { 0 };
        unsigned int mock2Times { 0 };

        sequenceNode_t testObject( "some-selector" );

        constexpr timeData_t cTimeDelta { 0.1 };

        nodePtr_t pMock1( new actionNode_t( "mock1", [ & ]( timeData_t ) ->status_t {
            ++mock1Times;
            return status_t::SUCCESS;
        } ) );

        nodePtr_t pMock2( new actionNode_t( "mock2", [ & ]( timeData_t ) ->status_t {
            ++mock2Times;
            return status_t::FAILURE;
        } ) );

        testObject.mAddChild( pMock1 );
        testObject.mAddChild( pMock2 );

        SMD_HASSERT( status_t::FAILURE == testObject.mTick( cTimeDelta ) );

        SMD_HASSERT( 1 == mock1Times );
        SMD_HASSERT( 1 == mock2Times );
    }
    testPassed();
}

//==============================================================================================================================
void check_fbtBuilder( void ) {
    string testName;

    auto testPassed = [ &testName ]{
        cout << setw( 100 ) << ( "check_fbtBuilder: " + testName + ": " ) << " PASSED" << endl;
    };

    testName = "Cant create a behaviour tree with zero nodes";
    {
        builder_t testObject;

        SMD_CHK_EXCEPT_CT( testObject.mBuild() );
    }
    testPassed();

    testName = "Cant create an unested action node";
    {
        builder_t testObject;

        SMD_CHK_EXCEPT_CT(
            testObject
                .mDo( "some-node-1", []( timeData_t ) { return status_t::RUNNING; } )
                .mBuild()
        );
    }
    testPassed();

    testName = "Can create inverter node";
    {
        builder_t testObject;

        auto pNode = testObject
            .mInverter( "some-inverter" )
                .mDo( "some-node", []( timeData_t ) { return status_t::SUCCESS; } )
            .mEnd()
            .mBuild();

        SMD_HASSERT( typeid( inverterNode_t ) == typeid( *pNode ) );
        SMD_HASSERT( status_t::FAILURE == pNode->mTick( 0.1f ) );
    }
    testPassed();

    testName = "Cant create an unbalanced behaviour tree";
    {
        builder_t testObject;

        SMD_CHK_EXCEPT_CT(
            testObject
                .mInverter( "some-inverter" )
                .mDo( "some-node", []( timeData_t ) { return status_t::SUCCESS; } )
            .mBuild();
        );
    }
    testPassed();

    testName = "Condition is syntactic sugar for do";
    {
        builder_t testObject;

        auto pNode = testObject
            .mInverter( "some-inverter" )
                .mCondition( "some-node", []( timeData_t ) { return true; } )
            .mEnd()
            .mBuild();

        SMD_HASSERT( typeid( inverterNode_t ) == typeid( *pNode ) );
        SMD_HASSERT( status_t::FAILURE == pNode->mTick( 0.1f ) );
    }
    testPassed();

    testName = "Can invert an inverter";
    {
        builder_t testObject;

        auto pNode = testObject
            .mInverter( "some-inverter" )
                .mInverter( "some-other-inverter" )
                    .mDo( "some-node", []( timeData_t ) { return status_t::SUCCESS; } )
                .mEnd()
            .mEnd()
            .mBuild();

        SMD_HASSERT( typeid( inverterNode_t ) == typeid( *pNode ) );
        SMD_HASSERT( status_t::SUCCESS == pNode->mTick( 0.1f ) );
    }
    testPassed();

    testName = "Adding more than a single child to inverter throws exception";
    {
        builder_t testObject;

        SMD_CHK_EXCEPT_CT(
            testObject
                .mInverter( "some-inverter" )
                    .mDo( "some-node",       []( timeData_t ) { return status_t::SUCCESS; } )
                    .mDo( "some-other-node", []( timeData_t ) { return status_t::SUCCESS; } )
                .mEnd()
                .mBuild();
        );
    }
    testPassed();

    testName = "Can create a sequence";
    {
        builder_t testObject;

        uint32_t invokeCount { 0 };

        auto pNode = testObject
            .mSequence( "some-sequence" )
                .mDo( "some-action-1", [ & ]( timeData_t ) {
                    ++invokeCount;
                    return status_t::SUCCESS;
                } )
                .mDo( "some-action-2", [ & ]( timeData_t ) {
                    ++invokeCount;
                    return status_t::SUCCESS;
                } )
            .mEnd()
            .mBuild();

        SMD_HASSERT( typeid( sequenceNode_t ) == typeid( *pNode ) );
        SMD_HASSERT( status_t::SUCCESS == pNode->mTick( 0.1f ) );
        SMD_HASSERT( 2 == invokeCount );
    }
    testPassed();

    testName = "Can create parallel";
    {
        builder_t testObject;

        uint32_t invokeCount { 0 };

        auto pNode = testObject
            .mParallel( "some-parallel", 2, 2 )
                .mDo( "some-action-1", [ & ]( timeData_t ) {
                    ++invokeCount;
                    return status_t::SUCCESS;
                } )
                .mDo( "some-action-2", [ & ]( timeData_t ) {
                    ++invokeCount;
                    return status_t::SUCCESS;
                } )
            .mEnd()
            .mBuild();

        SMD_HASSERT( typeid( parallelNode_t ) == typeid( *pNode ) );
        SMD_HASSERT( status_t::SUCCESS == pNode->mTick( 0.1f ) );
        SMD_HASSERT( 2 == invokeCount );
    }
    testPassed();

    testName = "Can create selector";
    {
        builder_t testObject;

        uint32_t invokeCount { 0 };

        auto pNode = testObject
            .mSelector( "some-selector" )
                .mDo( "some-action-1", [ & ]( timeData_t ) {
                    ++invokeCount;
                    return status_t::FAILURE;
                } )
                .mDo( "some-action-2", [ & ]( timeData_t ) {
                    ++invokeCount;
                    return status_t::SUCCESS;
                } )
            .mEnd()
            .mBuild();

        SMD_HASSERT( typeid( selectorNode_t ) == typeid( *pNode ) );
        SMD_HASSERT( status_t::SUCCESS == pNode->mTick( 0.1f ) );
        SMD_HASSERT( 2 == invokeCount );
    }
    testPassed();

    testName = "Can splice sub tree";
    {
        builder_t testObject;

        uint32_t invokeCount { 0 };

        auto pSpliced = testObject
            .mSequence( "spliced" )
                .mDo( "test", [ & ]( timeData_t ) {
                    ++invokeCount;
                    return status_t::SUCCESS;
                } )
            .mEnd()
            .mBuild();

        auto pTree = testObject
            .mSequence( "parent-tree" )
                .mSplice( pSpliced )
            .mEnd()
            .mBuild();

        pTree->mTick( 0.1f );

        SMD_HASSERT( 1 == invokeCount );
    }
    testPassed();

    testName = "Splicing an unnested sub tree throws exception";
    {
        builder_t testObject;

        uint32_t invokeCount { 0 };

        auto pSpliced = testObject
            .mSequence( "spliced" )
                .mDo( "test", [ & ]( timeData_t ) {
                    ++invokeCount;
                    return status_t::SUCCESS;
                } )
            .mEnd()
            .mBuild();

        SMD_CHK_EXCEPT_CT(
            testObject
                .mSplice( pSpliced )
        );
    }
    testPassed();
}

//==============================================================================================================================
int main( void ) try {
    cout << left << setfill( '.' );

    check_fbtActionNode();   cout << endl;
    check_fbtInverterNode(); cout << endl;
    check_fbtParallelNode(); cout << endl;
    check_fbtSelectorNode(); cout << endl;
    check_fbtSequenceNode(); cout << endl;
    check_fbtBuilder();

    return 0;

} catch ( const exception& cEx ) {
    cerr << cEx.what() << endl;
    return 1;

} catch ( ... ) {
    cerr << "FATAL ERROR !!!" << endl;
    return 2;
}
