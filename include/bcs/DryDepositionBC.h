#pragma once

#include "IntegratedBC.h"

class DryDepositionBC : public IntegratedBC
{
public:
  static InputParameters validParams();

  DryDepositionBC(const InputParameters & parameters);

protected:
  virtual Real computeQpResidual();
  virtual Real computeQpJacobian();

  Real _dry_deposition_const;
};
