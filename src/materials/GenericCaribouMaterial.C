#include "GenericCaribouMaterial.h"

registerMooseObject("caribouApp", GenericCaribouMaterial);

template <>
InputParameters
validParams<GenericCaribouMaterial>()
{
  InputParameters params = validParams<STMaterial>();
  params.addClassDescription("Generic CARIBOU material which provides constant"
                             " parameters required to implement the atmospheric"
                             " dispersion model.");
  params.addRequiredParam<Real>("decay_constant", "Decay constant for this "
                                "radionuclide.");
  params.addParam<Real>("settling_velocity", 0.0, "The z component of the "
                        "settling velocity for this radionuclide. Must be "
                        "negative.");
  params.addParam<Real>("wet_scavenge_constant", 0.0, "The scavenging "
                        "coefficient for wet deposition.");
  return params;
}

GenericCaribouMaterial::GenericCaribouMaterial(const InputParameters & parameters)
  : STMaterial(parameters),
    _decay_const(declareProperty<Real>("decay_constant")),
    _settling_v(declareProperty<RealVectorValue>("settling_velocity")),
    _wet_scavenge(declareProperty<Real>("wet_scavenge_constant"))
{
  if (getParam<Real>("settling_velocity") > 0.0)
    mooseError("Settling velocity was not declared as negative.");
}

void
GenericCaribouMaterial::computeQpProperties()
{
  /// Compute properties required for the base scalar transport kernels.
  STMaterial::computeQpProperties();

  /// Compute properties for additional sinks and sources in the scalar transport
  /// equation.
  _decay_const[_qp] = getParam<Real>("decay_constant");
  _settling_v[_qp] = {0.0, 0.0, getParam<Real>("settling_velocity")};
  _wet_scavenge[_qp] = getParam<Real>("wet_scavenge_constant");
}
