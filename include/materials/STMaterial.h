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

  MooseEnum _interp_type;

  MaterialProperty<Real> & _diffusivity;
  MaterialProperty<RealVectorValue> & _velocity;

  MooseUtils::DelimitedFileReader _csv_reader;

  TrilinearInterpolation _tri_interp;
  BilinearInterpolation _bi_interp;

}
