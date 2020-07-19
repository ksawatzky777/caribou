#include "SpeciesDecay.h"

registerMooseObject("caribouApp", SpeciesDecay);

template <>
InputParameters
validParams<SpeciesDecay>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Radioactive decay kernel which utilizes a decay"
                             " constant from the materials system.");
  return params;
}

SpeciesDecay::SpeciesDecay(const InputParameters & parameters)
  : Kernel(parameters),
    _decay_const(getMaterialProperty<Real>("decay_const"))
{
}

Real
SpeciesDecay::computeQpResidual()
{
  return _test[_i][_qp] * _decay_const[_qp] * _u[_qp];
}

Real
SpeciesDecay::computeQpJacobian()
{
  return _test[_i][_qp] * _decay_const[_qp] * _phi[_j][_qp];
}
