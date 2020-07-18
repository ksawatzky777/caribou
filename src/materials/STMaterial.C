#include "STMaterial.h"
#include "MooseUtils.h"
#include "DelimitedFileReader.h"

registerMooseObject("caribouApp", STMaterial);

template <>
InputParameters
validParams<STMaterial>()
{
  InputParameters params = validParams<Material>();
  params.addRequiredParam<unsigned>("num_dims", "The number of dimensions "
                                    "the problem should consider.");
  params.addRequiredParam<std::string>("dim_file_name", "Name of the file which"
                                       " contains the x-y-z interpolation "
                                       "points to be used in conjuction with "
                                       "the datafiles for the components of "
                                       "the velocity.");
  params.addRequiredParam<std::string>("u_file_name", "Name of the file for the"
                                       " u component of the velocity.");
  params.addRequiredParam<std::string>("v_file_name", "Name of the file for the"
                                       " v component of the velocity.");
  params.addParam<std::string>("w_file_name", "Name of the file for the w "
                               "component of the velocity. This parameter is "
                               "necessary for a 3D problem.");
  params.addParam<std::string>("delimiter", ",", "CSV file delimiter, default "
                               "is assumed to be a comma.");
  params.addRequiredParam<Real>("diffusivity", "Value of the diffusion "
                                "coefficient, assumed constant across the "
                                "whole domain.");
  params.addParam<RealVectorValue>("const_velocity", "Velocity vector for "
                                   "advection, overrides the velocity provided "
                                   "by the datafiles.");

  return params;
}

STMaterial::STMaterial(const InputParameters & parameters)
  : Material(parameters),
    _diffusivity(declareProperty<Real>("diffusivity")),
    _velocity(declareProperty<RealVectorValue>("material_velocity"))
{
    std::string _u_file_name = getParam<std::string>("u_file_name");
    std::string _v_file_name = getParam<std::string>("v_file_name");
    std::string _dim_file_name = getParam<std::string>("dim_file_name");
    std::string _delimiter = ",";

    _const_v = parameters.isParamSetByUser("const_velocity");

  if (parameters.isParamSetByUser("delimiter"))
    _delimiter = getParam<std::string>("delimiter");

  if (_const_v != true)
  {
    if (getParam<unsigned>("num_dims") == 2)
        twoDConstruct(_u_file_name, _v_file_name, _dim_file_name, _delimiter);
  }

  if (_const_v != true)
  {
    if (getParam<unsigned>("num_dims") == 3
        && parameters.isParamSetByUser("w_file_name"))
    {
     std::string _w_file_name = getParam<std::string>("w_file_name");
     threeDConstruct(_u_file_name, _v_file_name, _w_file_name, _dim_file_name,
                     _delimiter);
    }
    else
    {
     mooseError("w_file_name was not provided.");
    }
  }
}

void
STMaterial::twoDConstruct(std::string & _u_file_name,
                          std::string & _v_file_name,
                          std::string & _dim_file_name,
                          std::string & _delimiter)
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

  std::vector<Real> _fake_z_coords;

  _fake_z_coords.resize(_dim_reader.getData(_coord_names[0]).size(), 0.0);

  _2_d_interp.push_back(TrilinearInterpolation(
                _dim_reader.getData(_coord_names[0]),
                _dim_reader.getData(_coord_names[1]),
                _u_reader.getData(_u_data_names[0]),
                _fake_z_coords));
  _2_d_interp.push_back(TrilinearInterpolation(
                _dim_reader.getData(_coord_names[0]),
                _dim_reader.getData(_coord_names[1]),
                _v_reader.getData(_v_data_names[0]),
                _fake_z_coords));
}

void
STMaterial::threeDConstruct(std::string & _u_file_name,
                            std::string & _v_file_name,
                            std::string & _w_file_name,
                            std::string & _dim_file_name,
                            std::string & _delimiter)
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

  _3_d_interp.push_back(TrilinearInterpolation(
                 _dim_reader.getData(_coord_names[0]),
                 _dim_reader.getData(_coord_names[1]),
                 _dim_reader.getData(_coord_names[2]),
                 _u_reader.getData(_u_data_names[0])));
  _3_d_interp.push_back(TrilinearInterpolation(
                 _dim_reader.getData(_coord_names[0]),
                 _dim_reader.getData(_coord_names[1]),
                 _dim_reader.getData(_coord_names[2]),
                 _v_reader.getData(_v_data_names[0])));
  _3_d_interp.push_back(TrilinearInterpolation(
                 _dim_reader.getData(_coord_names[0]),
                 _dim_reader.getData(_coord_names[1]),
                 _dim_reader.getData(_coord_names[2]),
                 _w_reader.getData(_w_data_names[0])));
}

void
STMaterial::twoDComputeQpProperties()
{
  _diffusivity[_qp] = getParam<Real>("diffusivity");

  _velocity[_qp] = {_2_d_interp[0].sample(_q_point[_qp](0), _q_point[_qp](1),
                    0.0), _2_d_interp[1].sample(_q_point[_qp](0),
                    _q_point[_qp](1), 0.0), 0.0};
}

void
STMaterial::threeDComputeQpProperties()
{
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
  if (_const_v != true)
  {
    if (getParam<unsigned>("num_dims") == 2)
      twoDComputeQpProperties();

    if (getParam<unsigned>("num_dims") == 3)
      threeDComputeQpProperties();
  }
  else
  {
    _diffusivity[_qp] = getParam<Real>("diffusivity");
    _velocity[_qp] = getParam<RealVectorValue>("const_velocity");
  }
}
