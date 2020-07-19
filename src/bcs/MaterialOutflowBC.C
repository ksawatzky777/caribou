#include "MaterialOutflowBC.h"
#include "Function.h"

registerMooseObject("caribouApp", MaterialOutflowBC);

InputParameters
MaterialOutflowBC::validParams()
{
  InputParameters params = IntegratedBC::validParams();
  return params;
}

MaterialOutflowBC::MaterialOutflowBC(const InputParameters & parameters)
  : IntegratedBC(parameters),
    _velocity(getMaterialProperty<RealVectorValue>("material_velocity"))
{
}

Real
MaterialOutflowBC::computeQpResidual()
{
  return _test[_i][_qp] * _u[_qp] * _velocity[_qp] * _normals[_qp];
}

Real
MaterialOutflowBC::computeQpJacobian()
{
  return _test[_i][_qp] * _phi[_j][_qp] * _velocity[_qp] * _normals[_qp];
}
