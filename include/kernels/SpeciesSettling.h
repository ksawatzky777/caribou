#pragma once

#include "Kernel.h"

// Forward declaration.
class SpeciesSettling;

/**
 * Transport of the material along the negative z direction via advection. The
 * equivalent of a weak gravitational term for scalar transport.
 */
template <>
InputParameters validParams<SpeciesSettling>();

class SpeciesSettling : public Kernel
{
public:
  static InputParameters validParams();

  SpeciesSettling(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;

  /// Settling velocity provided by the material system.
  const MaterialProperty<RealVectorValue> & _settling_v;
};
