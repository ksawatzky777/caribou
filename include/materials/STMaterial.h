#include "Material.h"
#include "TrilinearInterpolation.h"
#include "DelimitedFileReader.h"

class STMaterial;

template <>
InputParameters validParams<STMaterial>();

class STMaterial : public Material
{
public:
   STMaterial(const InputParameters & parameters);

protected:
  virtual void computeQpProperties() override;

private:
  MaterialProperty<Real> & _diffusivity;
  MaterialProperty<RealVectorValue> & _velocity
}
