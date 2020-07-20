#pragma once

#include "Kernel.h"

// Forward declaration.
class WetDeposition;

/**
 * Removal of the variable from the system through wet desposition. Uses a wet
 * scavenging constant provided by the material system.
 */
 template <>
 InputParameters validParams<WetDeposition>();

class WetDeposition : public Kernel
{
public:
  static InputParameters validParams();

  WetDeposition(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  /// Scavenging coefficient provided by the material system.
  const MaterialProperty<Real> & _scavenge_const;
};
