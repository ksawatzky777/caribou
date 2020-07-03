#include "STAdvection.h"

registerMooseObject("caribou", STAdvection);

template <>
InputParameters
validParams<STAdvection>()
{
  InputParameters params = validParams<Kernel>();
  return params;
}

STAdvection::STAdvection(const InputParameters & parameters)
  : Kernel(parameters),
  _velocity(getMaterialProperty<RealVectorValue>("advection_velocity"))
{
}

Real
STAdvection::computeQpResidual()
{
  return _test[_i][_qp] * (_velocity[_qp] * _grad_u[_qp]);
}

Real
STAdvection::computeQpJacobian()
{
  return _test[_i][_qp] * (_velocity[_qp] * _grad_phi[_j][_qp]);
}
