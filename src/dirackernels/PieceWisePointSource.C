#include "PieceWisePointSource.h"

registerMooseObject("caribouApp", PieceWisePointSource);

template <>
InputParameters
validParams<PieceWisePointSource>()
{
  InputParameters params = validParams<DiracKernel>();
  params.addRequiredParam<std::vector<Real>>("rates", "The rates of emission of"
                                             " this point source. Each rate "
                                             "given must have a respective "
                                             "activation time provided in "
                                             "activation_times.");
  params.addRequiredParam<std::vector<Real>>("activation_times", "Activation "
                                             "times for the point source. Each "
                                             "activation time must have a "
                                             "respective rate provided in "
                                             "rates.");
  params.addRequiredParam<std::vector<Real>>("point", "The point in which this "
                                             "point source is located (x, y, "
                                             "z).");
  return params;
}

PieceWisePointSource::PieceWisePointSource(const InputParameters & parameters)
  : DiracKernel(parameters),
   _rates(getParam<std::vector<Real>>("rates")),
   _times(getParam<std::vector<Real>>("activation_times")),
   _input_point(getParam<std::vector<Real>>("point"))
{
  if (_rates.size() != _times.size())
  {
    mooseError("Rates provided (dims=", _rates.size(), ") do not have the same "
               "dimensions as the times provided (dims=", _times.size(), ").");
  }

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
PieceWisePointSource::addPoints()
{
  addPoint(_point);
}

Real
PieceWisePointSource::computeQpResidual()
{
  for (unsigned _counter = 0; _counter < _rates.size() - 1; _counter++)
  {
    if (_t >= _times[_counter] && _t < _times[_counter + 1])
    {
      return -_test[_i][_qp] * _rates[_counter];
    }
  }

  if (_t > _times[_rates.size() - 1])
  {
    return -_test[_i][_qp] * _rates[_rates.size() - 1];
  }
  else
  {
    return 0.0;
  }
}
