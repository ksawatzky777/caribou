#include "STMaterial.h"
#include "MooseUtils.h"
#include "DelimitedFileReader.h"

registerMooseObject("caribou", STMaterial);

template <>
InputParameters
validParams<STMaterial>()
{
  InputParameters params = validParams<Material>();
  params.addRequiredParam<MooseEnum>("interp_type", STMaterial::interpTypes(),
  "The type of interpolation to perform. It also implicitly defines the number "
  "of dimensions for the problem. bilinear for 2 dimensions and trilinear for "
  "3.");
  params.addRequiredParam<std::string>("u_file_name", "Name of the file for the"
  " u component of the velocity.");
  params.addRequiredParam<std::string>("v_file_name", "Name of the file for the"
  " v component of the velocity.");
  params.addRequiredParam<std::string>("w_file_name", "Name of the file for the"
  " w component of the velocity.");
  params.addRequiredParam<Real>("diffusivity", "Value of the diffusion "
              "coefficient, assumed constant across the whole domain.");

  return params;
}

STMaterial::STMaterial(const InputParameters & parameters)
  : Material(parameters),
    _diffusivity(declareProperty<Real>("diffusivity")),
    _velocity(declareProperty<RealVectorValue>("velocity"))
{
  _diffusivity = getParam<Real>("diffusivity");
  std::string _v_file_name = getParam<std::string>("v_file_name");
  std::string _w_file_name = getParam<std::string>("w_file_name");

  MooseUtils::DelimitedFileReader _csv_reader(getParam<std::string>("u_file_"
                                                                    "name"));
  //You were working here.

  std::vector<Real> _u_data;
  std::vector<Real> _v_data;
  std::vector<Real> _w_data;

}
