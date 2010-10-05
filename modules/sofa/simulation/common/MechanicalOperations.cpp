#include <sofa/simulation/common/MechanicalOperations.h>
#include <sofa/simulation/common/MechanicalVisitor.h>
#include <sofa/core/MultiVecId.h>
#include <sofa/core/VecId.h>
namespace sofa
{

namespace simulation
{

namespace common
{

MechanicalOperations::MechanicalOperations(sofa::core::objectmodel::BaseContext* ctx, const sofa::core::MechanicalParams* mparams)
    :mparams(*mparams),ctx(ctx),executeVisitor(*ctx)
{
}

MechanicalOperations::MechanicalOperations(sofa::core::objectmodel::BaseContext* ctx, const sofa::core::ExecParams* params)
    :mparams(*params),ctx(ctx),executeVisitor(*ctx)
{
}

/// Propagate the given displacement through all mappings
void MechanicalOperations::propagateDx(core::MultiVecDerivId dx)
{
    executeVisitor( MechanicalPropagateDxVisitor(&mparams,dx,false) );
}

/// Propagate the given displacement through all mappings and reset the current force delta
void MechanicalOperations::propagateDxAndResetDf(core::MultiVecDerivId dx, core::MultiVecDerivId df)
{
    executeVisitor( MechanicalPropagateDxAndResetForceVisitor(&mparams,dx,df,false) );
}

/// Propagate the given position through all mappings
void MechanicalOperations::propagateX(core::MultiVecCoordId x)
{
    executeVisitor( MechanicalPropagateXVisitor(&mparams, x, false) //Don't ignore the masks
                  );


}

/// Propagate the given position through all mappings and reset the current force delta
void MechanicalOperations::propagateXAndResetF(core::MultiVecCoordId x, core::MultiVecDerivId f)
{
    executeVisitor( MechanicalPropagateXAndResetForceVisitor(&mparams,x,f,false) );
}

/// Apply projective constraints to the given vector
void MechanicalOperations::projectResponse(core::MultiVecDerivId dx, double **W)
{
    executeVisitor( MechanicalApplyConstraintsVisitor(&mparams, dx, W) );
}

void MechanicalOperations::addMdx(core::MultiVecDerivId res, core::MultiVecDerivId dx, double factor)
{
    executeVisitor( MechanicalAddMDxVisitor(res,dx,factor,&mparams) );
}

///< res += factor M.dx
void MechanicalOperations::integrateVelocity(core::MultiVecDerivId res, core::ConstMultiVecCoordId x, core::ConstMultiVecDerivId v, double dt)
{
    executeVisitor( MechanicalVOpVisitor(&mparams,res,x,v,dt) );
}

///< res = x + v.dt
void MechanicalOperations::accFromF(core::MultiVecDerivId a, core::ConstMultiVecDerivId f) ///< a = M^-1 . f
{

    executeVisitor( MechanicalAccFromFVisitor(a,&mparams) );
}

/// Compute the current force (given the latest propagated position and velocity)
void MechanicalOperations::computeForce(core::MultiVecDerivId result, bool clear, bool accumulate)
{
    if (clear)
    {
        executeVisitor( MechanicalResetForceVisitor(&mparams, result, false), true ); // enable prefetching
        //finish();
    }
    executeVisitor( MechanicalComputeForceVisitor(&mparams,result, accumulate) , true ); // enable prefetching
}

/// Compute the current force delta (given the latest propagated displacement)
void MechanicalOperations::computeDf(core::MultiVecDerivId df, bool clear, bool accumulate)
{
    if (clear)
    {
        executeVisitor( MechanicalResetForceVisitor(&mparams, df) );
//	finish();
    }
    executeVisitor( MechanicalComputeDfVisitor( df,  accumulate, &mparams) );

}

/// Compute the current force delta (given the latest propagated velocity)
void MechanicalOperations::computeDfV(core::MultiVecDerivId df, bool clear, bool accumulate)
{
    if (clear)
    {
        executeVisitor( MechanicalResetForceVisitor(&mparams, df) );
        //finish();
    }
    executeVisitor( MechanicalComputeDfVisitor(df, accumulate, &mparams) );

}

/// accumulate $ df += (m M + b B + k K) dx $ (given the latest propagated displacement)
void MechanicalOperations::addMBKdx(core::MultiVecDerivId df, double m, double b, double k, bool clear, bool accumulate)
{
    if (clear)
    {
        executeVisitor( MechanicalResetForceVisitor(&mparams, df, true) );
        //finish();
    }
    mparams.setBFactor(b);
    mparams.setKFactor(k);
    mparams.setMFactor(m);
    executeVisitor( MechanicalAddMBKdxVisitor(df, accumulate, &mparams) );
}

/// accumulate $ df += (m M + b B + k K) velocity $
void MechanicalOperations::addMBKv(core::MultiVecDerivId df, double m, double b, double k, bool clear, bool accumulate)
{
    if (clear)
    {
        executeVisitor( MechanicalResetForceVisitor(&mparams, df, true) );
        //finish();
    }
    mparams.setBFactor(b);
    mparams.setKFactor(k);
    mparams.setMFactor(m);
    /* useV = true */
    executeVisitor( MechanicalAddMBKdxVisitor(df, accumulate, &mparams) );
}

/// Add dt*Gravity to the velocity
void MechanicalOperations::addSeparateGravity(double dt, core::MultiVecDerivId result)
{
    mparams.setDt(dt);
    executeVisitor( MechanicalAddSeparateGravityVisitor(&mparams, result) );
}

void MechanicalOperations::computeContactForce(core::MultiVecDerivId result)
{
    executeVisitor( MechanicalResetForceVisitor(&mparams, result) );
    //finish();
    executeVisitor( MechanicalComputeContactForceVisitor(&mparams, result) );

}

void MechanicalOperations::computeContactDf(core::MultiVecDerivId df)
{
    executeVisitor( MechanicalResetForceVisitor(&mparams, df) );
    //finish();

}

void MechanicalOperations::computeAcc(double t, core::MultiVecDerivId a, core::MultiVecCoordId x, core::MultiVecDerivId v)
{
    MultiVecDerivId f( VecDerivId::force() );
    executeVisitor( MechanicalPropagatePositionAndVelocityVisitor(&mparams,t,x,v) );
    computeForce(f);

    accFromF(a,f);
    projectResponse(a);

}

void MechanicalOperations::computeContactAcc(double t, core::MultiVecDerivId a, core::MultiVecCoordId x, core::MultiVecDerivId v)
{
    MultiVecDerivId f( VecDerivId::force() );
    executeVisitor( MechanicalPropagatePositionAndVelocityVisitor(&mparams,t,x,v) );
    computeContactForce(f);

    accFromF(a,f);
    projectResponse(a);

}

/// @}

/// @name Matrix operations using LinearSolver components
/// @{

void MechanicalOperations::m_resetSystem()
{
}

void MechanicalOperations::m_setSystemMBKMatrix(double mFact, double bFact, double kFact)
{
}

void MechanicalOperations::m_setSystemRHVector(core::MultiVecDerivId v)
{
}

void MechanicalOperations::m_setSystemLHVector(core::MultiVecDerivId v)
{
}

void MechanicalOperations::m_solveSystem()
{
}

void MechanicalOperations::m_print( std::ostream& out )
{
}

/// @}

/// @name Matrix operations
/// @{

// BaseMatrix & BaseVector Computations
void MechanicalOperations::getMatrixDimension(unsigned int * const, unsigned int * const, sofa::core::behavior::MultiMatrixAccessor* matrix)
{
}

void MechanicalOperations::addMBK_ToMatrix(const sofa::core::behavior::MultiMatrixAccessor* matrix, double mFact, double bFact, double kFact)
{
}


/*
void MechanicalOperations::multiVector2BaseVector(core::ConstMultiVecId src, defaulttype::BaseVector *dest, const sofa::core::behavior::MultiMatrixAccessor* matrix)
{
}


void MechanicalOperations::multiVectorPeqBaseVector(core::MultiVecId dest, defaulttype::BaseVector *src, const sofa::core::behavior::MultiMatrixAccessor* matrix)
{
}
*/


/// @}

/// @name Debug operations
/// @{

/// Dump the content of the given vector.
void MechanicalOperations::print( core::ConstMultiVecId v, std::ostream& out )
{
}


void MechanicalOperations::printWithElapsedTime( core::ConstMultiVecId v,  unsigned time, std::ostream& out )
{
}

/// @}

}

}

}
