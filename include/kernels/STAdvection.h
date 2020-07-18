//* This file is a modification of the ConservativeAdvection kernel.
//* It utilizes a velocity profile provided by the material system as opposed
//* to a constant velocity provided by the input parameters system.

//* This kernel is still in developement and is to be used temporarily while
//* a more elegent method of converting the ConservativeAdvection kernel to use
//* the material system is developed.

//* Original Copyright and Disclaimer:
//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "Kernel.h"

// Forward Declaration
class STAdvection;

/**
 * Advection of the variable by the velocity provided by the material system.
 * Options for numerical stabilization are: none; full upwinding
 */
template <>
InputParameters validParams<STAdvection>();

class STAdvection : public Kernel
{
public:
  static InputParameters validParams();

  STAdvection(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
  virtual void computeResidual() override;
  virtual void computeJacobian() override;

  /// advection velocity from the material system
  const MaterialProperty<RealVectorValue> & _velocity;

  /// enum to make the code clearer
  enum class JacRes
  {
    CALCULATE_RESIDUAL = 0,
    CALCULATE_JACOBIAN = 1
  };

  /// Type of upwinding
  const enum class UpwindingType { none, full } _upwinding;

  /// Nodal value of u, used for full upwinding
  const VariableValue & _u_nodal;

  /// In the full-upwind scheme, whether a node is an upwind node
  std::vector<bool> _upwind_node;

  /// In the full-upwind scheme d(total_mass_out)/d(variable_at_node_i)
  std::vector<Real> _dtotal_mass_out;

  /// Returns - _grad_test * velocity
  Real negSpeedQp() const;

  /// Calculates the fully-upwind Residual and Jacobian (depending on res_or_jac)
  void fullUpwind(JacRes res_or_jac);
};
