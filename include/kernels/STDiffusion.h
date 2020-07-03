#pragma once

#include "Diffusion.h"

class STDiffusion;

template <>
InputParameters validParams<STDiffusion>();

class STDiffusion : public Diffusion
{
public:
  STDiffusion(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  const MaterialProperty<Real> & _diffusivity;
};
