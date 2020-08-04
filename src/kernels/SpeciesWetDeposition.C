#include "SpeciesWetDeposition.h"

registerMooseObject("caribouApp", SpeciesWetDeposition);

template <>
InputParameters
validParams<SpeciesWetDeposition>()
{
  InputParameters params = validParams<Kernel>();
  params.addClassDescription("Wet deposition kernel which uses a scavenging "
                             "coefficient provided by the material system.");
  return params;
}

SpeciesWetDeposition::SpeciesWetDeposition(const InputParameters & parameters)
  : Kernel(parameters),
    _scavenge_const(getMaterialProperty<Real>("wet_scavenge_constant"))
{
}

Real
SpeciesWetDeposition::computeQpResidual()
{
  return _test[_i][_qp] * _scavenge_const[_qp] * _u[_qp];
}

Real
SpeciesWetDeposition::computeQpJacobian()
{
  return _test[_i][_qp] * _scavenge_const[_qp] * _phi[_j][_qp];
}
