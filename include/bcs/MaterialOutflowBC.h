#pragma once

#include "IntegratedBC.h"

class MaterialOutflowBC : public IntegratedBC
{
public:
  static InputParameters validParams();

  MaterialOutflowBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  const MaterialProperty<RealVectorValue> & _velocity;
};
