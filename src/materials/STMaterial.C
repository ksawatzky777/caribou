#include "STMaterial.h"
#include "MooseUtils.h"
#include "DelimitedFileReader.h"
#include "MooseMesh.h"

registerMooseObject("caribouApp", STMaterial);

template <>
InputParameters
validParams<STMaterial>()
{
  InputParameters params = validParams<Material>();
  params.addClassDescription("Generic scalar transport material which provides "
                             "the variables required to implement advection-"
                             "diffusion.");
  params.addRequiredParam<Real>("diffusivity", "Value of the diffusion "
                                "coefficient, assumed constant across the "
                                "whole domain.");
  params.addParam<RealVectorValue>("const_velocity", "Velocity vector for "
                                   "advection, overrides the velocity provided "
                                   "by the datafiles.");
  params.addParam<bool>("time_dependance", false, "Boolean to determine if the "
                        "velocity provided in the csv files is time dependant "
                        "or not.");
  params.addParam<std::string>("dim_file_name", " ", "Name of the file which"
                                       " contains the x-y-z interpolation "
                                       "points to be used in conjuction with "
                                       "the datafiles for the components of "
                                       "the velocity.");
  params.addParam<std::string>("u_file_name", " ", "Name of the file for the"
                                       " u component of the velocity.");
  params.addParam<std::string>("v_file_name", " ", "Name of the file for the"
                                       " v component of the velocity.");
  params.addParam<std::string>("w_file_name", " ", "Name of the file for the w "
                               "component of the velocity. This parameter is "
                               "necessary for a 3D problem.");
  params.addParam<std::string>("delimiter", ",", "CSV file delimiter, default "
                               "is assumed to be a comma.");
  return params;
}

STMaterial::STMaterial(const InputParameters & parameters)
  : Material(parameters),
    _diffusivity(declareProperty<Real>("diffusivity")),
    _velocity(declareProperty<RealVectorValue>("material_velocity")),
    _num_dims(_mesh.dimension()),
    _velocity_time_dependant(getParam<bool>("time_dependance")),
    _dim_file_name(getParam<std::string>("dim_file_name")),
    _u_file_name(getParam<std::string>("u_file_name")),
    _v_file_name(getParam<std::string>("v_file_name")),
    _w_file_name(getParam<std::string>("w_file_name")),
    _delimiter(getParam<std::string>("delimiter"))
{
  _const_v = parameters.isParamSetByUser("const_velocity");

  /// Initialize the time index to 0.
  _t_index = 0;

  /// Logic to initialize the interpolator objects.
  if (_const_v != true && _num_dims == 2)
  {
    if (parameters.isParamSetByUser("u_file_name")
        && parameters.isParamSetByUser("v_file_name")
        && parameters.isParamSetByUser("dim_file_name"))
    {
      /// 2D interpolator initialization.
      twoDConstruct(_u_file_name, _v_file_name, _dim_file_name, _delimiter,
                    _t_index);
    }
    else
    {
      mooseError("Property file names were not provided.");
    }
  }

  if (_const_v != true && _num_dims == 3)
  {
    if (parameters.isParamSetByUser("u_file_name")
        && parameters.isParamSetByUser("v_file_name")
        && parameters.isParamSetByUser("w_file_name")
        && parameters.isParamSetByUser("dim_file_name"))
    {
      /// 3D interpolator initialization.
      threeDConstruct(_u_file_name, _v_file_name, _w_file_name, _dim_file_name,
                     _delimiter, _t_index);
    }
    else
    {
      mooseError("w_file_name was not provided.");
    }
  }
}

void
STMaterial::cleanAxisData(std::vector<Real> & _array_to_clean)
{
  unsigned _first_to_remove = 0;
  for (unsigned i = 1; i < _array_to_clean.size(); i++)
  {
    if (_array_to_clean[i] ==  0.0 && _array_to_clean[i - 1] != 0.0)
      _first_to_remove = i;
      break;
  }
  if (_first_to_remove != 0)
    _array_to_clean.erase(_array_to_clean.begin() + _first_to_remove,
                          _array_to_clean.end());
  if (_array_to_clean[0] == 0.0 && _array_to_clean[1] == 0.0)
    _array_to_clean.erase(_array_to_clean.begin() + 1, _array_to_clean.end());
}

void
STMaterial::computeTimeIndex()
{
  for (unsigned i = _t_index; i < _time_axis.size() - 1; i++)
  {
    if (_t >= _time_axis[i] && _t < _time_axis[i + 1])
      _t_index = i;
      break;
  }
}

void
STMaterial::twoDConstruct(std::string & _u_file_name,
                          std::string & _v_file_name,
                          std::string & _dim_file_name,
                          std::string & _delimiter,
                          unsigned _t_index)
{
  MooseUtils::DelimitedFileReader _u_reader(_u_file_name);
  MooseUtils::DelimitedFileReader _v_reader(_v_file_name);
  MooseUtils::DelimitedFileReader _dim_reader(_dim_file_name);

  _u_reader.setDelimiter(_delimiter);
  _v_reader.setDelimiter(_delimiter);
  _dim_reader.setDelimiter(_delimiter);

  _u_reader.read();
  _v_reader.read();
  _dim_reader.read();

  const std::vector<std::string> _u_data_names = _u_reader.getNames();
  const std::vector<std::string> _v_data_names = _v_reader.getNames();
  const std::vector<std::string> _coord_names = _dim_reader.getNames();

  std::vector<Real> _temp_z_coords;
  _temp_z_coords.push_back(0.0);

  if (_is_transient && _velocity_time_dependant)
  {
    _time_axis = _dim_reader.getData(_coord_names[2]);
    cleanAxisData(_time_axis);
  }

  std::vector<Real> _x_axis = _dim_reader.getData(_coord_names[0]);
  std::vector<Real> _y_axis = _dim_reader.getData(_coord_names[1]);

  cleanAxisData(_x_axis);
  cleanAxisData(_y_axis);

  _2_d_interp.push_back(TrilinearInterpolation(_x_axis, _y_axis, _temp_z_coords,
                                   _u_reader.getData(_u_data_names[_t_index])));
  _2_d_interp.push_back(TrilinearInterpolation(_x_axis, _y_axis, _temp_z_coords,
                                   _v_reader.getData(_v_data_names[_t_index])));
}

void
STMaterial::threeDConstruct(std::string & _u_file_name,
                            std::string & _v_file_name,
                            std::string & _w_file_name,
                            std::string & _dim_file_name,
                            std::string & _delimiter,
                            unsigned _t_index)
{
  MooseUtils::DelimitedFileReader _u_reader(_u_file_name);
  MooseUtils::DelimitedFileReader _v_reader(_v_file_name);
  MooseUtils::DelimitedFileReader _w_reader(_w_file_name);
  MooseUtils::DelimitedFileReader _dim_reader(_dim_file_name);

  _u_reader.setDelimiter(_delimiter);
  _v_reader.setDelimiter(_delimiter);
  _w_reader.setDelimiter(_delimiter);
  _dim_reader.setDelimiter(_delimiter);

  _u_reader.read();
  _v_reader.read();
  _w_reader.read();
  _dim_reader.read();

  const std::vector<std::string> & _u_data_names = _u_reader.getNames();
  const std::vector<std::string> & _v_data_names = _v_reader.getNames();
  const std::vector<std::string> & _w_data_names = _w_reader.getNames();
  const std::vector<std::string> _coord_names = _dim_reader.getNames();

  if (_is_transient && _velocity_time_dependant)
  {
    _time_axis = _dim_reader.getData(_coord_names[2]);
    cleanAxisData(_time_axis);
  }

  std::vector<Real> _x_axis = _dim_reader.getData(_coord_names[0]);
  std::vector<Real> _y_axis = _dim_reader.getData(_coord_names[1]);
  std::vector<Real> _z_axis = _dim_reader.getData(_coord_names[2]);

  cleanAxisData(_x_axis);
  cleanAxisData(_y_axis);
  cleanAxisData(_z_axis);

  _3_d_interp.push_back(TrilinearInterpolation(_x_axis, _y_axis, _z_axis,
                                   _u_reader.getData(_u_data_names[_t_index])));
  _3_d_interp.push_back(TrilinearInterpolation(_x_axis, _y_axis, _z_axis,
                                   _v_reader.getData(_v_data_names[_t_index])));
  _3_d_interp.push_back(TrilinearInterpolation(_x_axis, _y_axis, _z_axis,
                                   _w_reader.getData(_w_data_names[_t_index])));
}

void
STMaterial::twoDComputeQpProperties()
{
  /// Updates the interpolator object every change in data time step.
  if (_is_transient && _velocity_time_dependant)
  {
    computeTimeIndex();
    twoDConstruct(_u_file_name, _v_file_name, _dim_file_name, _delimiter,
                  _t_index);
  }

  /// Compute properties.
  _diffusivity[_qp] = getParam<Real>("diffusivity");

  _velocity[_qp] = {_2_d_interp[0].sample(_q_point[_qp](0), _q_point[_qp](1),
                    0.0), _2_d_interp[1].sample(_q_point[_qp](0),
                    _q_point[_qp](1), 0.0), 0.0};
}

void
STMaterial::threeDComputeQpProperties()
{
  /// Updates the interpolator object if the
  if (_is_transient && _velocity_time_dependant)
  {
    computeTimeIndex();
    threeDConstruct(_u_file_name, _v_file_name, _w_file_name, _dim_file_name,
                    _delimiter, _t_index);
  }
  _diffusivity[_qp] = getParam<Real>("diffusivity");

  _velocity[_qp] = {_3_d_interp[0].sample(_q_point[_qp](0), _q_point[_qp](1),
                    _q_point[_qp](2)),
                    _3_d_interp[1].sample(_q_point[_qp](0), _q_point[_qp](1),
                    _q_point[_qp](2)),
                    _3_d_interp[2].sample(_q_point[_qp](0), _q_point[_qp](1),
                    _q_point[_qp](2))};
}

void
STMaterial::computeQpProperties()
{
  if (_const_v == false)
  {
    if (_num_dims == 2)
      twoDComputeQpProperties();

    if (_num_dims == 3)
      threeDComputeQpProperties();
  }
  else
  {
    _diffusivity[_qp] = getParam<Real>("diffusivity");
    _velocity[_qp] = getParam<RealVectorValue>("const_velocity");
  }
}
