#pragma once

#include "IntegratedBC.h"

/**
 * Implements a dry deposition boundary condition, which is a variation of a
 * mass-flux boundary condition.
 */
class DryDepositionBC : public IntegratedBC
{
public:
  static InputParameters validParams();

  DryDepositionBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  /// Dry deposition constant to be used at the boundary.
  Real _dry_deposition_const;
};
