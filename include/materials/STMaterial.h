#pragma once

#include "Material.h"
#include "TrilinearInterpolation.h"
#include "BilinearInterpolation.h"
#include "DelimitedFileReader.h"

class STMaterial;

template <>
InputParameters validParams<STMaterial>();

class STMaterial : public Material
{
public:
   STMaterial(const InputParameters & parameters);

   enum INTERPOLATOR
   {
    BILINEAR,
    TRILINEAR
   };

   static MooseEnum interpTypes()
   {
     return MooseEnum("bilinear=0 trilinear=1");
   }

protected:
  virtual void computeQpProperties() override;

  unsigned _num_dims;

  MooseEnum _interp_type;

private:
  MaterialProperty<Real> & _diffusivity;
  MaterialProperty<RealVectorValue> & _velocity;
}
