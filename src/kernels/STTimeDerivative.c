#include "STTimeDerivative.h"

registerMooseObject("caribouApp", STTimeDerivative);

template <>
InputParameters
validParams<STTimeDerivative>()
{
  InputParameters params = validParams<STTimeDerivative>();
  return params;
}

STTimeDerivative::STTimeDerivative(const InputParameters & parameters)
  : STTimeDerivative(parameters)
{
}

Real STTimeDerivative::computeQpResidual()
{
  return TimeDerivative::computeQpResidual();
}

Real
STTimeDerivative::computeQpJacobian()
{
  return TimeDerivative::computeQpJacobian();
}
