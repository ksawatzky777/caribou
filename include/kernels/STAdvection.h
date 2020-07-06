#pragma once
#include "Kernel.h"

class STAdvection;

template <>
InputParameters validParams<STAdvection>();

class STAdvection : public Kernel
{
public:
  STAdvection(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  const MaterialProperty<RealVectorValue> & _velocity;
};
