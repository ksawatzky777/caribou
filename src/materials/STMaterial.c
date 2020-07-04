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
  _diffusivity = getParam<Real>("diffusivity");

  std::string _u_file_name = getParam<std::string>("u_file_name");
  std::string _v_file_name = getParam<std::string>("v_file_name");

  switch (_interp_type)
  {
    case BILINEAR:
    {
      if (getParam<unsigned>("num_dims") == 2)
      {
        bilinearConstruct(_u_file_name, _v_file_name);
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
          && parameter.isParamSetByUser("w_file_name"))
      {
        std::string _w_file_name = getParam<std::string>("w_file_name");
        trilinearConstruct(_u_file_name, _v_file_name, _w_file_name);
      }
      else
      {
        mooseError("Number of dimensions don't match the interpolation scheme "
                   "or the w_file_name was not provided.");
      }
      break;
    }
    default:{
      mooseError("Invalid enum type.");
    }
  }
}

void
STMaterial::bilinearConstruct(std::string & _u_file_name,
                                   std::string & _v_file_name)
{

}

void
STMaterial::trilinearConstruct(std::string & _u_file_name,
                                    std::string & _v_file_name,
                                    std::string & _w_file_name)
{

}

void
STMaterial::bilinearComputeQpProperties()
{

}

void
STMaterial::trilinearComputeQpProperties()
{

}

void
STMaterial::computeQpProperties()
{

}
