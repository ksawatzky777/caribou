#include "WetDeposition.h"

registerMooseObject("caribouApp", WetDeposition);

template <>
InputParameters
validParams<WetDeposition>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Wet deposition kernel which uses a scavenging "
                             "coefficient provided by the material system.");
  return params;
}

WetDeposition::WetDeposition(const InputParameters & parameters)
  : Kernel(parameters),
    _scavenge_const(getMaterialProperty<Real>("wet_scavenge_constant"))
{
}

Real
WetDeposition::computeQpResidual()
{
  return _test[_i][_qp] * _scavenge_const[_qp] * _u[_qp];
}

Real
WetDeposition::computeQpJacobian()
{
  return _test[_i][_qp] * _scavenge_const[_qp] * _phi[_j][_qp];
}
