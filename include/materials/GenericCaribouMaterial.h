#pragma once

#include "STMaterial.h"

class GenericCaribouMaterial;

/**
 * The material which provides all the constants and parameters required for
 * the kernels used by CARIBOU's atmospheric dispersion model.
 */

template <>
InputParameters validParams<GenericCaribouMaterial>();

class GenericCaribouMaterial : public STMaterial
{
public:
  GenericCaribouMaterial(const InputParameters & parameters);

protected:

  virtual void computeQpProperties() override;

  /// Decay constant this material is providing.
  MaterialProperty<Real> & _decay_const;

  /// Settling velocity this material is providing.
  MaterialProperty<RealVectorValue> & _settling_v;

  /// Wet scavenging coefficient this material is providing.
  MaterialProperty<Real> & _wet_scavenge;
};
