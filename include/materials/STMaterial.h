#pragma once

#include "Material.h"
#include "TrilinearInterpolation.h"

class STMaterial;

template <>
InputParameters validParams<STMaterial>();

class STMaterial : public Material
{
public:
   STMaterial(const InputParameters & parameters);

   enum INTERPOLATOR
   {
    BICUBIC,
    TRILINEAR
   };

   static MooseEnum interpTypes()
   {
     return MooseEnum("bicubic=0 trilinear=1");
   }

protected:
  void twoDConstruct(std::string & _u_file_name,
                     std::string & _v_file_name,
                     std::string & _delimiter);
  void threeDConstruct(std::string & _u_file_name,
                       std::string & _v_file_name,
                       std::string & _w_file_name,
                       std::string & _delimiter);

  virtual void computeQpProperties();
  virtual void twoDComputeQpProperties();
  virtual void threeDComputeQpProperties();

  MooseEnum _interp_type;

  std::vector<TrilinearInterpolation> _2_d_interp;
  std::vector<TrilinearInterpolation> _3_d_interp;

  Real _param_diffusivity;

  MaterialProperty<Real> & _diffusivity;
  MaterialProperty<RealVectorValue> & _velocity;
};
