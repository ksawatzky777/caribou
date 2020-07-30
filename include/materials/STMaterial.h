#pragma once

#include "Material.h"
#include "TrilinearInterpolation.h"

// Forward declaration.
class STMaterial;

/**
 * A generic scalar transport material which provides a velocity profile and
 * diffusion coefficient for advection-diffusion. Can accept velocity values
 * from a properly formatted series of input csv files or as a constant provided
 * by the InputParameters system.
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
                     std::string & _delimiter,
                     unsigned _t_index);

  /// Method to initialize 3D trilinear interpolators to interpolate velocity
  /// components from the given csv files.
  void threeDConstruct(std::string & _u_file_name,
                       std::string & _v_file_name,
                       std::string & _w_file_name,
                       std::string & _dim_file_name,
                       std::string & _delimiter,
                       unsigned _t_index);
  /// Method to compute the time index of the data relative to the current
  /// simulation time.
  void computeTimeIndex();

  /// Method to remove irrelevent datapoints from the data read from the csv
  /// files. Searches for a zero followed by a second zero. It that removes
  /// everything from the 2nd zero to the end of the vector.
  void cleanAxisData(std::vector<Real> & _array_to_clean);

  /// Main property compute method. Calls either twoDComputeQpProperties or
  /// threeDComputeQpProperties depending on the number of dimensions.
  virtual void computeQpProperties();

  /// Property compute method for 2D.
  virtual void twoDComputeQpProperties();

  /// Property compute method for 3D.
  virtual void threeDComputeQpProperties();

  /// Variables to hold file names. Should probably clean this up and use a map
  /// instead.
  std::string _u_file_name;
  std::string _v_file_name;
  std::string _w_file_name;
  std::string _dim_file_name;
  std::string _delimiter;

  /// Vectors of interpolator objects to interpolate data.
  std::vector<TrilinearInterpolation> _2_d_interp;
  std::vector<TrilinearInterpolation> _3_d_interp;

  /// Boolean variables to clean up the code somewhat.
  bool _velocity_time_dependant;
  bool _const_v;

  /// Number of dimensions in the mesh.
  unsigned _num_dims;

  /// Current index entry of the provided velocity field and a temporary index
  /// value for the next velocity field. Temporary index value is written to the
  /// current index value if they are different (current simulation time is
  /// ahead of the velocity field).
  unsigned _t_index;
  unsigned _temp_t_index;

  /// Vector of values for the data time axis.
  std::vector<Real> _time_axis;

  /// Diffusion coefficient which this material is providing.
  MaterialProperty<Real> & _diffusivity;

  /// Velocity profile which this material is supplying.
  MaterialProperty<RealVectorValue> & _velocity;
};
