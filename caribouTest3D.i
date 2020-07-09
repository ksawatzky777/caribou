[Mesh]
  type = GeneratedMesh
  dim = 3
  nx = 10
  ny = 10
  nz = 10
  xmin = 0.0
  xmax = 3100.0
  ymin = 0.0
  ymax = 3100.0
  zmin = 0.0
  zmax = 11110.0
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
    value = 1
    point = '1550.0 1550.0 5555.0'
  [../]
[]

[Materials]
  [./test]
    type = STMaterial
    num_dims = 3
    u_file_name = u.csv
    v_file_name = v.csv
    w_file_name = w.csv
    dim_file_name = coords.csv
    diffusivity = 0.1
  [../]
[]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'
  num_steps = 50
  dt = 1
[]

[Outputs]
  execute_on = 'timestep_end'
  exodus = true
[]
