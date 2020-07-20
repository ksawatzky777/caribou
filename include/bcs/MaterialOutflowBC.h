#pragma once

#include "IntegratedBC.h"

/**
 * Implements a mass flux boundary condition using a velocity supplied by the
 * materials system.
 */
class MaterialOutflowBC : public IntegratedBC
{
public:
  static InputParameters validParams();

  MaterialOutflowBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  /// Velocity supplied by the materials system.
  const MaterialProperty<RealVectorValue> & _velocity;
};
