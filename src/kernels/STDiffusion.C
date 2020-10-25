#include "STDiffusion.h"

registerMooseObject("caribouApp", STDiffusion);

template <>
InputParameters
validParams<STDiffusion>()
{
  InputParameters params = validParams<Diffusion>();
  params.addClassDescription("Implements the Laplacian opperator with a "
                             "constant diffusion coefficient taken from the "
                             "materials system.");
  return params;
}

STDiffusion::STDiffusion(const InputParameters & parameters)
  : Diffusion(parameters),
  _diffusivity(getMaterialProperty<std::vector<Real>>("diffusivity"))
{
}

Real
STDiffusion::computeQpResidual()
{
  Real _qp_residual = 0.0;

  if (_diffusivity[_qp].size() == 1)
  {
    return _diffusivity[_qp][0] * Diffusion::computeQpResidual();
  }
  else
  {
    _qp_residual += _diffusivity[_qp][0] * _grad_u[_qp](0) * _grad_test[_i][_qp](0)
                  + _diffusivity[_qp][1] * _grad_u[_qp](1) * _grad_test[_i][_qp](1);
    if (_diffusivity[_qp].size() > 2)
    {
      _qp_residual += _diffusivity[_qp][2] * _grad_u[_qp](2) * _grad_test[_i][_qp](2);
    }
    return _qp_residual;
  }
}

Real STDiffusion::computeQpJacobian()
{
  Real _qp_jacobian = 0.0;

  if (_diffusivity[_qp].size() == 1)
  {
    return _diffusivity[_qp][0] * Diffusion::computeQpJacobian();
  }
  else
  {
    _qp_jacobian += _diffusivity[_qp][0] * _grad_phi[_j][_qp](0) * _grad_test[_i][_qp](0)
                  + _diffusivity[_qp][1] * _grad_phi[_j][_qp](1) * _grad_test[_i][_qp](1);
    if (_diffusivity[_qp].size() > 2)
    {
      _qp_jacobian += _diffusivity[_qp][2] * _grad_phi[_j][_qp](2) * _grad_test[_i][_qp](2);
    }
    return _qp_jacobian;
  }
}
