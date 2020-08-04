#include "TimedPointSource.h"

registerMooseObject("caribouApp", TimedPointSource);

template <>
InputParameters
validParams<TimedPointSource>()
{
  InputParameters params = validParams<DiracKernel>();
  params.addRequiredParam<Real>("rate", "The rate of emission of this point "
                                "source.");
  params.addRequiredParam<std::vector<Real>>("point", "The point in which this "
                                             "point source is located (x, y, "
                                             "z).");
  params.addRequiredParam<Real>("deactivation_time", "Time this kernel is "
                                "deactivated.");
  params.addParam<Real>("activation_time", 0.0, "Time this kernel is activated "
                        "at.");
  params.declareControllable("rate");
  return params;
}

TimedPointSource::TimedPointSource(const InputParameters & parameters)
  : DiracKernel(parameters),
    _rate(getParam<Real>("rate")),
    _input_point(getParam<std::vector<Real>>("point")),
    _time_on(getParam<Real>("activation_time")),
    _time_off(getParam<Real>("deactivation_time"))
{
  _point(0) = _input_point[0];

  if (_input_point.size() > 1)
  {
    _point(1) = _input_point[1];
    if (_input_point.size() > 2)
    {
      _point(2) = _input_point[2];
    }
  }
}

void
TimedPointSource::addPoints()
{
  addPoint(_point);
}

Real
TimedPointSource::computeQpResidual()
{
  if (_t >= _time_on && _t <= _time_off)
  {
    return -_test[_i][_qp] * _rate;
  }
  else
  {
    return 0.0;
  }
}
