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
  void twoDConstruct(std::string & _delimiter,
                     std::vector<std::string> & _file_names);

  /// Method to initialize 3D trilinear interpolators to interpolate velocity
  /// components from the given csv files.
  void threeDConstruct(std::string & _delimiter,
                       std::vector<std::string> & _file_names);
  /// Method to compute the time index of the data relative to the current
  /// simulation time.
  void computeTimeIndex();

  /// Methods to update the interpolator objects when the velocity field is time
  /// dependant. This is computationally taxing and should be replaced
  /// with a quadrilinear interpolation scheme in the future.
  void twoDUpdate();
  void threeDUpdate();

  /// Method to remove irrelevent datapoints from the data read from the csv
  /// files.
  void cleanAxisData(std::vector<Real> & _array_to_clean);

  /// Main property compute method. Calls either twoDComputeQpProperties or
  /// threeDComputeQpProperties depending on the number of dimensions.
  virtual void computeQpProperties() override;

  /// Property compute method for 2D.
  virtual void twoDComputeQpProperties();

  /// Property compute method for 3D.
  virtual void threeDComputeQpProperties();

  /// Vectors of values for the data axis.
  std::vector<std::vector<Real>> _dimensions;

  /// Vectors of values for the data names.
  std::vector<std::vector<std::string>> _data_names;

  ///Vectors of values for the data.
  std::vector<std::vector<Real>> _u_data;
  std::vector<std::vector<Real>> _v_data;
  std::vector<std::vector<Real>> _w_data;

  /// Vectors of interpolator objects to interpolate data.
  std::vector<TrilinearInterpolation> _2_d_interp;
  std::vector<TrilinearInterpolation> _3_d_interp;

  /// Boolean variables to clean up the code somewhat.
  bool _velocity_time_dependant;
  bool _const_v;

  /// Number of dimensions in the mesh.
  unsigned _num_dims;

  /// Current index entry of the time dependant velocity field and the previous
  /// index entry.
  unsigned _t_index;
  unsigned _previous_t_index;

  /// Old simulation time. Used to detect if the simulation has advanced in a
  /// time step.
  Real _old_time;

  /// Diffusion coefficient which this material is providing.
  MaterialProperty<std::vector<Real>> & _diffusivity;

  /// Velocity profile which this material is supplying.
  MaterialProperty<RealVectorValue> & _velocity;
};
