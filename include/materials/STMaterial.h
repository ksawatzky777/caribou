#pragma once

#include "Material.h"
#include "TrilinearInterpolation.h"

// Forward declaration.
class STMaterial;

/**
 * A generic scalar transport material which provides a velocity profile and
 * diffusion coefficient for scalar transport. Can accept velocity values from
 * a properly formatted series of input csv files or as a constant provided by
 * the InputParameters system.
 */

template <>
InputParameters validParams<STMaterial>();

class STMaterial : public Material
{
public:
   STMaterial(const InputParameters & parameters);

protected:

  /// Method to initialize 2D trilinear interpolators to interpolate velocity
  /// components from the given csv files.
  void twoDConstruct(std::string & _u_file_name,
                     std::string & _v_file_name,
                     std::string & _dim_file_name,
                     std::string & _delimiter);

  /// Method to initialize 3D trilinear interpolators to interpolate velocity
  /// components from the given csv files.
  void threeDConstruct(std::string & _u_file_name,
                       std::string & _v_file_name,
                       std::string & _w_file_name,
                       std::string & _dim_file_name,
                       std::string & _delimiter);

  virtual void computeQpProperties();

  /// Property compute method for 2D.
  virtual void twoDComputeQpProperties();

  /// Property compute method for 3D.
  virtual void threeDComputeQpProperties();

  /// Vectors of interpolator objects to interpolate data.
  std::vector<TrilinearInterpolation> _2_d_interp;
  std::vector<TrilinearInterpolation> _3_d_interp;

  /// Boolean to check to see if a constant velocity was provided.
  /// Cleans up code.
  bool _const_v;

  /// Number of dimensions in the mesh.
  unsigned _num_dims;

  /// Diffusion coefficient which this material is providing.
  MaterialProperty<Real> & _diffusivity;

  /// Velocity profile which this material is supplying.
  MaterialProperty<RealVectorValue> & _velocity;
};
