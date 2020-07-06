#pragma once

#include "Material.h"
#include "TrilinearInterpolation.h"
#include "BilinearInterpolation.h"

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
  void bilinearConstruct(std::string & _u_file_name,
                         std::string & _v_file_name,
                         std::string & _delimiter);
  void trilinearConstruct(std::string & _u_file_name,
                          std::string & _v_file_name,
                          std::string & _w_file_name,
                          std::string & _delimiter);

  virtual void computeQpProperties();
  virtual void bilinearComputeQpProperties();
  virtual void trilinearComputeQpProperties();

  MooseEnum _interp_type;

  std::vector<BilinearInterpolation> _bi_interp;
  std::vector<TrilinearInterpolation> _tri_interp;

  Real _param_diffusivity;

  MaterialProperty<Real> & _diffusivity;
  MaterialProperty<RealVectorValue> & _velocity;
}
