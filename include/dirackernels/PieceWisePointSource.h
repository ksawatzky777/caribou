#pragma once

#include "DiracKernel.h"

// Forward declaration.
class PieceWisePointSource;

/**
 * This kernel provides a point source which releases material at a rate "_rate"
 * at specified times "_times".
 */

 template <>
 InputParameters validParams<PieceWisePointSource>();

 class PieceWisePointSource : public DiracKernel
 {
 public:
   PieceWisePointSource(const InputParameters & parameters);

   /// Adds the point this kernels acts on.
   virtual void addPoints() override;

 protected:
   virtual Real computeQpResidual() override;

   /// The rate of emission of this point source.
   const std::vector<Real> _rates;

   /// The time that the source emits material at the given rate.
   const std::vector<Real> _times;

   /// Point provided by the parameter system with 1 -> 3 coordinates.
   std::vector<Real> _input_point;

   /// Libmesh point this kernel acts on.
   Point _point;

 };
