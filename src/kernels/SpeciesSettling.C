#include "SpeciesSettling.h"

registerMooseObject("caribouApp", SpeciesSettling);

template <>
InputParameters
validParams<SpeciesSettling>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Advection of the species along the negative z "
                             "axis using a settling velocity from the material "
                             "system. The equivalent of a weak gravitational "
                             "term for scalar transport.");
  return params;
}

SpeciesSettling::SpeciesSettling(const InputParameters & parameters)
  : Kernel(parameters),
    _settling_v(getMaterialProperty<RealVectorValue>("settling_velocity"))
{
}

Real
SpeciesSettling::computeQpResidual()
{
  return _test[_i][_qp] * (_settling_v[_qp] * _grad_u[_qp]);
}

Real
SpeciesSettling::computeQpJacobian()
{
  return _test[_i][_qp] * (_settling_v[_qp] * _grad_phi[_j][_qp]);
}
