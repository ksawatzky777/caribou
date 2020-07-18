#pragma once

#include "TimeDerivative.h"

// Forward Declaration.
class STTimeDerivative;

/**
 * Implements a generic time derivative. Currently it fully inherits from the
 * TimeDerivative kernel but additional functionality may be added in the
 * future.
 */

template <>
InputParameters validParams<STTimeDerivative>();

class STTimeDerivative : public TimeDerivative
{
public:
  STTimeDerivative(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual() override;
  virtual Real computeQpJacobian() override;
};
