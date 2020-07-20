//* This file is part of the MOOSE framework
//* https://www.mooseframework.org
//*
//* All rights reserved, see COPYRIGHT for full restrictions
//* https://github.com/idaholab/moose/blob/master/COPYRIGHT
//*
//* Licensed under LGPL 2.1, please see LICENSE for details
//* https://www.gnu.org/licenses/lgpl-2.1.html

#pragma once

#include "IntegratedBC.h"

/**
 * Implements a mass flux boundary condition using a velocity supplied by the
 * user.
 */
class ConstantOutflowBC : public IntegratedBC
{
public:
  static InputParameters validParams();

  ConstantOutflowBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  /// Velocity supplied by the input parameters system.
  RealVectorValue _velocity;
};
