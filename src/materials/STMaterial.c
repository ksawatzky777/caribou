#include "STMaterial.h"
#include "MooseUtils.h"

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
  params.addRequiredParam<std::string>("file_path", "Location of the file and "
  "the name of the file in a DilimitedFileReader compatible format.");
  params.addRequiredParam<Real>("diffusivity", "Value of the diffusion "
              "coefficient, assumed constant across the whole domain.");

  return params;
}

STMaterial::STMaterial(const InputParameters & parameters)
  : Material(parameters),

    _diffusivity(declareProperty<Real>("diffusivity")),

    _velocity(declareProperty<RealVectorValue>)
