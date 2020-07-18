#include "STTimeDerivative.h"

registerMooseObject("caribouApp", STTimeDerivative);

template <>
InputParameters
validParams<STTimeDerivative>()
{
  InputParameters params = validParams<TimeDerivative>();
  params.addClassDescription("Currently identical to the TimeDerivative kernel."
                             " Additional functionality pertinent to CARIBOU "
                             "may be added later.");
  return params;
}

STTimeDerivative::STTimeDerivative(const InputParameters & parameters)
  : TimeDerivative(parameters)
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
