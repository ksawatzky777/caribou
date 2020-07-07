#include "STMaterial.h"
#include "MooseUtils.h"
#include "DelimitedFileReader.h"

registerMooseObject("caribouApp", STMaterial);

template <>
InputParameters
validParams<STMaterial>()
{
  InputParameters params = validParams<Material>();
  params.addRequiredParam<unsigned>("num_dims", 2, "The number of dimensions "
                                    "the problem should consider. The default "
                                    "is 2 dimensions.");
  params.addRequiredParam<MooseEnum>("interp_type", STMaterial::interpTypes(),
                                     "The type of interpolation to perform. It "
                                     "also implicitly defines the number "
                                     "of dimensions for the problem. BICUBIC "
                                     "for 2 dimensions and TRILINEAR for "
                                     "3.");
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

  return params;
}

STMaterial::STMaterial(const InputParameters & parameters)
  : Material(parameters),
    _diffusivity(declareProperty<Real>("diffusivity")),
    _velocity(declareProperty<RealVectorValue>("velocity")),
    _interp_type(getParam<MooseEnum>("interp_type"))
{
  _param_diffusivity = getParam<Real>("diffusivity");

  std::string _u_file_name = getParam<std::string>("u_file_name");
  std::string _v_file_name = getParam<std::string>("v_file_name");
  std::string _delimiter = ",";

  if (parameters.isParamSetByUser("delimiter"))
    _delimiter = getParam<std::string>("delimiter");

  switch (_interp_type)
  {
    case BICUBIC:
    {
      if (getParam<unsigned>("num_dims") == 2)
      {
        twoDConstruct(_u_file_name, _v_file_name, _delimiter);
      }
      else
      {
        mooseError("Number of dimensions don't match the interpolation scheme "
                   ".");
      }
      break;
    }
    case TRILINEAR:
    {
      if (getParam<unsigned>("num_dims") == 3
          && parameters.isParamSetByUser("w_file_name"))
      {
        std::string _w_file_name = getParam<std::string>("w_file_name");
        threeDConstruct(_u_file_name, _v_file_name, _w_file_name, _delimiter
                          );
      }
      else
      {
        mooseError("Number of dimensions don't match the interpolation scheme "
                   "or the w_file_name was not provided.");
      }
      break;
    }
    default:
    {
      mooseError("Invalid enum type.");
    }
  }
}

void
STMaterial::twoDConstruct(std::string & _u_file_name,
                              std::string & _v_file_name,
                              std::string & _delimiter)
{
  MooseUtils::DelimitedFileReader _u_reader(_u_file_name);
  MooseUtils::DelimitedFileReader _v_reader(_u_file_name);

  _u_reader.setDelimiter(_delimiter);
  _v_reader.setDelimiter(_delimiter);

  _u_reader.read();
  _v_reader.read();

  const std::vector<std::string> & _u_data_names = _u_reader.getNames();
  const std::vector<std::string> & _v_data_names = _v_reader.getNames();

  if (_u_data_names.size() != 3)
    mooseError("Data files are not formatted for a 2D problem.");
  else if (_v_data_names.size() != 3)
    mooseError("Data files are not formatted for a 2D problem.");

  std::vector<Real> _fake_z_coords_u;
  std::vector<Real> _fake_z_coords_v;
  _fake_z_coords_u.resize(_u_reader.getData(_u_data_names[0]).size(), 0.0);
  _fake_z_coords_v.resize(_v_reader.getData(_v_data_names[0]).size(), 0.0);

  _2_d_interp.push_back(TrilinearInterpolation(
                _u_reader.getData(_u_data_names[0]),
                _u_reader.getData(_u_data_names[1]),
                _u_reader.getData(_u_data_names[2]),
                _fake_z_coords_u));
  _2_d_interp.push_back(TrilinearInterpolation(
                _v_reader.getData(_v_data_names[0]),
                _v_reader.getData(_v_data_names[1]),
                _v_reader.getData(_v_data_names[2]),
                _fake_z_coords_v));
}

void
STMaterial::threeDConstruct(std::string & _u_file_name,
                               std::string & _v_file_name,
                               std::string & _w_file_name,
                               std::string & _delimiter)
{
  MooseUtils::DelimitedFileReader _u_reader(_u_file_name);
  MooseUtils::DelimitedFileReader _v_reader(_u_file_name);
  MooseUtils::DelimitedFileReader _w_reader(_w_file_name);

  _u_reader.setDelimiter(_delimiter);
  _v_reader.setDelimiter(_delimiter);
  _w_reader.setDelimiter(_delimiter);

  _u_reader.read();
  _v_reader.read();
  _w_reader.read();

  const std::vector<std::string> & _u_data_names = _u_reader.getNames();
  const std::vector<std::string> & _v_data_names = _v_reader.getNames();
  const std::vector<std::string> & _w_data_names = _w_reader.getNames();

  if (_u_data_names.size() != 4)
    mooseError("Data files are not formatted for a 3D problem.");
  else if (_v_data_names.size() != 4)
    mooseError("Data files are not formatted for a 3D problem.");
  else if (_w_data_names.size() != 4)
    mooseError("Data files are not formatted for a 3D problem.");

  _3_d_interp.push_back(TrilinearInterpolation(
                _u_reader.getData(_u_data_names[0]),
                _u_reader.getData(_u_data_names[1]),
                _u_reader.getData(_u_data_names[2]),
                _u_reader.getData(_u_data_names[3])));
  _3_d_interp.push_back(TrilinearInterpolation(
                 _v_reader.getData(_v_data_names[0]),
                 _v_reader.getData(_v_data_names[1]),
                 _v_reader.getData(_v_data_names[2]),
                 _v_reader.getData(_v_data_names[3])));
  _3_d_interp.push_back(TrilinearInterpolation(
                 _w_reader.getData(_u_data_names[0]),
                 _w_reader.getData(_u_data_names[1]),
                 _w_reader.getData(_u_data_names[2]),
                 _w_reader.getData(_u_data_names[3])));
}

void
STMaterial::twoDComputeQpProperties()
{
  _diffusivity[_qp] = _param_diffusivity;

  _velocity[_qp] = {_2_d_interp[0].sample(_q_point[_qp](0), _q_point[_qp](1),
                    0.0), _2_d_interp[1].sample(_q_point[_qp](0),
                    _q_point[_qp](1), 0.0), 0.0};
}

void
STMaterial::threeDComputeQpProperties()
{
  _diffusivity[_qp] = _param_diffusivity;

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
  switch (_interp_type)
  {
    case BICUBIC:
    {
      if (getParam<unsigned>("num_dims") == 2)
      {
        twoDComputeQpProperties();
      }
      else
      {
        mooseError("Number of dimensions don't match the interpolation scheme"
                   ".");
      }
      break;
    }
    case TRILINEAR:
    {
      if (getParam<unsigned>("num_dims") == 3)
      {
        threeDComputeQpProperties();
      }
      else
      {
        mooseError("Number of dimensions don't match the interpolation scheme"
                   ".");
      }
      break;
    }
    default:
    {
      mooseError("Invalid enum type.");
    }
  }
}
