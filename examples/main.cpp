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

#include <memory>
#include <exception>
#include <iostream>

using namespace std;
using namespace smd::fbt;

using timeData_t = float;
using nodePtr_t  = shared_ptr< node_i< timeData_t > >;
using builder_t  = builder_c< timeData_t >;

#define DO( name, code )        .mDo( name, []( timeData_t ) code )
#define CONDITION( name, code ) .mCondition( name, []( timeData_t timeData ) code )
#define SEQUENCE( name )        .mSequence( name )
#define PARALLEL( name )        .mParallel( name, 0, 0 )
#define END                     .mEnd()
#define BUILD                   .mBuild()

//==============================================================================================================================
nodePtr_t startUp( void ) {
    builder_t builder;

    nodePtr_t pTree {
        builder
            SEQUENCE( "My-Sequence" )
                DO( "Action-1", {
                    cout << "Do Action-1" << endl;
                    return status_t::SUCCESS;
                } )
                DO( "Action-2", {
                    cout << "Do Action-2" << endl;
                    return status_t::SUCCESS;
                } )

                PARALLEL( "Make Parallel" )
                    SEQUENCE( "Seq1" )
                        CONDITION( "Condition-for-Action-3", {
                            return timeData < 0.2f;
                        } )
                        DO( "Action-3", {
                            cout << "Do Action-3" << endl;
                            return status_t::SUCCESS;
                        } )
                    END

                    SEQUENCE( "Seq2" )
                        CONDITION( "Condition-for-Action-4", {
                            return timeData > 0.2f;
                        } )
                        DO( "Action-4", {
                            cout << "Do Action-4" << endl;
                            return status_t::SUCCESS;
                        } )
                    END
                END
            END
            BUILD
    };

    return pTree;
}

//==============================================================================================================================
void update( nodePtr_t pTree, timeData_t timeData ) {
    pTree->mTick( timeData );
}

//==============================================================================================================================
int main( void ) try {
    nodePtr_t pTree { startUp() };

    update( pTree, 0.1f );
    update( pTree, 0.3f );

    return 0;

} catch ( const exception& cEx ) {
    cerr << cEx.what() << endl;
    return 1;

} catch ( ... ) {
    cerr << "FATAL ERROR !!!" << endl;
    return 2;
}
