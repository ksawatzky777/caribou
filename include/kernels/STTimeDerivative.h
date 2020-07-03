#pragma once

#include "TimeDerivative.h"

class STTimeDerivative;

template <>
InputParameters validParams<STTimeDerivative>();

class STTimeDerivative : public TimeDerivative
{
public:
  STTimeDerivative(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
};
