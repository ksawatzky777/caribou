#include "STDiffusion.h"

registerMooseObject("caribouApp", STDiffusion);

template <>
InputParameters
validParams<STDiffusion>()
{
  InputParameters params = validParams<Diffusion>();
  return params;
}

STDiffusion::STDiffusion(const InputParameters & parameters)
  : Diffusion(parameters),
  _diffusivity(getMaterialProperty<Real>("diffusivity"))
{
}

Real
STDiffusion::computeQpResidual()
{
  return _diffusivity[_qp] * Diffusion::computeQpResidual();
}

Real STDiffusion::computeQpJacobian()
{
  return _diffusivity[_qp] * Diffusion::computeQpJacobian();
}
