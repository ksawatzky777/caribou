#pragma once

#include "Kernel.h"

// Forward declaration.
class SpeciesWetDeposition;

/**
 * Removal of the variable from the system through wet desposition. Uses a wet
 * scavenging constant provided by the material system.
 */
template <>
InputParameters validParams<SpeciesWetDeposition>();

class SpeciesWetDeposition : public Kernel
{
public:
  static InputParameters validParams();

  SpeciesWetDeposition(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  /// Scavenging coefficient provided by the material system.
  const MaterialProperty<Real> & _scavenge_const;
};
