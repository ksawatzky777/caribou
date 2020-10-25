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
  params.addRequiredParam<std::vector<Real>>("diffusivity", "Value of the "
                                "diffusion coefficient. Can declare either a "
                                "single value or a vector of up to 3 values "
                                "(one for each spatial dimension)");
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
    _diffusivity(declareProperty<std::vector<Real>>("diffusivity")),
    _velocity(declareProperty<RealVectorValue>("material_velocity")),
    _num_dims(_mesh.dimension()),
    _velocity_time_dependant(getParam<bool>("time_dependance"))
{
  _const_v = parameters.isParamSetByUser("const_velocity");
  std::string _delimiter = getParam<std::string>("delimiter");
  std::vector<std::string> _file_names;

  if (getParam<std::vector<Real>>("diffusivity").size() != _num_dims
      && getParam<std::vector<Real>>("diffusivity").size() > 1)
  {
    mooseError("Must declare values of the diffusion coefficient in all "
               "mesh directions, or a single value.");
  }

  /// Initialize the time index to 0 and old time to the initial sim time.
  _t_index = 0;
  _previous_t_index = 0;
  _old_time = _t;

  /// Logic to initialize the interpolator objects.
  if (_const_v != true && _num_dims == 2)
  {
    if (parameters.isParamSetByUser("u_file_name")
        && parameters.isParamSetByUser("v_file_name")
        && parameters.isParamSetByUser("dim_file_name"))
    {
      /// Fetch file names.
      _file_names.push_back(getParam<std::string>("u_file_name"));
      _file_names.push_back(getParam<std::string>("v_file_name"));
      _file_names.push_back(getParam<std::string>("dim_file_name"));

      /// 2D interpolator initialization.
      twoDConstruct(_delimiter, _file_names);
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
      /// Fetch file names.
      _file_names.push_back(getParam<std::string>("u_file_name"));
      _file_names.push_back(getParam<std::string>("v_file_name"));
      _file_names.push_back(getParam<std::string>("w_file_name"));
      _file_names.push_back(getParam<std::string>("dim_file_name"));

      /// 3D interpolator initialization.
      threeDConstruct(_delimiter, _file_names);
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
  if (_array_to_clean.size() > 1)
  {
    unsigned _first_to_remove = 0;
    for (unsigned i = 1; i < _array_to_clean.size(); i++)
    {
      if (_array_to_clean[i] ==  0.0 && _array_to_clean[i - 1] != 0.0)
      {
        _first_to_remove = i;
        break;
      }
    }
    if (_first_to_remove != 0)
    {
      _array_to_clean.erase(_array_to_clean.begin() + _first_to_remove,
                            _array_to_clean.end());
    }
    if (_array_to_clean[0] == 0.0 && _array_to_clean[1] == 0.0)
      _array_to_clean.erase(_array_to_clean.begin() + 1, _array_to_clean.end());
  }
}

/// Slow for large data files.
void
STMaterial::computeTimeIndex()
{
  /// Check all elements except the last.
  for (unsigned i = 0; i < _dimensions[3].size() - 1; i++)
  {
    if (_t >= _dimensions[3][i] && _t < _dimensions[3][i + 1])
    {
      _t_index = i;
      break;
    }
  }
  /// Checks the last element.
  if (_t >= _dimensions[3][_dimensions[3].size() - 1])
    _t_index = _dimensions[3].size() - 1;
}

void
STMaterial::twoDConstruct(std::string & _delimiter,
                          std::vector<std::string> & _file_names)
{
  std::vector<MooseUtils::DelimitedFileReader> _reader;

  for (unsigned i = 0; i < _file_names.size(); i++)
  {
    _reader.push_back(MooseUtils::DelimitedFileReader(_file_names[i]));

    _reader[i].setDelimiter(_delimiter);

    _reader[i].read();

    _data_names.push_back(_reader[i].getNames());
  }

  /// Zero vector for dimensions which don't exist in the scope of the problem.
  std::vector<Real> _zero_vector;
  _zero_vector.push_back(0.0);

  /// Read dimensions from the dim file.
  if (_is_transient && _velocity_time_dependant)
  {
    _dimensions.push_back(_reader[2].getData(_data_names[2][0]));
    _dimensions.push_back(_reader[2].getData(_data_names[2][1]));
    _dimensions.push_back(_zero_vector);
    _dimensions.push_back(_reader[2].getData(_data_names[2][2]));
    cleanAxisData(_dimensions[3]);
  }
  else
  {
    _dimensions.push_back(_reader[2].getData(_data_names[2][0]));
    _dimensions.push_back(_reader[2].getData(_data_names[2][1]));
    _dimensions.push_back(_zero_vector);
    _dimensions.push_back(_zero_vector);
  }

  /// Clean the dimensions (remove unnecessary zeros).
  cleanAxisData(_dimensions[0]);
  cleanAxisData(_dimensions[1]);

  /// Read weather data from files.
  for (unsigned i = 0; i < _dimensions[3].size(); i++)
  {
    _u_data.push_back(_reader[0].getData(_data_names[0][i]));
    _v_data.push_back(_reader[1].getData(_data_names[1][i]));
  }

  /// Initialize interpolator vectors.
  _2_d_interp.push_back(TrilinearInterpolation(_dimensions[0],
                                               _dimensions[1],
                                               _dimensions[2],
                                               _u_data[_t_index]));
  _2_d_interp.push_back(TrilinearInterpolation(_dimensions[0],
                                               _dimensions[1],
                                               _dimensions[2],
                                               _v_data[_t_index]));
}

void
STMaterial::threeDConstruct(std::string & _delimiter,
                            std::vector<std::string> & _file_names)
{
  std::vector<MooseUtils::DelimitedFileReader> _reader;

  for (unsigned i = 0; i < _file_names.size(); i++)
  {
    _reader.push_back(MooseUtils::DelimitedFileReader(_file_names[i]));

    _reader[i].setDelimiter(_delimiter);

    _reader[i].read();

    _data_names.push_back(_reader[i].getNames());
  }

  /// Read dimensions from the dim file.
  if (_is_transient && _velocity_time_dependant)
  {
    _dimensions.push_back(_reader[3].getData(_data_names[3][0]));
    _dimensions.push_back(_reader[3].getData(_data_names[3][1]));
    _dimensions.push_back(_reader[3].getData(_data_names[3][2]));
    _dimensions.push_back(_reader[3].getData(_data_names[3][3]));
    cleanAxisData(_dimensions[3]);
  }
  else
  {
    std::vector<Real> _zero_vector;
    _zero_vector.push_back(0.0);

    _dimensions.push_back(_reader[3].getData(_data_names[3][0]));
    _dimensions.push_back(_reader[3].getData(_data_names[3][1]));
    _dimensions.push_back(_reader[3].getData(_data_names[3][2]));
    _dimensions.push_back(_zero_vector);
  }

  /// Clean the dimensions (remove unnecessary zeros).
  cleanAxisData(_dimensions[0]);
  cleanAxisData(_dimensions[1]);
  cleanAxisData(_dimensions[2]);

  /// Read weather data from files.
  for (unsigned i = 0; i < _dimensions[3].size(); i++)
  {
    _u_data.push_back(_reader[0].getData(_data_names[0][i]));
    _v_data.push_back(_reader[1].getData(_data_names[1][i]));
    _w_data.push_back(_reader[2].getData(_data_names[2][i]));
  }

  /// Initialize interpolator vectors.
  _3_d_interp.push_back(TrilinearInterpolation(_dimensions[0],
                                               _dimensions[1],
                                               _dimensions[2],
                                               _u_data[_t_index]));
  _3_d_interp.push_back(TrilinearInterpolation(_dimensions[0],
                                               _dimensions[1],
                                               _dimensions[2],
                                               _v_data[_t_index]));
  _3_d_interp.push_back(TrilinearInterpolation(_dimensions[0],
                                               _dimensions[1],
                                               _dimensions[2],
                                               _w_data[_t_index]));
}

void
STMaterial::twoDUpdate()
{
  /// Clear interpolator objects.
  _2_d_interp.clear();

  /// Re-initialize the interpolator object to use data from the current
  /// velocity field time-step.
  _2_d_interp.push_back(TrilinearInterpolation(_dimensions[0],
                                               _dimensions[1],
                                               _dimensions[2],
                                               _u_data[_t_index]));
  _2_d_interp.push_back(TrilinearInterpolation(_dimensions[0],
                                               _dimensions[1],
                                               _dimensions[2],
                                               _v_data[_t_index]));
}

void
STMaterial::threeDUpdate()
{
  /// Clear interpolator objects.
  _3_d_interp.clear();

  /// Re-initialize the interpolator object to use data from the current
  /// velocity field time-step.
  _3_d_interp.push_back(TrilinearInterpolation(_dimensions[0],
                                               _dimensions[1],
                                               _dimensions[2],
                                               _u_data[_t_index]));
  _3_d_interp.push_back(TrilinearInterpolation(_dimensions[0],
                                               _dimensions[1],
                                               _dimensions[2],
                                               _v_data[_t_index]));
  _3_d_interp.push_back(TrilinearInterpolation(_dimensions[0],
                                               _dimensions[1],
                                               _dimensions[2],
                                               _w_data[_t_index]));
}

void
STMaterial::twoDComputeQpProperties()
{
  if (_is_transient && _velocity_time_dependant)
  {
    if (_t != _old_time)
    {
      _old_time = _t;
      computeTimeIndex();
      if (_t_index != _previous_t_index)
      {
        _previous_t_index = _t_index;
        twoDUpdate();
      }
    }
  }

  /// Compute properties.
  _diffusivity[_qp] = getParam<std::vector<Real>>("diffusivity");;

  _velocity[_qp] = {_2_d_interp[0].sample(_q_point[_qp](0), _q_point[_qp](1),
                    0.0), _2_d_interp[1].sample(_q_point[_qp](0),
                    _q_point[_qp](1), 0.0), 0.0};
}

void
STMaterial::threeDComputeQpProperties()
{
  if (_is_transient && _velocity_time_dependant)
  {
    if (_t != _old_time)
    {
      _old_time = _t;
      computeTimeIndex();
      if (_t_index != _previous_t_index)
      {
        _previous_t_index = _t_index;
        threeDUpdate();
      }
    }
  }

  /// Compute properties.
  _diffusivity[_qp] = getParam<std::vector<Real>>("diffusivity");;

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
    _diffusivity[_qp] = getParam<std::vector<Real>>("diffusivity");
    _velocity[_qp] = getParam<RealVectorValue>("const_velocity");
  }
}
