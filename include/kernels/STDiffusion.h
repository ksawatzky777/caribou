#pragma once

#include "Diffusion.h"

// Forward Declaration
class STDiffusion;

/**
 * Diffusion of the variable implemented by the Diffusion kernel (part of the
 * MOOSE framework), with a diffusion coefficient provided by the material
 * system.
 */

template <>
InputParameters validParams<STDiffusion>();

class STDiffusion : public Diffusion
{
public:
  static InputParameters validParams();

  STDiffusion(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  /// Diffusion coefficient provided by the material system.
  const MaterialProperty<std::vector<Real>> & _diffusivity;
};
