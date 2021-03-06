[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 100
  ny = 100
  xmin = 0.0
  xmax = 1550.0
  ymin = 0.0
  ymax = 1550.0
  zmax = 0.0
[]

[Variables]
  [./concentration]
    order = FIRST
    family = LAGRANGE

    [./InitialCondition]
      type = ConstantIC
      value = 0.0
    [../]
  [../]
[]

[Kernels]
  [./diff]
    type = STDiffusion
    variable = concentration
  [../]

  [./advc]
    type = STAdvection
    variable = concentration
    upwinding_type = full
  [../]

  [./time]
    type = STTimeDerivative
    variable = concentration
  [../]
[]

[DiracKernels]
  [./srce]
    variable = concentration
    type = ConstantPointSource
    value = 1.0
    point = '775.0 775.0 0.0'
  [../]
[]

[BCs]
  [./left]
    type = ConstantOutflowBC
    variable = concentration
    boundary = '3'
    velocity = '-10.0 0.0 0.0'
  [../]
[]

[Materials]
  [./test]
    type = STMaterial
    diffusivity = 1.0
    const_velocity = '-10.0 0.0 0.0'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  num_steps = 10
  dt = 1
[]

[Outputs]
  execute_on = 'timestep_end'
  exodus = true
[]
