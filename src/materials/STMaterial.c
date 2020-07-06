#include "STMaterial.h"
#include "MooseUtils.h"
#include "DelimitedFileReader.h"

registerMooseObject("caribou", STMaterial);

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
                                     "of dimensions for the problem. BILINEAR "
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

  if (parameters.isParamSetByUser("delimiter"))
    std::string _delimiter = getParam<std::string>("delimiter");
  else
    std::string _delimiter = ",";

  switch (_interp_type)
  {
    case BILINEAR:
    {
      if (getParam<unsigned>("num_dims") == 2)
      {
        bilinearConstruct(_u_file_name, _v_file_name, _delimiter);
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
        trilinearConstruct(_u_file_name, _v_file_name, _w_file_name, _delimiter
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
STMaterial::bilinearConstruct(std::string & _u_file_name,
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

  _bi_interp.resize(2);
  _bi_interp[0] = BilinearInterpolation(_u_reader.getData(_u_data_names[0]),
                                        _u_reader.getData(_u_data_names[1]),
                                        _u_reader.getData(_u_data_names[2]));
  _bi_interp[1] = BilinearInterpolation(_v_reader.getData(_v_data_names[0]),
                                        _v_reader.getData(_v_data_names[1]),
                                        _v_reader.getData(_v_data_names[2]));
}

void
STMaterial::trilinearConstruct(std::string & _u_file_name,
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

  _tri_interp.resize(3);
  _tri_interp[0] = TrilinearInterpolation(_u_reader.getData(_u_data_names[0]),
                                          _u_reader.getData(_u_data_names[1]),
                                          _u_reader.getData(_u_data_names[2]),
                                          _u_reader.getData(_u_data_names[3]));
  _tri_interp[1] = TrilinearInterpolation(_v_reader.getData(_v_data_names[0]),
                                          _v_reader.getData(_v_data_names[1]),
                                          _v_reader.getData(_v_data_names[2]),
                                          _v_reader.getData(_v_data_names[3]));
  _tri_interp[2] = TrilinearInterpolation(_w_reader.getData(_u_data_names[0]),
                                          _w_reader.getData(_u_data_names[1]),
                                          _w_reader.getData(_u_data_names[2]),
                                          _w_reader.getData(_u_data_names[3]));
}

void
STMaterial::bilinearComputeQpProperties()
{
  _diffusivity[_qp] = _param_diffusivity;

  _velocity[_qp] = {_bi_interp[0].sample(_q_point[_qp][0], _q_point[_qp][1]),
                    _bi_interp[1].sample(_q_point[_qp][0], _q_point[_qp][1])};
}

void
STMaterial::trilinearComputeQpProperties()
{
  _diffusivity[_qp] = _param_diffusivity;

  _velocity[_qp] = {_tri_interp[0].sample(_q_point[_qp][0], _q_point[_qp][1],
                    _q_point[_qp][2]),
                    _tri_interp[1].sample(_q_point[_qp][0], _q_point[_qp][1],
                    _q_point[_qp][2]),
                    _tri_interp[2].sample(_q_point[_qp][0], _q_point[_qp][1],
                    _q_point[_qp][2])};
}

void
STMaterial::computeQpProperties()
{
  switch (_interp_type)
  {
    case BILINEAR:
    {
      if (getParam<unsigned>("num_dims") == 2)
      {
        bilinearComputeQpProperties();
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
        trilinearComputeQpProperties();
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
