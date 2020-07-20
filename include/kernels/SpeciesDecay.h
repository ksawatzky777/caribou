#pragma once

#include "Kernel.h"

// Forward Declaration.
class SpeciesDecay;

/**
 * Radioactive decay of the variable utilizing a decay constant provided by the
 * material system.
 */
 template <>
 InputParameters validParams<SpeciesDecay>();

class SpeciesDecay : public Kernel
{
public:
  static InputParameters validParams();

  SpeciesDecay(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  /// Decay constant provided by the material system.
  const MaterialProperty<Real> & _decay_const;
};
