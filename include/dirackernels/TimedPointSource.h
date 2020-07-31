#pragma once

#include "DiracKernel.h"

// Forward declaration.
class TimedPointSource;

/**
 * This kernel provides a point source which s activated at some time "_time_on"
 * and deactivated at some time "_time_off".
 */

template <>
InputParameters validParams<TimedPointSource>();

class TimedPointSource : public DiracKernel
{
public:
  TimedPointSource(const InputParameters & parameters);

  /// Adds the point this kernels acts on.
  virtual void addPoints() override;

protected:
  virtual Real computeQpResidual() override;

  /// The rate of emission of this point source.
  const Real & _rate;

  /// Point provided by the parameter system with 1 -> 3 coordinates.
  std::vector<Real> _input_point;

  /// Libmesh point this kernel acts on.
  Point _point;

  /// The activation and deactivation times for this kernel.
  const Real & _time_on;
  const Real & _time_off;
};
